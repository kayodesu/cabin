/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <string.h>
#include "class.h"
#include "access.h"
#include "field.h"
#include "../heap/object.h"
#include "../../util/util.h"
#include "../../classfile/constant.h"


// 计算实例字段的个数，同时给它们编号
static void calc_instance_field_id(struct class *c)
{
    assert(c != NULL);

    int id = 0;
    if (c->super_class != NULL) {
        id = c->super_class->instance_fields_count; // todo 父类的私有变量是不是也算在了里面，不过问题不大，浪费点空间吧了
        assert(id >= 0);
    }

    for (int i = 0; i < c->fields_count; i++) {
        if (!IS_STATIC(c->fields[i].access_flags))  {
            c->fields[i].id = id++;
            if (c->fields[i].category_two) {
                id++;
            }
        }
    }

    assert(id >= 0);
    c->instance_fields_count = id;

    VM_MALLOCS(struct slot, c->instance_fields_count, values);
    c->inited_instance_fields_values = values;

    // 将父类中的变量拷贝过来
    if (c->super_class != NULL) {
        memcpy(values, c->super_class->inited_instance_fields_values,
               c->super_class->instance_fields_count * sizeof(struct slot));
    }
    // 初始化本类中的实例变量
    for (int i = 0; i < c->fields_count; i++) {
        struct field *field = c->fields + i;
        if (!IS_STATIC(field->access_flags)) {
            assert(field->id < c->instance_fields_count);
            switch (field->descriptor[0]) {
                case 'B':
                case 'C':
                case 'I':
                case 'S':
                case 'Z':
                    values[field->id] = islot(0);
                    break;
                case 'F':
                    values[field->id] = fslot(0.0f);
                    break;
                case 'J':
                    values[field->id] = lslot(0L);
                    values[field->id + 1] = phslot;
                    break;
                case 'D':
                    values[field->id] = dslot(0.0);
                    values[field->id + 1] = phslot;
                    break;
                default:
                    values[field->id] = rslot(NULL);
                    break;
            }
        }
    }

    // todo 保证 values 的每一项都被初始化了
}

// 计算静态字段的个数，同时给它们编号
static void calc_static_field_id(struct class *c)
{
    assert(c != NULL);

    int id = 0;
    for (int i = 0; i < c->fields_count; i++) {
        if (IS_STATIC(c->fields[i].access_flags)) {
            c->fields[i].id = id++;
            if (c->fields[i].category_two) {
                id++;
            }
        }
    }
    c->static_fields_count = id;

    VM_MALLOCS(struct slot, c->static_fields_count, values);
    c->static_fields_values = values;
    // 初始化本类中的静态变量
    for (int i = 0; i < c->fields_count; i++) {
        struct field *field = c->fields + i;
        if (IS_STATIC(field->access_flags)) {
            assert(field->id < c->static_fields_count);
            switch (field->descriptor[0]) {
                case 'B':
                case 'C':
                case 'I':
                case 'S':
                case 'Z':
                    values[field->id] = islot(0);
                    break;
                case 'F':
                    values[field->id] = fslot(0.0f);
                    break;
                case 'J':
                    values[field->id] = lslot(0L);
                    values[field->id + 1] = phslot;
                    break;
                case 'D':
                    values[field->id] = dslot(0.0);
                    values[field->id + 1] = phslot;
                    break;
                default:
                    values[field->id] = rslot(NULL);
                    break;
            }
        }
    }

    // todo 保证 values 的每一项都被初始化了
}

