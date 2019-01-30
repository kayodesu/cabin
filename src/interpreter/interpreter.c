/*
 * Author: Jia Yang
 */

#include "../jvm.h"
#include "../rtda/thread/thread.h"
#include "../rtda/thread/frame.h"
#include "../rtda/heap/strpool.h"
#include "../classfile/constant.h"
#include "../rtda/heap/arrobj.h"
#include "../rtda/ma/resolve.h"
#include "../rtda/ma/access.h"

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


/*
 * 标识当前指令是否有 wide 扩展。
 * 只对
 * iload, fload, aload, lload, dload,
 * istore, fstore, astore, lstore, dstore,
 * ret, iinc
 * 有效。
 * 以上指令执行前需检查此标志，执行后需复位（置为false）此标志。
 */
static bool wide_extending = false; // todo 多线程并发访问有问题！！！！！！！！！！

static jint fetch_wide_index(struct frame *frame)
{
    if (wide_extending) {
        wide_extending = false; // recover
        return bcr_readu2(&frame->reader);
    }
    return bcr_readu1(&frame->reader);
}

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
        frame_stack_pushl(frame, CP_LONG(cp, index));//rtcp_get_long(rtcp, index));
    } else if (type == CONSTANT_Double) {
        frame_stack_pushd(frame, CP_DOUBLE(cp, index));//rtcp_get_double(rtcp, index));
    } else {
        jvm_abort("error. %d\n", type);
    }
}

// comparison instructions ---------------------------------------------------------------------------------------------
/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define CMP_RESULT(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

static inline void lcmp(struct frame *frame)
{
    jlong v2 = frame_stack_popl(frame);
    jlong v1 = frame_stack_popl(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void fcmpl(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void fcmpg(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

static inline void dcmpl(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void dcmpg(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

// control instructions ------------------------------------------------------------------------------------------------
static void inline __goto(struct frame *frame)
{
    int offset = bcr_reads2(&frame->reader);
    bcr_skip(&frame->reader, offset - 3);  // minus instruction length
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

// math instructions ---------------------------------------------------------------------------------------------------
static inline void __frem(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    jvm_abort("not implement\n");
//    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
}


static inline void __drem(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    jvm_abort("未实现\n");
//    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
}

static inline void ishl(struct frame *frame)
{
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    jint shift = frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, value << shift);
}

static inline void lshl(struct frame *frame)
{
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, value << shift);
}

// 逻辑右移 shift logical right
static inline void ishr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, (~(((jint)1) >> shift)) & (value >> shift));
}

static inline void lshr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, (~(((jlong)1) >> shift)) & (value >> shift));
}

// 算术右移 shift arithmetic right
static inline void iushr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, value >> shift);
}

static inline void lushr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, value >> shift);
}

static inline void iinc(struct frame *frame)
{
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
}

// extended instructions -----------------------------------------------------------------------------------------------
void multianewarray(struct frame *frame);

static inline void ifnull(struct frame *frame)
{
    int offset = frame_reads2(frame);
    if (frame_stack_popr(frame) == NULL) {
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
    }
}

static inline void ifnonnull(struct frame *frame)
{
    int offset = frame_reads2(frame);
    if (frame_stack_popr(frame) != NULL) {
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
    }
}

// reference instructions ----------------------------------------------------------------------------------------------
void new(struct frame *);
void putstatic(struct frame *);
void getstatic(struct frame *);
void putfield(struct frame *);
void getfield(struct frame *);
void instanceof(struct frame *);
void checkcast(struct frame *);
void invokedynamic(struct frame *);
void newarray(struct frame *);
void anewarray(struct frame *);
void arraylength(struct frame *);
void monitorenter(struct frame *);
void monitorexit(struct frame *);

