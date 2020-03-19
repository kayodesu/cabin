/*
 * Author: kayo
 */

#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <pthread.h>
#include "../runtime/thread_info.h"
#include "class.h"
#include "../interpreter/interpreter.h"
#include "prims.h"
#include "invoke.h"
#include "../native/registry.h"
#include "../classfile/constants.h"

using namespace std;
using namespace utf8;
using namespace method_type;
using namespace method_handles;


Field::Field(Class *c, BytecodeReader &r)
{
    assert(c != nullptr);
    clazz = c;
    ConstantPool &cp = c->cp;

    modifiers = r.readu2();
    name = cp.utf8(r.readu2());
    descriptor = cp.utf8(r.readu2());

    category_two = descriptor[0] == 'J' || descriptor[0]== 'D';

    // parse field's attributes
    u2 attr_count = r.readu2();
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp.utf8(r.readu2());
        u4 attr_len = r.readu4();

        if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(ConstantValue) == attr_name) {
            /*
             * ConstantValue属性表示一个常量字段的值。
             * 在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
             *
             * 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
             */
            u2 index = r.readu2();
            if (isStatic()) {
                utf8_t d = *descriptor;
                if (d == 'Z') {
                    staticValue.z = jint2jbool(cp._int(index));
                } else if (d == 'B') {
                    staticValue.b = jint2jbyte(cp._int(index));
                } else if (d == 'C') {
                    staticValue.c = jint2jchar(cp._int(index));
                } else if (d == 'S') {
                    staticValue.s = jint2jshort(cp._int(index));
                } else if (d == 'I') {
                    staticValue.i = cp._int(index);
                } else if (d == 'J') {
                    staticValue.j = cp._long(index);
                } else if (d == 'F') {
                    staticValue.f = cp._float(index);
                } else if (d == 'D') {
                    staticValue.d = cp._double(index);
                } else if(equals(descriptor, S(sig_java_lang_String))) {
                    staticValue.r = cp.resolveString(index);
                }
            }
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            c->signature = cp.utf8(r.readu2());
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rtVisiAnnos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rtInvisiAnnos.emplace_back(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

Class *Field::getType()
{
    if (type == nullptr) {
        if (*descriptor == '[') { // array
            type = loadClass(clazz->loader, descriptor);
        } else if (*descriptor == 'L') { // non array Object
            utf8_t buf[strlen(descriptor)];
            buf[strlen(strcpy(buf, descriptor + 1)) - 1] = 0; // don't include the first 'L' and the last ';'
            type = loadClass(clazz->loader, buf);
        } else { // primitive
            assert(strlen(descriptor) == 1);
            auto name = getPrimClassName(*descriptor);
            assert(name != nullptr);
            type = loadBootClass(name);
        }
    }

    return type;
}

bool Field::isPrim() const
{
    return getPrimClassName(*descriptor) != nullptr;
}

string Field::toString() const
{
    ostringstream oss;
    oss << clazz->className << "~" << name << "~" << descriptor << "~" << id;
    return oss.str();
}

ostream &operator <<(ostream &os, const Field &field)
{
    os << field.toString() << endl;
    return os;
}

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


Class *ConstantPool::resolveClass(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Class or _type[i] == JVM_CONSTANT_ResolvedClass);

    if (_type[i] == JVM_CONSTANT_ResolvedClass)
        return (Class *) _info[i];

    Class *c = loadClass(clazz->loader, className(i));
    type(i, JVM_CONSTANT_ResolvedClass);
    info(i, (slot_t) c);

    return c;
}

Method *ConstantPool::resolveMethod(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Methodref or _type[i] == JVM_CONSTANT_ResolvedMethod);

    if (type(i) == JVM_CONSTANT_ResolvedMethod)
        return (Method *) _info[i];

    Class *c = resolveClass(methodClassIndex(i));
    Method *m = c->lookupMethod(methodName(i), methodType(i));

    type(i, JVM_CONSTANT_ResolvedMethod);
    info(i, (slot_t) m);

    return m;
}

