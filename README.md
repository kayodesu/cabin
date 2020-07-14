# JVM
An experimental JVM written in C++.

## Development environment
* Win10 64 bit
* MinGW64
* g++

## Dependence
* jre8 lib
* zlib 1.2.11
* minizip 1.2.0
* libffi 3.3

## Features Support
* Java arithmetic, flow control, object-oriented programming(virtual method, inherit, etc.)
* String
* Array
* Exception
<!--
* Multi-thread
* Garbage collection -->
## Run
Ensure your Java version is Java8, and already set `JAVA_HOME` environment variable.

One command-line option:
* -cp path: set class path.
```
C:\>jvm HelloWorld -cp D:\code\kayo\testclasses
```
or, using CLASS_PATH environment variable.
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
C:\>jvm HelloWorld -cp D:\code\jvm\testclasses
Hello, world!

C:\>
```

## Test Cases(36/99[36.36%] Passed)
[Pass] HelloWorld.java
[Pass] JumpMultiLoop.java
[Fail] PrimeNumber.java
[Pass] PrintArgs.java
[Fail] RuntimeTest.java
[Fail] SysPropsTest.java

[Pass] array.ArrayTest.java
[Fail] array.BigArrayTest.java
[Pass] array.BubbleSort.java
[Pass] array.ZeroLenArrayTest.java

[Fail] atomic.AtomicIntegerTest.java

[Fail] classloader.ClassLoaderTest.java

[Fail] datetime.TimeZoneTest.java

[Pass] exception.CatchTest.java
[Fail] exception.ClassLibExTest.java
[Pass] exception.FinallyReturnTest.java
[Pass] exception.FinallyTest.java
[Fail] exception.InstructionExTest.java
[Fail] exception.InstructionNpeTest.java
[Pass] exception.StackTraceTest.java
[Fail] exception.UncaughtTest.java

[Fail] field.ConstantStaticFieldsTest.java
[Fail] field.FieldAccessTest.java
[Pass] field.FieldsTest.java

[Pass] file.FileDescriptorTest.java
[Fail] file.FileNotFoundTest.java
[Fail] file.FileTest.java
[Fail] file.RandomAccessFileTest.java

[Pass] initialization.Init1.java
[Pass] initialization.Init2.java
[Fail] initialization.InitInterface.java
[Pass] initialization.InitStaticField.java
[Pass] initialization.ObjectInitTest.java

[Pass] instructions.AThrow.java
[Fail] instructions.CheckCast.java
[Fail] instructions.InvokeTest.java
[Fail] instructions.LookupSwitch.java
[Pass] instructions.NewArray.java
[Fail] instructions.NewMultiArrayTest.java
[Pass] instructions.TableSwitch.java

[Fail] interface0.InterfaceDefaultMethodTest.java
[Fail] interface0.InterfaceMethodTest.java
[Fail] interface0.InterfaceTest.java

[Pass] invoke.InvokeFuncTest.java
[Pass] invoke.InvokeFuncTest1.java
[Fail] invoke.MethodHandleNativesTest.java
[Fail] invoke.MethodHandleTest.java
[Fail] invoke.MethodHandleTest1.java
[Fail] invoke.MethodHandleTest2.java
[Pass] invoke.MethodTypeTest.java

[Fail] jetty.FileServer.java

[Fail] lambda.LambdaTest.java

[Fail] method.ArgsPassTest.java

[Fail] network.InetAddressTest.java
[Fail] network.SocketConnectTest.java
[Fail] network.SocketListenTest.java
[Fail] network.UrlTest.java

[Fail] nio.ByteBufferTest.java

[Pass] object.InheritanceTest.java
[Pass] object.ObjectTest.java

[Fail] primary.DoubleTest.java
[Fail] primary.LongTest.java

[Pass] reflect.ArrayClassTest.java
[Fail] reflect.ArrayGetTest.java
[Fail] reflect.ArraySetTest.java
[Fail] reflect.CallerClassTest.java
[Fail] reflect.ClassInitTest.java
[Fail] reflect.ClassTest.java
[Pass] reflect.ClassTest1.java
[Pass] reflect.ClassTest2.java
[Pass] reflect.ClassTest3.java
[Pass] reflect.DeclaringClassTest.java
[Fail] reflect.FieldTest.java
[Fail] reflect.GenericTest.java
[Pass] reflect.MethodTest.java
[Fail] reflect.NestTest.java
[Pass] reflect.PrimitiveClassTest.java

[Fail] stream.StreamTest.java

[Pass] string.InternTest.java
[Pass] string.StringTest.java

[Fail] sunmisc.UnsafeGetter.java
[Fail] sunmisc.UnsafeMemoryTest.java
[Fail] sunmisc.UnsafeObjectTest.java
[Fail] sunmisc.UnsafeParkTest.java
[Fail] sunmisc.UnsafeTest.java

[Fail] thread.AliveTest.java
[Pass] thread.DaemonTest.java
[Fail] thread.DumpAllThreads.java
[Fail] thread.InterruptFlagTest.java
[Fail] thread.InterruptionTest.java
[Fail] thread.MainThreadTest.java
[Fail] thread.RunnableTest.java
[Fail] thread.SleepTest.java
[Fail] thread.SynchronizedTest.java
[Fail] thread.ThreadSubClassTest.java
[Fail] thread.WaitTest.java

[Pass] wrapper.DoubleTest.java
[Pass] wrapper.FloatTest.java
[Fail] wrapper.IntegerCacheTest.java

## Reference
* jvms8.pdf
