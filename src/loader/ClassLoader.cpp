/*
 * Author: kayo
 */

#include <memory>
#include "../debug.h"
#include "../symbol.h"
#include "ClassLoader.h"
#include "../../zlib/minizip/unzip.h"
#include "../rtda/ma/Class.h"
#include "../rtda/ma/ArrayClass.h"
#include "../rtda/ma/Field.h"
#include "../rtda/heap/ClassObject.h"

#if TRACE_LOAD_CLASS
#define TRACE PRINT_TRACE
#else
#define TRACE(...)
#endif

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

static unique_ptr<pair<u1 *, size_t>> read_class_from_dir(const char *dir_path, const char *class_name)
{
    assert(dir_path != nullptr);
    assert(class_name != nullptr);

    char file_path[PATH_MAX];
    sprintf(file_path, "%s/%s.class", dir_path, class_name);

    FILE *f = fopen(file_path, "rb");
    if (f != NULL) { // find out
        fseek(f, 0, SEEK_END); //定位到文件末
        size_t file_len = (size_t) ftell(f); //文件长度

        u1 *bytecode = (u1 *) vm_malloc(sizeof(u1) * file_len);
        fseek(f, 0, SEEK_SET);
        fread(bytecode, 1, file_len, f);
        fclose(f);
        return make_unique<pair<u1 *, size_t>>(bytecode, file_len);
    }

    if (errno != ENOFILE) {
        // file is exist, but open failed
        jvm_abort("%s, %s\n", file_path, strerror(errno)); // todo
    }

    // not find
    return nullptr;
}

static unique_ptr<pair<u1 *, size_t>> read_class(const char *class_name)
{
    // search jre/lib
    for (auto &jar : jreLibJars) {
        auto content = read_class_from_jar(jar.c_str(), class_name);
        if (content) // find out
            return content;
    }

    // search jre/lib/ext
    for (auto &jar : jreExtJars) {
        auto content = read_class_from_jar(jar.c_str(), class_name);
        if (content) // find out
            return content;
    }

    // search user paths
    for (auto &dir : userDirs) {
        auto content = read_class_from_dir(dir.c_str(), class_name);
        if (content) // find out
            return content;
    }

    // search user jars
    for (auto &jar : userJars) {
        auto content = read_class_from_jar(jar.c_str(), class_name);
        if (content) // find out
            return content;
    }

    return nullptr; // not find
}

ClassLoader::ClassLoader()
{
    /*
     * 先加载java.lang.Class类，
     * 这又会触发java.lang.Object等类和接口的加载。
     */
//    jlClass = loadSysClass(S(java_lang_Class));

    // 加载基本类型（int, float, etc.）的 class
//    loadPrimitiveTypes();

    // todo
    //    for (auto &primitiveType : primitiveTypes) {
//        loadPrimitiveClasses(primitiveType.name);
//        loadClass(primitiveType.arrayClassName);
//        loadClass(primitiveType.wrapperClassName);
//    }

    // 给已经加载的每一个类关联类对象。
//    for (auto iter : loadedClasses) {
//        iter.second->clsobj = ClassObject::newInst(iter.second); // todo ClassObject可不可以重用，每次都要new吗？？
//    }

    // 缓存一下常见类
//    jlString = loadSysClass(S(java_lang_String));
//    jlClassArr = loadArrayClass(S(array_java_lang_Class));
//    jlObjectArr = loadArrayClass(S(array_java_lang_Object));
//    charArr = loadArrayClass(S(array_C));
}

void ClassLoader::putToPool(const char *className, Class *c)
{
    assert(className != nullptr);
    assert(strlen(className) > 0);
    assert(c != nullptr);
    loadedClasses.insert(make_pair(className, c));
}

Class *ClassLoader::loading(const char *className)
{
    auto content = read_class(className);
    if (!content) {
        jvm_abort("class not find: %s", className);
    }

    return new Class(this, content->first, content->second);
}

static Class* verification(Class *c)
{
    if (c->magic != 0xcafebabe) {
        jvm_abort("error. magic = %u(0x%x)", c->magic, c->magic);
    }

    // todo 验证版本号
    /*
     * Class版本号和Java版本对应关系
     * JDK 1.8 = 52
     * JDK 1.7 = 51
     * JDK 1.6 = 50
     * JDK 1.5 = 49
     * JDK 1.4 = 48
     * JDK 1.3 = 47
     * JDK 1.2 = 46
     * JDK 1.1 = 45
     */
    if (c->major_version != 52) {
        jvm_abort("only support jdk 8"); // todo class version
    }

    return c;
}

static Class* preparation(Class *c)
{
//    const struct rtcp* const rtcp = c->rtcp;

    // 如果静态变量属于基本类型或String类型，有final修饰符，
    // 且它的值在编译期已知，则该值存储在class文件常量池中。
    for (size_t i = 0; i < c->fields.size(); i++) {
        if (!c->fields[i]->isStatic()) {
            continue;
        }

//        const int index = c->fields[i]->constant_value_index;

        // 值已经在常量池中了
//        const bool b = (index != INVALID_CONSTANT_VALUE_INDEX);
// todo
//        c->setFieldValue(Field.id, Field.descriptor,
//        [&]() -> jint { return b ? rtcp->getInt(index) : 0; },  // todo byte short 等都是用 int 表示的吗
//        [&]() -> jfloat { return b ? rtcp->getFloat(index) : 0; },
//        [&]() -> jlong { return b ? rtcp->getLong(index) : 0; },
//        [&]() -> jdouble { return b ? rtcp->getDouble(index) : 0; },
//        [&]() -> jref {
//            if (field.descriptor == "Ljava/lang/String;" and b) {
//                // todo
//                const string &str = rtcp->getStr(index);
//                return JStringObj::newJStringObj(class->loader, strToJstr(str));
//            }
//            return nullptr;
//        });
    }

    return c;
}

/*
 * 解析（Resolution）是根据运行时常量池的符号引用来动态决定具体的值的过程。
 */
static Class* resolution(Class *c)
{
    // todo
    return c;
}

static Class* initialization(Class *c)
{
    // todo
    return c;
}

Class *ClassLoader::loadNonArrClass(const char *class_name)
{
// todo 解析，初始化是在这里进行，还是待使用的时候再进行
    Class *c = loading(class_name);
    return initialization(resolution(preparation(verification(c))));
}

Class *ClassLoader::loadClass(const char *className)
{
    assert(className != nullptr);
    auto iter = loadedClasses.find(className);
    if (iter != loadedClasses.end()) {
        TRACE("find loaded class (%s) from pool.", className);
        return iter->second;
    }

    Class *c = nullptr;
    if (className[0] == '[') {
        c = new ArrayClass(className);
    } else {
        c = loadNonArrClass(className);
    }

    if (c == nullptr) {
        VM_UNKNOWN_ERROR("loader class failed. %s", className);
        return nullptr;
    }

    if (java_lang_Class_class != nullptr) {
        c->clsobj = ClassObject::newInst(c);
    }

    assert(strcmp(className, c->className) == 0);
    auto res = loadedClasses.insert(make_pair(c->className, c));
    TRACE("load class (%s).", className);
    return c;
}

ClassLoader::~ClassLoader()
{

}
