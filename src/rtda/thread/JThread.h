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

void jthread_invoke_method(struct jthread *thread, struct jmethod *method, const struct slot *args);

void jthread_destroy(struct jthread *thread);

#endif //JVM_JTHREAD_H
