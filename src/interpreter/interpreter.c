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
//    struct rtcp *rtcp = frame->m.method->clazz->rtcp;
//    u1 type = rtcp_get_type(rtcp, index);
    struct constant_pool *cp = &frame->m.method->clazz->constant_pool;
    resolve_single_constant(frame->m.method->clazz, index);
    u1 type = CP_TYPE(cp, index);

    if (type == CONSTANT_Integer) {
        frame_stack_pushi(frame, CP_INT(cp, index));//rtcp_get_int(rtcp, index));
    } else if (type == CONSTANT_Float) {
        frame_stack_pushf(frame, CP_FLOAT(cp, index));//rtcp_get_float(rtcp, index));
    } else if (type == CONSTANT_ResolvedString) {
//        const char *str = CP_STRING(cp, index);//rtcp_get_str(rtcp, index);
//        struct object *so = get_str_from_pool(frame->m.method->clazz->loader, str);
        frame_stack_pushr(frame, (jref) CP_INFO(cp, index));
    } else if (type == CONSTANT_ResolvedClass) {
//        const char *class_name = CP_CLASS_NAME(cp, index);//rtcp_get_class_name(rtcp, index);
        struct class *c = (struct class *) CP_INFO(cp, index);//classloader_load_class(frame->m.method->clazz->loader, class_name);
        frame_stack_pushr(frame, c->clsobj);
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

void ldc2_w(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
//    struct rtcp *rtcp = frame->m.method->clazz->rtcp;
//    u1 type = rtcp_get_type(rtcp, index);
    struct constant_pool *cp = &frame->m.method->clazz->constant_pool;
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
    jint shift =frame_stack_popi(frame) & 0x1f;
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

    struct slot *s = frame_locals_get(frame, index);
    if (s->t != JINT) {
        VM_UNKNOWN_ERROR("type mismatch. wants %d, but gets %d", JINT, s->t);
    }

    s->v.i += value;
}

// stack instructions --------------------------------------------------------------------------------------------------
// 复制栈顶数值（只支持分类一的数据）并将复制值压入栈顶。
static inline void dup(struct frame *frame)
{
    assert(frame != NULL);
    assert(!frame_stack_empty(frame));
    frame->stack[frame->stack_top + 1] = frame->stack[frame->stack_top];
    frame->stack_top++;
}

/*
 * Duplicate the top value on the operand stack and insert the
 * duplicated value two values down in the operand stack.
 *
 * The dup_x1 instruction must not be used unless both value1 and
 * value2 are values of a category 1 computational type
 */
static inline void dup_x1(struct frame *frame)
{
    assert(frame != NULL);
    frame->stack[frame->stack_top + 1] = frame->stack[frame->stack_top];
    frame->stack[frame->stack_top] = frame->stack[frame->stack_top - 1];
    frame->stack[frame->stack_top - 1] = frame->stack[frame->stack_top + 1];
    frame->stack_top++;
}

// 复制操作数栈栈顶的值（类型一），
// 并插入到栈顶以下2个（栈顶类型一，栈顶下一个类型二）或3个（三个值都是类型一）值之后
static inline void dup_x2(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, -1), FSFT(frame, -2), 3 * sizeof(*(frame->stack)));
    frame->stack[frame->stack_top - 2] = frame->stack[frame->stack_top + 1];
    frame->stack_top++;

}

