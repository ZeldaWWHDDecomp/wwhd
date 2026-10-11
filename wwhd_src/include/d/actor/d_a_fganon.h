/* fganon_class (Phantom Ganon), WWHD layout. */
#ifndef WWHD_D_A_FGANON_H
#define WWHD_D_A_FGANON_H
#include "bindings.h"
#include "gc_api/tu.h"

// HD: tww actor members are shifted by +0x11C through mKenTevStr, then +0x234.
// HD adds the previous-position and node-rotation fields at the end.
struct fganon_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs1;
    /* 0x3D0 */ request_of_phase_process_class mPhs2;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ be<u8> m2BD;
    /* 0x3DA */ be<u8> mSwitchNo;
    /* 0x3DB */ be<u8> m2BF;
    /* 0x3DC */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3E0 */ gptr<mDoExt_baseAnm> mpBrkAnm1;
    /* 0x3E4 */ gptr<J3DModel> mpKenModel;
    /* 0x3E8 */ gptr<mDoExt_baseAnm> mpBrkAnm2;
    /* 0x3EC */ be<s8> m2D0;
    /* 0x3ED */ be<u8> m2D1[0x2D4 - 0x2D1];
    /* 0x3F0 */ dKy_tevstr_c mKenTevStr;
    /* 0x5B8 */ be<s16> m384;
    /* 0x5BA */ be<s16> mAction;
    /* 0x5BC */ be<s16> mMode;
    /* 0x5BE */ be<s16> m38A;
    /* 0x5C0 */ cXyz m38C;
    /* 0x5CC */ be<s16> m398;
    /* 0x5CE */ be<s16> m39A;
    /* 0x5D0 */ be<f32> m39C;
    /* 0x5D4 */ be<f32> m3A0;
    /* 0x5D8 */ be<s16> m3A4[5];
    /* 0x5E2 */ be<s16> m3AE;
    /* 0x5E4 */ be<f32> m3B0;
    /* 0x5E8 */ be<s16> m3B4;
    /* 0x5EA */ be<s16> m3B6;
    /* 0x5EC */ be<s16> m3B8;
    /* 0x5EE */ be<u8> m3BA[0x3BC - 0x3BA];
    /* 0x5F0 */ be<f32> m3BC;
    /* 0x5F4 */ be<f32> m3C0;
    /* 0x5F8 */ gptr<void> mEmitters1[2];
    /* 0x600 */ gptr<void> mEmitters2[2];
    /* 0x608 */ gptr<void> mEmitters3[2];
    /* 0x610 */ be<u32> m3DC;
    /* 0x614 */ cXyz m3E0;
    /* 0x620 */ cXyz m3EC;
    /* 0x62C */ cXyz m3F8;
    /* 0x638 */ be<f32> m404;
    /* 0x63C */ be<s8> m408;
    /* 0x63D */ be<s8> m409;
    /* 0x63E */ be<s8> m40A;
    /* 0x63F */ be<s8> m40B;
    /* 0x640 */ dCcD_Sph mBallTgSph;
    /* 0x76C */ dCcD_Sph mBallAtSph;
    /* 0x898 */ cXyz m664;
    /* 0x8A4 */ be<s8> m670;
    /* 0x8A5 */ be<s8> m671;
    /* 0x8A6 */ be<u8> m672;
    /* 0x8A7 */ be<s8> m673;
    /* 0x8A8 */ gptr<J3DModel> mpEnergySphereModel;
    /* 0x8AC */ gptr<mDoExt_baseAnm> mpBtkAnm;
    /* 0x8B0 */ gptr<mDoExt_baseAnm> mpBrkAnm3;
    /* 0x8B4 */ be<f32> m680;
    /* 0x8B8 */ be<s8> m684;
    /* 0x8B9 */ be<s8> m685;
    /* 0x8BA */ be<s8> m686;
    /* 0x8BB */ be<s8> m687;
    /* 0x8BC */ be<s8> m688;
    /* 0x8BD */ be<s8> m689;
    /* 0x8BE */ be<s8> m68A;
    /* 0x8BF */ be<s8> m68B;
    /* 0x8C0 */ be<s8> m68C;
    /* 0x8C1 */ be<s8> mbIsMaterialized;
    /* 0x8C2 */ be<s8> m68E;
    /* 0x8C3 */ be<u8> m68F;
    /* 0x8C4 */ be<s8> m690;
    /* 0x8C5 */ be<u8> m691[0x694 - 0x691];
    /* 0x8C8 */ be<f32> m694;
    /* 0x8CC */ be<f32> m698;
    /* 0x8D0 */ be<f32> m69C;
    /* 0x8D4 */ be<s16> m6A0;
    /* 0x8D6 */ be<s16> m6A2;
    /* 0x8D8 */ be<s16> m6A4;
    /* 0x8DA */ be<s16> m6A6;
    /* 0x8DC */ be<u32> m6A8;
    /* 0x8E0 */ be<s8> m6AC;
    /* 0x8E1 */ be<u8> m6AD[0x6B0 - 0x6AD];
    /* 0x8E4 */ be<u32> mBokoID;
    /* 0x8E8 */ be<u32> mCapeID;
    /* 0x8EC */ dBgS_AcchCir mAcchCir;
    /* 0x92C */ dBgS_ObjAcch mAcch;
    /* 0xAF0 */ dCcD_Stts mStts;
    /* 0xB2C */ dCcD_Cyl mCyl;
    /* 0xC5C */ dCcD_Sph mWeponSph;
    /* 0xD88 */ be<s16> mB54;
    /* 0xD8A */ be<s16> mB56;
    /* 0xD8C */ be<u8> mB58[0xB5C - 0xB58];
    /* 0xD90 */ cXyz mB5C;
    /* 0xD9C */ cXyz mB68;
    /* 0xDA8 */ be<s16> mB74;
    /* 0xDAA */ be<s16> mB76;
    /* 0xDAC */ be<u8> mB78[0xB80 - 0xB78];
    /* 0xDB4 */ be<f32> mB80;
    /* 0xDB8 */ be<f32> mB84;
    /* 0xDBC */ be<u8> mB88;
    /* 0xDBD */ be<s8> mB89;
    /* 0xDBE */ be<u8> mB8A;
    /* 0xDBF */ be<u8> mB8B;
    // HD: previous rendered position drives the added joint rotation callback.
    /* 0xDC0 */ cXyz mHdPreviousPos;
    /* 0xDCC */ be<s16> mHdNodeY;
    /* 0xDCE */ be<s16> mHdNodeZ;
};
WWHD_SIZE(fganon_class, 0xDD0);
WWHD_OFFSET(fganon_class, mpMorf, 0x3DC);
WWHD_OFFSET(fganon_class, mMode, 0x5BC);
WWHD_OFFSET(fganon_class, mWeponSph, 0xC5C);

