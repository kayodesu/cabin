/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASSREADER_H
#define JVM_CLASSREADER_H

#include <string>
#include "../jvmtype.h"

class ClassReader {
public:
    struct Content {
        static const int BYTECODE = 0;
        static const int PATH = 1;
        static const int INVALID = -1;

        int tag;

        s1 *bytecode;
        size_t bytecodeLen;

        char classFilePath[PATH_MAX];

        Content(): tag(INVALID), bytecode(nullptr) {}
    };

private:
    static Content invalidContent;

    Content readClassFromJar(const std::string &jar_path, const std::string &class_name);

    // className 不能有.class后缀
    Content readClassFromDir(const std::string &__dir, const std::string &className);

public:
    Content readClass(const std::string &class_name);
};


#endif //JVM_CLASSREADER_H
