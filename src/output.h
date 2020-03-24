/*
 * Author: kayo
 */

#ifndef JVM_OUTPUT_H
#define JVM_OUTPUT_H

#include "vmdef.h"

void write_bytes(jref obj, jbyte bytes[], jint len, bool append);

#endif //JVM_OUTPUT_H
