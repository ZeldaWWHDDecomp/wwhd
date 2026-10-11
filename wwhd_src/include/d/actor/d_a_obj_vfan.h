/* Ganon's Tower Phantom Ganon door, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
void mDoAud_seStart(u32);
inline u8 fpcM_CreateResult(base_process_class* p) { return (u8)p->mCreateResult; }
enum { dRes_INDEX_VFAN_BDL_V_FAN_00_e = 4, dRes_INDEX_VFAN_DZB_V_FAN_00_e = 7 };
enum { JA_SE_READ_RIDDLE_1 = 0x806, JA_SE_OBJ_GN_SW_DR_LIGHT = 0x6A20, JA_SE_OBJ_GN_SW_DR_BREAK = 0x6A21 };
namespace dSv_event_flag_c { enum { UNK_3A08 = 0x3A08 }; }
namespace dPa_name {
enum {
    ID_AK_SN_GANONDOORSMOKE00 = 0x83CD,
    ID_AK_SN_GANONDOORFLASH00 = 0x83CE,
    ID_AK_SN_GANONDOORFLASH01 = 0x83CF,
    ID_AK_SN_GANONDOORFLASH02 = 0x83D0,
    ID_AK_SN_GANONDOORFLASH03 = 0x83D1,
    ID_AK_SN_GANONDOORFLASH04 = 0x83D2,
    ID_AK_SN_GANONDOORFLASH05 = 0x83D3,
    ID_AK_SN_GANONDOORFLASH06 = 0x83D4,
    ID_AK_SN_GANONDOORFLASH07 = 0x83D5,
    ID_AK_SN_GANONDOORFLASH08 = 0x83D6,
    ID_AK_SN_GANONDOORFLASH09 = 0x83D7,
};
}
namespace daObjVfan {
struct Act_c : dBgS_MoveBgActor {
    Act_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x10032B30; // HD: actor vtable.
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x10032A78);
    }
    static const char M_arcname[];
    BOOL Delete();
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void ParticleSet();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Cyl mCyl;
    /* 0x558 */ be<s32> mState;
    /* 0x55C */ be<u8> mIsAlive;
    /* 0x55D */ u8 _55D[3];
    /* 0x560 */ be<s32> mBreakTimer;
};
WWHD_OFFSET(Act_c, mStts, 0x3EC);
WWHD_OFFSET(Act_c, mCyl, 0x428);
WWHD_OFFSET(Act_c, mBreakTimer, 0x560);
}  // namespace daObjVfan
using daObjVfan::Act_c;

WWHD_SIZE(daObjVfan::Act_c, 0x564);
