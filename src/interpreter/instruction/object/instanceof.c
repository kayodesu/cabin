/*
 * Author: Jia Yang
 */
#include "../../../rtda/heap/object.h"

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
void instanceof(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    const char *class_name = rtcp_get_class_name(frame->m.method->clazz->rtcp, index);

    struct class *jclass = classloader_load_class(frame->m.method->clazz->loader, class_name); // todo resolve_class ???

    jref obj = frame_stack_popr(frame);  //os_popr(frame->operand_stack);
    if (obj == NULL) {
        frame_stack_pushi(frame, 0); //os_pushi(frame->operand_stack, 0);
        return;
    }

    frame_stack_pushi(frame, object_is_instance_of(obj, jclass) ? 1 : 0);
//    os_pushi(frame->operand_stack, (jint)(jobject_is_instance_of(obj, class) ? 1 : 0));
}
