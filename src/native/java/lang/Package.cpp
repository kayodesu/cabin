#include "../../jni_inner.h"
#include "../../../vmdef.h"
#include "../../../objects/string_object.h"
#include "../../../objects/class_loader.h"
#include "../../../objects/array_object.h"

/*
 * Author: Yo Ka
 */

// private static native String getSystemPackage0(String name);
static jstrref getSystemPackage0(jstrref name)
{
    const char *utf8_name = name->toUtf8();

    const char *pkg = getBootPackage(utf8_name);
    if (pkg == nullptr) {
        return nullptr;
    } else {
        return newString(pkg);
    }
}

// private static native String[] getSystemPackages0();
static jarrref getSystemPackages0()
{
    utf8_set &packages = getBootPackages();
    auto size = packages.size();

    auto ao = newArray(loadArrayClass(S(array_java_lang_String)), size);
    auto p = (Object **) ao->data;
    for (auto pkg : packages) {
        *p++ = newString(pkg);
    }

    return ao;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getSystemPackage0", _STR_ STR, (void *) getSystemPackage0},
        { "getSystemPackages0", "()[" STR, (void *) getSystemPackages0 }
};

void java_lang_Package_registerNatives()
{
    registerNatives("java/lang/Package", methods, ARRAY_LENGTH(methods));
}

