/*
 * Author: Jia Yang
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <stdbool.h>
#include <string.h>
#include "../../jtypes.h"
#include "rtcp.h"
#include "../../loader/classloader.h"
#include "../thread/frame.h"
#include "../primitive_types.h"

struct jobject;
struct stack_frame;
struct jthread;

struct jclass {
    struct clshead {
        // todo
    } head;

    u2 access_flags;
    u4 magic;
    u2 minor_version;
    u2 major_version;

    // object of java/lang/Class of this class
    // 通过此字段，每个Class结构体实例都与一个类对象关联。
    struct jobject *clsobj;

    char *pkg_name;

    // 必须是全限定类名，用作 hash 表中的 key
    const char *class_name;

    bool inited; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    struct classloader *loader; // todo

    struct jclass *super_class;

    struct jclass **interfaces;
    u2 interfaces_count;

    struct rtcp *rtcp;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面，
     * 这样，当外界需要所有的 public functions 时，可以返回此指针，数量就是 public_methods_count
     */
    struct jmethod *methods;
    u2 methods_count;
    u2 public_methods_count;

    /*
     * 本类中所定义的变量（不包括继承而来的）
     * include both class variables and instance variables,
     * declared by this class or interface type.
     * 类型二统计为两个数量
     *
     * 所有的 public fields 都放在了最前面，
     * 这样，当外界需要所有的 public fields 时，可以返回此指针，数量就是 public_fields_count
     *
     * todo 接口中的变量怎么处理
     */
    struct jfield *fields;
    u2 fields_count;
    u2 public_fields_count;

    // instance_field_count 有可能大于 fields_count，因为 instance_field_count 包含了继承过来的 field.
    // 类型二统计为两个数量
    int instance_fields_count;
    // 已经按变量类型初始化好的变量值，供创建此类的对象时使用。
    struct slot *inited_instance_fields_values;

    /*
     * 类型二统计为两个数量
     */
    int static_fields_count;
    struct slot *static_fields_values; // 保存所有类变量的值

//    struct bootstrap_methods_attribute *bootstrap_methods_attribute;

    // enclosing_info[0]: the immediately enclosing class
    // enclosing_info[1]: the immediately enclosing method or constructor's name (can be null).
    // enclosing_info[2]: the immediately enclosing method or constructor's descriptor (null if name is).
    struct jobject* enclosing_info[3];

    bool deprecated;
    const char *signature;
    const char *source_file_name;
};

struct jclass *jclass_create(struct classloader *loader, u1 *bytecode, size_t len);

/*
 * 创建基本类型（int, float etc.）的 class.
 */
struct jclass* jclass_create_primitive_class(struct classloader *loader, const char *class_name);

struct jclass* jclass_create_arr_class(struct classloader *loader, const char *class_name);

void classloader_put_to_pool(struct classloader *loader, const char *class_name, struct jclass *c);

void jclass_destroy(struct jclass *c);

/*
 * 调用类的类初始化方法。
 * clinit are the static initialization blocks for the class, and static field initialization.
 */
void jclass_clinit(struct jclass *c, struct jthread *thread);

struct slot* copy_inited_instance_fields_values(const struct jclass *c);

void set_static_field_value_by_id(struct jclass *c, int id, const struct slot *value);
void set_static_field_value_by_nt(struct jclass *c,
                                  const char *name, const char *descriptor, const struct slot *value);

const struct slot* get_static_field_value_by_id(const struct jclass *c, int id);
const struct slot* get_static_field_value_by_nt(const struct jclass *c, const char *name, const char *descriptor);


//struct jfield* jclass_get_field(struct jclass *c, const char *name, const char *descriptor);
//struct jfield** jclass_get_fields(struct jclass *c, bool public_only);
struct jfield* jclass_lookup_field(struct jclass *c, const char *name, const char *descriptor);
struct jfield* jclass_lookup_static_field(struct jclass *c, const char *name, const char *descriptor);
struct jfield* jclass_lookup_instance_field(struct jclass *c, const char *name, const char *descriptor);

/*
 * 有可能返回NULL todo
 */
struct jmethod* jclass_get_declared_method(struct jclass *c, const char *name, const char *descriptor);
struct jmethod* jclass_get_declared_static_method(struct jclass *c, const char *name, const char *descriptor);
struct jmethod* jclass_get_declared_nonstatic_method(struct jclass *c, const char *name, const char *descriptor);

