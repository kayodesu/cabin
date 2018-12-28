/*
 * Author: Jia Yang
 */

#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/heap/object.h"

/*
 * putfield指令给实例变量赋值，它需要三个操作数。
 * 前两个操作数是常量池索引和变量值，用法和putstatic一样。
 * 第三个操作数是对象引用，从操作数栈中弹出
 */
void putfield(struct frame *frame)
{
    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(&frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    /*
     * 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
     */
    if (IS_FINAL(ref->resolved_field->access_flags)) {
        // todo
        if (frame->m.method->clazz != ref->resolved_class || strcmp(frame->m.method->name, "<init>") != 0) {
            jvm_abort("java.lang.IllegalAccessError\n"); // todo
        }
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
#endif

    struct slot *s = frame_stack_pop_slot(frame);
    if (slot_is_ph(s)) {
        s = frame_stack_pop_slot(frame);
    }

    jref obj = frame_stack_popr(frame);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    set_instance_field_value_by_id(obj, ref->resolved_field->id, s);
}
