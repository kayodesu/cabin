/*
 * Author: Kayo
 */

#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <iostream>
#include <typeinfo>
#include "kayo.h"
#include "debug.h"
#include "native/registry.h"
#include "runtime/thread.h"
#include "objects/class.h"
#include "objects/Prims.h"
#include "interpreter/interpreter.h"

using namespace std;
using namespace utf8;

#if TRACE_KAYO
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

Heap g_heap;

vector<std::string> jreLibJars;
vector<std::string> jreExtJars;
//vector<std::string> userDirs;
//vector<std::string> userJars;

//StrPool *g_str_pool;

Object *sysThreadGroup;

vector<Thread *> g_all_threads;


static void *gcLoop(void *arg)
{
    // todo
    return nullptr;
}

static void findJars(const char *path, vector<std::string> &result)
{
    DIR *dir = opendir(path);
    if (dir == nullptr) {
        jvm_abort("open dir failed. %s\n", path);
    }

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
            if (suffix != nullptr && strcmp(suffix, ".jar") == 0)
                result.emplace_back(abspath);
        }
    }

    closedir(dir);
}

static char main_class_name[FILENAME_MAX] = { 0 };
static char *main_func_args[METHOD_PARAMETERS_MAX_COUNT];
static int main_func_args_count = 0;

char javaHome[PATH_MAX] = { 0 };
static char bootstrap_classpath[PATH_MAX] = { 0 };
char classpath[PATH_MAX] = { 0 };

static void showUsage(const char *name)
{
//    printf("Usage: %s [-options] class [arg1 arg2 ...]\n", name);
//    printf("\nwhere options include:\n");
//    printf("\t-help\t\tprint out this message\n");
//    printf("\t-version\tprint out version number and copyright information\n");
//    printf("\t-verbose\tprint out information about class loading, etc.\n"); // todo verbose
//    printf("\t-verbosegc\tprint out results of garbage collection\n"); // todo verbose gc
//    printf("\t-noasyncgc\tturn off asynchronous garbage collection\n"); // todo noasync gc
//    printf("\t-bcp<path>\tset the bootstrap class path\n");
//    printf("\t-cp<path>\tset the class path\n");

    printf("Usage: %s [-options] class [arg1 arg2 ...]\n", name);
    printf("                 (to run a class file)\n");
    printf("   or  %s [-options] -jar jarfile [arg1 arg2 ...]\n", name); // todo
    printf("                 (to run a standalone jar file)\n");
    printf("\nwhere options include:\n");
    printf("  -bcp\t\t   <jar/zip files and directories separated by :>\n");
    printf("  -bootclasspath   <jar/zip files and directories separated by :>\n"); // todo
    printf("\t\t   locations where to find the system classes\n");
    printf("  -cp\t\t   <jar/zip files and directories separated by :>\n");
    printf("  -classpath\t   <jar/zip files and directories separated by :>\n"); // todo
    printf("\t\t   locations where to find application classes\n");
    printf("  -D<name>=<value> set a system property\n");
    printf("  -verbose[:class|gc|jni]\n");// todo
    printf("\t\t   :class print out information about class loading, etc.\n");// todo
    printf("\t\t   :gc print out results of garbage collection\n");
    printf("\t\t   :jni print out native method dynamic resolution\n");
    printf("  -version\t   print out version number and copyright information\n");// todo
    printf("  -? -help\t   print out this message\n");

//    printf("  -Xbootclasspath:%s\n", BCP_MESSAGE);
//    printf("\t\t   locations where to find the system classes\n");
//    printf("  -Xbootclasspath/a:%s\n", BCP_MESSAGE);
//    printf("\t\t   locations are appended to the bootstrap class path\n");
//    printf("  -Xbootclasspath/p:%s\n", BCP_MESSAGE);
//    printf("\t\t   locations are prepended to the bootstrap class path\n");
//    printf("  -Xbootclasspath/c:%s\n", BCP_MESSAGE);
//    printf("\t\t   locations where to find Classpath's classes\n");
//    printf("  -Xbootclasspath/v:%s\n", BCP_MESSAGE);
//    printf("\t\t   locations where to find JamVM's classes\n");
//    printf("  -Xasyncgc\t   turn on asynchronous garbage collection\n");
//    printf("  -Xcompactalways  always compact the heap when garbage-collecting\n");
//    printf("  -Xnocompact\t   turn off heap-compaction\n");
//#ifdef INLINING
//    printf("  -Xnoinlining\t   turn off interpreter inlining\n");
//    printf("  -Xshowreloc\t   show opcode relocatability\n");
//    printf("  -Xreplication:[none|always|<value>]\n");
//    printf("\t\t   none : always re-use super-instructions\n");
//    printf("\t\t   always : never re-use super-instructions\n");
//    printf("\t\t   <value> copy when usage reaches threshold value\n");
//    printf("  -Xcodemem:[unlimited|<size>] (default maximum heapsize/4)\n");
//#endif
//    printf("  -Xms<size>\t   set the initial size of the heap\n");
//    printf("\t\t   (default = MAX(physical memory/64, %dM))\n",
//           DEFAULT_MIN_HEAP/MB);
//    printf("  -Xmx<size>\t   set the maximum size of the heap\n");
//    printf("\t\t   (default = MIN(physical memory/4, %dM))\n",
//           DEFAULT_MAX_HEAP/MB);
//    printf("  -Xss<size>\t   set the Java stack size for each thread "
//                   "(default = %dK)\n", DEFAULT_STACK/KB);
//    printf("\t\t   size may be followed by K,k or M,m (e.g. 2M)\n");
}

