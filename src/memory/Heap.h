/*
 * Author: kayo
 */

#ifndef JVM_HEAP_H
#define JVM_HEAP_H

#include <cstddef>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include "../jtypes.h"
#include "MemMgr.h"

class Class;

class Heap {
    void *raw;

    /* so called method area */
    MemMgr *classArea;
    MemMgr *bytecodeArea;
    MemMgr *methodArea;
    MemMgr *fieldArea;

    /* real heap saves objects */
    MemMgr *objectArea;

public:
    Heap() noexcept;
    ~Heap();

    void *allocClass();

    void *allocBytecode(size_t size)
    {
        assert(size > 0);
        return bytecodeArea->get(size);
    }

    void *allocMethods(u2 methodsCount);
    void *allocMethod() { return allocMethods(1); }

    void *allocFields(u2 fieldsCount);
    void *allocField() { return allocFields(1); }

    void *allocObject(size_t size)
    {
        assert(size > 0);
        return objectArea->get(size);
    }

    std::vector<Class *> getClasses();
    
    std::string toString();

    friend void gc();
};

#endif //JVM_HEAP_H