Method* ConstantPool::resolveInterfaceMethod(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_InterfaceMethodref or _type[i] == JVM_CONSTANT_ResolvedInterfaceMethod);

    if (type(i) == JVM_CONSTANT_ResolvedInterfaceMethod)
        return (Method *) _info[i];

    Class *c = resolveClass(interfaceMethodClassIndex(i));
    Method *m = c->lookupMethod(interfaceMethodName(i), interfaceMethodType(i));

    type(i, JVM_CONSTANT_ResolvedInterfaceMethod);
    info(i, (slot_t) m);

    return m;
}

Field *ConstantPool::resolveField(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Fieldref or _type[i] == JVM_CONSTANT_ResolvedField);

    if (type(i) == JVM_CONSTANT_ResolvedField)
        return (Field *) _info[i];

    Class *c = resolveClass(fieldClassIndex(i));
    Field *f = c->lookupField(fieldName(i), fieldType(i));

    type(i, JVM_CONSTANT_ResolvedField);
    info(i, (slot_t) f);

    return f;
}

Object *ConstantPool::resolveString(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_String or _type[i] == JVM_CONSTANT_ResolvedString);

    if (type(i) == JVM_CONSTANT_ResolvedString)
        return (Object *) _info[i];

    const utf8_t *str = string(i);
    Object *so = stringClass->intern(str);

    type(i, JVM_CONSTANT_ResolvedString);
    info(i, (slot_t) so);
    return so;
}

Object *ConstantPool::resolveMethodType(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_MethodType);
    return fromMethodDescriptor(methodTypeDescriptor(i), clazz->loader);
}

Object *ConstantPool::resolveMethodHandle(u2 i)
{
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_MethodHandle);

    auto caller = getCaller();

    u2 kind = methodHandleReferenceKind(i);
    u2 index = methodHandleReferenceIndex(i);

    const utf8_t *d1 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;)"
            "Ljava/lang/invoke/MethodHandle;";
    const utf8_t *d2 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)"
            "Ljava/lang/invoke/MethodHandle;";
    const utf8_t *d3 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/Class;)"
            "Ljava/lang/invoke/MethodHandle;";

    switch (kind) {
        case JVM_REF_getField: {
            Field *f = resolveField(index);

            // public MethodHandle findGetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findGetter", d1);
            return RSLOT(execJavaFunc(m, caller, f->clazz, newString(f->name), f->getType()));
        }
        case JVM_REF_getStatic: {
            Field *f = resolveField(index);

            // public MethodHandle findStaticGetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findStaticGetter", d1);
            return RSLOT(execJavaFunc(m, caller, f->clazz, newString(f->name), f->getType()));
        }
        case JVM_REF_putField: {
            Field *f = resolveField(index);

            // public MethodHandle findSetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findSetter", d1);
            return RSLOT(execJavaFunc(m, caller, f->clazz, newString(f->name), f->getType()));
        }
        case JVM_REF_putStatic: {
            Field *f = resolveField(index);

            // public MethodHandle findStaticSetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findStaticSetter", d1);
            return RSLOT(execJavaFunc(m, caller, f->clazz, newString(f->name), f->getType()));
        }
        case JVM_REF_invokeVirtual :{
            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("");
        }
        case JVM_REF_invokeStatic: {
            Method *m = resolveMethod(index);
            assert(m->isStatic());

            // public MethodHandle findStatic(Class<?> refc, String name, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;
            return RSLOT(execJavaFunc(caller->clazz->getDeclaredInstMethod("findStatic", d2),
                                      caller, m->clazz, newString(m->name), m->getType()));
        }
        case JVM_REF_invokeSpecial: {
            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("");
        }
        case JVM_REF_newInvokeSpecial: {
            // public MethodHandle findConstructor(Class<?> refc, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;

            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("");
        }
        case JVM_REF_invokeInterface: {
            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("");
        }
        default:
            NEVER_GO_HERE_ERROR("wrong reference kind: %d.\n", kind);
    }

    // todo
    jvm_abort("");
}

