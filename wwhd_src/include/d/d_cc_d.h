/* Colliders (d_cc_d, c_cc_d), WWHD layouts. 
 *
 * Unchanged from GameCube in size (dCcD_Stts 0x3C, dCcD_GObjInf 0xF8 [constructor 02515FB8],
 * dCcD_Sph 0x12C [kamome], dCcD_Cyl 0x130, dCcD_Cps 0x138, dCcD_Tri 0x150) and, where measured,
 * in member offsets. Source data (dCcD_SrcSph/Cyl/...) lives in .data: pass its address.
 * [v] used by a verified function, [g] GameCube offset/signature not yet exercised. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "f_op/f_op_actor.h"
#include "wwhd.h"

WWHD_OPAQUE(dCcD_SrcSph);
WWHD_OPAQUE(dCcD_SrcCyl);
WWHD_OPAQUE(dCcD_SrcCps);
WWHD_OPAQUE(dCcD_SrcTri);

struct dCcD_Stts {
    /* 0x00 */ cXyz m_cc_move;
    /* 0x0C */ gptr<fopAc_ac_c> mp_actor;
    /* 0x10 */ be<u32> m_apid;
    /* 0x14 */ be<u8> m_weight;
    /* 0x15 */ be<u8> field_0x15;
    /* 0x16 */ be<u8> m_dmg;
    /* 0x17 */ u8 _17;
    /* 0x18 */ be<u32> __vtbl;
    /* 0x1C */ be<u32> __vtbl_gstts;   /* dCcD_GStts */
    /* 0x20 */ be<u8> mAtSpl;
    /* 0x21 */ be<u8> mTgSpl;
    /* 0x22 */ be<u8> mRoomId;
    /* 0x23 */ u8 _23;
    /* 0x24 */ be<u16> mActorPerfTblId;
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<u32> mAtApid;
    /* 0x2C */ be<u32> mAtOldApid;
    /* 0x30 */ be<u32> mTgApid;
    /* 0x34 */ be<u32> mTgOldApid;
    /* 0x38 */ be<u32> mFlag;
    /* 02515F14 [g] */
    void Init(s32 weight, s32 dmg, fopAc_ac_c* actor) { gabi::call(0x02515F14, this, weight, dmg, actor); }
    /* 02515E50 dCcD_GStts::Move, on the dCcD_GStts sub-object [g] */
    void Move() { gabi::call(0x02515E50, gabi::at<u8>(gabi::ea(this) + 0x1C)); }
    u8 GetDmg() { return m_dmg; }
    void SetWeight(u8 w) { m_weight = w; }
    fopAc_ac_c* GetActor() { return mp_actor; }
};
WWHD_OFFSET(dCcD_Stts, mFlag, 0x38);
WWHD_SIZE(dCcD_Stts, 0x3C);

/* dCcD_GObjAt / Tg / Co: dCcD_GAtTgCoCommonBase (0x1C) + extras */
struct dCcD_GObjCommon {
    /* 0x00 */ be<u32> mSPrm;
    /* 0x04 */ be<u32> mRPrm;
    /* 0x08 */ be<u32> mHitCallback;
    /* 0x0C */ be<u32> mApid;
    /* 0x10 */ gptr<fopAc_ac_c> mAc;
    /* 0x14 */ be<s8> mEffCounter;
    /* 0x15 */ u8 _15[3];
    /* 0x18 */ be<u32> __vtbl;
};
struct dCcD_GObjAt : dCcD_GObjCommon {
    /* 0x1C */ be<u8> mSe;
    /* 0x1D */ be<u8> mMtrl;
    /* 0x1E */ be<u8> mHitMark;
    /* 0x1F */ be<u8> mSpl;
    /* 0x20 */ cXyz mHitPos;
    /* 0x2C */ cXyz mVec;
    /* 0x38 */ cXyz mRVec;
};
struct dCcD_GObjTg : dCcD_GObjCommon {
    /* 0x1C */ be<u8> mSe;
    /* 0x1D */ be<u8> mMtrl;
    /* 0x1E */ be<u8> mHitMark;
    /* 0x1F */ be<u8> mSpl;
    /* 0x20 */ cXyz mVec;
    /* 0x2C */ cXyz mRVec;
    /* 0x38 */ cXyz mHitPos;
    /* 0x44 */ be<u32> mpShieldFrontRangeYAngle;
};
struct dCcD_GObjCo : dCcD_GObjCommon {};

