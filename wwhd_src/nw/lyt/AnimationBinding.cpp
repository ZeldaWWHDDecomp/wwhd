/* Base animation construction and binding dispatch reconstructed from WWHD.
 * The merged AnimatorLink type is reused unchanged; no external reference used. */
#include "nw/lyt/AnimationBinding.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/lyt/ResourceProvider.h"
using namespace nw::lyt;
static u8 sTransformMethods;
static u8 sBoundMethods;

/* 02871340 */
TransformAnimation* TransformAnimation::create(TransformAnimation* animation) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!animation) animation = static_cast<TransformAnimation*>(animationAllocate(sizeof(TransformAnimation)));
    if (animation) {
        animation->link.mpPrevious = nullptr;
        animation->link.mpNext = nullptr;
        animation->resource = nullptr;
        animation->enabled = 1;
        animation->methods = reinterpret_cast<AnimationTransformMethods*>(&sTransformMethods);
        animation->frame = 0.0f;
    }
    return animation;
}

/* 028713A4 */
void TransformAnimation::destroy(u32 flags) {
    if (this && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)animationDelete), this);
}

/* 028713F4 */
BoundAnimation* BoundAnimation::create(BoundAnimation* animation) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!animation) animation = static_cast<BoundAnimation*>(animationAllocate(sizeof(BoundAnimation)));
    if (animation) {
        TransformAnimation::create(animation);
        animation->textures = nullptr;
        animation->methods = reinterpret_cast<AnimationTransformMethods*>(&sBoundMethods);
        animation->bindingCount = 0;
        animation->bindingCapacity = 0;
        animation->bindings = nullptr;
    }
    return animation;
}

/* 0287145C */
void BoundAnimation::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    void* storage = bindings;
    methods = reinterpret_cast<AnimationTransformMethods*>(&sBoundMethods);
    if (storage) layoutReleaseAnimationStorage(storage);
    storage = textures;
    if (storage) layoutReleaseAnimationStorage(storage);
    if (flags & 1) animationDelete(this);
}

/* 028714D0 */
void BoundAnimation::applyBindings() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!(u8)enabled) return;
    u32 remaining = bindingCount;
    for (u32 index = 0; remaining; ++index, --remaining) {
        AnimationBindingRecord* records = bindings;
        AnimationTrack* track = records[index].track;
        u32 kind = track->targetKind;
        AnimationTransformMethods* table = methods;
        void* target = records[index].target;
        u32 callback = kind ? table->applyMaterial : table->applyPane;
        gabi::call_ptr<void>(callback, this, target, track);
    }
}

/* 02871710 */
u32 BoundAnimation::appendBinding(void* target, AnimationTrack* track) {
    u32 count = bindingCount;
    u32 capacity = bindingCapacity;
    if (count >= capacity) return 0;
    AnimationBindingRecord* records = bindings;
    bindingCount = count + 1;
    records[count].target = target;
    records[count].track = track;
    return 1;
}

struct AnimationPrepareMethods { u8 prefix[0x4C]; be<u32> prepare; };
struct AnimationTextureMethods { u8 prefix[0x2C]; be<u32> texture; };

/* 0287157C */
void BoundAnimation::prepareFromResource(AnimationBindingResource* source, ResourceProvider* provider) {
    auto* dispatch = reinterpret_cast<AnimationPrepareMethods*>((AnimationTransformMethods*)methods);
    u32 capacity = source->trackCount;
    gabi::tail_ptr<void>(dispatch->prepare, this, source, provider, capacity);
}

/* 02871590 */
void BoundAnimation::prepareStorage(AnimationBindingResource* source, ResourceProvider* provider, u32 capacity) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    resource = source;
    textures = nullptr;
    u32 textureCount = source->textureCount;
    if (textureCount != 0) {
        auto* storage = static_cast<gptr<void>*>(animationAlignedAllocate(textureCount * 4, 4));
        if (!storage) textures = nullptr;
        else {
            for (u32 index = 0; index < textureCount; ++index) {
                if (gabi::ea(&storage[index]) != 0) storage[index] = nullptr;
            }
            textures = storage;
            for (u32 index = 0; index < u16(source->textureCount); ++index) {
                u32 offset = source->textureNameOffsets[index];
                auto* name = layoutAtOffset<be<u8>>(source->textureNameOffsets, offset);
                auto* dispatch = reinterpret_cast<AnimationTextureMethods*>((ResourceProviderMethods*)provider->methods);
                auto* destinations = static_cast<gptr<void>*>((void*)textures);
                void* texture = gabi::call_ptr<void*>(dispatch->texture, provider, name);
                destinations[index] = texture;
            }
        }
    }
    auto* storage = static_cast<AnimationBindingRecord*>(animationAlignedAllocate(capacity * 8, 4));
    if (storage) {
        for (u32 index = 0; index < capacity; ++index) {
            if (gabi::ea(&storage[index]) != 0) {
                storage[index].track = nullptr;
                storage[index].target = nullptr;
            }
        }
    }
    bindings = storage;
    if (storage) bindingCapacity = capacity;
}

/* WWHD glue
ext 0287F3DC void* animationAlignedAllocate(u32, s32)
ext 0273AD10 void* animationAllocate(u32)
ext 0273AF40 void animationDelete(void*) callback
ext 0287F3FC void layoutReleaseAnimationStorage(void*)
data sTransformMethods 1017FE94 nocheck
data sBoundMethods 1017FF24 nocheck
*/
