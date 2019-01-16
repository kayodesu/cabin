/*
 * Author: Jia Yang
 */

#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/arrobj.h"

/*
 * 创建多维数组
 * todo 注意这种情况，基本类型的多维数组 int[][][]
 */
void multianewarray(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;
    int index = bcr_readu2(&frame->reader);
    const char *class_name = CP_UTF8(&curr_class->constant_pool, index);//rtcp_get_class_name(curr_class->rtcp, index); // 这里解析出来的直接就是数组类。
//    printvm("multi array class name: %s\n", class_name);  ////////////////////////////////////////////

    int arr_dim = bcr_readu1(&frame->reader); // 多维数组的维度
    size_t arr_lens[arr_dim]; // 每一维数组的长度
    for (int i = arr_dim - 1; i >= 0; i--) {
        int len = frame_stack_popi(frame);
        if (len < 0) {  // todo 等于0的情况
            thread_throw_negative_array_size_exception(frame->thread, len);
        }
        arr_lens[i] = (size_t) len;
    }

    struct class *arr_class = classloader_load_class(curr_class->loader, class_name);
    struct object *arr = arrobj_create_multi(arr_class, arr_dim, arr_lens);
    frame_stack_pushr(frame, arr);
}