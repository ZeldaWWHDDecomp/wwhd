#pragma once
#include "gc_api/gc_api.h"

struct klft_class : fopAc_ac_c {
    static constexpr u32 __vt = 0x10012FF4;
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mCounter;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ gptr<mDoExt_McaMorf> mpMorf[2]; /* rope anchors */
    /* 0x3C4 */ cXyz mHimoTop[2];               /* path points 2 and 3 */
    /* 0x3DC */ be<s16> mAnchorRotY;
    /* 0x3DE */ be<u8> mType;
    /* 0x3DF */ be<u8> mSwitchNo;
    /* 0x3E0 */ cXyz mSway;
    /* 0x3EC */ cXyz mSwayAmplitude;
    /* 0x3F8 */ cXyz mSwayTarget;
    /* 0x404 */ cXyz mRotTarget;
    /* 0x410 */ cXyz mRotAmplitude;
    /* 0x41C */ csXyz mShakeAngle;
    /* 0x422 */ u8 _422[6];
    /* 0x428 */ Mtx34 mBgMtx;
    /* 0x458 */ gptr<dBgW> mpBgW;
    /* 0x45C */ mDoExt_3DlineMat1_c mLineMat;
    /* 0x5E4 */ cXyz mPathP0;
    /* 0x5F0 */ cXyz mPathP1;
    /* 0x5FC */ cXyz mPathDir;
    /* 0x608 */ cXyz mCenter;
    /* 0x614 */ be<s16> mTiltDir;
    /* 0x616 */ be<s16> mTilt;
    /* 0x618 */ be<s16> mWheelRot;
    /* 0x61A */ be<s16> mHitTimer;
    /* 0x61C */ be<f32> mWheelSe;
    /* 0x620 */ be<f32> mSink;
    /* 0x624 */ be<f32> mSinkTarget;
    /* 0x628 */ be<f32> mPos; /* 20..80 along the path */
    /* 0x62C */ be<f32> mSpeed;
    /* 0x630 */ be<f32> mSpeedTarget;
    /* 0x634 */ dCcD_Stts mStts;
    /* 0x670 */ dCcD_Sph mSph; /* Deku Leaf wind target */
    /* 0x79C */ dCcD_Cyl mCoCyl[2];
    /* 0x9FC */ dCcD_Sph mWindSph[2];
    /* 0xC54 */ be<s16> mRideTimer;
    /* 0xC56 */ u8 _C56[2];
};
WWHD_OFFSET(klft_class, mLineMat, 0x45C);
WWHD_OFFSET(klft_class, mStts, 0x634);
WWHD_OFFSET(klft_class, mWindSph, 0x9FC);
WWHD_OFFSET(klft_class, mRideTimer, 0xC54);

// Unit-private bindings for HD overloads and ABI details.
// HD: J3DNode_toJoint has already performed the out-of-line node accessor.
struct KlftJointView {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(KlftJointView, mJntNo, 4);
/* 0200796C / 02007990: CPad main stick X / Y of a port (f1) */
static inline f32 CPad_GET_STICK_POS_X(u32 port) { return gabi::call<f32>(0x0200796C, port); }
static inline f32 CPad_GET_STICK_POS_Y(u32 port) { return gabi::call<f32>(0x02007990, port); }
static inline f32 std_fabsf(f32 x) { return std::fabs(x); }
/* 02518CC8 def_se_set(actor, hit obj, material) */
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 p) {
    gabi::call(0x02518CC8, a, obj, p);
}
static inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
static inline s16 cM_rad2s(f32 r) { return gabi::call<s16>(0x02019510, r); }
/* 0201A4DC csXyz::operator+(const csXyz&) const: the result comes back in r3:r4 (x<<16|y, z<<16) */
static inline void csXyz_pl(const csXyz* a, const csXyz* b, csXyz* out) {
    u32 r3 = gabi::call<u32>(0x0201A4DC, a, b);
    u32 r4 = gabi::cpu->r[4];
    out->x = (s16)(r3 >> 16);
    out->y = (s16)(r3 & 0xFFFF);
    out->z = (s16)(r4 >> 16);
}
/* fopAcM_seStart on current.pos (HD inline: null checks on the actor and the position) */
static inline void klft_seStartCurrent(fopAc_ac_c* a, u32 id, u32 prm) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, prm, dComIfGp_getReverb(a->current.roomNo));
}
/* mDoExt_3DlineMat1_c (HD): constructor 025EB82C, init 025EBA58, update 025ED1BC, destructor
 * 025EB8B8 (the matcher names it draw); dComIfGd_set3DlineMat: sort packets at play+0x5FB4 */
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_c* l, u16 a, u16 b, void* tex, s32 c) {
    return gabi::call<BOOL>(0x025EBA58, l, a, b, tex, c);
}
static inline void klftUpdateRopeMaterial(mDoExt_3DlineMat1_c* l, u16 segs, GXColor* color,
                                          dKy_tevstr_c* tev) {
    gabi::call(0x025ED1BC, l, segs, color, tev);
}

