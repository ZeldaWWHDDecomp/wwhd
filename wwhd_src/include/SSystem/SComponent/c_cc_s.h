/* Collision-server layout and private views of GameCube collision objects. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct cCcD_Obj;
struct cCcD_GStts {
    be<u32> __vtbl;
};
struct cCcD_DivideInfo {
    be<u32> mRangeBits, __vtbl;
};
struct CcAab {
    cXyz mMin, mMax;
    be<u32> __vtbl;
};
struct cCcD_DivideArea {
    CcAab mAab;
    u8 mXDiffIsZero, padX[3];
    be<f32> mScaledXDiff, mInvScaledXDiff;
    u8 mYDiffIsZero, padY[3];
    be<f32> mScaledYDiff, mInvScaledYDiff;
    u8 mZDiffIsZero, padZ[3];
    be<f32> mScaledZDiff, mInvScaledZDiff;
};
struct CcObjectVtable {
    be<u32> prefix[7], GetGObjInf, gap[3], GetShapeAttr, gap30, ClrAtHit, gap38, ClrTgHit, gap40,
        ClrCoHit;
};
struct CcStatusVtable {
    be<u32> prefix[7], GetGStts, gap[3], ClrAt, gap30, ClrTg;
};
struct cCcD_Stts {
    cXyz m_cc_move;
    gptr<void> mp_actor;
    be<u32> m_apid;
    // HD: byte wrappers preserve 32-bit guest address wrapping.
    be<u8> m_weight, field_0x15, m_dmg, padding;
    gptr<CcStatusVtable> __vtbl;
};
struct cCcD_ObjCommonBase {
    be<u32> mSPrm, mRPrm;
    gptr<cCcD_Obj> mHitObj;
    be<u32> __vtbl;
};
struct cCcD_ObjAt : cCcD_ObjCommonBase {
    be<u32> mType;
    be<u8> mAtp;
    u8 padding[3];
};
struct cCcD_ObjTg : cCcD_ObjCommonBase {
    be<u32> mType;
};
struct cCcD_Obj {
    cCcD_ObjAt mObjAt;
    cCcD_ObjTg mObjTg;
    cCcD_ObjCommonBase mObjCo;
    gptr<CcObjectVtable> __vtbl;
    be<u32> mFlags;
    gptr<cCcD_Stts> mStts;
    cCcD_DivideInfo mDivideInfo;
};
using cCcD_GObjInf = cCcD_Obj;
struct CcShapeVtable {
    be<u32> prefix[5], CrossAtTg, gap18[13], CrossCo, gap50[15], GetCoCP, gap90, CalcAabBox;
};
struct cCcD_ShapeAttr {
    CcAab mAab;
    gptr<CcShapeVtable> __vtbl;
};
struct CcServerVtable {
    be<u32> prefix[3], CalcTgPlusDmg, gap10, SetPosCorrect, gap18, SetCoGObjInf, gap20,
        SetAtTgGObjInf, gap28, ChkNoHitGAtTg, gap30, ChkAtTgHitAfterCross, gap38, ChkNoHitGCo,
        gap40[3], MoveAfterCheck, gap50, SetCoGCorrectProc;
};
// HD: private complete view; the shared cCcS declaration remains opaque for existing callers.
class cCcSState {
  public:
    gptr<cCcD_Obj> mpObjAt[0x100], mpObjTg[0x300], mpObjCo[0x100], mpObj[0x500];
    be<s32> mObjAtCount, mObjTgCount, mObjCoCount, mObjCount;
    cCcD_DivideArea mDivideArea;
    gptr<CcServerVtable> __vtbl;
    void Ct();
    void Dt();
    void Set(cCcD_Obj *);
    void Remove(cCcD_Obj *);
    void ClrCoHitInf();
    void ClrTgHitInf();
    void ClrAtHitInf();
    u32 ChkNoHitAtTg(cCcD_Obj *, cCcD_Obj *);
    void ChkAtTg();
    u32 ChkNoHitCo(cCcD_Obj *, cCcD_Obj *);
    void ChkCo();
    void CalcTgPlusDmg(cCcD_Obj *, cCcD_Obj *, cCcD_Stts *, cCcD_Stts *);
    void SetAtTgCommonHitInf(cCcD_Obj *, cCcD_Obj *, cXyz *);
    void SetCoCommonHitInf(cCcD_Obj *, cXyz *, cCcD_Obj *, cXyz *, f32);
    void SetPosCorrect(cCcD_Obj *, cXyz *, cCcD_Obj *, cXyz *, f32);
    void CalcArea();
    void Move();
    void DrawClear();
    void SetCoGCorrectProc(cCcD_Obj *, cCcD_Obj *);
    void SetCoGObjInf(u32, u32, cCcD_GObjInf *, cCcD_GObjInf *, cCcD_Stts *, cCcD_Stts *,
                      cCcD_GStts *, cCcD_GStts *);
    void SetAtTgGObjInf(u32, u32, cCcD_Obj *, cCcD_Obj *, cCcD_GObjInf *, cCcD_GObjInf *,
                        cCcD_Stts *, cCcD_Stts *, cCcD_GStts *, cCcD_GStts *, cXyz *);
    u32 ChkNoHitGAtTg(cCcD_GObjInf *, cCcD_GObjInf *, cCcD_GStts *, cCcD_GStts *);
    u32 ChkAtTgHitAfterCross(u32, u32, cCcD_GObjInf *, cCcD_GObjInf *, cCcD_Stts *, cCcD_Stts *,
                             cCcD_GStts *, cCcD_GStts *);
    u32 ChkNoHitGCo(cCcD_Obj *, cCcD_Obj *);
    void MoveAfterCheck();
};
WWHD_SIZE(CcAab, 0x1C);
WWHD_SIZE(cCcD_Stts, 0x1C);
WWHD_SIZE(cCcD_Obj, 0x50);
WWHD_SIZE(cCcD_DivideArea, 0x40);
WWHD_SIZE(cCcSState, 0x2854);
WWHD_OFFSET(cCcD_Obj, __vtbl, 0x3C);
WWHD_OFFSET(cCcD_Obj, mStts, 0x44);
WWHD_OFFSET(cCcSState, mDivideArea, 0x2810);
WWHD_OFFSET(cCcSState, __vtbl, 0x2850);
WWHD_OFFSET(CcShapeVtable, CrossCo, 0x4C);
WWHD_OFFSET(CcShapeVtable, GetCoCP, 0x8C);
WWHD_OFFSET(CcServerVtable, MoveAfterCheck, 0x4C);
enum : u32 {
    cCcD_AtSPrm_Set_e = 1,
    cCcD_TgSPrm_Set_e = 1,
    cCcD_CoSPrm_Set_e = 1,
    cCcD_AtSPrm_GrpAll_e = 14,
    cCcD_CoSPrm_IGrpAll_e = 14,
    cCcD_AtSPrm_NoTgHitInfSet_e = 16,
    cCcD_TgSPrm_NoAtHitInfSet_e = 16,
    cCcD_CoSPrm_Sph3DCrr_e = 0x80,
    cCcD_CoSPrm_NoCrr_e = 0x100,
    cCcD_CoSPrm_NoCoHitInfSet_e = 0x200,
    cCcD_BsRevHit_e = 2
};
enum WeightType { WeightType_0, WeightType_1, WeightType_2 };

// Unit-local declarations for HD out-of-line GameCube operations.
u32 cCcD_DivideInfo_Chk(cCcD_DivideInfo *, cCcD_DivideInfo *);
void cM3dGAab_CalcCenter(CcAab *, cXyz *);
void cCcD_Stts_PlusCcMove(cCcD_Stts *, f32, f32, f32);
void PSVECAdd(cXyz *, cXyz *, cXyz *);
f64 squareRoot(f64);
void *operator_new(u32);
void zeroMemory(void *, u32);
void cCcD_DivideArea__ct(cCcD_DivideArea *);
void cM3dGAab_ClearForMinMax(CcAab *);
void cM3dGAab_SetMinMaxAab(CcAab *, CcAab *);
void cCcD_DivideArea_SetArea(cCcD_DivideArea *, CcAab *);
void cCcD_DivideArea_CalcDivideInfo(cCcD_DivideArea *, cCcD_DivideInfo *, CcAab *, u32);
void assertion(const char *, u32, const char *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
