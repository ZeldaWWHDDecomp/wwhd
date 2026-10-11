// WWHD collision-data primitives.
#include "SSystem/SComponent/c_cc_d.h"
using namespace gabi;
using namespace CcDataHD;
// HD: guest vtables and diagnostic strings have fixed image addresses.
enum GuestAddress : u32 {
    kPlaneVtable = 0x1000136C,
    kBgTriangleVtable = 0x100011DC,
    kBgNodeVtable = 0x100011FC,
    kCapsuleVtable = 0x10001718,
    kTriangleVtable = 0x10001728,
    kCylinderVtable = 0x100017D8,
    kSphereVtable = 0x100017E8,
    kDivideInfoVtable = 0x10001520,
    kDivideAreaVtable = 0x100017F8,
    kStatusVtable = 0x10001530,
    kAttackVtable = 0x10001568,
    kTargetVtable = 0x10001578,
    kHitInfoVtable = 0x10001598,
    kCollisionVtable = 0x10001588,
    kObjectVtable = 0x10001648,
    kAssertFile = 0x10001398,
    kMoveXAssertion = 0x100013A4,
    kMoveYAssertion = 0x100013EC,
    kMoveZAssertion = 0x10001434,
    kMoveRangeAssertion = 0x1000147C,
};

u32 ccNew(u32);
void ccDelete(u32);
void constructPlane(u32);
void destroyCapsule(u32, u32);
void aabSet(u32, u32, u32);
void aabClear(u32);
void aabSetMinMax(u32, u32);
void aabPlusRadius(u32, f32);
void constructAttack(u32);
void constructTarget(u32);
void constructDivideInfo(u32);
void PSVECSubtract(u32, u32, u32);
f32 PSVECDotProduct(u32, u32);
void PSVECScale(u32, u32, f32);
void PSVECAdd(u32, u32, u32);
f32 PSVECMag(u32);
void PSVECNormalize(u32, u32);
void ccAssert(u32, u32, u32);
void registerGlobal(u32);
void constructHeaderByte(u32);
void constructHeaderFlag(u32);
u32 crossCpsCps(u32, u32, u32);
u32 crossCpsCyl(u32, u32, u32);
u32 crossCpsSph(u32, u32, u32);
u32 crossTriTri(u32, u32, u32);
u32 crossTriCyl(u32, u32, u32);
u32 crossSphTri(u32, u32, u32);
u32 crossCpsTri(u32, u32, u32);
u32 crossCylCylAtTg(u32, u32, u32);
u32 crossCylSphAtTg(u32, u32, u32);
u32 crossCylCylCo(u32, u32, u32);
u32 crossSphCylCo(u32, u32, u32);
u32 crossSphCylAtTg(u32, u32, u32);
u32 crossSphSphAtTg(u32, u32, u32);
u32 crossSphSphCo(u32, u32, u32);
u32 crossCpsTriangleCopy(u32, u32, u32);
using CcVector = Vector;
using CcTriangle = Triangle;
static void copyVector(u32 dst, u32 src) {
    auto* source = gabi::at<WordVector>(src);
    auto* target = gabi::at<WordVector>(dst);
    u32 x = source->mCoord[0], y = source->mCoord[1], z = source->mCoord[2];
    target->mCoord[0] = x;
    target->mCoord[1] = y;
    target->mCoord[2] = z;
}
static void floatVector(u32 dst, u32 src) {
    auto* source = gabi::at<Vector>(src);
    auto* target = gabi::at<Vector>(dst);
    f32 x = source->mCoord[0], y = source->mCoord[1], z = source->mCoord[2];
    target->mCoord[0] = x;
    target->mCoord[1] = y;
    target->mCoord[2] = z;
}
// Espresso slw uses the low six shift bits, and returns zero when bit five is set.
static u32 shiftOne(u32 n) { return (n & 32) ? 0u : (1u << (n & 31)); }
static u32 divideAxis(u32 p, u32 box, u32 axis, u32 last, bool rejectOutside) {
    auto* area = gabi::at<cCcD_DivideArea>(p);
    auto* bounds = gabi::at<Aab>(box);
    auto* partition = &area->mAxis[axis];
    if (partition->mDiffIsZero)
        return (1u << (last + 1)) - 1;
    f32 origin = area->mAab.mMin.mCoord[axis], low = bounds->mMin.mCoord[axis],
        high = bounds->mMax.mCoord[axis], inv = partition->mInvScaledDiff;
    s32 lo = ftoi((low - origin) * inv), hi = ftoi((high - origin) * inv);
    if (rejectOutside && ((lo < 0 && hi < 0) || (lo > (s32)last && hi > (s32)last)))
        return 0;
    if (hi > (s32)last)
        hi = (s32)last;
    u32 mask = shiftOne((u32)hi + 1u) - 1u;
    if (lo > 0)
        mask &= ~(shiftOne((u32)lo - 1u) - 1u);
    return mask;
}
// HD: overload-specific free adapters retain the verified ABI; autoglue cannot register overloaded methods.
// Shape attributes contain a geometry object at +0x20; its triangle base
// copies normal/d through floating loads and vertices through integer loads.
static void makeTriangle(u32 dst, u32 shape) {
    auto* triangle = gabi::at<Triangle>(dst);
    auto* source = gabi::at<cCcD_TriAttr>(shape);
    for (u32 i = 0; i < 4; i++)
        triangle->mPlane.mCoefficients[i] = source->mTri.mPlane.mCoefficients[i].get();
    triangle->mPlane.mVtable = kPlaneVtable;
    for (u32 i = 0; i < 9; i++)
        triangle->mVertex[i / 3].mCoord[i % 3] = source->mTri.mVertex[i / 3].mCoord[i % 3].get();
}

