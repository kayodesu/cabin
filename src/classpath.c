#include <dirent.h>
#include <minizip/unzip.h>
#include "cabin.h"


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

void set_classpath(const char *cp)
{
    assert(cp != NULL);
    strcpy(classpath, cp);
}

const char *get_classpath()
{
    return classpath;
}

void set_bootstrap_classpath(const char *bcp)
{
    assert(bcp != NULL);
    strcpy(bootstrap_classpath, bcp);
}

void init_classpath()
{
    assert(g_java_home[0] != 0);

    char home[PATH_MAX];
    snprintf(home, PATH_MAX, "%s/jmods", g_java_home);
    findFilesBySuffix(home, "jmod", jdk_modules);

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
THROW_JAVA_EXCEPTION
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

    u1 *bytecode = vm_malloc(sizeof(u1) * file_info.uncompressed_size);  // new u1[file_info.uncompressed_size];
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

    for (char **modules = jdk_modules; *modules != NULL; modules++) {
        u1 *bytecode = read_class(*modules, class_name, IN_MODULE, bytecode_len);
        if (bytecode != NULL) { // find out
            return bytecode;
        }
    }

    return NULL;
}
