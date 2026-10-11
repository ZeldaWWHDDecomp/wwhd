/* d_cc_d: colliders (dCcD_Stts, dCcD_GObjInf, dCcD_Cps/Tri/Cyl/Sph), WWHD.
 *
 * Translation unit 025157AC..02516854 (the __sinit closes it; d_camera ends at 02515788 before it,
 * d_cc_mass_s starts at 025168E8 with the dCcMassS_Obj / dCcMassS_HitInf constructors).
 * Ported from the GameCube d_cc_d.cpp on the shared layouts of wwhd_src/include/d/d_cc_d.h; the
 * compiler-generated destructors, constructors and the small virtuals (GetGStts, ClrAt/ClrTg,
 * GetShapeAttr) are written from the WWHD code.
 *
 * Layout (HD, unchanged sizes): dCcD_Stts 0x3C with the dCcD_GStts sub-object at +0x1C (its
 * vtable first: +0x1C vtable, +0x20 AtSpl, +0x21 TgSpl, +0x22 RoomId, +0x24 ActorPerfTblId,
 * +0x28/+0x2C At apid/old, +0x30/+0x34 Tg apid/old, +0x38 flags); dCcD_GObjInf 0xF8 with the
 * cCcD_ObjHitInf vtable at +0x3C and dCcD_GObjAt +0x50, dCcD_GObjTg +0x94, dCcD_GObjCo +0xDC
 * (dCcD_GAtTgCoCommonBase 0x1C: SPrm, RPrm, callback, apid, actor, eff counter, vtable +0x18). */
#include "bindings.h"

void CcAPI_cCcD_GStts_dt(u32 p, s32 flags);
void CcAPI_cCcD_Stts_dt(u32 p, s32 flags);
void CcAPI_cCcD_Stts_Ct(u32 p);
void CcAPI_cCcD_Stts_Init(u32 p, s32 weight, s32 dmg, u32 actor, u32 apid);
void CcAPI_cCcD_Stts_ClrAt(u32 p);
void CcAPI_cCcD_Stts_ClrTg(u32 p);
void CcAPI_cCcD_GObjInf_ct(u32 p);
void CcAPI_cCcD_GObjInf_dt(u32 p, s32 flags);
void CcAPI_cCcD_Obj_Set(u32 p, u32 src);
void CcAPI_cCcD_CpsAttr_dt(u32 p, s32 flags);
void CcAPI_cCcD_TriAttr_dt(u32 p, s32 flags);
void CcAPI_cCcD_CylAttr_dt(u32 p, s32 flags);
void CcAPI_cCcD_SphAttr_dt(u32 p, s32 flags);
void CcAPI_cM3dGCps_Set(u32 p, u32 src);
void CcAPI_cM3dGCyl_Set(u32 p, u32 src);
void CcAPI_cM3dGSph_Set(u32 p, u32 src);
void CcAPI_cM3dGCyl_SetC(u32 p, u32 c);
void CcAPI_cM3dGSph_SetC(u32 p, u32 c);
void CcAPI_cM3dGSph_ct(u32 p);
void CcAPI_cXyz_sub(u32 a, u32 out, u32 b);