// HD: constants and shared header initialization retain their original guest storage.
static f32 kZero;
static f32 kXZPartitionScale;
static f32 kOne;
static f32 kYPartitionScale;
static f32 kEpsilon;
static f32 kRangeMin;
static f32 kRangeMax;
static f32 kNegativeOne;
static f32 kPiMin;
static f32 kPiMax;
static HeaderZero sHeaderZero;
static HeaderStatics sHeaderStatics;
static Vector sZeroVector;
static Vector sVirtualCenter;
static DestructorRecord sHeaderZeroDestructor;
static DestructorRecord sHeaderByteDestructor;
static DestructorRecord sHeaderFlagDestructor;

/* 0200B56C */
u32 cBgW_TriElm__ct(u32 p) {
    if (!p)
        p = ccNew(24u);
    if (p) {
        gabi::at<cBgW_TriElm>(p)->mVtable = kBgTriangleVtable;
        constructPlane(p);
    }
    return p;
}

/* 0200B5C0 */
u32 cBgW_NodeTree__ct(u32 p) {
    if (!p)
        p = ccNew(28u);
    if (p) {
        gabi::at<cBgW_NodeTree>(p)->mAab.mVtable = kBgNodeVtable;
    }
    return p;
}

/* 0200B600 */
void cCcD_CpsAttr__dt(u32 p, u32 flags) {
    if (!p)
        return;
    gabi::at<cCcD_CpsAttr>(p)->mCps.mVtable = kCapsuleVtable;
    destroyCapsule(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps), 0u);
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B660 */
void cCcD_TriAttr__dt(u32 p, u32 flags) {
    if (!p)
        return;
    gabi::at<cCcD_TriAttr>(p)->mTri.mPlane.mVtable = kTriangleVtable;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B680 */
void cCcD_CylAttr__dt(u32 p, u32 flags) {
    if (!p)
        return;
    gabi::at<cCcD_CylAttr>(p)->mCyl.mVtable = kCylinderVtable;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B6A0 */
void cCcD_SphAttr__dt(u32 p, u32 flags) {
    if (!p)
        return;
    gabi::at<cCcD_SphAttr>(p)->mSph.mVtable = kSphereVtable;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B6C0 */
u32 cCcD_DivideInfo__ct(u32 p) {
    if (!p)
        p = ccNew(8u);
    if (p) {
        gabi::at<cCcD_DivideInfo>(p)->mRangeBits = 0u;
        gabi::at<cCcD_DivideInfo>(p)->mVtable = kDivideInfoVtable;
    }
    return p;
}

/* 0200B708 */
void cCcD_DivideInfo__dt(u32 p, u32 flags) {
    if (!p)
        return;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B7C0 */
void cCcD_DivideArea__dt(u32 p, u32 flags) {
    if (!p)
        return;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200BD18 */
void cCcD_GStts__dt(u32 p, u32 flags) {
    if (!p)
        return;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200BD88 */
void cCcD_Stts__dt(u32 p, u32 flags) {
    if (!p)
        return;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200C110 */
void cCcD_GObjInf__dt(u32 p, u32 flags) {
    if (!p)
        return;
    if (flags & 1u)
        ccDelete(p);
}

/* 0200B71C */
u32 CcDataHD::cCcD_DivideInfo::Chk(u32 q) {
    u32 m = mRangeBits.get() & gabi::at<cCcD_DivideInfo>(q)->mRangeBits.get();
    return (m & 0x7FFu) && (m & 0xFFE00000u) && (m & 0x1FF800u);
}

/* 0200B750 */
u32 cCcD_DivideArea__ct(u32 p) {
    if (!p)
        p = ccNew(64u);
    if (p) {
        f32 z = kZero.get();
        gabi::at<cCcD_DivideArea>(p)->mAxis[0].mDiffIsZero = 0;
        gabi::at<cCcD_DivideArea>(p)->mAxis[1].mScaledDiff = z;
        gabi::at<cCcD_DivideArea>(p)->mAab.mVtable = kDivideAreaVtable;
        gabi::at<cCcD_DivideArea>(p)->mAxis[0].mInvScaledDiff = z;
        gabi::at<cCcD_DivideArea>(p)->mAxis[1].mDiffIsZero = 0;
        gabi::at<cCcD_DivideArea>(p)->mAxis[1].mInvScaledDiff = z;
        gabi::at<cCcD_DivideArea>(p)->mAxis[2].mDiffIsZero = 0;
        gabi::at<cCcD_DivideArea>(p)->mAxis[0].mScaledDiff = z;
        gabi::at<cCcD_DivideArea>(p)->mAxis[2].mInvScaledDiff = z;
        gabi::at<cCcD_DivideArea>(p)->mAxis[2].mScaledDiff = z;
    }
    return p;
}

/* 0200B7D4 */
void CcDataHD::cCcD_DivideArea::SetArea(u32 box) {
    const u32 p = gabi::ea(this);
    aabSet(p, box, box + 12);
    f32 x = (mAab.mMax.mCoord[0].get() - mAab.mMin.mCoord[0].get()) * kXZPartitionScale.get(),
        eps = kEpsilon.get(), one = kOne.get();
    mAxis[0].mScaledDiff = x;
    mAxis[0].mDiffIsZero = std::fabs(x) < eps;
    if (!(std::fabs(x) < eps))
        mAxis[0].mInvScaledDiff = one / mAxis[0].mScaledDiff.get();
    f32 y = (mAab.mMax.mCoord[1].get() - mAab.mMin.mCoord[1].get()) * kYPartitionScale.get();
    mAxis[1].mScaledDiff = y;
    mAxis[1].mDiffIsZero = std::fabs(y) < eps;
    if (!(std::fabs(y) < eps))
        mAxis[1].mInvScaledDiff = one / mAxis[1].mScaledDiff.get();
    f32 z = (mAab.mMax.mCoord[2].get() - mAab.mMin.mCoord[2].get()) * kXZPartitionScale.get();
    mAxis[2].mScaledDiff = z;
    mAxis[2].mDiffIsZero = std::fabs(z) < eps;
    if (!(std::fabs(z) < eps))
        mAxis[2].mInvScaledDiff = one / mAxis[2].mScaledDiff.get();
}

/* 0200B8CC */
void CcDataHD::cCcD_DivideArea::CalcDivideInfo(u32 out, u32 box, u32 all) {
    const u32 p = gabi::ea(this);
    if (all) {
        gabi::at<cCcD_DivideInfo>(out)->mRangeBits = 0xFFFFFFFFu;
        return;
    }
    u32 m = divideAxis(p, box, 0, 10, false);
    m |= divideAxis(p, box, 1, 9, false) << 11;
    m |= divideAxis(p, box, 2, 10, false) << 21;
    gabi::at<cCcD_DivideInfo>(out)->mRangeBits = m;
}

/* 0200BAC0 */
void CcDataHD::cCcD_DivideArea::CalcDivideInfoOverArea(u32 out, u32 box) {
    const u32 p = gabi::ea(this);
    u32 m = divideAxis(p, box, 0, 10, true);
    m |= divideAxis(p, box, 1, 9, true) << 11;
    m |= divideAxis(p, box, 2, 10, true) << 21;
    gabi::at<cCcD_DivideInfo>(out)->mRangeBits = m;
}

/* 0200BD2C */
u32 cCcD_Stts__ct(u32 p) {
    if (!p)
        p = ccNew(28u);
    if (p) {
        gabi::at<cCcD_Stts>(p)->m_apid = 0u;
        gabi::at<cCcD_Stts>(p)->mVtable = kStatusVtable;
        gabi::at<cCcD_Stts>(p)->mReserved = 0;
        gabi::at<cCcD_Stts>(p)->mp_actor = 0u;
        gabi::at<cCcD_Stts>(p)->mInitFlags = 0;
        gabi::at<cCcD_Stts>(p)->m_dmg = 0;
        gabi::at<cCcD_Stts>(p)->m_weight = 0;
    }
    return p;
}

/* 0200BD9C */
void CcDataHD::cCcD_Stts::Ct() {
    f32 z = kZero.get();
    mp_actor = 0u;
    m_apid = 0xFFFFFFFFu;
    m_weight = 0;
    m_cc_move.mCoord[0] = z;
    mInitFlags = 0;
    m_cc_move.mCoord[1] = z;
    m_dmg = 0;
    m_cc_move.mCoord[2] = z;
}

/* 0200BDD0 */
void CcDataHD::cCcD_Stts::Init(s32 weight, s32 flags, u32 actor, u32 id) {
    const u32 p = gabi::ea(this);
    u32 v = mVtable.get(), fn = gabi::at<StatusVtable>(v)->Ct.get();
    gabi::call_ptr<void>(fn, p, weight, flags, actor, id);
    m_weight = (u8)weight;
    mInitFlags = (u8)flags;
    mp_actor = actor;
    m_apid = id;
}

/* 0200BE28 */
void CcDataHD::cCcD_Stts::PlusCcMove(f32 x, f32 y, f32 z) {
    f32 a = m_cc_move.mCoord[0].get() + x, c = m_cc_move.mCoord[2].get() + z,
        b = m_cc_move.mCoord[1].get() + y;
    m_cc_move.mCoord[2] = c;
    m_cc_move.mCoord[0] = a;
    m_cc_move.mCoord[1] = b;
    if (std::isnan(a))
        ccAssert(kAssertFile, 0x1E7u, kMoveXAssertion);
    if (std::isnan(m_cc_move.mCoord[1].get()))
        ccAssert(kAssertFile, 0x1E8u, kMoveYAssertion);
    if (std::isnan(m_cc_move.mCoord[2].get()))
        ccAssert(kAssertFile, 0x1E9u, kMoveZAssertion);
    f32 lo = kRangeMin.get(), hi = kRangeMax.get();
    if (!(lo < m_cc_move.mCoord[0].get() && m_cc_move.mCoord[0].get() < hi &&
          lo < m_cc_move.mCoord[1].get() && m_cc_move.mCoord[1].get() < hi &&
          lo < m_cc_move.mCoord[2].get() && m_cc_move.mCoord[2].get() < hi))
        ccAssert(kAssertFile, 0x1EBu, kMoveRangeAssertion);
}

/* 0200BF6C */
void CcDataHD::cCcD_Stts::ClrTg() { m_dmg = 0; }

/* 0200BF78 */
u32 cCcD_ObjAt__ct(u32 p) {
    if (!p)
        p = ccNew(24u);
    if (p) {
        gabi::at<cCcD_ObjAt>(p)->mBase.mHitObj = 0u;
        gabi::at<cCcD_ObjAt>(p)->mBase.mVtable = kAttackVtable;
        gabi::at<cCcD_ObjAt>(p)->mType = 0u;
        gabi::at<cCcD_ObjAt>(p)->mAtp = 0;
        gabi::at<cCcD_ObjAt>(p)->mBase.mRPrm = 0u;
        gabi::at<cCcD_ObjAt>(p)->mBase.mSPrm = 0u;
    }
    return p;
}

/* 0200BFD0 */
u32 cCcD_ObjTg__ct(u32 p) {
    if (!p)
        p = ccNew(20u);
    if (p) {
        gabi::at<cCcD_ObjTg>(p)->mType = 0u;
        gabi::at<cCcD_ObjTg>(p)->mBase.mVtable = kTargetVtable;
        gabi::at<cCcD_ObjTg>(p)->mBase.mRPrm = 0u;
        gabi::at<cCcD_ObjTg>(p)->mBase.mHitObj = 0u;
        gabi::at<cCcD_ObjTg>(p)->mBase.mSPrm = 0u;
    }
    return p;
}

/* 0200C034 */
u32 cCcD_Obj__ct(u32 p) {
    if (!p)
        p = ccNew(80u);
    if (p) {
        gabi::at<cCcD_Obj>(p)->mVtable = kHitInfoVtable;
        constructAttack(p);
        constructTarget(gabi::ea(&gabi::at<cCcD_Obj>(p)->mObjTg));
        gabi::at<cCcD_Obj>(p)->mObjCo.mSPrm = 0u;
        gabi::at<cCcD_Obj>(p)->mObjCo.mRPrm = 0u;
        gabi::at<cCcD_Obj>(p)->mObjCo.mHitObj = 0u;
        gabi::at<cCcD_Obj>(p)->mStts = 0u;
        gabi::at<cCcD_Obj>(p)->mObjCo.mVtable = kCollisionVtable;
        gabi::at<cCcD_Obj>(p)->mVtable = kObjectVtable;
        gabi::at<cCcD_Obj>(p)->mFlags = 0u;
        constructDivideInfo(gabi::ea(&gabi::at<cCcD_Obj>(p)->mDivideInfo));
        gabi::at<cCcD_Obj>(p)->mFlags = 0u;
    }
    return p;
}

/* 0200C0D4 */
void CcDataHD::cCcD_Obj::Set(u32 q) {
    mFlags = gabi::at<cCcD_SrcObj>(q)->mFlags.get();
    mObjAt.mBase.mSPrm = gabi::at<cCcD_SrcObj>(q)->mAtSPrm.get();
    mObjAt.mType = gabi::at<cCcD_SrcObj>(q)->mAtType.get();
    mObjAt.mAtp = gabi::at<cCcD_SrcObj>(q)->mAtp.get();
    mObjTg.mBase.mSPrm = gabi::at<cCcD_SrcObj>(q)->mTgSPrm.get();
    mObjTg.mType = gabi::at<cCcD_SrcObj>(q)->mTgType.get();
    mObjCo.mSPrm = gabi::at<cCcD_SrcObj>(q)->mCoSPrm.get();
}

/* 0200C124 */
u32 cCcD_CpsAttr_CrossAtTg_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mAtCps.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C20C */
u32 cCcD_CpsAttr_CrossCo_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mCoCps.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C548 */
u32 cCcD_TriAttr_CrossAtTg_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mAtTri.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C63C */
u32 cCcD_TriAttr_CrossCo_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mCoTri.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C79C */
u32 cCcD_CylAttr_CrossAtTg_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mAtCyl.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C7EC */
u32 cCcD_CylAttr_CrossCo_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mCoCyl.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200CA18 */
u32 cCcD_SphAttr_CrossAtTg_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mAtSph.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200CA68 */
u32 cCcD_SphAttr_CrossCo_Shape(u32 p, u32 q, u32 out) {
    u32 v = gabi::at<cCcD_ShapeAttr>(q)->mVtable.get(), fn = gabi::at<ShapeVtable>(v)->mCoSph.get();
    return gabi::call_ptr<u32>(fn, q, p, out);
}

/* 0200C140 */
u32 cCcD_CpsAttr_CrossAtTg_Cps(u32 p, u32 q, u32 out) {
    return crossCpsCps(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps), out);
}

/* 0200C14C */
u32 cCcD_CpsAttr_CrossAtTg_Cyl(u32 p, u32 q, u32 out) {
    return crossCpsCyl(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), out);
}

/* 0200C200 */
u32 cCcD_CpsAttr_CrossAtTg_Sph(u32 p, u32 q, u32 out) {
    return crossCpsSph(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), out);
}

/* 0200C614 */
u32 cCcD_TriAttr_CrossAtTg_Tri(u32 p, u32 q, u32 out) {
    return crossTriTri(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri),
                       gabi::ea(&gabi::at<cCcD_TriAttr>(q)->mTri), out);
}

/* 0200C620 */
u32 cCcD_TriAttr_CrossAtTg_Cyl(u32 p, u32 q, u32 out) {
    return crossTriCyl(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri),
                       gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), out);
}

/* 0200C62C */
u32 cCcD_TriAttr_CrossAtTg_Sph(u32 p, u32 q, u32 out) {
    return crossSphTri(gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph),
                       gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri), out);
}

