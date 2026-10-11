#pragma once
#include "gc_api/gc_api.h"

// HD: private line-material and compiler ABI views.
inline constexpr u32 SITEM_SPHERE_CONSTRUCTOR = 0x025166F0;
inline constexpr u32 SITEM_SPHERE_DESTRUCTOR = 0x02515AE8;
struct SitemLine {
    gptr<cXyz> mpSegments;
    gptr<be<u8>> mpSize;
};
struct SitemLineMaterial {
    u8 mMaterialState[0x130];
    be<u32> mVtable;
    u8 mLineState[0x10];
    gptr<SitemLine> mpLines;
};
WWHD_OFFSET(SitemLineMaterial, mpLines, 0x144);
WWHD_SIZE(SitemLineMaterial, 0x148);
inline const dBgS_ObjAcch_vt sitemAcchVt = {0x1003CDB4, 0x1003CDD4, 0x1003CDC4};
/* 025E9960 mDoExt_3DlineMat0_c::mDoExt_3DlineMat0_c (HD 0x148), 025E99E0 its deleting destructor,
 * 025E9B80 init(u16 numLines, u16 numSegs, BOOL hasSize) (as d_a_mo2.h) */
static inline void mDoExt_3DlineMat0_ct(void* l) { gabi::call(0x025E9960, l); }
static inline void mDoExt_3DlineMat0_dt(void* l, s32 flags) { gabi::call(0x025E99E0, l, flags); }
static inline BOOL mDoExt_3DlineMat0_init(void* l, u16 numLines, u16 numSegs, BOOL hasSize) {
    return gabi::call<BOOL>(0x025E9B80, l, numLines, numSegs, hasSize);
}
/* 025EAF58 mDoExt_3DlineMat0_c::update(u16 segs, GXColor& color, dKy_tevstr_c*) (the overload
 * without size) */
static inline void mDoExt_3DlineMat0_update2(void* l, s32 segs, const GXColor* color,
                                             dKy_tevstr_c* tev) {
    gabi::call(0x025EAF58, l, segs, color, tev);
}
/* GHS array helpers */
static inline void sitemConstructArray(u32 p, u32 n, u32 size, u32 ctor) {
    gabi::call(0x028EFFD0, p, n, size, ctor);
}
static inline void sitemDestroyArray(u32 p, u32 n, u32 size, u32 dtor) {
    gabi::call(0x028F0164, p, n, size, dtor, 0, 0);
}

// HD: each simulated link stores position, solver state and width.
struct SitemChainSegment {
    cXyz mPos;
    u8 mSolverState[0xC];
    be<f32> mSize;
};
WWHD_OFFSET(SitemChainSegment, mSize, 0x18);
WWHD_SIZE(SitemChainSegment, 0x1C);

struct sitem_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<J3DModel> mpModel;
    /* 0x3D4 */ be<u8> mType;      /* param bits 0..7 (0xFF: 0) */
    /* 0x3D5 */ be<u8> mSizeType;  /* param bits 8..15 (> 3: 0) */
    /* 0x3D6 */ be<u8> mItemBitNo; /* param bits 16..23 */
    /* 0x3D7 */ be<u8> mItemTbl;   /* param bits 24..31 (0xFF: 0) */
    /* 0x3D8 */ be<s16> mCount;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ be<s16> mMode;
    /* 0x3DE */ be<s16> mTimer[2];
    /* 0x3E2 */ be<s16> mTimer2;
    /* 0x3E4 */ cXyz mPos;
    /* 0x3F0 */ cXyz mOldPos;
    /* 0x3FC */ cXyz mHandPos;
    /* 0x408 */ be<s16> mHandAngX;
    /* 0x40A */ be<s16> mHandAngY;
    /* 0x40C */ u8 _40C[0x410 - 0x40C];
    /* 0x410 */ be<f32> mChainLength;
    /* 0x414 */ u8 _414[4];
    /* 0x418 */ be<f32> mChainDrift;
    /* 0x41C */ be<f32> mSwingAmplitude;
    /* 0x420 */ be<f32> mGroundY;
    /* 0x424 */ be<f32> mCutChainLength;
    /* 0x428 */ SitemLineMaterial mChain; /* mDoExt_3DlineMat0_c (HD 0x148; vtable at +0x130) */
    /* 0x570 */ SitemChainSegment mChainSeg[10];
    /* 0x688 */ SitemLineMaterial mChain2;
    /* 0x7D0 */ SitemChainSegment mChain2Seg[5];
    /* 0x85C */ dCcD_Stts mStts;
    /* 0x898 */ dCcD_Sph mSph[4];
    /* 0xD48 */ dCcD_Sph mBmSph;
    /* 0xE74 */ cXyz mHandCenter;
    /* 0xE80 */ be<f32> mWindPush;
    /* 0xE84 */ be<s16> mWindAngle;
    /* 0xE86 */ u8 _E86[2];
    /* 0xE88 */ dBgS_AcchCir mAcchCir;
    /* 0xEC8 */ dBgS_ObjAcch mAcch;
    /* 0x108C */ dPa_followEcallBack mFollow[2];
    /* 0x10B4 */ csXyz mFollowAngle[2];
    be<s8> mFollowActive;
    u8 mEndPadding[3];
};
WWHD_OFFSET(sitem_class, mPhs, 0x3C8);
WWHD_OFFSET(sitem_class, mCutChainLength, 0x424);
WWHD_OFFSET(sitem_class, mChain2, 0x688);
WWHD_OFFSET(sitem_class, mStts, 0x85C);
WWHD_OFFSET(sitem_class, mBmSph, 0xD48);
WWHD_OFFSET(sitem_class, mAcchCir, 0xE88);
WWHD_OFFSET(sitem_class, mAcch, 0xEC8);
WWHD_OFFSET(sitem_class, mFollow, 0x108C);
WWHD_SIZE(sitem_class, 0x10C4);

