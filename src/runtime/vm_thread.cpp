#include <cassert>
#include <thread>
#include "vm_thread.h"
#include "../cabin.h"
#include "../debug.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../metadata/class.h"
#include "../metadata/field.h"
#include "../objects/array.h"
#include "../interpreter/interpreter.h"
#include "frame.h"

#if TRACE_THREAD
#define TRACE PRINT_TRACE
#else
#define TRACE(x)
#endif

using namespace std;
using namespace slot;

thread_local Thread *curr_thread;

Thread *getCurrentThread()
{
    return curr_thread;
}

static inline void saveCurrentThread(Thread *thread)
{
    curr_thread = thread;
}

// Various field and method into java.lang.Thread cached at startup and used in thread creation
static Field *eetop_field;
static Field *thread_status_field;
// static Method *runMethod;

// Cached java.lang.Thread class
static Class *thread_class;

Thread *g_main_thread;

Thread *initMainThread()
{
    thread_class = loadBootClass(S(java_lang_Thread));

    eetop_field = thread_class->lookupInstField("eetop", S(J));
    thread_status_field = thread_class->lookupInstField("threadStatus", S(I));

    g_main_thread = new Thread();

    initClass(thread_class);

    Class *thread_group_class = loadBootClass(S(java_lang_ThreadGroup));
    g_sys_thread_group = thread_group_class->allocObject();

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    initClass(thread_group_class);
    execJavaFunc(thread_group_class->getConstructor(S(___V)), {g_sys_thread_group});

    g_main_thread->setThreadGroupAndName(g_sys_thread_group, MAIN_THREAD_NAME);
    saveCurrentThread(g_main_thread);
    return g_main_thread;
}

void createVMThread(void *(*start)(void *), const utf8_t *thread_name)
{
    assert(start != nullptr && thread_name != nullptr); // vm thread must have a name

//    static auto start = [](void *args) {
//        auto a = (VMThreadInitInfo *) args;
//        auto newThread = new Thread();
//        newThread->setThreadGroupAndName(sysThreadGroup, a->threadName);
//        return a->start(nullptr);
//    };
//
//    pthread_t tid;
//    int ret = pthread_create(&tid, nullptr, start, (void *) info);
//    if (ret != 0) {
//        thread_throw(new InternalError("create Thread failed"));
//    }

//    static auto __start = [](void *(*start)(void *), const utf8_t *thread_name) {
//        auto new_thread = new Thread();
//        new_thread->setThreadGroupAndName(sysThreadGroup, thread_name);
//        return start(nullptr);
//    };
//
//    std::thread t(__start, start, thread_name);
//    t.detach();
}

static mutex new_thread_mutex;

Thread::Thread(Object *tobj0, jint priority): tobj(tobj0)
{
    assert(THREAD_MIN_PRIORITY <= priority && priority <= THREAD_MAX_PRIORITY);

    scoped_lock lock(new_thread_mutex);

    saveCurrentThread(this);
    g_all_threads.push_back(this);

    // tid = pthread_self();
    tid = this_thread::get_id();

    if (tobj == nullptr)
        tobj = thread_class->allocObject();

    tobj->setLongField(eetop_field, (jlong) this);
    tobj->setIntField(S(priority), S(I), priority);
//    if (vmEnv.sysThreadGroup != nullptr)   todo
//        setThreadGroupAndName(vmEnv.sysThreadGroup, nullptr);
}

Thread *Thread::from(Object *tobj0)
{
    assert(tobj0 != nullptr);
    assert(0 <= eetop_field->id && eetop_field->id < tobj0->clazz->inst_fields_count);
    jlong eetop = tobj0->getLongField(eetop_field);
    return reinterpret_cast<Thread *>(eetop);
}

Thread *Thread::from(jlong threadId)
{
    JVM_PANIC("ffffffffffffffffffffffff"); // todo

    for (Thread *t : g_all_threads) {
        // private long tid; // Thread ID
        auto tid = t->tobj->getLongField("tid", "J");
        if (tid == threadId)
            return t;
    }

    // todo 无效的 threadId
    return nullptr;
}

