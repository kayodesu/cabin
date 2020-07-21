/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../src/sysinfo.h"

using namespace std;

int main()
{
    cout << "processor number: " << processorNumber() << endl;

    cout << "page size: " << pageSize() << endl;

    cout << "free memory: " << freeMemoryCount() << endl;
    cout << "total memory: " << totalMemoryCount() << endl;
//    cout << "max memory: " << maxMemoryCount() << endl;

    cout << "os name: " << osName() << endl;
    cout << "os arch: " << osArch() << endl;
}