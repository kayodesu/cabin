#include "cabin.h"
#include "attributes.h"
#include "jni.h"


typedef struct exception_table ExceptionTable;

void exception_table_init(ExceptionTable *et, Class *clazz, BytecodeReader *r)
{
    et->start_pc = bcr_readu2(r);
    et->end_pc = bcr_readu2(r);
    et->handler_pc = bcr_readu2(r);
    u2 index = bcr_readu2(r);
    if (index == 0) {
        // 异常处理项的 catch_type 有可能是 0。
        // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
        et->catch_type = NULL;
    } else {
        et->catch_type = vm_malloc(sizeof(struct exception_catch_type)); //new exception_table::CatchType;
        if (cp_get_type(&clazz->cp, index) == JVM_CONSTANT_ResolvedClass) {
            et->catch_type->resolved = true;
            et->catch_type->clazz = resolve_class(&clazz->cp, index);
        } else {
            // Note:
            // 不能在这里load class，有形成死循环的可能。
            // 比如当前方法是 Throwable 中的方法，而此方法又抛出了 Throwable 子类的Exception（记为A），
            // 而此时 Throwable 还没有构造完成，所以无法构造其子类 A。
            et->catch_type->resolved = false;
            et->catch_type->class_name = cp_class_name(&clazz->cp, index);
        }
    }
}

jarrRef get_parameter_types(const Method *m)
{
    assert(m != NULL);
    jarrRef ptypes;
    parse_method_descriptor(m->descriptor, m->clazz->loader, &ptypes, NULL);
    return ptypes;
}

jclsRef get_return_type(const Method *m)
{
    assert(m != NULL);
    jref rtype;
    parse_method_descriptor(m->descriptor, m->clazz->loader, NULL, &rtype);
    return rtype;
}

jarrRef get_exception_types(const Method *m)
{
    assert(m != NULL);

    int count = 0;
    Class *types[m->exception_tables_len];  // big enough
    for (u2 i = 0; i < m->exception_tables_len; i++) {
        ExceptionTable *t = m->exception_tables + i;
        if (t->catch_type == NULL)
            continue;

        if (!t->catch_type->resolved) {
            t->catch_type->clazz = load_class(m->clazz->loader, t->catch_type->class_name);
            t->catch_type->resolved = true;
        }
        types[count++] = t->catch_type->clazz;
    }

    Class *ac = load_class(m->clazz->loader, S(array_java_lang_Class));
    jarrRef exception_types = alloc_array(ac, count);
    for (int i = 0; i < count; i++)
        array_set_ref(exception_types, i, types[i]->java_mirror);

    return exception_types;
}

u2 cal_method_args_slots_count(const utf8_t *descriptor, bool is_static)
{
    assert(descriptor != NULL);
    u2 count = 0;

    const char *b = strchr(descriptor, '(');
    const char *e = strchr(descriptor, ')');
    if (b == NULL || e == NULL) {
        JVM_PANIC("error. %s\n", descriptor);
    }

    while (++b < e) {
        if (*b == 'B' || *b == 'C' || *b == 'I' || *b == 'F' || *b == 'S'|| *b == 'Z'/* boolean */) {
            count++;
        } else if (*b == 'D' || *b == 'J'/* long */) {
            count += 2;
        } else if (*b == 'L') { // reference
            count++;
            b = strchr(b, ';');
            if (b == NULL) {
                JVM_PANIC("error. %s\n", descriptor);
            }
        } else if (*b == '[') { // array reference
            count++;
            while (*(++b) == '[');

            if (*b == 'L') {
                b = strchr(b, ';');
                if (b == NULL) {
                    JVM_PANIC("error. %s\n", descriptor);
                }
            }
        }
    }

    if (!is_static) { // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
        count++; // this reference
    }
    return count;
}

// static void calArgsSlotsCount(Method *m)
// {
//     // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
//     m->arg_slot_count = calArgsSlotsCount(m->descriptor, m->isStatic());
// }

/*
 * 解析方法的 code 属性
 */
