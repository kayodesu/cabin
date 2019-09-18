/*
 * Author: kayo
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include <cstddef>
#include <string>
#include <vector>
#include "Member.h"
#include "../../classfile/Attribute.h"
#include "../../native/registry.h"
#include "../../utf8.h"
#include "../../symbol.h"


class ArrayObject;

class Method: public Member {
    ArrayObject *parameterTypes = nullptr; // [Ljava/lang/Class;
    Class *returnType = nullptr;     // java/lang/Class
    ArrayObject *exceptionTypes = nullptr; // [Ljava/lang/Class;

public:
    int vtableIndex = -1;
    int itableIndex = -1;

    u2 maxStack = 0;
    u2 maxLocals = 0;
    u2 arg_slot_count = 0;

    std::vector<LineNumberTable> lineNumberTables;

    u1 *code = nullptr;
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

    /*
     * 判断此方法是否由 invokevirtual 指令调用，
     * final方法虽然非虚，但也由 invokevirtual 调用。
     * todo
     * 一个 final non-private 方法则可以覆写基类的虚方法，并且可以被基类引用通过invokevirtual调用到。
     * 参考 https://www.zhihu.com/question/45131640
     */
    bool isVirtual() const
    {
        return !isPrivate() && !isStatic() && !utf8_equals(name, SYMBOL(object_init));
    }

    ArrayObject *getParameterTypes();
    Class *getReturnType();
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

        struct CatchType {
            bool resolved;
            union {
                Class *clazz; // if resolved
                const char *className; // if not resolved
            } u;
        } *catchType = nullptr;

        ExceptionTable(Class *clazz, BytecodeReader &r);
    };

    std::vector<ExceptionTable> exceptionTables;

public:
    ~Method()
    {
        for (auto &t : exceptionTables)
            delete t.catchType;
    }
};

#endif //JVM_JMETHOD_H