static void read_constant_pool(struct constant pool[], u2 count, struct bytecode_reader *reader)
{
    // constant pool 从 1 开始计数，第0位无效
    pool[0] = (struct constant) { .tag = INVALID_CONSTANT };
    for (int i = 1; i < count; i++) {
        struct constant *c = pool + i;

        c->tag = bcr_readu1(reader);
        switch (c->tag) {
            case CLASS_CONSTANT:
                c->u.class_name_index = bcr_readu2(reader);
                break;
            case FIELD_REF_CONSTANT:
            case METHOD_REF_CONSTANT:
            case INTERFACE_METHOD_REF_CONSTANT:
                c->u.ref_constant.class_index = bcr_readu2(reader);
                c->u.ref_constant.name_and_type_index = bcr_readu2(reader);
                break;
            case STRING_CONSTANT:
                c->u.string_index = bcr_readu2(reader);
                break;
            case INTEGER_CONSTANT:
            case FLOAT_CONSTANT:
                bcr_read_bytes(reader, c->u.bytes4, 4);
                break;
            case LONG_CONSTANT:
            case DOUBLE_CONSTANT:
                bcr_read_bytes(reader, c->u.bytes8, 8);
                // 在Class文件的常量池中，所有的8字节的常量都占两个表成员（项）的空间。
                // 如果一个 CONSTANT_Long_info 或 CONSTANT_Double_info 结构的项在常量池中的索引为 n，
                // 则常量池中下一个有效的项的索引为 n + 2，此时常量池中索引为n+1的项有效但必须被认为不可用
                i++;
                assert(i < count);
                pool[i] = (struct constant) { .tag = PLACEHOLDER_CONSTANT };
                break;
            case NAME_AND_TYPE_CONSTANT:
                c->u.name_and_type_constant.name_index = bcr_readu2(reader);
                c->u.name_and_type_constant.descriptor_index = bcr_readu2(reader);
                break;
            case UTF8_CONSTANT:
                c->u.utf8_constant.length = bcr_readu2(reader);
                c->u.utf8_constant.bytes = malloc(c->u.utf8_constant.length);
                CHECK_MALLOC_RESULT(c->u.utf8_constant.bytes);
                bcr_read_bytes(reader, c->u.utf8_constant.bytes, c->u.utf8_constant.length);
                break;
            case METHOD_HANDLE_CONSTANT:
                c->u.method_handle_constant.reference_kind = bcr_readu1(reader);
                c->u.method_handle_constant.reference_index = bcr_readu2(reader);
                break;
            case METHOD_TYPE_CONSTANT:
                c->u.method_descriptor_index = bcr_readu2(reader);
                break;
            case INVOKE_DYNAMIC_CONSTANT:
                c->u.invoke_dynamic_constant.bootstrap_method_attr_index = bcr_readu2(reader);
                c->u.invoke_dynamic_constant.name_and_type_index = bcr_readu2(reader);
                break;
            default:
                jvm_abort("error. unknown constant tag: %d\n", c->tag);
        }
    }
}