void Class::calcFieldsId()
{
    int insId = 0;
    if (superClass != nullptr) {
        insId = superClass->instFieldsCount; // todo 父类的私有变量是不是也算在了里面，不过问题不大，浪费点空间吧了
    }

    for(auto f : fields) {
        if (!f->isStatic()) {
            f->id = insId++;
            if (f->category_two)
                insId++;
        }
    }

    instFieldsCount = insId;
}

void Class::parseAttribute(BytecodeReader &r)
{
    u2 attr_count = r.readu2();

    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp.utf8(r.readu2());
        u4 attr_len = r.readu4();

        if (S(Signature) == attr_name) {
            signature = cp.utf8(r.readu2());
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(SourceFile) == attr_name) {
            u2 source_file_index = r.readu2();
            if (source_file_index > 0) {
                sourceFileName = cp.utf8(source_file_index);
            } else {
                /*
                 * 并不是每个class文件中都有源文件信息，这个因编译时的编译器选项而异。
                 * todo 什么编译选项
                 */
                sourceFileName = "Unknown source file";
            }
        } else if (S(EnclosingMethod) == attr_name) {
            u2 classIndex = r.readu2();
            u2 methodIndex = r.readu2(); // 指向 CONSTANT_NameAndType_info

            if (classIndex > 0) {
                enclosing.clazz = loadClass(loader, cp.className(classIndex));

                if (methodIndex > 0) {
                    enclosing.name = newString(cp.nameOfNameAndType(methodIndex));
                    enclosing.descriptor = newString(cp.typeOfNameAndType(methodIndex));
                }
            }
        } else if (S(BootstrapMethods) == attr_name) {
            u2 num = r.readu2();
            for (u2 k = 0; k < num; k++)
                bootstrap_methods.emplace_back(r);
        } else if (S(InnerClasses) == attr_name) {
            u2 num = r.readu2();
            for (u2 k = 0; k < num; k++)
                inner_classes.emplace_back(r);
        } else if (S(SourceDebugExtension) == attr_name) { // ignore
//            u1 source_debug_extension[attr_len];
//            bcr_read_bytes(reader, source_debug_extension, attr_len);
            r.skip(attr_len); // todo
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rt_visi_annos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rt_invisi_annos.emplace_back(r);
        } else if (S(Module) == attr_name) {
            module = new Module(cp, r);
        } else if (S(ModulePackages) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++) {
                u2 index = r.readu2();
                modulePackages.push_back(cp.packageName(index));
            }
        }
//        else if (S(ModuleHashes) == attr_name) {
//            r.skip(attr_len); // todo
//        } else if (S(ModuleTarget) == attr_name) {
//            r.skip(attr_len); // todo
//        }
        else if (S(ModuleMainClass) == attr_name) {
            u2 main_class_index = r.readu2();
            moduleMainClass = cp.className(main_class_index);
        }
//        else if (S(NestHost) == attr_name) {
//            u2 host_class_index = r.readu2(); // todo
//        } else if (S(NestMembers) == attr_name) {
//            u2 num = r.readu2();
//            vector<u2> nest_classes; // todo
//            for (u2 j = 0; j < num; j++) {
//                nest_classes.push_back(r.readu2());
//            }
//        }
        else { // unknown attribute
            printvm("unknown attribute: %s\n", attr_name); // todo
            r.skip(attr_len);
        }
    }
}

