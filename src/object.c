#include "cabin.h"
#include "symbol.h"
#include "dynstr.h"
#include "heap.h"
#include "object.h"
#include "encoding.h"


static inline void init(Object *o, Class *c)
{
    o->clazz = c;
    pthread_mutex_init(&o->mutex, &g_pthread_mutexattr_recursive);
}

static Object *create_non_array_object(Class *c, bool is_in_heap)
{
    assert(!is_array_class(c));

    size_t size = non_array_object_size(c);
    Object *o = (is_in_heap ? heap_malloc(g_heap, size) : vm_calloc(size));
    init(o, c);
    o->data = (slot_t *) (o + 1);
    return o;
}

Object *alloc_object(Class *c)
{
    assert(!is_array_class(c));
    return create_non_array_object(c, true);
}

Object *create_local_object(Class *c)
{
    assert(!is_array_class(c));
    return create_non_array_object(c, false);
}

Object *alloc_array(Class *ac, jint arr_len)
{
    assert(ac != NULL);
    assert(is_array_class(ac));
    assert(arr_len >= 0); // 长度为0的array是合法的

    size_t size = array_object_size(ac, arr_len);
    Object *o = (Object *) heap_malloc(g_heap, size);
    init(o, ac);

    o->arr_len = arr_len;
    // java 数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的
    // heap 申请对象时已经清零了。
    o->data = (slot_t *) (o + 1);
    return o;
}

Object *alloc_multi_array(Class *ac, jint dim, const jint lens[])
{
    assert(ac != NULL);
    assert(dim >= 1);
    assert(is_array_class(ac));

    size_t size = array_object_size(ac, lens[0]);
    Object *o = (Object *) heap_malloc(g_heap, size);
    init(o, ac);

    o->arr_len = lens[0];
    assert(o->arr_len >= 0); // 长度为0的array是合法的

    o->data = (slot_t *) (o + 1);

    for (int d = 1; d < dim; d++) {
        for (int i = 0; i < o->arr_len; i++) {
            array_set_ref(o, i, alloc_multi_array(component_class(ac), dim - 1, lens + 1));
        }
    }

    return o;
}

Object *clone_object(const Object *o) 
{
    assert(o != NULL);

    if (o->clazz == g_class_class) {
        JVM_PANIC("Object of java.lang.Class don't support clone"); // todo
    }

    size_t s = object_size(o);
    void *p = heap_malloc(g_heap, s);
    memcpy(p, o, s);

    // todo mutex 怎么处理

    Object *clone = (Object *) p;
    clone->data = (slot_t *) (clone + 1);
    return clone;
}

void set_field_value0(Object *o, Field *f, const slot_t *value)
{
    assert(o != NULL && f != NULL && !IS_STATIC(f) && value != NULL);

    o->data[f->id] = value[0];
    if (f->category_two) {
        o->data[f->id + 1] = value[1];
    }
}

void set_field_value1(Object *o, int id, jref value)
{
    assert(o != NULL);
    Field *f = lookup_inst_field(o->clazz, id);

    if (is_prim_field(f)) {
        const slot_t *unbox = prim_wrapper_obj_unbox(value);
        o->data[id] = *unbox;
        if (f->category_two)
            o->data[id+1] = *++unbox;
    } else {
        set_ref_field0(o, f, value);
    }
}

bool is_instance_of(const Object *o, Class *c) 
{
    assert(o != NULL);
    if (c == NULL)  // todo
        return false;
    return is_subclass_of(o->clazz, c);
}

const slot_t *prim_wrapper_obj_unbox(const Object *box) 
{
    assert(box != NULL && box->clazz!= NULL && is_prim_wrapper_class(box->clazz));

    Class *c = box->clazz;
    if (!is_prim_wrapper_class(c)) {
        JVM_PANIC("error"); // todo
    }

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    Field *f = lookup_field(c, S(value), get_prim_descriptor_by_wrapper_class_name(c->class_name));
    if (f == NULL) {
        JVM_PANIC("error, %s, %s\n", S(value), c->class_name); // todo
    }
    return box->data + f->id;
}

size_t object_size(const Object *o) 
{
    assert(o != NULL && o->clazz != NULL);

    if (is_array_object(o)) {
        return array_object_size(o->clazz, o->arr_len);
    } else {
        return non_array_object_size(o->clazz);
    }
}

