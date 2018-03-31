#include <string>

#include "jvm.h"
#include "native/registry.h"

using namespace std;


static const string JRE8_LIB_PATH = "C:\\Program Files\\Java\\jre1.8.0_162\\lib";  // todo

// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
string bootstrapClasspath = JRE8_LIB_PATH;

// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
string extensionClasspath = JRE8_LIB_PATH + "/ext";

// 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
string userClasspath = "D:\\code\\jvm\\testclasses\\out"; // todo


int main(int argc, char* argv[]) {
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
