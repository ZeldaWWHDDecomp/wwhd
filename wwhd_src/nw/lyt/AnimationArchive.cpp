/* FLAN resource block index reconstructed from the original tag scan.
 * Uses canonical file/block layouts; no external format documentation used. */
#include "nw/lyt/AnimationArchive.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
enum AnimationArchiveTag : u32 { AnimationFileTag = 0x464C414E, AnimationInfoTag = 0x70616931, AnimationNameTag = 0x70617431, AnimationSharedTag = 0x70616831 };

/* 028727D0 */
void AnimationArchive::clear() {
    file = nullptr;
    shared = nullptr;
    animation = nullptr;
    tag = nullptr;
}

/* 028727E8 */
void AnimationArchive::attach(FontFile* resource) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    clear();
    if (!layoutValidateResourceHeader(resource, AnimationFileTag, 0x02020000, 1)) return;
    file = resource;
    u32 count = resource->blockCount;
    u32 offset = resource->headerSize;
    FontBlock* block = layoutAtOffset<FontBlock>(resource, offset);
    for (u32 index = 0; index < count;) {
        switch ((u32)block->tag) {
        case AnimationSharedTag: shared = block; break;
        case AnimationInfoTag: animation = block; break;
        case AnimationNameTag: tag = reinterpret_cast<AnimationTagBlock*>(block); break;
        }
        count = resource->blockCount;
        ++index;
        u32 size = block->size;
        block = layoutAtOffset<FontBlock>(block, size);
    }
}

/* 028728EC */
u32 AnimationArchive::groupCount() {
    AnimationTagBlock* block = tag;
    return block ? (u16)block->groupCount : 0;
}

/* 02872904 */
const be<u8>* AnimationArchive::name() {
    AnimationTagBlock* block = tag;
    if (!block) return nullptr;
    return layoutAtOffset<const be<u8>>(block, (u32)block->nameOffset);
}

/* WWHD glue
*/
