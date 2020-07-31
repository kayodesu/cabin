#include "../../../jni_inner.h"
#include "../../../../jvmstd.h"
#include "../../../../runtime/frame.h"
#include "../../../../metadata/class.h"
#include "../../../../metadata/field.h"
#include "../../../../metadata/method.h"
#include "../../../../objects/mh.h"
#include "../../../../objects/array_object.h"
#include "../../../../interpreter/interpreter.h"

/*
 * Author: Yo Ka
 */

using namespace utf8;
using namespace member_name;

/*
 * Fetch MH-related JVM parameter.
 * which=0 retrieves MethodHandlePushLimit
 * which=1 retrieves stack slot push size (in address units)
 * 
 * static native int getConstant(int which);
 */
static jint getConstant(jint which)
{
    // todo 啥意思
    // if (which == 4)
    //     return 1;
    // else
        return 0;
}

/* Method flags */

#define MB_LAMBDA_HIDDEN        1
#define MB_LAMBDA_COMPILED      2
#define MB_CALLER_SENSITIVE     4
#define MB_DEFAULT_CONFLICT     8


#define IS_METHOD        0x010000
#define IS_CONSTRUCTOR   0x020000
#define IS_FIELD         0x040000
#define IS_TYPE          0x080000
#define CALLER_SENSITIVE 0x100000

#define SEARCH_SUPERCLASSES 0x100000
#define SEARCH_INTERFACES   0x200000

#define ALL_KINDS (IS_METHOD | IS_CONSTRUCTOR | IS_FIELD | IS_TYPE)
                
#define REFERENCE_KIND_SHIFT 24
#define REFERENCE_KIND_MASK  (0xf000000 >> REFERENCE_KIND_SHIFT)

static int __method_flags(Method *m) 
{
    assert(m != nullptr);

    int flags = m->access_flags;

    if(m->access_flags & MB_CALLER_SENSITIVE)
        flags |= CALLER_SENSITIVE;

    return flags;
}

// static native void init(MemberName self, Object ref);
static void init(jobject self, jobject ref)
{
    initMemberName(self, ref);
}
/*
func getMNFlags(method *heap.Method) int32 {
	flags := int32(method.AccessFlags)
	if method.IsStatic() {
		flags |= MN_IS_METHOD | (references.RefInvokeStatic << MN_REFERENCE_KIND_SHIFT)
	} else if method.IsConstructor() {
		flags |= MN_IS_CONSTRUCTOR | (references.RefInvokeSpecial << MN_REFERENCE_KIND_SHIFT)
	} else {
		flags |= MN_IS_METHOD | (references.RefInvokeSpecial << MN_REFERENCE_KIND_SHIFT)
	}
	return flags
}
*/

// static native void expand(MemberName self);
static void expand(jobject self)
{
    expandMemberName(self);
}

/*
 * todo 说明这函数是干嘛的。。。。。。。。。
 * // There are 4 entities in play here:
//   * LC: lookupClass
//   * REFC: symbolic reference class (MN.clazz before resolution);
//   * DEFC: resolved method holder (MN.clazz after resolution);
//   * PTYPES: parameter types (MN.type)
//
// What we care about when resolving a MemberName is consistency between DEFC and PTYPES.
// We do type alias (TA) checks on DEFC to ensure that. DEFC is not known until the JVM
// finishes the resolution, so do TA checks right after MHN.resolve() is over.
//
// All parameters passed by a caller are checked against MH type (PTYPES) on every invocation,
// so it is safe to call a MH from any context.
//
// REFC view on PTYPES doesn't matter, since it is used only as a starting point for resolution and doesn't
// participate in method selection.
 *
 * static native MemberName resolve(MemberName self, Class<?> caller) throws LinkageError, ClassNotFoundException;
 */
