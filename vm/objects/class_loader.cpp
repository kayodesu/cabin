/*
 * Author: kayo
 */

#include <memory>
#include "class_loader.h"
#include "../symbol.h"
#include "../loader/ClassLoader.h"
#include "../rtda/ma/Class.h"
#include "../interpreter/interpreter.h"
#include "../../zlib/minizip/unzip.h"

using namespace std;

static unique_ptr<pair<u1 *, size_t>> read_class_from_jar(const char *jar_path, const char *class_name)
{
    unz_global_info64 global_info;
    unz_file_info64 file_info;

    unzFile jar_file = unzOpen64(jar_path);
    if (jar_file == NULL) {
        // todo error
        printvm("unzOpen64 failed: %s\n", jar_path);
        return nullptr;
    }
    if (unzGetGlobalInfo64(jar_file, &global_info) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jar_file);
        printvm("unzGetGlobalInfo64 failed: %s\n", jar_path);
        return nullptr;
    }

    if (unzGoToFirstFile(jar_file) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jar_file);
        printvm("unzGoToFirstFile failed: %s\n", jar_path);
        return nullptr;
    }

    for (unsigned long long int i = 0; i < global_info.number_entry; i++) {
        char file_name[PATH_MAX];
        if (unzGetCurrentFileInfo64(jar_file, &file_info, file_name, sizeof(file_name), NULL, 0, NULL, 0) != UNZ_OK) {
            unzClose(jar_file);
            printvm("unzGetCurrentFileInfo64 failed: %s\n", jar_path);
            return nullptr;
        }

        char *p = strrchr(file_name, '.');
        if (p != NULL && strcmp(p, ".class") == 0) {
            *p = 0; // 去掉后缀

            if (strcmp(file_name, class_name) == 0) {
                // find out!
                if (unzOpenCurrentFile(jar_file) != UNZ_OK) {
                    // todo error
                    unzClose(jar_file);
                    printvm("unzOpenCurrentFile failed: %s\n", jar_path);
                    return nullptr;
                }

                auto uncompressed_size = (size_t) file_info.uncompressed_size;
                auto bytecode = (u1 *) vm_malloc(sizeof(u1) * uncompressed_size);
                if (unzReadCurrentFile(jar_file, bytecode, (unsigned int) uncompressed_size) != uncompressed_size) {
                    // todo error
                    unzCloseCurrentFile(jar_file);  // todo 干嘛的
                    unzClose(jar_file);
                    printvm("unzReadCurrentFile failed: %s\n", jar_path);
                    return nullptr;
                }
                unzCloseCurrentFile(jar_file); // todo 干嘛的
                unzClose(jar_file);
                return make_unique<pair<u1 *, size_t>>(bytecode, uncompressed_size);
            }
        }

        int t = unzGoToNextFile(jar_file);
        if (t == UNZ_END_OF_LIST_OF_FILE) {
            break;
        }
        if (t != UNZ_OK) {
            // todo error
            unzClose(jar_file);
            printvm("unzGoToNextFile failed: %s\n", jar_path);
            return nullptr;
        }
    }

    unzClose(jar_file);
    return nullptr;
}

Class *loadSystemClass(const char *className)
{
    assert(className != nullptr);

    for (auto &jar : jreLibJars) {
        auto content = read_class_from_jar(jar.c_str(), className);
        if (content) { // find out
            return new Class(bootClassLoader, content->first, content->second);
        }
    }

    // todo error
    return nullptr;
}

Object *getSystemClassLoader()
{
    Class *scl = loadSystemClass(SYMBOL(java_lang_ClassLoader));
    if (scl == nullptr) {
        // todo
        return nullptr;
    }

    // public static ClassLoader getSystemClassLoader();
    Method *get = scl->getDeclaredStaticMethod(SYMBOL(getSystemClassLoader), SYMBOL(___java_lang_ClassLoader));
    return (Object *) *execJavaFunc(get);
}