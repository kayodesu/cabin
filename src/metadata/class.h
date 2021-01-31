#ifndef CABIN_CLASS_H
#define CABIN_CLASS_H

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include <mutex>
#include "../cabin.h"
#include "constant_pool.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../objects/string_object.h"
#include "../util/bytecode_reader.h"
#include "../classfile/attributes.h"
#include "../classfile/constants.h"
#include "../heap/heap.h"

class Method;
class Field;

/*
 * The metadata of a class.
 */

class Class {
public:
    enum State {
        EMPTY,
        LOADED,
        LINKED,
        INITING,
        INITED
    } state = EMPTY;

    ConstantPool cp;

    const utf8_t *pkg_name; // 包名之间以 '.' 分隔。

    // 必须是全限定类名，包名之间以 '/' 分隔。
    const utf8_t *class_name;

    int access_flags;

    bool inited = false; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    Object *java_mirror = nullptr;

    // the class loader who loaded this class
    // 可能为null，表示 bootstrap class loader.
    Object *loader;

    Class *super_class = nullptr;

    // 本类声明实现的interfaces，父类声明实现的不包括在内。
    // 但如果父类声明实现，本类也声明了实现的接口，则包括在内。
    std::vector<Class *> interfaces;

    // 本类所实现的所有interfaces，包括本类和所有祖先类声明实现接口。
    // 各接口之间相互独立（没有重复的，且没有继承关系）。
    std::unordered_set<Class *> indep_interfaces; // independent interfaces

    Class *nest_host = nullptr;
    std::vector<Class *> nest_members;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面
     */
    std::vector<Method *> methods;
    u2 public_methods_count = 0;

    /*
     * 本类中所定义的变量（不包括继承而来的）
     * include both class variables and instance variables,
     * declared by this class or interface type.
     * 类型二统计为两个数量
     *
     * 所有的 public fields 都放在了最前面，
     *
     * todo 接口中的变量怎么处理
     */
    // Field *fields = nullptr;
    // u2 field_count = 0;        // declared fields count
    std::vector<Field *> fields;    
    u2 public_fields_count = 0; // declared public fields count

    // instFieldsCount 有可能大于 fieldsCount，因为 instFieldsCount 包含了继承过来的 field.
    // 类型二统计为两个数量
    int inst_fields_count = 0;

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

    struct {
        Class *clazz = nullptr;       // the immediately enclosing class
        Object *name = nullptr;       // the immediately enclosing method or constructor's name (can be null).
        Object *descriptor = nullptr; // the immediately enclosing method or constructor's type (null if name is).
    } enclosing;

    std::vector<InnerClass> inner_classes;

    bool deprecated = false;
    const char *signature = nullptr;
    const char *source_file_name = nullptr;

    std::vector<BootstrapMethod> bootstrap_methods;

    std::vector<Annotation> rt_visi_annos;   // runtime visible annotations
    std::vector<Annotation> rt_invisi_annos; // runtime invisible annotations

    // Hash
    size_t operator()(const Class *c) const {
        return (uintptr_t) c;
    }

    // Equals
    bool operator()(const Class *c1, const Class *c2) const {
        if (c1 == c2)
            return true;
        if (c1 == nullptr || c2 == nullptr)
            return false;
        return c1->equals(c2);
//        return (c1->loader == c2->loader && utf8::equals(c1->class_name, c2->class_name));
    }

private:
    // 计算字段的个数，同时给它们编号
    void calcFieldsId();
    void parseAttribute(BytecodeReader &r);

    // 根据类名生成包名
    void genPkgName();
    
    void createVtable();
    void createItable();
    void generateIndepInterfaces();

    u1 *bytecode = nullptr;

    std::mutex clinit_mutex;

    Class(Object *loader, u1 *bytecode, size_t len);

    // 创建数组或 primitive class，这两种类型的类由虚拟机直接生成。
    explicit Class(const char *class_name);

    // 创建 array class，由虚拟机直接生成。
    Class(Object *loader, const char *class_name);

public:
    ~Class();

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
    void clinit();

    /*
     * 比较两个类是否相等
     */
    bool equals(const Class *c) const
    {
        return this == c
               or (c != nullptr and loader == c->loader and utf8::equals(class_name, c->class_name));
    }

    size_t objectSize() const;       // for non array class
    size_t objectSize(jint arr_len); // for array class

    Object *allocObject(); // alloc non array object
    Array *allocArray(jint arr_len);
    Array *allocMultiArray(jint dim, const jint lens[]);

    // Generate the class object of this (aka 'java_mirror' field).
    // This Object 只能由虚拟机创建
    void generateClassObject();

    Field *lookupField(const char *name, const char *descriptor);
    Field *lookupStaticField(const char *name, const char *descriptor);
    Field *lookupInstField(const char *name, const char *descriptor);

    void injectInstField(const utf8_t *name, const utf8_t *descriptor);

    Field *getDeclaredField(const char *name, const char *descriptor) const;
    Field *getDeclaredInstField(int id, bool ensureExist = true);

