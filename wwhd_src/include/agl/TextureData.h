/* Local texture data layout established by the assigned WWHD constructor and accesses. */
#pragma once
#include "wwhd.h"
#include "sead/GraphicsSurface.h"
namespace agl {
struct TextureData {
    be<u32> format;
    sead::GraphicsSurface surface;
    be<u8> minimumWidth, minimumHeight, minimumDepth, maximumMipCount;
    be<u32> componentMapping[4];
    gptr<const void> methods;
    static TextureData* create(TextureData*);
    void updateSurface();
    void configure(
        u32 dimension, u32 format, u32 width, u32 height, u32 depth, s32 mipCount, u32 tileMode, u32 antiAlias);
    void copySurface(const sead::GraphicsSurface* source);
    void setMipCount(s32 count);
    void invalidate() const;
    void setDimensions(u32 width, u32 height, u32 depth);
    static void destroy(TextureData*, u32 flags);
};
WWHD_SIZE(TextureData, 0x90);
WWHD_OFFSET(TextureData, surface, 4);
WWHD_OFFSET(TextureData, minimumWidth, 0x78);
WWHD_OFFSET(TextureData, minimumHeight, 0x79);
WWHD_OFFSET(TextureData, minimumDepth, 0x7A);
WWHD_OFFSET(TextureData, maximumMipCount, 0x7B);
WWHD_OFFSET(TextureData, componentMapping, 0x7C);
WWHD_OFFSET(TextureData, methods, 0x8C);
}
