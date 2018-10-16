/*
 * Author: Jia Yang
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include "../../slot.h"
#include "../fieldvalues.h"
#include "../ma/jclass.h"

struct jobject {
    /*
     * 对象头
     */
    struct objhead {

    } head;

    // 保存所有实例变量的值
    // 包括此Object中定义的和从父类继承来的。
    struct fieldvalues *instance_field_values;
//    struct slot *instance_fields_values;
    int instance_fields_count;

    struct jclass *jclass;

};

struct jobject* jobject_create(struct jclass *c);

void jobject_destroy(struct jobject *o);


bool jobject_is_instance_of(const struct jobject *o, const struct jclass *c);

//void jobject_set_field_value(struct jobject *o, int id, const struct slot *v);

/*
 * set filed value by name and type
 */
//void jobject_set_field_value_nt(struct jobject *o, const char *name, const char *descriptor, struct slot *v);

//struct slot* jobject_instance_field_value(struct jobject *o, int id);

/*
 * get the point of filed value by name and type
 */
//struct slot* jobject_instance_field_value_nt(struct jobject *o, const char *name, const char *descriptor);


#endif //JVM_JOBJECT_H