namespace CollisionHD {
enum HitFlag : u32 { kHit = 1 };
enum dCcG_AtRPrm_e : u32 { dCcG_AtRPrm_ShieldHit_e = 1, dCcG_AtRPrm_HitNoActor_e = 2 };
enum dCcG_TgRPrm_e : u32 { dCcG_TgRPrm_HitNoActor_e = 1, dCcG_TgRPrm_ShieldHit_e = 2 };
enum dCcG_CoRPrm_e : u32 { dCcG_CoRPrm_HitNoActor_e = 1 };
enum dCcG_At_Spl : u8 { dCcG_At_Spl_UNK8 = 8 };

// HD: GHS vtable copies used by this translation unit.
enum ProcessSearch : u32 { kJudgeByID = 0x025E1234 };
enum Vtable : u32 {
    kSttsGSttsVtable = 0x1004AEC0,
    kCapsuleVtable = 0x1004AF60,
    kCapsuleAttrVtable = 0x1004AF70,
    kTriangleVtable = 0x1004B058,
    kTriangleAttrVtable = 0x1004B068,
    kCylinderVtable = 0x1004B150,
    kCylinderAttrVtable = 0x1004B160,
    kSphereVtable = 0x1004B248,
    kSphereAttrVtable = 0x1004B258,
    kCommonVtable = 0x1004AE38,
    kAttackVtable = 0x1004AE48,
    kTargetVtable = 0x1004AE58,
    kCorrectionVtable = 0x1004AE68,
    kGSttsVtable = 0x1004AE78,
    kObjectVtable = 0x1004AED0,
    kShapeAttrVtable = 0x100015A8,
    kAabVtable = 0x1004AE14,
    kSphereObjectVtable = 0x1004B200,
};

struct ShapeAttr {
    cXyz mMin, mMax;
    be<u32> mAabVtable, mVtable;
};
WWHD_SIZE(ShapeAttr, 0x20);
struct Capsule {
    cXyz mStart, mEnd;
    be<u32> mVtable;
    be<f32> mRadius;
};
WWHD_SIZE(Capsule, 0x20);
WWHD_OFFSET(Capsule, mVtable, 0x18);
WWHD_OFFSET(Capsule, mRadius, 0x1C);
struct Triangle {
    u8 mPlane[0x10];
    be<u32> mVtable;
    u8 mVertices[0x24];
};
WWHD_SIZE(Triangle, 0x38);
struct ActorView {
    u8 mHeader[4];
    be<u32> mId;
    u8 mActorData[0x31E];
    be<s8> mRoomNo;
};
WWHD_OFFSET(ActorView, mRoomNo, 0x326);
struct HitVtable {
    u8 mOtherMethods[0x1C];
    be<u32> GetGObjInf;
    u8 mOtherHitMethods[0x14];
    be<u32> ClrAtHit;
    u8 mPad[4];
    be<u32> ClrTgHit;
};
WWHD_OFFSET(HitVtable, ClrAtHit, 0x34);
WWHD_OFFSET(HitVtable, ClrTgHit, 0x3C);
struct SourceAtTg {
    be<u8> mSe, mHitMark, mSpl, mMtrl;
    be<u32> mSPrm;
};
WWHD_SIZE(SourceAtTg, 8);
struct SourceObj {
    u8 mObj[0x1C];
    SourceAtTg mAt, mTg;
    be<u32> mCoSPrm;
    u8 mGeometry[0x24];
};
WWHD_OFFSET(SourceObj, mCoSPrm, 0x2C);

struct dCcD_Stts {
    /* 0x00 */ cXyz m_cc_move;
    /* 0x0C */ gptr<fopAc_ac_c> mp_actor;
    /* 0x10 */ be<u32> m_apid;
    /* 0x14 */ be<u8> m_weight;
    /* 0x15 */ be<u8> mReservedDamage;
    /* 0x16 */ be<u8> m_dmg;
    /* 0x17 */ u8 _17;
    /* 0x18 */ be<u32> __vtbl;
    /* 0x1C */ be<u32> __vtbl_gstts; /* dCcD_GStts */
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
    u32 GetGStts();
    void Ct();
    void ClrAt();
    void ClrTg();
    void Init(s32 weight, s32 dmg, u32 actor);
};
struct dCcD_GAtTgCoCommonBase {
    /* 0x00 */ be<u32> mSPrm;
    /* 0x04 */ be<u32> mRPrm;
    /* 0x08 */ be<u32> mHitCallback;
    /* 0x0C */ be<u32> mApid;
    /* 0x10 */ gptr<fopAc_ac_c> mAc;
    /* 0x14 */ be<s8> mEffCounter;
    /* 0x15 */ u8 _15[3];
    /* 0x18 */ be<u32> __vtbl;
    u32 GetAc();
    void SetEffCounterTimer();
};
struct dCcD_GObjAt : dCcD_GAtTgCoCommonBase {
    /* 0x1C */ be<u8> mSe;
    /* 0x1D */ be<u8> mMtrl;
    /* 0x1E */ be<u8> mHitMark;
    /* 0x1F */ be<u8> mSpl;
    /* 0x20 */ cXyz mHitPos;
    /* 0x2C */ cXyz mVec;
    /* 0x38 */ cXyz mRVec;
    void Set(u32 src);
};
struct dCcD_GObjTg : dCcD_GAtTgCoCommonBase {
    /* 0x1C */ be<u8> mSe;
    /* 0x1D */ be<u8> mMtrl;
    /* 0x1E */ be<u8> mHitMark;
    /* 0x1F */ be<u8> mSpl;
    /* 0x20 */ cXyz mVec;
    /* 0x2C */ cXyz mRVec;
    /* 0x38 */ cXyz mHitPos;
    /* 0x44 */ be<u32> mpShieldFrontRangeYAngle;
    void Set(u32 src);
};
struct dCcD_GObjCo : dCcD_GAtTgCoCommonBase {};
struct cCcD_ObjCommon {
    /* 0x00 */ be<u32> mSPrm;
    /* 0x04 */ be<u32> mRPrm;
    /* 0x08 */ be<u32> mHitObj;
    /* 0x0C */ be<u32> __vtbl;
};
struct dCcD_GObjInf {
    /* 0x000 */ cCcD_ObjCommon mObjAt; /* + mType 0x10, mAtp 0x14 */
    /* 0x010 */ be<u32> mAtType;
    /* 0x014 */ be<u8> mAtp;
    /* 0x015 */ u8 _015[3];
    /* 0x018 */ cCcD_ObjCommon mObjTg; /* + mType 0x28 */
    /* 0x028 */ be<u32> mTgType;
    /* 0x02C */ cCcD_ObjCommon mObjCo;
    /* 0x03C */ be<u32> __vtbl_hitinf;
    /* 0x040 */ be<u32> mFlags;
    /* 0x044 */ gptr<dCcD_Stts> mStts; /* [v kamome] */
    /* 0x048 */ u8 mDivideInfo[8];
    /* 0x050 */ dCcD_GObjAt mGObjAt;
    /* 0x094 */ dCcD_GObjTg mGObjTg;
    /* 0x0DC */ dCcD_GObjCo mGObjCo; /* its vtable (0x0F4) is the last word */

