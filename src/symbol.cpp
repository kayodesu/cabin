/*
 * Author: Yo Ka
 */

#include "symbol.h"
#include "util/encoding.h"

#define SYMBOL_VALUE(name, value) value
const char *symbol_values[] = {
    SYMBOL_PAIRS(SYMBOL_VALUE)
};

void initSymbol()
{
    for (const char *s : symbol_values)
        utf8::save(s);
}
