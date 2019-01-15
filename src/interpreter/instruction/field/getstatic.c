#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/ma/resolve.h"

/*
 * Author: Jia Yang
 */

void getstatic(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(reader);
//    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
//    resolve_static_field_ref(curr_class, ref);
    struct field *f = resolve_field(curr_class, index);

    if (!f->clazz->inited) {
        class_clinit(f->clazz, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

    const struct slot *s = get_static_field_value_by_id(f->clazz, f->id);
    frame_stack_push_slot(frame, s);
}

