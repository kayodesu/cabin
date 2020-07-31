/*
 * Author: Yo Ka
 */

#include <sstream>
#include "descriptor.h"
#include "../metadata/class.h"
#include "../objects/class_loader.h"
#include "../objects/prims.h"
#include "../objects/array_object.h"
#include "../runtime/vm_thread.h"

using namespace std;

// @b: include
// @e：exclude
// eg. Ljava/lang/String;
static ClassObject *convertDescElement2ClassObject(char *&b, char *e, jref loader)
{
    assert(b != nullptr && e != nullptr);

    if (*b == 'L') { // reference
        char *t = strchr(b, ';');
        if (t == nullptr || t >= e) {
            goto error;
        }

//        *t = 0;   // end string
        b++; // jump 'L'
//        string s(b, t-b);
        auto c = loadClass(loader, string(b, t - b).c_str());
//        *t = ';'; // recover
        b = t + 1;
        return c->java_mirror;
    }

    if (*b == '[') { // array reference, 描述符形如 [B 或 [[Ljava/lang/String; 的形式
        char *t = b;
        while (*(++t) == '[');
        if (!isPrimDescriptor(*t)) {
            t = strchr(t, ';');
            if (t == nullptr || t >= e) {
                goto error;
            }
        }

//        char k = *(++t);
//        *t = 0; // end string
        t++;
        auto c = loadArrayClass(loader, string(b, t - b).c_str());
//        *t = k; // recover
        b = t;
        return c->java_mirror;
    }

    if (isPrimDescriptor(*b)) { // prim type
        const char *class_name = getPrimClassName(*b);
        b++;
        return loadBootClass(class_name)->java_mirror;
    }

error:
    signalException(S(java_lang_UnknownError), nullptr); // todo
    return nullptr;
}

int numElementsInDescriptor(const char *b, const char *e)
{
    assert(b != nullptr && e != nullptr);

    int no_params;
    b--;
    for(no_params = 0; ++b < e; no_params++) {
        if(*b == '[')
            while(*++b == '[');
        if(*b == 'L')
            while(*++b != ';');
    }

    return no_params;
}

//int numElementsInDescriptor(const char *descriptor)
//{
//    assert(descriptor != nullptr);
//    return numElementsInDescriptor(descriptor, descriptor + strlen(descriptor));
//}

// @b: include
// @e：exclude
// eg. I[BLjava/lang/String;ZZ
static Array *convertDesc2ClassObjectArray(char *b, char *e, jref loader)
{
    int num = numElementsInDescriptor(b, e);
    Array *types = newArray(loadArrayClass(S(array_java_lang_Class)), num);

    for (int i = 0; b < e; i++) {
        ClassObject *co = convertDescElement2ClassObject(b, e, loader);
        if (exceptionOccured())
            return nullptr;

        assert(i < num);
        types->setRef(i, co);
    }

    return types;
}

pair<Array *, ClassObject *> parseMethodDescriptor(const char *desc, jref loader)
{
    assert(desc != nullptr);

    char *e = strchr(desc, ')');
    if (e == nullptr || *desc != '(') {
        signalException(S(java_lang_UnknownError), nullptr); // todo
        return make_pair(nullptr, nullptr);
    }

    Array *ptypes = convertDesc2ClassObjectArray((char *) (desc + 1), e, loader);
    e++; // jump ')'
    ClassObject *rtype = convertDescElement2ClassObject(e, e + strlen(e), loader);
    return make_pair(ptypes, rtype);
}

static string convertTypeToDesc(Class *type)
{
    assert(type != nullptr);

    if (type->isPrimClass()) {
        return getPrimDescriptorByClassName(type->class_name);
    }

    if (type->isArrayClass()) {
        return type->class_name;
    }

    // 普通类
    ostringstream oss;
    oss << 'L';
    oss << type->class_name;
    oss << ';';
    return oss.str();
}

string unparseMethodDescriptor(Array *ptypes /*Class *[]*/, ClassObject *rtype)
{
    ostringstream oss;

    if (ptypes == nullptr) { // no argument
        oss << "()";
    } else {
        oss << "(";
        for (int i = 0; i < ptypes->len; i++) {
            ClassObject *co = ptypes->get<ClassObject *>(i);
            assert(co != nullptr);
            oss << convertTypeToDesc(co->jvm_mirror);        
        }
        oss << ")";
    }

    if (rtype == nullptr) { // no return value
        oss << "V";
    } else {
        oss << convertTypeToDesc(rtype->jvm_mirror);
    }

    return oss.str();
}

string unparseMethodDescriptor(jref method_type)
{
    assert(method_type != nullptr);

    // private final Class<?>[] ptypes;
    auto ptypes = method_type->getRefField<Array>("ptypes", S(array_java_lang_Class));
    // private final Class<?> rtype;
    auto rtype = method_type->getRefField<ClassObject>("rtype", S(sig_java_lang_Class));

    return unparseMethodDescriptor(ptypes, rtype);
}