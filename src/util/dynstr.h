#ifndef CABIN_DYNSTR_H
#define CABIN_DYNSTR_H

#include <stdint.h>

/*
 * dynamic string
 */

typedef struct dynstr {
    char *buf; 
    size_t buf_len;
} DynStr;

void dynstr_init(DynStr *);
void dynstr_release(DynStr *);

DynStr *dynstr_copy(DynStr *, const char *);
DynStr *dynstr_concat(DynStr *, const char *);

DynStr *dynstr_printf(DynStr *, const char * format, ...);

#endif //CABIN_DYNSTR_H
