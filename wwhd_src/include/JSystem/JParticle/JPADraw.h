// JPA1 draw clipboard, with Cafe's additional 4x4 projection matrix.
#pragma once
#include "JSystem/JParticle/JPA_math_drawcalc.h"
#include "JSystem/JParticle/JPADrawVisitor_container.h"
#include "JSystem/JParticle/JPADraw_shapes.h"
#include "d/d_com_inf_game.h"
struct JPAParticleRenderer;
namespace jpa_emitter_calc { struct JPABaseEmitter; }
namespace JPADrawExtension {
struct JPADrawTexCoord_l { be<f32> x,y; };
struct JPADrawClipBoard_l {
    u8 mSetupTev[4];
    JPADrawTexCoord_l mGlobalScale, mPivot;
    JPADrawTexCoord_l mTexCoordPt[4];
    gptr<void> mDrawMtxPtr;
    be<f32> mProjectionMtx[4][4];
    Mtx34 mDrawYBBMtx, mDrawMtx;
    be<u32> mPrmColor,mEnvColor;
    be<u32> mDirTypeFunc,mRotTypeFunc,mBasePlaneTypeFunc;
    be<f32> mScaleAnmTiming;
    be<s16> mColorAnmFrame;
    u8 mTail[2];
};
WWHD_SIZE(JPADrawTexCoord_l,8);
WWHD_OFFSET(JPADrawClipBoard_l,mTexCoordPt,0x14);
WWHD_OFFSET(JPADrawClipBoard_l,mDrawMtxPtr,0x34);
WWHD_OFFSET(JPADrawClipBoard_l,mProjectionMtx,0x38);
WWHD_OFFSET(JPADrawClipBoard_l,mDrawYBBMtx,0x78);
WWHD_OFFSET(JPADrawClipBoard_l,mDrawMtx,0xA8);
WWHD_OFFSET(JPADrawClipBoard_l,mPrmColor,0xD8);
WWHD_OFFSET(JPADrawClipBoard_l,mDirTypeFunc,0xE0);
WWHD_OFFSET(JPADrawClipBoard_l,mScaleAnmTiming,0xEC);
WWHD_OFFSET(JPADrawClipBoard_l,mColorAnmFrame,0xF0);
WWHD_SIZE(JPADrawClipBoard_l,0xF4);
struct JPADrawOrderVtable_l { u8 mPrefix[0x74]; be<u32> getChildOrder; };
struct JPADrawOrderShape_l {
    gptr<JPADrawOrderVtable_l> __vtbl;
    u32 getChildOrder() { return gabi::call_ptr<u32>(__vtbl->getChildOrder,this); }
};
struct JPADrawVisitorDefFlags_l {
    be<s32> mbIsEnableDrawParent,mbHasPrmAnm,mbHasEnvAnm,mbIsStripe,mbIsPointOrLine,mbIsEnableAlpha;
};
WWHD_SIZE(JPADrawVisitorDefFlags_l,0x18);
struct JPADraw_l : JPAMathDraw::JPADraw {
    JPADrawBaseShape_l* baseShape() { return reinterpret_cast<JPADrawBaseShape_l*>(dc.pbsp.get()); }
    JPADrawExtraShape_l* extraShape() { return reinterpret_cast<JPADrawExtraShape_l*>(dc.pesp.get()); }
    JPADrawSweepShape_l* sweepShape() { return reinterpret_cast<JPADrawSweepShape_l*>(dc.pssp.get()); }
    JPAMathDraw::JPADraw& execution() { return *this; }
    u32 childOrder() { return reinterpret_cast<JPADrawOrderShape_l*>(dc.pbsp.get())->getChildOrder(); }
    void setDrawExecVisitorsBeforeCB(const JPADrawVisitorDefFlags_l&);
    void setDrawExecVisitorsAfterCB(const JPADrawVisitorDefFlags_l&);
    void setDrawCalcVisitors(const JPADrawVisitorDefFlags_l&);
    bool initialize(jpa_emitter_calc::JPABaseEmitter*,JPAMathDraw::JPATextureResource*);
    void draw(const Mtx34*,const void*);
    void drawAlternateFirst(const Mtx34*,const void*);
    void drawAlternateSecond(const Mtx34*,const void*);
    void zDrawAlternateChild(u32);
    void zDrawAlternateParticle(u32);
    void drawAlternateChild(u32);
    void drawAlternateParticle(u32);
    void drawHDChild();
    void drawHDParticle();
    void zDrawEmitterOnly();
    void drawEmitterOnly(const Mtx34*,const void*);
    void zDrawChild();
    void zDrawParticle();
    void drawChild();
    void drawParticle();
    void zDraw();
    void zDrawAlternate(u32);
    void loadYBBMtx(const Mtx34*,JPADrawClipBoard_l*);
    bool loadTexture(u32,u8,JPAParticleRenderer*);
    void getTextureName(SafeString*,s16);
    void setChildClipBoard(s32);
    void setParticleClipBoard(s32);
    void drawHD(const Mtx34*,const void*);
    void setupRenderState();
    void bindTextureUniforms();
    void updateUniforms(u32);
    void updateFrameTexture(JPAMathDraw::JPAParticleHD*);
};
WWHD_OFFSET(JPADraw_l,mTexIdx,0xF0);
WWHD_OFFSET(JPADraw_l,mDrawFlags,0xF2);
WWHD_OFFSET(JPADraw_l,mUniformsUpdated,0xF3);
WWHD_OFFSET(JPADraw_l,mTextureCopyEnabled,0xF5);
WWHD_SIZE(JPADraw_l,0xF8);
}
