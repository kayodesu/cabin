/*
 * Author: Yo Ka
 */

#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <iostream>
#include "../runtime/vm_thread.h"
#include "class.h"
#include "method.h"
#include "field.h"
#include "../objects/array_object.h"
#include "../objects/class_object.h"
#include "../interpreter/interpreter.h"
#include "../objects/prims.h"
#include "../objects/mh.h"
#include "../classfile/constants.h"
#include "descriptor.h"

using namespace std;
using namespace utf8;
using namespace method_handles;


void Class::calcFieldsId()
{
    int ins_id = 0;
    if (super_class != nullptr) {
        ins_id = super_class->inst_field_count; // todo 父类的私有变量是不是也算在了里面，不过问题不大，浪费点空间吧了
    }

    for (Field *f: fields) {
        if (!f->isStatic()) {
            f->id = ins_id++;
            if (f->category_two)
                ins_id++;
        }
    }

    inst_field_count = ins_id;
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
                source_file_name = cp.utf8(source_file_index);
            }
        } else if (S(EnclosingMethod) == attr_name) {
            u2 classIndex = r.readu2(); // 指向 CONSTANT_Class_info
            u2 methodIndex = r.readu2(); // 指向 CONSTANT_NameAndType_info

            if (classIndex > 0) {
                // enclosing.clazz = loadClass(loader, cp.className(classIndex));
                enclosing.clazz = cp.resolveClass(classIndex);

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
                module_packages.push_back(cp.packageName(index));
            }
        } else if (S(ModuleMainClass) == attr_name) {
            module_main_class = cp.className(r.readu2());
        } else if (S(NestHost) == attr_name) {
//            utf8_t *name = cp.className(r.readu2());
            nest_host = cp.resolveClass(r.reads2());//loadClass(loader, name);
        } else if (S(NestMembers) == attr_name) {
            u2 num = r.readu2();
            for (u2 j = 0; j < num; j++) {
                utf8_t *name = cp.className(r.readu2());
                printvm("%s\n", name);
                // todo 不要在这里 loadClass，有死循环的问题。
                // 比如java.lang.invoke.TypeDescriptor和其NestMember：java.lang.invoke.TypeDescriptor$OfField
//                nest_members.push_back(loadClass(loader, name));
            }
        } else { // unknown attribute
            printvm("unknown attribute: %s\n", attr_name); // todo
            r.skip(attr_len);
        }
    }

    // A class with no NestHost or NestMembers attribute,
    // implicitly forms a nest with itself as the nest host, and sole nest member.
    if (nest_host == nullptr)
        nest_host = this;
    if (nest_members.empty())
        nest_members.push_back(this);
}

