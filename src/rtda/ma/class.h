/*
 * Author: Jia Yang
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <stdbool.h>
#include <string.h>
#include "../../jtypes.h"
#include "../../loader/classloader.h"
#include "../thread/frame.h"
#include "../primitive_types.h"

struct object;
struct stack_frame;
struct thread;
struct field;

struct constant_pool {
    u1 *type;
    uintptr_t *info;
};

// Macros for accessing constant pool entries
#define CP_TYPE(cp, i)                   ((cp)->type[i])
#define CP_INFO(cp, i)                   ((cp)->info[i])

//#define CP_METHOD_CLASS(cp, i)           (u2)(cp)->info[i]
//#define CP_METHOD_NAME_TYPE(cp, i)       (u2)((cp)->info[i]>>16)
#define CP_INTERFACE_CLASS(cp, i)        (u2)(cp)->info[i]
#define CP_INTERFACE_NAME_TYPE(cp, i)    (u2)((cp)->info[i]>>16)

#define CP_UTF8(cp, i)                   (char *)((cp)->info[i])
#define CP_STRING(cp, i)                 CP_UTF8(cp, (u2)(cp)->info[i])
#define CP_CLASS_NAME(cp, i)             CP_UTF8(cp, (u2)(cp)->info[i])
#define CP_NAME_TYPE_NAME(cp, i)         CP_UTF8(cp, (u2)(cp)->info[i])
#define CP_NAME_TYPE_TYPE(cp, i)         CP_UTF8(cp, (u2)((cp)->info[i]>>16))

#define CP_FIELD_CLASS(cp, i)       (u2)(cp)->info[i]
#define CP_FIELD_CLASS_NAME(cp, i)       CP_UTF8(cp, (u2)(cp)->info[i])
#define CP_FIELD_NAME(cp, i)        CP_NAME_TYPE_NAME(cp, (u2)((cp)->info[i]>>16))
#define CP_FIELD_TYPE(cp, i)        CP_NAME_TYPE_TYPE(cp, (u2)((cp)->info[i]>>16))

#define CP_METHOD_CLASS CP_FIELD_CLASS
#define CP_METHOD_CLASS_NAME CP_FIELD_CLASS_NAME
#define CP_METHOD_NAME CP_FIELD_NAME
#define CP_METHOD_TYPE CP_FIELD_TYPE

#define CP_INT(cp, i)                    *(jint *) &((cp)->info[i])
#define CP_FLOAT(cp, i)                  *(jfloat *) &((cp)->info[i])
#define CP_LONG(cp, i)                   *(jlong *) &((cp)->info[i])
#define CP_DOUBLE(cp, i)                 *(jdouble *) &((cp)->info[i])

struct class {
    struct clsheader {
        // todo
    } head;

    u2 access_flags;
    u4 magic;
    u2 minor_version;
    u2 major_version;

    struct constant_pool constant_pool;
    int cp_count;

    // object of java/lang/Class of this class
    // 通过此字段，每个Class结构体实例都与一个类对象关联。
    struct object *clsobj;

    char *pkg_name;

    // 必须是全限定类名，用作 hash 表中的 key
    const char *class_name;

    // 如果类没有<clinit>方法，是不是inited直接职位true  todo
    bool inited; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    struct classloader *loader; // todo

    struct class *super_class;

    struct class **interfaces;
    u2 interfaces_count;

//    struct rtcp *rtcp;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面，
     * 这样，当外界需要所有的 public functions 时，可以返回此指针，数量就是 public_methods_count
     */
    struct method *methods;
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
    struct field *fields;
    u2 fields_count;
    u2 public_fields_count;

    // instance_field_count 有可能大于 fields_count，因为 instance_field_count 包含了继承过来的 field.
    // 类型二统计为两个数量
    int instance_fields_count;

    // 已经按变量类型初始化好的变量值，供创建此类的对象时使用。
//    struct slot *inited_instance_fields_values;

    /*
     * 类型二统计为两个数量
     */
    int static_fields_count;
    struct slot *static_fields_values; // 保存所有类变量的值

    // vtable 只保存虚方法。
    // 该类所有函数自有函数（除了private, static, final, abstract）和 父类的函数虚拟表。
    struct {
        const char *name; // method name
        const char *descriptor; // method descriptor
        struct method *method;
    } *vtable; // todo
    int vtable_len;

    void *itable; // todo

//    struct bootstrap_methods_attribute *bootstrap_methods_attribute;

    // enclosing_info[0]: the immediately enclosing class
    // enclosing_info[1]: the immediately enclosing method or constructor's name (can be null).
    // enclosing_info[2]: the immediately enclosing method or constructor's descriptor (null if name is).
    struct object* enclosing_info[3];

    bool deprecated;
    const char *signature;
    const char *source_file_name;
};

struct class *class_create(struct classloader *loader, u1 *bytecode, size_t len);

/*
 * 创建基本类型（int, float etc.）的 class.
 */
struct class* class_create_primitive_class(struct classloader *loader, const char *class_name);

struct class* class_create_arr_class(struct classloader *loader, const char *class_name);

void classloader_put_to_pool(struct classloader *loader, const char *class_name, struct class *c);

void class_destroy(struct class *c);

/*
 * 类的初始化在下列情况下触发：
 * 1. 执行new指令创建类实例，但类还没有被初始化。
 * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
 * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
 * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
 * 5. 执行某些反射操作时。
 *
 * 调用类的类初始化方法。
 * clinit are the static initialization blocks for the class, and static field initialization.
 */
