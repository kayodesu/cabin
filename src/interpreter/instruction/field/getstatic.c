#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/ma/field.h"

/*
 * Author: Jia Yang
 */

void getstatic(struct frame *frame)
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
    frame_stack_push_slot(frame, get_static_field_value_by_id(cls, ref->resolved_field->id));
}

