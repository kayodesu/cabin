/*
 * Author: Jia Yang
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "jtypes.h"

/*
 * 是否开启调试模式，
 * 如不开启，可注释掉此define
 */
//#define JVM_DEBUG

#ifndef PATH_MAX
#define PATH_MAX 260 // todo
#endif

// todo
//#define JVM_STRING_LEN_MAX (0xFFFF - 1)
//#define JVM_PKT_NAME_LEN_MAX JVM_STRING_LEN_MAX
//#define JVM_CLASS_NAME_LEN_MAX JVM_STRING_LEN_MAX

// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
extern char bootstrap_classpath[];

// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
extern char extension_classpath[];

// 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
/*
 * 用户类路径的默认值是当前目录，也就是“.”。可以设置CLASSPATH环境变量来修改用户类路径。
 * 可以通过 -cp 选项修改，
 * -cp 选项的优先级更高，可以覆盖CLASSPATH环境变量设置。
 * -cp 选项既可以指定目录，也可以指定JAR文件或者ZIP文件。 todo
 */
extern char user_classpath[]; // todo


// todo 说明
extern struct classloader *bootstrap_loader;

#define VM_MALLOC_EXT(type, count, extra_len, var_name) type *(var_name) = malloc(sizeof(type) * (count) + (extra_len))
#define VM_MALLOCS(type, count, var_name) VM_MALLOC_EXT(type, count, 0, var_name)
#define VM_MALLOC(type, var_name) VM_MALLOCS(type, 1, var_name)

/*
 * 用来表示字段和方法。
 * name_and_type 的名称来源于 Java 虚拟机规范文档.
 * todo 字段的描述符形式，方法的描述符形式
 */
struct name_and_type {
    const char *name;
    const char *descriptor;
};

// The system Thread group.
extern struct jobject *system_thread_group;

// name of main thread
#define MAIN_THREAD_NAME "main"

#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

// 出现异常，退出jvm
#define jvm_abort(...) do { printf("fatal error. "); printf(__VA_ARGS__); exit(-1); } while(false)

#endif //JVM_JVM_H
