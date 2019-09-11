#include <iostream>
#include <pthread.h>
#include <unistd.h>
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

void *thread_func(void *arg)
{
    auto hm = (HeapMgr *) arg;
    for (int k = 0; k < 1000; k++) {
        void *p = hm->get(10);
        sleep(3);
        hm->back(p, 10);
    }
    return nullptr;
}

/*
 * 测试并发访问
 */
void test_multi_access()
{
    HeapMgr mgr;

    for (int i = 0; i < 100; i++) {
        pthread_t tid;
        pthread_create(&tid, nullptr, thread_func, &mgr);
    }
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

    test_multi_access();

    //test_get_and_back();

    cout << "------------------------------------------------------" << endl;
    //test_stack_overflow_error();

    return 0;
}
