#include <assert.h>
// #include <thread>
// #include <mutex>
#include "vm_thread.h"
#include "../cabin.h"
#include "../debug.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../metadata/class.h"
#include "../metadata/field.h"
#include "../interpreter/interpreter.h"
#include "frame.h"

#if TRACE_THREAD
#define TRACE PRINT_TRACE
#else
#define TRACE(x)
#endif

// using namespace std;

static pthread_key_t key;
// thread_local Thread *curr_thread;

Thread *getCurrentThread()
{
    return (Thread *) pthread_getspecific(key);
    // return curr_thread;
}

static inline void saveCurrentThread(Thread *thread)
{
    // curr_thread = thread;
    pthread_setspecific(key, thread);
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
    thread_class = load_boot_class(S(java_lang_Thread));

    eetop_field = lookup_inst_field0(thread_class, "eetop", S(J));
    thread_status_field = lookup_inst_field0(thread_class, "threadStatus", S(I));

    pthread_key_create(&key, NULL);

    g_main_thread = create_thread(NULL, THREAD_NORM_PRIORITY);

    initClass(thread_class);

    Class *thread_group_class = load_boot_class(S(java_lang_ThreadGroup));
    g_sys_thread_group = alloc_object(thread_group_class);

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    initClass(thread_group_class);
    Method *constructor = get_constructor(thread_group_class, S(___V));
    exec_java_func1(constructor, g_sys_thread_group);

    set_thread_group_and_name(g_main_thread, g_sys_thread_group, MAIN_THREAD_NAME);
    saveCurrentThread(g_main_thread);
    return g_main_thread;
}

void createVMThread(void *(*start)(void *), const utf8_t *thread_name)
{
    assert(start != NULL && thread_name != NULL); // vm thread must have a name

//    static auto start = [](void *args) {
//        auto a = (VMThreadInitInfo *) args;
//        auto newThread = new Thread();
//        newThread->setThreadGroupAndName(sysThreadGroup, a->threadName);
//        return a->start(NULL);
//    };
//
//    pthread_t tid;
//    int ret = pthread_create(&tid, NULL, start, (void *) info);
//    if (ret != 0) {
//        thread_throw(new InternalError("create Thread failed"));
//    }

//    static auto __start = [](void *(*start)(void *), const utf8_t *thread_name) {
//        auto new_thread = new Thread();
//        new_thread->setThreadGroupAndName(sysThreadGroup, thread_name);
//        return start(NULL);
//    };
//
//    std::thread t(__start, start, thread_name);
//    t.detach();
}

// static mutex new_thread_mutex;
static pthread_mutex_t new_thread_mutex = PTHREAD_MUTEX_INITIALIZER;

Thread *create_thread(Object *_tobj, jint priority)
{
    assert(THREAD_MIN_PRIORITY <= priority && priority <= THREAD_MAX_PRIORITY);

    Thread *t = (Thread *)vm_calloc(sizeof(Thread));

    t->tobj = _tobj;

    // scoped_lock lock(new_thread_mutex);
    pthread_mutex_lock(&new_thread_mutex);

    saveCurrentThread(t);
//    g_all_threads.push_back(this);
    add_thread(t);

    t->tid = pthread_self();
    // tid = this_thread::get_id();

    if (t->tobj == NULL)
        t->tobj = alloc_object(thread_class);

    set_long_field0(t->tobj, eetop_field, (jlong) t);
    set_int_field(t->tobj, S(priority), priority);
//    if (vmEnv.sysThreadGroup != NULL)   todo
//        setThreadGroupAndName(vmEnv.sysThreadGroup, NULL);


    pthread_mutex_unlock(&new_thread_mutex);
    return t;
}

Thread *thread_from_tobj(Object *tobj)
{
    assert(tobj != NULL);
    assert(0 <= eetop_field->id && eetop_field->id < tobj->clazz->inst_fields_count);
    jlong eetop = get_long_field0(tobj, eetop_field);
    return (Thread *)eetop;
}

Thread *thread_from_id(jlong thread_id)
{
    JVM_PANIC("ffffffffffffffffffffffff"); // todo

    for (int i = 0; i < g_all_threads_count; i++) {
        // private long tid; // Thread ID
        jlong tid = get_long_field(g_all_threads[i]->tobj, "tid");
        if (tid == thread_id)
            return g_all_threads[i];
    }

    // todo 无效的 thread_id
    return NULL;
}