void Class::createVtable()
{
    assert(vtable.empty());

    if (super_class == nullptr) {
        int i = 0;
        for (auto &m : methods) {
            if (m->isVirtual()) {
                vtable.push_back(m);
                m->vtable_index = i++;
            }
        }
        return;
    }

    // 将父类的vtable复制过来
    vtable.assign(super_class->vtable.begin(), super_class->vtable.end());

    for (auto m : methods) {
        if (m->isVirtual()) {
            auto iter = find_if(vtable.begin(), vtable.end(), [=](Method *m0){
                return utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor); });
            if (iter != vtable.end()) {
                // 重写了父类的方法，更新
                m->vtable_index = (*iter)->vtable_index;
                *iter = m;
            } else {
                // 子类定义了要给新方法，加到 vtable 后面
                vtable.push_back(m);
                m->vtable_index = vtable.size() - 1;
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

void Class::ITable::add(const ITable &itable)
{
    for (auto ifc: itable.interfaces) {
        interfaces.emplace_back(ifc.first, methods.size());  
        for (auto m: itable.methods) 
            methods.push_back(m);
    }
}

Method *Class::findFromITable(Class *interface_class, int itable_index)
{
    assert(interface_class != nullptr && interface_class->isInterface());

    for (auto interface: itable.interfaces) {
        if (interface.first->equals(interface_class)) {
            size_t offset = interface.second;
            assert(offset + itable_index < itable.methods.size());
            return itable.methods[offset + itable_index];
        }
    }

    return nullptr;
}

/*
 * 为什么需要itable,而不是用vtable解决所有问题？
 * 一个类可以实现多个接口，而每个接口的函数编号是个自己相关的，
 * vtable 无法解决多个对应接口的函数编号问题。
 * 而对继承一个类只能继承一个父亲，子类只要包含父类vtable，
 * 并且和父类的函数包含部分编号是一致的，就可以直接使用父类的函数编号找到对应的子类实现函数。
 */
void Class::createItable()
{
    if (isInterface()) {
        // 接口间的继承虽然用 extends 关键字（可以同时继承多个接口），但被继承的接口不是子接口的 super_class，
        // 而是在子接口的 interfaces 里面。所以接口的 super_class 就是 java/lang/Object 

        for (Class *super_interface: interfaces) {
            itable.add(super_interface->itable);
        }

        itable.interfaces.emplace_back(this, itable.methods.size());
        int index = 0;
        for (Method *m : methods) {
            // todo default 方法怎么处理？进不进 itable？
            // todo 调用 default 方法 生成什么调用指令？
            m->itable_index = index++;
            itable.methods.push_back(m);
        }
        
        return;
    }

    /* parse non interface class */

    if (super_class != nullptr) {
        itable = super_class->itable;
    }

    for (auto ifc : interfaces) {
        for (auto tmp : itable.interfaces) {
            if (ifc->equals(tmp.first)) {
                // 此接口已经在 itable.interfaces 中了
                goto next;
            }
        }

        // 发现一个新实现的接口
        itable.interfaces.emplace_back(ifc, itable.methods.size());
        // 检查新实现接口的方法是不是已经被重写了
        for (auto m: ifc->methods) {
            for (auto m0: itable.methods) {
                if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                    m = m0; // 重写了接口方法，更新
                    break;
                }
            }
            itable.methods.push_back(m);
        }
next:;
    }
    
    // 遍历 itable.methods，检查有没有接口函数在本类中被重写了。
    for (size_t i = 0; i < itable.methods.size(); i++) {
        auto m = itable.methods[i];
        for (auto m0 : methods) {
            if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                //m0->itable_index = m->itable_index;
                itable.methods[i] = m0; // 重写了接口方法，更新
                break;
            }
        }
    }
}

void Class::generateIndepInterfaces()
{
    if (super_class != nullptr) {
        indep_interfaces = super_class->indep_interfaces;
    }
    for (Class *c: interfaces) {
        indep_interfaces.insert(c);
    }

    /* 独立化 indep_interfaces */

    vector<Class *> need_del_interfaces;
    for (Class *c: indep_interfaces) {
        // 将 c 的父接口全部放入
        for (Class *i: c->interfaces) {
            need_del_interfaces.push_back(i);
        }
    }

    for (size_t i = 0; i < need_del_interfaces.size(); i++) {
        Class *del = need_del_interfaces[i];
        indep_interfaces.erase(del);
        // 将 del 的父接口全部放入
        for (Class *c: del->interfaces) {
            need_del_interfaces.push_back(c);
        }
    }
}

void Class::genPkgName()
{
    char *pkg = dup(class_name);
    char *p = strrchr(pkg, '/');
    if (p == nullptr) {
        free(pkg);
        pkg_name = ""; // 包名可以为空
    } else {
        *p = 0; // 得到包名
        slash2Dot(pkg);
        auto hashed = find(pkg);
        if (hashed != nullptr) {
            free(pkg);
            pkg_name = hashed;
        } else {
            pkg_name = pkg;
            save(pkg_name);
        }
    }
}

Class::Class(Object *loader, u1 *bytecode, size_t len): loader(loader), bytecode(bytecode)
{
    assert(bytecode != nullptr);

    BytecodeReader r(bytecode, len);

    auto magic = r.readu4();
    if (magic != 0xcafebabe) {
        Thread::signalException(S(java_lang_ClassFormatError), "bad magic");
        return;
    }

    r.readu2(); // minor_version
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
                Thread::signalException(S(java_lang_ClassFormatError), MSG("bad constant tag: %d\n", tag));
                return;
        }
    }

    access_flags = r.readu2();
    class_name = cp.className(r.readu2());
    genPkgName();

    u2 _super_class = r.readu2();
    if (_super_class == 0) { // invalid constant pool reference
        this->super_class = nullptr;
    } else {
        if (utf8::equals(class_name, S(java_lang_Object))) {
            Thread::signalException(S(java_lang_ClassFormatError), "Object has super");
            return;
        }
        this->super_class = cp.resolveClass(_super_class);
    }

    // parse interfaces
    u2 interface_count = r.readu2();
    for (u2 i = 0; i < interface_count; i++)
        interfaces.push_back(cp.resolveClass(r.readu2()));

    // parse fields
    u2 fields_count = r.readu2();
    if (fields_count > 0) {
        fields.resize(fields_count);
        auto last_field = fields_count - 1;
        for (u2 i = 0; i < fields_count; i++) {
            auto f = new Field(this, r);
            // 保证所有的 public fields 放在前面             
            if (f->isPublic())
                fields[public_field_count++] = f;
            else
                fields[last_field--] = f;
        }
    }

    calcFieldsId();

    // parse methods
    u2 methods_count = r.readu2();
    if (methods_count > 0) {
        methods.resize(methods_count);
        auto last_method = methods_count - 1;
        for (u2 i = 0; i < methods_count; i++) {
            auto m = new Method(this, r);
            // 保证所有的 public methods 放在前面
            if (m->isPublic())
                methods[public_method_count++] = m;
            else
                methods[last_method--] = m;
        }
    }

    parseAttribute(r); // parse class attributes

    createVtable(); // todo 接口有没有必要创建 vtable
    createItable();
    generateIndepInterfaces();

    if (g_class_class != nullptr) {
        java_mirror = generateClassObject(this);
    }

    state = LOADED;
}

