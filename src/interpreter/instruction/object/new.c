/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/resolve.h"

/*
 * new指令专门用来创建类实例。数组由专门的指令创建
 * 如果类还没有被初始化，会触发类的初始化。
 */
void new(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

//    const char *class_name = CP_CLASS_NAME(&frame->m.method->clazz->constant_pool, bcr_readu2(reader));//rtcp_get_class_name(frame->m.method->clazz->rtcp, bcr_readu2(reader)); // todo reads2  ?????
//    assert(class_name != NULL);
//    assert(strlen(class_name) > 0);

    struct class *c = resolve_class(frame->method->clazz, bcr_readu2(reader));  // todo
    if (!c->inited) {
        class_clinit(c, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

    if (IS_INTERFACE(c->access_flags) || IS_ABSTRACT(c->access_flags)) {
        jvm_abort("java.lang.InstantiationError\n");  // todo 抛出 InstantiationError 异常
    }

    // todo java/lang/Class 会在这里创建，为什么会这样，怎么处理
//    assert(strcmp(c->class_name, "java/lang/Class") == 0);

    frame_stack_pushr(frame, object_create(c));
//    os_pushr(frame->operand_stack, jobject_create(c));
}