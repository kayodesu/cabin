/*
 * Author: Jia Yang
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include <string>
#include <map>
#include <thread>
#include "../methodarea/Jclass.h"
#include "../heap.h"
#include "../../../Base.h"

class Jobject: public Base {
//    std::map<std::string, jvm_value> instanceFields;  // key == className~fieldName
    /*
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
    Jvalue *instanceFields;
protected:
    Jclass *jclass;
public:

    // 创建非数组对象
    explicit Jobject(Jclass *jclass);

    Jclass* getClass() {
        return jclass;
    }

    virtual const Jvalue& getInstanceFieldValue(int id);

    virtual const Jvalue& getInstanceFieldValue(std::string name, std::string descriptor);

    /*
     * @id: id of the field
     */
    virtual void setInstanceFieldValue(int id, const Jvalue &v);

    virtual void setInstanceFieldValue(std::string name, std::string descriptor, const Jvalue &v);

    bool isInstanceOf(const Jclass *jclass) const;

    Jobject* clone() const {
        jvmAbort("error clone\n");
        return nullptr;  // todo
    }

    std::string toString() const override;

    class Monitor {
//        Jthread *owner;
//        std::mutex mutex;
    };
};

#endif //JVM_JOBJECT_H
