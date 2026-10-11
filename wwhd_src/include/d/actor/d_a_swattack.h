// Hit trigger: WWHD actor members shift +0x11C from GameCube.
#pragma once
#include "gc_api/gc_api.h"

class daSwAt_c : public fopAc_ac_c {
public:
    daSwAt_c() {
        // HD: GHS installs this actor and cylinder AABB vtables explicitly.
        fopAc_ac_c_ct(this);
        __vtbl = 0x1003EA94;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1003EA84);
    }
    cPhs_State _create();
    bool _execute();
    void CreateInit();

    /* 0x3AC */ u8 mUnused[0xC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<u8> mAtType;
    /* 0x525 */ u8 mPadding[3];
    /* 0x528 */ be<u32> mSwitchNo;
};
WWHD_OFFSET(daSwAt_c, mStts, 0x3B8);
WWHD_OFFSET(daSwAt_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwAt_c, mAtType, 0x524);
WWHD_OFFSET(daSwAt_c, mSwitchNo, 0x528);
WWHD_SIZE(daSwAt_c, 0x52C);

namespace daSwAt_prm {
inline u8 getAtType(daSwAt_c* ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
inline u8 getSwitchNo(daSwAt_c* ac) { return (fopAcM_GetParam(ac) >> 8) & 0xFF; }
}  // namespace daSwAt_prm

