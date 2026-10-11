/* Collision manager, GameCube names and unchanged HD offsets. Shared collider and actor
 * fields remain authoritative. Private base/camera-frame views avoid altering opaque shared
 * declarations. GHS arguments beyond r10 remain in an explicit stack view. */
#pragma once
// HD: the play-layout binding needs its existing opaque cCcS; keep that view separate.
#define cCcS cCcSOpaqueBindingHD
#include "bindings.h"
#undef cCcS

// Base arguments use the existing derived views only through their unchanged base fields.
using cCcD_Obj = dCcD_GObjInf;
using cCcD_GObjInf = dCcD_GObjInf;
using cCcD_Stts = dCcD_Stts;

struct dCcD_GStts {
    enum { Stts_NoActor_e = 1 };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mAtSpl;
    /* 0x05 */ be<u8> mTgSpl;
    /* 0x06 */ be<u8> mRoomId;
    /* 0x07 */ u8 paddingRoom;
    /* 0x08 */ be<u16> mActorPerfTblId;
    /* 0x0A */ u8 paddingPerf[2];
    /* 0x0C */ be<u32> mAtApid;
    /* 0x10 */ be<u32> mAtOldApid;
    /* 0x14 */ be<u32> mTgApid;
    /* 0x18 */ be<u32> mTgOldApid;
    /* 0x1C */ be<u32> mFlag;
};
WWHD_OFFSET(dCcD_GStts, mAtSpl, 4);
WWHD_OFFSET(dCcD_GStts, mTgSpl, 5);
WWHD_OFFSET(dCcD_GStts, mActorPerfTblId, 8);
WWHD_OFFSET(dCcD_GStts, mAtApid, 0xC);
WWHD_OFFSET(dCcD_GStts, mAtOldApid, 0x10);
WWHD_OFFSET(dCcD_GStts, mTgApid, 0x14);
WWHD_OFFSET(dCcD_GStts, mTgOldApid, 0x18);
WWHD_OFFSET(dCcD_GStts, mFlag, 0x1C);
WWHD_SIZE(dCcD_GStts, 0x20);
using cCcD_GStts = dCcD_GStts;