void Class::createVtable()
{
    assert(vtable.empty());

    if (superClass == nullptr) {
        int i = 0;
        for (auto &m : methods) {
            if (m->isVirtual()) {
                vtable.push_back(m);
                m->vtableIndex = i++;
            }
        }
        return;
    }

    // 将父类的vtable复制过来
    vtable.assign(superClass->vtable.begin(), superClass->vtable.end());

    for (auto m : methods) {
        if (m->isVirtual()) {
            auto iter = find_if(vtable.begin(), vtable.end(), [=](Method *m0){
                return utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor); });
            if (iter != vtable.end()) {
                // 重写了父类的方法，更新
                m->vtableIndex = (*iter)->vtableIndex;
                *iter = m;
            } else {
                // 子类定义了要给新方法，加到 vtable 后面
                vtable.push_back(m);
                m->vtableIndex = vtable.size() - 1;
            }
        }
    }
}

Class::ITable::ITable(const Class::ITable &itable)
{
    interfaces.assign(itable.interfaces.begin(), itable.interfaces.end());
    methods.assign(itable.methods.begin(), itable.methods.end());
}

Class::ITable& Class::ITable::operator=(const Class::ITable &itable)
{
    interfaces.assign(itable.interfaces.begin(), itable.interfaces.end());
    methods.assign(itable.methods.begin(), itable.methods.end());
    return *this;
}

/*
 * todo 为什么需要itable,而不是用vtable解决所有问题？
 * 一个类可以实现多个接口，而每个接口的函数编号是个自己相关的，
 * vtable 无法解决多个对应接口的函数编号问题。
 * 而对继承一个类只能继承一个父亲，子类只要包含父类vtable，
 * 并且和父类的函数包含部分编号是一致的，就可以直接使用父类的函数编号找到对应的子类实现函数。
 */
void Class::createItable()
{
    if (isInterface()) {
        int index = 0;
        if (superClass != nullptr) {
            itable = superClass->itable;
            index = itable.methods.size();
        }
        for (Method *m : methods) {
            // todo default 方法怎么处理？进不进 itable？
            // todo 调用 default 方法 生成什么调用指令？
            m->itableIndex = index++;
            itable.methods.push_back(m);
        }
        return;
    }

    /* parse non interface class */

    if (superClass != nullptr) {
        itable  = superClass->itable;
    }

    // 遍历 itable.methods，检查有没有接口函数在本类中被重写了。
    for (auto m : itable.methods) {
        for (auto m0 : methods) {
            if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                m = m0; // 重写了接口方法，更新
                break;
            }
        }
    }

    for (auto ifc : interfaces) {
        for (auto tmp : itable.interfaces) {
            if (utf8::equals(tmp.first->className, ifc->className)) {
                // 此接口已经在 itable.interfaces 中了
                goto next;
            }
        }

        itable.interfaces.emplace_back(ifc, itable.methods.size());
        for (auto m : ifc->methods) {
            for (auto m0 : methods) {
                if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                    m = m0; // 重写了接口方法，更新
                    break;
                }
            }
            itable.methods.push_back(m);
        }
next:;
    }
}

const void Class::genPkgName()
{
    char *pkg = dup(className);
    char *p = strrchr(pkg, '/');
    if (p == nullptr) {
        free(pkg);
        pkgName = ""; // 包名可以为空
    } else {
        *p = 0; // 得到包名
        slash2Dots(pkg);
        auto hashed = find(pkg);
        if (hashed != nullptr) {
            free(pkg);
            pkgName = hashed;
        } else {
            pkgName = pkg;
            save(pkgName);
        }
    }
}

