/*
 * Author: kayo
 */

#ifndef JVM_HEAP_MANAGER_H
#define JVM_HEAP_MANAGER_H

#include <cstddef>
#include <string>
#include "../jtypes.h"

class HeapMgr {
    struct Node {
        uintptr_t head;
        size_t len;
        Node *next;
        Node(uintptr_t head, size_t len, Node *next): head(head), len(len), next(next) { }
    } *freelist;

    void *heap;

public:
    HeapMgr();
    ~HeapMgr();

    void *get(size_t len);
    void back(void *p, size_t len);
    
    std::string toString() const;
};

#endif //JVM_HEAP_MANAGER_H
