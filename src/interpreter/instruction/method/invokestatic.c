/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/ma/resolve.h"

// invokestatic指令用来调用静态方法。
// 如果类还没有被初始化，会触发类的初始化。
void invokestatic(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;

    int index = bcr_readu2(&frame->reader);

    struct method *m = resolve_method(curr_class, index);

    if (IS_ABSTRACT(m->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }
    if (!IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError\n");
    }

    frame->stack_top -= m->arg_slot_count;
    struct slot *args = frame->stack + frame->stack_top + 1;

    thread_invoke_method(frame->thread, m, args);

    if (!m->clazz->inited) {
        class_clinit(m->clazz, frame->thread);
    }
}