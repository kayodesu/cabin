#include <vector>
#include "../../../runtime/frame.h"
#include "../../../objects/array_object.h"
#include "../../jni_inner.h"

/*
 * Author: Yo Ka
 */

using namespace std;

// private static native String getSystemPackage0(String name);
static jstrref getSystemPackage0(JNIEnv *env, jclass clazz, jstrref name)
{
    // auto name = frame->getLocalAsRef(0)->toUtf8();

    auto pkg = getBootPackage(name->toUtf8());
    if (pkg == nullptr) {
        return nullptr;
    } else {
        return newString(pkg);
    }
}

// private static native String[] getSystemPackages0();
static jarrref getSystemPackages0(JNIEnv *env, jclass clazz)
{
    utf8_set &packages = getBootPackages();
    auto size = packages.size();

    auto ao = newArray(loadArrayClass(S(array_java_lang_String)), size);
    int i = 0;
    for (auto pkg : packages) {
        ao->set(i, newString(pkg));
        i++;
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

