/*
 * Author: kayo
 */

#include <cstdlib>
#include "exceptions.h"
#include "interpreter/interpreter.h"
#include "rtda/heap/StringObject.h"
#include "loader/bootstrap_class_loader.h"
#include "rtda/ma/Class.h"

#define EXCEPTION_EXIT (-1)

[[noreturn]] void raiseException(const char *exceptionName, const char *msg)
{
    assert(exceptionName != nullptr);
    Class *c = bootClassLoader->loadClass(exceptionName);
    c->clinit();

    jref o = Object::newInst(c);
    if (msg == nullptr) {
        execJavaFunc(c->getConstructor("()V"), o);
    } else {
        execJavaFunc(c->getConstructor("(Ljava/lang/String;)V"), { (slot_t) o, (slot_t) StringObject::newInst(msg) });
    }

    // public void printStackTrace()
    auto printStackTrace = c->lookupInstMethod("printStackTrace", "()V");
    execJavaFunc(printStackTrace, o);

    exit(EXCEPTION_EXIT);
}
