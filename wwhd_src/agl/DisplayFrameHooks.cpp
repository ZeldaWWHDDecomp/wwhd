// The pass listener runs on a stack dispatch context built for the view's category and render target.
#include "agl/DisplayFrameState.h"
#include "agl/RenderDispatchContext.h"
using namespace agl::render;
/* 027D6BD0 */
void agl::render::DisplayFrameState::invokePassHook() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mPassHook) {
        gabi::FrameLocal<RenderDispatchContext> context(8);
        u32 category = mCategory;
        u32 target = gabi::ea(mRenderTarget);
        createDirectDispatchContext(context.get(),category,target);
        category = mCategory;
        RenderFrameHook_l* hook = mPassHook;
        context->mKind = category + 2u;
        hook->mListener.invoke(context.get());
    }
}
/* WWHD glue
*/
