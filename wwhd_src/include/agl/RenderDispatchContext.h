// Render dispatch context: the 0x24-byte record a layer's render/dispatch call and the render view's frame hooks build
// on the stack and hand to their callbacks (constructors 027D4EA4 and 027D4FE8; layouts from their stores and the
// merged consumers in agl/RenderLayer.cpp).
#pragma once
#include "wwhd.h"
#include "agl/RenderQueue.h"
namespace agl { namespace render {
struct RenderDispatchContext {
    be<s32> mKind;                         // -1 for a layer dispatch, 0 for a direct one (callers may set it after)
    be<u32> mPhase;
    be<u32> mTarget;                       // the caller's second word (a layer render passes its surface)
    be<u32> mLayerIndex;                   // the layer's manager index; ~0 for a direct dispatch
    gptr<agl::RenderLayerPrefix> mLayer;
    gptr<sead::ViewCamera> mCamera;
    gptr<sead::ProjectionState> mProjection;
    gptr<sead::Viewport> mViewport;
    be<u8> mEnabled;
    u8 mPadding[3];
};
WWHD_SIZE(RenderDispatchContext,0x24);
WWHD_OFFSET(RenderDispatchContext,mLayerIndex,0xC);
WWHD_OFFSET(RenderDispatchContext,mLayer,0x10);
WWHD_OFFSET(RenderDispatchContext,mViewport,0x1C);
WWHD_OFFSET(RenderDispatchContext,mEnabled,0x20);
// Layer flag bits (RenderLayerPrefix::mFlags) the layer dispatch reads.
enum : u32 {
    kLayerDrawDisabled = 0x4,
    kLayerWorkspaceCamera = 0x40,
    kLayerCameraEnabled = 0x80,
    kLayerActiveProjection = 0x1000,
};
RenderDispatchContext* createLayerDispatchContext(RenderDispatchContext*,u32,u32,u32,agl::RenderLayerPrefix*);
RenderDispatchContext* createDirectDispatchContext(RenderDispatchContext*,u32,u32);
// The dispatch TU's plain sead text (vtable 1016BFFC): deleting destructor (027D4E8C).
void destroyDispatchText(sead::SafeText*,u32);
} }
