/*
 * Author: Jia Yang
 */

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include "Jclass.h"
#include "../objectarea/JclassObj.h"
#include "../../../../lib/zlib/minizip/unzip.h"
#include "PrimitiveType.h"
#include "../../../classpath/ClassReader.h"

using namespace std;

JclassObj* ClassLoader::getJclassObjFromPool(const std::string &className) {
    auto iter = find_if(jclassObjPool.begin(), jclassObjPool.end(), [&](JclassObj *o) {
        return o->getClassName() == className;
    });

    if (iter != jclassObjPool.end()) {
        return *iter;
    }

    auto o = new JclassObj(jclassClass, className);
    jclassObjPool.insert(o);
    return o;
}

ClassLoader::ClassLoader(): jclassClass(nullptr) {
    /*
     * 先加载java.lang.Class类，
     * 这又会触发java.lang.Object等类和接口的加载。
     * 然后遍历classMap，给已经加载的每一个类关联类对象。
     */
    jclassClass = loadClass("java/lang/Class");

    for_each(loadedClasses.begin(), loadedClasses.end(), [=](Jclass *c) {
        c->classObj = getJclassObjFromPool(c->className);//new JclassObj(jclassClass, c->className);
    });


    for (int i = 0; i < sizeof(primitiveTypes)/sizeof(primitiveTypes[0]); i++) {
        loadPrimitiveClasses(primitiveTypes[i].name);
        loadClass(primitiveTypes[i].arrayClassName);
        loadClass(primitiveTypes[i].wrapperClassName);
    }
}

Jclass* ClassLoader::loadPrimitiveClasses(const string &className) {
    auto c = new Jclass;
    c->accessFlags = ACC_PUBLIC; // todo
    c->pkgName = ""; // todo 包名
    c->className = className;  // todo
    c->loader = this;
    c->isInited = true;
    c->superClass = nullptr;

    c->interfacesCount = 0;

    c->rtcp = nullptr;
    c->constantsCount = 0;

    c->methods = nullptr;
    c->methodsCount = 0;

    c->fields = nullptr;
    c->fieldsCount = 0;

    c->instanceFieldCount = 0;
    c->staticFieldCount = 0;
    c->staticFieldValues = nullptr;

    if (jclassClass != nullptr) {
        c->classObj = getJclassObjFromPool(c->className);//new JclassObj(jclassClass, className);
    }

    loadedClasses.push_back(c);
    return c;
}

/*
 * 字符串在class文件中是以MUTF-8（Modified UTF-8）方
式编码的。但为什么没有用标准的UTF-8编码方式，笔者没有找到
明确的原因 [1] 。MUTF-8编码方式和UTF-8大致相同，但并不兼容。
差别有两点：一是null字符（代码点U+0000）会被编码成2字节：
0xC0、0x80；二是补充字符（Supplementary Characters，代码点大于
U+FFFF的Unicode字符）是按UTF-16拆分为代理对（Surrogate Pair）
分别编码的。

 在Java虚拟机运行期间，字符串以java.lang.String（后面
简称String）对象的形式存在，而在String对象内部，字符串又是以
UTF16格式保存的。
 */

#if 0
#undef BYTECODE
#undef PATH
#undef INVALID

#define BYTECODE 0
#define PATH 1
#define INVALID -1

struct Content {
    int tag;

    s1 *bytecode;
    size_t bytecode_len;

    char class_file_path[PATH_MAX];
};

#define INVALID_CONTENT (Content){ .tag = INVALID }

Content read_class_from_jar(const string &jar_path, const string &class_name) {
    unz_global_info64 global_info;
    unz_file_info64 file_info;

    unzFile jarFile = unzOpen64(jar_path.c_str());
    if (jarFile == NULL) {
        // todo error
        jprintf("error. unzOpen64\n");
        return INVALID_CONTENT;
    }
    if (unzGetGlobalInfo64(jarFile, &global_info) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        jprintf("error. unzGetGlobalInfo64\n");
        return INVALID_CONTENT;
    }

    if (unzGoToFirstFile(jarFile) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        jprintf("error. unzGoToFirstFile\n");
        return INVALID_CONTENT;
    }

    for (int i = 0; i < global_info.number_entry; i++) {
        char file_name[PATH_MAX];
        if (unzGetCurrentFileInfo64(jarFile, &file_info, file_name, sizeof(file_name), NULL, 0, NULL, 0) != UNZ_OK) {
            //throw “文件错误”;
            unzClose(jarFile);
            return INVALID_CONTENT;
        }

        char *p = strrchr(file_name, '.');
        if (p != NULL && streq(p, ".class")) {
            *p = 0; // 去掉后缀
            if (streq(file_name, class_name.c_str())) {
                // find out!
                if (unzOpenCurrentFile(jarFile) != UNZ_OK) {
                    // todo error
                    jprintf("error. unzOpenCurrentFile\n");
                    unzClose(jarFile);
                    return INVALID_CONTENT;
                }
                Content c = { .tag = BYTECODE };
                c.bytecode_len = file_info.uncompressed_size;
                c.bytecode = new s1[c.bytecode_len];//malloc(sizeof(char) * c.bytecode_len);
                int len = unzReadCurrentFile(jarFile, c.bytecode, c.bytecode_len);
                if (len != c.bytecode_len) {
                    // todo error
                    jprintf("error. unzReadCurrentFile\n");
                    unzCloseCurrentFile(jarFile);
                    unzClose(jarFile);
                    return INVALID_CONTENT;
                }
                unzCloseCurrentFile(jarFile);
                unzClose(jarFile);
                return c;
            }
        }
        if (unzGoToNextFile(jarFile) != UNZ_OK) {
            // todo error
//            jprintf("error. unzGoToNextFile\n");
            unzClose(jarFile);
            return INVALID_CONTENT;
        }
    }

    return INVALID_CONTENT;
}

