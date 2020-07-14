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

## Test Cases [36/99(36.36%) Passed]
[<font color=#00FF00>Pass</font>] HelloWorld.java  [<font color=#00FF00>Pass</font>] JumpMultiLoop.java  [<font color=#FF0000>Fail</font>] PrimeNumber.java  [<font color=#00FF00>Pass</font>] PrintArgs.java  [<font color=#FF0000>Fail</font>] RuntimeTest.java  [<font color=#FF0000>Fail</font>] SysPropsTest.java  
[<font color=#00FF00>Pass</font>] array.ArrayTest.java  [<font color=#FF0000>Fail</font>] array.BigArrayTest.java  [<font color=#00FF00>Pass</font>] array.BubbleSort.java  [<font color=#00FF00>Pass</font>] array.ZeroLenArrayTest.java  
[<font color=#FF0000>Fail</font>] atomic.AtomicIntegerTest.java  
[<font color=#FF0000>Fail</font>] classloader.ClassLoaderTest.java  
[<font color=#FF0000>Fail</font>] datetime.TimeZoneTest.java  
[<font color=#00FF00>Pass</font>] exception.CatchTest.java  [<font color=#FF0000>Fail</font>] exception.ClassLibExTest.java  [<font color=#00FF00>Pass</font>] exception.FinallyReturnTest.java  [<font color=#00FF00>Pass</font>] exception.FinallyTest.java  [<font color=#FF0000>Fail</font>] exception.InstructionExTest.java  [<font color=#FF0000>Fail</font>] exception.InstructionNpeTest.java  [<font color=#00FF00>Pass</font>] exception.StackTraceTest.java  [<font color=#FF0000>Fail</font>] exception.UncaughtTest.java  
[<font color=#FF0000>Fail</font>] field.ConstantStaticFieldsTest.java  [<font color=#FF0000>Fail</font>] field.FieldAccessTest.java  [<font color=#00FF00>Pass</font>] field.FieldsTest.java  
[<font color=#00FF00>Pass</font>] file.FileDescriptorTest.java  [<font color=#FF0000>Fail</font>] file.FileNotFoundTest.java  [<font color=#FF0000>Fail</font>] file.FileTest.java  [<font color=#FF0000>Fail</font>] file.RandomAccessFileTest.java  
[<font color=#00FF00>Pass</font>] initialization.Init1.java  [<font color=#00FF00>Pass</font>] initialization.Init2.java  [<font color=#FF0000>Fail</font>] initialization.InitInterface.java  [<font color=#00FF00>Pass</font>] initialization.InitStaticField.java  [<font color=#00FF00>Pass</font>] initialization.ObjectInitTest.java  
[<font color=#00FF00>Pass</font>] instructions.AThrow.java  [<font color=#FF0000>Fail</font>] instructions.CheckCast.java  [<font color=#FF0000>Fail</font>] instructions.InvokeTest.java  [<font color=#FF0000>Fail</font>] instructions.LookupSwitch.java  [<font color=#00FF00>Pass</font>] instructions.NewArray.java  [<font color=#FF0000>Fail</font>] instructions.NewMultiArrayTest.java  [<font color=#00FF00>Pass</font>] instructions.TableSwitch.java  
[<font color=#FF0000>Fail</font>] interface0.InterfaceDefaultMethodTest.java  [<font color=#FF0000>Fail</font>] interface0.InterfaceMethodTest.java  [<font color=#FF0000>Fail</font>] interface0.InterfaceTest.java  
[<font color=#00FF00>Pass</font>] invoke.InvokeFuncTest.java  [<font color=#00FF00>Pass</font>] invoke.InvokeFuncTest1.java  [<font color=#FF0000>Fail</font>] invoke.MethodHandleNativesTest.java  [<font color=#FF0000>Fail</font>] invoke.MethodHandleTest.java  [<font color=#FF0000>Fail</font>] invoke.MethodHandleTest1.java  [<font color=#FF0000>Fail</font>] invoke.MethodHandleTest2.java  [<font color=#00FF00>Pass</font>] invoke.MethodTypeTest.java  
[<font color=#FF0000>Fail</font>] jetty.FileServer.java  
[<font color=#FF0000>Fail</font>] lambda.LambdaTest.java  
[<font color=#FF0000>Fail</font>] method.ArgsPassTest.java  
[<font color=#FF0000>Fail</font>] network.InetAddressTest.java  [<font color=#FF0000>Fail</font>] network.SocketConnectTest.java  [<font color=#FF0000>Fail</font>] network.SocketListenTest.java  [<font color=#FF0000>Fail</font>] network.UrlTest.java  
[<font color=#FF0000>Fail</font>] nio.ByteBufferTest.java  
[<font color=#00FF00>Pass</font>] object.InheritanceTest.java  [<font color=#00FF00>Pass</font>] object.ObjectTest.java  
[<font color=#FF0000>Fail</font>] primary.DoubleTest.java  [<font color=#FF0000>Fail</font>] primary.LongTest.java  
[<font color=#00FF00>Pass</font>] reflect.ArrayClassTest.java  [<font color=#FF0000>Fail</font>] reflect.ArrayGetTest.java  [<font color=#FF0000>Fail</font>] reflect.ArraySetTest.java  [<font color=#FF0000>Fail</font>] reflect.CallerClassTest.java  [<font color=#FF0000>Fail</font>] reflect.ClassInitTest.java  [<font color=#FF0000>Fail</font>] reflect.ClassTest.java  [<font color=#00FF00>Pass</font>] reflect.ClassTest1.java  [<font color=#00FF00>Pass</font>] reflect.ClassTest2.java  [<font color=#00FF00>Pass</font>] reflect.ClassTest3.java  [<font color=#00FF00>Pass</font>] reflect.DeclaringClassTest.java  [<font color=#FF0000>Fail</font>] reflect.FieldTest.java  [<font color=#FF0000>Fail</font>] reflect.GenericTest.java  [<font color=#00FF00>Pass</font>] reflect.MethodTest.java  [<font color=#FF0000>Fail</font>] reflect.NestTest.java  [<font color=#00FF00>Pass</font>] reflect.PrimitiveClassTest.java  
[<font color=#FF0000>Fail</font>] stream.StreamTest.java  
[<font color=#00FF00>Pass</font>] string.InternTest.java  [<font color=#00FF00>Pass</font>] string.StringTest.java  
[<font color=#FF0000>Fail</font>] sunmisc.UnsafeGetter.java  [<font color=#FF0000>Fail</font>] sunmisc.UnsafeMemoryTest.java  [<font color=#FF0000>Fail</font>] sunmisc.UnsafeObjectTest.java  [<font color=#FF0000>Fail</font>] sunmisc.UnsafeParkTest.java  [<font color=#FF0000>Fail</font>] sunmisc.UnsafeTest.java  
[<font color=#FF0000>Fail</font>] thread.AliveTest.java  [<font color=#00FF00>Pass</font>] thread.DaemonTest.java  [<font color=#FF0000>Fail</font>] thread.DumpAllThreads.java  [<font color=#FF0000>Fail</font>] thread.InterruptFlagTest.java  [<font color=#FF0000>Fail</font>] thread.InterruptionTest.java  [<font color=#FF0000>Fail</font>] thread.MainThreadTest.java  [<font color=#FF0000>Fail</font>] thread.RunnableTest.java  [<font color=#FF0000>Fail</font>] thread.SleepTest.java  [<font color=#FF0000>Fail</font>] thread.SynchronizedTest.java  [<font color=#FF0000>Fail</font>] thread.ThreadSubClassTest.java  [<font color=#FF0000>Fail</font>] thread.WaitTest.java  
[<font color=#00FF00>Pass</font>] wrapper.DoubleTest.java  [<font color=#00FF00>Pass</font>] wrapper.FloatTest.java  [<font color=#FF0000>Fail</font>] wrapper.IntegerCacheTest.java  
## Reference
* jvms8.pdf
