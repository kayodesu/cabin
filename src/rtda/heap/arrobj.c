/*
 * Author: Jia Yang
 */

#include "object.h"
#include "arrobj.h"

Object *arrobj_create(Class *arr_class, jint arr_len)
{
    assert(arr_class != NULL);
    assert(class_is_array(arr_class));
    assert(arr_len >= 0);

    // 判断数组单个元素的大小
    // 除了基本类型的数组外，其他都是引用类型的数组
    // 多维数组是数组的数组，也是引用类型的数组
    size_t ele_size = sizeof(jref);
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
    size_t size = sizeof(Object) + mem_len;
    Object *o = hm_get(&g_heap_mgr, size);
    o->clazz = arr_class;
    o->size = size;
    o->u.a.ele_size = ele_size;
    o->u.a.len = arr_len;
    // java数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的 todo
    memset(o->data, 0, mem_len);
    return o;
}

Object *arrobj_create_multi(Class *arr_class, size_t arr_dim, const size_t *arr_lens)
{
    assert(arr_class != NULL);
    assert(arr_lens != NULL);
    assert(class_is_array(arr_class));

    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    Object *o = arrobj_create(arr_class, len);
    if (arr_dim == 1) {
        return o;
    }

    // todo
    Class *ele_class = load_class(arr_class->loader, arr_class->class_name + 1); // jump '['
    for (size_t i = 0; i < len; i++) {
        arrobj_set(jref, o, i, arrobj_create_multi(ele_class, arr_dim - 1, arr_lens + 1));
    }

    return o;
}

void arrobj_copy(Object *dst, jint dst_pos, const Object *src, jint src_pos, jint len)
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
}
