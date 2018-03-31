/*
 * Author: Jia Yang
 */

#include "Jmember.h"

using namespace std;

bool Jmember::isAccessibleTo(const Jclass *visitor) const {
    if (jclass == visitor || isPublic())  // todo 对不对
        return true;

    if (isPrivate()) {
        return false;
    }

    // 字段是protected，则只有 子类 和 同一个包下的类 可以访问
    if (isProtected()) {
        return visitor->isSubclassOf(jclass) || jclass->pkgName == visitor->pkgName;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return jclass->pkgName == visitor->pkgName;
}
