/*
 * Author: kayo
 */

#include <iostream>
#include "../kayo.h"
#include "../debug.h"
#include "../rtda/thread/Thread.h"
#include "../rtda/thread/Frame.h"
#include "../rtda/heap/StrPool.h"
#include "../classfile/constant.h"
#include "../rtda/heap/ArrayObject.h"
#include "../rtda/heap/ClassObject.h"
#include "../rtda/ma/Field.h"
#include "../rtda/ma/resolve.h"
#include "interpreter.h"
#include "../symbol.h"

using namespace std;

#if TRACE_INTERPRETER
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

#if TRACE_INTERPRETER
// the mapping of instructions's code and name
static const char *instruction_names[] = {
        "nop",

        // Constants [0x01 ... 0x14]
        "aconst_null",
        "iconst_m1", "iconst_0", "iconst_1", "iconst_2", "iconst_3", "iconst_4", "iconst_5",
        "lconst_0", "lconst_1",
        "fconst_0", "fconst_1", "fconst_2",
        "dconst_0", "dconst_1",
        "bipush", "sipush",
        "ldc", "ldc_w", "ldc2_w",

        // Loads [0x15 ... 0x35]
        "iload", "lload", "fload", "dload", "aload",
        "iload_0", "iload_1", "iload_2", "iload_3",
        "lload_0", "lload_1", "lload_2", "lload_3",
        "fload_0", "fload_1", "fload_2", "fload_3",
        "dload_0", "dload_1", "dload_2", "dload_3",
        "aload_0", "aload_1", "aload_2", "aload_3",
        "iaload", "laload", "faload", "daload", "aaload", "baload", "caload", "saload",

        // Stores [0x36 ... 0x56]
        "istore", "lstore", "fstore", "dstore", "astore",
        "istore_0", "istore_1", "istore_2", "istore_3",
        "lstore_0", "lstore_1", "lstore_2", "lstore_3",
        "fstore_0", "fstore_1", "fstore_2", "fstore_3",
        "dstore_0", "dstore_1", "dstore_2", "dstore_3",
        "astore_0", "astore_1", "astore_2", "astore_3",
        "iastore", "lastore", "fastore", "dastore", "aastore", "bastore", "castore", "sastore",

        // Stack [0x57 ... 0x5f]
        "pop", "pop2", "dup", "dup_x1", "dup_x2", "dup2", "dup2_x1", "dup2_x2", "swap",

        // Math [0x60 ... 0x84]
        "iadd", "ladd", "fadd", "dadd",
        "isub", "lsub", "fsub", "dsub",
        "imul", "lmul", "fmul", "dmul",
        "idiv", "ldiv", "fdiv", "ddiv",
        "irem", "lrem", "frem", "drem",
        "ineg", "lneg", "fneg", "dneg",
        "ishl", "lshl", "ishr", "lshr", "iushr", "lushr",
        "iand", "land", "ior", "lor", "ixor", "lxor", "iinc",

        // Conversions [0x85 ... 0x93]
        "i2l", "i2f", "i2d",
        "l2i", "l2f", "l2d",
        "f2i", "f2l", "f2d",
        "d2i", "d2l", "d2f",
        "i2b", "i2c", "i2s",

        // Comparisons [0x94 ... 0xa6]
        "lcmp", "fcmpl", "fcmpg", "dcmpl", "dcmpg",
        "ifeq", "ifne", "iflt", "ifge", "ifgt", "ifle",
        "if_icmpeq", "if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt", "if_icmple",
        "if_acmpeq", "if_acmpne",

        // Control [0xa7 ... 0xb1]
        "goto", "jsr", "ret", "tableswitch", "lookupswitch",
        "ireturn", "lreturn", "freturn", "dreturn","areturn", "return",

        // References [0xb2 ... 0xc3]
        "getstatic", "putstatic", "getfield", "putfield",
        "invokevirtual", "invokespecial", "invokestatic", "invokeinterface", "invokedynamic",
        "new", "newarray", "anewarray", "arraylength",
        "athrow", "checkcast", "instanceof", "monitorenter", "monitorexit",

        // Extended [0xc4 ... 0xc9]
        "wide", "multianewarray", "ifnull", "ifnonnull", "goto_w", "jsr_w",

        // Reserved [0xca ... 0xff]
        "breakpoint",
        "notused", "notused", "notused", "notused", "notused", // [0xcb ... 0xcf]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xd0 ... 0xd7]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xd8 ... 0xdf]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xe0 ... 0xe7]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xe8 ... 0xef]
        "notused", "notused", "notused", "notused", "notused", "notused", "notused", "notused", // [0xf0 ... 0xf7]
        "notused", "notused", "notused", "notused", "notused", "notused", // [0xf8 ... 0xfd]
        "invokenative", "impdep2",
};
#endif

