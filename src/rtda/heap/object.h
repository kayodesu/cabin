/*
 * Author: Jia Yang
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include "../../slot.h"
#include "../ma/class.h"

struct object {
    /*
     * 对象头
     */
    struct objheader {

    } header;

    struct class *clazz;
    size_t size; // size of this object

    union {
        // effective only if object of java/lang/Class
        // save the entity class (class, interface, array class, primitive type, or void) represented by this object
        struct class *entity_class;

        // effective only if object of java/lang/String
        // 保存字符串的值。同时用作 key in string pool
        NO_ACCESS char *str;

        // effective only if object of array
        struct {
            size_t ele_size; // 表示数组每个元素的大小
            size_t len; // 数组的长度
        } a;

        // effective only if object of java/lang/Throwable
        // 存放的就是Java虚拟机栈信息
        void *stack_trace;
    } u;

    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t data[];
};

struct object* object_create(struct class *c);

static inline bool object_is_array(const struct object *o)
{
    assert(o != NULL);
    return class_is_array(o->clazz);
}

bool object_is_primitive(const struct object *o);
bool object_is_jlstring(const struct object *o);
bool object_is_jlclass(const struct object *o);

struct object* object_clone(const struct object *src);

void set_instance_field_value(struct object *o, struct field *f, const slot_t *value);
//void set_instance_field_value_by_nt(struct object *o,
//                                    const char *name, const char *descriptor, const slot_t *value);

const slot_t* get_instance_field_value(const struct object *o, struct field *f);

const slot_t* get_instance_field_value_by_id(const struct object *o, int id);
const slot_t* get_instance_field_value_by_nt(const struct object *o, const char *name, const char *descriptor);

/*
 * todo 说明
 */
const slot_t* priobj_unbox(const struct object *po);

void object_destroy(struct object *o);

bool object_is_instance_of(const struct object *o, const struct class *c);

const char* object_to_string(const struct object *o);

#endif //JVM_JOBJECT_H