static void parse_code_attr(Method *m, BytecodeReader *r)
{
    m->max_stack = bcr_readu2(r);
    m->max_locals = bcr_readu2(r);
    m->code_len = bcr_readu4(r);
    // m->code = bcr_curr_pos(r);
    m->code = vm_malloc(m->code_len);
    memcpy(m->code, bcr_curr_pos(r), m->code_len);
    bcr_skip(r, m->code_len);

    // parse exception tables
    m->exception_tables_len = bcr_readu2(r);
    BUILD_ARRAY(m->exception_tables, m->exception_tables_len, exception_table_init, m->clazz, r);
    // m->exception_tables = (ExceptionTable *) vm_malloc(m->exception_tables_len * sizeof(*m->exception_tables));
    // for (int i = 0; i < m->exception_tables_len; i++) {
    //     exception_table_init(m->exception_tables + i, m->clazz, r);
    //     // m->exception_tables.emplace_back(m->clazz, r);
    // }

    // parse attributes of code's attribute
    u2 attr_count = bcr_readu2(r);
    for (int k = 0; k < attr_count; k++) {
        const char *attr_name = cp_utf8(&m->clazz->cp, bcr_readu2(r));
        u4 attr_len = bcr_readu4(r);

        if (S(LineNumberTable) == attr_name) {
            m->line_number_tables_count = bcr_readu2(r);
            BUILD_ARRAY(m->line_number_tables, m->line_number_tables_count, line_number_table_init, r);
        } else if (S(StackMapTable) == attr_name) {
            bcr_skip(r, attr_len); // todo ....
        } else if (S(LocalVariableTable) == attr_name) {
            m->local_variable_tables_count = bcr_readu2(r);
            BUILD_ARRAY(m->local_variable_tables, m->local_variable_tables_count, local_variable_table_init, r);
        } else if (S(LocalVariableTypeTable) == attr_name) {
            m->local_variable_type_tables_count = bcr_readu2(r);
            BUILD_ARRAY(m->local_variable_type_tables, m->local_variable_type_tables_count, local_variable_type_table_init, r);
        } else {
            // unknown attribute
            bcr_skip(r, attr_len);
        }
    }
}

void init_method(Method *m, Class *c, BytecodeReader *r)
{
    assert(m != NULL && c != NULL && r != NULL);

    memset(m, 0, sizeof(Method));
    m->clazz = c;
    ConstantPool *cp = &c->cp;

    m->access_flags = bcr_readu2(r);
    m->name = cp_utf8(cp, bcr_readu2(r));
    m->descriptor = cp_utf8(cp, bcr_readu2(r));
    u2 attr_count = bcr_readu2(r);

    m->itable_index = m->vtable_index = -1;

    // note: 构造函数（<init>方法）是非static的，也会传递this reference  todo
    m->arg_slot_count = cal_method_args_slots_count(m->descriptor, IS_STATIC(m));

    // parse method's attributes
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp_utf8(cp, bcr_readu2(r));
        u4 attr_len = bcr_readu4(r);

        if (S(Code) == attr_name) {
            parse_code_attr(m, r);
        } else if (S(Deprecated) == attr_name) {
            m->deprecated = true;
        } else if (S(Synthetic) == attr_name) {
            SET_SYNTHETIC(m);
        } else if (S(Signature) == attr_name) {
            m->signature = cp_utf8(cp, bcr_readu2(r));
        } else if (S(MethodParameters) == attr_name) {
            m->parameters_count = bcr_readu1(r); // 这里就是 u1，不是u2
            BUILD_ARRAY(m->parameters, m->parameters_count, method_parameter_init, cp, r);
        } else if (S(Exceptions) == attr_name) {
            m->checked_exceptions_count = bcr_readu2(r);
            m->checked_exceptions = vm_malloc(sizeof(u2) * m->checked_exceptions_count);
            for (u2 j = 0; j < m->checked_exceptions_count; j++)
                m->checked_exceptions[j] = bcr_readu2(r);
        }
        //  else if (S(RuntimeVisibleParameterAnnotations) == attr_name) {
        //     u2 num = bcr_readu2(r);
        //     rt_visi_para_annos.resize(num);
        //     for (u2 j = 0; j < num; j++) {
        //         u2 numAnnos = bcr_readu2(r);
        //         for (u2 k = 0; k < numAnnos; k++)
        //             rt_visi_para_annos[j].emplace_back(r);
        //     }
        // } else if (S(RuntimeInvisibleParameterAnnotations) == attr_name) {
        //     u2 num = bcr_readu2(r);
        //     rt_invisi_para_annos.resize(num);
        //     for (u2 j = 0; j < num; j++) {
        //         u2 numAnnos = bcr_readu2(r);
        //         for (u2 k = 0; k < numAnnos; k++)
        //             rt_invisi_para_annos[j].emplace_back(r);
        //     }
        // } else if (S(RuntimeVisibleAnnotations) == attr_name) {
        //     u2 num = bcr_readu2(r);
        //     for (u2 j = 0; j < num; j++)
        //         rt_visi_annos.emplace_back(r);
        // } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
        //     u2 num = bcr_readu2(r);
        //     for (u2 j = 0; j < num; j++)
        //         rt_invisi_annos.emplace_back(r);
        // } else if (S(AnnotationDefault) == attr_name) {
        //     annotation_default.read(r);
        // } 
        else {
            // unknown attribute
            bcr_skip(r, attr_len);
        }
    }

    const char *t = strchr(m->descriptor, ')'); // find return
    assert(t != NULL);
    t++;
    if (*t == 'V') {
        m->ret_type = RET_VOID;
    } else if (*t == 'D') {
        m->ret_type = RET_DOUBLE;
    } else if (*t == 'F') {
        m->ret_type = RET_FLOAT;
    } else if (*t == 'J') {
        m->ret_type = RET_LONG;
    } else if (*t == 'L' || *t == '[') {
        m->ret_type = RET_REFERENCE;
    } else  if (*t == 'I') {
        m->ret_type = RET_INT;
    } else  if (*t == 'B') {
        m->ret_type = RET_BYTE;
    } else  if (*t == 'Z') {
        m->ret_type = RET_BOOL;
    } else  if (*t == 'C') {
        m->ret_type = RET_CHAR;
    } else  if (*t == 'S') {
        m->ret_type = RET_SHORT;
    } else {
        // todo error
        JVM_PANIC(m->descriptor);
    }

    if (IS_NATIVE(m)) {
        // 本地方法帧的操作数栈至少要能容纳返回值，
        // 4 slots are big enough.
        m->max_stack = 4;
        // 因为本地方法帧的局部变量表只用来存放参数值，
        // 所以把arg_slot_count赋给max_locals字段刚好。
        m->max_locals = m->arg_slot_count;

        m->code_len = 2;
        m->code = vm_malloc(sizeof(u1) * m->code_len);
        m->code[0] = JVM_OPC_invokenative;

        if (m->ret_type == RET_VOID) {
            m->code[1] = JVM_OPC_return;
        } else if (m->ret_type == RET_DOUBLE) {
            m->code[1] = JVM_OPC_dreturn;
        } else if (m->ret_type == RET_FLOAT) {
            m->code[1] = JVM_OPC_freturn;
        } else if (m->ret_type == RET_LONG) {
            m->code[1] = JVM_OPC_lreturn;
        } else if (m->ret_type == RET_REFERENCE) {
            m->code[1] = JVM_OPC_areturn;
        } else {
            m->code[1] = JVM_OPC_ireturn;
        }
    }
}

