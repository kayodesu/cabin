#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "dynstr.h"
#include "cabin.h"


#define INIT_LEN 128

void dynstr_init(DynStr *ds)
{
    assert(ds != NULL);

    ds->buf_len = INIT_LEN;
    ds->buf = vm_malloc(sizeof(char) * INIT_LEN);
    ds->buf[0] = 0; // end string
}

DynStr *dynstr_reserve(DynStr *ds, size_t min_len)
{
    assert(ds != NULL);

    if (ds->buf_len >= min_len)
        return ds;

    while ((ds->buf_len *= 2) < min_len);

    ds->buf = vm_realloc(ds->buf, ds->buf_len);
    return ds;
}

DynStr *dynstr_copy(DynStr *ds, const char *s)
{
    assert(ds != NULL);

    if (s == NULL)
        return ds;

    dynstr_reserve(ds, strlen(s) + 1);
    strcpy(ds->buf, s);

    return ds;
}

DynStr *dynstr_concat(DynStr *ds, const char *s)
{
    assert(ds != NULL);

    if (s == NULL)
        return ds;

    dynstr_reserve(ds, strlen(ds->buf) + strlen(s) + 1);
    strcat(ds->buf, s);

    return ds;
}

void dynstr_release(DynStr *ds)
{
    if (ds != NULL) {
        free(ds->buf);
    }
}
