/** Draw visitor views for the qualified HD particle calculation subset.
 * HD: each context owns its clipboard; resource getters retain their virtual ABI.
 * The particle prefix is shared with the common particle layout. */
#pragma once
#include "JSystem/JParticle/JPAParticle.h"
namespace JPA_calcvis_a {
struct JPADrawParams {
    u8 mAxis[12];
    be<f32> mScaleOut;
    be<f32> mScaleX;
    be<f32> mScaleY;
    u8 padding_18[0x14];
    be<u32> mPrmColor;
    be<u32> mEnvColor;
};
struct JPADrawClipBoard {
    u8 padding[0xEC];
    be<f32> mScaleAnmTiming;
    be<s16> mColorAnmFrame;
};
struct JPADraw {
    u8 padding[0xE8];
    be<u32> mPrmColor;
    be<u32> mEnvColor;
};
struct JPAEmitterCallBackVtable {
    u8 padding[0x2C];
    be<u32> draw;
};
struct JPAEmitterCallBack {
    gptr<JPAEmitterCallBackVtable> __vtbl;
};
struct JPABaseEmitter {
    u8 padding_0[0x194];
    be<f32> mCurFrame;
    u8 padding_198[0x4C];
    gptr<JPAEmitterCallBack> mpCallBack;
};
struct JPAExtraShapeVtable {
    u8 padding_0[0x44];
    be<u32> getScaleInTiming;
    u8 padding_48[4];
    be<u32> getScaleOutTiming;
    u8 padding_50[4];
    be<u32> getScaleInValueX;
    u8 padding_58[4];
    be<u32> getScaleInValueY;
    u8 padding_60[0x14];
    be<u32> getAnmCycleX;
    u8 padding_78[4];
    be<u32> getAnmCycleY;
    u8 padding_80[4];
    be<u32> getIncreaseRateX;
    u8 padding_88[4];
    be<u32> getIncreaseRateY;
    u8 padding_90[4];
    be<u32> getDecreaseRateX;
    u8 padding_98[4];
    be<u32> getDecreaseRateY;
};
struct JPAExtraShape {
    gptr<JPAExtraShapeVtable> __vtbl;
};
struct JPABaseShapeVtable {
    u8 padding_0[0x174];
    be<u32> getPrmColor;
    u8 padding_178[12];
    be<u32> getEnvColor;
    u8 padding_188[12];
    be<u32> getColorRegAnmMaxFrm;
};
struct JPABaseShape {
    gptr<JPABaseShapeVtable> __vtbl;
};
struct JPADrawContext {
    gptr<JPADrawClipBoard> pcb;
    u8 padding_4[8];
    gptr<JPABaseEmitter> pbe;
    gptr<JPABaseShape> pbsp;
    gptr<JPAExtraShape> pesp;
    u8 padding_18[8];
    gptr<JPADraw> mpDraw;
};
WWHD_OFFSET(JPADrawParams, mScaleOut, 0xC);
WWHD_OFFSET(JPADrawParams, mScaleY, 0x14);
WWHD_OFFSET(JPADrawParams, mPrmColor, 0x2C);
WWHD_OFFSET(JPADrawClipBoard, mScaleAnmTiming, 0xEC);
WWHD_OFFSET(JPABaseEmitter, mpCallBack, 0x1E4);
WWHD_OFFSET(JPAExtraShapeVtable, getScaleOutTiming, 0x4C);
WWHD_OFFSET(JPAExtraShapeVtable, getAnmCycleX, 0x74);
WWHD_OFFSET(JPAExtraShapeVtable, getDecreaseRateY, 0x9C);
WWHD_OFFSET(JPABaseShapeVtable, getColorRegAnmMaxFrm, 0x194);
WWHD_OFFSET(JPADrawContext, mpDraw, 0x20);
struct JPADrawExecCallBack {
    void exec(const JPADrawContext *pDC);
};
struct JPADrawCalcScaleX {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleY {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleXBySpeed {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleYBySpeed {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleAnmTimingRepeatX {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleAnmTimingRepeatY {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleAnmTimingReverseX {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleAnmTimingReverseY {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleCopyX2Y {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcScaleAnmTimingNormal {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcColorPrm {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorEnv {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorCopyFromEmitter {
    void calc(const JPADrawContext *pDC, JPABaseParticle *ptcl);
};
struct JPADrawCalcColorAnmFrameNormal {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorAnmFrameRepeat {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorAnmFrameReverse {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorAnmFrameMerge {
    void calc(const JPADrawContext *pDC);
};
struct JPADrawCalcColorAnmFrameRandom {
    void calc(const JPADrawContext *pDC);
};
} // namespace JPA_calcvis_a