// HD: HIO data starts before its vtable; GC member names retain their original suffixes.
struct daFganon_HIO_c {
    be<s8> mNo;
    be<u8> m05;
    be<u8> m06;
    be<u8> m07;
    be<s16> m08;
    be<s16> m0A;
    be<f32> m0C;
    be<f32> m10;
    be<f32> m14;
    be<f32> m18;
    be<f32> m1C;
    be<f32> m20;
    be<f32> m24;
    be<f32> m28;
    be<f32> m2C;
    be<f32> m30;
    be<f32> m34;
    be<f32> m38;
    be<s16> m3C;
    be<s16> m3E;
    be<s16> m40;
    be<s16> m42;
    be<f32> m44;
    be<s16> m48;
    u8 _pad46[2];
    be<u32> __vtbl;
};
WWHD_SIZE(daFganon_HIO_c, 0x4C);
struct FganonDebugRegisters {
    u8 _pad[8];
    be<f32> f[30];
    be<s16> s[8];
};
WWHD_SIZE(FganonDebugRegisters, 0x90);

template <class T> inline T *fgView(u32 address) {
    return gabi::at<T>(address);
}
struct FganonModelFlags {
    u8 _pad[0x74];
    be<u32> mFlags;
};
struct FganonJointInfo {
    u8 _pad[4];
    be<u16> mJointNo;
};
struct FganonEmitter {
    u8 _pad0[0x5C];
    be<s32> mMaxFrame;
    u8 _pad60[0x1F0 - 0x60];
    be<f32> mGlobalRotation[9];
    u8 _pad214[0x22C - 0x214];
    cXyz mGlobalTranslation;
    u8 _pad238[0x247 - 0x238];
    be<u8> mGlobalAlpha;
    u8 _pad248[0x254 - 0x248];
    be<u32> mStatus;
    u8 _pad258[0x262 - 0x258];
    be<u8> mParticleCount;
};
struct FganonAttackInfo {
    be<u32> mpObj;
    be<u32> mpActor;
    be<u8> mDamage;
    be<u8> mbDead;
    be<u8> mResultingAttackType;
    u8 _padB[9];
    be<u32> pParticlePos;
    be<u32> mHdResult;
};
WWHD_SIZE(FganonAttackInfo, 0x1C);
struct FganonPlayerDemo {
    u8 _pad[0x420];
    be<u16> mDemoType;
    u8 _pad422[6];
    be<u32> mDemoParam;
    u8 _pad42C[4];
    be<u32> mDemoMode;
};
struct FganonPlayerCut {
    u8 _pad[0x69E8];
    be<u8> mCutType;
};
struct FganonBoko {
    u8 _pad[0x3B4];
    be<u32> mpModel;
    u8 _pad3B8[0x434 - 0x3B8];
    be<u8> mFloorFlg;
};
struct FganonCape {
    u8 _pad[0x362C];
    cXyz m1BE0;
    cXyz m1BEC;
};

