/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <limits.h>
#include <stdbool.h>
#include "../heap/jobject.h"
#include "../../../lib/uthash/utarray.h"

struct stack_frame;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */
struct jthread {
    UT_array *vm_stack; // 虚拟机栈，一个线程只有一个虚拟机栈
    struct jobject *jlt_obj; // object of java.lang.Thread   todo 干嘛用的

    size_t pc;
};

struct jthread* jthread_create();

static inline bool jthread_is_stack_empty(const struct jthread *thread)
{
    assert(thread != NULL);
    return utarray_len(thread->vm_stack) == 0;
}

static inline struct stack_frame* jthread_top_frame(struct jthread *thread)
{
    assert(thread != NULL);
    void *p = utarray_back(thread->vm_stack);
    return p == NULL ? NULL : *(struct stack_frame **)p;
}

static inline void jthread_pop_frame(struct jthread *thread)
{
    assert(thread != NULL);
    utarray_pop_back(thread->vm_stack);
}

void jthread_push_frame(struct jthread *thread, struct stack_frame *frame);

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
void jthread_invoke_method(struct jthread *thread, struct jmethod *method, const struct slot *args);

void jthread_destroy(struct jthread *thread);

#endif //JVM_JTHREAD_H
