#include <limits.h>
#include <assert.h>
#include "loader/classloader.h"
#include "rtda/thread/jthread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"

/*
 * Author: Jia Yang
 */

char bootstrap_classpath[PATH_MAX] = { 0 };
char extension_classpath[PATH_MAX] = { 0 };
char *user_classpath = "D:\\code\\jvm\\test"; // todo;

void start_jvm(const char *main_class_name)
{
    assert(main_class_name != NULL);

    struct classloader *loader = classloader_create();
    struct jthread *main_thread = jthread_create();

#if 0
    createMainThreadGroup();
    mainThread->joinToMainThreadGroup();

    initJVM(classLoader, mainThread);
#endif

    printvm("LOADING main class!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    struct jclass *main_class = classloader_load_class(loader, main_class_name);
    if (main_class == NULL) {
        jvm_abort("error %s\n", main_class_name);  // todo
        return;
    }

    struct jmethod *main_method = jclass_lookup_static_method(main_class, "main", "([Ljava/lang/String;)V");
    if (main_method == NULL) {
        jvm_abort("没有找到main方法\n");  // todo
    } else {
        if (!IS_PUBLIC(main_method->access_flags)) {
            // todo
            jvm_abort("..................");
        }
        if (!IS_STATIC(main_method->access_flags)) {
            // todo
            jvm_abort("...................");
        }
    }

    jthread_push_frame(main_thread, sf_create(main_thread, main_method));

    // 开始在主线程中执行 main 方法
    interpret(main_thread);

    // main_thread 退出，做一些清理工作。
    jthread_destroy(main_thread);
    classloader_destroy(loader);
}
