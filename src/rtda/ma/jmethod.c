/*
 * Author: Jia Yang
 */

#include "jmethod.h"
#include "access.h"
#include "../heap/jobject.h"

struct jobject* jmethod_parse_descriptor(struct classloader *loader, const char *method_descriptor,
                                         struct jobject **parameter_types_add, int parameter_num_max)
{
    assert(loader != NULL);
    assert(method_descriptor != NULL);
    assert(parameter_types_add != NULL);

    int dlen = strlen(method_descriptor);
    char descriptor[dlen + 1];
    strcpy(descriptor, method_descriptor);

    if (parameter_num_max < 0)
        parameter_num_max = dlen;
    struct jobject* buf[parameter_num_max]; // big enough

    int parameter_types_count = 0;

    char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == NULL || e == NULL) {
        VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
    }

    // parameter_types
    while (++b < e) {
        if (*b == 'L') { // reference
            char *t = strchr(b, ';');
            if (t == NULL) {
                VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
            }

            *t = 0;   // end string
            buf[parameter_types_count++] = classloader_load_class(loader, b + 1 /* jump 'L' */)->clsobj;
            *t = ';'; // recover
            b = t;
        } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[java/lang/String; 的形式
            char *t = b;
            while (*(++t) == '[');
            if (!pt_is_primitive_descriptor(*t)) {
                t = strchr(t, ';');
                if (t == NULL) {
                    VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
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
        }
    }

    // todo parameter_types_count == 0 是不是要填一个 void.class

    struct jobject *parameter_types
            = jarrobj_create(classloader_load_class(loader, "[Ljava/lang/Class;"), parameter_types_count);
    for (int i = 0; i < parameter_types_count; i++) {
        jarrobj_set(struct jobject *, parameter_types, i, buf[i]);
    }

    *parameter_types_add = parameter_types;
    
    // create return_type
    const char *class_name = ++e;
    if (pt_is_primitive_descriptor(*e)) {
        class_name = pt_get_class_name_by_descriptor(*e);
    } else if (*class_name == 'L') {
        class_name++;
        char *t = strchr(class_name, ';');
        if (t == NULL) {
            VM_UNKNOWN_ERROR("descriptor error. %s", descriptor);
        }
        *t = 0;   // end string
    }

    return classloader_load_class(loader, class_name)->clsobj; // return_type
}

/*
 * parse method descriptor and create parameter_types and return_type of the method
 */
//static inline void parse_descriptor(struct jmethod *method)
//{
//    assert(method != NULL);
//    method->return_type = 
//            jmethod_parse_descriptor(method->jclass->loader, method->descriptor, &(method->parameter_types));

//    struct jobject **parameter_types = malloc(sizeof(struct jclsobj **) * method->arg_slot_count);
//    CHECK_MALLOC_RESULT(parameter_types);
//
//    int parameter_types_count = 0;
//    struct classloader *loader = method->jclass->loader;
//
//    char *b = strchr(method->descriptor, '(');
//    const char *e = strchr(method->descriptor, ')');
//    if (b == NULL || e == NULL) {
//        VM_UNKNOWN_ERROR("descriptor error. %s\n", method->descriptor);
//    }
//
//    // parameter_types
//    while (++b < e) {
//        if (*b == 'L') { // reference
//            char *t = strchr(b, ';');
//            if (t == NULL) {
//                jvm_abort("error. %s\n", method->descriptor);
//            }
//
//            *t = 0;   // end string
//            parameter_types[parameter_types_count++] = classloader_load_class(loader, b + 1)->clsobj;
//            *t = ';'; // recover
//            b = t;
//        } else if (*b == '[') { // array reference, 描述符形如 [B 或 [[java/lang/String; 的形式
//            char *t = b;
//            while (*(++t) == '[');
//            if (!pt_is_primitive_descriptor(*t)) {
//                t = strchr(t, ';');
//                if (t == NULL) {
//                    jvm_abort("error. %s\n", method->descriptor);
//                }
//            }
//
//            char k = *(++t);
//            *t = 0; // end string
//            parameter_types[parameter_types_count++] = classloader_load_class(loader, b)->clsobj;
//            *t = k; // recover
//            b = t;
//        } else if (pt_is_primitive_descriptor(*b)) {
//            const char *class_name = pt_get_class_name_by_descriptor(*b);
//            parameter_types[parameter_types_count++] = classloader_load_class(loader, class_name)->clsobj;
//        } else {
//            jvm_abort("error %s\n", method->descriptor);
//        }
//    }
//
//    method->parameter_types
//            = jarrobj_create(classloader_load_class(loader, "[Ljava/lang/Class;"), parameter_types_count);
//    for (int i = 0; i < parameter_types_count; i++) {
//        jarrobj_set(struct jobject *, method->parameter_types, i, parameter_types[i]);
//    }
//
//    // create return_type
//    const char *class_name = ++e;
//    if (pt_is_primitive_descriptor(*e)) {
//        class_name = pt_get_class_name_by_descriptor(*e);
//    }
//    method->return_type = classloader_load_class(loader, class_name)->clsobj;
//}

struct jobject* jmethod_get_parameter_types(struct jmethod *method)
{
    assert(method != NULL);
    if (method->parameter_types == NULL) {
        method->return_type = jmethod_parse_descriptor(method->jclass->loader, method->descriptor,
                                                       &(method->parameter_types), method->arg_slot_count);
    }
    assert(method->parameter_types != NULL);
    return method->parameter_types;
}

struct jobject* jmethod_get_return_type(struct jmethod *method)
{
    assert(method != NULL);
    if (method->return_type == NULL) {
        method->return_type = jmethod_parse_descriptor(method->jclass->loader, method->descriptor,
                                                       &(method->parameter_types), method->arg_slot_count);
    }
    assert(method->return_type != NULL);
    return method->return_type;
}

struct jobject* jmethod_get_exception_types(struct jmethod *method)
{
    assert(method != NULL);

    if (method->exception_types == NULL) {
        int count = 0;
        struct jobject **exception_types = malloc(sizeof(struct jobject **) * method->exception_tables_count);
        CHECK_MALLOC_RESULT(exception_types);
        for (int i = 0; i < method->exception_tables_count; i++) {
            struct jclass *c = method->exception_tables[i].catch_type;
            if (c != NULL) {
                exception_types[count++] = c->clsobj;
            }
        }

        method->exception_types
                = jarrobj_create(classloader_load_class(method->jclass->loader, "[Ljava/lang/Class;"), count);
        for (int i = 0; i < count; i++) {
            jarrobj_set(struct jobject *, method->exception_types, i, exception_types[i]);
        }
    }

    assert(method->exception_types != NULL);
    return method->exception_types;
}

static void cal_arg_slot_count(struct jmethod *method)
{
    method->arg_slot_count = 0;

    const char *b = strchr(method->descriptor, '(');
    const char *e = strchr(method->descriptor, ')');
    if (b == NULL || e == NULL) {
        jvm_abort("error. %s\n", method->descriptor);
    }

    while (++b < e) {
        if (*b == 'B' || *b == 'C' || *b == 'I' || *b == 'F' || *b == 'S'|| *b == 'Z'/* boolean */) {
            method->arg_slot_count++;
        } else if (*b == 'D' || *b == 'J'/* long */) {
            method->arg_slot_count += 2;
        } else if (*b == 'L') { // reference
            method->arg_slot_count++;
            b = strchr(b, ';');
            if (b == NULL) {
                jvm_abort("error. %s\n", method->descriptor);
            }
        } else if (*b == '[') { // array reference
            method->arg_slot_count++;
            while (*(++b) == '[');

            if (*b == 'L') {
                b = strchr(b, ';');
                if (b == NULL) {
                    jvm_abort("error. %s\n", method->descriptor);
                }
            }
        }
    }

    if (!IS_STATIC(method->access_flags)) { // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
        method->arg_slot_count++; // this reference
    }
}

/*
 * 解析方法的 code 属性
 */
static void parse_code_attr(struct jmethod *method, struct code_attribute *a)
{
    method->max_stack = a->max_stack;
    method->max_locals = a->max_locals;

    method->code = a->code;
    method->code_length = a->code_length;

    method->line_number_table_count = 0;
    method->line_number_tables = NULL;

    method->exception_tables_count = a->exception_tables_length;
    // todo exceptionTableCount == 0
    method->exception_tables = malloc(sizeof(struct exception_table) * method->exception_tables_count);
    for (int i = 0; i < method->exception_tables_count; i++) {
        const struct code_attribute_exception_table *t0 = a->exception_tables + i;
        struct exception_table *t = method->exception_tables + i;
        t->start_pc = t0->start_pc;
        t->end_pc = t0->end_pc;
        t->handler_pc = t0->handler_pc;
        if (t0->catch_type == 0) {
            /*
             * 异常处理项的 catch_type 有可能是 0。
             * 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
             */
            t->catch_type = NULL;
        } else {
            const char *class_name = rtcp_get_class_name(method->jclass->rtcp, t0->catch_type);
            t->catch_type = classloader_load_class(method->jclass->loader, class_name);
        }
    }

    // 解析 code 属性的属性
    for (int k = 0; k < a->attributes_count; k++) {
        if (strcmp(a->attributes[k]->name, StackMapTable) == 0) {
            // todo
        } else if (strcmp(a->attributes[k]->name, LineNumberTable) == 0) {   // 可选属性
            struct line_number_table_attribute *tmp = (struct line_number_table_attribute *) a->attributes[k];
            method->line_number_table_count = tmp->line_number_table_length;
            method->line_number_tables = malloc(sizeof(struct line_number_table) * method->line_number_table_count);
            for (int i = 0; i < method->line_number_table_count; i++) {
                method->line_number_tables[i].start_pc = tmp->line_number_tables[i].start_pc;
                method->line_number_tables[i].line_number = tmp->line_number_tables[i].line_number;
            }
        } else if (strcmp(a->attributes[k]->name, LocalVariableTable) == 0) {   // 可选属性
            //         printvm("not parse attr: LocalVariableTable\n");
        } else if (strcmp(a->attributes[k]->name, LocalVariableTypeTable) == 0) {   // 可选属性
            //         printvm("not parse attr: LocalVariableTypeTable\n");
        }
    }
}

struct jmethod* jmethod_create(const struct jclass *c, const struct member_info *info)
{
    assert(c != NULL && info != NULL);

    VM_MALLOC(struct jmethod, method);
    method->name = rtcp_get_str(c->rtcp, info->name_index);
    method->descriptor = rtcp_get_str(c->rtcp, info->descriptor_index);
    method->access_flags = info->access_flags;
    method->jclass = c;

    method->max_stack = method->max_locals = method->exception_tables_count = 0;
    method->line_number_table_count = method->code_length = 0;
    method->exception_tables = NULL;
    method->line_number_tables = NULL;
    method->code = NULL;

    // parameter_types, return_type, exception_types
    // 先不解析，待需要时再解析，以节省时间。
    method->parameter_types = method->exception_types = NULL;
    method->return_type = NULL;

    cal_arg_slot_count(method);

    for (int j = 0; j < info->attributes_count; j++) {
        struct attribute_common *attr = info->attributes[j];
        // todo methods Attributes
        if (strcmp(attr->name, Code) == 0) {
            parse_code_attr(method, (struct code_attribute *) attr);
        } else if (strcmp(attr->name, Synthetic) == 0) {
            set_synthetic(&method->access_flags);  // todo
        } else if (strcmp(attr->name, Signature) == 0) {  // 可选属性
//                jvm_printf("not parse attr: Signature\n");
        }  else if (strcmp(attr->name, Exceptions) == 0) {
//                jvm_printf("not parse attr: Exceptions\n");
        } else if (strcmp(attr->name, RuntimeVisibleParameterAnnotations) == 0) {
            //           runtime_parameter_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeVisibleParameterAnnotations\n");
        } else if (strcmp(attr->name, RuntimeInvisibleParameterAnnotations) == 0) {
            //           runtime_parameter_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeInvisibleParameterAnnotations\n");
        } else if (strcmp(attr->name, Deprecated) == 0) {  // 可选属性
//                jvm_printf("not parse attr: Deprecated\n");
        } else if (strcmp(attr->name, RuntimeVisibleAnnotations) == 0) {
            //          runtime_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (strcmp(attr->name, RuntimeInvisibleAnnotations) == 0) {
            //         runtime_annotations_attr *a = attr;
//                jvm_printf("not parse attr: RuntimeInvisibleAnnotations\n");
        } else if (strcmp(attr->name, AnnotationDefault) == 0) {
            //        annotation_default_attr *a = attr;
//                jvm_printf("not parse attr: AnnotationDefault\n");
        } else {
            jvm_abort("Unknown: %s", attr->name);
        }
    }

    if (IS_NATIVE(method->access_flags)) {
        method->max_stack = 4;  // todo 本地方法帧的操作数栈至少要能容纳返回值， 为了简化代码，暂时给maxStack字段赋值为4
        method->max_locals = method->arg_slot_count; // todo 因为本地方法帧的局部变量表只用来存放参数值，所以把argSlotCount赋给maxLocals字段刚好。

        size_t code_len = 2;
        VM_MALLOCS(s1, code_len, code);
        code[0] = (s1) 0xfe;  // 0xfe 是 "impdep1" 指令的代码，用这条指令来执行 native 方法。
        const char *t = strchr(method->descriptor, ')'); // find return
        if (t == NULL) {
            //todo error
            printvm("method's descriptor 格式不对：%s\n", method->descriptor);
        }

        ++t;
        if (*t == 'V') {
            code[1] = (s1) 0xb1; // return
        } else if (*t == 'D') {
            code[1] = (s1) 0xaf; // dreturn
        } else if (*t == 'F') {
            code[1] = (s1) 0xae; // freturn
        } else if (*t == 'J') {
            code[1] = (s1) 0xad; // lreturn
        } else if (*t == 'L' || *t == '[') {
            code[1] = (s1) 0xb0; // areturn
        } else {
            code[1] = (s1) 0xac; // ireturn
        }
        method->code = code;
        method->code_length = code_len;
    }

    return method;
}

void jmethod_destroy(struct jmethod *m)
{
    if (m == NULL) {
        // todo
        return;
    }

    // todo

    free(m);
}


bool jmethod_is_accessible_to(const struct jmethod *method, const struct jclass *visitor)
{
    // todo  实现对不对

    if (!jclass_is_accessible_to(method->jclass, visitor)) {
        return false;
    }

    if (method->jclass == visitor || IS_PUBLIC(method->access_flags))  // todo 对不对
        return true;

    if (IS_PRIVATE(method->access_flags)) {
        return false;
    }

    // 字段是protected，则只有 子类 和 同一个包下的类 可以访问
    if (IS_PROTECTED(method->access_flags)) {
        return jclass_is_subclass_of(visitor, method->jclass)
               || strcmp(method->jclass->pkg_name, visitor->pkg_name) == 0;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return strcmp(method->jclass->pkg_name, visitor->pkg_name) == 0;
}

int jmethod_get_line_number(const struct jmethod *method, int pc)
{
    assert(method != NULL);

    // native函数没有字节码
    if (IS_NATIVE(method->access_flags)) {
        return -2;
    }

    /*
     * 和源文件名一样，并不是每个方法都有行号表。如果方法没有
行号表，自然也就查不到pc对应的行号，这种情况下返回–1

     todo
     */
    if (method->line_number_tables == NULL) {
        return -1;
    }

    for (int i = method->line_number_table_count - 1; i >= 0; i--) {
        if (pc >= method->line_number_tables[i].start_pc)
            return method->line_number_tables[i].line_number;
    }
    return -1;
}

int jmethod_find_exception_handler(struct jmethod *method, struct jclass *exception_type, size_t pc)
{
    for (int i = 0; i < method->exception_tables_count; i++) {
        const struct exception_table *t = method->exception_tables + i;
        if (t->start_pc <= pc && pc < t->end_pc) {
            if (t->catch_type == NULL) {
                return t->handler_pc;  // catch all
            }
            if (jclass_is_subclass_of(exception_type, t->catch_type)) {
                return t->handler_pc;
            }
        }
    }

    return -1;
}

char *jmethod_to_string(const struct jmethod *method)
{
#define MAX_LEN 1023 // big enough
    VM_MALLOCS(char, MAX_LEN + 1, result);

    if (method != NULL) {
        int n = snprintf(result, MAX_LEN,
                         "method%s: %s~%s~%s", IS_NATIVE(method->access_flags) ? "(native)" : "",
                         method->jclass->class_name, method->name, method->descriptor);
        if (n < 0) {
            jvm_abort("snprintf 出错\n"); // todo
        }
        assert(0 <= n && n <= MAX_LEN);
        result[n] = 0;
    } else {
        strcpy(result, "method: NULL");
    }

    return result;
#undef MAX_LEN
}
