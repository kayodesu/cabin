/*
 * Author: kayo
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include "../objects/slot.h"
#include "../objects/Method.h"
#include "../util/BytecodeReader.h"

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

    slot_t *lvars;    // local variables
    slot_t *ostack;   // operand stack

    Frame(Method *m, bool vm_invoke, slot_t *lvars, slot_t *ostack, Frame *prev)
            : method(m), reader(m->code, m->codeLen),
              vm_invoke(vm_invoke), prev(prev), lvars(lvars), ostack(ostack)
    {
        assert(m != nullptr);
        assert(lvars != nullptr);
        assert(ostack != nullptr);
    }

    jint getLocalAsInt(int index)
    {
        return * (jint *) (lvars + index);
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
        return * (jfloat *) (lvars + index);
    }

    jlong getLocalAsLong(int index)
    {
        return * (jlong *) (lvars + index);
    }

    jdouble getLocalAsDouble(int index)
    {
        return * (jdouble *) (lvars + index);
    }

    template <typename T = Object>
    T *getLocalAsRef(int index)
    {
        return (* (T **) (lvars + index));
    }

    // push to ostack.
    void pushi(jint v)    { *(jint *) ostack = v; ostack++; }
    void pushf(jfloat v)  { *(jfloat *) ostack = v; ostack++; }
    void pushl(jlong v)   { *(jlong *) ostack = v; ostack += 2; }
    void pushd(jdouble v) { *(jdouble *) ostack = v; ostack += 2; }
    void pushr(jref v)    { *(jref *) ostack = v; ostack++; }

    // pop from ostack.
    jint    popi() { ostack--;    return * (jint *) ostack; }
    jfloat  popf() { ostack--;    return * (jfloat *) ostack; }
    jlong   popl() { ostack -= 2; return * (jlong *) ostack; }
    jdouble popd() { ostack -= 2; return * (jdouble *) ostack; }
    jref    popr() { ostack--;    return * (jref *) ostack; }

    // the end address of this frame
    intptr_t end()
    {
        return (intptr_t)(ostack + method->maxStack);
    }

    void clearStack()
    {
        ostack = (slot_t *)(this + 1);
    }

//    static size_t size(const Method *m);
//    size_t size() const;

    std::string toString() const;
};

#endif //JVM_STACK_FRAME_H
