/*
 * Author: kayo
 */

#ifndef JVM_GC_H
#define JVM_GC_H

#include "../jtypes.h"

/*
 * 判断对象是否可达(GC Roots Analysis)
 *
 * 可作为GC Roots对象的包括如下几种：
    a.虚拟机栈(栈桢中的本地变量表)中的引用的对象
    b.方法区中的类静态属性引用的对象
    c.方法区中的常量引用的对象
    d.本地方法栈中JNI的引用的对象
 */
bool objectAccessible(jref obj);

#endif //JVM_GC_H
