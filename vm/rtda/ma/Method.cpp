/*
 * Author: kayo
 */

#include <sstream>
#include "Method.h"
#include "../heap/Object.h"
#include "../heap/ArrayObject.h"
#include "../heap/ClassObject.h"
#include "../../symbol.h"
#include "../../classfile/constant.h"
#include "../../interpreter/interpreter.h"

using namespace std;

Method::ExceptionTable::ExceptionTable(Class *clazz, BytecodeReader &r)
{
    startPc = r.readu2();
    endPc = r.readu2();
    handlerPc = r.readu2();
    u2 type = r.readu2();
    if (type == 0) {
        // 异常处理项的 catch_type 有可能是 0。
        // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
        catchType = nullptr;
    } else {
        catchType = new CatchType;
        if (CP_TYPE(clazz->cp, type) == CONSTANT_ResolvedClass) {
            catchType->resolved = true;
            catchType->u.clazz = (Class *) CP_INFO(clazz->cp, type);
        } else {
            // 不能在这里load class，有形成死循环的可能。
            // 比如当前方法是 Throwable 中的方法，而此方法又抛出了 Throwable 子类的Exception（记为A），
            // 而此时 Throwable 还没有构造完成，所以无法构造其子类 A。
            catchType->resolved = false;
            catchType->u.className = CP_CLASS_NAME(clazz->cp, type);
        }
    }
}

ArrayObject *Method::getParameterTypes()
{
    if (parameterTypes == nullptr) {
        int dlen = strlen(descriptor);
        char desc[dlen + 1];
        strcpy(desc, descriptor);

        Object* buf[METHOD_PARAMETERS_MAX_COUNT];

        int parameter_types_count = 0;

        char *b = strchr(desc, '(');
        const char *e = strchr(desc, ')');
        if (b == nullptr || e == nullptr) {
            string s = "descriptor error. ";
            s.append(desc).c_str();
            raiseException(UNKNOWN_ERROR, s.append(desc).c_str());
        }

        // parameter types
        while (++b < e) {
            if (*b == 'L') { // reference
                char *t = strchr(b, ';');
                if (t == nullptr) {
                    string s = "descriptor error. ";
                    s.append(desc).c_str();
                    raiseException(UNKNOWN_ERROR, s.append(desc).c_str());
                }

                *t = 0;   // end string
                buf[parameter_types_count++] = clazz->loader->loadClass(b + 1 /* jump 'L' */)->clsobj;
                *t = ';'; // recover
                b = t;
            } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
                char *t = b;
                while (*(++t) == '[');
                if (!isPrimitiveDescriptor(*t)) {
                    t = strchr(t, ';');
                    if (t == nullptr) {
                        string s = "descriptor error. ";
                        s.append(desc).c_str();
                        raiseException(UNKNOWN_ERROR, s.append(desc).c_str());
                    }
                }

                char k = *(++t);
                *t = 0; // end string
                buf[parameter_types_count++] = clazz->loader->loadClass(b)->clsobj;
                *t = k; // recover
                b = t;
            } else if (isPrimitiveDescriptor(*b)) {
                const char *class_name = primitiveDescriptor2className(*b);
                buf[parameter_types_count++] = clazz->loader->loadClass(class_name)->clsobj;
            } else {
                string s = "descriptor error. ";
                s.append(desc).c_str();
                raiseException(UNKNOWN_ERROR, s.append(desc).c_str());
            }
        }

        // todo parameter_types_count == 0 是不是要填一个 void.class

        parameterTypes = ArrayObject::newInst(java_lang_Class_array_class, parameter_types_count);
        for (int i = 0; i < parameter_types_count; i++)
            parameterTypes->set(i, buf[i]);
    }

    return parameterTypes;
}

ClassObject *Method::getReturnType()
{
    if (returnType == nullptr) {
        const char *e = strchr(descriptor, ')');
        if (e == nullptr) {
            string s = "descriptor error. ";
            s.append(descriptor).c_str();
            raiseException(UNKNOWN_ERROR, s.append(descriptor).c_str());
        }
        returnType = clazz->loader->loadClass(descriptorToClassName(++e).c_str())->clsobj;
    }
    return returnType;
}

