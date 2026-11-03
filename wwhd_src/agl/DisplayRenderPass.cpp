// Layer passes: tail calls into the render layer with the view's category, render target and flag bit 3; the deferred
// command sort runs only after a submission marked the list unsorted (flag 0x40).
#include "agl/DisplayFrameState.h"
#include "agl/RenderQueue.h"
using namespace agl::render;
s32 compareDeferredCommands(const void*,const void*);
/* 027D6BB0 */
void agl::render::DisplayFrameState::prepareRenderPass(agl::RenderLayerPrefix* layer,u32 options) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = mFlags;
    sead::RenderSurface* surface = targetSurface();
    u32 category = mCategory;
    layer->render(category,surface,(flags >> 3) & 1u,options);
}
/* 027D6ED0 */
void agl::render::DisplayFrameState::finishRenderPass(agl::RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 flags = mFlags;
    u32 category = mCategory;
    sead::RenderSurface* surface = targetSurface();
    layer->dispatch(category,gabi::ea(surface),(flags >> 3) & 1u);
}
/* 027D6E80 */
void agl::render::DisplayFrameState::sortDeferred() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mFlags & 0x40u) {
        mDeferred.sort(gabi::guest_fn((const void*)compareDeferredCommands));
        mFlags = (u32)mFlags & ~0x40u;
    }
}
/* WWHD glue
ext 027C424C void agl::RenderLayerPrefix::render(u32,sead::RenderSurface*,u32,u32)
ext 027C4538 void agl::RenderLayerPrefix::dispatch(u32,u32,u32)
ext 027DA7D4 s32 compareDeferredCommands(const void*,const void*) callback
*/
