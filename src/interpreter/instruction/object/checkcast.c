/*
 * Author: Jia Yang
 */


#include "../../../rtda/thread/thread.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/resolve.h"

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
void checkcast(struct frame *frame)
{
    jref obj = RSLOT(frame->stack - 1); // 不改变操作数栈
    int index = bcr_readu2(&frame->reader);
    if (obj == NULL) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

//    resolve_single_constant(frame->m.method->clazz, index);

    struct class *c = resolve_class(frame->method->clazz, index);
    if (!object_is_instance_of(obj, c)) {
        thread_throw_class_cast_exception(obj->clazz->class_name, c->class_name);
    }
}
