/*
 * Author: Jia Yang
 */

#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/field.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/resolve.h"
#include "../../../rtda/thread/thread.h"

/*
 * putfield指令给实例变量赋值，它需要三个操作数。
 * 前两个操作数是常量池索引和变量值，用法和putstatic一样。
 * 第三个操作数是对象引用，从操作数栈中弹出
 */
void putfield(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    struct field *f = resolve_field(frame->method->clazz, index);

    // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
    if (IS_FINAL(f->access_flags)) {
        // todo
        if (frame->method->clazz != f->clazz || strcmp(frame->method->name, "<init>") != 0) {
            jvm_abort("java.lang.IllegalAccessError\n"); // todo
        }
    }

    if (f->category_two) {
        frame->stack -= 2;
    } else {
        frame->stack--;
    }
    slot_t *value = frame->stack;

    jref obj = frame_stack_popr(frame);
    if (obj == NULL) {
        thread_throw_null_pointer_exception();
    }

    set_instance_field_value(obj, f, value);
}
