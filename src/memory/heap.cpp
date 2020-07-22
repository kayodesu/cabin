#include <vector>
#include "heap.h"
#include "../metadata/class.h"
#include "../metadata/method.h"
#include "../metadata/field.h"
#include "../config.h"

/*
 * Author: Yo Ka
 */

using namespace std;


Heap::Heap() noexcept
{
    size = VM_HEAP_SIZE;
    assert(size > 0);

    mem = (address) malloc(size);
    assert(mem != 0);    

    freelist = new Node(mem, size, nullptr);
}

Heap::~Heap()
{    
    for (auto p = freelist; p != nullptr;) {
        auto t = p->next;
        delete p;
        p = t;
    }

    free((void *) mem);
}


address Heap::jumpFreelist(address p)
{
    assert(in(p));

    Node *curr = freelist;
    for (; curr != nullptr; curr = curr->next) {
        if (p < curr->head)
            return p; // p is not in freelist
        auto offset = p - curr->head;
        if (0 <= offset and offset <= curr->len) {
            // p is in freelist, jump
            return p + curr->len;
        }
    }

    return p; // p is not in freelist
}

void *Heap::alloc(size_t len)
{
    assert(len > 0);
    lock();

    void *p = nullptr;
    Node *prev = nullptr;
    Node *curr = freelist;
    for (; curr != nullptr; prev = curr, curr = curr->next) {
        if (curr->len == len) {
            if (prev != nullptr) {
                prev->next = curr->next;
            } else {
                assert(curr == freelist);
                freelist = curr->next;
            }
            p = (void *) curr->head;
            delete curr;

            goto over;
        }

        if (curr->len > len) {
            p = (void *) (curr->head);
            curr->head += len;
            curr->len -= len;

            goto over;
        }
    }

over:
    unlock();

    if (p != nullptr) {
        memset(p, 0, len);
        return p;
    }

    jvm_abort("java_lang_OutOfMemoryError"); // todo 堆可以扩张
}

void Heap::back(address p, size_t len)
{
    assert(in(p));
    assert(len > 0);

    lock();

    Node *prev = nullptr;
    Node *curr = freelist;
    for (; curr != nullptr; prev = curr, curr = curr->next) {
        if (p > curr->head)
            continue;

        assert(p + len <= curr->head);
        if (prev == nullptr) {
            assert(curr == freelist);
            if (p + len == curr->head) { // 空间右连续
                curr->head = p;
                curr->len += len;
                goto over;
            } else { // 右不连续
                freelist = new Node(p, len, curr);
                goto over;
            }
        }

        assert(prev != nullptr);
        assert(prev->head + prev->len <= p);
        if (prev->head + prev->len == p) {
            if (p + len == curr->head) {
                // 左右都连续
                prev->len += (len + curr->len);
                prev->next = curr->next;
                delete curr;
                goto over;
            } else {
                // 左连续，右不连续
                prev->len += len;
                goto over;
            }
        } else {
            if (p + len == curr->head) {
                // 左不连续，右连续
                curr->len += len;
                curr->head = p;
                goto over;
            } else {
                // 左右都不连续
                prev->next = new Node(p, len, curr);
                goto over;
            }
        }
    }

over:
    unlock();
}

size_t Heap::freeMemory()
{
    size_t free_mem = 0;
    for (auto node = freelist; node != nullptr; node = node->next) {
        free_mem += node->len;
    }
    return free_mem;
}

string Heap::toString()
{
    lock();
    stringstream ss;

    ss << "freelist: " << endl << '|';
    for (auto node = freelist; node != nullptr; node = node->next) {
        ss << (void *) node->head << ',';
        ss << node->len << "(0x" << hex << node->len << ")|";
    }

    unlock();
    return ss.str();
}