static void parse_attribute(struct class *c, struct bytecode_reader *reader)
{
    u2 attr_count = bcr_readu2(reader);

    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = rtcp_get_str(c->rtcp, bcr_readu2(reader));
        u4 attr_len = bcr_readu4(reader);

        if (strcmp(Signature, attr_name) == 0) {
            c->signature = rtcp_get_str(c->rtcp, bcr_readu2(reader));
        } else if (strcmp(Synthetic, attr_name) == 0) {
            set_synthetic(&c->access_flags);
        } else if (strcmp(Deprecated, attr_name) == 0) {
            c->deprecated = true;
        } else if (strcmp(SourceFile, attr_name) == 0) {
            u2 source_file_index = bcr_readu2(reader);
            if (source_file_index >= 0) {
                c->source_file_name = rtcp_get_str(c->rtcp, source_file_index);
            } else {
                /*
                 * 并不是每个class文件中都有源文件信息，这个因编译时的编译器选项而异。
                 * todo 什么编译选项
                 */
                c->source_file_name = "Unknown source file";
            }
        } else if (strcmp(EnclosingMethod, attr_name) == 0) {
            u2 enclosing_class_index = bcr_readu2(reader);
            u2 enclosing_method_index = bcr_readu2(reader);

            c->enclosing_info[0] = c->enclosing_info[1] = c->enclosing_info[2] = NULL;
            if (enclosing_class_index > 0) {
                struct class *enclosing_class
                        = classloader_load_class(c->loader, rtcp_get_class_name(c->rtcp, enclosing_class_index));
                c->enclosing_info[0] = jclsobj_create(enclosing_class);

                if (enclosing_method_index > 0) {
                    const struct name_and_type *nt = rtcp_get_name_and_type(c->rtcp, enclosing_method_index);
                    c->enclosing_info[1] = strobj_create(nt->name);
                    c->enclosing_info[2] = strobj_create(nt->descriptor);
                }
            }
        } else if (strcmp(BootstrapMethods, attr_name) == 0) {
            u2 num = bcr_readu2(reader);
            struct bootstrap_method methods[num];

            for (u2 k = 0; k < num; k++) {
                methods[k].bootstrap_method_ref = bcr_readu2(reader);
                methods[k].num_bootstrap_arguments = bcr_readu2(reader);
                methods[k].bootstrap_arguments = malloc(sizeof(u2) * methods[k].num_bootstrap_arguments); // todo 没有 free
                CHECK_MALLOC_RESULT(methods[k].bootstrap_arguments);
                for (int j = 0; j < methods[k].num_bootstrap_arguments; j++) {
                    methods[k].bootstrap_arguments[j] = bcr_readu2(reader);
                }
            }

            rtcp_build_invoke_dynamic_constant(c->rtcp, methods);
        } else if (strcmp(InnerClasses, attr_name) == 0) { // ignore
//            u2 num = bcr_readu2(reader);
//            struct inner_class classes[num];
//            for (u2 k = 0; k < num; k++) {
//                classes[k].inner_class_info_index = bcr_readu2(reader);
//                classes[k].outer_class_info_index = bcr_readu2(reader);
//                classes[k].inner_name_index = bcr_readu2(reader);
//                classes[k].inner_class_access_flags = bcr_readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else if (strcmp(SourceDebugExtension, attr_name) == 0) { // ignore
//            u1 source_debug_extension[attr_len];
//            bcr_read_bytes(reader, source_debug_extension, attr_len);
            bcr_skip(reader, attr_len);
        } else if (strcmp(RuntimeVisibleAnnotations, attr_name) == 0) { // ignore
//            u2 runtime_annotations_num = bcr_readu2(reader);
//            struct annotation annotations[runtime_annotations_num];
//            for (u2 k = 0; k < runtime_annotations_num; k++) {
//                read_annotation(reader, annotations + i);
//            }
            bcr_skip(reader, attr_len);
        } else if (strcmp(RuntimeInvisibleAnnotations, attr_name) == 0) { // ignore
//            u2 runtime_annotations_num = bcr_readu2(reader);
//            struct annotation annotations[runtime_annotations_num];
//            for (u2 k = 0; k < runtime_annotations_num; k++) {
//                read_annotation(reader, annotations + i);
//            }
            bcr_skip(reader, attr_len);
        }
        else {
            // unknown attribute
            bcr_skip(reader, attr_len);
        }
    }
}

struct class *jclass_create(struct classloader *loader, u1 *bytecode, size_t len)
{
    assert(loader != NULL);
    assert(bytecode != NULL);

    struct bytecode_reader reader;
    bcr_init(&reader, bytecode, len);

    VM_MALLOC(struct class, c);
    c->loader = loader;
    c->inited = false;
    c->deprecated = false;

    c->magic = bcr_readu4(&reader);
    c->minor_version = bcr_readu2(&reader);
    c->major_version = bcr_readu2(&reader);

    u2 constant_pool_count = bcr_readu2(&reader);
    struct constant constant_pool[constant_pool_count];
    read_constant_pool(constant_pool, constant_pool_count, &reader);
    c->rtcp = rtcp_create(constant_pool, constant_pool_count);

    c->access_flags = bcr_readu2(&reader);

    c->class_name = rtcp_get_class_name(c->rtcp, bcr_readu2(&reader));
    c->pkg_name = vm_strdup(c->class_name);
    char *p = strrchr(c->pkg_name, '/');
    if (p == NULL) {
        c->pkg_name[0] = 0; // 包名可以为空
    } else {
        *p = 0; // 得到包名
    }

