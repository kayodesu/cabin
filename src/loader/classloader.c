/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <errno.h>
#include "classloader.h"
#include "minizip/unzip.h"
#include "../rtda/ma/class.h"
#include "../rtda/ma/access.h"
#include "../rtda/ma/field.h"
#include "../util/hashmap.h"
#include "../rtda/heap/object.h"
#include "../jtypes.h"
#include "../rtda/primitive_types.h"

struct classloader {
    struct hashmap *loaded_class_pool; // 保存 class *
//    struct hashmap *classobj_pool; // java.lang.Class 类的对象池，保存 object *

    // 缓存一下常见类
    struct class *jlclass; // java.lang.Class 的类
    struct class *jlstring; // java.lang.String 的类
};

struct class* classloader_get_jlclass(struct classloader *loader)
{
    assert(loader != NULL);
    assert(loader->jlclass != NULL);
    return loader->jlclass;
}

struct class* classloader_get_jlstring(struct classloader *loader)
{
    assert(loader != NULL);
    assert(loader->jlstring != NULL);
    return loader->jlstring;
}

struct bytecode_content {
    s1 *bytecode;
    size_t len;
};

static struct bytecode_content invalid_bytecode_content = { NULL, 0 };

#define IS_INVALID(bytecode_content) ((bytecode_content).bytecode == NULL || (bytecode_content).len == 0)

static struct bytecode_content read_class_from_jar(const char *jar_path, const char *class_name)
{
    unz_global_info64 global_info;
    unz_file_info64 file_info;

    unzFile jar_file = unzOpen64(jar_path);
    if (jar_file == NULL) {
        // todo error
        printvm("unzOpen64 failed: %s\n", jar_path);
        return invalid_bytecode_content;
    }
    if (unzGetGlobalInfo64(jar_file, &global_info) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jar_file);
        printvm("unzGetGlobalInfo64 failed: %s\n", jar_path);
        return invalid_bytecode_content;
    }

    if (unzGoToFirstFile(jar_file) != UNZ_OK) {
        // todo throw “文件错误”;
        unzClose(jar_file);
        printvm("unzGoToFirstFile failed: %s\n", jar_path);
        return invalid_bytecode_content;
    }

    for (int i = 0; i < global_info.number_entry; i++) {
        char file_name[PATH_MAX];
        if (unzGetCurrentFileInfo64(jar_file, &file_info, file_name, sizeof(file_name), NULL, 0, NULL, 0) != UNZ_OK) {
            unzClose(jar_file);
            printvm("unzGetCurrentFileInfo64 failed: %s\n", jar_path);
            return invalid_bytecode_content;
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
                    return invalid_bytecode_content;
                }

                size_t uncompressed_size = file_info.uncompressed_size;
                VM_MALLOCS(s1, uncompressed_size, bytecode);
                if (unzReadCurrentFile(jar_file, bytecode, (unsigned int) uncompressed_size) != uncompressed_size) {
                    // todo error
                    unzCloseCurrentFile(jar_file);  // todo 干嘛的
                    unzClose(jar_file);
                    printvm("unzReadCurrentFile failed: %s\n", jar_path);
                    return invalid_bytecode_content;
                }
                unzCloseCurrentFile(jar_file); // todo 干嘛的
                unzClose(jar_file);
                return (struct bytecode_content) { bytecode, uncompressed_size };
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
            return invalid_bytecode_content;
        }
    }

    unzClose(jar_file);
    return invalid_bytecode_content;
}

static struct bytecode_content read_class_from_dir(const char *dir_path, const char *class_name)
{
    assert(dir_path != NULL);
    assert(class_name != NULL);

    char file_path[PATH_MAX];
    sprintf(file_path, "%s/%s.class\0", dir_path, class_name);

    FILE *f = fopen(file_path, "rb");
    if (f != NULL) { // find out
        fseek(f, 0, SEEK_END); //定位到文件末
        size_t file_len = (size_t) ftell(f); //文件长度

        VM_MALLOCS(s1, file_len, bytecode);
        fseek(f, 0, SEEK_SET);
        fread(bytecode, 1, file_len, f);
        fclose(f);
        return (struct bytecode_content) { bytecode, file_len };
    }

    if (errno != ENOFILE) {
        // file is exist, but open failed
        jvm_abort("%s\n", strerror(errno)); // todo
    }

    // not find
    return invalid_bytecode_content;
}

static struct bytecode_content read_class(const char *class_name)
{
    // search jre/lib
    for (int i = 0; i < jre_lib_jars_count; i++) {
        struct bytecode_content content = read_class_from_jar(jre_lib_jars[i], class_name);
        if (!IS_INVALID(content)) // find out
            return content;
    }

    // search jre/lib/ext
    for (int i = 0; i < jre_ext_jars_count; i++) {
        struct bytecode_content content = read_class_from_jar(jre_ext_jars[i], class_name);
        if (!IS_INVALID(content)) // find out
            return content;
    }

    // search user paths
    for (int i = 0; i < user_dirs_count; i++) {
        struct bytecode_content content = read_class_from_dir(user_dirs[i], class_name);
        if (!IS_INVALID(content)) // find out
            return content;
    }

    // search user jars
    for (int i = 0; i < user_jars_count; i++) {
        struct bytecode_content content = read_class_from_jar(user_jars[i], class_name);
        if (!IS_INVALID(content)) // find out
            return content;
    }

    return invalid_bytecode_content; // not find
}

