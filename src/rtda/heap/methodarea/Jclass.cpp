/*
 * Author: Jia Yang
 */

#include <string>
#include <algorithm>

#include "Jclass.h"
#include "Jmethod.h"
#include "Jfield.h"
#include "../../../native/registry.h"
#include "../../../classfile/attribute/SourceFileAttr.h"

using namespace std;


// 计算实例字段的个数，同时给它们编号
void Jclass::calcInstanceFieldId() {
    int id = 0;

    if (superClass != nullptr)
        id = superClass->instanceFieldCount;

    for (int i = 0; i < fieldsCount; i++) {
        if (!fields[i].isStatic()) {
            fields[i].id = id++;
//            if (filed is long or double) id++;  todo 需要这个吗
        }
    }
    instanceFieldCount = id;
}

// 计算静态字段的个数，同时给它们编号
void Jclass::calcStaticFieldId() {
    int id = 0;
    for (int i = 0; i < fieldsCount; i++) {
        if (fields[i].isStatic()) {
            fields[i].id = id++;
//            if (filed is long or double) id++;  todo 需要这个吗
        }
    }
    staticFieldCount = id;
}

Jclass::Jclass(ClassLoader *loader, ClassFile *cf): sourcefileName("unknown") {
    magic = cf->magic;
    minorVersion = cf->minorVersion;
    majorVersion = cf->majorVersion;
    accessFlags = cf->accessFlags;
    isInited = false;
    this->loader = loader;

    constantsCount = cf->constantPoolCount;
//    buildRtcp(cf->constant_pool, cf->constant_pool_count);
    rtcp = new RTCP(cf->constantPool, cf->constantPoolCount);

//    class_name = get_rtc_class(class, cf->this_class);//RTCP[cf->this_class].u.class_name;
    className = rtcp->getClassName(cf->thisClass);//RTCP[cf->this_class]->getValue();
    unsigned long index = className.rfind('/');//strrchr(className, '/');
    if (index == -1) { // todo 是 -1 吗
        pkgName = ""; // 包名可以为空
    } else {
        pkgName = className.substr(0, index);
    }

//    print_rtcp(class);  // todo

    if (cf->superClass == 0) {
        superClass = nullptr; // 可以没有父类
    } else {
        superClass = loader->loadClass(rtcp->getClassName(cf->superClass));
        // 从父类中拷贝继承来的field   todo 要不要从新new个field不然delete要有问题，继承过来的field的类名问题
//        for_each(superClass->instanceFields.begin(), superClass->instanceFields.end(), [](Jfield *f) {
//            if (!f->isPrivate()) {
//                instanceFields.push_back(new Jfield(*f));
//            }
//        });
    }

    interfacesCount = cf->interfacesCount;
    interfaces = new Jclass*[interfacesCount];
    for (int i = 0; i < cf->interfacesCount; i++) {
        string interfaceName = rtcp->getClassName(cf->interfaces[i]);
        if (interfaceName.empty()) {
            // todo error
            jprintf("error\n");
        } else {
            interfaces[i] = loader->loadClass(interfaceName);
        }
    }

    fieldsCount = cf->fieldsCount;
    fields = new Jfield[fieldsCount];
    for (int i = 0; i < fieldsCount; i++) {
        new (fields + i)Jfield(this, cf->fields[i]);
    }

    calcStaticFieldId();
    calcInstanceFieldId();

//    /*
//     * Java要求类成员变量必须用默认值初始化！
//     *
//     * Java的类成员变量默认值
//     * boolean: false
//     * byte: 0
//     * short: 0
//     * char:'\u0000' 数值为0 ???? todo
//     * int: 0
//     * long: 0
//     * float: 0.0
//     * double: 0.0
//     * 数组: null
//     * 引用: null
//     *
//     * 如果静态变量属于基本类型或String类型，有final修饰符，且它的值在编译期已知，则该值存储在class文件常量池中。
//     */
//    staticFieldValues = new Jvalue[staticFieldCount]();
////    memset(staticFieldValues, 0, sizeof(Jvalue) * staticFieldCount);
//
//    for (int i = 0; i < fieldsCount; i++) {
//        if (fields[i].extra != nullptr) {
//            memcpy(staticFieldValues + i, fields[i].extra, sizeof(Jvalue));
//            delete fields[i].extra;
//        }
//    }

    methodsCount = cf->methodsCount;
    methods = new Jmethod[methodsCount];//malloc(sizeof(*methods) * methods_count);
    for (int i = 0; i < cf->methodsCount; i++) {
        new (methods + i)Jmethod(this, cf->methods[i]);
    }

    for (int i = 0; i < cf->attributesCount; i++) {
        Attribute *attr = cf->attributes[i];
        string attrName = rtcp->getStr(attr->attributeNameIndex);

        // todo class attributes
        if (attrName == InnerClasses) {
     //       inner_classes_attr *a = attr;
//            jprintf("not parse attr: InnerClasses\n");
        } else if (attrName == EnclosingMethod) {  // 可选属性
//            jprintf("not parse attr: EnclosingMethod\n");
        } else if (attrName == Synthetic) {
    //        SET_SYNTHETIC(access_flags);  // todo
            setSynthetic();  // todo
        } else if (attrName == Signature) {  // 可选属性
//            jprintf("not parse attr: Signature\n");
        } else if (attrName == SourceFile) {  // 可选属性
            SourceFileAttr *a = static_cast<SourceFileAttr *>(attr);
            sourcefileName = rtcp->getStr(a->sourcefileIndex);
//            jprintf("not parse attr: SourceFile\n");
        } else if (attrName == SourceDebugExtension) {  // 可选属性
//            jprintf("not parse attr: SourceDebugExtension\n");
        } else if (attrName == Deprecated) {  // 可选属性
//            jprintf("not parse attr: Deprecated\n");
        } else if (attrName == RuntimeVisibleAnnotations) {
  //          runtime_annotations_attr *a = attr;
//            jprintf("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (attrName == RuntimeInvisibleAnnotations) {
 //           runtime_annotations_attr *a = attr;
//            jprintf("not parse attr: RuntimeInvisibleAnnotations\n");
        } else if (attrName == BootstrapMethods) {
 //           bootstrap_methods_attr *a = attr;
//            jprintf("not parse attr: BootstrapMethods\n");
        }
    }
}

