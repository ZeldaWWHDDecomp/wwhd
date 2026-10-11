/* Boss heart-container marker, WWHD layout (zeldaret/tww d_a_boss_item.h).
 * HD: the actor base grows from 0x290 to 0x3AC; mPhase remains unused. */
#pragma once
#include "gc_api/gc_api.h"

struct bossitem_class : fopAc_ac_c {
    enum : u32 { __vt = 0x1000B04C }; // HD: virtual destructor table.
    bossitem_class();
    /* 0x3AC */ request_of_phase_process_class mPhase;
};
WWHD_OFFSET(bossitem_class, mPhase, 0x3AC);
WWHD_SIZE(bossitem_class, 0x3B4);

namespace daBossItem_prm {
inline int getStage(bossitem_class* actor) { return fopAcM_GetParam(actor) & 0xFF; }
}
