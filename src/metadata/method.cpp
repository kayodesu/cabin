#include "method.h"
#include "class.h"
#include "../objects/array.h"
#include "descriptor.h"

using namespace std;
using namespace utf8;

Method::ExceptionTable::ExceptionTable(Class *clazz, BytecodeReader &r)
{
    start_pc = r.readu2();
    end_pc = r.readu2();
    handler_pc = r.readu2();
    u2 index = r.readu2();
    if (index == 0) {
        // 异常处理项的 catch_type 有可能是 0。
        // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
        catch_type = nullptr;
    } else {
        catch_type = new CatchType;
        if (clazz->cp.type(index) == JVM_CONSTANT_ResolvedClass) {
            catch_type->resolved = true;
            catch_type->u.clazz = clazz->cp.resolveClass(index);
        } else {
            // Note:
            // 不能在这里load class，有形成死循环的可能。
            // 比如当前方法是 Throwable 中的方法，而此方法又抛出了 Throwable 子类的Exception（记为A），
            // 而此时 Throwable 还没有构造完成，所以无法构造其子类 A。
            catch_type->resolved = false;
            catch_type->u.class_name = clazz->cp.className(index);
        }
    }
}

Array *Method::getParameterTypes()
{
    pair<Array *, ClsObj *> p = parseMethodDescriptor(descriptor, clazz->loader);
    return p.first;
}

ClsObj *Method::getReturnType()
{
    pair<Array *, ClsObj *> p = parseMethodDescriptor(descriptor, clazz->loader);
    return p.second;
}

Array *Method::getExceptionTypes()
{
    if (exception_types == nullptr) {
        int count = 0;
        Class *types[exception_tables.size()];
        for (auto t : exception_tables) {
            if (t.catch_type == nullptr)
                continue;

            if (!t.catch_type->resolved) {
                t.catch_type->u.clazz = loadClass(clazz->loader, t.catch_type->u.class_name);
                t.catch_type->resolved = true;
            }
            types[count++] = t.catch_type->u.clazz;
        }

        auto ac = loadClass(clazz->loader, S(array_java_lang_Class));
        exception_types = ac->allocArray(count);
        for (int i = 0; i < count; i++)
            exception_types->setRef(i, types[i]->java_mirror);
    }

    assert(exception_types != nullptr);
    return exception_types;
}