    u2 super_class = bcr_readu2(&reader);
    if (super_class == 0) { // why 0
        c->super_class = NULL; // 可以没有父类
    } else {
        c->super_class = classloader_load_class(loader, rtcp_get_class_name(c->rtcp, super_class));
        // 从父类中拷贝继承来的field   todo 要不要从新new个field不然delete要有问题，继承过来的field的类名问题
//        for_each(superClass->instanceFields.begin(), superClass->instanceFields.end(), [](JField *f) {
//            if (!f->isPrivate()) {
//                instanceFields.push_back(new JField(*f));
//            }
//        });
    }

    // parse interfaces
    c->interfaces_count = bcr_readu2(&reader);
    if (c->interfaces_count == 0) {
        c->interfaces = NULL;
    } else {
        c->interfaces = malloc(sizeof(struct class *) * c->interfaces_count);
        CHECK_MALLOC_RESULT(c->interfaces);

        for (int i = 0; i < c->interfaces_count; i++) {
            const char *interface_name = rtcp_get_class_name(c->rtcp, bcr_readu2(&reader));
            if (interface_name[0] == 0) { // empty
                printvm("error\n"); // todo
            } else {
                c->interfaces[i] = classloader_load_class(loader, interface_name);
            }
        }
    }

    // parse fields
    c->fields_count = bcr_readu2(&reader);
    c->public_fields_count = 0;
    if (c->fields_count == 0) {
        c->fields = NULL;
    } else {
        c->fields = malloc(sizeof(struct field) * c->fields_count);
        CHECK_MALLOC_RESULT(c->fields);
        for (int i = 0, back = c->fields_count - 1; i < c->fields_count; i++) {
            u2 access_flags = bcr_peeku2(&reader);
            // 保证所有的 public fields 放在前面
            if (IS_PUBLIC(access_flags)) {
                field_init(c->fields + c->public_fields_count++, c, &reader);
            } else {
                field_init(c->fields + back--, c, &reader);
            }
        }
    }

    calc_static_field_id(c);
    calc_instance_field_id(c);

    // parse methods
    c->methods_count = bcr_readu2(&reader);
    c->public_methods_count = 0;
    if (c->methods_count == 0) {
        c->methods = NULL;
    } else {
        c->methods = malloc(sizeof(struct method) * c->methods_count);
        CHECK_MALLOC_RESULT(c->methods);
        for (int i = 0, back = c->methods_count - 1; i < c->methods_count; i++) {
            u2 access_flags = bcr_peeku2(&reader);
            // 保证所有的 public methods 放在前面
            if (IS_PUBLIC(access_flags)) {
                method_init(c->methods + c->public_methods_count++, c, &reader);
            } else {
                method_init(c->methods + back--, c, &reader);
            }
        }
    }

    parse_attribute(c, &reader); // parse class attributes
    return c;
}

static void jclass_clear(struct class *c)
{
    assert(c != NULL);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = NULL;
    c->class_name = NULL;
    c->loader = NULL;
    c->inited = false;
    c->deprecated = false;

    c->clsobj = NULL;
    c->rtcp = NULL;
    c->super_class = NULL;
    c->inited_instance_fields_values = NULL;
    c->static_fields_values = NULL;
    c->instance_fields_count = c->static_fields_count = c->fields_count = c->public_fields_count = 0;
    c->interfaces_count = c->methods_count = c->public_methods_count = 0;
    c->interfaces = NULL;
    c->methods = NULL;
    c->fields = NULL;
    c->source_file_name = NULL;
    c->enclosing_info[0] = c->enclosing_info[1] = c->enclosing_info[2] = NULL;
}

struct class* jclass_create_primitive_class(struct classloader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);

    // todo class_name 是不是基本类型

    VM_MALLOC(struct class, c);
    jclass_clear(c);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = vm_strdup(class_name);
    c->loader = loader;
    c->inited = true;

    // todo super_class ???? java.lang.Object ??????

    return c;
}

struct class* jclass_create_arr_class(struct classloader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);

    // todo class_name 是不是 array
    VM_MALLOC(struct class, c);
    jclass_clear(c);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = vm_strdup(class_name);
    c->loader = loader;
    c->inited = true; // 数组类不需要初始化
    c->super_class = classloader_load_class(loader, "java/lang/Object");

    c->interfaces_count = 2;
    c->interfaces = malloc(sizeof(struct class *) * 2);
    c->interfaces[0] = classloader_load_class(loader, "java/lang/Cloneable");
    c->interfaces[1] = classloader_load_class(loader, "java/io/Serializable");

    return c;
}

