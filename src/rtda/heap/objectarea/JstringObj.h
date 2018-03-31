/*
 * Author: Jia Yang
 */

#ifndef JVM_JSTRINGOBJ_H
#define JVM_JSTRINGOBJ_H


#include "Jobject.h"
#include "JarrayObj.h"

class JstringObj: public Jobject {
public:
    // 根据str构造 java.lang.String 对象。
    JstringObj(ClassLoader *loader, const jstring &str);

    explicit JstringObj(ClassLoader *loader): Jobject(loader->loadClass("java/lang/String")) {};

    jstring value() {
        auto v = static_cast<JarrayObj *>(getInstanceFieldValue("value", "[C").r);
        jstring s;
        for (int i = 0; i < v->length(); i++) {
            s.push_back(v->get<jchar>(i));
        }
        return s;
    }

//    std::string toString() const override {
//        return jstrToStr(value());
//    }
};


#endif //JVM_JSTRINGOBJ_H
