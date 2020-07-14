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
[<font color=#00FF00>Pass</font>] HelloWorld.java<br/>[<font color=#00FF00>Pass</font>] JumpMultiLoop.java<br/>[<font color=#FF0000>Fail</font>] PrimeNumber.java<br/>[<font color=#00FF00>Pass</font>] PrintArgs.java<br/>[<font color=#FF0000>Fail</font>] RuntimeTest.java<br/>[<font color=#FF0000>Fail</font>] SysPropsTest.java<br/>
[<font color=#00FF00>Pass</font>] array.ArrayTest.java<br/>[<font color=#FF0000>Fail</font>] array.BigArrayTest.java<br/>[<font color=#00FF00>Pass</font>] array.BubbleSort.java<br/>[<font color=#00FF00>Pass</font>] array.ZeroLenArrayTest.java<br/>
[<font color=#FF0000>Fail</font>] atomic.AtomicIntegerTest.java<br/>
[<font color=#FF0000>Fail</font>] classloader.ClassLoaderTest.java<br/>
[<font color=#FF0000>Fail</font>] datetime.TimeZoneTest.java<br/>
[<font color=#00FF00>Pass</font>] exception.CatchTest.java<br/>[<font color=#FF0000>Fail</font>] exception.ClassLibExTest.java<br/>[<font color=#00FF00>Pass</font>] exception.FinallyReturnTest.java<br/>[<font color=#00FF00>Pass</font>] exception.FinallyTest.java<br/>[<font color=#FF0000>Fail</font>] exception.InstructionExTest.java<br/>[<font color=#FF0000>Fail</font>] exception.InstructionNpeTest.java<br/>[<font color=#00FF00>Pass</font>] exception.StackTraceTest.java<br/>[<font color=#FF0000>Fail</font>] exception.UncaughtTest.java<br/>
[<font color=#FF0000>Fail</font>] field.ConstantStaticFieldsTest.java<br/>[<font color=#FF0000>Fail</font>] field.FieldAccessTest.java<br/>[<font color=#00FF00>Pass</font>] field.FieldsTest.java<br/>
[<font color=#00FF00>Pass</font>] file.FileDescriptorTest.java<br/>[<font color=#FF0000>Fail</font>] file.FileNotFoundTest.java<br/>[<font color=#FF0000>Fail</font>] file.FileTest.java<br/>[<font color=#FF0000>Fail</font>] file.RandomAccessFileTest.java<br/>
[<font color=#00FF00>Pass</font>] initialization.Init1.java<br/>[<font color=#00FF00>Pass</font>] initialization.Init2.java<br/>[<font color=#FF0000>Fail</font>] initialization.InitInterface.java<br/>[<font color=#00FF00>Pass</font>] initialization.InitStaticField.java<br/>[<font color=#00FF00>Pass</font>] initialization.ObjectInitTest.java<br/>
[<font color=#00FF00>Pass</font>] instructions.AThrow.java<br/>[<font color=#FF0000>Fail</font>] instructions.CheckCast.java<br/>[<font color=#FF0000>Fail</font>] instructions.InvokeTest.java<br/>[<font color=#FF0000>Fail</font>] instructions.LookupSwitch.java<br/>[<font color=#00FF00>Pass</font>] instructions.NewArray.java<br/>[<font color=#FF0000>Fail</font>] instructions.NewMultiArrayTest.java<br/>[<font color=#00FF00>Pass</font>] instructions.TableSwitch.java<br/>
[<font color=#FF0000>Fail</font>] interface0.InterfaceDefaultMethodTest.java<br/>[<font color=#FF0000>Fail</font>] interface0.InterfaceMethodTest.java<br/>[<font color=#FF0000>Fail</font>] interface0.InterfaceTest.java<br/>
[<font color=#00FF00>Pass</font>] invoke.InvokeFuncTest.java<br/>[<font color=#00FF00>Pass</font>] invoke.InvokeFuncTest1.java<br/>[<font color=#FF0000>Fail</font>] invoke.MethodHandleNativesTest.java<br/>[<font color=#FF0000>Fail</font>] invoke.MethodHandleTest.java<br/>[<font color=#FF0000>Fail</font>] invoke.MethodHandleTest1.java<br/>[<font color=#FF0000>Fail</font>] invoke.MethodHandleTest2.java<br/>[<font color=#00FF00>Pass</font>] invoke.MethodTypeTest.java<br/>
[<font color=#FF0000>Fail</font>] jetty.FileServer.java<br/>
[<font color=#FF0000>Fail</font>] lambda.LambdaTest.java<br/>
[<font color=#FF0000>Fail</font>] method.ArgsPassTest.java<br/>
[<font color=#FF0000>Fail</font>] network.InetAddressTest.java<br/>[<font color=#FF0000>Fail</font>] network.SocketConnectTest.java<br/>[<font color=#FF0000>Fail</font>] network.SocketListenTest.java<br/>[<font color=#FF0000>Fail</font>] network.UrlTest.java<br/>
[<font color=#FF0000>Fail</font>] nio.ByteBufferTest.java<br/>
[<font color=#00FF00>Pass</font>] object.InheritanceTest.java<br/>[<font color=#00FF00>Pass</font>] object.ObjectTest.java<br/>
[<font color=#FF0000>Fail</font>] primary.DoubleTest.java<br/>[<font color=#FF0000>Fail</font>] primary.LongTest.java<br/>
[<font color=#00FF00>Pass</font>] reflect.ArrayClassTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.ArrayGetTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.ArraySetTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.CallerClassTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.ClassInitTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.ClassTest.java<br/>[<font color=#00FF00>Pass</font>] reflect.ClassTest1.java<br/>[<font color=#00FF00>Pass</font>] reflect.ClassTest2.java<br/>[<font color=#00FF00>Pass</font>] reflect.ClassTest3.java<br/>[<font color=#00FF00>Pass</font>] reflect.DeclaringClassTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.FieldTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.GenericTest.java<br/>[<font color=#00FF00>Pass</font>] reflect.MethodTest.java<br/>[<font color=#FF0000>Fail</font>] reflect.NestTest.java<br/>[<font color=#00FF00>Pass</font>] reflect.PrimitiveClassTest.java<br/>
[<font color=#FF0000>Fail</font>] stream.StreamTest.java<br/>
[<font color=#00FF00>Pass</font>] string.InternTest.java<br/>[<font color=#00FF00>Pass</font>] string.StringTest.java<br/>
[<font color=#FF0000>Fail</font>] sunmisc.UnsafeGetter.java<br/>[<font color=#FF0000>Fail</font>] sunmisc.UnsafeMemoryTest.java<br/>[<font color=#FF0000>Fail</font>] sunmisc.UnsafeObjectTest.java<br/>[<font color=#FF0000>Fail</font>] sunmisc.UnsafeParkTest.java<br/>[<font color=#FF0000>Fail</font>] sunmisc.UnsafeTest.java<br/>
[<font color=#FF0000>Fail</font>] thread.AliveTest.java<br/>[<font color=#00FF00>Pass</font>] thread.DaemonTest.java<br/>[<font color=#FF0000>Fail</font>] thread.DumpAllThreads.java<br/>[<font color=#FF0000>Fail</font>] thread.InterruptFlagTest.java<br/>[<font color=#FF0000>Fail</font>] thread.InterruptionTest.java<br/>[<font color=#FF0000>Fail</font>] thread.MainThreadTest.java<br/>[<font color=#FF0000>Fail</font>] thread.RunnableTest.java<br/>[<font color=#FF0000>Fail</font>] thread.SleepTest.java<br/>[<font color=#FF0000>Fail</font>] thread.SynchronizedTest.java<br/>[<font color=#FF0000>Fail</font>] thread.ThreadSubClassTest.java<br/>[<font color=#FF0000>Fail</font>] thread.WaitTest.java<br/>
[<font color=#00FF00>Pass</font>] wrapper.DoubleTest.java<br/>[<font color=#00FF00>Pass</font>] wrapper.FloatTest.java<br/>[<font color=#FF0000>Fail</font>] wrapper.IntegerCacheTest.java<br/>
## Reference
* jvms8.pdf
