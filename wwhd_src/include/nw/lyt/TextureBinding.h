/* Texture descriptor and sampler views from constructor and copy instructions. */
#pragma once
#include "nw/lyt/FontResource.h"
namespace nw { namespace lyt {
struct LayoutSampler { be<u32> words[3]; };
struct TextureResourceView {
    be<u32> descriptorWord;
    be<u16> width, height;
    be<u8> format;
    u8 formatPadding[3];
    TextureObject surface;
};
struct TextureBinding {
    be<u32> descriptorWord;
    be<u16> width, height;
    union {
        be<u32> samplingFlags;
        struct { be<u16> samplingModes; u8 modePadding[2]; };
        struct { u8 modePrefix; be<u8> format; u8 formatPadding[2]; };
    };
    LayoutSampler sampler;
    gptr<TextureObject> surface;
    static TextureBinding* create(TextureBinding* binding);
    void initializeSampler();
    void assignResource(TextureResourceView* resource);
    void setClamping(u32 horizontal, u32 vertical);
    void setFiltering(u32 magnification, u32 minification);
    void exportResource(TextureResourceView* output);
};
WWHD_SIZE(LayoutSampler, 0xC);
WWHD_SIZE(TextureBinding, 0x1C);
WWHD_OFFSET(TextureBinding, samplingFlags, 8);
WWHD_OFFSET(TextureBinding, format, 9);
WWHD_OFFSET(TextureBinding, sampler, 0xC);
WWHD_OFFSET(TextureBinding, surface, 0x18);
WWHD_OFFSET(TextureResourceView, surface, 0xC);
} }
void* textureBindingAllocate(u32 size);
void textureSamplerInitialize(nw::lyt::LayoutSampler*, u32, u32);
void textureSamplerLod(nw::lyt::LayoutSampler*, f32, f32, f32);
void textureSamplerZFilter(nw::lyt::LayoutSampler*, u32, u32);
void textureSamplerClamping(nw::lyt::LayoutSampler*, u32, u32, u32);
void textureSamplerFiltering(nw::lyt::LayoutSampler*, u32, u32, u32);
