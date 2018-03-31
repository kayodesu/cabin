/*
 * Author: Jia Yang
 */

#ifndef JVM_RUNTIMECONSTANT_H
#define JVM_RUNTIMECONSTANT_H

#include <vector>

#include "Jmember.h"
#include "../../../jvmtype.h"
#include "../../../classfile/Constant.h"
#include "../../../util/encoding.h"

class Jfield;
class Jmethod;

struct NameAndType {
    std::string name;
    std::string descriptor;
};

struct FieldRef {
    std::string className;
    NameAndType nameAndType;
    Jfield *field;
};

struct MethodRef {
    std::string className;
    NameAndType nameAndType;
    Jmethod *method;
} ;//, jinterface_method_ref;

// RunTime Constant Pool
class RTCP {
//    std::vector<std::pair<u1, void *>> pool;
    std::pair<u1, void *> *pool;

    void buildUtf8Constant(Constant **&cfcp, size_t index);
    void buildStringConstant(Constant **&cfcp, size_t index);
    void buildClassConstant(Constant **&cfcp, size_t index);
    void buildNameAndTypeConstant(Constant **&cfcp, size_t index);
    void buildFieldRefConstant(Constant **&cfcp, size_t index);
    void buildMethodRefConstant(Constant **&cfcp, size_t index);
    void buildIntegerConstant(Constant **&cfcp, size_t index);
    void buildFloatConstant(Constant **&cfcp, size_t index);
    void buildLongConstant(Constant **&cfcp, size_t index);
    void buildDoubleConstant(Constant **&cfcp, size_t index);
    void buildMethodHandleConstant(Constant **&cfcp, size_t index);
    void buildMethodTypeConstant(Constant **&cfcp, size_t index);
    void buildInvokeDynamicConstant(Constant **&cfcp, size_t index);

    bool checkType(int index, int type) const;
    bool checkType(int index, int type1, int type2) const;
public:
    RTCP(Constant **&cfcp, size_t count);

    ~RTCP() {
        // todo
    }

    const auto& get(int index) {
        return pool[index];
    }

    jint getInt(int index) {
        checkType(index, INTEGER_CONSTANT);
        return *((jint *)pool[index].second);
    }

    jfloat getFloat(int index) {
        checkType(index, FLOAT_CONSTANT);
        return *((jint *)pool[index].second);
    }

    jlong getLong(int index) const {
        checkType(index, LONG_CONSTANT);
        return *((jlong *)pool[index].second);
    }

    jdouble getDouble(int index) const {
        checkType(index, DOUBLE_CONSTANT);
        return *((jlong *)pool[index].second);
    }

    const std::string& getStr(int index) const {
        checkType(index, UTF8_CONSTANT, STRING_CONSTANT);
        return *((std::string *)pool[index].second);
    }

    const std::string& getClassName(int index) const {
        checkType(index, CLASS_CONSTANT);
        return *((std::string *)pool[index].second);
    }

    const NameAndType& getNameAndType(int index) const {
        checkType(index, NAME_AND_TYPE_CONSTANT);
        return *((NameAndType *)pool[index].second);
    }

    FieldRef& getFieldRef(int index) const {
        checkType(index, FIELD_REF_CONSTANT);
        return *((FieldRef *)pool[index].second);
    }

    MethodRef& getMethodRef(int index) const {
        checkType(index, METHOD_REF_CONSTANT);
        return *((MethodRef *)pool[index].second);
    }
};

#endif //JVM_RUNTIMECONSTANT_H
