/**
 * hd_sinit_02035FD8.cpp
 * HD - header static initialization and SafeString companion stubs
 *
 * HD-only translation unit with no GameCube counterpart. Verified function by function
 * against the recompiled original (tools/verify, unit hd_sinit_02035FD8).
 */

#include "bindings.h"

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02035FD8 */
// HD: the common header statics; the pi pair is copied from this translation unit's rodata.
static void __sinit_hd_header_cpp() {
    sinit_header_statics_rodata(0x1020098C, 0x1018F3AC, 0x10200998, 0x1000510C);
}

/* 0203606C */
// HD: sead::SafeStringBase<char>'s deleting destructor, this translation unit's copy.
static void SafeString_dtor(SafeString* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02036080 */
// HD: sead::SafeStringBase<char>::assureTerminationImpl_, empty in this translation unit.
static void SafeString_assureTerminationImpl() {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
