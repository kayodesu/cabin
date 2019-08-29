/*
 * Author: kayo
 */

#include "Member.h"
#include "Class.h"
#include "../../utf8.h"

using namespace std;

bool Member::isAccessibleTo(const Class *visitor) const
{
    // todo  实现对不对

    if (!clazz->isAccessibleTo(visitor))
        return false;

    if (clazz == visitor or isPublic())  // todo 对不对
        return true;

    if (isPrivate())
        return false;

    // 字段是protected，则只有 子类 和 同一个包下的类 可以访问
    if (isProtected()) {
        return visitor->isSubclassOf(clazz) || utf8_equals(clazz->pkgName, visitor->pkgName);
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return utf8_equals(clazz->pkgName, visitor->pkgName);
}

const string descriptorToClassName(const char *descriptor)
{
    assert(descriptor != nullptr);

    if (*descriptor == '[') { // array
        return descriptor;
    } else if (*descriptor == 'L') { // non array Object
        string s = ++descriptor;
        return s.substr(0, s.length() - 1); // don't include the last ';'
    } else { // primitive
        return primitiveDescriptor2className(*descriptor);
    }
}

//ClassObject *descriptorToType(ClassLoader *loader, const char *descriptor)
//{
//    assert(loader != nullptr);
//    assert(descriptor != nullptr);
//
//    return loader->loadClass(descriptorToClassName(descriptor).c_str())->clsobj;
//}
