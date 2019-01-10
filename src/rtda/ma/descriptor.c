/*
 * Author: Jia Yang
 */

#include "descriptor.h"
#include "../heap/object.h"
#include "../heap/arrobj.h"
#include "../heap/clsobj.h"

char* type_to_descriptor(const struct object *type, char *descriptor, int *len)
{
    assert(type != NULL);
    assert(descriptor != NULL);
    assert(len != NULL);
    assert(object_is_jlclass(type)); // must be object of java/lang/Class

    struct class *c = type->u.entity_class;
    char d = pt_get_descriptor_by_class_name(c->class_name);
    if (d != 0) { // primitive type
        descriptor[0] = d;
        *len = 1;
        return descriptor;
    }

    int desc_len = strlen(c->class_name);
    if (class_is_array(c)) { // 数组
        if (desc_len > *len) {
            jvm_abort("长度不够"); // todo
            return NULL;
        }
        strcpy(descriptor, c->class_name);
    } else { // 普通类
        desc_len += 2; // plus 2 for 'L' and ';'
        if (desc_len > *len) {
            jvm_abort("长度不够"); // todo
            return NULL;
        }
        descriptor[0] = 'L';
        strcpy(descriptor + 1, c->class_name);
        descriptor[desc_len - 1] = ';';
        descriptor[desc_len] = 0;
    }

    *len = desc_len;
    return descriptor;
}

struct object *descriptor_to_type(struct classloader *loader, const char *descriptor)
{
    assert(loader != NULL);
    assert(descriptor != NULL);

    char buf[strlen(descriptor) + 1];
    return classloader_load_class(loader, descriptor_to_class_name(descriptor, buf))->clsobj;
}

const char* types_to_descriptor(const struct object *types)
{
    assert(types != NULL);
    VM_MALLOCS(char, DESCRIPTOR_MAX_LEN + 1, descriptor);

    if (object_is_array(types)) {
        // must be array object of java/lang/Class
        assert(strcmp(types->clazz->class_name, "[Ljava/lang/Class;") == 0);

        int i = 0;
        int arr_len = arrobj_len(types);
        for (int t = 0; t < arr_len; t++) {
            struct object *type = arrobj_get(struct object *, types, t);
            int len = DESCRIPTOR_MAX_LEN - i;
            type_to_descriptor(type, descriptor + i, &len);
            i += len;
        }

        return descriptor;
    } else {
        int len = DESCRIPTOR_MAX_LEN;
        return type_to_descriptor(types, descriptor, &len);
    }
}

const char* descriptor_to_class_name(const char *descriptor, char *buf)
{
    assert(descriptor != NULL);
    if (*descriptor == '[') { // array
        return descriptor;
    } else if (*descriptor == 'L') { // non array object
        strcpy(buf, ++descriptor);
        buf[strlen(buf) - 1] = 0; // set last ';' is 0 to end string
        return buf;
    } else { // primitive
        return pt_get_class_name_by_descriptor(*descriptor);
    }
}

struct object* method_descriptor_to_parameter_types(struct classloader *loader, const char *method_descriptor)
{
    assert(loader != NULL);
    assert(method_descriptor != NULL);

    int dlen = strlen(method_descriptor);
    char descriptor[dlen + 1];
    strcpy(descriptor, method_descriptor);

    struct object* buf[METHOD_PARAMETERS_MAX_COUNT];

    int parameter_types_count = 0;

    char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == NULL || e == NULL) {
        VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
        return NULL;
    }

    // parameter types
    while (++b < e) {
        if (*b == 'L') { // reference
            char *t = strchr(b, ';');
            if (t == NULL) {
                VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
                return NULL;
            }

            *t = 0;   // end string
            buf[parameter_types_count++] = classloader_load_class(loader, b + 1 /* jump 'L' */)->clsobj;
            *t = ';'; // recover
            b = t;
        } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
            char *t = b;
            while (*(++t) == '[');
            if (!pt_is_primitive_descriptor(*t)) {
                t = strchr(t, ';');
                if (t == NULL) {
                    VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
                    return NULL;
                }
            }

            char k = *(++t);
            *t = 0; // end string
            buf[parameter_types_count++] = classloader_load_class(loader, b)->clsobj;
            *t = k; // recover
            b = t;
        } else if (pt_is_primitive_descriptor(*b)) {
            const char *class_name = pt_get_class_name_by_descriptor(*b);
            buf[parameter_types_count++] = classloader_load_class(loader, class_name)->clsobj;
        } else {
            VM_UNKNOWN_ERROR("descriptor error %s", descriptor);
            return NULL;
        }
    }

    // todo parameter_types_count == 0 是不是要填一个 void.class

    struct object *parameter_types
            = arrobj_create(classloader_load_class(loader, "[Ljava/lang/Class;"), parameter_types_count);
    for (int i = 0; i < parameter_types_count; i++) {
        arrobj_set(struct object *, parameter_types, i, buf[i]);
    }
    return parameter_types;
}

struct object* method_descriptor_to_return_type(struct classloader *loader, const char *method_descriptor)
{
    assert(loader != NULL);
    assert(method_descriptor != NULL);

    const char *e = strchr(method_descriptor, ')');
    if (e == NULL) {
        VM_UNKNOWN_ERROR("descriptor error. %s", method_descriptor);
        return NULL;
    }

    return descriptor_to_type(loader, ++e);
}
