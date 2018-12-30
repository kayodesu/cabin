/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../rtda/ma/rtcp.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/heap/object.h"

/*
 * invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
 */
void invokevirtual(struct frame *frame)
{
    struct class *curr_class = frame->m.method->clazz;

    int index = bcr_readu2(&frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
//    if (ref->resolved_method == NULL) {
//        ref->resolved_class = classloader_load_class(frame->m.method->class->loader, ref->class_name);
//        ref->resolved_method = jclass_get_declared_nonstatic_method(ref->resolved_class, ref->name, ref->descriptor);
//    }
    resolve_non_static_method_ref(curr_class, ref);

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *frame_stack_pop_slot(frame);
    }

    struct object *obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        thread_throw_null_pointer_exception(frame->thread);
    }

    // 下面这样写对不对 todo
    struct method *method = ref->resolved_method;
    if (obj->clazz != curr_class) {
        // 从对象的类中查找真正要调用的方法
        method = class_lookup_method(obj->clazz, ref->name, ref->descriptor);
    }

//    struct method *method = ref->resolved_method;
//    if (!IS_FINAL(method->access_flags)) {
//        method = class_search_vtable(obj->clazz, method->vtable_index);
//        if (method == NULL) {
////            class_search_vtable(obj->clazz, ref->resolved_method->vtable_index);
////            printvm("method->vtable_index   %d\n", ref->resolved_method->vtable_index);
////            printvm("%d, %d, %d, %d\n", IS_PRIVATE(ref->resolved_method->access_flags), IS_STATIC(ref->resolved_method->access_flags), IS_FINAL(ref->resolved_method->access_flags), IS_ABSTRACT(ref->resolved_method->access_flags));
////            printvm("---   %s\n, ", obj->clazz->class_name); // todo
////            jvm_abort("++++ %s~%s~%s\n, ", ref->class_name, ref->name, ref->descriptor); // todo
//            jvm_abort("");
//        }
//    }

    thread_invoke_method(frame->thread, method, args);
}
