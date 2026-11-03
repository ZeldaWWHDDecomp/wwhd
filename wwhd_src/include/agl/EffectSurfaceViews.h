// AGL#12's effect pass context: the working textures, sampler, render buffer and colour target that the
// render entries' apply routines (027AC390 ...) draw through. Its members are the canonical agl texture and
// render-target types (offsets asserted below and in their own headers).
#pragma once
#include "wwhd.h"
#include "agl/TextureData.h"
#include "agl/TextureSampler.h"
#include "agl/RenderBuffer.h"
#include "agl/RenderSurface.h"
namespace agl {
// Texture pool at +0x330 (0279ADD4 acquires a TextureData from it, 0279AE68 releases one); opaque here.
struct EffectTexturePool_l { u8 storage[0x1C]; };
struct EffectContext_l {
    gptr<TextureData> source;
    gptr<TextureData> working;
    be<u32> format;
    TextureSampler sampler;
    RenderBuffer frame;
    ColorSurface color;
    be<u8> mappingChanged; u8 _32D[3];
    EffectTexturePool_l pool;
};
}
WWHD_OFFSET(agl::EffectContext_l,format,8);
WWHD_OFFSET(agl::EffectContext_l,sampler,0xC);
WWHD_OFFSET(agl::EffectContext_l,frame,0x1A4);
WWHD_OFFSET(agl::EffectContext_l,color,0x1E4);
WWHD_OFFSET(agl::EffectContext_l,mappingChanged,0x32C);
WWHD_OFFSET(agl::EffectContext_l,pool,0x330);
// View extent: pool storage size is the recorded 0279A95C constructor extent.
WWHD_SIZE(agl::EffectContext_l,0x34C);
namespace agl {
// A colour/depth surface's SurfaceData_l prefix is the surface's TextureData (same 0x90-byte layout, two
// declarations pending one fold); the effect passes hand it to TextureSampler::copyTexture (027BDEB4).
static_assert(sizeof(SurfaceData_l) == sizeof(TextureData), "surface texture prefix");
inline const TextureData* surfaceTexture(const SurfaceData_l* surface) {
    return reinterpret_cast<const TextureData*>(surface);
}
}
