// #include <string>
// #include <vector>
// #include <optional>
// #include <filesystem>
#include <dirent.h>
#include <minizip/unzip.h>
#include "classpath.h"
#include "../runtime/vm_thread.h"
#include "../exception.h"

// using namespace std;
// using namespace std::filesystem;

#if 0
// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
static vector<string> jre_lib_jars;
// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
static vector<string> jre_ext_jars;
#endif

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

// static vector<string> jdk_modules;
#define JDK_MODULES_MAX_COUNT 512 // big enough
static char *jdk_modules[JDK_MODULES_MAX_COUNT] = { NULL };

static void findFilesBySuffix(const char *path, const char *suffix, char **result)
{
    // path curr_path(path);
    // if (!exists(curr_path)) {
    //     // todo error
    //     return;
    // }

    // directory_entry entry(curr_path);
    // if (entry.status().type() != file_type::directory) {
    //     // todo error
    //     return;
    // }

    // directory_iterator files(curr_path);
    // for (auto& f: files) {
    //     if (f.is_regular_file()) {
    //         char abspath[PATH_MAX];
    //         // sprintf 和 snprintf 会自动在加上字符串结束符'\0'
    //         sprintf(abspath, "%s/%s", path0, f.path().filename().string().c_str()); // 绝对路径

    //         char *tmp = strrchr(abspath, '.');
    //         if (tmp != NULL && strcmp(++tmp, suffix) == 0) {
    //             *result++ = strdup(abspath);
    //             // result.emplace_back(abspath);
    //         }
    //     }
    // }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        // todo error
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char abspath[PATH_MAX];
        // sprintf 和 snprintf 会自动在加上字符串结束符'\0'
        snprintf(abspath, PATH_MAX, "%s/%s", path, entry->d_name); // 绝对路径

        // check suffix
        char *tmp = strrchr(abspath, '.');
        if (tmp != NULL && strcmp(++tmp, suffix) == 0) {
            *result++ = strdup(abspath);
        }
    }
}

static char bootstrap_classpath[PATH_MAX] = { 0 };
static char classpath[PATH_MAX] = { 0 };

void setClasspath(const char *cp)
{
    assert(cp != NULL);
    strcpy(classpath, cp);
}

const char *getClasspath()
{
    return classpath;
}

void setBootstrapClasspath(const char *bcp)
{
    assert(bcp != NULL);
    strcpy(bootstrap_classpath, bcp);
}

