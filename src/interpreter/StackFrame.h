/*
 * Author: Jia Yang
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include <string>
#include <stack>
#include <cassert>
#include "../rtda/heap/methodarea/Jclass.h"
#include "Slot.h"
#include "../rtda/heap/objectarea/Jobject.h"

class Jobject;

class OperandStack {
private:
    std::stack<Slot> s;

public:
//#ifdef JVM_DEBUG
    auto size() {
        return s.size();
    }
//#endif
    void clear() {
        while(!s.empty())
            s.pop();
    }

    void push(jint v) {
        s.push(Slot::intSlot(v));
    }

    void push(jfloat v) {
        s.push(Slot::floatSlot(v));
    }

    void push(jreference v) {
        s.push(Slot::referenceSlot(v));
    }

    void push(jlong v) {
        s.push(Slot::longSlot(v));
        s.push(Slot::invalidSlot());  // todo 这样对吗
    }

    void push(jdouble v) {
        s.push(Slot::doubleSlot(v));
        s.push(Slot::invalidSlot());  // todo 这样对吗
    }

    void push(const Slot &v) {
        s.push(v);
        if (v.isClassificationTwo()) {
            s.push(Slot::invalidSlot());
        }
    }

    void directPush(const Slot &v) {
        s.push(v);
    }


//    void push(const Slot *v[], int count) {
//        for (int i = 0; i < count; i++) {
//            push(v[i]);
//        }
//    }

    bool empty() {
        return s.empty();
    }

    jint popInt() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        jint v = s.top().getInt();
        s.pop();
        return v;
    }

    jfloat popFloat() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        jfloat v = s.top().getFloat();
        s.pop();
        return v;
    }

    jlong popLong() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        // long 使用两个Slot表示，第一个是Long的Slot，第二个是无效Slot
        if (s.top().type == INVALID_JVM_TYPE) {
            s.pop();   // todo
        }

        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        jlong v = s.top().getLong();
        s.pop();
        return v;
    }

    jdouble popDouble() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        // double 使用两个Slot表示，第一个是Long的Slot，第二个是无效Slot
        if (s.top().type == INVALID_JVM_TYPE) {
            s.pop();        // todo
        }

        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        jdouble v = s.top().getDouble();
        s.pop();
        return v;
    }

    jreference popReference() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }

        jreference v = s.top().getReference();
        s.pop();
        return v;
    }

    Slot popSlot() {
        if (s.empty()) {
            jvmAbort("error. operand stack is empty.\n");
        }

        Slot slot = s.top();
        s.pop();
        return slot;
    }

    Slot popSlotJumpInvalid() {
        if (s.empty()) {
            jvmAbort("error. operand stack is empty.\n");
        }

        Slot slot = popSlot();
        if (slot.type == INVALID_JVM_TYPE) {
            return popSlotJumpInvalid();
        }
        return slot;
    }

    Slot &peek() {
        if (s.empty()) {
            jvmAbort("operand stack is empty.\n");
        }
        return s.top();
    }

//    Slot &peekFromTop(int index) {
//        if (s.size() <= index) {
//            jvmAbort("error.\n");
//        }
//        return s.
//    }
};

struct Jthread;

class StackFrame {
    Slot *localVars; // 局部变量表
    int maxLocals;  // todo

    bool interruptedStatus;
    bool exeStatus; // todo 是否执行完毕
    bool procExceptionStatus; // 处理异常
public:
    OperandStack operandStack; // 操作栈

    BytecodeReader *reader;

    Jmethod *method;
    Jthread *thread;

    StackFrame(Jthread *thread, Jmethod *method);

    void procException() {
        procExceptionStatus = true;
    }

    bool isProcException() {
        return procExceptionStatus;
    }

    void exeOver() {
        exeStatus = true;
    }

    bool isExeOver() {
        return exeStatus;
    }


//static void print_slots(const slot *slots, int count)
//{
//    printf("---------------------------------------------\n");
//    for (int i = 0; i < count; i++) {
//        const slot *s = slots + i;
//        if (s->type == PRIMITIVE_INT) {
//            printf("PRIMITIVE_INT: %d\n", s->value.i);
//        } else if (s->type == PRIMITIVE_FLOAT) {
//            printf("PRIMITIVE_FLOAT: %f\n", s->value.f);
//        } else if (s->type == PRIMITIVE_LONG) {
//            printf("PRIMITIVE_LONG: %ld\n", s->value.l);
//        } else if (s->type == PRIMITIVE_DOUBLE) {
//            printf("PRIMITIVE_DOUBLE: %f\n", s->value.d);
//        } else if (s->type == REFERENCE) {
//            printf("REFERENCE: %p\n", s->value.p);
//        } else {
//            printf("print_slots: %d\n", s->type);
//        }
//    }
//    printf("---------------------------------------------\n");
//}


    /*todo const */Slot& getLocalVars(int index) {
        assert(index >= 0 and index < maxLocals);
        return localVars[index];
    }

    void setLocalVars(int index, const Slot &s) {
        assert(index >= 0 and index < maxLocals);
        localVars[index] = s;
        if (s.isClassificationTwo()) {
            assert(index + 1 < maxLocals);
            localVars[index + 1] = Slot::invalidSlot();
        }
    }

    void invokeMethod(Jmethod *method, Slot *args = nullptr);

    /*
     * Interrupts the function of this stack_frame.
     */
    void interrupt() {
        interruptedStatus = true;
    }

    /*
     * Tests whether the current stack_frame has been interrupted.
     * The interrupted status of the stack_frame is cleared by this method.
     *
     * return: true if the current stack_frame has been interrupted;
     *         false otherwise.
     */
    bool interrupted() {
        bool tmp = interruptedStatus;
        interruptedStatus = false;
        return tmp;
    }

    std::string toString();
};

#endif //JVM_STACK_FRAME_H
