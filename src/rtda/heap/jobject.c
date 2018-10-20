/*
 * Author: Jia Yang
 */

#include <mem.h>
#include "jobject.h"
#include "mm/halloc.h"
#include "../ma/jfield.h"

struct jobject* jobject_create(struct jclass *c)
{
    assert(c != NULL);

    HEAP_ALLOC(struct jobject, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    o->jclass = c;

    if (is_array(c)) {  // todo
        o->instance_fields_count = 0;
        o->instance_field_values = NULL;
    } else {
        o->instance_fields_count = c->instance_fields_count;
        o->instance_field_values = fv_create(o->jclass, o->instance_fields_count);
    }

    return o;
}

struct jobject* jclassobj_create(struct jclass *jclass_class, const char *class_name)
{
    // todo 判断jclass_class.class_name == "java/lang/Class"
    struct jobject *o = jobject_create(jclass_class);
    strcpy(o->c.class_name, class_name);
//    o->t = CLASS_OBJECT; // todo

    return o;
}

struct jobject* jstrobj_create(struct classloader *loader, const char *str)
{
    struct jobject *so = jobject_create(classloader_load_class(loader, "java/lang/String"));
    so->s.str = str;
    so->s.wstr = NULL; // todo
    return so;
}

struct jobject *jarrobj_create(struct jclass *arr_class, jint arr_len)
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

    struct jobject *o = jobject_create(arr_class);
    o->a.data = malloc(arr_len * ele_size);  // todo NULL
    o->a.ele_size = ele_size;
    o->a.len = arr_len;

    return o;
}

struct jobject *jarrobj_create_multi(struct jclass *arr_class, size_t arr_dim, const size_t *arr_lens)
{
    assert(arr_class != NULL);
    assert(arr_lens != NULL);
    // todo 判断 arr_class 是不是 array
    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    struct jobject * o = jarrobj_create(arr_class, len);
    if (arr_dim == 1) {
        return o;
    }

    // todo
    struct jclass *ele_class = classloader_load_class(arr_class->loader, arr_class->class_name + 1); // jump '['
    for (size_t i = 0; i < len; i++) {
        *(struct jobject **)jarrobj_index(o, i) = jarrobj_create_multi(ele_class, arr_dim - 1, arr_lens + 1);
    }

    return o;
}

bool jarrobj_is_same_type(const struct jobject *one, const struct jobject *other)
{
    if (one == NULL || one->t != ARRAY_OBJECT) {
        jvm_abort("eeeeeeeeeeeee\n");
    }

    if (other == NULL || other->t != ARRAY_OBJECT) {
        jvm_abort("eeeeeeeeeeeee\n");
    }

    return one->a.ele_size == other->a.ele_size;
}

bool jarrobj_check_bounds(const struct jobject *o, jint index)
{
    if (o == NULL || o->t != ARRAY_OBJECT) {
        jvm_abort("eeeeeeeeeeeee\n");
    }

    return index >= 0 && index < o->a.len;
}

void* jarrobj_index(struct jobject *o, jint index)
{
    if (o == NULL || o->t != ARRAY_OBJECT) {
        jvm_abort("eeeeeeeeeeeee\n");
    }

    if (index < 0 || index >= o->a.len) {
        jvm_abort("len = %zd, index = %d\n", o->a.len, index);
        return NULL;
    }

    return o->a.data + o->a.ele_size * index;
}

void jarrobj_copy(struct jobject *dst, jint dst_pos, const struct jobject *src, jint src_pos, jint len)
{
    if (dst == NULL || dst->t != ARRAY_OBJECT
        || src == NULL || src->t != ARRAY_OBJECT) {
        jvm_abort("eeeeeeeeeeeee\n");
    }
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
        || src_pos + len > src->a.len
        || dst_pos + len > dst->a.len) {
        // todo "java.lang.IndexOutOfBoundsException"
        jvm_abort("java.lang.IndexOutOfBoundsException\n");
    }

    s1 *d = dst->a.data;
    const s1 *s = src->a.data;
    size_t ele_size = src->a.ele_size;
    memcpy(d + dst_pos * ele_size, s + src_pos * ele_size, len * ele_size);
}

bool jobject_is_instance_of(const struct jobject *o, const struct jclass *c)
{
    if (o == NULL || c == NULL)  // todo
        return false;
    return jclass_is_subclass_of(o->jclass, c);
}

void jobject_destroy(struct jobject *o)
{
    if (o == NULL) {
        // todo
    }

    // todo

    hfree(o);
}
