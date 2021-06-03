#ifndef CABIN_META_H
#define CABIN_META_H

#include "cabin.h"
#include "slot.h"
#include "bytecode_reader.h"


/* Constant Pool */


// 从 1 开始计数，第0位无效
typedef struct constant_pool {
    u1 *type;
    slot_t *info;
    u2 size;

    Class *clazz;
    // mutable std::recursive_mutex mutex;
    /*mutable*/ pthread_mutex_t mutex;
} ConstantPool;

void cp_init(ConstantPool *, Class *clazz, u2 size);
void cp_release(ConstantPool *);

u1 cp_get_type(ConstantPool *, u2 i);
void cp_set_type(ConstantPool *, u2 i, u1 new_type);

void cp_set(ConstantPool *, u2 i, slot_t new_info);

utf8_t *cp_utf8(ConstantPool *, u2 i);

utf8_t *cp_string(ConstantPool *, u2 i);
utf8_t *cp_class_name(ConstantPool *, u2 i);
utf8_t *cp_module_name(ConstantPool *, u2 i);
utf8_t *cp_package_name(ConstantPool *, u2 i);
utf8_t *cp_name_of_name_and_type(ConstantPool *, u2 i);
utf8_t *cp_type_of_name_and_type(ConstantPool *, u2 i);

u2 cp_field_class_index(ConstantPool *, u2 i);
u2 cp_method_class_index(ConstantPool *, u2 i);
u2 cp_interface_method_class_index(ConstantPool *, u2 i);
u2 cp_invoke_dynamic_bootstrap_method_index(ConstantPool *, u2 i);

u2 cp_method_handle_reference_kind(ConstantPool *, u2 i);
u2 cp_method_handle_reference_index(ConstantPool *, u2 i);

utf8_t *cp_field_class_name(ConstantPool *, u2 i);
utf8_t *cp_field_name(ConstantPool *, u2 i);
utf8_t *cp_field_type(ConstantPool *, u2 i);

utf8_t *cp_method_class_name(ConstantPool *, u2 i);
utf8_t *cp_method_name(ConstantPool *, u2 i);
utf8_t *cp_method_type(ConstantPool *, u2 i);

utf8_t *cp_interface_method_class_name(ConstantPool *, u2 i);
utf8_t *cp_interface_method_name(ConstantPool *, u2 i);
utf8_t *cp_interface_method_type(ConstantPool *, u2 i);

utf8_t *cp_invoke_dynamic_method_name(ConstantPool *, u2 i);
utf8_t *cp_invoke_dynamic_method_type(ConstantPool *, u2 i);

utf8_t *cp_method_type_descriptor(ConstantPool *, u2 i);

jint cp_get_int(ConstantPool *, u2 i);
jfloat cp_get_float(ConstantPool *, u2 i);
jlong cp_get_long(ConstantPool *, u2 i);
jdouble cp_get_double(ConstantPool *, u2 i);
void cp_set_int(ConstantPool *, u2 i, jint new_int);
void cp_set_float(ConstantPool *, u2 i, jfloat new_float);
void cp_set_long(ConstantPool *, u2 i, jlong new_long);
void cp_set_double(ConstantPool *, u2 i, jdouble new_double);
    
Class *resolve_class(ConstantPool *, u2 i);
Method *resolve_method(ConstantPool *, u2 i);
Method *resolve_interface_method(ConstantPool *, u2 i);
Method *resolve_method_or_interface_method(ConstantPool *, u2 i);
Field *resolve_field(ConstantPool *, u2 i);
Object *resolve_string(ConstantPool *, u2 i);
Object *resolve_method_type(ConstantPool *, u2 i);
Object *resolve_method_handle(ConstantPool *, u2 i);

/* Class */

typedef enum ClassState {
    EMPTY_STATE,
    CLASS_LOADED,
    CLASS_LINKED,
    CLASS_INITING,
    CLASS_INITED
} ClassState;

typedef struct class Class;

/*
 * The metadata of a class.
 */
struct class {
    ClassState state;

    ConstantPool cp;

    const utf8_t *pkg_name; // 包名之间以 '.' 分隔。

    // 必须是全限定类名，包名之间以 '/' 分隔。
    const utf8_t *class_name;

    int access_flags;
    bool hidden; // if is hidden class.
    bool inited; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    Object *java_mirror;

    // the class loader who loaded this class
    // 可能为null，表示 bootstrap class loader.
    Object *loader;

    Class *super_class;

    // 本类声明实现的interfaces，父类声明实现的不包括在内。
    // 但如果父类声明实现，本类也声明了实现的接口，则包括在内。
    Class **interfaces;
    u2 interfaces_count;