    void Set(u32 src);
    u32 GetGObjInf();
    void ClrAtHit();
    u32 ChkAtHit();
    void ResetAtHit();
    u32 GetAtHitObj();
    u32 GetAtHitGObj();
    u8 ChkAtNoGuard();
    void ClrTgHit();
    void ResetTgHit();
    u32 ChkTgHit();
    u32 GetTgHitObj();
    u32 GetTgHitGObj();
    u8 GetTgHitObjSe();
    u32 GetCoHitObj();
    void ClrCoHit();
    u32 ChkCoHit();
};
struct dCcD_Sph : dCcD_GObjInf {
    /* 0x0F8 */ ShapeAttr mShapeAttr;
    /* 0x118 */ cM3dGSph mSph;
    void Set(u32 src);
    void StartCAt(cXyz* pos);
    void MoveCAt(cXyz* pos);
    u32 GetShapeAttr();
};
struct dCcD_Cyl : dCcD_GObjInf {
    /* 0x0F8 */ ShapeAttr mShapeAttr;
    /* 0x118 */ cM3dGCyl mCyl;
    void Set(u32 src);
    void StartCAt(cXyz* pos);
    void StartCTg(cXyz* pos);
    void MoveCAtTg(cXyz* pos);
    void MoveCAt(cXyz* pos);
    void MoveCTg(cXyz* pos);
    u32 GetShapeAttr();
};
struct dCcD_Cps : dCcD_GObjInf {
    /* 0x0F8 */ ShapeAttr mShapeAttr;
    /* 0x118 */ Capsule mCps; /* cM3dGCps (raw bytes: raw units do pointer arithmetic on it) */
    void Set(u32 src);
    u32 GetShapeAttr();
};
struct dCcD_Tri : dCcD_GObjInf {
    /* 0x0F8 */ ShapeAttr mShapeAttr;
    Triangle mTri;
    void Set(u32 src);
    u32 GetShapeAttr();
};
struct dCcD_GStts {
    be<u32> mVtable;
    be<u8> mAtSpl, mTgSpl, mRoomId, mReserved;
    be<u16> mActorPerfTblId;
    u8 mPadding[2];
    be<u32> mAtApid, mAtOldApid, mTgApid, mTgOldApid, mFlag;
    void Ct();
    void Move();
};
WWHD_SIZE(dCcD_Stts, 0x3C);
WWHD_SIZE(dCcD_GAtTgCoCommonBase, 0x1C);
WWHD_SIZE(dCcD_GObjAt, 0x44);
WWHD_SIZE(dCcD_GObjTg, 0x48);
WWHD_SIZE(dCcD_GObjCo, 0x1C);
WWHD_SIZE(dCcD_GObjInf, 0xF8);
WWHD_SIZE(dCcD_Sph, 0x12C);
WWHD_SIZE(dCcD_Cyl, 0x130);
WWHD_SIZE(dCcD_Cps, 0x138);
WWHD_SIZE(dCcD_Tri, 0x150);
WWHD_SIZE(dCcD_GStts, 0x20);
} // namespace CollisionHD

using namespace CollisionHD;
// HD: GHS construction/deleting adapters preserve null and flag handling.

/* cXyz::operator-: out = a - b */

/* the counter decrement of ClrAtHit/ClrTgHit/ClrCoHit (SubtractXxEffCounter): never below 0 */
static inline void subtract_eff_counter(be<s8>& counter) {
    s8 c = (s8)((u8)counter.get() - 1);
    if (c < 0)
        c = 0;
    counter = c;
}

/* 025157AC */
u32 dCcD_GetGObjInf(u32 pobj) {
    if (pobj == 0) {
        JUT_ASSERT_fail("d_cc_d.cpp", 0x254, "pobj != (0)");
        return 0;
    }
    return gabi::call_ptr<u32>(
        gabi::at<HitVtable>(gabi::at<CollisionHD::dCcD_GObjInf>(pobj)->__vtbl_hitinf)
            ->GetGObjInf.get(),
        pobj); /* GetGObjInf() */
}

/* 0251580C: dCcD_GStts::~dCcD_GStts */
/* 0251580C */
void dCcD_GStts_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    CcAPI_cCcD_GStts_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515860: dCcD_Stts::~dCcD_Stts */
/* 02515860 */
void dCcD_Stts__dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    auto* view = gabi::at<CollisionHD::dCcD_Stts>(p);
    view->__vtbl_gstts = kSttsGSttsVtable;
    dCcD_GStts_dt(p + offsetof(CollisionHD::dCcD_Stts, __vtbl_gstts), 0);
    CcAPI_cCcD_Stts_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 025158CC / 025158E0 / 025158F4: dCcD_GObjCo / dCcD_GObjTg / dCcD_GObjAt::~ (nothing to destroy) */
/* 025158CC */
void dCcD_GObjCo_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 025158E0 */
void dCcD_GObjTg_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 025158F4 */
void dCcD_GObjAt_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515908: dCcD_GObjInf::~dCcD_GObjInf (the matcher's name for 02515980) */
/* 02515908 */
void dCcD_GObjInf_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    dCcD_GObjCo_dt(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjCo), 2);
    dCcD_GObjTg_dt(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjTg), 2);
    dCcD_GObjAt_dt(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjAt), 2);
    CcAPI_cCcD_GObjInf_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515980: dCcD_Cps::~dCcD_Cps (matcher: dCcD_GObjInf::~dCcD_GObjInf) */
