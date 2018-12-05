#include <string.h>
#include <assert.h>
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

    int len = sizeof(names) / sizeof(*names);
    if (t < 0 || t >= len) {
        printvm("access beyoud bounds, %d of %d\n", t, len); // todo
        return "unknown";
    }

    return names[t];
}
