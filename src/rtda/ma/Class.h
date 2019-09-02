/*
 * Author: kayo
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include "../../jtypes.h"
#include "../../loader/ClassLoader.h"
#include "../thread/Frame.h"
#include "../primitive_types.h"
#include "Access.h"
#include "../../loader/bootstrap_class_loader.h"
#include "ConstantPool.h"

class Field;
class Method;
class BytecodeReader;
class ClassObject;
class ArrayClass;

struct Class: public Access {
    u4 magic;
    u2 minor_version;
    u2 major_version;

    ConstantPool cp;

    // Object of java/lang/Class of this class
    // 通过此字段，每个Class结构体实例都与一个类对象关联。
    ClassObject *clsobj;

    const char *pkgName;

    // 必须是全限定类名
    const char *className;

    // 如果类没有<clinit>方法，是不是inited直接职位true  todo
    bool inited = false; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    ClassLoader *loader; // todo

    Class *superClass = nullptr;

    std::vector<Class *> interfaces;

    /*
     * 本类中定义的所有方法（不包括继承而来的）
     * 所有的 public functions 都放在了最前面，
     * 这样，当外界需要所有的 public functions 时，可以返回此指针，数量就是 public_methods_count
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
     * 这样，当外界需要所有的 public fields 时，可以返回此指针，数量就是 public_fields_count
     *
     * todo 接口中的变量怎么处理
     */
    std::vector<Field *> fields;
    u2 publicFieldsCount = 0;

    // instance_field_count 有可能大于 fields_count，因为 instance_field_count 包含了继承过来的 field.
    // 类型二统计为两个数量
    int instFieldsCount;

    /*
     * 类型二统计为两个数量
     */
    int staticFieldsCount;
    slot_t *staticFieldsValues; // 保存所有类变量的值

    // vtable 只保存虚方法。
    // 该类所有函数自有函数（除了private, static, final, abstract）和 父类的函数虚拟表。
//    struct {
//        const char *name; // method name
//        const char *descriptor; // method descriptor
//        Method *method;
//    } *vtable; // todo
//    int vtable_len;
    std::vector<Method *> vtable;

    void *itable; // todo

//    struct bootstrap_methods_attribute *bootstrap_methods_attribute;

    struct {
        Object *clazz = nullptr;      // the immediately enclosing class
        Object *name = nullptr;       // the immediately enclosing method or constructor's name (can be null).
        Object *descriptor = nullptr; // the immediately enclosing method or constructor's descriptor (null if name is).
    } enclosing;

    bool deprecated = false;
    const char *signature;
    const char *sourceFileName;

private:
    // 计算字段的个数，同时给它们编号
    void calcFieldsId();
    void parseAttribute(BytecodeReader &r);

    // 根据类名生成包名
    const void genPkgName();

protected:
    Class(ClassLoader *loader, const char *className)
            : className(className), loader(loader) { }

    void createVtable();

public:
    Class(ClassLoader *loader, const u1 *bytecode, size_t len);
    ~Class();

    /*
     * 类的初始化在下列情况下触发：
     * 1. 执行new指令创建类实例，但类还没有被初始化。
     * 2. 执行 putstatic、getstatic 指令存取类的静态变量，但声明该字段的类还没有被初始化。
     * 3. 执行 invokestatic 调用类的静态方法，但声明该方法的类还没有被初始化。
     * 4. 当初始化一个类时，如果类的超类还没有被初始化，要先初始化类的超类。
     * 5. 执行某些反射操作时。
     *
     * 调用类的类初始化方法。
     * clinit are the static initialization blocks for the class, and static Field initialization.
     */
    void clinit();

    Field *lookupField(const char *name, const char *descriptor);
    Field *lookupStaticField(const char *name, const char *descriptor);
    Field *lookupInstField(const char *name, const char *descriptor);

    Method *lookupMethod(const char *name, const char *descriptor);
    Method *lookupStaticMethod(const char *name, const char *descriptor);
    Method *lookupInstMethod(const char *name, const char *descriptor);

    /*
     * 有可能返回NULL todo
     * get在本类中定义的类，不包括继承的。
     */
    Method *getDeclaredMethod(const char *name, const char *descriptor);
    Method *getDeclaredStaticMethod(const char *name, const char *descriptor);
    Method *getDeclaredInstMethod(const char *name, const char *descriptor);

    std::vector<Method *> getDeclaredMethods(const char *name, bool public_only);

    Method *getConstructor(const char *descriptor);
    std::vector<Method *> getConstructors(bool public_only);

    bool isAccessibleTo(const Class *visitor) const;
    bool isSubclassOf(const Class *father) const;

    void setStaticFieldValue(struct Field *f, const slot_t *value);
    const slot_t *getStaticFieldValue(const struct Field *f);

    bool isArray() const;
    bool isPrimitive() const
    {
        return isPrimitiveClassName(className);
    }

    /*
     * 计算一个类的继承深度。
     * 如：java.lang.Object的继承的深度为0
     * java.lang.Number继承自java.lang.Object, java.lang.Number的继承深度为1.
     */
    int inheritedDepth() const;

    ArrayClass *arrayClass() const;

    std::string toString() const;
};

#endif //JVM_JCLASS_H
