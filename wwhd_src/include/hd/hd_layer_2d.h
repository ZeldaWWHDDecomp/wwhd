/** HD 2D render-layer layouts. Library camera/projection objects remain opaque. */
#pragma once
#include "hd/hd_color_fader.h"
#include "hd/hd_layer_base.h"
#include "hd/hd_multi_filter.h"
struct hdProjection_l {
    be<u8> mDirty;
    u8 mStorage[0x93];
    be<f32> mNear;
    be<f32> mFar;
    u8 mTail[0x10];
};
WWHD_OFFSET(hdProjection_l, mNear, 0x94);
WWHD_OFFSET(hdProjection_l, mFar, 0x98);
WWHD_SIZE(hdProjection_l, 0xAC);
struct hdCamera_l { u8 mStorage[0xB8]; };
struct hdRenderContextVtable_l { u8 mReserved[0x24]; be<u32> mUpdate; };
struct hdRenderContext_l {
    be<u32> mMatrix[12];
    gptr<hdRenderContextVtable_l> mpVtable;
    cXyz mEye;
    cXyz mTarget;
    cXyz mUp;
    be<f32> mHeight;
    be<u8> mPerspective;
    u8 mPadding[3];
};
WWHD_OFFSET(hdRenderContext_l, mpVtable, 0x30);
WWHD_OFFSET(hdRenderContext_l, mEye, 0x34);
WWHD_OFFSET(hdRenderContext_l, mHeight, 0x58);
WWHD_SIZE(hdRenderContext_l, 0x60);
struct hdLayer2D_c {
    hdLayerBase_c mBase;
    hdProjection_l mProjection;
    hdProjection_l mSecondaryProjection;
    hdCamera_l mCamera;
    hdRenderContext_l mRenderContext;
    void initialize();
};
WWHD_OFFSET(hdLayer2D_c, mBase, 0);
WWHD_OFFSET(hdLayer2D_c, mProjection, 0x610);
WWHD_OFFSET(hdLayer2D_c, mSecondaryProjection, 0x6BC);
WWHD_OFFSET(hdLayer2D_c, mCamera, 0x768);
WWHD_OFFSET(hdLayer2D_c, mRenderContext, 0x820);
WWHD_SIZE(hdLayer2D_c, 0x880);
struct hdLayer2DUI_c : hdLayer2D_c {
    void initialize();
    void drawUI(hdLayerRenderInfo_c* renderInfo);
    void drawParticleBackground(hdLayerRenderInfo_c* renderInfo);
    void drawParticleForeground(hdLayerRenderInfo_c* renderInfo);
    void drawScreen(hdLayerRenderInfo_c* renderInfo);
    void draw(hdLayerRenderInfo_c* renderInfo);
};
struct hdLayer2DDRC_c : hdLayer2D_c {
    static u32 getFirstParameter();
    static u32 getSecondParameter();
    void destroy(u32 flags);
    void destroyBase(u32 flags);
    void initialize();
};
struct hdLayer2DFore_c : hdLayer2D_c {
    static u32 getFirstParameter();
    static u32 getSecondParameter();
    void initialize();
};
struct hdLayer2DDRCFore_c : hdLayer2D_c {
    hdProjection_l mParticleProjection;
    void initialize();
    void drawParticleForeground();
    void drawParticleBackground();
    void drawParticles(void* renderInfo);
    void destroy(u32 flags);
    static u32 getFirstParameter();
    static u32 getSecondParameter();
};
WWHD_OFFSET(hdLayer2DDRCFore_c, mParticleProjection, 0x880);
WWHD_SIZE(hdLayer2DDRCFore_c, 0x92C);
// The shared render-info types hide the display selector at +4.
// Keep only that missing prefix view; pass/viewport/bounds use shared fields.
struct hdRenderDisplaySelection_l {
    u8 mReserved[4];
    be<u32> mDisplay;
};
WWHD_OFFSET(hdRenderDisplaySelection_l, mDisplay, 4);
// Both canonical records view the same callback argument. UI/DRC originals
// load viewport+8 and bounds+1C; layer dispatch calls its +1C slot mpViewport.
// Use the filter view for those named slots without changing the guest address.
inline hdFilterRenderInfo_c* filterRenderInfo(hdLayerRenderInfo_c* info) {
    return reinterpret_cast<hdFilterRenderInfo_c*>(info);
}
inline u32 renderDisplay(hdLayerRenderInfo_c* info) {
    return reinterpret_cast<hdRenderDisplaySelection_l*>(info)->mDisplay;
}
struct hdLayerPlayView_l { gptr<void> mpView; u8 mReserved[0x30]; };
WWHD_SIZE(hdLayerPlayView_l, 0x34);
struct hdLayerUIDrawState_l {
    u8 mReserved[0x68];
    gptr<void> mpUIRenderer;
    u8 mTail[0x178];
};
WWHD_SIZE(hdLayerUIDrawState_l, 0x1E4);
struct hdLayerPlay_l {
    u8 mReserved[0x5AB0];
    gptr<void> mpParticles;
    u8 mBeforeView[0x40];
    be<s8> mCurrentView;
    u8 mPadding[3];
    hdLayerPlayView_l mViews[4];
    u8 mBeforeUI[0x168];
    hdLayerUIDrawState_l mUIDrawState;
    u8 mDrawCommand[0x80];
    gptr<void> mpDrawData;
};
WWHD_OFFSET(hdLayerPlay_l, mCurrentView, 0x5AF4);
WWHD_OFFSET(hdLayerPlay_l, mViews, 0x5AF8);
WWHD_OFFSET(hdLayerPlay_l, mUIDrawState, 0x5D30);
WWHD_OFFSET(hdLayerPlay_l, mpDrawData, 0x5F94);
struct hdLayerParticleDrawParams_l {
    gptr<Mtx34> mpTransform;
    be<u32> mProjection[16];
    be<f32> mScale;
    be<f32> mAspect;
    be<u32> mFlags;
};
WWHD_SIZE(hdLayerParticleDrawParams_l, 0x50);
