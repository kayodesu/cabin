/*
 * Author: Jia Yang
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include "class.h"
#include "../../classfile/attribute.h"
#include "../../native/registry.h"

/*
 * 异常处理表
 * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
 * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
 */
struct exception_table {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    struct class *catch_type;
};

struct method {
    /*
     * class 表示定义此方法的类。
     * 对于一个子类，如果有一个方法是从父类中继承而来的，
     * 那么此方法的 class 依然指向父类，即真正定义此方法的类。
     */
    struct class *clazz;
    const char *name;
    const char *descriptor;
    int vtable_index;

    NO_ACCESS struct object *parameter_types; // "[Ljava/lang/Class;"
    NO_ACCESS struct object *return_type;     // java/lang/Class
    NO_ACCESS struct object *exception_types; // "[Ljava/lang/Class;"

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

    const u1 *code;
    size_t code_length;

    bool deprecated;
    const char *signature;

    native_method_t native_method; // present only if native
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

void method_init(struct method *method, struct class *c, struct bytecode_reader *reader);

bool method_is_accessible_to(const struct method *method, const struct class *visitor);

// 查找 pc 所对应的行号
int method_get_line_number(const struct method *method, int pc);

struct object* method_get_parameter_types(struct method *method);
struct object* method_get_return_type(struct method *method);
struct object* method_get_exception_types(struct method *method);

/*
 * @pc, 发生异常的位置
 */
int method_find_exception_handler(struct method *method, struct class *exception_type, size_t pc);

char *method_to_string(const struct method *method);

void method_release(struct method *m);

#endif //JVM_JMETHOD_H
