#ifndef JVM_HEAP_H
#define JVM_HEAP_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <mutex>
#include "../jvmstd.h"

using address = uintptr_t;

class Heap {
    address mem;
    size_t size;

    struct Node {
        address head;
        size_t len;
        Node *next;
        Node(address head, size_t len, Node *next): head(head), len(len), next(next) { }
    } *freelist;

    std::recursive_mutex mutex;

    bool in(address p)
    {
        return mem <= p and p < mem + size;
    }

    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    void back(address p, size_t len);

    /*
     * 如果不在 freelist 里面，返回 p，
     * 负责跳过此 freelist's Node.
     */
    address jumpFreelist(address p);

public:
    Heap() noexcept;
    ~Heap();
    
    void *alloc(size_t len);

    // 堆总大小，以字节为单位。
    size_t totalMemory()
    {
        return size;
    }

    // 堆还有多少剩余空间，以字节为单位。
    size_t freeMemory();
    
    std::string toString();

    friend void gc();
};

#endif //JVM_HEAP_H
