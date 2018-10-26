/*
 * Author: Jia Yang
 */

#include <string.h>
#include <wchar.h>
#include "jobject.h"
#include "mm/halloc.h"
#include "../ma/jfield.h"
#include "../../util/encoding.h"

struct jobject* jobject_create(struct jclass *c)
{
    assert(c != NULL);

    HEAP_ALLOC(struct jobject, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    o->jclass = c;
    o->t = NORMAL_OBJECT;

    if (is_array(c)) {  // todo
        o->instance_fields_count = 0;
        o->instance_field_values = NULL;
    } else {
        o->instance_fields_count = c->instance_fields_count;
        o->instance_field_values = fv_create(c, false);
    }

    return o;
}

struct jobject* jclassobj_create(struct jclass *jclass_class, const char *class_name)
{
    // todo 判断jclass_class.class_name == "java/lang/Class"
    struct jobject *o = jobject_create(jclass_class);
    strcpy(o->c.class_name, class_name);
    o->t = CLASS_OBJECT;

    return o;
}

/*
 * todo
 * 在class文件中，字符串是以MUTF8格式保存的，这一点在3.3.7
节讨论过。在Java虚拟机运行期间，字符串以java.lang.String（后面
简称String）对象的形式存在，而在String对象内部，字符串又是以
UTF16格式保存的。字符串相关功能大部分都是由String（和
StringBuilder等）类提供的，本节只实现一些辅助功能即可。

 注意，这里其实是跳过了String的构造函数，
直接用hack的方式创建实例。在前面分析过String类的代码，这样做
虽然有点投机取巧，但确实是没有问题的。

 todo 为什么要这么做， 而不是像其他类一样正常初始化
 */
struct jobject* jstrobj_create(struct classloader *loader, const char *str)
{
    assert(loader != NULL && str != NULL);

    struct jobject *so = jobject_create(classloader_load_class(loader, "java/lang/String"));

    printvm("create a string: %p[%s]\n", so, str); ////////////////////////////////////////////////////////////////////////

    so->s.str = strdup(str);
    so->s.wstr = utf8_to_unicode(str);

    so->t = STRING_OBJECT;
//    JArrayObj *jcharArr = JArrayObj::newJArrayObj(loader->loadClass("[C"), len);
    jint len = wcslen(so->s.wstr);
    struct jobject *jchars = jarrobj_create(classloader_load_class(loader, "[C"), len);
    // 不要使用 wcscpy 直接字符串拷贝，
    // 因为 wcscpy 函数会自动添加字符串结尾 L'\0'，
    // 但 jchars 没有空间容纳字符串结尾符，因为 jchar 是字符数组，不是字符串
    for (size_t i = 0; i < len; i++) {
        *(jchar *)jarrobj_index(jchars, i) = so->s.wstr[i]; // todo 可以使用 memcpy
    }

// todo 要不要调用 <clinit>, <init>方法。

    // 给 java/lang/String 类的 value 变量赋值  todo
    for (int i = 0; i < so->jclass->fields_count; i++) {
        struct jfield *field = so->jclass->fields[i];
        if (!IS_STATIC(field->access_flags)
            && strcmp(field->descriptor, "[C") == 0
            && strcmp(field->name, "value") == 0) {
//            setFieldValue(jclass->fields[i].id, Slot(jcharArr));
            struct slot s = rslot(jchars);
            fv_set_by_id(so->instance_field_values, field->id, &s);
            break;
        }
    }

    return so;
}

const char* jstrobj_value(struct jobject *so)
{
    STROBJ_CHECK(so);

    if (strlen(so->s.str) > 0) {
        return so->s.str;
    }

    const struct slot *s = fv_get_by_nt(so->instance_field_values, "value", "[C");
    if (s == NULL || s->t != REFERENCE) {
        jvm_abort("error\n"); // todo
    }

    struct jobject *jchars = s->v.r; // jchar[]
    if (jchars == NULL) {
        return NULL;
    }
    ARROBJ_CHECK(jchars);

    so->s.wstr = wcsdup(jchars->a.data);
    so->s.str = unicode_to_utf8( so->s.wstr);
    return so->s.str;  // todo
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
    o->t = ARRAY_OBJECT;

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
    struct jobject *o = jarrobj_create(arr_class, len);
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
    ARROBJ_CHECK(one);
    ARROBJ_CHECK(other);
    return one->a.ele_size == other->a.ele_size;
}

bool jarrobj_check_bounds(const struct jobject *o, jint index)
{
    ARROBJ_CHECK(o);
    bool b =  index >= 0 && index < o->a.len;
    if (!b) {
        printvm("array index out of bounds. index is %d, array length is %d\n", index, o->a.len);
    }
    return b;
}

void* jarrobj_index(struct jobject *o, jint index)
{
    ARROBJ_CHECK(o);
    if (index < 0 || index >= o->a.len) {
        jvm_abort("len = %zd, index = %d\n", o->a.len, index);
        return NULL;
    }

    return o->a.data + o->a.ele_size * index;
}

void jarrobj_copy(struct jobject *dst, jint dst_pos, const struct jobject *src, jint src_pos, jint len)
{
    ARROBJ_CHECK(dst);
    ARROBJ_CHECK(src);

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
