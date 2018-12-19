#include <dirent.h>
#include <sys/stat.h>
#include "loader/classloader.h"
#include "rtda/thread/jthread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/strpool.h"
#include "jvm.h"
#include "rtda/ma/jclass.h"
#include "rtda/heap/jobject.h"
#include "native/registry.h"

/*
 * Author: Jia Yang
 */

char bootstrap_classpath[PATH_MAX] = { 0 };
char extension_classpath[PATH_MAX] = { 0 };
char user_classpath[PATH_MAX] = "./"; // default: current path. todo


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


struct classloader *bootstrap_loader = NULL;

struct jobject *system_thread_group = NULL;
struct jthread *main_thread = NULL;

static void init_jvm(struct classloader *loader, struct jthread *main_thread)
{
    // 先加载sun.mis.VM类，然后执行其类初始化方法
    struct jclass *vm_class = classloader_load_class(loader, "sun/misc/VM");
    if (vm_class == NULL) {
        jvm_abort("vm_class is null\n");  // todo throw exception
        return;
    }

//    struct jmethod *init = jclass_lookup_static_method(vm_class, "initialize", "()V");
//    if (init == NULL) {
//        jvm_abort("error %s\n", jclass_to_string(vm_class));
//        return;
//    }

//    struct stack_frame *init_frame = sf_create(main_thread, init);
//    jthread_push_frame(main_thread, init_frame);
    // vm_class 的 clinit 后于 init_frame加入到线程的执行栈中
    // 保证vm_class 的 clinit方法先执行。
//    jclass_clinit(vm_class, init_frame);  // todo clinit 方法到底在何时调用
    jclass_clinit0(vm_class, main_thread);
    interpret(main_thread);
}

// todo 这函数干嘛的
static void create_system_thread_group(struct classloader *loader)
{
    struct jclass *thread_group_class = classloader_load_class(loader, "java/lang/ThreadGroup"); // todo NULL
    system_thread_group = jobject_create(thread_group_class);

    /*
     * java/lang/ThreadGroup 的无参数构造函数主要用来：
     * Creates an empty Thread group that is not in any Thread group.
     * This method is used to create the system Thread group.
     */
    struct jmethod *constructor = jclass_get_constructor(thread_group_class, "()V");

    // 启动一个临时线程来执行 system thread group 的构造函数
    struct jthread *tmp = jthread_create(loader);
    struct stack_frame *frame = sf_create(tmp, constructor);
    struct slot arg = rslot(system_thread_group);
    sf_set_local_var(frame, 0, &arg);

    jthread_push_frame(tmp, frame);
    jclass_clinit(thread_group_class, frame);  // todo
    interpret(tmp);
    jthread_destroy(tmp);
}

/*
 * main thread 由虚拟机启动。
 */
static void create_main_thread(struct classloader *loader)
{
    main_thread = jthread_create(loader);
    struct jobject *main_thread_obj = jthread_get_obj(main_thread);

    // 调用 java/lang/Thread 的构造函数
    struct jmethod *constructor
            = jclass_get_constructor(main_thread_obj->jclass, "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    struct stack_frame *frame = sf_create(main_thread, constructor);
    frame->local_vars[0] = rslot(main_thread_obj);
    frame->local_vars[1] = rslot(system_thread_group);
    frame->local_vars[2] = rslot((jref) jstrobj_create(MAIN_THREAD_NAME));
    jthread_push_frame(main_thread, frame);
    jclass_clinit0(main_thread_obj->jclass, main_thread); // 最后压栈，保证先执行。

    interpret(main_thread);
}

static char main_class_name[FILENAME_MAX] = { 0 };

static void start_jvm()
{
    build_str_pool();

    struct classloader *loader = classloader_create(true);
    create_system_thread_group(loader);
    create_main_thread(loader);
    init_jvm(loader, main_thread);

    struct jclass *main_class = classloader_load_class(loader, main_class_name);
    if (main_class == NULL) {
        jvm_abort("error %s\n", main_class_name);  // todo
        return;
    }

    struct jmethod *main_method = jclass_lookup_static_method(main_class, "main", "([Ljava/lang/String;)V");
    if (main_method == NULL) {
        jvm_abort("can't find method main.\n");
    } else {
        if (!IS_PUBLIC(main_method->access_flags)) {
            jvm_abort("method main must be public.\n");
        }
        if (!IS_STATIC(main_method->access_flags)) {
            jvm_abort("method main must be static.\n");
        }
    }

    jthread_push_frame(main_thread, sf_create(main_thread, main_method));

    // 开始在主线程中执行 main 方法
    interpret(main_thread);

    // main_thread 退出，做一些清理工作。
    jthread_destroy(main_thread);
    classloader_destroy(loader);
}

/*
 * -bcp path: Bootstrap Class Path, JavaHome路径, 对应 jre/lib 目录。
 * -cp: Class Path, user class path.
 */
static void parse_args(int argc, char* argv[])
{
    // 可执行程序的名字为 argv[0]，跳过。
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (strcmp(name, "-bcp") == 0) { // parse Bootstrap Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(bootstrap_classpath, argv[i]);
            } if (strcmp(name, "-cp") == 0) { // parse Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(user_classpath, argv[i]);
                strcpy(user_dirs[user_dirs_count++], argv[i]); // todo
            } else {
                jvm_abort("不认识的参数：%s\n", name);
            }
        } else {
            strcpy(main_class_name, argv[i]);
        }
    }

    if (main_class_name[0] == 0) {  // empty
        jvm_abort("no input file\n");
    }
}

