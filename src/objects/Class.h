/*
 * Author: kayo
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include "pthread.h"
#include "../jtypes.h"
#include "class_loader.h"
#include "Modifier.h"
#include "ConstantPool.h"
#include "Object.h"
#include "../util/BytecodeReader.h"
#include "../classfile/Attribute.h"

class Field;
class Method;
class Class;

// java/lang/Class
class Class: public Object {
public:
    enum {
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
        Object *descriptor = nullptr; // the immediately enclosing method or constructor's descriptor (null if name is).
    } enclosing;

    struct InnerClass {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;

        InnerClass(ConstantPool &cp, BytecodeReader &r);
    };
    std::vector<InnerClass> innerClasses;

    bool deprecated = false;
    const char *signature;
    const char *sourceFileName = "Unknown source file";

    struct BootstrapMethod {
        /*
         * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
         * 常量池在该索引处的值必须是一个 CONSTANT_MethodHandle_info 结构。
         * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
         * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
         */
        u2 bootstrapMethodRef;

            /*
             * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
             * 常量池在该索引出必须是下列结构之一：
             * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
             * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
             */
        std::vector<u2> bootstrapArguments;

        explicit BootstrapMethod(BytecodeReader &r);
        slot_t *resolveArgs(ConstantPool &cp, slot_t *result);
        ~BootstrapMethod();
    };
    std::vector<BootstrapMethod> bootstrapMethods;

    std::vector<Annotation> rtVisiAnnos;   // runtime visible annotations
    std::vector<Annotation> rtInvisiAnnos; // runtime invisible annotations

private:
    // 计算字段的个数，同时给它们编号
    void calcFieldsId();
    void parseAttribute(BytecodeReader &r);

    // 根据类名生成包名
    const void genPkgName();

    void createVtable();
    void createItable();

    u1 *bytecode = nullptr;

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
        return this == c or (c != nullptr and loader == c->loader and utf8::equals(className, c->className));
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
    const utf8_t *moduleName;
    const utf8_t *moduleMainClass = nullptr;
    /*
     * The value of the module_flags item is as follows:
        0x0020 (ACC_OPEN)
        Indicates that this module is open.
        0x1000 (ACC_SYNTHETIC)
        Indicates that this module was not explicitly or implicitly declared.
        0x8000 (ACC_MANDATED)
        Indicates that this module was implicitly declared.
     */
    u2 module_flags;
    // If moduleVersion is NULL, then no version information about the current module is present.
    const utf8_t *moduleVersion;
    std::vector<const utf8_t *> modulePackages;

    struct ModuleRequire {
        const utf8_t *requireModuleName;
        /*
         * The value of the requires_flags item is as follows:
            0x0020 (ACC_TRANSITIVE)
            Indicates that any module which depends on the current module,
            implicitly declares a dependence on the module indicated by this entry.
            0x0040 (ACC_STATIC_PHASE)
            Indicates that this dependence is mandatory in the static phase, i.e., at
            compile time, but is optional in the dynamic phase, i.e., at run time.
            0x1000 (ACC_SYNTHETIC)
            Indicates that this dependence was not explicitly or implicitly declared
            in the source of the module declaration.
            0x8000 (ACC_MANDATED)
            Indicates that this dependence was implicitly declared in the source of
            the module declaration.
            If the current module is not java.base, and the class file version number
            is 54.0 or above, then neither ACC_TRANSITIVE nor ACC_STATIC_PHASE
            may be set in requires_flags.
         */
        u2 flags;
        // If moduleVersion is NULL, then no version information about the current module is present.
        const utf8_t *version;

        explicit ModuleRequire(ConstantPool &cp, BytecodeReader &r) {
            requireModuleName = cp.moduleName(r.readu2());
            flags = r.readu2();
            u2 v = r.readu2();
            version = v == 0 ? nullptr : cp.utf8(v);
        }
    };
    std::vector<ModuleRequire> requires;

    struct ModuleExport {
        const utf8_t *exportPackageName;
        /*
         * The value of the exports_flags item is as follows:
            0x1000 (ACC_SYNTHETIC)
            Indicates that this export was not explicitly or implicitly declared in
            the source of the module declaration.
            0x8000 (ACC_MANDATED)
            Indicates that this export was implicitly declared in the source of the
            module declaration.
         */
        u2 flags;
        // denoting a module whose code can access the types and members in this exported package.
        std::vector<const utf8_t *> exports_to;

        explicit ModuleExport(ConstantPool &cp, BytecodeReader &r)
        {
            exportPackageName = cp.packageName(r.readu2());
            flags = r.readu2();
            u2 exports_to_count = r.readu2();
            for (u2 i = 0; i < exports_to_count; i++) {
                exports_to.push_back(cp.moduleName(r.readu2()));
            }
        };
    };
    std::vector<ModuleExport> exports;

    struct ModuleOpen {
        const utf8_t *openPackageName;
        /*
         * The value of the opens_flags item is as follows:
            0x1000 (ACC_SYNTHETIC)
            Indicates that this opening was not explicitly or implicitly declared in
            the source of the module declaration.
            0x8000 (ACC_MANDATED)
            Indicates that this opening was implicitly declared in the source of the
            module declaration.
         */
        u2 flags;
        // denoting a module whose code can access the types and members in this opened package.
        std::vector<const utf8_t *> opens_to;

        explicit ModuleOpen(ConstantPool &cp, BytecodeReader &r)
        {
            openPackageName = cp.packageName(r.readu2());
            flags = r.readu2();
            u2 exports_to_count = r.readu2();
            for (u2 i = 0; i < exports_to_count; i++) {
                opens_to.push_back(cp.moduleName(r.readu2()));
            }
        };
    };
    std::vector<ModuleOpen> opens;

    // todo 说明
    std::vector<const utf8_t *> uses;

    struct ModuleProvide {
        // representing a service interface for which the current module provides a service implementation.
        const utf8_t *className;
        std::vector<const utf8_t *> provides_with;

        explicit ModuleProvide(ConstantPool &cp, BytecodeReader &r)
        {
            className = cp.className(r.readu2());
            u2 provides_with_count = r.readu2();
            for (u2 i = 0; i < provides_with_count; i++) {
                provides_with.push_back(cp.className(r.readu2()));
            }
        }
    };
    // todo 说明
    std::vector<ModuleProvide> provides;

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
    
    /*---------------------- for String class ----------------------*/
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
};

#endif //JVM_JCLASS_H