void class_clinit(struct class *c, struct thread *thread);

//struct slot* copy_inited_instance_fields_values(const struct class *c);

void set_static_field_value_by_id(struct class *c, int id, const struct slot *value);
void set_static_field_value_by_nt(struct class *c,
                                  const char *name, const char *descriptor, const struct slot *value);

const struct slot* get_static_field_value_by_id(const struct class *c, int id);
const struct slot* get_static_field_value_by_nt(const struct class *c, const char *name, const char *descriptor);


//struct field* jclass_get_field(struct class *c, const char *name, const char *descriptor);
//struct field** jclass_get_fields(struct class *c, bool public_only);
struct field* class_lookup_field(struct class *c, const char *name, const char *descriptor);
struct field* class_lookup_static_field(struct class *c, const char *name, const char *descriptor);
struct field* class_lookup_instance_field(struct class *c, const char *name, const char *descriptor);

/*
 * 有可能返回NULL todo
 * get在本类中定义的类，不包括继承的。
 */
struct method* class_get_declared_method(struct class *c, const char *name, const char *descriptor);
struct method* class_get_declared_static_method(struct class *c, const char *name, const char *descriptor);
struct method* class_get_declared_nonstatic_method(struct class *c, const char *name, const char *descriptor);

struct method** class_get_declared_methods(struct class *c, const char *name, bool public_only, int *count);

struct method* class_get_constructor(struct class *c, const char *descriptor);
struct method** class_get_constructors(struct class *c, bool public_only, int *count);

static inline struct method* class_search_vtable(struct class *c, int vtable_index)
{
    assert(c != NULL);
    if (0 <= vtable_index && vtable_index < c->vtable_len) {
        return c->vtable[vtable_index].method;
    }
    return NULL;
}

/*
 * 在类的继承体系中查找方法
 */
struct method* class_lookup_method(struct class *c, const char *name, const char *descriptor);
struct method* class_lookup_static_method(struct class *c, const char *name, const char *descriptor);
struct method* class_lookup_instance_method(struct class *c, const char *name, const char *descriptor);

bool class_is_subclass_of(const struct class *c, const struct class *father);

bool class_is_accessible_to(const struct class *c, const struct class *visitor);

/*
 * 计算一个类的继承深度。
 * 如：java.lang.Object的继承的深度为0
 * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
 */
int class_inherited_depth(const struct class *c);

/*
 * 参数@c可以为空
 */
char *class_to_string(const struct class *c);

/*
 * 需要调用者释放返回值(free())
 */
char* get_arr_class_name(const char *class_name);

//struct class* jclass_array_class(struct class *c);

/*
 * Returns the representing the component class of an array class.
 * If this class does not represent an array class this method returns null.
 */
struct class* class_component_class(const struct class *arr_cls);

static inline bool class_is_array(const struct class *c)
{
    return c != NULL && c->class_name[0] == '[';
}

//static inline bool is_string(const struct class *c)
//{
//    return c != NULL && strcmp(c->class_name, "java/lang/String") == 0;  // todo 对不对
//}
//
//static inline bool is_class(const struct class *c)
//{
//    return c != NULL && strcmp(c->class_name, "java/lang/Class") == 0;  // todo 对不对
//}

static inline bool class_is_primitive(const struct class *c)
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
static inline bool is_primitive_array(const struct class *c)
{
    if (strlen(c->class_name) != 2 || c->class_name[0] != '[')
        return false;

    return strchr("ZBCSIFJD", c->class_name[1]) != NULL;
}

// 是否是一维数组
static inline bool is_one_dimension_array(const struct class *c)
{
    return is_primitive_array(c) || (strlen(c->class_name) >= 2 && c->class_name[0] == '[' && c->class_name[1] != '[');
}

// 是否是一维引用数组
static inline bool is_one_dimension_ref_array(const struct class *c)
{
    return is_one_dimension_array(c) && !is_primitive_array(c);
}

/*
 * 是否是引用的数组
 * 可能一维或多维（多维肯定是引用的数组）
 */
static inline bool is_ref_array(const struct class *c) { return !is_primitive_array(c); }

// 是否是多维数组
static inline bool is_multi_array(const struct class *c) { return class_is_array(c) && !is_one_dimension_array(c); }

static inline bool is_bool_array(const struct class *c) { return strcmp(c->class_name, "[Z") == 0; }

static inline bool is_byte_array(const struct class *c) { return strcmp(c->class_name, "[B") == 0; }

static inline bool is_bool_or_byte_array(const struct class *c) { return is_bool_array(c) || is_byte_array(c); }

static inline bool is_char_array(const struct class *c) { return strcmp(c->class_name, "[C") == 0; }

static inline bool is_short_array(const struct class *c) { return strcmp(c->class_name, "[S") == 0; }

static inline bool is_int_array(const struct class *c) { return strcmp(c->class_name, "[I") == 0; }

static inline bool is_float_array(const struct class *c) { return strcmp(c->class_name, "[F") == 0; }

static inline bool is_long_array(const struct class *c) { return strcmp(c->class_name, "[J") == 0; }

static inline bool is_double_array(const struct class *c) { return strcmp(c->class_name, "[D") == 0; }

#endif //JVM_JCLASS_H
