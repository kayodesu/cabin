/*
 * Author: Jia Yang
 */

#include "../../../rtda/thread/frame.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/ma/resolve.h"

/*
 * putstatic指令给类的某个静态变量赋值，它需要两个操作数。
 * 第一个操作数是uint16索引，来自字节码。通过这个索引可以从当
 *    前类的运行时常量池中找到一个字段符号引用，解析这个符号引用
 *    就可以知道要给类的哪个静态变量赋值。
 * 第二个操作数是要赋给静态变量的值，从操作数栈中弹出。
 */
void putstatic(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(reader);
//    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
//    resolve_static_field_ref(curr_class, ref);
//
//    struct class *cls = ref->resolved_field->clazz;
    struct field *f = resolve_field(curr_class, index);

    if (!f->clazz->inited) {
        class_clinit(f->clazz, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

    struct slot *s = frame_stack_pop_slot(frame);
    if (slot_is_ph(s)) {
        s = frame_stack_pop_slot(frame);
    }

    set_static_field_value_by_id(f->clazz, f->id, s);
}
