/*
 * Author: Jia Yang
 */

#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/heap/object.h"

/*
 * getfield指令获取对象的实例变量值，然后推入操作数栈，它需
 * 要两个操作数。第一个操作数是uint16索引，用法和前面三个指令
 * 一样。第二个操作数是对象引用，用法和putfield一样。
 */
void getfield(struct frame *frame)
{
    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(&frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    jref obj = frame_stack_popr(frame);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    const struct slot *s = get_instance_field_value_by_id(obj, ref->resolved_field->id);
    frame_stack_push_slot(frame, s);


#if 0
    // 检查 slot 的类型与 field 的类型是否匹配  todo
    char d = *ref->resolved_field->descriptor;
    if (((d == 'B' || d == 'C' || d == 'I' || d == 'S' || d == 'Z') && s->t == JINT)
        || (d == 'F' && s->t == JFLOAT)
        || (d == 'J' && s->t == JLONG)
        || (d == 'D' && s->t == JDOUBLE)
        || ((d  == 'L' || d  == '[') && s->t == JREF)) {
        os_pushs(frame->operand_stack, s);
    } else {
        VM_UNKNOWN_ERROR("type mismatch error. field's descriptor is %s, but slot is %s",
                  ref->resolved_field->descriptor, slot_to_string(s));
    }
#endif
}