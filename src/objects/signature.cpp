/*
 * Author: kayo
 */

#include <cassert>
#include <cstdarg>
#include "signature.h"

using namespace slot;

//void parse_method_argv(const char *signature, const jvalue *values, slot_t *slots)
//{
//    assert(signature != nullptr && values != nullptr && slots != nullptr);
//    const char *p = signature;
//    assert(*p == '(');
//    p++; // skip start (
//
//    for (; *p != ')'; values++) {
//        if(*p == 'Z') {
//            *slots++ = islot(values->z);
//            p++;
//        } else if(*p == 'B') {
//            *slots++ = islot(values->b);
//            p++;
//        } else if(*p == 'C') {
//            *slots++ = islot(values->c);
//            p++;
//        } else if(*p == 'S') {
//            *slots++ = islot(values->s);
//            p++;
//        } else if(*p == 'I') {
//            *slots++ = islot(values->i);
//            p++;
//        } else if(*p == 'F') {
//            *slots++ = fslot(values->f);
//            p++;
//        } else if(*p == 'J') {
//            *slots++ = lslot(values->j);
//            slots++;
//            p++;
//        } else if(*p == 'D') {
//            *slots++ = dslot(values->d);
//            slots++;
//            p++;
//        } else {
//            if(*p == '[')
//            for(p++; *p == '['; p++);
//
//            if(*p == 'L')
//                while(*p++ != ';');
//            else
//                p++;
//
//            *slots++ = rslot((jref)(values->l));
//        }
//    }
//}

//void parse_method_args_va_list(const char *signature, va_list &args, jvalue *values)
//{
//    assert(signature != nullptr && values != nullptr);
//    const char *p = signature;
//    assert(*p == '(');
//    p++; // skip start (
//
//    for (; *p != ')'; values++) {
//        if(*p == 'Z') {
//            values->z = va_arg(args, jboolean);
//            p++;
//        } else if(*p == 'B') {
//            values->b = va_arg(args, jbyte);
//            p++;
//        } else if(*p == 'C') {
//            values->c = va_arg(args, jchar);
//            p++;
//        } else if(*p == 'S') {
//            values->s = va_arg(args, jshort);
//            p++;
//        } else if(*p == 'I') {
//            values->i = va_arg(args, jint);
//            p++;
//        } else if(*p == 'F') {
//            values->f = va_arg(args, jfloat);
//            p++;
//        } else if(*p == 'J') {
//            values->j = va_arg(args, jlong);
//            p++;
//        } else if(*p == 'D') {
//            values->d = va_arg(args, jdouble);
//            p++;
//        } else {
//            if(*p == '[')
//            for(p++; *p == '['; p++);
//
//            if(*p == 'L')
//                while(*p++ != ';');
//            else
//                p++;
//
//            values->l = va_arg(args, jobject);
//        }
//    }
//}

