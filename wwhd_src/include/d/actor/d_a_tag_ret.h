#pragma once
#include "gc_api/gc_api.h"

namespace daTagRet {
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_LINK_ID_W = 8, PRM_LINK_ID_S = 0 };
    Act_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1003FC28;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1003FC18);
    }
    cPhs_State _create();
    bool _execute();
    u32 prm_get_linkID();
    /* 0x3AC */ u8 m290[0xC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);
}
