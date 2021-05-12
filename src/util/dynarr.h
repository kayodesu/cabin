#ifndef CABIN_DYNARR_H
#define CABIN_DYNARR_H

/*
 * dynamic array
 */

typedef struct dynarr {
    void **buf; 
    size_t buf_len;
} DynArr;

void dynarr_init(DynArr *);
void dynarr_release(DynArr *);

#endif // CABIN_DYNARR_H