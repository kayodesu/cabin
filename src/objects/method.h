/*
 * Author: kayo
 */

#ifndef KAYO_METHOD_H
#define KAYO_METHOD_H

#include <vector>
#include "Modifier.h"
#include "../classfile/attributes.h"
#include "../native/registry.h"
#include "../util/encoding.h"

class Object;
class Array;
class Class;

class Method {
//    Array *parameterTypes = nullptr; // [Ljava/lang/Class;
//    Class *returnType = nullptr;     // Ljava/lang/Class;
    Object *type_obj = nullptr;          // Ljava/lang/invoke/MethodType;
    Array *exceptionTypes = nullptr; // [Ljava/lang/Class;

public:
    // 定义此 Method 的类
    Class *clazz;
    const utf8_t *name = nullptr;
    const utf8_t *type = nullptr;

    jint modifiers;

    bool deprecated = false;
    const char *signature = nullptr;

    int vtableIndex = -1;
    int itableIndex = -1;

    u2 maxStack = 0;
    u2 maxLocals = 0;
    u2 arg_slot_count = 0;

    u1 *code = nullptr;
    size_t codeLen = 0;

    native_method_t nativeMethod = nullptr; // present only if native

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
    std::vector<LineNumberTable> line_number_tables;
    std::vector<LocalVariableTable> local_variable_tables;
    std::vector<LocalVariableTypeTable> local_variable_type_tables;;

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
    [[nodiscard]] bool isVirtual() const
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

    [[nodiscard]] jint getLineNumber(int pc) const;

    /*
     * @pc, 发生异常的位置
     */
    int findExceptionHandler(Class *exception_type, size_t pc);

    [[nodiscard]] std::string toString() const;

    [[nodiscard]] bool isPublic() const       { return Modifier::isPublic(modifiers); }
    [[nodiscard]] bool isProtected() const    { return Modifier::isProtected(modifiers); }
    [[nodiscard]] bool isPrivate() const      { return Modifier::isPrivate(modifiers); }
    [[nodiscard]] bool isAbstract() const     { return Modifier::isAbstract(modifiers); }
    [[nodiscard]] bool isStatic() const       { return Modifier::isStatic(modifiers); }
    [[nodiscard]] bool isFinal() const        { return Modifier::isFinal(modifiers); }
    [[nodiscard]] bool isSynchronized() const { return Modifier::isSynchronized(modifiers); }
    [[nodiscard]] bool isNative() const       { return Modifier::isNative(modifiers); }
    [[nodiscard]] bool isStrict() const       { return Modifier::isStrict(modifiers); }
    [[nodiscard]] bool isVarargs() const      { return Modifier::isVarargs(modifiers); }

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
        if (isNative()) {
            delete[] code;
        }
        for (auto &t : exceptionTables) {
            delete t.catchType;
        }
    }
};


#endif //KAYO_METHOD_H
