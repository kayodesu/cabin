#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include "../metadata/class.h"
#include "../metadata/method.h"
#include "../classfile/bytecode_reader.h"

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

    slot_t *lvars;   // local variables
    slot_t *ostack;  // operand stack

    Frame(Method *m, bool vm_invoke, slot_t *_lvars, slot_t *_ostack, Frame *prev)
            : method(m), reader(m->code, m->code_len), vm_invoke(vm_invoke),
              prev(prev), lvars(_lvars), ostack(_ostack)
    {
        assert(m != nullptr);
        assert(_lvars != nullptr);
        assert(_ostack != nullptr);
    }

    // push to ostack.
    void push(slot_t v)   { *ostack++ = v; }
    void pushi(jint v)    { slot::setInt(ostack, v); ostack++; }
    void pushf(jfloat v)  { slot::setFloat(ostack, v); ostack++; }
    void pushl(jlong v)   { slot::setLong(ostack, v); ostack += 2; }
    void pushd(jdouble v) { slot::setDouble(ostack, v); ostack += 2; }
    void pushr(jref v)    { slot::setRef(ostack, v); ostack++; }

    // pop from ostack.
    jint    popi() { ostack--;    return slot::getInt(ostack); }
    jfloat  popf() { ostack--;    return slot::getFloat(ostack); }
    jlong   popl() { ostack -= 2; return slot::getLong(ostack); }
    jdouble popd() { ostack -= 2; return slot::getDouble(ostack); }
    jref    popr() { ostack--;    return slot::getRef(ostack); }

    // the end address of this frame
    [[nodiscard]] intptr_t end() const
    {
        return (intptr_t)(ostack + method->max_stack);
    }

    void clearStack()
    {
        ostack = (slot_t *)(this + 1);
    }

    [[nodiscard]] virtual std::string toString() const;
};

#endif //JVM_STACK_FRAME_H
