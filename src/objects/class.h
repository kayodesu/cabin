/*
 * Author: kayo
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <unordered_set>
#include <mutex>
#include <pthread.h>
#include "../vmdef.h"
#include "constant_pool.h"
#include "class_loader.h"
#include "Modifier.h"
#include "class.h"
#include "object.h"
#include "string_object.h"
#include "../util/bytecode_reader.h"
#include "../classfile/attributes.h"
#include "../classfile/constants.h"

class Method;
class Field;

// Object of java/lang/Class
class Class: public Object {
public:
    enum State {
        EMPTY,
        LOADED,
        LINKED,
        INITING,
        INITED
    } state = EMPTY;

    ConstantPool cp;

    const utf8_t *pkgName; // 包名之间以 '.' 分隔。

    // 必须是全限定类名，包名之间以 '/' 分隔。
    const utf8_t *className;

    jint modifiers;

    bool inited = false; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    // the class loader who loaded this class
    // 可能为null，表示 bootstrap class loader.
    Object *loader;

    Class *superClass = nullptr;

    /*
     * 本类明确实现的interfaces，父类实现的不包括在内。
     * 但如果父类实现，本类也声明了实现的接口，则包括在内。
     */
    std::vector<Class *> interfaces;

    Class *nest_host = nullptr;
    std::vector<Class *> nest_members;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面
     */
    std::vector<Method *> methods;
    u2 publicMethodsCount = 0;

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
    std::vector<Field *> fields;
    u2 publicFieldsCount = 0;

//    Field *fields;
//    u2 fields_count;
//    u2 public_fields_count;

    // instFieldsCount 有可能大于 fieldsCount，因为 instFieldsCount 包含了继承过来的 field.
    // 类型二统计为两个数量
    int instFieldsCount = 0;

    // vtable 只保存虚方法。
    // 该类所有函数自有函数（除了private, static, final, abstract）和 父类的函数虚拟表。
    std::vector<Method *> vtable;

    struct ITable {
        std::vector<std::pair<Class *, size_t /* offset */>> interfaces;
        std::vector<Method *> methods;

        ITable() = default;
        ITable(const ITable &itable);
        ITable& operator=(const ITable &itable);
    };

    ITable itable;

    struct {
        Class *clazz = nullptr;       // the immediately enclosing class
        Object *name = nullptr;       // the immediately enclosing method or constructor's name (can be null).
        Object *descriptor = nullptr; // the immediately enclosing method or constructor's type (null if name is).
    } enclosing;

    std::vector<InnerClass> inner_classes;

    bool deprecated = false;
    const char *signature = nullptr;
    const char *sourceFileName = "Unknown source file";

    std::vector<BootstrapMethod> bootstrap_methods;

    std::vector<Annotation> rt_visi_annos;   // runtime visible annotations
    std::vector<Annotation> rt_invisi_annos; // runtime invisible annotations

private:
    // 计算字段的个数，同时给它们编号
    void calcFieldsId();
    void parseAttribute(BytecodeReader &r);

    // 根据类名生成包名
    void genPkgName();

    void createVtable();
    void createItable();

    u1 *bytecode = nullptr;

//    std::mutex clinit_mutex;
    pthread_mutex_t clinitLock = PTHREAD_MUTEX_INITIALIZER;

    Class(Object *loader, u1 *bytecode, size_t len);

    // 创建数组或 primitive class，这两种类型的类由虚拟机直接生成。
    explicit Class(const char *className);

    static Class *newClass(Object *loader, u1 *bytecode, size_t len)
    {
        void *mem = g_heap.allocClass();
        auto c = new(mem) Class(loader, bytecode, len);
        c->state = LOADED;
        return c;
    }

    static Class *newClass(const char *className)
    {
        void *mem = g_heap.allocClass();
        auto c = new(mem) Class(className);
        c->state = LOADED;
        return c;
    }
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

    static size_t getSize()
    {
        return sizeof(Class) + sizeof(slot_t)*CLASS_CLASS_INST_FIELDS_COUNT;
    }

    size_t size() const override
    {
        return getSize();
    }

    /*
     * 比较两个类是否相等
     */
    bool equals(Class *c) const
    {
        return this == c
               or (c != nullptr and loader == c->loader and utf8::equals(className, c->className));
    }

    Field *lookupField(const char *name, const char *descriptor);
    Field *lookupStaticField(const char *name, const char *descriptor);
    Field *lookupInstField(const char *name, const char *descriptor);

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

    std::vector<Method *> getDeclaredMethods(const utf8_t *name, bool public_only);

    Method *getConstructor(const utf8_t *descriptor);

    // Class<?>[] parameterTypes;
    Method *getConstructor(Array *parameterTypes);

    std::vector<Method *> getConstructors(bool public_only);

    bool isSubclassOf(Class *father);

    /*
     * 计算一个类的继承深度。
     * 如：java.lang.Object的继承的深度为0
     * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
     */
    int inheritedDepth() const;

    bool isArrayClass() const;
    bool isPrimClass() const;

    /*
      * 是否是基本类型的数组（当然是一维的）。
      * 基本类型
      * bool, byte, char, short, int, float, long, double
      * 分别对应的数组类型为
      * [Z,   [B,   [C,   [S,    [I,  [F,    [J,   [D
      */
    bool isPrimArrayClass() const;

    bool isArrayObject() const override { return false; }

    Class *arrayClass() const;

    std::string toString() const;

    bool isInterface() const { return Modifier::isInterface(modifiers); }
    bool isPublic() const    { return Modifier::isPublic(modifiers); }
    bool isProtected() const { return Modifier::isProtected(modifiers); }
    bool isPrivate() const   { return Modifier::isPrivate(modifiers); }
    bool isAbstract() const  { return Modifier::isAbstract(modifiers); }
    bool isStatic() const    { return Modifier::isStatic(modifiers); }
    bool isFinal() const     { return Modifier::isFinal(modifiers); }
    bool isStrict() const    { return Modifier::isStrict(modifiers); }
    bool isSuper() const     { return Modifier::isSuper(modifiers); }
    bool isModule() const    { return Modifier::isModule(modifiers); }

    void setSynthetic() { Modifier::setSynthetic(modifiers); }

    /*---------------------- for module-info.class ----------------------*/
private:
    Module *module = nullptr;
    std::vector<const utf8_t *> modulePackages;
    const utf8_t *moduleMainClass = nullptr;

    /*---------------------- for array class ----------------------*/
private:
    Class *compClass = nullptr; // component class
    Class *eleClass = nullptr;  // element class
public:
    size_t eleSize = 0;

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
    std::unordered_set<Object *, StrObjHash, StrObjEquals> *strpool;
    pthread_mutex_t strpoolMutex;
public:
    void buildStrPool();
    Object *intern(const utf8_t *str);
    Object *intern(Object *so);

    /* 将 ClassLoader 模块的函数定为友元，Class的构造函数只允许 ClassLoader 模块访问 */

    friend void initClassLoader();
    friend Class *loadBootClass(const utf8_t *name);
    friend Class *defineClass(jref classLoader, u1 *bytecode, size_t len);

    /*---------------------- for java.lang.Class class ----------------------*/
    // set by VM
   // private transient Module module;
   jref modulexxxxxxx;// set by VM  todo
};

#endif //JVM_JCLASS_H
