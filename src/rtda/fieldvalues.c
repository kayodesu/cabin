/*
 * Author: Jia Yang
 */

#include "fieldvalues.h"
#include "ma/jfield.h"
#include "ma/jclass.h"

struct fieldvalues* fv_create(struct jclass *jclass, bool static_fields)
{
    assert(jclass != NULL);

    int fields_count = static_fields ? jclass->static_fields_count : jclass->instance_fields_count; // todo == 0 ???

    VM_MALLOC_EXT(struct fieldvalues, 1, fields_count * sizeof(struct slot), fv);
    fv->jclass = jclass;
    fv->fields_count = fields_count;

    // 给个变量赋予默认值           todo 说明默认值
    for (int i = 0; i < jclass->fields_count; i++) {
        struct jfield *field = jclass->fields[i];
        if (static_fields == IS_STATIC(field->access_flags)) {
            switch (field->descriptor[0]) {
                case 'B':
                case 'C':
                case 'I':
                case 'S':
                case 'Z':
                    fv->values[field->id] = islot(0);
                    break;
                case 'F':
                    fv->values[field->id] = fslot(0.0f);
                    break;
                case 'J':
                    fv->values[field->id] = lslot(0L);
                    fv->values[field->id + 1] = phslot();
                    break;
                case 'D':
                    fv->values[field->id] = dslot(0.0);
                    fv->values[field->id + 1] = phslot();
                    break;
                default:
                    fv->values[field->id] = rslot(NULL);
                    break;
            }
        }
    }

    return fv;
}

void fv_detroy(struct fieldvalues *fv)
{

}

void fv_set_by_id(struct fieldvalues *fv, int id, const struct slot *value)
{
    assert(fv != NULL && value != NULL);
    assert(id >= 0 && id < fv->fields_count);
    fv->values[id] = *value;
    if (slot_is_category_two(value)) {
        assert(id + 1 < fv->fields_count);
        fv->values[id + 1] = phslot();
    }
}

void fv_set_by_nt(struct fieldvalues *fv,
                           const char *name, const char *descriptor, const struct slot *value)
{
    assert(fv != NULL && name != NULL && descriptor != NULL && value != NULL);

    struct jfield *f = jclass_lookup_field(fv->jclass, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    fv_set_by_id(fv, f->id, value);
}

const struct slot* fv_get_by_id(const struct fieldvalues *fv, int id)
{
    assert(fv != NULL);
    assert(id >= 0 && id < fv->fields_count);
    return fv->values + id;
}

const struct slot* fv_get_by_nt(const struct fieldvalues *fv, const char *name, const char *descriptor)
{
    assert(fv != NULL && name != NULL && descriptor != NULL);

    struct jfield *f = jclass_lookup_field(fv->jclass, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    return fv_get_by_id(fv, f->id);
}