Class::Class(const char *className)
        : class_name(dup(className)), /* 形参className可能非持久，复制一份 */
          access_flags(JVM_ACC_PUBLIC), inited(true),
          loader(nullptr), super_class(g_object_class)
{
    assert(className != nullptr);
    assert(isPrimClassName(className));

    pkg_name = "";

    createVtable();
    createItable();
    generateIndepInterfaces();

    if (g_class_class != nullptr) {
        java_mirror = generateClassObject(this);
    }

    state = LOADED;
}

Class::Class(Object *loader, const char *className)
        : class_name(dup(className)), /* 形参className可能非持久，复制一份 */
          access_flags(JVM_ACC_PUBLIC), inited(true),
          loader(loader), super_class(g_object_class)
{
    assert(className != nullptr);
    assert(className[0] == '[');

    pkg_name = "";

    interfaces.push_back(loadBootClass(S(java_lang_Cloneable)));
    interfaces.push_back(loadBootClass(S(java_io_Serializable)));

    createVtable();
    createItable();
    generateIndepInterfaces();

    if (g_class_class != nullptr) {
        java_mirror = generateClassObject(this);
    }

    state = LOADED;
}


Class::~Class()
{
    delete[] bytecode;

    // todo something else
}

void Class::clinit()
{
    if (inited) {
        return;
    }

    scoped_lock lock(clinit_mutex);
    if (inited) { // 需要再次判断 inited，有可能被其他线程置为 true
        return;
    }

    state = INITING;

    if (super_class != nullptr) {
        super_class->clinit();
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
}

Field *Class::lookupField(const utf8_t *name, const utf8_t *descriptor)
{
//    for (u2 i = 0; i < field_count; i++) {
//        Field *f = fields + i;
//        if (utf8::equals(f->name, name) && utf8::equals(f->descriptor, descriptor))
//            return f;
//    }
//
//    // todo 在父类中查找
//    Field *field;
//    if (super_class != nullptr) {
//        if ((field = super_class->lookupField(name, descriptor)) != nullptr)
//            return field;
//    }
//
//    // todo 在父接口中查找
//    for (auto c : interfaces) {
//        if ((field = c->lookupField(name, descriptor)) != nullptr)
//            return field;
//    }
//
//    return nullptr;

    Field *f;
    Class *clazz = this;
    do {
        if ((f = clazz->getDeclaredField(name, descriptor)) != nullptr)
            return f;
        clazz = clazz->super_class;
    } while (clazz != nullptr);

    for (Class *c: indep_interfaces) {
        // 在接口 c 及其父接口中查找
        if ((f = c->lookupField(name, descriptor)) != nullptr)
            return f;
    }

    return nullptr;
}

Field *Class::lookupStaticField(const utf8_t *name, const utf8_t *descriptor)
{
    Field *field = lookupField(name, descriptor);
    if (field != nullptr && !field->isStatic()) {
        Thread::signalException(S(java_lang_IncompatibleClassChangeError));
        return nullptr;
    }
    return field;
}

Field *Class::lookupInstField(const utf8_t *name, const utf8_t *descriptor)
{
    Field* field = lookupField(name, descriptor);
    if (field != nullptr && field->isStatic()) {
        Thread::signalException(S(java_lang_IncompatibleClassChangeError));
        return nullptr;
    }
    return field;
}

Field *Class::getDeclaredField(const char *name, const char *descriptor) const
{
    for (Field *f: fields) {
        if (utf8::equals(f->name, name) && utf8::equals(f->descriptor, descriptor))
            return f;
    }
    return nullptr;
}

Field *Class::getDeclaredInstField(int id, bool ensureExist)
{
    for (Field *f: fields) {
        if (!f->isStatic() && f->id == id)
            return f;
    }

    if (ensureExist) {
        // not find, but ensure exist, so...
        Thread::signalException(S(java_lang_NoSuchFieldError), NEW_MSG("%s, id = %d.", class_name, id));
    }

    // not find
    return nullptr;
}

void Class::injectInstField(const utf8_t *name, const utf8_t *descriptor)
{
    assert(name != nullptr && descriptor != nullptr);
    if (state != LOADED) {
        jvm_abort("error"); // todo 只能在loade之后 inject Field
    }

    int access_flags = JVM_ACC_PRIVATE | JVM_ACC_SYNTHETIC;
    auto f = new Field(this, name, descriptor, access_flags);
    fields.push_back(f);
    inst_field_count++;
}

Method *Class::getDeclaredMethod(const utf8_t *name, const utf8_t *descriptor, bool ensureExist)
{
    for (auto m : methods) {
        // if (m->isSignaturePolymorphic()) {
        //     if (strcmp(name, "invoke") == 0&&strcmp(m->name, "invoke") == 0) {
        //         m->isSignaturePolymorphic();
        //         printvm("===== %s, %s, %s\n", m->clazz->class_name, m->name, m->descriptor);
        // }
        if (utf8::equals(m->name, name) && utf8::equals(m->descriptor, descriptor))
            return m;
    }

    if (ensureExist) {
        // not find, but ensure exist, so...
        Thread::signalException(S(java_lang_NoSuchMethodError), NEW_MSG("%s~%s~%s\n", class_name, name, descriptor));
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
        // not find, but ensure exist, so...
        Thread::signalException(S(java_lang_NoSuchMethodError), NEW_MSG("%s~%s~%s\n", class_name, name, descriptor));
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
        Thread::signalException(S(java_lang_NoSuchMethodError), NEW_MSG("%s~%s~%s\n", class_name, name, descriptor));
    }

    // not find
    return nullptr;
}

Method *Class::getDeclaredPolymorphicSignatureMethod(const utf8_t *name)
{
    assert(name != nullptr);

    for (auto m : methods) {
        if (utf8::equals(m->name, name) && m->isSignaturePolymorphic())
            return m;
    }
    
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

Method *Class::getConstructor(Array *parameter_types)
{
    assert(parameter_types != nullptr);

//    Class *c = loadBootClass("java/lang/invoke/MethodType");
//
//    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes);
//    Method *m = c->getDeclaredStaticMethod(
//            "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
//    auto mt = RSLOT(execJavaFunc(m, { loadBootClass(S(void))->java_mirror, parameter_types } ));
//
//    // public String toMethodDescriptorString();
//    m = c->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
//    auto s = (RSLOT(execJavaFunc(m, {mt})))->toUtf8();
    string desc = unparseMethodDescriptor(parameter_types, nullptr);
    return getConstructor(desc.c_str());
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

//    // todo 在父类中查找
//    if (super_class != nullptr) {
//        if ((method = super_class->lookupMethod(name, descriptor)) != nullptr)
//            return method;
//    }
//
//    // todo 在父接口中查找
//    for (auto c : interfaces) {
//        if ((method = c->lookupMethod(name, descriptor)) != nullptr)
//            return method;
//    }

    Class *super = super_class;
    while (super != nullptr) {
        if ((method = super->getDeclaredMethod(name, descriptor, false)) != nullptr)
            return method;
        super = super->super_class;
    }

    for (Class *c: indep_interfaces) {
        // 在接口 c 及其父接口中查找
        if ((method = c->lookupMethod(name, descriptor)) != nullptr)
            return method;
    }

    return nullptr;
}

Method *Class::lookupStaticMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    if (m != nullptr && !m->isStatic()) {
        Thread::signalException(S(java_lang_IncompatibleClassChangeError));
        return nullptr;
    }
    return m;
}

Method *Class::lookupInstMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    // todo m == nullptr
    if (m != nullptr && m->isStatic()) {
        Thread::signalException(S(java_lang_IncompatibleClassChangeError));
        return nullptr;
    }
    return m;
}

