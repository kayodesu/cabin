/*
 * Author: Jia Yang
 */

#ifndef JVM_REFERENCES_H
#define JVM_REFERENCES_H

#include "../interpreter/stack_frame.h"
#include "../rtda/ma/jmethod.h"
#include "../rtda/ma/jfield.h"
#include "../rtda/heap/jarrobj.h"
#include "../rtda/heap/jstrobj.h"
#include "../util/encoding.h"

/*
 * 类的初始化在下列情况下触发：
 * 1. 执行new指令创建类实例，但类还没有被初始化。
 * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
 * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
 * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
 * 5. 执行某些反射操作时。
 */


static void __new(struct stack_frame *frame)
{
    struct bytecode_reader *reader = frame->reader;

    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, bcr_readu2(reader)); // todo reads2  ?????
//    const string className = frame->method->jclass->rtcp->getClassName(s);
    if (class_name == NULL || strlen(class_name) == 0) {
        jvm_abort("error. className is empty\n");
    }

    struct jclass *c = resolve_class(frame->method->jclass, class_name);  // todo
    if (!c->inited) {
        jclass_clinit(c, frame);
        bcr_set_pc(reader, frame->thread->pc); // 将pc指向本条指令之前，初始化完类后，继续执行本条指令。
        return;
    }

    if (IS_INTERFACE(c->access_flags) || IS_ABSTRACT(c->access_flags)) {
//        // todo 抛出 InstantiationError 异常
        jvm_abort("error\n");
    }

    struct jobject *o;
    if (strcmp(c->class_name, "java/lang/String") == 0) {
//        o = new JStringObj(classLoader); // todo 在哪里找字符串的内容？
        jvm_abort("fffffffffffffff");
    } else {
        o = jobject_create(c); //JObject::newJobject(c);
    }

    os_pushr(frame->operand_stack, o); // frame->operandStack.push(o);
}

//#define RESOLVE_FIELD_REF_CODE_BLOCK(is_static) \
//    struct jclass *curr_class = frame->method->jclass; \
//     \
//    int index = bcr_readu2(frame->reader); \
//    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index); \
//     \
//    struct jclass *c = resolve_class(curr_class, ref->class_name); \
//    if (!c->inited) { \
//        jclass_clinit(c, frame);  \
//        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/ \
//        bcr_set_pc(frame->reader, frame->thread->pc); \
//        return; \
//    } \
//    struct jfield *field = resolve_field(curr_class, ref, is_static); \
//    if (field == NULL) { \
//        /* todo */ \
//        jvm_abort("error\n"); \
//    }

//static struct jfield* fetch_field(struct stack_frame *frame, bool is_static)
//{
//    struct jclass *curr_class = frame->method->jclass;
//
//    int index = bcr_readu2(frame->reader);
//    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
//
//    struct jclass *c = resolve_class(curr_class, ref->class_name);
//    if (!c->inited) {
//        jclass_clinit(c, frame);
//        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
//        bcr_set_pc(frame->reader, frame->thread->pc);
//        return NULL;
//    }
//    struct jfield *field = resolve_field(curr_class, ref, is_static);
//    if (field == NULL) {
//        /* todo */
//        jvm_abort("error\n");
//    }
//
//    // todo c 和 field->jclass 是不是一样的
//
//    return field;
//}

/*
 * putstatic指令给类的某个静态变量赋值，它需要两个操作数。
 * 第一个操作数是uint16索引，来自字节码。通过这个索引可以从当
 *    前类的运行时常量池中找到一个字段符号引用，解析这个符号引用
 *    就可以知道要给类的哪个静态变量赋值。
 * 第二个操作数是要赋给静态变量的值，从操作数栈中弹出。
 */