/* 0200C7B8 */
u32 cCcD_CylAttr_CrossAtTg_Cps(u32 p, u32 q, u32 out) {
    return crossCpsCyl(gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps),
                       gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl), out);
}

/* 0200C7C8 */
u32 cCcD_CylAttr_CrossAtTg_Tri(u32 p, u32 q, u32 out) {
    return crossCpsTri(gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl),
                       gabi::ea(&gabi::at<cCcD_TriAttr>(q)->mTri), out);
}

/* 0200C7D4 */
u32 cCcD_CylAttr_CrossAtTg_Cyl(u32 p, u32 q, u32 out) {
    return crossCylCylAtTg(gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl),
                           gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), out);
}

/* 0200C7E0 */
u32 cCcD_CylAttr_CrossAtTg_Sph(u32 p, u32 q, u32 out) {
    return crossCylSphAtTg(gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl),
                           gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), out);
}

/* 0200C84C */
u32 cCcD_CylAttr_CrossCo_Cyl(u32 p, u32 q, u32 out) {
    return crossCylCylCo(gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl),
                         gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), out);
}

/* 0200C858 */
u32 cCcD_CylAttr_CrossCo_Sph(u32 p, u32 q, u32 out) {
    return crossSphCylCo(gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl),
                         gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), out);
}

