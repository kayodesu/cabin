/*
 * Author: Jia Yang
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include "jclass.h"
#include "../../classfile/attribute.h"

/*
 * 异常处理表
 * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
 * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
 */
struct exception_table {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    struct jclass *catch_type;
};

struct jmethod {
    /*
     * jclass 表示定义此方法的类。
     * 对于一个子类，如果有一个方法是从父类中继承而来的，
     * 那么此方法的 jclass 依然指向父类，即真正定义此方法的类。
     */
    struct jclass *jclass;
    const char *name;
    const char *descriptor;

    struct jobject *parameter_types; // "[Ljava/lang/Class;"
    struct jobject *return_type;     // java/lang/Class
    struct jobject *exception_types; // "[Ljava/lang/Class;"

    u2 access_flags;
    u2 max_stack;
    u2 max_locals;
    u2 arg_slot_count;

    u2 exception_tables_count;
    /*
     * start_pc 给出的是try{ }语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
     * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
     */
    struct exception_table *exception_tables;

    u4 line_number_table_count;
    struct line_number_table *line_number_tables;

    u1 *code;
    size_t code_length;

    bool deprecated;
    const char *signature;

#if 0
    // 此方法可能会抛出的受检异常
    char *checked_exceptions;
    u2 checked_exceptions_num;

    struct parameter_annotation *runtime_visible_parameter_annotations;
    u2 runtime_visible_parameter_annotations_num;

    struct parameter_annotation *runtime_invisible_parameter_annotations;
    u2 runtime_invisible_parameter_annotations_num;

    struct annotation *runtime_visible_annotations;
    u2 runtime_visible_annotations_num;

    struct annotation *runtime_invisible_annotations;
    u2 runtime_invisible_annotations_num;
#endif
};

struct bytecode_reader;

void jmethod_init(struct jmethod *method, struct jclass *c, struct bytecode_reader *reader);

bool jmethod_is_accessible_to(const struct jmethod *method, const struct jclass *visitor);

// 查找 pc 所对应的行号
int jmethod_get_line_number(const struct jmethod *method, int pc);

/*
 * parse method descriptor and create parameter_types and return_type of the method
 * 
 * 返回 return type.
 */
struct jobject* jmethod_parse_descriptor(struct classloader *loader,
                                         const char *method_descriptor,
                                         struct jobject **parameter_types_add,
                                         int parameter_num_max);

struct jobject* jmethod_get_parameter_types(struct jmethod *method);
struct jobject* jmethod_get_return_type(struct jmethod *method);
struct jobject* jmethod_get_exception_types(struct jmethod *method);

/*
 * @pc, 发生异常的位置
 */
int jmethod_find_exception_handler(struct jmethod *method, struct jclass *exception_type, size_t pc);

char *jmethod_to_string(const struct jmethod *method);

void jmethod_release(struct jmethod *m);

#endif //JVM_JMETHOD_H
