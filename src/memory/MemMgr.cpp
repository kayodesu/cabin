/*
 * Author: kayo
 */

#include <cassert>
#include <cstring>
#include <sstream>
#include "MemMgr.h"
#include "../kayo.h"

using namespace std;

MemMgr::MemMgr(address mem, size_t size)
{
    assert(mem != 0);
    assert(size > 0);

    freelist = new Node(mem, size, nullptr);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); // 同一线程可重入的锁

    pthread_mutex_init(&mutex, &attr);
}

MemMgr::~MemMgr()
{
    for (auto p = freelist; p != nullptr;) {
        auto t = p->next;
        delete p;
        p = t;
    }
}

void MemMgr::lock()
{
    pthread_mutex_lock(&mutex);
}

void MemMgr::unlock()
{
    pthread_mutex_unlock(&mutex);
}

address MemMgr::jumpFreelist(address p)
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

void *MemMgr::get(size_t len)
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

void MemMgr::back(address p, size_t len)
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

string MemMgr::toString()
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
