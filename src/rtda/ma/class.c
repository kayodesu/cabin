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
#include "../heap/strobj.h"
#include "../heap/clsobj.h"
#include "../../symbol.h"
#include "../../utf8.h"
#include "resolve.h"
#include "../../interpreter/interpreter.h"


// 计算实例字段的个数，同时给它们编号
static void calc_instance_field_id(Class *c)
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

//    VM_MALLOCS(struct slot, c->instance_fields_count, values);
//    c->inited_instance_fields_values = values;
//
//    // 将父类中的变量拷贝过来
//    if (c->super_class != NULL) {
//        memcpy(values, c->super_class->inited_instance_fields_values,
//               c->super_class->instance_fields_count * sizeof(struct slot));
//    }
//    // 初始化本类中的实例变量
//    for (int i = 0; i < c->fields_count; i++) {
//        Field *field = c->fields + i;
//        if (!IS_STATIC(field->access_flags)) {
//            assert(field->id < c->instance_fields_count);
//            switch (field->descriptor[0]) {
//                case 'B':
//                case 'C':
//                case 'I':
//                case 'S':
//                case 'Z':
//                    values[field->id] = islot(0);
//                    break;
//                case 'F':
//                    values[field->id] = fslot(0.0f);
//                    break;
//                case 'J':
//                    values[field->id] = lslot(0L);
//                    values[field->id + 1] = phslot;
//                    break;
//                case 'D':
//                    values[field->id] = dslot(0.0);
//                    values[field->id + 1] = phslot;
//                    break;
//                default:
//                    values[field->id] = rslot(NULL);
//                    break;
//            }
//        }
//    }
}

// 计算静态字段的个数，同时给它们编号
static void calc_static_field_id(Class *c)
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
    c->static_fields_values = vm_calloc(c->static_fields_count, sizeof(slot_t));
}

