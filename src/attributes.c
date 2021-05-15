#include "cabin.h"
#include "attributes.h"
#include "constants.h"

void annotation_init(Annotation *anno, BytecodeReader *r)
{
    assert(anno != NULL && r != NULL);

    anno->type_index = bcr_readu2(r);
    anno->element_value_pairs_count = bcr_readu2(r);
    BUILD_ARRAY(anno->element_value_pairs, anno->element_value_pairs_count, element_value_pair_init, r);
}    

void element_value_init(ElementValue *ev, BytecodeReader *r)
{
    ev->tag = bcr_readu1(r);
    switch (ev->tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'S':
        case 'Z':
        case 's':
            ev->const_value_index = bcr_readu2(r);
            break;
        case 'c':
            ev->class_info_index = bcr_readu2(r);
            break;
        case 'e':
            ev->enum_const_value.type_name_index = bcr_readu2(r);
            ev->enum_const_value.const_name_index = bcr_readu2(r);
            break;
        case '@':
            annotation_init(&ev->annotation_value, r);
            break;
        case '[': 
            ev->array_value.len = bcr_readu2(r);
            BUILD_ARRAY(ev->array_value.value, ev->array_value.len, element_value_init, r);
            break;
        default:
            raise_exception(S(java_lang_UnknownError), NULL); // todo msg
            // throw java_lang_UnknownError("unknown tag: "+ std::to_string(ev->tag));
    }
}

void element_value_pair_init(ElementValuePair *pair, BytecodeReader *r)
{
    pair->element_name_index = bcr_readu2(r);
    element_value_init(&pair->value, r);
}

void inner_class_init(InnerClass *ic, BytecodeReader *r)
{
    ic->inner_class_info_index = bcr_readu2(r);
    ic->outer_class_info_index = bcr_readu2(r);
    ic->inner_name_index = bcr_readu2(r);
    ic->inner_class_access_flags = bcr_readu2(r);
}

void bootstrap_method_init(BootstrapMethod *bm, BytecodeReader *r)
{
    bm->bootstrap_method_ref = bcr_readu2(r);
    bm->args_count = bcr_readu2(r);
    bm->args = vm_malloc(bm->args_count * sizeof(*(bm->args)));
    for (u2 i = 0; i < bm->args_count; i++) {
        bm->args[i] = bcr_readu2(r);
    }
}

slot_t *resolve_bootstrap_method_args(BootstrapMethod *bm, ConstantPool *cp, slot_t *result)
{
    assert(bm != NULL && cp != NULL && result != NULL);

    for (u2 k = 0; k < bm->args_count; k++) {
        u2 i = bm->args[k];
        switch (cp_get_type(cp, i)) {
            case JVM_CONSTANT_String:
                slot_set_ref(result, resolve_string(cp, i));
                result++;
                break;
            case JVM_CONSTANT_Class:
                slot_set_ref(result, resolve_class(cp, i)->java_mirror);
                result++;
                break;
            case JVM_CONSTANT_Integer:
                slot_set_int(result, cp_get_int(cp, i));
                result++;
                break;
            case JVM_CONSTANT_Float:
                slot_set_float(result, cp_get_float(cp, i));
                result++;
                break;
            case JVM_CONSTANT_Long:
                slot_set_long(result, cp_get_long(cp, i));
                result += 2;
                break;
            case JVM_CONSTANT_Double:
                slot_set_double(result, cp_get_double(cp, i));
                result += 2;
                break;
            case JVM_CONSTANT_MethodHandle:
                slot_set_ref(result, resolve_method_handle(cp, i));
                result++;
                break;
            case JVM_CONSTANT_MethodType:
                slot_set_ref(result, resolve_method_type(cp, i));
                result++;
                break;
            default:
                JVM_PANIC("never goes here, wrong type."); // todo
        }
    }

    return result;
}

void line_number_table_init(LineNumberTable *t, BytecodeReader *r)
{
    t->start_pc = bcr_readu2(r);
    t->line_number = bcr_readu2(r);
}

void method_parameter_init(MethodParameter *mp, ConstantPool *cp, BytecodeReader *r)
{
    u2 name_index = bcr_readu2(r);
    // If the value of the name_index item is zero,
    // then this parameters element indicates a formal parameter with no name.
    if (name_index > 0) {
        mp->name = cp_utf8(cp, name_index);
    } else {
        mp->name = NULL;
    }
    mp->access_flags = bcr_readu2(r);
}

void local_variable_table_init(LocalVariableTable *t, BytecodeReader *r)
{
    t->start_pc = bcr_readu2(r);
    t->length = bcr_readu2(r);
    t->name_index = bcr_readu2(r);
    t->descriptor_index = bcr_readu2(r);
    t->index = bcr_readu2(r);
}

void local_variable_type_table_init(LocalVariableTypeTable *t, BytecodeReader *r)
{
    t->start_pc = bcr_readu2(r);
    t->length = bcr_readu2(r);
    t->name_index = bcr_readu2(r);
    t->signature_index = bcr_readu2(r);
    t->index = bcr_readu2(r);
}

static void module_require_init(struct module_require *require, ConstantPool *cp, BytecodeReader *r)
{
    require->require_module_name = cp_module_name(cp, bcr_readu2(r));
    require->flags = bcr_readu2(r);
    u2 v = bcr_readu2(r);
    require->version = v == 0 ? NULL : cp_utf8(cp, v);
}

static void module_export_init(struct module_export *export, ConstantPool *cp, BytecodeReader *r)
{
    export->export_package_name = cp_package_name(cp, bcr_readu2(r));
    export->flags = bcr_readu2(r);
    u2 n = export->exports_to_count = bcr_readu2(r);
    BUILD_ARRAY0(export->exports_to, n, cp_module_name(cp, bcr_readu2(r)));
};

static void module_open_init(struct module_open *open, ConstantPool *cp, BytecodeReader *r)
{
    open->open_package_name = cp_package_name(cp, bcr_readu2(r));
    open->flags = bcr_readu2(r);
    u2 n = open->opens_to_count = bcr_readu2(r);
    BUILD_ARRAY0(open->opens_to, n, cp_module_name(cp, bcr_readu2(r)));
};

static void module_provide_init(struct module_provide *provide, ConstantPool *cp, BytecodeReader *r)
{
    provide->class_name = cp_class_name(cp, bcr_readu2(r));
    u2 n = provide->provides_with_count = bcr_readu2(r);
    BUILD_ARRAY0(provide->provides_with, n, cp_class_name(cp, bcr_readu2(r)));
}

Module *module_create(ConstantPool *cp, BytecodeReader *r)
{
    Module *module = vm_malloc(sizeof(Module));

    module->module_name = cp_module_name(cp, bcr_readu2(r));
    module->module_flags = bcr_readu2(r);
    u2 v = bcr_readu2(r);
    module->module_version = v == 0 ? NULL : cp_utf8(cp, v);

    module->requires_count = bcr_readu2(r);
    BUILD_ARRAY(module->requires, module->requires_count, module_require_init, cp, r);

    module->exports_count = bcr_readu2(r);
    BUILD_ARRAY(module->exports, module->exports_count, module_export_init, cp, r);

    module->opens_count = bcr_readu2(r);
    BUILD_ARRAY(module->opens, module->opens_count, module_open_init, cp, r);

    module->uses_count = bcr_readu2(r);
    BUILD_ARRAY0(module->uses, module->uses_count, cp_class_name(cp, bcr_readu2(r)));

    module->provides_count = bcr_readu2(r);
    BUILD_ARRAY(module->provides, module->provides_count, module_provide_init, cp, r);

    return module;
}