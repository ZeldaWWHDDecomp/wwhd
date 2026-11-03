/* Texture descriptor setup and Cafe sampler calls reconstructed from WWHD.
 * SDK argument facts come from the project's local import signatures; no external code used. */
#include "nw/lyt/TextureBinding.h"
using namespace nw::lyt;
struct SamplerModes { be<u32> values[3]; };
static SamplerModes sClampingModes;
static SamplerModes sFilteringModes;

/* 02875380 */
void TextureBinding::initializeSampler() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    textureSamplerInitialize(&sampler, 2, 1);
    textureSamplerLod(&sampler, 0.0f, 15.9f, 0.0f);
    textureSamplerZFilter(&sampler, 0, 0);
}

/* 028753E4 */
TextureBinding* TextureBinding::create(TextureBinding* binding) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!binding) binding = static_cast<TextureBinding*>(textureBindingAllocate(sizeof(TextureBinding)));
    if (binding) {
        binding->height = 0;
        binding->width = 0;
        binding->samplingModes = 0x319;
        binding->surface = nullptr;
        binding->descriptorWord = 0;
        binding->initializeSampler();
    }
    return binding;
}

/* 02875448 */
void TextureBinding::assignResource(TextureResourceView* resource) {
    if (!resource) return;
    descriptorWord = resource->descriptorWord;
    width = resource->width;
    height = resource->height;
    u32 type = resource->format;
    surface = &resource->surface;
    format = type;
}

/* 0287547C */
void TextureBinding::setClamping(u32 horizontal, u32 vertical) {
    u32 flags = samplingFlags;
    u32 modes = (horizontal << 2) | (vertical & 3);
    samplingFlags = (flags & 0x0FFFFFFF) | ((modes & 15) << 28);
    u32 horizontalMode = sClampingModes.values[horizontal];
    u32 verticalMode = sClampingModes.values[vertical];
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)textureSamplerClamping), &sampler, horizontalMode, verticalMode, 2);
}

/* 028754B4 */
void TextureBinding::setFiltering(u32 magnification, u32 minification) {
    u32 flags = samplingFlags;
    u32 modes = (magnification << 1) | (minification & 1);
    samplingFlags = (flags & 0xF0FFFFFF) | ((modes & 15) << 24);
    u32 minificationMode = sFilteringModes.values[minification];
    u32 magnificationMode = sFilteringModes.values[magnification];
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)textureSamplerFiltering), &sampler, minificationMode, magnificationMode, 0);
}

/* 028754EC */
void TextureBinding::exportResource(TextureResourceView* output) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<be<u32>> dimensions(8);
    u32 h = height;
    u32 w = width;
    u32 descriptor = descriptorWord;
    u32 type = format;
    output->descriptorWord = descriptor;
    *dimensions = (w << 16) | h;
    u32 packed = *dimensions;
    u32 outputWidth = packed >> 16;
    u32 outputHeight = packed & 0xFFFF;
    output->width = outputWidth;
    output->format = type;
    output->height = outputHeight;
    TextureObject* image = surface;
    auto* source = reinterpret_cast<const be<u32>*>(image);
    auto* destination = reinterpret_cast<be<u32>*>(&output->surface);
    for (u32 index = 0; index < 39; ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
}

/* WWHD glue
ext 0273AD10 void* textureBindingAllocate(u32)
ext C0006438 void textureSamplerInitialize(nw::lyt::LayoutSampler*, u32, u32)
ext C0006460 void textureSamplerLod(nw::lyt::LayoutSampler*, f32, f32, f32)
ext C0006478 void textureSamplerZFilter(nw::lyt::LayoutSampler*, u32, u32)
ext C0006448 void textureSamplerClamping(nw::lyt::LayoutSampler*, u32, u32, u32) callback
ext C0006470 void textureSamplerFiltering(nw::lyt::LayoutSampler*, u32, u32, u32) callback
data sClampingModes 101804D0 nocheck
data sFilteringModes 101804DC nocheck
*/