void jclass_destroy(struct class *c)
{
    if (c == NULL) {
        // todo
        return;
    }

    for (int i = 0; i < c->methods_count; i++) {
        jmethod_release(c->methods + i);
    }

    for (int i = 0; i < c->fields_count; i++) {
        jfield_release(c->fields + i);
    }

    rtcp_destroy(c->rtcp);

    // todo

    free(c);
}

void jclass_clinit(struct class *c, struct thread *thread)
{
    if (c->inited) {
        return;
    }

    struct method *method = jclass_get_declared_method(c, "<clinit>", "()V"); // todo 并不是每个类都有<clinit>方法？？？？？
    if (method != NULL) {
        if (!IS_STATIC(method->access_flags)) {
            // todo error
            printvm("error\n");
        }

        jthread_invoke_method(thread, method, NULL);
    }

    c->inited = true;

    /*
     * 超类放在最后判断，
     * 这样可以保证超类的初始化方法对应的帧在子类上面，
     * 使超类初始化方法先于子类执行。
     */
    if (c->super_class != NULL) {
        jclass_clinit(c->super_class, thread);
    }
}

//struct field* jclass_get_field(struct class *c, const char *name, const char *descriptor)
//{
//    for (int i = 0; i < c->fields_count; i++) {
//        if (strcmp(c->fields[i]->name, name) == 0 && strcmp(c->fields[i]->descriptor, descriptor) == 0) {
//            return c->fields[i];
//        }
//    }
//    return NULL;
//}
//
//struct field** jclass_get_fields(struct class *c, bool public_only)
//{
//    VM_MALLOCS(struct field *, c->fields_count + 1, fields);  // add 1 for NULL to end the array
//
//    struct field **f = fields;
//    for (int i = 0; i < c->fields_count; i++) {
//        if (!public_only || IS_PUBLIC(c->fields[i]->access_flags)) {
//            *f++ = c->fields[i];
//        }
//    }
//    *f = NULL; // end of this array
//
//    return fields;
//}

struct field* jclass_lookup_field0(struct class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->fields_count; i++) {
        if (strcmp(c->fields[i].name, name) == 0 && strcmp(c->fields[i].descriptor, descriptor) == 0) {
            return c->fields + i;
        }
    }

    // todo 在父类中查找
    struct field *field;
    if (c->super_class != NULL) {
        if ((field = jclass_lookup_field0(c->super_class, name, descriptor)) != NULL)
            return field;
    }

    // todo 在父接口中查找
    for (int i = 0; i < c->interfaces_count; i++) {
        if ((field = jclass_lookup_field0(c->interfaces[i], name, descriptor)) != NULL)
            return field;
    }

    return NULL;
}

struct field* jclass_lookup_field(struct class *c, const char *name, const char *descriptor)
{
    struct field *f = jclass_lookup_field0(c, name, descriptor);
    if (f == NULL) {
        // java.lang.NoSuchFieldError  todo
        jvm_abort("java.lang.NoSuchFieldError. %s, %s, %s\n", c->class_name, name, descriptor); // tood
    }
    return f;
}

struct field* jclass_lookup_static_field(struct class *c, const char *name, const char *descriptor)
{
    struct field *field = jclass_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (!IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}

struct field* jclass_lookup_instance_field(struct class *c, const char *name, const char *descriptor)
{
    struct field* field = jclass_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}

struct method* jclass_get_declared_method(struct class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        if (strcmp(c->methods[i].name, name) == 0 && strcmp(c->methods[i].descriptor, descriptor) == 0) {
            return c->methods + i;
        }
    }

    return NULL;
}

struct method* jclass_get_declared_static_method(struct class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        struct method *m = c->methods + i;
        if (IS_STATIC(m->access_flags) && strcmp(m->name, name) == 0 && strcmp(m->descriptor, descriptor) == 0) {
            return m;
        }
    }

    return NULL;
}

