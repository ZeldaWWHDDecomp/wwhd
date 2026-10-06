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

namespace d_cc_d_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cCcD_GStts_dt(u32 p, s32 flags) { gabi::call(0x0200BD18, p, flags); }
static inline void cCcD_Stts_dt(u32 p, s32 flags) { gabi::call(0x0200BD88, p, flags); }
static inline void cCcD_Stts_Ct(u32 p) { gabi::call(0x0200BD9C, p); }
static inline void cCcD_Stts_Init(u32 p, s32 weight, s32 dmg, u32 actor, u32 apid) {
    gabi::call(0x0200BDD0, p, weight, dmg, actor, apid);
}
static inline void cCcD_Stts_ClrAt(u32 p) { gabi::call(0x0200CCEC, p); }
static inline void cCcD_Stts_ClrTg(u32 p) { gabi::call(0x0200BF6C, p); }
static inline void cCcD_GObjInf_ct(u32 p) { gabi::call(0x0200C034, p); }
static inline void cCcD_GObjInf_dt(u32 p, s32 flags) { gabi::call(0x0200C110, p, flags); }
static inline void cCcD_Obj_Set(u32 p, u32 src) { gabi::call(0x0200C0D4, p, src); }
static inline void cCcD_CpsAttr_dt(u32 p, s32 flags) { gabi::call(0x0200B600, p, flags); }
static inline void cCcD_TriAttr_dt(u32 p, s32 flags) { gabi::call(0x0200B660, p, flags); }
static inline void cCcD_CylAttr_dt(u32 p, s32 flags) { gabi::call(0x0200B680, p, flags); }
static inline void cCcD_SphAttr_dt(u32 p, s32 flags) { gabi::call(0x0200B6A0, p, flags); }
static inline void cM3dGCps_Set(u32 p, u32 src) { gabi::call(0x020181FC, p, src); }
static inline void cM3dGCyl_Set(u32 p, u32 src) { gabi::call(0x020185E0, p, src); }
static inline void cM3dGSph_Set(u32 p, u32 src) { gabi::call(0x02018EDC, p, src); }
static inline void cM3dGCyl_SetC(u32 p, u32 c) { gabi::call(0x020182E0, p, c); }
static inline void cM3dGSph_SetC(u32 p, u32 c) { gabi::call(0x02018D40, p, c); }
static inline void cM3dGSph_ct(u32 p) { gabi::call(0x02018C40, p); }
static inline void cXyz_sub(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); } /* cXyz::operator-: out = a - b */


static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }

/* the counter decrement of ClrAtHit/ClrTgHit/ClrCoHit (SubtractXxEffCounter): never below 0 */
static inline void subtract_eff_counter(u32 a) {
    s8 c = (s8)(gabi::load<u8>(a) - 1);
    if (c < 0) c = 0;
    gabi::store<s8>(a, c);
}

/* 025157AC */
static u32 dCcD_GetGObjInf(u32 pobj) {
    WWHD_FUNC(0x025157AC, u32, pobj);
    if (pobj == 0) {
        JUT_ASSERT_fail(STR(0x1004AE04), 0x254, STR(0x1004ADF8));
        return 0;
    }
    return gabi::call_ptr<u32>(ld(ld(pobj + 0x3C) + 0x1C), pobj); /* GetGObjInf() */
}
VERIFY(0x025157AC, dCcD_GetGObjInf);

/* 0251580C: dCcD_GStts::~dCcD_GStts */
static void dCcD_GStts_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x0251580C, void, p, flags);
    if (p == 0) return;
    cCcD_GStts_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x0251580C, dCcD_GStts_dt);

/* 02515860: dCcD_Stts::~dCcD_Stts */
static void dCcD_Stts_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02515860, void, p, flags);
    if (p == 0) return;
    st(p + 0x1C, 0x1004AEC0);
    dCcD_GStts_dt(p + 0x1C, 0);
    cCcD_Stts_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x02515860, dCcD_Stts_dt);

/* 025158CC / 025158E0 / 025158F4: dCcD_GObjCo / dCcD_GObjTg / dCcD_GObjAt::~ (nothing to destroy) */
static void dCcD_GObjCo_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x025158CC, void, p, flags);
    if (p == 0) return;
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x025158CC, dCcD_GObjCo_dt);

