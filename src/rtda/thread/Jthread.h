/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include "../../interpreter/StackFrame.h"
#include "../heap/methodarea/Jclass.h"
#include "../../jvm.h"


/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */
class Jthread {
    std::stack<StackFrame *> vmStack; // 虚拟机栈
    Jobject *jlThreadObj; // object of java.lang.Thread
public:
    size_t pc;

    Jthread();
    void joinToMainThreadGroup();

    jreference getJavaLangThreadObj() {
        return jlThreadObj;
    }

    bool isStackEmpty() {
        return vmStack.empty();
    }

    void pushFrame(StackFrame *frame) {
        vmStack.push(frame);
    }

    StackFrame *topFrame() {
        return vmStack.top();
    }

    void popFrame() {
        vmStack.pop();
    }

//// 打印虚拟机栈
//#define PRINT_VM_STACK(thread) \
//    do {\
//        for (size_t i = 0; i < vector_size((thread)->vmStack); i++) {\
//            StackFrame *frame0 = vector_get((thread)->vmStack, StackFrame *, i);\
//            jvm_printf("%s: %s\n", frame0->method->m.class->class_name, frame0->method->m.name);\
//        }\
//    } while(0)
//static void print_vm(const )
//{
//    for (size_t i = 0; i < vector_size(thread->vmStack); i++) {
//        StackFrame *frame = vector_get(thread->vmStack, StackFrame *, i);
//        jvm_printf("%s: %s\n", frame->method->m.class->class_name, frame->method->m.name);
//    }
//}

//void jthread_destroy() {
//    if (thread == NULL)
//        return;
//
//    for (size_t i = 0; i < vector_size(thread->vmStack); i++) {
//        stack_frame_destroy(vector_get(thread->vmStack, StackFrame * , i));
//    }
//
//    vector_destroy(thread->vmStack);
//    free(thread);
//}

};

#endif //JVM_JTHREAD_H
