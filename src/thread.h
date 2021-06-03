#ifndef CABIN_THREAD_H
#define CABIN_THREAD_H

#include "cabin.h"
#include "slot.h"
#include "bytecode_reader.h"

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */


/* Thread states */

#define JVMTI_THREAD_STATE_ALIVE                     0x001
#define JVMTI_THREAD_STATE_TERMINATED                0x002
#define JVMTI_THREAD_STATE_RUNNABLE                  0x004
#define JVMTI_THREAD_STATE_WAITING_INDEFINITELY      0x010
#define JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT      0x020
#define JVMTI_THREAD_STATE_SLEEPING                  0x040
#define JVMTI_THREAD_STATE_WAITING                   0x080
#define JVMTI_THREAD_STATE_IN_OBJECT_WAIT            0x100
#define JVMTI_THREAD_STATE_PARKED                    0x200
#define JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER  0x400

#define CREATING           0x0
#define RUNNING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_RUNNABLE)
#define WAITING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_INDEFINITELY)
#define TIMED_WAITING      (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT)
#define OBJECT_WAIT        (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|WAITING)
#define OBJECT_TIMED_WAIT  (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|TIMED_WAITING)
#define SLEEPING           (JVMTI_THREAD_STATE_SLEEPING|TIMED_WAITING)
#define PARKED             (JVMTI_THREAD_STATE_PARKED|WAITING)
#define TIMED_PARKED       (JVMTI_THREAD_STATE_PARKED|TIMED_WAITING)
#define BLOCKED            JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER
#define TERMINATED         JVMTI_THREAD_STATE_TERMINATED

/* thread priorities */

#define THREAD_MIN_PRIORITY   1
#define THREAD_NORM_PRIORITY  5
#define THREAD_MAX_PRIORITY   10

///* Suspend states */
//
//#define SUSP_NONE      0
//#define SUSP_BLOCKING  1
//#define SUSP_CRITICAL  2
//#define SUSP_SUSPENDED 3
//
///* Park states */
//
//#define PARK_BLOCKED   0
//#define PARK_RUNNING   1
//#define PARK_PERMIT    2

typedef struct frame Frame;

typedef struct vm_thread {
    /*
     * VM stack 中的 Frame 布局：
     * ------------------------------------------------------------------
     * |lvars|Frame|ostack|, |lvars|Frame|ostack|, |lvars|Frame|ostack| ...
     * ------------------------------------------------------------------
     */
    u1 vm_stack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    Frame *top_frame;

    Object *tobj;  // 所关联的 Object of java.lang.Thread
    pthread_t tid; // 所关联的 local thread 对应的id

    jbool interrupted;
    
    jref exception;
} Thread;

Thread *create_thread(Object *_tobj, jint priority);

extern Thread *g_main_thread;

Thread *init_main_thread();

void create_vm_thread(void *(*start)(void *), const utf8_t *thread_name);

Thread *get_current_thread();

Thread *thread_from_tobj(Object *tobj);
Thread *thread_from_id(jlong thread_id);

void set_thread_group_and_name(Thread *, Object *group, const char *name);

void set_thread_status(Thread *, jint status);
jint get_thread_status(Thread *);

bool is_thread_alive(Thread *);

struct frame *alloc_frame(Thread *, Method *, bool vm_invoke);
#define pop_frame(_thrd) (_thrd)->top_frame = (_thrd)->top_frame->prev

/*
 * return a reference of java/lang/management/ThreadInfo
 * where maxDepth < 0 to request entire stack dump
 */
jref to_java_lang_management_ThreadInfo(const Thread *, jbool locked_monitors, jbool locked_synchronizers, jint max_depth);

int count_stack_frames(const Thread *);

/*
 * return [Ljava/lang/StackTraceElement;
 * where @max_depth < 0 to request entire stack dump
 */
jarrRef dump_thread(const Thread *, int max_depth);

/* Frame */

struct frame {
    Method *method;
    BytecodeReader reader;

    /*
     * this frame 执行的函数是否由虚拟机调用
     * 由虚拟机调用的函数不会将返回值压入下层frame的栈中，
     * 也不会后续执行其下层frame，而是直接返回。
     */
    bool vm_invoke;

    Frame *prev;
    
    jref jni_local_ref_table[JNI_LOCAL_REFERENCE_TABLE_MAX_CAPACITY];
    int jni_local_ref_count;

    slot_t *lvars;   // local variables
    slot_t *ostack;  // operand stack
};

void init_frame(Frame *_this, Method *m, bool vm_invoke, slot_t *lvars, slot_t *ostack, Frame *prev);

// push to ostack.
static inline void ostack_push(Frame *f, slot_t v)   { *f->ostack++ = v; }
static inline void ostack_pushi(Frame *f, jint v)    { slot_set_int(f->ostack, v); f->ostack++; }
static inline void ostack_pushf(Frame *f, jfloat v)  { slot_set_float(f->ostack, v); f->ostack++; }
static inline void ostack_pushl(Frame *f, jlong v)   { slot_set_long(f->ostack, v); f->ostack += 2; }
static inline void ostack_pushd(Frame *f, jdouble v) { slot_set_double(f->ostack, v); f->ostack += 2; }
static inline void ostack_pushr(Frame *f, jref v)    { slot_set_ref(f->ostack, v); f->ostack++; }

// pop from ostack.
static inline jint    ostack_popi(Frame *f) { f->ostack--;    return slot_get_int(f->ostack); }
static inline jfloat  ostack_popf(Frame *f) { f->ostack--;    return slot_get_float(f->ostack); }
static inline jlong   ostack_popl(Frame *f) { f->ostack -= 2; return slot_get_long(f->ostack); }
static inline jdouble ostack_popd(Frame *f) { f->ostack -= 2; return slot_get_double(f->ostack); }
static inline jref    ostack_popr(Frame *f) { f->ostack--;    return slot_get_ref(f->ostack); }

// the end address of this frame
#define get_frame_end_address(_frame) ((intptr_t)((_frame)->ostack + (_frame)->method->max_stack))

#define clear_frame_stack(_frame) (_frame)->ostack = (slot_t *)((_frame) + 1)

char *get_frame_info(const Frame *);

#endif // CABIN_THREAD_H