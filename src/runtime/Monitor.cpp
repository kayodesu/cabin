/*
 * Author: kayo
 */

#include "Monitor.h"
#include "thread.h"

Monitor::Monitor()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); // 同一线程可重入的锁

    pthread_mutex_init(&mutex, &attr);
}

void Monitor::lock(Thread *thread)
{
    assert(thread != nullptr);

    if(owner == thread) {
        count++;
    } else {
        entering++;
    //    disableSuspend(thread);
        thread->setStatus(BLOCKED);
        pthread_mutex_lock(&mutex);
        thread->setStatus(RUNNING);
    //    enableSuspend(thread);
        entering--;
        owner = thread;
    }
}

bool Monitor::tryLock(Thread *thread)
{
    assert(thread != nullptr);

    if(owner == thread) {
        count++;
    } else {
        if(pthread_mutex_trylock(&mutex) != 0)
            return false;
        owner = thread;
    }

    return true;
}

void Monitor::unlock(Thread *thread)
{
    assert(thread != nullptr);

    if(owner == thread) {
        if(count == 0) {
            owner = nullptr;
            pthread_mutex_unlock(&mutex);
        } else {
            count--;
        }
    }
}

bool Monitor::wait(Thread *thread)
{
    assert(thread != nullptr);

    // Check we own the monitor
    if(owner != thread)
        return false; // todo

    // todo ...................
    //pthread_cond_wait()
}

bool Monitor::timedwait(Thread *thread, long long ms, int ns)
{
    assert(thread != nullptr);

    // todo ...................
    //pthread_cond_timedwait()
}

bool Monitor::notify(Thread *thread)
{
    assert(thread != nullptr);
}

bool Monitor::notifyAll(Thread *thread)
{
    assert(thread != nullptr);
}
