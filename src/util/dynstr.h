#ifndef CABIN_DYNSTR_H
#define CABIN_DYNSTR_H

/*
 * dynamic string
 */

typedef struct dynstr DynStr;

struct dynstr {
    // 用于记录buf数组中使用的字节的数目
    // 和dynstr存储的字符串的长度相等
    int len;

    // 用于记录buf数组中没有使用的字节的数目
    int free;

    // 字节数组，用于储存字符串
    char buf[];   //buf的大小等于len+free+1，其中多余的1个字节是用来存储’\0’的。
};

DynStr *dynstr_create();


#endif //CABIN_DYNSTR_H
