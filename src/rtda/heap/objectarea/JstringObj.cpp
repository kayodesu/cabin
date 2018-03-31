/*
 * Author: Jia Yang
 */

#include "JstringObj.h"
#include "JarrayObj.h"
#include "../methodarea/Jfield.h"

/*
 * 在class文件中，字符串是以MUTF8格式保存的，这一点在3.3.7
节讨论过。在Java虚拟机运行期间，字符串以java.lang.String（后面
简称String）对象的形式存在，而在String对象内部，字符串又是以
UTF16格式保存的。字符串相关功能大部分都是由String（和
StringBuilder等）类提供的，本节只实现一些辅助功能即可。

 注意，这里其实是跳过了String的构造函数，
直接用hack的方式创建实例。在前面分析过String类的代码，这样做
虽然有点投机取巧，但确实是没有问题的。
 */
JstringObj::JstringObj(ClassLoader *loader, const jstring &str)
        : Jobject(loader->loadClass("java/lang/String")) {
    size_t len = str.length();
    JarrayObj *jchararr = new JarrayObj(loader->loadClass("[C"), len);
    for (size_t i = 0; i < len; i++) {
        jchararr->set(i, str[i]); // todo  这样不对，暂时先这样做
    }

// todo 要不要调用 <clinit>, <init>方法。
//        jobject *jstring = jobject_create(c);


// 给 java/lang/String 类的 value 变量赋值  todo
    for (int i = 0; i < jclass->fieldsCount; i++) {
        if (!jclass->fields[i].isStatic()
            && jclass->fields[i].descriptor == "[C"
            && jclass->fields[i].name == "value") {
            Jvalue v;
            v.r = jchararr;
            setInstanceFieldValue(jclass->fields[i].id, v);
        }
    }
}
