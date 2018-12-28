/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <stddef.h>
#include <stdbool.h>
#include "../../util/vector.h"

struct classloader;
struct frame;
struct method;
struct object;
struct slot;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */


struct invokedynamic_temp_store {
    // java/lang/invoke/MethodType
    struct object *invoked_type;

    // java/lang/invoke/MethodHandles$Lookup
    struct object *caller;

    // java/lang/invoke/CallSite
    struct object *call_set;

    // java/lang/invoke/MethodHandle
    struct object *exact_method_handle;
};

#define FRMHUB_SLOTS_COUNT_MAX 32

struct thread {
    struct vector vm_stack; // 虚拟机栈，一个线程只有一个虚拟机栈
//    struct object *this_obj; // object of java.lang.Thread   todo 干嘛用的

    struct vector frame_cache[FRMHUB_SLOTS_COUNT_MAX];

    struct object *jltobj; // object of java/lang/Thread

    struct invokedynamic_temp_store dyn;
};

struct thread* thread_create(struct classloader *loader, struct object *jl_thread_obj);

//void jthread_set_pc(struct thread *thread, size_t new_pc);
//size_t jthread_get_pc(const struct thread *thread);

bool thread_is_stack_empty(const struct thread *thread);

int thread_stack_depth(const struct thread *thread);

struct frame* thread_top_frame(struct thread *thread);

struct frame* thread_stack_frame_from_top(struct thread *thread, int from_top);

struct frame* thread_pop_frame(struct thread *thread);

void thread_recycle_frame(struct frame *frame);

/*
 * 返回完整的虚拟机栈
 * 顺序为由栈底到栈顶
 * 由调用者释放返回的 array of struct frame *
 */
struct frame** thread_get_frames(const struct thread *thread, int *num);

/*
 * 生成包含@method的栈帧，并将其压入@thread的虚拟机栈中，
 * 同时中断当前虚拟机栈栈顶的栈帧，以期执行@method所对应的新生成的栈帧。
 *
 * Note：这里只是生成栈帧并压栈，如果在一个循环中调用此方法，会造成联系压入多个栈帧，如：
 * 当前虚拟机栈：
 * ...|top frame|
 * 执行下面语句
 *      for(int i = 0; i < 3; i++) jthread_invoke_method(...);
 * 后的虚拟机栈：
 * ...|top frame|new frame 1|new frame 2|new frame 3|
 * top frame中断后会执行new frame 3，但是如果@method方法有返回值，
 * 执行new frame 3后，其返回值会压入new frame 2，这是错误的，
 * 因为不是new frame 2调用的它，是top frame调用的。
 * 这种错误的压入会造成new frame 2的操作栈溢出（错误的压入new frame 3的返回值所致）。
 *
 * 综上：不支持在循环中调用 jthread_invoke_method 来执行带返回值的方法（@method）。
 */
void thread_invoke_method(struct thread *thread, struct method *method, const struct slot *args);

/*
 * 这个函数存在的意义是为了解决函数jthread_invoke_method无法在循环中执行带返回值的方法的问题，
 * 参加 jthread_invoke_method 的注释。
 */
void thread_invoke_method_with_shim(struct thread *thread, struct method *method, const struct slot *args,
                                     void (* shim_action)(struct frame *));

void thread_handle_uncaught_exception(struct thread *thread, struct object *exception);

_Noreturn void thread_throw_null_pointer_exception(struct thread *thread);
_Noreturn void thread_throw_negative_array_size_exception(struct thread *thread, int array_size);
_Noreturn void thread_throw_array_index_out_of_bounds_exception(struct thread *thread, int index);
_Noreturn void thread_throw_class_cast_exception(
        struct thread *thread, const char *from_class_name, const char *to_class_name);

void thread_destroy(struct thread *thread);

#endif //JVM_JTHREAD_H
