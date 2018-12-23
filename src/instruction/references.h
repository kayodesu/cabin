/*
 * Author: Jia Yang
 */

#ifndef JVM_REFERENCES_H
#define JVM_REFERENCES_H

#include "../interpreter/stack_frame.h"
#include "../rtda/ma/jmethod.h"
#include "../rtda/ma/jfield.h"
#include "../util/encoding.h"
#include "../rtda/heap/jobject.h"
#include "../rtda/ma/symref.h"
#include "../rtda/ma/descriptor.h"

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
static void __new(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;
    size_t saved_pc = reader->pc - 1;

    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, bcr_readu2(reader)); // todo reads2  ?????
    assert(class_name != NULL);
    assert(strlen(class_name) > 0);

    struct jclass *c = resolve_class(frame->method->jclass, class_name);  // todo
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

    os_pushr(frame->operand_stack, jobject_create(c));
}

/*
 * putstatic指令给类的某个静态变量赋值，它需要两个操作数。
 * 第一个操作数是uint16索引，来自字节码。通过这个索引可以从当
 *    前类的运行时常量池中找到一个字段符号引用，解析这个符号引用
 *    就可以知道要给类的哪个静态变量赋值。
 * 第二个操作数是要赋给静态变量的值，从操作数栈中弹出。
 */
static void putstatic(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    struct jclass *cls = ref->resolved_field->jclass;

    if (!cls->inited) {
        jclass_clinit(cls, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }

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
}

static void getstatic(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;
    size_t saved_pc = reader->pc - 1;

    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    struct jclass *cls = ref->resolved_field->jclass;

    if (!cls->inited) {
        jclass_clinit(cls, frame->thread);
        reader->pc = saved_pc; // recover pc
        return;
    }
    os_pushs(frame->operand_stack, get_static_field_value_by_id(cls, ref->resolved_field->id));
}

/*
 * putfield指令给实例变量赋值，它需要三个操作数。
 * 前两个操作数是常量池索引和变量值，用法和putstatic一样。
 * 第三个操作数是对象引用，从操作数栈中弹出
 */
static void putfield(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    /*
     * 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
     */
    if (IS_FINAL(ref->resolved_field->access_flags)) {
        // todo
        if (frame->method->jclass != ref->resolved_class || strcmp(frame->method->name, "<init>") != 0) {
            jvm_abort("java.lang.IllegalAccessError\n"); // todo
        }
    }

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

    jref obj = os_popr(frame->operand_stack);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    set_instance_field_value_by_id(obj, ref->resolved_field->id, &s);
}

/*
 * getfield指令获取对象的实例变量值，然后推入操作数栈，它需
 * 要两个操作数。第一个操作数是uint16索引，用法和前面三个指令
 * 一样。第二个操作数是对象引用，用法和putfield一样。
 */
static void getfield(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_non_static_field_ref(curr_class, ref);

    jref obj = os_popr(frame->operand_stack);
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    const struct slot *s = get_instance_field_value_by_id(obj, ref->resolved_field->id);

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
}

static void athrow(struct stack_frame *frame)
{
    jref exception = os_popr(frame->operand_stack);
    if (exception == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    struct jthread *curr_thread = frame->thread;

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (!jthread_is_stack_empty(curr_thread)) {
        struct stack_frame *top = jthread_top_frame(curr_thread);
        if (top->type == SF_TYPE_NORMAL) {
            int handler_pc = jmethod_find_exception_handler(top->method, exception->jclass, top->reader->pc - 1); // instruction length todo 好像是错的
            if (handler_pc >= 0) {  // todo 可以等于0吗
                /*
                 * 找到可以处理的函数了
                 * 操作数栈清空
                 * 把异常对象引用推入栈顶
                 * 跳转到异常处理代码之前
                 */
                os_clear(top->operand_stack);
                os_pushr(top->operand_stack, exception);
                top->reader->pc = (size_t) handler_pc;
                sf_proc_exception(top);
                return;
            }
        }

        // top frame 无法处理异常，弹出
        jthread_pop_frame(curr_thread);
        if (top == frame) {
            // 当前执行的 frame 不能直接销毁，设置成执行完毕即可，由解释器销毁
            sf_exe_over(frame);
        } else {
            sf_destroy(top);
        }
    }

    jthread_handle_uncaught_exception(curr_thread, exception);
}

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
static void instanceof(struct stack_frame *frame)
{
    int index = bcr_readu2(frame->reader);
    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, index);

    struct jclass *jclass = classloader_load_class(frame->method->jclass->loader, class_name); // todo resolve_class ???

    jref obj = os_popr(frame->operand_stack);
    if (obj == NULL) {
        os_pushi(frame->operand_stack, 0);
        return;
    }

    os_pushi(frame->operand_stack, (jint)(jobject_is_instance_of(obj, jclass) ? 1 : 0));
}

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
static void checkcast(struct stack_frame *frame)
{
    jref obj = os_top(frame->operand_stack)->v.r;
    int index = bcr_readu2(frame->reader);
    if (obj == NULL) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, index);

    struct jclass *jclass = classloader_load_class(frame->method->jclass->loader, class_name); // todo resolve_class ???
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
static void invokestatic(struct stack_frame *frame)
{
    size_t saved_pc = frame->reader->pc - 1;
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_static_method_ref(curr_class, ref);

    if (IS_ABSTRACT(ref->resolved_method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    if (!ref->resolved_class->inited) {
        jclass_clinit(ref->resolved_class, frame->thread);
        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
//        bcr_set_pc(frame->reader, jthread_get_pc(frame->thread));
        frame->reader->pc = saved_pc; // recover pc
        return;
    }

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);
    }

    jthread_invoke_method(frame->thread, ref->resolved_method, args);
}

/*
 * invokespecial指令用于调用一些需要特殊处理的实例方法，包括实例初始化方法、私有方法和父类方法。
 */
static void invokespecial(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_non_static_method_ref(curr_class, ref);

    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
//    if (method->name == "<init>" && method->jclass != c) {
//        // todo java.lang.NoSuchMethodError
//        jvm_abort("java.lang.NoSuchMethodError\n");
//    }

    struct jmethod *method = ref->resolved_method;
    /*
     * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (IS_SUPER(ref->resolved_class->access_flags)
            && !IS_PRIVATE(method->access_flags)
            && jclass_is_subclass_of(curr_class, ref->resolved_class) // todo
            && strcmp(ref->resolved_method->name, "<init>") != 0) {
        struct jmethod *tmp = jclass_lookup_method(curr_class->super_class, ref->name, ref->descriptor);
        if (tmp != NULL) {
            method = tmp;
        }
    }

    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    struct slot args[method->arg_slot_count];
    for (int i = method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    jthread_invoke_method(frame->thread, method, args);
}

/*
 * invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派），这也是Java语言中最常见的方法分派方式。
 */
static void invokevirtual(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_non_static_method_ref(curr_class, ref);

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);
    }

    struct jobject *obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    // 从对象的类中查找真正要调用的方法
    struct jmethod *method = jclass_lookup_method(obj->jclass, ref->name, ref->descriptor);
    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    jthread_invoke_method(frame->thread, method, args);
}