struct cCcD_objVtableHD {
    /* 0x00 */ u8 unusedBeforeGetInfo[0x1C];
    /* 0x1C */ be<u32> GetGObjInf;
    /* 0x20 */ u8 unusedBeforeGetShape[0xC];
    /* 0x2C */ be<u32> GetShapeAttr;
};
WWHD_OFFSET(cCcD_objVtableHD, GetGObjInf, 0x1C);
WWHD_OFFSET(cCcD_objVtableHD, GetShapeAttr, 0x2C);
WWHD_SIZE(cCcD_objVtableHD, 0x30);
struct cCcS_vtableHD {
    /* 0x00 */ u8 unusedBeforeCorrection[0x54];
    /* 0x54 */ be<u32> SetCoGCorrectProc;
    /* 0x58 */ be<u32> unusedVirtual;
    /* 0x5C */ be<u32> CalcParticleAngle;
};
WWHD_OFFSET(cCcS_vtableHD, SetCoGCorrectProc, 0x54);
WWHD_OFFSET(cCcS_vtableHD, CalcParticleAngle, 0x5C);
WWHD_SIZE(cCcS_vtableHD, 0x60);
struct cCcD_DivideInfo {
    /* 0x00 */ be<u32> mRangeBits;
    /* 0x04 */ be<u32> __vtbl;
};
WWHD_SIZE(cCcD_DivideInfo, 8);
struct cCcD_DivideArea {
    /* 0x00 */ u8 mUnaccessed[0x40];
};
WWHD_SIZE(cCcD_DivideArea, 0x40);
class cCcS {
  public:
    void Ct();
    void Dt();
    void Move();
    void DrawClear();
    bool ChkNoHitGAtTg(cCcD_GObjInf*, cCcD_GObjInf*, cCcD_GStts*, cCcD_GStts*);
    /* 0x0000 */ gptr<cCcD_Obj> mpObjAt[0x100];
    /* 0x0400 */ gptr<cCcD_Obj> mpObjTg[0x300];
    /* 0x1000 */ gptr<cCcD_Obj> mpObjCo[0x100];
    /* 0x1400 */ gptr<cCcD_Obj> mpObj[0x500];
    /* 0x2800 */ be<s32> mObjAtCount;
    /* 0x2804 */ be<s32> mObjTgCount;
    /* 0x2808 */ be<s32> mObjCoCount;
    /* 0x280C */ be<s32> mObjCount;
    /* 0x2810 */ cCcD_DivideArea mDivideArea;
    /* 0x2850 */ gptr<cCcS_vtableHD> __vtbl;
};
WWHD_OFFSET(cCcS, mpObjAt, 0);
WWHD_OFFSET(cCcS, mpObjTg, 0x400);
WWHD_OFFSET(cCcS, mpObjCo, 0x1000);
WWHD_OFFSET(cCcS, mpObj, 0x1400);
WWHD_OFFSET(cCcS, mObjAtCount, 0x2800);
WWHD_OFFSET(cCcS, mObjTgCount, 0x2804);
WWHD_OFFSET(cCcS, mObjCoCount, 0x2808);
WWHD_OFFSET(cCcS, mObjCount, 0x280C);
WWHD_OFFSET(cCcS, mDivideArea, 0x2810);
WWHD_OFFSET(cCcS, __vtbl, 0x2850);
WWHD_SIZE(cCcS, 0x2854);
struct dCcS_massStorageHD {
    /* 0x000 */ u8 mUnaccessed[0x1A0];
};
WWHD_SIZE(dCcS_massStorageHD, 0x1A0);
class dCcS : public cCcS {
  public:
    void Ct();
    void Dt();
    u32 ChkShieldFrontRange(cCcD_Obj*, cCcD_Obj*);
    u32 ChkShield(cCcD_Obj*, cCcD_Obj*, dCcD_GObjInf*, dCcD_GObjInf*);
    void CalcTgPlusDmg(cCcD_Obj*, cCcD_Obj*, cCcD_Stts*, cCcD_Stts*);
    s32 GetRank(u32);
    // HD: following methods name the register arguments; remaining arguments use stackArgsHD.
    void SetCoGObjInf(u32, u32, dCcD_GObjInf*, dCcD_GObjInf*, cCcD_Stts*, cCcD_Stts*, dCcD_GStts*);
    void ProcAtTgHitmark(u32, u32, cCcD_Obj*, cCcD_Obj*, dCcD_GObjInf*, dCcD_GObjInf*, cCcD_Stts*);
    void SetAtTgGObjInf(u32, u32, cCcD_Obj*, cCcD_Obj*, dCcD_GObjInf*, dCcD_GObjInf*, cCcD_Stts*);
    bool ChkAtTgHitAfterCross(u32, u32, dCcD_GObjInf*, dCcD_GObjInf*, cCcD_Stts*, cCcD_Stts*,
                              dCcD_GStts*);
    void SetPosCorrect(cCcD_Obj*, cXyz*, cCcD_Obj*, cXyz*, f32);
    void CalcParticleAngle(dCcD_GObjInf*, cCcD_Stts*, cCcD_Stts*, csXyz*);
    bool ChkCamera(cXyz*, cXyz*, f32, fopAc_ac_c*, fopAc_ac_c*);
    void Move();
    void Draw();
    void MassClear();
    bool ChkNoHitGCo(cCcD_Obj*, cCcD_Obj*);
    void MoveAfterCheck();
    /* 0x2854 */ dCcS_massStorageHD mMass_Mng;
};
WWHD_OFFSET(dCcS, mMass_Mng, 0x2854);
WWHD_SIZE(dCcS, 0x29F4);

