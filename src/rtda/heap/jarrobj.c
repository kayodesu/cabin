/*
 * Author: Jia Yang
 */

#include "jarrobj.h"

struct jarrobj *jarrobj_create(struct jclass *c, size_t arr_len)
{
    if (c == NULL) {
        jvm_abort("eeeeeeeeeeee\n"); // todo
    }
    if (!is_array(c)) {
        jvm_abort("error. is not array. %s\n", c->class_name); // todo
    }

    // 判断数组单个元素的大小
    // 除了基本类型的数组外，其他都是引用类型的数组
    // 多维数组是数组的数组，也是引用类型的数组
    size_t ele_size = sizeof(jref);
    char t = c->class_name[1]; // jump '['
    if (t == 'Z') { ele_size = sizeof(jbool); }
    else if (t == 'B') { ele_size = sizeof(jbyte); }
    else if (t == 'C') { ele_size = sizeof(jchar); }
    else if (t == 'S') { ele_size = sizeof(jshort); }
    else if (t == 'I') { ele_size = sizeof(jint); }
    else if (t == 'F') { ele_size = sizeof(jfloat); }
    else if (t == 'J') { ele_size = sizeof(jlong); }
    else if (t == 'D') { ele_size = sizeof(jdouble); }

    struct jarrobj *ao = malloc(sizeof(*ao) + arr_len * ele_size);
    ao->obj = jobject_create(c);
    ao->ele_size = ele_size;
    ao->len = arr_len;

    return ao;
}

struct jarrobj *jarrobj_create_mutil(struct jclass *c, size_t arr_dim, const size_t *arr_lens)
{
    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    struct jarrobj * ao = jarrobj_create(c, len);
    if (arr_dim == 1) {
        return ao;
    }

    // todo
    struct jclass *ele_class = classloader_load_class(c->loader, c->class_name + 1); // jump '['
    for (size_t i = 0; i < len; i++) {
        *(struct jarrobj **)jarrobj_index(ao, i) = jarrobj_create_mutil(ele_class, arr_dim - 1, arr_lens + 1);
    }
}

void* jarrobj_index(struct jarrobj *ao, jint index)
{
    // todo 判断范围
    return ao->data + ao->ele_size * index;
}

void jarrobj_copy(struct jarrobj *dst, jint dst_pos, const struct jarrobj *src, jint src_pos, jint len)
{
    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (!jarrobj_is_same_type(src, dst)) {
        // todo error  ArrayStoreException
        printvm("ArrayStoreException\n");
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->len
        || dst_pos + len > dst->len) {
        // todo "java.lang.IndexOutOfBoundsException"
        printvm("java.lang.IndexOutOfBoundsException\n");
    }

    s1 *d = dst->data;
    const s1 *s = src->data;
    size_t ele_size = src->ele_size;
    memcpy(d + dst_pos * ele_size, s + src_pos * ele_size, len * ele_size);
}