/* 02515980 */
void dCcD_Cps_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    auto* view = gabi::at<CollisionHD::dCcD_Cps>(p);
    view->mCps.mVtable = kCapsuleVtable;           /* cM3dGCps */
    view->mShapeAttr.mVtable = kCapsuleAttrVtable; /* cCcD_CpsAttr */
    CcAPI_cCcD_CpsAttr_dt(p + offsetof(CollisionHD::dCcD_Cps, mShapeAttr), 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 025159F8: dCcD_Tri::~dCcD_Tri */
/* 025159F8 */
void dCcD_Tri_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    auto* view = gabi::at<CollisionHD::dCcD_Tri>(p);
    view->mTri.mVtable = kTriangleVtable;           /* cM3dGTri */
    view->mShapeAttr.mVtable = kTriangleAttrVtable; /* cCcD_TriAttr */
    CcAPI_cCcD_TriAttr_dt(p + offsetof(CollisionHD::dCcD_Tri, mShapeAttr), 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515A70: dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
/* 02515A70 */
void dCcD_Cyl__dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    auto* view = gabi::at<CollisionHD::dCcD_Cyl>(p);
    view->mCyl.__vtbl = kCylinderVtable;            /* cM3dGCyl */
    view->mShapeAttr.mVtable = kCylinderAttrVtable; /* cCcD_CylAttr */
    CcAPI_cCcD_CylAttr_dt(p + offsetof(CollisionHD::dCcD_Cyl, mShapeAttr), 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515AE8: dCcD_Sph::~dCcD_Sph */
/* 02515AE8 */
void dCcD_Sph_dt(u32 p, s32 flags) {
    if (p == 0)
        return;
    auto* view = gabi::at<CollisionHD::dCcD_Sph>(p);
    view->mSph.__vtbl = kSphereVtable;            /* cM3dGSph */
    view->mShapeAttr.mVtable = kSphereAttrVtable; /* cCcD_SphAttr */
    CcAPI_cCcD_SphAttr_dt(p + offsetof(CollisionHD::dCcD_Sph, mShapeAttr), 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* 02515B60: dCcD_GAtTgCoCommonBase::dCcD_GAtTgCoCommonBase (0x1C) */
/* 02515B60 */
u32 dCcD_GAtTgCoCommonBase_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x1C));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_GAtTgCoCommonBase>(p);
    view->mAc.v = 0;              /* mAc */
    view->__vtbl = kCommonVtable; /* vtable */
    view->mEffCounter = 0;        /* mEffCounter */
    view->mApid = 0;              /* mApid */
    view->mRPrm = 0;              /* mRPrm */
    view->mHitCallback = 0;       /* mHitCallback */
    view->mSPrm = 0;              /* mSPrm */
    return p;
}

/* 02515BBC: dCcD_GAtTgCoCommonBase::GetAc (fopAcM_SearchByID inlined) */
/* 02515BBC */
u32 CollisionHD::dCcD_GAtTgCoCommonBase::GetAc() {
    u32 apid = mApid;
    if (apid == 0xFFFFFFFFu)
        return 0;
    if (mAc.v == 0) {
        gabi::Local<be<u32>> key;
        *key = apid;
        mAc.v = gabi::ea(fopAcIt_Judge(kJudgeByID, key.get()));
        if (mAc.v == 0)
            mApid = 0xFFFFFFFFu;
    }
    return mAc.v;
}

/* 02515C40 */
void CollisionHD::dCcD_GAtTgCoCommonBase::SetEffCounterTimer() { mEffCounter = 5; }

/* 02515C4C: dCcD_GObjAt::dCcD_GObjAt (0x44) */
/* 02515C4C */
u32 dCcD_GObjAt_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x44));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_GObjAt>(p);
    dCcD_GAtTgCoCommonBase_ct(p);
    view->mSe = 0;                /* mSe */
    view->__vtbl = kAttackVtable; /* vtable */
    view->mHitMark = 0;           /* mHitMark */
    view->mSpl = 0;               /* mSpl */
    view->mMtrl = 0;              /* mMtrl */
    return p;
}

/* 02515CB4: dCcD_GObjAt::Set (dCcD_SrcGObjAt: Se +0, HitMark +1, Spl +2, base SPrm +4) */
/* 02515CB4 */
void CollisionHD::dCcD_GObjAt::Set(u32 src) {
    mEffCounter = 0;
    mSPrm = gabi::at<SourceAtTg>(src)->mSPrm.get();
    mSe = gabi::at<SourceAtTg>(src)->mSe.get();
    mMtrl = 0;
    mHitMark = gabi::at<SourceAtTg>(src)->mHitMark.get();
    mSpl = gabi::at<SourceAtTg>(src)->mSpl.get();
}