WWHD_OFFSET(sitem_class, mFollowAngle, 0x10B4);
WWHD_OFFSET(sitem_class, mFollowActive, 0x10C0);

inline be<u32>& sitemWord(be<f32>& value) { return *reinterpret_cast<be<u32>*>(&value); }

struct SitemSphereSource {
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
WWHD_SIZE(SitemSphereSource, 0x40);

struct SitemDebugRegisters {
    u8 mBeforeSwing[0x34];
    be<f32> mSwingAmplitude;
    u8 mBeforeCut[0x4C - 0x38];
    be<f32> mCutChainBias;
};
inline SitemDebugRegisters* sitemDebug() { return gabi::at<SitemDebugRegisters>(0x1047B608); }
inline cXyz* sitemFarPosition() { return gabi::at<cXyz>(0x1046DED8); }

struct CcAtInfo_sitem {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B[0x1C - 0xB];
};
/* 02518DB0 at_power_check(CcAtInfo*), 02518CC8 def_se_set(actor, cCcD_Obj*, u32 mtrl) */
static inline void sitemAttackPowerCheck(CcAtInfo_sitem* i) { gabi::call(0x02518DB0, i); }
static inline void sitemDefenseSound(fopAc_ac_c* a, u32 obj, u32 mtrl) {
    gabi::call(0x02518CC8, a, obj, mtrl);
}

struct SitemFollowVtable {
    u8 mBeforeRemove[0x44];
    be<u32> mRemove;
};
inline void sitemFollowRemove(dPa_followEcallBack* callback) {
    u32 vtable = gabi::load<u32>(gabi::ea(callback));
    gabi::call_ptr<void>(gabi::at<SitemFollowVtable>(vtable)->mRemove, callback);
}
inline u32 sitemFollowConstruct(dPa_followEcallBack* p) {
    return gabi::call<u32>(0x025A5894, p, (u8)0, (u8)0);
}
inline void sitemSphereConstruct(dCcD_Sph* p) { gabi::call(0x025166F0, p); }
inline void sitemSphereDestroy(dCcD_Sph* p) { gabi::call(0x02515AE8, p, 2); }
inline void sitemAcchDestroy(dBgS_ObjAcch* p) { gabi::call(0x024EFD9C, p, 0); }
inline void sitemCircleDestroy(cM3dGCir_l* p) { gabi::call(0x02018034, p, 2); }
inline void sitemActorDestroy(sitem_class* p) { gabi::call(0x025D50BC, p, 0); }
inline void sitemGroundDestroy(dBgS_GndChk* p) { gabi::call(0x02008DAC, p, 0); }
// HD: Create dispatches the engine entry twice rather than inlining Execute.
inline BOOL sitemExecuteCall(sitem_class* p) { return gabi::call<BOOL>(0x024867D0, p); }
