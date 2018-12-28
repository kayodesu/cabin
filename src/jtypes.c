#include "jtypes.h"
#include "jvm.h"

/*
 * Author: Jia Yang
 */

const char* get_jtype_name(enum jtype t)
{
    static const char* names[] = {
        "error_type", "jbyte", "jbool", "jchar", "jshort", "jint",
        "jlong", "jfloat", "jdouble", "jref", "placeholder", "not a tpe"
    };

    int len = sizeof(names) / sizeof(*names);
    if (t < 0 || t >= len) {
        VM_UNKNOWN_ERROR("access beyond bounds, %d of %d\n", t, len);
        return "unknown";
    }

    return names[t];
}
