// WWHD viewport layouts reconstructed from local game instructions.
#pragma once
#include "sead/RenderMath.h"
namespace sead {
struct RenderSurface {
    be<f32> mWidth, mHeight, mLeft, mBottom, mRight, mTop;
};
struct Viewport {
    be<f32> mLeft, mBottom, mRight, mTop;
    be<u32> mOrientation;
    gptr<void> mTable;
    static Viewport* createEmpty(Viewport* object);
    static Viewport* createRect(Viewport* object, f32 x, f32 y, f32 width, f32 height);
    static Viewport* createSurface(Viewport* object, const RenderVector2* size);
    void setSize(const RenderVector2* size);
    void mapPosition(RenderVector2* output, const RenderSurface* surface) const;
    void mapExtent(RenderVector2* output, const RenderSurface* surface) const;
    void apply(const RenderSurface* surface) const;
    void applyViewport(const RenderSurface* surface) const;
    void applyScissor(const RenderSurface* surface) const;
    void scaledHalfExtent(RenderVector2* output, const RenderVector2* scale) const;
};
}
WWHD_OFFSET(sead::Viewport,mLeft,0);
WWHD_OFFSET(sead::Viewport,mBottom,4);
WWHD_OFFSET(sead::Viewport,mRight,8);
WWHD_OFFSET(sead::Viewport,mTop,12);
WWHD_OFFSET(sead::Viewport,mOrientation,16);
WWHD_OFFSET(sead::Viewport,mTable,20);
WWHD_SIZE(sead::Viewport,24);
WWHD_OFFSET(sead::RenderSurface,mWidth,0);
WWHD_OFFSET(sead::RenderSurface,mTop,20);
WWHD_SIZE(sead::RenderSurface,24);
