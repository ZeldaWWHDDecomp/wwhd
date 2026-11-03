// Context layout from its HD constructor; opaque subobjects retain exact extents.
#pragma once
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
namespace j3d {
struct PacketDrawBuffer_l;
struct RenderContext {
    be<u32> mState;
    be<u32> mFlags;
    u8 mRenderState[0x64];
    gptr<void> mTarget;
    gptr<void> mSource;
    gptr<PacketDrawBuffer_l> mDrawBuffers[2];
    be<u32> mEnabled;
    ContextMode mCoreModes[3];
    be<u8> mProjectionDirty;
    u8 mStatusPadding[3];
    u8 mProjection[0xAC];
    gptr<void> mProjectionOwner;
    u8 mViewMatrix[0x40];
    u8 mViewStatePrefix[0xC];
    be<u32> mViewEnabled;
    u8 mViewState[0xDC];
    be<u32> mRenderMode;
    u8 mTrailingState[0x2C];
    static RenderContext* create(RenderContext*);
    static void enableGlobal();
    ContextMode* currentCoreMode();
};
void initializeRenderContextRuntime();
}
WWHD_OFFSET(j3d::RenderContext,mTarget,0x6C);
WWHD_OFFSET(j3d::RenderContext,mSource,0x70);
WWHD_OFFSET(j3d::RenderContext,mDrawBuffers,0x74);
WWHD_OFFSET(j3d::RenderContext,mEnabled,0x7C);
WWHD_OFFSET(j3d::RenderContext,mCoreModes,0x80);
WWHD_OFFSET(j3d::RenderContext,mProjectionDirty,0x98);
WWHD_OFFSET(j3d::RenderContext,mProjection,0x9C);
WWHD_OFFSET(j3d::RenderContext,mProjectionOwner,0x148);
WWHD_OFFSET(j3d::RenderContext,mViewMatrix,0x14C);
WWHD_OFFSET(j3d::RenderContext,mViewEnabled,0x198);
WWHD_OFFSET(j3d::RenderContext,mRenderMode,0x278);
WWHD_SIZE(j3d::RenderContext,0x2A8);
