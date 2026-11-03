// Depth-blur full-surface draw setup, derived from the game's own frame and calls.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "sead/RenderState.h"
#include "sead/Viewport.h"

#include "agl/DepthBlurContext.h"

static sead::RenderVector2 sDepthBlurDrawScale;
static sead::RenderVector2 sDepthBlurDrawOffset;
static void constructDepthDrawViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyDepthDrawSurface(sead::RenderSurface* surface);
static void applyDepthDrawViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void constructDepthDrawState(sead::CompactRenderState* state);
static void applyDepthDrawState(sead::CompactRenderState* state);
static u32 drawDepthBlurAdd(void* renderer, sead::Viewport* viewport, sead::RenderVector2* scale,
    sead::RenderVector2* offset, u32 mode, f32 strength);

/* 02780C34 */
static u32 drawDepthBlurFullSurface(void*, const DepthBlurContext* context, u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::Viewport> viewport(0x8C);
    constructDepthDrawViewport(viewport, frameSurface(context->mOutput));
    applyDepthDrawSurface(frameSurface(context->mOutput));
    applyDepthDrawViewport(viewport, frameSurface(context->mOutput));
    gabi::FrameLocal<sead::CompactRenderState> state(0x18);
    constructDepthDrawState(state);
    state->mDepthWrite = 0;
    state->mWriteRed = 1;
    state->mBlendEnable = 0;
    state->mWriteBlue = 1;
    state->mWriteGreen = 1;
    state->mDepthTest = 0;
    state->mWriteAlpha = 1;
    applyDepthDrawState(state);
    gabi::FrameLocal<sead::RenderVector2> doubled(8), scale(0x10);
    const f32 x = sDepthBlurDrawScale.x;
    doubled->x = gabi::fadds_ppc(x, x);
    const f32 y = sDepthBlurDrawScale.y;
    doubled->y = gabi::fadds_ppc(y, y);
    scale->x = doubled->x;
    scale->y = doubled->y;
    const f32 strength = gabi::fsubs_ppc(context->mParameters->mLevel.mValue, 1.0f);
    return drawDepthBlurAdd(&context->mTargets->mBlurTarget, viewport, scale, &sDepthBlurDrawOffset, mode, strength);
}

/* WWHD glue
 data sDepthBlurDrawScale 104A0920
 data sDepthBlurDrawOffset 104A0908
 ext 0274F5F0 void constructDepthDrawViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274D690 void applyDepthDrawSurface(sead::RenderSurface* surface)
 ext 0274F964 void applyDepthDrawViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274FDBC void constructDepthDrawState(sead::CompactRenderState* state)
 ext 0274FEB0 void applyDepthDrawState(sead::CompactRenderState* state)
 ext 027A919C u32 drawDepthBlurAdd(void* renderer, sead::Viewport* viewport, sead::RenderVector2* scale, sead::RenderVector2* offset, u32 mode, f32 strength)
*/
