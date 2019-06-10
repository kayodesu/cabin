/*
 * Author: Jia Yang
 */

#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include "jvm.h"
#include "loader/ClassLoader.h"
#include "rtda/thread/Thread.h"
#include "rtda/ma/Access.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/StrPool.h"
#include "symbol.h"


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

//size_t g_initial_heap_size = 67108864; // 64Mb  64 * 1024 * 1024
HeapMgr g_heap_mgr;

ClassLoader *g_bootstrap_loader = nullptr;
StrPool g_str_pool;

void init_symbol();

static void start_jvm(const char *main_class_name)
{
    init_symbol();

    auto loader = new ClassLoader(true);
    create_main_thread(loader);

    // 先加载 sun.mis.VM 类，然后执行其类初始化方法
    Class *vm_class = loadSysClass("sun/misc/VM");
    if (vm_class == nullptr) {
        jvm_abort("vm_class is null\n");  // todo throw exception
        return;
    }

    // VM类的 "initialize~()V" 方法需调用执行
    // 在VM类的类初始化方法中调用了 "initialize" 方法。
    vm_class->clinit();

    Class *main_class = loader->loadClass(main_class_name);
    Method *main_method = main_class->lookupStaticMethod(S(main), S(_array_java_lang_String__V));
    if (main_method == nullptr) {
        jvm_abort("can't find method main."); // todo
    } else {
        if (!main_method->isPublic()) {
            jvm_abort("method main must be public."); // todo
        }
        if (!main_method->isStatic()) {
            jvm_abort("method main must be static."); // todo
        }
    }

    // 开始在主线程中执行 main 方法
    slot_t args[] = { 0, (uintptr_t) NULL };
    exec_java_func(main_method, args); //  todo


    // todo 如果有其他的非后台线程在执行，则main线程需要在此wait

    // todo main_thread 退出，做一些清理工作。
}

static void find_jars(const char *path, char jars[][PATH_MAX], int *jars_count)
{
    DIR *dir = opendir(path);
    if (dir == nullptr) {
        printvm("open dir failed. %s\n", path);
    }

    int count = 0;
    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char abspath[PATH_MAX];
        // sprintf 和 snprintf 会自动在加上字符串结束符'\0'
        sprintf(abspath, "%s/%s", path, entry->d_name); // 绝对路径

        stat(abspath, &statbuf);
        if (S_ISREG(statbuf.st_mode)) { // 常规文件
            char *suffix = strrchr(abspath, '.');
            if (suffix != nullptr && strcmp(suffix, ".jar") == 0) {
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
    if (p != nullptr && strcmp(p, ".class") == 0) {
        *p = 0;
    }

    // parse bootstrap classpath
    if (bootstrap_classpath[0] == 0) { // empty
        // 命令行参数没有设置 bootstrap_classpath 的值，那么使用 JAVA_HOME 环境变量
        char *java_home = getenv("JAVA_HOME"); // JAVA_HOME 是 JDK 的目录
        if (java_home == nullptr) {
            vm_internal_error("no java lib"); // todo
        }
        strcpy(bootstrap_classpath, java_home);
        strcat(bootstrap_classpath, "/jre/lib");
    }

    find_jars(bootstrap_classpath, jre_lib_jars, &jre_lib_jars_count);

    // 第0个位置放rt.jar，因为rt.jar常用，所以放第0个位置首先搜索。
    for (int i = 0; i < jre_lib_jars_count; i++) {
        char *s = strrchr(jre_lib_jars[i], '\\');
        char *t = strrchr(jre_lib_jars[i], '/');
        if ((s != nullptr && strcmp(s + 1, "rt.jar") == 0) || (t != nullptr && strcmp(t + 1, "rt.jar") == 0)) { // find
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
        if (classpath == nullptr) {
            getcwd(user_dirs[user_dirs_count++], PATH_MAX); // current working path
        } else {
            strcpy(user_dirs[user_dirs_count++], classpath);
        }
    } else {
        const char *delim = ";"; // 各个path以分号分隔
        char *path = strtok(user_classpath, delim);
        while (path != nullptr) {
            const char *suffix = strrchr(path, '.');
            if (suffix != nullptr && strcmp(suffix, ".jar") == 0) { // jar file
                strcpy(user_jars[user_jars_count++], path);
            } else { // directory
                strcpy(user_dirs[user_dirs_count++], path);
            }
            path = strtok(nullptr, delim);
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

void vm_internal_error(const char *msg)
{
    assert(msg != nullptr);
    // todo
    jvm_abort(msg);
}

void vm_out_of_memory_error(const char *msg)
{
    assert(msg != nullptr);
    // todo
    jvm_abort(msg);
}

void vm_stack_overflow_error()
{
    // todo
    jvm_abort("");
}

void vm_unknown_error(const char *msg)
{
    assert(msg != nullptr);
    // todo
    jvm_abort(msg);
}
