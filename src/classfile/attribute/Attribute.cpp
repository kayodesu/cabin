/*
 * Author: Jia Yang
 */

#include "Attribute.h"
#include "../../util/encoding.h"
#include "CodeAttr.h"
#include "ConstantValueAttr.h"
#include "SignatureAttr.h"
#include "SyntheticAttr.h"
#include "DeprecatedAttr.h"
#include "LineNumberTableAttr.h"
#include "StackMapTableAttr.h"
#include "LocalVariableTableAttr.h"
#include "LocalVariableTypeTableAttr.h"
#include "ExceptionsAttr.h"
#include "RuntimeParameterAnnotationsAttr.h"
#include "AnnotationDefaultAttr.h"
#include "SourceFileAttr.h"
#include "InnerClassesAttr.h"
#include "EnclosingMethodAttr.h"
#include "MethodParametersAttr.h"
#include "SourceDebugExtensionAttr.h"
#include "RuntimeAnnotationsAttr.h"
#include "BootstrapMethodsAttr.h"
#include "UnknownAttr.h"

using namespace std;

Attribute* Attribute::parseAttr(BytecodeReader &reader, Constant **constantPool) {
    u2 attributeNameIndex = reader.readu2();

    Constant *constant = constantPool[attributeNameIndex];
    if (constant == nullptr) {  // constant_pool[0] == NULL
        // todo
        jvmAbort("error\n");
        return nullptr;
    }
    if (constant->tag != UTF8_CONSTANT) {
        //todo
        jvmAbort("error. %d\n", constant->tag);
    }

    Utf8Constant *uc = static_cast<Utf8Constant *>(constant);
    string attrName = decodeMUTF8(uc->bytes, uc->length);

    Attribute *attr = nullptr;

    if (Code == attrName) {
        attr = new CodeAttr(reader, constantPool);
    } else if (ConstantValue == attrName) {
        attr = new ConstantValueAttr(reader);
    } else if (Signature == attrName) {
        attr = new SignatureAttr(reader);
    } else if (Synthetic == attrName) {
        attr = new SyntheticAttr(reader);
    } else if (Deprecated == attrName) {
        attr = new DeprecatedAttr(reader);
    } else if (LineNumberTable == attrName) {
        attr = new LineNumberTableAttr(reader);
    } else if (StackMapTable == attrName) {
        attr = new StackMapTableAttr(reader);
    } else if (LocalVariableTable == attrName) {
        attr = new LocalVariableTableAttr(reader);
    } else if (LocalVariableTypeTable == attrName) {
        attr = new LocalVariableTypeTableAttr(reader);
    } else if (Exceptions == attrName) {
        attr = new ExceptionsAttr(reader);
    } else if (RuntimeVisibleParameterAnnotations == attrName) {
        attr = new RuntimeVisibleParameterAnnotationsAttr(reader);
    } else if (RuntimeInvisibleParameterAnnotations == attrName) {
        attr = new RuntimeInvisibleParameterAnnotationsAttr(reader);
    } else if (AnnotationDefault == attrName) {
        attr = new AnnotationDefaultAttr(reader);
    } else if (SourceFile == attrName) {
        attr = new SourceFileAttr(reader);
    } else if (InnerClasses == attrName) {
        attr = new InnerClassesAttr(reader);
    } else if (EnclosingMethod == attrName) {
        attr = new EnclosingMethodAttr(reader);
    } else if (MethodParameters == attrName) {
        attr = new MethodParametersAttr(reader);
    } else if (SourceDebugExtension == attrName) {
        attr = new SourceDebugExtensionAttr(reader);
    } else if (RuntimeVisibleAnnotations == attrName) {
        attr = new RuntimeVisibleAnnotationsAttr(reader);
    } else if (RuntimeInvisibleAnnotations == attrName) {
        attr = new RuntimeInvisibleAnnotationsAttr(reader);
    } else if (BootstrapMethods == attrName) {
        attr = new BootstrapMethodsAttr(reader);
    }

    if (attr == nullptr) {
        jprintf("unknown attr = %s\n", attrName);
        attr = new UnknownAttr(reader);
    }
    attr->attributeNameIndex = attributeNameIndex;
    return attr;
}