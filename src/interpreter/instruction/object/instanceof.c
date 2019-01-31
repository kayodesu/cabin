/*
 * Author: Jia Yang
 */
#include "../../../rtda/heap/object.h"
#include "../../../classfile/constant.h"
#include "../../../rtda/ma/resolve.h"

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
void instanceof(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    struct class *c = resolve_class(frame->method->clazz, index);

    jref obj = frame_stack_popr(frame);
    if (obj == NULL) {
        frame_stack_pushi(frame, 0);
        return;
    }

    frame_stack_pushi(frame, object_is_instance_of(obj, c) ? 1 : 0);
}
