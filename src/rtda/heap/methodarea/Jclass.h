/*
 * Author: Jia Yang
 */

#ifndef JVM_JCLASS_H
#define JVM_JCLASS_H

#include <string>
#include <vector>
#include "../../../classfile/ClassFile.h"
#include "../../../loader/ClassLoader.h"
#include "../../../interpreter/slot/Slot.h"
#include "../../../classfile/Constant.h"
#include "../../../BytecodeReader.h"
#include "Jmember.h"
#include "AccessPermission.h"
#include "RTCP.h"
#include "../../../interpreter/StackFrame.h"
#include "PrimitiveType.h"


class JclassObj;
class RTCP;
class Jfield;
class Jmethod;
class StackFrame;

class JclassHead {

};

class Jclass: public AccessPermission {
    void calcStaticFieldId();
    void calcInstanceFieldId();
public:
    u4 magic;
    u2 minorVersion;
    u2 majorVersion;

    // object of java/lang/Class of this class
    JclassObj *classObj;

    std::string pkgName;
    std::string className;
    bool isInited; // 此类是否被初始化过了（是否调用了<clinit>方法）。

    ClassLoader *loader; // todo

    Jclass *superClass;

    Jclass **interfaces;
    int interfacesCount;

    RTCP *rtcp;
    int constantsCount;

    Jmethod *methods;
    int methodsCount;

    Jfield *fields;
    int fieldsCount;

    // instanceFieldCount 有可能大于 fieldsCount，因为instanceFieldCount包含了继承过来的field.
    int instanceFieldCount;
    int staticFieldCount;

    Jvalue *staticFieldValues;

    std::string sourcefileName;

    /*
     * 查找方法
     * 只查找本类，不查找从父类和接口中继承来的
     */
    Jmethod* getMethod(const std::string &name, const std::string &descriptor);

public:
    Jclass() {

    }

    Jclass(ClassLoader *loader, ClassFile *cf);

    JclassObj* getClassObj() {
        return classObj;
    }

    // 调用此类的类初始化方法（clinit）
    void clinit(StackFrame *invokeFrame);

    std::vector<Jfield> getFields(bool publicOnly);

    /*
     * @id: id of the field
     */
    void setStaticField(int id, const Jvalue &v);

    void setStaticField(const std::string &name, const std::string &descriptor, const Jvalue &v);

    bool isSubclassOf(const Jclass *father) const;

    Jfield* lookupField(const std::string &name, const std::string &descriptor);

    Jmethod* getConstructor(const std::string &descriptor);

    /*
     * 查找方法
     * 与getMethod方法不同，此方法会在父类及父接口中查找
     */
    Jmethod* lookupMethod(const std::string &name, const std::string &descriptor);
    Jmethod* lookupStaticMethod(const std::string &name, const std::string &descriptor);
    Jmethod* lookupInstanceMethod(const std::string &name, const std::string &descriptor);

    bool isArray() const {
        return className[0] == '[';
    }

    /*
     * 创建此类的数组类
     */
    Jclass* arrayClass();

    // 如果类D想访问类C，需要满足两个条件之一：C是public，或者C和D在同一个运行时包内。
    // todo 运行时包后续再做，现在先简单的检察包名
    bool isAccessibleTo(const Jclass *visitor) const {
        if (isPublic() || visitor->pkgName == pkgName) {
            return true;
        }
        return false;
    }

//    bool isPrimitive() {
//        for (int i = 0; i < sizeof(primitiveTypes)/sizeof(*primitiveTypes); i++) {
//            if (primitiveTypes[i].wrapperClassName == className) {
//                return true;
//            }
//        }
//        return false;
//    }


    /*
     * 是否是基本类型的数组（当然是一维的）
     * bool, byte, char, short, int, float, long, double
     */
    bool isPrimitiveArray() const {
        if (className.length() != 2 || className[0] != '[')
            return false;

        return strchr("ZBCSIFJD", className[1]) != NULL;
    }

    // 是否是一维数组
    bool isOneDimensionArray() const {
        return isPrimitiveArray() || (className.length() >= 2 && className[0] == '[' && className[1] != '[');
    }

    // 是否是一维引用数组
    bool isOneDimensionReferenceArray() const {
        return isOneDimensionArray() && !isPrimitiveArray();
    }

    // 是否是多维数组
    bool isMultiArray() const {
        return isArray() && !isOneDimensionArray();
    }

    bool isPrimitiveBoolArray() const {
        return className == "[Z";//streq(class_name, "[Z");
    }

    bool isPrimitiveByteArray() const {
        return className == "[B";//streq(class_name, "[B");
    }

    bool isPrimitiveBoolOrByteArray() const {
        return isPrimitiveBoolArray() or isPrimitiveByteArray();
    }

    bool isPrimitiveCharArray() const {
        return className == "[C";
    }

    bool isPrimitiveShortArray() const {
        return className == "[S";
    }

    bool isPrimitiveIntArray() const {
        return className == "[I";
    }

    bool isPrimitiveFloatArray() const {
        return className == "[F";
    }

    bool isPrimitiveLongArray() const {
        return className == "[J";
    }

    bool isPrimitiveDoubleArray() const {
        return className == "[D";
    }

    std::string toString() {
        return className;
    }
};

#endif //JVM_JCLASS_H
