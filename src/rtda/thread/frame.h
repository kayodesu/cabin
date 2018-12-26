/*
 * Author: Jia Yang
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include <stdbool.h>
#include <assert.h>
#include "../../slot.h"
#include "../ma/jmethod.h"
#include "jthread.h"
#include "../../util/bytecode_reader.h"

enum frame_type {
    SF_TYPE_NORMAL,
    SF_TYPE_SHIM
};

struct frame {
    enum frame_type type;
    union {
        struct jmethod *method;

        // just for shim frame
        void (* shim_action)(struct frame *);
    } m;

    struct jthread *thread;

    bool interrupted_status;
    bool exe_status; // todo 是否执行完毕
    bool proc_exception_status; // 处理异常

    struct bytecode_reader reader;

    u4 max_locals_and_stack; // max_stack(u2) + max_locals(u2)

    struct slot *stack; // operand stack
    u2 stack_top;

    struct slot locals[]; // local variables
};

#define frame_has_more(frame) bcr_has_more(&((frame)->reader))
#define frame_skip(frame)     bcr_skip(&((frame)->reader))
#define frame_readu1(frame)   bcr_readu1(&((frame)->reader))
#define frame_reads1(frame)   bcr_reads1(&((frame)->reader))
#define frame_readu2(frame)   bcr_readu2(&((frame)->reader))
#define frame_reads2(frame)   bcr_reads2(&((frame)->reader))
#define frame_readu4(frame)   bcr_readu4(&((frame)->reader))
#define frame_reads4(frame)   bcr_reads4(&((frame)->reader))

struct frame* frame_create_normal(struct jthread *thread, struct jmethod *method);

/*
 * create a shim stack frame.
 * @shim_action, could be NULL
 */
struct frame* frame_create_shim(struct jthread *thread, void (* shim_action)(struct frame *));

#define frame_create(thread, method) \
    _Generic((method), \
        struct jmethod *: frame_create_normal, \
        void (*)(struct frame *): frame_create_shim \
    )(thread, method)

void frame_bind(struct frame *frame, struct jthread *thread, struct jmethod *method);

// ----------  locals
static inline void frame_locals_set(struct frame *f, int index, const struct slot *value)
{
    assert(f != NULL);
    assert(value != NULL);

    f->locals[index] = *value;
    if (slot_is_category_two(value)) {
        f->locals[++index] = phslot;
    }
}

#define frame_locals_get(f, index) ((f)->locals + (index))
#define frame_locals_geti(f, index) slot_geti(frame_locals_get(f, index))
#define frame_locals_getz(f, index) (frame_locals_geti(f, index) != 0)
#define frame_locals_getf(f, index) slot_getf(frame_locals_get(f, index))
#define frame_locals_getl(f, index) slot_getl(frame_locals_get(f, index))
#define frame_locals_getd(f, index) slot_getd(frame_locals_get(f, index))
#define frame_locals_getr(f, index) slot_getr(frame_locals_get(f, index))

// ----------- operand stack
static inline void frame_stack_clear(struct frame *f)
{
    assert(f != NULL);
    f->stack_top = 0;
}

static inline struct slot* frame_stack_top(struct frame *f)
{
    assert(f != NULL);
    return f->stack + f->stack_top - 1;
}

static inline struct slot* frame_stack_pop_slot(struct frame *f)
{
    assert(f != NULL);
    assert(f->stack_top > 0);
    return f->stack + --(f->stack_top);
}

static inline jint frame_stack_popi(struct frame *f)
{
    assert(f != NULL);
    struct slot *s = frame_stack_pop_slot(f);
    assert(s != NULL && s->t == JINT);
    return s->v.i;
}

static inline jfloat frame_stack_popf(struct frame *f)
{
    assert(f != NULL);
    struct slot *s = frame_stack_pop_slot(f);
    assert(s != NULL && s->t == JFLOAT);
    return s->v.f;
}

