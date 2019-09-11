#include <iostream>
#include "../vm/heapmgr/HeapMgr.h"
#include "../vm/rtda/heap/ArrayObject.h"
#include "../vm/rtda/ma/ArrayClass.h"

/*
 * Author: kayo
 */

using namespace std;

void initJVM(int argc, char* argv[]);

void test_1_dimension()
{
    ArrayClass *ac = loadArrayClass("[int");
    ArrayObject *ao = ArrayObject::newInst(ac, 1);

    cout << ao->get<int>(0) << endl; // 应该为0，数组有默认初始值。
}

void test_2_dimension()
{
    ArrayClass *ac = loadArrayClass("[[int");
    size_t lens[] = { 4, 4 };
    ArrayObject *ao = ArrayObject::newInst(ac, 2, lens);

    auto o = ao->get<ArrayObject *>(1);
    o->set(2, 5); // int[1][2] = 5;
    cout << o->get<int>(2) << endl; // expect 5
}

void test_3_dimension()
{
    ArrayClass *ac = loadArrayClass("[[int");
    size_t lens[] = { 4, 4, 4 };
    ArrayObject *ao = ArrayObject::newInst(ac, 3, lens);

    auto o1 = ao->get<ArrayObject *>(1);
    auto o2 = o1->get<ArrayObject *>(2);
    o2->set(3, 5); // int[1][2][3] = 5;
    cout << o2->get<int>(3) << endl; // expect 5
}

int main()
{
    initJVM(0, nullptr);

    test_1_dimension();
    test_2_dimension();
    test_3_dimension();

    return 0;
}