// 复制栈顶一个（分类二类型)或两个（分类一类型）数值并将复制值压入栈顶。
static inline void dup2(struct frame *frame)
{
    assert(frame != NULL);
    memcpy(FSFT(frame, 1), FSFT(frame, -1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
    dup_x1指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个或3个值之后。

    Form 1:
    ..., value3, value2, value1 →
    ..., value2, value1, value3, value2, value1
    where value1, value2, and value3 are all values of a category 1 computational type.

    Form 2:
    ..., value2, value1 →
    ..., value1, value2, value1
    where value1 is a value of a category 2 computational type
    and value2 is a value of a category 1 computational type.
 */
static inline void dup2_x1(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, 0), FSFT(frame, -2), 3 * sizeof(*(frame->stack)));
    memmove(FSFT(frame, -2), FSFT(frame, 1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
    dup_x2指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个、3个或者4个值之后

    Form 1:
    ..., value4, value3, value2, value1 →
    ..., value2, value1, value4, value3, value2, value1
    where value1, value2, value3, and value4 are all values of a category 1 computational type.

    Form 2:
    ..., value3, value2, value1 →
    ..., value1, value3, value2, value1
    where value1 is a value of a category 2 computational type and
    value2 and value3 are both values of a category 1 computational type.

    Form 3:
    ..., value3, value2, value1 →
    ..., value2, value1, value3, value2, value1
    where value1 and value2 are both values of a category 1
    computational type and value3 is a value of a category 2 computational type.

    Form 4:
    ..., value2, value1 →
    ..., value1, value2, value1
    where value1 and value2 are both values of a category 2 computational type.
 */
static inline void dup2_x2(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, -1), FSFT(frame, -3), 3 * sizeof(*(frame->stack)));
    memmove(FSFT(frame, -3), FSFT(frame, 1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
 * 交换操作数栈顶的两个值（都为类型一）
 *
 * The Java Virtual Machine does not provide an instruction
 * implementing a swap on operands of category 2 computational types.
 */
static inline void __swap(struct frame *frame)
{
    assert(frame != NULL);
    const struct slot top1 = *frame_stack_pop_slot(frame);
    const struct slot top2 = *frame_stack_pop_slot(frame);

    frame_stack_push_slot_directly(frame, &top1);
    frame_stack_push_slot_directly(frame, &top2);
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
void athrow(struct frame *);
void instanceof(struct frame *);
void checkcast(struct frame *);
void invokestatic(struct frame *);
void invokespecial(struct frame *);
void invokevirtual(struct frame *);
void invokeinterface(struct frame *);
void invokedynamic(struct frame *);
void newarray(struct frame *);
void anewarray(struct frame *);
void arraylength(struct frame *);
void monitorenter(struct frame *);
void monitorexit(struct frame *);

// --------------------------------------------------------
void* interpret(void *thread0)
{
    assert(thread0 != NULL);
    struct thread *thread = thread0;

    while (!thread_is_stack_empty(thread)) {
        struct frame *frame = thread_top_frame(thread);
        if (frame->type == SF_TYPE_SHIM) {
            if (frame->m.shim_action != NULL) {
                frame->m.shim_action(frame);
            }

            thread_pop_frame(thread);
            frame_destroy(frame);
            print2("shim frame(%p) exe over, destroy.\n", frame);
            continue;
        }

                print2("executing frame(%p): %s, pc = %lu\n", frame, frame_to_string(frame), frame->reader.pc);
        while (frame_has_more(frame)) {
            u1 opcode = frame_readu1(frame);

                    print3("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instruction_names[opcode], frame->reader.pc);

            switch (opcode) {
                case 0x00: break; // nop

                // Constants
                case 0x01: frame_stack_pushr(frame, NULL); break; // aconst_null

                case 0x02: frame_stack_pushi(frame, -1); break;   // iconst_m1
                case 0x03: frame_stack_pushi(frame, 0); break; // iconst_0
                case 0x04: frame_stack_pushi(frame, 1); break; // iconst_1
                case 0x05: frame_stack_pushi(frame, 2); break; // iconst_2
                case 0x06: frame_stack_pushi(frame, 3); break; // iconst_3
                case 0x07: frame_stack_pushi(frame, 4); break; // iconst_4
                case 0x08: frame_stack_pushi(frame, 5); break; // iconst_5

                case 0x09: frame_stack_pushl(frame, 0); break; // lconst_0
                case 0x0a: frame_stack_pushl(frame, 1); break; // lconst_1

                case 0x0b: frame_stack_pushf(frame, 0); break; // fconst_0
                case 0x0c: frame_stack_pushf(frame, 1); break; // fconst_1
                case 0x0d: frame_stack_pushf(frame, 2); break; // fconst_2

                case 0x0e: frame_stack_pushd(frame, 0); break; // dconst_0
                case 0x0f: frame_stack_pushd(frame, 1); break; // dconst_1

                case 0x10: frame_stack_pushi(frame, frame_readu1(frame)); break; // bipush, Byte Integer push
                case 0x11: frame_stack_pushi(frame, frame_readu2(frame)); break; // sipush, Short Integer push
                case 0x12: __ldc(frame, frame_readu1(frame)); break; // ldc
                case 0x13: __ldc(frame, frame_readu2(frame)); break; // ldc_w
                case 0x14: ldc2_w(frame); break; // ldc2_w

                // Loads
                case 0x15: frame_iload(frame, fetch_wide_index(frame)); break; // iload
                case 0x16: frame_lload(frame, fetch_wide_index(frame)); break; // lload
                case 0x17: frame_fload(frame, fetch_wide_index(frame)); break; // fload
                case 0x18: frame_dload(frame, fetch_wide_index(frame)); break; // dload
                case 0x19: frame_aload(frame, fetch_wide_index(frame)); break; // aload

                case 0x1a: frame_iload(frame, 0); break; // iload_0
                case 0x1b: frame_iload(frame, 1); break; // iload_1
                case 0x1c: frame_iload(frame, 2); break; // iload_2
                case 0x1d: frame_iload(frame, 3); break; // iload_3

                case 0x1e: frame_lload(frame, 0); break; // lload_0
                case 0x1f: frame_lload(frame, 1); break; // lload_1
                case 0x20: frame_lload(frame, 2); break; // lload_2
                case 0x21: frame_lload(frame, 3); break; // lload_3

                case 0x22: frame_fload(frame, 0); break; // fload_0
                case 0x23: frame_fload(frame, 1); break; // fload_1
                case 0x24: frame_fload(frame, 2); break; // fload_2
                case 0x25: frame_fload(frame, 3); break; // fload_3

                case 0x26: frame_dload(frame, 0); break; // dload_0
                case 0x27: frame_dload(frame, 1); break; // dload_1
                case 0x28: frame_dload(frame, 2); break; // dload_2
                case 0x29: frame_dload(frame, 3); break; // dload_3

                case 0x2a: frame_aload(frame, 0); break; // aload_0
                case 0x2b: frame_aload(frame, 1); break; // aload_1
                case 0x2c: frame_aload(frame, 2); break; // aload_2
                case 0x2d: frame_aload(frame, 3); break; // aload_3

                case 0x2e: frame_iaload(frame); break; // iaload
                case 0x2f: frame_laload(frame); break; // laload
                case 0x30: frame_faload(frame); break; // faload
                case 0x31: frame_daload(frame); break; // daload
                case 0x32: frame_aaload(frame); break; // aaload
                case 0x33: frame_baload(frame); break; // baload
                case 0x34: frame_caload(frame); break; // caload
                case 0x35: frame_saload(frame); break; // saload

                // Stores
                case 0x36: frame_istore(frame, fetch_wide_index(frame)); break; // istore
                case 0x37: frame_lstore(frame, fetch_wide_index(frame)); break; // lstore
                case 0x38: frame_fstore(frame, fetch_wide_index(frame)); break; // fstore
                case 0x39: frame_dstore(frame, fetch_wide_index(frame)); break; // dstore
                case 0x3a: frame_astore(frame, fetch_wide_index(frame)); break; // astore

                case 0x3b: frame_istore(frame, 0); break; // istore_0
                case 0x3c: frame_istore(frame, 1); break; // istore_1
                case 0x3d: frame_istore(frame, 2); break; // istore_2
                case 0x3e: frame_istore(frame, 3); break; // istore_3

                case 0x3f: frame_lstore(frame, 0); break; // lstore_0
                case 0x40: frame_lstore(frame, 1); break; // lstore_1
                case 0x41: frame_lstore(frame, 2); break; // lstore_2
                case 0x42: frame_lstore(frame, 3); break; // lstore_3

                case 0x43: frame_fstore(frame, 0); break; // fstore_0
                case 0x44: frame_fstore(frame, 1); break; // fstore_1
                case 0x45: frame_fstore(frame, 2); break; // fstore_2
                case 0x46: frame_fstore(frame, 3); break; // fstore_3

                case 0x47: frame_dstore(frame, 0); break; // dstore_0
                case 0x48: frame_dstore(frame, 1); break; // dstore_1
                case 0x49: frame_dstore(frame, 2); break; // dstore_2
                case 0x4a: frame_dstore(frame, 3); break; // dstore_3

                case 0x4b: frame_astore(frame, 0); break; // astore_0
                case 0x4c: frame_astore(frame, 1); break; // astore_1
                case 0x4d: frame_astore(frame, 2); break; // astore_2
                case 0x4e: frame_astore(frame, 3); break; // astore_3

                case 0x4f: frame_iastore(frame); break; // iastore
                case 0x50: frame_lastore(frame); break; // lastore
                case 0x51: frame_fastore(frame); break; // fastore
                case 0x52: frame_dastore(frame); break; // dastore
                case 0x53: frame_aastore(frame); break; // aastore
                case 0x54: frame_bastore(frame); break; // bastore
                case 0x55: frame_castore(frame); break; // castore
                case 0x56: frame_sastore(frame); break; // sastore

                // Stack
                case 0x57: frame->stack_top--; break;    // pop
                case 0x58: frame->stack_top -= 2; break; // pop2
                case 0x59: dup(frame); break;            // dup
                case 0x5a: dup_x1(frame); break;         // dup_x1
                case 0x5b: dup_x2(frame); break;         // dup_x2
                case 0x5c: dup2(frame);  break;          // dup2
                case 0x5d: dup2_x1(frame); break;        // dup2_x1
                case 0x5e: dup2_x2(frame); break;        // dup2_x2
                case 0x5f: __swap(frame); break;         // swap

                // Math
#undef CAL
#define CAL(type, t, oper) \
    do { \
        type v2 = frame_stack_pop##t(frame); \
        type v1 = frame_stack_pop##t(frame); \
        frame_stack_push##t(frame, v1 oper v2); /* todo 相加溢出的问题 */ \
    } while(false)
                case 0x60: CAL(jint, i, +); break;    // iadd
                case 0x61: CAL(jlong, l, +); break;   // ladd
                case 0x62: CAL(jfloat, f, +); break;  // fadd
                case 0x63: CAL(jdouble, d, +); break; // dadd

                case 0x64: CAL(jint, i, -); break;    // isub
                case 0x65: CAL(jlong, l, -); break;   // lsub
                case 0x66: CAL(jfloat, f, -); break;  // fsub
                case 0x67: CAL(jdouble, d, -); break; // dsub

                case 0x68: CAL(jint, i, *); break;    // imul
                case 0x69: CAL(jlong, l, *); break;   // lmul
                case 0x6a: CAL(jfloat, f, *); break;  // fmul
                case 0x6b: CAL(jdouble, d, *); break; // dmul

                case 0x6c: CAL(jint, i, /); break;    // idiv
                case 0x6d: CAL(jlong, l, /); break;   // ldiv
                case 0x6e: CAL(jfloat, f, /); break;  // fdiv
                case 0x6f: CAL(jdouble, d, /); break; // ddiv

                case 0x70: CAL(jint, i, %); break;  // irem
                case 0x71: CAL(jlong, l, %); break; // lrem
                case 0x72: __frem(frame); break; // frem
                case 0x73: __drem(frame); break; // drem

                case 0x74: frame_stack_pushi(frame, -frame_stack_popi(frame)); break; // ineg
                case 0x75: frame_stack_pushl(frame, -frame_stack_popl(frame)); break; // lneg
                case 0x76: frame_stack_pushf(frame, -frame_stack_popf(frame)); break; // fneg
                case 0x77: frame_stack_pushd(frame, -frame_stack_popd(frame)); break; // dneg

                case 0x78: ishl(frame); break; // ishl
                case 0x79: lshl(frame); break; // lshl

                case 0x7a: ishr(frame); break; // ishr
                case 0x7b: lshr(frame); break; // lshr

                case 0x7c: iushr(frame); break;  // iushr
                case 0x7d: lushr(frame); break;  // lushr

                case 0x7e: CAL(jint, i, &); break; // iand
                case 0x7f: CAL(jlong, l, &); break; // land

                case 0x80: CAL(jint, i, |); break;  // ior
                case 0x81: CAL(jlong, l, |); break;  // lor

                case 0x82: CAL(jint, i, ^); break; // ixor
                case 0x83: CAL(jlong, l, ^); break; // lxor

                case 0x84: iinc(frame); break; // iinc

                // Conversions
#undef x2y
#define x2y(x, y) frame_stack_push##y(frame, x##2##y(frame_stack_pop##x(frame)))
                case 0x85: x2y(i, l); break; // i2l
                case 0x86: x2y(i, f); break; // i2f
                case 0x87: x2y(i, d); break; // i2d

                case 0x88: x2y(l, i); break; // l2i
                case 0x89: x2y(l, f); break; // l2f
                case 0x8a: x2y(l, d); break; // l2d

                case 0x8b: x2y(f, i); break; // f2i
                case 0x8c: x2y(f, l); break; // f2l
                case 0x8d: x2y(f, d); break; // f2d

                case 0x8e: x2y(d, i); break; // d2i
                case 0x8f: x2y(d, l); break; // d2l
                case 0x90: x2y(d, f); break; // d2f

                case 0x91: frame_stack_pushi(frame, i2b(frame_stack_popi(frame))); break; // i2b
                case 0x92: frame_stack_pushi(frame, i2c(frame_stack_popi(frame))); break; // i2c
                case 0x93: frame_stack_pushi(frame, i2s(frame_stack_popi(frame))); break; // i2s

                // Comparisons
                case 0x94: lcmp(frame); break; // lcmp
                case 0x95: fcmpl(frame); break; // fcmpl
                case 0x96: fcmpg(frame); break; // fcmpg
                case 0x97: dcmpl(frame); break; // dcmpl
                case 0x98: dcmpg(frame); break; // dcmpg

#undef IF_COND
#define IF_COND(cond) \
    do { \
        jint v = frame_stack_popi(frame); \
        jint offset = frame_reads2(frame); \
        if (v cond 0) \
            frame_skip(frame, offset - 3);  /* minus instruction length */ \
    } while(false)
                case 0x99: IF_COND(==); break; // ifeq
                case 0x9a: IF_COND(!=); break; // ifne
                case 0x9b: IF_COND(<);  break; // iflt
                case 0x9c: IF_COND(>=); break; // ifge
                case 0x9d: IF_COND(>);  break; // ifgt
                case 0x9e: IF_COND(<=); break; // ifle

#undef IF_ICMP_COND
#define IF_ICMP_COND(cond) \
    do { \
        jint v2 = frame_stack_popi(frame); \
        jint v1 = frame_stack_popi(frame); \
        jint offset = frame_reads2(frame); \
        if (v1 cond v2) \
            frame_skip(frame, offset - 3); /* minus instruction length */ \
    } while(false)
                case 0x9f: IF_ICMP_COND(==); break; // if_icmpeq
                case 0xa0: IF_ICMP_COND(!=); break; // if_icmpne
                case 0xa1: IF_ICMP_COND(<);  break; // if_icmplt
                case 0xa2: IF_ICMP_COND(>=); break; // if_icmpge
                case 0xa3: IF_ICMP_COND(>);  break; // if_icmpgt
                case 0xa4: IF_ICMP_COND(<=); break; // if_icmple

#undef IF_ACMP_COND
#define IF_ACMP_COND(cond) \
    do { \
        jref v2 = frame_stack_popr(frame); \
        jref v1 = frame_stack_popr(frame); \
        jint offset = frame_reads2(frame); \
        if (v1 cond v2) \
            frame_skip(frame, offset - 3);  /* minus instruction length */ \
    } while (false)
                case 0xa5: IF_ACMP_COND(==); break; // if_acmpeq
                case 0xa6: IF_ACMP_COND(!=); break; // if_acmpne

                // Controls
                case 0xa7: __goto(frame); break; // goto

                // 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
                // 从Java 6开始，已经不再使用这些指令
                case 0xa8: vm_internal_error("jsr doesn't support after jdk 6."); break; // jsr
                case 0xa9: vm_internal_error("ret doesn't support after jdk 6."); break; // ret
                case 0xaa: tableswitch(frame); break;  // tableswitch
                case 0xab: lookupswitch(frame); break; // lookupswitch

#undef TRETURN
#define TRETURN(t) \
    do { \
        assert(frame == thread_top_frame(frame->thread)); \
        thread_pop_frame(frame->thread); \
        \
        struct frame *invoke_frame = thread_top_frame(frame->thread); \
        assert(invoke_frame != NULL); \
        frame_stack_push##t(invoke_frame, frame_stack_pop##t(frame)); \
        frame_exe_over(frame); \
    } while (false)
                case 0xac: TRETURN(i); break; // ireturn
                case 0xad: TRETURN(l); break; // lreturn
                case 0xae: TRETURN(f); break; // freturn
                case 0xaf: TRETURN(d); break; // dreturn
                case 0xb0: TRETURN(r); break; // areturn
                case 0xb1: frame_exe_over(thread_pop_frame(frame->thread)); break; // return

                // References
                case 0xb2: getstatic(frame); break; // getstatic
                case 0xb3: putstatic(frame); break; // putstatic
                case 0xb4: getfield(frame); break;  // getfield
                case 0xb5: putfield(frame); break;  // putfield

                case 0xb6: invokevirtual(frame); break;   // invokevirtual
                case 0xb7: invokespecial(frame); break;   // invokespecial
                case 0xb8: invokestatic(frame); break;    // invokestatic
                case 0xb9: invokeinterface(frame); break; // invokeinterface
                case 0xba: invokedynamic(frame); break;   // invokedynamic

                case 0xbb: new(frame); break;       // new
                case 0xbc: newarray(frame); break;    // newarray
                case 0xbd: anewarray(frame); break;   // anewarray
                case 0xbe: arraylength(frame); break; // arraylength

                case 0xbf: athrow(frame); break;       // athrow
                case 0xc0: checkcast(frame); break;    // checkcast
                case 0xc1: instanceof(frame); break;   // instanceof
                case 0xc2: monitorenter(frame); break; // monitorenter
                case 0xc3: monitorexit(frame); break;  // monitorexit

                // Extended
                case 0xc4: wide_extending = true; break; // wide
                case 0xc5: multianewarray(frame); break; // multianewarray
                case 0xc6: ifnull(frame); break; // ifnull
                case 0xc7: ifnonnull(frame); break; // ifnonnull
                case 0xc8: vm_internal_error("goto_w doesn't support"); break; // goto_w  // todo
                case 0xc9: vm_internal_error("jsr_w doesn't support after jdk 6."); // jsr_w // todo

                // Reserved
                case 0xca: jvm_abort("debugger used instructions. %s\n", frame_to_string(frame)); break; // breakpoint, // debugger used instruction// todo
                case 0xfe: frame->m.method->native_method(frame); break; // jvm used instruction，本 jvm 用来调用本地方法。
                case 0xff: jvm_abort("jvm used instruction, not used in this jvm. %s", frame_to_string(frame)); // todo
                default:
                    jvm_abort("This instruction isn't used. %s\n", frame_to_string(frame));
                    break;
            }

            if (frame_is_exe_over(frame)) {
                print2("frame(%p) exe over, destroy.\n", frame);
                thread_recycle_frame(frame);
                break;
            }

            if (frame_interrupted(frame)) {
                print2("frame(%p) interrupted.\n", frame);
                break; // 当前函数执行被中断。跳出循环，终止当前 frame 的执行。
            }
        }
    }

   print2("interpret exit.\n");
    return NULL; // todo
}
