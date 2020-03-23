/*
 * Author: kayo
 */

#ifndef KAYOVM_THROWABLES_H
#define KAYOVM_THROWABLES_H

#include <exception>
#include <cassert>
#include <sstream>
#include "../jtypes.h"
#include "../symbol.h"

class Object;

class Throwable: public std::exception {
    Object *javaThrowable = nullptr;

public:
    explicit Throwable(Object *javaException): javaThrowable(javaException)
    {
        assert(this->javaThrowable != nullptr);
    }

    explicit Throwable(Class *ec, const utf8_t *msg = nullptr);

    explicit Throwable(const utf8_t *exceptionName, const utf8_t *msg = nullptr);

    Object *getJavaThrowable()
    {
        return javaThrowable;
    }

    void printStackTrace();
};

#define DefineThrowableClass(ClassName, fullClassName) \
struct ClassName: public Throwable { \
    explicit ClassName(const utf8_t *msg = nullptr): Throwable(fullClassName, msg) { } \
}

/* package java.lang */
DefineThrowableClass(ClassNotFoundException,         S(java_lang_ClassNotFoundException));
DefineThrowableClass(CloneNotSupportedException,     S(java_lang_CloneNotSupportedException));
DefineThrowableClass(IndexOutOfBoundsException,      S(java_lang_IndexOutOfBoundsException));
DefineThrowableClass(ArrayIndexOutOfBoundsException, S(java_lang_ArrayIndexOutOfBoundsException));
DefineThrowableClass(UnknownError,                   S(java_lang_UnknownError));
DefineThrowableClass(InternalError,                  S(java_lang_InternalError));
DefineThrowableClass(IllegalAccessError,             S(java_lang_IllegalAccessError));
DefineThrowableClass(AbstractMethodError,            S(java_lang_AbstractMethodError));
DefineThrowableClass(ArrayStoreException,            S(java_lang_ArrayStoreException));
DefineThrowableClass(IncompatibleClassChangeError,   S(java_lang_IncompatibleClassChangeError));
DefineThrowableClass(InstantiationException,         S(java_lang_InstantiationException));
DefineThrowableClass(NullPointerException,           S(java_lang_NullPointerException));
DefineThrowableClass(NoSuchFieldError,               S(java_lang_NoSuchFieldError));
DefineThrowableClass(NoSuchMethodError,              S(java_lang_NoSuchMethodError));
DefineThrowableClass(NegativeArraySizeException,     S(java_lang_NegativeArraySizeException));
DefineThrowableClass(ClassCastException,             S(java_lang_ClassCastException));
DefineThrowableClass(ClassFormatError,               S(java_lang_ClassFormatError));
DefineThrowableClass(StackOverflowError,             S(java_lang_StackOverflowError));
DefineThrowableClass(IllegalArgumentException,       S(java_lang_IllegalArgumentException));

/* package java.io */
DefineThrowableClass(IOException,           S(java_io_IOException));
DefineThrowableClass(FileNotFoundException, S(java_io_FileNotFoundException));

#undef DefineThrowableClass

#endif //KAYOVM_THROWABLES_H