struct dCcS_stackArgsHD {
    dCcD_GStts* tgSttsAsGSttsHD() { return reinterpret_cast<dCcD_GStts*>(tgStts.get()); }
    /* 0x00 */ gptr<cCcD_Stts> tgStts;
    /* 0x04 */ gptr<dCcD_GStts> atGStts;
    /* 0x08 */ gptr<dCcD_GStts> tgGStts;
    /* 0x0C */ gptr<cXyz> hitPos;
};
WWHD_OFFSET(dCcS_stackArgsHD, tgStts, 0);
WWHD_OFFSET(dCcS_stackArgsHD, atGStts, 4);
WWHD_OFFSET(dCcS_stackArgsHD, tgGStts, 8);
WWHD_OFFSET(dCcS_stackArgsHD, hitPos, 0xC);
WWHD_SIZE(dCcS_stackArgsHD, 0x10);
struct cCcD_capsuleHD {
    /* 0x00 */ cXyz mStart;
    /* 0x0C */ cXyz mEnd;
    /* 0x18 */ be<u32> __vtbl;
    /* 0x1C */ be<f32> mRadius;
};
WWHD_OFFSET(cCcD_capsuleHD, __vtbl, 0x18);
WWHD_SIZE(cCcD_capsuleHD, 0x20);
struct dCcS_aabViewHD {
    /* 0x00 */ cXyz mMin;
    /* 0x0C */ cXyz mMax;
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_OFFSET(dCcS_aabViewHD, mMin, 0);
WWHD_OFFSET(dCcS_aabViewHD, mMax, 0xC);
WWHD_OFFSET(dCcS_aabViewHD, __vtbl, 0x18);
WWHD_SIZE(dCcS_aabViewHD, 0x1C);
// HD: private composition exposes the two unchanged base subobjects without shared-header
// conflicts.
struct cCcD_CpsAttr {
    /* 0x00 */ dCcS_aabViewHD mAab;
    /* 0x1C */ be<u32> __vtbl_shape;
    /* 0x20 */ cCcD_capsuleHD mCps;
};
WWHD_OFFSET(cCcD_CpsAttr, mAab, 0);
WWHD_OFFSET(cCcD_CpsAttr, __vtbl_shape, 0x1C);
WWHD_OFFSET(cCcD_CpsAttr, mCps, 0x20);
WWHD_SIZE(cCcD_CpsAttr, 0x40);
struct dCcS_cameraFrameHD {
    /* 0x00 */ cCcD_DivideInfo divideInfo;
    /* 0x08 */ be<f32> cross_len;
    /* 0x0C */ cCcD_CpsAttr cpsAttr;
};
WWHD_OFFSET(dCcS_cameraFrameHD, divideInfo, 0);
WWHD_OFFSET(dCcS_cameraFrameHD, cross_len, 8);
WWHD_OFFSET(dCcS_cameraFrameHD, cpsAttr, 0xC);
WWHD_SIZE(dCcS_cameraFrameHD, 0x4C);

void PSVECSubtract(const cXyz*, const cXyz*, cXyz*);
f32 PSVECMag(const cXyz*);
void PSVECNormalize(const cXyz*, cXyz*);
void cM3d_CalcVecZAngleHD(cXyz*, csXyz*);
void dKy_SordFlush_setHD(cXyz*, s32);
void cCcD_Stts_PlusCcMoveHD(cCcD_Stts*, f32, f32, f32);
void cCcS_constructHD(cCcS*);
void dCcMassS_Mng_constructHD(dCcS_massStorageHD*);
void dCcMassS_Mng_CtHD(dCcS_massStorageHD*);
void dCcMassS_Mng_ClearHD(dCcS_massStorageHD*);
void cM3dGCps_constructHD(cCcD_capsuleHD*);
void cM3dGCps_SetHD(cCcD_capsuleHD*, cXyz*, cXyz*, f32);
void cCcD_CpsAttr_CalcAabBoxHD(cCcD_CpsAttr*);
void cCcD_CpsAttr_destroyHD(cCcD_CpsAttr*, s32);
BOOL cCcD_CpsAttr_CrossCoHD(cCcD_CpsAttr*, void*, be<f32>*);
void cCcD_DivideInfo_constructHD(cCcD_DivideInfo*);
void cCcD_DivideInfo_destroyHD(cCcD_DivideInfo*, s32);
BOOL cCcD_DivideInfo_ChkHD(void*, cCcD_DivideInfo*);
void cCcD_DivideArea_CalcDivideInfoOverAreaHD(cCcD_DivideArea*, cCcD_DivideInfo*, cCcD_CpsAttr*);
u32 dCcD_GObjInf_ChkAtNoGuardHD(dCcD_GObjInf*);
void dCcD_GAtTgCoCommonBase_SetEffCounterTimerHD(dCcD_GObjCommon*);
void dCcS_ProcAtTgHitmarkStackHD(dCcS*, u32, u32, cCcD_Obj*, cCcD_Obj*, dCcD_GObjInf*,
                                 dCcD_GObjInf*, cCcD_Stts*, cCcD_Stts*, dCcD_GStts*, dCcD_GStts*,
                                 cXyz*);

enum : u32 {
    cCcD_CoSPrm_Set_e = 1,
    cCcD_CoSPrm_Sph3DCrr_e = 0x80,
    cCcD_CoSPrm_NoCrr_e = 0x100,
    dCcG_AtSPrm_NoConHit_e = 1,
    dCcG_AtSPrm_NoHitMark_e = 2,
    dCcG_AtSPrm_StopNoConHit_e = 4,
    dCcG_AtRPrm_ShieldHit_e = 1,
    dCcG_AtRPrm_HitNoActor_e = 2,
    dCcG_TgSPrm_Shield_e = 1,
    dCcG_TgSPrm_NoConHit_e = 2,
    dCcG_TgSPrm_NoHitMark_e = 4,
    dCcG_TgSPrm_ShieldFrontRange_e = 8,
    dCcG_TgRPrm_HitNoActor_e = 1,
    dCcG_TgRPrm_ShieldHit_e = 2,
    dCcG_CoSPrm_AtLasso_e = 1,
    dCcG_CoSPrm_TgLasso_e = 2,
    dCcG_CoRPrm_HitNoActor_e = 1,
    dCcG_AtHitMark_None_e = 0,
    dCcG_AtHitMark_Unk1_e = 1,
    dCcG_AtHitMark_Big_e = 0xF,
    dCcG_TgHitMark_None_e = 0,
    dCcG_TgHitMark_Unk255_e = 0xFF,
};
namespace dPa_name {
enum : u16 { ID_IT_JN_PIYOHIT00 = 0x27B, ID_AK_JN_CRITICALHITFLASH = 0x10, ID_AK_JN_OK = 0xD };
}
