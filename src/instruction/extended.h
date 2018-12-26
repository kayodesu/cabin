/*
 * Author: Jia Yang
 */

#ifndef JVM_EXTENDED_H
#define JVM_EXTENDED_H

#include "../rtda/thread/frame.h"
#include "loads.h"

/*
 * 创建多维数组
 * todo 注意这种情况，基本类型的多维数组 int[][][]
 */
static void multianewarray(struct frame *frame)
{
    struct jclass *curr_class = frame->m.method->jclass;
    int index = bcr_readu2(&frame->reader);
    const char *class_name = rtcp_get_class_name(curr_class->rtcp, index); // 这里解析出来的直接就是数组类。
//    printvm("multi array class name: %s\n", class_name);  ////////////////////////////////////////////

    int arr_dim = bcr_readu1(&frame->reader); // 多维数组的维度
    size_t arr_lens[arr_dim]; // 每一维数组的长度
    for (int i = arr_dim - 1; i >= 0; i--) {
        int len = frame_stack_popi(frame);
        if (len < 0) {  // todo 等于0的情况
            jthread_throw_negative_array_size_exception(frame->thread, len);
        }
        arr_lens[i] = (size_t) len;
    }

    struct jclass *arr_class = classloader_load_class(curr_class->loader, class_name);
    struct jobject *arr = jarrobj_create_multi(arr_class, arr_dim, arr_lens);
    frame_stack_pushr(frame, arr);
}

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

#endif //JVM_EXTENDED_H