struct method* jclass_get_declared_nonstatic_method(struct class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        struct method *m = c->methods + i;
        if (!IS_STATIC(m->access_flags) && strcmp(m->name, name) == 0 && strcmp(m->descriptor, descriptor) == 0) {
            return m;
        }
    }

    return NULL;
}

struct method** jclass_get_methods(struct class *c, const char *name, bool public_only, int *count)
{
    assert(c != NULL);
    assert(name != NULL);
    assert(count != NULL);

    VM_MALLOCS(struct method *, c->methods_count, methods);
    *count = 0;

    for (int i = 0; i < c->methods_count; i++) {
        if ((!public_only || IS_PUBLIC(c->methods[i].access_flags)) && (strcmp(c->methods[i].name, name) == 0)) {
            methods[*count] = c->methods + i;
            (*count)++;
        }
    }

    return methods;
}

struct method* jclass_get_constructor(struct class *c, const char *descriptor)
{
    return jclass_get_declared_method(c, "<init>", descriptor);
}

struct method** jclass_get_constructors(struct class *c, bool public_only, int *count)
{
    struct method **constructors = jclass_get_methods(c, "<init>", public_only, count);
    if (*count < 1) {
        jvm_abort("至少有一个constructor\n");
    }
    return constructors;
}

static struct method* jclass_lookup_method0(struct class *c, const char *name, const char *descriptor)
{
    struct method *method = jclass_get_declared_method(c, name, descriptor);
    if (method != NULL) {
        return method;
    }

    // todo 在父类中查找
    if (c->super_class != NULL) {
        if ((method = jclass_lookup_method0(c->super_class, name, descriptor)) != NULL)
            return method;
    }

    // todo 在父接口中查找
    for (int i = 0; i < c->interfaces_count; i++) {
        if ((method = jclass_lookup_method0(c->interfaces[i], name, descriptor)) != NULL)
            return method;
    }

    return NULL;
}

struct method* jclass_lookup_method(struct class *c, const char *name, const char *descriptor)
{
    struct method *m = jclass_lookup_method0(c, name, descriptor);
    if (m == NULL) {
        // todo java.lang.NoSuchMethodError
        VM_UNKNOWN_ERROR("can not find method. %s~%s~%s", c->class_name, name, descriptor);
    }
    return m;
}

