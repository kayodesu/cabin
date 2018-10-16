/*
 * Author: Jia Yang
 */

#ifndef JVM_JARRAYOBJ_H
#define JVM_JARRAYOBJ_H

#include "jobject.h"

/*
 * 数组对象
 */
struct jarrobj {
    struct jobject *obj;

    size_t len; // len of array   todo： size_t 类型好像不对，应该是 jint
    size_t ele_size;  // size of single element
    s1 data[];
};

/*
 * 创建一维数组
 */
struct jarrobj *jarrobj_create(struct jclass *c, size_t arr_len);

/*
 * 创建多维数组
 */
struct jarrobj *jarrobj_create_mutil(struct jclass *c, size_t arr_dim, const size_t *arr_lens);

/*
 * 判断两个数组是否是同一类型的数组
 * todo 这里的判断略简陋
 */
static inline bool jarrobj_is_same_type(const struct jarrobj *one, const struct jarrobj *other)
{
    return one->ele_size == other->ele_size;
}

static inline bool jarrobj_check_bounds(const struct jarrobj *ao, jint index)
{
    assert(ao != NULL);
    return index >= 0 && index < ao->len;
}

void jarrobj_copy(struct jarrobj *dst, jint dst_pos, const struct jarrobj *src, jint src_pos, jint len);

void* jarrobj_index(struct jarrobj *ao, jint index);

#endif //JVM_JARRAYOBJ_H
