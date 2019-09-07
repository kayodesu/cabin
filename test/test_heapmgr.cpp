#include <iostream>
#include "../vm/heapmgr/HeapMgr.h"

/*
 * Author: kayo
 */

using namespace std;

int main()
{
    HeapMgr mgr;

    while (true) {
        void *p = mgr.get(1024);
        if (p == nullptr)
            break;
    }

    return 0;
}
