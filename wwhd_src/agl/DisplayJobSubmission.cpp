// Job submission (RenderQueueManager::collectJobs 027C1848 / executeView): the two fixed jobs go first with no layer,
// then one job per pending layer after the layer's before-submit hook (table +0x80); indices past a table's count
// fall back to its first job (compiler bound check). The queue may be NULL (collectJobs without flag 4).
#include "agl/DisplayFrameState.h"
using namespace agl::render;
static RenderJob* boundedJob(const RenderJobTable* table,u32 index) {
    return index < (u32)table->mCount ? table->at(index) : table->at(0);
}
/* 027D72B8 */
void agl::render::DisplayFrameState::submitPrimaryJobs(RenderPointerArray* queue) {
    gabi::AutoFrame frame{gabi::current_function()};
    RenderJob* job = mPrimaryJobs.mJobs;
    job->submit(queue,NULL);
    job = boundedJob(&mPrimaryJobs,1);
    job->submit(queue,NULL);
    GuestRange<gptr<agl::RenderLayerPrefix>> pending = pointerRange<agl::RenderLayerPrefix>(mPending);
    gptr<agl::RenderLayerPrefix>* slot = pending.first;
    gptr<agl::RenderLayerPrefix>* end = pending.end;
    u32 index = 2;
    for (; slot != end; ++slot) {
        agl::RenderLayerPrefix* layer = *slot;
        gabi::call_ptr<void>(layerSubmitMethods(layer)->mBeforeSubmit.mFunction,layer);
        job = boundedJob(&mPrimaryJobs,index++);
        job->submit(queue,*slot);
    }
    mFlags = (u32)mFlags | 0x40u;
}
/* 027D7384 */
void agl::render::DisplayFrameState::submitSecondaryJobs(RenderPointerArray* queue) {
    gabi::AutoFrame frame{gabi::current_function()};
    GuestRange<gptr<agl::RenderLayerPrefix>> pending = pointerRange<agl::RenderLayerPrefix>(mPending);
    gptr<agl::RenderLayerPrefix>* slot = pending.first;
    gptr<agl::RenderLayerPrefix>* end = pending.end;
    for (u32 index = 0; slot != end; ++slot) {
        RenderJob* job = boundedJob(&mSecondaryJobs,index++);
        job->submit(queue,*slot);
    }
    mCurrentJob = 0;
}
/* WWHD glue
*/
