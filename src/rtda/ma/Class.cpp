/*
 * Author: kayo
 */

#include <vector>
#include <algorithm>
#include <sstream>
#include "Class.h"
#include "Field.h"
#include "resolve.h"
#include "../../interpreter/interpreter.h"
#include "../../classfile/constant.h"
#include "../heap/ClassObject.h"
#include "../heap/StringObject.h"

using namespace std;

void Class::calcFieldsId()
{
    int insId = 0;
    int staticId = 0;

    if (superClass != nullptr) {
        insId = superClass->instFieldsCount; // todo 父类的私有变量是不是也算在了里面，不过问题不大，浪费点空间吧了
    }

    for(auto f : fields) {
        if (f->isStatic()) {
            f->id = staticId++;
            if (f->categoryTwo)
                staticId++;
        } else {
            f->id = insId++;
            if (f->categoryTwo)
                insId++;
        }
    }

    instFieldsCount = insId;
    staticFieldsCount = staticId;

    staticFieldsValues =(slot_t *)vm_calloc(staticFieldsCount, sizeof(slot_t)); // 清零
}

void Class::parseAttribute(BytecodeReader &r)
{
    u2 attr_count = r.readu2();

    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = CP_UTF8(cp, r.readu2());
        u4 attr_len = r.readu4();

        if (S(Signature) == attr_name) {
            signature = CP_UTF8(cp, r.readu2());
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(SourceFile) == attr_name) {
            u2 source_file_index = r.readu2();
            if (source_file_index >= 0) {
                sourceFileName = CP_UTF8(cp, source_file_index);
            } else {
                /*
                 * 并不是每个class文件中都有源文件信息，这个因编译时的编译器选项而异。
                 * todo 什么编译选项
                 */
                sourceFileName = "Unknown source file";
            }
        } else if (S(EnclosingMethod) == attr_name) {
            u2 enclosing_class_index = r.readu2();
            u2 enclosing_method_index = r.readu2();

            if (enclosing_class_index > 0) {
                Class *enclosing_class = loader->loadClass(CP_CLASS_NAME(cp, enclosing_class_index));
                enclosing.clazz = ClassObject::newInst(enclosing_class);

                if (enclosing_method_index > 0) {
                    enclosing.name = StringObject::newInst(CP_NAME_TYPE_NAME(cp, enclosing_method_index));
                    enclosing.descriptor = StringObject::newInst(CP_NAME_TYPE_TYPE(cp, enclosing_method_index));
                }
            }
        } else if (S(BootstrapMethods) == attr_name) {
//            u2 num = r.readu2();
//            BootstrapMethod methods[num];
//
//            for (u2 k = 0; k < num; k++) {
//                methods[k].bootstrap_method_ref = r.readu2();
//                methods[k].num_bootstrap_arguments = r.readu2();
//                methods[k].bootstrap_arguments = vm_malloc(sizeof(u2) * methods[k].num_bootstrap_arguments); // todo 没有 free
//                for (int j = 0; j < methods[k].num_bootstrap_arguments; j++) {
//                    methods[k].bootstrap_arguments[j] = r.readu2();
//                }
//            }

//            rtcp_build_invoke_dynamic_constant(c->rtcp, methods);  todo

            r.skip(attr_len);
        } else if (S(InnerClasses) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct inner_class classes[num];
//            for (u2 k = 0; k < num; k++) {
//                classes[k].inner_class_info_index = readu2(reader);
//                classes[k].outer_class_info_index = readu2(reader);
//                classes[k].inner_name_index = readu2(reader);
//                classes[k].inner_class_access_flags = readu2(reader);
//            }
            r.skip(attr_len);
        } else if (S(SourceDebugExtension) == attr_name) { // ignore
//            u1 source_debug_extension[attr_len];
//            bcr_read_bytes(reader, source_debug_extension, attr_len);
            r.skip(attr_len);
        } else if (S(RuntimeVisibleAnnotations) == attr_name) { // ignore
//            u2 runtime_annotations_num = readu2(reader);
//            struct annotation annotations[runtime_annotations_num];
//            for (u2 k = 0; k < runtime_annotations_num; k++) {
//                read_annotation(reader, annotations + i);
//            }
            r.skip(attr_len);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) { // ignore
//            u2 runtime_annotations_num = readu2(reader);
//            struct annotation annotations[runtime_annotations_num];
//            for (u2 k = 0; k < runtime_annotations_num; k++) {
//                read_annotation(reader, annotations + i);
//            }
            r.skip(attr_len);
        }
        else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

void Class::createVtable()
{
    assert(vtable.empty());

    if (superClass == nullptr) {
        assert(strcmp(className, "java/lang/Object") == 0);
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
    vtable.insert(vtable.end(), superClass->vtable.begin(), superClass->vtable.end());

    for (auto m : methods) {
        if (m->isVirtual()) {
            auto iter = find_if(vtable.begin(), vtable.end(), [=](Method *m0){
                return utf8_equals(m->name, m0->name) && utf8_equals(m->descriptor, m0->descriptor); });
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

const void Class::genPkgName()
{
    char *tmp = strdup(className);
    char *p = strrchr(tmp, '/');
    if (p == nullptr) {
        free(tmp);
        pkgName = ""; // 包名可以为空
    } else {
        *p = 0; // 得到包名
        auto hashed = find_saved_utf8(tmp);
        if (hashed != nullptr) {
            free(tmp);
            pkgName = hashed;
        } else {
            pkgName = tmp;
            save_utf8(pkgName);
        }
    }
}

Class::Class(ClassLoader *loader, const u1 *bytecode, size_t len)
{
    assert(loader != nullptr);
    assert(bytecode != nullptr);

    BytecodeReader r(bytecode, len);

    this->loader = loader;

    magic = r.readu4();
    minor_version = r.readu2();
    major_version = r.readu2();

    // init constant pool
    u2 cp_count = r.readu2();
    cp.type = new u1[cp_count];
    cp.info = new slot_t[cp_count];

    // constant pool 从 1 开始计数，第0位无效
    CP_TYPE(cp, 0) = CONSTANT_Invalid;
    for (int i = 1; i < cp_count; i++) {
        u1 tag = CP_TYPE(cp, i) = r.readu1();
        switch (tag) {
            case CONSTANT_Class:
            case CONSTANT_String:
            case CONSTANT_MethodType:
                CP_INFO(cp, i) = r.readu2();
                break;
            case CONSTANT_NameAndType:
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_Dynamic:
            case CONSTANT_InvokeDynamic: {
                u2 index1 = r.readu2(); // class_index
                u2 index2 = r.readu2(); // name_and_type_index
                CP_INFO(cp, i) = (index2 << 16) + index1;
                break;
            }
            case CONSTANT_Integer: {
                u1 bytes[4];
                r.readBytes(bytes, 4);
                CP_INT(cp, i) = bytes_to_int32(bytes);
                break;
            }
            case CONSTANT_Float: {
                u1 bytes[4];
                r.readBytes(bytes, 4);
                CP_FLOAT(cp, i) = bytes_to_float(bytes);
                break;
            }
            case CONSTANT_Long: {
                u1 bytes[8];
                r.readBytes(bytes, 8);
                CP_LONG(cp, i) = bytes_to_int64(bytes);

                i++;
                CP_TYPE(cp, i) = CONSTANT_Placeholder;
                break;
            }
            case CONSTANT_Double: {
                u1 bytes[8];
                r.readBytes(bytes, 8);
                CP_DOUBLE(cp, i) = bytes_to_double(bytes);

                i++;
                CP_TYPE(cp, i) = CONSTANT_Placeholder;
                break;
            }
            case CONSTANT_Utf8: {
                u2 utf8_len = r.readu2();
                char buf[utf8_len + 1];
                r.readBytes((u1 *) buf, utf8_len);
                buf[utf8_len] = 0;

                const char *utf8 = find_saved_utf8(buf);
                if (utf8 == nullptr) {
                    utf8 = strdup(buf);
                    utf8 = save_utf8(utf8);
                }
                CP_INFO(cp,i) = (uintptr_t) utf8;
                break;
            }
            case CONSTANT_MethodHandle: {
                u2 index1 = r.readu1(); // 这里确实是 bcr_readu1  reference_kind
                u2 index2 = r.readu2(); // reference_index
                CP_INFO(cp, i) = (index2 << 16) + index1;
                break;
            }
//            case CONSTANT_MethodType:
//                c->u.method_descriptor_index = readu2(reader);
//                break;
//            case CONSTANT_Dynamic:
//            case CONSTANT_InvokeDynamic:
//                c->u.invoke_dynamic_constant.bootstrap_method_attr_index = readu2(reader);
//                c->u.invoke_dynamic_constant.name_and_type_index = readu2(reader);
//                break;
            default:
                stringstream ss;
                ss << "bad constant tag: " << tag;
                raiseException(CLASS_FORMAT_ERROR, ss.str().c_str());
        }
    }


    accessFlags = r.readu2();

    className = CP_CLASS_NAME(cp, r.readu2());
    genPkgName();

    u2 super_class = r.readu2();
    if (super_class == 0) { // why 0
        this->superClass = nullptr; // 可以没有父类
    } else {
        this->superClass = resolve_class(this, super_class);
//        super_class = classloader_load_class(loader, CP_CLASS_NAME(cp, super_class));//rtcp_get_class_name(rtcp, super_class));
        // 从父类中拷贝继承来的field   todo 要不要从新new个field不然delete要有问题，继承过来的field的类名问题
//        for_each(superClass->instanceFields.begin(), superClass->instanceFields.end(), [](JField *f) {
//            if (!f->isPrivate()) {
//                instanceFields.push_back(new JField(*f));
//            }
//        });
    }

    // parse interfaces
    u2 interfacesCount = r.readu2();
    for (u2 i = 0; i < interfacesCount; i++)
        interfaces.push_back(resolve_class(this, r.readu2()));

    // parse fields
    u2 fieldsCount = r.readu2();
    fields.resize(fieldsCount);
    auto lastField = fieldsCount - 1;
    for (u2 i = 0; i < fieldsCount; i++) {
        auto f = new Field(this, r);
        // 保证所有的 public fields 放在前面
        if (f->isPublic())
            fields[publicFieldsCount++] = f;
        else
            fields[lastField--] = f;
    }

    calcFieldsId();

    // parse methods
    u2 methodsCount = r.readu2();
    methods.resize(methodsCount);
    auto lastMethod = methodsCount - 1;
    for (u2 i = 0; i < methodsCount; i++) {
        auto m = new Method(this, r);
        // 保证所有的 public methods 放在前面
        if (m->isPublic())
            methods[publicMethodsCount++] = m;
        else
            methods[lastMethod--] = m;
    }

    parseAttribute(r); // parse class attributes
    createVtable();
//    if (strcmp(className, "sun/util/PreHashedMap") == 0)
//        print_vtable(c);
}

Class::~Class()
{
    delete cp.type;
    delete cp.info;
    // todo
}

void Class::clinit()
{
    if (inited) {
        return;
    }

    if (superClass != nullptr && !superClass->inited) {
        superClass->clinit();
    }

    // 在这里先行 set inited true, 如不这样，后面执行<clinit>时，
    // 可能调用putstatic等函数也会触发<clinit>的调用造成死循环。
    inited = true;

    Method *method = getDeclaredMethod(S(class_init), S(___V));
    if (method != nullptr) { // 有的类没有<clinit>方法
        if (!method->isStatic()) {
            // todo error
            printvm("error\n");
        }

        execJavaFunc(method);
    }
}

Field *Class::lookupField(const char *name, const char *descriptor)
{
    for (auto f : fields) {
        if (utf8_equals(f->name, name) && utf8_equals(f->descriptor, descriptor))
            return f;
    }

    // todo 在父类中查找
    struct Field *field;
    if (superClass != nullptr) {
        if ((field = superClass->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    // todo 在父接口中查找
    for (auto c : interfaces) {
        if ((field = c->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    stringstream ss;
    ss << className << '~' << name << '~' << descriptor;
    raiseException(NO_SUCH_FIELD_ERROR, ss.str().c_str());
}

Field *Class::lookupStaticField(const char *name, const char *descriptor)
{
    Field *field = lookupField(name, descriptor);
    // todo Field == nullptr
    if (!field->isStatic()) {
        raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
    }
    return field;
}

Field *Class::lookupInstField(const char *name, const char *descriptor)
{
    Field* field = lookupField(name, descriptor);
    // todo Field == nullptr
    if (field->isStatic()) {
        raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
    }
    return field;
}

Method *Class::getDeclaredMethod(const char *name, const char *descriptor)
{
    for (auto m : methods) {
        if (utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    return nullptr;

}

Method *Class::getDeclaredStaticMethod(const char *name, const char *descriptor)
{
    for (auto m : methods) {
        if (m->isStatic() && utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    return nullptr;
}

Method *Class::getDeclaredInstMethod(const char *name, const char *descriptor)
{
    for (auto m : methods) {
        if (!m->isStatic() && utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    return nullptr;
}

vector<Method *> Class::getDeclaredMethods(const char *name, bool public_only)
{
    assert(name != nullptr);
    vector<Method *> declaredMethods;

    for (auto m : methods) {
        if ((!public_only || m->isPublic()) && (utf8_equals(m->name, name)))
            declaredMethods.push_back(m);
    }

    return declaredMethods;
}

Method *Class::getConstructor(const char *descriptor)
{
    return getDeclaredMethod(S(object_init), descriptor);
}

vector<Method *> Class::getConstructors(bool public_only)
{
    return getDeclaredMethods(S(object_init), public_only);
}

Method *Class::lookupMethod(const char *name, const char *descriptor)
{
    Method *method = getDeclaredMethod(name, descriptor);
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

    stringstream ss;
    ss << className << '~' << name << '~' << descriptor;
    raiseException(NO_SUCH_METHOD_ERROR, ss.str().c_str());
}

Method *Class::lookupStaticMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    if (!m->isStatic()) {
        raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
    }
    return m;
}

Method *Class::lookupInstMethod(const char *name, const char *descriptor)
{
    Method *m = lookupMethod(name, descriptor);
    // todo m == nullptr
    if (m->isStatic()) {
        raiseException(INCOMPATIBLE_CLASS_CHANGE_ERROR);
    }
    return m;
}

bool Class::isSubclassOf(const Class *father) const
{
    if (this == father)
        return true;

    if (superClass != nullptr && superClass->isSubclassOf(father))
        return true;

    for (auto c : interfaces) {
        if (c->isSubclassOf(father))
            return true;
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

void Class::setStaticFieldValue(Field *f, const slot_t *value)
{
    assert(f != nullptr && value != nullptr);

    staticFieldsValues[f->id] =  value[0];
    if (f->categoryTwo) {
        staticFieldsValues[f->id + 1] = value[1];
    }
}

const slot_t* Class::getStaticFieldValue(const Field *f)
{
    return staticFieldsValues + f->id;
}

bool Class::isArray() const
{
    return className[0] == '[';
}

ArrayClass *Class::arrayClass() const
{
    char arrayClassName[strlen(className) + 8]; // big enough

    // 数组
    if (className[0] == '[') {
        sprintf(arrayClassName, "[%s", className);
        return loadArrayClass(arrayClassName); // todo
    }

    // 基本类型
    const char *tmp = primitiveClassName2arrayClassName(className);
    if (tmp != nullptr)
        return loadArrayClass(tmp); // todo

    // 类引用
    sprintf(arrayClassName, "[L%s;", className);
    return loadArrayClass(arrayClassName); // todo
}

bool Class::isAccessibleTo(const Class *visitor) const
{
    // todo 实现对不对
    assert(visitor != nullptr); // todo

    if (this == visitor || isPublic())  // todo 对不对
        return true;

    if (isPrivate())
        return false;

    // 字段是 protected，则只有 子类 和 同一个包下的类 可以访问
    if (isProtected()) {
        return visitor->isSubclassOf(this) || utf8_equals(pkgName, visitor->pkgName);
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return utf8_equals(pkgName, visitor->pkgName);
}

string Class::toString() const
{
    string s = "class: ";
    s += className;
    return s;
}

