#include <string>
#include <iostream>

#include <cstring>
#include <cstdlib>

#include "jvm.h"
#include "native/registry.h"

using namespace std;


//static const string JRE8_LIB_PATH = R"(C:\Program Files\Java\jre1.8.0_162\lib)";  // todo

//// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
//string bootstrapClasspath;
//
//// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
//string extensionClasspath;
//
//// 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
//string userClasspath = "D:\\code\\jvm\\testclasses\\out"; // todo

int printLevel = 1;
static string mainClass = "";

/*
 * -printlevel: 打印级别
 * -bootstrapclasspath path: JavaHome路径, 对应 jre/lib 目录
 */
static bool parseArgs(int argc, char* argv[]) {
    // 可执行程序的名字为 argv[0]，跳过。
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *name = argv[i];
            if (++i >= argc) {
                cout << "缺少参数：" << name << endl;
                return false;
            }
            const char *value = argv[i];

            if (strcmp(name, "-printlevel") == 0) {
                printLevel = atoi(value);
            } else if (strcmp(name, "-bootstrapclasspath") == 0) {
                JvmEnv::bootstrapClasspath = value;
            } else {
                cout << "不认识的参数：" << name << endl;
                return false;
            }
        } else {
            mainClass = argv[i];
        }
    }
    if (printLevel < 1 /*or printLevel > 4*/) {
        cout << "printLevel不合法：" << printLevel << endl;
        return false;
    }

    if (mainClass.empty()) {
        cout << "无main class" << endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (!parseArgs(argc, argv)) {
        return -1;
    }

    if (JvmEnv::bootstrapClasspath.empty()) {
        char *javaHome = getenv("JAVA_HOME");
        if (javaHome == nullptr) {
            // todo errro
            jprintf("no javaHome");
            return -1;
        }
        JvmEnv::bootstrapClasspath = javaHome;
        JvmEnv::extensionClasspath = JvmEnv::bootstrapClasspath + "/ext";
    }

    // todo 测试 JAVA_HOME 是不是  java8  版本

    registerAllNativeMethods();

//    printRegisteredNativeMethods();


//    testClassLoader();
//    testEncoding();
//    return 0;

//    startJVM("com/github/jiayanggo/HelloWorld");
//    startJVM("com/github/jiayanggo/string/StringTest");
//    startJVM("com/github/jiayanggo/field/FieldsTest0");

//    startJVM("com/github/jiayanggo/exception/CatchTest");
//    startJVM("com/github/jiayanggo/exception/UncaughtTest");

//    startJVM("com/github/jiayanggo/array/BubbleSort");

//    startJVM("com/github/jiayanggo/reflection/GetClassTest");
//    startJVM("com/github/jiayanggo/reflection/FieldTest");


    startJVM("com/github/jiayanggo/instructions/ANewArray");
    return 0;
}
