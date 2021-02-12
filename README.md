# Cabin VM
An experimental JVM written in C++.

## Development environment
* Win10 64 bit
* MinGW64
* g++

## Dependence
* jre8 lib
* zlib 1.2.11
* minizip 1.2.0

## Features  
- [x] Java arithmetic, flow control, object-oriented programming(virtual method, inherit, etc.)  
- [x] String  
- [x] Array  
- [x] Exception  
- [ ] Lambda  
- [ ] Multi-thread  
- [ ] Garbage collection  
## Run
Ensure your Java version is Java8, and already set `JAVA_HOME` environment variable.

One command-line option:
* -cp path: set class path.
```
C:\>cabin HelloWorld -cp D:\Code\yovm\test-classes\out\production\test-classes
```
or, using CLASS_PATH environment variable.
```
C:\>cabin HelloWorld
```

## Test Cases [70/111(63.06%) Passed]
:heavy_check_mark: HelloWorld.java<br/>:heavy_check_mark: JumpMultiLoop.java<br/>:x: JVMTest.java<br/>:heavy_check_mark: PrintArgs.java<br/>:x: PrintTest.java<br/>:heavy_check_mark: RuntimeTest.java<br/>:x: SysPropsTest.java<br/><br/>:heavy_check_mark: array.ArrayCastTest.java<br/>:heavy_check_mark: array.ArrayClassTest.java<br/>:heavy_check_mark: array.ArrayInitTest.java<br/>:heavy_check_mark: array.ArrayTest.java<br/>:heavy_check_mark: array.BigArrayTest.java<br/>:heavy_check_mark: array.BubbleSort.java<br/>:heavy_check_mark: array.ZeroLenArrayTest.java<br/><br/>:x: atomic.AtomicIntegerTest.java<br/><br/>:heavy_check_mark: classloader.ClassLoaderTest.java<br/><br/>:x: datetime.TimeZoneTest.java<br/><br/>:heavy_check_mark: exception.CatchTest.java<br/>:x: exception.ClassLibExTest.java<br/>:heavy_check_mark: exception.FinallyReturnTest.java<br/>:heavy_check_mark: exception.FinallyTest.java<br/>:heavy_check_mark: exception.InstructionExTest.java<br/>:heavy_check_mark: exception.InstructionNpeTest.java<br/>:heavy_check_mark: exception.StackTraceTest.java<br/>:heavy_check_mark: exception.UncaughtTest.java<br/><br/>:heavy_check_mark: field.ConstantStaticFieldsTest.java<br/>:heavy_check_mark: field.FieldAccessTest.java<br/>:heavy_check_mark: field.FieldsTest.java<br/><br/>:heavy_check_mark: file.FileDescriptorTest.java<br/>:heavy_check_mark: file.FileNotFoundTest.java<br/>:x: file.FileTest.java<br/>:x: file.RandomAccessFileTest.java<br/><br/>:heavy_check_mark: initialization.Init1.java<br/>:heavy_check_mark: initialization.Init2.java<br/>:heavy_check_mark: initialization.InitInterface.java<br/>:heavy_check_mark: initialization.InitStaticField.java<br/>:heavy_check_mark: initialization.ObjectInitTest.java<br/><br/>:heavy_check_mark: instructions.AThrow.java<br/>:heavy_check_mark: instructions.CheckCast.java<br/>:x: instructions.InvokeTest.java<br/>:heavy_check_mark: instructions.LookupSwitch.java<br/>:heavy_check_mark: instructions.NewArray.java<br/>:heavy_check_mark: instructions.NewMultiArrayTest.java<br/>:heavy_check_mark: instructions.TableSwitch.java<br/><br/>:heavy_check_mark: interface0.InterfaceDefaultMethodTest.java<br/>:heavy_check_mark: interface0.InterfaceMethodTest.java<br/>:x: interface0.InterfaceTest.java<br/><br/>:heavy_check_mark: invoke.InvokeFuncTest.java<br/>:heavy_check_mark: invoke.InvokeFuncTest1.java<br/>:x: invoke.MemberNameTest.java<br/>:x: invoke.MethodHandleNativesTest.java<br/>:x: invoke.MethodHandleTest.java<br/>:x: invoke.MethodHandleTest1.java<br/>:x: invoke.MethodHandleTest2.java<br/>:heavy_check_mark: invoke.MethodTypeTest.java<br/><br/>:x: jetty.FileServer.java<br/><br/>:x: lambda.LambdaTest.java<br/><br/>:x: method.ArgsPassTest.java<br/><br/>:x: network.InetAddressTest.java<br/>:x: network.SocketConnectTest.java<br/>:x: network.SocketListenTest.java<br/>:x: network.UrlTest.java<br/><br/>:heavy_check_mark: nio.ByteBufferTest.java<br/><br/>:x: object.EqualsTest.java<br/>:heavy_check_mark: object.InheritanceTest.java<br/>:heavy_check_mark: object.ObjectTest.java<br/><br/>:x: primary.DoubleTest.java<br/>:x: primary.LongTest.java<br/><br/>:heavy_check_mark: reflect.ArrayClassTest.java<br/>:heavy_check_mark: reflect.ArrayGetTest.java<br/>:heavy_check_mark: reflect.ArrayLengthTest.java<br/>:heavy_check_mark: reflect.ArraySetTest.java<br/>:heavy_check_mark: reflect.CallerClassTest.java<br/>:heavy_check_mark: reflect.ClassInitTest.java<br/>:x: reflect.ClassTest.java<br/>:heavy_check_mark: reflect.ClassTest1.java<br/>:heavy_check_mark: reflect.ClassTest2.java<br/>:heavy_check_mark: reflect.ClassTest3.java<br/>:heavy_check_mark: reflect.ClassTest4.java<br/>:heavy_check_mark: reflect.DeclaringClassTest.java<br/>:heavy_check_mark: reflect.FieldTest.java<br/>:heavy_check_mark: reflect.GenericTest.java<br/>:heavy_check_mark: reflect.MethodTest.java<br/>:x: reflect.NestTest.java<br/>:heavy_check_mark: reflect.PrimitiveClassTest.java<br/><br/>:x: stream.StreamTest.java<br/><br/>:heavy_check_mark: string.InternTest.java<br/>:heavy_check_mark: string.StringTest.java<br/><br/>:heavy_check_mark: sunmisc.UnsafeCASTest.java<br/>:heavy_check_mark: sunmisc.UnsafeMemoryTest.java<br/>:x: sunmisc.UnsafeMemoryTest1.java<br/>:heavy_check_mark: sunmisc.UnsafeObjectTest.java<br/>:heavy_check_mark: sunmisc.UnsafeObjectTest1.java<br/>:heavy_check_mark: sunmisc.UnsafeObjectTest2.java<br/>:x: sunmisc.UnsafeObjectVolatileTest.java<br/>:x: sunmisc.UnsafeParkTest.java<br/>:heavy_check_mark: sunmisc.UnsafeTest.java<br/><br/>:x: thread.AliveTest.java<br/>:heavy_check_mark: thread.DaemonTest.java<br/>:x: thread.DumpAllThreads.java<br/>:x: thread.InterruptFlagTest.java<br/>:x: thread.InterruptionTest.java<br/>:x: thread.MainThreadTest.java<br/>:x: thread.RunnableTest.java<br/>:x: thread.SleepTest.java<br/>:x: thread.SynchronizedTest.java<br/>:x: thread.ThreadSubClassTest.java<br/>:x: thread.WaitTest.java<br/><br/>:heavy_check_mark: wrapper.DoubleTest.java<br/>:heavy_check_mark: wrapper.FloatTest.java<br/>:heavy_check_mark: wrapper.IntegerCacheTest.java<br/><br/>
## Reference
* jvms8.pdf
