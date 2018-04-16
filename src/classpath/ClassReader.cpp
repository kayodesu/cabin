/*
 * Author: Jia Yang
 */

#include <stack>
#include <dirent.h>  // todo 平台适配问题
#include <sys/stat.h>// todo 平台适配问题
#include <algorithm>
#include <cstring>
#include "ClassReader.h"
#include "../../lib/zlib/minizip/unzip.h"
#include "../jvm.h"

using namespace std;

ClassReader::Content ClassReader::invalidContent;

ClassReader::Content ClassReader::readClassFromJar(const string &jarPath, const string &className) {
    unz_global_info64 globalInfo;
    unz_file_info64 fileInfo;

    unzFile jarFile = unzOpen64(jarPath.c_str());
    if (jarFile == NULL) {
        // todo error
        jprintf("error. unzOpen64\n");
        return invalidContent;
    }
    if (unzGetGlobalInfo64(jarFile, &globalInfo) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        jprintf("error. unzGetGlobalInfo64\n");
        return invalidContent;
    }

    if (unzGoToFirstFile(jarFile) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        jprintf("error. unzGoToFirstFile\n");
        return invalidContent;
    }

    for (int i = 0; i < globalInfo.number_entry; i++) {
        char file_name[PATH_MAX];
        if (unzGetCurrentFileInfo64(jarFile, &fileInfo, file_name, sizeof(file_name), NULL, 0, NULL, 0) != UNZ_OK) {
            //throw “文件错误”;
            unzClose(jarFile);
            return invalidContent;
        }

        char *p = strrchr(file_name, '.');
        if (p != NULL && strcmp(p, ".class") == 0) {
            *p = 0; // 去掉后缀
            if (strcmp(file_name, className.c_str()) == 0) {
                // find out!
                if (unzOpenCurrentFile(jarFile) != UNZ_OK) {
                    // todo error
                    jprintf("error. unzOpenCurrentFile\n");
                    unzClose(jarFile);
                    return invalidContent;
                }
                Content c;
                c.tag = Content::BYTECODE;
                c.bytecodeLen = fileInfo.uncompressed_size;
                c.bytecode = new s1[c.bytecodeLen];//malloc(sizeof(char) * c.bytecode_len);
                int len = unzReadCurrentFile(jarFile, c.bytecode, c.bytecodeLen);
                if (len != c.bytecodeLen) {
                    // todo error
                    jprintf("error. unzReadCurrentFile\n");
                    unzCloseCurrentFile(jarFile);
                    unzClose(jarFile);
                    return invalidContent;
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
            return invalidContent;
        }
    }

    return invalidContent;
}

ClassReader::Content ClassReader::readClassFromDir(const string &__dir, const string &className) {
    stack<string> dirs;
    if (!__dir.empty())
        dirs.push(__dir);

    while (!dirs.empty()) {
        const string path = dirs.top();
        dirs.pop();

        DIR *dir = opendir(path.c_str());
        if (dir == NULL) {
            jprintf("error. %s\n", path.c_str());
        }
        struct dirent *entry;
        struct stat statbuf;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
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
                        Content c = readClassFromJar(abspath, className);
                        if (c.tag != Content::INVALID) {
                            return c;
                        }
                    } else if (abspath.compare(index, string::npos, ".class") == 0) {
                        // className不带.class后缀，而abspath有.class后缀
                        auto i = abspath.rfind(className + ".class");
                        if (i != string::npos) {
                            Content c;
                            c.tag = Content::PATH;
                            strcpy(c.classFilePath, abspath.c_str());
                            return c;
                        }
                    }
                }
            }
        }
        closedir(dir);
    }

    return invalidContent;
}

ClassReader::Content ClassReader::readClass(const string &class_name) {
    Content c = readClassFromDir(JvmEnv::bootstrapClasspath, class_name);
    if (c.tag != Content::INVALID) {
        return c;
    }

    Content c1 = readClassFromDir(JvmEnv::extensionClasspath, class_name);
    if (c1.tag != Content::INVALID) {
        return c1;
    }

    return readClassFromDir(JvmEnv::userClasspath, class_name);
}