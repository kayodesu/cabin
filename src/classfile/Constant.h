/*
 * Author: Jia Yang
 */

#ifndef JVM_CONSTANT_H
#define JVM_CONSTANT_H

#include "../jvmdef.h"
#include "../BytecodeReader.h"

#define CLASS_CONSTANT					7
#define FIELD_REF_CONSTANT				9
#define METHOD_REF_CONSTANT				10
#define INTERFACE_METHOD_REF_CONSTANT	11
#define STRING_CONSTANT					8
#define INTEGER_CONSTANT				3
#define FLOAT_CONSTANT					4
#define LONG_CONSTANT					5
#define DOUBLE_CONSTANT					6
#define NAME_AND_TYPE_CONSTANT			12
#define UTF8_CONSTANT					1
#define METHOD_HANDLE_CONSTANT			15
#define METHOD_TYPE_CONSTANT			16
#define INVOKE_DYNAMIC_CONSTANT			18

struct Constant {
    u1 tag;

//    Constant(BytecodeReader &reader) {
//        tag = reader.readu1();
//    }
    static Constant* parseConstant(BytecodeReader &reader);
};

struct ClassConstant: Constant {
    u2 nameIndex;

    explicit ClassConstant(BytecodeReader &reader) {
        nameIndex = reader.readu2();
    }
} ;

struct MemberRefConstant: Constant {
    u2 classIndex;
    u2 nameAndTypeIndex;

    explicit MemberRefConstant(BytecodeReader &reader) {
        classIndex = reader.readu2();
        nameAndTypeIndex = reader.readu2();
    }
} ;//, field_ref_constant, method_ref_constant, interface_method_ref_constant;

struct StringConstant: Constant {
    u2 stringIndex;

    explicit StringConstant(BytecodeReader &reader) {
        stringIndex = reader.readu2();
    }
} ;

struct FourBytesNumConstant: Constant {
    u1 bytes[4];

    explicit FourBytesNumConstant(BytecodeReader &reader) {
        reader.readbytes(bytes, 4);
    }
} ;//, integer_constant, float_constant;

typedef FourBytesNumConstant IntegerConstant;
typedef FourBytesNumConstant FloatConstant;

struct EightBytesNumConstant: Constant {
    u1 bytes[8];
    explicit EightBytesNumConstant(BytecodeReader &reader) {
        reader.readbytes(bytes, 8);
    }
}; //, long_constant, double_constant;

typedef EightBytesNumConstant LongConstant;
typedef EightBytesNumConstant DoubleConstant;

struct NameAndTypeConstant: Constant {
    u2 nameIndex;
    u2 descriptorIndex;

    explicit NameAndTypeConstant(BytecodeReader &reader) {
        nameIndex = reader.readu2();
        descriptorIndex = reader.readu2();
    }
} ;

struct Utf8Constant: Constant {
    u2 length;
    u1 *bytes; // [length];

    explicit Utf8Constant(BytecodeReader &reader) {
        length = reader.readu2();
        bytes = new u1[length];
        reader.readbytes(bytes, length);
    }

    ~Utf8Constant() {
        delete[] bytes;
    }
};

struct MethodHandleConstant: Constant {
    u1 referenceKind;
    u2 referenceIndex;

    explicit MethodHandleConstant(BytecodeReader &reader) {
        referenceKind = reader.readu1();
        referenceIndex = reader.readu2();
    }
} ;

struct MethodTypeConstant: Constant {
    u2 descriptorIndex;

    explicit MethodTypeConstant(BytecodeReader &reader) {
        descriptorIndex = reader.readu2();
    }
} ;

struct InvokeDynamicConstant: Constant {
    u2 bootstrapMethodAttrIndex;
    u2 nameAndTypeIndex;

    explicit InvokeDynamicConstant(BytecodeReader &reader) {
        bootstrapMethodAttrIndex = reader.readu2();
        nameAndTypeIndex = reader.readu2();
    }
} ;


//#define CONSTANT_TAG(constant_point) *(u1 *)constant_point

//void* parse_constant(bytecode_reader *reader);

#endif //JVM_CONSTANT_H