static jobject resolve(jobject self/*MemberName*/, jclass caller)
{
     return resolveMemberName(self, caller != nullptr ? caller->jvm_mirror : nullptr);


//    jvm_abort("resolve");

//    // todo
//    // private Class<?> clazz;       // class in which the method is defined
//    // private String   name;        // may be null if not yet materialized
//    // private Object   type;        // may be null if not yet materialized
//    // private int      flags;       // modifier bits; see reflect.Modifier
//    // private Object   resolution;  // if null, this guy is resolved
//    auto clazz = self->getRefField<ClassObject>("clazz", "Ljava/lang/Class;")->jvm_mirror;
//    auto name = self->getRefField("name", "Ljava/lang/String;");
//    // type maybe a String or an Object[] or a MethodType
//    // Object[]: (Class<?>) Object[0] is return type
//    //           (Class<?>[]) Object[1] is parameter types
//    auto type = self->getRefField("type", "Ljava/lang/Object;");
//    jint flags = self->getIntField("flags", "I");
//    auto resolution = self->getRefField("resolution", "Ljava/lang/Object;");
//
//    Method *m = self->clazz->lookupInstMethod("getSignature", "()Ljava/lang/String;");
//    jobject sig = RSLOT(execJavaFunc(m, {self}));
//
//    auto refKind = getRefKind(self);
//    switch (flags & ALL_KINDS) {
//        case IS_METHOD: {
//            Object *descriptor = nullptr;
//
//            // TODO "java/lang/invoke/MethodHandle" 及其子类暂时特殊处理，因为取到的type一直是错的，我也不知道为什么？？？？
//            if (equals(clazz->class_name, "java/lang/invoke/MethodHandle") &&
//                    (equals(name->toUtf8(), "invoke")
//                    || equals(name->toUtf8(), "invokeBasic")
//                    || equals(name->toUtf8(), "invokeExact")
//                    || equals(name->toUtf8(), "invokeWithArguments")
//                    || equals(name->toUtf8(), "linkToSpecial")
//                    || equals(name->toUtf8(), "linkToStatic")
//                    || equals(name->toUtf8(), "linkToVirtual")
//                    || equals(name->toUtf8(), "linkToInterface"))) {
//                descriptor = newString("([Ljava/lang/Object;)Ljava/lang/Object;");
//            } else if (equals(clazz->class_name, "java/lang/invoke/BoundMethodHandle$Species_L") && equals(name->toUtf8(), "make")) {
//                descriptor = newString("(Ljava/lang/invoke/MethodType;Ljava/lang/invoke/LambdaForm;Ljava/lang/Object;)Ljava/lang/invoke/BoundMethodHandle;");
//            } else if (equals(type->clazz->class_name, S(java_lang_String))) {
//                descriptor = type;
//            } else if (equals(type->clazz->class_name, "java/lang/invoke/MethodType")) {
//                descriptor = toMethodDescriptor(type);
//            } else if (type->isArrayObject()) {
//                auto arr = (Array *) (type);
//
//                auto rtype = arr->get<ClassObject *>(0);
//                auto ptypes = arr->get<Array *>(1);
//                descriptor = toMethodDescriptor(findMethodType(rtype, ptypes));
//            } else {
//                jvm_abort("never go here.");
//            }
//            assert(descriptor != nullptr);
//
//            // printf("resolve: self(%p), %s, %s, %s\n", self, name->toUtf8(), descriptor->toUtf8(), sig->toUtf8()); /////////////////////////////////
//
//            m = clazz->lookupMethod(name->toUtf8(), descriptor->toUtf8());
//            if (m == nullptr) {
//                int xxxx = 2;
//            }
//            flags |= __method_flags(m);
//            self->setIntField("flags", "I", flags);
//            // if (refKind == JVM_REF_invokeStatic) {
//            //     m = clazz->getDeclaredStaticMethod(name->toUtf8(), descriptor->toUtf8());
//            //     flags |= __method_flags(m);
//            //     self->setIntField("flags", "I", flags);
//            // } else {
//            //     jvm_abort("not support!");
//            // }
//
//            return self;
//        }
//        case IS_CONSTRUCTOR: {
//            jvm_abort("IS_CONSTRUCTOR.");
//            break;
//        }
//        case IS_FIELD: {
//            #if 0
//                        FieldBlock *fb;
//
//            fb = lookupField(clazz, name_sym, type_sym);
//            if(fb == NULL)
//                goto throw_excep;
//
//            flags |= fb->access_flags;
//            INST_DATA(mname, int, mem_name_flags_offset) = flags;
//            INST_DATA(mname, FieldBlock*, mem_name_vmtarget_offset) = fb;
//            break;
//            #endif
//            Field *f = clazz->lookupField(name->toUtf8(), sig->toUtf8());
//            flags |= f->access_flags;
//            self->setIntField("flags", "I", flags);
//            return self;
//        }
//        default:
//            jvm_abort("never go here.");
//    }
//
//    jvm_abort("not support!");
}