/* 0200CA34 */
u32 cCcD_SphAttr_CrossAtTg_Cps(u32 p, u32 q, u32 out) {
    return crossCpsSph(gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps),
                       gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph), out);
}

/* 0200CA44 */
u32 cCcD_SphAttr_CrossAtTg_Tri(u32 p, u32 q, u32 out) {
    return crossSphTri(gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph),
                       gabi::ea(&gabi::at<cCcD_TriAttr>(q)->mTri), out);
}

/* 0200CA50 */
u32 cCcD_SphAttr_CrossAtTg_Cyl(u32 p, u32 q, u32 out) {
    return crossSphCylAtTg(gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph),
                           gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), out);
}

/* 0200CA5C */
u32 cCcD_SphAttr_CrossAtTg_Sph(u32 p, u32 q, u32 out) {
    return crossSphSphAtTg(gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph),
                           gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), out);
}

/* 0200CAC8 */
u32 cCcD_SphAttr_CrossCo_Cyl(u32 p, u32 q, u32 out) {
    return crossSphCylCo(gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl),
                         gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph), out);
}

/* 0200CAD8 */
u32 cCcD_SphAttr_CrossCo_Sph(u32 p, u32 q, u32 out) {
    return crossSphSphCo(gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph),
                         gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), out);
}

