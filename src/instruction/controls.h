/*
 * Author: Jia Yang
 */

#ifndef JVM_CONTROLS_H
#define JVM_CONTROLS_H

#include "../rtda/thread/frame.h"

static void inline __goto(struct frame *frame)
{
    int offset = bcr_reads2(&frame->reader);
    bcr_skip(&frame->reader, offset - 3);  // minus instruction length
}

/*
 * 在Java 6之前，Oracle的Java编译器使用 jsr, jsr_w 和 ret 指令来实现 finally 子句。
 * 从Java 6开始，已经不再使用这些指令
 */

static void jsr(struct frame *frame)
{
    jvm_abort("jsr doesn't support after jdk 6.");
}

static void ret(struct frame *frame)
{
    // ret 有 wide 扩展
    jvm_abort("ret doesn't support after jdk 6.");
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

#define TRETURN(t) \
    do { \
        assert(frame == jthread_top_frame(frame->thread)); \
        jthread_pop_frame(frame->thread); \
        \
        struct frame *invoke_frame = jthread_top_frame(frame->thread); \
        assert(invoke_frame != NULL); \
        frame_stack_push##t(invoke_frame, frame_stack_pop##t(frame)); \
        frame_exe_over(frame); \
    } while (false)


#endif //JVM_CONTROLS_H
