/*
 * Author: Jia Yang
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include "../../slot.h"
#include "../ma/jclass.h"

struct jobject {
    /*
     * 对象头
     */
    struct objheader {

    } header;

    // 保存所有实例变量的值
    // 包括此Object中定义的和从父类继承来的。
    int instance_fields_count;
    struct slot *instance_fields_values;

    struct jclass *jclass;

    /*
     * extra字段保持对象的额外信息。
     * 1. 对于 java/lang/Class 对象，extra字段类型为 struct jclass*, 保存 
     *    The entity class (class, interface, array class, primitive type, or void) represented by this object
     * 2. 对于 java/lang/String 对象，extra字段类型为 char *, 保存字符串的值。同时用作 key in string pool
     * 3. 对于数组对象，
     *    前 sizeof(jint) 个字节表示数组每个元素的大小，
     *    紧接着的 sizeof(jint) 个字节表示数组的长度，
     *    再后面是数组的数据。
     * 4. 异常对象的extra字段中存放的就是Java虚拟机栈信息 todo 
     */
    void *extra;
};

struct jobject* jobject_create(struct jclass *c);

static inline bool jobject_is_array(const struct jobject *o)
{
    assert(o != NULL);
    return jclass_is_array(o->jclass);
}

bool jobject_is_primitive(const struct jobject *o);
bool jobject_is_jlstring(const struct jobject *o);
bool jobject_is_jlclass(const struct jobject *o);

// java/lang/Sting 对象操作函数
struct jobject* jstrobj_create(const char *str);
const char* jstrobj_value(struct jobject *so);

// java/lang/Class 对象操作函数
/*
 * @entity_class: todo 说明
 */
struct jobject* jclsobj_create(struct jclass *entity_class);
struct jclass* jclsobj_entity_class(const struct jobject *co);

// 数组对象操作函数
/*
 * 创建一维数组
 * todo 说明 c 是什么东西
 */
struct jobject *jarrobj_create(struct jclass *arr_class, jint arr_len);

/*
 * 创建多维数组
 * todo 说明 c 是什么东西
 */
struct jobject *jarrobj_create_multi(struct jclass *arr_class, size_t arr_dim, const size_t *arr_lens);

#define ARR_ELE_SIZE(o) (*(jint *) ((o)->extra))
#define ARR_LEN(o) (*(jint *) (((s1 *)((o)->extra)) + sizeof(jint)))
#define ARR_DATA(o) (((s1 *)((o)->extra)) + 2 * sizeof(jint))

static inline jint jarrobj_len(const struct jobject *o)
{
    assert(o != NULL);
    assert(jobject_is_array(o));
    assert(o->extra != NULL);
    return ARR_LEN(o);
}

static inline void* jarrobj_data(const struct jobject *o)
{
    assert(o != NULL);
    assert(jobject_is_array(o));
    assert(o->extra != NULL);
    return ARR_DATA(o);
}

/*
 * 判断两个数组是否是同一类型的数组
 * todo 这里的判断略简陋
 */
bool jarrobj_is_same_type(const struct jobject *one, const struct jobject *other);

bool jarrobj_check_bounds(const struct jobject *ao, jint index);

static inline void* jarrobj_index(const struct jobject *o, jint index)
{
    assert(o != NULL);
    assert(jobject_is_array(o));
    assert(o->extra != NULL);
    assert(0 <= index && index < ARR_LEN(o));
    return ARR_DATA(o) + ARR_ELE_SIZE(o) * index;
}

#define jarrobj_set(T, arrobj, index, data) (*(T *)jarrobj_index(arrobj, index) = (data))
#define jarrobj_get(T, arrobj, index) (*(T *)jarrobj_index(arrobj, index))

void jarrobj_copy(struct jobject *dst, jint dst_pos,
                  const struct jobject *src, jint src_pos,
                  jint len);

/*
 * clone @src to @dest if @dest is not NULL,
 * else clone @src and return new one.
 */
struct jobject* jobject_clone(const struct jobject *src, struct jobject *dest);

void set_instance_field_value_by_id(const struct jobject *o, int id, const struct slot *value);
void set_instance_field_value_by_nt(const struct jobject *o,
                                    const char *name, const char *descriptor, const struct slot *value);

const struct slot* get_instance_field_value_by_id(const struct jobject *o, int id);
const struct slot* get_instance_field_value_by_nt(const struct jobject *o, const char *name, const char *descriptor);

/*
 * todo 说明
 */
struct slot jpriobj_unbox(const struct jobject *po);

void jobject_destroy(struct jobject *o);

bool jobject_is_instance_of(const struct jobject *o, const struct jclass *c);

const char* jobject_to_string(const struct jobject *o);

#endif //JVM_JOBJECT_H
