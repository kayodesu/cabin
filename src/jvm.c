#include "loader/classloader.h"
#include "rtda/thread/jthread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/strpool.h"
#include "jvm.h"
#include "rtda/ma/jclass.h"
#include "rtda/heap/jobject.h"

/*
 * Author: Jia Yang
 */

char bootstrap_classpath[PATH_MAX] = { 0 };
char extension_classpath[PATH_MAX] = { 0 };
char *user_classpath = "D:\\code\\jvm\\testclasses"; // todo;


static void init_jvm(struct classloader *loader, struct jthread *main_thread)
{
    // 先加载sun.mis.VM类，然后执行其类初始化方法
    struct jclass *vm_class = classloader_load_class(loader, "sun/misc/VM");
    if (vm_class == NULL) {
        jvm_abort("vm_class is null\n");  // todo throw exception
        return;
    }

    struct jmethod *init = jclass_lookup_static_method(vm_class, "initialize", "()V");
    if (init == NULL) {
        jvm_abort("error %s\n", jclass_to_string(vm_class));
        return;
    }

    struct stack_frame *init_frame = sf_create(main_thread, init);
    jthread_push_frame(main_thread, init_frame);
    // vm_class 的 clinit 后于 init_frame加入到线程的执行栈中
    // 保证vm_class 的 clinit方法先执行。
    jclass_clinit(vm_class, init_frame);  // todo clinit 方法到底在何时调用
    interpret(main_thread);
}

// todo 这函数干嘛的
void create_main_thread_group(struct classloader *loader, struct jthread *main_thread)
{
    struct jclass *thread_group_class = classloader_load_class(loader, "java/lang/ThreadGroup"); // todo is NULL
    struct jobject *main_thread_group = jobject_create(thread_group_class);

    struct jmethod * constructor = jclass_get_constructor(thread_group_class, "()V");
    struct stack_frame *frame = sf_create(main_thread, constructor);
    struct slot arg = rslot(main_thread_group);
    sf_set_local_var(frame, 0, &arg);

    jthread_push_frame(main_thread, frame);
    jclass_clinit(thread_group_class, frame);  // todo
    interpret(main_thread);
}

void start_jvm(const char *main_class_name)
{
    assert(main_class_name != NULL);

    build_str_pool();

    struct classloader *loader = classloader_create();
    struct jthread *main_thread = jthread_create();

#if 0
    createMainThreadGroup();
    mainThread->joinToMainThreadGroup();
#endif

    init_jvm(loader, main_thread);

#ifdef JVM_DEBUG
    printvm("loading main class: %s !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", main_class_name);
#endif

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