static inline jlong frame_stack_popl(struct frame *f)
{
    assert(f != NULL);

    frame_stack_pop_slot(f); // pop placeholder
    struct slot *s = frame_stack_pop_slot(f);
    assert(s != NULL && s->t == JLONG);
    return s->v.l;
}

static inline jdouble frame_stack_popd(struct frame *f)
{
    assert(f != NULL);

    frame_stack_pop_slot(f); // pop placeholder
    struct slot *s = frame_stack_pop_slot(f);
    assert(s != NULL && s->t == JDOUBLE);
    return s->v.d;
}

static inline jref frame_stack_popr(struct frame *f)
{
    assert(f != NULL);
    struct slot *s = frame_stack_pop_slot(f);
    assert(s != NULL && s->t == JREF);
    return s->v.r;
}

static inline void frame_stack_pushi(struct frame *f, jint i)
{
    assert(f != NULL);
    f->stack[f->stack_top++] = islot(i);
}
static inline void frame_stack_pushf(struct frame *frame, jfloat f)
{
    assert(frame != NULL);
    frame->stack[frame->stack_top++] = fslot(f);
}

static inline void frame_stack_pushl(struct frame *f, jlong l)
{
    assert(f != NULL);
    f->stack[f->stack_top++] = lslot(l);
    f->stack[f->stack_top++] = phslot;
}

static inline void frame_stack_pushd(struct frame *f, jdouble d)
{
    assert(f != NULL);
    f->stack[f->stack_top++] = dslot(d);
    f->stack[f->stack_top++] = phslot;
}

static inline void frame_stack_pushr(struct frame *f, jref r)
{
    assert(f != NULL);
    f->stack[f->stack_top++] = rslot(r);
}

/*
 * 不判断 slot 的类型直接 push
 */
static inline void frame_stack_push_slot_directly(struct frame *f, const struct slot *s)
{
    assert(f != NULL);
    assert(s != NULL);
    f->stack[f->stack_top++] = *s;
}

static inline void frame_stack_push_slot(struct frame *f, const struct slot *s)
{
    frame_stack_push_slot_directly(f, s);
    if (slot_is_category_two(s)) {
        struct slot phs = phslot;
        frame_stack_push_slot(f, &phs);
    }
}

// 将一个值推入操作栈中
// os: 要推入值的 operand_stack 的地址
// v: 值
#define frame_stack_push(f, v) \
    _Generic((v), \
        jbyte:   frame_stack_pushi, \
        jchar:   frame_stack_pushi, \
        jshort:  frame_stack_pushi, \
        jint:    frame_stack_pushi, \
        jfloat:  frame_stack_pushf, \
        jlong:   frame_stack_pushl, \
        jdouble: frame_stack_pushd, \
        jref:    frame_stack_pushr, \
        const struct slot *: frame_stack_push_slot \
    )(f, v)


static inline void frame_proc_exception(struct frame *f)
{
    assert(f != NULL);
    f->proc_exception_status = true;
}

/*
 * todo 正在处理异常？ 用 sf_is_procing_exception 会不会好一点
 */
static inline bool frame_is_proc_exception(struct frame *f)
{
    assert(f != NULL);
    return f->proc_exception_status;
}

static inline void frame_exe_over(struct frame *f)
{
    assert(f != NULL);
    f->exe_status = true;
}

static inline bool frame_is_exe_over(struct frame *f)
{
    assert(f != NULL);
    return f->exe_status;
}

/*
 * Interrupts the function of this StackFrame.
 */
static inline void frame_interrupt(struct frame *f)
{
    assert(f != NULL);
    f->interrupted_status = true;
}

/*
 * Tests whether the current StackFrame has been interrupted.
 * The interrupted status of the StackFrame is cleared by this method.
 *
 * return: true if the current StackFrame has been interrupted;
 *         false otherwise.
 */
static inline bool frame_interrupted(struct frame *f)
{
    assert(f != NULL);

    bool tmp = f->interrupted_status;
    f->interrupted_status = false;
    return tmp;
}

char* frame_to_string(const struct frame *f);

void frame_destroy(struct frame *f);

#endif //JVM_STACK_FRAME_H
