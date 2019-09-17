/*
 * Author: kayo
 */

#include <iostream>
#include <sstream>
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
        "invokenative", "impdep2"
};
#endif


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
            stringstream ss;
            ss << "error. Invalid array type: " << arr_type;
            raiseException(UNKNOWN_ERROR, ss.str().c_str());
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

/*
 * 执行当前线程栈顶的frame
 */
static slot_t *exec()
{
    Thread *thread = thread_self();

    Method *resolved_method;
    slot_t *args;

    Frame *frame = thread->topFrame;
    TRACE("executing frame: %s\n", frame->toString().c_str());

    BytecodeReader *reader = &frame->reader;
    Class *clazz = frame->method->clazz;
    slot_t *stack = frame->stack;
    slot_t *locals = frame->locals;
    
    jint index;
    slot_t *value;

#define CHANGE_FRAME(newFrame) \
    do { \
        /*frame->stack = stack;  stack指针在变动，需要设置一下 todo */ \
        frame = newFrame; \
        reader = &frame->reader; \
        stack = frame->stack; \
        clazz = frame->method->clazz; \
        locals = frame->locals; \
    } while (false)

    static void *labels[] = {
        &&nop,

        // Constants [0x01 ... 0x14]
        &&opc_aconst_null, 
        &&opc_iconst_m1, &&opc_iconst_0,  &&opc_iconst_1, &&opc_iconst_2, &&opc_iconst_3, &&opc_iconst_4, &&opc_iconst_5,
        &&opc_lconst_0, &&opc_lconst_1,
        &&opc_fconst_0, &&opc_fconst_1, &&opc_fconst_2, 
        &&opc_dconst_0, &&opc_dconst_1,
        &&opc_bipush, &&opc_sipush,
        &&opc_ldc, &&opc_ldc_w, &&opc_ldc2_w, 

        // Loads [0x15 ... 0x35]
        &&opc_iload, &&opc_lload, &&opc_fload,&&opc_dload, &&opc_aload, 
        &&opc_iload_0, &&opc_iload_1,  &&opc_iload_2, &&opc_iload_3, 
        &&opc_lload_0, &&opc_lload_1,  &&opc_lload_2, &&opc_lload_3,
        &&opc_fload_0, &&opc_fload_1,  &&opc_fload_2,  &&opc_fload_3,
        &&opc_dload_0, &&opc_dload_1, &&opc_dload_2, &&opc_dload_3,
        &&opc_aload_0, &&opc_aload_1, &&opc_aload_2, &&opc_aload_3,
        &&opc_iaload, &&opc_laload, &&opc_faload, &&opc_daload, &&opc_aaload, &&opc_baload, &&opc_caload, &&opc_saload,

        // Stores [0x36 ... 0x56]
        &&opc_istore,  &&opc_lstore, &&opc_fstore, &&opc_dstore,  &&opc_astore, 
        &&opc_istore_0, &&opc_istore_1, &&opc_istore_2, &&opc_istore_3,
        &&opc_lstore_0, &&opc_lstore_1, &&opc_lstore_2, &&opc_lstore_3,
        &&opc_fstore_0, &&opc_fstore_1, &&opc_fstore_2, &&opc_fstore_3,
        &&opc_dstore_0, &&opc_dstore_1, &&opc_dstore_2, &&opc_dstore_3,
        &&opc_astore_0, &&opc_astore_1, &&opc_astore_2, &&opc_astore_3,
        &&opc_iastore, &&opc_lastore, &&opc_fastore, &&opc_dastore, &&opc_aastore, &&opc_bastore, &&opc_castore, &&opc_sastore,

        // Stack [0x57 ... 0x5f]
        &&opc_pop, &&opc_pop2, 
        &&opc_dup, &&opc_dup_x1, &&opc_dup_x2, &&opc_dup2, &&opc_dup2_x1, &&opc_dup2_x2, 
        &&opc_swap,

        // Math [0x60 ... 0x84]
        &&opc_iadd, &&opc_ladd, &&opc_fadd, &&opc_dadd, 
        &&opc_isub, &&opc_lsub, &&opc_fsub, &&opc_dsub, 
        &&opc_imul, &&opc_lmul, &&opc_fmul, &&opc_dmul, 
        &&opc_idiv, &&opc_ldiv, &&opc_fdiv, &&opc_ddiv, 
        &&opc_irem, &&opc_lrem, &&opc_frem, &&opc_drem,
        &&opc_ineg, &&opc_lneg, &&opc_fneg, &&opc_dneg, 
        &&opc_ishl, &&opc_lshl, &&opc_ishr, &&opc_lshr, &&opc_iushr, &&opc_lushr, 
        &&opc_iand, &&opc_land, &&opc_ior, &&opc_lor, &&opc_ixor, &&opc_lxor, &&opc_iinc,

        // Conversions [0x85 ... 0x93]
        &&opc_i2l, &&opc_i2f, &&opc_i2d, 
        &&opc_l2i, &&opc_l2f, &&opc_l2d, 
        &&opc_f2i, &&opc_f2l, &&opc_f2d,
        &&opc_d2i, &&opc_d2l, &&opc_d2f, 
        &&opc_i2b, &&opc_i2c, &&opc_i2s, 

        // Comparisons [0x94 ... 0xa6]
        &&opc_lcmp, &&opc_fcmpl, &&opc_fcmpg, &&opc_dcmpl, &&opc_dcmpg, 
        &&opc_ifeq, &&opc_ifne, &&opc_iflt, &&opc_ifge, &&opc_ifgt,&&opc_ifle, 
        &&opc_if_icmpeq, &&opc_if_icmpne, &&opc_if_icmplt, &&opc_if_icmpge, &&opc_if_icmpgt, &&opc_if_icmple, 
        &&opc_if_acmpeq, &&opc_if_acmpne, 

        // Control [0xa7 ... 0xb1]
        &&opc_goto, &&opc_jsr, &&opc_ret, &&opc_tableswitch, &&opc_lookupswitch, 
        &&opc_ireturn, &&opc_lreturn, &&opc_freturn, &&opc_dreturn, &&opc_areturn, &&opc_return,

        // References [0xb2 ... 0xc3]
        &&opc_getstatic, &&opc_putstatic, &&opc_getfield, &&opc_putfield,
        &&opc_invokevirtual, &&opc_invokespecial, &&opc_invokestatic,&&opc_invokeinterface, &&opc_invokedynamic,
        &&opc_new,&&opc_newarray, &&opc_anewarray, &&opc_arraylength, 
        &&opc_athrow, &&opc_checkcast, &&opc_instanceof, &&opc_monitorenter, &&opc_monitorexit,

        // Extended [0xc4 ... 0xc9]
        &&opc_wide, &&opc_multianewarray, &&opc_ifnull, &&opc_ifnonnull, &&opc_goto_w, &&opc_jsr_w, 

        // Reserved [0xca ... 0xff]
        &&opc_breakpoint,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused, &&opc_notused,
        &&opc_notused,  &&opc_notused, &&opc_invokenative, &&opc_impdep2
    };

#if TRACE_INTERPRETER    
#define DISPATCH \
do { \
    u1 opcode = reader->readu1(); \
    TRACE("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc); \
    goto *labels[opcode]; \
} while(false)
#else
#define DISPATCH goto *labels[reader->readu1()];
#endif

nop:
    DISPATCH
opc_aconst_null:
    *frame->stack++ = 0;
    DISPATCH
opc_iconst_m1:
    *frame->stack++ = -1;
    DISPATCH
opc_iconst_0:
    *frame->stack++ = 0;
    DISPATCH
opc_iconst_1:
    *frame->stack++ = 1; 
    DISPATCH
opc_iconst_2:
    *frame->stack++ = 2;
    DISPATCH
opc_iconst_3:
    *frame->stack++ = 3;
    DISPATCH
opc_iconst_4:
    *frame->stack++ = 4;
    DISPATCH
opc_iconst_5:
    *frame->stack++ = 5;
    DISPATCH

opc_lconst_0: 
    frame->pushl(0); 
    DISPATCH
opc_lconst_1:
     frame->pushl(1); 
     DISPATCH

opc_fconst_0: 
    *frame->stack++ = 0; 
    DISPATCH
opc_fconst_1:
    *((jfloat*) frame->stack) = (float) 1.0;
    frame->stack++;
    DISPATCH
opc_fconst_2:
    *((jfloat*) frame->stack) = (float) 2.0;
    frame->stack++;
    DISPATCH

opc_dconst_0: 
    frame->pushd(0); 
    DISPATCH
opc_dconst_1: 
    frame->pushd(1); 
    DISPATCH

opc_bipush: // Byte Integer push
    frame->pushi(reader->readu1()); 
    DISPATCH
opc_sipush: // Short Integer push
    frame->pushi(reader->readu2());
     DISPATCH

opc_ldc:
    index = reader->readu1();
    goto __ldc;
opc_ldc_w:
    index = reader->readu2();
__ldc:
    ConstantPool &cp = frame->method->clazz->cp;
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Integer || type == CONSTANT_Float || type == CONSTANT_ResolvedString) {
        *frame->stack++ = CP_INFO(cp, index);
    } else if (type == CONSTANT_String) {
        frame->pushr(resolve_string(frame->method->clazz, index));
    } else if (type == CONSTANT_Class) {
        frame->pushr(resolve_class(frame->method->clazz, index)->clsobj);
    } else if (type == CONSTANT_ResolvedClass) {
        auto c = (Class *) CP_INFO(cp, index);
        frame->pushr(c->clsobj);
    } else {
        stringstream ss;
        ss << "unknown type: " << type;
        raiseException(UNKNOWN_ERROR, ss.str().c_str());
    }
    DISPATCH

opc_ldc2_w:
    {
        index = frame->reader.readu2();
        ConstantPool &cp = frame->method->clazz->cp;
        u1 type = CP_TYPE(cp, index);

        if (type == CONSTANT_Long) {
            frame->pushl(CP_LONG(cp, index));
        } else if (type == CONSTANT_Double) {
            frame->pushd(CP_DOUBLE(cp, index));
        } else {
            stringstream ss;
            ss << "unknown type: " << type;
            raiseException(UNKNOWN_ERROR, ss.str().c_str());
        }
        DISPATCH
    }

opc_iload:
opc_fload:
opc_aload: 
    index = reader->readu1();
    *frame->stack++ = locals[index];
    DISPATCH

opc_lload:
opc_dload: 
    index = reader->readu1();
    *frame->stack++ = locals[index];
    *frame->stack++ = locals[index + 1];
    DISPATCH

opc_iload_0:
opc_fload_0:
opc_aload_0:
    *frame->stack++ = locals[0];
    DISPATCH
opc_iload_1:
opc_fload_1: 
opc_aload_1: 
    *frame->stack++ = locals[1];
    DISPATCH
opc_iload_2:
opc_fload_2:
opc_aload_2: 
    *frame->stack++ = locals[2];
    DISPATCH
opc_iload_3: 
opc_fload_3:
opc_aload_3: 
    *frame->stack++ = locals[3];
    DISPATCH

opc_lload_0:
opc_dload_0:
    *frame->stack++ = locals[0];
    *frame->stack++ = locals[1];
    DISPATCH
opc_lload_1: 
opc_dload_1: 
    *frame->stack++ = locals[1];
    *frame->stack++ = locals[2];
    DISPATCH
opc_lload_2: 
opc_dload_2: 
    *frame->stack++ = locals[2];
    *frame->stack++ = locals[3];
    DISPATCH
opc_lload_3: 
opc_dload_3: 
    *frame->stack++ = locals[3];
    *frame->stack++ = locals[4];
    DISPATCH

#define GET_AND_CHECK_ARRAY \
    index = frame->popi(); \
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
opc_iaload: 
    ARRAY_LOAD_CATEGORY_ONE(jint); 
    DISPATCH
opc_faload:
    ARRAY_LOAD_CATEGORY_ONE(jfloat); 
    DISPATCH
opc_aaload:
    ARRAY_LOAD_CATEGORY_ONE(jref);
    DISPATCH
opc_baload: 
    ARRAY_LOAD_CATEGORY_ONE(jbyte); 
    DISPATCH
opc_caload: 
    ARRAY_LOAD_CATEGORY_ONE(jchar); 
    DISPATCH
opc_saload: 
    ARRAY_LOAD_CATEGORY_ONE(jshort); 
    DISPATCH

opc_laload: 
opc_daload:
    {
        GET_AND_CHECK_ARRAY
        value = (slot_t *) arr->index(index);
        *frame->stack++ = value[0];
        *frame->stack++ = value[1];
        DISPATCH
    }

opc_istore: 
opc_fstore: 
opc_astore:
    index = reader->readu1();
    locals[index] = *--frame->stack;
    DISPATCH

opc_lstore: 
opc_dstore:
    index = reader->readu1();
    locals[index + 1] = *--frame->stack;
    locals[index] = *--frame->stack;
    DISPATCH

opc_istore_0: 
opc_fstore_0: 
opc_astore_0:
    locals[0] = *--frame->stack;
    DISPATCH
opc_istore_1: 
opc_fstore_1: 
opc_astore_1:
    locals[1] = *--frame->stack;
    DISPATCH
opc_istore_2: 
opc_fstore_2: 
opc_astore_2: 
    locals[2] = *--frame->stack;
    DISPATCH
opc_istore_3: 
opc_fstore_3: 
opc_astore_3: 
    locals[3] = *--frame->stack;
    DISPATCH

opc_lstore_0:
opc_dstore_0:
    locals[1] = *--frame->stack;
    locals[0] = *--frame->stack;
    DISPATCH
opc_lstore_1:
opc_dstore_1:
    locals[2] = *--frame->stack;
    locals[1] = *--frame->stack;
    DISPATCH
opc_lstore_2: 
opc_dstore_2:
    locals[3] = *--frame->stack;
    locals[2] = *--frame->stack;
    DISPATCH
opc_lstore_3: 
opc_dstore_3:
    locals[4] = *--frame->stack;
    locals[3] = *--frame->stack;
    DISPATCH

#define ARRAY_STORE_CATEGORY_ONE(type) \
{ \
    auto value = (type) *--frame->stack; \
    GET_AND_CHECK_ARRAY \
    arr->set(index, value); \
}
opc_iastore: 
    ARRAY_STORE_CATEGORY_ONE(jint); 
    DISPATCH
opc_fastore: 
    ARRAY_STORE_CATEGORY_ONE(jfloat);
    DISPATCH
opc_aastore:
    ARRAY_STORE_CATEGORY_ONE(jref); 
    DISPATCH
opc_bastore:
    ARRAY_STORE_CATEGORY_ONE(jbyte);
    DISPATCH
opc_castore:
    ARRAY_STORE_CATEGORY_ONE(jchar);
    DISPATCH
opc_sastore:
    ARRAY_STORE_CATEGORY_ONE(jshort); 
    DISPATCH

opc_lastore: 
opc_dastore:
    frame->stack -= 2;
    value = frame->stack;
    GET_AND_CHECK_ARRAY
    memcpy(arr->index(index), value, sizeof(slot_t) * 2);
    DISPATCH

opc_pop:
    frame->stack--;
    DISPATCH
opc_pop2:
    frame->stack -= 2;
    DISPATCH
opc_dup:
    frame->stack[0] = frame->stack[-1];
    frame->stack++;
    DISPATCH
opc_dup_x1:
    frame->stack[0] = frame->stack[-1];
    frame->stack[-1] = frame->stack[-2];
    frame->stack[-2] = frame->stack[0];
    frame->stack++;
    DISPATCH
opc_dup_x2:
    frame->stack[0] = frame->stack[-1];
    frame->stack[-1] = frame->stack[-2];
    frame->stack[-2] = frame->stack[-3];
    frame->stack[-3] = frame->stack[0];
    frame->stack++;
    DISPATCH
opc_dup2:
    frame->stack[0] = frame->stack[-2];
    frame->stack[1] = frame->stack[-1];
    frame->stack += 2;
    DISPATCH
opc_dup2_x1:
    // ..., value3, value2, value1 →
    // ..., value2, value1, value3, value2, value1
    frame->stack[1] = frame->stack[-1];
    frame->stack[0] = frame->stack[-2];
    frame->stack[-1] = frame->stack[-3];
    frame->stack[-2] = frame->stack[1];
    frame->stack[-3] = frame->stack[0];
    frame->stack += 2;
    DISPATCH
opc_dup2_x2:
    // ..., value4, value3, value2, value1 →
    // ..., value2, value1, value4, value3, value2, value1
    frame->stack[1] = frame->stack[-1];
    frame->stack[0] = frame->stack[-2];
    frame->stack[-1] = frame->stack[-3];
    frame->stack[-2] = frame->stack[-4];
    frame->stack[-3] = frame->stack[1];
    frame->stack[-4] = frame->stack[0];
    frame->stack += 2;
    DISPATCH
opc_swap:
    swap(frame->stack[-1], frame->stack[-2]);
    DISPATCH

#define BINARY_OP(type, n, oper) \
{ \
    frame->stack -= (n);\
    ((type *) frame->stack)[-1] = ((type *) frame->stack)[-1] oper ((type *) frame->stack)[0]; \
}
opc_iadd:
    BINARY_OP(jint, 1, +)
    DISPATCH
opc_ladd:
    BINARY_OP(jlong, 2, +)
    DISPATCH
opc_fadd:
    BINARY_OP(jfloat, 1, +)
    DISPATCH
opc_dadd: 
    BINARY_OP(jdouble, 2, +)
    DISPATCH

opc_isub:
    BINARY_OP(jint, 1, -)
    DISPATCH
opc_lsub:
    BINARY_OP(jlong, 2, -)
    DISPATCH
opc_fsub:
    BINARY_OP(jfloat, 1, -)
    DISPATCH
opc_dsub: 
    BINARY_OP(jdouble, 2, -)
    DISPATCH

opc_imul:
    BINARY_OP(jint, 1, *)
    DISPATCH
opc_lmul:
    BINARY_OP(jlong, 2, *)
    DISPATCH
opc_fmul: 
    BINARY_OP(jfloat, 1, *)
    DISPATCH
opc_dmul:
    BINARY_OP(jdouble, 2, *)
    DISPATCH

opc_idiv:
    BINARY_OP(jint, 1, /)
    DISPATCH
opc_ldiv:
    BINARY_OP(jlong, 2, /)
    DISPATCH
opc_fdiv:
    BINARY_OP(jfloat, 1, /)
    DISPATCH
opc_ddiv:
    BINARY_OP(jdouble, 2, /)
    DISPATCH

opc_irem:
    BINARY_OP(jint, 1, %)
    DISPATCH
opc_lrem: 
    BINARY_OP(jlong, 2, %)
    DISPATCH

opc_frem:
    {
        jfloat v2 = frame->popf();
        jfloat v1 = frame->popf();
        jvm_abort("not implement\n");
        //    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
        DISPATCH
    }
opc_drem:
    {
        jdouble v2 = frame->popd();
        jdouble v1 = frame->popd();
        jvm_abort("not implement\n");
        //    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
        DISPATCH
    }
    
opc_ineg:
    frame->pushi(-frame->popi());
    DISPATCH
opc_lneg: 
    frame->pushl(-frame->popl());
    DISPATCH
opc_fneg: 
    frame->pushf(-frame->popf());
    DISPATCH
opc_dneg: 
    frame->pushd(-frame->popd());
    DISPATCH

    jint shift;
    jint ivalue;
    jlong lvalue;
opc_ishl:
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    shift = frame->popi() & 0x1f;
    ivalue = frame->popi();
    frame->pushi(ivalue << shift);
    DISPATCH
opc_lshl: 
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    shift = frame->popi() & 0x3f;
    lvalue = frame->popl();
    frame->pushl(lvalue << shift);
    DISPATCH
opc_ishr:
    // 逻辑右移 shift logical right
    shift = frame->popi() & 0x1f;
    ivalue = frame->popi();
    frame->pushi((~(((jint)1) >> shift)) & (ivalue >> shift));
    DISPATCH

opc_lshr: 
    shift = frame->popi() & 0x3f;
    lvalue = frame->popl();
    frame->pushl((~(((jlong)1) >> shift)) & (lvalue >> shift));
    DISPATCH

opc_iushr: 
    // 算术右移 shift arithmetic right
    shift = frame->popi() & 0x1f;
    ivalue = frame->popi();
    frame->pushi(ivalue >> shift);
    DISPATCH

opc_lushr: 
    shift = frame->popi() & 0x3f;
    lvalue = frame->popl();
    frame->pushl(lvalue >> shift);
    DISPATCH

opc_iand: 
    BINARY_OP(jint, 1, &);
    DISPATCH
opc_land:
    BINARY_OP(jlong, 2, &);
    DISPATCH
opc_ior: 
    BINARY_OP(jint, 1, |);
    DISPATCH
opc_lor: 
    BINARY_OP(jlong, 2, |);
    DISPATCH
opc_ixor: 
    BINARY_OP(jint, 1, ^);
    DISPATCH
opc_lxor:
    BINARY_OP(jlong, 2, ^);
    DISPATCH

opc_iinc: 
    index = reader->readu1();
    ISLOT(locals + index) = ISLOT(locals + index) + reader->reads1();
    DISPATCH

opc_i2l: 
    frame->pushl(i2l(frame->popi()));
    DISPATCH
opc_i2f: 
    frame->pushf(i2f(frame->popi()));
    DISPATCH
opc_i2d: 
    frame->pushd(i2d(frame->popi()));
    DISPATCH

opc_l2i: 
    frame->pushi(l2i(frame->popl()));
    DISPATCH
opc_l2f: 
    frame->pushf(l2f(frame->popl()));
    DISPATCH
opc_l2d: 
    frame->pushd(l2d(frame->popl()));
    DISPATCH

opc_f2i:
    frame->pushi(f2i(frame->popf()));
    DISPATCH
opc_f2l: 
    frame->pushl(f2l(frame->popf()));
    DISPATCH
opc_f2d: 
    frame->pushd(f2d(frame->popf()));
    DISPATCH

opc_d2i:
    frame->pushi(d2i(frame->popd()));
    DISPATCH
opc_d2l:
    frame->pushl(d2l(frame->popd()));
    DISPATCH
opc_d2f: 
    frame->pushf(d2f(frame->popd()));
    DISPATCH

opc_i2b:  // todo byte or bool????
    frame->pushi(i2b(frame->popi()));
    DISPATCH
opc_i2c: 
    frame->pushi(i2c(frame->popi()));
    DISPATCH
opc_i2s:
    frame->pushi(i2s(frame->popi()));
    DISPATCH

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

opc_lcmp: 
    CMP(jlong, l, DO_CMP(v1, v2, -1));
    DISPATCH
opc_fcmpl:
    CMP(jfloat, f, DO_CMP(v1, v2, -1));
    DISPATCH
opc_fcmpg: 
    CMP(jfloat, f, DO_CMP(v1, v2, 1));
    DISPATCH
opc_dcmpl: 
    CMP(jdouble, d, DO_CMP(v1, v2, -1));
    DISPATCH
opc_dcmpg:
    CMP(jdouble, d, DO_CMP(v1, v2, 1));
    DISPATCH

#define IF_COND(cond) \
{ \
    jint v = frame->popi(); \
    jint offset = reader->reads2(); \
    if (v cond 0) \
        reader->skip(offset - 3);  /* minus instruction length */ \
}
opc_ifeq:
    IF_COND(==);
    DISPATCH
opc_ifne:
    IF_COND(!=);
    DISPATCH
opc_iflt:
    IF_COND(<);
    DISPATCH
opc_ifge: 
    IF_COND(>=);
    DISPATCH
opc_ifgt: 
    IF_COND(>);
    DISPATCH
opc_ifle: 
    IF_COND(<=);
    DISPATCH

#define IF_ICMP_COND(cond) \
{ \
    frame->stack -= 2;\
    jint offset = reader->reads2(); \
    if (ISLOT(frame->stack) cond ISLOT(frame->stack + 1)) \
        reader->skip(offset - 3); /* minus instruction length */ \
    DISPATCH \
}
opc_if_icmpeq: 
    IF_ICMP_COND(==);
    DISPATCH
opc_if_icmpne:
    IF_ICMP_COND(!=);
    DISPATCH
opc_if_icmplt:
    IF_ICMP_COND(<);
    DISPATCH
opc_if_icmpge: 
    IF_ICMP_COND(>=);
    DISPATCH
opc_if_icmpgt: 
    IF_ICMP_COND(>);
    DISPATCH
opc_if_icmple: 
    IF_ICMP_COND(<=);
    DISPATCH

#define IF_ACMP_COND(cond) \
{ \
    frame->stack -= 2;\
    jint offset = reader->reads2(); \
    if (RSLOT(frame->stack) cond RSLOT(frame->stack + 1)) \
        reader->skip(offset - 3);  /* minus instruction length */ \
}
opc_if_acmpeq:
    IF_ACMP_COND(==);
    DISPATCH
opc_if_acmpne: 
    IF_ACMP_COND(!=);
    DISPATCH

opc_goto: 
    int offset = reader->reads2();
    reader->skip(offset - 3);  // minus instruction length
    DISPATCH

// 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
// 从Java 6开始，已经不再使用这些指令
opc_jsr:
    raiseException(INTERNAL_ERROR, "jsr doesn't support after jdk 6.");
    DISPATCH
opc_ret:
    raiseException(INTERNAL_ERROR, "ret doesn't support after jdk 6.");
    DISPATCH

opc_tableswitch:
    tableswitch(frame);
    DISPATCH
opc_lookupswitch:
    lookupswitch(frame);
    DISPATCH

    int ret_value_slot_count;

opc_ireturn:
opc_freturn:
opc_areturn:
    ret_value_slot_count = 1;
    goto __method_return;
opc_lreturn:
opc_dreturn:
    ret_value_slot_count = 2;
    goto __method_return;
opc_return:
    ret_value_slot_count = 0;
__method_return:
    Frame *invoke_frame = thread->topFrame = frame->prev;
    frame->stack -= ret_value_slot_count;
    if (frame->vm_invoke || invoke_frame == nullptr) {
        return frame->stack;
    } else {
        slot_t *ret_value = frame->stack;
        for (int i = 0; i < ret_value_slot_count; i++) {
            *invoke_frame->stack++ = *ret_value++;
        }
        CHANGE_FRAME(invoke_frame);
        TRACE("executing frame: %s\n", frame->toString().c_str());
    }
    DISPATCH

opc_getstatic: 
    index = reader->readu2();
    Field *f = resolve_field(frame->method->clazz, index);

    if (!f->clazz->inited) {
        f->clazz->clinit();
    }

    value = (slot_t *) f->clazz->getStaticFieldValue(f);
    *frame->stack++ = value[0];
    if (f->categoryTwo) {
        *frame->stack++ = value[1];
    }
    DISPATCH

opc_putstatic:
    {
        index = reader->readu2();
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
        DISPATCH
    }

opc_getfield:
    {
        index = reader->readu2();
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
        DISPATCH
    }

opc_putfield:
    {
        index = reader->readu2();
        Field *f = resolve_field(frame->method->clazz, index);

        // 如果是final字段，则只能在构造函数中初始化，否则抛出java.lang.IllegalAccessError。
        if (f->isFinal()) {
            // todo
            if (frame->method->clazz != f->clazz || !utf8_equals(frame->method->name, S(object_init))) {
                raiseException(ILLEGAL_ACCESS_ERROR);
            }
        }

        if (f->categoryTwo) {
            frame->stack -= 2;
        } else {
            frame->stack--;
        }
        value = frame->stack;

        jref obj = frame->popr();
        if (obj == nullptr) {
            thread_throw_null_pointer_exception();
        }

        obj->setFieldValue(f, value);
        DISPATCH
    }
opc_invokevirtual:
    {
        // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
        index = reader->readu2();
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
        goto __invoke_method;
    }
opc_invokespecial:
    {
        // invokespecial指令用于调用一些需要特殊处理的实例方法，
        // 包括构造函数、私有方法和通过super关键字调用的超类方法。
        index = reader->readu2();
//
//    // 假定从方法符号引用中解析出来的类是C，方法是M。如果M是构造函数，则声明M的类必须是C，
////    if (method->name == "<init>" && method->class != c) {
////        // todo java.lang.NoSuchMethodError
////        jvm_abort("java.lang.NoSuchMethodError\n");
////    }

        Method *m = resolve_method(clazz, index);
        /*
         * 如果调用的中超类中的函数，但不是构造函数，不是private 函数，且当前类的ACC_SUPER标志被设置，
         * 需要一个额外的过程查找最终要调用的方法；否则前面从方法符号引用中解析出来的方法就是要调用的方法。
         * todo 详细说明
         */
        if (m->clazz->isSuper()
            && !m->isPrivate()
            && clazz->isSubclassOf(m->clazz) // todo
            && !utf8_equals(m->name, S(object_init))) {
            m = clazz->superClass->lookupMethod(m->name, m->descriptor);
        }

        if (m->isAbstract()) {
            raiseException(ABSTRACT_METHOD_ERROR);
        }
        if (m->isStatic()) {
            raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
        }

        frame->stack -= m->arg_slot_count;
        args = frame->stack;
        auto obj = (jref) args[0];
        if (obj == nullptr) {
            thread_throw_null_pointer_exception();
        }

        resolved_method = m;
        goto __invoke_method;
    }
opc_invokestatic:
    {
        // invokestatic指令用来调用静态方法。
        // 如果类还没有被初始化，会触发类的初始化。
        index = reader->readu2();
        Method *m = resolve_method(clazz, index);
        if (m->isAbstract()) {
            raiseException(ABSTRACT_METHOD_ERROR);
        }
        if (!m->isStatic()) {
            raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
        }

        if (!m->clazz->inited) {
            m->clazz->clinit();
        }

        frame->stack -= m->arg_slot_count;
        args = frame->stack;
        resolved_method = m;
        goto __invoke_method;
    }
opc_invokeinterface:
    {
        index = reader->readu2();

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

        Method *m = resolve_method(clazz, index);
        assert(m->clazz->isInterface());

        /* todo 本地方法 */

        frame->stack -= m->arg_slot_count;
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
            raiseException(ABSTRACT_METHOD_ERROR);
        }
        if (!method->isPublic()) {
            raiseException(ILLEGAL_ACCESS_ERROR);
        }

        resolved_method = method;
        goto __invoke_method;
    }
opc_invokedynamic:
    {
        // 每一个 invokedynamic 指令都称为 Dynamic Call Site(动态调用点)

        jvm_abort("not implement!!!\n"); // todo

        Class *currClass = frame->method->clazz;
        ConstantPool &cp = currClass->cp;
        // The run-time constant pool item at that index must be a symbolic reference to a call site specifier.
        /*
         * CONSTANT_InvokeDynamic_info {
                u1 tag;
                u2 bootstrap_method_attr_index;
                u2 name_and_type_index;
            }
         */
        index = reader->readu2(); // point to CONSTANT_InvokeDynamic_info
        reader->readu1(); // this byte must always be zero.
        reader->readu1(); // this byte must always be zero.

        // 调用方法
        // public static MethodType fromMethodDescriptorString(String descriptor, ClassLoader loader);
        // to get MethodType
        const char *descriptor = CP_NAME_TYPE_TYPE(cp, index);
        auto so = StringObject::newInst(descriptor);
        Class *mt = loadSysClass("java/lang/invoke/MethodType");
        auto fromMethodDescriptorString = mt->getDeclaredStaticMethod(
                "fromMethodDescriptorString",
                "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/invoke/MethodType;");
        slot_t *retValue = execJavaFunc(fromMethodDescriptorString, {(slot_t) so, (slot_t) nullptr});
        assert(retValue != nullptr);
        auto methodTypeObj = (jref) retValue[0]; // get MethodType

        // 调用方法
        // public static Lookup lookup();
        // to get MethodHandles$Lookup
        Class *mh = loadSysClass("java/lang/invoke/MethodHandles");
        auto lookup = mh->getDeclaredStaticMethod("lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
        retValue = execJavaFunc(lookup);
        assert(retValue != nullptr);
        auto lookupObj = (jref) retValue[0]; // get MethodHandles$Lookup

        /////////////////////
        BootstrapMethod &bm = currClass->bootstrapMethods[CP_BOOTSTRAP_METHOD_ATTR_INDEX(currClass->cp, index)];
        const char *name = CP_NAME_TYPE_NAME(cp, index);
        descriptor = CP_NAME_TYPE_TYPE(cp, index);

        auto refKind = CP_METHOD_HANDLE_REFERENCE_KIND(cp, bm.bootstrapMethodRef);
        auto refIndex = CP_METHOD_HANDLE_REFERENCE_INDEX(cp, bm.bootstrapMethodRef);
        switch (refKind) {
            case REF_KIND_GET_FIELD:
                DISPATCH // todo
            case REF_KIND_GET_STATIC:
                DISPATCH // todo
            case REF_KIND_PUT_FIELD:
                DISPATCH // todo
            case REF_KIND_PUT_STATIC:
                DISPATCH // todo
            case REF_KIND_INVOKE_VIRTUAL:
                DISPATCH // todo
            case REF_KIND_INVOKE_STATIC: {
                const char *className = CP_METHOD_CLASS_NAME(cp, refIndex);
                Class *bootstrapClass = currClass->loader->loadClass(className);

                // bootstrap method is static,  todo 对不对
                // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
                // 后续的参数由 ref->argc and ref->args 决定
                Method *bootstrapMethod = bootstrapClass->getDeclaredStaticMethod(
                                CP_METHOD_NAME(cp, refIndex), CP_METHOD_TYPE(cp, refIndex));
                assert(bm.bootstrapArguments.size() + 3 == bootstrapMethod->arg_slot_count);
                retValue = execJavaFunc(bootstrapMethod, {}); // todo 参数是啥？？ bm.bootstrapArguments ???
                assert(retValue != nullptr);
                auto callSet = (jref) retValue;

                // public abstract MethodHandle dynamicInvoker()
                auto dynamicInvoker = callSet->clazz->lookupInstMethod(
                        "dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
                retValue = execJavaFunc(dynamicInvoker, callSet);
                assert(retValue != nullptr);
                auto exactMethodHandle = (jref) retValue;

                // public final Object invokeExact(Object... args) throws Throwable
                Method *exactMethod
                        = exactMethodHandle->clazz->lookupInstMethod("invokeExact", "[Ljava/lang/Object;");
                // todo args
                jvm_abort("");

                // invoke exact method, invokedynamic completely execute over.
                execJavaFunc(exactMethod, {});  // todo 参数是啥？？

                DISPATCH
            }
            case REF_KIND_INVOKE_SPECIAL:
                DISPATCH // todo
            case REF_KIND_NEW_INVOKE_SPECIAL:
                DISPATCH // todo
            case REF_KIND_INVOKE_INTERFACE:
                DISPATCH // todo
            default:
                jvm_abort("never goes here"); // todo never goes here
                break;
        }
        jvm_abort("never goes here"); // todo
    }
__invoke_method:
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
    DISPATCH

opc_new:
    // new指令专门用来创建类实例。数组由专门的指令创建
    // 如果类还没有被初始化，会触发类的初始化。
    Class *c = resolve_class(clazz, reader->readu2());  // todo
    if (!c->inited) {
        c->clinit();
    }

    if (c->isInterface() || c->isAbstract()) {
        raiseException(INSTANTIATION_ERROR);
    }

    // todo java/lang/Class 会在这里创建，为什么会这样，怎么处理
    //    assert(strcmp(c->class_name, "java/lang/Class") == 0);

    frame->pushr(Object::newInst(c));
    DISPATCH

opc_newarray: 
    newarray(frame);
    DISPATCH
opc_anewarray: 
    anewarray(frame);
    DISPATCH

opc_arraylength: 
    Object *o = frame->popr();
    if (o == nullptr) {
        thread_throw_null_pointer_exception();
    }
    if (!o->isArray()) {
        raiseException(UNKNOWN_ERROR, "not a array"); // todo
    }
    frame->pushi(((ArrayObject *) o)->len);
    DISPATCH

opc_athrow:
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
            DISPATCH  // todo
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

opc_checkcast: 
    jref obj = RSLOT(frame->stack - 1); // 不改变操作数栈
    index = reader->readu2();

    // 如果引用是null，则指令执行结束。也就是说，null 引用可以转换成任何类型
    if (obj != nullptr) {
        Class *c = resolve_class(frame->method->clazz, index);
        if (!obj->isInstanceOf(c)) {
            thread_throw_class_cast_exception(obj->clazz->className, c->className);
        }
    }
    DISPATCH

opc_instanceof:
    {
        index = reader->readu2();
        Class *c = resolve_class(clazz, index);

        jref obj = frame->popr();
        if (obj == nullptr)
            frame->pushi(0);
        else
            frame->pushi(obj->isInstanceOf(c) ? 1 : 0);
        DISPATCH
    }
opc_monitorenter:
    frame->popr();
    // todo
    DISPATCH

opc_monitorexit: 
    frame->popr();
    // todo
    DISPATCH

opc_wide: 
    int __opcode = reader->readu1();
    TRACE("%d(0x%x), %s, pc = %lu\n", __opcode, __opcode, instruction_names[__opcode], frame->reader.pc);
    index = reader->readu2();
    switch (__opcode) {
        case OPC_ILOAD:
        case OPC_FLOAD:
        case OPC_ALOAD:
            *frame->stack++ = locals[index];
            break;
        case OPC_LLOAD:
        case OPC_DLOAD:
            *frame->stack++ = locals[index];
            *frame->stack++ = locals[index + 1];
            break;
        case OPC_ISTORE:
        case OPC_FSTORE:
        case OPC_ASTORE:
            locals[index] = *--frame->stack;
            break;
        case OPC_LSTORE:
        case OPC_DSTORE:
            locals[index + 1] = *--frame->stack;
            locals[index] = *--frame->stack;
            break;
        case OPC_RET:
            raiseException(INTERNAL_ERROR, "ret doesn't support after jdk 6.");
            break;
        case OPC_IINC:
            ISLOT(locals + index) = ISLOT(locals + index) + reader->readu2();
            break;
        default:
            // todo never goes here.
            break;
    }
    DISPATCH

opc_multianewarray:
    multianewarray(frame);
    DISPATCH

opc_ifnull: 
    offset = reader->reads2();
    if (frame->popr() == nullptr) {
        reader->skip(offset - 3); // minus instruction length
    }
    DISPATCH

opc_ifnonnull: 
    offset = reader->reads2();
    if (frame->popr() != nullptr) {
        reader->skip(offset - 3); // minus instruction length
    }
    DISPATCH

opc_goto_w: // todo
    raiseException(INTERNAL_ERROR, "goto_w doesn't support");
    DISPATCH
opc_jsr_w: // todo
    raiseException(INTERNAL_ERROR, "jsr_w doesn't support after jdk 6.");
    DISPATCH
opc_breakpoint:
    // todo
    DISPATCH
opc_notused:
    jvm_abort("This instruction isn't used.\n"); // todo
    DISPATCH
opc_invokenative:
    frame->method->nativeMethod(frame);
    DISPATCH
opc_impdep2:
    jvm_abort("This instruction isn't used.\n"); // todo
    DISPATCH
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
