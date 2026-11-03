// Draw sequencing starts from tww JPADraw::draw; Cafe adds projection and alternate banks.
#include "JSystem/JParticle/JPADraw_passes.h"
#include "JSystem/JParticle/JPADraw_gpu_state.h"
using namespace JPADrawExtension;
static JPADrawRenderState_l sDefaultDrawRenderState;
void JPADrawProjectionCopy(const void*,void*);
void JPADrawPassSetupTev(void*,void*,void*);
void JPADrawPassDepth(JPADraw_l*);
void JPADrawPassDepthAlternate(JPADraw_l*,u32);
void JPADrawPassChild(JPADraw_l*);
void JPADrawPassParticle(JPADraw_l*);
void JPADrawPassAlternateChild(JPADraw_l*,u32);
void JPADrawPassAlternateParticle(JPADraw_l*,u32);
void JPADrawApplyPassState(JPADrawRenderState_l*);
void JPADrawPassSetChildClipboard(JPADraw_l*,s32);
void JPADrawPassSetParticleClipboard(JPADraw_l*,s32);
void JPADrawPassDepthEmitterOnly(JPADraw_l*);
/* 0282BEC8 */
void JPADraw_l::draw(const Mtx34* drawMtx,const void* projection) {
    auto colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mPrmColor = u32(colors->scaleAndColors().mGlobalPrmColor);
    colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mEnvColor = u32(colors->scaleAndColors().mGlobalEnvColor);
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mDrawMtxPtr = const_cast<Mtx34*>(drawMtx);
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    JPADrawProjectionCopy(projection,cb->mProjectionMtx);
    JPADrawPassSetupTev(gabi::at<JPADrawClipBoard_l>(u32(dc.pcb)),dc.pbsp.get(),dc.petx.get());
    for (s32 i = 0; i < u8(execution().execEmtrVisNum); ++i) {
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrVis[i].get())->exec(&dc);
    }
    if (baseShape()->isEnableAnmTone()) JPADrawPassDepth(this);
    mDrawFlags = u8(mDrawFlags) & ~1;
    if (childOrder() && dc.pssp.get()) JPADrawPassChild(this);
    JPADrawPassParticle(this);
    if (!childOrder() && dc.pssp.get()) JPADrawPassChild(this);
    JPADrawApplyPassState(&sDefaultDrawRenderState);
}
/* 0282CB9C */
void JPADraw_l::drawAlternateFirst(const Mtx34* drawMtx,const void* projection) {
    auto cb = drawAlternateClipBoards();
    auto colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb->mPrmColor = u32(colors->scaleAndColors().mGlobalPrmColor);
    colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb->mEnvColor = u32(colors->scaleAndColors().mGlobalEnvColor);
    cb->mDrawMtxPtr = const_cast<Mtx34*>(drawMtx);
    JPADrawProjectionCopy(projection,cb->mProjectionMtx);
    if (baseShape()->isEnableAnmTone()) JPADrawPassDepthAlternate(this,0);
    mDrawFlags = u8(mDrawFlags) & ~1;
    if (childOrder() && dc.pssp.get()) JPADrawPassAlternateChild(this,0);
    JPADrawPassAlternateParticle(this,0);
    if (!childOrder() && dc.pssp.get()) JPADrawPassAlternateChild(this,0);
}
/* 0282CCAC */
void JPADraw_l::drawAlternateSecond(const Mtx34* drawMtx,const void* projection) {
    auto cb = &drawAlternateClipBoards()[1];
    auto colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb->mPrmColor = u32(colors->scaleAndColors().mGlobalPrmColor);
    colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb->mEnvColor = u32(colors->scaleAndColors().mGlobalEnvColor);
    cb->mDrawMtxPtr = const_cast<Mtx34*>(drawMtx);
    JPADrawProjectionCopy(projection,cb->mProjectionMtx);
    if (baseShape()->isEnableAnmTone()) JPADrawPassDepthAlternate(this,1);
    mDrawFlags = u8(mDrawFlags) & ~1;
    if (childOrder() && dc.pssp.get()) JPADrawPassAlternateChild(this,1);
    JPADrawPassAlternateParticle(this,1);
    if (!childOrder() && dc.pssp.get()) JPADrawPassAlternateChild(this,1);
}
/* 0282C038 */
void JPADraw_l::zDrawEmitterOnly() {
    mDrawFlags = u8(mDrawFlags) | 1;
    if (childOrder() && dc.pssp.get()) {
        mDrawFlags = u8(mDrawFlags) | 2;
        JPADrawPassSetChildClipboard(this,-1);
        auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
        dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
        for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
            reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    }
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawPassSetParticleClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mActive);
    for (s32 i = 0; i < u8(execution().execEmtrPVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrPVis[i].get())->exec(&dc);
    if (!childOrder() && dc.pssp.get()) {
        mDrawFlags = u8(mDrawFlags) | 2;
        JPADrawPassSetChildClipboard(this,-1);
        emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
        dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
        for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
            reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    }
}
/* 0282C1EC */
void JPADraw_l::drawEmitterOnly(const Mtx34* drawMtx,const void* projection) {
    auto colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    auto cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mPrmColor = u32(colors->scaleAndColors().mGlobalPrmColor);
    colors = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mEnvColor = u32(colors->scaleAndColors().mGlobalEnvColor);
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    cb->mDrawMtxPtr = const_cast<Mtx34*>(drawMtx);
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    JPADrawProjectionCopy(projection,cb->mProjectionMtx);
    for (s32 i = 0; i < u8(execution().execEmtrVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrVis[i].get())->exec(&dc);
    if (baseShape()->isEnableAnmTone()) JPADrawPassDepthEmitterOnly(this);
    mDrawFlags = u8(mDrawFlags) & ~1;
    if (childOrder() && dc.pssp.get()) {
        mDrawFlags = u8(mDrawFlags) | 2;
        JPADrawPassSetChildClipboard(this,-1);
        auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
        dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
        for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
            reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    }
    mDrawFlags = u8(mDrawFlags) & ~2;
    JPADrawPassSetParticleClipboard(this,-1);
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
    dc.mpActiveParticles = gabi::ea(&emitter->particles().mActive);
    for (s32 i = 0; i < u8(execution().execEmtrPVisNum); ++i)
        reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrPVis[i].get())->exec(&dc);
    if (!childOrder() && dc.pssp.get()) {
        mDrawFlags = u8(mDrawFlags) | 2;
        JPADrawPassSetChildClipboard(this,-1);
        emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(dc.pbe.get());
        dc.mpActiveParticles = gabi::ea(&emitter->particles().mChild);
        for (s32 i = 0; i < u8(execution().execEmtrCVisNum); ++i)
            reinterpret_cast<JPADrawExecVisitor_l*>(execution().mpExecEmtrCVis[i].get())->exec(&dc);
    }
}
/* WWHD glue
ext 0282AC2C void JPADrawPassSetChildClipboard(JPADraw_l*,s32)
ext 0282B280 void JPADrawPassSetParticleClipboard(JPADraw_l*,s32)
ext 0282C038 void JPADrawPassDepthEmitterOnly(JPADraw_l*)
data sDefaultDrawRenderState 104B474C
ext 028E8970 void JPADrawProjectionCopy(const void*,void*)
ext 028303D8 void JPADrawPassSetupTev(void*,void*,void*)
ext 0282BA44 void JPADrawPassDepth(JPADraw_l*)
ext 0282C750 void JPADrawPassDepthAlternate(JPADraw_l*,u32)
ext 0282BAE0 void JPADrawPassChild(JPADraw_l*)
ext 0282BCD4 void JPADrawPassParticle(JPADraw_l*)
ext 0282C804 void JPADrawPassAlternateChild(JPADraw_l*,u32)
ext 0282C9D0 void JPADrawPassAlternateParticle(JPADraw_l*,u32)
ext 02750370 void JPADrawApplyPassState(JPADrawRenderState_l*)
*/