struct method* jclass_lookup_static_method(struct class *c, const char *name, const char *descriptor)
{
    struct method *m = jclass_lookup_method(c, name, descriptor);
    if (!IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

struct method* jclass_lookup_instance_method(struct class *c, const char *name, const char *descriptor)
{
    struct method *m = jclass_lookup_method(c, name, descriptor);
    // todo m == nullptr
    if (IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

bool jclass_is_subclass_of(const struct class *c, const struct class *father)
{
    if (c == father)
        return true;

    if (c->super_class != NULL && jclass_is_subclass_of(c->super_class, father))
        return true;

    for (int i = 0; i < c->interfaces_count; i++) {
        if (jclass_is_subclass_of(c->interfaces[i], father))
            return true;
    }

    return false;
}

int jclass_inherited_depth(const struct class *c)
{
    assert(c != NULL);

    int depth;
    for (depth = 0; c->super_class != NULL; c = c->super_class) {
        depth++;
    }
    return depth;
}

struct slot* copy_inited_instance_fields_values(const struct class *c)
{
    assert(c != NULL);
    VM_MALLOCS(struct slot, c->instance_fields_count, copy);
    memcpy(copy, c->inited_instance_fields_values, c->instance_fields_count * sizeof(struct slot));
    return copy;
}

void set_static_field_value_by_id(struct class *c, int id, const struct slot *value)
{
    assert(c != NULL && value != NULL);
    assert(id >= 0 && id < c->static_fields_count);
    c->static_fields_values[id] = *value;
    if (slot_is_category_two(value)) {
        assert(id + 1 < c->static_fields_count);
        c->static_fields_values[id + 1] = phslot;
    }
}

void set_static_field_value_by_nt(struct class *c,
                  const char *name, const char *descriptor, const struct slot *value)
{
    assert(c != NULL && name != NULL && descriptor != NULL && value != NULL);

    struct field *f = jclass_lookup_field(c, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    set_static_field_value_by_id(c, f->id, value);
}

const struct slot* get_static_field_value_by_id(const struct class *c, int id)
{
    assert(c != NULL);
    if (!(id >= 0 && id < c->static_fields_count)) {
        printvm("-------------  %d, %d, %s\n", id, c->static_fields_count, c->class_name);
    }
    assert(id >= 0 && id < c->static_fields_count);

    return c->static_fields_values + id;
}

const struct slot* get_static_field_value_by_nt(const struct class *c, const char *name, const char *descriptor)
{
    assert(c != NULL && name != NULL && descriptor != NULL);

    struct field *f = jclass_lookup_field(c, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    return get_static_field_value_by_id(c, f->id);
}

char* get_arr_class_name(const char *class_name)
{
    assert(class_name != NULL);

    VM_MALLOCS(char, strlen(class_name) + 8 /* big enough */, array_class_name);

    // 数组
    if (class_name[0] == '[') {
        sprintf(array_class_name, "[%s\0", class_name);
        return array_class_name;
    }

    // 基本类型
    const char *arr_cls_name = pt_get_array_class_name_by_class_name(class_name);
    if (arr_cls_name != NULL) {
        strcpy(array_class_name, arr_cls_name);
        return array_class_name;
    }

    // 类引用
    sprintf(array_class_name, "[L%s;\0", class_name);
    return array_class_name;
}

//struct class* jclass_array_class(struct class *c)
//{
//    assert(c != NULL);
//
//    // todo 好像不对，没有考虑基本类型的类，多维怎么搞
//    char array_class_name[strlen(c->class_name) + 8]; // big enough
//    sprintf(array_class_name, "[L%s;\0", c->class_name);
//
//    if (c->class_name[0] == '[') {
//        sprintf(array_class_name, "[%s\0", c->class_name);
//        return classloader_load_class(c->loader, array_class_name);
//    }
//
//    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
//        if (strcmp(primitive_types[i].name, c->class_name) == 0) {
//            return classloader_load_class(c->loader, primitive_types[i].array_class_name);
//        }
//    }
//
//    sprintf(array_class_name, "[L%s;\0", c->class_name);
//    return classloader_load_class(c->loader, array_class_name);
//}

struct class* jclass_component_class(const struct class *arr_cls)
{
    assert(arr_cls != NULL);
    if (!jclass_is_array(arr_cls)) {
        return NULL;
    }

    const char *component_name = arr_cls->class_name;
    for (; *component_name == '['; component_name++);

    if (*component_name != 'L') {
        return classloader_load_class(arr_cls->loader, component_name);
    }

    component_name++;
    int last = strlen(component_name) - 1;
    assert(last > 0);
    if (component_name[last] != ';') {
        VM_UNKNOWN_ERROR("%s", arr_cls->class_name); // todo
        return NULL;
    } else {
        char buf[last + 1];
        strncpy(buf, component_name, (size_t) last);
        buf[last] = 0;
        return classloader_load_class(arr_cls->loader, buf);
    }
}

bool jclass_is_accessible_to(const struct class *c, const struct class *visitor)
{
    // todo 实现对不对
    assert(c != NULL && visitor != NULL); // todo

    if (c == visitor || IS_PUBLIC(c->access_flags))  // todo 对不对
        return true;

    if (IS_PRIVATE(c->access_flags)) {
        return false;
    }

    // 字段是 protected，则只有 子类 和 同一个包下的类 可以访问
    if (IS_PROTECTED(c->access_flags)) {
        return jclass_is_subclass_of(visitor, c) || strcmp(c->pkg_name, visitor->pkg_name) == 0;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return strcmp(c->pkg_name, visitor->pkg_name) == 0;
}

char *jclass_to_string(const struct class *c)
{
    VM_MALLOCS(char, PATH_MAX, result);
    strcpy(result, "class: ");
    strcat(result, c == NULL ? "NULL" : c->class_name);
    return result;
}
