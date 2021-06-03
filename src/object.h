#ifndef CABIN_OBJECT_H
#define CABIN_OBJECT_H

#include <pthread.h>

#include "cabin.h"
#include "meta.h"
#include "slot.h"
#include "symbol.h"
#include "class_loader.h"

struct object {
    // 对象头，放在Object类的最开始处
    union {
        struct {
            unsigned int accessible: 1; // gc时判断对象是否可达
            unsigned int marked: 2;
        };
        uintptr_t all_flags; // 以指针的大小对齐 todo 这样对齐有什么用
    };

    Class *clazz;
    pthread_mutex_t mutex;

 //   union {
        Class *jvm_mirror; // present only if Object of java.lang.Class

        jsize arr_len; // present only if Object of Array

        // present only if object of java/lang/ClassLoader
        // save the all loaded classes by this ClassLoader
        struct point_hash_map *classes;
        // std::unordered_map<const utf8_t *, Class *, Utf8Hash, Utf8Comparator> *classes = NULL;
 //   }

    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t *data;
};

#define lock_object(o) pthread_mutex_lock(&(o->mutex))
#define unlock_object(o) pthread_mutex_unlock(&(o->mutex))

// alloc non array object
Object *alloc_object(Class *); 

// create local(non Heap) non array object
Object *create_local_object(Class *);

// 一维数组
Object *alloc_array(Class *, jint arr_len);
#define alloc_array0(class_loader, arr_class_name, arr_len) alloc_array(load_array_class(class_loader, arr_class_name), arr_len)
#define alloc_string_array(arr_len) alloc_array0(BOOT_CLASS_LOADER, S(array_java_lang_String), arr_len)
#define alloc_class_array(arr_len)  alloc_array0(BOOT_CLASS_LOADER, S(array_java_lang_Class), arr_len)
#define alloc_object_array(arr_len) alloc_array0(BOOT_CLASS_LOADER, S(array_java_lang_Object), arr_len)

// 多维数组
Object *alloc_multi_array(Class *, jint dim, const jint lens[]);

jstrRef alloc_string(const utf8_t *str);
jstrRef alloc_string0(const unicode_t *str, jsize len);

jsize stringGetLength(jstrRef so);
jsize stringGetUTFLength(jstrRef so);

/* Set field value */

#define set_byte_field0(obj, field, v) slot_set_byte((obj)->data + (field)->id, v)
#define set_bool_field0(obj, field, v) slot_set_bool((obj)->data + (field)->id, v)
#define set_char_field0(obj, field, v) slot_set_char((obj)->data + (field)->id, v)
#define set_short_field0(obj, field, v) slot_set_short((obj)->data + (field)->id, v)
#define set_int_field0(obj, field, v) slot_set_int((obj)->data + (field)->id, v)
#define set_long_field0(obj, field, v) slot_set_long((obj)->data + (field)->id, v)
#define set_float_field0(obj, field, v) slot_set_float((obj)->data + (field)->id, v)
#define set_double_field0(obj, field, v) slot_set_double((obj)->data + (field)->id, v)
#define set_ref_field0(obj, field, v) slot_set_ref((obj)->data + (field)->id, v)

#define set_byte_field(obj, name, v) set_byte_field0(obj, lookup_inst_field0((obj)->clazz, name, S(B)), v)
#define set_bool_field(obj, name, v) set_bool_field0(obj, lookup_inst_field0((obj)->clazz, name, S(Z)), v)
#define set_int_field(obj, name, v) set_int_field0(obj, lookup_inst_field0((obj)->clazz, name, S(I)), v)
#define set_long_field(obj, name, v) set_long_field0(obj, lookup_inst_field0((obj)->clazz, name, S(J)), v)
#define set_ref_field(obj, name, descriptor, v) set_ref_field0(obj, lookup_inst_field0((obj)->clazz, name, descriptor), v)

void set_field_value0(Object *o, Field *f, const slot_t *value);
void set_field_value1(Object *o, int id, jref value);