void Jclass::clinit(StackFrame *invokeFrame) {
    if (isInited) {
        return;
    }

    Jmethod *method = getMethod("<clinit>", "()V"); // todo 并不是每个类都有<clinit>方法？？？？？
    if (method != nullptr) {
        if (!method->isStatic()) {
            // todo error
            jprintf("error\n");
        }

        invokeFrame->invokeMethod(method);
    }

    isInited = true;

    /*
     * 超类放在最后判断，
     * 这样可以保证超类的初始化方法对应的帧在子类上面，
     * 使超类初始化方法先于子类执行。
     */
    if (superClass != nullptr) {
        superClass->clinit(invokeFrame);
    }
}

vector<Jfield> Jclass::getFields(bool publicOnly) {
    vector<Jfield> result;
    for (int i = 0; i < fieldsCount; i++) {
        if (publicOnly) {
            if (fields[i].isPublic())
                result.push_back(fields[i]);
        } else {
            result.push_back(fields[i]);
        }
    }

    return result;
}

void Jclass::setStaticField(int id, const Jvalue &v) {
    staticFieldValues[id] = v;
}

void Jclass::setStaticField(const string &name, const string &descriptor, const Jvalue &v) {
    auto field = lookupField(name, descriptor);
    if (field == nullptr) {
        // todo
        jvmAbort("");
    }
    setStaticField(field->id, v);
}