static void showVersionAndCopyright()
{
    // todo 完善 License
    printf("Java version \"%s\"\n", JAVA_COMPAT_VERSION);
    printf("kayo version %s\n", VM_VERSION);
#if defined(__GNUC__) && defined(__VERSION__)
    printf("Compiled with: g++ %s\n", __VERSION__);
#endif
    printf("Copyright (C) 2020 kayo <kayodesu@outlook.com>\n\n");
    printf("This program is free software; you can redistribute it and/or\n");
    printf("modify it under the terms of the GNU General Public License\n");
    printf("as published by the Free Software Foundation; either version 2,\n");
    printf("or (at your option) any later version.\n\n");
    printf("This program is distributed in the hope that it will be useful,\n");
    printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    printf("GNU General Public License for more details.\n");
//    printf("\nBuild information:\n\nExecution Engine: %s\n", getExecutionEngineName());  // todo
 //   printf("\nBoot Library Path: %s\n", classlibDefaultBootDllPath());  // todo
 //   printf("Boot Class Path: %s\n", classlibDefaultBootClassPath());  // todo
}

static void parseCommandLine(int argc, char *argv[])
{
    // 可执行程序的名字为 argv[0]
    const char *vmName = argv[0];

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (strcmp(name, "-bcp") == 0 or strcmp(name, "-bootclasspath") == 0) { // parse Bootstrap Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(bootstrap_classpath, argv[i]);
            } else if (strcmp(name, "-cp") == 0 or strcmp(name, "-classpath") == 0) { // parse Class Path
                if (++i >= argc) {
                    jvm_abort("缺少参数：%s\n", name);
                }
                strcpy(classpath, argv[i]);
            } else if (strcmp(name, "-help") == 0 or strcmp(name, "-?") == 0) {
                showUsage(vmName);
                exit(0);
            } else if (strcmp(name, "-version") == 0) {
                showVersionAndCopyright();
                exit(0);
            } else {
                printf("Unrecognised command line option: %s\n", argv[i]);
                showUsage(vmName);
                exit(-1);
            }
        } else {
            if (main_class_name[0] == 0) {
                strcpy(main_class_name, argv[i]);
            } else {
                // main function's arguments
                main_func_args[main_func_args_count++] = argv[i];
                if (main_func_args_count > METHOD_PARAMETERS_MAX_COUNT) {
                    // todo many args!!! abort!
                }
            }
        }
    }

    if (main_class_name[0] == 0) {  // empty  todo
        jvm_abort("no input file\n");
    }
}