ArrayObject *Method::getExceptionTypes()
{
    if (exceptionTypes == nullptr) {
        int count = 0;
        Object *types[exceptionTables.size()];
        for (auto t : exceptionTables) {
            if (t.catchType == nullptr)
                continue;

            if (!t.catchType->resolved) {
                t.catchType->u.clazz = clazz->loader->loadClass((t.catchType->u.className));
                t.catchType->resolved = true;
            }
            types[count++] = t.catchType->u.clazz->clsobj;
        }

        auto ac = (ArrayClass *)(clazz->loader->loadClass(S(array_java_lang_Class)));
        exceptionTypes = ArrayObject::newInst(ac, count);
        for (int i = 0; i < count; i++)
            exceptionTypes->set(i, types[i]);
    }

    assert(exceptionTypes != nullptr);
    return exceptionTypes;
}

void Method::calArgsSlotsCount()
{
    arg_slot_count = 0;

    const char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == nullptr || e == nullptr) {
        jvm_abort("error. %s\n", descriptor);
    }

    while (++b < e) {
        if (*b == 'B' || *b == 'C' || *b == 'I' || *b == 'F' || *b == 'S'|| *b == 'Z'/* boolean */) {
            arg_slot_count++;
        } else if (*b == 'D' || *b == 'J'/* long */) {
            arg_slot_count += 2;
        } else if (*b == 'L') { // reference
            arg_slot_count++;
            b = strchr(b, ';');
            if (b == nullptr) {
                jvm_abort("error. %s\n", descriptor);
            }
        } else if (*b == '[') { // array reference
            arg_slot_count++;
            while (*(++b) == '[');

            if (*b == 'L') {
                b = strchr(b, ';');
                if (b == nullptr) {
                    jvm_abort("error. %s\n", descriptor);
                }
            }
        }
    }

    if (!isStatic()) { // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
        arg_slot_count++; // this reference
    }
}

/*
 * 解析方法的 code 属性
 */
