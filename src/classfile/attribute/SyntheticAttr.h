/*
 * Author: Jia Yang
 */

#ifndef JVM_SYNTHETICATTR_H
#define JVM_SYNTHETICATTR_H

#include "Attribute.h"

/*
 * 如果一个类成员没有在源文件中出现，则必须标记带有Synthetic属性，或者设置ACC_SYNTHETIC标志。
 * 唯一的例外是某些与人工实现无关的、由编译器自动产生的方法，
 * 也就是说，Java编程语言的默认的实例初始化方法（无参数的实例初始化方法）、类初始化方法，
 * 以及Enum.values()和Enum.valueOf()等方法是不用使用Synthetic属性或ACC_SYNTHETIC标记的。
 */
struct SyntheticAttr: public Attribute {
    explicit SyntheticAttr(BytecodeReader &reader): Attribute(reader) {}
};

#endif //JVM_SYNTHETICATTR_H
