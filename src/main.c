#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "jvm.h"
#include "native/registry.h"

int print_level = 1;
static char main_class[NAME_MAX] = { 0 };

/*
 * -printlevel: 打印级别
 * -bootstrapclasspath path: JavaHome路径, 对应 jre/lib 目录
 */
static bool parse_args(int argc, char* argv[])
{
    // 可执行程序的名字为 argv[0]，跳过。
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (++i >= argc) {
                printvm("缺少参数：%s\n", name);
                return false;
            }
            const char *value = argv[i];

            if (strcmp(name, "-printlevel") == 0) {
                print_level = atoi(value);
            } else if (strcmp(name, "-bootstrapclasspath") == 0) {
                strcpy(bootstrap_classpath, value);
            } else {
                printvm("不认识的参数：%s\n", name);
                return false;
            }
        } else {
            strcpy(main_class, argv[i]);
        }
    }
    if (print_level < 1 /*or printLevel > 4*/) {
        printvm("print_level 不合法：%d\n", print_level);
        return false;
    }

    if (main_class[0] == 0) { // empty
        printvm("无main class\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argc, argv)) {
        jvm_abort("error\n"); // todo
        return -1;
    }

    // 命令行参数没有设置 bootstrap_classpath 的值，那么使用 JAVA_HOME 环境变量
    if (bootstrap_classpath[0] == 0) { // empty
        char *java_home = getenv("JAVA_HOME");
        if (java_home == NULL) {
            // todo error
            printvm("no java home");
            return -1;
        }
        strcpy(bootstrap_classpath, java_home);
    }

    if (extension_classpath[0] == 0) { // empty
        strcpy(extension_classpath, bootstrap_classpath);
        strcat(extension_classpath, "/ext");
    }

    printvm("bootstrap_classpath: %s\n", bootstrap_classpath);
    printvm("extension_classpath: %s\n", extension_classpath);
    printvm("user_classpath: %s\n", user_classpath);

    // todo 测试 JAVA_HOME 是不是  java8  版本

    register_all_native_methods();

//    printRegisteredNativeMethods();

    void test();
    test();

    return 0;
}

void test() {
    /* ---------------- passed -------------------------*/


    /* ---------------- NOT passed -------------------------*/
    // array
//    startJVM("io/github/jiayanggo/array/ArrayDemo");
//    startJVM("io/github/jiayanggo/array/ArrayTest");
//    startJVM("io/github/jiayanggo/array/BubbleSort");

    // string
    start_jvm("io/github/jiayanggo/string/StringTest");
//    start_jvm("io/github/jiayanggo/string/Mutf8Test");

//    startJVM("io/github/jiayanggo/HelloWorld");

//    startJVM("io/github/jiayanggo/string/StringOut");
//    startJVM("io/github/jiayanggo/ObjectTest");
//    startJVM("io/github/jiayanggo/field/FieldsTest");

//    startJVM("io/github/jiayanggo/exception/CatchTest");
//    startJVM("io/github/jiayanggo/exception/UncaughtTest");

//    startJVM("io/github/jiayanggo/array/BubbleSort");

//    startJVM("io/github/jiayanggo/reflection/GetClassTest");
//    startJVM("io/github/jiayanggo/reflection/FieldTest");

//    startJVM("io/github/jiayanggo/nio/ByteBufferTest");  // todo 没有实现的native方法。

//    startJVM("io/github/jiayanggo/instructions/ANewArray"); // todo 三维数组实现不正确。
}