/*
 * Author: kayo
 */

#include <vector>
#include <sstream>
#include "../kayo.h"
#include "../slot.h"
#include "attributes.h"
#include "../runtime/thread_info.h"
#include "../objects/class.h"
#include "constants.h"

using namespace std;

void Annotation::read(BytecodeReader &r)
{
    type_index = r.readu2();
    u2 num = r.readu2();
    for (u2 i = 0; i < num; i++) {
        elementValuePairs.emplace_back(ElementValuePair(r));
    }
}

void ElementValue::read(BytecodeReader &r)
{
    tag = r.readu1();
    u2 num;
    switch (tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'S':
        case 'Z':
        case 's':
            const_value_index = r.readu2();
            break;
        case 'c':
            class_info_index = r.readu2();
            break;
        case 'e':
            enum_const_value.type_name_index = r.readu2();
            enum_const_value.const_name_index = r.readu2();
            break;
        case '@':
            annotation_value.read(r);
            break;
        case '[':
            num = r.readu2();
            for (u2 i = 0; i < num; i++) {
//                arrayValue.push_back(new ElementValue(r));
                arrayValue.emplace_back(ElementValue(r));
            }
            break;
        default:
            jvm_abort(NEW_MSG("unknown tag: %d\n", tag)); // todo

            thread_throw(new UnknownError()); // todo
    }
}

ElementValuePair::ElementValuePair(BytecodeReader &r)
{
    element_name_index = r.readu2();
    value.read(r);
}

InnerClass::InnerClass(BytecodeReader &r)
{
    inner_class_info_index = r.readu2();
    outer_class_info_index = r.readu2();
    inner_name_index = r.readu2();
    inner_class_access_flags = r.readu2();
}

BootstrapMethod::BootstrapMethod(BytecodeReader &r)
{
    bootstrapMethodRef = r.readu2();
    u2 num = r.readu2();
    for (u2 i = 0; i < num; i++) {
        bootstrapArguments.push_back(r.readu2());
    }
}

slot_t *BootstrapMethod::resolveArgs(ConstantPool *cp, slot_t *result)
{
    assert(result != nullptr);
    for (u2 i : bootstrapArguments) {
        switch (cp->type(i)) {
            case JVM_CONSTANT_String:
                RSLOT(result) = cp->resolveString(i);
                result++;
                break;
            case JVM_CONSTANT_Class:
                RSLOT(result) = cp->resolveClass(i);
                result++;
                break;
            case JVM_CONSTANT_Integer:
                ISLOT(result) = cp->_int(i);
                result++;
                break;
            case JVM_CONSTANT_Float:
                FSLOT(result) = cp->_float(i);
                result++;
                break;
            case JVM_CONSTANT_Long:
                LSLOT(result) = cp->_long(i);
                result += 2;
                break;
            case JVM_CONSTANT_Double:
                DSLOT(result) = cp->_double(i);
                result += 2;
                break;
            case JVM_CONSTANT_MethodHandle:
                RSLOT(result) = cp->resolveMethodHandle(i);
                result++;
                break;
            case JVM_CONSTANT_MethodType:
                RSLOT(result) = cp->resolveMethodType(i);
                result++;
                break;
            default:
                jvm_abort("never goes here, wrong type."); // todo
        }
    }

    return result;
}

BootstrapMethod::~BootstrapMethod()
{
    //bootstrapArguments.~vector();
}

LineNumberTable::LineNumberTable(BytecodeReader &r)
{
    start_pc = r.readu2();
    line_number = r.readu2();
}

MethodParameter::MethodParameter(ConstantPool &cp, BytecodeReader &r)
{
    u2 name_index = r.readu2();
    // If the value of the name_index item is zero,
    // then this parameters element indicates a formal parameter with no name.
    if (name_index > 0) {
        name = cp.utf8(name_index);
    }
    accessFlags = r.readu2();
}

LocalVariableTable::LocalVariableTable(BytecodeReader &r)
{
    start_pc = r.readu2();
    length = r.readu2();
    name_index = r.readu2();
    descriptor_index = r.readu2();
    index = r.readu2();
}

LocalVariableTypeTable::LocalVariableTypeTable(BytecodeReader &r)
{
    start_pc = r.readu2();
    length = r.readu2();
    name_index = r.readu2();
    signature_index = r.readu2();
    index = r.readu2();
}

Module::Module(ConstantPool &cp, BytecodeReader &r)
{
    moduleName = cp.moduleName(r.readu2());
    moduleFlags = r.readu2();
    u2 v = r.readu2();
    moduleVersion = v == 0 ? nullptr : cp.utf8(v);

    u2 requires_count = r.readu2();
    for (u2 j = 0; j < requires_count; j++) {
        requires.emplace_back(cp, r);
    }

    u2 exports_count = r.readu2();
    for (u2 j = 0; j < exports_count; j++) {
        exports.emplace_back(cp, r);
    }

    u2 opens_count = r.readu2();
    for (u2 j = 0; j < opens_count; j++) {
        opens.emplace_back(cp, r);
    }

    u2 uses_count = r.readu2();
    for (u2 j = 0; j < uses_count; j++) {
        uses.push_back(cp.className(r.readu2()));
    }

    u2 provides_count = r.readu2();
    for (u2 j = 0; j < provides_count; j++) {
        provides.emplace_back(cp, r);
    }
}

Module::Require::Require(ConstantPool &cp, BytecodeReader &r)
{
    requireModuleName = cp.moduleName(r.readu2());
    flags = r.readu2();
    u2 v = r.readu2();
    version = v == 0 ? nullptr : cp.utf8(v);
}

Module::Export::Export(ConstantPool &cp, BytecodeReader &r)
{
    exportPackageName = cp.packageName(r.readu2());
    flags = r.readu2();
    u2 exports_to_count = r.readu2();
    for (u2 i = 0; i < exports_to_count; i++) {
        exports_to.push_back(cp.moduleName(r.readu2()));
    }
};

Module::Open::Open(ConstantPool &cp, BytecodeReader &r)
{
    openPackageName = cp.packageName(r.readu2());
    flags = r.readu2();
    u2 exports_to_count = r.readu2();
    for (u2 i = 0; i < exports_to_count; i++) {
        opens_to.push_back(cp.moduleName(r.readu2()));
    }
};

Module::Provide::Provide(ConstantPool &cp, BytecodeReader &r)
{
    className = cp.className(r.readu2());
    u2 provides_with_count = r.readu2();
    for (u2 i = 0; i < provides_with_count; i++) {
        provides_with.push_back(cp.className(r.readu2()));
    }
}