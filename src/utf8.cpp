/*
 * Author: kayo
 */

#include <cstring>
#include <cassert>
#include <unordered_set>
#include "utf8.h"

using namespace std;

struct Utf8Cmp {
    bool operator()(const char *s, const char *t) const
    {
        assert(s != nullptr && t != nullptr);
        return utf8_equals(s, t);
    }
};

static unordered_set<const char *, Utf8Hash, Utf8Cmp> utf8Set;

const char *save_utf8(const char *utf8)
{
    assert(utf8 != nullptr);
    return *utf8Set.insert(utf8).first;
}

const char *find_saved_utf8(const char *utf8)
{
    assert(utf8 != nullptr);
    auto iter = utf8Set.find(utf8);
    if (iter == utf8Set.end())
        return nullptr;
    return *iter;
}