Class::Class(Object *loader, u1 *bytecode, size_t len)
        : Object(classClass), loader(loader), bytecode(bytecode)
{
    assert(bytecode != nullptr);

    BytecodeReader r(bytecode, len);

    auto magic = r.readu4();
    if (magic != 0xcafebabe) {
        thread_throw(new ClassFormatError("bad magic"));
    }

    auto minor_version = r.readu2();
    auto major_version = r.readu2();
    /*
     * Class版本号和Java版本对应关系
     * JDK 1.8 = 52
     * JDK 1.7 = 51
     * JDK 1.6 = 50
     * JDK 1.5 = 49
     * JDK 1.4 = 48
     * JDK 1.3 = 47
     * JDK 1.2 = 46
     * JDK 1.1 = 45
     */
    if (major_version != 52) {
//        thread_throw(new ClassFormatError("bad class version")); // todo
    }

    // init constant pool
    new (&cp) ConstantPool(this, r.readu2());
    for (u2 i = 1; i < cp.size; i++) {
        u1 tag = r.readu1();
        cp.type(i, tag);
        switch (tag) {
            case JVM_CONSTANT_Class:
            case JVM_CONSTANT_String:
            case JVM_CONSTANT_MethodType:
            case JVM_CONSTANT_Module:
            case JVM_CONSTANT_Package:
                cp.info(i, r.readu2());
                break;
            case JVM_CONSTANT_NameAndType:
            case JVM_CONSTANT_Fieldref:
            case JVM_CONSTANT_Methodref:
            case JVM_CONSTANT_InterfaceMethodref:
            case JVM_CONSTANT_Dynamic:
            case JVM_CONSTANT_InvokeDynamic: {
                slot_t index1 = r.readu2();
                slot_t index2 = r.readu2();
                cp.info(i, (index2 << 16) + index1);
                break;
            }
            case JVM_CONSTANT_Integer: {
                u1 bytes[4];
                r.readBytes(bytes, 4);
                cp._int(i, bytes_to_int32(bytes));
                break;
            }
            case JVM_CONSTANT_Float: {
                u1 bytes[4];
                r.readBytes(bytes, 4);
                cp._float(i, bytes_to_float(bytes));
                break;
            }
            case JVM_CONSTANT_Long: {
                u1 bytes[8];
                r.readBytes(bytes, 8);
                cp._long(i, bytes_to_int64(bytes));
                cp.type(++i, JVM_CONSTANT_Placeholder);
                break;
            }
            case JVM_CONSTANT_Double: {
                u1 bytes[8];
                r.readBytes(bytes, 8);
                cp._double(i, bytes_to_double(bytes));
                cp.type(++i, JVM_CONSTANT_Placeholder);
                break;
            }
            case JVM_CONSTANT_Utf8: {
                u2 utf8_len = r.readu2();
                char buf[utf8_len + 1];
                r.readBytes((u1 *) buf, utf8_len);
                buf[utf8_len] = 0;

                const char *utf8 = find(buf);
                if (utf8 == nullptr) {
                    utf8 = strdup(buf);
                    utf8 = save(utf8);
                }
                cp.info(i, (slot_t) utf8);
                break;
            }
            case JVM_CONSTANT_MethodHandle: {
                slot_t index1 = r.readu1(); // 这里确实是 readu1, reference_kind
                slot_t index2 = r.readu2(); // reference_index
                cp.info(i, (index2 << 16) + index1);
                break;
            }
            default:
                thread_throw(new ClassFormatError(NEW_MSG("bad constant tag: %d\n", tag)));
        }
    }

    modifiers = r.readu2();
    className = cp.className(r.readu2());
    genPkgName();

    u2 super_class = r.readu2();
    if (super_class == 0) { // invalid constant pool reference
        this->superClass = nullptr;
    } else {
        this->superClass = cp.resolveClass(super_class);
    }

    // parse interfaces
    u2 interfacesCount = r.readu2();
    for (u2 i = 0; i < interfacesCount; i++)
        interfaces.push_back(cp.resolveClass(r.readu2()));

    // parse fields
    u2 fieldsCount = r.readu2();
    if (fieldsCount > 0) {
        fields.resize(fieldsCount);
        auto lastField = fieldsCount - 1;
        for (u2 i = 0; i < fieldsCount; i++) {
            auto f = new(g_heap.allocField()) Field(this, r);
            // 保证所有的 public fields 放在前面
            if (f->isPublic())
                fields[publicFieldsCount++] = f;
            else
                fields[lastField--] = f;
        }
    }

    calcFieldsId();

    // parse methods
    u2 methodsCount = r.readu2();
    if (methodsCount > 0) {
        methods.resize(methodsCount);
        auto lastMethod = methodsCount - 1;
        for (u2 i = 0; i < methodsCount; i++) {
            auto m = new(g_heap.allocMethod()) Method(this, r);
            // 保证所有的 public methods 放在前面
            if (m->isPublic())
                methods[publicMethodsCount++] = m;
            else
                methods[lastMethod--] = m;
        }
    }

    parseAttribute(r); // parse class attributes

    createVtable(); // todo 接口有没有必要创建 vtable
    createItable();

    data = (slot_t *)(this + 1);
    state = LOADED;
}

