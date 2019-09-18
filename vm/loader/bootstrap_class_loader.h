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
extern Class *java_lang_Object;
extern Class *java_lang_Class;
extern Class *java_lang_String;
extern Class *java_lang_Cloneable;
extern Class *java_lang_Thread;
extern Class *java_lang_ThreadGroup;

extern Class *java_io_Serializable;

extern ArrayClass *char_array_class; // [C
extern ArrayClass *java_lang_Class_array_class; // [Ljava/lang/Class;
extern ArrayClass *java_lang_Object_array_class; // [Ljava/lang/Object;

void initBootClassLoader();

#endif //KAYOVM_BOOTSTRAP_CLASS_LOADER_H
