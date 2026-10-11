/** Destroyed Wind Temple wall smoke and rubble. Local only. */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JPAEmitter.h"
u32 daObjHomensmoke_getParam(fopAc_ac_c*, s32, s32);
void daObjHomensmoke_smokeCallback_ct(dPa_smokeEcallBack*, u32);
void daObjHomensmoke_actor_dt(fopAc_ac_c*, u32);
void daObjHomensmoke_setRTMatrix(const Mtx34*, Mtx34*, cXyz*);
namespace daObjHomensmoke {
struct Act_c : fopAc_ac_c {
    int param_get_arg0() { return daObjHomensmoke_getParam(this, 1, 0) & 0x1; }
    int param_get_axis() { return daObjHomensmoke_getParam(this, 1, 1) & 0x1; }
    void set_mtx();
    cPhs_State _create();
    bool _delete();
    bool _execute();

    /* 0x3AC */ u8 m294[0x3B4 - 0x3AC];
    /* 0x3B4 */ Mtx34 mMtx;
    /* 0x3E4 */ be<s32> mType;
    /* 0x3E8 */ dPa_smokeEcallBack mSmokeCb;
    /* 0x408 */ be<BOOL> mbInitialized;
    /* 0x40C */ cXyz mSmokePos;
};
WWHD_OFFSET(Act_c, mMtx, 0x3B4);
WWHD_OFFSET(Act_c, mSmokeCb, 0x3E8);
WWHD_OFFSET(Act_c, mSmokePos, 0x40C);

WWHD_OFFSET(Act_c, mType, 0x3E4);
WWHD_OFFSET(Act_c, mbInitialized, 0x408);
WWHD_SIZE(Act_c, 0x418);
struct CullBox { cXyz mMin, mMax; };
struct CullBoxes { CullBox boxes[2]; };
struct RateTable { be<f32> rates[2]; };
// HD: emitter prefix and expanded global transform/color state used by this actor.
struct Emitter {
    u8 _00[8]; cXyz mEmitterScale;
    u8 _14[0x20]; be<f32> mRate;
    u8 _38[0x24]; be<s32> mMaxFrame;
    u8 _60[0x190]; Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale, mGlobalTranslation, mGlobalParticleScale;
    GXColor mGlobalPrmColor;
};
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Emitter, mGlobalPrmColor, 0x244);
WWHD_SIZE(Emitter, 0x248);
// HD: assignment copies only these light-object fields; unlisted bytes stay untouched.
struct LightBlock {
    be<f32> mPositionDirection[6]; be<u8> mColor[4];
    be<u16> mColorS10[4]; be<f32> mFactors[8]; u8 _44[0x40];
};
struct TevCopyView {
    LightBlock mLightObj;
    be<u32> mLightPosWorld[3]; be<u16> mColorC0[4];
    be<u32> mColorK[2]; be<u16> mFogColor[4];
    be<f32> mFogAndBlend[3]; be<u8> mState[9]; u8 _BD[3];
    LightBlock mHdLightParam[2];
};
WWHD_SIZE(LightBlock, 0x84);
WWHD_OFFSET(TevCopyView, mLightPosWorld, 0x84);
WWHD_OFFSET(TevCopyView, mHdLightParam, 0xC0);
WWHD_SIZE(TevCopyView, 0x1C8);
template<class T, unsigned N> inline void copyMembers(be<T> (&dst)[N], const be<T> (&src)[N]) {
    for (unsigned i = 0; i < N; ++i) { T value = src[i]; dst[i] = value; }
}
inline void copyLight(LightBlock& dst, const LightBlock& src) {
    copyMembers(dst.mPositionDirection, src.mPositionDirection);
    copyMembers(dst.mColor, src.mColor);
    copyMembers(dst.mColorS10, src.mColorS10);
    copyMembers(dst.mFactors, src.mFactors);
}
inline void copyTev(dKy_tevstr_c& dst, const dKy_tevstr_c& src) {
    TevCopyView& d = reinterpret_cast<TevCopyView&>(dst);
    const TevCopyView& s = reinterpret_cast<const TevCopyView&>(src);
    copyLight(d.mLightObj, s.mLightObj);
    copyMembers(d.mLightPosWorld, s.mLightPosWorld);
    copyMembers(d.mColorC0, s.mColorC0);
    copyMembers(d.mColorK, s.mColorK);
    copyMembers(d.mFogColor, s.mFogColor);
    copyMembers(d.mFogAndBlend, s.mFogAndBlend);
    copyMembers(d.mState, s.mState);
    copyLight(d.mHdLightParam[0], s.mHdLightParam[0]);
    copyLight(d.mHdLightParam[1], s.mHdLightParam[1]);
}
}

WWHD_SIZE(daObjHomensmoke::CullBox, 0x18);
WWHD_SIZE(daObjHomensmoke::CullBoxes, 0x30);
WWHD_SIZE(daObjHomensmoke::RateTable, 8);
