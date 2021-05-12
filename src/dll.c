#include <assert.h>
#include <windows.h>
#include "cabin.h"

static char *mangle_class_and_method_name(Method *m) 
{
    assert(m != NULL);

    char *class_name = m->clazz->class_name;
    int len = strlen(class_name) + strlen(m->name) + 16;
    char *mangled = vm_malloc(len*sizeof(char));
    // sprintf(mangled, "Java_%s_%s", class_name, m->name);

    // // 将类名之间的'/'替换为'_'
    // for (char *t = mangled; *t != 0; t++)
    //     if (*t == '/')
    //         *t = '_';

    strcpy(mangled, "Java_");

    int i = strlen("Java_");
    for (char *t = class_name; *t != 0; t++) {
        if (*t == '/') {
             // 将类名之间的'/'替换为'_'
            mangled[i++] = '_';
        } else if (*t == '$') {
            // 将类名之间的'$'替换为"_00024"，比如
            // class: jdk/internal/util/SystemProps$Raw, method: platformProperties
            // Java_jdk_internal_util_SystemProps_00024Raw_platformProperties
            mangled[i++] = '_';
            mangled[i++] = '0';
            mangled[i++] = '0';
            mangled[i++] = '0';
            mangled[i++] = '2';
            mangled[i++] = '4';
        } else {
            mangled[i++] = *t;
        }
        if (i >= len - 1) {
            JVM_PANIC("xxxxxxxxxxxx"); // todo
        }
    }

    mangled[i++] = '_';
    mangled[i] = 0;
    // todo 判断mangled空间够不够存放m->name
    strcat(mangled, m->name);

    return mangled;
}

static Method *find_native = NULL;
static HMODULE libjava;

void *find_from_java_dll(Method *m)
{
    assert(m != NULL && ACC_IS_NATIVE(m->access_flags));

    char *mangled = mangle_class_and_method_name(m);
    

    // exec_java_func2(find_native, NULL, alloc_string("registerNatives"));


    return GetProcAddress(libjava, mangled);

//    printvm("%s, %p\n", mangled, p);
}

void init_dll()
{
    libjava = LoadLibrary("C:\\Program Files\\Java\\jdk-16\\bin\\java.dll");
 //   DWORD e = GetLastError();
 //   printvm("%p\n", java_dll);
    
    Class *ldr_class = load_boot_class(S(java_lang_ClassLoader));
    if(ldr_class != NULL) {
        // private static long findNative(ClassLoader loader, String entryName)
        find_native = lookup_static_method(ldr_class,
                                "findNative", "(Ljava/lang/ClassLoader;Ljava/lang/String;)J");
    }

    if(find_native == NULL) {
        ERR("Expected \"findNative\" method missing in java.lang.ClassLoader");
    }
}
