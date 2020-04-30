/*
 * Author: Yo Ka
 */

#ifndef KAYOVM_MEM_MGR_H
#define KAYOVM_MEM_MGR_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <mutex>

using address = uintptr_t;

class MemMgr {
    struct Node {
        address head;
        size_t len;
        Node *next;
        Node(address head, size_t len, Node *next): head(head), len(len), next(next) { }
    } *freelist;

    std::recursive_mutex mutex;

    address mem;
    size_t size;

    bool in(address p)
    {
        return mem <= p and p < mem + size;
    }

public:
    MemMgr(address mem, size_t size);
    virtual ~MemMgr();

    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    virtual void *get(size_t len);
    void back(address p, size_t len);

    /*
     * 如果不在 freelist 里面，返回 p，
     * 负责跳过此 freelist's Node.
     */
    address jumpFreelist(address p);

    address getMem()
    {
        return mem;
    }

    size_t getSize() const
    {
        return size;
    }

    virtual std::string toString();
};

#endif //KAYOVM_MEM_MGR_H
