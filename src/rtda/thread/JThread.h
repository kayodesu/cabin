/*
 * Author: Jia Yang
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <stddef.h>
#include <stdbool.h>

struct stack_frame;
struct jmethod;
struct slot;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */

struct jthread;

struct jthread* jthread_create();

void jthread_set_pc(struct jthread *thread, size_t new_pc);
size_t jthread_get_pc(const struct jthread *thread);

bool jthread_is_stack_empty(const struct jthread *thread);

struct stack_frame* jthread_top_frame(struct jthread *thread);

void jthread_pop_frame(struct jthread *thread);

void jthread_push_frame(struct jthread *thread, struct stack_frame *frame);

/*
 * 生成包含@method的栈帧，并将其压入@thread的虚拟机栈中，
 * 同时中断当前虚拟机栈栈顶的栈帧，以期执行@method所对应的新生成的栈帧。
 *
 * Note：这里只是生成栈帧并压栈，如果在一个循环中调用此方法，会造成联系压入多个栈帧，如：
 * 当前虚拟机栈：
 * ...|top frame|
 * 执行下面语句
 *      for(int i = 0; i < 3; i++) jthread_invoke_method(...);
 * 后的虚拟机栈：
 * ...|top frame|new frame 1|new frame 2|new frame 3|
 * top frame中断后会执行new frame 3，但是如果@method方法有返回值，
 * 执行new frame 3后，其返回值会压入new frame 2，这是错误的，
 * 因为不是new frame 2调用的它，是top frame调用的。
 * 这种错误的压入会造成new frame 2的操作栈溢出（错误的压入new frame 3的返回值所致）。
 *
 * 综上：不支持在循环中调用 jthread_invoke_method 来执行带返回值的方法（@method）。
 */
void jthread_invoke_method(struct jthread *thread, struct jmethod *method, const struct slot *args);

void jthread_destroy(struct jthread *thread);

#endif //JVM_JTHREAD_H
