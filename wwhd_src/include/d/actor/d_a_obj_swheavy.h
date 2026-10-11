#pragma once
// HD: actor base grows from 0x290 to 0x3AC; subsequent tww members move by 0x11C.
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
struct SwheavyBgW : dBgW {
    u8 mBeforeCrr[0xA4];
    be<u32> mCrrFunc;
    u8 mBeforeRide[4];
    be<u32> mRideCallback;
};
WWHD_OFFSET(SwheavyBgW, mCrrFunc, 0xA8);
WWHD_OFFSET(SwheavyBgW, mRideCallback, 0xB0);
namespace daObjSwheavy {
struct Attr_c {
    u8 m00[4];
    be<u32> mFlags;
    be<f32> mVSpring, mVSpeedDecay, mPushVSpeed0;
    be<s16> m14, mMiniPushDelay, mPushDelay;
    u8 mPadding[2];
};
struct Act_c : fopAc_ac_c {
    u8 create_heap();
    void set_mtx();
    void init_mtx();
    void mode_upper_init();
    void mode_lower_init();
    void top_bg_aim_req(f32 target, s16 timer);
    void mode_l_u_init();
    BOOL _delete();
    BOOL _draw();
    void set_push_flag();
    void calc_top_pos();
    void mode_u_l_init();
    void mode_upper();
    void mode_u_l();
    void mode_lower();
    void mode_l_u();
    s32 _create();
    BOOL _execute();
    enum Flag { FLAG_OBEY_SAVE = 1, FLAG_STAY_PRESSED = 2, FLAG_IS_TOGGLE = 4, FLAG_REQ_HEAVY = 8 };
    enum Prm_e { PRM_TYPE_W = 3, PRM_TYPE_S = 24, PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 8 };
    enum : u32 { VTABLE = 0x1002FB4C };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel1;
    /* 0x3B8 */ gptr<J3DModel> mpModel2;
    /* 0x3BC */ gptr<SwheavyBgW> mpBgW1;
    /* 0x3C0 */ gptr<SwheavyBgW> mpBgW2;
    /* 0x3C4 */ Mtx34 mMtx1;
    /* 0x3F4 */ Mtx34 mMtx2;
    /* 0x424 */ be<u32> mType;
    /* 0x428 */ be<u32> mMode;
    /* 0x42C */ be<u8> mRiding;
    /* 0x42D */ be<u8> mPrevRiding;
    /* 0x42E */ be<s16> mMiniPushTimer;
    /* 0x430 */ be<u8> mMiniPushFlg;
    /* 0x431 */ be<u8> mHeavyRiding;
    /* 0x432 */ be<u8> mPrevHeavyRiding;
    /* 0x433 */ u8 mPadding;
    /* 0x434 */ be<s16> mPushTimer;
    /* 0x436 */ be<u8> mPushFlg;
    /* 0x437 */ be<u8> mChangingState;
    /* 0x438 */ be<f32> mTargetHFrac;
    /* 0x43C */ be<f32> mCurHFrac;
    /* 0x440 */ be<f32> mVSpeed;
    /* 0x444 */ be<f32> m328;
    /* 0x448 */ be<f32> m32C;
    /* 0x44C */ be<s16> m330;
    /* 0x44E */ u8 mPadding2[2];
    /* 0x450 */ be<f32> m334;
    /* 0x454 */ be<f32> m338;
    /* 0x458 */ be<f32> mTopPos;
};
WWHD_OFFSET(Act_c, mMtx1, 0x3C4);
WWHD_OFFSET(Act_c, mType, 0x424);
WWHD_OFFSET(Act_c, mTopPos, 0x458);
WWHD_SIZE(Act_c, 0x45C);
} // namespace daObjSwheavy

struct SwheavyAttrTable {
    daObjSwheavy::Attr_c mAttr[4];
};
struct SwheavyModeTable {
    gabi::ptmf<void (daObjSwheavy::Act_c::*)()> mProc[4];
};
struct SwheavyModel {
    u8 mPrefix[0xBC];
    cXyz mBaseScale;
    Mtx34 mBaseTRMtx;
};
struct SwheavyPlayer : fopAc_ac_c {
    u8 mBeforeEquip[0xC];
    be<u32> mEquipFlags;
};
struct SwheavyColorFloat {
    be<f32> r, g, b, a;
};
WWHD_SIZE(daObjSwheavy::Attr_c, 0x1C);
WWHD_OFFSET(SwheavyModel, mBaseTRMtx, 0xC8);
WWHD_OFFSET(SwheavyPlayer, mEquipFlags, 0x3B8);
void Swheavy_assert(const char*, u32, const char*);
SwheavyBgW* Swheavy_newBgW(s32);
BOOL Swheavy_setBg(SwheavyBgW*, cBgD_t*, u32, Mtx34*);
void Swheavy_ZXYrotM(Mtx34*, s16, s16, s16);
void Swheavy_actorCtor(daObjSwheavy::Act_c*);
void Swheavy_actorDtor(daObjSwheavy::Act_c*, s32);
enum { SWHEAVY_PLAYER_PROC = 0xA8, JA_SE_OBJ_FOOT_SW_ON = 0x6930 };

enum {
    dRes_INDEX_HHBOT_BDL_HHBOT1_e = 4,
    dRes_INDEX_HHBOT_BDL_HHBOT2_e = 5,
    dRes_INDEX_HHBOT_DZB_HHBOT1_e = 8,
    dRes_INDEX_HHBOT_DZB_HHBOT2_e = 9
};
