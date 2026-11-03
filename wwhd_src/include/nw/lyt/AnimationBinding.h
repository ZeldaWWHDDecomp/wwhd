/* Base animation binding views reconstructed from their constructors and dispatches. */
#pragma once
#include "nw/lyt/FontResource.h"
#include "nw/lyt/LYT_animframe.h"
namespace nw { namespace lyt {
struct AnimationTransformMethods {
    u8 prefix[0x94];
    be<u32> applyPane;
    u8 panePadding[4];
    be<u32> applyMaterial;
};
struct AnimationTrack { u8 prefix[0x1D]; be<u8> targetKind; };
struct AnimationBindingRecord { gptr<void> target; gptr<AnimationTrack> track; };
struct TransformAnimation {
    AnimatorLink link;
    gptr<void> resource;
    be<f32> frame;
    be<u8> enabled;
    u8 statePadding[3];
    gptr<AnimationTransformMethods> methods;
    static TransformAnimation* create(TransformAnimation* animation);
    void destroy(u32 flags);
};
struct ResourceProvider;
struct LayoutPane;
struct LayoutMaterial;
struct NamedPaneGroup;
struct AnimationBindingResource {
    u8 prefix[0xC];
    be<u16> textureCount, trackCount;
    be<u32> trackOffset, textureNameOffsets[1];
};
struct BoundAnimation : TransformAnimation {
    gptr<void> textures;
    gptr<AnimationBindingRecord> bindings;
    be<u16> bindingCount, bindingCapacity;
    static BoundAnimation* create(BoundAnimation* animation);
    void destroy(u32 flags);
    void applyBindings();
    void applyPane(LayoutPane* pane, AnimationTrack* track);
    void applyMaterial(LayoutMaterial* material, AnimationTrack* track);
    void prepareFromResource(AnimationBindingResource* resource, ResourceProvider* provider);
    void prepareStorage(AnimationBindingResource* resource, ResourceProvider* provider, u32 capacity);
    u32 appendBinding(void* target, AnimationTrack* track);
    u32 appendMaterialBinding(LayoutMaterial* material, AnimationTrack* track);
    void bindGroupTracks(NamedPaneGroup* group);
};
WWHD_SIZE(TransformAnimation, 0x18);
WWHD_OFFSET(TransformAnimation, resource, 8);
WWHD_OFFSET(TransformAnimation, enabled, 0x10);
WWHD_OFFSET(TransformAnimation, methods, 0x14);
WWHD_SIZE(BoundAnimation, 0x24);
WWHD_OFFSET(BoundAnimation, textures, 0x18);
WWHD_OFFSET(BoundAnimation, bindings, 0x1C);
WWHD_OFFSET(BoundAnimation, bindingCount, 0x20);
WWHD_SIZE(AnimationBindingRecord, 8);
WWHD_OFFSET(AnimationTrack, targetKind, 0x1D);
WWHD_OFFSET(AnimationTransformMethods, applyPane, 0x94);
WWHD_OFFSET(AnimationTransformMethods, applyMaterial, 0x9C);
} }
void layoutReleaseAnimationStorage(void* storage);
void* animationAllocate(u32 size);
void animationDelete(void* storage);
void* animationAlignedAllocate(u32 size, s32 alignment);