Class::Class(const char *className)
        : Object(classClass), className(dup(className)), /* 形参className可能非持久，复制一份 */
          modifiers(Modifier::MOD_PUBLIC), inited(true),
          loader(nullptr), superClass(objectClass)
{
    assert(className != nullptr);
    assert(className[0] == '[' || isPrimClassName(className));

    pkgName = "";

    if (className[0] == '[') {
        interfaces.push_back(loadBootClass(S(java_lang_Cloneable)));
        interfaces.push_back(loadBootClass(S(java_io_Serializable)));
    }

    createVtable();

    data = (slot_t *)(this + 1);
    state = LOADED;
}

Class::~Class()
{
    // todo something else

}

void Class::clinit()
{
    if (inited) {
        return;
    }

//    scoped_lock lock(clinit_mutex);
    pthread_mutex_lock(&clinitLock);
    if (inited) { // 需要再次判断 inited，有可能被其他线程置为 true
        pthread_mutex_unlock(&clinitLock);
        return;
    }

    state = INITING;

    if (superClass != nullptr) {
        superClass->clinit();
    }

    // 在这里先行 set inited true, 如不这样，后面执行<clinit>时，
    // 可能调用putstatic等函数也会触发<clinit>的调用造成死循环。
    inited = true;

    Method *method = getDeclaredMethod(S(class_init), S(___V), false);
    if (method != nullptr) { // 有的类没有<clinit>方法
        execJavaFunc(method);
    }

    inited = true;
    state = INITED;
    pthread_mutex_unlock(&clinitLock);
}

