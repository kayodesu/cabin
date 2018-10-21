/*
 * Author: Jia Yang
 */

#include "jmethod.h"
#include "../../jvm.h"
#include "access.h"

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

    method->exception_tables_count = a->exception_tables_length;
    // todo exceptionTableCount == 0
    method->exception_tables = malloc(sizeof(struct exception_table) * method->exception_tables_count);
    for (int i = 0; i < method->exception_tables_count; i++) {
        method->exception_tables[i].start_pc = a->exception_tables[i].start_pc;
        method->exception_tables[i].end_pc = a->exception_tables[i].end_pc;
        method->exception_tables[i].handler_pc = a->exception_tables[i].handler_pc;
        if (a->exception_tables[i].catch_type == 0) {
            method->exception_tables[i].catch_type = NULL; // todo  表示 finally 字句？？？
        } else {
            const char *class_name = rtcp_get_class_name(method->jclass->rtcp, a->exception_tables[i].catch_type);
            method->exception_tables[i].catch_type = classloader_load_class(method->jclass->loader, class_name);
        }
    }

    // 解析 code 属性的属性
    for (int k = 0; k < a->attributes_count; k++) {
        if (strcmp(a->attributes[k]->name, StackMapTable) == 0) {
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
        VM_MALLOC(s1, code);
        code[0] = (s1) 0xfe;  // todo  0xfe 是啥
        const char *t = strchr(method->descriptor, ')'); // find return  todo how?????
        if (t == NULL) {
            //todo error
            printvm("error\n");
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
        return jclass_is_subclass_of(visitor, method->jclass) || strcmp(method->jclass->pkg_name, visitor->pkg_name) == 0;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return strcmp(method->jclass->pkg_name, visitor->pkg_name) == 0;
}

int jmethod_find_exception_handler(struct jmethod *method, struct jclass *exception_type, size_t pc)   // todo pc 是什么？
{
    for (int i = 0; i < method->exception_tables_count; i++) {
//        struct exception_table *t = method->exception_tables[i];
        if (method->exception_tables[i].start_pc <= pc && pc < method->exception_tables[i].end_pc) {
            if (method->exception_tables[i].catch_type == NULL) {
                return method->exception_tables[i].handler_pc;  // catch all  todo
            }
            if (jclass_is_subclass_of(exception_type, method->exception_tables[i].catch_type)) {
                return method->exception_tables[i].handler_pc;
            }
        }
    }

    return -1;
}

char *jmethod_to_string(const struct jmethod *method)
{
    if (method == NULL) {
        return "method: NULL";
    }
    snprintf(global_buf, GLOBAL_BUF_LEN,
             "method: %s~%s~%s\0", method->jclass->class_name, method->name, method->descriptor);
    return global_buf;
}
