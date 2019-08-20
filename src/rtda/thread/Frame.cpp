/*
 * Author: kayo
 */

#include <sstream>
#include "Frame.h"
#include "../ma/Class.h"

using namespace std;

Frame::Frame(Method *m, bool vm_invoke, Frame *prev)
        : method(m), reader(m->code, m->codeLen), vm_invoke(vm_invoke), prev(prev)
{
    locals = reinterpret_cast<slot_t *>(this + 1);
    stack = locals + m->maxLocals;
}

size_t Frame::size(const Method *m)
{
    return sizeof(Frame) + (m->maxStack*sizeof(slot_t)) + (m->maxLocals*sizeof(slot_t));
}

string Frame::toString()
{
    ostringstream oss;
    oss << "(" << this << ")";
    if (method->isNative())
        oss << "(native)";
    oss << method->clazz->className << "~" << method->name << "~" << method->descriptor;
    oss << ", pc = " << reader.pc;
    return oss.str();
}
