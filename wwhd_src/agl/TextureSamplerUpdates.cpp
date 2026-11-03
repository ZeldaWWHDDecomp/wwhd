/* Sampler SDK forwarding and sequential texture copy from assigned WWHD code. */
#include "agl/TextureSampler.h"
using namespace agl;
static void* copySamplerSurface(void*, const void*, u32, u32);
static void setSamplerClamping(NativeSampler_l*, u32, u32, u32);
static void setSamplerXYFilter(NativeSampler_l*, u32, u32, u32);
static void setSamplerZMFilter(NativeSampler_l*, u32, u32);
static void setSamplerLod(NativeSampler_l*, f32, f32, f32);
static void setSamplerCompare(NativeSampler_l*, u32);
static void releaseTextureSampler(void*);
/* 027BDEB4 */
void TextureSampler::copyTexture(const TextureData* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    const be<u32>* input = gabi::at<const be<u32>>(gabi::ea(source));
    be<u32>* output = gabi::at<be<u32>>(gabi::ea(this));
    for (u32 i = 0; i < 30; ++i)
        output[i] = input[i];
    texture.minimumWidth = source->minimumWidth;
    texture.minimumHeight = source->minimumHeight;
    texture.minimumDepth = source->minimumDepth;
    texture.maximumMipCount = source->maximumMipCount;
    texture.componentMapping[0] = source->componentMapping[0];
    texture.componentMapping[1] = source->componentMapping[1];
    texture.componentMapping[2] = source->componentMapping[2];
    texture.componentMapping[3] = source->componentMapping[3];
    copySamplerSurface(gabi::at<void>(gabi::ea(this) + 0xB0), &texture.surface, 0x74, 0);
    u32 nativeDepth = *gabi::at<const be<u32>>(gabi::ea(this) + 0xBC);
    u32 nativeMipCount = *gabi::at<const be<u32>>(gabi::ea(this) + 0xC0);
    *gabi::at<be<u32>>(gabi::ea(this) + 0x130) = nativeDepth;
    u32 oldFlags = dirty;
    *gabi::at<be<u32>>(gabi::ea(this) + 0x128) = nativeMipCount;
    *gabi::at<be<u32>>(gabi::ea(this) + 0x124) = 0;
    dirty = oldFlags | 5;
    *gabi::at<be<u32>>(gabi::ea(this) + 0x12C) = 0;
}
/* 027BE2B0 */
void TextureSampler::destroy(TextureSampler* sampler, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sampler && (flags & 1)) releaseTextureSampler(sampler);
}
/* 027BE32C */
void TextureSampler::updateClamping() {
    gabi::AutoFrame frame{gabi::current_function()};
    setSamplerClamping(&nativeSampler, settings.modes[3], settings.modes[4], settings.modes[5]);
}
/* 027BE340 */
void TextureSampler::updateFilters() {
    gabi::AutoFrame frame{gabi::current_function()};
    setSamplerXYFilter(&nativeSampler, settings.modes[0], settings.modes[1], settings.option);
    setSamplerZMFilter(&nativeSampler, static_cast<u32>(texture.surface.mDimension) == 5, settings.modes[2]);
}
/* 027BE398 */
void TextureSampler::updateLod() {
    gabi::AutoFrame frame{gabi::current_function()};
    setSamplerLod(
        &nativeSampler, settings.scalarParameters[4], settings.scalarParameters[5], settings.scalarParameters[6]);
}
/* 027BE3AC */
void TextureSampler::updateCompare() {
    gabi::AutoFrame frame{gabi::current_function()};
    setSamplerCompare(&nativeSampler, settings.tail);
}
/* WWHD glue
ext C0009988 void* copySamplerSurface(void*,const void*,u32,u32)
ext C0006448 void setSamplerClamping(NativeSampler_l*,u32,u32,u32)
ext C0006470 void setSamplerXYFilter(NativeSampler_l*,u32,u32,u32)
ext C0006478 void setSamplerZMFilter(NativeSampler_l*,u32,u32)
ext C0006460 void setSamplerLod(NativeSampler_l*,f32,f32,f32)
ext C0006450 void setSamplerCompare(NativeSampler_l*,u32)
ext 0273AF40 void releaseTextureSampler(void*)
*/