/* 02515CE4: dCcD_GObjTg::dCcD_GObjTg (0x48) */
/* 02515CE4 */
u32 dCcD_GObjTg_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x48));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_GObjTg>(p);
    dCcD_GAtTgCoCommonBase_ct(p);
    view->mMtrl = 0;
    view->__vtbl = kTargetVtable;
    view->mSe = 0;
    view->mSpl = 0;
    view->mpShieldFrontRangeYAngle = 0; /* mpShieldFrontRangeYAngle */
    view->mHitMark = 0;
    return p;
}

/* 02515D50: dCcD_GObjTg::Set */
/* 02515D50 */
void CollisionHD::dCcD_GObjTg::Set(u32 src) {
    mEffCounter = 0;
    mSPrm = gabi::at<SourceAtTg>(src)->mSPrm.get();
    mSe = gabi::at<SourceAtTg>(src)->mSe.get();
    mMtrl = 0;
    mHitMark = gabi::at<SourceAtTg>(src)->mHitMark.get();
    mSpl = gabi::at<SourceAtTg>(src)->mSpl.get();
    mHitPos.copy(*cXyz_Zero);
    mpShieldFrontRangeYAngle = 0;
}

/* 02515DA0: dCcD_GStts::dCcD_GStts (0x20, vtable first) */
/* 02515DA0 */
u32 dCcD_GStts_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x20));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_GStts>(p);
    view->mReserved = 0;
    view->mVtable = kGSttsVtable;
    view->mFlag = 0;                /* mFlag */
    view->mTgApid = 0xFFFFFFFFu;    /* mTgApid */
    view->mTgOldApid = 0xFFFFFFFFu; /* mTgOldApid */
    view->mAtApid = 0xFFFFFFFFu;    /* mAtApid */
    view->mAtSpl = 0;               /* mAtSpl */
    view->mAtOldApid = 0xFFFFFFFFu; /* mAtOldApid */
    view->mActorPerfTblId = 0xFFFF; /* mActorPerfTblId */
    view->mRoomId = 0;              /* mRoomId */
    view->mTgSpl = 0;               /* mTgSpl */
    return p;
}

/* 02515E18: dCcD_GStts::Ct */
/* 02515E18 */
void CollisionHD::dCcD_GStts::Ct() {
    mAtSpl = 0;
    mTgSpl = 0;
    mRoomId = 0;
    mActorPerfTblId = 0xFFFF;
    mAtApid = 0xFFFFFFFFu;
    mAtOldApid = 0xFFFFFFFFu;
    mTgApid = 0xFFFFFFFFu;
    mTgOldApid = 0xFFFFFFFFu;
    mFlag = 0;
}

/* 02515E50: dCcD_GStts::Move */
/* 02515E50 */
void CollisionHD::dCcD_GStts::Move() {
    u32 tg = mTgApid.get(), at = mAtApid.get();
    mTgOldApid = tg; /* mTgOldApid = mTgApid */
    mAtOldApid = at; /* mAtOldApid = mAtApid */
    mAtApid = 0;
    mTgApid = 0;
}

/* 02515E70: dCcD_Stts::GetGStts */
/* 02515E70 */
u32 CollisionHD::dCcD_Stts::GetGStts() {
    u32 p = gabi::ea(this);
    return p + offsetof(CollisionHD::dCcD_Stts, __vtbl_gstts);
}

/* 02515E78: dCcD_Stts::Ct */
/* 02515E78 */
void CollisionHD::dCcD_Stts::Ct() {
    u32 p = gabi::ea(this);
    CcAPI_cCcD_Stts_Ct(p);
    gabi::at<CollisionHD::dCcD_GStts>(p + offsetof(CollisionHD::dCcD_Stts, __vtbl_gstts))->Ct();
}

/* 02515EAC: dCcD_Stts::ClrAt */
/* 02515EAC */
void CollisionHD::dCcD_Stts::ClrAt() {
    u32 p = gabi::ea(this);
    CcAPI_cCcD_Stts_ClrAt(p);
    mAtSpl = 0; /* mAtSpl */
}

/* 02515EE0: dCcD_Stts::ClrTg */
/* 02515EE0 */
void CollisionHD::dCcD_Stts::ClrTg() {
    u32 p = gabi::ea(this);
    CcAPI_cCcD_Stts_ClrTg(p);
    mTgSpl = 0; /* mTgSpl */
}

/* 02515F14: dCcD_Stts::Init */
/* 02515F14 */
void CollisionHD::dCcD_Stts::Init(s32 weight, s32 dmg, u32 actor) {
    u32 p = gabi::ea(this);
    u32 procId =
        actor != 0 ? gabi::at<ActorView>(actor)->mId.get() : 0xFFFFFFFFu; /* fopAcM_GetID */
    CcAPI_cCcD_Stts_Init(p, weight, dmg, actor, procId);
    if (actor != 0) {
        s8 roomNo = gabi::at<ActorView>(actor)->mRoomNo.get(); /* fopAcM_GetRoomNo */
        mRoomId = roomNo != -1 ? (u8)roomNo : 0;
    } else {
        mFlag = mFlag.get() | 1; /* OnNoActor */
        mRoomId = 0;
    }
}

