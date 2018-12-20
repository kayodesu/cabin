/*
 * Virtual Machine Errors
 * A Java Virtual Machine implementation throws an object that is an instance of
 * a subclass of the class VirtualMachineError when an internal error or resource
 * limitation prevents it from implementing the semantics described in this chapter.
 * This specification cannot predict where internal errors or resource limitations may
 * be encountered and does not mandate precisely when they can be reported. Thus,
 * any of the VirtualMachineError subclasses defined below may be thrown at any
 * time during the operation of the Java Virtual Machine
 *
 * Author: Jia Yang
 */

#ifndef JVM_VM_ERROR_H
#define JVM_VM_ERROR_H

#define ERR_MSG_MAX_LEN 1023

/*
 * java.lang.InternalError
 *
 * An internal error has occurred in the Java Virtual Machine implementation
 * because of a fault in the software implementing the virtual machine,
 * a fault in the underlying host system software, or a fault in the hardware.
 * This error is delivered asynchronously when it is detected and may occur at any point in a program.
 */
void vm_internal_error(const char *msg);

/*
 * java.lang.OutOfMemoryError
 *
 * The Java Virtual Machine implementation has run out of either virtual or physical memory,
 * and the automatic storage manager was unable to reclaim enough memory to satisfy an object creation request.
 */
void vm_out_of_memory_error(const char *msg);

/*
 * java.lang.StackOverflowError
 *
 * The Java Virtual Machine implementation has run out of stack space for a thread,
 * typically because the thread is doing an unbounded number of recursive invocations
 * as a result of a fault in the executing program.
 */
void vm_stack_overflow_error(const char *msg);

/*
 * java.lang.UnknownError
 *
 * An exception or error has occurred, but the Java Virtual Machine
 * implementation is unable to report the actual exception or error.
 */
void vm_unknown_error(const char *msg);

#define VM_UNKNOWN_ERROR(...) \
        do { \
            char msg[ERR_MSG_MAX_LEN + 1] = { 0 }; \
            snprintf(msg, ERR_MSG_MAX_LEN, __VA_ARGS__); \
            vm_unknown_error(msg); \
        } while (false)

#endif //JVM_VM_ERROR_H
