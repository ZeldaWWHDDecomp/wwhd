// A finished job's command (RenderJob::execute 027D74AC) joins the deferred list only when its display list recorded
// something; the command remembers its layer, takes an atomically reserved slot, and the command listener is told.
#include "agl/DisplayFrameState.h"
#include "sead/ElementAt.h"
#include "agl/RenderDispatchContext.h"
using namespace agl::render;
/* 027D71D4 */
void agl::render::DisplayFrameState::appendDeferred(agl::gx2util::TimedCounter* command,agl::RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)recording(command)->mUsed) {
        command->mRate = gabi::ea(layer);
        u32 counter = gabi::ea(&mDeferred.mCount);
        u32 index;
        do { index = gabi::load_reserved(counter); } while (!gabi::store_conditional(counter,index + 1u));
        sead::elementAt<gptr<agl::gx2util::TimedCounter>>(gabi::ea(deferredCommands()),index) = command; /* guest EA arithmetic: wraps at 32 bits as on the console */
        if (mCommandHook) {
            gabi::FrameLocal<RenderDispatchContext> context(8);
            u32 category = mCategory;
            u32 target = gabi::ea(mRenderTarget);
            createDirectDispatchContext(context.get(),category,target);
            RenderFrameHook_l* hook = mCommandHook;
            context->mKind = 4;
            hook->mListener.invoke(context.get());
        }
    }
}
/* WWHD glue
*/
