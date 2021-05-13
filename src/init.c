#include <time.h>
#include <windows.h>
#include "cabin.h"
#include "util/encoding.h"
#include "heap.h"
#include "jni.h"
#include "util/endianness.h"

Heap *g_heap;

// vector<pair<const utf8_t *, const utf8_t *>> g_properties;
struct property g_properties[PROPERTIES_MAX_COUNT];
int g_properties_count = 0;

Object *g_sys_thread_group;

//vector<Thread *> g_all_threads;
Thread *g_all_threads[VM_THREADS_MAX_COUNT];
int g_all_threads_count = 0;

char g_java_home[PATH_MAX] = { 0 };

u2 g_classfile_major_version = 0;
u2 g_classfile_manor_version = 0;

Object *g_app_class_loader;
Object *g_platform_class_loader;

bool g_vm_initing = true;

void init_classpath();
void init_dll();
void init_native();

static void *gc_loop(void *arg)
{
    // todo
    return NULL;
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
void init_properties()
{
    g_properties_count = 0;
    g_properties[g_properties_count++] = (struct property) { "java.version", VM_VERSION };
    g_properties[g_properties_count++] = (struct property) { "java.vendor", "cabin" };
    g_properties[g_properties_count++] = (struct property) { "java.vendor.url", "doesn't have" };
    g_properties[g_properties_count++] = (struct property) { "java.home", g_java_home };
    utf8_t *class_version = vm_malloc(sizeof(utf8_t) * 32);
    sprintf(class_version, "%d.%d",
            JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION, JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION);
    g_properties[g_properties_count++] = (struct property) { "java.class.version", class_version };
    g_properties[g_properties_count++] = (struct property) { "java.class.path", get_classpath()};
    g_properties[g_properties_count++] = (struct property) { "os.name", os_name() };
    g_properties[g_properties_count++] = (struct property) { "os.arch", os_arch() };
    g_properties[g_properties_count++] = (struct property) { "os.version",  "" }; // todo
    g_properties[g_properties_count++] = (struct property) { "file.separator", get_file_separator() };
    g_properties[g_properties_count++] = (struct property) { "path.separator", get_path_separator() };
    g_properties[g_properties_count++] = (struct property) { "line.separator", get_line_separator() }; // System.out.println最后输出换行符就会用到这个
    g_properties[g_properties_count++] = (struct property) { "user.name", "" };// todo
    g_properties[g_properties_count++] = (struct property) { "user.home", "" };// todo
    g_properties[g_properties_count++] = (struct property) { "user.dir", "" };// todo
    g_properties[g_properties_count++] = (struct property) { "user.country", "CN" }; // todo
    g_properties[g_properties_count++] = (struct property) { "file.encoding", "UTF-8" };// todo
    g_properties[g_properties_count++] = (struct property) { "sun.stdout.encoding", "UTF-8" };// todo
    g_properties[g_properties_count++] = (struct property) { "sun.stderr.encoding", "UTF-8" };// todo

    if (g_properties_count > PROPERTIES_MAX_COUNT) {
        JVM_PANIC("Too many properties.");
    }
}

static void init_heap()
{
    g_heap = create_heap();
    if (g_heap == NULL) {
        JVM_PANIC("init Heap failed"); // todo
    }
}

// JDK major version to classfile major version
#define CLASSFILE_VERSION(jdk_version) ((jdk_version) -1 + 45)

// Access JAVA_HOME/release file to get the version of JDK
static void read_jdk_version()
{
    char release[PATH_MAX];
    strcpy(release, g_java_home);
    strcat(release, "/release");
    FILE *fp = fopen(release, "r");
    if (fp == NULL) {
        JVM_PANIC("打开文件失败" ); // todo
    }

#define JAVA_HOME_RELEASE_FILE_LINE_MAX 5120 // May be big enough?
    char line[JAVA_HOME_RELEASE_FILE_LINE_MAX] = { 0 };
    const char *begin = "JAVA_VERSION=\"";

    // 读取文本，直到碰到'\n'
    while (fscanf(fp, "%[^\n]\n", line) != EOF) {
        // JAVA_VERSION="x.x.x_xxx" // jdk8及其以下的jdk, JAVA_VERSION="1.8.0_221"
        // JAVA_VERSION="xx.xx.xx"  // jdk9及其以上的jdk, JAVA_VERSION="11.0.1", JAVA_VERSION="15"

        // JDK版本与class file版本对应关系
        // JDK 1.1 = 45，JDK 1.2 = 46, ... 以此类推。
        char *pos = strstr(line, begin);
        if (pos != NULL) {
            pos += strlen(begin);

            char *underline;
            if ((underline = strchr(pos, '_')) != NULL) {
                // jdk8及其以下的jdk
                assert(pos[0] == '1');
                assert(pos[1] == '.');
                pos += 2; // jump "1."
                g_classfile_major_version = CLASSFILE_VERSION(*pos - '0');
                pos += 2; // jump "x."
                g_classfile_manor_version = 0; // todo
                // g_classfile_manor_version = stoi(line.substr(pos, underline - pos));
            } else {
                // jdk9及其以上的jdka
                char *t = strchr(pos, '.');
                if (t == NULL) { // 形如 JAVA_VERSION="15"
                    g_classfile_major_version = CLASSFILE_VERSION(atoi(pos));
                    g_classfile_manor_version = 0;
                } else { // 形如 JAVA_VERSION="11.0.1"
                    *t = 0;
                    g_classfile_major_version = CLASSFILE_VERSION(atoi(pos));
                    pos = t + 1; // jump '.'
                    t = strchr(pos, '.');
                    if (t != NULL) {
                        *t = 0;
                    }
                    g_classfile_major_version = CLASSFILE_VERSION(atoi(pos));
                }
            }
            break;
        }

        memset(line, 0, JAVA_HOME_RELEASE_FILE_LINE_MAX * sizeof(*line));
    }

    fclose(fp);
    
    if (g_classfile_major_version > JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION
        || g_classfile_manor_version > JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION) {
        JVM_PANIC("不支持的jdk版本"); // todo
    }
}

pthread_mutexattr_t g_pthread_mutexattr_recursive;

void init_jvm(JavaVMInitArgs *vm_init_args)
{    
    pthread_mutexattr_init(&g_pthread_mutexattr_recursive);
    pthread_mutexattr_settype(&g_pthread_mutexattr_recursive, PTHREAD_MUTEX_RECURSIVE);

    // parse_command_line(argc, argv);

    char *home = getenv("JAVA_HOME");
    if (home == NULL) {
        JVM_PANIC("java_lang_InternalError, %s\n", "no java lib"); // todo
    }
    
    strcpy(g_java_home, home);

//    g_java_home = R"(C:\Program Files\Java\jre1.8.0_221)"; // todo for testing ...........
//    g_java_home = R"(C:\Program Files\Java\jdk-11.0.1)"; // todo for testing .............
//    g_java_home = R"(C:\Program Files\Java\jdk-15)"; // todo for testing .................

    read_jdk_version();

    /* order is important */
    init_utf8_pool();
    init_classpath();
    init_symbol();
    init_prims();
    init_heap();
    init_properties();
    init_class_loader();
    init_native();
    init_dll();
    init_main_thread();
    init_method_handle();

    // --------------------------------------

    // 设置 jdk/internal/misc/UnsafeConstants 的下列值
    // static final int ADDRESS_SIZE0;
    // static final int PAGE_SIZE;
    // static final boolean BIG_ENDIAN;
    // static final boolean UNALIGNED_ACCESS;
    // static final int DATA_CACHE_LINE_FLUSH_SIZE;
    Class *uc = load_boot_class("jdk/internal/misc/UnsafeConstants");
    init_class(uc);

    lookup_static_field(uc, "ADDRESS_SIZE0", "I")->static_value.i = sizeof(void *);
    lookup_static_field(uc, "PAGE_SIZE", "I")->static_value.i = page_size();
    lookup_static_field(uc, "BIG_ENDIAN", "Z")->static_value.z = is_big_endian();
    // todo UNALIGNED_ACCESS
    // todo DATA_CACHE_LINE_FLUSH_SIZE

    // --------------------------------------

    Class *sys = load_boot_class(S(java_lang_System));
    init_class(sys);
    
    Method *m = lookup_static_method(sys, "initPhase1", S(___V));
    assert(m != NULL);
    exec_java_func(m, NULL);

    // todo
    // AccessibleObject.java中说AccessibleObject类会在initPhase1阶段初始化，
    // 但我没有在initPhase1中找到初始化AccessibleObject的代码
    // 所以`暂时`先在这里初始化一下。待日后研究清楚了再说。
    Class *acc = load_boot_class("java/lang/reflect/AccessibleObject");
    init_class(acc);

    //   todo "initPhase2 is not implement    
    // m = lookup_static_method(sys, "initPhase2", "(ZZ)I");
    // assert(m != NULL);
    // jint ret = slot_get_int(exec_java_func(m, (slot_t[]) {islot(1), islot(1)}));
    // assert(ret == 0); // 等于0表示成功

    m = lookup_static_method(sys, "initPhase3", S(___V));
    assert(m != NULL);
    exec_java_func(m, NULL);

    // --------------------------------------

    g_platform_class_loader = get_platform_class_loader();
    assert(g_platform_class_loader != NULL);
    g_app_class_loader = get_app_class_loader();
    assert(g_app_class_loader != NULL);

    // Main Thread Set ContextClassLoader
    set_ref_field(g_main_thread->tobj, S(contextClassLoader),
                                     S(sig_java_lang_ClassLoader), g_app_class_loader);

                                     
    create_vm_thread(gc_loop, GC_THREAD_NAME); // gc thread
    
    g_vm_initing = false;
    TRACE("init jvm is over.\n");
}

void show_usage(const char *name)
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

void show_version_and_copyright()
{
    printf("Java version \"%s\"\n", JAVA_COMPAT_VERSION);
    printf("kayo version %s\n", VM_VERSION);
#if defined(__GNUC__) && defined(__VERSION__)
    printf("Compiled with: g++ %s\n", __VERSION__);
#endif
    printf("Copyright (C) 2020 Yo Ka <kayodesu@outlook.com>\n\n");
//    printf("\nBuild information:\n\nExecution Engine: %s\n", getExecutionEngineName());  // todo
    //   printf("\nBoot Library Path: %s\n", classlibDefaultBootDllPath());  // todo
    //   printf("Boot Class Path: %s\n", classlibDefaultBootClassPath());  // todo
}

