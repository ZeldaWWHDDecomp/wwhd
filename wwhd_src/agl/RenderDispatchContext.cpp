// Render dispatch contexts: built on the caller's stack for a layer render/dispatch (027C424C, 027C4538) or a render
// view frame hook; stores follow the original order.
#include "agl/RenderDispatchContext.h"
#include "agl/ShaderStageState.h"
using namespace agl::render;
static sead::ViewCamera sDefaultCamera_l;
static sead::ProjectionState sDefaultProjection_l;
/* 027D4E8C */
void agl::render::destroyDispatchText(sead::SafeText* text,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1u)) releaseRenderStorage(text);
}
/* 027D4EA4 */
agl::render::RenderDispatchContext* agl::render::createLayerDispatchContext(RenderDispatchContext* context,u32 phase,u32 target,u32 enabled,agl::RenderLayerPrefix* layer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!context) context = static_cast<RenderDispatchContext*>(allocateRenderStorage(sizeof(RenderDispatchContext)));
    if (!context) return nullptr;
    context->mPhase = phase;
    context->mTarget = target;
    context->mKind = -1;
    u32 index = layer->mManagerIndex;
    context->mLayer = layer;
    context->mLayerIndex = index;
    u32 flags = layer->mFlags;
    if ((flags & kLayerWorkspaceCamera) && (flags & kLayerCameraEnabled)) {
        context->mCamera = &layer->mWorkspace.mCamera;
    } else {
        sead::ViewCamera* camera = layer->mCamera;
        context->mCamera = camera ? camera : &sDefaultCamera_l;
    }
    if ((u32)layer->mFlags & kLayerActiveProjection) {
        context->mProjection = layer->mWorkspace.mActiveProjection;
    } else {
        sead::ProjectionState* projection = layer->mProjection;
        context->mProjection = projection ? projection : &sDefaultProjection_l;
    }
    context->mViewport = &layer->mScissor;
    context->mEnabled = !((u32)layer->mFlags & kLayerDrawDisabled) && enabled != 0;
    return context;
}
/* 027D4FE8 */
agl::render::RenderDispatchContext* agl::render::createDirectDispatchContext(RenderDispatchContext* context,u32 phase,u32 target) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!context) context = static_cast<RenderDispatchContext*>(allocateRenderStorage(sizeof(RenderDispatchContext)));
    if (!context) return nullptr;
    context->mTarget = target;
    context->mPhase = phase;
    context->mKind = 0;
    context->mLayerIndex = ~0u;
    context->mEnabled = 1;
    context->mProjection = nullptr;
    context->mLayer = nullptr;
    context->mCamera = nullptr;
    context->mViewport = nullptr;
    return context;
}
/* WWHD glue
data sDefaultCamera_l 104A2098
data sDefaultProjection_l 104A20FC
*/