static void dCcD_GObjTg_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x025158E0, void, p, flags);
    if (p == 0) return;
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x025158E0, dCcD_GObjTg_dt);

static void dCcD_GObjAt_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x025158F4, void, p, flags);
    if (p == 0) return;
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x025158F4, dCcD_GObjAt_dt);

/* 02515908: dCcD_GObjInf::~dCcD_GObjInf (the matcher's name for 02515980) */
static void dCcD_GObjInf_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02515908, void, p, flags);
    if (p == 0) return;
    dCcD_GObjCo_dt(p + 0xDC, 2);
    dCcD_GObjTg_dt(p + 0x94, 2);
    dCcD_GObjAt_dt(p + 0x50, 2);
    cCcD_GObjInf_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x02515908, dCcD_GObjInf_dt);

/* 02515980: dCcD_Cps::~dCcD_Cps (matcher: dCcD_GObjInf::~dCcD_GObjInf) */
static void dCcD_Cps_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02515980, void, p, flags);
    if (p == 0) return;
    st(p + 0x130, 0x1004AF60); /* cM3dGCps */
    st(p + 0x114, 0x1004AF70); /* cCcD_CpsAttr */
    cCcD_CpsAttr_dt(p + 0xF8, 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x02515980, dCcD_Cps_dt);

/* 025159F8: dCcD_Tri::~dCcD_Tri */
static void dCcD_Tri_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x025159F8, void, p, flags);
    if (p == 0) return;
    st(p + 0x128, 0x1004B058); /* cM3dGTri */
    st(p + 0x114, 0x1004B068); /* cCcD_TriAttr */
    cCcD_TriAttr_dt(p + 0xF8, 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x025159F8, dCcD_Tri_dt);

/* 02515A70: dCcD_Cyl::~dCcD_Cyl (matcher: dBgS_Acch::~dBgS_Acch) */
static void dCcD_Cyl_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02515A70, void, p, flags);
    if (p == 0) return;
    st(p + 0x12C, 0x1004B150); /* cM3dGCyl */
    st(p + 0x114, 0x1004B160); /* cCcD_CylAttr */
    cCcD_CylAttr_dt(p + 0xF8, 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x02515A70, dCcD_Cyl_dt);

/* 02515AE8: dCcD_Sph::~dCcD_Sph */
static void dCcD_Sph_dt(u32 p, s32 flags) {
    WWHD_FUNC(0x02515AE8, void, p, flags);
    if (p == 0) return;
    st(p + 0x128, 0x1004B248); /* cM3dGSph */
    st(p + 0x114, 0x1004B258); /* cCcD_SphAttr */
    cCcD_SphAttr_dt(p + 0xF8, 0);
    dCcD_GObjInf_dt(p, 0);
    if (flags & 1) operator_delete(gabi::at<void>(p));
}
VERIFY(0x02515AE8, dCcD_Sph_dt);

/* 02515B60: dCcD_GAtTgCoCommonBase::dCcD_GAtTgCoCommonBase (0x1C) */
static u32 dCcD_GAtTgCoCommonBase_ct(u32 p) {
    WWHD_FUNC(0x02515B60, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x1C));
        if (p == 0) return p;
    }
    st(p + 0x10, 0);          /* mAc */
    st(p + 0x18, 0x1004AE38); /* vtable */
    st8(p + 0x14, 0);         /* mEffCounter */
    st(p + 0xC, 0);           /* mApid */
    st(p + 0x4, 0);           /* mRPrm */
    st(p + 0x8, 0);           /* mHitCallback */
    st(p + 0x0, 0);           /* mSPrm */
    return p;
}
VERIFY(0x02515B60, dCcD_GAtTgCoCommonBase_ct);

/* 02515BBC: dCcD_GAtTgCoCommonBase::GetAc (fopAcM_SearchByID inlined) */
static u32 dCcD_GAtTgCoCommonBase_GetAc(dCcD_GObjCommon* b) {
    WWHD_FUNC(0x02515BBC, u32, b);
    u32 apid = b->mApid;
    if (apid == 0xFFFFFFFFu) return 0;
    if (b->mAc.v == 0) {
        gabi::Local<be<u32>> key;
        *key = apid;
        b->mAc.v = gabi::ea(fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get()));
        if (b->mAc.v == 0) b->mApid = 0xFFFFFFFFu;
    }
    return b->mAc.v;
}
VERIFY(0x02515BBC, dCcD_GAtTgCoCommonBase_GetAc);

