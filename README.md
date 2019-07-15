# JVM
A JVM is written in C++.

## Development environment
* Win10 x64
* CLion
* MinGW

## Dependence
* zlib, minizib
* pthread lib
* jre8 lib

## Features Support
✔ Java arithmetic, flow control, object-oriented programming(virtual method, inherit, etc.)
✔ String
✔ Array
✔ Exception
✘ Multi-thread
✘ Garbage collection
## Run
Ensure your Java version is Java8.

Two command-line options:
* -bcp path: set jre lib path.
* -cp path: set class path.
```
C:\>jvm HelloWorld -bcp "C:\Program Files\Java\jre1.8.0_162\lib" -cp D:\code\jvm\testclasses
```
or, using JAVA_HOME and CLASS_PATH environment variables.
```
C:\>jvm HelloWorld
```

## Quiz
### Hello World
```java
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello, world!");
    }
}
```

```
C:\>jvm HelloWorld -bcp "C:\Program Files\Java\jre1.8.0_162\lib" -cp D:\code\jvm\testclasses
Hello, world!

C:\>
```
### Bubble Sort
```java
package array;

import java.util.Arrays;

public class BubbleSort {
    public static void main(String[] args) {
        int[] arr = { 22, 84, 77, 11, 95, 9, 78, 56, 36, 97, 65, 36, 10, 24, 92 };
        bubbleSort(arr);
        System.out.println(Arrays.toString(arr));
    }

    private static void bubbleSort(int[] arr) {
        for (int i = 0; i < arr.length - 1; i++){
            for (int j = 0; j < arr.length - 1 - i; j++){
                if(arr[j] > arr[j + 1]){
                    int temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }
}
```

```
C:\>jvm array/BubbleSort
[9, 10, 11, 22, 24, 36, 36, 56, 65, 77, 78, 84, 92, 95, 97]

C:\>
```
### Stack Trace
```java
package exception;

public class StackTraceTest {

    public static void main(String[] args) {
        try {
            foo();
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }

    private static void foo() { bar(); }
    private static void bar() { bad(); }
    private static void bad() { throw new RuntimeException("BAD!"); }
}
```
```
C:\>jvm exception/StackTraceTest
java.lang.RuntimeException: BAD!
        at exception/StackTraceTest.bad(StackTraceTest.java:15)
        at exception/StackTraceTest.bar(StackTraceTest.java:14)
        at exception/StackTraceTest.foo(StackTraceTest.java:13)
        at exception/StackTraceTest.main(StackTraceTest.java:7)

C:\>
```
## Reference
* jvms8.pdf
* 张秀宏，自己动手写Java虚拟机。
* 周志明，深入理解Java虚拟机——JVM高级特性与最佳实践(第2版)。
