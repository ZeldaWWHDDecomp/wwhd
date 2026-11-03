// Render jobs of a display frame: the 0x1C-byte job records 027D5E5C constructs, their submit/execute methods and the
// priority comparator the render queue sorts them with.
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "agl/RenderQueue.h"
#include "agl/RenderInputCollections.h"
namespace agl { namespace render {
struct DisplayFrameState;
// Layer table slot +0x80 (function word +0x84): called by 027D72B8 before a layer's job is submitted; a bare blr in
// every shipped layer table. The merged RenderLayerMethods_l names the slots up to +0x78.
struct RenderLayerSubmitMethods_l : agl::RenderLayerMethods_l { sead::VtableSlot mBeforeSubmit; };
inline RenderLayerSubmitMethods_l* layerSubmitMethods(agl::RenderLayerPrefix* layer) {
    return static_cast<RenderLayerSubmitMethods_l*>(layer->methods());
}
// What a frame job does when it executes (027D74AC); the job's name is set from the same kind (027D6070).
enum RenderJobKind : u32 {
    kRenderJobDraw = 0,     // "Layer(Draw)": record the owner layer's draw pass
    kRenderJobBegin = 1,    // "Layer(Begin)": prepare the attached surface
    kRenderJobEnd = 2,      // "Layer(End)": run the frame's pass hook
    kRenderJobGpuCalc = 3,  // "Layer(GPUCalc)": record the owner layer's finish pass
};
struct RenderJobDispatch_l { u8 mPrefix[12]; be<u32> mExecute; u8 mDestroyPrefix[4]; be<u32> mDestroy; };
struct RenderJob {
    sead::SafeText mName;
    be<u8> mActive;
    u8 mPadding[3];
    gptr<RenderJobDispatch_l> mDispatch;
    be<u32> mKind;
    gptr<DisplayFrameState> mFrame;
    gptr<agl::RenderLayerPrefix> mOwner;
    s32 priority() const;
    void submit(RenderPointerArray*,agl::RenderLayerPrefix*);
    RenderJob* configureLayerCommand(u32,DisplayFrameState*);
    void execute();
};
RenderJob* createRenderJob(RenderJob*);
void releaseRenderJob(void*,u32);
s32 compareRenderJobs(const RenderJob*,const RenderJob*);
} }
WWHD_SIZE(agl::render::RenderJob,0x1C);
WWHD_OFFSET(agl::render::RenderJob,mActive,8);
WWHD_OFFSET(agl::render::RenderJob,mDispatch,0xC);
WWHD_OFFSET(agl::render::RenderJob,mKind,0x10);
WWHD_OFFSET(agl::render::RenderJob,mFrame,0x14);
WWHD_OFFSET(agl::render::RenderJob,mOwner,0x18);
WWHD_OFFSET(agl::render::RenderJobDispatch_l,mExecute,0xC);
WWHD_OFFSET(agl::render::RenderJobDispatch_l,mDestroy,0x14);
WWHD_OFFSET(agl::render::RenderLayerSubmitMethods_l,mBeforeSubmit,0x80);
WWHD_OFFSET(agl::RenderLayerPrefix,mRenderState,0x5C);