/* 02515C40 */
static void dCcD_GAtTgCoCommonBase_SetEffCounterTimer(dCcD_GObjCommon* b) {
    WWHD_FUNC(0x02515C40, void, b);
    b->mEffCounter = 5;
}
VERIFY(0x02515C40, dCcD_GAtTgCoCommonBase_SetEffCounterTimer);

/* 02515C4C: dCcD_GObjAt::dCcD_GObjAt (0x44) */
static u32 dCcD_GObjAt_ct(u32 p) {
    WWHD_FUNC(0x02515C4C, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x44));
        if (p == 0) return p;
    }
    dCcD_GAtTgCoCommonBase_ct(p);
    st8(p + 0x1C, 0);         /* mSe */
    st(p + 0x18, 0x1004AE48); /* vtable */
    st8(p + 0x1E, 0);         /* mHitMark */
    st8(p + 0x1F, 0);         /* mSpl */
    st8(p + 0x1D, 0);         /* mMtrl */
    return p;
}
VERIFY(0x02515C4C, dCcD_GObjAt_ct);

/* 02515CB4: dCcD_GObjAt::Set (dCcD_SrcGObjAt: Se +0, HitMark +1, Spl +2, base SPrm +4) */
static void dCcD_GObjAt_Set(dCcD_GObjAt* at, u32 src) {
    WWHD_FUNC(0x02515CB4, void, at, src);
    at->mEffCounter = 0;
    at->mSPrm = ld(src + 4);
    at->mSe = gabi::load<u8>(src + 0);
    at->mMtrl = 0;
    at->mHitMark = gabi::load<u8>(src + 1);
    at->mSpl = gabi::load<u8>(src + 2);
}
VERIFY(0x02515CB4, dCcD_GObjAt_Set);

/* 02515CE4: dCcD_GObjTg::dCcD_GObjTg (0x48) */
static u32 dCcD_GObjTg_ct(u32 p) {
    WWHD_FUNC(0x02515CE4, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x48));
        if (p == 0) return p;
    }
    dCcD_GAtTgCoCommonBase_ct(p);
    st8(p + 0x1D, 0);
    st(p + 0x18, 0x1004AE58);
    st8(p + 0x1C, 0);
    st8(p + 0x1F, 0);
    st(p + 0x44, 0);          /* mpShieldFrontRangeYAngle */
    st8(p + 0x1E, 0);
    return p;
}
VERIFY(0x02515CE4, dCcD_GObjTg_ct);

/* 02515D50: dCcD_GObjTg::Set */
static void dCcD_GObjTg_Set(dCcD_GObjTg* tg, u32 src) {
    WWHD_FUNC(0x02515D50, void, tg, src);
    tg->mEffCounter = 0;
    tg->mSPrm = ld(src + 4);
    tg->mSe = gabi::load<u8>(src + 0);
    tg->mMtrl = 0;
    tg->mHitMark = gabi::load<u8>(src + 1);
    tg->mSpl = gabi::load<u8>(src + 2);
    tg->mHitPos.copy(*cXyz_Zero);
    tg->mpShieldFrontRangeYAngle = 0;
}
VERIFY(0x02515D50, dCcD_GObjTg_Set);

/* 02515DA0: dCcD_GStts::dCcD_GStts (0x20, vtable first) */
static u32 dCcD_GStts_ct(u32 p) {
    WWHD_FUNC(0x02515DA0, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x20));
        if (p == 0) return p;
    }
    st8(p + 0x7, 0);
    st(p + 0x0, 0x1004AE78);
    st(p + 0x1C, 0);           /* mFlag */
    st(p + 0x14, 0xFFFFFFFFu); /* mTgApid */
    st(p + 0x18, 0xFFFFFFFFu); /* mTgOldApid */
    st(p + 0xC, 0xFFFFFFFFu);  /* mAtApid */
    st8(p + 0x4, 0);           /* mAtSpl */
    st(p + 0x10, 0xFFFFFFFFu); /* mAtOldApid */
    gabi::store<u16>(p + 0x8, 0xFFFF); /* mActorPerfTblId */
    st8(p + 0x6, 0);           /* mRoomId */
    st8(p + 0x5, 0);           /* mTgSpl */
    return p;
}
VERIFY(0x02515DA0, dCcD_GStts_ct);