    // 本类所实现的所有interfaces，包括本类和所有祖先类声明实现接口。
    // 各接口之间相互独立（没有重复的，且没有继承关系）。
    // std::unordered_set<Class *> indep_interfaces; // independent interfaces

    Class *nest_host;
    
    struct {
        bool loaded;
        union {
            Class *c; // if nest members is loaded
            utf8_t *class_name; // if nest members is not loaded
        };
    } *nest_members;
    u2 nest_members_count;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面
     */
    Method *methods;
    u2 methods_count; // declared methods count

    /*
     * 本类中所定义的变量（不包括继承而来的）
     * include both class variables and instance variables,
     * declared by this class or interface type.
     * 类型二统计为两个数量
     *
     * todo 接口中的变量怎么处理
     */
    Field *fields;
    u2 fields_count; // declared fields count

    // instFieldsCount 有可能大于 fieldsCount，因为 instFieldsCount 包含了继承过来的 field.
    // 类型二统计为两个数量
    int inst_fields_count;

    // vtable 只保存虚方法。
    // 该类所有函数自有函数（除了private, static, final, abstract）和 父类的函数虚拟表。
    Method **vtable;
    size_t vtable_len;
    
    /*
    * 为什么需要itable,而不是用vtable解决所有问题？
    * 一个类可以实现多个接口，而每个接口的函数编号是个自己相关的，
    * vtable 无法解决多个对应接口的函数编号问题。
    * 而对继承一个类只能继承一个父亲，子类只要包含父类vtable，
    * 并且和父类的函数包含部分编号是一致的，就可以直接使用父类的函数编号找到对应的子类实现函数。
    */

#if 0
    std::vector<Method *> vtable;

    struct ITable {
        std::vector<std::pair<Class *, size_t /* offset */>> interfaces;
        std::vector<Method *> methods;

        ITable() = default;
        ITable(const ITable &itable);
        ITable& operator=(const ITable &itable);
        void add(const ITable &itable);
    } itable;
    Method *findFromITable(Class *interface_class, int index);
#endif

    struct {
        Class *clazz;       // the immediately enclosing class
        Object *name;       // the immediately enclosing method or constructor's name (can be null).
        Object *descriptor; // the immediately enclosing method or constructor's type (null if name is).
    } enclosing;

    struct inner_class *inner_classes;
    u2 inner_classes_count;

    bool deprecated;
    const char *signature;
    const char *source_file_name;

    struct bootstrap_method *bootstrap_methods;
    u2 bootstrap_methods_count;

    struct annotation *rt_visi_annos;   // runtime visible annotations
    u2 rt_visi_annos_count;

    struct annotation *rt_invisi_annos; // runtime invisible annotations
    u2 rt_invisi_annos_count;

    // u1 *bytecode;

    pthread_mutex_t clinit_mutex;

    // Save extra data of some special classes.
     union {
         // for module-info.class
         struct {
             struct module *module;
            
             const utf8_t **module_packages;
             u2 module_packages_count;

             const utf8_t *module_main_class;
         } module_info;

         // for array class
         struct {
             Class *comp_class; // component class
             Class *ele_class;  // element class
             size_t ele_size;
         } array;

         // for java.lang.String class
         struct {
             // A pool of strings, initially empty, is maintained privately by the String class.
             struct point_hash_set *str_pool;
             pthread_mutex_t str_pool_mutex;
         } string;

         // for java.lang.Class class
         struct {
            // set by VM
            // private transient Module module;
            jref module;// set by VM  todo
         } class;
     };
};

void destroy_class(Class *);

// for non array class
size_t non_array_object_size(const Class *);       

// for array class
size_t array_object_size(Class *, jint arr_len); 

/*
 * 比较两个类是否相等
 */
bool class_equals(const Class *, const Class *);

// Generate the class object of this (aka 'java_mirror' field).
// This Object 只能由虚拟机创建
void gen_class_object(Class *);

TJE Field *lookup_field(Class *, const utf8_t *name, const utf8_t *descriptor); 
TJE Field *lookup_static_field(Class *, const char *name, const char *descriptor); 
TJE Field *lookup_inst_field(Class *, int id); 
TJE Field *lookup_inst_field0(Class *, const char *name, const char *descriptor); 

bool inject_inst_field(Class *, const utf8_t *name, const utf8_t *descriptor);

TJE Field *get_declared_field(const Class *, const char *name); 
TJE Field *get_declared_field0(const Class *, const char *name, const char *descriptor); 
TJE Field *get_declared_inst_field(const Class *, int id); 

TJE Method *lookup_method(Class *, const char *name, const char *descriptor); 
TJE Method *lookup_static_method(Class *, const char *name, const char *descriptor); 
TJE Method *lookup_inst_method(Class *, const char *name, const char *descriptor); 

