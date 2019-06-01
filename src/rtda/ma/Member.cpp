/*
 * Author: Jia Yang
 */

#include "Member.h"
#include "Class.h"
#include "../../utf8.h"

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