/* 02515E18: dCcD_GStts::Ct */
static void dCcD_GStts_Ct(u32 p) {
    WWHD_FUNC(0x02515E18, void, p);
    st8(p + 0x4, 0);
    st8(p + 0x5, 0);
    st8(p + 0x6, 0);
    gabi::store<u16>(p + 0x8, 0xFFFF);
    st(p + 0xC, 0xFFFFFFFFu);
    st(p + 0x10, 0xFFFFFFFFu);
    st(p + 0x14, 0xFFFFFFFFu);
    st(p + 0x18, 0xFFFFFFFFu);
    st(p + 0x1C, 0);
}
VERIFY(0x02515E18, dCcD_GStts_Ct);

/* 02515E50: dCcD_GStts::Move */
static void dCcD_GStts_Move(u32 p) {
    WWHD_FUNC(0x02515E50, void, p);
    u32 tg = ld(p + 0x14), at = ld(p + 0xC);
    st(p + 0x18, tg); /* mTgOldApid = mTgApid */
    st(p + 0x10, at); /* mAtOldApid = mAtApid */
    st(p + 0xC, 0);
    st(p + 0x14, 0);
}
VERIFY(0x02515E50, dCcD_GStts_Move);

/* 02515E70: dCcD_Stts::GetGStts */
static u32 dCcD_Stts_GetGStts(u32 p) {
    WWHD_FUNC(0x02515E70, u32, p);
    return p + 0x1C;
}
VERIFY(0x02515E70, dCcD_Stts_GetGStts);

/* 02515E78: dCcD_Stts::Ct */
static void dCcD_Stts_Ct(u32 p) {
    WWHD_FUNC(0x02515E78, void, p);
    cCcD_Stts_Ct(p);
    dCcD_GStts_Ct(p + 0x1C);
}
VERIFY(0x02515E78, dCcD_Stts_Ct);

/* 02515EAC: dCcD_Stts::ClrAt */
static void dCcD_Stts_ClrAt(u32 p) {
    WWHD_FUNC(0x02515EAC, void, p);
    cCcD_Stts_ClrAt(p);
    st8(p + 0x20, 0); /* mAtSpl */
}
VERIFY(0x02515EAC, dCcD_Stts_ClrAt);

/* 02515EE0: dCcD_Stts::ClrTg */
static void dCcD_Stts_ClrTg(u32 p) {
    WWHD_FUNC(0x02515EE0, void, p);
    cCcD_Stts_ClrTg(p);
    st8(p + 0x21, 0); /* mTgSpl */
}
VERIFY(0x02515EE0, dCcD_Stts_ClrTg);

/* 02515F14: dCcD_Stts::Init */
static void dCcD_Stts_Init(u32 p, s32 weight, s32 dmg, u32 actor) {
    WWHD_FUNC(0x02515F14, void, p, weight, dmg, actor);
    u32 procId = actor != 0 ? ld(actor + 4) : 0xFFFFFFFFu; /* fopAcM_GetID */
    cCcD_Stts_Init(p, weight, dmg, actor, procId);
    if (actor != 0) {
        s8 roomNo = gabi::load<s8>(actor + 0x326); /* fopAcM_GetRoomNo */
        st8(p + 0x22, roomNo != -1 ? (u8)roomNo : 0);
    } else {
        st(p + 0x38, ld(p + 0x38) | 1); /* OnNoActor */
        st8(p + 0x22, 0);
    }
}
VERIFY(0x02515F14, dCcD_Stts_Init);

