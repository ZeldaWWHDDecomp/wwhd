#pragma once
#include "JSystem/J3DGraph/PipelineLookupInstances.h"
#include "sead/RenderState.h"
#include "sead/HeapRegistry.h"
namespace j3d::pipeline {
struct RenderBufferStorage_l { u8 mStorage[0x40]; };
struct RenderSurfaceStorage_l { u8 mStorage[0x148]; };
struct PipelineRenderState {
    gptr<void> mMethods;
    sead::Disposer mDisposer;
    PipelineSlot_l mSlots[19];
    sead::CompactRenderState mRenderState;
    be<u8> mFlag;
    u8 mPadding[3];
    RenderBufferStorage_l mBuffer;
    RenderSurfaceStorage_l mSurface;
    u32 updateSlot(u32 index,void* owner);
    u32 updateScaledSlot(u32 index,void* owner);
    void copySlot(u32 index,void* owner,u32 applyState);
    void copyBetweenSlots(u32 destination,u32 source);
    static PipelineRenderState* create(PipelineRenderState*);
    static PipelineRenderState* instance(sead::RegistryHeap*);
    static void destroy(PipelineRenderState*,u32);
};
struct PipelineRendererDisposer { static void destroy(sead::Disposer*,u32); };
}
extern gptr<j3d::pipeline::PipelineRenderState>& sPipelineRenderer;
extern gptr<sead::Disposer>& sPipelineRendererDisposer;
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mSlots,0x14);
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mRenderState,0x2C0);
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mFlag,0x334);
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mBuffer,0x338);
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mSurface,0x378);
WWHD_SIZE(j3d::pipeline::PipelineRenderState,0x4C0);
WWHD_SIZE(j3d::pipeline::RenderBufferStorage_l,0x40);
WWHD_SIZE(j3d::pipeline::RenderSurfaceStorage_l,0x148);
WWHD_OFFSET(j3d::pipeline::PipelineRenderState,mDisposer,4);
