/*
 * Author: Yo Ka
 */

#include <ctime>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include "jvmstd.h"
#include "debug.h"
#include "runtime/thread_info.h"
#include "objects/class.h"
#include "objects/method.h"
#include "objects/array_object.h"
#include "interpreter/interpreter.h"
#include "memory/Heap.h"

using namespace std;
using namespace std::filesystem;
using namespace utf8;

#if TRACE_KAYO
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

Heap *g_heap;

bool g_jdk_version_9_and_upper;

vector<string> jreLibJars;
vector<string> jreExtJars;

vector<string> g_jdk_modules;

//IFN ifn;

//vector<std::string> userDirs;
//vector<std::string> userJars;

//StrPool *g_str_pool;

vector<pair<const utf8_t *, const utf8_t *>> g_properties;

Object *sysThreadGroup;

vector<Thread *> g_all_threads;


static void *gcLoop(void *arg)
{
    // todo
    return nullptr;
}

static void findFilesBySuffix(const char *__path, const char *suffix, vector<std::string> &result)
{
    path curr_path(__path);
    if (!exists(curr_path)) {
        // todo error
        return;
    }

    directory_entry entry(curr_path);
    if (entry.status().type() != file_type::directory) {
        // todo error
        return;
    }

    directory_iterator files(curr_path);
    for (auto& f: files) {
        if (f.is_regular_file()) {
            char abspath[PATH_MAX];
            // sprintf 和 snprintf 会自动在加上字符串结束符'\0'
            sprintf(abspath, "%s/%s", __path, f.path().filename().string().c_str()); // 绝对路径

            char *tmp = strrchr(abspath, '.');
            if (tmp != nullptr && strcmp(++tmp, suffix) == 0)
                result.emplace_back(abspath);
        }
    }
}

static char main_class_name[FILENAME_MAX] = { 0 };
static char *main_func_args[METHOD_PARAMETERS_MAX_COUNT];
static int main_func_args_count = 0;

string g_java_home;

u2 g_classfile_major_version = 0;
u2 g_classfile_manor_version = 0;

static char bootstrap_classpath[PATH_MAX] = { 0 };
char classpath[PATH_MAX] = { 0 };

static void showUsage(const char *name);
static void showVersionAndCopyright();

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

/*
 * System properties. The following properties are guaranteed to be defined:
 * java.version         Java version number
 * java.vendor          Java vendor specific string
 * java.vendor.url      Java vendor URL
 * java.home            Java installation directory
 * java.class.version   Java class version number
 * java.class.path      Java classpath
 * os.name              Operating System Name
 * os.arch              Operating System Architecture
 * os.version           Operating System Version
 * file.separator       File separator ("/" on Unix)
 * path.separator       Path separator (":" on Unix)
 * line.separator       Line separator ("\n" on Unix)
 * user.name            User account name
 * user.home            User home directory
 * user.dir             User's current working directory
 */
static void initProperties()
{
    g_properties.emplace_back("java.version", VM_VERSION);
    g_properties.emplace_back("java.vendor", "kayo" );
    g_properties.emplace_back("java.vendor.url", "doesn't have");
    g_properties.emplace_back("java.home", g_java_home.c_str());
    auto class_version = new utf8_t[32];
    sprintf(class_version, "%d.%d",
            JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION, JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION);
    g_properties.emplace_back("java.class.version", class_version);
    g_properties.emplace_back("java.class.path", classpath);
    g_properties.emplace_back("os.name", "");
    g_properties.emplace_back("os.arch", "");
    g_properties.emplace_back("os.version",  "");
    g_properties.emplace_back("file.separator", "\\"); // todo 根据操作系统判断
    g_properties.emplace_back("path.separator", ";"); // todo 根据操作系统判断
    g_properties.emplace_back("line.separator", "\r\n"); // System.out.println最后输出换行符就会用到这个  // todo 根据操作系统判断
    g_properties.emplace_back("user.name", "");
    g_properties.emplace_back("user.home", "");
    g_properties.emplace_back("user.dir", "");
    g_properties.emplace_back("user.country", "CN");
    g_properties.emplace_back("file.encoding", "UTF-8");
    g_properties.emplace_back("sun.stdout.encoding", "UTF-8");
    g_properties.emplace_back("sun.stderr.encoding", "UTF-8");
}

static void initHeap()
{
    g_heap = new Heap;
    if (g_heap == nullptr) {
        jvm_abort("init Heap failed"); // todo
    }
}

void initJNI();