/*
 * get在本类中定义的类，不包括继承的。
 */
TJE Method *get_declared_method(Class *, const utf8_t *name, const utf8_t *descriptor); 
Method *get_declared_method_noexcept(Class *c, const utf8_t *name, const utf8_t *descriptor); 
TJE Method *get_declared_static_method(Class *, const utf8_t *name, const utf8_t *descriptor); 
TJE Method *get_declared_inst_method(Class *, const utf8_t *name, const utf8_t *descriptor); 
// polymorphic signature
TJE Method *get_declared_poly_method(Class *, const utf8_t *name); 

// std::vector<Method *> get_declared_methods(Class *, const utf8_t *name, bool public_only);
Method **get_declared_methods(Class *c, const utf8_t *name, bool public_only, int *count);

Method *get_constructor(Class *, const utf8_t *descriptor);

// Class<?>[] parameterTypes;
Method *get_constructor0(Class *, jarrRef parameter_types);

// std::vector<Method *> get_constructors(Class *, bool public_only);
Method **get_constructors(Class *c, bool public_only, int *count);

bool is_subclass_of(Class *son, Class *father);

/*
 * 计算一个类的继承深度。
 * 如：java.lang.Object的继承的深度为0
 * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
 */
int inherited_depth(const Class *);

bool is_prim_class(const Class *);
bool is_prim_wrapper_class(const Class *);

#define is_boolean_class(c) (strcmp((c)->class_name, "boolean") == 0)
#define is_byte_class(c)    (strcmp((c)->class_name, "byte") == 0)
#define is_char_class(c)    (strcmp((c)->class_name, "char") == 0)
#define is_short_class(c)   (strcmp((c)->class_name, "short") == 0)
#define is_int_class(c)     (strcmp((c)->class_name, "int") == 0)
#define is_float_class(c)   (strcmp((c)->class_name, "float") == 0)
#define is_long_class(c)    (strcmp((c)->class_name, "long") == 0)
#define is_double_class(c)  (strcmp((c)->class_name, "double") == 0)
#define is_void_class(c)    (strcmp((c)->class_name, "void") == 0)

bool is_array_class(const Class *);

/*
 * 是否是基本类型的数组（当然是一维的）。
 * 基本类型
 * bool, byte, char, short, int, float, long, double
 * 分别对应的数组类型为
 * [Z,   [B,   [C,   [S,    [I,  [F,    [J,   [D
 */
bool is_prim_array_class(const Class *);

#define is_boolean_array_class(c) (strcmp((c)->class_name, "[Z") == 0)
#define is_byte_array_class(c)    (strcmp((c)->class_name, "[B") == 0)
#define is_char_array_class(c)    (strcmp((c)->class_name, "[C") == 0)
#define is_short_array_class(c)   (strcmp((c)->class_name, "[S") == 0)
#define is_int_array_class(c)     (strcmp((c)->class_name, "[I") == 0)
#define is_float_array_class(c)   (strcmp((c)->class_name, "[F") == 0)
#define is_long_array_class(c)    (strcmp((c)->class_name, "[J") == 0)
#define is_double_array_class(c)  (strcmp((c)->class_name, "[D") == 0)
#define is_ref_array_class(c)     (!is_prim_array_class(c))

Class *array_class(const Class *);

char *get_class_info(const Class *c);

/* for array class */

/*
 * 返回数组类的维度，非数组return 0
 */
int array_class_dim(const Class *) ;

// 判断数组单个元素的大小
// 除了基本类型的数组外，其他都是引用类型的数组
// 多维数组是数组的数组，也是引用类型的数组
size_t get_ele_size(Class *);

/*
 * Returns the representing the component class of an array class.
 * If this class does not represent an array class this method returns null.
 *
 * like，依次调用 componentClass():
 * [[I -> [I -> int -> null
 */
TJE Class *component_class(Class *); 

/*
 * Returns the representing the element class of an array class.
 * If this class does not represent an array class this method returns null.
 *
 * like [[[I 的元素类是 int.class
 */
Class *element_class(Class *);

jstrRef intern_string(jstrRef s);

/* Method */

typedef enum ret_type {
    RET_INVALID, RET_VOID, RET_BYTE, RET_BOOL, RET_CHAR,
    RET_SHORT, RET_INT, RET_FLOAT, RET_LONG, RET_DOUBLE, RET_REFERENCE
} RetType;

typedef struct method Method;

struct method {
    Class *clazz; // 定义此 Method 的类
    const utf8_t *name;
    const utf8_t *descriptor;

    int access_flags;

    bool deprecated;
    const char *signature;