const char *get_object_info(const Object *o)
{
    DynStr ds;
    dynstr_init(&ds);
    dynstr_printf(&ds, "Object(%p), %s\n", o, o->clazz->class_name);
    // todo
    return ds.buf;
}


bool array_check_bounds(const jarrRef a, jint index)
{
    return 0 <= index && index < a->arr_len;
}

void *array_index(const jarrRef a, jint index0)
{
    assert(0 <= index0 && index0 < a->arr_len);
    return ((u1 *) (a->data)) + get_ele_size(a->clazz)*index0;
}

#define ARRAY_SET(jtype, type) \
    void array_set_##type(jarrRef a, jint i, jtype v) \
    { \
        assert(a != NULL); \
        assert(is_##type##_array_class(a->clazz)); \
        assert(0 <= i && i < a->arr_len); \
        *(jtype *)array_index(a, i) = v; \
    }

ARRAY_SET(jbyte, byte)
ARRAY_SET(jboolean, boolean)
ARRAY_SET(jchar, char)
ARRAY_SET(jshort, short)
ARRAY_SET(jint, int)
ARRAY_SET(jlong, long)
ARRAY_SET(jfloat, float)
ARRAY_SET(jdouble, double)

#undef ARRAY_SET

void array_set_ref(jarrRef a, int i, jref value)
{
    assert(a != NULL);
    assert(0 <= i && i < a->arr_len);
    assert(is_ref_array_class(a->clazz));

    slot_t *data = (slot_t *) array_index(a, i);
    if (value == NULL) {
        *data = rslot(NULL);
    } else if (is_prim_array(a)) {
        const slot_t *unbox = prim_wrapper_obj_unbox(value);
        *data = *unbox;
        if (a->clazz->array.ele_size > sizeof(slot_t)) // todo
            *++data = *++unbox;
    } else {
        *data = rslot(value);
    }
}

void array_copy(jarrRef dst, jint dst_pos, const jarrRef src, jint src_pos, jint len)
{
    assert(src != NULL);
    assert(is_array_object(src));

    assert(dst != NULL);
    assert(is_array_object(dst));

    if (len < 1) {
        // need to do nothing
        return;
    }

    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (get_ele_size(src->clazz) != get_ele_size(dst->clazz)) {
        // throw java_lang_ArrayStoreException();
        raise_exception(S(java_lang_ArrayStoreException), NULL); // todo msg
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->arr_len
        || dst_pos + len > dst->arr_len) {
        // throw java_lang_ArrayIndexOutOfBoundsException();
        raise_exception(S(java_lang_ArrayIndexOutOfBoundsException), NULL);
    }

    memcpy(array_index(dst, dst_pos), array_index(src, src_pos), get_ele_size(src->clazz) * len);
}

const char *arr_class_name_to_ele_class_name(const utf8_t *arr_class_name)
{
    assert(arr_class_name != NULL);
    assert(arr_class_name[0] == '['); // must be array class name

    const utf8_t *ele_name = arr_class_name;
    while (*++ele_name == '[');

    const utf8_t *prim_class_name = get_prim_class_name(*ele_name);
    if (prim_class_name != NULL) {  // primitive type
        return prim_class_name;
    }

    // 普通类: Lxx/xx/xx; 型
    assert(*ele_name == 'L');
    ele_name++; // jump 'L'

    size_t last = strlen(ele_name) - 1;
    assert(last > 0);
    assert(ele_name[last] == ';');

    char *buf = vm_malloc(sizeof(*buf) * (last + 1));
    strncpy(buf, ele_name, (size_t) last);
    buf[last] = 0;
    return buf;
}

// set java/lang/String 的 coder 变量赋值
// private final byte coder;
// 可取一下两值之一：
// @Native static final byte LATIN1 = 0;
// @Native static final byte UTF16  = 1;
#define STRING_CODE_LATIN1 0
#define STRING_CODE_UTF16 1

utf8_t *string_to_utf8(jstrRef so)
{
    assert(so != NULL);
    assert(g_string_class != NULL);
    assert(is_string_object(so));
    
    // byte[] value;
    jarrRef value = get_ref_field(so, S(value), S(array_B));
    
    jbyte code = get_byte_field(so, S(coder));
    if (code == STRING_CODE_LATIN1) {
        utf8_t *utf8 = vm_malloc(sizeof(utf8_t) * (value->arr_len + 1));
        utf8[value->arr_len] = 0;
        memcpy(utf8, value->data, value->arr_len * sizeof(jbyte));
        return utf8;
    }
    if (code == STRING_CODE_UTF16) {
        utf8_t *u = unicode_to_utf8((unicode_t *)value->data, value->arr_len);
        return u;
    }

    SHOULD_NEVER_REACH_HERE("%d", code);
}

unicode_t *string_to_unicode(jstrRef so)
{
    assert(so != NULL);
    assert(g_string_class != NULL);
    assert(is_string_object(so));
    
    // byte[] value;
    jarrRef value = get_ref_field(so, S(value), S(array_B));

    jbyte code = get_byte_field(so, S(coder));
    if (code == STRING_CODE_LATIN1) {
        unicode_t *u = utf8_to_unicode((utf8_t *)value->data, value->arr_len);
        return u;
    }
    if (code == STRING_CODE_UTF16) {
        unicode_t *u = vm_malloc(sizeof(unicode_t) * (value->arr_len + 1));
        u[value->arr_len] = 0;
        memcpy(u, value->data, value->arr_len * sizeof(jbyte));
        return u;
    }

    SHOULD_NEVER_REACH_HERE("%d", code);
}

jstrRef alloc_string(const utf8_t *str)
{
    assert(g_string_class != NULL && str != NULL);

    init_class(g_string_class);
    assert(lookup_field(g_string_class, "COMPACT_STRINGS", "Z")->static_value.z);

    jstrRef so = alloc_object(g_string_class);
    size_t len = utf8_length(str);

    // set java/lang/String 的 value 变量赋值
    // private final byte[] value;
    jarrRef value = alloc_array0(BOOT_CLASS_LOADER, S(array_B), len); // [B
    memcpy(value->data, str, len);
    set_ref_field(so, S(value), S(array_B), value);

    set_byte_field(so, S(coder), STRING_CODE_LATIN1);
    return so;
}

jstrRef alloc_string0(const unicode_t *str, jsize len)
{
    assert(str != NULL && len >= 0);
    utf8_t *utf8 = unicode_to_utf8(str, len);
    jstrRef so = alloc_string(utf8);
    //delete[] utf8; todo
    return so;
}

bool string_equals(jstrRef x, jstrRef y)
{
    assert(x != NULL && y != NULL);
    assert(is_string_object(x) && is_string_object(y));

    // public boolean equals(Object anObject);
    Method *equals = get_declared_inst_method(g_string_class, "equals", "(Ljava/lang/Object;)Z");
    return slot_get_bool(exec_java(equals, (slot_t[]) { rslot(x), rslot(y) })) != jfalse;
}

size_t string_hash(jstrRef x)
{
    assert(x != NULL && is_string_object(x));

    // public int hashCode();
    Method *hashCode = get_declared_inst_method(g_string_class, "hashCode", "()I");
    return (size_t) slot_get_int(exec_java(hashCode, (slot_t[]) { rslot(x) }));
}


jsize get_string_length(jstrRef so)
{
    // todo
    
    // private final byte coder;
    // 可取一下两值之一：
    // @Native static final byte LATIN1 = 0;
    // @Native static final byte UTF16  = 1;
    jbyte code = get_byte_field(so, S(coder));
    if (code == 1) {
        JVM_PANIC("not implement");
    } else {
        // private final byte[] value;
        jarrRef value = get_ref_field(so, S(value), S(array_B));
        return value->arr_len;
    }
}

jsize get_string_uft_length(jstrRef so)
{
    // todo
    
    // private final byte coder;
    // 可取一下两值之一：
    // @Native static final byte LATIN1 = 0;
    // @Native static final byte UTF16  = 1;
    jbyte code = get_byte_field(so, S(coder));
    if (code == 1) {
        JVM_PANIC("not implement");
    } else {
        // private final byte[] value;
        jarrRef value = get_ref_field(so, S(value), S(array_B));
        return value->arr_len;
    }
}

