/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include "registry.h"
#include "../jvm.h"
#include "../util/hashmap.h"

// key 的组成为 "class_name~method_name~method_descriptor" 的形式，
// key 的长度要足够存储上述格式的字符和外加字符串结尾的'\0'
#define KEY_LEN (3 * FILENAME_MAX + 3)  // big enough?

struct native_method {
    char key[KEY_LEN];
    void (* method)(struct stack_frame *);
};

static struct hashmap *native_methods;

//void print_registered_native_methods()
//{
//    printvm("\n");
//    hashmap_print(native_methods);
//}

static char* gen_key(const char *class_name, const char *method_name, const char *method_descriptor, char key[])
{
    sprintf(key, "%s~%s~%s\0", class_name, method_name, method_descriptor);
    return key;
}

void register_native_method(const char *class_name, const char *method_name,
                            const char *method_descriptor, void (* method)(struct stack_frame *))
{
    VM_MALLOC(struct native_method, nm);
    nm->method = method;
    gen_key(class_name, method_name, method_descriptor, nm->key);
    hashmap_put(native_methods, nm->key, nm);
}

void (* find_native_method(const char *class_name,
                           const char *method_name, const char *method_descriptor))(struct stack_frame *)
{
    char key[KEY_LEN];
    gen_key(class_name, method_name, method_descriptor, key);

    struct native_method *nm = hashmap_find(native_methods, key);
    if (nm != NULL) {
        return nm->method;
    }

    // todo not find;   UnsatisfiedLinkError异常
    jvm_abort("can't find native function: %s\n", key);
    return NULL;
}

void java_lang_Class_registerNatives();
void java_lang_Float_registerNatives();
void java_lang_System_registerNatives();
void java_lang_Double_registerNatives();
void java_lang_Object_registerNatives();
void java_lang_String_registerNatives();
void java_lang_Throwable_registerNatives();
void java_lang_Thread_registerNatives();

void java_io_FileDescriptor_registerNatives();
void java_io_FileInputStream_registerNatives();
void java_io_FileOutputStream_registerNatives();

void sun_misc_VM_registerNatives();
void sun_misc_Unsafe_registerNatives();

void sun_reflect_Reflection_registerNatives();

void java_security_AccessController_registerNatives();

void register_all_native_methods()
{
    native_methods = hashmap_create_str_key();

    java_lang_Class_registerNatives();
    java_lang_Float_registerNatives();
    java_lang_System_registerNatives();
    java_lang_Double_registerNatives();
    java_lang_Object_registerNatives();
    java_lang_String_registerNatives();
    java_lang_Throwable_registerNatives();
    java_lang_Thread_registerNatives();

    java_io_FileDescriptor_registerNatives();
    java_io_FileInputStream_registerNatives();
    java_io_FileOutputStream_registerNatives();

    sun_misc_VM_registerNatives();
    sun_misc_Unsafe_registerNatives();

    sun_reflect_Reflection_registerNatives();

    java_security_AccessController_registerNatives();
}