// static native int getMembers(Class<?> defc, String matchName, String matchSig,
//                              int matchFlags, Class<?> caller, int skip, MemberName[] results);
static jint getMembers(jclass defc, jstring match_name, jstring match_sig, 
                        jint match_flags, jclass caller, jint skip, jobjectArray results)
{
    int search_super = (match_flags & SEARCH_SUPERCLASSES) != 0;
    int search_intf = (match_flags & SEARCH_INTERFACES) != 0;
    int local = !(search_super || search_intf);
    char *name_sym = nullptr, *sig_sym = nullptr;

    if (match_name != nullptr) {
        auto x = match_name->toUtf8();
        jvm_abort("unimplemented");
    }

    if (match_sig != nullptr) {
        auto x = match_sig->toUtf8();
        jvm_abort("unimplemented");
    }

    if(match_flags & IS_FIELD)
        jvm_abort("unimplemented");

    if(!local)
        jvm_abort("unimplemented");

    if(match_flags & (IS_METHOD | IS_CONSTRUCTOR)) {
        int count = 0;

        for (Method *m : defc->jvm_mirror->methods) {
            if(m->name == SYMBOL(class_init))
                continue;
            if(m->name == SYMBOL(object_init))
                continue;
            if(skip-- > 0)
                continue;

            if(count < results->len) {
                Object *member_name = results->get<jobject>(count);
                count++;
                int flags = __method_flags(m) | IS_METHOD;

                flags |= (m->isStatic() ? JVM_REF_invokeStatic : JVM_REF_invokeVirtual) << REFERENCE_KIND_SHIFT;

                member_name->setIntField("flags", "I", flags);
                member_name->setRefField("clazz", "Ljava/lang/Class;", m->clazz->java_mirror);
                member_name->setRefField("name", "Ljava/lang/String;", g_string_class->intern(m->name));
                member_name->setRefField("type", "Ljava/lang/Object;", newString(m->descriptor));
                // INST_DATA(mname, int, mem_name_flags_offset) = flags;
                // INST_DATA(mname, Class*, mem_name_clazz_offset) = mb->class;
                // INST_DATA(mname, Object*, mem_name_name_offset) =
                //                 findInternedString(createString(mb->name));
                // INST_DATA(mname, Object*, mem_name_type_offset) =
                //                 createString(mb->type);
                // INST_DATA(mname, MethodBlock*, mem_name_vmtarget_offset) = mb;
            }
        }

        return count;
    }


    jvm_abort("unimplemented");
}

// static native long objectFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong objectFieldOffset(jobject self)
{
    // private Class<?> clazz;       // class in which the method is defined
    // private String   name;        // may be null if not yet materialized
    // private Object   type;        // may be null if not yet materialized
    auto clazz = self->getRefField<ClassObject>("clazz", "Ljava/lang/Class;")->jvm_mirror;
    auto name = self->getRefField("name", "Ljava/lang/String;");
    // type maybe a String or an Object[] or a MethodType
    // Object[]: (Class<?>) Object[0] is return type
    //           (Class<?>[]) Object[1] is parameter types
    auto type = self->getRefField("type", "Ljava/lang/Object;");

    Method *m = self->clazz->lookupInstMethod("getSignature", "()Ljava/lang/String;");
    jobject sig = RSLOT(execJavaFunc(m, {self}));

    Field *f = clazz->lookupField(name->toUtf8(), sig->toUtf8());
    return f->id;
}

// static native long staticFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong staticFieldOffset(jobject self)
{
    jvm_abort("staticFieldOffset");
}

// static native Object staticFieldBase(MemberName self);  // e.g., returns clazz
static jobject staticFieldBase(jobject self)
{
    jvm_abort("staticFieldBase");
}

// static native Object getMemberVMInfo(MemberName self);  // returns {vmindex,vmtarget}
static jobject getMemberVMInfo(jobject self)
{
    jvm_abort("getMemberVMInfo");
}

// static native void setCallSiteTargetNormal(CallSite site, MethodHandle target);
static void setCallSiteTargetNormal(jobject site, jobject target)
{
    jvm_abort("setCallSiteTargetNormal");
}

// static native void setCallSiteTargetVolatile(CallSite site, MethodHandle target);
static void setCallSiteTargetVolatile(jobject site, jobject target)
{
    jvm_abort("setCallSiteTargetVolatile");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,

    // MemberName support

    {"init", "(Ljava/lang/invoke/MemberName;Ljava/lang/Object;)V", (void *)init},
    {"expand", "(Ljava/lang/invoke/MemberName;)V", (void *)expand},
    {"resolve", "(Ljava/lang/invoke/MemberName;" CLS ")Ljava/lang/invoke/MemberName;", (void *)resolve},
    {"getMembers", _CLS STR STR "I" CLS "I[Ljava/lang/invoke/MemberName;)I", (void *)getMembers},

    // Field layout queries parallel to sun.misc.Unsafe:

    {"objectFieldOffset", "(Ljava/lang/invoke/MemberName;)J", (void *)objectFieldOffset},
    {"staticFieldOffset", "(Ljava/lang/invoke/MemberName;)J", (void *)staticFieldOffset},
    {"staticFieldBase", "(Ljava/lang/invoke/MemberName;)" OBJ, (void *)staticFieldBase},
    {"getMemberVMInfo", "(Ljava/lang/invoke/MemberName;)" OBJ, (void *)getMemberVMInfo},

    // MethodHandle support
    {"getConstant", "(I)I", (void *)getConstant},

    // CallSite support
    /* Tell the JVM that we need to change the target of a CallSite. */
    {"setCallSiteTargetNormal", "(Ljava/lang/invoke/CallSite;Ljava/lang/invoke/MethodHandle)V", (void *)setCallSiteTargetNormal},
    {"setCallSiteTargetVolatile", "(Ljava/lang/invoke/CallSite;Ljava/lang/invoke/MethodHandle)V", (void *)setCallSiteTargetVolatile},
};

void java_lang_invoke_MethodHandleNatives_registerNatives()
{
    registerNatives("java/lang/invoke/MethodHandleNatives", methods, ARRAY_LENGTH(methods));
}