//static struct object* get_jclass_obj_from_pool(struct classloader *loader, const char *class_name)
//{
//    struct object *clsobj = hashmap_find(loader->classobj_pool, class_name);
////    HASH_FIND_STR(loader->classobj_pool, class_name, clsobj);
//    if (clsobj != NULL) {
//        printvm("find out class obj: %s\n", class_name);  /////////////////////////////////////
//        return clsobj;
//    }
//
//    clsobj = jclsobj_create(loader->jlclass, class_name);
//    hashmap_put(loader->classobj_pool, clsobj->c.class_name, clsobj);
////    HASH_ADD_KEYPTR(hh, loader->classobj_pool, class_name, strlen(class_name), clsobj);
//    return clsobj;
//}

//static void set_clsobj(struct classloader *loader, struct class *c)
//{
////    struct jclass *c = c0;
//    c->clsobj = get_jclass_obj_from_pool(loader, c->class_name);
//}

struct classloader* classloader_create(bool is_bootstrap_loader)
{
    VM_MALLOC(struct classloader, loader);
    if (is_bootstrap_loader) {
        g_bootstrap_loader = loader;
    }

    loader->loaded_class_pool = hashmap_create_str_key(false);
//    loader->classobj_pool = hashmap_create_str_key();

    /*
     * 先加载java.lang.Class类，
     * 这又会触发java.lang.Object等类和接口的加载。
     */
    loader->jlclass = NULL; // 不要胡乱删除这句。因为classloader_load_class中要判断loader->jclass_class是否为NULL。
    loader->jlclass = classloader_load_class(loader, "java/lang/Class");

    // 加载基本类型（int, float, etc.）的 class
    pt_load_primitive_types();
//    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
//        struct class *c = jclass_create_primitive_class(loader, primitive_types[i].class_name);
//        hashmap_put(loader->loaded_class_pool, primitive_types[i].class_name, c);
//    }

    // todo
    //    for (auto &primitiveType : primitiveTypes) {
//        loadPrimitiveClasses(primitiveType.name);
//        loadClass(primitiveType.arrayClassName);
//        loadClass(primitiveType.wrapperClassName);
//    }

    // 给已经加载的每一个类关联类对象。
    int size = hashmap_size(loader->loaded_class_pool);
    void *values[size];
    hashmap_values(loader->loaded_class_pool, values);
    for (int i = 0; i < size; i++) {
        struct class *c = values[i];
        c->clsobj = jclsobj_create(c);
    }

    // 缓存一下常见类
    loader->jlstring = classloader_load_class(loader, "java/lang/String");
    return loader;
}

void classloader_put_to_pool(struct classloader *loader, const char *class_name, struct class *c)
{
    assert(loader != NULL);
    assert(class_name != NULL);
    assert(strlen(class_name) > 0);
    assert(c != NULL);
    hashmap_put(loader->loaded_class_pool, class_name, c);
}

static struct class* loading(struct classloader *loader, const char *class_name)
{
    struct bytecode_content content = read_class(class_name);
    if (IS_INVALID(content)) {
        jvm_abort("class not find: %s", class_name);
    }

//    return jclass_create_by_classfile(loader, classfile_create(content.bytecode, content.len));
    return jclass_create(loader, content.bytecode, content.len);
}

static struct class* verification(struct class *c)
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

static struct class* preparation(struct class *c)
{
//    const struct rtcp* const rtcp = c->rtcp;

    // 如果静态变量属于基本类型或String类型，有final修饰符，
    // 且它的值在编译期已知，则该值存储在class文件常量池中。
    for (int i = 0; i < c->fields_count; i++) {
        if (!IS_STATIC(c->fields[i].access_flags)) {
            continue;
        }

//        const int index = c->fields[i]->constant_value_index;

        // 值已经在常量池中了
//        const bool b = (index != INVALID_CONSTANT_VALUE_INDEX);
// todo
//        c->setFieldValue(field.id, field.descriptor,
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
static struct class* resolution(struct class *c)
{
    // todo
    return c;
}

static struct class* initialization(struct class *c)
{
    // todo
    return c;
}

static struct class* load_non_arr_class(struct classloader *loader, const char *class_name)
{
// todo 解析，初始化是在这里进行，还是待使用的时候再进行
    struct class *c = loading(loader, class_name);
    return initialization(resolution(preparation(verification(c))));
}

struct class* classloader_find_class(const struct classloader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);
    return hashmap_find(loader->loaded_class_pool, class_name);
}

struct class* classloader_load_class(struct classloader *loader, const char *class_name)
{
    assert(loader != NULL);
    assert(class_name != NULL);

    struct class *c = hashmap_find(loader->loaded_class_pool, class_name);
    if (c != NULL) {
        assert(strcmp(c->class_name, class_name) == 0);
        return c;
    }

    if (class_name[0] == '[') {
        c = jclass_create_arr_class(loader, class_name);
    } else {
        c = load_non_arr_class(loader, class_name);
    }

    if (c == NULL) {
        VM_UNKNOWN_ERROR("loader class failed. %s", class_name);
        return NULL;
    }

    assert(strcmp(c->class_name, class_name) == 0);

    if (loader->jlclass != NULL) {
        c->clsobj = jclsobj_create(c);
    }

    hashmap_put(loader->loaded_class_pool, c->class_name, c);
    return c;
}

void classloader_destroy(struct classloader *loader)
{
    assert(loader != NULL);

    if (loader->loaded_class_pool != NULL) {
        hashmap_destroy(loader->loaded_class_pool);
    }
//    if (loader->classobj_pool != NULL) {
//        hashmap_destroy(loader->classobj_pool);
//    }
    if (loader->jlclass != NULL) {
        jclass_destroy(loader->jlclass);
    }

    free(loader);
}
