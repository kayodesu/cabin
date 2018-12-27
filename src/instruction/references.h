/*
 * Author: Jia Yang
 */

#ifndef JVM_REFERENCES_H
#define JVM_REFERENCES_H

#include "../rtda/thread/frame.h"
#include "../rtda/ma/method.h"
#include "../rtda/ma/field.h"
#include "../util/encoding.h"
#include "../rtda/heap/object.h"
#include "../rtda/ma/symref.h"
#include "../rtda/ma/descriptor.h"
#include "../classfile/constant.h"

/*
 * 类的初始化在下列情况下触发：
 * 1. 执行new指令创建类实例，但类还没有被初始化。
 * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
 * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
 * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
 * 5. 执行某些反射操作时。
 */

/*
 * new指令专门用来创建类实例。数组由专门的指令创建 todo
 */
static void __new(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

    const char *class_name = rtcp_get_class_name(frame->m.method->jclass->rtcp, bcr_readu2(reader)); // todo reads2  ?????
    assert(class_name != NULL);
    assert(strlen(class_name) > 0);

    struct class *c = resolve_class(frame->m.method->jclass, class_name);  // todo
    if (!c->inited) {
        jclass_clinit(c, frame->thread);
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

/*
 * putstatic指令给类的某个静态变量赋值，它需要两个操作数。
 * 第一个操作数是uint16索引，来自字节码。通过这个索引可以从当
 *    前类的运行时常量池中找到一个字段符号引用，解析这个符号引用
 *    就可以知道要给类的哪个静态变量赋值。
 * 第二个操作数是要赋给静态变量的值，从操作数栈中弹出。
 */
static void putstatic(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    struct class *cls = ref->resolved_field->jclass;

    if (!cls->inited) {
        jclass_clinit(cls, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

#if 0
    // todo
    char d = *ref->resolved_field->descriptor;
    struct slot s;
    if (d == 'B' || d == 'C' || d == 'I' || d == 'S' || d == 'Z') {
        s = islot(os_popi(frame->operand_stack));
    } else if (d == 'F') {
        s = fslot(os_popf(frame->operand_stack));
    } else if (d == 'J') {
        s = lslot(os_popl(frame->operand_stack));
    } else if (d == 'D') {
        s = dslot(os_popd(frame->operand_stack));
    } else if (d  == 'L' || d  == '[') {
        s = rslot(os_popr(frame->operand_stack));
    } else {
        VM_UNKNOWN_ERROR("field's descriptor error. %s", ref->resolved_field->descriptor);
    }
    set_static_field_value_by_id(cls, ref->resolved_field->id, &s);
#endif
    struct slot *s = frame_stack_pop_slot(frame);
    if (slot_is_ph(s)) {
        s = frame_stack_pop_slot(frame);
    }
    set_static_field_value_by_id(cls, ref->resolved_field->id, s);
}

static void getstatic(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    struct class *cls = ref->resolved_field->jclass;

    if (!cls->inited) {
        jclass_clinit(cls, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }
    frame_stack_push_slot(frame, get_static_field_value_by_id(cls, ref->resolved_field->id));
//    os_pushs(frame->operand_stack, get_static_field_value_by_id(cls, ref->resolved_field->id));
}

/*
 * putfield指令给实例变量赋值，它需要三个操作数。
 * 前两个操作数是常量池索引和变量值，用法和putstatic一样。
 * 第三个操作数是对象引用，从操作数栈中弹出
 */
static void putfield(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(&frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    /*
     * 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
     */
    if (IS_FINAL(ref->resolved_field->access_flags)) {
        // todo
        if (frame->m.method->jclass != ref->resolved_class || strcmp(frame->m.method->name, "<init>") != 0) {
            jvm_abort("java.lang.IllegalAccessError\n"); // todo
        }
    }

#if 0
    // todo
    char d = *ref->resolved_field->descriptor;
    struct slot s;
    if (d == 'B' || d == 'C' || d == 'I' || d == 'S' || d == 'Z') {
        s = islot(os_popi(frame->operand_stack));
    } else if (d == 'F') {
        s = fslot(os_popf(frame->operand_stack));
    } else if (d == 'J') {
        s = lslot(os_popl(frame->operand_stack));
    } else if (d == 'D') {
        s = dslot(os_popd(frame->operand_stack));
    } else if (d  == 'L' || d  == '[') {
        s = rslot(os_popr(frame->operand_stack));
    } else {
        VM_UNKNOWN_ERROR("field's descriptor error. %s", ref->resolved_field->descriptor);
    }
#endif

    struct slot *s = frame_stack_pop_slot(frame);
    if (slot_is_ph(s)) {
        s = frame_stack_pop_slot(frame);
    }

    jref obj = frame_stack_popr(frame);  //os_popr(frame->operand_stack);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    set_instance_field_value_by_id(obj, ref->resolved_field->id, s);
}

/*
 * getfield指令获取对象的实例变量值，然后推入操作数栈，它需
 * 要两个操作数。第一个操作数是uint16索引，用法和前面三个指令
 * 一样。第二个操作数是对象引用，用法和putfield一样。
 */
static void getfield(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(&frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    jref obj = frame_stack_popr(frame);  //os_popr(frame->operand_stack);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    const struct slot *s = get_instance_field_value_by_id(obj, ref->resolved_field->id);
    frame_stack_push_slot(frame, s);
#if 0
    // 检查 slot 的类型与 field 的类型是否匹配  todo
    char d = *ref->resolved_field->descriptor;
    if (((d == 'B' || d == 'C' || d == 'I' || d == 'S' || d == 'Z') && s->t == JINT)
        || (d == 'F' && s->t == JFLOAT)
        || (d == 'J' && s->t == JLONG)
        || (d == 'D' && s->t == JDOUBLE)
        || ((d  == 'L' || d  == '[') && s->t == JREF)) {
        os_pushs(frame->operand_stack, s);
    } else {
        VM_UNKNOWN_ERROR("type mismatch error. field's descriptor is %s, but slot is %s",
                  ref->resolved_field->descriptor, slot_to_string(s));
    }
#endif
}

static void athrow(struct frame *frame)
{
    jref exception = frame_stack_popr(frame);  //os_popr(frame->operand_stack);
    if (exception == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    struct thread *curr_thread = frame->thread;

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (!jthread_is_stack_empty(curr_thread)) {
        struct frame *top = jthread_top_frame(curr_thread);
        if (top->type == SF_TYPE_NORMAL) {
            int handler_pc = jmethod_find_exception_handler(top->m.method, exception->jclass, top->reader.pc - 1); // instruction length todo 好像是错的
            if (handler_pc >= 0) {  // todo 可以等于0吗
                /*
                 * 找到可以处理的函数了
                 * 操作数栈清空
                 * 把异常对象引用推入栈顶
                 * 跳转到异常处理代码之前
                 */
                frame_stack_clear(top);
                frame_stack_pushr(top, exception);
                top->reader.pc = (size_t) handler_pc;
                frame_proc_exception(top);
                return;
            }
        }

        // top frame 无法处理异常，弹出
        jthread_pop_frame(curr_thread);
        if (top == frame) {
            // 当前执行的 frame 不能直接销毁，设置成执行完毕即可，由解释器销毁
            frame_exe_over(frame);
        } else {
            frame_destroy(top); // todo
        }
    }

    jthread_handle_uncaught_exception(curr_thread, exception);
}

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
static void instanceof(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    const char *class_name = rtcp_get_class_name(frame->m.method->jclass->rtcp, index);

    struct class *jclass = classloader_load_class(frame->m.method->jclass->loader, class_name); // todo resolve_class ???

    jref obj = frame_stack_popr(frame);  //os_popr(frame->operand_stack);
    if (obj == NULL) {
        frame_stack_pushi(frame, 0); //os_pushi(frame->operand_stack, 0);
        return;
    }

    frame_stack_pushi(frame, jobject_is_instance_of(obj, jclass) ? 1 : 0);
//    os_pushi(frame->operand_stack, (jint)(jobject_is_instance_of(obj, class) ? 1 : 0));
}

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
static void checkcast(struct frame *frame)
{
    jref obj = slot_getr(frame_stack_top(frame));
    int index = bcr_readu2(&frame->reader);
    if (obj == NULL) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

    const char *class_name = rtcp_get_class_name(frame->m.method->jclass->rtcp, index);

    struct class *jclass = classloader_load_class(frame->m.method->jclass->loader, class_name); // todo resolve_class ???
    if (!jobject_is_instance_of(obj, jclass)) {
        jthread_throw_class_cast_exception(frame->thread, obj->jclass->class_name, jclass->class_name);
    }
}

/*
 * 方法调用系列：
 * invokestatic指令用来调用静态方法。
 * invokespecial指令用来调用无须动态绑定的实例方法，包括构造函数、私有方法和通过super关键字调用的超类方法。
 * 剩下的情况则属于动态绑定。
 *     如果是针对接口类型的引用调用方法，就使用invokeinterface指令，
 *     否则使用invokevirtual指令。
 */
static void invokestatic(struct frame *frame)
{
    size_t saved_pc = frame->reader.pc - 1;
    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(&frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    if (ref->resolved_method == NULL) {
        ref->resolved_class = classloader_load_class(frame->m.method->jclass->loader, ref->class_name);
        // 不用按类层次搜索，直接get
        ref->resolved_method = jclass_get_declared_static_method(ref->resolved_class, ref->name, ref->descriptor);
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
        jclass_clinit(ref->resolved_class, frame->thread);
    }
}

/*
 * invokespecial指令用于调用一些需要特殊处理的实例方法，包括构造函数、私有方法和通过super关键字调用的超类方法。
 */
static void invokespecial(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;

    int index = bcr_readu2(&frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
//    if (ref->resolved_method == NULL) {
//        ref->resolved_class = classloader_load_class(frame->m.method->class->loader, ref->class_name);
//        ref->resolved_method = jclass_get_declared_nonstatic_method(ref->resolved_class, ref->name, ref->descriptor);
//    }
    resolve_non_static_method_ref(curr_class, ref);

    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
//    if (method->name == "<init>" && method->class != c) {
//        // todo java.lang.NoSuchMethodError
//        jvm_abort("java.lang.NoSuchMethodError\n");
//    }

    struct method *method = ref->resolved_method;
    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (IS_SUPER(ref->resolved_class->access_flags)
            && !IS_PRIVATE(method->access_flags)
            && jclass_is_subclass_of(curr_class, ref->resolved_class) // todo
            && strcmp(ref->resolved_method->name, "<init>") != 0) {
        method = jclass_lookup_method(curr_class->super_class, ref->name, ref->descriptor);
    }

    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    struct slot args[method->arg_slot_count];
    for (int i = method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *frame_stack_pop_slot(frame);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    jthread_invoke_method(frame->thread, method, args);
}

/*
 * invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
 */
static void invokevirtual(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;

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
        jthread_throw_null_pointer_exception(frame->thread);
    }

    // 从对象的类中查找真正要调用的方法
    struct method *method = jclass_lookup_method(obj->jclass, ref->name, ref->descriptor);
    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    jthread_invoke_method(frame->thread, method, args);
}

/*
 * invokeinterface指令用于调用接口方法，它会在运行时搜索一个实现了这个接口方法的对象，找出适合的方法进行调用。
 */
static void invokeinterface(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;
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

    struct method_ref *ref = rtcp_get_interface_method_ref(curr_class->rtcp, index);
//    resolve_non_static_method_ref(curr_class, ref);

    /* todo 本地方法 */

    struct slot args[arg_slot_count];
    for (int i = arg_slot_count - 1; i >= 0; i--) {
        args[i] = *frame_stack_pop_slot(frame);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    struct method *method = jclass_lookup_method(obj->jclass, ref->name, ref->descriptor);
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

    jthread_invoke_method(frame->thread, method, args);
}

/*
 * todo 说明 invokedynamic!!!!!!!
 */
static void set_invoked_type(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.invoked_type = frame_stack_popr(frame);
    int i = 3;
}

static void set_caller(struct frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.caller = frame_stack_popr(frame);
    int i = 3;
}

static void set_call_set(struct frame *frame)
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
// 每一个 invokedynamic 指令都称为 Dynamic Call Site(动态调用点)
static void invokedynamic(struct frame *frame)
{
  //  jvm_abort("invokedynamic not implement\n");  // todo

    size_t saved_pc = frame->reader.pc - 1;

    struct class *curr_class = frame->m.method->jclass;
    struct thread *curr_thread = frame->thread;

    // The run-time constant pool item at that index must be a symbolic reference to a call site specifier.
    int index = bcr_readu2(&frame->reader); // CONSTANT_InvokeDynamic_info

    bcr_readu1(&frame->reader); // this byte must always be zero.
    bcr_readu1(&frame->reader); // this byte must always be zero.

    struct invoke_dynamic_ref *ref = rtcp_get_invoke_dynamic(curr_class->rtcp, index);

    // create java/lang/invoke/MethodType of bootstrap method
    struct object *parameter_types = method_descriptor_to_parameter_types(curr_class->loader, ref->nt->descriptor);
    struct object *return_type = method_descriptor_to_return_type(curr_class->loader, ref->nt->descriptor);
    struct object *invoked_type = curr_thread->dyn.invoked_type;
    if (invoked_type == NULL) {
        struct class *c = classloader_load_class(g_bootstrap_loader, "java/lang/invoke/MethodType");

        // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes)
        if (jarrobj_len(parameter_types) > 0) {
            struct method *get_method_type = jclass_lookup_static_method(
                    c, "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");

            struct slot args[] = { rslot(return_type), rslot(parameter_types) };
            jthread_invoke_method_with_shim(curr_thread, get_method_type, args, set_invoked_type);
        } else {
            struct method *get_method_type = jclass_lookup_static_method(
                    c, "methodType", "(Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
            struct slot args[] = { rslot(return_type) };
            jthread_invoke_method_with_shim(curr_thread, get_method_type, args, set_invoked_type);
        }

        if (!c->inited) { // 后压栈，先执行
            jclass_clinit(c, curr_thread);
        }
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

    struct object *caller = curr_thread->dyn.caller;
    if (caller == NULL) {
        struct class *c = classloader_load_class(g_bootstrap_loader, "java/lang/invoke/MethodHandles");
        struct method *lookup = jclass_lookup_static_method(c, "lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
        jthread_invoke_method_with_shim(curr_thread, lookup, NULL, set_caller);

        if (!c->inited) { // 后压栈，先执行
            jclass_clinit(c, curr_thread);
        }
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

#if 1
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
        if (call_set != NULL) {
            if (exact_method_handle != NULL) {
                // public final Object invokeExact(Object... args) throws Throwable
                struct method *exact_method = jclass_lookup_instance_method(
                        exact_method_handle->jclass, "invokeExact", "[Ljava/lang/Object;");
                // todo args
                jvm_abort(""); ////////////////////////////////////////////////////
                // invoke exact method, invokedynamic completely execute over.
                jthread_invoke_method(curr_thread, exact_method, NULL);
                return;
            } else {
                // public abstract MethodHandle dynamicInvoker()
                struct method *dynamic_invoker = jclass_lookup_instance_method(
                        call_set->jclass, "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
                struct slot arg = rslot(call_set);
                jthread_invoke_method_with_shim(curr_thread, dynamic_invoker, &arg, set_exact_method_handle);
                frame->reader.pc = saved_pc; // recover pc
                return;
            }
        }

        struct method_ref *mr = ref->handle->ref.mr;

        struct class *bootstrap_class = classloader_load_class(frame->m.method->jclass->loader, mr->class_name);
        struct method *bootstrap_method = jclass_lookup_static_method(bootstrap_class, mr->name, mr->descriptor);
        // todo 说明 bootstrap_method 的格式
        // bootstrap_method is static,  todo 对不对
        // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
        // 后续的参数由 ref->argc and ref->args 决定
        assert(ref->argc + 3 == bootstrap_method->arg_slot_count);
        struct slot args[bootstrap_method->arg_slot_count];
        args[0] = rslot(caller);
        args[1] = rslot(strobj_create(mr->name));
        args[2] = rslot(invoked_type);
        // parse remaining args
        for (int i = 0, j = 3; i < ref->argc; i++, j++) {
            args[j] = rtc_to_slot(curr_class->loader, curr_class->rtcp, ref->args[i]); // todo 用哪个类的 rtcp
        }

        jthread_invoke_method_with_shim(curr_thread, bootstrap_method, args, set_call_set);
        frame->reader.pc = saved_pc; // recover pc
        return;
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

//    jvm_abort("invokedynamic not implement\n");  // todo
#endif
}

/*
 * 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
 * 显然基本类型数组肯定都是一维数组，
 * 如果引用类型数组的元素也是数组，那么它就是多维数组。
 */
static void newarray(struct frame *frame)
{
    jint arr_len = frame_stack_popi(frame);  //os_popi(frame->operand_stack);
    if (arr_len < 0) {
        jthread_throw_negative_array_size_exception(frame->thread, arr_len);
        return;
    }

    // todo arrLen == 0 的情况

    /*
     * AT_BOOLEAN = 4
     * AT_CHAR    = 5
     * AT_FLOAT   = 6
     * AT_DOUBLE  = 7
     * AT_BYTE    = 8
     * AT_SHORT   = 9
     * AT_INT     = 10
     * AT_LONG    = 11
     */
    int arr_type = bcr_readu1(&frame->reader);
    char *arr_name;
    switch (arr_type) {
        case 4: arr_name = "[Z"; break;
        case 5: arr_name = "[C"; break;
        case 6: arr_name = "[F"; break;
        case 7: arr_name = "[D"; break;
        case 8: arr_name = "[B"; break;
        case 9: arr_name = "[S"; break;
        case 10: arr_name = "[I"; break;
        case 11: arr_name = "[J"; break;
        default:
            VM_UNKNOWN_ERROR("error. Invalid array type: %d", arr_type);
            return;
    }

    struct class *c = classloader_load_class(frame->m.method->jclass->loader, arr_name);
    // 因为前面已经判断了 arr_len >= 0，所以 arr_len 转型为 size_t 是安全的
    frame_stack_pushr(frame, arrobj_create(c, (size_t) arr_len));
    //os_pushr(frame->operand_stack, (jref) jarrobj_create(c, (size_t) arr_len));
}

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 * 创建一维引用类型数组
 */
static void anewarray(struct frame *frame)
{
    struct class *curr_class = frame->m.method->jclass;
    jint arr_len = frame_stack_popi(frame);  //os_popi(frame->operand_stack);
    if (arr_len < 0) {
        jthread_throw_negative_array_size_exception(frame->thread, arr_len);
        return;
    }

    // todo arrLen == 0 的情况

    int index = bcr_readu2(&frame->reader);
    const char *class_name = rtcp_get_class_name(frame->m.method->jclass->rtcp, index); // 数组元素的类
    char *arr_class_name = get_arr_class_name(class_name);
    struct class *arr_class = classloader_load_class(curr_class->loader, arr_class_name);
    free(arr_class_name);
    frame_stack_pushr(frame, arrobj_create(arr_class, (size_t) arr_len));
    //os_pushr(frame->operand_stack, (jref) jarrobj_create(arr_class, (size_t) arr_len));
}

static void arraylength(struct frame *frame)
{
    struct object *o = frame_stack_popr(frame);
    if (o == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }
    if (!jobject_is_array(o)) {
        vm_unknown_error("not a array");
    }

    frame_stack_pushi(frame, jarrobj_len(o));
}

static void monitorenter(struct frame *frame)
{
    jref o = frame_stack_popr(frame); //os_popr(frame->operand_stack);

    // todo
//    thread := frame.Thread()
//    ref := frame.OperandStack().PopRef()
//    if ref == nil {
//        frame.RevertNextPC()
//        thread.ThrowNPE()
//    } else {
//        ref.Monitor().Enter(thread)
//    }
}

static void monitorexit(struct frame *frame)
{
    jref o = frame_stack_popr(frame); //os_popr(frame->operand_stack);

    // todo
//    thread := frame.Thread()
//    ref := frame.OperandStack().PopRef()
//    if ref == nil {
//        frame.RevertNextPC()
//        thread.ThrowNPE()
//    } else {
//        ref.Monitor().Exit(thread)
//    }
}

#endif //JVM_REFERENCES_H