void set_thread_group_and_name(Thread *thrd, Object *group, const char *name)
{
    assert(thrd != NULL && group != NULL && name != NULL);

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = get_constructor(thrd->tobj->clazz, "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    exec_java_func3(constructor, thrd->tobj, group, alloc_string(name));
}

void set_thread_status(Thread *thrd, jint status)
{
    set_int_field0(thrd->tobj, thread_status_field, status);
}

jint get_thread_status(Thread *thrd)
{
    return get_int_field0(thrd->tobj, thread_status_field);
}

bool is_thread_alive(Thread *thrd)
{
    assert(thrd != NULL && thrd->tobj != NULL);
    // auto status = tobj->getIntField("threadStatus", "I");
    // todo
    return false;
}

Frame *alloc_frame(Thread *thrd, Method *m, bool vm_invoke)
{
    assert(thrd != NULL && m != NULL);

    intptr_t mem = thrd->top_frame == NULL ? (intptr_t) thrd->vm_stack : get_frame_end_address(thrd->top_frame);
    size_t size = sizeof(Frame) + (m->max_locals + m->max_stack) * sizeof(slot_t);
    if (mem + size - (intptr_t) thrd->vm_stack > VM_STACK_SIZE) {
//        thread_throw(new StackOverflowError);
        // todo 栈已经溢出无法执行程序了。不要抛异常了，无法执行了。
        JVM_PANIC("StackOverflowError");
    }

    slot_t *lvars = (slot_t *)(mem);
    Frame *new_frame = (Frame *)(lvars + m->max_locals);
    slot_t *ostack = (slot_t *)(new_frame + 1);
    // thrd->top_frame = new (new_frame) Frame(m, vm_invoke, lvars, ostack, thrd->top_frame);    
    init_frame(new_frame, m, vm_invoke, lvars, ostack, thrd->top_frame);
    thrd->top_frame = new_frame;
    return thrd->top_frame;
}

int count_stack_frames(const Thread *thrd)
{
    int count = 0;
    for (Frame *frame = thrd->top_frame; frame != NULL; frame = frame->prev) {
        count++;
    }
    return count;
}

// vector<Frame *> Thread::getStackFrames()
// {
//     vector<Frame *> vec;
//     for (auto frame = top_frame; frame != NULL; frame = frame->prev) {
//         vec.push_back(frame);
//     }
//     vec.reserve(vec.size());
//     assert((int) vec.size() == count_stack_frames(this));
//     return vec;
// }

jref to_java_lang_management_ThreadInfo(const Thread *thrd, jbool locked_monitors, jbool locked_synchronizers, jint max_depth)
{
    // todo
//    JVM_PANIC("to_java_lang_management_ThreadInfo\n");
//    return NULL;

    // private volatile String name;
    jstring name = get_ref_field(thrd->tobj, "name", "Ljava/lang/String;");
    // private long tid;
    jlong tid = get_long_field(thrd->tobj, "tid");

    Class *c = load_boot_class("java/lang/management/ThreadInfo");
    clinit(c);
    jref thread_info = alloc_object(c);
    // private String threadName;
    set_ref_field(thread_info, "threadName", "Ljava/lang/String;", name);
    // private long threadId;
    set_long_field(thread_info, "threadId", tid);

    return thread_info;
}

jarray dump_thread(const Thread *thrd, int max_depth)
{
    assert(thrd != NULL);

    int count = count_stack_frames(thrd);
    if (max_depth >= 0 && count > max_depth) {
        count = max_depth;
    }

    Class *c = load_boot_class(S(java_lang_StackTraceElement));
    // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber);
    Method *constructor = get_constructor(c, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    jarray arr = alloc_array0(S(array_java_lang_StackTraceElement), count);
    size_t i = count - 1;
    for (Frame *f = thrd->top_frame; f != NULL && i >= 0; f = f->prev, i--) {
        jref o = alloc_object(c);
        exec_java_func(constructor, (slot_t []) { rslot(o),
                                    rslot(alloc_string(f->method->clazz->class_name)),
                                    rslot(alloc_string(f->method->name)),
                                    rslot(alloc_string(f->method->clazz->source_file_name)),
                                    islot(get_line_number(f->method, f->reader.pc)) }
        );
        jarray_set_ref(arr, i, o);
    }

    return arr;
}

// jarray Thread::dump(int max_depth)
// {
//     vector<Frame *> vec = getStackFrames();
//     size_t size = vec.size();
//     if (max_depth >= 0 && size > (size_t) max_depth) {
//         size = (size_t) max_depth;
//     }

//     auto c = loadBootClass(S(java_lang_StackTraceElement));
//     // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber);
//     Method *constructor = get_constructor(c, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

//     jarray arr = alloc_array0(S(array_java_lang_StackTraceElement), size);
//     for (size_t i = 0; i < size; i++) {
//         Frame *f = vec[i];
//         jref o = alloc_object(c);
//         execJavaFunc(constructor, { rslot(o),
//                                     rslot(alloc_string(f->method->clazz->class_name)),
//                                     rslot(alloc_string(f->method->name)),
//                                     rslot(alloc_string(f->method->clazz->source_file_name)),
//                                     islot(get_line_number(f->method, f->reader.pc)) }
//         );
//         jarray_set_ref(arr, i, o);
//     }

//     return arr;
// }
