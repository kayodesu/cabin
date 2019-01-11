/*
 * Author: Jia Yang
 */
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "loader/classloader.h"
#include "rtda/thread/thread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/strpool.h"
#include "jvm.h"
#include "rtda/ma/class.h"
#include "rtda/heap/object.h"
#include "native/registry.h"
#include "rtda/heap/strobj.h"
#include "rtda/heap/mgr/heap_mgr.h"


#define JRE_LIB_JARS_MAX_COUNT 64 // big enough
#define JRE_EXT_JARS_MAX_COUNT 64 // big enough
#define USER_DIRS_MAX_COUNT 64 // big enough? // todo
#define USER_JARS_MAX_COUNT 64 // big enough? // todo

int jre_lib_jars_count = 0;
char jre_lib_jars[JRE_LIB_JARS_MAX_COUNT][PATH_MAX];

int jre_ext_jars_count = 0;
char jre_ext_jars[JRE_EXT_JARS_MAX_COUNT][PATH_MAX];

int user_dirs_count = 0;
int user_jars_count = 0;
char user_dirs[USER_DIRS_MAX_COUNT][PATH_MAX];
char user_jars[USER_JARS_MAX_COUNT][PATH_MAX];

size_t g_initial_heap_size = 67108864; // 64Mb
struct heap_mgr g_heap_mgr;

struct classloader *g_bootstrap_loader = NULL;

struct object *system_thread_group = NULL;
struct thread *main_thread = NULL;

static void init_jvm(struct classloader *loader, struct thread *main_thread)
{
    // 先加载 sun.mis.VM 类，然后执行其类初始化方法
    struct class *vm_class = classloader_load_class(loader, "sun/misc/VM");
    if (vm_class == NULL) {
        jvm_abort("vm_class is null\n");  // todo throw exception
        return;
    }

    // VM类的 "initialize~()V" 方法需调用执行
    // 在VM类的类初始化方法中调用了 "initialize" 方法。
    class_clinit(vm_class, main_thread);
    interpret(main_thread);
}

/*
 * main thread 由虚拟机启动。
 */
static void create_main_thread(struct classloader *loader)
{
    struct class *jltg_class = classloader_load_class(loader, "java/lang/ThreadGroup");
    system_thread_group = object_create(jltg_class);

    struct class *jlt_class = classloader_load_class(loader, "java/lang/Thread");
    struct object *jlt_obj = object_create(jlt_class);

    // from java/lang/Thread.java
    // public final static int MIN_PRIORITY = 1;
    // public final static int NORM_PRIORITY = 5;
    // public final static int MAX_PRIORITY = 10;
    struct slot value = islot(5);  // todo. why set this? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    set_instance_field_value_by_nt(jlt_obj, "priority", "I", &value);

    main_thread = thread_create(loader, jlt_obj);

    // 调用 java/lang/Thread 的构造函数
    struct method *constructor
            = class_get_constructor(jlt_obj->clazz, "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    struct slot args[] = {
            rslot(jlt_obj),
            rslot(system_thread_group),
            rslot(strobj_create(MAIN_THREAD_NAME)) // thread name
    };
    thread_invoke_method(main_thread, constructor, args);
    class_clinit(jlt_obj->clazz, main_thread); // 最后压栈，保证先执行。


    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    struct slot arg = rslot(system_thread_group);
    thread_invoke_method(main_thread, class_get_constructor(jltg_class, "()V"), &arg);
    class_clinit(jltg_class, main_thread);

    // 现在，main thread's vm stack 里面按将要执行的顺序（从栈顶到栈底）分别为：
    // java/lang/ThreadGroup~<clinit>
    // java/lang/ThreadGroup~<init>~()V
    // java/lang/Thread~<clinit>
    // java/lang/Thread~<init>~(Ljava/lang/ThreadGroup;Ljava/lang/String;)V
    interpret(main_thread);
}

static void start_jvm(const char *main_class_name)
{
    hm_init(&g_heap_mgr);
    build_str_pool();

    struct classloader *loader = classloader_create(true);
    create_main_thread(loader);
    init_jvm(loader, main_thread);

    struct class *main_class = classloader_load_class(loader, main_class_name);
    struct method *main_method = class_lookup_static_method(main_class, "main", "([Ljava/lang/String;)V");
    if (main_method == NULL) {
        jvm_abort("can't find method main."); // todo
    } else {
        if (!IS_PUBLIC(main_method->access_flags)) {
            jvm_abort("method main must be public."); // todo
        }
        if (!IS_STATIC(main_method->access_flags)) {
            jvm_abort("method main must be static."); // todo
        }
    }

    struct slot args[] = { islot(0), rslot(NULL) }; // todo
    thread_invoke_method(main_thread, main_method, args);

    // 开始在主线程中执行 main 方法
    interpret(main_thread);

    // todo 如果有其他的非后台线程在执行，则main线程需要在此wait

    // main_thread 退出，做一些清理工作。
    thread_destroy(main_thread);
    classloader_destroy(loader);
}