/* 02515FB8: dCcD_GObjInf::dCcD_GObjInf */
static u32 dCcD_GObjInf_ct(u32 p) {
    WWHD_FUNC(0x02515FB8, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0xF8));
        if (p == 0) return p;
    }
    cCcD_GObjInf_ct(p);
    st(p + 0x3C, 0x1004AED0);
    dCcD_GObjAt_ct(p + 0x50);
    dCcD_GObjTg_ct(p + 0x94);
    dCcD_GAtTgCoCommonBase_ct(p + 0xDC); /* dCcD_GObjCo */
    st(p + 0xF4, 0x1004AE68);
    return p;
}
VERIFY(0x02515FB8, dCcD_GObjInf_ct);

/* 02516030: dCcD_GObjInf::Set (dCcD_SrcGObjInf: cCcD_SrcObj +0, At +0x1C, Tg +0x24, Co SPrm +0x2C) */
static void dCcD_GObjInf_Set(dCcD_GObjInf* inf, u32 src) {
    WWHD_FUNC(0x02516030, void, inf, src);
    cCcD_Obj_Set(gabi::ea(inf), src);
    dCcD_GObjAt_Set(&inf->mGObjAt, src + 0x1C);
    dCcD_GObjTg_Set(&inf->mGObjTg, src + 0x24);
    inf->mGObjCo.mEffCounter = 0;
    inf->mGObjCo.mSPrm = ld(src + 0x2C);
}
VERIFY(0x02516030, dCcD_GObjInf_Set);

/* 02516090 */
static u32 dCcD_GObjInf_GetGObjInf(u32 p) {
    WWHD_FUNC(0x02516090, u32, p);
    return p;
}
VERIFY(0x02516090, dCcD_GObjInf_GetGObjInf);

/* 02516094 */
static void dCcD_GObjInf_ClrAtHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516094, void, inf);
    inf->mObjAt.mRPrm = inf->mObjAt.mRPrm & ~1u; /* cCcD_ObjHitInf::ClrAtHit */
    inf->mObjAt.mHitObj = 0;
    inf->mGObjAt.mRPrm = inf->mGObjAt.mRPrm & ~3u; /* OffAtShieldHit, OffAtHitNoActor */
    inf->mGObjAt.mApid = 0xFFFFFFFFu;               /* ClrActorInfo */
    inf->mGObjAt.mAc.v = 0;
    subtract_eff_counter(inf->mGObjAt.mEffCounter.addr());
}
VERIFY(0x02516094, dCcD_GObjInf_ClrAtHit);

/* 025160DC */
static u32 dCcD_GObjInf_ChkAtHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x025160DC, u32, inf);
    if (!(inf->mObjAt.mRPrm & 1)) return 0;
    if (!(inf->mGObjAt.mRPrm & 2) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjAt) == 0) return 0;
    return 1;
}
VERIFY(0x025160DC, dCcD_GObjInf_ChkAtHit);

/* 02516138 */
static void dCcD_GObjInf_ResetAtHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516138, void, inf);
    gabi::call_ptr(ld(inf->__vtbl_hitinf + 0x34), inf); /* ClrAtHit() */
    inf->mGObjAt.mEffCounter = 0;
}
VERIFY(0x02516138, dCcD_GObjInf_ResetAtHit);

/* 02516178 */
static u32 dCcD_GObjInf_GetAtHitObj(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516178, u32, inf);
    if (!(inf->mGObjAt.mRPrm & 2) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjAt) == 0) return 0;
    return inf->mObjAt.mHitObj;
}
VERIFY(0x02516178, dCcD_GObjInf_GetAtHitObj);

/* 025161D8: GetAtHitGObj (matcher: GetTgHitGObj, a duplicate name) */
static u32 dCcD_GObjInf_GetAtHitGObj(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x025161D8, u32, inf);
    u32 obj = dCcD_GObjInf_GetAtHitObj(inf);
    if (obj == 0) return 0;
    return dCcD_GetGObjInf(obj);
}
VERIFY(0x025161D8, dCcD_GObjInf_GetAtHitGObj);

/* 02516204 */
static u8 dCcD_GObjInf_ChkAtNoGuard(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516204, u8, inf);
    return inf->mGObjAt.mSpl >= 8;
}
VERIFY(0x02516204, dCcD_GObjInf_ChkAtNoGuard);

