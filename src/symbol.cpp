/*
 * Author: kayo
 */

#include "symbol.h"
#include "utf8.h"

#define SYMBOL_VALUE(name, value) value
const char *symbol_values[] = {
    SYMBOL_PAIRS(SYMBOL_VALUE)
};

void init_symbol()
{
    for (auto &symbol_value : symbol_values)
        save_utf8(symbol_value);
}
