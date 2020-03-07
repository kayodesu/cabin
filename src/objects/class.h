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
#include "../jtypes.h"
#include "class_loader.h"
#include "Modifier.h"
#include "class.h"
#include "object.h"
#include "../util/BytecodeReader.h"
#include "../classfile/attribute.h"
#include "../native/registry.h"
#include "../classfile/constant.h"

class Class;

class Field {
public:
    // 定义此 Field 的类
    Class *clazz;
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    jint modifiers;

    bool deprecated = false;
    const char *signature = nullptr;

private:
    // the declared type(class Object) of this field
    // like, int k; the type of k is int.class
    Class *type = nullptr;
public:
    bool category_two;

    union {
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
            slot_t data[2] = { 0, 0 };
        } staticValue;

        int id = 0;
    };

    std::vector<Annotation> rtVisiAnnos;   // runtime visible annotations
    std::vector<Annotation> rtInvisiAnnos; // runtime invisible annotations

public:
    Field(Class *c, BytecodeReader &r);

    Class *getType();

    bool isPrim() const;

    std::string toString() const;
    friend std::ostream &operator <<(std::ostream &os, const Field &field);

    bool isPublic() const    { return Modifier::isPublic(modifiers); }
    bool isProtected() const { return Modifier::isProtected(modifiers); }
    bool isPrivate() const   { return Modifier::isPrivate(modifiers); }
    bool isStatic() const    { return Modifier::isStatic(modifiers); }
    bool isFinal() const     { return Modifier::isFinal(modifiers); }
    bool isTransient() const { return Modifier::isTransient(modifiers); }
    bool isVolatile() const  { return Modifier::isVolatile(modifiers); }

    void setSynthetic() { Modifier::setSynthetic(modifiers); }
};

//std::ostream &operator<<(std::ostream &os, const Field &field);

class Array;

class Method {
//    Array *parameterTypes = nullptr; // [Ljava/lang/Class;
//    Class *returnType = nullptr;     // Ljava/lang/Class;
    Object *type = nullptr;          // Ljava/lang/invoke/MethodType;
    Array *exceptionTypes = nullptr; // [Ljava/lang/Class;

public:
    // 定义此 Method 的类
    Class *clazz;
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    jint modifiers;

    bool deprecated = false;
    const char *signature = nullptr;

    int vtableIndex = -1;
    int itableIndex = -1;

    u2 maxStack = 0;
    u2 maxLocals = 0;
    u2 arg_slot_count = 0;

    u1 *code = nullptr;
    size_t codeLen = 0;

    native_method_t nativeMethod = nullptr; // present only if native

    std::vector<MethodParameter> parameters;

    /*
     * Each value in the exception_index_table array must be a valid index into
     * the constant_pool table. The constant_pool entry at that index must be a
     * CONSTANT_Class_info structure representing a class type that this
     * method is declared to throw.
     */
    // checked exceptions the method may throw.
    std::vector<u2> checkedExceptions;

    std::vector<std::vector<Annotation>> rtVisiParaAnnos;   // runtime visible parameter annotations
    std::vector<std::vector<Annotation>> rtInvisiParaAnnos; // runtime invisible parameter annotations

    std::vector<Annotation> rtVisiAnnos;   // runtime visible annotations
    std::vector<Annotation> rtInvisiAnnos; // runtime invisible annotations

    ElementValue annotationDefault;
private:
    std::vector<LineNumberTable> line_number_tables;
    std::vector<LocalVariableTable> local_variable_tables;
    std::vector<LocalVariableTypeTable> local_variable_type_tables;;

    void calArgsSlotsCount();
    void parseCodeAttr(BytecodeReader &r);

public:
    Method(Class *c, BytecodeReader &r);

    /*
     * 判断此方法是否由 invokevirtual 指令调用，
     * final方法虽然非虚，但也由 invokevirtual 调用。
     * todo
     * 一个 final non-private 方法则可以覆写基类的虚方法，并且可以被基类引用通过invokevirtual调用到。
     * 参考 https://www.zhihu.com/question/45131640
     */
    bool isVirtual() const
    {
        return !isPrivate() && !isStatic() && !utf8::equals(name, S(object_init));
    }

    static u2 calArgsSlotsCount(const utf8_t *descriptor, bool isStatic);

    Array *getParameterTypes();
    Class *getReturnType();

    /*
     * return the type of the method,
     * Object of java/lang/invoke/MethodType.
     */
    Object *getType();
    Array *getExceptionTypes();

    jint getLineNumber(int pc) const;

    /*
     * @pc, 发生异常的位置
     */
    int findExceptionHandler(Class *exception_type, size_t pc);

    std::string toString() const;

