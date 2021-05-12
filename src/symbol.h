#ifndef CABIN_SYMBOL_H
#define CABIN_SYMBOL_H

extern const char *symbol_values[];
#define SYMBOL_NAME_ENUM(name) symbol_##name
#define SYMBOL(name) symbol_values[SYMBOL_NAME_ENUM(name)]

#define S SYMBOL

#define SYMBOL_PAIRS(action) \
    /* Method and field names, etc. */\
    action(f, "f"), \
    action(m, "m"), \
    action(B, "B"), \
    action(C, "C"), \
    action(S, "S"), \
    action(I, "I"), \
    action(F, "F"), \
    action(D, "D"), \
    action(J, "J"), \
    action(Z, "Z"), \
    action(pd, "pd"), \
    action(put, "put"), \
    action(cap, "cap"), \
    action(run, "run"), \
    action(cons, "cons"), \
    action(main, "main"), \
    action(data, "data"), \
    action(name, "name"), \
    action(root, "root"), \
    action(exit, "exit"), \
    action(slot, "slot"), \
    action(type, "type"), \
    action(flag, "flag"), \
    action(flags, "flags"), \
    action(invoke, "invoke"), \
    action(invokeExact, "invokeExact"), \
    action(invokeBasic, "invokeBasic"), \
    action(linkToVirtual, "linkToVirtual"), \
    action(linkToStatic, "linkToStatic"), \
    action(linkToSpecial, "linkToSpecial"), \
    action(linkToInterface, "linkToInterface"), \
    action(coder, "coder"), \
    action(clazz, "clazz"), \
    action(queue, "queue"), \
    action(group, "group"), \
    action(count, "count"), \
    action(value, "value"), \
    action(create, "create"), \
    action(daemon, "daemon"), \
    action(thread, "thread"),\
    action(form, "form"), \
    action(vmData, "vmData"), \
    action(vmdata, "vmdata"), \
    action(vmtarget, "vmtarget"), \
    action(vmentry, "vmentry"), \
    action(offset, "offset"), \
    action(valueOf, "valueOf"), \
    action(enqueue, "enqueue"), \
    action(address, "address"), \
    action(referent, "referent"), \
    action(vmThread, "vmThread"), \
    action(priority, "priority"), \
    action(threadId, "threadId"), \
    action(finalize, "finalize"), \
    action(hashtable, "hashtable"), \
    action(backtrace, "backtrace"), \
    action(initCause, "initCause"), \
    action(loadClass, "loadClass"), \
    action(addThread, "addThread"), \
    action(returnType, "returnType"), \
    action(removeThread, "removeThread"), \
    action(declaringClass, "declaringClass"), \
    action(parameterTypes, "parameterTypes"), \
    action(printStackTrace, "print_stack_trace"), \
    action(fillInStackTrace, "fillInStackTrace"), \
    action(exceptionHandler, "exceptionHandler"), \
    action(createBootPackage, "createBootPackage"), \
    action(uncaughtException, "uncaughtException"), \
    action(newLibraryUnloader, "newLibraryUnloader"), \
    action(contextClassLoader, "contextClassLoader"), \
    action(getSystemClassLoader, "getSystemClassLoader"), \
    action(getPlatformClassLoader, "getPlatformClassLoader"), \
    action(registerNatives, "register_natives"), \
    \
    /* Constant pool attribute names */\
    action(Code, "Code"), \
    action(ConstantValue, "ConstantValue"), \
    action(Signature, "Signature"), \
    action(Synthetic, "Synthetic"), \
    action(Deprecated, "Deprecated"), \
    action(LineNumberTable, "LineNumberTable"), \
    action(StackMapTable, "StackMapTable"), \
    action(LocalVariableTable, "LocalVariableTable"), \
    action(LocalVariableTypeTable, "LocalVariableTypeTable"), \
    action(Exceptions, "Exceptions"), \
    action(AnnotationDefault, "AnnotationDefault"), \
    action(SourceFile, "SourceFile"), \
    action(Module, "Module"), \
    action(ModulePackages, "ModulePackages"), \
    action(ModuleHashes, "ModuleHashes"), \
    action(ModuleTarget, "ModuleTarget"), \
    action(ModuleMainClass, "ModuleMainClass"), \
    action(NestHost, "NestHost"), \
    action(NestMembers, "NestMembers"), \
    action(InnerClasses, "InnerClasses"), \
    action(EnclosingMethod, "EnclosingMethod"), \
    action(SourceDebugExtension, "SourceDebugExtension"), \
    action(BootstrapMethods, "BootstrapMethods"), \
    action(MethodParameters, "MethodParameters"), \
    action(RuntimeVisibleAnnotations, "RuntimeVisibleAnnotations"), \
    action(RuntimeInvisibleAnnotations, "RuntimeInvisibleAnnotations"), \
    action(RuntimeVisibleParameterAnnotations, "RuntimeVisibleParameterAnnotations"), \
    action(RuntimeInvisibleParameterAnnotations, "RuntimeInvisibleParameterAnnotations"), \
    \
    /* Primitive type names */\
    action(int, "int"), \
    action(void, "void"), \
    action(byte, "byte"), \
    action(char, "char"), \
    action(long, "long"), \
    action(short, "short"), \
    action(float, "float"), \
    action(double, "double"), \
    action(boolean, "boolean"), \
    \
    /* Class and object initialise names */\
    action(object_init, "<init>"), \
    action(class_init, "<clinit>"), \
    \
    /* Class names */\
    action(java_lang_Void, "java/lang/Void"), \
    action(java_lang_Byte, "java/lang/Byte"), \
    action(java_lang_Long, "java/lang/Long"), \
    action(java_lang_Enum, "java/lang/Enum"), \
    action(java_lang_Short, "java/lang/Short"), \
    action(java_lang_Float, "java/lang/Float"), \
    action(java_nio_Buffer, "java/nio/Buffer"), \
    action(java_lang_Class, "java/lang/Class"), \
    action(java_lang_Number, "java/lang/Number"), \
    action(java_lang_Double, "java/lang/Double"), \
    action(java_lang_Object, "java/lang/Object"), \
    action(java_lang_String, "java/lang/String"), \
    action(java_lang_Package, "java/lang/Package"), \
    action(java_lang_Thread, "java/lang/Thread"), \
    action(java_lang_Runtime, "java/lang/Runtime"), \
    action(java_lang_System, "java/lang/System"), \
    action(java_lang_Boolean, "java/lang/Boolean"), \
    action(java_lang_Integer, "java/lang/Integer"), \
    action(java_util_HashMap, "java/util/HashMap"), \
    action(java_lang_VMThread, "java/lang/VMThread"), \
    action(java_lang_Character, "java/lang/Character"), \
    action(java_lang_VMRuntime, "java/lang/VMRuntime"), \
    action(java_lang_Throwable, "java/lang/Throwable"), \
    action(java_lang_Cloneable, "java/lang/Cloneable"), \
    action(java_io_Serializable, "java/io/Serializable"), \
    action(java_lang_VMThrowable, "java/lang/VMThrowable"), \
    action(java_lang_ThreadGroup, "java/lang/ThreadGroup"), \
    action(java_lang_ClassLoader, "java/lang/ClassLoader"),   \
    action(java_lang_reflect_Field, "java/lang/reflect/Field"), \
    action(java_lang_invoke_MethodHandle, "java/lang/invoke/MethodHandle"), \
    action(java_lang_invoke_MemberName, "java/lang/invoke/MemberName"), \
    action(java_lang_invoke_MethodHandleNatives, "java/lang/invoke/MethodHandleNatives"), \
    action(java_lang_invoke_VarHandle, "java/lang/invoke/VarHandle"), \
    action(java_lang_reflect_Executable, "java/lang/reflect/Executable"), \
    action(java_lang_reflect_VMField, "java/lang/reflect/VMField"), \
    action(java_lang_VMClassLoader, "java/lang/VMClassLoader"), \
    action(java_lang_ref_Reference, "java/lang/ref/Reference"), \
    action(sun_reflect_annotation_AnnotationInvocationHandler, \
           "sun/reflect/annotation/AnnotationInvocationHandler"), \
    action(java_lang_reflect_Method, "java/lang/reflect/Method"), \
    action(java_lang_reflect_VMMethod, "java/lang/reflect/VMMethod"), \
    action(java_lang_StackTraceElement, "java/lang/StackTraceElement"), \
    action(java_lang_ref_SoftReference, "java/lang/ref/SoftReference"), \
    action(java_lang_ref_WeakReference, "java/lang/ref/WeakReference"), \
    action(java_lang_reflect_Constructor, "java/lang/reflect/Constructor"), \
    action(java_lang_reflect_VMConstructor, "java/lang/reflect/VMConstructor"), \
    action(java_lang_ref_PhantomReference, "java/lang/ref/PhantomReference"), \
    action(java_nio_DirectByteBufferImpl_ReadWrite, "java/nio/DirectByteBufferImpl$ReadWrite"), \
    action(java_lang_ClassLoader_NativeLibrary, "java/lang/ClassLoader$NativeLibrary"), \
    \
    /* Exception class names */\
    action(java_lang_Error, "java/lang/Error"), \
    action(java_lang_UnknownError, "java/lang/UnknownError"), \
    action(java_lang_LinkageError, "java/lang/LinkageError"), \
    action(java_lang_InternalError, "java/lang/InternalError"),     \
    action(java_lang_ClassFormatError, "java/lang/ClassFormatError"),   \
    action(java_lang_VirtualMachineError, "java/lang/VirtualMachineError"), \
    action(java_lang_OutOfMemoryError, "java/lang/OutOfMemoryError"), \
    action(java_lang_NoSuchFieldError, "java/lang/NoSuchFieldError"), \
    action(java_lang_NoSuchMethodError, "java/lang/NoSuchMethodError"), \
    action(java_lang_ClassCastException, "java/lang/ClassCastException"), \
    action(java_lang_CloneNotSupportedException, "java/lang/CloneNotSupportedException"), \
    action(java_lang_StackOverflowError, "java/lang/StackOverflowError"), \
    action(java_lang_InstantiationError, "java/lang/InstantiationError"), \
    action(java_lang_IllegalAccessError, "java/lang/IllegalAccessError"), \
    action(java_lang_ArithmeticException, "java/lang/ArithmeticException"), \
    action(java_lang_AbstractMethodError, "java/lang/AbstractMethodError"), \
    action(java_lang_ArrayStoreException, "java/lang/ArrayStoreException"), \
    action(java_lang_IndexOutOfBoundsException, "java/lang/IndexOutOfBoundsException"), \
    action(java_lang_UnsatisfiedLinkError, "java/lang/UnsatisfiedLinkError"), \
    action(java_lang_InterruptedException, "java/lang/InterruptedException"), \
    action(java_lang_NullPointerException, "java/lang/NullPointerException"), \
    action(java_lang_NoClassDefFoundError, "java/lang/NoClassDefFoundError"), \
    action(java_lang_IllegalAccessException, "java/lang/IllegalAccessException"), \
    action(java_lang_ClassNotFoundException, "java/lang/ClassNotFoundException"), \
    action(java_lang_InstantiationException, "java/lang/InstantiationException"), \
    action(java_lang_IllegalArgumentException, "java/lang/IllegalArgumentException"), \
    action(java_lang_NegativeArraySizeException, "java/lang/NegativeArraySizeException"), \
    action(java_lang_ExceptionInInitializerError, "java/lang/ExceptionInInitializerError"), \
    action(java_lang_IllegalThreadStateException, "java/lang/IllegalThreadStateException"), \
    action(java_lang_IllegalMonitorStateException, "java/lang/IllegalMonitorStateException"), \
    action(java_lang_IncompatibleClassChangeError, "java/lang/IncompatibleClassChangeError"), \
    action(java_lang_ArrayIndexOutOfBoundsException, "java/lang/ArrayIndexOutOfBoundsException"), \
    action(java_lang_StringIndexOutOfBoundsException, "java/lang/StringIndexOutOfBoundsException"), \
    action(java_io_IOException, "java/io/IOException"), \
    action(java_io_FileNotFoundException, "java/io/FileNotFoundException"), \
    \
    /* Array class names */\
    action(array_V, "[V"), \
    action(array_Z, "[Z"), \
    action(array_B, "[B"), \
    action(array_C, "[C"), \
    action(array_S, "[S"), \
    action(array_I, "[I"), \
    action(array_J, "[J"), \
    action(array_F, "[F"), \
    action(array_D, "[D"), \
    action(array_java_lang_Class, "[Ljava/lang/Class;"), \
    action(array_java_lang_Object, "[Ljava/lang/Object;"), \
    action(array_java_lang_String, "[Ljava/lang/String;"), \
    action(array_java_lang_Package, "[Ljava/lang/Package;"), \
    action(array_java_lang_Thread, "[Ljava/lang/Thread;"), \
    action(array_java_lang_reflect_Field, "[Ljava/lang/reflect/Field;"), \
    action(array_java_lang_reflect_Method, "[Ljava/lang/reflect/Method;"), \
    action(array_java_lang_StackTraceElement, "[Ljava/lang/StackTraceElement;"), \
    action(array_java_lang_reflect_Constructor, "[Ljava/lang/reflect/Constructor;"), \
    \
    /* Field signatures */\
    action(sig_java_lang_Class, "Ljava/lang/Class;"), \
    action(sig_java_lang_Object, "Ljava/lang/Object;"), \
    action(sig_java_lang_String, "Ljava/lang/String;"), \
    action(sig_java_lang_Thread, "Ljava/lang/Thread;"), \
    action(sig_java_lang_VMThread, "Ljava/lang/VMThread;"), \
    action(sig_java_lang_ThreadGroup, "Ljava/lang/ThreadGroup;"), \
    action(sig_java_lang_ClassLoader, "Ljava/lang/ClassLoader;"), \
    action(sig_java_lang_invoke_MethodType, "Ljava/lang/invoke/MethodType;"), \
    action(sig_java_lang_reflect_Field, "Ljava/lang/reflect/Field;"), \
    action(sig_java_lang_reflect_VMField, "Ljava/lang/reflect/VMField;"), \
    action(sig_java_lang_reflect_Method, "Ljava/lang/reflect/Method;"), \
    action(sig_java_lang_reflect_VMMethod, "Ljava/lang/reflect/VMMethod;"), \
    action(sig_java_lang_reflect_Constructor, "Ljava/lang/reflect/Constructor;"), \
    action(sig_java_lang_reflect_VMConstructor, "Ljava/lang/reflect/VMConstructor;"), \
    action(sig_java_lang_ref_ReferenceQueue, "Ljava/lang/ref/ReferenceQueue;"), \
    action(sig_java_security_ProtectionDomain, "Ljava/security/ProtectionDomain;"), \
    action(sig_java_lang_Thread_UncaughtExceptionHandler, "Ljava/lang/Thread$UncaughtExceptionHandler;"), \
    \
    /* Method signatures */\
    action(___V, "()V"), \
    action(___Z, "()Z"), \
    action(_I__V, "(I)V"), \
    action(_J__V, "(J)V"), \
    action(_java_lang_String_I__java_lang_Package, \
           "(Ljava/lang/String;I)Ljava/lang/Package;"), \
    action(_java_lang_Thread_java_lang_Throwable__V, \
           "(Ljava/lang/Thread;Ljava/lang/Throwable;)V"), \
    action(_java_lang_VMThread_java_lang_String_I_Z__V, \
           "(Ljava/lang/VMThread;Ljava/lang/String;IZ)V"), \
    action(_java_lang_Throwable__java_lang_Throwable, \
           "(Ljava/lang/Throwable;)Ljava/lang/Throwable;"), \
    action(_java_lang_String__V, "(Ljava/lang/String;)V"), \
    action(_java_lang_Thread__V, "(Ljava/lang/Thread;)V"), \
    action(_java_lang_Throwable__V, "(Ljava/lang/Throwable;)V"), \
    action(_array_java_lang_String__V, "([Ljava/lang/String;)V"), \
    action(___java_lang_Object, "()Ljava/lang/Object;"), \
    action(___java_lang_String, "()Ljava/lang/String;"), \
    action(___java_lang_ClassLoader, "()Ljava/lang/ClassLoader;"), \
    action(_java_lang_Object_java_lang_Object__java_lang_Object, \
           "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"), \
    action(_java_lang_String_I_java_lang_String_java_lang_String_Z__V, \
           "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Z)V"), \
    action(_java_lang_String__java_lang_Class, "(Ljava/lang/String;)Ljava/lang/Class;"), \
    \
    /* Native method simple signatures */\
    action(V_V, "V_V"), \
    action(V_Z, "V_Z"), \
    action(V_I, "V_I"), \
    action(V_R, "V_R"), \
    action(V_J, "V_J"), \
    \
    action(Z_R, "Z_R"), \
    action(I_V, "I_V"), \
    action(I_I, "I_I"), \
    action(I_Z, "I_Z"), \
    action(I_J, "I_J"), \
    action(F_I, "F_I"), \
    action(D_J, "D_J"), \
    action(J_V, "J_V"), \
    action(J_D, "J_D"), \
    action(J_J, "J_J"), \
    action(J_B, "J_B"), \
    action(R_V, "R_V"), \
    action(R_R, "R_R"), \
    action(R_Z, "R_Z"), \
    action(R_I, "R_I"), \
    action(R_J, "R_J"), \
    action(I_F, "I_F"), \
    \
    action(IJ_V, "IJ_V"), \
    action(IJ_J, "IJ_J"), \
    action(JJ_V, "JJ_V"), \
    action(IR_I, "IR_I"), \
    action(RZ_V, "RZ_V"), \
    action(RI_V, "RI_V"), \
    action(RJ_V, "RJ_V"), \
    action(RJ_B, "RJ_B"), \
    action(RJ_I, "RJ_I"), \
    action(RJ_R, "RJ_R"), \
    action(RJ_J, "RJ_J"), \
    action(RR_V, "RR_V"), \
    action(RI_R, "RI_R"), \
    action(RR_Z, "RR_Z"), \
    action(RI_J, "RI_J"), \
    action(RZ_Z, "RZ_Z"), \
    action(RZ_R, "RZ_R"), \
    action(RR_I, "RR_I"), \
    action(RR_J, "RR_J"), \
    action(RR_R, "RR_R"), \
    \
    action(RIR_V, "RIR_V"), \
    action(RJJ_V, "RJJ_V"), \
    action(RJB_V, "RJB_V"), \
    action(RRZ_V, "RRZ_V"), \
    action(RZJ_V, "RZJ_V"), \
    action(RRR_V, "RRR_V"), \
    action(RIZ_V, "RIZ_V"), \
    action(RRI_I, "RRI_I"), \
    action(RRZ_R, "RRZ_R"), \
    action(RRR_R, "RRR_R"), \
    action(RRR_Z, "RRR_Z"), \
    action(RRI_J, "RRI_J"), \
    action(RRR_J, "RRR_J"), \
    action(RJJ_J, "RJJ_J"), \
    action(RRJ_I, "RRJ_I"), \
    action(RRI_Z, "RRI_Z"), \
    action(RRJ_Z, "RRJ_Z"), \
    action(RRJ_B, "RRJ_B"), \
    action(RRJ_S, "RRJ_S"), \
    action(RRJ_C, "RRJ_C"), \
    action(RRJ_J, "RRJ_J"), \
    action(RRJ_F, "RRJ_F"), \
    action(RRJ_D, "RRJ_D"), \
    action(RRJ_R, "RRJ_R"), \
    \
    action(RIIZ_V, "RIIZ_V"), \
    action(RRJB_V, "RRJB_V"), \
    action(RRJR_V, "RRJR_V"), \
    action(RRJC_V, "RRJC_V"), \
    action(RRJI_V, "RRJI_V"), \
    action(RRJZ_V, "RRJZ_V"), \
    action(RRJS_V, "RRJS_V"), \
    action(RRJJ_V, "RRJJ_V"), \
    action(RRJF_V, "RRJF_V"), \
    action(RRJD_V, "RRJD_V"), \
    action(RZZJ_V, "RZZJ_V"), \
    action(RRIZ_R, "RRIZ_R"), \
    action(RZRR_R, "RZRR_R"), \
    action(RJRR_R, "RJRR_R"), \
    action(RRII_I, "RRII_I"), \
    action(RJII_Z, "RJII_Z"), \
    action(RJJJ_Z, "RJJJ_Z"), \
    action(RRZZ_Z, "RRZZ_Z"), \
    action(RRRR_R, "RRRR_R"), \
    \
    action(RIIIZ_V, "RIIIZ_V"), \
    action(RRJJB_V, "RRJJB_V"), \
    action(RIRII_V, "RIRII_V"), \
    action(RZRRR_V, "RZRRR_V"), \
    action(RRIIZ_V, "RRIIZ_V"), \
    action(RRJRR_Z, "RRJRR_Z"), \
    action(RRJRR_R, "RRJRR_R"), \
    action(RRIIJ_R, "RRIIJ_R"), \
    action(RRJII_Z, "RRJII_Z"), \
    action(RRIZZ_Z, "RRIZZ_Z"), \
    action(RRJII_I, "RRJII_I"), \
    action(RRJJJ_J, "RRJJJ_J"), \
    action(RRJJJ_Z, "RRJJJ_Z"), \
    \
    action(RRJRJJ_V, "RRJRJJ_V"), \
    action(RRBIIZ_V, "RRBIIZ_V"), \
    \
    action(RRJRJJJ_V, "RRJRJJJ_V"), \
    action(RRRIIRR_R, "RRRIIRR_R"), \
    action(RRRIRIR_I, "RRRIRIR_I"), \
    \
    action(RRIIRIZR_V, "RRIIRIZR_V"), \
    \
    action(RRRRIIRZIR_R, "RRRRIIRZIR_R")
    

#define SYMBOL_ENUM(name, value) SYMBOL_NAME_ENUM(name)

enum {
    SYMBOL_PAIRS(SYMBOL_ENUM),
    MAX_SYMBOL_ENUM
};

void init_symbol();

#endif //CABIN_SYMBOL_H