/* cCcD_ObjAt/Tg/Co (cCcD_ObjCommonBase 0x10: SPrm, RPrm, mHitObj, vtable) */
struct cCcD_ObjCommon {
    /* 0x00 */ be<u32> mSPrm;
    /* 0x04 */ be<u32> mRPrm;
    /* 0x08 */ be<u32> mHitObj;
    /* 0x0C */ be<u32> __vtbl;
};

struct dCcD_GObjInf {
    /* 0x000 */ cCcD_ObjCommon mObjAt;    /* + mType 0x10, mAtp 0x14 */
    /* 0x010 */ be<u32> mAtType;
    /* 0x014 */ be<u8> mAtp;
    /* 0x015 */ u8 _015[3];
    /* 0x018 */ cCcD_ObjCommon mObjTg;    /* + mType 0x28 */
    /* 0x028 */ be<u32> mTgType;
    /* 0x02C */ cCcD_ObjCommon mObjCo;
    /* 0x03C */ be<u32> __vtbl_hitinf;
    /* 0x040 */ be<u32> mFlags;
    /* 0x044 */ gptr<dCcD_Stts> mStts;    /* [v kamome] */
    /* 0x048 */ u8 mDivideInfo[8];
    /* 0x050 */ dCcD_GObjAt mGObjAt;
    /* 0x094 */ dCcD_GObjTg mGObjTg;
    /* 0x0DC */ dCcD_GObjCo mGObjCo;     /* its vtable (0x0F4) is the last word */

    void SetStts(dCcD_Stts* s) { mStts = s; }
    /* out-of-line in WWHD (d_cc_d.cpp) [g] */
    BOOL ChkAtHit() { return gabi::call<BOOL>(0x025160DC, this); }
    void ClrAtHit() { gabi::call(0x02516094, this); }
    BOOL ChkTgHit() { return gabi::call<BOOL>(0x025162A4, this); }
    void ClrTgHit() { gabi::call(0x0251621C, this); }
    BOOL ChkCoHit() { return gabi::call<BOOL>(0x02516464, this); }   /* [v kamome] */
    void* GetTgHitObj() { return gabi::call<void*>(0x02516300, this); }
    /* HD: dCcD_GAtTgCoCommonBase::GetAc is out of line (02515BBC), on the At/Tg/Co sub-object [v cc] */
    fopAc_ac_c* GetAtHitAc() { return gabi::call<fopAc_ac_c*>(0x02515BBC, &mGObjAt); }
    fopAc_ac_c* GetTgHitAc() { return gabi::call<fopAc_ac_c*>(0x02515BBC, &mGObjTg); }
    fopAc_ac_c* GetCoHitAc() { return gabi::call<fopAc_ac_c*>(0x02515BBC, &mGObjCo); }
    u8 GetTgHitMark() { return mGObjTg.mHitMark; }
    cXyz* GetTgHitPosP() { return &mGObjTg.mHitPos; }
    void SetAtSpl(u8 s) { mGObjAt.mSpl = s; }
    void SetTgSpl(u8 s) { mGObjTg.mSpl = s; }
    void SetAtSe(u8 s) { mGObjAt.mSe = s; }
    void SetTgSe(u8 s) { mGObjTg.mSe = s; }
    void SetAtHitMark(u8 m) { mGObjAt.mHitMark = m; }
    void SetTgHitMark(u8 m) { mGObjTg.mHitMark = m; }
    void SetAtAtp(u8 a) { mAtp = a; }
    void SetAtType(u32 t) { mAtType = t; }
    void SetTgType(u32 t) { mTgType = t; }
    void OnAtSPrmBit(u32 b) { mObjAt.mSPrm |= b; }
    void OffAtSPrmBit(u32 b) { mObjAt.mSPrm &= ~b; }
    void OnTgSPrmBit(u32 b) { mObjTg.mSPrm |= b; }
    void OffTgSPrmBit(u32 b) { mObjTg.mSPrm &= ~b; }
    void OnCoSPrmBit(u32 b) { mObjCo.mSPrm |= b; }
    void OffCoSPrmBit(u32 b) { mObjCo.mSPrm &= ~b; }
};
WWHD_OFFSET(dCcD_GObjInf, mStts, 0x44);
WWHD_OFFSET(dCcD_GObjInf, mGObjTg, 0x94);
WWHD_SIZE(dCcD_GObjInf, 0xF8);