void Thread::setThreadGroupAndName(Object *thread_group, const char *thread_name)
{
    assert(thread_group != nullptr);
    assert(thread_name != nullptr);

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = tobj->clazz->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    execJavaFunc(constructor, { tobj, thread_group, newString(thread_name) });
}

void Thread::setStatus(jint status)
{
    tobj->setIntField(thread_status_field, status);
}

jint Thread::getStatus()
{
    return tobj->getIntField(thread_status_field);
}

bool Thread::isAlive()
{
    assert(tobj != nullptr);
    // auto status = tobj->getIntField("threadStatus", "I");
    // todo
    return false;
}

Frame *Thread::allocFrame(Method *m, bool vm_invoke)
{
    assert(m != nullptr);

    intptr_t mem = top_frame == nullptr ? (intptr_t) vm_stack : top_frame->end();
    auto size = sizeof(Frame) + (m->max_locals + m->max_stack) * sizeof(slot_t);
    if (mem + size - (intptr_t) vm_stack > VM_STACK_SIZE) {
//        thread_throw(new StackOverflowError);
        // todo 栈已经溢出无法执行程序了。不要抛异常了，无法执行了。
        JVM_PANIC("StackOverflowError");
    }

    auto lvars = (slot_t *)(mem);
    auto new_frame = (Frame *)(lvars + m->max_locals);
    auto ostack = (slot_t *)(new_frame + 1);
    top_frame = new (new_frame) Frame(m, vm_invoke, lvars, ostack, top_frame);
    return top_frame;
}

void Thread::popFrame()
{
    assert(top_frame != nullptr);
    top_frame = top_frame->prev;
}

int Thread::countStackFrames()
{
    int count = 0;
    for (Frame *frame = top_frame; frame != nullptr; frame = frame->prev) {
        count++;
    }
    return count;
}

vector<Frame *> Thread::getStackFrames()
{
    vector<Frame *> vec;
    for (auto frame = top_frame; frame != nullptr; frame = frame->prev) {
        vec.push_back(frame);
    }
    vec.reserve(vec.size());
    assert((int) vec.size() == countStackFrames());
    return vec;
}

jref Thread::to_java_lang_management_ThreadInfo(jbool locked_monitors, jbool locked_synchronizers, jint max_depth)
{
    // todo
//    JVM_PANIC("to_java_lang_management_ThreadInfo\n");
//    return nullptr;

    // private volatile String name;
    auto name = tobj->getRefField("name", "Ljava/lang/String;");
    // private long tid;
    auto tid = tobj->getLongField("tid", "J");

    Class *c = loadBootClass("java/lang/management/ThreadInfo");
    c->clinit();
    jref thread_info = c->allocObject();
    // private String threadName;
    thread_info->setRefField("threadName", "Ljava/lang/String;", name);
    // private long threadId;
    thread_info->setLongField("threadId", "J", tid);

    return thread_info;
}

Array *Thread::dump(int max_depth)
{
    vector<Frame *> vec = getStackFrames();
    size_t size = vec.size();
    if (max_depth >= 0 && size > (size_t) max_depth) {
        size = (size_t) max_depth;
    }

    auto c = loadBootClass(S(java_lang_StackTraceElement));
    // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber);
    Method *constructor = c->getConstructor("(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    Array *arr = newArray(S(array_java_lang_StackTraceElement), size);
    for (size_t i = 0; i < size; i++) {
        Frame *f = vec[i];
        jref o = c->allocObject();
        execJavaFunc(constructor, { rslot(o),
                                    rslot(newString(f->method->clazz->class_name)),
                                    rslot(newString(f->method->name)),
                                    rslot(newString(f->method->clazz->source_file_name)),
                                    islot(f->method->getLineNumber(f->reader.pc)) }
        );
        arr->setRef(i, o);
    }

    return arr;
}
