/*
 * Author: kayo
 */

#include "method.h"
#include "class.h"
#include "invoke.h"

using namespace std;

Method::ExceptionTable::ExceptionTable(Class *clazz, BytecodeReader &r)
{
    startPc = r.readu2();
    endPc = r.readu2();
    handlerPc = r.readu2();
    u2 index = r.readu2();
    if (index == 0) {
        // 异常处理项的 catch_type 有可能是 0。
        // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
        catchType = nullptr;
    } else {
        catchType = new CatchType;
        if (clazz->cp.type(index) == JVM_CONSTANT_ResolvedClass) {
            catchType->resolved = true;
            catchType->u.clazz = clazz->cp.resolveClass(index); // (Class *) CP_INFO(clazz->cp, index);
        } else {
            // Note:
            // 不能在这里load class，有形成死循环的可能。
            // 比如当前方法是 Throwable 中的方法，而此方法又抛出了 Throwable 子类的Exception（记为A），
            // 而此时 Throwable 还没有构造完成，所以无法构造其子类 A。
            catchType->resolved = false;
            catchType->u.className = clazz->cp.className(index); //CP_CLASS_NAME(clazz->cp, index);
        }
    }
}

Array *Method::getParameterTypes()
{
    Object *type = getType();

    // private final Class<?>[] ptypes;
    auto ptypes = type->getInstFieldValue<jarrref>("ptypes", "[Ljava/lang/Class;");
    assert(ptypes != nullptr);
    return ptypes;

//    if (parameterTypes == nullptr) {
//        int dlen = strlen(descriptor);
//        char desc[dlen + 1];
//        strcpy(desc, descriptor);
//
//        Object* buf[METHOD_PARAMETERS_MAX_COUNT];
//
//        int parameter_types_count = 0;
//
//        char *b = strchr(desc, '(');
//        const char *e = strchr(desc, ')');
//        if (b == nullptr || e == nullptr) {
//            thread_throw(new UnknownError(NEW_MSG("descriptor error. %s\n", desc)));
//        }
//
//        // parameter types
//        while (++b < e) {
//            if (*b == 'L') { // reference
//                char *t = strchr(b, ';');
//                if (t == nullptr) {
//                    thread_throw(new UnknownError(NEW_MSG("descriptor error. %s\n", desc)));
//                }
//
//                *t = 0;   // end string
//                buf[parameter_types_count++] = loadClass(clazz->loader, b + 1 /* jump 'L' */);
//                *t = ';'; // recover
//                b = t;
//            } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
//                char *t = b;
//                while (*(++t) == '[');
//                if (!isPrimDescriptor(*t)) {
//                    t = strchr(t, ';');
//                    if (t == nullptr) {
//                        thread_throw(new UnknownError(NEW_MSG("descriptor error. %s\n", desc)));
//                    }
//                }
//
//                char k = *(++t);
//                *t = 0; // end string
//                buf[parameter_types_count++] = loadClass(clazz->loader, b);
//                *t = k; // recover
//                b = t;
//            } else if (isPrimDescriptor(*b)) {
//                const char *class_name = primDescriptor2className(*b);
//                buf[parameter_types_count++] = loadClass(clazz->loader, class_name);
//            } else {
//                thread_throw(new UnknownError(NEW_MSG("descriptor error. %s\n", desc)));
//            }
//        }
//
//        // todo parameter_types_count == 0 是不是要填一个 void.class
//
//        parameterTypes = newArray(loadBootClass(S(array_java_lang_Class)), parameter_types_count);
//        for (int i = 0; i < parameter_types_count; i++)
//            parameterTypes->set(i, buf[i]);
//    }
//
//    assert(parameterTypes != nullptr);
//    return parameterTypes;
}

