/*
 * Author: Jia Yang
 */

#ifndef JVM_JARROBJ_H
#define JVM_JARROBJ_H

#include <stddef.h>
#include "../../jtypes.h"
#include "jobject.h"

struct jarrobj {
    struct jobject obj;

    jint len; // len of array
    size_t ele_size;  // size of single element
    s1 *data;
    enum jobject_type t;
};

/*
 * 创建一维数组
 * todo 说明 c 是什么东西
 */
struct jarrobj *jarrobj_create0(struct jclass *arr_class, jint arr_len);
struct jarrobj *jarrobj_create_multi0(struct jclass *arr_class, size_t arr_dim, const size_t *arr_lens);
bool jarrobj_is_same_type0(const struct jarrobj *one, const struct jarrobj *other);
bool jarrobj_check_bounds0(const struct jarrobj *ao, jint index);
void* jarrobj_index0(struct jarrobj *o, jint index);
void jarrobj_copy0(struct jarrobj *dst, jint dst_pos, const struct jarrobj *src, jint src_pos, jint len);
struct jarrobj* jarrobj_clone0(const struct jarrobj *src);

#endif //JVM_JARROBJ_H
