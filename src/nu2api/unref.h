#ifndef NU2API_UNREF_H
#define NU2API_UNREF_H

#include "nu2api/nucore/common.h"

/* RefPack decompressor; returns the number of bytes written to destination. */
i32 unref(u8 *source, u8 *destination);

#endif
