/*
 * Author: kayo
 */

#include <cstdlib>
#include "throwables.h"
#include "../interpreter/interpreter.h"
#include "Class.h"
#include "class_loader.h"
#include "../runtime/Thread.h"

Throwable::Throwable(const utf8_t *exceptionName, const utf8_t *msg)
{
    assert(exceptionName != nullptr);

    Class *ec = loadBootClass(exceptionName);
    assert(ec != nullptr);
    initClass(ec);
//    ec->clinit();

    javaThrowable = newObject(ec);
    if (msg == nullptr) {
        execJavaFunc(ec->getConstructor("()V"), javaThrowable);
    } else {
        execJavaFunc(ec->getConstructor("(Ljava/lang/String;)V"), javaThrowable, newString(msg));
    }
}

void Throwable::printStackTrace()
{
    assert(javaThrowable != nullptr);

    Method *printStackTrace = javaThrowable->clazz->lookupInstMethod(S(printStackTrace), S(___V));
    execJavaFunc(printStackTrace, javaThrowable);
}

