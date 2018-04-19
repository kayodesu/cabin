/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASSREADER_H
#define JVM_CLASSREADER_H

#include <string>
#include "../jvm.h"
#include "../exception/IoException.h"
#include "../exception/ClassNotFindException.h"

class ClassReader {
public:
    struct Content {
        static const int BYTECODE = 0;
        static const int PATH = 1;

        int tag;

        s1 *bytecode;
        size_t bytecodeLen;

        char classFilePath[PATH_MAX];

        Content(s1 *bytecode_, size_t bytecodeLen_): tag(BYTECODE), bytecode(bytecode_), bytecodeLen(bytecodeLen_){};

        Content(const char *classFilePath_): tag(PATH) {
            strcpy(classFilePath, classFilePath_);
        };
    };

private:
    Content readClassFromJar(const std::string &jar_path, const std::string &class_name) throw(IoException, ClassNotFindException) ;

    // className 不能有.class后缀
    Content readClassFromDir(const std::string &__dir, const std::string &className) throw(IoException, ClassNotFindException);

public:
    Content readClass(const std::string &class_name) throw(IoException, ClassNotFindException) ;
};


#endif //JVM_CLASSREADER_H
