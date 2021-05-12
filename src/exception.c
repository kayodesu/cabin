#include <stdlib.h>
#include "exception.h"
#include "interpreter/interpreter.h"
#include "metadata/class.h"
#include "objects/class_loader.h"
#include "objects/object.h"
#include "runtime/vm_thread.h"

// Object *JavaException::getExcep()
// {
//     // if(VM_initing) {
//     //     fprintf(stderr, "Exception occurred while VM initialising.\n");
//     //     if(message)
//     //         fprintf(stderr, "%s: %s\n", excep_name, message);
//     //     else
//     //         fprintf(stderr, "%s\n", excep_name);
//     //     exit(1);
//     // }

//     if (excep == NULL) {

//         Class *ec = loadBootClass(excep_class_name);
//         assert(ec != NULL); // todo

//         initClass(ec);
//         excep = alloc_object(ec);
//         if (msg.empty()) {
//             exec_java_func1(get_constructor(ec, S(___V)), { excep });
//         } else {
//             exec_java_func1(get_constructor(ec, S(_java_lang_String__V)), { excep, alloc_string(msg.c_str()) });
//         }
//     }

//     return excep;
// }

extern bool g_vm_initing;

void raise_exception(const char *exception_class_name, const char *msg)
{
    if (g_vm_initing) {
        fprintf(stderr, "Exception occurred while VM initialising.\n");
        if(msg != NULL)
            fprintf(stderr, "%s: %s\n", exception_class_name, msg);
        else
            fprintf(stderr, "%s\n", msg);
        exit(1); // todo
    }
    Thread *thread = getCurrentThread();
    if (thread->exception != NULL) {
        Class *ec = loadClass(g_app_class_loader, exception_class_name);
        assert(ec != NULL); // todo
        
        initClass(ec);
        thread->exception = alloc_object(ec);
        if (msg == NULL) {
            exec_java_func1(get_constructor(ec, S(___V)), thread->exception);
        } else {
            exec_java_func2(get_constructor(ec, S(_java_lang_String__V)), thread->exception, alloc_string(msg));
        }

        longjmp(thread->jmpbuf, LONG_JMP_JAVA_EXCEP_VALUE);
    }
}

void printStackTrace(Object *e)
{
    assert(e != NULL);
    assert(is_subclass_of(e->clazz, load_boot_class("java/lang/Throwable")));

    // private String detailMessage;
    jstrref msg = get_ref_field(e, "detailMessage", S(sig_java_lang_String));
    printf("%s: %s\n", e->clazz->class_name, msg != NULL ? jstring_to_utf8(msg) : "null");

    // [Ljava/lang/Object;
    jobject backtrace = get_ref_field(e, "backtrace", "Ljava/lang/Object;");
    for (int i = 0; i < backtrace->arr_len; i++) {
        jref element = jarray_get(jref, backtrace, i); // java.lang.StackTraceElement

        // private String declaringClass;
        // private String methodName;
        // private String fileName;
        // private int    lineNumber;
        jstrref declaring_class = get_ref_field(element, "declaringClass", S(sig_java_lang_String));
        jstrref method_name = get_ref_field(element, "methodName", S(sig_java_lang_String));
        jstrref file_name = get_ref_field(element, "fileName", S(sig_java_lang_String));
        jint line_number = get_int_field(element, "lineNumber");

        printf("\tat %s.%s(%s:%d)\n",
               jstring_to_utf8(declaring_class),
               jstring_to_utf8(method_name),
               file_name ? jstring_to_utf8(file_name) : "(Unknown Source)",
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