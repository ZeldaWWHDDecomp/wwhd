/* Light-sensitive wall, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
// HD: opaque brk storage in the current shared API; typed frame control and existing bindings.
struct MkiekBrkAnm : mDoExt_baseAnm {
    u8 tail[0x68];
    void construct() { gabi::call(0x025E80D0, this); }
    BOOL init(J3DModelData* data, void* key, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry) {
        return gabi::call<BOOL>(0x025E8154, this, data, key, play, mode, rate, start, end, modify, entry);
    }
    bool isStop() { return mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mFrameCtrl.getRate() == 0.0f; }
    void entry(J3DModelData* data, f32 frame) { mDoExt_brkAnm_entry((mDoExt_brkAnm*)this, data, frame); }
};
WWHD_SIZE(MkiekBrkAnm, 0x78);
inline void dCcD_Sph_ct(dCcD_Sph* p) { gabi::call(0x025166F0, p); }
inline void __dt(dCcD_Sph* p) { gabi::call(0x02515AE8, p, 2); }
inline u8 fpcM_CreateResult(base_process_class* p) { return (u8)p->mCreateResult; }
struct MkiekDetect_view {
    BOOL chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, this, pos); }
};
inline MkiekDetect_view& dComIfGp_getDetect() {
    return *(MkiekDetect_view*)&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mDetect;
}
void mDoAud_seStart(u32);
inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
inline void fopAcM_onSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_onSwitch(sw, a->home.roomNo); }
namespace dPa_name { enum { ID_AK_SN_VANISHWALL00 = 0x819F }; }
enum { JA_SE_READ_RIDDLE_1 = 0x806, JA_SE_OBJ_L_OBJ_BRK_TAME = 0x69C2, JA_SE_OBJ_L_WALL_BREAK = 0x6964 };
enum {
    dRes_INDEX_MKIEK_BDL_MKIEK_e = 6,
    dRes_INDEX_MKIEK_BDL_YLSMK00_e = 9,
    dRes_INDEX_MKIEK_BRK_YLSMK00_e = 0xC,
    dRes_INDEX_MKIEK_DZB_MKIEK_e = 0xF,
};
struct MkiekModel_view : J3DModel {
    // HD: capture all loads before stores, including overlapping actor/model storage.
    void setBaseScale(const cXyz& scale) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        mBaseScale.x = x; mBaseScale.y = y; mBaseScale.z = z;
    }
};
namespace daObjMkiek {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0, PRM_SOUND_W = 1, PRM_SOUND_S = 8 };
    enum State_e { STATE_0, STATE_1, STATE_2 };
    Act_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1002C888;
        mBrkAnm.construct();
        dCcD_Stts_ct(&mStts);
        dCcD_Sph_ct(&mSph);
    }
    static const char M_arcname[];
    s32 prm_get_sound();
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void check();
    void demo_wait();
    void demo();
    BOOL Execute(Mtx34** o_mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ gptr<J3DModel> mpModelV;
    /* 0x3F0 */ MkiekBrkAnm mBrkAnm; // HD: brk storage occupies 0x78 bytes here.
    /* 0x468 */ dCcD_Stts mStts;
    /* 0x4A4 */ dCcD_Sph mSph;
    /* 0x5D0 */ be<u8> m458;
    /* 0x5D1 */ u8 _5D1;
    /* 0x5D2 */ be<s16> mDieEventIdx;
    /* 0x5D4 */ be<u32> mState;
    /* 0x5D8 */ be<s32> m460;
};
WWHD_OFFSET(Act_c, mStts, 0x468);
WWHD_OFFSET(Act_c, mSph, 0x4A4);
WWHD_OFFSET(Act_c, m460, 0x5D8);
WWHD_SIZE(Act_c, 0x5DC);
}  // namespace daObjMkiek
