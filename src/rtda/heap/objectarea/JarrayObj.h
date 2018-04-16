/*
 * Author: Jia Yang
 */

#ifndef JVM_JARRAY_H
#define JVM_JARRAY_H

#include "../../../jvm.h"
#include "Jobject.h"

class JarrayObj: public Jobject {
    jint len; // len of array
    void *data;

    jint eleSize;  // size of single element
    void calEleSize();
public:

    // 创建一维数组对象
    JarrayObj(Jclass *jclass, jint arrLen);
    // 创建多维数组对象
    JarrayObj(Jclass *jclass, jint arrDim, const jint *arrLens);

//    virtual const Jvalue& getInstanceFieldValue(int id) = delete;
//    virtual const Jvalue& getInstanceFieldValue(std::string name, std::string descriptor) = delete;
//    virtual void setInstanceFieldValue(int id, const Jvalue &v) = delete;
//    virtual void setInstanceFieldValue(std::string name, std::string descriptor, const Jvalue &v) = delete;

    /*
     * 判断两个数组是否是同一类型的数组
     * todo 这里的判断略简陋
     */
    static bool isSameType(const JarrayObj &one, const JarrayObj &other) {
        return one.eleSize == other.eleSize;
    }

    static void copy(JarrayObj &dest, jint destPos, const JarrayObj &src, jint srcPos, jint len) {
        /*
         * 首先确保src和dest都是数组，然后检查数组类型。
         * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
         */
        if (!isSameType(src, dest)) {
            // todo error  ArrayStoreException
            jprintf("ArrayStoreException\n");
        }

        auto eleSize = src.eleSize;

        if (srcPos < 0
            || destPos < 0
            || len < 0
            || srcPos + len > src.length()
            || destPos + len > dest.length()) {
            // todo "java.lang.IndexOutOfBoundsException"
            jprintf("java.lang.IndexOutOfBoundsException\n");
        }

        auto d = static_cast<u1 *>(dest.data);
        auto s = static_cast<u1 *>(src.data);
        memcpy(d + destPos * eleSize, s + srcPos * eleSize, len * eleSize);
    }

    jint length() const {
        return len;
    }

//    template <typename T>
//    T& operator[](int index) { // 左值
//
//    }

    template <typename T>
    void set(jint index, const T t) { // 左值
        assert(index >= 0);
        assert(index < len);
        ((T *)data)[index] = t;
    }

    template <typename T>
    const T& get(jint index) const { // 右值
        assert(index >= 0);
        assert(index < len);
        return ((T *)data)[index];
    }

    bool checkBounds(int index) const {
        return 0 <= index && index < len;
    }
};

#endif //JVM_JARRAY_H
