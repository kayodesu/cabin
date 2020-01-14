/*
 * Author: kayo
 */

#ifndef JVM_HEAP_H
#define JVM_HEAP_H

#include <cstddef>
#include <string>
#include <sstream>
#include <pthread.h>
#include <vector>
#include <cassert>
#include "../jtypes.h"
#include "Memory.h"

class Class;

class Heap {
    void *raw;

    /* so called method area */

    Memory *classArea;
    Memory *bytecodeArea;
    Memory *methodArea;
    Memory *fieldArea;

    /* real heap saves objects */
    Memory *objectArea;

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

    void *allocFields(u2 fieldsCount);

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
