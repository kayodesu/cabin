/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "registry.h"
#include "../jvm.h"
#include "../util/hashmap.h"

// key 的组成为 "class_name~method_name~method_descriptor" 的形式，
// key 的长度要足够存储上述格式的字符和外加字符串结尾的'\0'
#define KEY_LEN(class_name, method_name, method_descriptor) \
                 (strlen(class_name) + strlen(method_name) + strlen(method_descriptor) + 3 + 1)

static struct hashmap *native_methods;

void registerNatives(struct stack_frame *frame) { /* do nothing */ }

void register_native_method(const char *key, native_method_t method)
{
    hashmap_put(native_methods, key, method);
}

native_method_t find_native_method(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != NULL);
    assert(method_name != NULL);
    assert(method_descriptor != NULL);

    size_t key_len = KEY_LEN(class_name, method_name, method_descriptor);
    char key[key_len];
    snprintf(key, key_len - 1, "%s~%s~%s", class_name, method_name, method_descriptor);
    key[key_len - 1] = 0;

    native_method_t method = hashmap_find(native_methods, key);
    if (method != NULL) {
        return method;
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
void java_lang_Runtime_registerNatives();
void java_lang_ClassLoader_registerNatives();
void java_lang_ClassLoader$NativeLibrary_registerNatives();

void java_io_FileDescriptor_registerNatives();
void java_io_FileInputStream_registerNatives();
void java_io_FileOutputStream_registerNatives();
void java_io_WinNTFileSystem_registerNatives();

void sun_misc_VM_registerNatives();
void sun_misc_Unsafe_registerNatives();
void sun_misc_Signal_registerNatives();
void sun_io_Win32ErrorMode_registerNatives();

void sun_reflect_Reflection_registerNatives();
void sun_reflect_NativeConstructorAccessorImpl_registerNatives();
void sun_reflect_NativeMethodAccessorImpl_registerNatives();
void sun_reflect_ConstantPool_registerNatives();

void java_security_AccessController_registerNatives();

void java_util_concurrent_atomic_AtomicLong_registerNatives();

void register_all_native_methods()
{
    native_methods = hashmap_create_str_key(false);

    java_lang_Class_registerNatives();
    java_lang_Float_registerNatives();
    java_lang_System_registerNatives();
    java_lang_Double_registerNatives();
    java_lang_Object_registerNatives();
    java_lang_String_registerNatives();
    java_lang_Throwable_registerNatives();
    java_lang_Thread_registerNatives();
    java_lang_Runtime_registerNatives();
    java_lang_ClassLoader_registerNatives();
    java_lang_ClassLoader$NativeLibrary_registerNatives();

    java_io_FileDescriptor_registerNatives();
    java_io_FileInputStream_registerNatives();
    java_io_FileOutputStream_registerNatives();
    java_io_WinNTFileSystem_registerNatives();

    sun_misc_VM_registerNatives();
    sun_misc_Unsafe_registerNatives();
    sun_misc_Signal_registerNatives();

    sun_io_Win32ErrorMode_registerNatives();

    sun_reflect_Reflection_registerNatives();
    sun_reflect_NativeConstructorAccessorImpl_registerNatives();
    sun_reflect_NativeMethodAccessorImpl_registerNatives();
    sun_reflect_ConstantPool_registerNatives();

    java_security_AccessController_registerNatives();

    java_util_concurrent_atomic_AtomicLong_registerNatives();
}
