/*
 * Author: Jia Yang
 */

#include "RTCP.h"

using namespace std;
using namespace jvm::convert;

void RTCP::buildUtf8Constant(Constant **&cfcp, size_t index) {
    Utf8Constant *c = (Utf8Constant *) cfcp[index];
    pool[index] = make_pair(UTF8_CONSTANT, new string(decodeMUTF8(c->bytes, c->length)));
}

void RTCP::buildStringConstant(Constant **&cfcp, size_t index) {
    StringConstant *c = static_cast<StringConstant *>(cfcp[index]);
    buildUtf8Constant(cfcp, c->stringIndex);

    pool[index] = make_pair(STRING_CONSTANT, new string(getStr(c->stringIndex)));
}

void RTCP::buildClassConstant(Constant **&cfcp, size_t index) {
    ClassConstant *c = static_cast<ClassConstant *>(cfcp[index]);
    buildUtf8Constant(cfcp, c->nameIndex);  // todo
    pool[index] = make_pair(CLASS_CONSTANT, new string(getStr(c->nameIndex)));
}

void RTCP::buildNameAndTypeConstant(Constant **&cfcp, size_t index) {
    NameAndTypeConstant *c = static_cast<NameAndTypeConstant *>(cfcp[index]);

    buildUtf8Constant(cfcp, c->nameIndex);
    buildUtf8Constant(cfcp, c->descriptorIndex);

    NameAndType *nameAndType = new NameAndType;
    nameAndType->name = getStr(c->nameIndex);
    nameAndType->descriptor = getStr(c->descriptorIndex);
    pool[index] = make_pair(NAME_AND_TYPE_CONSTANT, nameAndType);
}

void RTCP::buildFieldRefConstant(Constant **&cfcp, size_t index) {
    MemberRefConstant *c = static_cast<MemberRefConstant *>(cfcp[index]);

    buildClassConstant(cfcp, c->classIndex);  // todo

    buildNameAndTypeConstant(cfcp, c->nameAndTypeIndex);
    FieldRef *ref = new FieldRef;
    ref->className = getClassName(c->classIndex);
    ref->nameAndType = getNameAndType(c->nameAndTypeIndex);
    ref->field = nullptr;
    pool[index] = make_pair(FIELD_REF_CONSTANT, ref);
}

void RTCP::buildMethodRefConstant(Constant **&cfcp, size_t index) {
    MemberRefConstant *c = static_cast<MemberRefConstant *>(cfcp[index]);

    buildClassConstant(cfcp, c->classIndex);  // todo

    buildNameAndTypeConstant(cfcp, c->nameAndTypeIndex);
    MethodRef *ref = new MethodRef;
    ref->className = getClassName(c->classIndex);
    ref->nameAndType = getNameAndType(c->nameAndTypeIndex);
    ref->method = nullptr;

    pool[index] = make_pair(METHOD_REF_CONSTANT, ref);
}

void RTCP::buildIntegerConstant(Constant **&cfcp, size_t index) {  // todo 大小端
    pool[index] = make_pair(INTEGER_CONSTANT, new jint(bytesToInt32(((IntegerConstant *)cfcp[index])->bytes)));
}

void RTCP::buildFloatConstant(Constant **&cfcp, size_t index) {
    pool[index] = make_pair(FLOAT_CONSTANT, new jfloat(bytesToFloat(((FloatConstant *)cfcp[index])->bytes)));
}

void RTCP::buildLongConstant(Constant **&cfcp, size_t index) {
    pool[index] = make_pair(LONG_CONSTANT, new jlong(bytesToInt64(((LongConstant *)cfcp[index])->bytes)));
}

void RTCP::buildDoubleConstant(Constant **&cfcp, size_t index) {
    pool[index] = make_pair(DOUBLE_CONSTANT, new jdouble(bytesToDouble(((DoubleConstant *)cfcp[index])->bytes)));
}

void RTCP::buildMethodHandleConstant(Constant **&cfcp, size_t index) {
    jprintf("not parse method_handle_constant\n");
    // todo
    pool[index] = make_pair(-1, nullptr);//todo
}

void RTCP::buildMethodTypeConstant(Constant **&cfcp, size_t index) {
    jprintf("not parse method_type_constant\n");
    // todo
    pool[index] = make_pair(-1, nullptr);//todo
}

void RTCP::buildInvokeDynamicConstant(Constant **&cfcp, size_t index) {
    jprintf("not parse method_dynamic_constant\n");
    // todo
    pool[index] = make_pair(-1, nullptr);//todo
}


bool RTCP::checkType(int index, int type) const {
    if (pool[index].first != type) {
        // todo error
        jvmAbort("error. want , bug tag = %d.\n", pool[index].first);
        return false;
    }
    return true;
}

bool RTCP::checkType(int index, int type1, int type2) const {
    if (pool[index].first != type1 && pool[index].first != type2) {
        // todo error
        jvmAbort("error. want, bug tag = %d.\n", pool[index].first);
        return false;
    }
    return true;
}

RTCP::RTCP(Constant **&cfcp, size_t count) {
    pool = new std::pair<u1, void *>[count];

    for (size_t i = 1; i < count; i++) {  // 从1开始
        u1 tag = cfcp[i]->tag;

        if (tag == CLASS_CONSTANT)  buildClassConstant(cfcp, i);
        else if (tag == FIELD_REF_CONSTANT) buildFieldRefConstant(cfcp, i);
        else if (tag == METHOD_REF_CONSTANT) buildMethodRefConstant(cfcp, i);
        else if (tag == INTERFACE_METHOD_REF_CONSTANT) buildMethodRefConstant(cfcp, i); // todo
        else if (tag == STRING_CONSTANT) buildStringConstant(cfcp, i);
        else if (tag == INTEGER_CONSTANT) buildIntegerConstant(cfcp, i);
        else if (tag == FLOAT_CONSTANT) buildFloatConstant(cfcp, i);
        else if (tag == LONG_CONSTANT) buildLongConstant(cfcp, i);
        else if (tag == DOUBLE_CONSTANT) buildDoubleConstant(cfcp, i);
        else if (tag == NAME_AND_TYPE_CONSTANT) buildNameAndTypeConstant(cfcp, i);
        else if (tag == UTF8_CONSTANT) buildUtf8Constant(cfcp, i);
        else if (tag == METHOD_HANDLE_CONSTANT) buildMethodHandleConstant(cfcp, i);
        else if (tag == METHOD_TYPE_CONSTANT) buildMethodTypeConstant(cfcp, i);
        else if (tag == INVOKE_DYNAMIC_CONSTANT) buildInvokeDynamicConstant(cfcp, i);

        if (tag == LONG_CONSTANT || tag == DOUBLE_CONSTANT) {
            i++; // todo
            pool[i] = make_pair(-1, nullptr);  // todo
        }
    }
}