    bool isPublic() const       { return Modifier::isPublic(modifiers); }
    bool isProtected() const    { return Modifier::isProtected(modifiers); }
    bool isPrivate() const      { return Modifier::isPrivate(modifiers); }
    bool isAbstract() const     { return Modifier::isAbstract(modifiers); }
    bool isStatic() const       { return Modifier::isStatic(modifiers); }
    bool isFinal() const        { return Modifier::isFinal(modifiers); }
    bool isSynchronized() const { return Modifier::isSynchronized(modifiers); }
    bool isNative() const       { return Modifier::isNative(modifiers); }
    bool isStrict() const       { return Modifier::isStrict(modifiers); }
    bool isVarargs() const      { return Modifier::isVarargs(modifiers); }

    void setSynthetic() { Modifier::setSynthetic(modifiers); }

private:
    /*
     * 异常处理表
     * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
     * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
     */
    struct ExceptionTable {
        u2 startPc;
        u2 endPc;
        u2 handlerPc;

        struct CatchType {
            bool resolved;
            union {
                Class *clazz; // if resolved
                const char *className; // if not resolved
            } u;
        } *catchType = nullptr;

        ExceptionTable(Class *clazz, BytecodeReader &r);
    };

    std::vector<ExceptionTable> exceptionTables;

public:
    ~Method()
    {
        for (auto &t : exceptionTables)
            delete t.catchType;
    }
};


// 从 1 开始计数，第0位无效
class ConstantPool {
    u1 *_type = nullptr;
    slot_t *_info = nullptr;

public:
    Class *clazz;
    u2 size = 0;

    ConstantPool() = default;

    explicit ConstantPool(Class *clazz, u2 size): clazz(clazz), size(size)
    {
        assert(clazz != nullptr);
        assert(size > 0);

        _type = new u1[size];
        _type[0] = CONSTANT_Invalid; // constant pool 从 1 开始计数，第0位无效

        _info = new slot_t[size];
    }

    ~ConstantPool()
    {
        delete[] _type;
        delete[] _info;
    }

    u1 type(u2 i)
    {
        assert(0 < i && i < size);
        return _type[i];
    }

    void type(u2 i, u1 newType)
    {
        assert(0 < i && i < size);
        _type[i] = newType;
    }

    slot_t info(u2 i)
    {
        assert(0 < i && i < size);
        return _info[i];
    }

    void info(u2 i, slot_t newInfo)
    {
        assert(0 < i && i < size);
        _info[i] = newInfo;
    }

    utf8_t *utf8(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Utf8);
        return (utf8_t *)(_info[i]);
    }

    utf8_t *string(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_String);
        return utf8((u2)_info[i]);
    }

    utf8_t *className(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Class);
        return utf8((u2)_info[i]);
    }

    utf8_t *moduleName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Module);
        return utf8((u2)_info[i]);
    }

    utf8_t *packageName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Package);
        return utf8((u2)_info[i]);
    }

    utf8_t *nameOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_NameAndType);
        return utf8((u2)_info[i]);
    }

    utf8_t *typeOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_NameAndType);
        return utf8((u2) (_info[i] >> 16));
    }

    u2 fieldClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return (u2)_info[i];
    }

    utf8_t *fieldClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return className((u2)_info[i]);
    }

    utf8_t *fieldName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *fieldType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 methodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return (u2)_info[i];
    }

    utf8_t *methodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return className((u2)_info[i]);
    }

    utf8_t *methodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 interfaceMethodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return (u2)_info[i];
    }

    utf8_t *interfaceMethodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return className((u2)_info[i]);
    }

    utf8_t *interfaceMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *interfaceMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodTypeDescriptor(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodType);
        return utf8((u2)_info[i]);
    }

    u2 methodHandleReferenceKind(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodHandle);
        return (u2) _info[i];
    }

    u2 methodHandleReferenceIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodHandle);
        return (u2) (_info[i] >> 16);
    }

    u2 invokeDynamicBootstrapMethodIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return (u2) _info[i];
    }

    utf8_t *invokeDynamicMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *invokeDynamicMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    jint _int(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Integer);
        return ISLOT(_info + i);
    }

    void _int(u2 i, jint newInt)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Integer);
        ISLOT(_info + i) = newInt;
    }

    jfloat _float(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Float);
        return FSLOT(_info + i);
    }

    void _float(u2 i, jfloat newFloat)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Float);
        FSLOT(_info + i) = newFloat;
    }

    jlong _long(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Long);
        return LSLOT(_info + i);
    }

    void _long(u2 i, jlong newLong)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Long);
        LSLOT(_info + i) = newLong;
    }

    jdouble _double(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Double);
        return DSLOT(_info + i);
    }

    void _double(u2 i, jdouble newDouble)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Double);
        DSLOT(_info + i) = newDouble;
    }

    Class *resolveClass(u2 i);
    Method *resolveMethod(u2 i);
    Method *resolveInterfaceMethod(u2 i);
    Field *resolveField(u2 i);
    Object *resolveString(u2 i);
    Object *resolveMethodType(u2 i);
    Object *resolveMethodHandle(u2 i);
};


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
        Object *descriptor = nullptr; // the immediately enclosing method or constructor's descriptor (null if name is).
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
    const void genPkgName();

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
