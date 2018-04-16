/*
 * Author: Jia Yang
 */

#include <string>
#include "Jfield.h"
#include "../../../classfile/attribute/ConstantValueAttr.h"
#include "../objectarea/JstringObj.h"

using namespace std;

Jfield::Jfield(Jclass *jclass, const MemberInfo &memberInfo): constantValueIndex(INVALID_CONSTANT_VALUE_INDEX), type(nullptr) {
    this->jclass = jclass;
    accessFlags = memberInfo.accessFlags;
    name = jclass->rtcp->getStr(memberInfo.nameIndex);
    descriptor = jclass->rtcp->getStr(memberInfo.descriptorIndex);

    if (descriptor[0] == 'B') {
        typeName = "java/lang/Byte";
//        type = jclass->loader->loadClass("java/lang/Byte");
    } else if (descriptor[0] == 'C') {
        typeName = "java/lang/Character";
        //       type = jclass->loader->loadClass("java/lang/Character");
    }  else if (descriptor[0] == 'I') {
        typeName = "java/lang/Integer";
        //    type = jclass->loader->loadClass("java/lang/Integer");
    } else if (descriptor[0] == 'S') {
        typeName = "java/lang/Short";
        //  type = jclass->loader->loadClass("java/lang/Short");
    } else if (descriptor[0] == 'Z') {
        typeName = "java/lang/Boolean";
        //   type = jclass->loader->loadClass("java/lang/Boolean");
    } else if (descriptor[0] == 'F') {
        typeName = "java/lang/Float";
        //    type = jclass->loader->loadClass("java/lang/Float");
    } else if (descriptor[0] == 'J') {
        typeName = "java/lang/Long";
        //    type = jclass->loader->loadClass("java/lang/Long");
    } else if (descriptor[0] == 'D') {
        typeName = "java/lang/Double";
        //   type = jclass->loader->loadClass("java/lang/Double");
    }  else if (descriptor[0] == '[') {
        typeName = descriptor;
        //   type = jclass->loader->loadClass(descriptor);
    } else if (descriptor[0] == 'L' and descriptor[descriptor.size() - 1] == ';') {
        string className = descriptor.substr(1, descriptor.size() - 2);
        typeName = className;
        //    type = jclass->loader->loadClass(className);
    } else {
        jvmAbort("%s\n", descriptor);
    }

    for (int j = 0; j < memberInfo.attributesCount; j++) {
        Attribute *attr = memberInfo.attributes[j];
        string attrName = jclass->rtcp->getStr(attr->attributeNameIndex);

        // todo fields Attributes
        if (attrName == Deprecated) { // 可选属性
            jprintf("not parse attr: Deprecated\n");
        } else if (attrName == ConstantValue) {
            // ConstantValue属性表示一个常量字段的值。在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
            // 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
            if (isStatic()) {  // todo
//                    constant_value_attr *a = attr;
                constantValueIndex = ((ConstantValueAttr *)attr)->constantvalueIndex;
//                int index = ((ConstantValueAttr *)attr)->constantvalueIndex;
//
//                auto v = new Jvalue;
//                switch (descriptor[0]) {
//                    case 'B':
//                    case 'C':
//                    case 'I':
//                    case 'S':
//                    case 'Z':
//                        v->i = jclass->rtcp->getInt(index);
//                        break;
//                    case 'F':
//                        v->f = jclass->rtcp->getFloat(index);
//                        break;
//                    case 'J':
//                        v->l = jclass->rtcp->getLong(index);
//                        break;
//                    case 'D':
//                        v->d = jclass->rtcp->getDouble(index);
//                        break;
//                    default:
//                        if (descriptor == "Ljava/lang/String;") {
//                            // todo
//                            string str = jclass->rtcp->getStr(index);
//                            v->r = new JstringObj(jclass->loader, strToJstr(str));
//                        } else {
//                            jvmAbort("error. ConstantValue: %s\n", descriptor.c_str());
//                        }
//                        break;
//                }
//                extra = v;
            }
        } else if (attrName == Synthetic) {
            setSynthetic();  // todo
        } else if (attrName == Signature) {  // 可选属性
//                SignatureAttr *a = attr;
//                jprintf("not parse attr: Signature\n");
        } else if (attrName == RuntimeVisibleAnnotations) {
//                runtime_annotations_attr *a = attr;
//                jprintf("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (attrName == RuntimeInvisibleAnnotations) {
//                runtime_annotations_attr *a = attr;
//                jprintf("not parse attr: RuntimeInvisibleAnnotations\n");
        }
    }
}

JclassObj* Jfield::getType() {
    if (type != nullptr) {
        return type;
    }
    type = jclass->loader->getJclassObjFromPool(typeName);
    return type;
//    char c = descriptor[0];
//    if (c == 'B')
//        return PRIMITIVE_BYTE;
//    if (c == 'C')
//        return PRIMITIVE_CHAR;
//    if (c == 'D')
//        return PRIMITIVE_DOUBLE;
//    if (c == 'F')
//        return PRIMITIVE_FLOAT;
//    if (c == 'I')
//        return PRIMITIVE_INT;
//    if (c == 'J')
//        return PRIMITIVE_LONG;
//    if (c == 'S')
//        return PRIMITIVE_SHORT;
//    if (c == 'Z')
//        return PRIMITIVE_BOOLEAN;
//    if (c == '[' or c == 'L')
//        return REFERENCE;
//    jvmAbort("error\n");
}