/* 0200C228 */
u32 cCcD_CpsAttr_CrossCo_Cps(u32 p, u32 q, u32 out) {
    Local<CcVector> hit;
    gabi::at<Vector>(out)->mCoord[0] = kZero.get();
    return crossCpsCps(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps), hit.a);
}

/* 0200C268 */
u32 cCcD_CpsAttr_CrossCo_Cyl(u32 p, u32 q, u32 out) {
    Local<CcVector> hit;
    gabi::at<Vector>(out)->mCoord[0] = kZero.get();
    return crossCpsCyl(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_CylAttr>(q)->mCyl), hit.a);
}

/* 0200C2A0 */
u32 cCcD_CpsAttr_CrossCo_Sph(u32 p, u32 q, u32 out) {
    Local<CcVector> hit;
    gabi::at<Vector>(out)->mCoord[0] = kZero.get();
    return crossCpsSph(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps),
                       gabi::ea(&gabi::at<cCcD_SphAttr>(q)->mSph), hit.a);
}

/* 0200C808 */
u32 cCcD_CylAttr_CrossCo_Cps(u32 p, u32 q, u32 out) {
    Local<CcVector> hit;
    gabi::at<Vector>(out)->mCoord[0] = kZero.get();
    return crossCpsCyl(gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps),
                       gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl), hit.a);
}

