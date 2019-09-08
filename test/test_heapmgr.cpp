#include <iostream>
#include "../vm/heapmgr/HeapMgr.h"

/*
 * Author: kayo
 */

using namespace std;

void initJVM(int argc, char* argv[]);

void test_get_and_back()
{
    HeapMgr mgr;

    cout << mgr.toString().c_str() << endl;

    void *p = mgr.get(1024);
    cout << endl << mgr.toString().c_str() << endl;

    void *q = mgr.get(1024);
    cout << endl << mgr.toString().c_str() << endl;

    mgr.back(p, 1024);
    cout << endl << mgr.toString().c_str() << endl;

    mgr.back(q, 1024);
    cout << endl << mgr.toString().c_str() << endl;
}

void test_stack_overflow_error()
{
    HeapMgr mgr;

    while (true) {
        void *p = mgr.get(10240);
        if (p == nullptr) {
            cout << "never goes here." << endl;
            return;
        }
    }
}

int main()
{
    initJVM(0, nullptr);

    test_get_and_back();
    cout << "------------------------------------------------------" << endl;
    test_stack_overflow_error();

    return 0;
}