/* 02515FB8: dCcD_GObjInf::dCcD_GObjInf */
/* 02515FB8 */
u32 dCcD_GObjInf_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0xF8));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_GObjInf>(p);
    CcAPI_cCcD_GObjInf_ct(p);
    view->__vtbl_hitinf = kObjectVtable;
    dCcD_GObjAt_ct(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjAt));
    dCcD_GObjTg_ct(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjTg));
    dCcD_GAtTgCoCommonBase_ct(p + offsetof(CollisionHD::dCcD_GObjInf, mGObjCo)); /* dCcD_GObjCo */
    view->mGObjCo.__vtbl = kCorrectionVtable;
    return p;
}

/* 02516030: dCcD_GObjInf::Set (dCcD_SrcGObjInf: cCcD_SrcObj +0, At +0x1C, Tg +0x24, Co SPrm +0x2C) */
/* 02516030 */
void CollisionHD::dCcD_GObjInf::Set(u32 src) {
    CcAPI_cCcD_Obj_Set(gabi::ea(this), src);
    mGObjAt.Set(src + offsetof(SourceObj, mAt));
    mGObjTg.Set(src + offsetof(SourceObj, mTg));
    mGObjCo.mEffCounter = 0;
    mGObjCo.mSPrm = gabi::at<SourceObj>(src)->mCoSPrm.get();
}

/* 02516090 */
u32 CollisionHD::dCcD_GObjInf::GetGObjInf() {
    u32 p = gabi::ea(this);
    return p;
}

/* 02516094 */
void CollisionHD::dCcD_GObjInf::ClrAtHit() {
    mObjAt.mRPrm = mObjAt.mRPrm & ~kHit; /* cCcD_ObjHitInf::ClrAtHit */
    mObjAt.mHitObj = 0;
    mGObjAt.mRPrm =
        mGObjAt.mRPrm &
        ~(dCcG_AtRPrm_HitNoActor_e | dCcG_AtRPrm_ShieldHit_e); /* OffAtShieldHit, OffAtHitNoActor */
    mGObjAt.mApid = 0xFFFFFFFFu;                               /* ClrActorInfo */
    mGObjAt.mAc.v = 0;
    subtract_eff_counter(mGObjAt.mEffCounter);
}

/* 025160DC */
u32 CollisionHD::dCcD_GObjInf::ChkAtHit() {
    if (!(mObjAt.mRPrm & kHit))
        return 0;
    if (!(mGObjAt.mRPrm & dCcG_AtRPrm_HitNoActor_e) && mGObjAt.GetAc() == 0)
        return 0;
    return 1;
}

/* 02516138 */
void CollisionHD::dCcD_GObjInf::ResetAtHit() {
    gabi::call_ptr(gabi::at<HitVtable>(__vtbl_hitinf)->ClrAtHit.get(), this); /* ClrAtHit() */
    mGObjAt.mEffCounter = 0;
}

/* 02516178 */
u32 CollisionHD::dCcD_GObjInf::GetAtHitObj() {
    if (!(mGObjAt.mRPrm & dCcG_AtRPrm_HitNoActor_e) && mGObjAt.GetAc() == 0)
        return 0;
    return mObjAt.mHitObj;
}

/* 025161D8: GetAtHitGObj (matcher: GetTgHitGObj, a duplicate name) */
/* 025161D8 */
u32 CollisionHD::dCcD_GObjInf::GetAtHitGObj() {
    u32 obj = (this)->GetAtHitObj();
    if (obj == 0)
        return 0;
    return dCcD_GetGObjInf(obj);
}

/* 02516204 */
u8 CollisionHD::dCcD_GObjInf::ChkAtNoGuard() { return mGObjAt.mSpl >= dCcG_At_Spl_UNK8; }

/* 0251621C */
void CollisionHD::dCcD_GObjInf::ClrTgHit() {
    mObjTg.mRPrm = mObjTg.mRPrm & ~kHit;
    mObjTg.mHitObj = 0;
    mGObjTg.mRPrm =
        mGObjTg.mRPrm &
        ~(dCcG_TgRPrm_HitNoActor_e | dCcG_TgRPrm_ShieldHit_e); /* OffTgHitNoActor, OffTgShieldHit */
    mGObjTg.mApid = 0xFFFFFFFFu;
    mGObjTg.mAc.v = 0;
    subtract_eff_counter(mGObjTg.mEffCounter);
}

/* 02516264 */
void CollisionHD::dCcD_GObjInf::ResetTgHit() {
    gabi::call_ptr(gabi::at<HitVtable>(__vtbl_hitinf)->ClrTgHit.get(), this); /* ClrTgHit() */
    mGObjTg.mEffCounter = 0;
}

/* 025162A4 */
u32 CollisionHD::dCcD_GObjInf::ChkTgHit() {
    if (!(mObjTg.mRPrm & kHit))
        return 0;
    if (!(mGObjTg.mRPrm & dCcG_TgRPrm_HitNoActor_e) && mGObjTg.GetAc() == 0)
        return 0;
    return 1;
}

/* 02516300 */
u32 CollisionHD::dCcD_GObjInf::GetTgHitObj() {
    if (!(mGObjTg.mRPrm & dCcG_TgRPrm_HitNoActor_e) && mGObjTg.GetAc() == 0)
        return 0;
    return mObjTg.mHitObj;
}

/* 02516360 */
u32 CollisionHD::dCcD_GObjInf::GetTgHitGObj() {
    u32 obj = (this)->GetTgHitObj();
    if (obj == 0)
        return 0;
    return dCcD_GetGObjInf(obj);
}