Class *Method::getReturnType()
{
    Object *type = getType();

    // private final Class<?> rtype;
    auto rtype = type->getInstFieldValue<Class *>("rtype", "Ljava/lang/Class;");
    assert(rtype != nullptr);
    return rtype;
//    if (returnType == nullptr) {
//        const char *e = strchr(descriptor, ')');
//        if (e == nullptr) {
//            thread_throw(new UnknownError(NEW_MSG("descriptor error. %s\n", descriptor)));
//        }
//        returnType = loadClass(clazz->loader, descriptorToClassName(++e).c_str());
//    }
//    return returnType;
}

Object *Method::getType()
{
    if (type == nullptr) {
        // public static MethodType fromMethodDescriptorString(String descriptor, ClassLoader loader)
        //                      throws IllegalArgumentException, TypeNotPresentException
//        Method *m = loadBootClass("java/lang/invoke/MethodType")->getDeclaredStaticMethod(
//                "fromMethodDescriptorString", "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/invoke/MethodType;");
//        slot_t *r = execJavaFunc(m, { (slot_t) newString(descriptor), (slot_t) clazz->loader });
//        assert(r != nullptr);
//        type = (Object *) *r;
        type = method_type::fromMethodDescriptor(descriptor, clazz->loader);
    }
    return type;
}

Array *Method::getExceptionTypes()
{
    if (exceptionTypes == nullptr) {
        int count = 0;
        Class *types[exceptionTables.size()];
        for (auto t : exceptionTables) {
            if (t.catchType == nullptr)
                continue;

            if (!t.catchType->resolved) {
                t.catchType->u.clazz = loadClass(clazz->loader, t.catchType->u.className);
                t.catchType->resolved = true;
            }
            types[count++] = t.catchType->u.clazz;
        }

        auto ac = loadClass(clazz->loader, S(array_java_lang_Class));
        exceptionTypes = newArray(ac, count);
        for (int i = 0; i < count; i++)
            exceptionTypes->set(i, types[i]);
    }

    assert(exceptionTypes != nullptr);
    return exceptionTypes;
}

u2 Method::calArgsSlotsCount(const utf8_t *descriptor, bool isStatic)
{
    assert(descriptor != nullptr);
    u2 count = 0;

    const char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == nullptr || e == nullptr) {
        jvm_abort("error. %s\n", descriptor);
    }

    while (++b < e) {
        if (*b == 'B' || *b == 'C' || *b == 'I' || *b == 'F' || *b == 'S'|| *b == 'Z'/* boolean */) {
            count++;
        } else if (*b == 'D' || *b == 'J'/* long */) {
            count += 2;
        } else if (*b == 'L') { // reference
            count++;
            b = strchr(b, ';');
            if (b == nullptr) {
                jvm_abort("error. %s\n", descriptor);
            }
        } else if (*b == '[') { // array reference
            count++;
            while (*(++b) == '[');

            if (*b == 'L') {
                b = strchr(b, ';');
                if (b == nullptr) {
                    jvm_abort("error. %s\n", descriptor);
                }
            }
        }
    }

    if (!isStatic) { // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
        count++; // this reference
    }
    return count;
}

