/*
 * Author: Jia Yang
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include <stdbool.h>
#include <assert.h>
#include "../../slot.h"
#include "../ma/method.h"
#include "../../util/bytecode_reader.h"

struct frame {
    struct method *method;
    struct bytecode_reader reader;
    struct frame *prev;

    /*
     * this frame 执行的函数是否由虚拟机调用
     * 由虚拟机调用的函数不会将返回值压入下层frame的栈中，
     * 也不会后续执行其下层frame，而是直接返回。
     */
    bool vm_invoke;

    slot_t *stack;   // operand stack
    slot_t locals[]; // local variables
};

#define FRAME_SIZE(method) (sizeof(struct frame) \
                            + ((method)->max_stack * sizeof(slot_t)) \
                            + ((method)->max_locals * sizeof(slot_t)))

#define frame_readu1(frame)   bcr_readu1(&((frame)->reader))
#define frame_reads1(frame)   bcr_reads1(&((frame)->reader))
#define frame_readu2(frame)   bcr_readu2(&((frame)->reader))
#define frame_reads2(frame)   bcr_reads2(&((frame)->reader))
#define frame_readu4(frame)   bcr_readu4(&((frame)->reader))
#define frame_reads4(frame)   bcr_reads4(&((frame)->reader))
#define frame_has_more(frame) bcr_has_more(&((frame)->reader))
#define frame_skip(frame, offset) bcr_skip(&((frame)->reader), offset)

#define frame_locals_geti(f, index) ISLOT((f)->locals + (index))
#define frame_locals_getz(f, index) (ISLOT((f)->locals + (index)) != 0)
#define frame_locals_getf(f, index) FSLOT((f)->locals + (index))
#define frame_locals_getl(f, index) LSLOT((f)->locals + (index))
#define frame_locals_getd(f, index) DSLOT((f)->locals + (index))
#define frame_locals_getr(f, index) RSLOT((f)->locals + (index))

static inline void frame_stack_pushi(struct frame *f, jint value)    { *(jint *) f->stack = value; f->stack++; }
static inline void frame_stack_pushf(struct frame *f, jfloat value)  { *(jfloat *) f->stack = value; f->stack++; }
static inline void frame_stack_pushl(struct frame *f, jlong value)   { *(jlong *) f->stack = value; f->stack += 2; }
static inline void frame_stack_pushd(struct frame *f, jdouble value) { *(jdouble *) f->stack = value; f->stack += 2; }
static inline void frame_stack_pushr(struct frame *f, jref value)    { *(jref *) f->stack = value; f->stack++; }

static inline jint    frame_stack_popi(struct frame *f) { f->stack--; return *(jint *)f->stack; }
static inline jfloat  frame_stack_popf(struct frame *f) { f->stack--; return *(jfloat *)f->stack; }
static inline jlong   frame_stack_popl(struct frame *f) { f->stack -= 2; return *(jlong *)f->stack; }
static inline jdouble frame_stack_popd(struct frame *f) { f->stack -= 2; return *(jdouble *)f->stack; }
static inline jref    frame_stack_popr(struct frame *f) { f->stack--; return *(jref *)f->stack; }

char* frame_to_string(const struct frame *f);

#endif //JVM_STACK_FRAME_H