static void putstatic(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    if (!ref->resolved_class->inited) {
        jclass_clinit(ref->resolved_class, frame);
        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
        bcr_set_pc(frame->reader, frame->thread->pc);
        return;
    }

    /*
     * 如果是final字段，则实际操作的是静态常量，只能在类初始化方法中给它赋值。
     * 否则，会抛出 IllegalAccessError异常。类初始化方法由编译器生成，名字是<clinit>，
     */
    if (IS_FINAL(ref->resolved_field->access_flags)) {
        // todo
        if (frame->method->jclass != ref->resolved_class || strcmp(frame->method->name, "<clinit>") != 0) {
            // panic("java.lang.IllegalAccessError")
            jvm_abort("%s, %s, %s\n", frame->method->jclass->class_name, ref->resolved_class->class_name, frame->method->name);
        }
    }

    // todo
//    jclass->setFieldValue(field->id, field->descriptor,
//    [&] { return frame->operandStack.popInt(); },
//    [&] { return frame->operandStack.popFloat(); },
//    [&] { return frame->operandStack.popLong(); },
//    [&] { return frame->operandStack.popDouble(); },
//    [&] { return frame->operandStack.popRef(); });

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
        // todo error
        jvm_abort("error, %s\n", ref->resolved_field->descriptor);
    }

    fv_set_by_id(ref->resolved_class->static_field_values, ref->resolved_field->id, &s);
}

static void getstatic(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct field_ref *ref = rtcp_get_field_ref(curr_class->rtcp, index);
    resolve_static_field_ref(curr_class, ref);

    if (!ref->resolved_class->inited) {
        jclass_clinit(ref->resolved_class, frame);
        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
        bcr_set_pc(frame->reader, frame->thread->pc);
        return;
    }

    // todo 如果声明字段的类还没有初始化好，也需要先初始化。

    os_pushs(frame->operand_stack, fv_get_by_id(ref->resolved_class->static_field_values, ref->resolved_field->id));
//    os_pushs(frame->operand_stack, c->static_fields_values + field->id);
//    auto value = c->getFieldValue(field->id);
//    frame->operandStack.push(value);
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
        if (frame->method->jclass != ref->resolved_class || frame->method->name != "<init>") {
            // panic("java.lang.IllegalAccessError")
            jvm_abort("error\n");
        }
    }

    jref obj = os_popr(frame->operand_stack);//frame->operandStack.popRef();
    if (obj == NULL) {
        // todo
        // java.lang.NullPointerException
        jvm_abort("error\n");
    }

    // todo
//    obj->setFieldValue(field->id, field->descriptor,
//    [&] { return frame->operandStack.popInt(); },
//    [&] { return frame->operandStack.popFloat(); },
//    [&] { return frame->operandStack.popLong(); },
//    [&] { return frame->operandStack.popDouble(); },
//    [&] { return frame->operandStack.popRef(); });
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
        // todo error
        jvm_abort("error, %s\n", ref->resolved_field->descriptor);
    }

    fv_set_by_id(obj->instance_field_values, ref->resolved_field->id, &s);
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
        // todo
        // java.lang.NullPointerException
        jvm_abort("error\n");
    }

    os_pushs(frame->operand_stack, fv_get_by_id(obj->instance_field_values, ref->resolved_field->id));
//    os_pushs(frame->operand_stack, obj->instance_fields_values + field->id);
//    auto value = obj->getFieldValue(field->id);
//    frame->operandStack.push(value);
}

static void athrow(struct stack_frame *frame)
{
    jref exception = os_popr(frame->operand_stack);//frame->operandStack.popRef();
    if (exception == NULL) {
        // todo
        // java.lang.NullPointerException
        jvm_abort("error\n");
    }

    struct jthread *curr_thread = frame->thread;

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (!jthread_is_stack_empty(curr_thread)) {
        struct stack_frame *top = jthread_top_frame(curr_thread);
        size_t pc = top->reader->pc - 1; // todo why -1 ???
        int handler_pc = jmethod_find_exception_handler(top->method, exception->jclass, pc);
        if (handler_pc >= 0) {  // todo 可以等于0吗
            /*
             * 找到可以处理的函数了
             * 操作数栈清空
             * 把异常对象引用推入栈顶
             * 跳转到异常处理代码之前
             */
            os_clear(top->operand_stack);
            os_pushr(top->operand_stack, exception);
            bcr_set_pc(top->reader, handler_pc);  // todo 是setPc还是skip
            sf_proc_exception(top);
//            top->operandStack.clear();
//            top->operandStack.push(exception);
//            top->reader->setPc(handler_pc);  // todo 是setPc还是skip
//            top->procException();
            return;
        }

        jthread_pop_frame(curr_thread); // curr_thread->popFrame();
    }

    // todo UncaughtException
    struct jstrobj *so = (struct jstrobj *)
            slot_getr(fv_get_by_nt(exception->instance_field_values, "detailMessage", "Ljava/lang/String;"));
    jvm_abort("UncaughtException. %s. %s\n", exception->jclass->class_name, so->str);  // todo
//    auto ref = (exception->getFieldValue("detailMessage", "Ljava/lang/String;")).getRef();
//    JStringObj *o = dynamic_cast<JStringObj *>(ref);
//    jvm_abort("UncaughtException. %s. %s\n", exception->jclass->class_name, jstrToStr(o->value()).c_str());
}

