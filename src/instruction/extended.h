/*
 * Author: Jia Yang
 */

#ifndef JVM_EXTENDED_H
#define JVM_EXTENDED_H

#include "../interpreter/stack_frame.h"

/*
 * 创建多维数组
 * todo 注意这种情况，基本类型的多维数组 int[][][]
 */
static void multianewarray(struct stack_frame *frame)
{
    struct jclass *curr_class = frame->method->jclass;
    int index = bcr_readu2(frame->reader);
    const char *class_name = rtcp_get_class_name(curr_class->rtcp, index); // 这里解析出来的直接就是数组类。
//    printvm("multi array class name: %s\n", class_name);  ////////////////////////////////////////////

    int arr_dim = bcr_readu1(frame->reader); // 多维数组的维度
    size_t arr_lens[arr_dim]; // 每一维数组的长度
    for (int i = arr_dim - 1; i >= 0; i--) {
        int len = os_popi(frame->operand_stack);
        if (len < 0) {  // todo 等于0的情况
            jthread_throw_negative_array_size_exception(frame->thread, len);
            return;
        }
        arr_lens[i] = (size_t) len;
    }

    struct jclass *arr_class = classloader_load_class(curr_class->loader, class_name);
    struct jobject *arr = jarrobj_create_multi(arr_class, arr_dim, arr_lens);
    os_pushr(frame->operand_stack, arr);
}

static void ifnull(struct stack_frame *frame)
{
    int offset = bcr_reads2(frame->reader);

    if (os_popr(frame->operand_stack) == NULL) {
        bcr_skip(frame->reader, offset - 3); // why减3？减去本条指令自身的长度
    }
}

static void ifnonnull(struct stack_frame *frame)
{
    int offset = bcr_reads2(frame->reader);

    if (os_popr(frame->operand_stack) != NULL) {
        bcr_skip(frame->reader, offset - 3); // why减3？减去本条指令自身的长度
    }
}

static void goto_w(struct stack_frame *frame)
{
    vm_internal_error("goto_w 不支持"); // todo
}

static void jsr_w(struct stack_frame *frame)
{
    vm_internal_error("jsr_w doesn't support after jdk 6.");
}

extern bool wide_extending;

static void wide(struct stack_frame *frame)
{
    wide_extending = true;
}

#endif //JVM_EXTENDED_H
