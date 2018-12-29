/*
 * Author: Jia Yang
 */

#include "../../../util/bytecode_reader.h"
#include "../../../rtda/ma/rtcp.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/ma/class.h"
#include "../../../rtda/ma/symref.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/ma/descriptor.h"
#include "../../../rtda/heap/arrobj.h"
#include "../../../classfile/constant.h"
#include "../../../rtda/heap/strobj.h"
#include "../../../rtda/heap/clsobj.h"


/*
 *
    struct {
        // bootstrap_method_attr_index项的值必须是对当前Class文件中引导方法表的bootstrap_methods[]数组的有效索引。
        u2 bootstrap_method_attr_index;
        u2 name_and_type_index;
    } invoke_dynamic_constant;

    struct {
        u2 num;
        struct bootstrap_method *methods;
    } bootstrap_methods;

    struct bootstrap_method {
        /*
        * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
        * 常量池在该索引处的值必须是一个CONSTANT_MethodHandle_info结构。
        * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
        * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
        *
        u2 bootstrap_method_ref;
        u2 num_bootstrap_arguments;
        /*
        * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
        * 常量池在该索引出必须是下列结构之一：
        * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
        * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
        *
        u2 *bootstrap_arguments;
    };

    struct {
        // reference_kind项的值必须在1至9之间（包括1和9），它决定了方法句柄的类型。
        // 方法句柄类型的值表示方法句柄的字节码行为。
        u1 reference_kind;

        * 2. If the value of the reference_kind item is 5 (REF_invokeVirtual) or 8
        *    (REF_newInvokeSpecial), then the constant_pool entry at that index must
        *    be a CONSTANT_Methodref_info structure representing a class's
        *    method or constructor for which a method handle is to be created.
        * 3. If the value of the reference_kind item is 6 (REF_invokeStatic)
        *    or 7 (REF_invokeSpecial), then if the class file version number
        *    is less than 52.0, the constant_pool entry at that index must be
        *    a CONSTANT_Methodref_info structure representing a class's method
        *    for which a method handle is to be created; if the class file
        *    version number is 52.0 or above, the constant_pool entry at that
        *    index must be either a CONSTANT_Methodref_info structure or a
        *    CONSTANT_InterfaceMethodref_info structure representing a
        *    class's or interface's method for which a method handle is to be created.
        u2 reference_index;
    } method_handle_constant;

    struct {
        u2 class_index;
        u2 name_and_type_index;
    } ref_constant, field_ref_constant, method_ref_constant, interface_method_ref_constant;
 */

static void set_bootstrap_method_type(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.bootstrap_method_type = frame_stack_popr(frame);
    int i = 3;
}

static void set_lookup(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.lookup = frame_stack_popr(frame);
    int i = 3;
}

static void set_call_set0(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.call_set = frame_stack_popr(frame);
    int i = 3;
}

static void set_exact_method_handle(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.exact_method_handle = frame_stack_popr(frame);
    int i = 3;
}

static void parse_bootstrap_method_type(
        struct class *curr_class, struct thread *curr_thread, struct invoke_dynamic_ref *ref)
{
// todo 注意有一个这个方法： MethodType.fromMethodDescriptorString 更方便
    struct object *parameter_types = method_descriptor_to_parameter_types(curr_class->loader, ref->nt->descriptor);
    struct object *return_type = method_descriptor_to_return_type(curr_class->loader, ref->nt->descriptor);
    struct class *c = classloader_load_class(g_bootstrap_loader, "java/lang/invoke/MethodType");

    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes)
    if (arrobj_len(parameter_types) > 0) {
        struct method *get_method_type = class_lookup_static_method(
                c, "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");

        struct slot args[] = { rslot(return_type), rslot(parameter_types) };
        thread_invoke_method_with_shim(curr_thread, get_method_type, args, set_bootstrap_method_type);
    } else {
        struct method *get_method_type = class_lookup_static_method(
                c, "methodType", "(Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
        struct slot args[] = { rslot(return_type) };
        thread_invoke_method_with_shim(curr_thread, get_method_type, args, set_bootstrap_method_type);
    }

    if (!c->inited) { // 后压栈，先执行
        class_clinit(c, curr_thread);
    }
}