/* 0251621C */
static void dCcD_GObjInf_ClrTgHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x0251621C, void, inf);
    inf->mObjTg.mRPrm = inf->mObjTg.mRPrm & ~1u;
    inf->mObjTg.mHitObj = 0;
    inf->mGObjTg.mRPrm = inf->mGObjTg.mRPrm & ~3u; /* OffTgHitNoActor, OffTgShieldHit */
    inf->mGObjTg.mApid = 0xFFFFFFFFu;
    inf->mGObjTg.mAc.v = 0;
    subtract_eff_counter(inf->mGObjTg.mEffCounter.addr());
}
VERIFY(0x0251621C, dCcD_GObjInf_ClrTgHit);

/* 02516264 */
static void dCcD_GObjInf_ResetTgHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516264, void, inf);
    gabi::call_ptr(ld(inf->__vtbl_hitinf + 0x3C), inf); /* ClrTgHit() */
    inf->mGObjTg.mEffCounter = 0;
}
VERIFY(0x02516264, dCcD_GObjInf_ResetTgHit);

/* 025162A4 */
static u32 dCcD_GObjInf_ChkTgHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x025162A4, u32, inf);
    if (!(inf->mObjTg.mRPrm & 1)) return 0;
    if (!(inf->mGObjTg.mRPrm & 1) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjTg) == 0) return 0;
    return 1;
}
VERIFY(0x025162A4, dCcD_GObjInf_ChkTgHit);

/* 02516300 */
static u32 dCcD_GObjInf_GetTgHitObj(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516300, u32, inf);
    if (!(inf->mGObjTg.mRPrm & 1) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjTg) == 0) return 0;
    return inf->mObjTg.mHitObj;
}
VERIFY(0x02516300, dCcD_GObjInf_GetTgHitObj);

/* 02516360 */
static u32 dCcD_GObjInf_GetTgHitGObj(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516360, u32, inf);
    u32 obj = dCcD_GObjInf_GetTgHitObj(inf);
    if (obj == 0) return 0;
    return dCcD_GetGObjInf(obj);
}
VERIFY(0x02516360, dCcD_GObjInf_GetTgHitGObj);

/* 0251638C */
static u8 dCcD_GObjInf_GetTgHitObjSe(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x0251638C, u8, inf);
    dCcD_GObjInf* hit = gabi::at<dCcD_GObjInf>(dCcD_GObjInf_GetTgHitGObj(inf));
    if (hit == nullptr) return 0;
    return hit->mGObjAt.mSe;
}
VERIFY(0x0251638C, dCcD_GObjInf_GetTgHitObjSe);

/* 025163BC */
static u32 dCcD_GObjInf_GetCoHitObj(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x025163BC, u32, inf);
    if (!(inf->mGObjCo.mRPrm & 1) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjCo) == 0) return 0;
    return inf->mObjCo.mHitObj;
}
VERIFY(0x025163BC, dCcD_GObjInf_GetCoHitObj);

/* 0251641C */
static void dCcD_GObjInf_ClrCoHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x0251641C, void, inf);
    inf->mObjCo.mRPrm = inf->mObjCo.mRPrm & ~1u;
    inf->mObjCo.mHitObj = 0;
    inf->mGObjCo.mRPrm = inf->mGObjCo.mRPrm & ~1u; /* OffCoHitNoActor */
    inf->mGObjCo.mApid = 0xFFFFFFFFu;
    inf->mGObjCo.mAc.v = 0;
    subtract_eff_counter(inf->mGObjCo.mEffCounter.addr());
}
VERIFY(0x0251641C, dCcD_GObjInf_ClrCoHit);

/* 02516464 */
static u32 dCcD_GObjInf_ChkCoHit(dCcD_GObjInf* inf) {
    WWHD_FUNC(0x02516464, u32, inf);
    if (!(inf->mObjCo.mRPrm & 1)) return 0;
    if (!(inf->mGObjCo.mRPrm & 1) && dCcD_GAtTgCoCommonBase_GetAc(&inf->mGObjCo) == 0) return 0;
    return 1;
}
VERIFY(0x02516464, dCcD_GObjInf_ChkCoHit);

