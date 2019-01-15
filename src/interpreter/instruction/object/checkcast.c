/*
 * Author: Jia Yang
 */


#include "../../../rtda/thread/thread.h"
#include "../../../rtda/heap/object.h"
#include "../../../classfile/constant.h"
#include "../../../rtda/ma/resolve.h"

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
void checkcast(struct frame *frame)
{
    jref obj = slot_getr(frame_stack_top(frame));
    int index = bcr_readu2(&frame->reader);
    if (obj == NULL) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

    resolve_single_constant(frame->m.method->clazz, index);
    struct class *c = (struct class *) CP_INFO(&frame->m.method->clazz->constant_pool, index);
    if (!object_is_instance_of(obj, c)) {
        thread_throw_class_cast_exception(frame->thread, obj->clazz->class_name, c->class_name);
    }
}
