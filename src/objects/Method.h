/*
 * Author: kayo
 */

#ifndef JVM_JMETHOD_H
#define JVM_JMETHOD_H

#include <cstddef>
#include <string>
#include <vector>
#include "../classfile/Attribute.h"
#include "../native/registry.h"
#include "../symbol.h"
#include "../util/encoding.h"
#include "ConstantPool.h"
#include "Modifier.h"


class Array;

class Method {
//    Array *parameterTypes = nullptr; // [Ljava/lang/Class;
//    Class *returnType = nullptr;     // Ljava/lang/Class;
    Object *type = nullptr;          // Ljava/lang/invoke/MethodType;
    Array *exceptionTypes = nullptr; // [Ljava/lang/Class;

public:
    // 定义此 Method 的类
    Class *clazz;
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    jint modifiers;

    bool deprecated = false;
    const char *signature = nullptr;

    int vtableIndex = -1;
    int itableIndex = -1;

    u2 maxStack = 0;
    u2 maxLocals = 0;
    u2 arg_slot_count = 0;

//    struct LineNumberTable {
//        // The value of the start_pc item must indicate the index into the code array
//        // at which the code for a new line in the original source file begins.
//        // The value of start_pc must be less than the value of the code_length
//        // item of the Code attribute of which this LineNumberTable is an attribute.
//        u2 start_pc;
//        // The value of the line_number item must give the corresponding line number in the original source file.
//        u2 line_number;
//
//        explicit LineNumberTable(BytecodeReader &r);
//    };
    std::vector<LineNumberTable> lineNumberTables;

    u1 *code = nullptr;
    size_t codeLen = 0;

    native_method_t nativeMethod = nullptr; // present only if native

//    struct Parameter {
//        const utf8_t *name = nullptr;
//        u2 accessFlags;
//
//        explicit Parameter(ConstantPool &cp, BytecodeReader &r);
//    };
    std::vector<MethodParameter> parameters;

    /*
     * Each value in the exception_index_table array must be a valid index into
     * the constant_pool table. The constant_pool entry at that index must be a
     * CONSTANT_Class_info structure representing a class type that this
     * method is declared to throw.
     */
    // checked exceptions the method may throw.
    std::vector<u2> checkedExceptions;

    std::vector<std::vector<Annotation>> rtVisiParaAnnos;   // runtime visible parameter annotations
    std::vector<std::vector<Annotation>> rtInvisiParaAnnos; // runtime invisible parameter annotations

    std::vector<Annotation> rtVisiAnnos;   // runtime visible annotations
    std::vector<Annotation> rtInvisiAnnos; // runtime invisible annotations

    ElementValue annotationDefault;
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
        return !isPrivate() && !isStatic() && !utf8::equals(name, S(object_init));
    }

    static u2 calArgsSlotsCount(const utf8_t *descriptor, bool isStatic);

    Array *getParameterTypes();
    Class *getReturnType();

    /*
     * return the type of the method,
     * Object of java/lang/invoke/MethodType.
     */
    Object *getType();
    Array *getExceptionTypes();

    jint getLineNumber(int pc) const;

    /*
     * @pc, 发生异常的位置
     */
    int findExceptionHandler(Class *exception_type, size_t pc);

    std::string toString() const;

    bool isPublic() const       { return Modifier::isPublic(modifiers); }
    bool isProtected() const    { return Modifier::isProtected(modifiers); }
    bool isPrivate() const      { return Modifier::isPrivate(modifiers); }
    bool isAbstract() const     { return Modifier::isAbstract(modifiers); }
    bool isStatic() const       { return Modifier::isStatic(modifiers); }
    bool isFinal() const        { return Modifier::isFinal(modifiers); }
    bool isSynchronized() const { return Modifier::isSynchronized(modifiers); }
    bool isNative() const       { return Modifier::isNative(modifiers); }
    bool isStrict() const       { return Modifier::isStrict(modifiers); }
    bool isVarargs() const      { return Modifier::isVarargs(modifiers); }

    void setSynthetic() { Modifier::setSynthetic(modifiers); }

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
