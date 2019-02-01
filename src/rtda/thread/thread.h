/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../../config.h"
#include "../../jtypes.h"

struct classloader;
struct frame;
struct method;
struct object;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */

struct thread {
    struct object *this_obj; // object of java.lang.Thread   todo 干嘛用的

    struct object *jltobj; // object of java/lang/Thread

    u1 vm_stack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    struct frame *top_frame;
};

void create_main_thread(struct classloader *loader);

struct thread *thread_create(struct classloader *loader, struct object *jl_thread_obj);

struct thread *thread_self();

struct frame *alloc_frame(struct method *m, bool vm_invoke);
void pop_frame();

int vm_stack_depth();

void thread_handle_uncaught_exception(struct object *exception);

_Noreturn void thread_throw_null_pointer_exception();
_Noreturn void thread_throw_negative_array_size_exception(int array_size);
_Noreturn void thread_throw_array_index_out_of_bounds_exception(int index);
_Noreturn void thread_throw_class_cast_exception(const char *from_class_name, const char *to_class_name);

void thread_destroy(struct thread *thread);

#endif //JVM_JTHREAD_H
