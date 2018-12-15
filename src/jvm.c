#include "loader/classloader.h"
#include "rtda/thread/jthread.h"
#include "rtda/ma/access.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/strpool.h"
#include "jvm.h"
#include "rtda/ma/jclass.h"
#include "rtda/heap/jobject.h"
#include "util/util.h"

/*
 * Author: Jia Yang
 */

char bootstrap_classpath[PATH_MAX] = { 0 };
char extension_classpath[PATH_MAX] = { 0 };
char *user_classpath = "D:\\code\\jvm\\testclasses"; // todo;

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

void start_jvm(const char *main_class_name)
{
    assert(main_class_name != NULL);

    build_str_pool();

    struct classloader *loader = classloader_create(true);
    create_system_thread_group(loader);
    create_main_thread(loader);
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

//void print_properties()
//{
//    if (bootstrap_loader == NULL) {
//        return;
//    }
//
//    struct jclass *c = classloader_find_class(bootstrap_loader, "java/lang/System");
//    if (c == NULL) {
//        return;
//    }
//
//    struct jfield *f = jclass_lookup_static_field(c, "props", "java/util/Properties");
//}