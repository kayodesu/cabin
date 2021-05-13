# Cabin VM
A toy JVM written in C.

## Development environment
* Win10 64 bit
* MinGW64
* g++

## Dependence
* openjdk16
* zlib 1.2.11
* minizip 1.2.0
* libffi 3.3

## Features  
- [x] Java arithmetic, flow control, object-oriented programming(virtual method, inherit, etc.)  
- [x] String  
- [x] Array  
- [x] Exception  
- [ ] Lambda  
- [ ] Multi-thread  
- [ ] Garbage collection  
## Run
Ensure you already set `JAVA_HOME` environment variable.

One command-line option:
* -cp path: set class path.
```
C:\>cabin HelloWorld -cp D:\Code\cabin\test-classes\out\production\test-classes
```
or, using CLASS_PATH environment variable.
```
C:\>cabin HelloWorld
```
