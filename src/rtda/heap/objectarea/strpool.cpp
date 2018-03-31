/*
 * Author: Jia Yang
 */


#include "Jobject.h"
#include "JstringObj.h"

using namespace std;

static map<jstring, JstringObj *> strPool;

JstringObj* putStrToPool(ClassLoader *loader, const jstring &str) {
    auto iter = strPool.find(str);
    if (iter != strPool.end()) {
        return iter->second;
    }

    auto strObj =  new JstringObj(loader, str);
    strPool.insert(make_pair(str, strObj));
    return strObj;
}

JstringObj* getStrFromPool(ClassLoader *loader, const jstring &str) {
    auto iter = strPool.find(str);
    return iter == strPool.end() ? putStrToPool(loader, str) : iter->second;
}
