/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASS_LOADER_H
#define JVM_CLASS_LOADER_H

#include <vector>
#include <string>
#include <set>

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

    /*
     * 类加载第1阶段：加载(loading)。
     * 在加载阶段虚拟机需要完成以下三件事：
     * 1. 通过一个类的全限定名来获取定义此类的二进制字节流。
     * 2. 将这个字节流所代表的静态存储结构转化为方法区的运行时数据结构。
     * 3. 在内存中生成一个代表此类的java.lang.Class对象，作为方法区这个类的各种数据的访问入口。
     */
    Jclass* loading(const std::string &className);

    /*
     * 类加载第2阶段：验证(verification)。
     */
    Jclass* verification(Jclass *jclass);

    /*
     * 类加载第3阶段：准备(preparation)。
     * 准备阶段是正式为类变量分配内存并设置类变量初始值的阶段，
     * 这些变量所使用的内存都将在方法区中进行分配，并用默认值初始化变量。
     *
     * 如果静态变量属于 基本类型 或 String类型，有final修饰符，且它的值在编译期已知，则该值存储在class文件常量池中。
     *
     * Java的类成员变量默认值
     * boolean: false
     * byte: 0
     * short: 0
     * char:'\u0000' 数值为0 ???? todo
     * int: 0
     * long: 0
     * float: 0.0
     * double: 0.0
     * 数组: null
     * 引用: null
     */
    Jclass* preparation(Jclass *jclass);

    /*
     * 类加载第4阶段：解析(resolution)。
     * 解析阶段是虚拟机将常量池内的符号引用替换为直接引用的过程。
     */
    Jclass* resolution(Jclass *jclass);

    /*
     * 类加载第5阶段：初始化(initialization)。
     * 初始化阶段是执行类构造器<clinit>的过程。
     */
    Jclass* initialization(Jclass *jclass);

public:
    ClassLoader();

    JclassObj* getJclassObjFromPool(const std::string &className);
    Jclass* loadClass(const std::string &className);

    void print();
};

#endif //JVM_CLASS_LOADER_H