static void find_jre_lib_jars()
{
    DIR *dir = opendir(bootstrap_classpath);
    if (dir == NULL) {
        printvm("open dir failed. %s\n", bootstrap_classpath);
    }

    // 第0个位置放rt.jar，因为rt.jar常用，所以放第0个位置首先搜索。
    sprintf(jre_lib_jars[0], "%s/%s\0", bootstrap_classpath, "rt.jar");
    jre_lib_jars_count = 1;

    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char abspath[PATH_MAX];
        sprintf(abspath, "%s/%s\0", bootstrap_classpath, entry->d_name); // 绝对路径

        stat(abspath, &statbuf);
        if (S_ISREG(statbuf.st_mode)) { // 常规文件
            char *suffix = strrchr(abspath, '.');
            if (suffix != NULL && strcmp(suffix, ".jar") == 0 && strcmp(entry->d_name, "rt.jar") != 0) {
                strcpy(jre_lib_jars[jre_lib_jars_count++], abspath);
            }
        }
    }

    closedir(dir);
}

static void find_jre_ext_jars()
{
    DIR *dir = opendir(extension_classpath);
    if (dir == NULL) {
        printvm("open dir failed. %s\n", extension_classpath);
    }

    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char abspath[PATH_MAX];
        sprintf(abspath, "%s/%s\0", extension_classpath, entry->d_name); // 绝对路径

        stat(abspath, &statbuf);
        if (S_ISREG(statbuf.st_mode)) { // 常规文件
            char *suffix = strrchr(abspath, '.');
            if (suffix != NULL && strcmp(suffix, ".jar") == 0) {
                strcpy(jre_ext_jars[jre_ext_jars_count++], abspath);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);

    // 命令行参数没有设置 bootstrap_classpath 的值，那么使用 JAVA_HOME 环境变量
    if (bootstrap_classpath[0] == 0) { // empty
        char *java_home = getenv("JAVA_HOME"); // JAVA_HOME 是 JDK 的目录
        if (java_home == NULL) {
            // todo error
            jvm_abort("no java lib");
            return -1;
        }
        strcpy(bootstrap_classpath, java_home);
        strcat(bootstrap_classpath, "/jre/lib");
    }

    find_jre_lib_jars();

    if (extension_classpath[0] == 0) {  // empty
        strcpy(extension_classpath, bootstrap_classpath);
        strcat(extension_classpath, "/ext");  // todo JDK9+ 的目录结构有变动！！！！！！！
    }

    find_jre_ext_jars();

    // 如果 main_class_name 有 .class 后缀，去掉后缀。
    char *p = strrchr(main_class_name, '.');
    if (p != NULL && strcmp(p, ".class") == 0) {
        *p = 0;
    }

#ifdef JVM_DEBUG
    printvm("bootstrap_classpath: %s\n", bootstrap_classpath);
    printvm("extension_classpath: %s\n", extension_classpath);
    printvm("user_classpath: %s\n", user_classpath);
#endif

    register_all_native_methods(); // todo 不要一次全注册，需要时再注册
    start_jvm();
    return 0;
}