/* cCcD_ShapeAttr (0x20, vtable at +0x1C) + cM3dG* */
struct cM3dGSph {
    cXyz mCenter;
    be<f32> mRadius;
    be<u32> __vtbl;
    void SetC(cXyz* c) { gabi::call(0x02018D40, this, c); }   /* 02018D40 [v kamome] */
    void SetR(f32 r) { gabi::call(0x02018C8C, this, r); }     /* 02018C8C [v kamome] */
};
struct cM3dGCyl {
    cXyz mCenter;
    be<f32> mRadius;
    be<f32> mHeight;
    be<u32> __vtbl;
    void SetC(cXyz* c) { gabi::call(0x020182E0, this, c); }   /* 020182E0 [g] */
    void SetR(f32 r) { gabi::call(0x020184DC, this, r); }     /* 020184DC [g] */
    void SetH(f32 h) { gabi::call(0x02018428, this, h); }     /* 02018428 [g] */
};

struct dCcD_Sph : dCcD_GObjInf {
    /* 0x0F8 */ u8 mShapeAttr[0x20];
    /* 0x118 */ cM3dGSph mSph;
    void Set(const dCcD_SrcSph* src) { gabi::call(0x0251677C, this, src); }   /* 0251677C [g] */
    void SetC(cXyz* c) { mSph.SetC(c); }
    void SetR(f32 r) { mSph.SetR(r); }
};
WWHD_OFFSET(dCcD_Sph, mSph, 0x118);
WWHD_SIZE(dCcD_Sph, 0x12C);

struct dCcD_Cyl : dCcD_GObjInf {
    /* 0x0F8 */ u8 mShapeAttr[0x20];
    /* 0x118 */ cM3dGCyl mCyl;
    void Set(const dCcD_SrcCyl* src) { gabi::call(0x02516518, this, src); }   /* 02516518 [g] */
    void SetC(cXyz* c) { mCyl.SetC(c); }
    void SetR(f32 r) { mCyl.SetR(r); }
    void SetH(f32 h) { mCyl.SetH(h); }
};
WWHD_OFFSET(dCcD_Cyl, mCyl, 0x118);
WWHD_SIZE(dCcD_Cyl, 0x130);

struct dCcD_Cps : dCcD_GObjInf {
    /* 0x0F8 */ u8 mShapeAttr[0x20];
    /* 0x118 */ u8 mCps[0x20];          /* cM3dGCps: cM3dGLin (start 0x0, end 0xC, vtable 0x18) + radius 0x1C */
};
WWHD_SIZE(dCcD_Cps, 0x138);

struct dCcD_Tri : dCcD_GObjInf {
    /* 0x0F8 */ u8 mTriAttr[0x58];
};
WWHD_SIZE(dCcD_Tri, 0x150);

/* ---- inline constructors (HD), as GHS expands them in actor constructors [v switem] ----
 * The global vtables are shared; cM3dGAab's vtable is a per-TU copy (pass the TU's address). */
inline void dCcD_Stts_ct(dCcD_Stts* s) {
    gabi::call(0x0200BD2C, s);                                    /* cCcD_Stts::cCcD_Stts */
    gabi::call(0x02515DA0, gabi::at<u8>(gabi::ea(s) + 0x1C));     /* dCcD_GStts::dCcD_GStts */
    s->__vtbl = 0x1004AE88;
    s->__vtbl_gstts = 0x1004AEC0;
}
inline void dCcD_Cyl_ct(dCcD_Cyl* c, u32 tu_aab_vtbl) {
    gabi::call(0x02515FB8, c);                                    /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x100015A8);            /* cCcD_ShapeAttr */
    gabi::store<u32>(gabi::ea(c) + 0x110, tu_aab_vtbl);           /* cM3dGAab (per TU) */
    gabi::call(0x02018590, &c->mCyl);                             /* cM3dGCyl::cM3dGCyl */
    c->__vtbl_hitinf = 0x1004B108;                                /* dCcD_Cyl */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x1004B160);
    c->mCyl.__vtbl = 0x1004B150;
}
/* out-of-line destructors (shared): 02515A70 dCcD_Cyl::~dCcD_Cyl, 02515860 dCcD_Stts::~dCcD_Stts
 * (the matcher names 02515A70 dBgS_Acch::~dBgS_Acch: wrong) [v switem] */
inline void dCcD_Cyl_dt(dCcD_Cyl* c, s32 flags) { gabi::call(0x02515A70, c, flags); }
inline void dCcD_Stts_dt(dCcD_Stts* s, s32 flags) { gabi::call(0x02515860, s, flags); }
/* cCcD_Obj::ChkAtType: the hit object's At type (cCcD_ObjAt::mType at +0x10) */
inline bool cCcD_Obj_ChkAtType(void* obj, u32 mask) { return (gabi::load<u32>(gabi::ea(obj) + 0x10) & mask) != 0; }