u2 Method::calArgsSlotsCount(const utf8_t *descriptor, bool isStatic)
{
    assert(descriptor != nullptr);
    u2 count = 0;

    const char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == nullptr || e == nullptr) {
        JVM_PANIC("error. %s\n", descriptor);
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
                JVM_PANIC("error. %s\n", descriptor);
            }
        } else if (*b == '[') { // array reference
            count++;
            while (*(++b) == '[');

            if (*b == 'L') {
                b = strchr(b, ';');
                if (b == nullptr) {
                    JVM_PANIC("error. %s\n", descriptor);
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
    max_stack = r.readu2();
    max_locals = r.readu2();
    code_len = r.readu4();
    code = r.currPos();
    r.skip(code_len);

    // parse exception tables
    int exception_tables_count = r.readu2();
    for (int i = 0; i < exception_tables_count; i++) {
        exception_tables.emplace_back(clazz, r);
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
            r.skip(attr_len); // todo ....
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

void *findNativeMethod(const char *class_name, const char *method_name, const char *method_type);

Method::Method(Class *c, BytecodeReader &r)
{
    assert(c != nullptr);
    clazz = c;
    ConstantPool &cp = c->cp;

    access_flags = r.readu2();
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
                checked_exceptions.push_back(r.readu2());
        } else if (S(RuntimeVisibleParameterAnnotations) == attr_name) {
            u2 num = r.readu2();
            rt_visi_para_annos.resize(num);
            for (u2 j = 0; j < num; j++) {
                u2 numAnnos = r.readu2();
                for (u2 k = 0; k < numAnnos; k++)
                    rt_visi_para_annos[j].emplace_back(r);
            }
        } else if (S(RuntimeInvisibleParameterAnnotations) == attr_name) {
            u2 num = r.readu2();
            rt_invisi_para_annos.resize(num);
            for (u2 j = 0; j < num; j++) {
                u2 numAnnos = r.readu2();
                for (u2 k = 0; k < numAnnos; k++)
                    rt_invisi_para_annos[j].emplace_back(r);
            }
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++)
                rt_visi_annos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++)
                rt_invisi_annos.emplace_back(r);
        } else if (S(AnnotationDefault) == attr_name) {
            annotation_default.read(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }

    const char *t = strchr(descriptor, ')'); // find return
    assert(t != nullptr);

    t++;
    if (*t == 'V') {
        ret_type = RET_VOID;
    } else if (*t == 'D') {
        ret_type = RET_DOUBLE;
    } else if (*t == 'F') {
        ret_type = RET_FLOAT;
    } else if (*t == 'J') {
        ret_type = RET_LONG;
    } else if (*t == 'L' || *t == '[') {
        ret_type = RET_REFERENCE;
    } else  if (*t == 'I') {
        ret_type = RET_INT;
    } else  if (*t == 'B') {
        ret_type = RET_BYTE;
    } else  if (*t == 'Z') {
        ret_type = RET_BOOL;
    } else  if (*t == 'C') {
        ret_type = RET_CHAR;
    } else  if (*t == 'S') {
        ret_type = RET_SHORT;
    } else {
        // todo error
    }

    if (isNative()) {
        // 本地方法帧的操作数栈至少要能容纳返回值，
        // 4 slots are big enough.
        max_stack = 4;
        // 因为本地方法帧的局部变量表只用来存放参数值，
        // 所以把arg_slot_count赋给max_locals字段刚好。
        max_locals = arg_slot_count;

        code_len = 2;
        code = new u1[code_len];
        code[0] = JVM_OPC_invokenative;

        if (ret_type == RET_VOID) {
            code[1] = JVM_OPC_return;
        } else if (ret_type == RET_DOUBLE) {
            code[1] = JVM_OPC_dreturn;
        } else if (ret_type == RET_FLOAT) {
            code[1] = JVM_OPC_freturn;
        } else if (ret_type == RET_LONG) {
            code[1] = JVM_OPC_lreturn;
        } else if (ret_type == RET_REFERENCE) {
            code[1] = JVM_OPC_areturn;
        } else {
            code[1] = JVM_OPC_ireturn;
        }

        native_method = findNativeMethod(clazz->class_name, name, descriptor);
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
    for (auto t : exception_tables) {
        // jvms: The start pc is inclusive and end pc is exclusive
        if (t.start_pc <= pc && pc < t.end_pc) {
            if (t.catch_type == nullptr)  // catch all
                return t.handler_pc;
            if (!t.catch_type->resolved) {
                t.catch_type->u.clazz = loadClass(clazz->loader, t.catch_type->u.class_name);
                t.catch_type->resolved = true;
            }
            if (exceptionType->isSubclassOf(t.catch_type->u.clazz))
                return t.handler_pc;
        }
    }

    return -1;
}

bool Method::isSignaturePolymorphic() 
{
    bool b = equals(clazz->class_name, S(java_lang_invoke_MethodHandle))
                 || equals(clazz->class_name, S(java_lang_invoke_VarHandle));
    if (!b)
        return false;

    Array *ptypes = getParameterTypes(); // Class<?>[]
    if (ptypes->arr_len != 1)
        return false;

    auto ptype = ptypes->get<ClsObj *>(0);
    if (!equals(ptype->jvm_mirror->class_name, S(array_java_lang_Object))) 
        return false;

    if (!(isVarargs() && isNative()))
        return false;

    return true;
}

string Method::toString() const
{
    ostringstream oss;
    oss << "method";
    if (isNative())
        oss << "(native)";
    oss << ": " << clazz->class_name << "~" << name << "~" << descriptor;
    return oss.str();
}