Jfield* Jclass::lookupField(const string &name, const string &descriptor) {
    for (int i = 0; i < fieldsCount; i++) {
        if (fields[i].name == name && fields[i].descriptor == descriptor) {
            return fields + i;
        }
    }

    // todo 在父类中查找
    Jfield *field;
    if (superClass != nullptr) {
        if ((field = superClass->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    // todo 在父接口中查找
    for (int i = 0; i < interfacesCount; i++) {
        if ((field = interfaces[i]->lookupField(name, descriptor)) != nullptr)
            return field;
    }

    // java.lang.NoSuchFieldError  todo
    jprintf("java.lang.NoSuchFieldError. %s, %s, %s\n", this->className.c_str(), name.c_str(), descriptor.c_str());
    return nullptr;
}

Jmethod* Jclass::getMethod(const string &name, const string &descriptor) {
    for (int i = 0; i < methodsCount; i++) {
        if (methods[i].name == name && methods[i].descriptor == descriptor) {
            return methods + i;
        }
    }
    return nullptr;
}

Jmethod* Jclass::getConstructor(const std::string &descriptor) {
    return getMethod("<init>", descriptor);
}

Jmethod* Jclass::lookupMethod(const string &name, const string &descriptor) {
    Jmethod *method = getMethod(name, descriptor);
    if (method != nullptr) {
        return method;
    }

    // todo 在父类中查找
    if (superClass != nullptr) {
        if ((method = superClass->lookupMethod(name, descriptor)) != nullptr)
            return method;
    }

    // todo 在父接口中查找
    for (int i = 0; i < interfacesCount; i++) {
        if ((method = interfaces[i]->lookupMethod(name, descriptor)) != nullptr)
            return method;
    }

    // todo java.lang.NoSuchMethodError
//    jvmAbort("java.lang.NoSuchMethodError. %s, %s, %s\n", this->className.c_str(), method_name.c_str(), method_descriptor.c_str());
    return nullptr;
}

Jmethod* Jclass::lookupStaticMethod(const string &name, const string &descriptor) {
    auto m = lookupMethod(name, descriptor);
    // todo m == nullptr
    if (!m->isStatic()) {
        // todo java.lang.IncompatibleClassChangeError
        jvmAbort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

Jmethod* Jclass::lookupInstanceMethod(const std::string &name, const std::string &descriptor) {
    auto m = lookupMethod(name, descriptor);
    // todo m == nullptr
    if (m->isStatic()) {
        // todo java.lang.IncompatibleClassChangeError
        jvmAbort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

bool Jclass::isSubclassOf(const Jclass *father) const {
    if (this == father)
        return true;

    if (superClass != nullptr && superClass->isSubclassOf(father))
        return true;

    for (int i = 0; i < interfacesCount; i++) {
        if (this->interfaces[i]->isSubclassOf(father))
            return true;
    }

    return false;
}

Jclass* Jclass::arrayClass() {
    // todo 好像不对，没有考虑基本类型的类
    return loader->loadClass("[L" + className + ";");
}

//void print_rtcp(const jclass *class)
//{
//    printf("\n");
//    jprintf("The RTCP of class %s(%d)\n", class->class_name, class->constants_count);
//    for (int i = 1; i < class->constants_count; i++) {
//        rtc c = class->RTCP[i];
//        if (c.tag == INTEGER_CONSTANT) {
//            jprintf("INTEGER_CONSTANT. %d\n", c.u.i);
//        } else if (c.tag == LONG_CONSTANT) {
//            jprintf("LONG_CONSTANT. %ld\n", c.u.l);
//        } else if (c.tag == FLOAT_CONSTANT) {
//            jprintf("FLOAT_CONSTANT. %f\n", c.u.f);
//        } else if (c.tag == DOUBLE_CONSTANT) {
//            jprintf("DOUBLE_CONSTANT. %f\n", c.u.d);
//        } else if (c.tag == UTF8_CONSTANT) {
//            jprintf("UTF8_CONSTANT. %s\n", c.u.str);
//        } else if (c.tag == STRING_CONSTANT) {
//            jprintf("STRING_CONSTANT. %s\n", c.u.str);
//        } else if (c.tag == NAME_AND_TYPE_CONSTANT) {
//            jprintf("NAME_AND_TYPE_CONSTANT. %s | %s\n", c.u.name_and_type.name, c.u.name_and_type.descriptor);
//        } else if (c.tag == FIELD_REF_CONSTANT) {
//            const jfield_ref *ref = &c.u.field_ref;
//            jprintf("FIELD_REF_CONSTANT. class_name = %s. field_name = %s. descriptor = %s\n", ref->class_name, ref->name, ref->descriptor);
//        } else if (c.tag == METHOD_REF_CONSTANT) {
//            const jmethod_ref *ref = &c.u.method_ref;
//            jprintf("METHOD_REF_CONSTANT. class_name = %s. field_name = %s. descriptor = %s\n", ref->class_name, ref->name, ref->descriptor);
//        } else if (c.tag == CLASS_CONSTANT) {
//            jprintf("CLASS_CONSTANT. %s\n",c.u.class_name);
//        } else {
//            jprintf("error. unknown tag = %d\n", c.tag);
//        }
//    }
//    printf("\n");
//}