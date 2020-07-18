/*
 * Author: Yo Ka
 */

#ifndef KAYOVM_MONITOR_H
#define KAYOVM_MONITOR_H

//#include <mutex>

class Thread;

class Monitor {
//    std::recursive_mutex mutex;
//    pthread_cond_t cond;
    Thread *owner = nullptr;
    int count = 0;
    int waiting = 0;
    int notifying = 0;
    int interrupting = 0;
    int entering = 0;
 //   Monitor *next;
//    char in_use;
public:
//    Monitor();
//
//    void lock(Thread *thread);
//
//    bool tryLock(Thread *thread);
//
//    void unlock(Thread *thread);
//
//    bool wait(Thread *thread);
//
//    bool timedwait(Thread *thread, long long ms, int ns);
//
//    bool notify(Thread *thread);
//    bool notifyAll(Thread *thread);
};


#endif //KAYOVM_MONITOR_H