bool Class::isSubclassOf(Class *father)
{
    assert(father != nullptr);

    if (this == father)
        return true;

    if (super_class != nullptr && super_class->isSubclassOf(father))
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
    const Class *c = this->super_class;
    for (; c != nullptr; c = c->super_class) {
        depth++;
    }
    return depth;
}

bool Class::isArrayClass() const
{
    return class_name[0] == '[';
}

bool Class::isPrimClass() const
{
    return isPrimClassName(class_name);
}

bool Class::isPrimWrapperClass() const
{
    return isPrimWrapperClassName(class_name);
}

bool Class::isVoidClass() const
{
    return strcmp(class_name, "void") == 0;
}

bool Class::isPrimArrayClass() const
{
    if (strlen(class_name) != 2 || class_name[0] != '[')
        return false;

    return strchr("ZBCSIFJD", class_name[1]) != nullptr;
}

Class *Class::arrayClass() const
{
    char buf[strlen(class_name) + 8]; // big enough

    // 数组
    if (class_name[0] == '[') {
        sprintf(buf, "[%s", class_name);
        return loadArrayClass(buf);
    }

    // 基本类型
    const char *tmp = getPrimArrayClassName(class_name);
    if (tmp != nullptr)
        return loadArrayClass(tmp);

    // 类引用
    sprintf(buf, "[L%s;", class_name);
    return loadArrayClass(buf);
}