void initClasspath()
{
    assert(g_java_home[0] != 0);

    if (IS_JDK9_PLUS) { // jdk9 开始使用模块
        // string home = g_java_home;
        char home[PATH_MAX];
        snprintf(home, PATH_MAX, "%s/jmods", g_java_home);
        findFilesBySuffix(home, "jmod", jdk_modules);

        // 第0个位置放java.base.jmod，因为java.base.jmod常用，所以放第0个位置首先搜索。
        // for (auto iter = jdk_modules.begin(); iter != jdk_modules.end(); iter++) {
        //     auto i = iter->rfind('\\');
        //     auto j = iter->rfind('/');
        //     if ((i != iter->npos && iter->compare(i + 1, 6, "java.base.jmod") == 0)
        //         || (j != iter->npos && iter->compare(j + 1, 6, "java.base.jmod") == 0)) {
        //         std::swap(*(jdk_modules.begin()), *iter);
        //         break;
        //     }
        // }
    } else {
        JVM_PANIC("do not support under jdk 9");
#if 0
        char extension_classpath[PATH_MAX] = { 0 };
//    char user_classpath[PATH_MAX] = { 0 };

        // parse bootstrap classpath
        if (bootstrap_classpath[0] == 0) { // empty
            strcpy(bootstrap_classpath, g_java_home);
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
#endif      
    }

    if (classpath[0] == 0) {  // empty
        char *cp = getenv("CLASSPATH");
        if (cp != NULL) {
            strcpy(classpath, cp);
        } else {
            // todo error. no CLASSPATH！
            JVM_PANIC("error. no CLASSPATH！");
        }
    }
//    else {
//        const char *delim = ";"; // 各个path以分号分隔
//        char *path = strtok(user_classpath, delim);
//        while (path != NULL) {
//            const char *suffix = strrchr(path, '.');
//            if (suffix != NULL && strcmp(suffix, ".jar") == 0) { // jar file
//                userJars.emplace_back(path);
//            } else { // directory
//                userDirs.emplace_back(path);
//            }
//            path = strtok(NULL, delim);
//        }
//    }
}

typedef enum ClassLocation {
    IN_JAR,
    IN_MODULE
} ClassLocation;

static int filename_compare_func(unzFile file, const char *filename1, const char *filename2)
{
    return strcmp(filename1, filename2);
}

/*
 * @param class_name: xxx/xxx/xxx
 */
static u1 *read_class(const char *path, const char *class_name, ClassLocation location, size_t *bytecode_len)
{
    assert(path != NULL && class_name != NULL && bytecode_len != NULL);

    unzFile module_file = unzOpen64(path);
    if (module_file == NULL) {
        // throw java_io_IOException(string("unzOpen64 failed: ") + path);
        raise_exception(S(java_io_IOException), NULL); // todo msg   
    }

    if (unzGoToFirstFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        // throw java_io_IOException(string("unzGoToFirstFile failed: ") + path);
        raise_exception(S(java_io_IOException), NULL); // todo msg   
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
    int k  = unzLocateFile(module_file, buf, filename_compare_func);
    if (k != UNZ_OK) {
        // not found
        unzClose(module_file);
        return NULL;
    }

    // find out!
    if (unzOpenCurrentFile(module_file) != UNZ_OK) {
        unzClose(module_file);
        // throw java_io_IOException(string("unzOpenCurrentFile failed: ") + path);
        raise_exception(S(java_io_IOException), NULL); // todo msg   
    }

    unz_file_info64 file_info;
    unzGetCurrentFileInfo64(module_file, &file_info, buf, sizeof(buf), NULL, 0, NULL, 0);

    u1 *bytecode = (u1 *) vm_malloc(sizeof(u1) * file_info.uncompressed_size);  // new u1[file_info.uncompressed_size];
    int size = unzReadCurrentFile(module_file, bytecode, (unsigned int) (file_info.uncompressed_size));
    unzCloseCurrentFile(module_file);
    unzClose(module_file);
    if (size != (int) file_info.uncompressed_size) {
        // throw java_io_IOException(string("unzReadCurrentFile failed: ") + path);
        raise_exception(S(java_io_IOException), NULL); // todo msg   
    }

    *bytecode_len = file_info.uncompressed_size;
    return bytecode;
    // return make_pair(bytecode, file_info.uncompressed_size);
}

u1 *read_boot_class(const utf8_t *class_name, size_t *bytecode_len)
{
    assert(class_name != NULL && bytecode_len != NULL);
//    assert(isSlashName(class_name));
    assert(class_name[0] != '['); // don't load array class

    if (IS_JDK9_PLUS) {
        // for (auto &mod : jdk_modules) {
        //     u1 *bytecode = read_class(mod.c_str(), class_name, IN_MODULE, bytecode_len);
        //     if (bytecode != NULL) { // find out
        //         return bytecode;
        //     }
        // }
        for (char **moudles = jdk_modules; *moudles != NULL; moudles++) {
            u1 *bytecode = read_class(*moudles, class_name, IN_MODULE, bytecode_len);
            if (bytecode != NULL) { // find out
                return bytecode;
            }
        }
    } else {
        JVM_PANIC("do not support under jdk 9");
#if 0
        for (auto &jar : jre_lib_jars) {
            auto content = readClass(jar.c_str(), class_name, IN_JAR);
            if (content.has_value()) { // find out
                return content;
            }
        }
#endif
    }

    return NULL;
}