struct FganonCameraView {
    u8 _pad[0xDC];
    cXyz mEye;
    cXyz mCenter;
    u8 _padF4[0x248 - 0xF4];
    be<u8> mBody; // HD: camera body follows the view storage.
};
struct FganonSaveView {
    u8 _hdHeader[0x20];
    dSv_info_c mSave;
};
struct FganonSaveEquipment {
    u8 _pad[0x2E];
    be<u8> mSelectEquip;
};
struct FganonJointTree {
    u8 _pad[8];
    be<u16> mJointCount;
};
struct FganonLightDefaults {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mAngles[4];
    be<f32> mFactors[8];
};
struct FganonHeaderZeros {
    be<u32> values[4];
};
struct FganonHeaderPi {
    be<f32> values[2];
};
struct FganonPlayerVirtuals {
    u8 _pad[0x64];
    be<u32> checkBottleSwing;
    u8 _pad68[0x114 - 0x68];
    be<u32> setPlayerPosAndAngle;
};
inline be<u32> &fgVectorWord(cXyz *v, u32 i) {
    return *fgView<be<u32>>(gabi::ea(v) + i * 4);
}
inline be<u16> &fgAngleWord(csXyz *v, u32 i) {
    return *fgView<be<u16>>(gabi::ea(v) + i * 2);
}
inline be<f32> &fgVectorFloat(cXyz *v, u32 i) {
    return *fgView<be<f32>>(gabi::ea(v) + i * 4);
}
inline be<f32> &fgMatrixFloat(u32 m, u32 i) {
    return fgView<Mtx34>(m)->m[i / 4][i % 4];
}
inline gptr<void> &fgEmitterAt(fganon_class *a, u32 i) {
    if (i < 2)
        return a->mEmitters1[i];
    if (i < 4)
        return a->mEmitters2[i - 2];
    return a->mEmitters3[i - 4];
}

struct FganonCylinderShapeVtables {
    u8 _pad[0x110];
    be<u32> mVtable;
    be<u32> mShapeVtable;
    u8 _pad118[0x130 - 0x118];
};
struct FganonCylinderGeometryVtable {
    u8 _pad[0x12C];
    be<u32> mVtable;
};


