/*
 * Author: Jia Yang
 */

#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/arrobj.h"
#include "../../../classfile/constant.h"

/*
 * todo
 * 这函数是干嘛的，
 * 实现完全错误
 * 创建一维引用类型数组
 */
void anewarray(struct frame *frame)
{
    struct class *curr_class = frame->method->clazz;
    jint arr_len = frame_stack_popi(frame);
    if (arr_len < 0) {
        thread_throw_array_index_out_of_bounds_exception(frame->thread, arr_len);
        return;
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
        class_name = CP_CLASS_NAME(cp, index);//rtcp_get_class_name(frame->m.method->clazz->rtcp, index);
    }

//    const char *class_name = CP_CLASS_NAME((&(frame->m.method->clazz->constant_pool)), index);//rtcp_get_class_name(frame->m.method->clazz->rtcp, index); // 数组元素的类
    char *arr_class_name = get_arr_class_name(class_name);
    struct class *arr_class = classloader_load_class(curr_class->loader, arr_class_name);
    free(arr_class_name);
    frame_stack_pushr(frame, arrobj_create(arr_class, (size_t) arr_len));
}