/* 0200CA84 */
u32 cCcD_SphAttr_CrossCo_Cps(u32 p, u32 q, u32 out) {
    Local<CcVector> hit;
    gabi::at<Vector>(out)->mCoord[0] = kZero.get();
    return crossCpsSph(gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps),
                       gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph), hit.a);
}

/* 0200C158 */
u32 cCcD_CpsAttr_CrossAtTg_Tri(u32 p, u32 q, u32 out) {
    Local<CcTriangle> tri;
    makeTriangle(tri.a, q);
    return crossCpsTriangleCopy(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps), tri.a, out);
}

/* 0200C564 */
u32 cCcD_TriAttr_CrossAtTg_Cps(u32 p, u32 q, u32 out) {
    Local<CcTriangle> tri;
    makeTriangle(tri.a, p);
    return crossCpsTriangleCopy(gabi::ea(&gabi::at<cCcD_CpsAttr>(q)->mCps), tri.a, out);
}

/* 0200C024 */
u32 cCcD_ShapeAttr_CrossAtTg_Unsupported() { return 0; }

/* 0200C02C */
u32 cCcD_ShapeAttr_CrossCo_Unsupported() { return 0; }

/* 0200C260 */
u32 cCcD_CpsAttr_CrossCo_Tri() { return 0; }

/* 0200C658 */
u32 cCcD_TriAttr_CrossCo_Cps() { return 0; }

/* 0200C660 */
u32 cCcD_TriAttr_CrossCo_Tri() { return 0; }

/* 0200C668 */
u32 cCcD_TriAttr_CrossCo_Cyl() { return 0; }

/* 0200C670 */
u32 cCcD_TriAttr_CrossCo_Sph() { return 0; }

/* 0200C844 */
u32 cCcD_CylAttr_CrossCo_Tri() { return 0; }

/* 0200CAC0 */
u32 cCcD_SphAttr_CrossCo_Tri() { return 0; }

/* 0200CCE4 */
u32 cCcD_Stts_GetGStts() { return 0; }

/* 0200CCFC */
u32 cCcD_Obj_GetGObjInf() { return 0; }

/* 0200CD04 */
u32 cCcD_Obj_GetShapeAttr() { return 0; }

/* 0200C794 */
u32 CcDataHD::cCcD_CylAttr::GetCoCP() {
    const u32 p = gabi::ea(this);
    return gabi::ea(&gabi::at<cCcD_CylAttr>(p)->mCyl);
}

/* 0200CA10 */
u32 CcDataHD::cCcD_SphAttr::GetCoCP() {
    const u32 p = gabi::ea(this);
    return gabi::ea(&gabi::at<cCcD_SphAttr>(p)->mSph);
}

/* 0200CCF0 */
u32 cCcD_ShapeAttr_GetCoCP() { return gabi::ea(&sVirtualCenter); }

/* 0200CCEC */
void cCcD_Stts_ClrAt() {}

/* 0200C2D8 */
void CcDataHD::cCcD_CpsAttr::CalcAabBox() {
    const u32 p = gabi::ea(this);
    aabClear(p);
    aabSetMinMax(p, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps));
    aabSetMinMax(p, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps.mEnd));
    f32 radius = mCps.mRadius.get();
    aabPlusRadius(p, radius);
}

/* 0200C328 */
u32 CcDataHD::cCcD_CpsAttr::GetNVec(u32 point, u32 out) {
    const u32 p = gabi::ea(this);
    Local<CcVector> line, relative, nearest;
    PSVECSubtract(gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps.mEnd),
                  gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps), line.a);
    f32 length = PSVECDotProduct(line.a, line.a);
    f32 eps = kEpsilon.get();
    if (std::fabs(length) < eps)
        return 0;
    PSVECSubtract(point, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps), relative.a);
    f32 dot = PSVECDotProduct(relative.a, line.a);
    f32 fraction = dot / length, zero = kZero.get();
    if (fraction < zero)
        copyVector(nearest.a, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps));
    else if (fraction > kOne.get())
        copyVector(nearest.a, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps.mEnd));
    else {
        PSVECScale(line.a, line.a, fraction);
        PSVECAdd(line.a, gabi::ea(&gabi::at<cCcD_CpsAttr>(p)->mCps), nearest.a);
    }
    PSVECSubtract(point, nearest.a, out);
    f32 mag = PSVECMag(out);
    if (std::fabs(mag) < eps) {
        gabi::at<Vector>(out)->mCoord[1] = zero;
        gabi::at<Vector>(out)->mCoord[0] = zero;
        gabi::at<Vector>(out)->mCoord[2] = zero;
        return 0;
    }
    PSVECNormalize(out, out);
    return 1;
}

