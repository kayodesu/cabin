#include <vector>
#include "../../../runtime/frame.h"
#include "../../../objects/array_object.h"
#include "../../jni_inner.h"

/*
 * Author: kayo
 */

using namespace std;

// private static native String getSystemPackage0(String name);
static void getSystemPackage0(Frame *frame)
{
    auto name = frame->getLocalAsRef(0)->toUtf8();

    auto pkg = getBootPackage(name);
    if (pkg == nullptr) {
        frame->pushr(nullptr);
    } else {
        frame->pushr(newString(pkg));
    }
}

// private static native String[] getSystemPackages0();
static void getSystemPackages0(Frame *frame)
{
    utf8_set &packages = getBootPackages();
    auto size = packages.size();

    auto ao = newArray(loadArrayClass(S(array_java_lang_String)), size);
    auto p = (Object **) ao->data;
    for (auto pkg : packages) {
        *p++ = newString(pkg);
    }

    frame->pushr(ao);
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