static void get_lookup(struct thread *curr_thread)
{
    struct class *c = classloader_load_class(g_bootstrap_loader, "java/lang/invoke/MethodHandles");
    struct method *m = class_lookup_static_method(c, "lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
    thread_invoke_method_with_shim(curr_thread, m, NULL, set_lookup);

    if (!c->inited) { // 后压栈，先执行
        class_clinit(c, curr_thread);
    }
}

//static struct slot rtc_to_slot(struct classloader *loader, const struct rtcp *rtcp, int index)
//{
//    assert(rtcp != NULL);
//
//    switch (rtcp->pool[index].t) {
//        case STRING_CONSTANT:
//            return rslot(strobj_create(rtcp_get_str(rtcp, index)));
//        case CLASS_CONSTANT:
//            return rslot(clsobj_create(classloader_load_class(loader, rtcp_get_class_name(rtcp, index))));
//        case INTEGER_CONSTANT:
//            return islot(rtcp_get_int(rtcp, index));
//        case LONG_CONSTANT:
//            return lslot(rtcp_get_long(rtcp, index));
//        case FLOAT_CONSTANT:
//            return fslot(rtcp_get_float(rtcp, index));
//        case DOUBLE_CONSTANT:
//            return dslot(rtcp_get_double(rtcp, index));
//        case METHOD_HANDLE_CONSTANT: {
//            struct method_handle *mh = rtcp_get_method_handle(rtcp, index);
//            // todo
//            jvm_abort("");
//            break;
//        }
//        case METHOD_TYPE_CONSTANT: {
//            const char *descriptor = rtcp_get_method_type(rtcp, index);
//            // todo
//            jvm_abort("");
//            break;
//        }
//        default:
//            VM_UNKNOWN_ERROR("unknown type. t = %d, index = %d\n", rtcp->pool[index].t, index);
//            break;
//    }
//}

static void set_call_set(struct class *curr_class, struct thread *curr_thread,
                        struct classloader *loader, struct invoke_dynamic_ref *ref)
{

    struct method_ref *mr = ref->handle->ref.mr;

    struct class *bootstrap_class = classloader_load_class(loader, mr->class_name);
    struct method *bootstrap_method = class_lookup_static_method(bootstrap_class, mr->name, mr->descriptor);
    // todo 说明 bootstrap_method 的格式
    // bootstrap_method is static,  todo 对不对
    // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
    // 后续的参数由 ref->argc and ref->args 决定
    assert(ref->argc + 3 == bootstrap_method->arg_slot_count);
    struct slot args[bootstrap_method->arg_slot_count];
    args[0] = rslot(curr_thread->dyn.lookup);
    args[1] = rslot(strobj_create(mr->name));
    args[2] = rslot(curr_thread->dyn.bootstrap_method_type);
    // parse remaining args
    for (int i = 0, j = 3; i < ref->argc; i++, j++) {
        args[j] = rtc_to_slot(curr_class->loader, curr_class->rtcp, ref->args[i]); // todo 用哪个类的 rtcp
    }

    thread_invoke_method_with_shim(curr_thread, bootstrap_method, args, set_call_set0);
}

// 每一个 invokedynamic 指令都称为 Dynamic Call Site(动态调用点)
void invokedynamic(struct frame *frame)
{
    jvm_abort("invokedynamic not implement\n");  // todo

    size_t saved_pc = frame->reader.pc - 1;

    struct class *curr_class = frame->m.method->clazz;
    struct thread *curr_thread = frame->thread;

    // The run-time constant pool item at that index must be a symbolic reference to a call site specifier.
    int index = bcr_readu2(&frame->reader); // CONSTANT_InvokeDynamic_info

    bcr_readu1(&frame->reader); // this byte must always be zero.
    bcr_readu1(&frame->reader); // this byte must always be zero.

    struct invoke_dynamic_ref *ref = rtcp_get_invoke_dynamic(curr_class->rtcp, index);

    struct object *bootstrap_method_type = curr_thread->dyn.bootstrap_method_type;
    if (bootstrap_method_type == NULL) {
        parse_bootstrap_method_type(curr_class, curr_thread, ref);
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

    struct object *lookup = curr_thread->dyn.lookup;
    if (lookup == NULL) {
        get_lookup(curr_thread);
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

    struct object *call_set = curr_thread->dyn.call_set;
    struct object *exact_method_handle = curr_thread->dyn.exact_method_handle;

    // todo
    switch (ref->handle->kind) {
        case REF_KIND_GET_FIELD:
            break;
        case REF_KIND_GET_STATIC:
            break;
        case REF_KIND_PUT_FIELD:
            break;
        case REF_KIND_PUT_STATIC:
            break;
        case REF_KIND_INVOKE_VIRTUAL:
            break;
        case REF_KIND_INVOKE_STATIC: {
            if (call_set == NULL) {
                set_call_set(curr_class, curr_thread, frame->m.method->clazz->loader, ref);
                frame->reader.pc = saved_pc; // recover pc
                return;
            }

            if (exact_method_handle != NULL) {
                // public final Object invokeExact(Object... args) throws Throwable
                struct method *exact_method = class_lookup_instance_method(
                        exact_method_handle->clazz, "invokeExact", "[Ljava/lang/Object;");
                // todo args
                jvm_abort(""); ////////////////////////////////////////////////////
                // invoke exact method, invokedynamic completely execute over.
                thread_invoke_method(curr_thread, exact_method, NULL);
                return;
            } else {
                // public abstract MethodHandle dynamicInvoker()
                struct method *dynamic_invoker = class_lookup_instance_method(
                        call_set->clazz, "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
                struct slot arg = rslot(call_set);
                thread_invoke_method_with_shim(curr_thread, dynamic_invoker, &arg, set_exact_method_handle);
                frame->reader.pc = saved_pc; // recover pc
                return;
            }
        }
        case REF_KIND_INVOKE_SPECIAL:
            break;
        case REF_KIND_NEW_INVOKE_SPECIAL:
            break;
        case REF_KIND_INVOKE_INTERFACE:
            break;
        default:
            VM_UNKNOWN_ERROR("unknown kind. %d", ref->handle->kind);
            break;
    }

    // todo rest thread.dyn 的值，下次调用时这个值要从新解析。
}