/* 025164C0: dCcD_Cps::Set (dCcD_SrcCps: dCcD_SrcGObjInf 0x30, then cM3dGCpsS) */
static void dCcD_Cps_Set(dCcD_GObjInf* cps, u32 src) {
    WWHD_FUNC(0x025164C0, void, cps, src);
    dCcD_GObjInf_Set(cps, src);
    cM3dGCps_Set(gabi::ea(cps) + 0x118, src + 0x30);
}
VERIFY(0x025164C0, dCcD_Cps_Set);

/* 02516504: dCcD_Cps::GetShapeAttr */
static u32 dCcD_Cps_GetShapeAttr(u32 p) {
    WWHD_FUNC(0x02516504, u32, p);
    return p + 0xF8;
}
VERIFY(0x02516504, dCcD_Cps_GetShapeAttr);

/* 0251650C: dCcD_Tri::Set */
static void dCcD_Tri_Set(dCcD_GObjInf* tri, u32 src) {
    WWHD_FUNC(0x0251650C, void, tri, src);
    dCcD_GObjInf_Set(tri, src);
}
VERIFY(0x0251650C, dCcD_Tri_Set);

/* 02516510: dCcD_Tri::GetShapeAttr */
static u32 dCcD_Tri_GetShapeAttr(u32 p) {
    WWHD_FUNC(0x02516510, u32, p);
    return p + 0xF8;
}
VERIFY(0x02516510, dCcD_Tri_GetShapeAttr);

/* 02516518: dCcD_Cyl::Set */
static void dCcD_Cyl_Set(dCcD_GObjInf* cyl, u32 src) {
    WWHD_FUNC(0x02516518, void, cyl, src);
    dCcD_GObjInf_Set(cyl, src);
    cM3dGCyl_Set(gabi::ea(cyl) + 0x118, src + 0x30);
}
VERIFY(0x02516518, dCcD_Cyl_Set);

/* 0251655C: dCcD_Cyl::StartCAt */
static void dCcD_Cyl_StartCAt(dCcD_GObjInf* cyl, cXyz* pos) {
    WWHD_FUNC(0x0251655C, void, cyl, pos);
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    cyl->mGObjAt.mVec.x = x; /* SetAtVec(cXyz::Zero) */
    cyl->mGObjAt.mVec.z = z;
    cyl->mGObjAt.mVec.y = y;
    cM3dGCyl_SetC(gabi::ea(cyl) + 0x118, gabi::ea(pos));
}
VERIFY(0x0251655C, dCcD_Cyl_StartCAt);

/* 02516580: dCcD_Cyl::StartCTg */
static void dCcD_Cyl_StartCTg(dCcD_GObjInf* cyl, cXyz* pos) {
    WWHD_FUNC(0x02516580, void, cyl, pos);
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    cyl->mGObjTg.mVec.x = x;
    cyl->mGObjTg.mVec.z = z;
    cyl->mGObjTg.mVec.y = y;
    cM3dGCyl_SetC(gabi::ea(cyl) + 0x118, gabi::ea(pos));
}
VERIFY(0x02516580, dCcD_Cyl_StartCTg);

/* 025165A4: dCcD_Cyl::MoveCAtTg */
static void dCcD_Cyl_MoveCAtTg(dCcD_GObjInf* cyl, cXyz* pos) {
    WWHD_FUNC(0x025165A4, void, cyl, pos);
    gabi::Local<cXyz> vel;
    cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(cyl) + 0x118); /* pos - *GetCP() */
    cyl->mGObjAt.mVec.copy(*vel);
    cyl->mGObjTg.mVec.copy(*vel);
    cM3dGCyl_SetC(gabi::ea(cyl) + 0x118, gabi::ea(pos));
}
VERIFY(0x025165A4, dCcD_Cyl_MoveCAtTg);

/* 02516618: dCcD_Cyl::MoveCAt */
static void dCcD_Cyl_MoveCAt(dCcD_GObjInf* cyl, cXyz* pos) {
    WWHD_FUNC(0x02516618, void, cyl, pos);
    gabi::Local<cXyz> vel;
    cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(cyl) + 0x118);
    cyl->mGObjAt.mVec.copy(*vel);
    cM3dGCyl_SetC(gabi::ea(cyl) + 0x118, gabi::ea(pos));
}
VERIFY(0x02516618, dCcD_Cyl_MoveCAt);

