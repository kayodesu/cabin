#include "cabin.h"
#include "hash.h"

static PHS boot_packages;

// <const utf8_t *, Class *>
static PHM boot_classes;  // todo 加读写锁

// vm中所有存在的 class loaders，include "boot class loader".
// const Object *
static PHS loaders;

static void add_class_to_class_loader(Object *class_loader, Class *c)
{
    assert(c != NULL);
    if (class_loader == BOOT_CLASS_LOADER) {
        phm_insert(&boot_classes, c->class_name, c);
        return;
    }

    phs_add(&loaders, class_loader);

    if (class_loader->classes == NULL) {
        class_loader->classes = phm_create((point_hash_func)utf8_hash, (point_equal_func)utf8_equals);
    }
    phm_insert(class_loader->classes, c->class_name, c);

    // Invoked by the VM to record every loaded class with this loader.
    // void addClass(Class<?> c);
//    Method *m = classLoader->clazz->getDeclaredInstMethod("addClass", "Ljava/lang/Class;");
//    assert(m != NULL);
//    execJavaFunc(m, { (slot_t) classLoader, (slot_t) c });
}

static void inject_fields(Class *c)
{
    assert(c != NULL);

    if (utf8_equals(c->class_name, S(java_lang_invoke_MemberName))) {
        //@Injected intptr_t vmindex; // vtable index or offset of resolved member
        bool b = inject_inst_field(c, "vmindex", S(I));
        if (!b) {
            JVM_PANIC("inject fields error"); // todo
        }
        return;
    }

    if (utf8_equals(c->class_name, "java/lang/invoke/ResolvedMethodName")) {
        //@Injected JVM_Method* vmtarget;
        //@Injected Class<?>    vmholder;
        bool b1 = inject_inst_field(c, "vmtarget", S(sig_java_lang_Object));
        bool b2 = inject_inst_field(c, "vmholder", S(sig_java_lang_Class));
        if (!b1 || !b2) {
            JVM_PANIC("inject fields error"); // todo
        }
        return;
    }
}

Class *define_prim_type_class(const char *class_name);
Class *define_array_class(Object *loader, const char *class_name);

Class *load_boot_class(const utf8_t *name)
{
    assert(name != NULL);
    assert(IS_SLASH_CLASS_NAME(name));
    assert(name[0] != '['); // don't load array class

    Class *found = (Class *) phm_find(&boot_classes, name);
    if (found != NULL) {
        TRACE("find loaded class (%s) from pool.", name);
        return found;
    }

    Class *c = NULL;
    if (is_prim_class_name(name)) {
        c = define_prim_type_class(name);
    } else {
        size_t bytecode_len;
        u1 *bytecode = read_boot_class(name, &bytecode_len);
        if (bytecode != NULL) { // find out
            c = define_class(BOOT_CLASS_LOADER, bytecode, bytecode_len);
        }
    }

    if (c != NULL) {
        // boot_packages.insert(c->pkg_name);        
        phs_add(&boot_packages, c->pkg_name);   
        inject_fields(c);
        add_class_to_class_loader(BOOT_CLASS_LOADER, c);
    }
    
    return c;
}

Class *load_array_class(Object *loader, const utf8_t *arr_class_name)
{
    assert(arr_class_name != NULL);
    assert(arr_class_name[0] == '['); // must be array class name

    const char *ele_class_name = arr_class_name_to_ele_class_name(arr_class_name);
    Class *c = load_class(loader, ele_class_name);
    if (c == NULL)
        return NULL; // todo

    /* Array Class 用它的元素的类加载器加载 */

    Class *arr_class = find_loaded_class(c->loader, arr_class_name);
    if (arr_class != NULL)
        return arr_class; // find out
    
    arr_class = define_array_class(c->loader, arr_class_name);
    assert(arr_class != NULL);
    if (arr_class->loader == BOOT_CLASS_LOADER) {
        // boot_packages.insert(arr_class->pkg_name); // todo array class 的pkg_name是啥
        phs_add(&boot_packages, arr_class->pkg_name);    // todo array class 的pkg_name是啥
    }
    add_class_to_class_loader(arr_class->loader, arr_class);
    return arr_class;
}

