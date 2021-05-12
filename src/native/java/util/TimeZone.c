#include "../../jni_internal.h"

// Gets the platform defined TimeZone ID.
// private static native String getSystemTimeZoneID(String javaHome);
jstring getSystemTimeZoneID(jstring java_home)
{
    if (java_home == NULL) {
        // NullPointerException todo
    }
    // todo
    JVM_PANIC("getSystemTimeZoneID");
}

/*
 * Gets the custom time zone ID based on the GMT offset of the platform. (e.g., "GMT+08:00")
 * 
 * private static native String getSystemGMTOffsetID();
 */
jstring getSystemGMTOffsetID()
{
    // todo
    JVM_PANIC("getSystemGMTOffsetID");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    {"getSystemTimeZoneID", _STR_ STR, TA(getSystemTimeZoneID) },
    {"getSystemGMTOffsetID", __STR, TA(getSystemGMTOffsetID) },
};

void java_util_TimeZone_registerNatives()
{
    registerNatives("java/util/TimeZone", methods, ARRAY_LENGTH(methods));
}