    Method *lookupMethod(const char *name, const char *descriptor);
    Method *lookupStaticMethod(const char *name, const char *descriptor);
    Method *lookupInstMethod(const char *name, const char *descriptor);

    /*
     * get在本类中定义的类，不包括继承的。
     */
    Method *getDeclaredMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist = true);
    Method *getDeclaredStaticMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist = true);
    Method *getDeclaredInstMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist = true);
    Method *getDeclaredPolymorphicSignatureMethod(const utf8_t *name);

    std::vector<Method *> getDeclaredMethods(const utf8_t *name, bool public_only);

    Method *getConstructor(const utf8_t *descriptor);

    // Class<?>[] parameterTypes;
    Method *getConstructor(Array *parameter_types);

    std::vector<Method *> getConstructors(bool public_only);

    bool isSubclassOf(Class *father);

    /*
     * 计算一个类的继承深度。
     * 如：java.lang.Object的继承的深度为0
     * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
     */
    int inheritedDepth() const;

    bool isPrimClass() const;
    bool isPrimWrapperClass() const;

    // void.class
    bool isVoidClass() const;
    bool isArrayClass() const;

    /*
      * 是否是基本类型的数组（当然是一维的）。
      * 基本类型
      * bool, byte, char, short, int, float, long, double
      * 分别对应的数组类型为
      * [Z,   [B,   [C,   [S,    [I,  [F,    [J,   [D
      */
    bool isPrimArrayClass() const;

    bool isBooleanArrayClass() const  { return strcmp(class_name, "[Z") == 0; }
    bool isByteArrayClass() const     { return strcmp(class_name, "[B") == 0; }
    bool isCharArrayClass() const     { return strcmp(class_name, "[C") == 0; }
    bool isShortArrayClass() const    { return strcmp(class_name, "[S") == 0; }
    bool isIntArrayClass() const      { return strcmp(class_name, "[I") == 0; }
    bool isFloatArrayClass() const    { return strcmp(class_name, "[F") == 0; }
    bool isLongArrayClass() const     { return strcmp(class_name, "[J") == 0; }
    bool isDoubleArrayClass() const   { return strcmp(class_name, "[D") == 0; }

    bool isRefArrayClass() const { return !isPrimArrayClass(); }

    Class *arrayClass() const;

    std::string toString() const;

    bool isInterface() const { return accIsInterface(access_flags); }
    bool isPublic() const    { return accIsPublic(access_flags); }
    bool isProtected() const { return accIsProtected(access_flags); }
    bool isPrivate() const   { return accIsPrivate(access_flags); }
    bool isAbstract() const  { return accIsAbstract(access_flags); }
    bool isStatic() const    { return accIsStatic(access_flags); }
    bool isFinal() const     { return accIsFinal(access_flags); }
    bool isStrict() const    { return accIsStrict(access_flags); }
    bool isSuper() const     { return accIsSuper(access_flags); }
    bool isModule() const    { return accIsModule(access_flags); }

    void setSynthetic() { accSetSynthetic(access_flags); }

    /*---------------------- for module-info.class ----------------------*/
private:
    Module *module = nullptr;
    std::vector<const utf8_t *> module_packages;
    const utf8_t *module_main_class = nullptr;

    /*---------------------- for array class ----------------------*/
private:
    Class *comp_class = nullptr; // component class
    Class *ele_class = nullptr;  // element class
public:
    size_t ele_size = 0;

    /*
     * 返回数组类的维度，非数组return 0
     */
    int dim() const;

    // 判断数组单个元素的大小
    // 除了基本类型的数组外，其他都是引用类型的数组
    // 多维数组是数组的数组，也是引用类型的数组
    size_t getEleSize();

    /*
     * Returns the representing the component class of an array class.
     * If this class does not represent an array class this method returns null.
     *
     * like，依次调用 componentClass():
     * [[I -> [I -> int -> null
     */
    Class *componentClass();

    /*
     * Returns the representing the element class of an array class.
     * If this class does not represent an array class this method returns null.
     *
     * like [[[I 的元素类是 int.class
     */
    Class *elementClass();
    
    /*---------------------- for java.lang.String class ----------------------*/
private:
    // A pool of strings, initially empty, is maintained privately by the String class.
    std::unordered_set<Object *, StrObjHash, StrObjEquals> *str_pool = nullptr;
    std::mutex str_pool_mutex;
public:
    void buildStrPool();
    jstrref intern(const utf8_t *str);
    jstrref intern(jstrref so);

    /*---------------------- for java.lang.Class class ----------------------*/
    // set by VM
    // private transient Module module;
    jref modulexxxxxxx = nullptr;// set by VM  todo

    /* 将 ClassLoader 模块的函数定为友元，Class的构造函数只允许 ClassLoader 模块访问 */

    friend void initClassLoader();
    friend Class *loadBootClass(const utf8_t *name);
    friend Class *defineClass(jref loader, u1 *bytecode, size_t len);
    friend Class *loadArrayClass(Object *loader, const utf8_t *arr_class_name);
};

#endif //CABIN_CLASS_H