/* 0200C678 */
void CcDataHD::cCcD_TriAttr::CalcAabBox() {
    const u32 p = gabi::ea(this);
    Local<CcVector> vertex;
    aabClear(p);
    floatVector(vertex.a, gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri.mVertex[0]));
    aabSetMinMax(p, vertex.a);
    floatVector(vertex.a, gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri.mVertex[1]));
    aabSetMinMax(p, vertex.a);
    floatVector(vertex.a, gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri.mVertex[2]));
    aabSetMinMax(p, vertex.a);
}

/* 0200C710 */
u32 CcDataHD::cCcD_TriAttr::GetNVec(u32 point, u32 out) {
    const u32 p = gabi::ea(this);
    f32 dot = PSVECDotProduct(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri), point);
    f32 side = dot + mTri.mPlane.mCoefficients[3].get(), zero = kZero.get();
    gabi::at<WordVector>(out)->mCoord[0] =
        gabi::at<WordVector>(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri))->mCoord[0].get();
    gabi::at<WordVector>(out)->mCoord[1] =
        gabi::at<WordVector>(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri))->mCoord[1].get();
    gabi::at<WordVector>(out)->mCoord[2] =
        gabi::at<WordVector>(gabi::ea(&gabi::at<cCcD_TriAttr>(p)->mTri))->mCoord[2].get();
    if (side < zero)
        PSVECScale(out, out, kNegativeOne.get());
    return 1;
}

/* 0200C864 */
void CcDataHD::cCcD_CylAttr::CalcAabBox() {
    const u32 p = gabi::ea(this);
    Local<CcVector> low, high;
    f32 r = mCyl.mRadius.get(), x = mCyl.mCenter.mCoord[0].get(), z = mCyl.mCenter.mCoord[2].get(),
        y = mCyl.mCenter.mCoord[1].get(), h = mCyl.mHeight.get();
    low->mCoord[0] = x - r;
    low->mCoord[1] = y;
    low->mCoord[2] = z - r;
    high->mCoord[1] = y + h;
    high->mCoord[0] = x + r;
    high->mCoord[2] = z + r;
    aabSet(p, low.a, high.a);
}

/* 0200C8CC */
u32 CcDataHD::cCcD_CylAttr::GetNVec(u32 point, u32 out) {
    Local<CcVector> nearest;
    f32 y = gabi::at<Vector>(point)->mCoord[1].get(), base = mCyl.mCenter.mCoord[1].get(),
        x = mCyl.mCenter.mCoord[0].get();
    nearest->mCoord[0] = x;
    nearest->mCoord[1] = base;
    nearest->mCoord[2] = mCyl.mCenter.mCoord[2].get();
    if (base > y) {
    } else {
        f32 height = mCyl.mHeight.get();
        if ((base + height) < y)
            nearest->mCoord[1] = base + height;
        else
            nearest->mCoord[1] = y;
    }
    PSVECSubtract(point, nearest.a, out);
    f32 mag = PSVECMag(out), eps = kEpsilon.get();
    if (std::fabs(mag) < eps) {
        f32 zero = kZero.get();
        gabi::at<Vector>(out)->mCoord[0] = zero;
        gabi::at<Vector>(out)->mCoord[1] = zero;
        gabi::at<Vector>(out)->mCoord[2] = zero;
        return 0;
    }
    PSVECNormalize(out, out);
    return 1;
}

/* 0200CAE4 */
void CcDataHD::cCcD_SphAttr::CalcAabBox() {
    const u32 p = gabi::ea(this);
    Local<CcVector> low, high;
    f32 x = mSph.mCenter.mCoord[0].get(), y = mSph.mCenter.mCoord[1].get(),
        z = mSph.mCenter.mCoord[2].get(), r = mSph.mRadius.get();
    low->mCoord[0] = x - r;
    low->mCoord[1] = y - r;
    low->mCoord[2] = z - r;
    high->mCoord[0] = x + r;
    high->mCoord[1] = y + r;
    high->mCoord[2] = z + r;
    aabSet(p, low.a, high.a);
}

