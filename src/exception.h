#ifndef CABIN_EXCEPTION_H
#define CABIN_EXCEPTION_H

#include <exception>
#include <string>
#include <cassert>
#include <utility>
#include "symbol.h"

class Object;

struct UncaughtException: public std::exception {
    Object *java_excep;
    explicit UncaughtException(Object *java_excep): java_excep(java_excep) { }
};

struct JavaException: public std::exception {
    const char *excep_class_name = nullptr;
    Object *excep = nullptr;
    std::string msg;

    explicit JavaException(const char *excep_class_name): excep_class_name(excep_class_name)
    {
        assert(excep_class_name != nullptr);
    }

    explicit JavaException(const char *excep_class_name, std::string msg)
        : excep_class_name(excep_class_name), msg(std::move(msg))
    {
        assert(excep_class_name != nullptr);
    }

    Object *getExcep();
};

#define DEF_EXCEP_CLASS(ClassName) \
    struct ClassName: public JavaException {\
        ClassName(): JavaException(S(ClassName)) { }\
        explicit ClassName(std::string msg): JavaException(S(ClassName), std::move(msg)) { }\
    }

DEF_EXCEP_CLASS(java_lang_ArrayStoreException);
DEF_EXCEP_CLASS(java_lang_UnknownError);
DEF_EXCEP_CLASS(java_lang_ArrayIndexOutOfBoundsException);
DEF_EXCEP_CLASS(java_lang_ArithmeticException);
DEF_EXCEP_CLASS(java_lang_ClassNotFoundException);
DEF_EXCEP_CLASS(java_lang_InternalError);
DEF_EXCEP_CLASS(java_lang_IncompatibleClassChangeError);
DEF_EXCEP_CLASS(java_lang_IllegalAccessError);
DEF_EXCEP_CLASS(java_lang_AbstractMethodError);
DEF_EXCEP_CLASS(java_lang_InstantiationException);
DEF_EXCEP_CLASS(java_lang_NegativeArraySizeException);
DEF_EXCEP_CLASS(java_lang_NullPointerException);
DEF_EXCEP_CLASS(java_lang_ClassCastException);
DEF_EXCEP_CLASS(java_lang_ClassFormatError);
DEF_EXCEP_CLASS(java_lang_LinkageError);
DEF_EXCEP_CLASS(java_lang_NoSuchFieldError);
DEF_EXCEP_CLASS(java_lang_NoSuchMethodError);
DEF_EXCEP_CLASS(java_lang_IllegalArgumentException);
DEF_EXCEP_CLASS(java_lang_CloneNotSupportedException);
DEF_EXCEP_CLASS(java_lang_VirtualMachineError);
DEF_EXCEP_CLASS(java_io_IOException);
DEF_EXCEP_CLASS(java_io_FileNotFoundException);

#undef DEF_EXCEP_CLASS

void printStackTrace(Object *e);

#endif //CABIN_EXCEPTION_H
