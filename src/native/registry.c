/*
 * Author: Jia Yang
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "registry.h"
#include "../../lib/uthash/uthash.h"
#include "../jvm.h"


//static map<string, function<void(StackFrame *)>> nativeMethods;

// key 的组成为 "class_name~method_name~method_descriptor" 的形式，
// key 的长度要足够存储上述格式的字符和外加字符串结尾的'\0'
#define KEY_LEN (3 * PATH_MAX + 3)  // todo PATH_MAX 对不对

static struct native_method {
    char key[KEY_LEN];
    void (* method)(struct stack_frame *);
    UT_hash_handle hh; // makes this structure hashable
} *native_methods = NULL;

//static set<NativeMethod, NativeMethod::Cmp> ssssss;

//void printRegisteredNativeMethods() {
//    jprintf("\n");
//    for (auto iter = nativeMethods.begin(); iter != nativeMethods.end(); iter++) {
//        jprintf("%s, %s\n", iter->first.c_str(), iter->second.target_type().name());
//    }
//    jprintf("\n");
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
    HASH_ADD_STR(native_methods, key, nm);
}

native_method_f find_native_method(const char *class_name, const char *method_name, const char *method_descriptor)
{
    char key[KEY_LEN];
    gen_key(class_name, method_name, method_descriptor, key);

    struct native_method *nm;
    HASH_FIND_STR(native_methods, key, nm);
    if (nm != NULL) {
        return nm->method;
    }

    // todo not find;   UnsatisfiedLinkError异常
    jvm_abort("not find native function: %s\n", key);
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
