// HD pipeline render-state construction; embedded extents follow its own allocation and offsets.
#include "JSystem/J3DGraph/PipelineRenderState.h"
#include "bindings.h"
using namespace j3d::pipeline;
void* allocatePipelineStorage(u32);
void createPipelineRenderBuffer(RenderBufferStorage_l*);
void createPipelineRenderSurface(RenderSurfaceStorage_l*);
static u8 sPipelineRendererMethods;
static PipelineRenderState* sPipelineRenderer;
static sead::Disposer* sPipelineRendererDisposer;
static sead::DisposerVtable sRendererDisposerMethods;
void* allocatePipelineRenderStorage(u32,sead::RegistryHeap*,s32);
void initializePipelineDisposer(sead::Disposer*,sead::RegistryHeap*,s32);
void finalizePipelineDisposer(sead::Disposer*,u32);
void destroyPipelineRenderBuffer(RenderBufferStorage_l*,u32);
void releasePipelineStorage(void*);
namespace j3d::pipeline {
/* 027F7D78 */
PipelineRenderState* PipelineRenderState::create(PipelineRenderState* state) {
    gabi::AutoFrame frame{0x027F7D78u};
    if(state==nullptr) state=static_cast<PipelineRenderState*>(allocatePipelineStorage(sizeof(PipelineRenderState)));
    if(state!=nullptr) {
        state->mMethods=&sPipelineRendererMethods;
        // Retain the original modular embedded-address check; valid storage never wraps.
        if(gabi::ea(state)+static_cast<u32>(offsetof(PipelineRenderState,mSlots))==0)
            allocatePipelineStorage(sizeof(state->mSlots));
        sead::CompactRenderState::create(&state->mRenderState);
        state->mFlag=0;
        createPipelineRenderBuffer(&state->mBuffer);
        createPipelineRenderSurface(&state->mSurface);
        state->mRenderState.mDepthWrite=0;
        state->mRenderState.mDepthTest=0;
        state->mRenderState.mBlendEnable=0;
    }
    return state;
}
/* 027F7E08 */
PipelineRenderState* PipelineRenderState::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F7E08u};
    PipelineRenderState* state=sPipelineRenderer;
    if(state!=nullptr) return state;
    state=static_cast<PipelineRenderState*>(allocatePipelineRenderStorage(sizeof(PipelineRenderState),heap,4));
    sead::Disposer* disposer=gabi::at<sead::Disposer>(gabi::ea(state)+offsetof(PipelineRenderState,mDisposer));
    if(disposer!=nullptr) {
        initializePipelineDisposer(disposer,heap,3);
        disposer->mVtable=&sRendererDisposerMethods;
    }
    sPipelineRendererDisposer=disposer;
    if(state!=nullptr) state=PipelineRenderState::create(state);
    sPipelineRenderer=state;
    return state;
}
/* 027F7EA8 */
void PipelineRenderState::destroy(PipelineRenderState* state,u32 flags) {
    gabi::AutoFrame frame{0x027F7EA8u};
    if(state==nullptr) return;
    state->mMethods=&sPipelineRendererMethods;
    destroyPipelineRenderBuffer(&state->mBuffer,2);
    if((flags&1u)!=0) releasePipelineStorage(state);
}
/* 027F8E78 */
void PipelineRendererDisposer::destroy(sead::Disposer* disposer,u32 flags) {
    gabi::AutoFrame frame{0x027F8E78u};
    if(disposer==nullptr) return;
    disposer->mVtable=&sRendererDisposerMethods;
    if(disposer==sPipelineRendererDisposer.get()) {
        PipelineRenderState* state=sPipelineRenderer;
        sPipelineRendererDisposer=nullptr;
        PipelineRenderState::destroy(state,2);
        sPipelineRenderer=nullptr;
    }
    finalizePipelineDisposer(disposer,0);
    if((flags&1u)!=0) releasePipelineStorage(disposer);
}
}
/* WWHD glue
data sPipelineRendererMethods 1016E9F0
data sPipelineRenderer 101F9968
data sPipelineRendererDisposer 101F996C
data sRendererDisposerMethods 1016EAFC
ext 0273B0D4 void* allocatePipelineRenderStorage(u32,sead::RegistryHeap*,s32)
ext 02752B0C void initializePipelineDisposer(sead::Disposer*,sead::RegistryHeap*,s32)
ext 02752BEC void finalizePipelineDisposer(sead::Disposer*,u32)
ext 027B9694 void destroyPipelineRenderBuffer(j3d::pipeline::RenderBufferStorage_l*,u32)
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 027B95B4 void createPipelineRenderBuffer(j3d::pipeline::RenderBufferStorage_l*)
ext 027B6CB8 void createPipelineRenderSurface(j3d::pipeline::RenderSurfaceStorage_l*)
*/