struct jmethod** jclass_get_methods(struct jclass *c, const char *name, bool public_only, int *count);

struct jmethod* jclass_get_constructor(struct jclass *c, const char *descriptor);
struct jmethod** jclass_get_constructors(struct jclass *c, bool public_only, int *count);

struct jmethod* jclass_lookup_method(struct jclass *c, const char *name, const char *descriptor);
struct jmethod* jclass_lookup_static_method(struct jclass *c, const char *name, const char *descriptor);
struct jmethod* jclass_lookup_instance_method(struct jclass *c, const char *name, const char *descriptor);

bool jclass_is_subclass_of(const struct jclass *c, const struct jclass *father);

bool jclass_is_accessible_to(const struct jclass *c, const struct jclass *visitor);

/*
 * 计算一个类的继承深度。
 * 如：java.lang.Object的继承的深度为0
 * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
 */
int jclass_inherited_depth(const struct jclass *c);

/*
 * 参数@c可以为空
 */
char *jclass_to_string(const struct jclass *c);

/*
 * 需要调用者释放返回值(free())
 */
char* get_arr_class_name(const char *class_name);

//struct jclass* jclass_array_class(struct jclass *c);

/*
 * Returns the representing the component class of an array class.
 * If this class does not represent an array class this method returns null.
 */
struct jclass* jclass_component_class(const struct jclass *arr_cls);

static inline bool jclass_is_array(const struct jclass *c)
{
    return c != NULL && c->class_name[0] == '[';
}

static inline bool is_string(const struct jclass *c)
{
    return c != NULL && strcmp(c->class_name, "java/lang/String") == 0;  // todo 对不对
}

static inline bool is_class(const struct jclass *c)
{
    return c != NULL && strcmp(c->class_name, "java/lang/Class") == 0;  // todo 对不对
}

static inline bool jclass_is_primitive(const struct jclass *c)
{
    assert(c != NULL);
    return pt_is_primitive_class_name(c->class_name);
}

/*
  * 是否是基本类型的数组（当然是一维的）。
  * 基本类型
  * bool, byte, char, short, int, float, long, double
  * 分别对应的数组类型为
  * [Z,   [B,   [C,   [S,    [I,  [F,    [J,   [D
  */
static inline bool is_primitive_array(const struct jclass *c)
{
    if (strlen(c->class_name) != 2 || c->class_name[0] != '[')
        return false;

    return strchr("ZBCSIFJD", c->class_name[1]) != NULL;
}

// 是否是一维数组
static inline bool is_one_dimension_array(const struct jclass *c)
{
    return is_primitive_array(c) || (strlen(c->class_name) >= 2 && c->class_name[0] == '[' && c->class_name[1] != '[');
}

// 是否是一维引用数组
static inline bool is_one_dimension_ref_array(const struct jclass *c)
{
    return is_one_dimension_array(c) && !is_primitive_array(c);
}

/*
 * 是否是引用的数组
 * 可能一维或多维（多维肯定是引用的数组）
 */
static inline bool is_ref_array(const struct jclass *c) { return !is_primitive_array(c); }

// 是否是多维数组
static inline bool is_multi_array(const struct jclass *c) { return jclass_is_array(c) && !is_one_dimension_array(c); }

static inline bool is_bool_array(const struct jclass *c) { return strcmp(c->class_name, "[Z") == 0; }

static inline bool is_byte_array(const struct jclass *c) { return strcmp(c->class_name, "[B") == 0; }

static inline bool is_bool_or_byte_array(const struct jclass *c) { return is_bool_array(c) || is_byte_array(c); }

static inline bool is_char_array(const struct jclass *c) { return strcmp(c->class_name, "[C") == 0; }

static inline bool is_short_array(const struct jclass *c) { return strcmp(c->class_name, "[S") == 0; }

static inline bool is_int_array(const struct jclass *c) { return strcmp(c->class_name, "[I") == 0; }

static inline bool is_float_array(const struct jclass *c) { return strcmp(c->class_name, "[F") == 0; }

static inline bool is_long_array(const struct jclass *c) { return strcmp(c->class_name, "[J") == 0; }

static inline bool is_double_array(const struct jclass *c) { return strcmp(c->class_name, "[D") == 0; }

#endif //JVM_JCLASS_H
