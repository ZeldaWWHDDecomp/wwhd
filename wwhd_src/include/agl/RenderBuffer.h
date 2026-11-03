/* WWHD render buffer extends the canonical logical frame-buffer prefix. */
#pragma once
#include "sead/CafeGameFramework.h"
#include "agl/RenderSurface.h"
#include "sead/DisplayTargets.h"
namespace agl {
struct RenderBuffer {
    sead::VirtualFrameBuffer mBase;
    gptr<ColorSurface> mColors[8];
    gptr<DepthSurface> mDepth;
    RenderBuffer* construct();
    void clearColors();
    void clear();
    void destroy(u32 flags);
    void resolveColor(sead::ScanTarget* target);
};
struct RenderBufferRegistry_l {
    be<s32> mCount;
    gptr<gptr<RenderBuffer>> mEntries;
};
WWHD_OFFSET(RenderBuffer,mBase,0);
WWHD_OFFSET(RenderBuffer,mColors,0x1C);
WWHD_OFFSET(RenderBuffer,mDepth,0x3C);
WWHD_SIZE(RenderBuffer,0x40);
WWHD_OFFSET(RenderBufferRegistry_l,mEntries,4);
WWHD_SIZE(RenderBufferRegistry_l,8);
}
