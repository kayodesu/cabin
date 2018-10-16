/*
 * Author: Jia Yang
 */

#include "fieldvalues.h"
#include "ma/jfield.h"
#include "ma/jclass.h"

struct fieldvalues* fv_create(struct jclass *jclass, int fields_count)
{
    assert(jclass != NULL);
    assert(fields_count >= 0);// todo == 0 ???

    VM_MALLOC_EXT(struct fieldvalues, 1, fields_count * sizeof(struct slot), fv);
    fv->jclass = jclass;
    fv->fields_count = fields_count;

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