string Class::toString() const
{
    string s = "class: ";
    s += class_name;
    return s;
}

int Class::dim() const
{
    int d = 0;
    while (class_name[d] == '[') d++;
    return d;
}

size_t Class::getEleSize()
{
    assert(isArrayClass());

    if (ele_size == 0) {
        // 判断数组单个元素的大小
        // 除了基本类型的数组外，其他都是引用类型的数组
        // 多维数组是数组的数组，也是引用类型的数组
        char t = class_name[1]; // jump '['
        if (t == 'Z') {
            ele_size = sizeof(jbool);
        } else if (t == 'B') {
            ele_size = sizeof(jbyte);
        } else if (t == 'C') {
            ele_size = sizeof(jchar);
        } else if (t == 'S') {
            ele_size = sizeof(jshort);
        } else if (t == 'I') {
            ele_size = sizeof(jint);
        } else if (t == 'F') {
            ele_size = sizeof(jfloat);
        } else if (t == 'J') {
            ele_size = sizeof(jlong);
        } else if (t == 'D') {
            ele_size = sizeof(jdouble);
        } else {
            ele_size = sizeof(jref);
        }
    }

    return ele_size;
}

Class *Class::componentClass()
{
    if (comp_class != nullptr)
        return comp_class;

    const char *comp_name = class_name;
    if (*comp_name != '[')
        return nullptr; // not a array

    comp_name++; // jump a '['

    // 判断 component's type
    if (*comp_name == '[') {
        comp_class = loadArrayClass(comp_name);
        return comp_class;
    }

    auto prim_class_name = getPrimClassName(*comp_name);
    if (prim_class_name != nullptr) {  // primitive type
        comp_class = loadBootClass(prim_class_name);
        return comp_class;
    }

    // 普通类: Lxx/xx/xx; 型
    comp_name++; // jump 'L'
    int last = strlen(comp_name) - 1;
    assert(last > 0);
    if (comp_name[last] != ';') {
        Thread::signalException(S(java_lang_UnknownError));
        return nullptr;
    } else {
        char buf[last + 1];
        strncpy(buf, comp_name, (size_t) last);
        buf[last] = 0;
        comp_class = loadClass(loader, buf); 
        return comp_class;
    }
}

Class *Class::elementClass()
{
    if (ele_class != nullptr)
        return ele_class;

    if (!isArrayClass())
        return nullptr;

    auto curr = this;
    while(true) {
        auto cc = curr->componentClass();
        assert(cc != nullptr);
        if (!cc->isArrayClass()) {
            ele_class = cc;
            return ele_class;
        }
        curr = cc;
    }
}

void Class::buildStrPool()
{
    assert(this == g_string_class);
    assert(str_pool == nullptr);
    str_pool = new unordered_set<Object *, StrObjHash, StrObjEquals>;
}

jstrref Class::intern(const utf8_t *str)
{
    assert(str != nullptr);
    assert(this == g_string_class);
    return intern(newString(str));
}

jstrref Class::intern(jstrref so) 
{
    assert(so != nullptr);
    assert(this == g_string_class);
    assert(so->clazz == g_string_class);

    scoped_lock lock(str_pool_mutex);

    // return either the newly inserted element
    // or the equivalent element already in the set
    Object *interned = *(str_pool->insert(so).first);
    return interned;
}
