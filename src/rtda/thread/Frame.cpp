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

bool Frame::objectAccessible(jref obj)
{
    int len = method->maxLocals + method->maxStack;
    for (int i = 0; i < len; i++) {
        // todo 这里要先判断 locals[i] 存放的是不是 jref ？
        if (locals[i] == (slot_t) obj)
            return true;
    }

    return false;
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
