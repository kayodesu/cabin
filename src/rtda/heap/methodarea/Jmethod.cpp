/*
 * Author: Jia Yang
 */

#include <string>

#include "Jmethod.h"
#include "../../../classfile/attribute/CodeAttr.h"
#include "../../../classfile/attribute/LineNumberTableAttr.h"
#include "../../../classfile/attribute/ExceptionsAttr.h"

using namespace std;

void Jmethod::calArgSlotCount() {
    argSlotCount = 0;

    auto b = descriptor.find('(');
    auto e = descriptor.find(')');
    if (b == string::npos or e == string::npos) {
        jvmAbort("error. %s\n", descriptor.c_str());
    }

    while (++b < e) {
        auto c = descriptor[b];
        if (c == 'B' || c == 'C' || c == 'I' || c == 'F' || c == 'S'|| c == 'Z'/*boolean*/) {
            argSlotCount++;
        } else if (c == 'D' || c == 'J'/*long*/) {
            argSlotCount += 2;
        } else if (c == 'L') { // reference
            argSlotCount++;
            b = descriptor.find(';', b);
            if (b == string::npos) {
                jvmAbort("error. %s\n", descriptor.c_str());
            }
        } else if (c == '[') { // array reference
            argSlotCount++;

            do {
                c = descriptor[++b];
            } while(c == '[');

            if (c == 'L') {
                b = descriptor.find(';', b);
                if (b == string::npos) {
                    jvmAbort("error. %s\n", descriptor.c_str());
                }
            }
        }
    }

    if (!isStatic()) { // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
        argSlotCount++; // this reference
    }
}

Jmethod::Jmethod(Jclass *jclass, const MemberInfo &methodInfo): exceptionTables(nullptr), lineNumberTables(nullptr) {
    this->jclass = jclass;
    accessFlags = methodInfo.accessFlags;
    name = jclass->rtcp->getStr(methodInfo.nameIndex);
    descriptor = jclass->rtcp->getStr(methodInfo.descriptorIndex);

    calArgSlotCount();

    for (int j = 0; j < methodInfo.attributesCount; j++) {
        Attribute *attr = methodInfo.attributes[j];
        string attrName = jclass->rtcp->getStr(attr->attributeNameIndex);

        // todo methods Attributes
        if (attrName == Code) {
            CodeAttr *a = static_cast<CodeAttr *>(attr);
            maxStack = a->maxStack;
            maxLocals = a->maxLocals;

            code = reinterpret_cast<s1 *>(a->code);
            codeLength = a->codeLength;

//            reader = new BytecodeReader(reinterpret_cast<const s1 *>(a->code), a->codeLength);

            exceptionTableCount = a->exceptionTableLength;
            // todo exceptionTableCount == 0
            exceptionTables = new ExceptionTable[exceptionTableCount];
            for (int i = 0; i < exceptionTableCount; i++) {
                exceptionTables[i].startPc = a->exceptionTables[i].startPc;
                exceptionTables[i].endPc = a->exceptionTables[i].endPc;
                exceptionTables[i].handlerPc = a->exceptionTables[i].handlerPc;
                if (a->exceptionTables[i].catchType == 0) {
                    exceptionTables[i].catchType = nullptr; // todo  表示 finally 字句？？？
                } else {
                    const string &className = jclass->rtcp->getClassName(a->exceptionTables[i].catchType);
                    exceptionTables[i].catchType = jclass->loader->loadClass(className);
                }
            }

            for (int k = 0; k < a->attributesCount; k++) {
                attr = a->attributes[j];
                attrName = jclass->rtcp->getStr(attr->attributeNameIndex);
                if (attrName == StackMapTable) {
                    //             stack_map_table_attr *a = attr;
//                        jvm_printf("not parse attr: StackMapTable\n");
                } else if (attrName == LineNumberTable) {   // 可选属性
                    LineNumberTableAttr *tmp = static_cast<LineNumberTableAttr *>(attr);
                    lineNumberTableCount = tmp->attributeLength;
                    lineNumberTables = new pair<u2, u2>[lineNumberTableCount];
                    for (int i = 0; i < lineNumberTableCount; i++) {
                        lineNumberTables[i].first = tmp->lineNumberTables[i].startPc;
                        lineNumberTables[i].second = tmp->lineNumberTables[i].lineNumber;
                    }
                } else if (attrName == LocalVariableTable) {   // 可选属性
//                        jvm_printf("not parse attr: LocalVariableTable\n");
                } else if (attrName == LocalVariableTypeTable) {   // 可选属性
//                        jvm_printf("not parse attr: LocalVariableTypeTable\n");
                }
            }
        } else if (attrName == Synthetic) {
            setSynthetic();   // todo
        } else if (attrName == Signature) {  // 可选属性
//                jvm_printf("not parse attr: Signature\n");
        }  else if (attrName == Exceptions) {
            //            exceptions_attr *a = attr;
//                jvm_printf("not parse attr: Exceptions\n");
            ExceptionsAttr *a = static_cast<ExceptionsAttr *>(attr);
        } else if (attrName == RuntimeVisibleParameterAnnotations) {
            //           runtime_parameter_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeVisibleParameterAnnotations\n");
        } else if (attrName == RuntimeInvisibleParameterAnnotations) {
            //           runtime_parameter_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeInvisibleParameterAnnotations\n");
        } else if (attrName == Deprecated) {  // 可选属性
//                jvm_printf("not parse attr: Deprecated\n");
        } else if (attrName == RuntimeVisibleAnnotations) {
            //          runtime_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (attrName == RuntimeInvisibleAnnotations) {
            //         runtime_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeInvisibleAnnotations\n");
        } else if (attrName == AnnotationDefault) {
            //        annotation_default_attr *a = attr;
//                jvm_printf("not parse attr: AnnotationDefault\n");
        }
    }

    if (isNative()) {
        maxStack = 4;  // todo 本地方法帧的操作数栈至少要能容纳返回值， 为了简化代码，暂时给maxStack字段赋值为4
        maxLocals = getArgSlotCount(); // todo 因为本地方法帧的局部变量表只用来存放参数值，所以把argSlotCount赋给maxLocals字段刚好。

        size_t codeLen = 2;
        s1 *code = new s1[codeLen];//malloc(sizeof(s1) * code_len);
        code[0] = (s1) 0xfe;
        size_t t = descriptor.find(')'); // find return
        if (t == string::npos) {
            //todo error
            jprintf("error\n");
        }
        char c = descriptor[++t];
        if (c == 'V') {
            code[1] = (s1) 0xb1; // return
        } else if (c == 'D') {
            code[1] = (s1) 0xaf; // dreturn
        } else if (c == 'F') {
            code[1] = (s1) 0xae; // freturn
        } else if (c == 'J') {
            code[1] = (s1) 0xad; // lreturn
        } else if (c == 'L' || c == '[') {
            code[1] = (s1) 0xb0; // areturn
        } else {
            code[1] = (s1) 0xac; // ireturn
        }
        this->code = code;
        this->codeLength = codeLen;
//        reader = new BytecodeReader(code, codeLen);
    }

}

int Jmethod::findExceptionHandler(Jclass *exceptionType, size_t pc) const {  // todo pc 是什么？
    for (int i = 0; i < exceptionTableCount; i++) {
        const auto &t = exceptionTables[i];
        if (t.startPc <= pc and pc < t.endPc) {
            if (t.catchType == nullptr) {
                return t.handlerPc;  // catch all  todo
            }
            if (exceptionType->isSubclassOf(t.catchType)) {
                return t.handlerPc;
            }
        }
    }

    return -1;
}
