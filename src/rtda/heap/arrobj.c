/*
 * Author: Jia Yang
 */

#include "object.h"
#include "arrobj.h"
#include "mgr/heap_mgr.h"

struct object *arrobj_create(struct class *arr_class, jint arr_len)
{
    assert(arr_class != NULL);
    assert(class_is_array(arr_class));
    assert(arr_len >= 0);

    // 判断数组单个元素的大小
    // 除了基本类型的数组外，其他都是引用类型的数组
    // 多维数组是数组的数组，也是引用类型的数组
    jint ele_size = sizeof(jref);
    char t = arr_class->class_name[1]; // jump '['
    if (t == 'Z') { ele_size = sizeof(jbool); }
    else if (t == 'B') { ele_size = sizeof(jbyte); }
    else if (t == 'C') { ele_size = sizeof(jchar); }
    else if (t == 'S') { ele_size = sizeof(jshort); }
    else if (t == 'I') { ele_size = sizeof(jint); }
    else if (t == 'F') { ele_size = sizeof(jfloat); }
    else if (t == 'J') { ele_size = sizeof(jlong); }
    else if (t == 'D') { ele_size = sizeof(jdouble); }

    size_t mem_len = ele_size * arr_len;
    size_t size = sizeof(struct object) + mem_len;
    struct object *o = hm_get(&g_heap_mgr, size);
    o->clazz = arr_class;
    o->size = size;
    o->u.a.ele_size = ele_size;
    o->u.a.len = arr_len;
    // java数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的 todo
    memset(o->data, 0, mem_len);

//    jint len = sizeof(jint) /* store ele_size */ + sizeof(jint) /* store arr_len */ + arr_len * ele_size /* data */;
//    o->extra = malloc(len);
//    CHECK_MALLOC_RESULT(o->extra);
//    // java数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的 todo
//    memset(o->extra, 0, len);

//    ARR_ELE_SIZE(o) = ele_size;
//    ARR_LEN(o) = arr_len;
    return o;
}

struct object *arrobj_create_multi(struct class *arr_class, size_t arr_dim, const size_t *arr_lens)
{
    assert(arr_class != NULL);
    assert(arr_lens != NULL);
    assert(class_is_array(arr_class));

    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    struct object *o = arrobj_create(arr_class, len);
    if (arr_dim == 1) {
        return o;
    }

    // todo
    struct class *ele_class = load_class(arr_class->loader, arr_class->class_name + 1); // jump '['
    for (size_t i = 0; i < len; i++) {
        arrobj_set(jref, o, i, arrobj_create_multi(ele_class, arr_dim - 1, arr_lens + 1));
    }

    return o;
}

//void arrobj_set(struct object *arr, int index, void *value)
//{
//    assert(arr != NULL);
//    assert(value != NULL);
//
//    u1 *p = (u1 *) arr->data;
//    memcpy(p + arr->u.ele_size * index, value, arr->u.ele_size);
//}

void arrobj_copy(struct object *dst, jint dst_pos, const struct object *src, jint src_pos, jint len)
{
    assert(src != NULL);
    assert(object_is_array(src));

    assert(dst != NULL);
    assert(object_is_array(dst));

    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (src->u.a.ele_size != dst->u.a.ele_size) {
        // todo error  ArrayStoreException
        printvm("ArrayStoreException\n");
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->u.a.len
        || dst_pos + len > dst->u.a.len) {
        // todo "java.lang.IndexOutOfBoundsException"
        jvm_abort("java.lang.IndexOutOfBoundsException\n");
    }

    memcpy(arrobj_index(dst, dst_pos), arrobj_index(src, src_pos), src->u.a.ele_size * len);
//    s1 *d = (s1 *) dst->data;
//    const s1 *s = (const s1 *) src->data;
//    jint ele_size = src->u.ele_size;
//    memcpy(d + dst_pos * ele_size, s + src_pos * ele_size, len * ele_size);
}

//void* jarrobj_copy_data(const struct object *o)
//{
//    assert(o != NULL);
//    assert(jobject_is_array(o));
//    assert(o->extra != NULL);
//
//    size_t data_len = 2 * sizeof(jint) + ARR_ELE_SIZE(o) * ARR_LEN(o);
//    void *copy = malloc(data_len);
//    CHECK_MALLOC_RESULT(copy);
//    return memcpy(copy, o->extra, data_len);
//}
//
//void *arrobj_clone(const struct object *o)
//{
//    assert(o != NULL);
//    assert(jobject_is_array(o));
//}