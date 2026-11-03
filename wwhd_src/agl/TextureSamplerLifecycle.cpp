/* Sampler lifecycle from assigned WWHD instructions. */
#include "agl/TextureSampler.h"
using namespace agl;
struct SamplerTexture_l {
    sead::GraphicsSurface surface;
    be<u32> firstMip, mipCount, firstSlice, sliceCount, componentMap, registers[5];
    be<u8> customBorder;
    u8 padding[3];
};
struct SamplerBorder_l {
    be<f32> red, green, blue, alpha;
};
struct SamplerLocations_l {
    u8 prefix[8];
    be<s16> vertex, pixel, geometry;
    u8 tail[2];
};
struct DefaultSamplerTextureOwner_l {
    u8 prefix[0x1C];
    gptr<TextureData> texture;
};
struct DefaultSamplerTextureInstance_l {
    gptr<DefaultSamplerTextureOwner_l> owner;
};
static DefaultSamplerTextureInstance_l sDefaultSamplerTexture;
static u8 sSamplerMethods;
static SamplerBorder_l sBorderPresetOne;
static SamplerBorder_l sBorderPresetTwo;
struct SamplerZero_l {
    be<f32> value;
};
static SamplerZero_l sSamplerZero;
WWHD_SIZE(SamplerTexture_l, 0xA0);
WWHD_OFFSET(SamplerTexture_l, firstMip, 0x74);
WWHD_OFFSET(SamplerTexture_l, componentMap, 0x84);
WWHD_OFFSET(SamplerTexture_l, customBorder, 0x9C);
WWHD_SIZE(SamplerBorder_l, 16);
WWHD_SIZE(SamplerLocations_l, 16);
WWHD_OFFSET(SamplerLocations_l, vertex, 8);
WWHD_OFFSET(DefaultSamplerTextureOwner_l, texture, 0x1C);
static void* allocateSampler(u32);
static void initializeSampler(NativeSampler_l*, u32, u32);
static void initializeSamplerTexture(SamplerTexture_l*);
static bool equalSamplerBorder(const SamplerBorder_l*, const SamplerBorder_l*);
static void setSamplerBorderMode(NativeSampler_l*, u32);
static void setVertexBorder(s32, f32, f32, f32, f32);
static void setPixelBorder(s32, f32, f32, f32, f32);
static void setGeometryBorder(s32, f32, f32, f32, f32);
static void setVertexTexture(SamplerTexture_l*, s32);
static void setPixelTexture(SamplerTexture_l*, s32);
static void setGeometryTexture(SamplerTexture_l*, s32);
static void setVertexSampler(NativeSampler_l*, s32);
static void setPixelSampler(NativeSampler_l*, s32);
static void setGeometrySampler(NativeSampler_l*, s32);
static void updateSamplerMapping(TextureSampler*);
static void updateSamplerBorder(TextureSampler*);
static void updateSamplerState(TextureSampler*);

