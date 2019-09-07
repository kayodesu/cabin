/*
 * Author: kayo
 */

#include "ClassLoader.h"
#include "bootstrap_class_loader.h"
#include "../symbol.h"
#include "../rtda/heap/ClassObject.h"

ClassLoader *bootClassLoader;

Class *java_lang_Object_class;
Class *java_lang_Class_class = nullptr;
Class *java_lang_String_class;
Class *java_lang_Cloneable_class;
Class *java_lang_Thread_class;
Class *java_lang_ThreadGroup_class;

Class *java_io_Serializable_class;

ArrayClass *char_array_class;
ArrayClass *java_lang_Class_array_class;
ArrayClass *java_lang_Object_array_class;

void initBootClassLoader()
{
    bootClassLoader = new ClassLoader();

    java_lang_Object_class = bootClassLoader->loadClass(S(java_lang_Object));
    java_lang_Class_class = bootClassLoader->loadClass(S(java_lang_Class));

    java_lang_Object_class->clsobj = ClassObject::newInst(java_lang_Object_class);
    java_lang_Class_class->clsobj = ClassObject::newInst(java_lang_Class_class);

    java_lang_String_class = bootClassLoader->loadClass(S(java_lang_String));
    java_lang_Cloneable_class = bootClassLoader->loadClass(S(java_lang_Cloneable));
    java_lang_Thread_class = bootClassLoader->loadClass(S(java_lang_Thread));
    java_lang_ThreadGroup_class = bootClassLoader->loadClass(S(java_lang_ThreadGroup));

    java_io_Serializable_class = bootClassLoader->loadClass(S(java_io_Serializable));

    char_array_class = (ArrayClass *) bootClassLoader->loadClass(S(array_C));
    java_lang_Class_array_class = (ArrayClass *) bootClassLoader->loadClass(S(array_java_lang_Class));
    java_lang_Object_array_class = (ArrayClass *) bootClassLoader->loadClass(S(array_java_lang_Object));

    // 加载基本类型（int, float, etc.）的 class
    loadPrimitiveTypes();
}
