#include "Prims.h"
#include "../symbol.h"
#include "../util/encoding.h"

/*
 * Author: kayo
 */

using namespace utf8;

Prims::Info Prims::prims[9];

void Prims::init()
{
    prims[0].className = S(void);
    prims[0].descriptor = 'V';
    prims[0].arrayClassName = S(array_V);
    prims[0].wrapperClassName = S(java_lang_Void);

    prims[1].className = S(boolean);
    prims[1].descriptor = 'Z';
    prims[1].arrayClassName = S(array_Z);
    prims[1].wrapperClassName = S(java_lang_Boolean);

    prims[2].className = S(byte);
    prims[2].descriptor = 'B';
    prims[2].arrayClassName = S(array_B);
    prims[2].wrapperClassName = S(java_lang_Byte);

    prims[3].className = S(char);
    prims[3].descriptor = 'C';
    prims[3].arrayClassName = S(array_C);
    prims[3].wrapperClassName = S(java_lang_Character);

    prims[4].className = S(short);
    prims[4].descriptor = 'S';
    prims[4].arrayClassName = S(array_S);
    prims[4].wrapperClassName = S(java_lang_Short);

    prims[5].className = S(int);
    prims[5].descriptor = 'I';
    prims[5].arrayClassName = S(array_I);
    prims[5].wrapperClassName = S(java_lang_Integer);

    prims[6].className = S(long);
    prims[6].descriptor = 'J';
    prims[6].arrayClassName = S(array_J);
    prims[6].wrapperClassName = S(java_lang_Long);

    prims[7].className = S(float);
    prims[7].descriptor = 'F';
    prims[7].arrayClassName = S(array_F);
    prims[7].wrapperClassName = S(java_lang_Float);

    prims[8].className = S(double);
    prims[8].descriptor = 'D';
    prims[8].arrayClassName = S(array_D);
    prims[8].wrapperClassName = S(java_lang_Double);
}

bool Prims::isPrimClassName(const utf8_t *className)
{
    assert(className != nullptr);
    for (auto &t : prims) {
        if (equals(t.className, className))
            return true;
    }
    return false;
}

bool Prims::isPrimDescriptor(utf8_t descriptor)
{
    for (auto &t : prims) {
        if (t.descriptor == descriptor)
            return true;
    }
    return false;
}

const utf8_t *Prims::getArrayClassName(const utf8_t *className)
{
    assert(className != nullptr);

    for (auto &t : prims) {
        if (equals(t.className, className))
            return t.arrayClassName;
    }
    return nullptr;
}

const utf8_t *Prims::descriptor2className(utf8_t descriptor)
{
    for (auto &t : prims) {
        if (t.descriptor == descriptor)
            return t.className;
    }
    return nullptr;
}
