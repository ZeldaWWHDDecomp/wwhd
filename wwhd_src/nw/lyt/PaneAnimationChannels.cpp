/* Pane transform and visibility tracks reconstructed from their target stores and call order. */
#include "nw/lyt/PaneAnimationChannels.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/lyt/AnimationBinding.h"
using namespace nw::lyt;
struct PaneTransformProperties { be<f32> values[10]; };

/* 02871CA8 */
void nw::lyt::applyPaneTransformChannels(LayoutPane* pane, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 index = 0;
    while (index < u8(set->count)) {
        u32 offset = offsets[index];
        auto* channel = layoutAtOffset<AnimationChannel>(set, offset);
        u32 keyOffset = channel->keyOffset;
        u32 count = channel->keyCount;
        auto* keys = layoutAtOffset<HermiteKey>(channel, keyOffset);
        u32 target = channel->target;
        f32 value = interpolateHermiteKeys(keys, count, time);
        auto* properties = reinterpret_cast<PaneTransformProperties*>(pane->content()->translation);
        properties->values[target] = value;
        u8 flags = pane->content()->flags;
        pane->content()->flags = flags | 0x10;
        ++index;
    }
}

/* 02871E18 */
void nw::lyt::applyPaneVisibilityChannels(LayoutPane* pane, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 index = 0;
    while (index < u8(set->count)) {
        u8 flags = pane->content()->flags;
        u32 offset = offsets[index];
        auto* channel = layoutAtOffset<AnimationChannel>(set, offset);
        u32 keyOffset = channel->keyOffset;
        u32 count = channel->keyCount;
        auto* keys = layoutAtOffset<DiscreteKey>(channel, keyOffset);
        u32 value = sampleDiscreteKeys(keys, count, time);
        pane->content()->flags = (flags & 0xFE) | u32(value != 0);
        ++index;
    }
}

struct PaneColorElementMethods { u8 prefix[0x34]; be<u32> colorElement; };
struct ColorByteConversion { be<u32> integer; };

static u32 roundedColorByte(f32 value) {
    f32 rounded = gabi::fadds_ppc(value, 0.5f);
    f32 nonnegative = rounded >= 0.0f ? rounded : 0.0f;
    f32 excess = gabi::fsubs_ppc(nonnegative, 255.0f);
    f32 clamped = excess >= 0.0f ? 255.0f : nonnegative;
    return u32(gabi::ftoi(clamped));
}

/* 02871EB0 */
void nw::lyt::applyPaneColorChannels(LayoutPane* pane, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 index = 0;
    while (index < u8(set->count)) {
        u32 offset = offsets[index];
        auto* channel = layoutAtOffset<AnimationChannel>(set, offset);
        u32 keyOffset = channel->keyOffset;
        u32 count = channel->keyCount;
        auto* keys = layoutAtOffset<HermiteKey>(channel, keyOffset);
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 byteValue = roundedColorByte(value);
        auto* dispatch = paneMethods<PaneColorElementMethods>(pane);
        u32 setter = dispatch->colorElement;
        gabi::FrameLocal<ColorByteConversion> conversion(8);
        conversion->integer = byteValue;
        u32 component = u8(u32(conversion->integer));
        u32 target = channel->target;
        gabi::call_ptr<void>(setter, pane, target, component);
        ++index;
    }
}

/* 028720B8 */
void nw::lyt::applyMaterialColorChannels(ColorWord* colors, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 index = 0;
    u32 limit = set->count;
    while (index < limit) {
        u32 offset = offsets[index];
        auto* channel = layoutAtOffset<AnimationChannel>(set, offset);
        u32 keyOffset = channel->keyOffset;
        u32 count = channel->keyCount;
        auto* keys = layoutAtOffset<HermiteKey>(channel, keyOffset);
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 byteValue = roundedColorByte(value);
        u32 target = channel->target;
        gabi::FrameLocal<ColorByteConversion> conversion(8);
        conversion->integer = byteValue;
        u8 component = u8(u32(conversion->integer));
        if (target < 8) {
            auto* bytes = reinterpret_cast<be<u8>*>(colors);
            bytes[target] = component;
            limit = set->count;
        }
        ++index;
    }
}

/* 02871FDC */
void BoundAnimation::applyPane(LayoutPane* pane, AnimationTrack* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* track = reinterpret_cast<AnimationSectionTrack*>(source);
    u32 index = 0;
    u32 limit = track->count;
    while (index < limit) {
        u32 offset = track->offsets[index];
        auto* set = layoutAtOffset<AnimationChannelSet>(track, offset);
        u32 tag = set->tag;
        auto* channels = layoutAtOffset<be<u32>>(set, 8);
        if (tag == 0x464C5041) {
            applyPaneTransformChannels(pane, set, channels, this->frame);
            limit = track->count;
        } else if (tag == 0x464C5649) {
            applyPaneVisibilityChannels(pane, set, channels, this->frame);
            limit = track->count;
        } else if (tag == 0x464C5643) {
            applyPaneColorChannels(pane, set, channels, this->frame);
            limit = track->count;
        }
        ++index;
    }
}

/* Packed material storage order follows the independent count fields in layoutFlags.
 * Texture bindings precede texture transforms, coordinate generators and optional
 * indirect/compare/shadow records. Targets remain unchecked as in the resource ABI. */
