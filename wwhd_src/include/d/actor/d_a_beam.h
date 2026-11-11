/* Laser beam actor. GameCube names, WWHD actor base +0x11C and HD animation sizes.
 * HD: member-function descriptors are eight bytes instead of GameCube's twelve.
 * Private shape/emitter views expose only fields this verified translation unit uses. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_cc_d.h"

#include "d/actor/d_a_hot_floor.h"

struct BeamAabHD {
    cXyz mMin, mMax;
    be<u32> __vtbl;
};
WWHD_SIZE(BeamAabHD, 0x1C);
struct BeamCpsGeometryHD {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_SIZE(BeamCpsGeometryHD, 0x20);
struct BeamCpsHD : dCcD_GObjInf {
    /* 0x0F8 */ BeamAabHD mAab;
    /* 0x114 */ be<u32> __vtbl_shape;
    /* 0x118 */ BeamCpsGeometryHD mCps;
};
WWHD_OFFSET(BeamCpsHD, mAab, 0xF8);
WWHD_OFFSET(BeamCpsHD, __vtbl_shape, 0x114);
WWHD_OFFSET(BeamCpsHD, mCps, 0x118);
WWHD_SIZE(BeamCpsHD, 0x138);
struct BeamEmitterHD {
    u8 mPad0[0x5C];
    /* 0x05C */ be<s32> mMaxFrame;
    u8 mPad60[0x1F0 - 0x60];
    /* 0x1F0 */ hot_floor_rotation_c mGlobalRot;
    u8 mPad214[0x220 - 0x214];
    /* 0x220 */ cXyz mGlobalScl;
    /* 0x22C */ cXyz mGlobalTrs;
    /* 0x238 */ cXyz mGlobalPrmScl;
    u8 mPad244[0x254 - 0x244];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(BeamEmitterHD, mGlobalRot, 0x1F0);
WWHD_OFFSET(BeamEmitterHD, mGlobalScl, 0x220);
WWHD_OFFSET(BeamEmitterHD, mGlobalTrs, 0x22C);
WWHD_OFFSET(BeamEmitterHD, mGlobalPrmScl, 0x238);
WWHD_OFFSET(BeamEmitterHD, mFlags, 0x254);
WWHD_SIZE(BeamEmitterHD, 0x258);
struct daBeam_c : fopAc_ac_c {
    using Func = gabi::ptmf<void (daBeam_c::*)()>;
    BOOL CreateHeap();
    void set_mtx();
    s32 CreateInit();
    BOOL _execute();
    BOOL checkRange(csXyz*);
    void move_search();
    void fix_search();
    void wait_proc();
    /* 0x3AC */ dCcD_Stts mStts;
    /* 0x3E8 */ BeamCpsHD mCps1;
    /* 0x520 */ BeamCpsHD mCps2;
    /* 0x658 */ be<u8> mAtHit;  /* was m53C: daBeam_AtHitCallback 02074C98 / daBeam_checkHitCallback 02075078 set 1 and store the hit position in mAtHitPos; _execute 02075988 then ends the beam there (no hot floor) and clears it (tww the same) */
    u8 mPad659[3];
    /* 0x65C */ cXyz mAtHitPos;  /* was m540: daBeam_AtHitCallback 02074C98 / daBeam_checkHitCallback 02075078 copy the AT hit position (mGObjAt.mHitPos); _execute 02075988 shortens the beam to it and copies it into mBeamEndPos (tww the same) */
    /* 0x668 */ be<u8> mSidePush;  /* was m54C: CreateInit 02075384: param bits 24-27 (0 for the searching type); while non-zero the hit on Link (hitDirection, in the AT hit callbacks) sets the AT / TG vectors to the beam's side direction (tww the same) */
    u8 mPad669[3];
    /* 0x66C */ Func mSearchProc;  /* was m550: _execute 02075988 calls it every frame; CreateInit 02075384 sets it; tww setSearchProc(func) stores into it */
    /* 0x674 */ Func mDefaultProc;  /* was m55C: _execute 02075988 restores mSearchProc from it when the switch is off; CreateInit 02075384 sets it; tww setDefaultProc(func) stores into it */
    /* 0x67C */ request_of_phase_process_class mPhase;
    /* 0x684 */ gptr<J3DModel> M_mdl;
    /* 0x688 */ gptr<J3DAnmTransform> M_bck;
    /* 0x68C */ mDoExt_bckAnm mBckAnm;
    /* 0x718 */ be<f32> mBeamFrame;  /* was m588: the beam's grow frame 0..5: move_search 02076260 counts it up while on target, fade (in move_search / wait_proc 02076598) and _execute 02075988 use it; set_mtx 02075124 width (it * (5 - mFadeFrame)) / 50 + 0.5; the BG line length is it * 20 * scale.z (tww the same) */
    /* 0x71C */ gptr<J3DAnmTevRegKey> M_brk;
    /* 0x720 */ mDoExt_brkAnm mBrkAnm;
    /* 0x798 */ be<f32> mFadeFrame;  /* was m5A8: fade (in move_search 02076260 / wait_proc 02076598) counts it up to 4 while mBeamOn and then ends the beam; set_mtx 02075124 narrows the beam by it (tww the same) */
    /* 0x79C */ gptr<J3DAnmTextureSRTKey> M_btk;
    /* 0x7A0 */ mDoExt_btkAnm mBtkAnm;
    /* 0x814 */ Mtx34 mHotFloorMtx;  /* was m5C4: set_mtx 02075124 builds the hot floor matrix (at mBeamEndPos, mHotFloorQuat, Y rotation, z scale mHotFloorScaleZ) into it and gives it to mpHotFloor (mtx_p) (tww the same) */
    /* 0x844 */ be<s16> mBeamOn;  /* was m5F4: move_search 02076260 sets 1 once mBeamFrame reached 5 on target; fade (in move_search / wait_proc 02076598) fades the beam out and clears it; _execute 02075988 sets the AT capsule only while it is 1 (tww the same) */
    /* 0x846 */ be<s16> mCycleTimer;  /* was m5F6: no use in WWHD (CreateInit 02075384 runs move_search for kind 2); tww CreateInit stores a parameter in it for kind 2 and timer_change counts it down: beam on while it is above 0, off below, reloaded with 60 at -30 */
    /* 0x848 */ be<u8> mHFloorParticle;  /* was m5F8: CreateInit 02075384: 1 - param bit 28; _execute 02075988 passes !it as bit 0 of the Hot_Floor parameters; tww checkHFloorParticle / onHFloorParticle / offHFloorParticle use it */
    /* 0x849 */ be<u8> mSmokeParticle;  /* was m5F9: CreateInit 02075384: 1 - param bit 29; _execute 02075988 passes !it as bit 1 of the Hot_Floor parameters; tww checkSmokeParticle / onSmokeParticle / offSmokeParticle use it */
    u8 mPad84A[2];
    /* 0x84C */ dBgS_ObjLinChk mLinChk;
    /* 0x8B8 */ cXyz mBeamEndPos;  /* was m668: _execute 02075988: the beam's end (mAtHitPos, the BG line cross or the line end), where the Hot_Floor is created and JA_SE_OBJ_STATUE_BEAM plays; set_mtx 02075124 places the hot floor there; daBeamDelete 02075924 deletes its sound (tww the same) */
    /* 0x8C4 */ Quaternion mHotFloorQuat;  /* was m674: _execute 02075988: Quaternion_setRotate from the hit polygon's normal; set_mtx 02075124 rotates the hot floor by it; CreateInit 02075384 sets ZeroQuat (tww the same) */
    /* 0x8D4 */ be<u32> mHotFloorId;  /* was m684: _execute 02075988 stores the fopAcM_create id of the Hot_Floor (-1 without one) and looks the actor up by it (tww the same) */
    /* 0x8D8 */ be<f32> mHotFloorScaleZ;  /* was m688: _execute 02075988 sets 1; set_mtx 02075124 scales the hot floor matrix's z by it (tww the same) */
    /* 0x8DC */ gptr<daHot_Floor_c> mpHotFloor;  /* was m68C: _execute 02075988 finds the daHot_Floor_c by mHotFloorId (nullptr while none); set_mtx 02075124 gives it mHotFloorMtx and mbSpawnParticle (tww the same) */
    /* 0x8E0 */ gptr<BeamEmitterHD> mpBeamEmitter;  /* was m690: the ID_AK_SN_LASERROOT00 emitter: move_search 02076260 creates it, move_search / wait_proc 02076598 end it; set_mtx 02075124 places and scales it; tww beamCheck() tests it */
    /* 0x8E4 */ be<f32> mEmitterOffsetZ;  /* was m694: CreateInit 02075384: 0.001 times a global float for the searching type, else 0; set_mtx 02075124 moves the emitter matrix along z by it */
    /* 0x8E8 */ be<s32> mSwitchNo;  /* was m698: CreateInit 02075384: param bits 16-23 (searching type); CreateInit / _execute 02075988 switch to wait_proc while that switch is set (0xFF: none) (tww the same) */
};
WWHD_OFFSET(daBeam_c, mStts, 0x3AC);
WWHD_OFFSET(daBeam_c, mCps1, 0x3E8);
WWHD_OFFSET(daBeam_c, mCps2, 0x520);
WWHD_OFFSET(daBeam_c, mSearchProc, 0x66C);
WWHD_OFFSET(daBeam_c, mDefaultProc, 0x674);
WWHD_OFFSET(daBeam_c, M_mdl, 0x684);
WWHD_OFFSET(daBeam_c, mBckAnm, 0x68C);
WWHD_OFFSET(daBeam_c, mBrkAnm, 0x720);
WWHD_OFFSET(daBeam_c, mBtkAnm, 0x7A0);
WWHD_OFFSET(daBeam_c, mLinChk, 0x84C);
WWHD_OFFSET(daBeam_c, mBeamEndPos, 0x8B8);
WWHD_OFFSET(daBeam_c, mSwitchNo, 0x8E8);
WWHD_SIZE(daBeam_c, 0x8EC);
// HD: the HIO vtable follows the fields rather than preceding them.
struct daBeam_HIO_c {
    /* 0x00 */ be<s8> mNo;
    u8 mPad1[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    u8 mPadA[2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daBeam_HIO_c, 0x10);

// HD: private adapters keep each verified call site's register widths and linkage.
s32 dSv_info_isSwitch(void*, s32, s32);
u32 dRes_control_getResIndexHD(void*, void*, s32);
void Beam_assertHD(void*, s32, void*);
u32 mDoExt_J3DModel_createHD(void*, u32, u32);
s32 mDoExt_bckAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32);
s32 mDoExt_brkAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
s32 mDoExt_btkAnm_initHD(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
BOOL Beam_CreateHeapHD(daBeam_c*);
void cXyz_subHD(const void*, void*, const void*);
f32 VECSquareMagHD(const void*);
f32 sqrtfHD(f32);
void MTXTransHD(void*, f32, f32, f32);
void mDoMtx_ZXYrotMHD(void*, s32, s32, s32);
void mDoMtx_stack_scaleMHD(f32, f32, f32);
void mDoMtx_stack_transMHD(f32, f32, f32);
void JPASetRMtxTVecfromMtxHD(void*, void*, void*);
void mDoMtx_stack_quatMHD(void*);
void mDoMtx_YrotMHD(void*, s32);
void MTXCopyHD(void*, void*);
void mDoAud_seDeleteObjectHD(void*);
void dComIfG_resDeleteHD(void*, void*);
void Beam_set_mtxHD(daBeam_c*);
void mDoExt_btkAnm_entryHD(void*, void*, f32);
void mDoExt_brkAnm_entryHD(void*, void*, f32);
void mDoExt_bckAnm_entryHD(void*, void*, f32);
void mDoExt_modelUpdateHD(void*, s32);
void* operator_newHD(u32);
void BeamHeaderObject_ct(void*);
void BeamFirstHeader_ct(void*);
void BeamSecondHeader_ct(void*);
void* Beam_HIO_ctHD(void*);
void operator_deleteHD(void*);
void dBgS_LinChk_dtHD(void*, s32);
void J3DFrameCtrl_dtHD(void*, s32);
void dCcD_Cps_dtHD(void*, s32);
void dCcD_Stts_dtHD(void*, s32);
void fopAc_ac_dtHD(void*, s32);
s32 dComIfGp_getReverbHD(s32);
void mDoAud_seStartHD(u32, void*, s32, s32);
f32 VECDotProductHD(void*, void*);
s32 fopAcM_IsActorHD(void*);
dComIfG_inf_c* dComIfGp_getHD();
s16 cM_atan2sHD(f32, f32);
BOOL Beam_checkRangeHD(daBeam_c*, csXyz*);
s32 cLib_addCalcAngleSHD(void*, s32, s32, s32, s32);
void cLib_addCalcAngleS2HD(void*, s32, s32, s32);
u32 dPa_control_setHD(void*, s32, s32, void*, void*, void*, s32, void*, s32, void*, void*, void*);
void dCcD_Stts_InitHD(void*, s32, s32, void*);
void dCcD_Cps_SetHD(void*, void*);
void cM3dGLin_SetStartEndHD(void*, void*, void*);
void fopAcM_setCullSizeBoxHD(void*, f32, f32, f32, f32, f32, f32);
void fopAc_ac_ctHD(void*);
void cCcD_Stts_ctHD(void*);
void dCcD_GStts_ctHD(void*);
void dCcD_GObjInf_ctHD(void*);
void cM3dGLin_ctHD(void*);
void mDoExt_bckAnm_ctHD(void*, s32);
void J3DMtxCalcAnm_ctHD(void*);
void mDoExt_brkAnm_ctHD(void*);
void mDoExt_btkAnm_ctHD(void*);
void dBgS_LinChk_ctHD(void*);
s32 dComIfG_resLoadHD(void*, void*);
s32 fopAcM_entrySolidHeapHD(void*, void*, s32);
s32 Beam_CreateInitHD(daBeam_c*);
void mDoExt_baseAnm_playHD(void*);
void cCcS_SetHD(void*, void*);
s32 cXyz_normalizeRS_HSquaredHD(void*);
void dBgS_LinChk_SetHD(void*, void*, void*, void*);
s32 cBgS_LineCrossHD(void*, void*);
u32 cBgS_GetTriPlaHD(void*, u32, u32);
void Quaternion_setRotateHD(void*, void*);
u32 fopAcM_createHD(s32, u32, void*, s32, void*, void*, s32, void*);
u32 fopAcM_SearchHD(void*, void*);
BOOL Beam_executeHD(daBeam_c*);
// HD: virtual entry views used by player-height and collider-hit dispatch.
struct BeamHitMethodsHD {
    u8 mPad[0x3C];
    be<u32> mpHit;
};
struct BeamPlayerMethodsHD {
    u8 mPad[0x24];
    be<u32> mpEyeHeight;
};
WWHD_SIZE(BeamHitMethodsHD, 0x40);
WWHD_SIZE(BeamPlayerMethodsHD, 0x28);
struct BeamBckHD {
    u8 mFrameCtrl[0x10];
    be<u32> mFrameCtrlVtable;
    u8 mCalculator[0x34];
    be<u32> mCalculatorVtable;
    u8 mPad4C[0x58 - 0x4C];
    be<u32> mAnmTransform;
    u8 mPad5C[0x7C - 0x5C];
    be<u32> mMorf, mAnm, mOldFrame, mOldFrameMorf;
};
WWHD_OFFSET(BeamBckHD, mCalculator, 0x14);
WWHD_OFFSET(BeamBckHD, mCalculatorVtable, 0x48);
WWHD_OFFSET(BeamBckHD, mAnmTransform, 0x58);
WWHD_OFFSET(BeamBckHD, mMorf, 0x7C);
WWHD_SIZE(BeamBckHD, 0x8C);

struct BeamSinCosHD {
    be<f32> mSin, mCos;
};
WWHD_SIZE(BeamSinCosHD, 8);
struct BeamHeaderStaticsHD {
    be<f32> mNegativePi, mPositivePi;
    u8 mFirstObject, mSecondObject, mPadA[2];
    daBeam_HIO_c mHIO;
    be<u32> mZero[4];
};
WWHD_OFFSET(BeamHeaderStaticsHD, mHIO, 0xC);
WWHD_OFFSET(BeamHeaderStaticsHD, mZero, 0x1C);
WWHD_SIZE(BeamHeaderStaticsHD, 0x2C);

// Resource enum from the GameCube archive; its generated res header is absent here.
enum {
    dRes_INDEX_YLESR00_BCK_YLESR00_e = 6,
    dRes_INDEX_YLESR00_BMD_YLESR00_e = 10,
    dRes_INDEX_YLESR00_BRK_YLESR00_e = 14,
    dRes_INDEX_YLESR00_BTK_YLESR00_e = 18,
};

// HD process IDs differ from the GameCube build's values.
enum BeamNamesHD {
    fpcNm_PLAYER_e = 0xA8,
    fpcNm_AM2_e = 0xCB,
    fpcNm_Hot_Floor_e = 0xE7,
    JA_SE_LK_BEAM_HIT = 0x2853,
    JA_SE_OBJ_STATUE_BEAM = 0x5067,
    ID_AK_SN_LASERROOT00 = 0x8121,
};