/* 0200CB7C */
u32 CcDataHD::cCcD_SphAttr::GetNVec(u32 point, u32 out) {
    gabi::at<Vector>(out)->mCoord[0] =
        gabi::at<Vector>(point)->mCoord[0].get() - mSph.mCenter.mCoord[0].get();
    gabi::at<Vector>(out)->mCoord[1] =
        gabi::at<Vector>(point)->mCoord[1].get() - mSph.mCenter.mCoord[1].get();
    gabi::at<Vector>(out)->mCoord[2] =
        gabi::at<Vector>(point)->mCoord[2].get() - mSph.mCenter.mCoord[2].get();
    f32 mag = PSVECMag(out), eps = kEpsilon.get();
    if (std::fabs(mag) < eps) {
        f32 zero = kZero.get();
        gabi::at<Vector>(out)->mCoord[0] = zero;
        gabi::at<Vector>(out)->mCoord[1] = zero;
        gabi::at<Vector>(out)->mCoord[2] = zero;
        return 0;
    }
    PSVECNormalize(out, out);
    return 1;
}

/* 0200CC2C */
void cCcD_static_init() {
    sHeaderZero.mWords[3] = 0;
    sHeaderZero.mWords[2] = 0;
    sHeaderZero.mWords[1] = 0;
    sHeaderZero.mWords[0] = 0;
    registerGlobal(gabi::ea(&sHeaderZeroDestructor));
    f32 lo = kPiMin.get(), hi = kPiMax.get();
    sHeaderStatics.mPiMin = lo;
    sHeaderStatics.mPiMax = hi;
    constructHeaderByte(gabi::ea(&sHeaderStatics.mByte));
    registerGlobal(gabi::ea(&sHeaderByteDestructor));
    constructHeaderFlag(gabi::ea(&sHeaderStatics.mFlag));
    registerGlobal(gabi::ea(&sHeaderFlagDestructor));
    f32 y = sZeroVector.mCoord[1].get(), x = sZeroVector.mCoord[0].get(),
        z = sZeroVector.mCoord[2].get();
    sVirtualCenter.mCoord[0] = x;
    sVirtualCenter.mCoord[2] = z;
    sVirtualCenter.mCoord[1] = y;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0273AD10 u32 ccNew(u32)
ext 0273AF40 void ccDelete(u32)
ext 020189A0 void constructPlane(u32)
ext 0201819C void destroyCapsule(u32,u32)
ext 02017D08 void aabSet(u32,u32,u32)
ext 02017DAC void aabClear(u32)
ext 02017E74 void aabSetMinMax(u32,u32)
ext 02017F54 void aabPlusRadius(u32,f32)
ext 0200BF78 void constructAttack(u32)
ext 0200BFD0 void constructTarget(u32)
ext 0200B6C0 void constructDivideInfo(u32)
ext 028E8DAC void PSVECSubtract(u32,u32,u32)
ext 028E8F44 f32 PSVECDotProduct(u32,u32)
ext 028E8E64 void PSVECScale(u32,u32,f32)
ext 028E8D88 void PSVECAdd(u32,u32,u32)
ext 028E8E10 f32 PSVECMag(u32)
ext 028E8EF0 void PSVECNormalize(u32,u32)
ext 0273AA24 void ccAssert(u32,u32,u32)
ext 028F026C void registerGlobal(u32)
ext 028ED6F8 void constructHeaderByte(u32)
ext 028EAB2C void constructHeaderFlag(u32)
ext 020159EC u32 crossCpsCps(u32,u32,u32)
ext 02015CF0 u32 crossCpsCyl(u32,u32,u32)
ext 020168D4 u32 crossCpsSph(u32,u32,u32)
ext 0201923C u32 crossTriTri(u32,u32,u32)
ext 02016A20 u32 crossTriCyl(u32,u32,u32)
ext 02014200 u32 crossSphTri(u32,u32,u32)
ext 020156D0 u32 crossCpsTri(u32,u32,u32)
ext 020186C4 u32 crossCylCylAtTg(u32,u32,u32)
ext 020186C8 u32 crossCylSphAtTg(u32,u32,u32)
ext 02014AF8 u32 crossCylCylCo(u32,u32,u32)
ext 020135C8 u32 crossSphCylCo(u32,u32,u32)
ext 02018F6C u32 crossSphCylAtTg(u32,u32,u32)
ext 02018F9C u32 crossSphSphAtTg(u32,u32,u32)
ext 02013B78 u32 crossSphSphCo(u32,u32,u32)
ext 02016CE8 u32 crossCpsTriangleCopy(u32,u32,u32)
data kZero 1000137C
data kXZPartitionScale 10001380
data kOne 10001384
data kYPartitionScale 10001388
data kEpsilon 100030B8
data kRangeMin 1000138C
data kRangeMax 10001390
data kNegativeOne 10001514
data kPiMin 10001518
data kPiMax 1000151C
data sHeaderZero 101FF4F4
data sHeaderStatics 101FF4E8
data sZeroVector 101FFBA8
data sVirtualCenter 101FF504
data sHeaderZeroDestructor 1018C690
data sHeaderByteDestructor 1018C69C
data sHeaderFlagDestructor 1018C6A8
*/