/* 02516680: dCcD_Cyl::MoveCTg */
static void dCcD_Cyl_MoveCTg(dCcD_GObjInf* cyl, cXyz* pos) {
    WWHD_FUNC(0x02516680, void, cyl, pos);
    gabi::Local<cXyz> vel;
    cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(cyl) + 0x118);
    cyl->mGObjTg.mVec.copy(*vel);
    cM3dGCyl_SetC(gabi::ea(cyl) + 0x118, gabi::ea(pos));
}
VERIFY(0x02516680, dCcD_Cyl_MoveCTg);

/* 025166E8: dCcD_Cyl::GetShapeAttr */
static u32 dCcD_Cyl_GetShapeAttr(u32 p) {
    WWHD_FUNC(0x025166E8, u32, p);
    return p + 0xF8;
}
VERIFY(0x025166E8, dCcD_Cyl_GetShapeAttr);

/* 025166F0: dCcD_Sph::dCcD_Sph (0x12C) */
static u32 dCcD_Sph_ct(u32 p) {
    WWHD_FUNC(0x025166F0, u32, p);
    if (p == 0) {
        p = gabi::ea(operator_new(0x12C));
        if (p == 0) return p;
    }
    dCcD_GObjInf_ct(p);
    st(p + 0x114, 0x100015A8); /* cCcD_ShapeAttr */
    st(p + 0x110, 0x1004AE14); /* cM3dGAab (this unit's copy) */
    cM3dGSph_ct(p + 0x118);
    st(p + 0x3C, 0x1004B200);  /* dCcD_Sph */
    st(p + 0x128, 0x1004B248); /* cM3dGSph */
    st(p + 0x114, 0x1004B258); /* cCcD_SphAttr */
    return p;
}
VERIFY(0x025166F0, dCcD_Sph_ct);

/* 0251677C: dCcD_Sph::Set */
static void dCcD_Sph_Set(dCcD_GObjInf* sph, u32 src) {
    WWHD_FUNC(0x0251677C, void, sph, src);
    dCcD_GObjInf_Set(sph, src);
    cM3dGSph_Set(gabi::ea(sph) + 0x118, src + 0x30);
}
VERIFY(0x0251677C, dCcD_Sph_Set);

/* 025167C0: dCcD_Sph::StartCAt */
static void dCcD_Sph_StartCAt(dCcD_GObjInf* sph, cXyz* pos) {
    WWHD_FUNC(0x025167C0, void, sph, pos);
    cXyz* zero = cXyz_Zero;
    f32 x = zero->x, z = zero->z, y = zero->y;
    sph->mGObjAt.mVec.x = x;
    sph->mGObjAt.mVec.z = z;
    sph->mGObjAt.mVec.y = y;
    cM3dGSph_SetC(gabi::ea(sph) + 0x118, gabi::ea(pos));
}
VERIFY(0x025167C0, dCcD_Sph_StartCAt);

/* 025167E4: dCcD_Sph::MoveCAt */
static void dCcD_Sph_MoveCAt(dCcD_GObjInf* sph, cXyz* pos) {
    WWHD_FUNC(0x025167E4, void, sph, pos);
    gabi::Local<cXyz> vel;
    cXyz_sub(gabi::ea(pos), gabi::ea(vel.get()), gabi::ea(sph) + 0x118);
    sph->mGObjAt.mVec.copy(*vel);
    cM3dGSph_SetC(gabi::ea(sph) + 0x118, gabi::ea(pos));
}
VERIFY(0x025167E4, dCcD_Sph_MoveCAt);

/* 0251684C: dCcD_Sph::GetShapeAttr */
static u32 dCcD_Sph_GetShapeAttr(u32 p) {
    WWHD_FUNC(0x0251684C, u32, p);
    return p + 0xF8;
}
VERIFY(0x0251684C, dCcD_Sph_GetShapeAttr);

/* 02516854: static initialisers (header statics) */
static void __sinit_d_cc_d_cpp() {
    WWHD_FUNC(0x02516854, void, (u32)0);
    sinit_header_statics(0x1046EFC8, 0x101D5718);
}
VERIFY(0x02516854, __sinit_d_cc_d_cpp);

}  // namespace d_cc_d_cpp