void Method::parseCodeAttr(BytecodeReader &r)
{
    maxStack = r.readu2();
    maxLocals = r.readu2();
    codeLen = r.readu4();
    code = r.currPos();
    r.skip(codeLen);

    // parse exception tables
    int exception_tables_count = r.readu2();
    for (int i = 0; i < exception_tables_count; i++) {
        exceptionTables.emplace_back(ExceptionTable(clazz, r));
    }

    // parse attributes of code's attribute
    u2 attr_count = r.readu2();
    for (int k = 0; k < attr_count; k++) {
        const char *attr_name = CP_UTF8(clazz->cp, r.readu2());
        u4 attr_len = r.readu4();

        if (S(LineNumberTable) == attr_name) {
            int count = r.readu2();
            for (int i = 0; i < count; i++) {
                lineNumberTables.emplace_back(LineNumberTable(r));
            }
        } else if (S(StackMapTable) == attr_name) { // ignore
            r.skip(attr_len);
        } else if (S(LocalVariableTable) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct local_variable_table tables[num];
//            for (int i = 0; i < num; i++) {
//                tables[i].start_pc = readu2(reader);
//                tables[i].length = readu2(reader);
//                tables[i].name_index = readu2(reader);
//                tables[i].descriptor_index = readu2(reader);
//                tables[i].index = readu2(reader);
//            }
            r.skip(attr_len);
        } else if (S(LocalVariableTypeTable) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct local_variable_type_table tables[num];
//            for (int i = 0; i < num; i++) {
//                tables[i].start_pc = readu2(reader);
//                tables[i].length = readu2(reader);
//                tables[i].name_index = readu2(reader);
//                tables[i].signature_index = readu2(reader);
//                tables[i].index = readu2(reader);
//            }
            r.skip(attr_len);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

Method::Method(Class *c, BytecodeReader &r): Member(c)
{
    ConstantPool &cp = c->cp;

    accessFlags = r.readu2();
    name = CP_UTF8(cp, r.readu2());
    descriptor = CP_UTF8(cp, r.readu2());
    u2 attr_count = r.readu2();

    calArgsSlotsCount();

    // parse method's attributes
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = CP_UTF8(cp, r.readu2());
        u4 attr_len = r.readu4();

        if (S(Code) == attr_name) {
            parseCodeAttr(r);
        } else if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            signature = CP_UTF8(cp, r.readu2());
        } else if (S(MethodParameters) == attr_name) { // ignore
//            u1 num = bcr_readu1(reader); // 这里就是 u1，不是u2
//            struct parameter parameters[num];
//            for (u2 k = 0; k < num; k++) {
//                parameters[k].name_index = readu2(reader);
//                parameters[k].access_flags = readu2(reader);
//            }
            r.skip(attr_len);
        } else if (S(Exceptions) == attr_name) { // ignore
//            method->exception_tables_count = readu2(reader);
//            u2 exception_index_table[num];
//            for (u2 k = 0; k < num; k++) {
//                exception_index_table[i] = readu2(reader);
//            }
            r.skip(attr_len);
        } else if (S(RuntimeVisibleParameterAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_visible_parameter_annotations_num = readu2(reader);
//            struct parameter_annotation *as
//                    = method->runtime_visible_parameter_annotations = malloc(sizeof(struct parameter_annotation) * num);
//            CHECK_MALLOC_RESULT(as);
//            for (u2 k = 0; k < num; k++) {
//                u2 nums = as[i].num_annotations = readu2(reader);
//                as[i].annotations = malloc(sizeof(struct annotation) * nums);
//                CHECK_MALLOC_RESULT(as[i].annotations);
//                for (int j = 0; j < nums; j++) {
//                    read_annotation(reader, as[i].annotations + j);
//                }
//            }
            r.skip(attr_len);
        } else if (S(RuntimeInvisibleParameterAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_invisible_parameter_annotations_num = readu2(reader);
//            struct parameter_annotation *as
//                    = method->runtime_invisible_parameter_annotations = malloc(sizeof(struct parameter_annotation) * num);
//            CHECK_MALLOC_RESULT(as);
//            for (u2 k = 0; k < num; k++) {
//                u2 nums = as[i].num_annotations = readu2(reader);
//                as[i].annotations = malloc(sizeof(struct annotation) * nums);
//                CHECK_MALLOC_RESULT(as[i].annotations);
//                for (int j = 0; j < nums; j++) {
//                    read_annotation(reader, as[i].annotations + j);
//                }
//            }
            r.skip(attr_len);
        } else if (S(RuntimeVisibleAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_visible_annotations_num = readu2(reader);
//            method->runtime_visible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(method->runtime_visible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, method->runtime_visible_annotations + i);
//            }
            r.skip(attr_len);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_invisible_annotations_num = readu2(reader);
//            method->runtime_invisible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(method->runtime_invisible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, method->runtime_invisible_annotations + i);
//            }
            r.skip(attr_len);
        } else if (S(AnnotationDefault) == attr_name) { // ignore
//            struct element_value ev;
//            read_element_value(reader, &ev);
            r.skip(attr_len);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }

    if (isNative()) {
        maxStack = 4;  // todo 本地方法帧的操作数栈至少要能容纳返回值， 为了简化代码，暂时给maxStack字段赋值为4
        maxLocals = arg_slot_count; // todo 因为本地方法帧的局部变量表只用来存放参数值，所以把argSlotCount赋给maxLocals字段刚好。

        codeLen = 2;
        auto code = new u1[codeLen];
        code[0] = OPC_INVOKENATIVE;
        const char *t = strchr(descriptor, ')'); // find return
        if (t == nullptr) {
            //todo error
            printvm("method's descriptor 格式不对：%s\n", descriptor);
        }

        ++t;
        if (*t == 'V') {
            code[1] = 0xb1; // return
        } else if (*t == 'D') {
            code[1] = 0xaf; // dreturn
        } else if (*t == 'F') {
            code[1] = 0xae; // freturn
        } else if (*t == 'J') {
            code[1] = 0xad; // lreturn
        } else if (*t == 'L' || *t == '[') {
            code[1] = 0xb0; // areturn
        } else {
            code[1] = 0xac; // ireturn
        }

        this->code = code;
        nativeMethod = findNativeMethod(clazz->className, name, descriptor);
    }
}

int Method::getLineNumber(int pc) const
{
    // native函数没有字节码
    if (isNative()) {
        return -2;
    }

    /*
     * 和源文件名一样，并不是每个方法都有行号表。
     * 如果方法没有行号表，自然也就查不到pc对应的行号，这种情况下返回–1
     todo
     */
    // 从后往前查
    for (auto iter = lineNumberTables.rbegin(); iter != lineNumberTables.rend(); iter++) {
        if (pc >= iter->start_pc)
            return iter->line_number;
    }
    return -1;
}

int Method::findExceptionHandler(Class *exceptionType, size_t pc)
{
    for (auto t : exceptionTables) {
        if (t.startPc <= pc && pc < t.endPc) {
            if (t.catchType == nullptr)  // catch all
                return t.handlerPc;
            if (!t.catchType->resolved) {
                t.catchType->u.clazz = clazz->loader->loadClass(t.catchType->u.className);
                t.catchType->resolved = true;
            }
            if (exceptionType->isSubclassOf(t.catchType->u.clazz))
                return t.handlerPc;
        }
    }

    return -1;
}

string Method::toString() const
{
    ostringstream oss;
    oss << "method";
    if (isNative())
        oss << "(native)";
    oss << ": "  << clazz->className << "~" << name << "~" << descriptor;
    return oss.str();
}