/*
 * instanceof指令需要两个操作数。
 * 第一个操作数是uint16索引，从方法的字节码中获取，通过这个索引可以从当前类的运行时常量池中找到一个类符号引用。
 * 第二个操作数是对象引用，从操作数栈中弹出。
 */
static void instanceof(struct stack_frame *frame)
{
    int index = bcr_readu2(frame->reader);//frame->reader->readu2();
//    const string &className = frame->method->jclass->rtcp->getClassName(index);
    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, index);

    struct jclass *jclass = classloader_load_class(frame->method->jclass->loader, class_name); // todo resolve_class ???

    jref obj = os_popr(frame->operand_stack);//frame->operandStack.popRef();
    if (obj == NULL) {
        os_pushi(frame->operand_stack, 0); //frame->operandStack.push((jint)0);
        return;
    }

    os_pushi(frame->operand_stack, (jint)(jobject_is_instance_of(obj, jclass) ? 1 : 0));  // todo
//    frame->operandStack.push((jint)(obj->isInstanceOf(jclass) ? 1 : 0));
}

/*
 * checkcast指令和instanceof指令很像，区别在于：instanceof指令会改变操作数栈（弹出对象引用，推入判断结果）
 * checkcast则不改变操作数栈（如果判断失败，直接抛出ClassCastException异常）
 */
static void checkcast(struct stack_frame *frame)
{
    jref obj = os_popr(frame->operand_stack);//frame->operandStack.top().getRef();
    int index = bcr_readu2(frame->reader);//frame->reader->readu2();
    if (obj == NULL) {
        // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
        return;
    }

    const char *class_name = rtcp_get_class_name(frame->method->jclass->rtcp, index);
//    const string &className = frame->method->jclass->rtcp->getClassName(index);

    struct jclass *jclass = classloader_load_class(frame->method->jclass->loader, class_name); // todo resolve_class ???
//    struct jclass *jclass = frame->method->jclass->loader->loadClass(className);// todo resolve_class ???
    if (!jobject_is_instance_of(obj, jclass)) {
        // java.lang.ClassCastException   todo
        jvm_abort("java.lang.ClassCastException. %s can not cast to %s\n", obj->jclass->class_name, jclass->class_name);
    }
}

