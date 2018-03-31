/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <vector>
#include <string>
#include <set>
#include "../../../jvmtype.h"

class Jclass;
class JclassObj;

class ClassLoader {
    Jclass *jclassClass; // java.lang.Class 的类

    std::vector<Jclass *> loadedClasses;
    std::set<JclassObj *> jclassObjPool; // java.lang.Class类的对象池


    /*
     * void和基本类型的类名就是void、int、float等。
     * 基本类型的类没有超类，也没有实现任何接口。
     */
    Jclass* loadPrimitiveClasses(const std::string &className);

    // load array class
    Jclass* loadArrClass(const std::string &className);

    // load non array class
    Jclass* loadNonArrClass(const std::string &className);

public:
    ClassLoader();

    JclassObj* getJclassObjFromPool(const std::string &className);
    Jclass* loadClass(const std::string &className);

    void print();
};

#endif //JVM_CLASS_LOADER_H