// constant instructions -----------------------------------------------------------------------------------------------
static void __ldc(Frame *frame, int index)
{
    ConstantPool &cp = frame->method->clazz->cp;
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Integer) {
        frame->pushi(CP_INT(cp, index));
    } else if (type == CONSTANT_Float) {
        frame->pushf(CP_FLOAT(cp, index));
    } else if (type == CONSTANT_String) {
        frame->pushr(resolve_string(frame->method->clazz, index));
    } else if (type == CONSTANT_ResolvedString) {
        frame->pushr((jref) CP_INFO(cp, index));
    } else if (type == CONSTANT_Class) {
        frame->pushr(resolve_class(frame->method->clazz, index)->clsobj);
    } else if (type == CONSTANT_ResolvedClass) {
        auto c = (Class *) CP_INFO(cp, index);
        frame->pushr(c->clsobj);
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

void ldc2_w(Frame *frame)
{
    int index = frame->reader.readu2();
    ConstantPool &cp = frame->method->clazz->cp;
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Long) {
        frame->pushl(CP_LONG(cp, index));
    } else if (type == CONSTANT_Double) {
        frame->pushd(CP_DOUBLE(cp, index));
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void tableswitch(Frame *frame)
{
    BytecodeReader &reader = frame->reader;
    size_t saved_pc = reader.pc - 1; // save the pc before 'tableswitch' instruction
    reader.align4();

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = reader.reads4();

    // low 和 height 标识了 case 的取值范围。
    s4 low = reader.reads4();
    s4 height = reader.reads4();

    // 跳转偏移量表，对应于各个 case 的情况
    s4 jump_offset_count = height - low + 1;
    s4 jump_offsets[jump_offset_count];
    reader.reads4s(jump_offset_count, jump_offsets);

    // 弹出要判断的值
    jint index = frame->popi();
    s4 offset;
    if (index < low || index > height) {
        offset = default_offset; // 没在 case 标识的范围内，跳转到 default 分支。
    } else {
        offset = jump_offsets[index - low]; // 找到对应的case了
    }

    // The target address that can be calculated from each jump table
    // offset, as well as the one that can be calculated from default,
    // must be the address of an opcode of an instruction within the method
    // that contains this tableswitch instruction.
    reader.pc = saved_pc + offset;
}

/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void lookupswitch(Frame *frame)
{
    BytecodeReader &reader = frame->reader;
    size_t saved_pc = reader.pc - 1; // save the pc before 'lookupswitch' instruction
    reader.align4();

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = reader.reads4();

    // case的个数
    s4 npairs = reader.reads4();
    assert(npairs >= 0); // The npairs must be greater than or equal to 0.

    // match_offsets 有点像 Map，它的 key 是 case 值，value 是跳转偏移量。
    s4 match_offsets[npairs * 2];
    reader.reads4s(npairs * 2, match_offsets);

    // 弹出要判断的值
    jint key = frame->popi();
    s4 offset = default_offset;
    for (int i = 0; i < npairs * 2; i += 2) {
        if (match_offsets[i] == key) { // 找到 case
            offset = match_offsets[i + 1];
            break;
        }
    }

    // The target address is calculated by adding the corresponding offset
    // to the address of the opcode of this lookupswitch instruction.
    reader.pc = saved_pc + offset;
}

// extended instructions -----------------------------------------------------------------------------------------------
/*
 * 创建多维数组
 * todo 注意这种情况，基本类型的多维数组 int[][][]
 */
static void multianewarray(Frame *frame)
{
    Class *curr_class = frame->method->clazz;
    int index = frame->reader.readu2();
    const char *class_name = CP_UTF8(curr_class->cp, index); // 这里解析出来的直接就是数组类。

    auto arr_dim = frame->reader.readu1(); // 多维数组的维度
    size_t arr_lens[arr_dim]; // 每一维数组的长度
    for (int i = arr_dim - 1; i >= 0; i--) {
        int len = frame->popi();
        if (len < 0) {  // todo 等于0的情况
            thread_throw_negative_array_size_exception(len);
        }
        arr_lens[i] = (size_t) len;
    }

    jref arr = ArrayObject::newInst(loadArrayClass(class_name), arr_dim, arr_lens);
    frame->pushr(arr);
}

/*
 * 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
 * 显然基本类型数组肯定都是一维数组，
 * 如果引用类型数组的元素也是数组，那么它就是多维数组。
 */
static void newarray(Frame *frame)
{
    jint arr_len = frame->popi();
    if (arr_len < 0) {
        thread_throw_negative_array_size_exception(arr_len);
    }

    // todo arrLen == 0 的情况

    int arr_type = frame->reader.readu1();
    const char *arr_name;
    switch (arr_type) {
        case 4: arr_name = "[Z"; break;  // AT_BOOLEAN
        case 5: arr_name = "[C"; break;  // AT_CHAR
        case 6: arr_name = "[F"; break;  // AT_FLOAT
        case 7: arr_name = "[D"; break;  // AT_DOUBLE
        case 8: arr_name = "[B"; break;  // AT_BYTE
        case 9: arr_name = "[S"; break;  // AT_SHORT
        case 10: arr_name = "[I"; break; // AT_INT
        case 11: arr_name = "[J"; break; // AT_LONG
        default:
            VM_UNKNOWN_ERROR("error. Invalid array type: %d", arr_type);
            return;
    }

    auto c = loadArrayClass(arr_name);
    frame->pushr(ArrayObject::newInst(c, (size_t) arr_len));
}

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 * 创建一维引用类型数组
 */
static void anewarray(Frame *frame)
{
    jint arr_len = frame->popi();
    if (arr_len < 0) {
        thread_throw_array_index_out_of_bounds_exception(arr_len);
    }

    // todo arrLen == 0 的情况

    int index = frame->reader.readu2();
    ConstantPool &cp = frame->method->clazz->cp;

    const char *class_name;
    u1 type = CP_TYPE(cp, index);
    if (type == CONSTANT_ResolvedClass) {
        auto c = (Class *) CP_INFO(cp, index);
        class_name = c->className;
    } else {
        class_name = CP_CLASS_NAME(cp, index);
    }

    auto ac = frame->method->clazz->loader->loadClass(class_name)->arrayClass();
    frame->pushr(ArrayObject::newInst(ac, (size_t) arr_len));
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
        // bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
        // 常量池在该索引处的值必须是一个CONSTANT_MethodHandle_info结构。
        // 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
        // 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
        u2 bootstrap_method_ref;
        u2 num_bootstrap_arguments;

        // bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
        // 常量池在该索引出必须是下列结构之一：
        // CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
        // CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
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

#if 0
static void set_bootstrap_method_type(Frame *frame)
{
    assert(frame != nullptr);
    frame->Thread->dyn.bootstrap_method_type = frame_stack_popr(Frame);
    int i = 3;
}

static void set_lookup(Frame *frame)
{
    assert(frame != nullptr);
    frame->Thread->dyn.lookup = frame_stack_popr(Frame);
    int i = 3;
}

static void set_call_set0(Frame *frame)
{
    assert(frame != nullptr);
    frame->Thread->dyn.call_set = frame_stack_popr(Frame);
    int i = 3;
}

static void set_exact_method_handle(Frame *frame)
{
    assert(frame != nullptr);
    frame->Thread->dyn.exact_method_handle = frame_stack_popr(Frame);
    int i = 3;
}

static void parse_bootstrap_method_type(
        Class *curr_class, struct Thread *curr_thread, struct invoke_dynamic_ref *ref)
{
// todo 注意有一个这个方法： MethodType.fromMethodDescriptorString 更方便
    struct Object *parameter_types = method_descriptor_to_parameter_types(curr_class->loader, ref->nt->descriptor);
    struct Object *return_type = method_descriptor_to_return_type(curr_class->loader, ref->nt->descriptor);
    Class *c = load_class(g_bootstrap_loader, "java/lang/invoke/MethodType");

    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes)
    if (arrobj_len(parameter_types) > 0) {
        Method *get_method_type = class_lookup_static_method(
                c, "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");

        struct slot args[] = { rslot(return_type), rslot(parameter_types) };
        thread_invoke_method_with_shim(curr_thread, get_method_type, args, set_bootstrap_method_type);
    } else {
        Method *get_method_type = class_lookup_static_method(
                c, "methodType", "(Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
        struct slot args[] = { rslot(return_type) };
        thread_invoke_method_with_shim(curr_thread, get_method_type, args, set_bootstrap_method_type);
    }

    if (!c->inited) { // 后压栈，先执行
        class_clinit(c, curr_thread);
    }
}

static void get_lookup(struct Thread *curr_thread)
{
    Class *c = load_class(g_bootstrap_loader, "java/lang/invoke/MethodHandles");
    Method *m = class_lookup_static_method(c, "lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
    thread_invoke_method_with_shim(curr_thread, m, NULL, set_lookup);

    if (!c->inited) { // 后压栈，先执行
        class_clinit(c, curr_thread);
    }
}

//static struct slot rtc_to_slot(struct ClassLoader *loader, const struct rtcp *rtcp, int index)
//{
//    assert(rtcp != nullptr);
//
//    switch (rtcp->pool[index].t) {
//        case STRING_CONSTANT:
//            return rslot(strobj_create(rtcp_get_str(rtcp, index)));
//        case CLASS_CONSTANT:
//            return rslot(clsobj_create(load_class(loader, rtcp_get_class_name(rtcp, index))));
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

static void set_call_set(Class *curr_class, struct Thread *curr_thread,
                        struct ClassLoader *loader, struct invoke_dynamic_ref *ref)
{

    struct method_ref *mr = ref->handle->ref.mr;

    Class *bootstrap_class = load_class(loader, mr->class_name);
    Method *bootstrap_method = class_lookup_static_method(bootstrap_class, mr->name, mr->descriptor);
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
#endif

// 每一个 invokedynamic 指令都称为 Dynamic Call Site(动态调用点)
void invokedynamic(Frame *frame)
{
    jvm_abort("invokedynamic not implement\n");  // todo

#if 0
    size_t saved_pc = frame->reader.pc - 1;

    Class *curr_class = frame->m.method->clazz;
    struct Thread *curr_thread = frame->Thread;

    // The run-time constant pool item at that index must be a symbolic reference to a call site specifier.
    int index = bcr_readu2(&frame->reader); // CONSTANT_InvokeDynamic_info

    bcr_readu1(&frame->reader); // this byte must always be zero.
    bcr_readu1(&frame->reader); // this byte must always be zero.

    struct invoke_dynamic_ref *ref = rtcp_get_invoke_dynamic(curr_class->rtcp, index);

    struct Object *bootstrap_method_type = curr_thread->dyn.bootstrap_method_type;
    if (bootstrap_method_type == NULL) {
        parse_bootstrap_method_type(curr_class, curr_thread, ref);
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

    struct Object *lookup = curr_thread->dyn.lookup;
    if (lookup == NULL) {
        get_lookup(curr_thread);
        frame->reader.pc = saved_pc; // recover pc
        return;
    }

    struct Object *call_set = curr_thread->dyn.call_set;
    struct Object *exact_method_handle = curr_thread->dyn.exact_method_handle;

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
                Method *exact_method = class_lookup_instance_method(
                        exact_method_handle->clazz, "invokeExact", "[Ljava/lang/Object;");
                // todo args
                jvm_abort(""); ////////////////////////////////////////////////////
                // invoke exact method, invokedynamic completely execute over.
                thread_invoke_method(curr_thread, exact_method, NULL);
                return;
            } else {
                // public abstract MethodHandle dynamicInvoker()
                Method *dynamic_invoker = class_lookup_instance_method(
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

    // todo rest Thread.dyn 的值，下次调用时这个值要从新解析。
#endif
}

#define CASE(x, body) case x: { body; break; }
#define CASE2(x, y, body) case x: case y: { body; break; }
#define CASE3(x, y, z, body) case x: case y: case z: { body; break; }

/*
 * 执行当前线程栈顶的frame
 */
static slot_t *exec()
{
    /*
     * 标识当前指令是否有 wide 扩展。
     * 只对
     * iload, fload, aload, lload, dload,
     * istore, fstore, astore, lstore, dstore,
     * ret, iinc
     * 有效。
     * 以上指令执行前需检查此标志，执行后需复位（置为false）此标志。
     */
    bool wide_extending = false;

#define FETCH_WIDE_INDEX \
    int index; \
    if (wide_extending) { \
        wide_extending = false; /* recover */  \
        index = frame->reader.readu2(); \
    } else { \
        index = frame->reader.readu1(); \
    }

    Thread *thread = thread_self();

    Method *resolved_method;
    slot_t *args;

    Frame *frame = thread->topFrame;
    TRACE("executing frame: %s\n", frame->toString().c_str());
    BytecodeReader *reader = &frame->reader;
    slot_t *stack = frame->stack;
    slot_t *locals = frame->locals;

#define CHANGE_FRAME(newFrame) \
    do { \
        /*frame->stack = stack;  stack指针在变动，需要设置一下 */ \
        frame = newFrame; \
        reader = &frame->reader; \
        stack = frame->stack; \
        locals = frame->locals; \
    } while (false)

    while (true) {
        u1 opcode = reader->readu1();
        TRACE("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc);

        switch (opcode) {
            CASE(OPC_NOP, { })
            CASE(OPC_ACONST_NULL, frame->pushr(nullptr))
            CASE(OPC_ICONST_M1, frame->pushi(-1))
            CASE(OPC_ICONST_0, frame->pushi(0))
            CASE(OPC_ICONST_1, frame->pushi(1))
            CASE(OPC_ICONST_2, frame->pushi(2))
            CASE(OPC_ICONST_3, frame->pushi(3))
            CASE(OPC_ICONST_4, frame->pushi(4))
            CASE(OPC_ICONST_5, frame->pushi(5))

            CASE(OPC_LCONST_0, frame->pushl(0))
            CASE(OPC_LCONST_1, frame->pushl(1))

            CASE(OPC_FCONST_0, frame->pushf(0))
            CASE(OPC_FCONST_1, frame->pushf(1))
            CASE(OPC_FCONST_2, frame->pushf(2))

            CASE(OPC_DCONST_0, frame->pushd(0))
            CASE(OPC_DCONST_1, frame->pushd(1))

            CASE(OPC_BIPUSH, frame->pushi(reader->readu1())) // Byte Integer push
            CASE(OPC_SIPUSH, frame->pushi(reader->readu2())) // Short Integer push

            CASE(OPC_LDC, __ldc(frame, reader->readu1()))
            CASE(OPC_LDC_W, __ldc(frame, reader->readu2()))
            CASE(OPC_LDC2_W, ldc2_w(frame))

            CASE3(OPC_ILOAD, OPC_FLOAD, OPC_ALOAD, {
                FETCH_WIDE_INDEX
                *frame->stack++ = locals[index];
            })

            CASE2(OPC_LLOAD, OPC_DLOAD, {
                FETCH_WIDE_INDEX
                *frame->stack++ = locals[index];
                *frame->stack++ = locals[index + 1];
            })

            CASE3(OPC_ILOAD_0, OPC_FLOAD_0, OPC_ALOAD_0, *frame->stack++ = locals[0])
            CASE3(OPC_ILOAD_1, OPC_FLOAD_1, OPC_ALOAD_1, *frame->stack++ = locals[1])
            CASE3(OPC_ILOAD_2, OPC_FLOAD_2, OPC_ALOAD_2, *frame->stack++ = locals[2])
            CASE3(OPC_ILOAD_3, OPC_FLOAD_3, OPC_ALOAD_3, *frame->stack++ = locals[3])

            CASE2(OPC_LLOAD_0, OPC_DLOAD_0, {
                *frame->stack++ = locals[0];
                *frame->stack++ = locals[1];
            })
            CASE2(OPC_LLOAD_1, OPC_DLOAD_1, {
                *frame->stack++ = locals[1];
                *frame->stack++ = locals[2];
            })
            CASE2(OPC_LLOAD_2, OPC_DLOAD_2, {
                *frame->stack++ = locals[2];
                *frame->stack++ = locals[3];
            })
            CASE2(OPC_LLOAD_3, OPC_DLOAD_3, {
                *frame->stack++ = locals[3];
                *frame->stack++ = locals[4];
            })

#define GET_AND_CHECK_ARRAY \
    jint index = frame->popi(); \
    auto arr = (ArrayObject *) frame->popr(); \
    if ((arr) == nullptr) \
        thread_throw_null_pointer_exception(); \
    if (!arr->checkBounds(index)) \
        thread_throw_array_index_out_of_bounds_exception(index);

#define ARRAY_LOAD_CATEGORY_ONE(type) \
{ \
    GET_AND_CHECK_ARRAY \
    *frame->stack++ = (slot_t) arr->get<type>(index); \
}
            CASE(OPC_IALOAD, ARRAY_LOAD_CATEGORY_ONE(jint))
            CASE(OPC_FALOAD, ARRAY_LOAD_CATEGORY_ONE(jfloat))
            CASE(OPC_AALOAD, ARRAY_LOAD_CATEGORY_ONE(jref))
            CASE(OPC_BALOAD, ARRAY_LOAD_CATEGORY_ONE(jbyte))
            CASE(OPC_CALOAD, ARRAY_LOAD_CATEGORY_ONE(jchar))
            CASE(OPC_SALOAD, ARRAY_LOAD_CATEGORY_ONE(jshort))

            CASE2(OPC_LALOAD, OPC_DALOAD, {
                GET_AND_CHECK_ARRAY
                auto value = (slot_t *) arr->index(index);
                *frame->stack++ = value[0];
                *frame->stack++ = value[1];
            })

            CASE3(OPC_ISTORE, OPC_FSTORE, OPC_ASTORE, {
                FETCH_WIDE_INDEX
                locals[index] = *--frame->stack;
            })

            CASE2(OPC_LSTORE, OPC_DSTORE, {
                FETCH_WIDE_INDEX
                locals[index + 1] = *--frame->stack;
                locals[index] = *--frame->stack;
            })

            CASE3(OPC_ISTORE_0, OPC_FSTORE_0, OPC_ASTORE_0, locals[0] = *--frame->stack)
            CASE3(OPC_ISTORE_1, OPC_FSTORE_1, OPC_ASTORE_1, locals[1] = *--frame->stack)
            CASE3(OPC_ISTORE_2, OPC_FSTORE_2, OPC_ASTORE_2, locals[2] = *--frame->stack)
            CASE3(OPC_ISTORE_3, OPC_FSTORE_3, OPC_ASTORE_3, locals[3] = *--frame->stack)

            CASE2(OPC_LSTORE_0, OPC_DSTORE_0, {
                locals[1] = *--frame->stack;
                locals[0] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_1, OPC_DSTORE_1, {
                locals[2] = *--frame->stack;
                locals[1] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_2, OPC_DSTORE_2, {
                locals[3] = *--frame->stack;
                locals[2] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_3, OPC_DSTORE_3, {
                locals[4] = *--frame->stack;
                locals[3] = *--frame->stack;
            })

#define ARRAY_STORE_CATEGORY_ONE(type) \
{ \
    auto value = (type) *--frame->stack; \
    GET_AND_CHECK_ARRAY \
    arr->set(index, value); \
}
            CASE(OPC_IASTORE, ARRAY_STORE_CATEGORY_ONE(jint))
            CASE(OPC_FASTORE, ARRAY_STORE_CATEGORY_ONE(jfloat))
            CASE(OPC_AASTORE, ARRAY_STORE_CATEGORY_ONE(jref))
            CASE(OPC_BASTORE, ARRAY_STORE_CATEGORY_ONE(jbyte))
            CASE(OPC_CASTORE, ARRAY_STORE_CATEGORY_ONE(jchar))
            CASE(OPC_SASTORE, ARRAY_STORE_CATEGORY_ONE(jshort))

            CASE2(OPC_LASTORE, OPC_DASTORE, {
                frame->stack -= 2;
                slot_t *value = frame->stack;
                GET_AND_CHECK_ARRAY
                memcpy(arr->index(index), value, sizeof(slot_t) * 2);
            })

            CASE(OPC_POP, frame->stack--)
            CASE(OPC_POP2, frame->stack -= 2)
            case OPC_DUP:
                frame->stack[0] = frame->stack[-1];
                frame->stack++;
                break;
            case OPC_DUP_X1:
                frame->stack[0] = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = frame->stack[0];
                frame->stack++;
                break;
            case OPC_DUP_X2:
                frame->stack[0] = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = frame->stack[-3];
                frame->stack[-3] = frame->stack[0];
                frame->stack++;
                break;
            case OPC_DUP2:
                frame->stack[0] = frame->stack[-2];
                frame->stack[1] = frame->stack[-1];
                frame->stack += 2;
                break;
            case OPC_DUP2_X1:
                // ..., value3, value2, value1 →
                // ..., value2, value1, value3, value2, value1
                frame->stack[1] = frame->stack[-1];
                frame->stack[0] = frame->stack[-2];
                frame->stack[-1] = frame->stack[-3];
                frame->stack[-2] = frame->stack[1];
                frame->stack[-3] = frame->stack[0];
                frame->stack += 2;
                break;
            case OPC_DUP2_X2:
                // ..., value4, value3, value2, value1 →
                // ..., value2, value1, value4, value3, value2, value1
                frame->stack[1] = frame->stack[-1];
                frame->stack[0] = frame->stack[-2];
                frame->stack[-1] = frame->stack[-3];
                frame->stack[-2] = frame->stack[-4];
                frame->stack[-3] = frame->stack[1];
                frame->stack[-4] = frame->stack[0];
                frame->stack += 2;
                break;
            case OPC_SWAP:
                swap(frame->stack[-1], frame->stack[-2]);
                break;

#define BINARY_OP(type, n, oper) \
{ \
    frame->stack -= (n);\
    ((type *) frame->stack)[-1] = ((type *) frame->stack)[-1] oper ((type *) frame->stack)[0]; \
}
            CASE(OPC_IADD, BINARY_OP(jint, 1, +))
            CASE(OPC_LADD, BINARY_OP(jlong, 2, +))
            CASE(OPC_FADD, BINARY_OP(jfloat, 1, +))
            CASE(OPC_DADD, BINARY_OP(jdouble, 2, +))

            CASE(OPC_ISUB, BINARY_OP(jint, 1, -))
            CASE(OPC_LSUB, BINARY_OP(jlong, 2, -))
            CASE(OPC_FSUB, BINARY_OP(jfloat, 1, -))
            CASE(OPC_DSUB, BINARY_OP(jdouble, 2, -))

            CASE(OPC_IMUL, BINARY_OP(jint, 1, *))
            CASE(OPC_LMUL, BINARY_OP(jlong, 2, *))
            CASE(OPC_FMUL, BINARY_OP(jfloat, 1, *))
            CASE(OPC_DMUL, BINARY_OP(jdouble, 2, *))

            CASE(OPC_IDIV, BINARY_OP(jint, 1, /))
            CASE(OPC_LDIV, BINARY_OP(jlong, 2, /))
            CASE(OPC_FDIV, BINARY_OP(jfloat, 1, /))
            CASE(OPC_DDIV, BINARY_OP(jdouble, 2, /))

            CASE(OPC_IREM, BINARY_OP(jint, 1, %))
            CASE(OPC_LREM, BINARY_OP(jlong, 2, %))

            case OPC_FREM: {
                jfloat v2 = frame->popf();
                jfloat v1 = frame->popf();
                jvm_abort("not implement\n");
                //    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
                break;
            }

            case OPC_DREM: {
                jdouble v2 = frame->popd();
                jdouble v1 = frame->popd();
                jvm_abort("not implement\n");
                //    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
                break;
            }

            CASE(OPC_INEG, frame->pushi(-frame->popi()))
            CASE(OPC_LNEG, frame->pushl(-frame->popl()))
            CASE(OPC_FNEG, frame->pushf(-frame->popf()))
            CASE(OPC_DNEG, frame->pushd(-frame->popd()))

            case OPC_ISHL: {
                // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
                jint shift = frame->popi() & 0x1f;
                jint value = frame->popi();
                frame->pushi(value << shift);
                break;
            }
            case OPC_LSHL: {
                // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
                jint shift = frame->popi() & 0x3f;
                jlong value = frame->popl();
                frame->pushl(value << shift);
                break;
            }
            case OPC_ISHR: {
                // 逻辑右移 shift logical right
                jint shift = frame->popi() & 0x1f;
                jint value = frame->popi();
                frame->pushi((~(((jint)1) >> shift)) & (value >> shift));
                break;
            }
            case OPC_LSHR: {
                jint shift = frame->popi() & 0x3f;
                jlong value = frame->popl();
                frame->pushl((~(((jlong)1) >> shift)) & (value >> shift));
                break;
            }
            case OPC_IUSHR: {
                // 算术右移 shift arithmetic right
                jint shift = frame->popi() & 0x1f;
                jint value = frame->popi();
                frame->pushi(value >> shift);
                break;
            }
            case OPC_LUSHR: {
                jint shift = frame->popi() & 0x3f;
                jlong value = frame->popl();
                frame->pushl(value >> shift);
                break;
            }

            CASE(OPC_IAND, BINARY_OP(jint, 1, &))
            CASE(OPC_LAND, BINARY_OP(jlong, 2, &))
            CASE(OPC_IOR, BINARY_OP(jint, 1, |))
            CASE(OPC_LOR, BINARY_OP(jlong, 2, |))
            CASE(OPC_IXOR, BINARY_OP(jint, 1, ^))
            CASE(OPC_LXOR, BINARY_OP(jlong, 2, ^))

            case OPC_IINC: {
                jint index, value;

                if (wide_extending) {
                    index = reader->readu2();
                    value = reader->reads2();
                    wide_extending = false;
                } else {
                    index = reader->readu1();
                    value = reader->reads1();
                }

                ISLOT(locals + index) = ISLOT(locals + index) + value;
                break;
            }

            CASE(OPC_I2L, frame->pushl(i2l(frame->popi())))
            CASE(OPC_I2F, frame->pushf(i2f(frame->popi())))
            CASE(OPC_I2D, frame->pushd(i2d(frame->popi())))

            CASE(OPC_L2I, frame->pushi(l2i(frame->popl())))
            CASE(OPC_L2F, frame->pushf(l2f(frame->popl())))
            CASE(OPC_L2D, frame->pushd(l2d(frame->popl())))

            CASE(OPC_F2I, frame->pushi(f2i(frame->popf())))
            CASE(OPC_F2L, frame->pushl(f2l(frame->popf())))
            CASE(OPC_F2D, frame->pushd(f2d(frame->popf())))

            CASE(OPC_D2I, frame->pushi(d2i(frame->popd())))
            CASE(OPC_D2L, frame->pushl(d2l(frame->popd())))
            CASE(OPC_D2F, frame->pushf(d2f(frame->popd())))

            CASE(OPC_I2B, frame->pushi(i2b(frame->popi()))) // todo byte or bool????
            CASE(OPC_I2C, frame->pushi(i2c(frame->popi())))
            CASE(OPC_I2S, frame->pushi(i2s(frame->popi())))

/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define DO_CMP(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

#define CMP(type, t, cmp_result) \
{ \
    type v2 = frame->pop##t(); \
    type v1 = frame->pop##t(); \
    frame->pushi(cmp_result); \
}

            CASE(OPC_LCMP, CMP(jlong, l, DO_CMP(v1, v2, -1)))
            CASE(OPC_FCMPL, CMP(jfloat, f, DO_CMP(v1, v2, -1)))
            CASE(OPC_FCMPG, CMP(jfloat, f, DO_CMP(v1, v2, 1)))
            CASE(OPC_DCMPL, CMP(jdouble, d, DO_CMP(v1, v2, -1)))
            CASE(OPC_DCMPG, CMP(jdouble, d, DO_CMP(v1, v2, 1)))

#define IF_COND(cond) \
{ \
    jint v = frame->popi(); \
    jint offset = reader->reads2(); \
    if (v cond 0) \
        reader->skip(offset - 3);  /* minus instruction length */ \
}
            CASE(OPC_IFEQ, IF_COND(==))
            CASE(OPC_IFNE, IF_COND(!=))
            CASE(OPC_IFLT, IF_COND(<))
            CASE(OPC_IFGE, IF_COND(>=))
            CASE(OPC_IFGT, IF_COND(>))
            CASE(OPC_IFLE, IF_COND(<=))

#define IF_ICMP_COND(cond) \
{ \
    frame->stack -= 2;\
    jint offset = reader->reads2(); \
    if (ISLOT(frame->stack) cond ISLOT(frame->stack + 1)) \
        reader->skip(offset - 3); /* minus instruction length */ \
    break; \
}
            CASE(OPC_IF_ICMPEQ, IF_ICMP_COND(==))
            CASE(OPC_IF_ICMPNE, IF_ICMP_COND(!=))
            CASE(OPC_IF_ICMPLT, IF_ICMP_COND(<))
            CASE(OPC_IF_ICMPGE, IF_ICMP_COND(>=))
            CASE(OPC_IF_ICMPGT, IF_ICMP_COND(>))
            CASE(OPC_IF_ICMPLE, IF_ICMP_COND(<=))

#define IF_ACMP_COND(cond) \
{ \
    frame->stack -= 2;\
    jint offset = reader->reads2(); \
    if (RSLOT(frame->stack) cond RSLOT(frame->stack + 1)) \
        reader->skip(offset - 3);  /* minus instruction length */ \
}
            CASE(OPC_IF_ACMPEQ, IF_ACMP_COND(==))
            CASE(OPC_IF_ACMPNE, IF_ACMP_COND(!=))

            case OPC_GOTO: {
                int offset = reader->reads2();
                reader->skip(offset - 3);  // minus instruction length
                break;
            }

            // 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
            // 从Java 6开始，已经不再使用这些指令
            case OPC_JSR:
                vm_internal_error("jsr doesn't support after jdk 6.");
                break;
            case OPC_RET:
                vm_internal_error("ret doesn't support after jdk 6.");
                break;

            case OPC_TABLESWITCH:
                tableswitch(frame);
                break;
            case OPC_LOOKUPSWITCH:
                lookupswitch(frame);
                break;

#define METHOD_RETURN(ret_value_slot_count) \
{ \
    Frame *invoke_frame = thread->topFrame = frame->prev; \
    frame->stack -= (ret_value_slot_count); \
     \
    if (frame->vm_invoke || invoke_frame == nullptr) { \
        return frame->stack; \
    } else { \
        slot_t *ret_value = frame->stack; \
        for (int i = 0; i < (ret_value_slot_count); i++) { \
            *invoke_frame->stack++ = *ret_value++; \
        } \
        CHANGE_FRAME(invoke_frame); \
        TRACE("executing frame: %s\n", frame->toString().c_str()); \
    } \
}
            CASE3(OPC_IRETURN, OPC_FRETURN, OPC_ARETURN, METHOD_RETURN(1))
            CASE2(OPC_LRETURN, OPC_DRETURN, METHOD_RETURN(2))
            CASE(OPC_RETURN, METHOD_RETURN(0))

            case OPC_GETSTATIC: {
                int index = reader->readu2();
                Field *f = resolve_field(frame->method->clazz, index);

                if (!f->clazz->inited) {
                    f->clazz->clinit();
                }

                const slot_t *value = f->clazz->getStaticFieldValue(f);
                *frame->stack++ = value[0];
                if (f->categoryTwo) {
                    *frame->stack++ = value[1];
                }
                break;
            }

            case OPC_PUTSTATIC: {
                int index = reader->readu2();
                Field *f = resolve_field(frame->method->clazz, index);

                if (!f->clazz->inited) {
                    f->clazz->clinit();
                }

                if (f->categoryTwo) {
                    frame->stack -= 2;
                } else {
                    frame->stack--;
                }

                f->clazz->setStaticFieldValue(f, frame->stack);
                break;
            }

            case OPC_GETFIELD: {
                int index = reader->readu2();
                Field *f = resolve_field(frame->method->clazz, index);
                jref obj = frame->popr();
                if (obj == nullptr) {
                    thread_throw_null_pointer_exception();
                }

//                const slot_t *value = obj->getInstFieldValue(f);
//                *frame->stack++ = value[0];
//                if (f->categoryTwo) {
//                    *frame->stack++ = value[1];
//                }
                obj->storeInstFieldValue(f, frame->stack);
                break;
            }

            case OPC_PUTFIELD: {
                int index = reader->readu2();
                Field *f = resolve_field(frame->method->clazz, index);

                // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
                if (f->isFinal()) {
                    // todo
                    if (frame->method->clazz != f->clazz || !utf8_equals(frame->method->name, S(object_init))) {
                        jvm_abort("java.lang.IllegalAccessError\n"); // todo
                    }
                }

                if (f->categoryTwo) {
                    frame->stack -= 2;
                } else {
                    frame->stack--;
                }
                slot_t *value = frame->stack;

                jref obj = frame->popr();
                if (obj == nullptr) {
                    thread_throw_null_pointer_exception();
                }

                obj->setFieldValue(f, value);
                break;
            }

            case OPC_INVOKEVIRTUAL: {
                // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
                int index = reader->readu2();
                Method *m = resolve_method(frame->method->clazz, index);

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                auto obj = (Object *) args[0];
                if (obj == nullptr) {
                    thread_throw_null_pointer_exception();
                }

                assert(m->vtableIndex >= 0);
                assert(m->vtableIndex < obj->clazz->vtable.size());
                resolved_method = obj->clazz->vtable[m->vtableIndex];
                assert(resolved_method == obj->clazz->lookupMethod(m->name, m->descriptor));
                goto invoke_method;
            }

            case OPC_INVOKESPECIAL: {
                // invokespecial指令用于调用一些需要特殊处理的实例方法，
                // 包括构造函数、私有方法和通过super关键字调用的超类方法。
                Class *curr_class = frame->method->clazz;
                int index = reader->readu2();
//
//    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
////    if (method->name == "<init>" && method->class != c) {
////        // todo java.lang.NoSuchMethodError
////        jvm_abort("java.lang.NoSuchMethodError\n");
////    }

                Method *m = resolve_method(curr_class, index);
                /*
                 * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
                 * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
                 * todo 详细说明
                 */
                if (m->clazz->isSuper()
                    && !m->isPrivate()
                    && curr_class->isSubclassOf(m->clazz) // todo
                    && !utf8_equals(m->name, S(object_init))) {
                    m = curr_class->superClass->lookupMethod(m->name, m->descriptor);
                }

                if (m->isAbstract()) {
                    // todo java.lang.AbstractMethodError
                    jvm_abort("java.lang.AbstractMethodError\n");
                }
                if (m->isStatic()) {
                    // todo java.lang.IncompatibleClassChangeError
                    jvm_abort("java.lang.IncompatibleClassChangeError\n");
                }

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                auto obj = (jref) args[0];
                if (obj == nullptr) {
                    thread_throw_null_pointer_exception();
                }

                resolved_method = m;
                goto invoke_method;
            }
            case OPC_INVOKESTATIC: {
                // invokestatic指令用来调用静态方法。
                // 如果类还没有被初始化，会触发类的初始化。
                Class *curr_class = frame->method->clazz;
                int index = reader->readu2();
                Method *m = resolve_method(curr_class, index);
                if (m->isAbstract()) {
                    // todo java.lang.AbstractMethodError
                    jvm_abort("java.lang.AbstractMethodError\n");
                }
                if (!m->isStatic()) {
                    // todo java.lang.IncompatibleClassChangeError
                    jvm_abort("java.lang.IncompatibleClassChangeError\n");
                }

                if (!m->clazz->inited) {
                    m->clazz->clinit();
                }

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                resolved_method = m;
                goto invoke_method;
            }
            case OPC_INVOKEINTERFACE: {
                Class *curr_class = frame->method->clazz;
                int index = reader->readu2();

                /*
                 * 此字节的值是给方法传递参数需要的slot数，
                 * 其含义和给method结构体定义的arg_slot_count字段相同。
                 * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
                 */
                reader->readu1();
                /*
                 * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
                 * 该字节的存在是为了保证Java虚拟机可以向后兼容。
                 */
                reader->readu1();

                Method *m = resolve_method(curr_class, index);

                /* todo 本地方法 */

                frame->stack-= m->arg_slot_count;
                args = frame->stack;

                auto obj = (jref) args[0];
                if (obj == nullptr) {
                    thread_throw_null_pointer_exception();
                }

                Method *method = obj->clazz->lookupMethod(m->name, m->descriptor);
                if (method == nullptr) {
                    jvm_abort("error\n"); // todo
                }

                if (method->isAbstract()) {
                    // todo java.lang.AbstractMethodError
                    jvm_abort("java.lang.AbstractMethodError\n");
                }
                if (!method->isPublic()) {
                    // todo java.lang.IllegalAccessError
                    jvm_abort("java.lang.IllegalAccessError\n");
                }

                resolved_method = method;
                goto invoke_method;
            }
            case OPC_INVOKEDYNAMIC:
                invokedynamic(frame);
                break;
invoke_method:
{
    assert(resolved_method);
    Frame *new_frame = allocFrame(resolved_method, false);
    if (resolved_method->arg_slot_count > 0 && args == nullptr) {
        jvm_abort("do not find args, %d\n", resolved_method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < resolved_method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        new_frame->locals[i] = args[i];
    }

    CHANGE_FRAME(new_frame);
    /*printvm("executing frame: %s\n", frame->toString().c_str());*/
    break;
}

            case OPC_NEW: {
                // new指令专门用来创建类实例。数组由专门的指令创建
                // 如果类还没有被初始化，会触发类的初始化。
                Class *c = resolve_class(frame->method->clazz, reader->readu2());  // todo
                if (!c->inited) {
                    c->clinit();
                }

                if (c->isInterface() || c->isAbstract()) {
                    jvm_abort("java.lang.InstantiationError\n");  // todo 抛出 InstantiationError 异常
                }

                // todo java/lang/Class 会在这里创建，为什么会这样，怎么处理
                //    assert(strcmp(c->class_name, "java/lang/Class") == 0);

                frame->pushr(Object::newInst(c));
				break;
            }

            CASE(OPC_NEWARRAY, newarray(frame))
            CASE(OPC_ANEWARRAY, anewarray(frame))

            case OPC_ARRAYLENGTH: {
                Object *o = frame->popr();
                if (o == nullptr) {
                    thread_throw_null_pointer_exception();
                }
                if (!o->isArray()) {
                    vm_unknown_error("not a array"); // todo
                }
                frame->pushi(((ArrayObject *) o)->len);
				break;
            }
            case OPC_ATHROW: {
                jref exception = frame->popr();
                if (exception == nullptr) {
                    thread_throw_null_pointer_exception();
                }

                // 遍历虚拟机栈找到可以处理此异常的方法
                while (true) {
                    int handler_pc = frame->method->findExceptionHandler(exception->clazz, reader->pc - 1); // instruction length todo 好像是错的
                    if (handler_pc >= 0) {  // todo 可以等于0吗
                        /*
                         * 找到可以处理的函数了
                         * 操作数栈清空
                         * 把异常对象引用推入栈顶
                         * 跳转到异常处理代码之前
                         */
//                frame_stack_clear(top);  // todo
//                frame_stack_pushr(top, exception);
                        frame->pushr(exception);
                        reader->pc = (size_t) handler_pc;
                        break;  // todo
                    }

                    // frame 无法处理异常，弹出
                    popFrame();

                    if (frame->prev == nullptr) {
                        break; // todo 说明下
                    }
                    CHANGE_FRAME(frame->prev);
                    /*printvm("executing frame: %s\n", frame->toString().c_str());*/
                }

                thread_handle_uncaught_exception(exception);
                return nullptr; // todo
            }
            case OPC_CHECKCAST: {
                jref obj = RSLOT(frame->stack - 1); // 不改变操作数栈
                int index = reader->readu2();

                // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
                if (obj != nullptr) {
                    Class *c = resolve_class(frame->method->clazz, index);
                    if (!obj->isInstanceOf(c)) {
                        thread_throw_class_cast_exception(obj->clazz->className, c->className);
                    }
                }
                break;
            }
            case OPC_INSTANCEOF: {
                int index = reader->readu2();
                Class *c = resolve_class(frame->method->clazz, index);

                jref obj = frame->popr();
                if (obj == nullptr)
                    frame->pushi(0);
                else
                    frame->pushi(obj->isInstanceOf(c) ? 1 : 0);
                break;
            }
            case OPC_MONITORENTER: {
                jref o = frame->popr();
                // todo
                break;
            }
            case OPC_MONITOREXIT: {
                jref o = frame->popr();
                // todo
                break;
            }

            CASE(OPC_WIDE, wide_extending = true)
            CASE(OPC_MULTIANEWARRAY, multianewarray(frame))

            case OPC_IFNULL: {
                int offset = reader->reads2();
                if (frame->popr() == nullptr) {
                    reader->skip(offset - 3); // minus instruction length
                }
                break;
            }
            case OPC_IFNONNULL: {
                int offset = reader->reads2();
                if (frame->popr() != nullptr) {
                    reader->skip(offset - 3); // minus instruction length
                }
                break;
            }

            case OPC_GOTO_W: // todo
                vm_internal_error("goto_w doesn't support");
                break;
            case OPC_JSR_W: // todo
                vm_internal_error("jsr_w doesn't support after jdk 6.");
                break;
            case OPC_INVOKENATIVE:
                frame->method->nativeMethod(frame);
                break;
            default:
                jvm_abort("This instruction isn't used. %d(0x%x)\n", opcode, opcode); // todo
        }
    }

    jvm_abort("Never goes here!");
}

slot_t *execJavaFunc(Method *method, const slot_t *args)
{
    assert(method != nullptr);
    Frame *frame = allocFrame(method, true);
    if (method->arg_slot_count > 0 && args == nullptr) {
        jvm_abort("do not find args, %d\n", method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame->locals[i] = args[i];
    }

    return exec();
}

slot_t *execJavaFunc(Method *method, initializer_list<slot_t> args)
{
    assert(method != nullptr);
    assert(method->arg_slot_count == args.size());

    Frame *frame = allocFrame(method, true);

    // 准备参数
    int i = 0;
    for (auto iter = args.begin(); i < method->arg_slot_count && iter != args.end(); i++, iter++) {
        // 传递参数到被调用的函数。
        frame->locals[i] = *iter;
    }

    return exec();
}
