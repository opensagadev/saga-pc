#include "saga.h"

/* EdRef's constructor (retail edlevelall unit). Kept out of edlevelall.cpp,
 * where EdTrace's empty body is visible and the compiler would remove the
 * size-check call retail still makes. */

EdRef::EdRef(char *type_name, char *member_name, i32 offset, i32 member_size, i32 member_attributes,
             EdControl *member_control, i32 group) {
    char direct_type[32];
    i32 length;
    attributes = 0;
    length = NuStrLen(type_name);
    if (length > 0 && type_name[length - 1] == '*') {
        NuStrCpy(direct_type, type_name);
        attributes |= 0x40000000;
        direct_type[length - 1] = '\0';
        type_name = direct_type;
    }
    type_id = theRegistry.GetTypeId(type_name);
    if (type_id >= 0) {
        EdType *type = theRegistry.GetType(type_id);
        if (type->size < 0 && member_size <= 0)
            EdTrace(type->name);
    } else {
        type_id = theRegistry.GetClassId(type_name);
        attributes |= 0x80000000;
    }
    name = member_name;
    member_offset = offset;
    size = member_size;
    attributes |= member_attributes;
    control = member_control;
    replication_group = group;
}
