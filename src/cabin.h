#ifndef CABIN_CABIN_H
#define CABIN_CABIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>
#include <pthread.h>
#include "symbol.h"
#include "util/hash.h"
#include "constants.h"

#ifndef PATH_MAX
    #ifdef MAX_PATH
        #define PATH_MAX MAX_PATH
    #else
        #define PATH_MAX 1024
    #endif
#endif

#define VM_VERSION "1.0" // version of this jvm, a string.
#define JAVA_COMPAT_VERSION "1.8.0_162"

// jvm 最大支持的classfile版本
#define JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION 60
#define JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION 65535

// size of heap
#define VM_HEAP_SIZE (512*1024*1024) // 512Mb

// every thread has a vm stack
#define VM_STACK_SIZE (512*1024)     // 512Kb

// jvm 最大支持的线程数量
#define VM_THREADS_MAX_COUNT 65535

/*
 * Java虚拟机中的整型类型的取值范围如下：
 * 1. 对于byte类型， 取值范围[-2e7, 2e7 - 1]。
 * 2. 对于short类型，取值范围[-2e15, 2e15 - 1]。
 * 3. 对于int类型，  取值范围[-2e31, 2e31 - 1]。
 * 4. 对于long类型， 取值范围[-2e63, 2e63 - 1]。
 * 5. 对于char类型， 取值范围[0, 65535]。
 */
typedef int8_t   jbyte;
typedef jbyte    jboolean; // 本虚拟机实现，byte 和 boolean 用同一类型
typedef jboolean jbool;
typedef uint16_t jchar;
typedef int16_t  jshort;
typedef int32_t  jint;
typedef int64_t  jlong;
typedef float    jfloat;
typedef double   jdouble;

#define jtrue  1
#define jfalse 0

typedef jint jsize;

#define JINT_TO_JBOOL(_i)  ((_i) != 0 ? true : false)
#define JINT_TO_JBYTE(_i)  ((jbyte)((_i) & 0xff))
#define JINT_TO_JCHAR(_i)  ((jchar)((_i) & 0xffff))
#define JINT_TO_JSHORT(_i) ((jshort)((_i) & 0xffff))

typedef int8_t  s1;  // s: signed
typedef int16_t s2;
typedef int32_t s4;

typedef uint8_t  u1; // u: unsigned
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

#define U2_MAX 65535

typedef struct object Object;
typedef struct class  Class;
typedef struct method Method;
typedef struct field  Field;

typedef Object* jref; // JVM 中的引用类型。
typedef jref jstrRef; // java.lang.String 的引用。
typedef jref jarrRef; // Array 的引用。
typedef jref jclsRef; // java.lang.Class 的引用。

typedef char utf8_t;
typedef jchar unicode_t;

struct heap;
extern struct heap *g_heap;

extern pthread_mutexattr_t g_pthread_mutexattr_recursive;

extern char g_java_home[];

extern u2 g_classfile_major_version;
extern u2 g_classfile_manor_version;

// The system Thread group.
extern Object *g_sys_thread_group;

struct vm_thread;

// todo 所有线程
extern struct vm_thread *g_all_threads[];
extern int g_all_threads_count;
#define add_thread(t) g_all_threads[g_all_threads_count++] = t

extern Object *g_app_class_loader;
extern Object *g_platform_class_loader;

extern struct property {
    const utf8_t *name;
    const utf8_t *value;
} g_properties[];

#define PROPERTIES_MAX_COUNT 128
extern int g_properties_count;

typedef struct {

} InitArgs;

// jvms规定函数最多有255个参数，this也算，long和double占两个长度
#define METHOD_PARAMETERS_MAX_COUNT 255

// jvms数组最大维度为255 
#define ARRAY_MAX_DIMENSIONS 255

#define INFO_MSG_MAX_LEN 1024

#define MAIN_THREAD_NAME "main" // name of main thread
#define GC_THREAD_NAME "gc"     // name of gc thread

#define vm_malloc malloc
#define vm_calloc(len) calloc(1, len)
#define vm_realloc realloc

