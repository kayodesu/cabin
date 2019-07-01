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

    static size_t size(const Method *m);
    std::string toString();
};

static inline void frame_stack_pushi(Frame *f, jint value)    { *(jint *) f->stack = value; f->stack++; }
static inline void frame_stack_pushf(Frame *f, jfloat value)  { *(jfloat *) f->stack = value; f->stack++; }
static inline void frame_stack_pushl(Frame *f, jlong value)   { *(jlong *) f->stack = value; f->stack += 2; }
static inline void frame_stack_pushd(Frame *f, jdouble value) { *(jdouble *) f->stack = value; f->stack += 2; }
static inline void frame_stack_pushr(Frame *f, jref value)    { *(jref *) f->stack = value; f->stack++; }

static inline jint    frame_stack_popi(Frame *f) { f->stack--; return *(jint *)f->stack; }
static inline jfloat  frame_stack_popf(Frame *f) { f->stack--; return *(jfloat *)f->stack; }
static inline jlong   frame_stack_popl(Frame *f) { f->stack -= 2; return *(jlong *)f->stack; }
static inline jdouble frame_stack_popd(Frame *f) { f->stack -= 2; return *(jdouble *)f->stack; }
static inline jref    frame_stack_popr(Frame *f) { f->stack--; return *(jref *)f->stack; }

#endif //JVM_STACK_FRAME_H
