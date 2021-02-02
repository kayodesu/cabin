#include "../../jni_internal.h"

// Gets the platform defined TimeZone ID.
// private static native String getSystemTimeZoneID(String javaHome);
jstring getSystemTimeZoneID(jstring java_home)
{
    if (java_home == nullptr) {
        // NullPointerException todo
    }
    // todo
    jvm_abort("getSystemTimeZoneID");
}

/*
 * Gets the custom time zone ID based on the GMT offset of the platform. (e.g., "GMT+08:00")
 * 
 * private static native String getSystemGMTOffsetID();
 */
jstring getSystemGMTOffsetID()
{
    // todo
    jvm_abort("getSystemGMTOffsetID");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    {"getSystemTimeZoneID", _STR_ STR, (void *)getSystemTimeZoneID},
    {"getSystemGMTOffsetID", __STR, (void *)getSystemGMTOffsetID},
};

void java_util_TimeZone_registerNatives()
{
    registerNatives("java/util/TimeZone", methods, ARRAY_LENGTH(methods));
}
