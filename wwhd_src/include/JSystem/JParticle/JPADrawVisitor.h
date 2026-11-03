/** JPA1 draw visitor helpers. HD: per-view emitter positions and inline draw parameters. */
#pragma once
#include "JSystem/JParticle/JPA_particle.h"
#include "JSystem/JMath/JMATrigonometric.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include "JSystem/JParticle/JPA_math_drawcalc.h"
#include "JSystem/JParticle/JPA_gpu_pool.h"
#include "JSystem/JParticle/JPAResourceManager.h"
namespace jpa_drawvisitor {
using Emitter=jpa_emitter_calc::JPABaseEmitter;
using DrawParams = JPAMathDraw::JPADrawParams;
struct Vec { be<u32> x,y,z; };
struct VecFloat {be<f32> x,y,z;};
struct Vec2 {be<f32> x,y;};
struct PointsCross {VecFloat values[8];};
struct Points {VecFloat values[4];};
struct Corners {Vec2 values[4];};
WWHD_SIZE(Corners,32);WWHD_SIZE(VecFloat,12);
struct Vertex {be<f32> x,y,z,s,t;};
struct CacheLine {be<u32> words[8];};
WWHD_SIZE(Vertex,20);
WWHD_OFFSET(JPAGpuPool<4>,mState,0x950);
struct Matrix { be<f32> m[3][4]; };
struct BaseShapeVtable {
 u8 prefix[0x4C];be<u32> getColorLoopOffset;u8 gapColor[4];be<u32> getTextureLoopOffset;
 u8 beforeGlobalTexture[0x64-0x58];be<u32> isEnableTextureAnmGlobal;u8 beforeChildOrder[4];be<u32> getChildOrder;u8 beforeTextureAnm[0x11C-0x70];be<u32> isEnableTextureAnm;u8 gapAnm[4];be<u32> textureIsEmpty;u8 gapTexture[0x134-0x128];be<u32> getTextureAnmKeyNum;u8 gapKey[4];be<u32> getTextureIndex;
 u8 gapIndex[4];be<u32> getTextureIndexFrame;
 u8 gapFrame[0x174-0x148];be<u32> getPrmColor;u8 gapPrm[12];be<u32> getEnvColor;
 u8 gapEnv[12];be<u32> getColorRegAnmMaxFrm;
 u8 beforeTiling[0xC];
 be<u32> getTilingX;u8 gapgetTilingX[4];
 be<u32> getTilingY;u8 gapgetTilingY[4];
 be<u32> getTexStaticTransX;u8 gapgetTexStaticTransX[4];
 be<u32> getTexStaticTransY;u8 gapgetTexStaticTransY[4];
 be<u32> getTexStaticScaleX;u8 gapgetTexStaticScaleX[4];
 be<u32> getTexStaticScaleY;u8 gapgetTexStaticScaleY[4];
 be<u32> getTexScrollTransX;u8 gapgetTexScrollTransX[4];
 be<u32> getTexScrollTransY;u8 gapgetTexScrollTransY[4];
 be<u32> getTexScrollScaleX;u8 gapgetTexScrollScaleX[4];
 be<u32> getTexScrollScaleY;u8 gapgetTexScrollScaleY[4];
 be<u32> getTexScrollRotate;u8 gapgetTexScrollRotate[4];

};
struct BaseShape {gptr<BaseShapeVtable> __vtbl;};
struct Color {be<u8> r,g,b,a;};
struct ColorFloat {be<f32> rgba[4];};
struct ClipBoard {
 u8 mSetupTev[4];be<f32> mGlobalScaleX,mGlobalScaleY,mPivotX,mPivotY;
 be<f32> mTexCoordPt[4][2];gptr<Matrix> mDrawMtxPtr;
 be<f32> mProjectionMtx[4][4];Matrix mDrawYBBMtx,mDrawMtx;
 Color mPrmColor,mEnvColor;be<u32> mDirTypeFunc,mRotTypeFunc,mBasePlaneTypeFunc;
 be<f32> mScaleAnmTiming;be<s16> mColorAnmFrame;u8 tail[2];
};
struct ParticleCallbackVtable {u8 prefix[0x24];be<u32> execute;u8 gap[4];be<u32> draw;};
struct ParticleCallback {gptr<ParticleCallbackVtable> __vtbl;};
struct SweepShapeVtable {u8 prefix[0xDC];be<u32> getPrm;u8 gap[4];be<u32> getEnv;};
struct SweepShape {gptr<SweepShapeVtable> __vtbl;};
struct Context {gptr<ClipBoard> pcb;gptr<ClipBoard> viewClip[2];gptr<Emitter> pbe;gptr<BaseShape> pbsp;gptr<void> pesp;gptr<SweepShape> pssp;gptr<void> petsp;gptr<void> mpDraw;gptr<void> mpList;gptr<void> mpTexResource;gptr<be<u16>> pTexIdx;};
WWHD_SIZE(DrawParams,0x3C);
WWHD_OFFSET(DrawParams,mLoopOffset,0x28);
WWHD_OFFSET(DrawParams,mTexIdx,0x3A);
WWHD_SIZE(Vec,12);WWHD_SIZE(Matrix,0x30);
WWHD_OFFSET(Context,pbe,0xC);WWHD_OFFSET(Context,pbsp,0x10);WWHD_OFFSET(Context,pTexIdx,0x2C);
WWHD_SIZE(Context,0x30);
WWHD_OFFSET(ClipBoard,mPrmColor,0xD8);WWHD_SIZE(Color,4);WWHD_SIZE(ColorFloat,16);
WWHD_OFFSET(ClipBoard,mColorAnmFrame,0xF0);
WWHD_OFFSET(BaseShapeVtable,getTextureIndexFrame,0x144);WWHD_OFFSET(BaseShapeVtable,getColorRegAnmMaxFrm,0x194);
struct JPADrawCalcColorPrm {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorEnv {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorAnmFrameNormal {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorAnmFrameRepeat {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorAnmFrameReverse {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorAnmFrameMerge {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcColorAnmFrameRandom {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcTextureAnmIndexNormal {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcTextureAnmIndexRepeat {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcTextureAnmIndexReverse {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcTextureAnmIndexMerge {void calc(const Context*,JPABaseParticle*);};
struct JPADrawCalcTextureAnmIndexRandom {void calc(const Context*,JPABaseParticle*);};
struct JPADrawExecCallBack {void exec(const Context*,JPABaseParticle*);void draw(const Context*,JPABaseParticle*);};
struct JPADrawExecLoadTexture {void exec(const Context*);void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecLoadDefaultTexture {void exec(const Context*);};
struct JPADrawExecSetPointSize {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecSetLineWidth {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRegisterPrmColorAnm {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRegisterPrmAlphaAnm {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRegisterEnvColorAnm {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRegisterPrmCEnv {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRegisterPrmAEnv {void exec(const Context*,JPABaseParticle*);};

struct JPADrawExecRegisterColorEmitterPE {void exec(const Context*);};
struct JPADrawExecRegisterColorEmitterP {void exec(const Context*);};
struct JPADrawExecRegisterColorEmitterE {void exec(const Context*);};
struct JPADrawExecDraw {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecDrawLine {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecDrawCross {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecGenIdtMtx {void exec(const Context*);};

struct JPADrawExecGenPrjMtx {void exec(const Context*);};

struct JPADrawExecPoint {void exec(const Context*,JPABaseParticle*);};

struct JPADrawExecBillBoard {void exec(const Context*,JPABaseParticle*);};

struct JPADrawExecRotBillBoard {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecYBillBoard {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRotYBillBoard {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecDirectional {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRotDirectional {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecSetTexMtx {void exec(const Context*,JPABaseParticle*);void exec(const Context*);};
struct JPADrawExecGenPrjTexMtx {void exec(const Context*);};

struct JPADrawExecRegisterColorChildPE {void exec(const Context*);};

// HD: Cafe descriptor storage and its cached GPU binding metadata.

struct JPADrawExecRotation {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRotationCross {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecDirBillBoard {void exec(const Context*,JPABaseParticle*);};

struct JPADrawExecDirectionalCross {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecRotDirectionalCross {void exec(const Context*,JPABaseParticle*);};
struct JPADrawExecStripeSubmit {void exec(const Context*);};

struct JPADrawExecStripe {void exec(const Context*);};
struct JPADrawExecStripeCross {void exec(const Context*);};
}
u32 JPADrawEmitterGroup(jpa_emitter_calc::JPABaseEmitter*);
void JPADrawAssert(const char*,u32,const char*);
void JPASetPointSize(JPABaseParticle*,u8);
void JPASetPrmColor(JPABaseParticle*,const jpa_drawvisitor::ColorFloat*);
void JPASetEnvColor(JPABaseParticle*,const jpa_drawvisitor::ColorFloat*);
void JPAIssueDraw(JPABaseParticle*,u32,u32,u32);
void JPASetEmitterTextureMtx(jpa_emitter_calc::JPABaseEmitter*,const jpa_drawvisitor::Matrix*);
WWHD_OFFSET(jpa_drawvisitor::ClipBoard,mProjectionMtx,0x38);
WWHD_OFFSET(jpa_drawvisitor::ClipBoard,mDrawMtx,0xA8);

void JPADrawFinish(JPABaseParticle*,u32);
void JPAVec2Construct(void*);
using JPAVecCtor = void(*)(void*);
void JPAConstructArray(void*,u32,u32,JPAVecCtor);
void JPAMultVec(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::VecFloat*,jpa_drawvisitor::VecFloat*);
bool JPAInverse(const jpa_drawvisitor::Matrix*,jpa_drawvisitor::Matrix*);
void JPAVec3Construct(void*);
void JPAMultVecArray(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::Points*,jpa_drawvisitor::Points*,u32);
void JPAMultVecSR(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::VecFloat*,jpa_drawvisitor::VecFloat*);
f64 JPADrawSqrt(f32);
void JPAConcat(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::Matrix*,jpa_drawvisitor::Matrix*);

void JPAGpuTextureMtx(JPAGpuState*,const jpa_drawvisitor::Matrix*,u32);
JPAMathDraw::JPATextureState* JPAGetTextureBinding(JPAParticleRenderer*,u32);
void JPAApplyTexture(JPAMathDraw::JPATextureState*,const JPAMathDraw::JPATextureDescriptor*);

void JPAMultVecCrossArray(const jpa_drawvisitor::Matrix*,const jpa_drawvisitor::PointsCross*,jpa_drawvisitor::PointsCross*,u32);
JPAMathDraw::JPATextureDescriptor* JPAStripeDescriptor(void*,u32);

void JPAPrepareStripe(JPABaseParticle*,jpa_emitter_calc::JPABaseEmitter*);

WWHD_SIZE(jpa_drawvisitor::Vec2,8);
WWHD_SIZE(jpa_drawvisitor::Points,48);
WWHD_SIZE(jpa_drawvisitor::PointsCross,96);
WWHD_SIZE(jpa_drawvisitor::CacheLine,32);
WWHD_SIZE(jpa_drawvisitor::BaseShapeVtable,0x1FC);
WWHD_SIZE(jpa_drawvisitor::BaseShape,4);
WWHD_SIZE(jpa_drawvisitor::ClipBoard,0xF4);
