/*
 * Author: Jia Yang
 */

#include "../jvm.h"
#include "../rtda/thread/thread.h"
#include "../rtda/thread/frame.h"
#include "../rtda/heap/strpool.h"
#include "../classfile/constant.h"
#include "../rtda/heap/arrobj.h"
#include "../rtda/ma/field.h"
#include "../rtda/ma/resolve.h"
#include "interpreter.h"
#include "../utf8.h"
#include "../symbol.h"

#if (PRINT_LEVEL >= 3)
// the mapping of instructions's code and name
static char* instruction_names[] = {
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
        [0xcb ... 0xfd] = "notused",
        "impdep1", "impdep2",
};
#endif

// constant instructions -----------------------------------------------------------------------------------------------
static void __ldc(struct frame *frame, int index)
{
    struct constant_pool *cp = &frame->method->clazz->constant_pool;
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Integer) {
        frame_stack_pushi(frame, CP_INT(cp, index));
    } else if (type == CONSTANT_Float) {
        frame_stack_pushf(frame, CP_FLOAT(cp, index));
    } else if (type == CONSTANT_String) {
        frame_stack_pushr(frame, resolve_string(frame->method->clazz, index));
    } else if (type == CONSTANT_ResolvedString) {
        frame_stack_pushr(frame, (jref) CP_INFO(cp, index));
    } else if (type == CONSTANT_Class) {
        frame_stack_pushr(frame, resolve_class(frame->method->clazz, index)->clsobj);
    } else if (type == CONSTANT_ResolvedClass) {
        struct class *c = (struct class *) CP_INFO(cp, index);
        frame_stack_pushr(frame, c->clsobj);
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

void ldc2_w(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    struct constant_pool *cp = &frame->method->clazz->constant_pool;
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Long) {
        frame_stack_pushl(frame, CP_LONG(cp, index));
    } else if (type == CONSTANT_Double) {
        frame_stack_pushd(frame, CP_DOUBLE(cp, index));
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void tableswitch(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1; // save the pc before 'tableswitch' instruction
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // low 和 height 标识了 case 的取值范围。
    s4 low = bcr_reads4(reader);
    s4 height = bcr_reads4(reader);

    // 跳转偏移量表，对应于各个 case 的情况
    s4 jump_offset_count = height - low + 1;
    s4 jump_offsets[jump_offset_count];
    bcr_reads4s(reader, jump_offset_count, jump_offsets);

    // 弹出要判断的值
    jint index = frame_stack_popi(frame);
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
    reader->pc = saved_pc + offset;
}

/*
 * todo 指令说明  好像是实现 switch 语句
 */
static void lookupswitch(struct frame *frame)
{
    struct bytecode_reader *reader = &frame->reader;
    size_t saved_pc = reader->pc - 1; // save the pc before 'lookupswitch' instruction
    bcr_align4(reader);

    // 默认情况下执行跳转所需字节码的偏移量
    // 对应于 switch 中的 default 分支。
    s4 default_offset = bcr_reads4(reader);

    // case的个数
    s4 npairs = bcr_reads4(reader);
    assert(npairs >= 0); // The npairs must be greater than or equal to 0.

    // match_offsets 有点像 Map，它的 key 是 case 值，value 是跳转偏移量。
    s4 match_offsets[npairs * 2];
    bcr_reads4s(reader, npairs * 2, match_offsets);

    // 弹出要判断的值
    jint key = frame_stack_popi(frame);
    s4 offset = default_offset;
    for (int i = 0; i < npairs * 2; i += 2) {
        if (match_offsets[i] == key) { // 找到 case
            offset = match_offsets[i + 1];
            break;
        }
    }

    // The target address is calculated by adding the corresponding offset
    // to the address of the opcode of this lookupswitch instruction.
    reader->pc = saved_pc + offset;
}


// extended instructions -----------------------------------------------------------------------------------------------
/*
 * 创建多维数组
 * todo 注意这种情况，基本类型的多维数组 int[][][]
 */
static void multianewarray(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;
    int index = bcr_readu2(&frame->reader);
    const char *class_name = CP_UTF8(&curr_class->constant_pool, index); // 这里解析出来的直接就是数组类。

    int arr_dim = bcr_readu1(&frame->reader); // 多维数组的维度
    size_t arr_lens[arr_dim]; // 每一维数组的长度
    for (int i = arr_dim - 1; i >= 0; i--) {
        int len = frame_stack_popi(frame);
        if (len < 0) {  // todo 等于0的情况
            thread_throw_negative_array_size_exception(len);
        }
        arr_lens[i] = (size_t) len;
    }

    struct class *arr_class = load_class(curr_class->loader, class_name);
    jref arr = arrobj_create_multi(arr_class, arr_dim, arr_lens);
    frame_stack_pushr(frame, arr);
}

/*
 * 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
 * 显然基本类型数组肯定都是一维数组，
 * 如果引用类型数组的元素也是数组，那么它就是多维数组。
 */
static void newarray(struct frame *frame)
{
    jint arr_len = frame_stack_popi(frame);
    if (arr_len < 0) {
        thread_throw_negative_array_size_exception(arr_len);
    }

    // todo arrLen == 0 的情况

    int arr_type = bcr_readu1(&frame->reader);
    char *arr_name;
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

    struct class *c = load_class(frame->method->clazz->loader, arr_name);
    frame_stack_pushr(frame, arrobj_create(c, (size_t) arr_len));
}

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 * 创建一维引用类型数组
 */
static void anewarray(struct frame *frame)
{
    jint arr_len = frame_stack_popi(frame);
    if (arr_len < 0) {
        thread_throw_array_index_out_of_bounds_exception(arr_len);
    }

    // todo arrLen == 0 的情况

    int index = bcr_readu2(&frame->reader);
    struct constant_pool *cp = &frame->method->clazz->constant_pool;

    const char *class_name;
    u1 type = CP_TYPE(cp, index);
    if (type == CONSTANT_ResolvedClass) {
        struct class *c = (struct class *) CP_INFO(cp, index);
        class_name = c->class_name;
    } else {
        class_name = CP_CLASS_NAME(cp, index);
    }

    char *arr_class_name = get_arr_class_name(class_name);
    struct class *arr_class = load_class(frame->method->clazz->loader, arr_class_name);
    free(arr_class_name);
    frame_stack_pushr(frame, arrobj_create(arr_class, (size_t) arr_len));
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
    struct class *c = load_class(g_bootstrap_loader, "java/lang/invoke/MethodType");

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
    struct class *c = load_class(g_bootstrap_loader, "java/lang/invoke/MethodHandles");
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

static void set_call_set(struct class *curr_class, struct thread *curr_thread,
                        struct classloader *loader, struct invoke_dynamic_ref *ref)
{

    struct method_ref *mr = ref->handle->ref.mr;

    struct class *bootstrap_class = load_class(loader, mr->class_name);
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
#endif

// 每一个 invokedynamic 指令都称为 Dynamic Call Site(动态调用点)
void invokedynamic(struct frame *frame)
{
    jvm_abort("invokedynamic not implement\n");  // todo

#if 0
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
#endif
}

#define CASE(x, body) case x: { body; break; }
#define CASE2(x, y, body) case x: case y: { body; break; }
#define CASE3(x, y, z, body) case x: case y: case z: { body; break; }

static slot_t * exec()
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
        index = bcr_readu2(&frame->reader); \
    } else { \
        index = bcr_readu1(&frame->reader); \
    }

    struct thread *thread = thread_self();

    struct method *resolved_method;
    slot_t *args;

    struct frame *frame = thread->top_frame;
    print2("executing frame(%p): %s, pc = %lu\n", frame, frame_to_string(frame), frame->reader.pc);

    while (true) {
        u1 opcode = frame_readu1(frame);
        print3("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc);

        switch (opcode) {
            CASE(OPC_NOP, { })
            CASE(OPC_ACONST_NULL, frame_stack_pushr(frame, NULL))
            CASE(OPC_ICONST_M1, frame_stack_pushi(frame, -1))
            CASE(OPC_ICONST_0, frame_stack_pushi(frame, 0))
            CASE(OPC_ICONST_1, frame_stack_pushi(frame, 1))
            CASE(OPC_ICONST_2, frame_stack_pushi(frame, 2))
            CASE(OPC_ICONST_3, frame_stack_pushi(frame, 3))
            CASE(OPC_ICONST_4, frame_stack_pushi(frame, 4))
            CASE(OPC_ICONST_5, frame_stack_pushi(frame, 5))

            CASE(OPC_LCONST_0, frame_stack_pushl(frame, 0))
            CASE(OPC_LCONST_1, frame_stack_pushl(frame, 1))

            CASE(OPC_FCONST_0, frame_stack_pushf(frame, 0))
            CASE(OPC_FCONST_1, frame_stack_pushf(frame, 1))
            CASE(OPC_FCONST_2, frame_stack_pushf(frame, 2))

            CASE(OPC_DCONST_0, frame_stack_pushd(frame, 0))
            CASE(OPC_DCONST_1, frame_stack_pushd(frame, 1))

            CASE(OPC_BIPUSH, frame_stack_pushi(frame, frame_readu1(frame))) // Byte Integer push
            CASE(OPC_SIPUSH, frame_stack_pushi(frame, frame_readu2(frame))) // Short Integer push

            CASE(OPC_LDC, __ldc(frame, frame_readu1(frame)))
            CASE(OPC_LDC_W, __ldc(frame, frame_readu2(frame)))
            CASE(OPC_LDC2_W, ldc2_w(frame))

            CASE3(OPC_ILOAD, OPC_FLOAD, OPC_ALOAD, {
                FETCH_WIDE_INDEX
                *frame->stack++ = frame->locals[index];
            })

            CASE2(OPC_LLOAD, OPC_DLOAD, {
                FETCH_WIDE_INDEX
                *frame->stack++ = frame->locals[index];
                *frame->stack++ = frame->locals[index + 1];
            })

            CASE3(OPC_ILOAD_0, OPC_FLOAD_0, OPC_ALOAD_0, *frame->stack++ = frame->locals[0])
            CASE3(OPC_ILOAD_1, OPC_FLOAD_1, OPC_ALOAD_1, *frame->stack++ = frame->locals[1])
            CASE3(OPC_ILOAD_2, OPC_FLOAD_2, OPC_ALOAD_2, *frame->stack++ = frame->locals[2])
            CASE3(OPC_ILOAD_3, OPC_FLOAD_3, OPC_ALOAD_3, *frame->stack++ = frame->locals[3])

            CASE2(OPC_LLOAD_0, OPC_DLOAD_0, {
                *frame->stack++ = frame->locals[0];
                *frame->stack++ = frame->locals[1];
            })
            CASE2(OPC_LLOAD_1, OPC_DLOAD_1, {
                *frame->stack++ = frame->locals[1];
                *frame->stack++ = frame->locals[2];
            })
            CASE2(OPC_LLOAD_2, OPC_DLOAD_2, {
                *frame->stack++ = frame->locals[2];
                *frame->stack++ = frame->locals[3];
            })
            CASE2(OPC_LLOAD_3, OPC_DLOAD_3, {
                *frame->stack++ = frame->locals[3];
                *frame->stack++ = frame->locals[4];
            })

#define GET_AND_CHECK_ARRAY \
    jint index = frame_stack_popi(frame); \
    jref arr = frame_stack_popr(frame); \
    if ((arr) == NULL) \
        thread_throw_null_pointer_exception(); \
    if (!arrobj_check_bounds(arr, index)) \
        thread_throw_array_index_out_of_bounds_exception(index);

#define ARRAY_LOAD_CATEGORY_ONE(type) \
{ \
    GET_AND_CHECK_ARRAY \
    *frame->stack++ = (slot_t) arrobj_get(type, arr, index); \
}
            CASE(OPC_IALOAD, ARRAY_LOAD_CATEGORY_ONE(jint))
            CASE(OPC_FALOAD, ARRAY_LOAD_CATEGORY_ONE(jfloat))
            CASE(OPC_AALOAD, ARRAY_LOAD_CATEGORY_ONE(jref))
            CASE(OPC_BALOAD, ARRAY_LOAD_CATEGORY_ONE(jbyte))
            CASE(OPC_CALOAD, ARRAY_LOAD_CATEGORY_ONE(jchar))
            CASE(OPC_SALOAD, ARRAY_LOAD_CATEGORY_ONE(jshort))

            CASE2(OPC_LALOAD, OPC_DALOAD, {
                GET_AND_CHECK_ARRAY
                slot_t *value = (slot_t *) arrobj_index(arr, index);
                *frame->stack++ = value[0];
                *frame->stack++ = value[1];
            })

            CASE3(OPC_ISTORE, OPC_FSTORE, OPC_ASTORE, {
                FETCH_WIDE_INDEX
                frame->locals[index] = *--frame->stack;
            })

            CASE2(OPC_LSTORE, OPC_DSTORE, {
                FETCH_WIDE_INDEX
                frame->locals[index + 1] = *--frame->stack;
                frame->locals[index] = *--frame->stack;
            })

            CASE3(OPC_ISTORE_0, OPC_FSTORE_0, OPC_ASTORE_0, frame->locals[0] = *--frame->stack)
            CASE3(OPC_ISTORE_1, OPC_FSTORE_1, OPC_ASTORE_1, frame->locals[1] = *--frame->stack)
            CASE3(OPC_ISTORE_2, OPC_FSTORE_2, OPC_ASTORE_2, frame->locals[2] = *--frame->stack)
            CASE3(OPC_ISTORE_3, OPC_FSTORE_3, OPC_ASTORE_3, frame->locals[3] = *--frame->stack)

            CASE2(OPC_LSTORE_0, OPC_DSTORE_0, {
                frame->locals[1] = *--frame->stack;
                frame->locals[0] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_1, OPC_DSTORE_1, {
                frame->locals[2] = *--frame->stack;
                frame->locals[1] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_2, OPC_DSTORE_2, {
                frame->locals[3] = *--frame->stack;
                frame->locals[2] = *--frame->stack;
            })
            CASE2(OPC_LSTORE_3, OPC_DSTORE_3, {
                frame->locals[4] = *--frame->stack;
                frame->locals[3] = *--frame->stack;
            })

#define ARRAY_STORE_CATEGORY_ONE(type) \
{ \
    type value = (type) *--frame->stack; \
    GET_AND_CHECK_ARRAY \
    arrobj_set(type, arr, index, value); \
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
                        memcpy(arrobj_index(arr, index), value, sizeof(slot_t) * 2);
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
            case OPC_SWAP: {
                slot_t tmp = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = tmp;
                break;
            }

#define BINARY_OP(type, oper) \
{ \
    frame->stack -= SLOTS(type);\
    ((type *) frame->stack)[-1] = ((type *) frame->stack)[-1] oper ((type *) frame->stack)[0]; \
}
            CASE(OPC_IADD, BINARY_OP(jint, +))
            CASE(OPC_LADD, BINARY_OP(jlong, +))
            CASE(OPC_FADD, BINARY_OP(jfloat, +))
            CASE(OPC_DADD, BINARY_OP(jdouble, +))

            CASE(OPC_ISUB, BINARY_OP(jint, -))
            CASE(OPC_LSUB, BINARY_OP(jlong, -))
            CASE(OPC_FSUB, BINARY_OP(jfloat, -))
            CASE(OPC_DSUB, BINARY_OP(jdouble, -))

            CASE(OPC_IMUL, BINARY_OP(jint, *))
            CASE(OPC_LMUL, BINARY_OP(jlong, *))
            CASE(OPC_FMUL, BINARY_OP(jfloat, *))
            CASE(OPC_DMUL, BINARY_OP(jdouble, *))

            CASE(OPC_IDIV, BINARY_OP(jint, /))
            CASE(OPC_LDIV, BINARY_OP(jlong, /))
            CASE(OPC_FDIV, BINARY_OP(jfloat, /))
            CASE(OPC_DDIV, BINARY_OP(jdouble, /))

            CASE(OPC_IREM, BINARY_OP(jint, %))
            CASE(OPC_LREM, BINARY_OP(jlong, %))

            CASE(OPC_FREM, {
                jfloat v2 = frame_stack_popf(frame);
                jfloat v1 = frame_stack_popf(frame);
                jvm_abort("not implement\n");
                //    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
            })

            CASE(OPC_DREM, {
                jdouble v2 = frame_stack_popd(frame);
                jdouble v1 = frame_stack_popd(frame);
                jvm_abort("未实现\n");
                //    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
            })

            CASE(OPC_INEG, frame_stack_pushi(frame, -frame_stack_popi(frame)))
            CASE(OPC_LNEG, frame_stack_pushl(frame, -frame_stack_popl(frame)))
            CASE(OPC_FNEG, frame_stack_pushf(frame, -frame_stack_popf(frame)))
            CASE(OPC_DNEG, frame_stack_pushd(frame, -frame_stack_popd(frame)))

            CASE(OPC_ISHL, {
                // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
                jint shift = frame_stack_popi(frame) & 0x1f;
                jint value = frame_stack_popi(frame);
                frame_stack_pushi(frame, value << shift);
            })
            CASE(OPC_LSHL, {
                // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
                jint shift = frame_stack_popi(frame) & 0x3f;
                jlong value = frame_stack_popl(frame);
                frame_stack_pushl(frame, value << shift);
            })
            CASE(OPC_ISHR, {
                // 逻辑右移 shift logical right
                jint shift = frame_stack_popi(frame) & 0x1f;
                jint value = frame_stack_popi(frame);
                frame_stack_pushi(frame, (~(((jint)1) >> shift)) & (value >> shift));
            })
            CASE(OPC_LSHR, {
                jint shift = frame_stack_popi(frame) & 0x3f;
                jlong value = frame_stack_popl(frame);
                frame_stack_pushl(frame, (~(((jlong)1) >> shift)) & (value >> shift));
            })
            CASE(OPC_IUSHR, {
                // 算术右移 shift arithmetic right
                jint shift = frame_stack_popi(frame) & 0x1f;
                jint value = frame_stack_popi(frame);
                frame_stack_pushi(frame, value >> shift);
            })
            CASE(OPC_LUSHR, {
                jint shift = frame_stack_popi(frame) & 0x3f;
                jlong value = frame_stack_popl(frame);
                frame_stack_pushl(frame, value >> shift);
            })

            CASE(OPC_IAND, BINARY_OP(jint, &))
            CASE(OPC_LAND, BINARY_OP(jlong, &))
            CASE(OPC_IOR, BINARY_OP(jint, |))
            CASE(OPC_LOR, BINARY_OP(jlong, |))
            CASE(OPC_IXOR, BINARY_OP(jint, ^))
            CASE(OPC_LXOR, BINARY_OP(jlong, ^))

            case OPC_IINC: {
                jint index, value;

                if (wide_extending) {
                    index = bcr_readu2(&frame->reader);
                    value = bcr_reads2(&frame->reader);
                    wide_extending = false;
                } else {
                    index = bcr_readu1(&frame->reader);
                    value = bcr_reads1(&frame->reader);
                }

                ISLOT(frame->locals + index) = ISLOT(frame->locals + index) + value;
                break;
            }

#define x2y(x, y) frame_stack_push##y(frame, x##2##y(frame_stack_pop##x(frame)))
            CASE(OPC_I2L, x2y(i, l))
            CASE(OPC_I2F, x2y(i, f))
            CASE(OPC_I2D, x2y(i, d))

            CASE(OPC_L2I, x2y(l, i))
            CASE(OPC_L2F, x2y(l, f))
            CASE(OPC_L2D, x2y(l, d))

            CASE(OPC_F2I, x2y(f, i))
            CASE(OPC_F2L, x2y(f, l))
            CASE(OPC_F2D, x2y(f, d))

            CASE(OPC_D2I, x2y(d, i))
            CASE(OPC_D2L, x2y(d, l))
            CASE(OPC_D2F, x2y(d, f))

            CASE(OPC_I2B, frame_stack_pushi(frame, i2b(frame_stack_popi(frame))))
            CASE(OPC_I2C, frame_stack_pushi(frame, i2c(frame_stack_popi(frame))))
            CASE(OPC_I2S, frame_stack_pushi(frame, i2s(frame_stack_popi(frame))))

/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define DO_CMP(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

#define CMP(type, t, cmp_result) \
{ \
    type v2 = frame_stack_pop##t(frame); \
    type v1 = frame_stack_pop##t(frame); \
    frame_stack_pushi(frame, cmp_result); \
}

            CASE(OPC_LCMP, CMP(jlong, l, DO_CMP(v1, v2, -1)))
            CASE(OPC_FCMPL, CMP(jfloat, f, DO_CMP(v1, v2, -1)))
            CASE(OPC_FCMPG, CMP(jfloat, f, DO_CMP(v1, v2, 1)))
            CASE(OPC_DCMPL, CMP(jdouble, d, DO_CMP(v1, v2, -1)))
            CASE(OPC_DCMPG, CMP(jdouble, d, DO_CMP(v1, v2, 1)))

#define IF_COND(cond) \
{ \
    jint v = frame_stack_popi(frame); \
    jint offset = frame_reads2(frame); \
    if (v cond 0) \
        frame_skip(frame, offset - 3);  /* minus instruction length */ \
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
    jint offset = frame_reads2(frame); \
    if (ISLOT(frame->stack) cond ISLOT(frame->stack + 1)) \
        frame_skip(frame, offset - 3); /* minus instruction length */ \
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
    jint offset = frame_reads2(frame); \
    if (RSLOT(frame->stack) cond RSLOT(frame->stack + 1)) \
        frame_skip(frame, offset - 3);  /* minus instruction length */ \
}
            CASE(OPC_IF_ACMPEQ, IF_ACMP_COND(==))
            CASE(OPC_IF_ACMPNE, IF_ACMP_COND(!=))

            CASE(OPC_GOTO, {
                int offset = bcr_reads2(&frame->reader);
                bcr_skip(&frame->reader, offset - 3);  // minus instruction length
            })

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
    struct frame *invoke_frame = thread->top_frame = frame->prev; \
    frame->stack -= (ret_value_slot_count); \
     \
    if (frame->vm_invoke || invoke_frame == NULL) { \
        return frame->stack; \
    } else { \
        slot_t *ret_value = frame->stack; \
        for (int i = 0; i < (ret_value_slot_count); i++) { \
            *invoke_frame->stack++ = *ret_value++; \
        } \
        frame = invoke_frame; \
    } \
}
            CASE3(OPC_IRETURN, OPC_FRETURN, OPC_ARETURN, METHOD_RETURN(1))
            CASE2(OPC_LRETURN, OPC_DRETURN, METHOD_RETURN(2))
            CASE(OPC_RETURN, METHOD_RETURN(0))

            CASE(OPC_GETSTATIC, {
                struct bytecode_reader *reader = &frame->reader;
                int index = bcr_readu2(reader);
                struct field *f = resolve_field(frame->method->clazz, index);

                if (!f->clazz->inited) {
                    class_clinit(f->clazz);
                }

                const slot_t *value = get_static_field_value(f->clazz, f);
                *frame->stack++ = value[0];
                if (f->category_two) {
                    *frame->stack++ = value[1];
                }
            })

            CASE(OPC_PUTSTATIC, {
                struct bytecode_reader *reader = &frame->reader;
                int index = bcr_readu2(reader);
                struct field *f = resolve_field(frame->method->clazz, index);

                if (!f->clazz->inited) {
                    class_clinit(f->clazz);
                }

                if (f->category_two) {
                    frame->stack -= 2;
                } else {
                    frame->stack--;
                }

                set_static_field_value(f->clazz, f, frame->stack);
            })

            CASE(OPC_GETFIELD, {
                int index = bcr_readu2(&frame->reader);
                struct field *f = resolve_field(frame->method->clazz, index);

                jref obj = frame_stack_popr(frame);
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                const slot_t *value = get_instance_field_value(obj, f);
                *frame->stack++ = value[0];
                if (f->category_two) {
                    *frame->stack++ = value[1];
                }
            })

            CASE(OPC_PUTFIELD, {
                int index = bcr_readu2(&frame->reader);
                struct field *f = resolve_field(frame->method->clazz, index);

                // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
                if (IS_FINAL(f->access_flags)) {
                    // todo
                    if (frame->method->clazz != f->clazz || !utf8_equals(frame->method->name, SYMBOL(object_init))) {
                        jvm_abort("java.lang.IllegalAccessError\n"); // todo
                    }
                }

                if (f->category_two) {
                    frame->stack -= 2;
                } else {
                    frame->stack--;
                }
                slot_t *value = frame->stack;

                jref obj = frame_stack_popr(frame);
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                set_instance_field_value(obj, f, value);
            })

            case OPC_INVOKEVIRTUAL: {
                // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
                int index = bcr_readu2(&frame->reader);
                struct method *m = resolve_method(frame->method->clazz, index);

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                struct object *obj = (struct object *) args[0];
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                // 下面这样写对不对 todo
//                if (obj->clazz != frame->method->clazz) {
                // 从对象的类中查找真正要调用的方法
                m = class_lookup_method(obj->clazz, m->name, m->descriptor);
//                }

                resolved_method = m;
                goto invoke_method;
            }

            case OPC_INVOKESPECIAL: {
                // invokespecial指令用于调用一些需要特殊处理的实例方法，
                // 包括构造函数、私有方法和通过super关键字调用的超类方法。
                struct class *curr_class = frame->method->clazz;
                int index = bcr_readu2(&frame->reader);
//
//    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
////    if (method->name == "<init>" && method->class != c) {
////        // todo java.lang.NoSuchMethodError
////        jvm_abort("java.lang.NoSuchMethodError\n");
////    }

                struct method *m = resolve_method(curr_class, index);
                /*
                 * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
                 * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
                 * todo 详细说明
                 */
                if (IS_SUPER(m->clazz->access_flags)
                    && !IS_PRIVATE(m->access_flags)
                    && class_is_subclass_of(curr_class, m->clazz) // todo
                    && !utf8_equals(m->name, SYMBOL(object_init))) {
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

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                jref obj = (jref) args[0];
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                resolved_method = m;
                goto invoke_method;
            }
            case OPC_INVOKESTATIC: {
                // invokestatic指令用来调用静态方法。
                // 如果类还没有被初始化，会触发类的初始化。
                struct class *curr_class = frame->method->clazz;

                int index = bcr_readu2(&frame->reader);

                struct method *m = resolve_method(curr_class, index);

                if (IS_ABSTRACT(m->access_flags)) {
                    // todo java.lang.AbstractMethodError
                    jvm_abort("java.lang.AbstractMethodError\n");
                }
                if (!IS_STATIC(m->access_flags)) {
                    // todo java.lang.IncompatibleClassChangeError
                    jvm_abort("java.lang.IncompatibleClassChangeError\n");
                }

                if (!m->clazz->inited) {
                    class_clinit(m->clazz);
                }

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                resolved_method = m;
                goto invoke_method;
            }
            case OPC_INVOKEINTERFACE: {
                struct class *curr_class = frame->method->clazz;
                int index = frame_readu2(frame); //bcr_readu2(&frame->reader);

                /*
                 * 此字节的值是给方法传递参数需要的slot数，
                 * 其含义和给method结构体定义的arg_slot_count字段相同。
                 * 这个数是可以根据方法描述符计算出来的，它的存在仅仅是因为历史原因。
                 */
                frame_readu1(frame);
                /*
                 * 此字节是留给Oracle的某些Java虚拟机实现用的，它的值必须是0。
                 * 该字节的存在是为了保证Java虚拟机可以向后兼容。
                 */
                frame_readu1(frame);

                struct method *m = resolve_method(curr_class, index);

                /* todo 本地方法 */

                frame->stack-= m->arg_slot_count;
                args = frame->stack;

                jref obj = (jref) args[0];
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
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

                resolved_method = method;
                goto invoke_method;
            }
            case OPC_INVOKEDYNAMIC:
                invokedynamic(frame);
                break;
            invoke_method:
            {
                struct frame *new_frame = alloc_frame(resolved_method, false);
                if (resolved_method->arg_slot_count > 0 && args == NULL) {
                    jvm_abort("do not find args, %d\n", resolved_method->arg_slot_count); // todo
                }

                // 准备参数
                for (int i = 0; i < resolved_method->arg_slot_count; i++) {
                    // 传递参数到被调用的函数。
                    //        frame_locals_set(frame, i, args + i);
                    new_frame->locals[i] = args[i];
                }

                frame = new_frame;
                break;
            }

            CASE(OPC_NEW, {
                // new指令专门用来创建类实例。数组由专门的指令创建
                // 如果类还没有被初始化，会触发类的初始化。
                struct bytecode_reader *reader = &frame->reader;
                struct class *c = resolve_class(frame->method->clazz, bcr_readu2(reader));  // todo
                if (!c->inited)
                    class_clinit(c);

                if (IS_INTERFACE(c->access_flags) || IS_ABSTRACT(c->access_flags)) {
                    jvm_abort("java.lang.InstantiationError\n");  // todo 抛出 InstantiationError 异常
                }

                // todo java/lang/Class 会在这里创建，为什么会这样，怎么处理
                //    assert(strcmp(c->class_name, "java/lang/Class") == 0);

                frame_stack_pushr(frame, object_create(c));
            })

            CASE(OPC_NEWARRAY, newarray(frame))
            CASE(OPC_ANEWARRAY, anewarray(frame))

            CASE(OPC_ARRAYLENGTH, {
                struct object *o = frame_stack_popr(frame);
                if (o == NULL) {
                    thread_throw_null_pointer_exception();
                }
                if (!object_is_array(o)) {
                    vm_unknown_error("not a array");
                }
                frame_stack_pushi(frame, arrobj_len(o));
            })

            case OPC_ATHROW: {
                jref exception = frame_stack_popr(frame);
                if (exception == NULL) {
                    thread_throw_null_pointer_exception();
                }

                // 遍历虚拟机栈找到可以处理此异常的方法
                while (frame != NULL) {
                    int handler_pc = method_find_exception_handler(frame->method, exception->clazz, frame->reader.pc - 1); // instruction length todo 好像是错的
                    if (handler_pc >= 0) {  // todo 可以等于0吗
                        /*
                         * 找到可以处理的函数了
                         * 操作数栈清空
                         * 把异常对象引用推入栈顶
                         * 跳转到异常处理代码之前
                         */
//                frame_stack_clear(top);  // todo
//                frame_stack_pushr(top, exception);
                        frame_stack_pushr(frame, exception);
                        frame->reader.pc = (size_t) handler_pc;
                        break;
                    }

                    // frame 无法处理异常，弹出
                    pop_frame();
                    frame = frame->prev;
                }

                thread_handle_uncaught_exception(exception);
                return NULL; // todo
            }

            CASE(OPC_CHECKCAST, {
                jref obj = RSLOT(frame->stack - 1); // 不改变操作数栈
                int index = bcr_readu2(&frame->reader);

                // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
                if (obj != NULL) {
                    struct class *c = resolve_class(frame->method->clazz, index);
                    if (!object_is_instance_of(obj, c)) {
                        thread_throw_class_cast_exception(obj->clazz->class_name, c->class_name);
                    }
                }
            })

            CASE(OPC_INSTANCEOF, {
                int index = bcr_readu2(&frame->reader);
                struct class *c = resolve_class(frame->method->clazz, index);

                jref obj = frame_stack_popr(frame);
                if (obj == NULL)
                    frame_stack_pushi(frame, 0);
                else
                    frame_stack_pushi(frame, object_is_instance_of(obj, c) ? 1 : 0);
            })

            CASE(OPC_MONITORENTER, {
                jref o = frame_stack_popr(frame);
                // todo
            })

            CASE(OPC_MONITOREXIT, {
                jref o = frame_stack_popr(frame);
                // todo
            })

            CASE(OPC_WIDE, wide_extending = true)
            CASE(OPC_MULTIANEWARRAY, multianewarray(frame))

            CASE(OPC_IFNULL, {
                int offset = frame_reads2(frame);
                if (frame_stack_popr(frame) == NULL) {
                    bcr_skip(&frame->reader, offset - 3); // minus instruction length
                }
            })

            CASE(OPC_IFNONNULL, {
                int offset = frame_reads2(frame);
                if (frame_stack_popr(frame) != NULL) {
                    bcr_skip(&frame->reader, offset - 3); // minus instruction length
                }
            })

            case OPC_GOTO_W: // todo
                vm_internal_error("goto_w doesn't support");
                break;
            case OPC_JSR_W: // todo
                vm_internal_error("jsr_w doesn't support after jdk 6.");
            CASE(OPC_INVOKENATIVE, frame->method->native_method(frame))
            default:
                jvm_abort("This instruction isn't used. %d(0x%x)\n", opcode, opcode); // todo
        }
    }

    jvm_abort("Never goes here!");
}

slot_t *exec_java_func(struct method *method, const slot_t *args)
{
    struct frame *frame = alloc_frame(method, true);
    if (method->arg_slot_count > 0 && args == NULL) {
        jvm_abort("do not find args, %d\n", method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame->locals[i] = args[i];
    }

    return exec();
}