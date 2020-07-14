/*
 * Author: Yo Ka
 */

#ifndef KAYO_METHOD_H
#define KAYO_METHOD_H

#include <vector>
#include "../classfile/attributes.h"
#include "../util/encoding.h"
#include "../classfile/constants.h"

class Object;
class Array;
class Class;
class ClassObject;

class Method {
    Object *type_obj = nullptr;       // Ljava/lang/invoke/MethodType;
    Array *exception_types = nullptr; // [Ljava/lang/Class;

public:
    enum RetType {
        RET_INVALID, RET_VOID, RET_BYTE, RET_BOOL, RET_CHAR,
        RET_SHORT, RET_INT, RET_FLOAT, RET_LONG, RET_DOUBLE, RET_REFERENCE
    };
    
    Class *clazz = nullptr; // 定义此 Method 的类
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    int accsee_flags;

    bool deprecated = false;
    const char *signature = nullptr;

    int vtable_index = -1;
    int itable_index = -1;

    u2 max_stack = 0;
    u2 max_locals = 0;
    u2 arg_slot_count = 0;

    u1 *code = nullptr;
    size_t code_len = 0;

    void *native_method = nullptr; // present only if native
    RetType ret_type = RET_INVALID;

    std::vector<MethodParameter> parameters;

    /*
     * Each value in the exception_index_table array must be a valid index into
     * the constant_pool table. The constant_pool entry at that index must be a
     * CONSTANT_Class_info structure representing a class type that this
     * method is declared to throw.
     */
    // checked exceptions the method may throw.
    std::vector<u2> checked_exceptions;

    std::vector<std::vector<Annotation>> rt_visi_para_annos;   // runtime visible parameter annotations
    std::vector<std::vector<Annotation>> rt_invisi_para_annos; // runtime invisible parameter annotations

    std::vector<Annotation> rt_visi_annos;   // runtime visible annotations
    std::vector<Annotation> rt_invisi_annos; // runtime invisible annotations

    ElementValue annotation_default;
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

    // is <clinit>?
    bool isClassInit() const 
    {
        return strcmp(name, "<clinit>") == 0;
    }

    // is <init>?
    bool isObjectInit() const 
    {
        return strcmp(name, "<init>") == 0;
    }

    static u2 calArgsSlotsCount(const utf8_t *descriptor, bool isStatic);

    Array *getParameterTypes();
    ClassObject *getReturnType();

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

    [[nodiscard]] bool isPublic() const       { return accIsPublic(accsee_flags); }
    [[nodiscard]] bool isProtected() const    { return accIsProtected(accsee_flags); }
    [[nodiscard]] bool isPrivate() const      { return accIsPrivate(accsee_flags); }
    [[nodiscard]] bool isAbstract() const     { return accIsAbstract(accsee_flags); }
    [[nodiscard]] bool isStatic() const       { return accIsStatic(accsee_flags); }
    [[nodiscard]] bool isFinal() const        { return accIsFinal(accsee_flags); }
    [[nodiscard]] bool isSynchronized() const { return accIsSynchronized(accsee_flags); }
    [[nodiscard]] bool isNative() const       { return accIsNative(accsee_flags); }
    [[nodiscard]] bool isStrict() const       { return accIsStrict(accsee_flags); }
    [[nodiscard]] bool isVarargs() const      { return accIsVarargs(accsee_flags); }
    [[nodiscard]] bool isSynthetic() const    { return accIsSynthetic(accsee_flags); }

    void setSynthetic() { accSetSynthetic(accsee_flags); }

private:
    /*
     * 异常处理表
     * start_pc 给出的是try{}语句块的第一条指令，end_pc 给出的则是try{}语句块的下一条指令。
     * 如果 catch_type 是 NULL（在class文件中是0），表示可以处理所有异常，这是用来实现finally子句的。
     */
    struct ExceptionTable {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;

        struct CatchType {
            bool resolved;
            union {
                Class *clazz; // if resolved
                const char *class_name; // if not resolved
            } u;
        } *catch_type = nullptr;

        ExceptionTable(Class *clazz, BytecodeReader &r);
    };

    std::vector<ExceptionTable> exception_tables;

public:
    ~Method()
    {
        if (isNative()) {
            delete[] code;
        }
        for (auto &t : exception_tables) {
            delete t.catch_type;
        }
    }
};


#endif //KAYO_METHOD_H
