/*
 * Author: kayo
 */

#include "ClassLoader.h"
#include "bootstrap_class_loader.h"
#include "../symbol.h"
#include "../rtda/ma/Class.h"

ClassLoader *bootClassLoader;

Class *java_lang_Object;
Class *java_lang_Class = nullptr;
Class *java_lang_String;
Class *java_lang_Cloneable;
Class *java_lang_Thread;
Class *java_lang_ThreadGroup;

Class *java_io_Serializable;

ArrayClass *char_array_class;
ArrayClass *java_lang_Class_array_class;
ArrayClass *java_lang_Object_array_class;

void initBootClassLoader()
{
    auto bcl = bootClassLoader = new ClassLoader();

    java_lang_Object = bcl->loadClass(S(java_lang_Object));
    java_lang_Class = bcl->loadClass(S(java_lang_Class));

    java_lang_Object->postInit();
    java_lang_Class->postInit();

    java_lang_String = bcl->loadClass(S(java_lang_String));
    java_lang_Cloneable = bcl->loadClass(S(java_lang_Cloneable));
    java_lang_Thread = bcl->loadClass(S(java_lang_Thread));
    java_lang_ThreadGroup = bcl->loadClass(S(java_lang_ThreadGroup));

    java_io_Serializable = bcl->loadClass(S(java_io_Serializable));

    char_array_class = (ArrayClass *) bcl->loadClass(S(array_C));
    java_lang_Class_array_class = (ArrayClass *) bcl->loadClass(S(array_java_lang_Class));
    java_lang_Object_array_class = (ArrayClass *) bcl->loadClass(S(array_java_lang_Object));

    // 加载基本类型（int, float, etc.）的 class
    loadPrimitiveTypes();
}
