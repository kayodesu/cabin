/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/resolve.h"

/*
 * invokeinterface指令用于调用接口方法，它会在运行时搜索一个实现了这个接口方法的对象，找出适合的方法进行调用。
 */
void invokeinterface(struct frame *frame)
{
    struct class *curr_class = frame->m.method->clazz;
    int index = frame_readu2(frame); //bcr_readu2(&frame->reader);

    /*
     * 此字节的值是给方法传递参数需要的slot数，
     * 其含义和给method结构体定义的arg_slot_count字段相同。
     * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
     */
    u1 arg_slot_count = frame_readu1(frame);
    /*
     * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
     * 该字节的存在是为了保证Java虚拟机可以向后兼容。
     */
    frame_readu1(frame);

//    struct method_ref *ref = rtcp_get_interface_method_ref(curr_class->rtcp, index);
//    resolve_non_static_method_ref(curr_class, ref);
    struct method *m = resolve_method(curr_class, index);

    /* todo 本地方法 */

    struct slot args[arg_slot_count];
    for (int i = arg_slot_count - 1; i >= 0; i--) {
        args[i] = *frame_stack_pop_slot(frame);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        thread_throw_null_pointer_exception(frame->thread);
    }

    struct method *method = class_lookup_method(obj->clazz, m->name, m->descriptor);
    if (method == NULL) {
        jvm_abort("error\n"); // todo
    }

    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }
    if (!IS_PUBLIC(method->access_flags)) {
        // todo java.lang.IllegalAccessError
        jvm_abort("java.lang.IllegalAccessError\n");
    }

    thread_invoke_method(frame->thread, method, args);
}

