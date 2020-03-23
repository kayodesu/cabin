/*
 * Author: kayo
 */

#ifndef KAYO_STRING_OBJECT_H
#define KAYO_STRING_OBJECT_H

#include "object.h"

jstrref newString(const utf8_t *str);

static inline jstrref newStringUnicode(const unicode_t *str, jsize len)
{
    // todo
    jvm_abort("not implement.");
}

struct StrObjEquals {
    bool operator()(Object *x, Object *y) const;
};

struct StrObjHash {
    size_t operator()(Object *x) const;
};

#endif //KAYO_STRING_OBJECT_H
