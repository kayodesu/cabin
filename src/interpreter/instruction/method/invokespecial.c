/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/access.h"
#include "../../../rtda/ma/resolve.h"

/*
 * invokespecial指令用于调用一些需要特殊处理的实例方法，包括构造函数、私有方法和通过super关键字调用的超类方法。
 */
void invokespecial(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;

    int index = bcr_readu2(&frame->reader);
//
//    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
////    if (method->name == "<init>" && method->class != c) {
////        // todo java.lang.NoSuchMethodError
////        jvm_abort("java.lang.NoSuchMethodError\n");
////    }
//
//    struct method *method = ref->resolved_method;

    struct method *m = resolve_method(curr_class, index);
    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (IS_SUPER(m->clazz->access_flags)
        && !IS_PRIVATE(m->access_flags)
        && class_is_subclass_of(curr_class, m->clazz) // todo
        && strcmp(m->name, "<init>") != 0) {
        m = class_lookup_method(curr_class->super_class, m->name, m->descriptor);
    }

    if (IS_ABSTRACT(m->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }
    if (IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError\n");
    }

    frame->stack_top -= m->arg_slot_count;
    struct slot *args = frame->stack + frame->stack_top + 1;
    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        thread_throw_null_pointer_exception(frame->thread);
    }

    thread_invoke_method(frame->thread, m, args);
}