// HD views used where the complete external class is unnecessary.
WWHD_OFFSET(FganonModelFlags, mFlags, 0x74);
WWHD_OFFSET(FganonJointInfo, mJointNo, 0x4);
WWHD_OFFSET(FganonEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(FganonEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(FganonEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(FganonEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(FganonEmitter, mStatus, 0x254);
WWHD_OFFSET(FganonEmitter, mParticleCount, 0x262);
WWHD_OFFSET(FganonAttackInfo, pParticlePos, 0x14);
WWHD_OFFSET(FganonAttackInfo, mHdResult, 0x18);
WWHD_OFFSET(FganonPlayerDemo, mDemoType, 0x420);
WWHD_OFFSET(FganonPlayerDemo, mDemoParam, 0x428);
WWHD_OFFSET(FganonPlayerDemo, mDemoMode, 0x430);
WWHD_OFFSET(FganonPlayerCut, mCutType, 0x69E8);
WWHD_OFFSET(FganonBoko, mpModel, 0x3B4);
WWHD_OFFSET(FganonBoko, mFloorFlg, 0x434);
WWHD_OFFSET(FganonCape, m1BE0, 0x362C);
WWHD_OFFSET(FganonCape, m1BEC, 0x3638);
WWHD_OFFSET(FganonCameraView, mEye, 0xDC);
WWHD_OFFSET(FganonCameraView, mCenter, 0xE8);
WWHD_OFFSET(FganonCameraView, mBody, 0x248);
WWHD_OFFSET(FganonSaveView, mSave, 0x20);
WWHD_OFFSET(FganonSaveEquipment, mSelectEquip, 0x2E);
WWHD_OFFSET(FganonJointTree, mJointCount, 0x8);
WWHD_OFFSET(FganonPlayerVirtuals, checkBottleSwing, 0x64);
WWHD_OFFSET(FganonPlayerVirtuals, setPlayerPosAndAngle, 0x114);
WWHD_OFFSET(FganonCylinderShapeVtables, mVtable, 0x110);
WWHD_OFFSET(FganonCylinderShapeVtables, mShapeVtable, 0x114);
WWHD_OFFSET(FganonCylinderGeometryVtable, mVtable, 0x12C);
WWHD_SIZE(FganonLightDefaults, 0x44);
WWHD_SIZE(FganonHeaderZeros, 0x10);
WWHD_SIZE(FganonHeaderPi, 8);

// HD guest ABI bindings used by this unit.
u32 fg_cBgS_LineCross(u32 a0, u32 a1);
f32 fg_cBgS_GroundCross(u32 a0, u32 a1);
void fg_cBgS_LinChk_dtor(u32 a0, u32 a1);
void fg_cBgS_Chk_cBgS_Chk(u32 a0, u32 a1);
void fg_cCcD_Stts_ctor(u32 a0);
void fg_cCcS_Set(u32 a0, u32 a1);
void fg_cLib_addCalc2(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_cLib_addCalc0(u32 a0, f32 a1, f32 a2);
void fg_cLib_addCalcAngleS2(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_MtxTrans(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_MtxPosition(u32 a0, u32 a1);
void fg_cM3dGCir_dtor(u32 a0, u32 a1);
void fg_cM3dGCyl_SetC(u32 a0, u32 a1);
void fg_cM3dGCyl_SetH(u32 a0, f32 a1);
void fg_cM3dGCyl_SetR(u32 a0, f32 a1);
void fg_cM3dGCyl_ctor(u32 a0);
void fg_cM3dGSph_SetR(u32 a0, f32 a1);
void fg_cM3dGSph_SetC(u32 a0, u32 a1);
u32 fg_cM_atan2s(f32 a0, f32 a1);
f32 fg_cM_rndF(f32 a0);
f32 fg_cM_rndFX(f32 a0);
void fg_csXyz_csXyz(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_cXyz___pl(u32 a0, u32 a1, u32 a2);
void fg_cXyz___mi(u32 a0, u32 a1, u32 a2);
void fg_cXyz___ml(u32 a0, u32 a1, f32 a2);
void fg_daFganon_Execute_inlines_demo_camera(u32 a0);
u32 fg_fopAcM_SearchByID(u32 a0);
void fg_JPABaseEmitter_setFlags(u32 a0, u32 a1);
u32 fg_emptyVirtual(u32 a0);
void fg_dBgS_Acch_dtor(u32 a0, u32 a1);
void fg_dBgS_AcchCir_dBgS_AcchCir(u32 a0);
void fg_dBgS_AcchCir_SetWall(u32 a0, f32 a1, f32 a2);
void fg_dBgS_Acch_dBgS_Acch(u32 a0);
void fg_dBgS_Acch_Set(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8);
void fg_dBgS_Acch_CrrPos(u32 a0, u32 a1);
void fg_dBgS_LinChk_Set(u32 a0, u32 a1, u32 a2, u32 a3);
u32 fg_dCam_getBody();
void fg_dCamera_c_SetTypeForce(u32 a0, u32 a1, u32 a2);
void fg_dCamera_c_Stop(u32 a0);
void fg_dCamera_c_Start(u32 a0);
void fg_dCamera_c_Set(u32 a0, u32 a1, u32 a2, f32 a3, u32 a4);
void fg_dCamera_c_SetTrimSize(u32 a0, u32 a1);
void fg_dCcD_Stts_dtor(u32 a0, u32 a1);
void fg_dCcD_Cyl_dtor(u32 a0, u32 a1);
void fg_dCcD_Sph_dtor(u32 a0, u32 a1);
void fg_dCcD_GStts_dCcD_GStts(u32 a0);
void fg_dCcD_GStts_Move(u32 a0);
void fg_dCcD_Stts_Init(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_dCcD_GObjInf_dCcD_GObjInf(u32 a0);
void fg_dCcD_GObjInf_ClrAtHit(u32 a0);
u32 fg_dCcD_GObjInf_ChkAtHit(u32 a0);
u32 fg_dCcD_GObjInf_GetAtHitObj(u32 a0);
u32 fg_dCcD_GObjInf_ChkTgHit(u32 a0);
u32 fg_dCcD_GObjInf_GetTgHitObj(u32 a0);
void fg_dCcD_Cyl_Set(u32 a0, u32 a1);
void fg_dCcD_Sph_ctor(u32 a0);
void fg_dCcD_Sph_Set(u32 a0, u32 a1);
void fg_dCcD_Sph_StartCAt(u32 a0, u32 a1);
void fg_dCcD_Sph_MoveCAt(u32 a0, u32 a1);
u32 fg_at_power_check1(u32 a0);
void fg_at_power_check2(u32 a0);
u32 fg_cc_at_check(u32 a0, u32 a1);
u32 fg_dComIfGp_getGameInfo();
void fg_dComIfGp_getGameInfoDiscard();
u32 fg_dComIfG_resLoad(u32 a0, u32 a1);
void fg_dComIfG_resDelete(u32 a0, u32 a1);
u32 fg_dComIfGp_getReverb(u32 a0);
u32 fg_dComIfGs_checkGetItem(u32 a0);
u32 fg_dComIfGp_getEnvLight();
void fg_dKy_SordFlush_set(u32 a0, u32 a1);
void fg_dScnKy_env_light_c_settingTevStruct(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_setLightTevColorType(u32 a0, u32 a1, u32 a2);
void fg_dMeter_mtrShow();
u32 fg_dPa_control_c_set1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                          u32 a9, u32 a10, u32 a11);
void fg_dPa_control_c_set2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                           u32 a9, u32 a10, u32 a11);
void fg_dSv_event_c_onEventBit(u32 a0, u32 a1);
u32 fg_dSv_event_c_isEventBit(u32 a0, u32 a1);
u32 fg_dSv_info_c_isSwitch(u32 a0, u32 a1, u32 a2);
void fg_dSnap_RegistFig(u32 a0, u32 a1, f32 a2, f32 a3, f32 a4);
u32 fg_fopAc_IsActor(u32 a0);
void fg_fopAc_ac_c_fopAc_ac_c(u32 a0);
void fg_fopAc_ac_c_dtor(u32 a0, u32 a1);
u32 fg_fopAcIt_Judge(u32 a0, u32 a1);
void fg_fopAcM_delete(u32 a0);
void fg_fopAcM_create1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);
u32 fg_fopAcM_create2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7);
u32 fg_fopAcM_entrySolidHeap(u32 a0, u32 a1, u32 a2);
u32 fg_fopAcM_searchActorAngleY(u32 a0, u32 a1);
void fg_fopAcM_orderPotentialEvent(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_fopAcM_setCarryNow(u32 a0, u32 a1);
void fg_fopAcM_cancelCarryNow(u32 a0);
void fg_fpcEx_Search(u32 a0, u32 a1);
void fg_mDoAud_subBgmStart(u32 a0);
void fg_mDoAud_subBgmStop();
void fg_mDoAud_bgmAllMute(u32 a0);
void fg_mDoAud_seStart(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoAud_monsSeStartPos(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);
void fg_JAIZelBasic_seDeleteObject(u32 a0);
void fg_mDoAud_changeSubBgmStatus(u32 a0);
void fg_mDoExt_modelUpdateDL(u32 a0, u32 a1);
u32 fg_mDoExt_J3DModel__create(u32 a0, u32 a1, u32 a2);
void fg_mDoExt_McaMorf_setAnm(u32 a0, u32 a1, u32 a2, f32 a3, f32 a4, f32 a5, f32 a6, u32 a7);
u32 fg_mDoExt_McaMorf_mDoExt_McaMorf(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f32 a6, u32 a7,
                                     u32 a8, u32 a9, u32 a10, u32 a11, u32 a12);
void fg_mDoExt_McaMorf_play(u32 a0, u32 a1, u32 a2, u32 a3);
void fg_mDoExt_McaMorf_entryDL(u32 a0);
void fg_mDoExt_McaMorf_calc(u32 a0);
void fg_mDoExt_baseAnm_play(u32 a0);
void fg_mDoExt_brkAnm_initDiscard(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7,
                                  u32 a8, u32 a9);
u32 fg_mDoExt_brkAnm_initResult(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7,
                                u32 a8, u32 a9);
u32 fg_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1);
void fg_mDoHIO_subRoot_c_deleteChild(u32 a0);
void fg_mDoMtx_YrotS(u32 a0, u32 a1);
void fg_mDoMtx_XrotM(u32 a0, u32 a1);
void fg_mDoMtx_YrotM(u32 a0, u32 a1);
void fg_mDoMtx_ZrotM(u32 a0, u32 a1);
u32 fg_operatorNew(u32 a0);
void fg_dl(u32 a0);
void fg_JUTReport(u32 a0, u32 a1, u32 a2, u32 a3);
u32 fg_J3DModelData_getJointTree(u32 a0);
void fg_J3DModel_setFlags(u32 a0, u32 a1);
u32 fg_J3DJoint_getJointInfo(u32 a0);
void fg_JPASetRMtxTVecfromMtx(u32 a0, u32 a1, u32 a2);
void fg_PSVECAdd(u32 a0, u32 a1, u32 a2);
f32 fg_PSVECSquareMag(u32 a0);
void fg_PSMTXCopy(u32 a0, u32 a1);
void fg_PSMTXTrans(u32 a0, f32 a1, f32 a2, f32 a3);
void fg_cSGlobe_ctor(u32 a0);
void fg_cSAngle_ctor(u32 a0);
void fg_register_global_object(u32 a0);
f32 fg_sqrtf(f32 a0);
void fg_mDoExt_brkAnm_entry(u32 animation, u32 data, f32 frame);
void fg_mDoExt_btkAnm_entry(u32 animation, u32 data, f32 frame);
u32 fg_mDoExt_brkAnm_ctor(u32 self);
u32 fg_mDoExt_btkAnm_ctor(u32 self);
BOOL fg_mDoExt_brkAnm_init(u32 animation, u32 data, u32 resource, u32 enable, s32 loop, f32 speed,
                           s32 start, s32 end, u32 modify, u32 allocator);
BOOL fg_mDoExt_btkAnm_init(u32 animation, u32 data, u32 resource, u32 enable, s32 loop, f32 speed,
                           s32 start, s32 end, u32 modify, u32 allocator);
void fg_dSv_info_c_onSwitch(u32 self, u32 index, s32 room);
void fg_dSv_info_c_offSwitch(u32 self, u32 index, s32 room);

// Resource indices shared with the GameCube archives.
enum dRes_INDEX_FGANON {
    /* BCK  */
    dRes_INDEX_FGANON_BCK_DAMAGE1_e = 0x6,
    dRes_INDEX_FGANON_BCK_DAMAGE2_e = 0x7,
    dRes_INDEX_FGANON_BCK_DOWN1_e = 0x8,
    dRes_INDEX_FGANON_BCK_KAITEN1_e = 0x9,
    dRes_INDEX_FGANON_BCK_KAITEN_S1_e = 0xA,
    dRes_INDEX_FGANON_BCK_LAST_DAMAGE1_e = 0xB,
    dRes_INDEX_FGANON_BCK_MOGAKU1_e = 0xC,
    dRes_INDEX_FGANON_BCK_NAGERU1_e = 0xD,
    dRes_INDEX_FGANON_BCK_NAGERU_S1_e = 0xE,
    dRes_INDEX_FGANON_BCK_SARU1_e = 0xF,
    dRes_INDEX_FGANON_BCK_TAME1_e = 0x10,
    dRes_INDEX_FGANON_BCK_TAME_S1_e = 0x11,
    dRes_INDEX_FGANON_BCK_TAME_S2_e = 0x12,
    dRes_INDEX_FGANON_BCK_TENNISLR1_e = 0x13,
    dRes_INDEX_FGANON_BCK_TENNISRL1_e = 0x14,
    dRes_INDEX_FGANON_BCK_TYAKUTI1_e = 0x15,
    dRes_INDEX_FGANON_BCK_WAIT1_e = 0x16,
    dRes_INDEX_FGANON_BCK_WARAU1_e = 0x17,
    /* BDLM */
    dRes_INDEX_FGANON_BDL_BPG_e = 0x1A,
    dRes_INDEX_FGANON_BDL_YDKBL00_e = 0x1B,
    dRes_INDEX_FGANON_BDL_YDKSP00_e = 0x1C,
    /* BRK  */
    dRes_INDEX_FGANON_BRK_DERU_KEN1_e = 0x1F,
    dRes_INDEX_FGANON_BRK_DERU_KEN2_e = 0x20,
    dRes_INDEX_FGANON_BRK_DERU_MAIN1_e = 0x21,
    dRes_INDEX_FGANON_BRK_DERU_MAIN2_e = 0x22,
    dRes_INDEX_FGANON_BRK_KIERU_KEN2_e = 0x23,
    dRes_INDEX_FGANON_BRK_KIERU_MAIN1_e = 0x24,
    dRes_INDEX_FGANON_BRK_KIERU_MAIN2_e = 0x25,
    dRes_INDEX_FGANON_BRK_YDKBL00_e = 0x26,
    dRes_INDEX_FGANON_BRK_YDKSP00_e = 0x27,
    /* BTK  */
    dRes_INDEX_FGANON_BTK_YDKBL00_e = 0x2A,
    dRes_INDEX_FGANON_BTK_YDKSP00_e = 0x2B,
};
enum dRes_INDEX_PGSW {
    /* BDLM */
    dRes_INDEX_PGSW_BDL_BPG_KEN1_e = 0x4,
    /* BRK  */
    dRes_INDEX_PGSW_BRK_KIERU_KEN1_e = 0x7,
};

enum FganonAction {
    ACT_STANDBY = 0,
    ACT_DERU = 1,
    ACT_KIERU = 2,
    ACT_FAIL = 3,
    ACT_FLY = 5,
    ACT_SHOT = 6,
    ACT_SPIN = 7,
    ACT_DOWN = 8,
    ACT_FLY2 = 9,
    ACT_SPIN2 = 10,
    ACT_SHOT2 = 11,
    ACT_START = 20,
    ACT_END = 21,
    ACT_LAST_END = 22,
};

#endif
