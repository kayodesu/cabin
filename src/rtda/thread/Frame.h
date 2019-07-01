/*
 * Author: Jia Yang
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include "../../slot.h"
#include "../ma/Method.h"
#include "../../util/BytecodeReader.h"

struct Frame {
    Method *method;
    BytecodeReader reader;

    /*
     * this frame 执行的函数是否由虚拟机调用
     * 由虚拟机调用的函数不会将返回值压入下层frame的栈中，
     * 也不会后续执行其下层frame，而是直接返回。
     */
    bool vm_invoke;

    Frame *prev;

    slot_t *stack;   // operand stack
    slot_t *locals;  // local variables

    Frame(Method *m, bool vm_invoke, Frame *prev);

    jint getLocalAsInt(int index)
    {
        return * (jint *) (locals + index);
    }

    jshort getLocalAsShort(int index)
    {
        return (jshort) getLocalAsInt(index);
    }

    jbool getLocalAsBool(int index)
    {
        return getLocalAsInt(index) != 0;
    }

    jfloat getLocalAsFloat(int index)
    {
        return * (jfloat *) (locals + index);
    }

    jlong getLocalAsLong(int index)
    {
        return * (jlong *) (locals + index);
    }

    jdouble getLocalAsDouble(int index)
    {
        return * (jdouble *) (locals + index);
    }

    template <typename T = Object>
    T *getLocalAsRef(int index)
    {
        return (* (T **) (locals + index));
    }

    // push to stack.
    void pushi(jint v)    { *(jint *) stack = v; stack++; }
    void pushf(jfloat v)  { *(jfloat *) stack = v; stack++; }
    void pushl(jlong v)   { *(jlong *) stack = v; stack += 2; }
    void pushd(jdouble v) { *(jdouble *) stack = v; stack += 2; }
    void pushr(jref v)    { *(jref *) stack = v; stack++; }

    // pop from stack.
    jint    popi() { stack--;    return * (jint *) stack; }
    jfloat  popf() { stack--;    return * (jfloat *) stack; }
    jlong   popl() { stack -= 2; return * (jlong *) stack; }
    jdouble popd() { stack -= 2; return * (jdouble *) stack; }
    jref    popr() { stack--;    return * (jref *) stack; }

    static size_t size(const Method *m);
    std::string toString();
};

#endif //JVM_STACK_FRAME_H
