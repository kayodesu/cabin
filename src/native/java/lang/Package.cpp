#include <vector>
#include "../../registry.h"
#include "../../../runtime/frame.h"
#include "../../../objects/array_object.h"

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

void java_lang_Package_registerNatives()
{
#undef C
#define C "java/lang/Package"
    registerNative(C, "getSystemPackage0", "(Ljava/lang/String;)Ljava/lang/String;", getSystemPackage0);
    registerNative(C, "getSystemPackages0", "()[Ljava/lang/String;", getSystemPackages0);
}

