#ifndef CABIN_DYNSTR_H
#define CABIN_DYNSTR_H

#include <stdint.h>
#include <string.h>

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

DynStr *dynstr_reserve(DynStr *ds, size_t min_len);

// concat to @_ds
#define dynstr_printf(_ds, _format, ...) \
do { \
    int len = strlen((_ds)->buf); \
    while (1) { \
        char *p = (_ds)->buf + len; \
        int n = snprintf(p, (_ds)->buf_len - len, _format, __VA_ARGS__); \
        if (n >= (_ds)->buf_len - len - 1) { /* 空间不够，扩容一倍 */ \
            dynstr_reserve((_ds), (_ds)->buf_len*2); \
        } else { \
            break; \
        } \
    } \
} while(0)

#endif //CABIN_DYNSTR_H