static void parse_attribute(Class *c, BytecodeReader *reader)
{
    u2 attr_count = readu2(reader);
    struct constant_pool *cp = &c->constant_pool;

    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = CP_UTF8(cp, readu2(reader));
        u4 attr_len = bcr_readu4(reader);

        if (S(Signature) == attr_name) {
            c->signature = CP_UTF8(cp, readu2(reader));
        } else if (S(Synthetic) == attr_name) {
            set_synthetic(&c->access_flags);
        } else if (S(Deprecated) == attr_name) {
            c->deprecated = true;
        } else if (S(SourceFile) == attr_name) {
            u2 source_file_index = readu2(reader);
            if (source_file_index >= 0) {
                c->source_file_name = CP_UTF8(cp, source_file_index);
            } else {
                /*
                 * 并不是每个class文件中都有源文件信息，这个因编译时的编译器选项而异。
                 * todo 什么编译选项
                 */
                c->source_file_name = "Unknown source file";
            }
        } else if (S(EnclosingMethod) == attr_name) {
            u2 enclosing_class_index = readu2(reader);
            u2 enclosing_method_index = readu2(reader);

            if (enclosing_class_index > 0) {
                Class *enclosing_class = load_class(c->loader, CP_CLASS_NAME(cp, enclosing_class_index));
                c->enclosing_info[0] = clsobj_create(enclosing_class);

                if (enclosing_method_index > 0) {
//                    const struct name_and_type *nt = rtcp_get_name_and_type(c->rtcp, enclosing_method_index);
                    c->enclosing_info[1] = strobj_create(CP_NAME_TYPE_NAME(cp, enclosing_method_index));//(nt->name);
                    c->enclosing_info[2] = strobj_create(CP_NAME_TYPE_TYPE(cp, enclosing_method_index));//(nt->descriptor);
                }
            }
        } else if (S(BootstrapMethods) == attr_name) {
            u2 num = readu2(reader);
            struct bootstrap_method methods[num];

            for (u2 k = 0; k < num; k++) {
                methods[k].bootstrap_method_ref = readu2(reader);
                methods[k].num_bootstrap_arguments = readu2(reader);
                methods[k].bootstrap_arguments = vm_malloc(sizeof(u2) * methods[k].num_bootstrap_arguments); // todo 没有 free
                for (int j = 0; j < methods[k].num_bootstrap_arguments; j++) {
                    methods[k].bootstrap_arguments[j] = readu2(reader);
                }
            }

//            rtcp_build_invoke_dynamic_constant(c->rtcp, methods);  todo
        } else if (S(InnerClasses) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct inner_class classes[num];
//            for (u2 k = 0; k < num; k++) {
//                classes[k].inner_class_info_index = readu2(reader);
//                classes[k].outer_class_info_index = readu2(reader);
//                classes[k].inner_name_index = readu2(reader);
//                classes[k].inner_class_access_flags = readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(SourceDebugExtension) == attr_name) { // ignore
//            u1 source_debug_extension[attr_len];
//            bcr_read_bytes(reader, source_debug_extension, attr_len);
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeVisibleAnnotations) == attr_name) { // ignore
//            u2 runtime_annotations_num = readu2(reader);
//            struct annotation annotations[runtime_annotations_num];
//            for (u2 k = 0; k < runtime_annotations_num; k++) {
//                read_annotation(reader, annotations + i);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) { // ignore
//            u2 runtime_annotations_num = readu2(reader);
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

static void create_vtable(Class *c)
{
    assert(c != NULL);
    if (c->super_class == NULL) {
        int vtable_len = c->methods_count;
        c->vtable = vm_malloc(sizeof(*(c->vtable)) * vtable_len);
        c->vtable_len = 0;

        for (int i = 0; i < c->methods_count; i++) {
            Method *m = c->methods + i;
            if (IS_PRIVATE(m->access_flags)
                || IS_STATIC(m->access_flags)
                || IS_FINAL(m->access_flags)
                || IS_ABSTRACT(m->access_flags)
                || utf8_equals(m->name, S(class_init))) {  //  todo strcmp(m->name, "<init>") == 0
                continue;
            }

            c->vtable[c->vtable_len].name = m->name;
            c->vtable[c->vtable_len].descriptor = m->descriptor;
            c->vtable[c->vtable_len].method = m;
            m->vtable_index = c->vtable_len;
            c->vtable_len++;
 //           printf("444444     %s~%s~%s\n", m->clazz->class_name, m->name, m->descriptor);//////////////////////////
        }

        return;
    }

    int vtable_len = c->super_class->vtable_len + c->methods_count;
    c->vtable = vm_malloc(sizeof(*(c->vtable)) * vtable_len);
    memcpy(c->vtable, c->super_class->vtable, c->super_class->vtable_len * sizeof(*(c->vtable)));
    c->vtable_len = c->super_class->vtable_len;

    for (int i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (IS_PRIVATE(m->access_flags)
            || IS_STATIC(m->access_flags)
            || IS_FINAL(m->access_flags)
            || IS_ABSTRACT(m->access_flags)
            || utf8_equals(m->name, S(class_init))) {  //  todo strcmp(m->name, "<init>") == 0
//            printf("1111111     %s~%s~%s\n", m->clazz->class_name, m->name, m->descriptor);//////////////////////////
            continue;
        }

        int j = 0;
        for (j = 0; j < c->super_class->vtable_len; j++) {
            if (utf8_equals(m->name, c->vtable[j].name) && utf8_equals(m->descriptor, c->vtable[j].descriptor)) {
                // 重写了父类的方法，更新
                c->vtable[j].method = m;
                m->vtable_index = j;
   //             printf("222222    %s~%s~%s\n", m->clazz->class_name, m->name, m->descriptor);//////////////////////////
                break;
            }
        }
        if (j == c->super_class->vtable_len) {
            // 子类定义了要给新方法，加到 vtable 后面
            c->vtable[c->vtable_len].name = m->name;
            c->vtable[c->vtable_len].descriptor = m->descriptor;
            c->vtable[c->vtable_len].method = m;
            m->vtable_index = c->vtable_len;
    //        printf("33333     %s~%s~%s\n", m->clazz->class_name, m->name, m->descriptor);//////////////////////////
            c->vtable_len++;
        }
    }
}

static void print_vtable(const Class *c)
{
    assert(c != NULL);
    for (int i = 0; i < c->vtable_len; i++) {
        printvm("%s~%s~%s\n", c->class_name, c->vtable[i].name, c->vtable[i].descriptor);
    }
}

Class *class_create(ClassLoader *loader, u1 *bytecode, size_t len)
{
    assert(loader != NULL);
    assert(bytecode != NULL);

    BytecodeReader reader;
    bcr_init(&reader, bytecode, len);

    Class *c = vm_malloc(sizeof(Class));
    c->loader = loader;
    c->inited = false;
    c->deprecated = false;
    c->enclosing_info[0] = c->enclosing_info[1] = c->enclosing_info[2] = NULL;

    c->magic = bcr_readu4(&reader);
    c->minor_version = readu2(&reader);
    c->major_version = readu2(&reader);

    u2 cp_count = readu2(&reader);

    c->constant_pool.type = vm_malloc(cp_count * sizeof(u1));
    c->constant_pool.info = vm_malloc(cp_count * sizeof(slot_t));
    struct constant_pool *cp = &c->constant_pool;

    // constant pool 从 1 开始计数，第0位无效
    CP_TYPE(cp, 0) = CONSTANT_Invalid;
    for (int i = 1; i < cp_count; i++) {
        u1 tag = CP_TYPE(cp, i) = bcr_readu1(&reader);
        switch (tag) {
            case CONSTANT_Class:
            case CONSTANT_String:
            case CONSTANT_MethodType:
                CP_INFO(cp, i) = readu2(&reader);
                break;
            case CONSTANT_NameAndType:
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_Dynamic:
            case CONSTANT_InvokeDynamic: {
                u2 index1 = readu2(&reader); // class_index
                u2 index2 = readu2(&reader); // name_and_type_index
                CP_INFO(cp, i) = (index2 << 16) + index1;
                break;
            }
            case CONSTANT_Integer: {
                u1 bytes[4];
                bcr_read_bytes(&reader, bytes, 4);
                CP_INT(cp, i) = bytes_to_int32(bytes);
                break;
            }
            case CONSTANT_Float: {
                u1 bytes[4];
                bcr_read_bytes(&reader, bytes, 4);
                CP_FLOAT(cp, i) = bytes_to_float(bytes);
                break;
            }
            case CONSTANT_Long: {
                u1 bytes[8];
                bcr_read_bytes(&reader, bytes, 8);
                CP_LONG(cp, i) = bytes_to_int64(bytes);

                i++;
                CP_TYPE(cp, i) = CONSTANT_Placeholder;
                break;
            }
            case CONSTANT_Double: {
                u1 bytes[8];
                bcr_read_bytes(&reader, bytes, 8);
                CP_DOUBLE(cp, i) = bytes_to_double(bytes);

                i++;
                CP_TYPE(cp, i) = CONSTANT_Placeholder;
                break;
            }
            case CONSTANT_Utf8: {
                u2 utf8_len = readu2(&reader);
                char *buf = vm_malloc(utf8_len + 1);
                bcr_read_bytes(&reader, (u1 *) buf, utf8_len);
                buf[utf8_len] = 0;

                const char *utf8;
                CP_INFO(cp,i) = (uintptr_t) (utf8 = new_utf8(buf));
                if (utf8 != buf) // use hashed utf8
                    free(buf);

                break;
            }
            case CONSTANT_MethodHandle: {
                u2 index1 = bcr_readu1(&reader); // 这里确实是 bcr_readu1  reference_kind
                u2 index2 = readu2(&reader); // reference_index
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
                // java_lang_ClassFormatError todo
                jvm_abort("error. bad constant tag: %d\n", tag);
        }
    }

    c->access_flags = readu2(&reader);

    c->class_name = CP_CLASS_NAME(cp, readu2(&reader));
    char *pkg_name = vm_strdup(c->class_name);

    char *p = strrchr(pkg_name, '/');
    if (p == NULL) {
        c->pkg_name = ""; // 包名可以为空
    } else {
        *p = 0; // 得到包名
        c->pkg_name = new_utf8(pkg_name);
        if (c->pkg_name != pkg_name) // use hashed utf8
            free(pkg_name);
    }

    u2 super_class = readu2(&reader);
    if (super_class == 0) { // why 0
        c->super_class = NULL; // 可以没有父类
    } else {
        c->super_class = resolve_class(c, super_class);
//        c->super_class = classloader_load_class(loader, CP_CLASS_NAME(cp, super_class));//rtcp_get_class_name(c->rtcp, super_class));
        // 从父类中拷贝继承来的field   todo 要不要从新new个field不然delete要有问题，继承过来的field的类名问题
//        for_each(superClass->instanceFields.begin(), superClass->instanceFields.end(), [](JField *f) {
//            if (!f->isPrivate()) {
//                instanceFields.push_back(new JField(*f));
//            }
//        });
    }

    // parse interfaces
    c->interfaces_count = readu2(&reader);
    if (c->interfaces_count == 0) {
        c->interfaces = NULL;
    } else {
        c->interfaces = vm_malloc(sizeof(Class *) * c->interfaces_count);

        for (int i = 0; i < c->interfaces_count; i++) {
//            const char *interface_name = CP_CLASS_NAME(cp, readu2(&reader));//rtcp_get_class_name(c->rtcp, readu2(&reader));
//            if (interface_name[0] == 0) { // empty
//                printvm("error\n"); // todo
//            } else {
//                c->interfaces[i] = classloader_load_class(loader, interface_name);
//            }
            c->interfaces[i] = resolve_class(c, readu2(&reader));
        }
    }

    // parse fields
    c->fields_count = readu2(&reader);
    c->public_fields_count = 0;
    if (c->fields_count == 0) {
        c->fields = NULL;
    } else {
        c->fields = vm_malloc(sizeof(Field) * c->fields_count);
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
    c->methods_count = readu2(&reader);
    c->public_methods_count = 0;
    if (c->methods_count == 0) {
        c->methods = NULL;
    } else {
        c->methods = vm_malloc(sizeof(Method) * c->methods_count);
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
//    create_vtable(c);
//    if (strcmp(c->class_name, "sun/util/PreHashedMap") == 0)
//        print_vtable(c);
    return c;
}

static void class_clear(Class *c)
{
    assert(c != NULL);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = NULL;
    c->class_name = NULL;
    c->loader = NULL;
    c->inited = false;
    c->deprecated = false;

    c->clsobj = NULL;
//    c->rtcp = NULL;
    c->super_class = NULL;
//    c->inited_instance_fields_values = NULL;
    c->static_fields_values = NULL;
    c->instance_fields_count = c->static_fields_count = c->fields_count = c->public_fields_count = 0;
    c->interfaces_count = c->methods_count = c->public_methods_count = 0;
    c->interfaces = NULL;
    c->methods = NULL;
    c->fields = NULL;
    c->source_file_name = NULL;
    c->enclosing_info[0] = c->enclosing_info[1] = c->enclosing_info[2] = NULL;
}

Class *class_create_primitive_class(ClassLoader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);

    // todo class_name 是不是基本类型

    Class *c = vm_malloc(sizeof(Class));
    class_clear(c);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = class_name;
    c->loader = loader;
    c->inited = true;

    // todo super_class ???? java.lang.Object ??????

    return c;
}

Class *class_create_arr_class(ClassLoader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);

    // todo class_name 是不是 array
    Class *c = vm_malloc(sizeof(Class));
    class_clear(c);

    c->access_flags = ACC_PUBLIC;
    c->pkg_name = ""; // todo 包名
    c->class_name = vm_strdup(class_name);
    c->loader = loader;
    c->inited = true; // 数组类不需要初始化
    c->super_class = load_sys_class(S(java_lang_Object));

    c->interfaces_count = 2;
    c->interfaces = malloc(sizeof(Class *) * 2);
    c->interfaces[0] = load_sys_class(S(java_lang_Cloneable));
    c->interfaces[1] = load_sys_class(S(java_io_Serializable));

    c->vtable_len = 0;
    c->vtable = NULL;

    return c;
}

void class_destroy(Class *c)
{
    if (c == NULL) {
        // todo
        return;
    }

    for (int i = 0; i < c->methods_count; i++) {
        method_release(c->methods + i);
    }

    for (int i = 0; i < c->fields_count; i++) {
        field_release(c->fields + i);
    }

//    rtcp_destroy(c->rtcp);

    // todo

    free(c);
}

void class_clinit(Class *c)
{
    if (c->inited) {
        return;
    }

    if (c->super_class != NULL && !c->super_class->inited) {
        class_clinit(c->super_class);
    }

    // 在这里先行 set inited true, 如不这样，后面执行<clinit>时，
    // 可能调用putstatic等函数也会触发<clinit>的调用造成死循环。
    c->inited = true;

    Method *method = class_get_declared_method(c, S(class_init), S(___V));
    if (method != NULL) { // 有的类没有<clinit>方法
        if (!IS_STATIC(method->access_flags)) {
            // todo error
            printvm("error\n");
        }

//        thread_invoke_method(thread, method, NULL);
        exec_java_func(method, NULL);
    }
}

Field* jclass_lookup_field0(Class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->fields_count; i++) {
        if (utf8_equals(c->fields[i].name, name) && utf8_equals(c->fields[i].descriptor, descriptor)) {
            return c->fields + i;
        }
    }

    // todo 在父类中查找
    Field *field;
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

Field* class_lookup_field(Class *c, const char *name, const char *descriptor)
{
    Field *f = jclass_lookup_field0(c, name, descriptor);
    if (f == NULL) {
        // java.lang.NoSuchFieldError  todo
        jvm_abort("java.lang.NoSuchFieldError. %s, %s, %s\n", c->class_name, name, descriptor); // tood
    }
    return f;
}

Field* class_lookup_static_field(Class *c, const char *name, const char *descriptor)
{
    Field *field = class_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (!IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}

Field* class_lookup_instance_field(Class *c, const char *name, const char *descriptor)
{
    Field* field = class_lookup_field(c, name, descriptor);
    // todo field == nullptr
    if (IS_STATIC(field->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return field;
}

Method* class_get_declared_method(Class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        if (utf8_equals(c->methods[i].name, name) && utf8_equals(c->methods[i].descriptor, descriptor)) {
            return c->methods + i;
        }
    }

    return NULL;
}

Method* class_get_declared_static_method(Class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (IS_STATIC(m->access_flags) && strcmp(m->name, name) == 0 && strcmp(m->descriptor, descriptor) == 0) {
            return m;
        }
    }

    return NULL;
}

Method* class_get_declared_nonstatic_method(Class *c, const char *name, const char *descriptor)
{
    for (int i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (!IS_STATIC(m->access_flags) && strcmp(m->name, name) == 0 && strcmp(m->descriptor, descriptor) == 0) {
            return m;
        }
    }

    return NULL;
}

Method** class_get_declared_methods(Class *c, const char *name, bool public_only, int *count)
{
    assert(c != NULL);
    assert(name != NULL);
    assert(count != NULL);

    Method **methods = vm_malloc(sizeof(Method *) * c->methods_count);
    *count = 0;

    for (int i = 0; i < c->methods_count; i++) {
        if ((!public_only || IS_PUBLIC(c->methods[i].access_flags)) && (utf8_equals(c->methods[i].name, name))) {
            methods[*count] = c->methods + i;
            (*count)++;
        }
    }

    return methods;
}

Method* class_get_constructor(Class *c, const char *descriptor)
{
    return class_get_declared_method(c, S(object_init), descriptor);
}

Method** class_get_constructors(Class *c, bool public_only, int *count)
{
    Method **constructors = class_get_declared_methods(c, S(object_init), public_only, count);
    if (*count < 1) {
        jvm_abort("至少有一个constructor\n");
    }
    return constructors;
}

static Method* class_lookup_method0(Class *c, const char *name, const char *descriptor)
{
    Method *method = class_get_declared_method(c, name, descriptor);
    if (method != NULL) {
        return method;
    }

    // todo 在父类中查找
    if (c->super_class != NULL) {
        if ((method = class_lookup_method0(c->super_class, name, descriptor)) != NULL)
            return method;
    }

    // todo 在父接口中查找
    for (int i = 0; i < c->interfaces_count; i++) {
        if ((method = class_lookup_method0(c->interfaces[i], name, descriptor)) != NULL)
            return method;
    }

    return NULL;
}

Method* class_lookup_method(Class *c, const char *name, const char *descriptor)
{
    Method *m = class_lookup_method0(c, name, descriptor);
    if (m == NULL) {
        // todo java.lang.NoSuchMethodError
        VM_UNKNOWN_ERROR("can not find method. %s~%s~%s", c->class_name, name, descriptor);
    }
    return m;
}

Method* class_lookup_static_method(Class *c, const char *name, const char *descriptor)
{
    Method *m = class_lookup_method(c, name, descriptor);
    if (!IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

Method* class_lookup_instance_method(Class *c, const char *name, const char *descriptor)
{
    Method *m = class_lookup_method(c, name, descriptor);
    // todo m == nullptr
    if (IS_STATIC(m->access_flags)) {
        // todo java.lang.IncompatibleClassChangeError
        jvm_abort("java.lang.IncompatibleClassChangeError");
    }
    return m;
}

bool class_is_subclass_of(const Class *c, const Class *father)
{
    if (c == father)
        return true;

    if (c->super_class != NULL && class_is_subclass_of(c->super_class, father))
        return true;

    for (int i = 0; i < c->interfaces_count; i++) {
        if (class_is_subclass_of(c->interfaces[i], father))
            return true;
    }

    return false;
}

int class_inherited_depth(const Class *c)
{
    assert(c != NULL);

    int depth;
    for (depth = 0; c->super_class != NULL; c = c->super_class) {
        depth++;
    }
    return depth;
}

void set_static_field_value(Class *c, Field *f, slot_t *value)
{
    assert(c != NULL && f != NULL && value != NULL);

    c->static_fields_values[f->id] =  value[0];
    if (f->category_two) {
        c->static_fields_values[f->id + 1] = value[1];
    }
}

const slot_t* get_static_field_value(const Class *c, const Field *f)
{
    return c->static_fields_values + f->id;
}

char* get_arr_class_name(const char *class_name)
{
    assert(class_name != NULL);
    char *array_class_name = vm_malloc(sizeof(char)*(strlen(class_name) + 8 /* big enough */));

    // 数组
    if (class_name[0] == '[') {
        sprintf(array_class_name, "[%s\0", class_name);
        return array_class_name;
    }

    // 基本类型
    const char *arr_cls_name = get_primitive_array_class_name_by_class_name(class_name);
    if (arr_cls_name != NULL) {
        strcpy(array_class_name, arr_cls_name);
        return array_class_name;
    }

    // 类引用
    sprintf(array_class_name, "[L%s;\0", class_name);
    return array_class_name;
}

//Class* jclass_array_class(Class *c)
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

Class* class_component_class(const Class *arr_cls)
{
    assert(arr_cls != NULL);
    if (!class_is_array(arr_cls)) {
        return NULL;
    }

    const char *component_name = arr_cls->class_name;
    for (; *component_name == '['; component_name++);

    if (*component_name != 'L') {
        return load_class(arr_cls->loader, component_name);
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
        return load_class(arr_cls->loader, buf);
    }
}

bool class_is_accessible_to(const Class *c, const Class *visitor)
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
        return class_is_subclass_of(visitor, c) || utf8_equals(c->pkg_name, visitor->pkg_name);
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return utf8_equals(c->pkg_name, visitor->pkg_name);
}

char *class_to_string(const Class *c)
{
    char *result = vm_malloc(sizeof(char) * PATH_MAX);
    strcpy(result, "class: ");
    strcat(result, c == NULL ? "NULL" : c->class_name);
    return result;
}