    int vtable_index;
    int itable_index;

    u2 max_stack;
    u2 max_locals;
    u2 arg_slot_count; // include 'this' if is not static.

    u1 *code;
    size_t code_len;

    void *native_method; // present only if native
    
    RetType ret_type;

    struct method_parameter *parameters;
    u1 parameters_count;

    /*
     * Each value in the exception_index_table array must be a valid index into
     * the constant_pool table. The constant_pool entry at that index must be a
     * CONSTANT_Class_info structure representing a class type that this
     * method is declared to throw.
     */
    // checked exceptions the method may throw.
    u2 *checked_exceptions;
    u2 checked_exceptions_count;

    // std::vector<std::vector<Annotation>> rt_visi_para_annos;   // runtime visible parameter annotations
    // std::vector<std::vector<Annotation>> rt_invisi_para_annos; // runtime invisible parameter annotations

    // std::vector<Annotation> rt_visi_annos;   // runtime visible annotations
    // std::vector<Annotation> rt_invisi_annos; // runtime invisible annotations

    // ElementValue annotation_default;

    struct line_number_table *line_number_tables;
    u2 line_number_tables_count;

    struct local_variable_table *local_variable_tables;
    u2 local_variable_tables_count;

    struct local_variable_type_table *local_variable_type_tables;
    u2 local_variable_type_tables_count;
    
    /*
    * 异常处理表
    * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
    * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
    */
    struct exception_table {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;

        struct exception_catch_type {
            bool resolved;
            union {
                Class *clazz; // if resolved
                const char *class_name; // if not resolved
            };
        } *catch_type;
    } *exception_tables;
    u2 exception_tables_len;
};

void init_method(Method *m, Class *c, BytecodeReader *r);
void release_method(Method *);

u2 cal_method_args_slots_count(const utf8_t *descriptor, bool is_static);

// [Ljava/lang/Class;
jarrRef get_parameter_types(const Method *);

jclsRef get_return_type(const Method *);

// [Ljava/lang/Class;
jarrRef get_exception_types(const Method *);

/*
 * @pc, 发生异常的位置
 */
int find_exception_handler(const Method *, Class *exception_type, size_t pc);

jint get_line_number(const Method *, int pc);

/*
* 判断此方法是否由 invokevirtual 指令调用，
* final方法虽然非虚，但也由 invokevirtual 调用。
* todo
* 一个 final non-private 方法则可以覆写基类的虚方法，并且可以被基类引用通过invokevirtual调用到。
* 参考 https://www.zhihu.com/question/45131640
*/
bool is_virtual_method(const Method *);

/*
 * Is signature polymorphic（签名多态性） method?
 *
 * A method is signature polymorphic if all of the following are true:
 * 1. It is declared in the java.lang.invoke.MethodHandle class or the java.lang.invoke.VarHandle class.
 * 2. It has a single formal parameter of type Object[].
 * 3. It has the ACC_VARARGS and ACC_NATIVE flags set.
 */
bool is_signature_polymorphic(const Method *);

char *get_method_info(const Method *m);

/* Field */

struct field {
    Class *clazz; // 定义此 Field 的类
    const utf8_t *name;
    const utf8_t *descriptor;

    int access_flags;

    bool deprecated;
    const char *signature;

    bool category_two;

    union {
        // Present if static field,
        // static value 必须初始化清零
        union {
            jbool z;
            jbyte b;
            jchar c;
            jshort s;
            jint i;
            jlong j;
            jfloat f;
            jdouble d;
            jref r;
            slot_t data[2];
        } static_value;

        // Present if instance field
        int id;
    };

    struct annotation *rt_visi_annos; // runtime visible annotations
    u2 rt_visi_annos_count;

    struct annotation *rt_invisi_annos; // runtime invisible annotations
    u2 rt_invisi_annos_count;
};

void init_field(Field *, Class *, BytecodeReader *);
void init_field0(Field *, Class *clazz, const utf8_t *name, const utf8_t *descriptor, int access_flags);

bool is_prim_field(const Field *);

// the declared type(class Object) of this field
// like, int k; the type of k is int.class
jclsRef get_field_type(Field *);

char *get_field_info(const Field *);

/* Descriptor */

int num_elements_in_method_descriptor(const char *method_descriptor);

TJE bool parse_method_descriptor(const char *desc, jref loader, jarrRef *ptypes, jref *rtype);

// 返回结果需要调用者释放(free())
char *unparse_method_descriptor(jarrRef ptypes /*Class *[]*/, jclsRef rtype);

// 返回结果需要调用者释放(free())
// @method_type: Object of java.lang.invoke.MethodType
char *unparse_method_descriptor0(jref method_type);

#endif // CABIN_META_H