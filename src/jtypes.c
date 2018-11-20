#include "jtypes.h"
#include "jvm.h"

/*
 * Author: Jia Yang
 */

const char* get_jtype_name(enum jtype t)
{
    static const char* names[] = {
        "jbyte", "jbool", "jchar", "jshort", "jint",
        "jlong", "jfloat", "jdouble", "jref", "placeholder", "not a tpe"
    };

    if (t < 0 || t >= sizeof(names) / sizeof(*names)) {
        printvm("访问越界, %d of %d\n", t, sizeof(names));
        return "unknown";
    }

    return names[t];
}