/*
 * invokeinterface指令用于调用接口方法，它会在运行时搜索一个实现了这个接口方法的对象，找出适合的方法进行调用。
 */
static void invokeinterface(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;
    int index = bcr_readu2(frame->reader);

    /*
     * 此字节的值是给方法传递参数需要的slot数，
     * 其含义和给method结构体定义的arg_slot_count字段相同。
     * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
     */
    bcr_readu1(frame->reader);
    /*
     * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
     * 该字节的存在是为了保证Java虚拟机可以向后兼容。
     */
    bcr_readu1(frame->reader);

    struct method_ref *ref = rtcp_get_interface_method_ref(curr_class->rtcp, index);
    resolve_non_static_method_ref(curr_class, ref);

    /* todo 本地方法 */

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    struct jmethod *method = jclass_lookup_method(obj->jclass, ref->name, ref->descriptor);
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
static void set_invoked_type(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.invoked_type = os_popr(frame->operand_stack);
}

static void set_caller(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.caller = os_popr(frame->operand_stack);
}

static void set_call_set(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.call_set = os_popr(frame->operand_stack);
}

static void set_exact_method_handle(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->thread->dyn.exact_method_handle = os_popr(frame->operand_stack);
}

static void invokedynamic(struct stack_frame *frame)
{
    size_t saved_pc = frame->reader->pc - 1;

    struct jclass *curr_class = frame->method->jclass;
    struct jthread *curr_thread = frame->thread;

    // The run-time constant pool item at that index must be a symbolic reference to a call site specifier.
    int index = bcr_readu2(frame->reader);

    bcr_readu1(frame->reader); // this byte must always be zero.
    bcr_readu1(frame->reader); // this byte must always be zero.

    struct invoke_dynamic_ref *ref = rtcp_get_invoke_dynamic(curr_class->rtcp, index);

    bool need_again = false;

    // create java/lang/invoke/MethodType of bootstrap method
    struct jobject *parameter_types = method_descriptor_to_parameter_types(curr_class->loader, ref->nt->descriptor);
    struct jobject *return_type = method_descriptor_to_return_type(curr_class->loader, ref->nt->descriptor);

    struct jobject *invoked_type = curr_thread->dyn.invoked_type;
    if (invoked_type == NULL) {
        need_again = true;
        struct jclass *mt = classloader_load_class(bootstrap_loader, "java/lang/invoke/MethodType");

        // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes)
        struct jmethod *get_method_type = jclass_lookup_static_method(
                mt, "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
        struct slot args[] = { rslot(return_type), rslot(parameter_types) };
        jthread_invoke_method_with_shim(curr_thread, get_method_type, args, set_invoked_type);

        if (!mt->inited) { // 后压栈，先执行
            jclass_clinit(mt, curr_thread);
        }
    }

    struct jobject *caller = curr_thread->dyn.caller;
    if (caller == NULL) {
        need_again = true;
        struct jclass *mhs = classloader_load_class(bootstrap_loader, "java/lang/invoke/MethodHandles");

        // Lookup(Class<?> lookupClass)
        struct jmethod *lookup = jclass_lookup_static_method(mhs, "lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
        jthread_invoke_method_with_shim(curr_thread, lookup, NULL, set_caller);

        if (!mhs->inited) { // 后压栈，先执行
            jclass_clinit(mhs, curr_thread);
        }
    }

    if (need_again) {
        frame->reader->pc = saved_pc; // recover pc
        return;
    }

    struct jobject *call_set = curr_thread->dyn.call_set;
    struct jobject *exact_method_handle = curr_thread->dyn.exact_method_handle;

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
                struct jmethod *exact_method = jclass_lookup_instance_method(
                        exact_method_handle->jclass, "invokeExact", "[Ljava/lang/Object;");
                // todo args
                jvm_abort(""); ////////////////////////////////////////////////////
                // invoke exact method, invokedynamic completely execute over.
                jthread_invoke_method(curr_thread, exact_method, NULL);
                return;
            } else {
                // public abstract MethodHandle dynamicInvoker()
                struct jmethod *dynamicInvoker = jclass_lookup_instance_method(
                        call_set->jclass, "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
                struct slot arg = rslot(call_set);
                jthread_invoke_method_with_shim(curr_thread, dynamicInvoker, &arg, set_exact_method_handle);
                frame->reader->pc = saved_pc; // recover pc
            }

            frame->reader->pc = saved_pc; // recover pc
            return;
        }

        struct method_ref *mr = ref->handle->ref.mr;

        struct jclass *bootstrap_class = classloader_load_class(frame->method->jclass->loader, mr->class_name);
        struct jmethod *bootstrap_method = jclass_lookup_static_method(bootstrap_class, mr->name, mr->descriptor);
        // todo 说明 bootstrap_method 的格式
        // bootstrap_method is static,  todo 对不对
        // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
        // 后续的参数由 ref->argc and ref->args 决定
        assert(ref->argc + 3 == bootstrap_method->arg_slot_count);
        struct slot args[bootstrap_method->arg_slot_count];
        args[0] = rslot(caller);
        args[1] = rslot(jstrobj_create(mr->name));
        args[2] = rslot(invoked_type);
        // parse remaining args
        for (int i = 0, j = 3; i < ref->argc; i++, j++) {
            args[j] = rtc_to_slot(curr_class->loader, curr_class->rtcp, ref->args[i]); // todo 用哪个类的 rtcp
        }

        jthread_invoke_method_with_shim(curr_thread, bootstrap_method, args, set_call_set);
        frame->reader->pc = saved_pc; // recover pc
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

    jvm_abort("invokedynamic not implement\n");  // todo
}

/*
 * 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
 * 显然基本类型数组肯定都是一维数组，
 * 如果引用类型数组的元素也是数组，那么它就是多维数组。
 */
static void newarray(struct stack_frame *frame)
{
    jint arr_len = os_popi(frame->operand_stack);
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
    int arr_type = bcr_readu1(frame->reader);
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

    struct jclass *c = classloader_load_class(frame->method->jclass->loader, arr_name);
    // 因为前面已经判断了 arr_len >= 0，所以 arr_len 转型为 size_t 是安全的
    os_pushr(frame->operand_stack, (jref) jarrobj_create(c, (size_t) arr_len));
}

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 * 创建一维引用类型数组
 */
static void anewarray(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;
    jint arr_len = os_popi(frame->operand_stack);
    if (arr_len < 0) {
        jthread_throw_negative_array_size_exception(frame->thread, arr_len);
        return;
    }

    // todo arrLen == 0 的情况

    int index = bcr_readu2(frame->reader);
    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, index); // 数组元素的类
    char *arr_class_name = get_arr_class_name(class_name);
    struct jclass *arr_class = classloader_load_class(curr_class->loader, arr_class_name);
    free(arr_class_name);
    os_pushr(frame->operand_stack, (jref) jarrobj_create(arr_class, (size_t) arr_len));
}

static void arraylength(struct stack_frame *frame)
{
    struct jobject *o = os_popr(frame->operand_stack);
    if (o == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }
    if (!jobject_is_array(o)) {
        vm_unknown_error("not a array");
    }

    os_pushi(frame->operand_stack, jarrobj_len(o));
}

static void monitorenter(struct stack_frame *frame)
{
    jref o = os_popr(frame->operand_stack);

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

static void monitorexit(struct stack_frame *frame)
{
    jref o = os_popr(frame->operand_stack);

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
