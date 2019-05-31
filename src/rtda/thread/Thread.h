/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include "../../config.h"
#include "../../jtypes.h"

class Object;
class ClassLoader;
class Frame;
class Method;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */

struct Thread {
    Object *this_obj; // Object of java.lang.Thread   todo 干嘛用的

    Object *jltobj; // Object of java/lang/Thread

    u1 vm_stack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    Frame *top_frame = nullptr;

    Thread(ClassLoader *loader, Object *jl_thread_obj);
};

void create_main_thread(ClassLoader *loader);

Thread *thread_self();

Frame *allocFrame(Method *m, bool vm_invoke);
void popFrame();

int vm_stack_depth();

void thread_handle_uncaught_exception(Object *exception);

void thread_throw_null_pointer_exception();
void thread_throw_negative_array_size_exception(int array_size);
void thread_throw_array_index_out_of_bounds_exception(int index);
void thread_throw_class_cast_exception(const char *from_class_name, const char *to_class_name);

#endif //JVM_JTHREAD_H