// --------------------------------------------------------
static slot_t * exec()
{
    struct thread *thread = thread_self();

    struct method *resolved_method;
    slot_t *args;

    struct frame *frame = thread->top_frame;
    print2("executing frame(%p): %s, pc = %lu\n", frame, frame_to_string(frame), frame->reader.pc);

    while (frame_has_more(frame)) {
        u1 opcode = frame_readu1(frame);
                print3("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc);

        switch (opcode) {
            case 0x00:
                break; // nop

                // Constants
            case 0x01:
                frame_stack_pushr(frame, NULL);
                break; // aconst_null

            case 0x02:
                frame_stack_pushi(frame, -1);
                break;   // iconst_m1
            case 0x03:
                frame_stack_pushi(frame, 0);
                break; // iconst_0
            case 0x04:
                frame_stack_pushi(frame, 1);
                break; // iconst_1
            case 0x05:
                frame_stack_pushi(frame, 2);
                break; // iconst_2
            case 0x06:
                frame_stack_pushi(frame, 3);
                break; // iconst_3
            case 0x07:
                frame_stack_pushi(frame, 4);
                break; // iconst_4
            case 0x08:
                frame_stack_pushi(frame, 5);
                break; // iconst_5

            case 0x09:
                frame_stack_pushl(frame, 0);
                break; // lconst_0
            case 0x0a:
                frame_stack_pushl(frame, 1);
                break; // lconst_1

            case 0x0b:
                frame_stack_pushf(frame, 0);
                break; // fconst_0
            case 0x0c:
                frame_stack_pushf(frame, 1);
                break; // fconst_1
            case 0x0d:
                frame_stack_pushf(frame, 2);
                break; // fconst_2

            case 0x0e:
                frame_stack_pushd(frame, 0);
                break; // dconst_0
            case 0x0f:
                frame_stack_pushd(frame, 1);
                break; // dconst_1

            case 0x10:
                frame_stack_pushi(frame, frame_readu1(frame));
                break; // bipush, Byte Integer push
            case 0x11:
                frame_stack_pushi(frame, frame_readu2(frame));
                break; // sipush, Short Integer push
            case 0x12:
                __ldc(frame, frame_readu1(frame));
                break; // ldc
            case 0x13:
                __ldc(frame, frame_readu2(frame));
                break; // ldc_w
            case 0x14:
                ldc2_w(frame);
                break; // ldc2_w

                // Loads
#undef ILOAD
#undef LLOAD
#undef FLOAD
#undef DLOAD
#undef ALOAD
#define ILOAD(index) frame_stack_pushi(frame, ISLOT(frame->locals + index))
#define LLOAD(index) frame_stack_pushl(frame, LSLOT(frame->locals + index))
#define FLOAD(index) frame_stack_pushf(frame, FSLOT(frame->locals + index))
#define DLOAD(index) frame_stack_pushd(frame, DSLOT(frame->locals + index))
#define ALOAD(index) frame_stack_pushr(frame, RSLOT(frame->locals + index))
            case 0x15:
                ILOAD(fetch_wide_index(frame));
                break; // iload
            case 0x16:
                LLOAD(fetch_wide_index(frame));
                break; // lload
            case 0x17:
                FLOAD(fetch_wide_index(frame));
                break; // fload
            case 0x18:
                DLOAD(fetch_wide_index(frame));
                break; // dload
            case 0x19:
                ALOAD(fetch_wide_index(frame));
                break; // aload

            case 0x1a:
                ILOAD(0);
                break; // iload_0
            case 0x1b:
                ILOAD(1);
                break; // iload_1
            case 0x1c:
                ILOAD(2);
                break; // iload_2
            case 0x1d:
                ILOAD(3);
                break; // iload_3

            case 0x1e:
                LLOAD(0);
                break; // lload_0
            case 0x1f:
                LLOAD(1);
                break; // lload_1
            case 0x20:
                LLOAD(2);
                break; // lload_2
            case 0x21:
                LLOAD(3);
                break; // lload_3

            case 0x22:
                FLOAD(0);
                break; // fload_0
            case 0x23:
                FLOAD(1);
                break; // fload_1
            case 0x24:
                FLOAD(2);
                break; // fload_2
            case 0x25:
                FLOAD(3);
                break; // fload_3

            case 0x26:
                DLOAD(0);
                break; // dload_0
            case 0x27:
                DLOAD(1);
                break; // dload_1
            case 0x28:
                DLOAD(2);
                break; // dload_2
            case 0x29:
                DLOAD(3);
                break; // dload_3

            case 0x2a:
                ALOAD(0);
                break; // aload_0
            case 0x2b:
                ALOAD(1);
                break; // aload_1
            case 0x2c:
                ALOAD(2);
                break; // aload_2
            case 0x2d:
                ALOAD(3);
                break; // aload_3

            case 0x2e:
                frame_iaload(frame);
                break; // iaload
            case 0x2f:
                frame_laload(frame);
                break; // laload
            case 0x30:
                frame_faload(frame);
                break; // faload
            case 0x31:
                frame_daload(frame);
                break; // daload
            case 0x32:
                frame_aaload(frame);
                break; // aaload
            case 0x33:
                frame_baload(frame);
                break; // baload
            case 0x34:
                frame_caload(frame);
                break; // caload
            case 0x35:
                frame_saload(frame);
                break; // saload

                // Stores
#undef ISTORE
#undef LSTORE
#undef FSTORE
#undef DSTORE
#undef ASTORE
#define ISTORE(index) ISLOT(frame->locals + index) = frame_stack_popi(frame)
#define LSTORE(index) LSLOT(frame->locals + index) = frame_stack_popl(frame)
#define FSTORE(index) FSLOT(frame->locals + index) = frame_stack_popf(frame)
#define DSTORE(index) DSLOT(frame->locals + index) = frame_stack_popd(frame)
#define ASTORE(index) RSLOT(frame->locals + index) = frame_stack_popr(frame)
            case 0x36:
                ISTORE(fetch_wide_index(frame));
                break; // istore
            case 0x37:
                LSTORE(fetch_wide_index(frame));
                break; // lstore
            case 0x38:
                FSTORE(fetch_wide_index(frame));
                break; // fstore
            case 0x39:
                DSTORE(fetch_wide_index(frame));
                break; // dstore
            case 0x3a:
                ASTORE(fetch_wide_index(frame));
                break; // astore

            case 0x3b:
                ISTORE(0);
                break; // istore_0
            case 0x3c:
                ISTORE(1);
                break; // istore_1
            case 0x3d:
                ISTORE(2);
                break; // istore_2
            case 0x3e:
                ISTORE(3);
                break; // istore_3

            case 0x3f:
                LSTORE(0);
                break; // lstore_0
            case 0x40:
                LSTORE(1);
                break; // lstore_1
            case 0x41:
                LSTORE(2);
                break; // lstore_2
            case 0x42:
                LSTORE(3);
                break; // lstore_3

            case 0x43:
                FSTORE(0);
                break; // fstore_0
            case 0x44:
                FSTORE(1);
                break; // fstore_1
            case 0x45:
                FSTORE(2);
                break; // fstore_2
            case 0x46:
                FSTORE(3);
                break; // fstore_3

            case 0x47:
                DSTORE(0);
                break; // dstore_0
            case 0x48:
                DSTORE(1);
                break; // dstore_1
            case 0x49:
                DSTORE(2);
                break; // dstore_2
            case 0x4a:
                DSTORE(3);
                break; // dstore_3

            case 0x4b:
                ASTORE(0);
                break; // astore_0
            case 0x4c:
                ASTORE(1);
                break; // astore_1
            case 0x4d:
                ASTORE(2);
                break; // astore_2
            case 0x4e:
                ASTORE(3);
                break; // astore_3

            case 0x4f:
                frame_iastore(frame);
                break; // iastore
            case 0x50:
                frame_lastore(frame);
                break; // lastore
            case 0x51:
                frame_fastore(frame);
                break; // fastore
            case 0x52:
                frame_dastore(frame);
                break; // dastore
            case 0x53:
                frame_aastore(frame);
                break; // aastore
            case 0x54:
                frame_bastore(frame);
                break; // bastore
            case 0x55:
                frame_castore(frame);
                break; // castore
            case 0x56:
                frame_sastore(frame);
                break; // sastore

                // Stack
            case 0x57:
                frame->stack--;
                break;    // pop
            case 0x58:
                frame->stack -= 2;
                break; // pop2
            case 0x59:
                frame->stack[0] = frame->stack[-1];
                frame->stack++;
//                dup(frame);
                break;            // dup
            case 0x5a:
                frame->stack[0] = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = frame->stack[0];
                frame->stack++;
//                dup_x1(frame);
                break;         // dup_x1
            case 0x5b:
                frame->stack[0] = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = frame->stack[-3];
                frame->stack[-3] = frame->stack[0];
                frame->stack++;
//                dup_x2(frame);
                break;         // dup_x2
            case 0x5c:
                frame->stack[0] = frame->stack[-2];
                frame->stack[1] = frame->stack[-1];
                frame->stack += 2;
//                dup2(frame);
                break;          // dup2
            case 0x5d:
                // ..., value3, value2, value1 →
                // ..., value2, value1, value3, value2, value1
                frame->stack[1] = frame->stack[-1];
                frame->stack[0] = frame->stack[-2];
                frame->stack[-1] = frame->stack[-3];
                frame->stack[-2] = frame->stack[1];
                frame->stack[-3] = frame->stack[0];
                frame->stack += 2;
//                dup2_x1(frame);
                break;        // dup2_x1
            case 0x5e:
                // ..., value4, value3, value2, value1 →
                // ..., value2, value1, value4, value3, value2, value1
                frame->stack[1] = frame->stack[-1];
                frame->stack[0] = frame->stack[-2];
                frame->stack[-1] = frame->stack[-3];
                frame->stack[-2] = frame->stack[-4];
                frame->stack[-3] = frame->stack[1];
                frame->stack[-4] = frame->stack[0];
                frame->stack += 2;
//                dup2_x2(frame);
                break;        // dup2_x2
            case 0x5f: {
                slot_t tmp = frame->stack[-1];
                frame->stack[-1] = frame->stack[-2];
                frame->stack[-2] = tmp;
//                __swap(frame);
                break;
            } // swap

                // Math
#undef BINARY_OP
#define BINARY_OP(type, oper) \
do { \
frame->stack -= SLOTS(type);\
((type *) frame->stack)[-1] = ((type *) frame->stack)[-1] oper ((type *) frame->stack)[0]; \
} while(false)
            case 0x60:
                BINARY_OP(jint, +);
                break; // iadd
            case 0x61:
                BINARY_OP(jlong, +);
                break; // ladd
            case 0x62:
                BINARY_OP(jfloat, +);
                break;  // fadd
            case 0x63:
                BINARY_OP(jdouble, +);
                break; // dadd

            case 0x64:
                BINARY_OP(jint, -);
                break;    // isub
            case 0x65:
                BINARY_OP(jlong, -);
                break;   // lsub
            case 0x66:
                BINARY_OP(jfloat, -);
                break;  // fsub
            case 0x67:
                BINARY_OP(jdouble, -);
                break; // dsub

            case 0x68:
                BINARY_OP(jint, *);
                break;    // imul
            case 0x69:
                BINARY_OP(jlong, *);
                break;   // lmul
            case 0x6a:
                BINARY_OP(jfloat, *);
                break;  // fmul
            case 0x6b:
                BINARY_OP(jdouble, *);
                break; // dmul

            case 0x6c:
                BINARY_OP(jint, /);
                break;    // idiv
            case 0x6d:
                BINARY_OP(jlong, /);
                break;   // ldiv
            case 0x6e:
                BINARY_OP(jfloat, /);
                break;  // fdiv
            case 0x6f:
                BINARY_OP(jdouble, /);
                break; // ddiv

            case 0x70:
                BINARY_OP(jint, %);
                break;  // irem
            case 0x71:
                BINARY_OP(jlong, %);
                break; // lrem
            case 0x72:
                __frem(frame);
                break; // frem
            case 0x73:
                __drem(frame);
                break; // drem

            case 0x74:
                frame_stack_pushi(frame, -frame_stack_popi(frame));
                break; // ineg
            case 0x75:
                frame_stack_pushl(frame, -frame_stack_popl(frame));
                break; // lneg
            case 0x76:
                frame_stack_pushf(frame, -frame_stack_popf(frame));
                break; // fneg
            case 0x77:
                frame_stack_pushd(frame, -frame_stack_popd(frame));
                break; // dneg

            case 0x78:
                ishl(frame);
                break; // ishl
            case 0x79:
                lshl(frame);
                break; // lshl

            case 0x7a:
                ishr(frame);
                break; // ishr
            case 0x7b:
                lshr(frame);
                break; // lshr

            case 0x7c:
                iushr(frame);
                break;  // iushr
            case 0x7d:
                lushr(frame);
                break;  // lushr

            case 0x7e:
                BINARY_OP(jint, &);
                break; // iand
            case 0x7f:
                BINARY_OP(jlong, &);
                break; // land

            case 0x80:
                BINARY_OP(jint, |);
                break;  // ior
            case 0x81:
                BINARY_OP(jlong, |);
                break;  // lor

            case 0x82:
                BINARY_OP(jint, ^);
                break; // ixor
            case 0x83:
                BINARY_OP(jlong, ^);
                break; // lxor

            case 0x84:
                iinc(frame);
                break; // iinc

                // Conversions
#undef x2y
#define x2y(x, y) frame_stack_push##y(frame, x##2##y(frame_stack_pop##x(frame)))
            case 0x85:
                x2y(i, l);
                break; // i2l
            case 0x86:
                x2y(i, f);
                break; // i2f
            case 0x87:
                x2y(i, d);
                break; // i2d

            case 0x88:
                x2y(l, i);
                break; // l2i
            case 0x89:
                x2y(l, f);
                break; // l2f
            case 0x8a:
                x2y(l, d);
                break; // l2d

            case 0x8b:
                x2y(f, i);
                break; // f2i
            case 0x8c:
                x2y(f, l);
                break; // f2l
            case 0x8d:
                x2y(f, d);
                break; // f2d

            case 0x8e:
                x2y(d, i);
                break; // d2i
            case 0x8f:
                x2y(d, l);
                break; // d2l
            case 0x90:
                x2y(d, f);
                break; // d2f

            case 0x91:
                frame_stack_pushi(frame, i2b(frame_stack_popi(frame)));
                break; // i2b
            case 0x92:
                frame_stack_pushi(frame, i2c(frame_stack_popi(frame)));
                break; // i2c
            case 0x93:
                frame_stack_pushi(frame, i2s(frame_stack_popi(frame)));
                break; // i2s

                // Comparisons
            case 0x94:
                lcmp(frame);
                break; // lcmp
            case 0x95:
                fcmpl(frame);
                break; // fcmpl
            case 0x96:
                fcmpg(frame);
                break; // fcmpg
            case 0x97:
                dcmpl(frame);
                break; // dcmpl
            case 0x98:
                dcmpg(frame);
                break; // dcmpg

#undef IF_COND
#define IF_COND(cond) \
do { \
jint v = frame_stack_popi(frame); \
jint offset = frame_reads2(frame); \
if (v cond 0) \
    frame_skip(frame, offset - 3);  /* minus instruction length */ \
} while(false)
            case 0x99:
                IF_COND(==);
                break; // ifeq
            case 0x9a:
                IF_COND(!=);
                break; // ifne
            case 0x9b:
                IF_COND(<);
                break; // iflt
            case 0x9c:
                IF_COND(>=);
                break; // ifge
            case 0x9d:
                IF_COND(>);
                break; // ifgt
            case 0x9e:
                IF_COND(<=);
                break; // ifle

#undef IF_ICMP_COND
#define IF_ICMP_COND(cond) \
do { \
frame->stack -= 2;\
jint offset = frame_reads2(frame); \
if (ISLOT(frame->stack) cond ISLOT(frame->stack + 1)) \
    frame_skip(frame, offset - 3); /* minus instruction length */ \
} while(false)
            case 0x9f:
                IF_ICMP_COND(==);
                break; // if_icmpeq
            case 0xa0:
                IF_ICMP_COND(!=);
                break; // if_icmpne
            case 0xa1:
                IF_ICMP_COND(<);
                break; // if_icmplt
            case 0xa2:
                IF_ICMP_COND(>=);
                break; // if_icmpge
            case 0xa3:
                IF_ICMP_COND(>);
                break; // if_icmpgt
            case 0xa4:
                IF_ICMP_COND(<=);
                break; // if_icmple

#undef IF_ACMP_COND
#define IF_ACMP_COND(cond) \
do { \
frame->stack -= 2;\
jint offset = frame_reads2(frame); \
if (RSLOT(frame->stack) cond RSLOT(frame->stack + 1)) \
    frame_skip(frame, offset - 3);  /* minus instruction length */ \
} while (false)
            case 0xa5: // if_acmpeq
                IF_ACMP_COND(==);
                break;
            case 0xa6: // if_acmpne
                IF_ACMP_COND(!=);
                break;

                // Controls
            case 0xa7: // goto
                __goto(frame);
                break;

                // 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
                // 从Java 6开始，已经不再使用这些指令
            case 0xa8: // jsr
                vm_internal_error("jsr doesn't support after jdk 6.");
                break;
            case 0xa9: // ret
                vm_internal_error("ret doesn't support after jdk 6.");
                break;
            case 0xaa: // tableswitch
                tableswitch(frame);
                break;
            case 0xab: // lookupswitch
                lookupswitch(frame);
                break;

#undef TRETURN
#define TRETURN(__type, t, __s) \
{ \
struct frame *invoke_frame = thread->top_frame = frame->prev; \
if (frame->vm_invoke || invoke_frame == NULL) { \
    frame->stack -= (__s); \
    return frame->stack; \
} else { \
    __type value = frame_stack_pop##t(frame); \
    frame_stack_push##t(invoke_frame, value); \
    frame = invoke_frame; \
} \
break; \
}
            case 0xac: TRETURN(jint, i, 1); // ireturn
            case 0xad: TRETURN(jlong, l, 2); // lreturn
            case 0xae: TRETURN(jfloat, f, 1); // freturn
            case 0xaf: TRETURN(jdouble, d, 2); // dreturn
            case 0xb0: TRETURN(jref, r, 1); // areturn
            case 0xb1: { // return
                struct frame *invoke_frame = thread->top_frame = frame->prev;
                if (frame->vm_invoke) {
                    return NULL;
                }

                if (invoke_frame == NULL) {
                    return NULL; // todo
                }

                frame = invoke_frame;
                break;
            }
                // References
            case 0xb2: // getstatic
                getstatic(frame);
                break;
            case 0xb3: // putstatic
                putstatic(frame);
                break;
            case 0xb4: // getfield
                getfield(frame);
                break;
            case 0xb5: // putfield
                putfield(frame);
                break;

            case 0xb6: { // invokevirtual
                // invokevirtual指令用于调用对象的实例方法，根据对象的实际类型进行分派（虚方法分派）。
                struct class *curr_class = frame->method->clazz;

                int index = bcr_readu2(&frame->reader);
                struct method *m = resolve_method(curr_class, index);

                frame->stack -= m->arg_slot_count;
                args = frame->stack;

                struct object *obj = (struct object *) args[0];
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                // 下面这样写对不对 todo
//    if (obj->clazz != curr_class) {
                // 从对象的类中查找真正要调用的方法
                m = class_lookup_method(obj->clazz, m->name, m->descriptor);
//    }

                resolved_method = m;
                goto invoke_method;
            }
            case 0xb7: { // invokespecial
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

                frame->stack -= m->arg_slot_count;
                args = frame->stack;
                jref obj = (jref) args[0];
                if (obj == NULL) {
                    thread_throw_null_pointer_exception();
                }

                resolved_method = m;
                goto invoke_method;
            }
            case 0xb8: { // invokestatic
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
            case 0xb9: { // invokeinterface
                struct class *curr_class = frame->method->clazz;
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
            case 0xba: // invokedynamic
                invokedynamic(frame);
                break;
invoke_method:
{
    struct frame *new_frame = alloc_frame(resolved_method, false);//frame_create(method, args);
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

            case 0xbb: // new
                new(frame);
                break;
            case 0xbc: // newarray
                newarray(frame);
                break;
            case 0xbd: // anewarray
                anewarray(frame);
                break;
            case 0xbe: // arraylength
                arraylength(frame);
                break;

            case 0xbf: { // athrow
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
            case 0xc0:
                checkcast(frame);
                break;    // checkcast
            case 0xc1:
                instanceof(frame);
                break;   // instanceof
            case 0xc2:
                monitorenter(frame);
                break; // monitorenter
            case 0xc3:
                monitorexit(frame);
                break;  // monitorexit

                // Extended
            case 0xc4:
                wide_extending = true;
                break; // wide
            case 0xc5:
                multianewarray(frame);
                break; // multianewarray
            case 0xc6:
                ifnull(frame);
                break; // ifnull
            case 0xc7:
                ifnonnull(frame);
                break; // ifnonnull
            case 0xc8:
                vm_internal_error("goto_w doesn't support");
                break; // goto_w  // todo
            case 0xc9:
                vm_internal_error("jsr_w doesn't support after jdk 6."); // jsr_w // todo

                // Reserved
            case 0xca:
                jvm_abort("debugger used instructions. %s\n", frame_to_string(frame));
                break; // breakpoint, // debugger used instruction// todo
            case 0xfe:
                frame->method->native_method(frame);
                break; // jvm used instruction，本 jvm 用来调用本地方法。
            case 0xff:
                jvm_abort("jvm used instruction, not used in this jvm. %s", frame_to_string(frame)); // todo
            default:
//                jvm_abort("This instruction isn't used. %d, %s\n", opcode, frame_to_string(frame));
                jvm_abort("This instruction isn't used. %d(0x%x)\n", opcode, opcode); // todo
                return NULL;
        }
    }

    jvm_abort("Never goes here!");
}

/*
 * execute a Java function
 */
slot_t *exec_java_func(struct method *method, const slot_t *args, bool vm_invoke)
{
//    if (IS_NATIVE(method->access_flags)) {
//        printvm("\n");
//    }
    struct frame *frame = alloc_frame(method, vm_invoke);
    if (method->arg_slot_count > 0 && args == NULL) {
        jvm_abort("do not find args, %d\n", method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
//        frame_locals_set(frame, i, args + i);
        frame->locals[i] = args[i];
    }

    return exec();
}
