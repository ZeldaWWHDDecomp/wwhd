// Water-level schedule tag, WWHD actor layout.
#pragma once
#include "gc_api/gc_api.h"
namespace daTagWaterlevel {
enum State_e { STATE_1=1, STATE_2=2, STATE_4=4 };
class Act_c : public fopAc_ac_c {
public:
    Act_c() { fopAc_ac_c_ct(this); __vtbl = 0x1003FD28; } // HD: virtual destructor

    enum Prm_e { PRM_SCH_W = 8, PRM_SCH_S = 0 };
    u8 prm_get_sch();
    cPhs_State _create();
    void bgm_proc();
    bool _execute();

    /* 0x3AC */ be<f32> field_0x290;
    /* 0x3B0 */ be<s32> mAction;
};
WWHD_OFFSET(Act_c, field_0x290, 0x3AC);
WWHD_SIZE(Act_c, 0x3B4);

}
u32 dKy_get_schbit();
f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
void mDoAud_stWaterLevelUp();
void mDoAud_stWaterLevelDown();
/* dComIfGp_event_chkEventFlag: gc_api/d/d_com_inf_game.h */