static SamplerTexture_l* samplerTexture(TextureSampler* s);
static SamplerBorder_l* samplerBorder(TextureSampler* s);
static bool matchingSamplerSurface(const sead::GraphicsSurface& a, const sead::GraphicsSurface& b);
static TextureSampler* initializeSamplerObject(TextureSampler* s);
static void initializeSamplerSource(TextureSampler* s, const TextureData* source);
/* 027BDF7C */
TextureSampler* TextureSampler::create(TextureSampler* s) {
    gabi::AutoFrame frame{gabi::current_function()};
    s = initializeSamplerObject(s);
    if (s) initializeSamplerSource(s, sDefaultSamplerTexture.owner->texture);
    return s;
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static SamplerTexture_l* samplerTexture(TextureSampler* s) {
    return reinterpret_cast<SamplerTexture_l*>(s->nativeTexture);
}
static SamplerBorder_l* samplerBorder(TextureSampler* s) {
    return reinterpret_cast<SamplerBorder_l*>(&s->settings.scalarParameters[0]);
}
static bool matchingSamplerSurface(const sead::GraphicsSurface& a, const sead::GraphicsSurface& b) {
    return static_cast<u32>(a.mDimension) == static_cast<u32>(b.mDimension) &&
        static_cast<u32>(a.mWidth) == static_cast<u32>(b.mWidth) &&
        static_cast<u32>(a.mHeight) == static_cast<u32>(b.mHeight) &&
        static_cast<u32>(a.mDepth) == static_cast<u32>(b.mDepth) &&
        static_cast<u32>(a.mMipCount) == static_cast<u32>(b.mMipCount) &&
        static_cast<u32>(a.mFormat) == static_cast<u32>(b.mFormat) &&
        static_cast<u32>(a.mSwizzle) == static_cast<u32>(b.mSwizzle) &&
        static_cast<u32>(a.mTileMode) == static_cast<u32>(b.mTileMode) &&
        static_cast<u32>(a.mSamples) == static_cast<u32>(b.mSamples);
}
static TextureSampler* initializeSamplerObject(TextureSampler* s) {
    if (!s) s = static_cast<TextureSampler*>(allocateSampler(0x198));
    if (!s) return nullptr;
    s->methods = &sSamplerMethods;
    TextureData::create(&s->texture);
    s->overrideEnabled = 0;
    s->customMapping = 0;
    s->componentMapping[1] = 1;
    s->componentMapping[3] = 3;
    s->componentMapping[2] = 2;
    s->componentMapping[0] = 0;
    TextureSamplerSettings::create(&s->settings);
    s->dirty = 0;
    return s;
}
static void initializeSamplerSource(TextureSampler* s, const TextureData* source) {
    if (!matchingSamplerSurface(s->texture.surface, source->surface))
        s->copyTexture(source);
    else {
        void* image = source->surface.mImage;
        void* mip = source->surface.mMipImage;
        s->texture.surface.mImage = image;
        s->texture.surface.mMipImage = mip;
        samplerTexture(s)->surface.mMipImage = mip;
        samplerTexture(s)->surface.mImage = image;
    }
    initializeSampler(&s->nativeSampler, 2, 1);
    s->dirty = 0xFF;
}

/* 027BE114 */
static TextureSampler* createSamplerWithTexture(TextureSampler* s, const TextureData* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    s = initializeSamplerObject(s);
    if (s) initializeSamplerSource(s, source);
    return s;
}
/* 027BE2C4 */
static void updateSamplerMapping(TextureSampler* s) {
    gabi::AutoFrame frame{gabi::current_function()};
    const be<u32>* m = static_cast<u8>(s->customMapping) ? s->componentMapping : s->texture.componentMapping;
    u32 packed = ((static_cast<u32>(m[0]) & 255) << 24) | ((static_cast<u32>(m[1]) & 255) << 16) |
        ((static_cast<u32>(m[2]) & 255) << 8) | (static_cast<u32>(m[3]) & 255);
    samplerTexture(s)->componentMap = packed;
    initializeSamplerTexture(samplerTexture(s));
}
/* 027BE3B8 */
static void updateSamplerBorder(TextureSampler* s) {
    gabi::AutoFrame frame{gabi::current_function()};
    SamplerBorder_l* border = samplerBorder(s);
    if (equalSamplerBorder(border, &sBorderPresetOne)) {
        setSamplerBorderMode(&s->nativeSampler, 1);
        samplerTexture(s)->customBorder = 0;
        return;
    }
    if (equalSamplerBorder(border, &sBorderPresetTwo)) {
        setSamplerBorderMode(&s->nativeSampler, 2);
        samplerTexture(s)->customBorder = 0;
        return;
    }
    gabi::FrameLocal<SamplerBorder_l> zero(8);
    zero->blue = sSamplerZero.value;
    zero->red = sSamplerZero.value;
    zero->alpha = sSamplerZero.value;
    zero->green = sSamplerZero.value;
    if (equalSamplerBorder(border, zero.get())) {
        setSamplerBorderMode(&s->nativeSampler, 0);
        samplerTexture(s)->customBorder = 0;
    } else {
        setSamplerBorderMode(&s->nativeSampler, 3);
        samplerTexture(s)->customBorder = 1;
    }
}
/* 027BE494 */
static void updateSamplerState(TextureSampler* s) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (static_cast<u8>(s->dirty) & 1) updateSamplerMapping(s);
    if (static_cast<u8>(s->dirty) & 2) s->updateClamping();
    if (static_cast<u8>(s->dirty) & 4) s->updateFilters();
    if (static_cast<u8>(s->dirty) & 8) s->updateLod();
    if (static_cast<u8>(s->dirty) & 16) s->updateCompare();
    if (static_cast<u8>(s->dirty) & 32) updateSamplerBorder(s);
    s->dirty = 0;
}
/* 027BE53C */
static bool bindTextureSampler(TextureSampler* s, const SamplerLocations_l* locations) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 vertex = locations->vertex;
    if (vertex == -1 && static_cast<s16>(locations->pixel) == -1 && static_cast<s16>(locations->geometry) == -1)
        return false;
    if (static_cast<u8>(s->dirty)) {
        updateSamplerState(s);
        vertex = locations->vertex;
    }
    s32 pixel = locations->pixel, geometry = locations->geometry;
    s32 bindings[3] = {vertex, pixel, geometry};
    for (u32 stage = 0; stage < 3; ++stage) {
        s32 binding = bindings[stage];
        if (binding == -1) continue;
        if (static_cast<u8>(samplerTexture(s)->customBorder)) {
            SamplerBorder_l* b = samplerBorder(s);
            if (stage == 0)
                setVertexBorder(binding, b->red, b->green, b->blue, b->alpha);
            else if (stage == 1)
                setPixelBorder(binding, b->red, b->green, b->blue, b->alpha);
            else
                setGeometryBorder(binding, b->red, b->green, b->blue, b->alpha);
        }
        if (stage == 0) {
            setVertexTexture(samplerTexture(s), binding);
            setVertexSampler(&s->nativeSampler, binding);
        } else if (stage == 1) {
            setPixelTexture(samplerTexture(s), binding);
            setPixelSampler(&s->nativeSampler, binding);
        } else {
            setGeometryTexture(samplerTexture(s), binding);
            setGeometrySampler(&s->nativeSampler, binding);
        }
    }
    return true;
}
/* WWHD glue
data sDefaultSamplerTexture 101F8DEC
data sSamplerMethods 10169D2C
data sBorderPresetOne 104A01CC
data sBorderPresetTwo 104A01EC
data sSamplerZero 10169D20
ext 0273AD10 void* allocateSampler(u32)
ext C0006438 void initializeSampler(NativeSampler_l*,u32,u32)
ext C0006498 void initializeSamplerTexture(SamplerTexture_l*)
ext 0274D4C0 bool equalSamplerBorder(const SamplerBorder_l*,const SamplerBorder_l*)
ext C0006440 void setSamplerBorderMode(NativeSampler_l*,u32)
ext C0006A20 void setVertexBorder(s32,f32,f32,f32,f32)
ext C00068E8 void setPixelBorder(s32,f32,f32,f32,f32)
ext C0006880 void setGeometryBorder(s32,f32,f32,f32,f32)
ext C0006A30 void setVertexTexture(SamplerTexture_l*,s32)
ext C00068F8 void setPixelTexture(SamplerTexture_l*,s32)
ext C00068A0 void setGeometryTexture(SamplerTexture_l*,s32)
ext C0006A18 void setVertexSampler(NativeSampler_l*,s32)
ext C00068E0 void setPixelSampler(NativeSampler_l*,s32)
ext C0006878 void setGeometrySampler(NativeSampler_l*,s32)
*/
