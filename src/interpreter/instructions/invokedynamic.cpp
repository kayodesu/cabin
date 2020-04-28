#include "../../jvmstd.h"
#include "../../objects/invoke.h"
#include "../../objects/object.h"
#include "../../objects/class.h"
#include "../../objects/method.h"
#include "../interpreter.h"

/*
 * Author: Yo Ka
 */

using namespace method_type;
using namespace method_handles;

void invokedynamic(BytecodeReader &reader, Class &clazz, slot_t *&ostack)
{
    ConstantPool &cp = clazz.cp;

    u2 index = reader.readu2(); // point to JVM_CONSTANT_InvokeDynamic_info
    reader.readu1(); // this byte must always be zero.
    reader.readu1(); // this byte must always be zero.

    const utf8_t *invokedName = cp.invokeDynamicMethodName(index);
    const utf8_t *invokedDescriptor = cp.invokeDynamicMethodType(index);

    auto invokedType = fromMethodDescriptor(invokedDescriptor, clazz.loader);
    auto caller = getCaller();

    BootstrapMethod &bm = clazz.bootstrap_methods.at(cp.invokeDynamicBootstrapMethodIndex(index));
    u2 refKind = cp.methodHandleReferenceKind(bm.bootstrapMethodRef);
    u2 refIndex = cp.methodHandleReferenceIndex(bm.bootstrapMethodRef);

    switch (refKind) {
        case JVM_REF_invokeStatic: {
            const utf8_t *className = cp.methodClassName(refIndex);
            Class *bootstrapClass = loadClass(clazz.loader, className);

            // bootstrap method is static,  todo 对不对
            // 前三个参数固定为 MethodHandles.Lookup caller, String invokedName, MethodType invokedType todo 对不对
            // 后续的参数由 ref->argc and ref->args 决定
            Method *bootstrapMethod = bootstrapClass->getDeclaredStaticMethod(
                    cp.methodName(refIndex), cp.methodType(refIndex));
            // args's length is big enough,多余的长度无所谓，bootstrapMethod 会按需读取的。
            slot_t args[3 + bm.bootstrapArguments.size() * 2];
            RSLOT(args) = caller;
            RSLOT(args + 1) = newString(invokedName);
            RSLOT(args + 2) = invokedType;
            bm.resolveArgs(&cp, args + 3);
            auto callSet = RSLOT(execJavaFunc(bootstrapMethod, args));

            // public abstract MethodHandle dynamicInvoker()
            auto dynInvoker = callSet->clazz->lookupInstMethod("dynamicInvoker", "()Ljava/lang/invoke/MethodHandle;");
            auto exactMethodHandle = RSLOT(execJavaFunc(dynInvoker, callSet));

            // public final Object invokeExact(Object... args) throws Throwable
            Method *invokeExact = exactMethodHandle->clazz->lookupInstMethod(
                    "invokeExact", "([Ljava/lang/Object;)Ljava/lang/Object;");
            assert(invokeExact->isVarargs());
            u2 slotsCount = Method::calArgsSlotsCount(invokedDescriptor, true);
            slot_t __args[slotsCount];
            RSLOT(__args) = exactMethodHandle; // __args[0] = (slot_t) exactMethodHandle;
            slotsCount--; // 减去"this"
            ostack -= slotsCount; // pop all args
            memcpy(__args + 1, ostack, slotsCount * sizeof(slot_t));
            // invoke exact method, invokedynamic completely execute over.
            execJavaFunc(invokeExact, __args);

            break;
        }
        case JVM_REF_newInvokeSpecial:
            jvm_abort("JVM_REF_newInvokeSpecial"); // todo
            break;
        default:
            jvm_abort("never goes here"); // todo
            break;
    }
    jvm_abort("never goes here"); // todo
}
