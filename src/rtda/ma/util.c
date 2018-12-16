#include "../heap/jobject.h"

/*
 * Author: Jia Yang
 */

/*
 * todo  descriptor 的结构
 */
#define DESCRIPTOR_MAX_LEN 1023

/*
 * 解析单个 type to descriptor。
 *
 * @len，即是输入参数又是输出参数。
 */
static char* type_to_descriptor(const struct jobject *type, char *descriptor, int *len)
{
    assert(type != NULL);
    assert(descriptor != NULL);
    assert(len != NULL);
    assert(jobject_is_jlclass(type)); // must be object of java/lang/Class

    int i = 0;
    struct jclass *c = jclsobj_entity_class(type);
    char d = pt_get_descriptor_by_class_name(c->class_name);
    if (d != 0) { // primitive type
        descriptor[i++] = d;
        if (i > DESCRIPTOR_MAX_LEN) {
            jvm_abort("超过本虚拟机的descriptor最大长度，不支持\n"); // todo
            return NULL;
        }
        return descriptor;
    }

    int tmp = strlen(c->class_name);
    if (i + tmp + 2 > DESCRIPTOR_MAX_LEN) {  // plus 2 for 'L' and ';'
        jvm_abort("超过本虚拟机的descriptor最大长度，不支持\n"); // todo
        return NULL;
    }
    descriptor[i++] = 'L';
    strcpy(descriptor + 1, c->class_name);
    i += tmp;
    descriptor[i++] = ';';
    descriptor[i] = 0;
    *len = i;
    return descriptor;
}

const char* types_to_descriptor(const struct jobject *types)
{
    assert(types != NULL);
    VM_MALLOCS(char, DESCRIPTOR_MAX_LEN + 1, descriptor);

    if (jobject_is_array(types)) {
        // must be array object of java/lang/Class
        assert(strcmp(types->jclass->class_name, "[Ljava/lang/Class;") == 0);

        int i = 0;
        int arr_len = jarrobj_len(types);
        for (int t = 0; t < arr_len; t++) {
            struct jobject *type = jarrobj_get(struct jobject *, types, t);
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
