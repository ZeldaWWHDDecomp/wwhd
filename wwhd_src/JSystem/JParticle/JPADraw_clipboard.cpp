// Clipboard construction starts from tww JPADraw.cpp; HD supports two alternate banks.
#include "JSystem/JParticle/JPADraw_passes.h"
using namespace JPADrawExtension;
void JPADrawClipboardCopyMatrix(const Mtx34*,Mtx34*);
void JPADrawClipboardIdentity(Mtx34*);
void JPADrawClipboardYBillboard(JPADraw_l*,const Mtx34*,JPADrawClipBoard_l*);
void JPADrawClipboardDirVel(JPABaseParticle*,void*,cXyz&);
void JPADrawClipboardDirPos(JPABaseParticle*,void*,cXyz&);
void JPADrawClipboardDirPosInv(JPABaseParticle*,void*,cXyz&);
void JPADrawClipboardDirEmitter(JPABaseParticle*,void*,cXyz&);
void JPADrawClipboardDirPrevious(JPABaseParticle*,void*,cXyz&);
void JPADrawClipboardRotY(f32,f32,Mtx34&);
void JPADrawClipboardRotX(f32,f32,Mtx34&);
void JPADrawClipboardRotZ(f32,f32,Mtx34&);
void JPADrawClipboardRotXYZ(f32,f32,Mtx34&);
void JPADrawClipboardRotYJiggle(f32,f32,Mtx34&);
void JPADrawClipboardPlaneXY(f32,f32,f32,f32,cXyz*);
void JPADrawClipboardPlaneXZ(f32,f32,f32,f32,cXyz*);
/* 0282AC2C */
void JPADraw_l::setChildClipBoard(s32 parity) {
    auto cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    if (parity >= 0) cb = &drawAlternateClipBoards()[parity];
    // HD: only direction billboards use identity here; regular billboards use the supplied matrix.
    switch (sweepShape()->getType()) {
    case DrawDirBillboard: JPADrawClipboardIdentity(&cb->mDrawMtx); break;
    case DrawYBillboard: JPADrawClipboardYBillboard(this,static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),cb); break;
    default: JPADrawClipboardCopyMatrix(static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),&cb->mDrawMtx); break;
    }
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    f32 scaleY = emitter->scaleAndColors().mGlobalParticleScale[1];
    f32 scaleX = emitter->scaleAndColors().mGlobalParticleScale[0];
    if (!sweepShape()->isInheritedScale()) {
        cb->mGlobalScale.x = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,sweepShape()->getScaleX()),scaleX);
        cb->mGlobalScale.y = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,sweepShape()->getScaleY()),scaleY);
    } else {
        cb->mGlobalScale.x = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,baseShape()->getBaseSizeX()),scaleX);
        cb->mGlobalScale.y = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,baseShape()->getBaseSizeY()),scaleY);
    }
    if (sweepShape()->getType() == DrawPoint) {
        f32 x = cb->mGlobalScale.x,y = cb->mGlobalScale.y;
        cb->mGlobalScale.x = gabi::fmuls_ppc(x,1.02f);
        cb->mGlobalScale.y = gabi::fmuls_ppc(y,1.02f);
    } else if (sweepShape()->getType() == DrawLine) {
        f32 x = cb->mGlobalScale.x,y = cb->mGlobalScale.y;
        cb->mGlobalScale.x = gabi::fmuls_ppc(x,1.02f);
        cb->mGlobalScale.y = gabi::fmuls_ppc(y,0.4f);
    }
    cb->mDirTypeFunc = 0;
    cb->mPivot.y = 0.0f;
    cb->mTexCoordPt[3].y = 1.0f;
    cb->mTexCoordPt[1].x = 1.0f;
    cb->mTexCoordPt[2].y = 1.0f;
    cb->mTexCoordPt[2].x = 1.0f;
    cb->mPivot.x = 0.0f;
    cb->mBasePlaneTypeFunc = 0;
    cb->mTexCoordPt[3].x = 0.0f;
    cb->mRotTypeFunc = 0;
    cb->mTexCoordPt[1].y = 0.0f;
    cb->mTexCoordPt[0].y = 0.0f;
    cb->mTexCoordPt[0].x = 0.0f;
    if (sweepShape()->getType() == DrawDirection || sweepShape()->getType() == DrawDirectionCross ||
        sweepShape()->getType() == DrawDirBillboard || sweepShape()->getType() == DrawStripe || sweepShape()->getType() == DrawStripeCross) {
        switch (sweepShape()->getDirType()) {
        case 0: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirVel)); break;
        case 1: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPos)); break;
        case 2: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPosInv)); break;
        case 3: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirEmitter)); break;
        case 4: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPrevious)); break;
        }
    }
    if (sweepShape()->getType() == DrawDirection || sweepShape()->getType() == DrawDirectionCross ||
        sweepShape()->getType() == DrawRotation || sweepShape()->getType() == DrawRotationCross) {
        switch (sweepShape()->getRotType()) {
        case 0: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotY)); break;
        case 1: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotX)); break;
        case 2: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotZ)); break;
        case 3: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotXYZ)); break;
        case 4: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotYJiggle)); break;
        }
    }
    if (sweepShape()->getType() == DrawDirection || sweepShape()->getType() == DrawRotation) {
        switch (sweepShape()->getBasePlaneType()) {
        case 0: cb->mBasePlaneTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardPlaneXY)); break;
        case 1: cb->mBasePlaneTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardPlaneXZ)); break;
        }
    }
}
/* 0282B280 */
void JPADraw_l::setParticleClipBoard(s32 parity) {
    auto cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    if (parity >= 0) cb = &drawAlternateClipBoards()[parity];
    // HD: only direction billboards use identity here; regular billboards use the supplied matrix.
    switch (baseShape()->getType()) {
    case DrawDirBillboard: JPADrawClipboardIdentity(&cb->mDrawMtx); break;
    case DrawYBillboard: JPADrawClipboardYBillboard(this,static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),cb); break;
    default: JPADrawClipboardCopyMatrix(static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),&cb->mDrawMtx); break;
    }
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    f32 scaleX = emitter->scaleAndColors().mGlobalParticleScale[0];
    f32 scaleY = emitter->scaleAndColors().mGlobalParticleScale[1];
    cb->mGlobalScale.x = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,baseShape()->getBaseSizeX()),scaleX);
    cb->mGlobalScale.y = gabi::fmuls_ppc(gabi::fmuls_ppc(25.0f,baseShape()->getBaseSizeY()),scaleY);
    if (baseShape()->getType() == DrawPoint) {
        f32 x = cb->mGlobalScale.x,y = cb->mGlobalScale.y;
        cb->mGlobalScale.x = gabi::fmuls_ppc(x,1.02f);
        cb->mGlobalScale.y = gabi::fmuls_ppc(y,1.02f);
    } else if (baseShape()->getType() == DrawLine) {
        f32 x = cb->mGlobalScale.x,y = cb->mGlobalScale.y;
        cb->mGlobalScale.x = gabi::fmuls_ppc(x,1.02f);
        cb->mGlobalScale.y = gabi::fmuls_ppc(y,0.4f);
    }
    if (extraShape() && extraShape()->isEnableScale()) {
        f32 pivotX = gabi::fsubs_ppc(f32(extraShape()->getPivotX()),1.0f);
        cb->mPivot.x = gabi::fmuls_ppc(pivotX,cb->mGlobalScale.x);
        f32 pivotY = gabi::fsubs_ppc(f32(extraShape()->getPivotY()),1.0f);
        cb->mPivot.y = gabi::fmuls_ppc(pivotY,cb->mGlobalScale.y);
    } else {
        cb->mPivot.x = 0.0f;
        cb->mPivot.y = 0.0f;
    }
    f32 tilingX = baseShape()->getTilingX();
    f32 tilingY = baseShape()->getTilingY();
    cb->mTexCoordPt[0].x = 0.0f;
    cb->mTexCoordPt[0].y = 0.0f;
    cb->mTexCoordPt[1].x = tilingX;
    cb->mTexCoordPt[1].y = 0.0f;
    cb->mTexCoordPt[2].x = tilingX;
    cb->mTexCoordPt[2].y = tilingY;
    cb->mTexCoordPt[3].x = 0.0f;
    cb->mTexCoordPt[3].y = tilingY;
    if (!baseShape()->textureIsEmpty() && !baseShape()->isEnableTextureAnm())
        mTexIdx = u16(dc.pTexIdx.get()[baseShape()->getTextureIndex()]);
    cb->mDirTypeFunc = 0;
    cb->mRotTypeFunc = 0;
    cb->mBasePlaneTypeFunc = 0;
    if (baseShape()->getType() == DrawDirection || baseShape()->getType() == DrawDirectionCross ||
        baseShape()->getType() == DrawDirBillboard || baseShape()->getType() == DrawStripe || baseShape()->getType() == DrawStripeCross) {
        switch (baseShape()->getDirType()) {
        case 0: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirVel)); break;
        case 1: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPos)); break;
        case 2: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPosInv)); break;
        case 3: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirEmitter)); break;
        case 4: cb->mDirTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardDirPrevious)); break;
        }
    }
    if (baseShape()->getType() == DrawDirection || baseShape()->getType() == DrawDirectionCross ||
        baseShape()->getType() == DrawRotation || baseShape()->getType() == DrawRotationCross) {
        switch (baseShape()->getRotType()) {
        case 0: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotY)); break;
        case 1: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotX)); break;
        case 2: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotZ)); break;
        case 3: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotXYZ)); break;
        case 4: cb->mRotTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardRotYJiggle)); break;
        }
    }
    if (baseShape()->getType() == DrawDirection || baseShape()->getType() == DrawRotation) {
        switch (baseShape()->getBasePlaneType()) {
        case 0: cb->mBasePlaneTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardPlaneXY)); break;
        case 1: cb->mBasePlaneTypeFunc = gabi::guest_fn(reinterpret_cast<const void*>(JPADrawClipboardPlaneXZ)); break;
        }
    }
}
/* WWHD glue
ext 028E90D4 void JPADrawClipboardCopyMatrix(const Mtx34*,Mtx34*)
ext 028E9098 void JPADrawClipboardIdentity(Mtx34*)
ext 0282AAF4 void JPADrawClipboardYBillboard(JPADraw_l*,const Mtx34*,JPADrawClipBoard_l*)
ext 02830418 void JPADrawClipboardDirVel(JPABaseParticle*,void*,cXyz&) callback
ext 02830434 void JPADrawClipboardDirPos(JPABaseParticle*,void*,cXyz&) callback
ext 02830450 void JPADrawClipboardDirPosInv(JPABaseParticle*,void*,cXyz&) callback
ext 02830480 void JPADrawClipboardDirEmitter(JPABaseParticle*,void*,cXyz&) callback
ext 028304F0 void JPADrawClipboardDirPrevious(JPABaseParticle*,void*,cXyz&) callback
ext 028305DC void JPADrawClipboardRotY(f32,f32,Mtx34&) callback
ext 02830624 void JPADrawClipboardRotX(f32,f32,Mtx34&) callback
ext 0283066C void JPADrawClipboardRotZ(f32,f32,Mtx34&) callback
ext 028306B4 void JPADrawClipboardRotXYZ(f32,f32,Mtx34&) callback
ext 02830720 void JPADrawClipboardRotYJiggle(f32,f32,Mtx34&) callback
ext 0283078C void JPADrawClipboardPlaneXY(f32,f32,f32,f32,cXyz*) callback
ext 028307C8 void JPADrawClipboardPlaneXZ(f32,f32,f32,f32,cXyz*) callback
*/
