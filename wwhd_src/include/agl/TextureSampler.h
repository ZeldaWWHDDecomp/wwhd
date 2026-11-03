/* Exact native sampler extent and accessed prefixes from assigned WWHD instructions. */
#pragma once
#include "agl/TextureData.h"
#include "agl/TextureSamplerSettings.h"
namespace agl {
struct NativeSampler_l {
    be<u32> registers[3];
};
struct TextureSampler {
    TextureData texture;
    be<u8> customMapping, overrideEnabled;
    u8 reserved[2];
    be<u32> componentMapping[4];
    NativeSampler_l nativeSampler;
    u8 nativeTexture[0xA0];
    TextureSamplerSettings settings;
    be<u8> dirty;
    u8 padding[3];
    gptr<const void> methods;
    static TextureSampler* create(TextureSampler*);
    void copyTexture(const TextureData* source);
    void updateClamping();
    void updateFilters();
    void updateLod();
    void updateCompare();
    static void destroy(TextureSampler*, u32 flags);
};
WWHD_SIZE(NativeSampler_l, 12);
WWHD_SIZE(TextureSampler, 0x198);
WWHD_OFFSET(TextureSampler, texture, 0);
WWHD_OFFSET(TextureSampler, customMapping, 0x90);
WWHD_OFFSET(TextureSampler, overrideEnabled, 0x91);
WWHD_OFFSET(TextureSampler, componentMapping, 0x94);
WWHD_OFFSET(TextureSampler, nativeSampler, 0xA4);
WWHD_OFFSET(TextureSampler, nativeTexture, 0xB0);
WWHD_OFFSET(TextureSampler, settings, 0x150);
WWHD_OFFSET(TextureSampler, dirty, 0x190);
WWHD_OFFSET(TextureSampler, methods, 0x194);
}