/* 0251638C */
u8 CollisionHD::dCcD_GObjInf::GetTgHitObjSe() {
    CollisionHD::dCcD_GObjInf* hit = gabi::at<CollisionHD::dCcD_GObjInf>((this)->GetTgHitGObj());
    if (hit == nullptr)
        return 0;
    return hit->mGObjAt.mSe;
}

/* 025163BC */
u32 CollisionHD::dCcD_GObjInf::GetCoHitObj() {
    if (!(mGObjCo.mRPrm & dCcG_CoRPrm_HitNoActor_e) && mGObjCo.GetAc() == 0)
        return 0;
    return mObjCo.mHitObj;
}

/* 0251641C */
void CollisionHD::dCcD_GObjInf::ClrCoHit() {
    mObjCo.mRPrm = mObjCo.mRPrm & ~kHit;
    mObjCo.mHitObj = 0;
    mGObjCo.mRPrm = mGObjCo.mRPrm & ~dCcG_CoRPrm_HitNoActor_e; /* OffCoHitNoActor */
    mGObjCo.mApid = 0xFFFFFFFFu;
    mGObjCo.mAc.v = 0;
    subtract_eff_counter(mGObjCo.mEffCounter);
}

/* 02516464 */
u32 CollisionHD::dCcD_GObjInf::ChkCoHit() {
    if (!(mObjCo.mRPrm & kHit))
        return 0;
    if (!(mGObjCo.mRPrm & dCcG_CoRPrm_HitNoActor_e) && mGObjCo.GetAc() == 0)
        return 0;
    return 1;
}

/* 025164C0: dCcD_Cps::Set (dCcD_SrcCps: dCcD_SrcGObjInf 0x30, then cM3dGCpsS) */
/* 025164C0 */
void CollisionHD::dCcD_Cps::Set(u32 src) {
    (this)->CollisionHD::dCcD_GObjInf::Set(src);
    CcAPI_cM3dGCps_Set(gabi::ea(&mCps), src + offsetof(SourceObj, mGeometry));
}

/* 02516504: dCcD_Cps::GetShapeAttr */
/* 02516504 */
u32 CollisionHD::dCcD_Cps::GetShapeAttr() {
    u32 p = gabi::ea(this);
    return p + offsetof(CollisionHD::dCcD_Cps, mShapeAttr);
}

/* 0251650C: dCcD_Tri::Set */
/* 0251650C */
void CollisionHD::dCcD_Tri::Set(u32 src) { (this)->CollisionHD::dCcD_GObjInf::Set(src); }

/* 02516510: dCcD_Tri::GetShapeAttr */
/* 02516510 */
u32 CollisionHD::dCcD_Tri::GetShapeAttr() {
    u32 p = gabi::ea(this);
    return p + offsetof(CollisionHD::dCcD_Tri, mShapeAttr);
}

/* 02516518: dCcD_Cyl::Set */
/* 02516518 */
void CollisionHD::dCcD_Cyl::Set(u32 src) {
    (this)->CollisionHD::dCcD_GObjInf::Set(src);
    CcAPI_cM3dGCyl_Set(gabi::ea(&mCyl), src + offsetof(SourceObj, mGeometry));
}

/* 0251655C: dCcD_Cyl::StartCAt */
/* 0251655C */
void CollisionHD::dCcD_Cyl::StartCAt(cXyz* pos) {
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    mGObjAt.mVec.x = x; /* SetAtVec(cXyz::Zero) */
    mGObjAt.mVec.z = z;
    mGObjAt.mVec.y = y;
    CcAPI_cM3dGCyl_SetC(gabi::ea(&mCyl), gabi::ea(pos));
}

/* 02516580: dCcD_Cyl::StartCTg */
/* 02516580 */
void CollisionHD::dCcD_Cyl::StartCTg(cXyz* pos) {
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    mGObjTg.mVec.x = x;
    mGObjTg.mVec.z = z;
    mGObjTg.mVec.y = y;
    CcAPI_cM3dGCyl_SetC(gabi::ea(&mCyl), gabi::ea(pos));
}

/* 025165A4: dCcD_Cyl::MoveCAtTg */
/* 025165A4 */
void CollisionHD::dCcD_Cyl::MoveCAtTg(cXyz* pos) {
    gabi::Local<cXyz> vel;
    CcAPI_cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(&mCyl)); /* pos - *GetCP() */
    mGObjAt.mVec.copy(*vel);
    mGObjTg.mVec.copy(*vel);
    CcAPI_cM3dGCyl_SetC(gabi::ea(&mCyl), gabi::ea(pos));
}

/* 02516618: dCcD_Cyl::MoveCAt */
/* 02516618 */
void CollisionHD::dCcD_Cyl::MoveCAt(cXyz* pos) {
    gabi::Local<cXyz> vel;
    CcAPI_cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(&mCyl));
    mGObjAt.mVec.copy(*vel);
    CcAPI_cM3dGCyl_SetC(gabi::ea(&mCyl), gabi::ea(pos));
}

