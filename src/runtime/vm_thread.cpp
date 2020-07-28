/*
 * Author: Yo Ka
 */

#include <cassert>
#include <thread>
#include "vm_thread.h"
#include "../jvmstd.h"
#include "../debug.h"
#include "../objects/class_loader.h"
#include "../objects/object.h"
#include "../metadata/class.h"
#include "../metadata/field.h"
#include "../objects/array_object.h"
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
    g_sys_thread_group = newObject(thread_group_class);

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
        tobj = newObject(thread_class);

    tobj->setLongField(eetop_field, (jlong) this);
    tobj->setIntField(S(priority), S(I), priority);
//    if (vmEnv.sysThreadGroup != nullptr)   todo
//        setThreadGroupAndName(vmEnv.sysThreadGroup, nullptr);
}

Thread *Thread::from(Object *tobj0)
{
    assert(tobj0 != nullptr);
    assert(0 <= eetop_field->id && eetop_field->id < tobj0->clazz->inst_field_count);
    jlong eetop = tobj0->getLongField(eetop_field);
    return reinterpret_cast<Thread *>(eetop);
}

Thread *Thread::from(jlong threadId)
{
    jvm_abort("ffffffffffffffffffffffff"); // todo

    for (Thread *t : g_all_threads) {
        // private long tid; // Thread ID
        auto tid = t->tobj->getLongField("tid", "J");
        if (tid == threadId)
            return t;
    }

    TRACE(NEW_MSG("invaild: %lld\n", threadId)); // todo 无效的 threadId
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
        jvm_abort("StackOverflowError");
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
//    jvm_abort("to_java_lang_management_ThreadInfo\n");
//    return nullptr;

    // private volatile String name;
    auto name = tobj->getRefField("name", "Ljava/lang/String;");
    // private long tid;
    auto tid = tobj->getLongField("tid", "J");

    Class *c = loadBootClass("java/lang/management/ThreadInfo");
    c->clinit();
    jref thread_info = newObject(c);
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

    Array *arr = newArray(loadArrayClass(S(array_java_lang_StackTraceElement)), size);
    for (size_t i = 0; i < size; i++) {
        Frame *f = vec[i];
        jref o = newObject(c);
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

Object *exceptionOccured() 
{
   return getCurrentThread()->exception; 
}

void setException(Object *exp) 
{
    getCurrentThread()->exception = exp;
}

void clearException() 
{
    getCurrentThread()->exception = nullptr;
}

void signalException(const char *excep_name, const char *message) 
{
    // if(VM_initing) {
    //     fprintf(stderr, "Exception occurred while VM initialising.\n");
    //     if(message)
    //         fprintf(stderr, "%s: %s\n", excep_name, message);
    //     else
    //         fprintf(stderr, "%s\n", excep_name);
    //     exit(1);
    // }

    assert(excep_name != nullptr);

    Class *ec = loadBootClass(excep_name);
    assert(ec != nullptr); // todo

    initClass(ec);
    jref exp = newObject(ec);
    if (message == nullptr) {
        execJavaFunc(ec->getConstructor("()V"), {exp});
    } else {
        execJavaFunc(ec->getConstructor("(Ljava/lang/String;)V"), { exp, newString(message) });
    }

    assert(getCurrentThread()->exception == nullptr); // todo

    getCurrentThread()->exception = exp;
}

void printStackTrace() 
{
    Thread *thread = getCurrentThread();
    jref e = thread->exception;
    if (e == nullptr)
        return;

    // private String detailMessage;
    jstrref msg = e->getRefField("detailMessage", S(sig_java_lang_String));
    printf("%s: %s\n", e->clazz->class_name, msg != nullptr ? msg->toUtf8() : "null");

    // [Ljava/lang/Object;
    auto backtrace = e->getRefField<Array>("backtrace", "Ljava/lang/Object;");
    for (int i = 0; i < backtrace->len; i++) {
        jref element = backtrace->get<jref>(i); // java.lang.StackTraceElement

        // private String declaringClass;
        // private String methodName;
        // private String fileName;
        // private int    lineNumber;
        jstrref declaring_class = element->getRefField("declaringClass", S(sig_java_lang_String));
        jstrref method_name = element->getRefField("methodName", S(sig_java_lang_String));
        jstrref file_name = element->getRefField("fileName", S(sig_java_lang_String));
        jint line_number = element->getIntField("lineNumber", S(I));

        printf("\tat %s.%s(%s:%d)\n",
               declaring_class->toUtf8(),
               method_name->toUtf8(),
               file_name ? file_name->toUtf8() : "(Unknown Source)",
               line_number);
    }

//    Class *throw_class = findSystemClass("java/lang/Throwable");
//    FieldBlock *field = findField(throw_class, "backtrace", "Ljava/lang/Object;");
//    MethodBlock *print = lookupMethod(writer->class, "println", "([C)V");
//    Object *array = (Object *)INST_DATA(excep)[field->offset];
//    char buff[256];
//    int *data, depth;
//    int i = 0;
//
//    if(array == NULL)
//        return;
//
//    data = &(INST_DATA(array)[1]);
//    depth = *INST_DATA(array);
//    for(; i < depth; ) {
//        MethodBlock *mb = (MethodBlock*)data[i++];
//        unsigned char *pc = (unsigned char *)data[i++];
//        ClassBlock *cb = CLASS_CB(mb->class);
//        unsigned char *dot_name = slash2dots(cb->name);
//            char *spntr = buff;
//            short *dpntr;
//            int len;
//
//        if(mb->access_flags & ACC_NATIVE)
//            len = sprintf(buff, "\tat %s.%s(Native method)", dot_name, mb->name);
//	    else if(cb->source_file_name == 0)
//		    len = sprintf(buff, "\tat %s.%s(Unknown source)", dot_name, mb->name);
//	    else
//		    len = sprintf(buff, "\tat %s.%s(%s:%d)", dot_name, mb->name, cb->source_file_name, mapPC2LineNo(mb, pc));
//
//        free(dot_name);
//        if((array = allocTypeArray(T_CHAR, len)) == NULL)
//            return;
//
//        dpntr = (short*)INST_DATA(array)+2;
//        for(; len > 0; len--)
//            *dpntr++ = *spntr++;
//
//        executeMethod(writer, print, array);
//    }
}