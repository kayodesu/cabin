/*
 * Author: Jia Yang
 */

#include "method.h"
#include "access.h"
#include "../heap/object.h"
#include "descriptor.h"
#include "../heap/arrobj.h"
#include "../../symbol.h"
#include "../../classfile/constant.h"
#include "../../interpreter/interpreter.h"
#include "../../utf8.h"

Object* method_get_parameter_types(Method *method)
{
    assert(method != NULL);
    if (method->parameter_types == NULL) {
        method->parameter_types = method_descriptor_to_parameter_types(method->clazz->loader, method->descriptor);
    }
    return method->parameter_types;
}

Object* method_get_return_type(Method *method)
{
    assert(method != NULL);
    if (method->return_type == NULL) {
        method->return_type = method_descriptor_to_return_type(method->clazz->loader, method->descriptor);
    }
    return method->return_type;
}

Object* method_get_exception_types(Method *method)
{
    assert(method != NULL);

    if (method->exception_types == NULL) {
        int count = 0;
        Object **exception_types = vm_malloc(sizeof(Object **) * method->exception_tables_count);
        for (int i = 0; i < method->exception_tables_count; i++) {
            Class *c = method->exception_tables[i].catch_type;
            if (c != NULL) {
                exception_types[count++] = c->clsobj;
            }
        }

        method->exception_types
                = arrobj_create(load_class(method->clazz->loader, "[Ljava/lang/Class;"), count);
        for (int i = 0; i < count; i++) {
            arrobj_set(Object *, method->exception_types, i, exception_types[i]);
        }
    }

    assert(method->exception_types != NULL);
    return method->exception_types;
}

static void cal_arg_slot_count(Method *method)
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
static void parse_code_attr(Method *method, BytecodeReader *reader)
{
    assert(method != NULL);
    assert(reader != NULL);

    method->max_stack = readu2(reader);
    method->max_locals = readu2(reader);

    method->code_length = bcr_readu4(reader);

//    method->code = malloc(sizeof(u1) * method->code_length);
//    CHECK_MALLOC_RESULT(method->code);
//    bcr_read_bytes(reader, method->code, method->code_length);

    method->code = bcr_curr_pos(reader);
    bcr_skip(reader, method->code_length);

    method->line_number_tables_count = 0;
    method->line_number_tables = NULL;

    // parse exception tables
    method->exception_tables_count = readu2(reader);
    if (method->exception_tables_count == 0) {
        method->exception_tables = NULL;
    } else {
        method->exception_tables = vm_malloc(sizeof(ExceptionTable) * method->exception_tables_count);
        for (int i = 0; i < method->exception_tables_count; i++) {
            ExceptionTable *t = method->exception_tables + i;
            t->start_pc = readu2(reader);
            t->end_pc = readu2(reader);
            t->handler_pc = readu2(reader);
            u2 catch_type = readu2(reader);
            if (catch_type == 0) {
                // 异常处理项的 catch_type 有可能是 0。
                // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
                t->catch_type = NULL;
            } else {
                if (CP_TYPE(&method->clazz->constant_pool, catch_type) == CONSTANT_ResolvedClass)
                    t->catch_type = (Class *) CP_INFO(&method->clazz->constant_pool, catch_type);
                else {
                    const char *class_name = CP_CLASS_NAME(&method->clazz->constant_pool, catch_type);
                    t->catch_type = load_class(method->clazz->loader, class_name);
                }
            }
        }
    }

    // parse attributes of code's attribute
    u2 attr_count = readu2(reader);
    for (int k = 0; k < attr_count; k++) {
        const char *attr_name = CP_UTF8(&method->clazz->constant_pool, readu2(reader));//rtcp_get_str(method->clazz->rtcp, readu2(reader));
        u4 attr_len = bcr_readu4(reader);

        if (S(LineNumberTable) == attr_name) {
            method->line_number_tables = newLineNumberTables(reader, &method->line_number_tables_count);
        } else if (S(StackMapTable) == attr_name) { // ignore
            bcr_skip(reader, attr_len);
        } else if (S(LocalVariableTable) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct local_variable_table tables[num];
//            for (int i = 0; i < num; i++) {
//                tables[i].start_pc = readu2(reader);
//                tables[i].length = readu2(reader);
//                tables[i].name_index = readu2(reader);
//                tables[i].descriptor_index = readu2(reader);
//                tables[i].index = readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(LocalVariableTypeTable) == attr_name) { // ignore
//            u2 num = readu2(reader);
//            struct local_variable_type_table tables[num];
//            for (int i = 0; i < num; i++) {
//                tables[i].start_pc = readu2(reader);
//                tables[i].length = readu2(reader);
//                tables[i].name_index = readu2(reader);
//                tables[i].signature_index = readu2(reader);
//                tables[i].index = readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else {
            // unknown attribute
            bcr_skip(reader, attr_len);
        }
    }
}

