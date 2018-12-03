/*
 * Author: Jia Yang
 */

#include "jarrobj.h"
#include "mm/halloc.h"

static struct jarrobj* jarrobj_alloc()
{
    HEAP_ALLOC(struct jarrobj, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    return o;
}

struct jarrobj *jarrobj_create0(struct jclass *arr_class, jint arr_len)
{
    if (arr_class == NULL || !is_array(arr_class)) {
        // todo arr_class is NULL
        jvm_abort("error. is not array. %s\n", arr_class->class_name); // todo
    }

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

    struct jarrobj *ao = jarrobj_alloc();
    jobject_init(&(ao->obj), arr_class);

    // todo java new 一个数组会自动初始化为0吗?????????!!!!!!!!!!!!!!!!!!
    ao->data = calloc(arr_len, ele_size);  // todo NULL
    ao->ele_size = ele_size;
    ao->len = arr_len;
    ao->t = ARRAY_OBJECT;

    return ao;
}

struct jarrobj *jarrobj_create_multi0(struct jclass *arr_class, size_t arr_dim, const size_t *arr_lens)
{
    assert(arr_class != NULL);
    assert(arr_lens != NULL);
    // todo 判断 arr_class 是不是 array
    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    struct jarrobj *ao = jarrobj_create0(arr_class, len);
    if (arr_dim == 1) {
        return ao;
    }

    // todo
    struct jclass *ele_class = classloader_load_class(arr_class->loader, arr_class->class_name + 1); // jump '['
    for (size_t i = 0; i < len; i++) {
        *(struct jarrobj **)jarrobj_index0(ao, i) = jarrobj_create_multi0(ele_class, arr_dim - 1, arr_lens + 1);
    }

    return ao;
}

bool jarrobj_is_same_type0(const struct jarrobj *one, const struct jarrobj *other)
{
    assert(one != NULL);
    assert(other != NULL);
    return one->ele_size == other->ele_size;  // todo 类型怎么判断
}

bool jarrobj_check_bounds0(const struct jarrobj *ao, jint index)
{
    assert(ao != NULL);

    bool b =  index >= 0 && index < ao->len;
    if (!b) {
        printvm("array index out of bounds. index is %d, array length is %d\n", index, ao->len);
    }
    return b;
}

void* jarrobj_index0(struct jarrobj *ao, jint index)
{
    assert(ao != NULL);

    if (index < 0 || index >= ao->len) {
        jvm_abort("len = %zd, index = %d\n", ao->len, index);
        return NULL;
    }

    return ao->data + ao->ele_size * index;
}

void jarrobj_copy0(struct jarrobj *dst, jint dst_pos, const struct jarrobj *src, jint src_pos, jint len)
{
    assert(dst != NULL);
    assert(src != NULL);

    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (!jarrobj_is_same_type0(src, dst)) {
        // todo error  ArrayStoreException
        printvm("ArrayStoreException\n");
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->len
        || dst_pos + len > dst->len) {
        // todo "java.lang.IndexOutOfBoundsException"
        jvm_abort("java.lang.IndexOutOfBoundsException\n");
    }

    s1 *d = dst->data;
    const s1 *s = src->data;
    size_t ele_size = src->ele_size;
    memcpy(d + dst_pos * ele_size, s + src_pos * ele_size, len * ele_size);
}

struct jarrobj* jarrobj_clone0(const struct jarrobj *src)
{
    assert(src != NULL);

    struct jarrobj *ao = jarrobj_alloc();
    ao->obj.instance_fields_count = 0;
    ao->obj.instance_fields_values = NULL;

    ao->len = src->len;
    ao->ele_size = src->ele_size;
    // o->a.data todo
    // extra todo

    ao->obj.t = ARRAY_OBJECT;
    ao->obj.jclass = src->obj.jclass;
    return ao;
}