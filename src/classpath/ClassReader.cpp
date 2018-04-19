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

using namespace std;


ClassReader::Content ClassReader::readClassFromJar(
        const string &jarPath, const string &className) throw(IoException, ClassNotFindException) {
    unz_global_info64 globalInfo;
    unz_file_info64 fileInfo;

    unzFile jarFile = unzOpen64(jarPath.c_str());
    if (jarFile == NULL) {
        // todo error
        jprintf("unzOpen64 failed: %s\n", jarPath);
        throw IoException("unzOpen64 failed: " + jarPath);
    }
    if (unzGetGlobalInfo64(jarFile, &globalInfo) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        throw IoException("unzGetGlobalInfo64 failed: " + jarPath);
    }

    if (unzGoToFirstFile(jarFile) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jarFile);
        jprintf("unzGoToFirstFile failed: %s\n", jarPath);
        throw IoException("unzGoToFirstFile failed: " + jarPath);
    }

    for (int i = 0; i < globalInfo.number_entry; i++) {
        char fileName[PATH_MAX];
        if (unzGetCurrentFileInfo64(jarFile, &fileInfo, fileName, sizeof(fileName), NULL, 0, NULL, 0) != UNZ_OK) {
            unzClose(jarFile);
            throw IoException(string("unzGetCurrentFileInfo64 failed: ") + fileName);
        }

        char *p = strrchr(fileName, '.');
        if (p != NULL && strcmp(p, ".class") == 0) {
            *p = 0; // 去掉后缀
            if (strcmp(fileName, className.c_str()) == 0) {
                // find out!
                if (unzOpenCurrentFile(jarFile) != UNZ_OK) {
                    // todo error
                    unzClose(jarFile);
                    throw IoException("unzOpenCurrentFile failed: " + jarPath);
                }

                unsigned int uncompressedSize = static_cast<unsigned int>(fileInfo.uncompressed_size);
                auto bytecode = new s1[uncompressedSize];//malloc(sizeof(char) * c.bytecode_len);
//                int len = unzReadCurrentFile(jarFile, bytecode, uncompressedSize);
                if (unzReadCurrentFile(jarFile, bytecode, uncompressedSize) != uncompressedSize) {
                    // todo error
                    jprintf("error. unzReadCurrentFile\n");
                    unzCloseCurrentFile(jarFile);
                    unzClose(jarFile);
                    throw IoException("unzReadCurrentFile failed: " + jarPath);
                }
                unzCloseCurrentFile(jarFile);
                unzClose(jarFile);
                return Content(bytecode, uncompressedSize);
            }
        }
        int t = unzGoToNextFile(jarFile);
        if (t == UNZ_END_OF_LIST_OF_FILE) {
            break;
        }
        if (t != UNZ_OK) {
            // todo error
            unzClose(jarFile);
            string msg = "unzGoToNextFile failed: " + jarPath;
            jprintf(msg.c_str());
            throw IoException(msg);
        }
    }

    throw ClassNotFindException(className);
}

ClassReader::Content ClassReader::readClassFromDir(const string &__dir, const string &className)
                                                    throw(IoException, ClassNotFindException) {
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
                        try {
                            return readClassFromJar(abspath, className);
                        } catch (ClassNotFindException &e) {
                            // need not do any thing
                            // 在此jar包内没找到，再其他jar包继续寻找即可
                        }
                    } else if (abspath.compare(index, string::npos, ".class") == 0) {
                        // className不带.class后缀，而abspath有.class后缀
                        auto i = abspath.rfind(className + ".class");
                        if (i != string::npos) {
                            return Content(abspath.c_str());
                        }
                    }
                }
            }
        }
        closedir(dir);
    }

    throw ClassNotFindException(className);
}

ClassReader::Content ClassReader::readClass(const string &className) throw(IoException, ClassNotFindException) {
    try {
        return readClassFromDir(JvmEnv::bootstrapClasspath, className);
    } catch (ClassNotFindException &e) {
        try {
            return readClassFromDir(JvmEnv::extensionClasspath, className);
        } catch (ClassNotFindException &e) {
            return readClassFromDir(JvmEnv::userClasspath, className);
        }
    }
}