/* Material channel validation and named pane-group binding reconstructed from WWHD. */
#include "nw/lyt/AnimationBinding.h"
#include "nw/lyt/PaneAnimationChannels.h"
#include "nw/lyt/PaneGroups.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
u32 trackBindingAlreadyPresent(BoundAnimation*, void*, AnimationTrack*);
struct MaterialLookupMethods { u8 prefix[0x64]; be<u32> findMaterial; };
struct GroupTrackMember { nw::ut::HeapLink link; gptr<LayoutPane> pane; };

/* 0287174C */
u32 BoundAnimation::appendMaterialBinding(LayoutMaterial* material, AnimationTrack* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 capacity = bindingCapacity;
    u32 used = bindingCount;
    if (used >= capacity) return 0;
    bool valid = true;
    auto* track = reinterpret_cast<AnimationSectionTrack*>(source);
    if (track) {
        u32 sectionCount = track->count;
        for (u32 section = 0; section < sectionCount; ++section) {
            auto* set = layoutAtOffset<AnimationChannelSet>(track, u32(track->offsets[section]));
            u32 remaining = set->count;
            auto* offsets = layoutAtOffset<be<u32>>(set, 8);
            u32 tag = remaining ? u32(set->tag) : 0;
            for (u32 index = 0; index < remaining; ++index) {
                auto* channel = layoutAtOffset<AnimationChannel>(set, u32(offsets[index]));
                if (tag == 0x464C5450) {
                    auto* resources = reinterpret_cast<gptr<TextureResourceView>*>((void*)textures);
                    if (!resources) valid = false;
                    u32 flags = material->featureFlags;
                    if ((flags >> 30) <= u8(channel->index)) valid = false;
                    if (u8(channel->curveKind) != 1) valid = false;
                    if (u8(channel->target) != 0) valid = false;
                    auto* animationResource = static_cast<AnimationBindingResource*>((void*)resource);
                    u32 count = animationResource->textureCount;
                    for (u32 texture = 0; texture < count; ++texture) {
                        TextureResourceView* candidate = resources[texture];
                        if (u32(candidate->descriptorWord) == 0) { valid = false; break; }
                    }
                    continue;
                }
                if (tag == 0x464C4D43) {
                    if (u8(channel->target) >= 8) valid = false;
                } else if (tag == 0x464C5453) {
                    u32 transform = channel->index;
                    if (transform >= 3) valid = false;
                    u32 flags = material->layoutFlags;
                    if (((flags >> 28) & 3) <= transform) valid = false;
                    if (u8(channel->target) >= 5) valid = false;
                } else if (tag == 0x464C494D) {
                    if (!(u32(material->layoutFlags) & (1u << 19))) valid = false;
                    if (u8(channel->target) >= 3) valid = false;
                } else if (tag == 0x464C4143) {
                    if (!(u32(material->layoutFlags) & (1u << 22))) valid = false;
                    if (u8(channel->target) != 0) valid = false;
                } else if (tag == 0x464C4653) {
                    if (!(u32(material->layoutFlags) & (1u << 15))) valid = false;
                    if (u8(channel->target) >= 7) valid = false;
                } else continue;
                if (u8(channel->curveKind) != 2) valid = false;
            }
        }
    }
    if (valid) {
        AnimationBindingRecord* records = bindings;
        bindingCount = used + 1;
        records[used].track = source;
        records[used].target = material;
    }
    return 1;
}

/* 028719BC */
void BoundAnimation::bindGroupTracks(NamedPaneGroup* group) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* animationResource = static_cast<AnimationBindingResource*>((void*)resource);
    u32 limit = animationResource->trackCount;
    u32 offset = animationResource->trackOffset;
    auto* offsets = layoutAtOffset<be<u32>>(animationResource, offset);
    u32 index = 0;
    while (index < limit) {
        auto* track = layoutAtOffset<AnimationTrack>(animationResource, u32(offsets[index]));
        u32 kind = track->targetKind;
        auto* sentinel = &group->members.root;
        nw::ut::HeapLink* entry = sentinel->next;
        bool hadMembers = entry != sentinel;
        if (kind == 0) {
            while (entry != sentinel) {
                auto* member = reinterpret_cast<GroupTrackMember*>(entry);
                LayoutPane* pane = member->pane;
                if (layoutShortNameEqual(reinterpret_cast<const be<u8>*>(track), pane->content()->name) != 0) {
                    pane = member->pane;
                    if (pane) {
                        if (trackBindingAlreadyPresent(this, pane, track) == 0 && appendBinding(pane, track) == 0) return;
                        limit = animationResource->trackCount;
                    }
                    break;
                }
                entry = entry->next;
            }
        } else {
            while (entry != sentinel) {
                auto* member = reinterpret_cast<GroupTrackMember*>(entry);
                LayoutPane* pane = member->pane;
                auto* methods = paneMethods<MaterialLookupMethods>(pane);
                auto* material = gabi::call_ptr<LayoutMaterial*>(methods->findMaterial, pane, track, u32(0));
                if (material) {
                    if (trackBindingAlreadyPresent(this, material, track) == 0 && appendMaterialBinding(material, track) == 0) return;
                    break;
                }
                entry = entry->next;
            }
            if (hadMembers) limit = animationResource->trackCount;
        }
        index = (index + 1) & 0xFFFF;
    }
}

/* WWHD glue
ext 02871708 u32 trackBindingAlreadyPresent(nw::lyt::BoundAnimation*, void*, nw::lyt::AnimationTrack*)
*/
