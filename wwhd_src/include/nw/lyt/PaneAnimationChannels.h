/* Relative channel offsets and pane animation target fields from WWHD. */
#pragma once
#include "nw/lyt/AnimationCurves.h"
#include "nw/lyt/PaneState.h"
#include "nw/lyt/LayoutMaterial.h"
#include "nw/lyt/TextureBinding.h"
namespace nw { namespace lyt {
struct AnimationChannelSet { be<u32> tag; be<u8> count; u8 padding[3]; };
struct AnimationChannel {
    be<u8> index, target, curveKind, padding;
    be<u16> keyCount, reserved;
    be<u32> keyOffset;
};
void applyPaneTransformChannels(LayoutPane*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyPaneVisibilityChannels(LayoutPane*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyPaneColorChannels(LayoutPane*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyMaterialColorChannels(ColorWord*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyTextureTransformChannels(LayoutMaterial*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyTexturePatternChannels(LayoutMaterial*, AnimationChannelSet*, const be<u32>*, gptr<TextureResourceView>*, f32 time);
void applyIndirectTransformChannels(LayoutMaterial*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyAlphaCompareChannels(LayoutMaterial*, AnimationChannelSet*, const be<u32>*, f32 time);
void applyShadowColorChannels(LayoutMaterial*, AnimationChannelSet*, const be<u32>*, f32 time);
struct AnimationSectionTrack {
    u8 prefix[0x1C]; be<u8> count, targetKind; u8 padding[2]; be<u32> offsets[1];
};
WWHD_OFFSET(AnimationSectionTrack, offsets, 0x20);
WWHD_SIZE(AnimationChannelSet, 8);
WWHD_SIZE(AnimationChannel, 12);
} }