void method_init(Method *method, Class *c, BytecodeReader *reader)
{
    method->clazz = c;
    struct constant_pool *cp = &c->constant_pool;

    method->access_flags = readu2(reader);
    method->name = CP_UTF8(cp, readu2(reader));
    method->descriptor = CP_UTF8(cp, readu2(reader));
    u2 attr_count = readu2(reader);

    method->max_stack = method->max_locals = method->exception_tables_count = 0;
    method->line_number_tables_count = method->code_length = 0;
    method->exception_tables = NULL;
    method->line_number_tables = NULL;
    method->code = NULL;
    method->deprecated = false;

    // parameter_types, return_type, exception_types
    // 先不解析，待需要时再解析，以节省时间。
    method->parameter_types = method->exception_types = NULL;
    method->return_type = NULL;

    cal_arg_slot_count(method);

    // parse method's attributes
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = CP_UTF8(cp, readu2(reader));//rtcp_get_str(c->rtcp, readu2(reader));
        u4 attr_len = bcr_readu4(reader);

        if (S(Code) == attr_name) {
            parse_code_attr(method, reader);
        } else if (S(Deprecated) == attr_name) {
            method->deprecated = true;
        } else if (S(Synthetic) == attr_name) {
            set_synthetic(&method->access_flags);
        } else if (S(Signature) == attr_name) {
            c->signature = CP_UTF8(cp, readu2(reader));//rtcp_get_str(c->rtcp, readu2(reader));
        } else if (S(MethodParameters) == attr_name) { // ignore
//            u1 num = bcr_readu1(reader); // 这里就是 u1，不是u2
//            struct parameter parameters[num];
//            for (u2 k = 0; k < num; k++) {
//                parameters[k].name_index = readu2(reader);
//                parameters[k].access_flags = readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(Exceptions) == attr_name) { // ignore
//            method->exception_tables_count = readu2(reader);
//            u2 exception_index_table[num];
//            for (u2 k = 0; k < num; k++) {
//                exception_index_table[i] = readu2(reader);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeVisibleParameterAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_visible_parameter_annotations_num = readu2(reader);
//            struct parameter_annotation *as
//                    = method->runtime_visible_parameter_annotations = malloc(sizeof(struct parameter_annotation) * num);
//            CHECK_MALLOC_RESULT(as);
//            for (u2 k = 0; k < num; k++) {
//                u2 nums = as[i].num_annotations = readu2(reader);
//                as[i].annotations = malloc(sizeof(struct annotation) * nums);
//                CHECK_MALLOC_RESULT(as[i].annotations);
//                for (int j = 0; j < nums; j++) {
//                    read_annotation(reader, as[i].annotations + j);
//                }
//            }
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeInvisibleParameterAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_invisible_parameter_annotations_num = readu2(reader);
//            struct parameter_annotation *as
//                    = method->runtime_invisible_parameter_annotations = malloc(sizeof(struct parameter_annotation) * num);
//            CHECK_MALLOC_RESULT(as);
//            for (u2 k = 0; k < num; k++) {
//                u2 nums = as[i].num_annotations = readu2(reader);
//                as[i].annotations = malloc(sizeof(struct annotation) * nums);
//                CHECK_MALLOC_RESULT(as[i].annotations);
//                for (int j = 0; j < nums; j++) {
//                    read_annotation(reader, as[i].annotations + j);
//                }
//            }
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeVisibleAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_visible_annotations_num = readu2(reader);
//            method->runtime_visible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(method->runtime_visible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, method->runtime_visible_annotations + i);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) { // ignore
//            u2 num = method->runtime_invisible_annotations_num = readu2(reader);
//            method->runtime_invisible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(method->runtime_invisible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, method->runtime_invisible_annotations + i);
//            }
            bcr_skip(reader, attr_len);
        } else if (S(AnnotationDefault) == attr_name) { // ignore
//            struct element_value ev;
//            read_element_value(reader, &ev);
            bcr_skip(reader, attr_len);
        } else {
            // unknown attribute
            bcr_skip(reader, attr_len);
        }
    }

    if (IS_NATIVE(method->access_flags)) {
        method->max_stack = 4;  // todo 本地方法帧的操作数栈至少要能容纳返回值， 为了简化代码，暂时给maxStack字段赋值为4
        method->max_locals = method->arg_slot_count; // todo 因为本地方法帧的局部变量表只用来存放参数值，所以把argSlotCount赋给maxLocals字段刚好。

        size_t code_len = 2;
//        VM_MALLOCS(u1, code_len, code);
        u1 *code = vm_malloc(code_len * sizeof(u1));
        code[0] = OPC_INVOKENATIVE;
        const char *t = strchr(method->descriptor, ')'); // find return
        if (t == NULL) {
            //todo error
            printvm("method's descriptor 格式不对：%s\n", method->descriptor);
        }

        ++t;
        if (*t == 'V') {
            code[1] = 0xb1; // return
        } else if (*t == 'D') {
            code[1] = 0xaf; // dreturn
        } else if (*t == 'F') {
            code[1] = 0xae; // freturn
        } else if (*t == 'J') {
            code[1] = 0xad; // lreturn
        } else if (*t == 'L' || *t == '[') {
            code[1] = 0xb0; // areturn
        } else {
            code[1] = 0xac; // ireturn
        }

        method->code = code;
        method->code_length = code_len;

        method->native_method = find_native_method(method->clazz->class_name, method->name, method->descriptor);
    }
}

