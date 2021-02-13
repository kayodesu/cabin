#include "exception.h"
#include "objects/class_loader.h"
#include "objects/array.h"
#include "interpreter/interpreter.h"

Object *JavaException::getExcep()
{
    // if(VM_initing) {
    //     fprintf(stderr, "Exception occurred while VM initialising.\n");
    //     if(message)
    //         fprintf(stderr, "%s: %s\n", excep_name, message);
    //     else
    //         fprintf(stderr, "%s\n", excep_name);
    //     exit(1);
    // }

    if (excep == nullptr) {

        Class *ec = loadBootClass(excep_class_name);
        assert(ec != nullptr); // todo

        initClass(ec);
        excep = ec->allocObject();
        if (msg.empty()) {
            execJavaFunc(ec->getConstructor(S(___V)), { excep });
        } else {
            execJavaFunc(ec->getConstructor(S(_java_lang_String__V)), { excep, newString(msg.c_str()) });
        }
    }

    return excep;
}

void printStackTrace(Object *e)
{
    assert(e != nullptr);
    assert(e->clazz->isSubclassOf(loadBootClass("java/lang/Throwable")));

    // private String detailMessage;
    jstrref msg = e->getRefField("detailMessage", S(sig_java_lang_String));
    printf("%s: %s\n", e->clazz->class_name, msg != nullptr ? msg->toUtf8() : "null");

    // [Ljava/lang/Object;
    auto backtrace = e->getRefField<Array>("backtrace", "Ljava/lang/Object;");
    for (int i = 0; i < backtrace->arr_len; i++) {
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