#define ARRAY_LENGTH(_arr_) (sizeof(_arr_)/sizeof(*(_arr_))) 

#define BUILD_ARRAY(_arr, _len, _init_func, ...) \
do { \
    (_arr) = vm_malloc((_len) * sizeof(*(_arr))); \
    for (int _i = 0; _i < (_len); _i++) { \
        _init_func((_arr) + _i, __VA_ARGS__); \
    } \
} while(false)

#define BUILD_ARRAY0(_arr, _len, _rvalue) \
do { \
    (_arr) = vm_malloc((_len) * sizeof(*(_arr))); \
    for (int _i = 0; _i < (_len); _i++) { \
        (_arr)[_i] = _rvalue; \
    } \
} while(false)

#define printvm(...) \
do { \
    printf("%s: %d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
} while(false)

#define println(...) do { printvm(__VA_ARGS__); printf("\n"); } while(0)

/* ------- 配置日志级别 ------ */
// 0: ERR
// 1: WARNING
// 2: DEBUG
// 3: TRACE
// 4: VERBOSE
#define LOG_LEVEL 0

#define ERR     println
#define WARN    println
#define DEBUG   println
#define TRACE   println
#define VERBOSE println

#if (LOG_LEVEL < 4)
#undef VERBOSE
#define VERBOSE(...)
#endif

#if (LOG_LEVEL < 3)
#undef TRACE
#define TRACE(...)
#endif

#if (LOG_LEVEL < 2)
#undef DEBUG
#define DEBUG(...)
#endif

#if (LOG_LEVEL < 1)
#undef WARNING
#define WARNING(...)
#endif

// 出现异常，退出jvm
#define JVM_PANIC(...) \
do { \
    printvm("fatal error! "); \
    printf(__VA_ARGS__); \
    exit(-1); \
} while(false)

// 退出jvm
#define JVM_EXIT exit(0);

/*---------------------------------------- System Info -------------------------------------------*/

int processor_number();

int page_size();

// 返回操作系统的名称。e.g. window 10
const char *os_name();

// 返回操作系统的架构。e.g. amd64
const char *os_arch();

const char *get_file_separator();
const char *get_path_separator();
const char *get_line_separator();

/*------------------------------------------ Class Path  -----------------------------------------*/

void set_bootstrap_classpath(const char *bcp);

void set_classpath(const char *cp);
const char *get_classpath();

/*
 * Read JDK 类库中的类，不包括Array Class.
 * xxx/xxx/xxx
 */
u1 *read_boot_class(const utf8_t *class_name, size_t *bytecode_len);

/*--------------------------------------------- Slot ---------------------------------------------*/

// 一个slot_t类型必须可以容纳 jbool, jbyte, jchar, jshort，jint，jfloat, jref 称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef intptr_t slot_t;

_Static_assert(sizeof(slot_t) >= sizeof(jbool), "");
_Static_assert(sizeof(slot_t) >= sizeof(jbyte), "");
_Static_assert(sizeof(slot_t) >= sizeof(jchar), "");
_Static_assert(sizeof(slot_t) >= sizeof(jshort), "");
_Static_assert(sizeof(slot_t) >= sizeof(jint), "");
_Static_assert(sizeof(slot_t) >= sizeof(jfloat), "");
_Static_assert(sizeof(slot_t) >= sizeof(jref), "");
_Static_assert(2*sizeof(slot_t) >= sizeof(jlong), "");
_Static_assert(2*sizeof(slot_t) >= sizeof(jdouble), "");

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

/* setter */

static inline void slot_set_int(slot_t *slots, jint v)       
{
    assert(slots != NULL); 
    ISLOT(slots) = v; 
}

static inline void slot_set_byte(slot_t *slots, jbyte v)     
{ 
    assert(slots != NULL); 
    slot_set_int(slots, v);
}

static inline void slot_set_bool(slot_t *slots, jbool v)    
{
    assert(slots != NULL); 
    slot_set_int(slots, v); 
}
#define slot_set_boolean slot_set_bool

static inline void slot_set_char(slot_t *slots, jchar v)    
{ 
    assert(slots != NULL); 
    slot_set_int(slots, v); 
}

static inline void slot_set_short(slot_t *slots, jshort v) 
{ 
    assert(slots != NULL);
    slot_set_int(slots, v); 
}

static inline void slot_set_float(slot_t *slots, jfloat v)  
{ 
    assert(slots != NULL);
    FSLOT(slots) = v;
}
    
static inline void slot_set_long(slot_t *slots, jlong v)  
{
    assert(slots != NULL); 
    LSLOT(slots) = v;
}

static inline void slot_set_double(slot_t *slots, jdouble v)
{ 
    assert(slots != NULL);
    DSLOT(slots) = v; 
}

static inline void slot_set_ref(slot_t *slots, jref v)      
{ 
    assert(slots != NULL); 
    RSLOT(slots) = v; 
}

/* getter */

#define slot_get_byte(slot_point)   JINT_TO_JBYTE(ISLOT(slot_point))
#define slot_get_bool(slot_point)   JINT_TO_JBOOL(ISLOT(slot_point))
#define slot_get_boolean slot_get_bool
#define slot_get_char(slot_point)   JINT_TO_JCHAR(ISLOT(slot_point))
#define slot_get_short(slot_point)  JINT_TO_JSHORT(ISLOT(slot_point))
#define slot_get_int(slot_point)    ISLOT(slot_point)
#define slot_get_float(slot_point)  FSLOT(slot_point)
#define slot_get_long(slot_point)   LSLOT(slot_point)
#define slot_get_double(slot_point) DSLOT(slot_point)
#define slot_get_ref(slot_point)    RSLOT(slot_point)

/* builder */

static inline slot_t islot(jint v)
{
    slot_t s;
    slot_set_int(&s, v);
    return s;
}

static inline slot_t fslot(jfloat v)
{
    slot_t s;
    slot_set_float(&s, v);
    return s;
}

static inline slot_t rslot(jref v)
{
    slot_t s;
    slot_set_ref(&s, v);
    return s;
}

/*---------------------------------------- Bytecode Reader ---------------------------------------*/

typedef struct bytecode_reader {
    u1 *bytecode;
    size_t len; // bytecode len

    size_t pc; // program count
} BytecodeReader;

void bcr_init(BytecodeReader *, u1 *bytecode, size_t len);
u1 *bcr_curr_pos(BytecodeReader *);
// @offset: 相对于当前位置的偏移
void bcr_setu1(BytecodeReader *, int offset, u1 value);
// @offset: 相对于当前位置的偏移
// 注意，setu2方法要和readu2方法相配套。
void bcr_setu2(BytecodeReader *, int offset, u2 value);
bool bcr_has_more(BytecodeReader *);
void bcr_skip(BytecodeReader *, int offset);
/*
 * todo 函数干什么用的
 */
void bcr_align4(BytecodeReader *);
void bcr_read_bytes(BytecodeReader *, u1 *buf, size_t len);
s1 bcr_reads1(BytecodeReader *);
u1 bcr_readu1(BytecodeReader *);
u2 bcr_readu2(BytecodeReader *);
u2 bcr_peeku2(BytecodeReader *);
s2 bcr_reads2(BytecodeReader *);
u4 bcr_readu4(BytecodeReader *);
u8 bcr_readu8(BytecodeReader *);
s4 bcr_reads4(BytecodeReader *);
/*
 * 读 @n 个s4数据到 @s4s 数组中
 */
void bcr_reads4s(BytecodeReader *, int n, s4 *s4s);

/*------------------------------------------ Prim Types ------------------------------------------*/

void init_prims();

bool is_prim_class_name(const utf8_t *class_name);
bool is_prim_descriptor(utf8_t descriptor);
bool is_prim_wrapper_class_name(const utf8_t *class_name);
const utf8_t *get_prim_array_class_name(const utf8_t *class_name);
const utf8_t *get_prim_class_name(utf8_t descriptor);
const utf8_t *get_prim_descriptor_by_wrapper_class_name(const utf8_t *wrapper_class_name);
const utf8_t *get_prim_descriptor_by_class_name(const utf8_t *class_name);
// const slot_t *primObjUnbox(const Object *box);

jref voidBox();
jref byteBox(jbyte x);
jref boolBox(jbool x);
jref charBox(jchar x);
jref shortBox(jshort x);
jref intBox(jint x);
jref floatBox(jfloat x);
jref longBox(jlong x);
jref doubleBox(jdouble x);

/*----------------------------------------- Interpreter ------------------------------------------*/

/*
 * 此方法用于虚拟机主动调用函数，
 * 函数调用指令（invokestatic, invokespecial, ...）中不能使用
 */

slot_t *exec_java_func(Method *, const slot_t *args);
#define exec_java_func_r(_method, _args) slot_get_ref(exec_java_func(_method, _args))

// Object[] args;
slot_t *exec_java_func0(Method *, jref _this, jarrRef args);

slot_t *exec_java_func1(Method *method, jref arg);

slot_t *exec_java_func2(Method *method, jref arg1, jref arg2);

slot_t *exec_java_func3(Method *method, jref arg1, jref arg2, jref arg3);

/*----------------------------------------- Constant Pool ----------------------------------------*/


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

/*----------------------------------------- Class Loader -----------------------------------------*/

// Cache 常用的类
extern Class *g_object_class;
extern Class *g_class_class;
extern Class *g_string_class;

void init_class_loader();

#define BOOT_CLASS_LOADER ((jref) NULL)

/*
 * 加载 JDK 类库中的类，不包括Array Class.
 * xxx/xxx/xxx
 */
Class *load_boot_class(const utf8_t *name);

Class *loadArrayClass(Object *loader, const utf8_t *arr_class_name);

static inline Class *loadArrayClass0(const utf8_t *arr_class_name)
{
    assert(arr_class_name != NULL);
    assert(arr_class_name[0] == '['); // must be array class name
    return loadArrayClass(g_app_class_loader, arr_class_name);
};

// Load byte[].class, boolean[].class, char[].class, short[].class, 
//      int[].class, float[].class, long[].class, double[].class.
Class *load_type_array_class(ArrayType type);

const utf8_t *get_boot_package(const utf8_t *name);
PHS *get_boot_packages();

/*
 * @name: 全限定类名，不带 .class 后缀
 *
 * class names:
 *    - primitive types: boolean, byte, int ...
 *    - primitive arrays: [Z, [B, [I ...
 *    - non-array classes: java/lang/Object ...
 *    - array classes: [Ljava/lang/Object; ...
 */
Class *load_class(Object *class_loader, const utf8_t *name);

Class *find_loaded_class(Object *class_loader, const utf8_t *name);

Class *define_class(jref class_loader, u1 *bytecode, size_t len);

Class *define_class1(jref class_loader, jref name,
                     jarrRef bytecode, jint off, jint len, jref protection_domain, jref source);

/*
 * 类的初始化在下列情况下触发：
 * 1. 执行new指令创建类实例，但类还没有被初始化。
 * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
 * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
 * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
 * 5. 执行某些反射操作时。
 *
 * 每个类的此方法只会执行一次。
 *
 * 调用类的类初始化方法。
 * clinit are the static initialization blocks for the class, and static Field initialization.
 */
Class *init_class(Class *c);

Class *link_class(Class *c);

jref get_platform_class_loader();

/*
 * 返回 System Class Loader(sun/misc/Launcher$AppClassLoader) to loader user classes.
 *
 * 继承体系为：
 * java/lang/Object
 *     java/lang/ClassLoader
 *         java/security/SecureClassLoader
 *             java/net/URLClassLoader
 *                 sun/misc/Launcher$AppClassLoader
 */
jref get_app_class_loader();

/* some methods for testing */

#define IS_SLASH_CLASS_NAME(class_name) (strchr(class_name, '.') == NULL)
#define IS_DOT_CLASS_NAME(class_name) (strchr(class_name, '/') == NULL)

// std::unordered_map<const utf8_t *, Class *, Utf8Hash, Utf8Comparator> *getAllBootClasses();
// const std::unordered_set<const Object *> &getAllClassLoaders();

// void printBootLoadedClasses();
// void printClassLoader(Object *class_loader);

/*--------------------------------------------- Class --------------------------------------------*/

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

#if 0
    // vtable 只保存虚方法。
    // 该类所有函数自有函数（除了private, static, final, abstract）和 父类的函数虚拟表。
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
             PHS *str_pool;
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

Field *lookup_field(Class *, const utf8_t *name, const utf8_t *descriptor);
Field *lookup_static_field(Class *, const char *name, const char *descriptor);
Field *lookup_inst_field(Class *, int id);
Field *lookup_inst_field0(Class *, const char *name, const char *descriptor);

bool inject_inst_field(Class *, const utf8_t *name, const utf8_t *descriptor);

Field *get_declared_field(const Class *, const char *name);
Field *get_declared_field0(const Class *, const char *name, const char *descriptor);
Field *get_declared_inst_field(const Class *, int id);

Method *lookup_method(Class *, const char *name, const char *descriptor);
Method *lookup_static_method(Class *, const char *name, const char *descriptor);
Method *lookup_inst_method(Class *, const char *name, const char *descriptor);

/*
 * get在本类中定义的类，不包括继承的。
 */
Method *get_declared_method(Class *, const utf8_t *name, const utf8_t *descriptor);
Method *get_declared_method_noexcept(Class *c, const utf8_t *name, const utf8_t *descriptor);
Method *get_declared_static_method(Class *, const utf8_t *name, const utf8_t *descriptor);
Method *get_declared_inst_method(Class *, const utf8_t *name, const utf8_t *descriptor);
// polymorphic signature
Method *get_declared_poly_method(Class *, const utf8_t *name);

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
Class *component_class(Class *);

/*
 * Returns the representing the element class of an array class.
 * If this class does not represent an array class this method returns null.
 *
 * like [[[I 的元素类是 int.class
 */
Class *element_class(Class *);

jstrRef intern_string(jstrRef s);

/*-------------------------------------------- Method --------------------------------------------*/

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
    u2 arg_slot_count;

    u1 *code;
    size_t code_len;

    // JNINativeMethod *native_method; // present only if native
    const utf8_t *native_simple_descriptor; //  present only if native
    
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
     * 判断此方法是否由 invokevirtual 指令调用，
     * final方法虽然非虚，但也由 invokevirtual 调用。
     * todo
     * 一个 final non-private 方法则可以覆写基类的虚方法，并且可以被基类引用通过invokevirtual调用到。
     * 参考 https://www.zhihu.com/question/45131640
     */
    // [[nodiscard]] bool isVirtual() const
    // {
    //     int acc = this->access_flags;
    //     return !ACC_IS_PRIVATE(acc) && !ACC_IS_STATIC(acc) && !utf8::equals(name, S(object_init));
    // }
    
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
 * Is signature polymorphic（签名多态性） method?
 *
 * A method is signature polymorphic if all of the following are true:
 * 1. It is declared in the java.lang.invoke.MethodHandle class or the java.lang.invoke.VarHandle class.
 * 2. It has a single formal parameter of type Object[].
 * 3. It has the ACC_VARARGS and ACC_NATIVE flags set.
 */
bool is_signature_polymorphic(const Method *);

char *get_method_info(const Method *m);

/*-------------------------------------------- Field ---------------------------------------------*/

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

/*------------------------------------------ Descriptor ------------------------------------------*/

int num_elements_in_method_descriptor(const char *method_descriptor);

bool parse_method_descriptor(const char *desc, jref loader, jarrRef *ptypes, jref *rtype);

// 返回结果需要调用者释放(free())
char *unparse_method_descriptor(jarrRef ptypes /*Class *[]*/, jclsRef rtype);

// 返回结果需要调用者释放(free())
// @method_type: Object of java.lang.invoke.MethodType
char *unparse_method_descriptor0(jref method_type);

/*-------------------------------------------- Object --------------------------------------------*/

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
        PHM *classes;
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
#define alloc_array0(arr_class_name, arr_len) alloc_array(loadArrayClass0(arr_class_name), arr_len)
#define alloc_string_array(arr_len) alloc_array0(S(array_java_lang_String), arr_len)
#define alloc_class_array(arr_len)  alloc_array0(S(array_java_lang_Class), arr_len)
#define alloc_object_array(arr_len) alloc_array0(S(array_java_lang_Object), arr_len)

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

void array_copy(jarrRef dst, jint dst_pos, const jarrRef src, jint src_pos, jint len);


/* String */

utf8_t *string_to_utf8(jstrRef so);
unicode_t *string_to_unicode(jstrRef so);
bool string_equals(jstrRef x, jstrRef y);
size_t string_hash(jstrRef x);
jsize get_string_length(jstrRef so);
jsize get_string_uft_length(jstrRef so);

/*------------------------------------------ Exception -------------------------------------------*/

#define LONG_JMP_JAVA_EXCEP_VALUE 1 // 不可以是0

void raise_exception(const char *exception_class_name, const char *msg);

void print_stack_trace(struct object *e);

/* Method Handle */

void init_method_handle();

jref findMethodType(const utf8_t *desc, jref loader);

jref linkMethodHandleConstant(Class *caller_class, int ref_kind,
                              Class *defining_class, const char *name, jref type);

// java/lang/invoke/MemberName 类的便利操作函数
// namespace member_name {
//     /*
//      * new MemberName
//      */
//     jref memberName(Method *m, jbyte refKind);

//     jbyte getRefKind(jref memberName);

//     bool isMethod(jref memberName);

//     bool isConstructor(jref memberName);

//     bool isField(jref memberName);

//     bool isType(jref memberName);

//     bool isStatic(jref memberName);
// }

void initMemberName(jref member_name, jref target);
void expandMemberName(jref member_name);
jref resolveMemberName(jref member_name, Class *caller);

// java/lang/invoke/MethodHandles 类的便利操作函数
// namespace method_handles {
    jref getCaller();
// }

// class java_lang_invoke_MemberName {
//     static Field *vmindex;
// public:
//     void init();
// };

/*-------------------------------------------- Thread --------------------------------------------*/

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */


/* Thread states */

#define JVMTI_THREAD_STATE_ALIVE                     0x001
#define JVMTI_THREAD_STATE_TERMINATED                0x002
#define JVMTI_THREAD_STATE_RUNNABLE                  0x004
#define JVMTI_THREAD_STATE_WAITING_INDEFINITELY      0x010
#define JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT      0x020
#define JVMTI_THREAD_STATE_SLEEPING                  0x040
#define JVMTI_THREAD_STATE_WAITING                   0x080
#define JVMTI_THREAD_STATE_IN_OBJECT_WAIT            0x100
#define JVMTI_THREAD_STATE_PARKED                    0x200
#define JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER  0x400

#define CREATING           0x0
#define RUNNING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_RUNNABLE)
#define WAITING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_INDEFINITELY)
#define TIMED_WAITING      (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT)
#define OBJECT_WAIT        (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|WAITING)
#define OBJECT_TIMED_WAIT  (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|TIMED_WAITING)
#define SLEEPING           (JVMTI_THREAD_STATE_SLEEPING|TIMED_WAITING)
#define PARKED             (JVMTI_THREAD_STATE_PARKED|WAITING)
#define TIMED_PARKED       (JVMTI_THREAD_STATE_PARKED|TIMED_WAITING)
#define BLOCKED            JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER
#define TERMINATED         JVMTI_THREAD_STATE_TERMINATED

/* thread priorities */

#define THREAD_MIN_PRIORITY   1
#define THREAD_NORM_PRIORITY  5
#define THREAD_MAX_PRIORITY   10

///* Suspend states */
//
//#define SUSP_NONE      0
//#define SUSP_BLOCKING  1
//#define SUSP_CRITICAL  2
//#define SUSP_SUSPENDED 3
//
///* Park states */
//
//#define PARK_BLOCKED   0
//#define PARK_RUNNING   1
//#define PARK_PERMIT    2

struct frame;

typedef struct vm_thread {
    /*
     * VM stack 中的 Frame 布局：
     * ------------------------------------------------------------------
     * |lvars|Frame|ostack|, |lvars|Frame|ostack|, |lvars|Frame|ostack| ...
     * ------------------------------------------------------------------
     */
    u1 vm_stack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    struct frame *top_frame;

    Object *tobj;  // 所关联的 Object of java.lang.Thread
    pthread_t tid; // 所关联的 local thread 对应的id

    jbool interrupted;

    jmp_buf jmpbuf;
    jref exception;
} Thread;

Thread *create_thread(Object *_tobj, jint priority);

extern Thread *g_main_thread;

Thread *init_main_thread();

void create_vm_thread(void *(*start)(void *), const utf8_t *thread_name);

Thread *get_current_thread();

Thread *thread_from_tobj(Object *tobj);
Thread *thread_from_id(jlong thread_id);

void set_thread_group_and_name(Thread *, Object *group, const char *name);

void set_thread_status(Thread *, jint status);
jint get_thread_status(Thread *);

bool is_thread_alive(Thread *);

struct frame *alloc_frame(Thread *, Method *, bool vm_invoke);
#define pop_frame(_thrd) (_thrd)->top_frame = (_thrd)->top_frame->prev

/*
 * return a reference of java/lang/management/ThreadInfo
 * where maxDepth < 0 to request entire stack dump
 */
jref to_java_lang_management_ThreadInfo(const Thread *, jbool locked_monitors, jbool locked_synchronizers, jint max_depth);

int count_stack_frames(const Thread *);

/*
 * return [Ljava/lang/StackTraceElement;
 * where @max_depth < 0 to request entire stack dump
 */
jarrRef dump_thread(const Thread *, int max_depth);

/*--------------------------------------------- Frame --------------------------------------------*/

typedef struct frame Frame;

struct frame {
    Method *method;
    BytecodeReader reader;

    /*
     * this frame 执行的函数是否由虚拟机调用
     * 由虚拟机调用的函数不会将返回值压入下层frame的栈中，
     * 也不会后续执行其下层frame，而是直接返回。
     */
    bool vm_invoke;

    Frame *prev;

    slot_t *lvars;   // local variables
    slot_t *ostack;  // operand stack
};

void init_frame(Frame *_this, Method *m, bool vm_invoke, slot_t *lvars, slot_t *ostack, Frame *prev);

// push to ostack.
static inline void ostack_push(Frame *f, slot_t v)   { *f->ostack++ = v; }
static inline void ostack_pushi(Frame *f, jint v)    { slot_set_int(f->ostack, v); f->ostack++; }
static inline void ostack_pushf(Frame *f, jfloat v)  { slot_set_float(f->ostack, v); f->ostack++; }
static inline void ostack_pushl(Frame *f, jlong v)   { slot_set_long(f->ostack, v); f->ostack += 2; }
static inline void ostack_pushd(Frame *f, jdouble v) { slot_set_double(f->ostack, v); f->ostack += 2; }
static inline void ostack_pushr(Frame *f, jref v)    { slot_set_ref(f->ostack, v); f->ostack++; }

// pop from ostack.
static inline jint    ostack_popi(Frame *f) { f->ostack--;    return slot_get_int(f->ostack); }
static inline jfloat  ostack_popf(Frame *f) { f->ostack--;    return slot_get_float(f->ostack); }
static inline jlong   ostack_popl(Frame *f) { f->ostack -= 2; return slot_get_long(f->ostack); }
static inline jdouble ostack_popd(Frame *f) { f->ostack -= 2; return slot_get_double(f->ostack); }
static inline jref    ostack_popr(Frame *f) { f->ostack--;    return slot_get_ref(f->ostack); }

// the end address of this frame
#define get_frame_end_address(_frame) ((intptr_t)((_frame)->ostack + (_frame)->method->max_stack))

#define clear_frame_stack(_frame) (_frame)->ostack = (slot_t *)((_frame) + 1)

char *get_frame_info(const Frame *);

#endif //CABIN_CABIN_H
