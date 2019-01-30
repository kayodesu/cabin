#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/ma/resolve.h"

/*
 * Author: Jia Yang
 */

void getstatic(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    int index = bcr_readu2(reader);
    struct field *f = resolve_field(frame->method->clazz, index);

    if (!f->clazz->inited) {
        class_clinit(f->clazz);
    }

    const slot_t *value = get_static_field_value(f->clazz, f);
    *frame->stack++ = value[0];
    if (f->category_two) {
        *frame->stack++ = value[1];
    }
}
