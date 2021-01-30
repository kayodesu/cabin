#include <iostream>

using namespace std;

union Slot {
public:
    int32_t i;
    float f;
    void *p;
    int64_t j; // jlong
    double d;
public:
    Slot() = default;
    Slot(int i) {this->i = i;}
    void setInt(int v) { i = v; }
    int getInt() const { return i; }
};


int main()
{
    cout << sizeof(short) << endl;
    cout << sizeof(int) << endl;
    cout << sizeof(long) << endl;
    cout << sizeof(float) << endl;
    cout << sizeof(void *) << endl;
    cout << sizeof(int64_t) << endl;
    cout << sizeof(double) << endl;

    cout << sizeof(Slot) << endl;
    Slot slot(8);
    slot.setInt(1000);
    cout << slot.getInt() << endl;


    cout << &slot << endl << &(slot.i) << endl;
}
