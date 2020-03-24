/*
 * Author: kayo
 */

#ifndef KAYO_SIGNATURE_H
#define KAYO_SIGNATURE_H

#include "../native/jni.h"

void parse_method_args_va_list(const char *signature, va_list &args, jvalue *values);

#endif //KAYO_SIGNATURE_H