jint get_line_number(const Method *m, int pc) 
{
    // native函数没有字节码
    if (IS_NATIVE(m)) {
        return -2;
    }

    /*
     * 和源文件名一样，并不是每个方法都有行号表。
     * 如果方法没有行号表，自然也就查不到pc对应的行号，这种情况下返回–1
     todo
     */
    // 从后往前查
    for (u2 i = m->line_number_tables_count - 1; i >= 0; i--) {
        if (pc >= m->line_number_tables[i].start_pc)
            return m->line_number_tables[i].line_number;
    }
    return -1;
}

int find_exception_handler(const Method *m, Class *exceptionType, size_t pc)
{
    for (u2 i = 0; i < m->exception_tables_len; i++) {
        ExceptionTable *t = m->exception_tables + i;
        // jvms: The start pc is inclusive and end pc is exclusive
        if (t->start_pc <= pc && pc < t->end_pc) {
            if (t->catch_type == NULL)  // catch all
                return t->handler_pc;
            if (!t->catch_type->resolved) {
                t->catch_type->clazz = load_class(m->clazz->loader, t->catch_type->class_name);
                t->catch_type->resolved = true;
            }
            if (is_subclass_of(exceptionType, t->catch_type->clazz))
                return t->handler_pc;
        }
    }

    return -1;
}

bool is_signature_polymorphic(const Method *m)
{
    bool b = utf8_equals(m->clazz->class_name, S(java_lang_invoke_MethodHandle))
                 || utf8_equals(m->clazz->class_name, S(java_lang_invoke_VarHandle));
    if (!b)
        return false;

    jarrRef ptypes = get_parameter_types(m); // Class<?>[]
    if (ptypes->arr_len != 1)
        return false;

    jclsRef ptype = array_get(jclsRef, ptypes, 0);
    if (!utf8_equals(ptype->jvm_mirror->class_name, S(array_java_lang_Object))) 
        return false;

    if (!(IS_VARARGS(m) && IS_NATIVE(m)))
        return false;

    return true;
}

void release_method(Method *m)
{
    // todo
    // if (IS_NATIVE(m)) {
    //     delete[] code;
    // }
    // for (auto &t : exception_tables) {
    //     delete t.catch_type;
    // }
}

char *get_method_info(const Method *m)
{
    char *info = vm_calloc(sizeof(char) * INFO_MSG_MAX_LEN);
    snprintf(info, INFO_MSG_MAX_LEN - 1, "method%s: %s~%s~%s",
                IS_NATIVE(m) ? "(native)" : "",
                m->clazz->class_name, m->name, m->descriptor);
    return info;
}