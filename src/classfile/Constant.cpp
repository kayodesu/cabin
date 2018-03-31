/*
 * Author: Jia Yang
 */

#include "Constant.h"

Constant* Constant::parseConstant(BytecodeReader &reader) {
    Constant *c = nullptr;
    u1 tag = reader.readu1();
    switch (tag) {
        case CLASS_CONSTANT:
            c = new ClassConstant(reader);
            break;
        case FIELD_REF_CONSTANT:
            c = new MemberRefConstant(reader);
            break;
        case METHOD_REF_CONSTANT:
            c = new MemberRefConstant(reader);
            break;
        case INTERFACE_METHOD_REF_CONSTANT:
            c = new MemberRefConstant(reader);
            break;
        case STRING_CONSTANT:
            c = new StringConstant(reader);
            break;
        case INTEGER_CONSTANT:
            c = new IntegerConstant(reader);
            break;
        case FLOAT_CONSTANT:
            c = new FloatConstant(reader);
            break;
        case LONG_CONSTANT:
            c = new LongConstant(reader);
            break;
        case DOUBLE_CONSTANT:
            c = new DoubleConstant(reader);
            break;
        case NAME_AND_TYPE_CONSTANT:
            c = new NameAndTypeConstant(reader);
            break;
        case UTF8_CONSTANT:
            c = new Utf8Constant(reader);
            break;
        case METHOD_HANDLE_CONSTANT:
            c = new MethodHandleConstant(reader);
            break;
        case METHOD_TYPE_CONSTANT:
            c = new MethodTypeConstant(reader);
            break;
        case INVOKE_DYNAMIC_CONSTANT:
            c = new InvokeDynamicConstant(reader);
            break;
        default:
            break;
    }
    if (c == nullptr) {
        jvmAbort("error. tag = %d\n", tag);  // todo
    }
    c->tag = tag;
    return c;
}