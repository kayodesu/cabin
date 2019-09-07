/*
 * Author: kayo
 */

#ifndef KAYOVM_ARRAYCLASS_H
#define KAYOVM_ARRAYCLASS_H

#include "Class.h"

/*
 * Array Class 由vm生成。
 */
class ArrayClass: public Class {
    size_t eleSize = 0;
    Class *compClass = nullptr; // component class
public:
    explicit ArrayClass(const char *className);

//    Object *newInst() = delete;

    // 判断数组单个元素的大小
    // 除了基本类型的数组外，其他都是引用类型的数组
    // 多维数组是数组的数组，也是引用类型的数组
    size_t getEleSize();

    /*
     * Returns the representing the component class of an array class.
     * If this class does not represent an array class this method returns null.
     */
    Class *componentClass();

    /*
      * 是否是基本类型的数组（当然是一维的）。
      * 基本类型
      * bool, byte, char, short, int, float, long, double
      * 分别对应的数组类型为
      * [Z,   [B,   [C,   [S,    [I,  [F,    [J,   [D
      */
    bool isPrimitiveArray() const
    {
        if (strlen(className) != 2 || className[0] != '[')
            return false;

        return strchr("ZBCSIFJD", className[1]) != nullptr;
    }
};

#endif //KAYOVM_ARRAYCLASS_H
