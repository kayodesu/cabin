/*
 * Author: Jia Yang
 */

#include <stdbool.h>
#include "symbol.h"
#include "utf8.h"
#include "jvm.h"

#define SYMBOL_VALUE(name, value) value
char* symbol_values[] = {
    SYMBOL_PAIRS(SYMBOL_VALUE)
};

void init_symbol()
{
    for(int i = 0; i < MAX_SYMBOL_ENUM; i++) {
        if(symbol_values[i] != new_utf8(symbol_values[i])) {
            // todo error
            printvm("%s\n", symbol_values[i]);
//            jam_fprintf(stderr, "Error when initialising VM symbols."
//                    "  Aborting VM.\n");
//            exit(1);
        }
//        printf(" %s\n", symbol_values[symbol_Signature]);
//        printf(" %d\n", SYMBOL(Signature) == SYMBOL(f));
    }
}
