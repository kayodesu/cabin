/*
 * Author: Yo Ka
 */
#include <iostream>
#include "../src/platform/sysinfo.h"

using namespace std;

int main()
{
    cout << "processor number: " << processorNumber() << endl;
    cout << "page size: " << pageSize() << endl;
    cout << "os name: " << osName() << endl;
    cout << "os arch: " << osArch() << endl;
    cout << "time zone: " << getTimeZone() << endl;
}