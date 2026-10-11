/**
 * m_Do_hostIO.cpp
 * HostIO - disabled registration entry points.
 *
 * WWHD, verified against the recompiled original (unit m_Do_hostIO).
 */
#include "m_Do/m_Do_hostIO.h"

// HD: adjacent no-op entry points; their individual roles are unconfirmed.
/* 025F0A08 */
static void mDoHIO_disabledEntry() {
}

/* 025F0A0C */
static void mDoHIO_disabledCompanion() {
}

/* 025F0A10 */
s8 mDoHIO_subRoot_c::createChild(const char* i_name, JORReflexible* i_reflexible) {
    // HD: registration is disabled and returns zero instead of searching the child table.
    return 0;
}

/* 025F0A18 */
void mDoHIO_subRoot_c::deleteChild(s8 i_childID) {
    // HD: there is no child table to update.
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
