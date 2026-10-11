/* Timer actor, WWHD members +0x11C from GameCube. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjTimer {
class Act_c : public fopAc_ac_c {
public:
    Act_c();
    enum : u32 { __vt = 0x10031028 };
    typedef void (Act_c::*ModeFunc)();
    struct ModeEntry { gabi::ptmf<ModeFunc> proc; };
    enum Mode_e { Mode_WAIT_e, Mode_COUNT_e };
    enum Prm_e { PRM_TIME_W = 8, PRM_TIME_S = 0, PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 16 };
    s32 prm_get_swSave() const;
    s32 prm_get_time() const;
    void mode_count_init();
    void mode_wait_init();
    cPhs_State _create();
    bool _execute();
    void mode_wait();
    void mode_count();
    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<u8> mIsStop;
    /* 0x3B5 */ u8 _3B5[3];
};
WWHD_OFFSET(Act_c, mMode, 0x3AC);
WWHD_OFFSET(Act_c, mTimer, 0x3B0);
WWHD_OFFSET(Act_c, mIsStop, 0x3B4);
WWHD_SIZE(Act_c, 0x3B8);
}
void mDoAud_seStart(u32);
enum {
    JA_SE_SYS_EV_TIMER_20 = 0x861,
    JA_SE_SYS_EV_TIMER_10 = 0x862,
    JA_SE_SYS_EV_TIMER_5 = 0x863,
    JA_SE_SYS_EV_TIMER_0 = 0x864,
};
