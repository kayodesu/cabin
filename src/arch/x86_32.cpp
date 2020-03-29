#include "../objects/method.h"
#include "../native/jni.h"
#include "../runtime/frame.h"

/*
 * Author: kayo
 */

jint JNICALL JVM_GetEnv(JavaVM *vm, void **penv, jint version);

void callJNIMethod(Frame *frame)
{
    assert(frame != nullptr && frame->method != nullptr);
    assert(frame->method->isNative() && frame->method->native_method != nullptr);

    JNIEnv *jni_env;
    jint r = JVM_GetEnv(nullptr, (void **)&jni_env, 0);
    assert(r == JNI_OK);

    Method *m = frame->method;
    u1 *sp;
    slot_t method_args[m->arg_slot_count + (m->isStatic() ? 2 : 1)]; // 为函数参数在栈上申请空间。
    asm volatile ("movl %%esp,%0" : "=m" (sp) :);

    // 准备参数
    // Notice: 在准备参数的过程中不能调用任何函数，否则会破坏栈帧。
    *(JNIEnv **)sp = jni_env;
    sp += sizeof(JNIEnv *);
    if(m->isStatic()) {
        *(jref *)sp = m->clazz;
        sp += sizeof(jref);
    }

    const char *p = m->type;
    const slot_t *slots = frame->lvars;
    assert(*p == '(');
    p++; // skip start (

    for (; *p != ')'; slots++, p++) {
        switch (*p) {
            case 'Z':
            case 'B':
                *(jbyte *)sp = JINT_TO_JBYTE(ISLOT(slots));
                sp += sizeof(jbyte);
                break;
            case 'C':
                *(jchar *)sp = JINT_TO_JCHAR(ISLOT(slots));
                sp += sizeof(jchar);
                break;
            case 'S':
                *(jshort *)sp = JINT_TO_JSHORT(ISLOT(slots));
                sp += sizeof(jshort);
                break;
            case 'I':
                *(jint *)sp = ISLOT(slots);
                sp += sizeof(jint);
                break;
            case 'F':
                *(jfloat *)sp = FSLOT(slots);
                sp += sizeof(jfloat);
                break;
            case 'J':
                *(jlong *)sp = LSLOT(slots);
                sp += sizeof(jlong);
                slots++;
                break;
            case 'D':
                *(jdouble *)sp = DSLOT(slots);
                sp += sizeof(jdouble);
                slots++;
                break;
            case '[':
                while (*++p == '[');
                if (*p != 'L') { // 基本类型的数组
                    goto __ref;
                }
            case 'L':
                while(*++p != ';');
            __ref:
                *(jref *)sp = RSLOT(slots);
                sp += sizeof(jref);
                break;
            default:
                // todo error
                break;
        }
    }

    switch (m->ret_type) {
        case Method::RET_VOID:
            ((void (*)()) m->native_method)();
            break;
        case Method::RET_BYTE:
            frame->pushi(((jbyte (*)()) m->native_method)());
            break;
        case Method::RET_BOOL:
            frame->pushi(((jbool (*)()) m->native_method)());
            break;
        case Method::RET_CHAR:
            frame->pushi(((jchar (*)()) m->native_method)());
            break;
        case Method::RET_SHORT:
            frame->pushi(((jshort (*)()) m->native_method)());
            break;
        case Method::RET_INT:
            frame->pushi(((jint (*)()) m->native_method)());
            break;
        case Method::RET_FLOAT:
            frame->pushf(((jfloat (*)()) m->native_method)());
            break;
        case Method::RET_LONG:
            frame->pushl(((jlong (*)()) m->native_method)());
            break;
        case Method::RET_DOUBLE:
            frame->pushd(((jdouble (*)()) m->native_method)());
            break;
        case Method::RET_REFERENCE:
            frame->pushr(((jref (*)()) m->native_method)());
            break;
        default:
            // todo error
            break;
    }
}
