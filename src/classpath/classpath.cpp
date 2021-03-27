#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <minizip/unzip.h>
#include "classpath.h"
#include "../runtime/vm_thread.h"
#include "../exception.h"

using namespace std;
using namespace std::filesystem;

// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
vector<string> jre_lib_jars;
// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
vector<string> jre_ext_jars;

/*
 * 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
 *
 * 用户类路径的默认值是当前目录。可以设置CLASSPATH环境变量来修改用户类路径。
 * 可以通过 -cp 选项修改，
 * -cp 选项的优先级更高，可以覆盖CLASSPATH环境变量设置。
 * -cp 选项既可以指定目录，也可以指定JAR文件。
 */
//extern std::vector<std::string> userDirs;
//extern std::vector<std::string> userJars;

vector<string> g_jdk_modules;

static void findFilesBySuffix(const char *path0, const char *suffix, vector<std::string> &result)
{
    path curr_path(path0);
    if (!exists(curr_path)) {
        // todo error
        return;
    }

    directory_entry entry(curr_path);
    if (entry.status().type() != file_type::directory) {
        // todo error
        return;
    }

    directory_iterator files(curr_path);
    for (auto& f: files) {
        if (f.is_regular_file()) {
            char abspath[PATH_MAX];
            // sprintf 和 snprintf 会自动在加上字符串结束符'\0'
            sprintf(abspath, "%s/%s", path0, f.path().filename().string().c_str()); // 绝对路径

            char *tmp = strrchr(abspath, '.');
            if (tmp != nullptr && strcmp(++tmp, suffix) == 0)
                result.emplace_back(abspath);
        }
    }
}

static char bootstrap_classpath[PATH_MAX] = { 0 };
static char classpath[PATH_MAX] = { 0 };

void setClasspath(const char *cp)
{
    assert(cp != nullptr);
    strcpy(classpath, cp);
}

const char *getClasspath()
{
    return classpath;
}

void setBootstrapClasspath(const char *bcp)
{
    assert(bcp != nullptr);
    strcpy(bootstrap_classpath, bcp);
}

void initClasspath()
{
    assert(!g_java_home.empty());

    if (IS_JDK9_PLUS) { // jdk9 开始使用模块
        findFilesBySuffix((g_java_home + "/jmods").c_str(), "jmod", g_jdk_modules);

        // 第0个位置放java.base.jmod，因为java.base.jmod常用，所以放第0个位置首先搜索。
        for (auto iter = g_jdk_modules.begin(); iter != g_jdk_modules.end(); iter++) {
            auto i = iter->rfind('\\');
            auto j = iter->rfind('/');
            if ((i != iter->npos && iter->compare(i + 1, 6, "java.base.jmod") == 0)
                || (j != iter->npos && iter->compare(j + 1, 6, "java.base.jmod") == 0)) {
                std::swap(*(g_jdk_modules.begin()), *iter);
                break;
            }
        }
    } else {
        char extension_classpath[PATH_MAX] = { 0 };
//    char user_classpath[PATH_MAX] = { 0 };

        // parse bootstrap classpath
        if (bootstrap_classpath[0] == 0) { // empty
            strcpy(bootstrap_classpath, g_java_home.c_str());
            strcat(bootstrap_classpath, "/lib");
        }

        findFilesBySuffix(bootstrap_classpath, "jar", jre_lib_jars);

        // 第0个位置放rt.jar，因为rt.jar常用，所以放第0个位置首先搜索。
        for (auto iter = jre_lib_jars.begin(); iter != jre_lib_jars.end(); iter++) {
            auto i = iter->rfind('\\');
            auto j = iter->rfind('/');
            if ((i != iter->npos && iter->compare(i + 1, 6, "rt.jar") == 0)
                || (j != iter->npos && iter->compare(j + 1, 6, "rt.jar") == 0)) {
                std::swap(*(jre_lib_jars.begin()), *iter);
                break;
            }
        }

        // parse extension classpath
        if (extension_classpath[0] == 0) {  // empty
            strcpy(extension_classpath, bootstrap_classpath);
            strcat(extension_classpath, "/ext");
        }

        findFilesBySuffix(extension_classpath, "jar", jre_ext_jars);
    }

    if (classpath[0] == 0) {  // empty
        char *cp = getenv("CLASSPATH");
        if (cp != nullptr) {
            strcpy(classpath, cp);
        } else {
            // todo error. no CLASSPATH！
            JVM_PANIC("error. no CLASSPATH！");
        }
    }
//    else {
//        const char *delim = ";"; // 各个path以分号分隔
//        char *path = strtok(user_classpath, delim);
//        while (path != nullptr) {
//            const char *suffix = strrchr(path, '.');
//            if (suffix != nullptr && strcmp(suffix, ".jar") == 0) { // jar file
//                userJars.emplace_back(path);
//            } else { // directory
//                userDirs.emplace_back(path);
//            }
//            path = strtok(nullptr, delim);
//        }
//    }
}

