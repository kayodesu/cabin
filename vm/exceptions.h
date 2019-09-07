/*
 * Author: kayo
 */

#ifndef KAYOVM_THROWABLES_H
#define KAYOVM_THROWABLES_H

/*
 * Virtual Machine Errors
 * A Java Virtual Machine implementation throws an Object that is an instance of
 * a subclass of the class VirtualMachineError when an internal error or resource
 * limitation prevents it from implementing the semantics described in this chapter.
 * This specification cannot predict where internal errors or resource limitations may
 * be encountered and does not mandate precisely when they can be reported. Thus,
 * any of the VirtualMachineError subclasses defined below may be thrown at any
 * time during the operation of the Java Virtual Machine
 */

/*
 * java.lang.InternalError
 *
 * An internal error has occurred in the Java Virtual Machine implementation
 * because of a fault in the software implementing the virtual machine,
 * a fault in the underlying host system software, or a fault in the hardware.
 * This error is delivered asynchronously when it is detected and may occur at any point in a program.
 */
#define INTERNAL_ERROR "java/lang/InternalError"

/*
 * java.lang.OutOfMemoryError
 *
 * The Java Virtual Machine implementation has run out of either virtual or physical memory,
 * and the automatic storage mgr was unable to reclaim enough memory to satisfy an Object creation request.
 */
#define OUT_OF_MEMORY_ERROR "java/lang/OutOfMemoryError"

/*
 * java.lang.StackOverflowError
 *
 * The Java Virtual Machine implementation has run out of stack space for a Thread,
 * typically because the Thread is doing an unbounded number of recursive invocations
 * as a result of a fault in the executing program.
 */
#define STACK_OVERFLOW_ERROR "java/lang/StackOverflowError"

/*
 * java.lang.UnknownError
 *
 * An exception or error has occurred, but the Java Virtual Machine
 * implementation is unable to report the actual exception or error.
 */
#define UNKNOWN_ERROR "java/lang/UnknownError"

#define INCOMPATIBLE_CLASS_CHANGE_ERROR "java/lang/IncompatibleClassChangeError"
#define ILLEGAL_ACCESS_ERROR "java/lang/IllegalAccessError"
#define ABSTRACT_METHOD_ERROR "java/lang/AbstractMethodError"
#define INSTANTIATION_ERROR "java/lang/InstantiationError"
#define NO_SUCH_FIELD_ERROR "java/lang/NoSuchFieldError"
#define NO_SUCH_METHOD_ERROR "java/lang/NoSuchMethodError"
#define CLASS_FORMAT_ERROR "java/lang/ClassFormatError"

#define INDEX_OUT_OF_BOUNDS_EXCEPTION "java/lang/IndexOutOfBoundsException"
#define CLONE_NOT_SUPPORTED_EXCEPTION "java/lang/CloneNotSupportedException"
#define CLASS_NOT_FOUND_EXCEPTION "java/lang/ClassNotFoundException"


[[noreturn]] void raiseException(const char *exceptionName, const char *msg = nullptr);

#endif //KAYOVM_THROWABLES_H