//static struct jmethod* fetch_method(struct stack_frame *frame, bool is_static, bool extra_read)
//{
//    struct jclass *curr_class = frame->method->jclass;
//
//    int index = bcr_readu2(frame->reader);
//    if (extra_read) { // todo 干嘛的
//        bcr_readu1(frame->reader);
//        bcr_readu1(frame->reader);
//    }
//    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
//
//    /* todo 本地方法 */
//
//    struct jclass *resolved_class = resolve_class(curr_class, ref->class_name);
//    if (!resolved_class->inited) {
//        jclass_clinit(resolved_class, frame);
//        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
//        bcr_set_pc(frame->reader, frame->thread->pc);
//        return NULL;
//    }
//
//    struct jmethod *resolved_method = resolve_method(curr_class, ref, is_static);
//    if (resolved_method == NULL) {
//        jvm_abort("error\n"); /* todo */
//    }
//
//    // todo c 和 method->jclass 是不是一样的
//    // 解析出的类为 resolved_class，方法为 resolved_method
//    // 则 resolved_class == method->jclass，或 resolved_class 是 resolved_method->jclass 的子类。
//
//    return resolved_method;
//}

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
//    struct jmethod *method = fetch_method(frame, true, false);
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_static_method_ref(curr_class, ref);

    if (IS_ABSTRACT(ref->resolved_method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    if (!ref->resolved_class->inited) {
        jclass_clinit(ref->resolved_class, frame);
        /* 将pc指向本条指令之前，初始化完类后，继续执行本条指令。*/
        bcr_set_pc(frame->reader, frame->thread->pc);
        return;
    }

    sf_invoke_method(frame, ref->resolved_method, NULL); //  frame->invokeMethod(method);
}

/*
 * invokespecial指令用于调用一些需要特殊处理的实例方法，包括实例初始化方法（§2.9）、私有方法和父类方法。
 */
static void invokespecial(struct stack_frame *frame)
{
//    struct jmethod *method = fetch_method(frame, false, false);
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
     * 如果调用的中超类中的函数，但不是构造函数，且当前类的ACC_SUPER标志被设置，
     * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
     * todo 详细说明
     */
    if (IS_SUPER(ref->resolved_class->access_flags)
            && jclass_is_subclass_of(curr_class, ref->resolved_class)
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

    if (method->arg_slot_count < 1) {
        jvm_abort("error\n");
    }

    struct slot args[method->arg_slot_count];
    for (int i = method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);//frame->operandStack.popSlot();
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        // todo  java.lang.NullPointerException
        jvm_abort("java.lang.NullPointerException\n");
    }

    sf_invoke_method(frame, method, args); //  frame->invokeMethod(method, args);
}

/*
 * invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派），这也是Java语言中最常见的方法分派方式。
 */
static void invokevirtual(struct stack_frame *frame)
{
//    struct jmethod *method = fetch_method(frame, false, false);
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_non_static_method_ref(curr_class, ref);

    if (ref->resolved_method->arg_slot_count < 1) {
        jvm_abort("error\n");
    }

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        if (strcmp(ref->resolved_method->name, "println") == 0) {
            // todo   println  暂时先这么搞
            struct jstrobj *so = (struct jstrobj *) slot_getr(args + 1);
            printvm("%s\n", so->str);
            return;
        }
        // todo  java.lang.NullPointerException
        jvm_abort("java.lang.NullPointerException\n");
    }

    // todo 返回的指针是不是jobject *   ？？？？？？？？
//    char *s = obj;
//    if (obj->getClass() != c) {
//        jprintf("error, %s, %s\n", obj->getClass()->toString().c_str(), c->toString().c_str());
//    }

//    jprintf("RRRRRRRRRRRRRRRRRR, %s, %s\n", obj->getClass()->className.c_str(), ref.className.c_str());

    // 从对象的类中查找真正要调用的方法
    struct jmethod *method = jclass_lookup_method(obj->jclass, ref->name, ref->descriptor);
    if (method == NULL) {
        jvm_abort("Can't find method: %s::%s:%s\n", obj->jclass->class_name, ref->name, ref->descriptor); // todo
    }

    if (IS_ABSTRACT(method->access_flags)) {
        // todo java.lang.AbstractMethodError
        jvm_abort("java.lang.AbstractMethodError\n");
    }

    sf_invoke_method(frame, method, args); //  frame->invokeMethod(method, args);
}

/*
 * invokeinterface指令用于调用接口方法，它会在运行时搜索一个实现了这个接口方法的对象，找出适合的方法进行调用。
 */