void method_release(Method *m)
{
    if (m == NULL) {
        // todo
        return;
    }

    // todo
}

bool method_is_accessible_to(const Method *method, const Class *visitor)
{
    // todo  实现对不对

    if (!class_is_accessible_to(method->clazz, visitor)) {
        return false;
    }

    if (method->clazz == visitor || IS_PUBLIC(method->access_flags))  // todo 对不对
        return true;

    if (IS_PRIVATE(method->access_flags)) {
        return false;
    }

    // 字段是protected，则只有 子类 和 同一个包下的类 可以访问
    if (IS_PROTECTED(method->access_flags)) {
        return class_is_subclass_of(visitor, method->clazz)
               || utf8_equals(method->clazz->pkg_name, visitor->pkg_name);
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return utf8_equals(method->clazz->pkg_name, visitor->pkg_name);
}

int method_get_line_number(const Method *method, int pc)
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

    for (int i = method->line_number_tables_count - 1; i >= 0; i--) {
        if (pc >= method->line_number_tables[i].start_pc)
            return method->line_number_tables[i].line_number;
    }
    return -1;
}

int method_find_exception_handler(Method *method, Class *exception_type, size_t pc)
{
    for (int i = 0; i < method->exception_tables_count; i++) {
        const ExceptionTable *t = method->exception_tables + i;
        if (t->start_pc <= pc && pc < t->end_pc) {
            if (t->catch_type == NULL) {
                return t->handler_pc;  // catch all
            }
            if (class_is_subclass_of(exception_type, t->catch_type)) {
                return t->handler_pc;
            }
        }
    }

    return -1;
}

char *method_to_string(const Method *method)
{
#define MAX_LEN 1023 // big enough
    char *result = vm_malloc(sizeof(char)*(MAX_LEN + 1));

    if (method != NULL) {
        int n = snprintf(result, MAX_LEN,
                         "method%s: %s~%s~%s", IS_NATIVE(method->access_flags) ? "(native)" : "",
                         method->clazz->class_name, method->name, method->descriptor);
        if (n < 0) {
            jvm_abort("snprintf 出错\n"); // todo
        }
        assert(0 <= n && n <= MAX_LEN);
        result[n] = 0;
    } else {
        strcpy(result, "method: NULL");
    }

    return result;
}