Class *load_type_array_class(ArrayType type)
{
    const char *arr_class_name = NULL;

    switch (type) {
        case JVM_AT_BOOLEAN: arr_class_name = S(array_Z); break;
        case JVM_AT_CHAR:    arr_class_name = S(array_C); break;
        case JVM_AT_FLOAT:   arr_class_name = S(array_F); break;
        case JVM_AT_DOUBLE:  arr_class_name = S(array_D); break;
        case JVM_AT_BYTE:    arr_class_name = S(array_B); break;
        case JVM_AT_SHORT:   arr_class_name = S(array_S); break;
        case JVM_AT_INT:     arr_class_name = S(array_I); break;
        case JVM_AT_LONG:    arr_class_name = S(array_J); break;
        default:
            raise_exception(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("Invalid array type: " + to_string(type));
    }

    return load_array_class(BOOT_CLASS_LOADER, arr_class_name);
}

const utf8_t *get_boot_package(const utf8_t *name)
{
    return (const utf8_t *) phs_find(&boot_packages, name);
}

PHS *get_boot_packages()
{
    return &boot_packages;
}

Class *find_loaded_class(Object *class_loader, const utf8_t *name)
{
    assert(name != NULL);
    assert(IS_SLASH_CLASS_NAME(name));

    if (class_loader == BOOT_CLASS_LOADER) {
        return (Class *) phm_find(&boot_classes, name);
    }

    // is not boot classLoader
    if (class_loader->classes != NULL) {
        return (Class *) phm_find(class_loader->classes, name);
    }

    // not find
    return NULL;
}

Class *load_class(Object *class_loader, const utf8_t *name)
{
    assert(name != NULL);
//    assert(isSlashName(name));

    utf8_t *slash_name = dot_to_slash_dup(name);
    Class *c = find_loaded_class(class_loader, slash_name);
    if (c != NULL)
        return c;

    if (slash_name[0] == '[')
        return load_array_class(class_loader, slash_name);

    // 先尝试用boot class loader load the class
    c = load_boot_class(slash_name);
    if (c != NULL || class_loader == NULL)
        return c;

    // todo 再尝试用扩展classLoader load the class

    // public Class<?> loadClass(String name) throws ClassNotFoundException
    Method *m = lookup_inst_method(class_loader->clazz, S(loadClass),
                                   S(_java_lang_String__java_lang_Class));
    assert(m != NULL);

    utf8_t *dot_name = slash_to_dot_dup(name);
    slot_t *slot = exec_java(m, (slot_t[]) { rslot(class_loader), rslot(alloc_string(dot_name)) });
    assert(slot != NULL);
    jclsRef co = slot_get_ref(slot);
    assert(co != NULL && co->jvm_mirror != NULL);
    c = co->jvm_mirror;
    add_class_to_class_loader(class_loader, c);
    // init_class(c); /////////// todo /////////////////////////////////////////
    return c;
}

Class *define_class1(jref class_loader, jref name,
                     jarrRef bytecode, jint off, jint len, jref protection_domain, jref source)
{
    u1 *data = (u1 *) bytecode->data;
    Class *c = define_class(class_loader, data + off, len);
    // c->class_name和name是否相同 todo
//    printvm("class_name: %s\n", c->class_name);
    return c;
}

Class *init_class(Class *c)
{
    assert(c != NULL);

    if (c->inited) {
        return c;
    }

    pthread_mutex_lock(&c->clinit_mutex);
    if (c->inited) { // 需要再次判断 inited，有可能被其他线程置为 true
        pthread_mutex_unlock(&c->clinit_mutex);
        return c;
    }

    c->state = CLASS_INITING;

    if (c->super_class != NULL) {
        init_class(c->super_class);
    }

    // 在这里先行 set inited true, 如不这样，后面执行<clinit>时，
    // 可能调用putstatic等函数也会触发<clinit>的调用造成死循环。
    c->inited = true;

    Method *m = get_declared_method_noexcept(c, S(class_init), S(___V));
    if (m != NULL) { // 有的类没有<clinit>方法
        exec_java(m, NULL);
    }

    c->inited = true;
    c->state = CLASS_INITED;
    pthread_mutex_unlock(&c->clinit_mutex);

    return c;
}

Class *link_class(Class *c)
{
    assert(c != NULL);

    // todo

    c->state = CLASS_LINKED;
    return c;
}

Object *get_platform_class_loader()
{
    Class *c = load_boot_class(S(java_lang_ClassLoader));
    assert(c != NULL);

    // public static ClassLoader getPlatformClassLoader();
    Method *get = get_declared_static_method(c, S(getPlatformClassLoader), S(___java_lang_ClassLoader));
    return exec_java_r(get, NULL);
}

Object *get_app_class_loader()
{
    Class *c = load_boot_class(S(java_lang_ClassLoader));
    assert(c != NULL);

    // public static ClassLoader getSystemClassLoader();
    Method *get = get_declared_static_method(c, S(getSystemClassLoader), S(___java_lang_ClassLoader));
    return exec_java_r(get, NULL);
}

Class *g_object_class = NULL;
Class *g_class_class = NULL;
Class *g_string_class = NULL;

void init_class_loader()
{    
    phs_init(&loaders, NULL, NULL);
    phs_init(&boot_packages, (point_hash_func) utf8_hash, (point_equal_func) utf8_equals);
    phm_init(&boot_classes, (point_hash_func) utf8_hash, (point_equal_func) utf8_equals);

    g_object_class = load_boot_class(S(java_lang_Object));
    g_class_class = load_boot_class(S(java_lang_Class));

    // g_class_class 至此创建完成。
    // 在 g_class_class 创建完成之前创建的 Class 都没有设置 java_mirror 字段，现在设置下。

    phm_touch_values(&boot_classes, (void (*)(void *)) gen_class_object);

    g_string_class = load_boot_class(S(java_lang_String));
    // build string pool
    g_string_class->string.str_pool = phs_create((point_hash_func) string_hash, (point_equal_func) string_equals);
    // g_string_class->str_pool = new unordered_set<Object *, StringHash, StringEquals>;
    // g_string_class->buildStrPool();

    phs_add(&loaders, BOOT_CLASS_LOADER);
}

PHM *get_all_boot_classes()
{
    return &boot_classes;
}

PHS *get_all_class_loaders()
{
    return &loaders;
}