enum ClassLocation {
    IN_JAR,
    IN_MODULE
};

/*
 * @param class_name: xxx/xxx/xxx
 */
static optional<pair<u1 *, size_t>> readClass(const char *path,
                                              const char *class_name, ClassLocation location)
{
    unzFile module_file = unzOpen64(path);
    if (module_file == nullptr) {
        throw java_io_IOException(string("unzOpen64 failed: ") + path);
    }

    if (unzGoToFirstFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        throw java_io_IOException(string("unzGoToFirstFile failed: ") + path);
    }

    char buf[strlen(class_name) + 32]; // big enough
    if (location == IN_JAR) {
        strcat(strcpy(buf, class_name), ".class");
    } else if (location == IN_MODULE) {
        // All classes 放在 module 的 "classes" 目录下
        strcat(strcat(strcpy(buf, "classes/"), class_name), ".class");
    } else {
        JVM_PANIC("never goes here."); // todo
    }
// typedef int (*unzFileNameComparer)(unzFile file, const char *filename1, const char *filename2);
//    int k = unzLocateFile(module_file, buf, 1);
    int k  = unzLocateFile(module_file, buf,
                           [](unzFile file, const char *filename1, const char *filename2) {
                               return strcmp(filename1, filename2);
                           });
    if (k != UNZ_OK) {
        // not found
        unzClose(module_file);
        return nullopt;
    }

    // find out!
    if (unzOpenCurrentFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        throw java_io_IOException(string("unzOpenCurrentFile failed: ") + path);
    }

    unz_file_info64 file_info{ };
    unzGetCurrentFileInfo64(module_file, &file_info, buf, sizeof(buf), nullptr, 0, nullptr, 0);

    auto bytecode = new u1[file_info.uncompressed_size];
    int size = unzReadCurrentFile(module_file, bytecode, (unsigned int) (file_info.uncompressed_size));
    unzCloseCurrentFile(module_file);
    unzClose(module_file);
    if (size != (int) file_info.uncompressed_size) {
        throw java_io_IOException(string("unzReadCurrentFile failed: ") + path);
    }
    return make_pair(bytecode, file_info.uncompressed_size);
}

optional<pair<u1 *, size_t>> readBootClass(const utf8_t *class_name)
{
    assert(class_name != nullptr);
//    assert(isSlashName(class_name));
    assert(class_name[0] != '['); // don't load array class

    if (IS_JDK9_PLUS) {
        for (auto &mod : g_jdk_modules) {
            auto content = readClass(mod.c_str(), class_name, IN_MODULE);
            if (content.has_value()) { // find out
                return content;
            }
        }
    } else {
        for (auto &jar : jre_lib_jars) {
            auto content = readClass(jar.c_str(), class_name, IN_JAR);
            if (content.has_value()) { // find out
                return content;
            }
        }
    }

    return nullopt;
}
