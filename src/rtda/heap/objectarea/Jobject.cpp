/*
 * Author: Jia Yang
 */

#include <algorithm>
#include "../../../loader/ClassLoader.h"
#include "Jobject.h"
#include "../methodarea/Jfield.h"

using namespace std;


Jobject::Jobject(Jclass *jclass): instanceFields(nullptr) {
//        jobject *obj = heap_malloc(sizeof(jobject) + sizeof(jvm_value) * class->instance_field_count);
    this->jclass = jclass;

    /*
     * Java要求类成员变量必须用默认值初始化！
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
    if (!jclass->isArray()) {
        instanceFields = new Jvalue[jclass->instanceFieldCount]();
    }
}

//void Jobject::setInstanceField(int id) {
//    assert(id >= 0 and id < jclass->instanceFieldCount);
//    return instanceFields[id];
//}

const Jvalue& Jobject::getInstanceFieldValue(int id) {
    if (id < 0 or id >= jclass->instanceFieldCount) {
        jvmAbort("%d\n", id);
    }
    return instanceFields[id];
}

const Jvalue& Jobject::getInstanceFieldValue(std::string name, std::string descriptor) {
    Jfield *field = jclass->lookupField(name, descriptor);
    if (field == nullptr) {
        // todo
        jvmAbort("%s, %s", name.c_str(), descriptor.c_str());
    }
    return getInstanceFieldValue(field->id);
}

void Jobject::setInstanceFieldValue(int id, const Jvalue &v) {
    if (id < 0 or id >= jclass->instanceFieldCount) {
        jvmAbort("%d\n", id);
    }

    instanceFields[id] = v;
    // todo long double
}

void Jobject::setInstanceFieldValue(string name, string descriptor, const Jvalue &v) {
    Jfield *field = jclass->lookupField(name, descriptor);
    if (field == nullptr) {
        // todo
        jvmAbort("");
    }
    setInstanceFieldValue(field->id, v);
}

bool Jobject::isInstanceOf(const Jclass *jclass) const {
    if (jclass == nullptr)
        return false;
    return this->jclass->isSubclassOf(jclass);
}

string Jobject::toString() const {
    return jclass->toString();
}

