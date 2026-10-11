/* AND switch actor: GameCube fields shifted by the HD actor base (+0x11C). */
#pragma once
#include "gc_api/gc_api.h"

enum {
    ACT_ON_ALL = 0,
    ACT_OFF_ALL,
    ACT_WAIT = 10,
    ACT_TIMER = 20,
    ACT_TIMER2,
    ACT_TIMER_SET = 30,
};

enum {
    fpcNm_BB_e = 0xB5,
    fpcNm_BK_e = 0xBD,
};

struct andsw0_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<s8> mAction;
    /* 0x3B5 */ be<u8> mNumSwitchesToCheck;
    /* 0x3B6 */ be<u8> mBehaviorType;
    /* 0x3B7 */ be<u8> mSwitchToSet;
    /* 0x3B8 */ be<u8> mFirstSwitchToCheck;
    /* 0x3B9 */ u8 _3B9;
    /* 0x3BA */ be<u16> mTimer;
    /* 0x3BC */ be<s16> mEventIdx;
    /* 0x3BE */ be<u8> mEventNo;
    /* 0x3BF */ be<s8> mEventState;
    andsw0_class() { fopAc_ac_c_ct(this); __vtbl = 0x100071AC; }
};
WWHD_OFFSET(andsw0_class, mAction, 0x3B4);
WWHD_OFFSET(andsw0_class, mEventState, 0x3BF);

WWHD_SIZE(andsw0_class, 0x3C0);

// HD: private view of the Bokoblin's one field used by the opening sequence.
struct bk_class : fopEn_enemy_c {
    u8 _3C8[0x137C - 0x3C8];
    /* 0x137C */ be<s8> m121C;
    u8 _137D[0x19A4 - 0x137D];
};
WWHD_OFFSET(bk_class, m121C, 0x137C);
WWHD_SIZE(bk_class, 0x19A4);

namespace dSv_event_flag_c { enum { UNK_0004 = 0x0004, UNK_0301 = 0x0301, UNK_0480 = 0x0480 }; }
inline BOOL fopAcM_IsActor(void* actor) { return fopAc_IsActor(actor); }
inline void fopAcM_onSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_onSwitch(sw, a->home.roomNo); }
inline void fopAcM_offSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_offSwitch(sw, a->home.roomNo); }
// Typed callback overload; the existing shared overload takes a guest address.
using AndswSearchFunc = void* (*)(void*, void*);
void* fpcM_Search(AndswSearchFunc callback, void* data);

struct AndswSearchActors {
    gptr<void> actors[7];
    gptr<void>& operator[](s32 i) { return actors[i]; }
};
WWHD_SIZE(AndswSearchActors, 0x1C);