static u32 textureTransformOffset(u32 flags) { return (flags >> 30) * 28; }
static u32 coordinateGeneratorOffset(u32 flags) {
    return textureTransformOffset(flags) + ((flags >> 28) & 3) * 20;
}
static u32 indirectTransformOffset(u32 flags) {
    return coordinateGeneratorOffset(flags) + (((flags >> 26) & 3) + ((flags >> 22) & 1)) * 8 + ((flags >> 20) & 3) * 4;
}

/* 0287220C */
void nw::lyt::applyTextureTransformChannels(LayoutMaterial* material, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < u8(set->count); ++index) {
        auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
        u32 transform = channel->index;
        u32 target = channel->target;
        auto* keys = layoutAtOffset<HermiteKey>(channel, u32(channel->keyOffset));
        u32 count = channel->keyCount;
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 flags = material->layoutFlags;
        be<u8>* storage = material->storage;
        *layoutAtOffset<be<f32>>(storage, textureTransformOffset(flags) + transform * 20 + target * 4) = value;
    }
}

/* 028722C0 */
void nw::lyt::applyTexturePatternChannels(LayoutMaterial* material, AnimationChannelSet* set, const be<u32>* offsets, gptr<TextureResourceView>* textures, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 limit = set->count;
    for (u32 index = 0; index < limit; ++index) {
        auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
        auto* keys = layoutAtOffset<DiscreteKey>(channel, u32(channel->keyOffset));
        u32 count = channel->keyCount;
        u32 selected = sampleDiscreteKeys(keys, count, time);
        TextureResourceView* resource = textures[selected];
        if (u32(resource->descriptorWord) != 0) {
            u32 target = channel->index;
            be<u8>* storage = material->storage;
            auto* binding = layoutAtOffset<TextureBinding>(storage, target * 28);
            binding->assignResource(resource);
            limit = set->count;
        }
    }
}

/* 02872374 */
void nw::lyt::applyIndirectTransformChannels(LayoutMaterial* material, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < u8(set->count); ++index) {
        auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
        auto* keys = layoutAtOffset<HermiteKey>(channel, u32(channel->keyOffset));
        u32 target = channel->target;
        u32 count = channel->keyCount;
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 flags = material->layoutFlags;
        be<u8>* storage = material->storage;
        *layoutAtOffset<be<f32>>(storage, indirectTransformOffset(flags) + target * 4) = value;
    }
}

/* 02872448 */
void nw::lyt::applyAlphaCompareChannels(LayoutMaterial* material, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < u8(set->count); ++index) {
        auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
        auto* keys = layoutAtOffset<HermiteKey>(channel, u32(channel->keyOffset));
        u32 count = channel->keyCount;
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 flags = material->layoutFlags;
        f32 nonnegative = value >= 0.0f ? value : 0.0f;
        f32 excess = gabi::fsubs_ppc(nonnegative, 1.0f);
        f32 clamped = excess >= 0.0f ? 1.0f : nonnegative;
        be<u8>* storage = material->storage;
        u32 offset = coordinateGeneratorOffset(flags) + ((flags >> 26) & 3) * 8;
        *layoutAtOffset<be<f32>>(storage, offset + 4) = clamped;
    }
}

/* 02872524 */
void nw::lyt::applyShadowColorChannels(LayoutMaterial* material, AnimationChannelSet* set, const be<u32>* offsets, f32 time) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < u8(set->count); ++index) {
        auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
        auto* keys = layoutAtOffset<HermiteKey>(channel, u32(channel->keyOffset));
        u32 count = channel->keyCount;
        f32 value = interpolateHermiteKeys(keys, count, time);
        u32 flags = material->layoutFlags;
        u32 offset = indirectTransformOffset(flags) + ((flags >> 19) & 1) * 12 + ((flags >> 23) & 7) * 4 + ((flags >> 16) & 7) * 20;
        u32 component = roundedColorByte(value);
        be<u8>* storage = material->storage;
        u32 target = channel->target;
        gabi::FrameLocal<ColorByteConversion> conversion(8);
        conversion->integer = component;
        *layoutAtOffset<be<u8>>(storage, offset + target) = u8(u32(conversion->integer));
    }
}

/* 02872664 */
void BoundAnimation::applyMaterial(LayoutMaterial* material, AnimationTrack* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* track = reinterpret_cast<AnimationSectionTrack*>(source);
    u32 limit = track->count;
    for (u32 index = 0; index < limit; ++index) {
        auto* set = layoutAtOffset<AnimationChannelSet>(track, u32(track->offsets[index]));
        u32 tag = set->tag;
        auto* offsets = layoutAtOffset<be<u32>>(set, 8);
        if (tag == 0x464C4D43) applyMaterialColorChannels(material->colors, set, offsets, this->frame);
        else if (tag == 0x464C5453) applyTextureTransformChannels(material, set, offsets, this->frame);
        else if (tag == 0x464C5450) {
            f32 time = this->frame;
            auto* resources = reinterpret_cast<gptr<TextureResourceView>*>((void*)textures);
            applyTexturePatternChannels(material, set, offsets, resources, time);
        }
        else if (tag == 0x464C494D) applyIndirectTransformChannels(material, set, offsets, this->frame);
        else if (tag == 0x464C4143) applyAlphaCompareChannels(material, set, offsets, this->frame);
        else if (tag == 0x464C4653) applyShadowColorChannels(material, set, offsets, this->frame);
        else continue;
        limit = track->count;
    }
}

/* WWHD glue
*/
