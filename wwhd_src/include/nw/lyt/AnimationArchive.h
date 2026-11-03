/* Animation archive index, reconstructed from its block-tag scan. */
#pragma once
#include "nw/lyt/ResourceHelpers.h"
namespace nw { namespace lyt {
struct AnimationTagBlock { u8 prefix[0xA]; be<u16> groupCount; u8 reserved[4]; be<u32> nameOffset; };
struct AnimationArchive {
    gptr<FontFile> file;
    gptr<FontBlock> animation;
    gptr<AnimationTagBlock> tag;
    gptr<FontBlock> shared;
    void clear();
    void attach(FontFile* resource);
    u32 groupCount();
    const be<u8>* name();
};
WWHD_SIZE(AnimationArchive, 0x10);
WWHD_OFFSET(AnimationArchive, tag, 8);
WWHD_OFFSET(AnimationArchive, shared, 0xC);
WWHD_OFFSET(AnimationTagBlock, groupCount, 0xA);
WWHD_OFFSET(AnimationTagBlock, nameOffset, 0x10);
} }
