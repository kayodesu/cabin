/*
 * Author: Jia Yang
 */

#ifndef JVM_FIELDVALUES_H
#define JVM_FIELDVALUES_H

#include "../slot.h"

struct jclass;

/*
 * 保存一组变量的值。
 * 具体的说就是一个类的所有类变量或一个对象的所有实例变量的值。
 * 类型一的数据占一个Slot，类型二的数据占两个Slot。
 */
struct fieldvalues {
    struct jclass *jclass;

    int fields_count;
    struct slot values[];
};

struct fieldvalues* fv_create(struct jclass *jclass, int fields_count);

void fv_set_by_id(struct fieldvalues *fv, int id, const struct slot *value);

/*
 * set filed value by name and type
 */
void fv_set_by_nt(struct fieldvalues *fv,
                           const char *name, const char *descriptor, const struct slot *value);

const struct slot* fv_get_by_id(const struct fieldvalues *fv, int id);

/*
 * get the point of filed value by name and type
 */
const struct slot* fv_get_by_nt(const struct fieldvalues *fv, const char *name, const char *descriptor);


void fv_detroy(struct fieldvalues *values);

#endif //JVM_FIELDVALUES_H