/* 02516680: dCcD_Cyl::MoveCTg */
/* 02516680 */
void CollisionHD::dCcD_Cyl::MoveCTg(cXyz* pos) {
    gabi::Local<cXyz> vel;
    CcAPI_cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(&mCyl));
    mGObjTg.mVec.copy(*vel);
    CcAPI_cM3dGCyl_SetC(gabi::ea(&mCyl), gabi::ea(pos));
}

/* 025166E8: dCcD_Cyl::GetShapeAttr */
/* 025166E8 */
u32 CollisionHD::dCcD_Cyl::GetShapeAttr() {
    u32 p = gabi::ea(this);
    return p + offsetof(CollisionHD::dCcD_Cyl, mShapeAttr);
}

/* 025166F0: dCcD_Sph::dCcD_Sph (0x12C) */
/* 025166F0 */
u32 dCcD_Sph_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x12C));
        if (p == 0)
            return p;
    }
    auto* view = gabi::at<CollisionHD::dCcD_Sph>(p);
    dCcD_GObjInf_ct(p);
    view->mShapeAttr.mVtable = kShapeAttrVtable; /* cCcD_ShapeAttr */
    view->mShapeAttr.mAabVtable = kAabVtable;    /* cM3dGAab (this unit's copy) */
    CcAPI_cM3dGSph_ct(p + offsetof(CollisionHD::dCcD_Sph, mSph));
    view->__vtbl_hitinf = kSphereObjectVtable;    /* dCcD_Sph */
    view->mSph.__vtbl = kSphereVtable;            /* cM3dGSph */
    view->mShapeAttr.mVtable = kSphereAttrVtable; /* cCcD_SphAttr */
    return p;
}

/* 0251677C: dCcD_Sph::Set */
/* 0251677C */
void CollisionHD::dCcD_Sph::Set(u32 src) {
    (this)->CollisionHD::dCcD_GObjInf::Set(src);
    CcAPI_cM3dGSph_Set(gabi::ea(&mSph), src + offsetof(SourceObj, mGeometry));
}

/* 025167C0: dCcD_Sph::StartCAt */
/* 025167C0 */
void CollisionHD::dCcD_Sph::StartCAt(cXyz* pos) {
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    mGObjAt.mVec.x = x;
    mGObjAt.mVec.z = z;
    mGObjAt.mVec.y = y;
    CcAPI_cM3dGSph_SetC(gabi::ea(&mSph), gabi::ea(pos));
}

/* 025167E4: dCcD_Sph::MoveCAt */
/* 025167E4 */
void CollisionHD::dCcD_Sph::MoveCAt(cXyz* pos) {
    gabi::Local<cXyz> vel;
    CcAPI_cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(&mSph));
    mGObjAt.mVec.copy(*vel);
    CcAPI_cM3dGSph_SetC(gabi::ea(&mSph), gabi::ea(pos));
}

/* 0251684C: dCcD_Sph::GetShapeAttr */
/* 0251684C */
u32 CollisionHD::dCcD_Sph::GetShapeAttr() {
    u32 p = gabi::ea(this);
    return p + offsetof(CollisionHD::dCcD_Sph, mShapeAttr);
}

/* 02516854: static initialisers (header statics) */
/* 02516854 */
void __sinit_d_cc_d_cpp() { sinit_header_statics(0x1046EFC8, 0x101D5718); }

/* WWHD glue (tools/verify/autoglue.py)
str 1004ADF8 "pobj != (0)" in 025157AC
str 1004AE04 "d_cc_d.cpp" in 025157AC
ext 0200BD18 void CcAPI_cCcD_GStts_dt(u32, s32)
ext 0200BD88 void CcAPI_cCcD_Stts_dt(u32, s32)
ext 0200BD9C void CcAPI_cCcD_Stts_Ct(u32)
ext 0200BDD0 void CcAPI_cCcD_Stts_Init(u32, s32, s32, u32, u32)
ext 0200CCEC void CcAPI_cCcD_Stts_ClrAt(u32)
ext 0200BF6C void CcAPI_cCcD_Stts_ClrTg(u32)
ext 0200C034 void CcAPI_cCcD_GObjInf_ct(u32)
ext 0200C110 void CcAPI_cCcD_GObjInf_dt(u32, s32)
ext 0200C0D4 void CcAPI_cCcD_Obj_Set(u32, u32)
ext 0200B600 void CcAPI_cCcD_CpsAttr_dt(u32, s32)
ext 0200B660 void CcAPI_cCcD_TriAttr_dt(u32, s32)
ext 0200B680 void CcAPI_cCcD_CylAttr_dt(u32, s32)
ext 0200B6A0 void CcAPI_cCcD_SphAttr_dt(u32, s32)
ext 020181FC void CcAPI_cM3dGCps_Set(u32, u32)
ext 020185E0 void CcAPI_cM3dGCyl_Set(u32, u32)
ext 02018EDC void CcAPI_cM3dGSph_Set(u32, u32)
ext 020182E0 void CcAPI_cM3dGCyl_SetC(u32, u32)
ext 02018D40 void CcAPI_cM3dGSph_SetC(u32, u32)
ext 02018C40 void CcAPI_cM3dGSph_ct(u32)
ext 0201ADE0 void CcAPI_cXyz_sub(u32, u32, u32)
*/
