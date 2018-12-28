/*
 * Author: Jia Yang
 */

#include "../../../rtda/thread/frame.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/ma/field.h"

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
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    struct class *cls = ref->resolved_field->clazz;

    if (!cls->inited) {
        class_clinit(cls, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

#if 0
    // todo
    char d = *ref->resolved_field->descriptor;
    struct slot s;
    if (d == 'B' || d == 'C' || d == 'I' || d == 'S' || d == 'Z') {
        s = islot(os_popi(frame->operand_stack));
    } else if (d == 'F') {
        s = fslot(os_popf(frame->operand_stack));
    } else if (d == 'J') {
        s = lslot(os_popl(frame->operand_stack));
    } else if (d == 'D') {
        s = dslot(os_popd(frame->operand_stack));
    } else if (d  == 'L' || d  == '[') {
        s = rslot(os_popr(frame->operand_stack));
    } else {
        VM_UNKNOWN_ERROR("field's descriptor error. %s", ref->resolved_field->descriptor);
    }
    set_static_field_value_by_id(cls, ref->resolved_field->id, &s);
#endif
    struct slot *s = frame_stack_pop_slot(frame);
    if (slot_is_ph(s)) {
        s = frame_stack_pop_slot(frame);
    }
    set_static_field_value_by_id(cls, ref->resolved_field->id, s);
}