static void initJVM(int argc, char *argv[])
{
    char *home = getenv("JAVA_HOME");
    if (home == nullptr) {
        jvm_abort("java_lang_InternalError, %s\n", "no java lib"); // todo
    }
    g_java_home = home;

    g_java_home = R"(C:\Program Files\Java\jdk1.8.0_221)"; // todo for testing ...............................

    /* Access JAVA_HOME/release file to get the version of JDK */
    ifstream ifs(g_java_home + "/release");
    if(!ifs.is_open()){
        jvm_abort("打开文件失败" ); // todo
    }
    string line;
    while(getline(ifs, line)){
        // JAVA_VERSION="x.x.x_xxx" // jdk8及其以下的jdk, JAVA_VERSION="1.8.0_221"
        // JAVA_VERSION="xx.xx.xx"  // jdk9及其以上的jdk, JAVA_VERSION="11.0.1"

        // JDK版本与class file版本对应关系
        // JDK 1.1 = 45，JDK 1.2 = 46, ... 以此类推。
        const char *begin = R"(JAVA_VERSION=")";
        size_t pos = line.find(begin);
        if (pos != string::npos) {
            pos += strlen(begin);
            size_t underline;
            if ((underline = line.find_first_of('_', pos)) != string::npos) {
                // jdk8及其以下的jdk
                g_jdk_version_9_and_upper = false;
                assert(line[pos] == '1');
                assert(line[pos+1] == '.');
                pos += 2; // jump "1."
                g_classfile_major_version = stoi(line.substr(pos, 1)) - 1 + 45;
                pos += 2; // jump "x."
                g_classfile_manor_version = stoi(line.substr(pos, underline - pos));
            } else {
                // jdk9及其以上的jdk
                g_jdk_version_9_and_upper = true;
                size_t t = line.find_first_of('.', pos);
                g_classfile_major_version = stoi(line.substr(pos, t - pos)) - 1 + 45;
                pos = ++t; // jump '.'
                t = line.find_first_of('.', pos);
                g_classfile_manor_version = stoi(line.substr(pos, t - pos));
            }
            break;
        }
    }
    ifs.close();
    if (g_classfile_major_version > JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION
            || g_classfile_manor_version > JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION) {
        jvm_abort("不支持的jdk版本"); // todo
    }

    if (g_jdk_version_9_and_upper) { // jdk9 开始使用模块
        findFilesBySuffix((g_java_home + "/jmods").c_str(), "jmod", g_jdk_modules);

        // 第0个位置放java.base.jmod，因为java.base.jmod常用，所以放第0个位置首先搜索。
        for (auto iter = g_jdk_modules.begin(); iter != g_jdk_modules.end(); iter++) {
            auto i = iter->rfind('\\');
            auto j = iter->rfind('/');
            if ((i != iter->npos && iter->compare(i + 1, 6, "java.base.jmod") == 0)
                || (j != iter->npos && iter->compare(j + 1, 6, "java.base.jmod") == 0)) {
                std::swap(*(g_jdk_modules.begin()), *iter);
                break;
            }
        }
    } else {
        char extension_classpath[PATH_MAX] = { 0 };
//    char user_classpath[PATH_MAX] = { 0 };

        // parse bootstrap classpath
        if (bootstrap_classpath[0] == 0) { // empty
            strcpy(bootstrap_classpath, g_java_home.c_str());
            strcat(bootstrap_classpath, "/jre/lib");
        }

        findFilesBySuffix(bootstrap_classpath, "jar", jreLibJars);

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

        findFilesBySuffix(extension_classpath, "jar", jreExtJars);
    }

    if (classpath[0] == 0) {  // empty
        char *cp = getenv("CLASSPATH");
        if (cp != nullptr) {
            strcpy(classpath, cp);
        } else {
           // todo error. no CLASSPATH！
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
    initHeap();
    initProperties();
    initJNI();
    initClassLoader();
    initMainThread();

    TRACE("init main thread over\n");
    // 先加载 sun.mis.VM or jdk.internal.misc.VM 类，然后执行其类初始化方法
    Class *vm = loadBootClass("sun/misc/VM");
    if (vm == nullptr) {
        vm = loadBootClass("jdk/internal/misc/VM");
    }
    if (vm == nullptr) {
        jvm_abort("xxx/misc/VM is null\n");  // todo throw exception
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
    mainThread->tobj->setRefField(S(contextClassLoader), S(sig_java_lang_ClassLoader), scl);

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

    createVMThread(gcLoop, GC_THREAD_NAME); // gc thread

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

    printf("run jvm: %lds\n", ((long)(time2)) - ((long)(time1)));
    return 0;
}


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
