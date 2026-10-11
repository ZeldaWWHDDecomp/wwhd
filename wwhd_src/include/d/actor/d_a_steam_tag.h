/** Steam vent actor. Local only. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_item.h"
struct SteamEmitter {
    u8 _00[0x5C]; be<s32> mMaxFrame;
    u8 _60[0x1B4 - 0x60]; be<s32> mActiveParticleCount;
    u8 _1B8[8]; be<s32> mChildParticleCount;
    u8 _1C4[0x2C]; Mtx34 mGlobalRotation;
    u8 _220[0xC]; cXyz mGlobalTranslation;
    u8 _238[0xF]; be<u8> mGlobalAlpha;
    u8 _248[0xC]; be<u32> mFlags;
    u8 _258[0xA]; be<u8> mRotationOrder;
    void stopCreateParticle() { mFlags = mFlags | 1u; }
    void playCreateParticle() { mFlags = mFlags & ~1u; }
    void becomeInvalidEmitter() { mMaxFrame = -1; stopCreateParticle(); }
    s32 getParticleNumber() { return mActiveParticleCount + mChildParticleCount; }
};
WWHD_OFFSET(SteamEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(SteamEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(SteamEmitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(SteamEmitter, mFlags, 0x254);
WWHD_OFFSET(SteamEmitter, mRotationOrder, 0x262);
// HD: only the point-wind prefix through its power is accessed by this unit.
struct SteamWindInfluencePrefix { u8 _00[0x1C]; be<f32> mStrength; };
struct SteamPointWindPrefix { be<u32> mpCps; SteamWindInfluencePrefix mWind; };


// HD: unit-local views keep the capsule layouts without importing the cylinder
// geometry header, whose standalone class conflicts with dCcD's caller-side cylinder.
struct SteamCapsuleShape {
    cXyz mStart;
    cXyz mEnd;
    be<f32> mRadius;
};
struct SteamCapsuleGeometry {
    cXyz mStart;
    cXyz mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
WWHD_OFFSET(SteamCapsuleGeometry, __vtbl, 0x18);
WWHD_OFFSET(SteamCapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(SteamCapsuleGeometry, 0x20);

struct daSteamTag_c : fopAc_ac_c {
    BOOL CreateInit();
    BOOL createEmitter();
    bool endEmitter();
    cPhs_State create();
    BOOL execute();

    /* 0x3AC */ gptr<SteamEmitter> mpEmitter;
    /* 0x3B0 */ u8 _3B0[2];
    /* 0x3B2 */ be<s16> mEmitTimer;
    /* 0x3B4 */ be<s16> mCreateTimer;
    /* 0x3B6 */ be<u8> m29A;
    /* 0x3B7 */ be<u8> m29B;
    /* 0x3B8 */ dCcD_Stts mGStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ dCcD_Cps mCps;
    /* 0x65C */ SteamCapsuleShape mPntWindCpsS;
    /* 0x678 */ SteamPointWindPrefix mPointWind;
};
WWHD_OFFSET(daSteamTag_c, mGStts, 0x3B8);
WWHD_OFFSET(daSteamTag_c, mCyl, 0x3F4);
WWHD_OFFSET(daSteamTag_c, mCps, 0x524);
WWHD_OFFSET(daSteamTag_c, mPntWindCpsS, 0x65C);
WWHD_OFFSET(daSteamTag_c, mPointWind, 0x678);


WWHD_SIZE(SteamCapsuleShape, 0x1C);
WWHD_SIZE(SteamPointWindPrefix, 0x24);
WWHD_OFFSET(daSteamTag_c, mPointWind.mWind.mStrength, 0x698);
WWHD_SIZE(daSteamTag_c, 0x69C);
struct SteamCpsConstruction {
    u8 _00[0x110]; be<u32> mAabVtable, mShapeVtable; SteamCapsuleGeometry mCps;
};
WWHD_OFFSET(SteamCpsConstruction, mCps, 0x118);
WWHD_SIZE(SteamCpsConstruction, 0x138);
struct SteamSafeStringVtable { u8 _00[0x14]; gfn assureTermination; };
struct SteamSafeString {
    gptr<be<u8>> mStringTop; gptr<SteamSafeStringVtable> __vtbl;
    void assureTermination() { gabi::call_ptr(__vtbl->assureTermination.get(), this); }
};
WWHD_SIZE(SteamSafeString, 8);
struct SteamPlayView { u8 _00[0x5134]; be<u8> mStartStageName[8]; };
inline bool SteamSafeString_eq(SteamSafeString* a, SteamSafeString* b) {
    // HD: retain all virtual calls and the bounded guest-byte comparison.
    a->assureTermination(); a->assureTermination();
    be<u8>* s1 = a->mStringTop;
    b->assureTermination();
    if (s1 == b->mStringTop.get()) return true;
    be<u8>* p = a->mStringTop; be<u8>* q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 c = p[i];
        if (c != q[i]) return false;
        if (c == 0) return true;
    }
    return false;
}
void SteamCps_Set(dCcD_Cps*, const dCcD_SrcCps*);
void SteamGeometryCps_Set(SteamCapsuleGeometry*, SteamCapsuleShape*);
void SteamCps_base_ct(dCcD_Cps*);
void SteamGeometryCps_ct(SteamCapsuleGeometry*);
void SteamCps_dt(dCcD_Cps*, s32);
void SteamPointWind_init(SteamPointWindPrefix*, SteamCapsuleShape*);
void SteamPointWind_move(SteamPointWindPrefix*);
void SteamPointWind_delete(SteamPointWindPrefix*);
void SteamGetXYZRotateMtx(s16, s16, s16, Mtx34*);
void SteamActor_dt(fopAc_ac_c*, u32);
inline SteamCapsuleGeometry* SteamCapsule(dCcD_Cps* cps) { return reinterpret_cast<SteamCapsuleGeometry*>(cps->mCps); }
inline void SteamCps_ct(dCcD_Cps* cps) {
    SteamCps_base_ct(cps);
    SteamCpsConstruction* shape = reinterpret_cast<SteamCpsConstruction*>(cps);
    shape->mShapeVtable = 0x100015A8; shape->mAabVtable = 0x1003D764;
    SteamGeometryCps_ct(&shape->mCps);
    cps->__vtbl_hitinf = 0x1004AF18;
    shape->mShapeVtable = 0x1004AF70; shape->mCps.__vtbl = 0x1004AF60;
}

struct SteamStageName { be<u8> name[8]; };
WWHD_SIZE(SteamStageName, 8);
void PSVECSubtract(const cXyz*, const cXyz*, cXyz*);