void Method::calArgsSlotsCount()
{
    // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
    arg_slot_count = calArgsSlotsCount(descriptor, isStatic());
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
        exceptionTables.emplace_back(clazz, r);
    }

    // parse attributes of code's attribute
    u2 attr_count = r.readu2();
    for (int k = 0; k < attr_count; k++) {
        const char *attr_name = clazz->cp.utf8(r.readu2());
        u4 attr_len = r.readu4();

        if (S(LineNumberTable) == attr_name) {
            u2 num = r.readu2();
            for (int i = 0; i < num; i++)
                line_number_tables.emplace_back(r);
        } else if (S(StackMapTable) == attr_name) {
            r.skip(attr_len); // todo ...........................................................
        } else if (S(LocalVariableTable) == attr_name) {
            u2 num = r.readu2();
            for (int i = 0; i < num; i++)
                local_variable_tables.emplace_back(r);
        } else if (S(LocalVariableTypeTable) == attr_name) {
            u2 num = r.readu2();
            for (int i = 0; i < num; i++)
                local_variable_type_tables.emplace_back(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

Method::Method(Class *c, BytecodeReader &r)
{
    assert(c != nullptr);
    clazz = c;
    ConstantPool &cp = c->cp;

    modifiers = r.readu2();
    name = cp.utf8(r.readu2());
    descriptor = cp.utf8(r.readu2());
    u2 attr_count = r.readu2();

    calArgsSlotsCount();

    // parse method's attributes
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp.utf8(r.readu2());
        u4 attr_len = r.readu4();

        if (S(Code) == attr_name) {
            parseCodeAttr(r);
        } else if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            signature = cp.utf8(r.readu2());
        } else if (S(MethodParameters) == attr_name) {
            u1 num = r.readu1(); // 这里就是 u1，不是u2
            for (u2 k = 0; k < num; k++)
                parameters.emplace_back(cp, r);
        } else if (S(Exceptions) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++)
                checkedExceptions.push_back(r.readu2());
        } else if (S(RuntimeVisibleParameterAnnotations) == attr_name) {
            u2 num = r.readu2();
            rtVisiParaAnnos.resize(num);
            for (u2 j = 0; j < num; j++) {
                u2 numAnnos = r.readu2();
                for (u2 k = 0; k < numAnnos; k++)
                    rtVisiParaAnnos[j].emplace_back(r);
            }
        } else if (S(RuntimeInvisibleParameterAnnotations) == attr_name) {
            u2 num = r.readu2();
            rtInvisiParaAnnos.resize(num);
            for (u2 j = 0; j < num; j++) {
                u2 numAnnos = r.readu2();
                for (u2 k = 0; k < numAnnos; k++)
                    rtInvisiParaAnnos[j].emplace_back(r);
            }
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++)
                rtVisiAnnos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++)
                rtInvisiAnnos.emplace_back(r);
        } else if (S(AnnotationDefault) == attr_name) {
            annotationDefault.read(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }

    if (isNative()) {
        // 本地方法帧的操作数栈至少要能容纳返回值，
        // 4 slots are big enough.
        maxStack = 4;
        // 因为本地方法帧的局部变量表只用来存放参数值，
        // 所以把argSlotCount赋给maxLocals字段刚好。
        maxLocals = arg_slot_count;

        codeLen = 2;
        code = new u1[codeLen];
        code[0] = JVM_OPC_invokenative;
        const char *t = strchr(descriptor, ')'); // find return
        assert(t != nullptr);

        ++t;
        if (*t == 'V') {
            code[1] = JVM_OPC_return;
        } else if (*t == 'D') {
            code[1] = JVM_OPC_dreturn;
        } else if (*t == 'F') {
            code[1] = JVM_OPC_freturn;
        } else if (*t == 'J') {
            code[1] = JVM_OPC_lreturn;
        } else if (*t == 'L' || *t == '[') {
            code[1] = JVM_OPC_areturn;
        } else {
            code[1] = JVM_OPC_ireturn;
        }

        nativeMethod = findNative(clazz->className, name, descriptor);
    }
}

jint Method::getLineNumber(int pc) const
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
    for (auto iter = line_number_tables.rbegin(); iter != line_number_tables.rend(); iter++) {
        if (pc >= iter->start_pc)
            return iter->line_number;
    }
    return -1;
}

int Method::findExceptionHandler(Class *exceptionType, size_t pc)
{
    for (auto t : exceptionTables) {
        // jvms: The start pc is inclusive and end pc is exclusive
        if (t.startPc <= pc && pc < t.endPc) {
            if (t.catchType == nullptr)  // catch all
                return t.handlerPc;
            if (!t.catchType->resolved) {
                t.catchType->u.clazz = loadClass(clazz->loader, t.catchType->u.className);
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
