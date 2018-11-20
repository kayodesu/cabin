#include <stdio.h>
#include <string.h>
#include "../src/util/hashmap.h"

/*
 * Author: Jia Yang
 */

void test_hashmap()
{
    struct hashmap *map = hashmap_create_str_key();
#define KEY1 "3444"
#define KEY2 "3444"
#define KEY3 "3444"
#define KEY4 "3444"
    hashmap_put(map, KEY1, "rrrr");
    hashmap_put(map, KEY2, "1122");
    const char *value = hashmap_find(map, KEY1);
    printf("%s\n", value);
}