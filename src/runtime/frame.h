/*
 * Author: kayo
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include "../objects/class.h"
#include "../objects/method.h"
#include "../util/bytecode_reader.h"

class Method;

class Frame {
public:
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
        return ISLOT(lvars + index);
    }

    jshort getLocalAsShort(int index)
    {
        return jint2jshort(getLocalAsInt(index));
    }

    jbool getLocalAsBool(int index)
    {
        return jint2jbool(getLocalAsInt(index));
    }

    jfloat getLocalAsFloat(int index)
    {
        return FSLOT(lvars + index);
    }

    jlong getLocalAsLong(int index)
    {
        return LSLOT(lvars + index);
    }

    jdouble getLocalAsDouble(int index)
    {
        return DSLOT(lvars + index);
    }

    template <typename T = Object>
    T *getLocalAsRef(int index)
    {
        return (T *) RSLOT(lvars + index);
    }

    // push to ostack.
    void push(slot_t v)   { *ostack++ = v; }
    void pushi(jint v)    { ISLOT(ostack) = v; ostack++; }
    void pushf(jfloat v)  { FSLOT(ostack) = v; ostack++; }
    void pushl(jlong v)   { LSLOT(ostack) = v; ostack += 2; }
    void pushd(jdouble v) { DSLOT(ostack) = v; ostack += 2; }
    void pushr(jref v)    { RSLOT(ostack) = v; ostack++; }

    // pop from ostack.
    jint    popi() { ostack--;    return ISLOT(ostack); }
    jfloat  popf() { ostack--;    return FSLOT(ostack); }
    jlong   popl() { ostack -= 2; return LSLOT(ostack); }
    jdouble popd() { ostack -= 2; return DSLOT(ostack); }
    jref    popr() { ostack--;    return RSLOT(ostack); }

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

    virtual std::string toString() const;
};

#endif //JVM_STACK_FRAME_H
