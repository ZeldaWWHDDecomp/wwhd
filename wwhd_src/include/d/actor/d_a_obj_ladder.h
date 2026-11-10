/** Drop-down ladder, WWHD layout.
 * HD: the moving-background actor base shifts tww members by 0x118. */
#pragma once
#include "gc_api/gc_api.h"
enum { JA_SE_OBJ_LADDER_FALL_1 = 0x6954, JA_SE_OBJ_LADDER_FALL_2 = 0x6955 };
enum {
    dRes_INDEX_MHSG_BDL_MHSG6_e=7, dRes_INDEX_MHSG_DZB_MHSG6_e=14,
    dRes_INDEX_MHSG_BDL_MHSG9_e=8, dRes_INDEX_MHSG_DZB_MHSG9_e=15,
    dRes_INDEX_MHSG_BDL_MHSG12_e=4, dRes_INDEX_MHSG_DZB_MHSG12_e=11,
    dRes_INDEX_MHSG_BDL_MHSG15_e=5, dRes_INDEX_MHSG_DZB_MHSG15_e=12,
    dRes_INDEX_MHSG_BDL_MHSG4H_e=6, dRes_INDEX_MHSG_DZB_MHSG4H_e=13,
};
namespace daObjLadder {
struct AttrType {
    be<s16> modelId;
    be<s16> dzbId;
    be<f32> height;
};

struct Act_c : dBgS_MoveBgActor {
    enum Mode_e { Mode_WAIT_e, Mode_DEMOREQ_e, Mode_VIB_e, Mode_DROP_e, Mode_FELL_e };
    s32 prm_get_type();
    s32 prm_get_swSave();
    u8 prm_get_evId();

    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void demo_end_reset();
    void mode_wait_init();
    void mode_wait();
    void mode_demoreq_init();
    void mode_demoreq();
    void mode_vib_init();
    void mode_vib();
    void mode_drop_init();
    void mode_drop();
    void mode_fell_init();
    void mode_fell();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* ppMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mType;
    /* 0x3F0 */ be<s32> mMode;
    /* 0x3F4 */ be<s16> mVibTimer;
    /* 0x3F6 */ be<s16> mBounceCount;  /* was unk2DE: mode_drop_init 023698FC sets it to attr field_0x10 (3); mode_drop 023699A0 plays JA_SE_OBJ_LADDER_FALL_1 + shock on the first landing (it == attr field_0x10), FALL_2 with a volume per bounce otherwise, and counts it down per bounce (tww mode_drop) */
    /* 0x3F8 */ be<f32> mGndY;
    /* 0x3FC */ dBgS_ObjGndChk mGndChk;
    /* 0x450 */ be<s16> mVibXAngle;  /* was unk338: mode_vib_init 02369810 0; mode_vib 02369934 adds attr field_0x16 (20000) and sets mVibXOffset = cM_scos(it) * attr field_0x1C */
    /* 0x452 */ be<s16> mVibYAngle;  /* was unk33A: mode_vib_init 02369810 0; mode_vib 02369934 adds attr field_0x18 (0x3CC3) and sets mVibYOffset = cM_scos(it) * attr field_0x20 */
    /* 0x454 */ be<f32> mVibXOffset;
    /* 0x458 */ be<f32> mVibYOffset;
    /* 0x45C */ be<s16> mEventIdx;
    /* 0x45E */ be<u8> mDemoAccepted;  /* was unk346: mode_demoreq 02369830 sets 1 when eventInfo.checkCommandDemoAccrpt(); demo_end_reset 023695C8 waits for dComIfGp_evmng_endCheck(mEventIdx) only while set, then clears it; Create 023693E0 / mode_demoreq_init 0236979C 0 */
    /* 0x45F */ u8 _45F;
};
WWHD_OFFSET(Act_c, mGndChk, 0x3FC);
WWHD_OFFSET(Act_c, mDemoAccepted, 0x45E);
WWHD_SIZE(Act_c, 0x460);
using LadderModeFunc = void (Act_c::*)();
struct LadderModeEntry {
    gabi::ptmf<LadderModeFunc> proc;
};
WWHD_SIZE(AttrType, 8);
WWHD_SIZE(LadderModeEntry, 8);
}  // namespace daObjLadder
