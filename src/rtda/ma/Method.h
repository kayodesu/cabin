/*
 * Author: Jia Yang
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include <cstddef>
#include <string>
#include <vector>
#include "Member.h"
#include "../../classfile/Attribute.h"
#include "../../native/registry.h"


class ArrayObject;
class ClassObject;

class Method: public Member {
    ArrayObject *parameterTypes = nullptr; // [Ljava/lang/Class;
    ClassObject *returnType = nullptr;     // java/lang/Class
    ArrayObject *exceptionTypes = nullptr; // [Ljava/lang/Class;

public:
    int vtableIndex;

    u2 maxStack = 0;
    u2 maxLocals = 0;
    u2 arg_slot_count = 0;

    std::vector<LineNumberTable> lineNumberTables;

    const u1 *code = nullptr;
    size_t codeLen = 0;

    native_method_t nativeMethod = nullptr; // present only if native
#if 0
    // 此方法可能会抛出的受检异常
    char *checked_exceptions;
    u2 checked_exceptions_num;

    struct parameter_annotation *runtime_visible_parameter_annotations;
    u2 runtime_visible_parameter_annotations_num;

    struct parameter_annotation *runtime_invisible_parameter_annotations;
    u2 runtime_invisible_parameter_annotations_num;

    struct annotation *runtime_visible_annotations;
    u2 runtime_visible_annotations_num;

    struct annotation *runtime_invisible_annotations;
    u2 runtime_invisible_annotations_num;
#endif

private:
    void calArgsSlotsCount();
    void parseCodeAttr(BytecodeReader &r);

public:
    Method(Class *c, BytecodeReader &r);

    ArrayObject *getParameterTypes();
    ClassObject *getReturnType();
    ArrayObject *getExceptionTypes();

    int getLineNumber(int pc) const;

    /*
     * @pc, 发生异常的位置
     */
    int findExceptionHandler(Class *exception_type, size_t pc);

    std::string toString() const;

private:
    /*
     * 异常处理表
     * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
     * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
     */
    struct ExceptionTable {
        u2 startPc;
        u2 endPc;
        u2 handlerPc;
        Class *catchType;

        ExceptionTable(Class *clazz, BytecodeReader &r);
    };

    std::vector<ExceptionTable> exceptionTables;
};

#endif //JVM_JMETHOD_H
