// Render job records: construction, kind/name setup, queue submission, release and the descending-priority comparator.
#include "agl/RenderJobOrder.h"
using namespace agl::render;
void* allocateRenderJobStorage(u32);
void releaseRenderJobStorage(void*);
static sead::SafeTextVtable sRenderJobTextMethods_l;
static RenderJobDispatch_l sRenderJobMethods_l;
static void setJobName(sead::SafeText* name,const char* text) { name->mText.v = gabi::guest_str(text); }
/* 027D76B8 */
s32 agl::render::RenderJob::priority() const {
    agl::RenderLayerPrefix* owner = mOwner;
    return owner ? (s32)(u32)owner->mRenderState : -10000;
}
/* 027D76D4 */
s32 agl::render::compareRenderJobs(const RenderJob* left,const RenderJob* right) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 rightPriority = right->priority();
    s32 leftPriority = left->priority();
    return (s32)((u32)rightPriority - (u32)leftPriority);
}
/* 027D7270 */
void agl::render::RenderJob::submit(RenderPointerArray* queue,agl::RenderLayerPrefix* owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner = owner;
    if (queue) {
        s32 count = queue->mCount;
        if (count < (s32)queue->mCapacity) {
            queue->entries()[count] = gabi::ea(this);
            queue->mCount = (u32)(s32)queue->mCount + 1u;
        }
    } else {
        gabi::call_ptr<void>(mDispatch->mExecute,this);
    }
}
/* 027D7498 */
void agl::render::releaseRenderJob(void* job,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (job && (flags & 1u)) releaseRenderJobStorage(job);
}
/* 027D6070 */
agl::render::RenderJob* agl::render::RenderJob::configureLayerCommand(u32 kind,DisplayFrameState* frame) {
    mFrame = frame;
    mKind = kind;
    if (kind == kRenderJobDraw) setJobName(&mName,"Layer(Draw)");
    else if (kind == kRenderJobBegin) setJobName(&mName,"Layer(Begin)");
    else if (kind == kRenderJobEnd) setJobName(&mName,"Layer(End)");
    else if (kind == kRenderJobGpuCalc) setJobName(&mName,"Layer(GPUCalc)");
    return this;
}
/* 027D5E5C */
agl::render::RenderJob* agl::render::createRenderJob(RenderJob* job) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!job) job = static_cast<RenderJob*>(allocateRenderJobStorage(sizeof(RenderJob)));
    if (!job) return NULL;
    job->mName.mVtable = &sRenderJobTextMethods_l;
    setJobName(&job->mName,"");
    job->mFrame = NULL;
    job->mDispatch = &sRenderJobMethods_l;
    job->mOwner = NULL;
    job->mActive = 0;
    return job;
}
/* WWHD glue
str 1016C2B0 "Layer(Draw)"
str 1016C2BC "Layer(Begin)"
str 1016C2CC "Layer(End)"
str 1016C2D8 "Layer(GPUCalc)"
str 1016C2AC ""
data sRenderJobTextMethods_l 1016C180
data sRenderJobMethods_l 1016C360
ext 0273AD10 void* allocateRenderJobStorage(u32)
ext 0273AF40 void releaseRenderJobStorage(void*)
*/
