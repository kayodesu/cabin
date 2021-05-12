#include "symbol.h"
#include "util/encoding.h"

#define SYMBOL_VALUE(name, value) value

const char *symbol_values[] = {
    SYMBOL_PAIRS(SYMBOL_VALUE)
};

void init_symbol()
{
    for (int i = 0; i < MAX_SYMBOL_ENUM; i++) {
        const char *s = symbol_values[i];
        const char *t = save_utf8(symbol_values[i]);
        if (t != s) {
            JVM_PANIC("symbol repeat.");
        }
    }
}
