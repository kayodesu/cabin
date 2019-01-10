/*
 * Author: Jia Yang
 */

#ifndef JVM_ARROBJ_H
#define JVM_ARROBJ_H

#include "object.h"

/*
 * 创建一维数组
 */
struct object *arrobj_create(struct class *arr_class, jint arr_len);

/*
 * 创建多维数组
 */
struct object *arrobj_create_multi(struct class *arr_class, size_t arr_dim, const size_t *arr_lens);

//#define ARR_ELE_SIZE(o) (*(jint *) ((o)->extra))
//#define ARR_LEN(o) (*(jint *) (((s1 *)((o)->extra)) + sizeof(jint)))
//#define ARR_DATA(o) (((s1 *)((o)->extra)) + 2 * sizeof(jint))

static inline jint arrobj_len(const struct object *o)
{
    assert(o != NULL);
    assert(object_is_array(o));
    return o->u.a.len;
}

static inline void* arrobj_data(const struct object *o)
{
    assert(o != NULL);
    assert(object_is_array(o));
    return o->data;
}

static inline bool arrobj_check_bounds(const struct object *o, jint index)
{
    assert(o != NULL);
    assert(object_is_array(o));
    return 0 <= index && index < o->u.a.len;
}

//static inline void* arrobj_index(struct object *arrobj, jint index)
//{
//    u1 *p = (u1 *) arrobj->data;
//    printvm("+++++++++++++++++++++++++++++++++++++++             %p, %d, %d\n", p, arrobj->u.a.ele_size, index);
//    return p + arrobj->u.a.ele_size * index;
//}

#define arrobj_index(arrobj, index) ( ((u1 *) ((arrobj)->data)) + (arrobj)->u.a.ele_size * (index) )

#define arrobj_set(T, arrobj, index, data) (* (T *) arrobj_index(arrobj, index) = (data))
#define arrobj_get(T, arrobj, index) (* (T *) arrobj_index(arrobj, index))

void arrobj_copy(struct object *dst, jint dst_pos,
                 const struct object *src, jint src_pos,
                 jint len);

#endif //JVM_ARROBJ_H