Field *Class::lookupField(const utf8_t *name, const utf8_t *descriptor)
{
    for (auto f : fields) {
        if (utf8::equals(f->name, name) && utf8::equals(f->descriptor, descriptor))
            return f;
    }

    // todo 在父类中查找
    Field *field;
    if (superClass != nullptr) {
        if ((field = superClass->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    // todo 在父接口中查找
    for (auto c : interfaces) {
        if ((field = c->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    thread_throw(new NoSuchFieldError(NEW_MSG("%s~%s~%s\n", className, name, descriptor)));
}

Field *Class::lookupStaticField(const utf8_t *name, const utf8_t *descriptor)
{
    Field *field = lookupField(name, descriptor);
    // todo Field == nullptr
    if (!field->isStatic()) {
        thread_throw(new IncompatibleClassChangeError());
    }
    return field;
}

Field *Class::lookupInstField(const utf8_t *name, const utf8_t *descriptor)
{
    Field* field = lookupField(name, descriptor);
    // todo Field == nullptr
    if (field->isStatic()) {
        thread_throw(new IncompatibleClassChangeError);
    }
    return field;
}

Field *Class::getDeclaredInstField(int id, bool ensureExist)
{
    for (auto f : fields) {
        if (!f->isStatic() && f->id == id)
            return f;
    }

    if (ensureExist) {
        // not find, but ensure exist, so...
        thread_throw(new NoSuchFieldError(NEW_MSG("%s, id = %d.", className, id)));
    }

    // not find
    return nullptr;
}

Method *Class::getDeclaredMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist)
{
    for (auto m : methods) {
        if (utf8::equals(m->name, name) && utf8::equals(m->descriptor, descriptor))
            return m;
    }

    if (ensureExist) {
        // not find, but ensure exist, so...
        thread_throw(new NoSuchMethodError(NEW_MSG("%s~%s~%s\n", className, name, descriptor)));
    }

    // not find
    return nullptr;
}

Method *Class::getDeclaredStaticMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist)
{
    for (auto m : methods) {
        if (m->isStatic() && utf8::equals(m->name, name) && utf8::equals(m->descriptor, descriptor))
            return m;
    }

    if (ensureExist) {
        // I don't find it, but you ensure exist, so...
        thread_throw(new NoSuchMethodError(NEW_MSG("%s~%s~%s\n", className, name, descriptor)));
    }

    // not find
    return nullptr;
}

Method *Class::getDeclaredInstMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist)
{
    for (auto m : methods) {
        if (!m->isStatic() && utf8::equals(m->name, name) && utf8::equals(m->descriptor, descriptor))
            return m;
    }

    if (ensureExist) {
        // not find, but ensure exist, so...
        thread_throw(new NoSuchMethodError(NEW_MSG("%s~%s~%s\n", className, name, descriptor)));
    }

    // not find
    return nullptr;
}

vector<Method *> Class::getDeclaredMethods(const utf8_t *name, bool public_only)
{
    assert(name != nullptr);
    vector<Method *> declaredMethods;

    for (auto m : methods) {
        if ((!public_only || m->isPublic()) && (utf8::equals(m->name, name)))
            declaredMethods.push_back(m);
    }

    return declaredMethods;
}

Method *Class::getConstructor(const utf8_t *descriptor)
{
    return getDeclaredMethod(S(object_init), descriptor);
}

Method *Class::getConstructor(Array *parameterTypes)
{
    assert(parameterTypes != nullptr);

    Class *c = loadBootClass("java/lang/invoke/MethodType");

    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes);
    Method *m = c->getDeclaredStaticMethod(
            "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
    auto mt = RSLOT(execJavaFunc(m, loadBootClass(S(void)), parameterTypes));

    // public String toMethodDescriptorString();
    m = c->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
    auto s = execJavaFunc(m, mt);
    return getConstructor(((jstrref) *s)->toUtf8());
}

vector<Method *> Class::getConstructors(bool public_only)
{
    return getDeclaredMethods(S(object_init), public_only);
}

Method *Class::lookupMethod(const char *name, const char *descriptor)
{
    Method *method = getDeclaredMethod(name, descriptor, false);
    if (method != nullptr) {
        return method;
    }

    // todo 在父类中查找
    if (superClass != nullptr) {
        if ((method = superClass->lookupMethod(name, descriptor)) != nullptr)
            return method;
    }

    // todo 在父接口中查找
    for (auto c : interfaces) {
        if ((method = c->lookupMethod(name, descriptor)) != nullptr)
            return method;
    }

    return nullptr;
}

Method *Class::lookupStaticMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    if (m != nullptr and !m->isStatic()) {
        thread_throw(new IncompatibleClassChangeError);
    }
    return m;
}

Method *Class::lookupInstMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    // todo m == nullptr
    if (m->isStatic()) {
        thread_throw(new IncompatibleClassChangeError);
    }
    return m;
}

bool Class::isSubclassOf(Class *father)
{
    assert(father != nullptr);

    if (this == father)
        return true;

    if (superClass != nullptr && superClass->isSubclassOf(father))
        return true;

    for (auto c : interfaces) {
        if (c->isSubclassOf(father))
            return true;
    }

    // array class 特殊处理
    if (isArrayClass() and father->isArrayClass() and dim() == father->dim()) {
        return elementClass()->isSubclassOf(father->elementClass());
    }

    return false;
}

int Class::inheritedDepth() const
{
    int depth = 0;
    const Class *c = this->superClass;
    for (; c != nullptr; c = c->superClass) {
        depth++;
    }
    return depth;
}

bool Class::isArrayClass() const
{
    return className[0] == '[';
}

bool Class::isPrimClass() const
{
    return isPrimClassName(className);
}

bool Class::isPrimArrayClass() const
{
    if (strlen(className) != 2 || className[0] != '[')
        return false;

    return strchr("ZBCSIFJD", className[1]) != nullptr;
}

Class *Class::arrayClass() const
{
    char buf[strlen(className) + 8]; // big enough

    // 数组
    if (className[0] == '[') {
        sprintf(buf, "[%s", className);
        return loadArrayClass(buf);
    }

    // 基本类型
    const char *tmp = getPrimArrayClassName(className);
    if (tmp != nullptr)
        return loadArrayClass(tmp);

    // 类引用
    sprintf(buf, "[L%s;", className);
    return loadArrayClass(buf);
}

string Class::toString() const
{
    string s = "class: ";
    s += className;
    return s;
}

int Class::dim() const
{
    int d = 0;
    while (className[d] == '[') d++;
    return d;
}

size_t Class::getEleSize()
{
    assert(isArrayClass());

    if (eleSize == 0) {
        // 判断数组单个元素的大小
        // 除了基本类型的数组外，其他都是引用类型的数组
        // 多维数组是数组的数组，也是引用类型的数组
        char t = className[1]; // jump '['
        if (t == 'Z') {
            eleSize = sizeof(jbool);
        } else if (t == 'B') {
            eleSize = sizeof(jbyte);
        } else if (t == 'C') {
            eleSize = sizeof(jchar);
        } else if (t == 'S') {
            eleSize = sizeof(jshort);
        } else if (t == 'I') {
            eleSize = sizeof(jint);
        } else if (t == 'F') {
            eleSize = sizeof(jfloat);
        } else if (t == 'J') {
            eleSize = sizeof(jlong);
        } else if (t == 'D') {
            eleSize = sizeof(jdouble);
        } else {
            eleSize = sizeof(jref);
        }
    }

    return eleSize;
}

Class *Class::componentClass()
{
    if (compClass != nullptr)
        return compClass;

    const char *compName = className;
    if (*compName != '[')
        return nullptr; // not a array

    compName++; // jump a '['

    // 判断 component's type
    if (*compName == '[') {
        compClass = loadArrayClass(compName);
        return compClass;
    }

    auto primClassName = getPrimClassName(*compName);
    if (primClassName != nullptr) {  // primitive type
        compClass = loadBootClass(primClassName);
        return compClass;
    }

    // 普通类: Lxx/xx/xx; 型
    compName++; // jump 'L'
    int last = strlen(compName) - 1;
    assert(last > 0);
    if (compName[last] != ';') {
        thread_throw(new UnknownError());
    } else {
        char buf[last + 1];
        strncpy(buf, compName, (size_t) last);
        buf[last] = 0;
        compClass = loadClass(loader, buf); // todo bug! ! 对于 ArrayClass 这个loader是bootClassLoader, 无法loader用户类
        return compClass;
    }
}

Class *Class::elementClass()
{
    if (eleClass != nullptr)
        return eleClass;

    if (!isArrayClass())
        return nullptr;

    auto curr = this;
    while(true) {
        auto cc = curr->componentClass();
        assert(cc != nullptr);
        if (!cc->isArrayClass()) {
            eleClass = cc;
            return eleClass;
        }
        curr = cc;
    }
}

void Class::buildStrPool()
{
    assert(this == stringClass);
    strpool = new unordered_set<Object *, StrObjHash, StrObjEquals>;
    pthread_mutex_init(&strpoolMutex, nullptr);
}

Object *Class::intern(const utf8_t *str)
{
    assert(str != nullptr);
    assert(this == stringClass);
    return intern(newString(str));
}

Object *Class::intern(Object *so)
{
    assert(so != nullptr);
    assert(this == stringClass);
    assert(so->clazz == stringClass);

    pthread_mutex_lock(&strpoolMutex);
    // return either the newly inserted element
    // or the equivalent element already in the set
    Object *interned = *(strpool->insert(so).first);
    pthread_mutex_unlock(&strpoolMutex);
    return interned;
}
