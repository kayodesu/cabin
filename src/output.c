/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <assert.h>
#include "output.h"
#include "jvm.h"

/*
 * jbyte 是有符号的，C语言的char是无符号的   todo
 */
void write_bytes(jref obj, jbyte bytes[], jint len)
{
    assert(obj != NULL);
    assert(bytes != NULL);
// todo
    char *data = (char *) bytes;
    for (jint i = 0; i < len; i++) {
        printf("%c", data[i]);
    }
    fflush(stdout);
}
