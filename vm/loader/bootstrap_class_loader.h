/*
 * Author: kayo
 */

#ifndef KAYOVM_BOOTSTRAP_CLASS_LOADER_H
#define KAYOVM_BOOTSTRAP_CLASS_LOADER_H

class Class;
class ArrayClass;
class ClassLoader;

extern ClassLoader *bootClassLoader;

// Cache 常用的类
extern Class *java_lang_Object_class;
extern Class *java_lang_Class_class;
extern Class *java_lang_String_class;
extern Class *java_lang_Cloneable_class;
extern Class *java_lang_Thread_class;
extern Class *java_lang_ThreadGroup_class;

extern Class *java_io_Serializable_class;

extern ArrayClass *char_array_class; // [C
extern ArrayClass *java_lang_Class_array_class; // [Ljava/lang/Class;
extern ArrayClass *java_lang_Object_array_class; // [Ljava/lang/Object;

void initBootClassLoader();

#endif //KAYOVM_BOOTSTRAP_CLASS_LOADER_H