// className 不能有.class后缀
Content read_class_from_dir(const string &__dir, const string &className) {
    stack<string> dirs;
    dirs.push(__dir);

    while (!dirs.empty()) {
        const string &path = dirs.top();
        dirs.pop();

        DIR *dir = opendir(path.c_str());
        if (dir == NULL) {
            jprintf("error. %s\n", path);
        }
        struct dirent *entry;
        struct stat statbuf;
        while ((entry = readdir(dir)) != NULL) {
            if (streq(entry->d_name, ".") || streq(entry->d_name, "..")) {
                continue;
            }

            string abspath = path + "/" + entry->d_name;  // 绝对路径
            replace(abspath.begin(), abspath.end(), '\\', '/');

            lstat(abspath.c_str(), &statbuf);

            if (S_ISDIR(statbuf.st_mode)) {
                dirs.push(abspath);
            } else if (S_ISREG(statbuf.st_mode)) { // 常规文件
                auto index = abspath.rfind('.');
                if (index != string::npos) {
                    if (abspath.compare(index, string::npos, ".jar") == 0) {
                        Content c = read_class_from_jar(abspath, className);
                        if (c.tag != INVALID) {
                            return c;
                        }
                    } else if (abspath.compare(index, string::npos, ".class") == 0) {
                        // className不带.class后缀，而abspath有.class后缀
                        auto i = abspath.rfind(className + ".class");
                        if (i != string::npos) {
                            Content c = { .tag = PATH };
                            strcpy(c.class_file_path, abspath.c_str());
                            return c;
                        }
                    }
                }
            }
        }
        closedir(dir);
    }

    return INVALID_CONTENT;
}

static Content read_class(const string &class_name) {
    Content c = read_class_from_dir(bootstrapClasspath, class_name);
    if (c.tag != INVALID) {
        return c;
    }

    Content c1 = read_class_from_dir(extensionClasspath, class_name);
    if (c1.tag != INVALID) {
        return c1;
    }

    return read_class_from_dir(userClasspath, class_name);
}

#endif



Jclass* ClassLoader::loadArrClass(const std::string &className) {
    auto c = new Jclass;
    c->accessFlags = ACC_PUBLIC; // todo
    c->pkgName = ""; // todo 包名
    c->className = className;  // todo
    c->loader = this;
    c->isInited = true; // 数组类不需要初始化
    c->superClass = loadClass("java/lang/Object");

    c->interfacesCount = 2;
    c->interfaces = new Jclass* [2];
    c->interfaces[0] = loadClass("java/lang/Cloneable");
    c->interfaces[1] = loadClass("java/io/Serializable");

    c->rtcp = nullptr;
    c->constantsCount = 0;

    c->methods = nullptr;
    c->methodsCount = 0;

    c->fields = nullptr;
    c->fieldsCount = 0;

    c->instanceFieldCount = 0;
    c->staticFieldCount = 0;
    c->staticFieldValues = nullptr;

//    if (jclassClass != nullptr) {
//        c->classObj = new JclassObj(jclassClass, className);
//    }
//
//    loadedClasses.push_back(c);
    return c;
}

Jclass* ClassLoader::loadNonArrClass(const std::string &className) {
    ClassReader classReader;
    ClassReader::Content c = classReader.readClass(className);
    if (c.tag == ClassReader::Content::INVALID) {
        // todo ClassNotFoundException
        jvmAbort("Don't find class: %s\n", className.c_str());// todo
    }

    ClassFile *cf = nullptr;
    if (c.tag == ClassReader::Content::PATH) {
        cf = new ClassFile(c.classFilePath);
    } else if (c.tag == ClassReader::Content::BYTECODE) {
        cf = new ClassFile(c.bytecode, c.bytecodeLen);
    }

    if (cf == nullptr) {
        // todo
        jvmAbort("error");
//        return nullptr;
    }

    Jclass *jclass = new Jclass(this, cf);
    return jclass;
}

// class_name: class全路径名，不能有.class后缀。例如 java/lang/Object
Jclass* ClassLoader::loadClass(const string &className) {
    auto iter = find_if(loadedClasses.begin(), loadedClasses.end(), [&](Jclass *c) { return c->className == className; });
    if (iter != loadedClasses.end()) {
        return *iter;
    }

    Jclass *c;
    if (className[0] == '[') {
        c = loadArrClass(className);
    } else {
        c = loadNonArrClass(className);
    }

    if (c == nullptr) {
        jvmAbort("error");
    }

    if (jclassClass != nullptr) {
        c->classObj = getJclassObjFromPool(c->className);//new JclassObj(jclassClass, className);
    }

    loadedClasses.push_back(c);
    return c;
}

void ClassLoader::print() {
    for_each(loadedClasses.begin(), loadedClasses.end(),
             [](Jclass *c){ jprintf("%p, %p, %s\n", c, c->getClassObj(), c->className.c_str()); });
}
