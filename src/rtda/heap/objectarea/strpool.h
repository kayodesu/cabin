/*
 * Author: Jia Yang
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

#include "JstringObj.h"

JstringObj* putStrToPool(ClassLoader *loader, const jstring &str);

JstringObj* getStrFromPool(ClassLoader *loader, const jstring &str);

#endif //JVM_STRPOOL_H