static void invokeinterface(struct stack_frame *frame)
{
//    struct jmethod *method = fetch_method(frame, false, true);
    struct jclass *curr_class = frame->method->jclass;

    int index = bcr_readu2(frame->reader);

    /*
     * 此字节的值是给方法传递参数需要的slot数，
     * 其含义和给Method结构体定义的argSlotCount字段相同。
     * 正如我们所知，这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
     */
    bcr_readu1(frame->reader);
    /*
     * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
     * 该字节的存在是为了保证Java虚拟机可以向后兼容。
     */
    bcr_readu1(frame->reader);
    struct method_ref *ref = rtcp_get_method_ref(curr_class->rtcp, index);
    resolve_non_static_method_ref(curr_class, ref);

    /* todo 本地方法 */


    if (ref->resolved_method->arg_slot_count < 1) {
        jvm_abort("error\n");
    }

    struct slot args[ref->resolved_method->arg_slot_count];
    for (int i = ref->resolved_method->arg_slot_count - 1; i >= 0; i--) {
        args[i] = *os_pops(frame->operand_stack);//frame->operandStack.popSlot();
    }

    jref obj = slot_getr(args); // args[0]
    if (obj == NULL) {
        // todo  java.lang.NullPointerException
        jvm_abort("java.lang.NullPointerException\n");
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

    sf_invoke_method(frame, method, args); //  frame->invokeMethod(method, args);
}


static void newarray(struct stack_frame *frame)
{
    jint arr_len = os_popi(frame->operand_stack);//frame->operandStack.popInt();
    if (arr_len < 0) {
        // todo  java.lang.NegativeArraySizeException
        jvm_abort("error. java.lang.NegativeArraySizeException. %d \n", arr_len);
        return;
    }

    // todo arrLen == 0 的情况

    /*
     * AT_BOOLEAN = 4
     * AT_CHAR = 5
     * AT_FLOAT = 6
     * AT_DOUBLE = 7
     * AT_BYTE = 8
     * AT_SHORT = 9
     * AT_INT = 10
     * AT_LONG = 11
     */
    int arr_type = bcr_readu1(frame->reader);//frame->reader->readu1();
    char *arr_name = NULL;
    switch (arr_type) {
        case 4: arr_name = "[Z"; break;
        case 5: arr_name = "[C"; break;
        case 6: arr_name = "[F"; break;
        case 7: arr_name = "[D"; break;
        case 8: arr_name = "[B"; break;
        case 9: arr_name = "[S"; break;
        case 10: arr_name = "[I"; break;
        case 11: arr_name = "[J"; break;
        default: arr_name = ""; break;
    }

    if (arr_name == NULL) {
        // todo
        jvm_abort("error. Invalid array type: %d.\n", arr_type);
        return;
    }

    struct jclass *c = classloader_load_class(frame->method->jclass->loader, arr_name);
    // 因为前面已经判断了 arr_len >= 0，所以 arr_len 转型为 size_t 是安全的
    os_pushr(frame->operand_stack, (jref) jarrobj_create(c, (size_t) arr_len));
    //frame->method->jclass->loader->loadClass(arrName);
    //frame->operandStack.push(JArrayObj::newJArrayObj(c, arrLen));
}

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 */
static void anewarray(struct stack_frame *frame)
{
#if 0
    jint arr_len = os_popi(frame->operand_stack);
    if (arr_len < 0) {
        // todo  java.lang.NegativeArraySizeException
        jvm_abort("error. java.lang.NegativeArraySizeException \n");
        return;
    }

    // todo arrLen == 0 的情况

    int index = bcr_readu2(frame->reader);//frame->reader->readu2();
    const char *className = rtcp_get_class_name(frame->method->jclass->rtcp, index);
    struct jclass *componentClass = resolve_class(frame->method->jclass, className);
    struct jclass *arrClass = jclass_array_class(componentClass);//componentClass->arrayClass();

    frame->operandStack.push(JArrayObj::newJArrayObj(arrClass, arrLen));
#endif
}

static void arraylength(struct stack_frame *frame)
{
    jref obj = os_popr(frame->operand_stack);//frame->operandStack.popRef();
    if (obj == NULL) {
        // todo java.lang.NullPointerException
        jvm_abort("error. java.lang.NullPointerException\n");
    }
    struct jarrobj *arr = (struct jarrobj *) obj;

    // todo 多维数组也可以？？？？？
//    if (!is_one_dimension_array(arr->class)) {
//        jprintf("error.\n"); // todo 一维数组?
//    }

    os_pushi(frame->operand_stack, arr->len); // frame->operandStack.push(arr->len);
}

static void monitorenter(struct stack_frame *frame)
{
    jref o = os_popr(frame->operand_stack);
    // todo
}

static void monitorexit(struct stack_frame *frame)
{
    jref o = os_popr(frame->operand_stack);
    // todo
}

#endif //JVM_REFERENCES_H
