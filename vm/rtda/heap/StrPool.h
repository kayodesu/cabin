/*
 * Author: kayo
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

#include <unordered_set>
#include "Object.h"
#include "StringObject.h"

class StrPool {
    struct StrObjPointEquals {
        bool operator()(StringObject *x, StringObject *y) const
        {
            return utf8_equals(x->getUtf8Value(), y->getUtf8Value());
        }
    };

    struct StrObjPointHash {
        Utf8Hash h;

        size_t operator()(StringObject *x) const noexcept
        {
            return h(x->getUtf8Value());
        }
    };

    std::unordered_set<StringObject *, StrObjPointHash, StrObjPointEquals> pool;

public:
    // return either the newly inserted element
    // or the equivalent element already in the set
    StringObject *put(StringObject *so)
    {
        assert(so != nullptr);
        return *(pool.insert(so).first);
    }

    StringObject *get(const char *str)
    {
        assert(str != nullptr);
        return put(StringObject::newInst(str));
    }
};

#endif //JVM_STRPOOL_H
