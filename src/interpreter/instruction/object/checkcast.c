/*
 * Author: Jia Yang
 */


#include "../../../rtda/thread/thread.h"
#include "../../../rtda/heap/object.h"

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

    const char *class_name = rtcp_get_class_name(frame->m.method->clazz->rtcp, index);

    struct class *jclass = classloader_load_class(frame->m.method->clazz->loader, class_name); // todo resolve_class ???
    if (!object_is_instance_of(obj, jclass)) {
        thread_throw_class_cast_exception(frame->thread, obj->clazz->class_name, jclass->class_name);
    }
}
