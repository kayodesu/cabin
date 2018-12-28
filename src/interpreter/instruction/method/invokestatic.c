/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../rtda/ma/rtcp.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/ma/symref.h"

// invokestatic指令用来调用静态方法。
// 如果类还没有被初始化，会触发类的初始化。
void invokestatic(struct frame *frame)
{
    size_t saved_pc = frame->reader.pc - 1;
    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(&frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    if (ref->resolved_method == NULL) {
        ref->resolved_class = classloader_load_class(frame->m.method->clazz->loader, ref->class_name);
        // 不用按类层次搜索，直接get
        ref->resolved_method = class_get_declared_static_method(ref->resolved_class, ref->name, ref->descriptor);
    }
//    resolve_static_method_ref(curr_class, ref);

    if (IS_ABSTRACT(ref->resolved_method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

//    if (!ref->resolved_class->inited) {
//        jclass_clinit(ref->resolved_class, frame->thread);
//        frame->reader.pc = saved_pc; // recover pc
//        return;
//    }

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *frame_stack_pop_slot(frame);
    }

    jthread_invoke_method(frame->thread, ref->resolved_method, args);

    if (!ref->resolved_class->inited) {
        class_clinit(ref->resolved_class, frame->thread);
    }
}