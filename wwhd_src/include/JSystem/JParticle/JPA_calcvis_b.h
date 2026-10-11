/* JPADrawVisitor calculation views, WWHD.
 * HD: context fields follow a 0x0C prefix; particle draw parameters are inline at 0x8C.
 * Explicit GHS vtable slots let the source retain cached target/table identities across calls. */
#pragma once
#include "gc_api/gc_api.h"
namespace jpa_calcvis_b {
struct JPADrawParams {
    u8 _00[0xC];
    /* 0x0C */ be<f32> mScaleOut;
    /* 0x10 */ be<f32> mScaleX;
    /* 0x14 */ be<f32> mScaleY;
    u8 _18[8];
    /* 0x20 */ be<f32> mAlphaOut;
    /* 0x24 */ be<f32> mAlphaWaveRandom;
};
WWHD_OFFSET(JPADrawParams, mScaleOut, 0xC);
WWHD_OFFSET(JPADrawParams, mAlphaOut, 0x20);
WWHD_OFFSET(JPADrawParams, mAlphaWaveRandom, 0x24);
WWHD_SIZE(JPADrawParams, 0x28);
struct JPABaseParticle {
    u8 _00[0x78];
    /* 0x78 */ be<f32> mCurFrame;
    u8 _7C[4];
    /* 0x80 */ be<f32> mCurNormTime;
    u8 _84[8];
    /* 0x8C */ JPADrawParams mDrawParams;
};
WWHD_OFFSET(JPABaseParticle, mCurFrame, 0x78);
WWHD_OFFSET(JPABaseParticle, mCurNormTime, 0x80);
WWHD_OFFSET(JPABaseParticle, mDrawParams, 0x8C);
WWHD_SIZE(JPABaseParticle, 0xB4);
struct JPABaseEmitter {
    u8 _00[0x194];
    be<f32> mCurrentFrame;
};
WWHD_OFFSET(JPABaseEmitter, mCurrentFrame, 0x194);
WWHD_SIZE(JPABaseEmitter, 0x198);
struct ExtraShapeVtable {
    u8 _00[0xCC];
    be<u32> getAlphaInTiming; u8 _D0[4];
    be<u32> getAlphaOutTiming; u8 _D8[4];
    be<u32> getAlphaInValue; u8 _E0[4];
    be<u32> getAlphaBaseValue; u8 _E8[0xF4 - 0xE8];
    be<u32> getAlphaWaveParam1; u8 _F8[4];
    be<u32> getAlphaWaveParam2; u8 _100[4];
    be<u32> getAlphaWaveParam3; u8 _108[0x114 - 0x108];
    be<u32> getAlphaIncreaseRate; u8 _118[4];
    be<u32> getAlphaDecreaseRate;
};
WWHD_OFFSET(ExtraShapeVtable, getAlphaInTiming, 0xCC);
WWHD_OFFSET(ExtraShapeVtable, getAlphaOutTiming, 0xD4);
WWHD_OFFSET(ExtraShapeVtable, getAlphaInValue, 0xDC);
WWHD_OFFSET(ExtraShapeVtable, getAlphaBaseValue, 0xE4);
WWHD_OFFSET(ExtraShapeVtable, getAlphaWaveParam1, 0xF4);
WWHD_OFFSET(ExtraShapeVtable, getAlphaWaveParam2, 0xFC);
WWHD_OFFSET(ExtraShapeVtable, getAlphaWaveParam3, 0x104);
WWHD_OFFSET(ExtraShapeVtable, getAlphaIncreaseRate, 0x114);
WWHD_OFFSET(ExtraShapeVtable, getAlphaDecreaseRate, 0x11C);
WWHD_SIZE(ExtraShapeVtable, 0x120);
struct JPAExtraShape { gptr<ExtraShapeVtable> __vtbl; };
WWHD_OFFSET(JPAExtraShape, __vtbl, 0);
WWHD_SIZE(JPAExtraShape, 4);
struct BaseShapeVtable {
    u8 _00[0x134];
    be<u32> getTextureAnmKeyNum; u8 _138[4];
    be<u32> getTextureIndex; u8 _140[4];
    be<u32> getTextureIndexFrame;
};
WWHD_OFFSET(BaseShapeVtable, getTextureAnmKeyNum, 0x134);
WWHD_OFFSET(BaseShapeVtable, getTextureIndex, 0x13C);
WWHD_OFFSET(BaseShapeVtable, getTextureIndexFrame, 0x144);
WWHD_SIZE(BaseShapeVtable, 0x148);
struct JPABaseShape { gptr<BaseShapeVtable> __vtbl; };
WWHD_OFFSET(JPABaseShape, __vtbl, 0);
WWHD_SIZE(JPABaseShape, 4);
struct JPADraw { u8 _00[0xF0]; be<u16> mTexIdx; };
WWHD_OFFSET(JPADraw, mTexIdx, 0xF0);
WWHD_SIZE(JPADraw, 0xF2);
struct JPADrawContext {
    u8 _00[0xC];
    /* 0x0C */ gptr<JPABaseEmitter> pbe;
    /* 0x10 */ gptr<JPABaseShape> pbsp;
    /* 0x14 */ gptr<JPAExtraShape> pesp;
    u8 _18[8];
    /* 0x20 */ gptr<JPADraw> mpDraw;
    u8 _24[8];
    /* 0x2C */ gptr<be<u16>> pTexIdx;
};
WWHD_OFFSET(JPADrawContext, pbe, 0xC);
WWHD_OFFSET(JPADrawContext, pbsp, 0x10);
WWHD_OFFSET(JPADrawContext, pesp, 0x14);
WWHD_OFFSET(JPADrawContext, mpDraw, 0x20);
WWHD_OFFSET(JPADrawContext, pTexIdx, 0x2C);
WWHD_SIZE(JPADrawContext, 0x30);
struct SinCosEntry { be<f32> sin; be<f32> cos; };
WWHD_OFFSET(SinCosEntry, cos, 4);
WWHD_SIZE(SinCosEntry, 8);
// HD: use captured guest targets; the object can be reloaded independently of its vtable.
template<class R, class T, class... Args>
R jpaCall(u32 target, T* object, Args... args) { return gabi::call_ptr<R>(target, object, args...); }
class JPADrawCalcAlpha {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
class JPADrawCalcAlphaFlickNrmSin {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
class JPADrawCalcAlphaFlickAddSin {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
class JPADrawCalcAlphaFlickMultSin {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
class JPADrawCalcTextureAnmIndexNormal {
public:
    void calc(const JPADrawContext* pDC);
};
class JPADrawCalcTextureAnmIndexRepeat {
public:
    void calc(const JPADrawContext* pDC);
};
class JPADrawCalcTextureAnmIndexReverse {
public:
    void calc(const JPADrawContext* pDC);
};
class JPADrawCalcTextureAnmIndexMerge {
public:
    void calc(const JPADrawContext* pDC);
};
class JPADrawCalcTextureAnmIndexRandom {
public:
    void calc(const JPADrawContext* pDC);
};
class JPADrawCalcChildAlphaOut {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
class JPADrawCalcChildScaleOut {
public:
    void calc(const JPADrawContext* pDC, JPABaseParticle* ptcl);
};
} // namespace jpa_calcvis_b
