#include <assert.h>
#include <stdlib.h>
#include "cabin.h"

extern bool g_vm_initing;

void raise_exception(const char *exception_class_name, const char *msg)
{
    if (g_vm_initing) {
        fprintf(stderr, "Exception occurred while VM initialising.\n");
        if(msg != NULL)
            fprintf(stderr, "%s: %s\n", exception_class_name, msg);
        else
            fprintf(stderr, "%s\n", exception_class_name);
        exit(1); // todo
    }
    Thread *thread = get_current_thread();
    if (thread->exception == NULL) {
        Class *ec = load_class(g_app_class_loader, exception_class_name);
        assert(ec != NULL); // todo

        init_class(ec);
        thread->exception = alloc_object(ec);
        if (msg == NULL) {
            exec_java(get_constructor(ec, S(___V)), (slot_t[]) { rslot(thread->exception) });
        } else {
            exec_java(get_constructor(ec, S(_java_lang_String__V)), 
                        (slot_t[]) { rslot(thread->exception), rslot(alloc_string(msg)) });
        }
    }

    assert(thread->exception != NULL);
}

void set_exception(jref e)
{
    get_current_thread()->exception = e;
}

Object *exception_occurred()
{
    Thread *thread = get_current_thread();
    jref e = thread->exception;
    return e;
}

void clear_exception()
{
    get_current_thread()->exception = NULL;
}

void print_stack_trace(Object *e)
{
    assert(e != NULL);
    assert(is_subclass_of(e->clazz, load_boot_class("java/lang/Throwable")));

    // private String detailMessage;
    jstrRef msg = get_ref_field(e, "detailMessage", S(sig_java_lang_String));
    printf("%s: %s\n", e->clazz->class_name, msg != NULL ? string_to_utf8(msg) : "null");

    // [Ljava/lang/Object;
    jref backtrace = get_ref_field(e, "backtrace", "Ljava/lang/Object;");
    assert(backtrace != NULL);
    for (int i = 0; i < backtrace->arr_len; i++) {
        jref element = array_get(jref, backtrace, i); // java.lang.StackTraceElement

        // private String declaringClass;
        // private String methodName;
        // private String fileName;
        // private int    lineNumber;
        jstrRef declaring_class = get_ref_field(element, "declaringClass", S(sig_java_lang_String));
        jstrRef method_name = get_ref_field(element, "methodName", S(sig_java_lang_String));
        jstrRef file_name = get_ref_field(element, "fileName", S(sig_java_lang_String));
        jint line_number = get_int_field(element, "lineNumber");

        printf("\tat %s.%s(%s:%d)\n",
               string_to_utf8(declaring_class),
               string_to_utf8(method_name),
               file_name ? string_to_utf8(file_name) : "(Unknown Source)",
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