static void initJVM(int argc, char *argv[])
{
//    char *home = getenv("JAVA_HOME");
//    if (home == nullptr) {
//        jvm_abort("java_lang_InternalError, %s\n", "no java lib"); // todo
//    }
//    strcpy(javaHome, home);

    char extension_classpath[PATH_MAX] = { 0 };
//    char user_classpath[PATH_MAX] = { 0 };

    // parse bootstrap classpath
    if (bootstrap_classpath[0] == 0) { // empty
        // 命令行参数没有设置 bootstrap_classpath 的值，那么使用 JAVA_HOME 环境变量
        char *javaHome = getenv("JAVA_HOME"); // JAVA_HOME 是 JDK 的目录
        if (javaHome == nullptr) {
            jvm_abort("java_lang_InternalError, %s\n", "no java lib"); // todo
        }
        strcpy(bootstrap_classpath, javaHome);
        strcat(bootstrap_classpath, "/jre/lib");
    }

    findJars(bootstrap_classpath, jreLibJars);

    // 第0个位置放rt.jar，因为rt.jar常用，所以放第0个位置首先搜索。
    for (auto iter = jreLibJars.begin(); iter != jreLibJars.end(); iter++) {
        auto i = iter->rfind('\\');
        auto j = iter->rfind('/');
        if ((i != iter->npos && iter->compare(i + 1, 6, "rt.jar") == 0)
            || (j != iter->npos && iter->compare(j + 1, 6, "rt.jar") == 0)) {
            std::swap(*(jreLibJars.begin()), *iter);
            break;
        }
    }

    // parse extension classpath
    if (extension_classpath[0] == 0) {  // empty
        strcpy(extension_classpath, bootstrap_classpath);
        strcat(extension_classpath, "/ext");  // todo JDK9+ 的目录结构有变动！！！！！！！
    }

    findJars(extension_classpath, jreExtJars);

    if (classpath[0] == 0) {  // empty
        char *cp = getenv("CLASSPATH");
        if (cp != nullptr) {
            strcpy(classpath, cp);
        } else {
           getcwd(classpath, PATH_MAX); // current working path
        }
    }
//    else {
//        const char *delim = ";"; // 各个path以分号分隔
//        char *path = strtok(user_classpath, delim);
//        while (path != nullptr) {
//            const char *suffix = strrchr(path, '.');
//            if (suffix != nullptr && strcmp(suffix, ".jar") == 0) { // jar file
//                userJars.emplace_back(path);
//            } else { // directory
//                userDirs.emplace_back(path);
//            }
//            path = strtok(nullptr, delim);
//        }
//    }


    /* order is important */
    initSymbol();
    Prims::init();
    initJNI();
    initClassLoader();
    initMainThread();

    TRACE("init main thread over\n");
    // 先加载 sun.mis.VM 类，然后执行其类初始化方法
    Class *vm = loadBootClass("sun/misc/VM");
    if (vm == nullptr) {
        jvm_abort("sun/misc/VM is null\n");  // todo throw exception
        return;
    }
    // VM类的 "initialize~()V" 方法需调用执行
    // 在VM类的类初始化方法中调用了 "initialize" 方法。
    initClass(vm);
}

int main(int argc, char* argv[])
{
    time_t time1;
    time(&time1);

    parseCommandLine(argc, argv);

    initJVM(argc, argv);
    TRACE("init jvm is over.\n");

    Object *scl = getSystemClassLoader();
    assert(scl != nullptr);

    // Main Thread Set ContextClassLoader
    mainThread->jThread->setFieldValue(S(contextClassLoader), S(sig_java_lang_ClassLoader), (slot_t) scl);

    Class *main_class = loadClass(scl, dots2Slash(main_class_name));
    assert(main_class != nullptr);

    Method *main_method = main_class->lookupStaticMethod(S(main), S(_array_java_lang_String__V));
    if (main_method == nullptr) {
        // java_lang_NoSuchMethodError, "main" todo
        jvm_abort("can't find method main."); // todo
    } else {
        if (!main_method->isPublic()) {
            jvm_abort("method main must be public."); // todo
        }
        if (!main_method->isStatic()) {
            jvm_abort("method main must be static."); // todo
        }
    }

    VMThreadInitInfo gcThreadInfo(gcLoop, GC_THREAD_NAME);
    createVMThread(&gcThreadInfo); // gc thread

    // 开始在主线程中执行 main 方法
    TRACE("begin to execute main function.\n");

    // Create the String array holding the command line args
    jarrref args = newArray(loadArrayClass(S(array_java_lang_String)), main_func_args_count);
    for (int i = 0; i < main_func_args_count; i++) {
        args->set(i, newString(main_func_args[i]));
    }
    // Call the main method
    execJavaFunc(main_method, args);

    // todo 如果有其他的非后台线程在执行，则main线程需要在此wait

    // todo main_thread 退出，做一些清理工作。

    time_t time2;
    time(&time2);

    printf("init jvm: %lds\n", ((long)(time2)) - ((long)(time1)));
    return 0;
}
