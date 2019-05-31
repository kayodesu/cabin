/*
 * Author: Jia Yang
 */

#include <cassert>
#include <cstdio>
#include "output.h"

/*
 * jbyte 是有符号的  todo
 */
void write_bytes(jref obj, jbyte bytes[], jint len, bool append)
{
    assert(obj != nullptr);
    assert(bytes != nullptr);
// todo
    char *data = (char *) bytes;
    for (jint i = 0; i < len; i++) {
        printf("%c", data[i]);
    }
    fflush(stdout);  // todo
}