static inline void klftConstructArray(void* p, s32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void klftDestroyArray(void* p, s32 n, u32 size, u32 dtor) {
    gabi::call(0x028F0164, p, n, size, dtor, 0, 0);
}
static inline void klftConstructSphere(void* p) { gabi::call(0x025166F0, p); }
static inline f32 REG0F(int i) { return REG_F(0, i); }

WWHD_SIZE(klft_class, 0xC58);

struct KlftGlobals {
    gptr<cXyz> mpWind;
    be<u32> mWindPower;
    be<f32> mNegativePi, mPi;
    be<s16> mWindAngle;
    be<u8> mMathInit, mRandomInit;
    be<u32> mZeroState[4];
};
WWHD_OFFSET(KlftGlobals, mWindAngle, 0x10);
WWHD_OFFSET(KlftGlobals, mZeroState, 0x14);
WWHD_SIZE(KlftGlobals, 0x24);
inline KlftGlobals& klftGlobals() { return *gabi::at<KlftGlobals>(0x10464BC4); }
struct KlftLine {
    gptr<cXyz> mpPos;
    gptr<be<u8>> mpSize;
};
WWHD_SIZE(KlftLine, 8);
inline KlftLine* klftLine(mDoExt_3DlineMat1_c* material) {
    return gabi::at<KlftLine>(material->mpLines);
}
struct KlftPathPoint {
    u8 mArg0, mArg1, mArg2, mArg3;
    cXyz m_position;
};
struct KlftPath {
    be<u16> m_num, m_nextID;
    u8 mArg0, m_closed, mPathState[2];
    gptr<KlftPathPoint> m_points;
};
WWHD_SIZE(KlftPathPoint, 0x10);
WWHD_OFFSET(KlftPath, m_points, 8);
struct KlftBgW {
    u8 mBackgroundState[0xA8];
    be<u32> mCrrFunc;
    u8 mCorrectionState[4];
    gfn mRideCallback;
};
WWHD_OFFSET(KlftBgW, mRideCallback, 0xB0);
inline KlftBgW* klftBackground(dBgW* background) { return reinterpret_cast<KlftBgW*>(background); }
inline void klftMathInit(be<u8>* p) { gabi::call(0x028ED6F8, p); }
inline void klftRandomInit(be<u8>* p) { gabi::call(0x028EAB2C, p); }
inline void klftSphereDestruct(dCcD_Sph* p) { gabi::call(0x02515AE8, p, 2); }
// HD: retain the unused second GPR expected by this unit's original verification contract.
inline void klftDeleteSoundObject(cXyz* pos, u32 sw) { gabi::call(0x025E1B34, pos, sw); }

struct KlftCylinderSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
    be<f32> mHeight;
};
WWHD_SIZE(KlftCylinderSource, 0x44);

struct KlftSphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    u8 mAtp;
    u8 mAtPadding0;
    u8 mAtPadding1;
    u8 mAtPadding2;
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    u8 mGAtSe;
    u8 mGAtHitMark;
    u8 mGAtSpl;
    u8 mGAtMtrl;
    be<u32> mGAtSPrm;
    u8 mGTgSe;
    u8 mGTgHitMark;
    u8 mGTgSpl;
    u8 mGTgMtrl;
    be<u32> mGTgSPrm;
    be<u32> mGCoSPrm;
    be<f32> mCenterX;
    be<f32> mCenterY;
    be<f32> mCenterZ;
    be<f32> mRadius;
};
WWHD_SIZE(KlftSphereSource, 0x40);
