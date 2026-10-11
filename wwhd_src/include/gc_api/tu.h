/* Per-translation-unit data (HD): GHS emits a copy of every vtable and of every out-of-line
 * inline function per translation unit, and inline constructors store the unit's own vtable
 * addresses. A GameCube-style unit defines `gc_tu` once (designated initialisers, only what it
 * uses); gc_api's inline constructors/destructors read it. */
#pragma once
#include "bindings.h"

struct gc_tu_data {
    u32 safeString;               /* sead::SafeString vtable (resource names, material names) */
    u32 safeStringAssure;         /* the TU's SafeString::assureTerminationImpl_ (empty) */
    dBgS_ObjAcch_vt objAcch;      /* dBgS_ObjAcch inline constructor: vtables at +0x10/+0x14/+0x20 */
    u32 objAcchDt[2];             /* ~dBgS_ObjAcch: vtables stored at +0x20, +0x14 */
    dBgS_GndChk_vt gndChk;        /* dBgS_GndChk inline constructor: +0x10/+0x20/+0x40/+0x4C */
    u32 gndChkDt[3];              /* ~dBgS_GndChk: vtables stored at +0x20, +0x40, +0x4C */
    u32 headerStatics[3];         /* __sinit: statics of the common headers: P (pi pair), D (dtor records), Z (zeroed) */
};
extern const gc_tu_data gc_tu;
