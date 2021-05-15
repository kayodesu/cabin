#include <time.h>
#include "cabin.h"
#include "jni.h"


void show_usage(const char *name);
void show_version_and_copyright();

static char main_class_name[FILENAME_MAX] = { 0 };
static char *main_func_args[METHOD_PARAMETERS_MAX_COUNT];
static int main_func_args_count = 0;

static void parse_command_line(int argc, char *argv[])
{
    // 可执行程序的名字为 argv[0]
    const char *vm_name = argv[0];

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (strcmp(name, "-bcp") == 0 || strcmp(name, "-bootclasspath") == 0) { // parse Bootstrap Class Path
                if (++i >= argc) {
                    JVM_PANIC("缺少参数：%s\n", name);
                }
                set_bootstrap_classpath(argv[i]);
            } else if (strcmp(name, "-cp") == 0 || strcmp(name, "-classpath") == 0) { // parse Class Path
                if (++i >= argc) {
                    JVM_PANIC("缺少参数：%s\n", name);
                }
                set_classpath(argv[i]);
            } else if (strcmp(name, "-help") == 0 || strcmp(name, "-?") == 0) {
                show_usage(vm_name);
                exit(0);
            } else if (strcmp(name, "-version") == 0) {
                show_version_and_copyright();
                exit(0);
            } else {
                printf("Unrecognised command line option: %s\n", argv[i]);
                show_usage(vm_name);
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
}


JavaVM *g_vm;
JNIEnv *g_jni_env;

int main(int argc, char* argv[])
{
    time_t time1;
    time(&time1);
    
    parse_command_line(argc, argv);
    JNI_CreateJavaVM(&g_vm, (void **) &g_jni_env, NULL);

    if (main_class_name[0] == 0) {  // empty  todo
        JVM_PANIC("no input file\n");
    }

    Class *main_class = load_class(g_app_class_loader, dot_to_slash(main_class_name));
    if (main_class == NULL) {
        JVM_PANIC("main_class == NULL"); // todo
    }

    init_class(main_class);

    Method *main_method = lookup_static_method(main_class, S(main), S(_array_java_lang_String__V));
    if (main_method == NULL) {
        // java_lang_NoSuchMethodError, "main" todo
        JVM_PANIC("can't find method main."); // todo
    } else {
        if (!IS_PUBLIC(main_method)) {
            JVM_PANIC("method main must be public."); // todo
        }
        if (!IS_STATIC(main_method)) {
            JVM_PANIC("method main must be static."); // todo
        }
    }

    // 开始在主线程中执行 main 方法
    TRACE("begin to execute main function.\n");

    // Create the String array holding the command line args
    jarrRef args = alloc_string_array(main_func_args_count);
    for (int i = 0; i < main_func_args_count; i++) {
        array_set_ref(args, i, alloc_string(main_func_args[i]));
    }

    // Call the main method
    exec_java(main_method, (slot_t[]) { rslot(args) });

    // todo 如果有其他的非后台线程在执行，则main线程需要在此wait

    // todo main_thread 退出，做一些清理工作。

    time_t time2;
    time(&time2);

    printf("\n--- Powered by CabinVM(%lds) ---\n", ((long)(time2)) - ((long)(time1)));
    return 0;
}