static void find_jars(const char *path, char jars[][PATH_MAX], int *jars_count)
{
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printvm("open dir failed. %s\n", path);
    }

    int count = 0;
    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char abspath[PATH_MAX];
        sprintf(abspath, "%s/%s\0", path, entry->d_name); // 绝对路径

        stat(abspath, &statbuf);
        if (S_ISREG(statbuf.st_mode)) { // 常规文件
            char *suffix = strrchr(abspath, '.');
            if (suffix != NULL && strcmp(suffix, ".jar") == 0) {
                strcpy(jars[count++], abspath);
            }
        }
    }

    *jars_count = count;
    closedir(dir);
}

int main(int argc, char* argv[])
{
    time_t time0;
    time(&time0);

    char bootstrap_classpath[PATH_MAX] = { 0 };
    char extension_classpath[PATH_MAX] = { 0 };
    char user_classpath[PATH_MAX] = { 0 };

    char main_class_name[FILENAME_MAX] = { 0 };

    // parse cmd arguments
    // 可执行程序的名字为 argv[0]，跳过。
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (strcmp(name, "-bcp") == 0) { // parse Bootstrap Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(bootstrap_classpath, argv[i]);
            } else if (strcmp(name, "-cp") == 0) { // parse Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(user_classpath, argv[i]);
            } else {
                jvm_abort("unknown 参数: %s\n", name);
            }
        } else {
            strcpy(main_class_name, argv[i]);
        }
    }

    if (main_class_name[0] == 0) {  // empty
        jvm_abort("no input file\n");
    }

    // 如果 main_class_name 有 .class 后缀，去掉后缀。
    char *p = strrchr(main_class_name, '.');
    if (p != NULL && strcmp(p, ".class") == 0) {
        *p = 0;
    }

    // parse bootstrap classpath
    if (bootstrap_classpath[0] == 0) { // empty
        // 命令行参数没有设置 bootstrap_classpath 的值，那么使用 JAVA_HOME 环境变量
        char *java_home = getenv("JAVA_HOME"); // JAVA_HOME 是 JDK 的目录
        if (java_home == NULL) {
            vm_internal_error("no java lib"); // todo
            return -1;
        }
        strcpy(bootstrap_classpath, java_home);
        strcat(bootstrap_classpath, "/jre/lib");
    }

    find_jars(bootstrap_classpath, jre_lib_jars, &jre_lib_jars_count);

    // 第0个位置放rt.jar，因为rt.jar常用，所以放第0个位置首先搜索。
    for (int i = 0; i < jre_lib_jars_count; i++) {
        char *s = strrchr(jre_lib_jars[i], '\\');
        char *t = strrchr(jre_lib_jars[i], '/');
        if ((s != NULL && strcmp(s + 1, "rt.jar") == 0) || (t != NULL && strcmp(t + 1, "rt.jar") == 0)) { // find
            char tmp[PATH_MAX];
            strcpy(tmp, jre_lib_jars[0]);
            strcpy(jre_lib_jars[0], jre_lib_jars[i]);
            strcpy(jre_lib_jars[i], tmp);
            break;
        }
    }

    // parse extension classpath
    if (extension_classpath[0] == 0) {  // empty
        strcpy(extension_classpath, bootstrap_classpath);
        strcat(extension_classpath, "/ext");  // todo JDK9+ 的目录结构有变动！！！！！！！
    }

    find_jars(extension_classpath, jre_ext_jars, &jre_ext_jars_count);

    // parse user classpath
    if (user_classpath[0] == 0) {  // empty
        char *classpath = getenv("CLASSPATH");
        if (classpath == NULL) {
            getcwd(user_dirs[user_dirs_count++], PATH_MAX); // current working path
        } else {
            strcpy(user_dirs[user_dirs_count++], classpath);
        }
    } else {
        char *delim = ";"; // 各个path以分号分隔
        char *path = strtok(user_classpath, delim);
        while (path != NULL) {
            const char *suffix = strrchr(path, '.');
            if (suffix != NULL && strcmp(suffix, ".jar") == 0) { // jar file
                strcpy(user_jars[user_jars_count++], path);
            } else { // directory
                strcpy(user_dirs[user_dirs_count++], path);
            }
            path = strtok(NULL, delim);
        }
    }

    register_all_native_methods(); // todo 不要一次全注册，需要时再注册

    time_t time2;
    time(&time2);    
    start_jvm(main_class_name);

    time_t time3;
    time(&time3);
    printf("run jvm: %lds\n", ((long)(time3)) - ((long)(time2)));
    return 0;
}


_Noreturn void vm_internal_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}

_Noreturn void vm_out_of_memory_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}

_Noreturn void vm_stack_overflow_error()
{
    // todo
    jvm_abort("");
}

_Noreturn void vm_unknown_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}
