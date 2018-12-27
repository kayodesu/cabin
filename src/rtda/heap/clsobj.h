/*
 * Author: Jia Yang
 */

#ifndef JVM_CLSOBJ_H
#define JVM_CLSOBJ_H

#include "object.h"

// java/lang/Class 对象操作函数
/*
 * @entity_class: todo 说明
 */
struct object* clsobj_create(struct class *entity_class);

#endif //JVM_CLSOBJ_H
