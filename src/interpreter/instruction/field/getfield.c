/*
 * Author: Jia Yang
 */

#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/resolve.h"
#include "../../../rtda/thread/thread.h"

/*
 * getfield指令获取对象的实例变量值，然后推入操作数栈，它需
 * 要两个操作数。第一个操作数是uint16索引，用法和前面三个指令
 * 一样。第二个操作数是对象引用，用法和putfield一样。
 */
void getfield(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;

    int index = bcr_readu2(&frame->reader);
    struct field *f = resolve_field(curr_class, index);

    jref obj = frame_stack_popr(frame);
    if (obj == NULL) {
        thread_throw_null_pointer_exception();
    }

    const slot_t *value = get_instance_field_value_by_id(obj, f->id);
    *frame->stack++ = value[0];
    if (f->category_two) {
        *frame->stack++ = value[1];
    }
}
