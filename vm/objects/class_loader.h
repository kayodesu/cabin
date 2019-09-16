/*
 * Author: kayo
 */

#ifndef KAYOVM_CLASS_LOADER_H
#define KAYOVM_CLASS_LOADER_H

class Object;
class Class;

Class *loadSystemClass(const char *className);

Object *getSystemClassLoader();

#endif //KAYOVM_CLASS_LOADER_H
