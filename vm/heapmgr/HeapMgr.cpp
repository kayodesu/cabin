/*
 * Author: kayo
 */

#include <cstring>
#include <cassert>
#include <sstream>
#include "HeapMgr.h"
#include "../kayo.h"
#include "../config.h"

using namespace std;

HeapMgr::HeapMgr()
{
    heap = malloc(VM_HEAP_SIZE);
    freelist = new Node((uintptr_t) heap, VM_HEAP_SIZE, nullptr);
}

void *HeapMgr::get(size_t len)
{
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
            auto t = (void *) curr->head;
            delete curr;

            memset(t, 0, len);
            return t;
        }
        if (curr->len > len) {
            auto t = (void *) (curr->head);
            curr->head += len;
            curr->len -= len;

            memset(t, 0, len);
            return t;
        }
    }

    raiseException(STACK_OVERFLOW_ERROR); // todo 堆可以扩张
}

void HeapMgr::back(void *p, size_t len)
{
    if (p == nullptr || len == 0)
        return;

    auto mem = (uintptr_t)(p);

    Node *prev = nullptr;
    Node *curr = freelist;
    for (; curr != nullptr; prev = curr, curr = curr->next) {
        if (mem > curr->head)
            continue;

        assert(mem + len <= curr->head);
        if (prev == nullptr) {
            assert(curr == freelist);
            if (mem + len == curr->head) { // 空间右连续
                curr->head = mem;
                curr->len += len;
                return;
            } else { // 右不连续
                freelist = new Node(mem, len, curr);
                return;
            }
        }

        assert(prev != nullptr);
        assert(prev->head + prev->len <= mem);
        if (prev->head + prev->len == mem) {
            if (mem + len == curr->head) {
                // 左右都连续
                prev->len += (len + curr->len);
                prev->next = curr->next;
                delete curr;
                return;
            } else {
                // 左连续，右不连续
                prev->len += len;
                return;
            }
        } else {
            if (mem + len == curr->head) {
                // 左不连续，右连续
                curr->len += len;
                curr->head = mem;
                return;
            } else {
                // 左右都不连续
                prev->next = new Node(mem, len, curr);
                return;
            }
        }
    }
}

string HeapMgr::toString() const
{
    stringstream ss;

    ss << "heap: " << heap << endl;

    ss << "freelist: " << endl << '|';
    for (auto node = freelist; node != nullptr; node = node->next) {
        ss << (void *) node->head << ',';
        ss << node->len << "(0x" << hex << node->len << ")|";
    }

    return ss.str();
}

HeapMgr::~HeapMgr()
{
    for (auto p = freelist; p != nullptr;) {
        auto t = p->next;
        delete p;
        p = t;
    }

    free(heap);
}
