// Event switch tag: WWHD actor members shift +0x11C from GameCube.
#pragma once
#include "gc_api/gc_api.h"

namespace daTagEvsw {
class Act_c : public fopAc_ac_c {
public:
    Act_c() {
        // HD: GHS installs the actor and cylinder AABB vtables explicitly.
        fopAc_ac_c_ct(this);
        __vtbl = 0x1003F0C0;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1003F0B0);
    }
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_EVENTBITID_W = 16,
        PRM_EVENTBITID_S = 8,
        PRM_TYPE_W = 2,
        PRM_TYPE_S = 24,
    };
    int prm_get_swSave();
    u16 prm_get_eventbitID();
    u16 prm_get_Type();

    cPhs_State _create();
    bool _delete() { return true; }
    bool _execute();
    bool _draw() { return true; }

    /* 0x3AC */ u8 mUnused[0xC];  /* GameCube 0x290 */
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);
}  // namespace daTagEvsw
