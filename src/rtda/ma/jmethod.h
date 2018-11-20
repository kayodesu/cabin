/*
 * Author: Jia Yang
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include "jclass.h"
#include "../../classfile/attribute.h"

struct jmethod {
    /*
     * jclass 表示定义此方法的类。
     * 对于一个子类，如果有一个方法是从父类中继承而来的，
     * 那么此方法的 jclass 依然指向父类，即真正定义此方法的类。
     */
    struct jclass *jclass;
    const char *name;
    const char *descriptor;

    u2 access_flags;
    u2 max_stack;
    u2 max_locals;
    u2 arg_slot_count;

    u2 exception_tables_count;
    struct exception_table {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        struct jclass *catch_type;
    } *exception_tables;

    u4 line_number_table_count;
    struct line_number_table *line_number_tables;

    s1 *code;
    size_t code_length;
};

struct jmethod* jmethod_create(const struct jclass *c, const struct member_info *info);

bool jmethod_is_accessible_to(const struct jmethod *method, const struct jclass *visitor);

int jmethod_find_exception_handler(struct jmethod *method, struct jclass *exception_type, size_t pc);

char *jmethod_to_string(const struct jmethod *method);

void jmethod_destroy(struct jmethod *m);

#endif //JVM_JMETHOD_H
