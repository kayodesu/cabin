/*
 * Author: kayo
 */

#ifndef JVM_ATTRIBUTE_H
#define JVM_ATTRIBUTE_H

#include <vector>
#include "../jtypes.h"
#include "../objects/ConstantPool.h"
#include "../util/BytecodeReader.h"

struct ElementValuePair;
class BytecodeReader;

struct Annotation {
    // The value of the type_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure representing a Field descriptor.
    // The Field descriptor denotes the type of the annotation represented  by this annotation structure.
    u2 type_index = 0;
    std::vector<ElementValuePair> elementValuePairs;

    Annotation() = default;

    explicit Annotation(BytecodeReader &r) { read(r); }

    void read(BytecodeReader &r);
};

/*
 The tag item uses a single ASCII character to indicate the type of the value of
 the element-value pair. This determines which item of the value union is in use.

 Interpretation of tag values as types
 tag Item    Type           value Item               constant Type
 ----------------------------------------------------------------------
 B          byte          const_value_index           CONSTANT_Integer
 C          char          const_value_index           CONSTANT_Integer
 D          double        const_value_index           CONSTANT_Double
 F          float         const_value_index           CONSTANT_Float
 I          int           const_value_index           CONSTANT_Integer
 J          long          const_value_index           CONSTANT_Long
 S          short         const_value_index           CONSTANT_Integer
 Z          boolean       const_value_index           CONSTANT_Integer
 s          String        const_value_index           CONSTANT_Utf8
 e          Enum type     enum_const_value            Not applicable
 c          Class         class_info_index            Not applicable
 @          Annotation    type annotation_value       Not applicable
 [          Array type    array_value                 Not applicable
 */
struct ElementValue {
    u1 tag;

    //union {
        u2 const_value_index;
        u2 class_info_index;
        struct {
            u2 type_name_index;
            u2 const_name_index;
        } enum_const_value;

        Annotation annotation_value;

        std::vector<ElementValue> arrayValue;
    //};

    ElementValue() = default;;

    explicit ElementValue(BytecodeReader &r) { read(r); }

    void read(BytecodeReader &r);
};

struct ElementValuePair {
    // The value of the element_name_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    // The constant_pool entry denotes the name of the element of the element-value pair
    // represented by this element_value_pairs entry.
    // In other words, the entry denotes an element of the annotation type specified by type_index.
    u2 element_name_index;
    ElementValue value;

    explicit ElementValuePair(BytecodeReader &r);
};

struct InnerClass {
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
    InnerClass(ConstantPool &cp, BytecodeReader &r);
};

struct BootstrapMethod {
    /*
     * bootstrap_method_ref 项的值必须是一个对常量池的有效索引。
     * 常量池在该索引处的值必须是一个 CONSTANT_MethodHandle_info 结构。
     * 注意：此CONSTANT_MethodHandle_info结构的reference_kind项应为值6（REF_invokeStatic）或8（REF_newInvokeSpecial），
     * 否则在invokedynamic指令解析调用点限定符时，引导方法会执行失败。
     */
    u2 bootstrapMethodRef;

    /*
     * bootstrap_arguments 数组的每个成员必须是一个对常量池的有效索引。
     * 常量池在该索引出必须是下列结构之一：
     * CONSTANT_String_info, CONSTANT_Class_info, CONSTANT_Integer_info, CONSTANT_Long_info,
     * CONSTANT_Float_info, CONSTANT_Double_info, CONSTANT_MethodHandle_info, CONSTANT_MethodType_info。
     */
    std::vector<u2> bootstrapArguments;

    explicit BootstrapMethod(BytecodeReader &r);
    slot_t *resolveArgs(ConstantPool &cp, slot_t *result);
    ~BootstrapMethod();
};

struct LineNumberTable {
    // The value of the start_pc item must indicate the index into the code array
    // at which the code for a new line in the original source file begins.
    // The value of start_pc must be less than the value of the code_length
    // item of the Code attribute of which this LineNumberTable is an attribute.
    u2 start_pc;
    // The value of the line_number item must give the corresponding line number in the original source file.
    u2 line_number;

    explicit LineNumberTable(BytecodeReader &r);
};

struct MethodParameter {
    const utf8_t *name = nullptr;
    u2 accessFlags;

    explicit MethodParameter(ConstantPool &cp, BytecodeReader &r);
};

struct Module {
    const utf8_t *moduleName;
    u2 moduleFlags;
    const utf8_t *moduleVersion;

    struct Require {
        const utf8_t *requireModuleName;
        u2 flags;
        // If requires_version is NULL, then no version information about the current module is present.
        const utf8_t *version;

        explicit Require(ConstantPool &cp, BytecodeReader &r) {
            requireModuleName = cp.moduleName(r.readu2());
            flags = r.readu2();
            u2 v = r.readu2();
            version = v == 0 ? nullptr : cp.utf8(v);
        }
    };
    std::vector<Require> requires;

    struct Export {
        const utf8_t *exportPackageName;
        u2 flags;
        std::vector<const utf8_t *> exports_to;

        explicit Export(ConstantPool &cp, BytecodeReader &r)
        {
            exportPackageName = cp.packageName(r.readu2());
            flags = r.readu2();
            u2 exports_to_count = r.readu2();
            for (u2 i = 0; i < exports_to_count; i++) {
                exports_to.push_back(cp.moduleName(r.readu2()));
            }
        };
    };
    std::vector<Export> exports;

    struct Open {
        const utf8_t *openPackageName;
        u2 flags;
        std::vector<const utf8_t *> opens_to;

        explicit Open(ConstantPool &cp, BytecodeReader &r)
        {
            openPackageName = cp.packageName(r.readu2());
            flags = r.readu2();
            u2 exports_to_count = r.readu2();
            for (u2 i = 0; i < exports_to_count; i++) {
                opens_to.push_back(cp.moduleName(r.readu2()));
            }
        };
    };
    std::vector<Open> opens;

    std::vector<const utf8_t *> uses;

    struct Provide {
        const utf8_t *className;
        std::vector<const utf8_t *> provides_with;

        explicit Provide(ConstantPool &cp, BytecodeReader &r)
        {
            className = cp.className(r.readu2());
            u2 provides_with_count = r.readu2();
            for (u2 i = 0; i < provides_with_count; i++) {
                provides_with.push_back(cp.className(r.readu2()));
            }
        }
    };
    std::vector<Provide> provides;

    explicit Module(ConstantPool &cp, BytecodeReader &r);
};

#endif //JVM_ATTRIBUTE_H