/* Get field value */

#define get_byte_field0(obj, field)   slot_get_byte((obj)->data + (field)->id)
#define get_bool_field0(obj, field)   slot_get_bool((obj)->data + (field)->id)
#define get_char_field0(obj, field)   slot_get_char((obj)->data + (field)->id)
#define get_short_field0(obj, field)  slot_get_short((obj)->data + (field)->id)
#define get_int_field0(obj, field)    slot_get_int((obj)->data + (field)->id)
#define get_float_field0(obj, field)  slot_get_float((obj)->data + (field)->id)
#define get_long_field0(obj, field)   slot_get_long((obj)->data + (field)->id)
#define get_double_field0(obj, field) slot_get_double((obj)->data + (field)->id)
#define get_ref_field0(obj, field)    slot_get_ref((obj)->data + (field)->id)

#define get_byte_field(obj, name)   get_byte_field0(obj, lookup_inst_field0((obj)->clazz, name, S(B)))
#define get_bool_field(obj, name)   get_bool_field0(obj, lookup_inst_field0((obj)->clazz, name, S(Z)))
#define get_char_field(obj, name)   get_char_field0(obj, lookup_inst_field0((obj)->clazz, name, S(C)))
#define get_short_field(obj, name)  get_short_field0(obj, lookup_inst_field0((obj)->clazz, name, S(S)))
#define get_int_field(obj, name)    get_int_field0(obj, lookup_inst_field0((obj)->clazz, name, S(I)))
#define get_float_field(obj, name)  get_float_field0(obj, lookup_inst_field0((obj)->clazz, name, S(F)))
#define get_long_field(obj, name)   get_long_field0(obj, lookup_inst_field0((obj)->clazz, name, S(J)))
#define get_double_field(obj, name) get_double_field0(obj, lookup_inst_field0((obj)->clazz, name, S(D)))
#define get_ref_field(obj, name, descriptor) get_ref_field0(obj, lookup_inst_field0((obj)->clazz, name, descriptor))

#define is_class_object(obj)  ((obj)->clazz == g_class_class)
#define is_string_object(obj) ((obj)->clazz == g_string_class)
#define is_array_object(obj)  ((obj)->clazz->class_name[0] == '[')
#define is_prim_array(obj)    (is_prim_descriptor((obj)->clazz->class_name[1]))

bool is_instance_of(const Object *o, Class *c);

const slot_t *prim_wrapper_obj_unbox(const Object *box);

size_t object_size(const Object *);

Object *clone_object(const Object *);

const char *get_object_info(const Object *);

/* Array */

/*
 * [[[I -> int
 * [Ljava/lang/Object; -> java/lang/Object
 */
const char * arr_class_name_to_ele_class_name(const utf8_t *arr_class_name);

void *array_index(const jarrRef a, jint index);
bool array_check_bounds(const jarrRef a, jint index);

void array_set_byte(jarrRef a, int i, jbyte value);
void array_set_boolean(jarrRef a, int i, jboolean value);
#define array_set_bool array_set_boolean
void array_set_char(jarrRef a, int i, jchar value);
void array_set_short(jarrRef a, int i, jshort value);
void array_set_int(jarrRef a, int i, jint value);
void array_set_long(jarrRef a, int i, jlong value);
void array_set_float(jarrRef a, int i, jfloat value);
void array_set_double(jarrRef a, int i, jdouble value);
void array_set_ref(jarrRef a, int i, jref value);

#define array_get(__jtype, __array, __index) (*(__jtype *) array_index(__array, __index))

TJE void array_copy(jarrRef dst, jint dst_pos, const jarrRef src, jint src_pos, jint len);


/* String */

utf8_t *string_to_utf8(jstrRef so);
unicode_t *string_to_unicode(jstrRef so);
bool string_equals(jstrRef x, jstrRef y);
size_t string_hash(jstrRef x);
jsize get_string_length(jstrRef so);
jsize get_string_uft_length(jstrRef so);

#endif // CABIN_OBJECT_H