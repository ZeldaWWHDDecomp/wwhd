// HD: shader preparation surrounds the GC parent/child draw ordering.
#include "JSystem/JParticle/JPADraw_shader.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
using namespace JPADrawExtension;
static JPADrawGlobalContext_l sDrawGlobalContext;
void JPADrawShaderSetupState(JPADraw_l*);
void JPADrawPrepareVertexState(JPAEmitterVertexState*);
JPADrawShaderCache_l* JPADrawGetShaderCache(JPADrawGlobalContext_l*);
void JPADrawPrepareShader(JPADrawShader_l*,u32);
void JPADrawPrepareShaderGroup(void*);
void JPADrawCallDisplayList(void*,u32);
void JPADrawLoadShader(JPADrawShader_l*);
void JPADrawShaderBindTextureUniforms(JPADraw_l*);
void JPADrawShaderCopyProjection(const void*,void*);
void JPADrawShaderCopyMatrix(const Mtx34*,Mtx34*);
void JPADrawShaderIdentity(Mtx34*);
void JPADrawShaderYBillboard(JPADraw_l*,const Mtx34*,JPADrawClipBoard_l*);
void JPADrawShaderDepthChild(JPADraw_l*);
void JPADrawShaderDepthParticle(JPADraw_l*);
void JPADrawShaderChild(JPADraw_l*);
void JPADrawShaderParticle(JPADraw_l*);
void JPADrawShaderApplyState(JPADrawRenderState_l*);
/* 0282D9C0 */
void JPADraw_l::drawHD(const Mtx34* drawMtx,const void* projection) {
    JPADrawShaderSetupState(this);
    auto emitter = reinterpret_cast<jpa_emitter_calc::JPABaseEmitter*>(dc.pbe.get());
    JPADrawPrepareVertexState(&emitter->mGpuState.mVertexState);
    auto cache = JPADrawGetShaderCache(&sDrawGlobalContext);
    auto program = gabi::at<jpa_emitter_extension::JPAShaderMaterial>(u32(emitter->mGpuState.mDrawContext));
    auto shader = static_cast<JPADrawShader_l*>(program->mShader.get());
    if (shader != cache->mShader.get()) {
        void* previousState = cache->mState.get();
        if (u8(shader->mFlags) & 2) {
            shader->mFlags = u8(shader->mFlags) & ~2;
            JPADrawPrepareShader(shader,0);
        }
        void* state = shader->mGroup->mState.get();
        if (previousState != state) JPADrawPrepareShaderGroup(state);
        u32 displayListSize = shader->mDisplayListSize;
        if (displayListSize) {
            JPADrawCallDisplayList(shader->mDisplayList.get(),displayListSize);
            cache->mState = state;
            cache->mShader = shader;
        } else {
            JPADrawLoadShader(shader);
            cache->mShader = shader;
            cache->mState = state;
        }
    }
    program = gabi::at<jpa_emitter_extension::JPAShaderMaterial>(u32(emitter->mGpuState.mDrawContext));
    auto vertexVtable = static_cast<const JPADrawVertexVtable_l*>(emitter->mGpuState.mVertexState.__vtbl.get());
    gabi::call_ptr<void>(vertexVtable->mBind,&emitter->mGpuState.mVertexState,program);
    JPADrawShaderBindTextureUniforms(this);
    auto cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    mUniformsUpdated[1] = 0;
    mUniformsUpdated[0] = 0;
    cb->mDrawMtxPtr = const_cast<Mtx34*>(drawMtx);
    cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
    JPADrawShaderCopyProjection(projection,cb->mProjectionMtx);
    switch (baseShape()->getType()) {
    case DrawDirBillboard:
        cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
        JPADrawShaderIdentity(&cb->mDrawMtx);
        break;
    case DrawYBillboard:
        cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
        JPADrawShaderYBillboard(this,static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),cb);
        break;
    default:
        cb = gabi::at<JPADrawClipBoard_l>(u32(dc.pcb));
        JPADrawShaderCopyMatrix(static_cast<Mtx34*>(cb->mDrawMtxPtr.get()),&cb->mDrawMtx);
        break;
    }
    if (baseShape()->isEnableAnmTone()) {
        if (childOrder() && dc.pssp.get()) JPADrawShaderDepthChild(this);
        JPADrawShaderDepthParticle(this);
        if (!childOrder() && dc.pssp.get()) JPADrawShaderDepthChild(this);
        JPADrawShaderSetupState(this);
    }
    if (childOrder() && dc.pssp.get()) JPADrawShaderChild(this);
    JPADrawShaderParticle(this);
    if (!childOrder() && dc.pssp.get()) JPADrawShaderChild(this);
    JPADrawShaderApplyState(&sDrawGlobalContext.mDefaultState);
}
/* WWHD glue
data sDrawGlobalContext 104B45C0
ext 0282CDBC void JPADrawShaderSetupState(JPADraw_l*)
ext 027FB678 void JPADrawPrepareVertexState(JPAEmitterVertexState*)
ext 027F29D4 JPADrawShaderCache_l* JPADrawGetShaderCache(JPADrawGlobalContext_l*)
ext 027BB9E0 void JPADrawPrepareShader(JPADrawShader_l*,u32)
ext 027B9F68 void JPADrawPrepareShaderGroup(void*)
ext C00060E0 void JPADrawCallDisplayList(void*,u32)
ext 027BB7CC void JPADrawLoadShader(JPADrawShader_l*)
ext 0282D094 void JPADrawShaderBindTextureUniforms(JPADraw_l*)
ext 028E8970 void JPADrawShaderCopyProjection(const void*,void*)
ext 028E90D4 void JPADrawShaderCopyMatrix(const Mtx34*,Mtx34*)
ext 028E9098 void JPADrawShaderIdentity(Mtx34*)
ext 0282AAF4 void JPADrawShaderYBillboard(JPADraw_l*,const Mtx34*,JPADrawClipBoard_l*)
ext 0282D680 void JPADrawShaderDepthChild(JPADraw_l*)
ext 0282D8EC void JPADrawShaderDepthParticle(JPADraw_l*)
ext 0282D4E8 void JPADrawShaderChild(JPADraw_l*)
ext 0282D754 void JPADrawShaderParticle(JPADraw_l*)
ext 02750370 void JPADrawShaderApplyState(JPADrawRenderState_l*)
*/
