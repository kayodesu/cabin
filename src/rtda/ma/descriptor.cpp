/*
 * Author: Jia Yang
 */

#include <sstream>
#include "descriptor.h"
#include "../heap/Object.h"
#include "../heap/ArrayObject.h"
#include "../heap/ClassObject.h"
#include "../../symbol.h"

using namespace std;

// 解析单个 type to descriptor。
static void type_to_descriptor(const ClassObject *type, ostringstream &os)
{
    assert(type != nullptr);

    Class *c = type->entityClass;
    char d = primitiveClassName2descriptor(c->className);
    if (d != 0) { // primitive type
        os << d;
    } else if (c->isArray()) { // 数组
        os << c->className;
    } else { // 普通类
        os << 'L' << c->className << ';';
    }
}

string typesToDescriptor(const Object *types)
{
    assert(types != nullptr);

    ostringstream os;

//    char *descriptor = vm_malloc(sizeof(char)*(DESCRIPTOR_MAX_LEN + 1));

    if (types->isArray()) {
        auto ao = (ArrayObject *) types;
//        int i = 0;
//        int arr_len = ao->len;
        for (int t = 0; t < ao->len; t++) {
            type_to_descriptor(ao->get<ClassObject *>(t), os);
//            Object *type = arrobj_get(Object *, types, t);
//            int len = DESCRIPTOR_MAX_LEN - i;
//            type_to_descriptor(type, descriptor + i, &len);
//            i += len;
        }

    } else {
        // todo 判断 types 是不是 ClassObject *
        type_to_descriptor((ClassObject *) types, os);
    }

    return os.str();
}

const char *descriptorToClassName(const char *descriptor, char *buf)
{
    assert(descriptor != nullptr);
    if (*descriptor == '[') { // array
        return descriptor;
    } else if (*descriptor == 'L') { // non array Object
        strcpy(buf, ++descriptor);
        buf[strlen(buf) - 1] = 0; // set last ';' is 0 to end string
        return buf;
    } else { // primitive
        return primitiveDescriptor2className(*descriptor);
    }
}

ArrayObject *methodDescriptorToParameterTypes(ClassLoader *loader, const char *method_descriptor)
{
    assert(loader != nullptr);
    assert(method_descriptor != nullptr);

    int dlen = strlen(method_descriptor);
    char descriptor[dlen + 1];
    strcpy(descriptor, method_descriptor);

    Object* buf[METHOD_PARAMETERS_MAX_COUNT];

    int parameter_types_count = 0;

    char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == nullptr || e == nullptr) {
        VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
        return nullptr;
    }

    // parameter types
    while (++b < e) {
        if (*b == 'L') { // reference
            char *t = strchr(b, ';');
            if (t == nullptr) {
                VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
                return nullptr;
            }

            *t = 0;   // end string
            buf[parameter_types_count++] = loader->loadClass(b + 1 /* jump 'L' */)->clsobj;
            *t = ';'; // recover
            b = t;
        } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
            char *t = b;
            while (*(++t) == '[');
            if (!isPrimitiveDescriptor(*t)) {
                t = strchr(t, ';');
                if (t == nullptr) {
                    VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
                    return nullptr;
                }
            }

            char k = *(++t);
            *t = 0; // end string
            buf[parameter_types_count++] = loader->loadClass(b)->clsobj;
            *t = k; // recover
            b = t;
        } else if (isPrimitiveDescriptor(*b)) {
            const char *class_name = primitiveDescriptor2className(*b);
            buf[parameter_types_count++] = loader->loadClass(class_name)->clsobj;
        } else {
            VM_UNKNOWN_ERROR("descriptor error %s", descriptor);
            return nullptr;
        }
    }

    // todo parameter_types_count == 0 是不是要填一个 void.class

    auto parameter_types = ArrayObject::newInst(loadArrayClass(S(array_java_lang_Class)), parameter_types_count);
    for (int i = 0; i < parameter_types_count; i++) {
//        arrobj_set(Object *, parameter_types, i, buf[i]);
        parameter_types->set(i, buf[i]);
    }
    return parameter_types;
}

ClassObject *descriptorToType(ClassLoader *loader, const char *descriptor)
{
    assert(loader != nullptr);
    assert(descriptor != nullptr);

    char buf[strlen(descriptor) + 1];
    return loader->loadClass(descriptorToClassName(descriptor, buf))->clsobj;
}

ClassObject *methodDescriptorToTeturnType(ClassLoader *loader, const char *method_descriptor)
{
    assert(loader != nullptr);
    assert(method_descriptor != nullptr);

    const char *e = strchr(method_descriptor, ')');
    if (e == nullptr) {
        VM_UNKNOWN_ERROR("descriptor error. %s", method_descriptor);
        return nullptr;
    }

    return descriptorToType(loader, ++e);
}
