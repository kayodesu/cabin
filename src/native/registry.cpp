/*
 * Author: Jia Yang
 */

#include <map>
#include <algorithm>
#include <set>

#include "registry.h"

using namespace std;

static map<string, function<void(StackFrame *)>> nativeMethods;

//static set<NativeMethod, NativeMethod::Cmp> ssssss;

void printRegisteredNativeMethods() {
    jprintf("\n");
    for (auto iter = nativeMethods.begin(); iter != nativeMethods.end(); iter++) {
        jprintf("%s, %s\n", iter->first.c_str(), iter->second.target_type().name());
    }
    jprintf("\n");
}

static string genKey(const string &className, const string &methodName, const string &methodDescriptor) {
    string key = className;
    key.push_back('~');
    key.append(methodName);
    key.push_back('~');
    key.append(methodDescriptor);
    return key;
}

void registerNativeMethod(const string &className,
                          const string &methodName,
                          const string &methodDescriptor,
                          function<void(StackFrame *)> nativeMethod) {
    nativeMethods[genKey(className, methodName, methodDescriptor)] = nativeMethod;   // todo 这里直接add有重复注册的问题
}

function<void(StackFrame *)> findNativeMethod(const string &className,
                                              const string &methodName,
                                              const string &methodDescriptor) {
    string key = genKey(className, methodName, methodDescriptor);

    auto iter = nativeMethods.find(key);
    if (iter == nativeMethods.end()) {
        // todo not find;   UnsatisfiedLinkError异常
        jvmAbort("not find native function: %s\n", key.c_str());
        return nullptr;
    } else {
        return iter->second;
    }
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

void registerAllNativeMethods() {
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

    sun_misc_Unsafe_registerNatives();
    sun_misc_VM_registerNatives();

    sun_reflect_Reflection_registerNatives();

    java_security_AccessController_registerNatives();
}

