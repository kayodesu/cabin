#include <vector>
#include "Heap.h"
#include "../objects/class.h"
#include "../objects/method.h"
#include "../objects/field.h"
#include "../config.h"

/*
 * Author: Yo Ka
 */

using namespace std;


Heap::Heap() noexcept
{
    size_t size = VM_HEAP_SIZE;

    const size_t classAreaSize = size / 20;          // 5% todo
    const size_t bytecodeAreaSize = (size / 10) * 3; // 30% todo
    const size_t methodAreaSize = size / 20;         // 5% todo
    const size_t fieldAreaSize = size / 20;          // 5% todo

    raw = malloc(size);
    assert(raw != nullptr);
    auto mem = (address) raw;

    assert(size > classAreaSize);
    classArea = new MemMgr(mem, classAreaSize);
    mem += classAreaSize;
    size -= classAreaSize;

    assert(size > bytecodeAreaSize);
    bytecodeArea = new MemMgr(mem, bytecodeAreaSize);
    mem += bytecodeAreaSize;
    size -= bytecodeAreaSize;

    assert(size > methodAreaSize);
    methodArea = new MemMgr(mem, methodAreaSize);
    mem += methodAreaSize;
    size -= methodAreaSize;

    assert(size > fieldAreaSize);
    fieldArea = new MemMgr(mem, fieldAreaSize);
    mem += fieldAreaSize;
    size -= fieldAreaSize;

    objectArea = new MemMgr(mem, size);
}

Heap::~Heap()
{
    free(raw);
}

void *Heap::allocClass()
{
    return classArea->get(Class::getSize());
}

void *Heap::allocMethods(u2 methodsCount)
{
    assert(methodsCount > 0);
    return methodArea->get(methodsCount * sizeof(Method));
}

void *Heap::allocFields(u2 fieldsCount)
{
    assert(fieldsCount > 0);
    return fieldArea->get(fieldsCount * sizeof(Field));
}

vector<Class *> Heap::getClasses()
{
    classArea->lock();

    vector<Class *> classes;

    address mem = classArea->getMem();
    const address end = mem + classArea->getSize();

    while ((mem = classArea->jumpFreelist(mem)) < end) {
        classes.push_back((Class *) mem);
        mem += Class::getSize();
    }

    classArea->unlock();
    return classes;
}

string Heap::toString()
{
    stringstream ss;

    ss << "class" << endl;
    ss << classArea->toString() << endl;

//    ss << "bytecodeArea" << endl;
//    ss << bytecodeArea->toString() << endl;

//    ss << "methodArea" << endl;
//    ss << methodArea->toString() << endl;
//
//    ss << "fieldArea" << endl;
//    ss << fieldArea->toString() << endl;

    ss << "objectArea" << endl;
    ss << objectArea->toString() << endl;

    return ss.str();
}