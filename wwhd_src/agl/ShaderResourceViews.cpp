/* Archive strings and relative block streams from the assigned WWHD instructions. */
#include "agl/ShaderResourceViews.h"
using namespace agl;
/* 027BA0B8 */
void ShaderFlags::clearDirty() {
    u32 value = flags;
    if (value & 1) flags = value & ~1u;
}
/* 027BC2D0 */
void* ShaderRelativeBase::add(void* base, u32 offset) {
    return gabi::at<void>(gabi::ea(base) + offset);
}
/* 027BC4AC */
const char* ShaderBinaryCursor::stringAfterTable() const {
    const ShaderBinaryHeader_l* header = binary;
    u32 count = header->stringCount;
    u32 cursor = gabi::ea(header) + static_cast<u32>(header->sectionOffset) + 0x10;
    u32 index = 0;
    for (;;) {
        while (*gabi::at<const be<u8>>(cursor) == 0)
            ++cursor;
        if (index == count) return gabi::at<const char>(cursor);
        do {
            ++cursor;
        } while (*gabi::at<const be<u8>>(cursor) != 0);
        ++index;
        ++cursor;
    }
}
/* 027BC4F8 */
const char* ShaderBinaryCursor::stringAt(u32 requested) const {
    const ShaderBinaryHeader_l* header = binary;
    u32 cursor = gabi::ea(header) + static_cast<u32>(header->sectionOffset) + 0x10;
    u32 index = 0;
    for (;;) {
        while (*gabi::at<const be<u8>>(cursor) == 0)
            ++cursor;
        if (index == requested) return gabi::at<const char>(cursor);
        do {
            ++cursor;
        } while (*gabi::at<const be<u8>>(cursor) != 0);
        ++index;
        ++cursor;
    }
}
/* 027BC654 */
void* ShaderBinaryCursor::blockAt(s32 requested) const {
    const ShaderBinaryHeader_l* header = binary;
    u32 cursor = gabi::ea(header) + static_cast<u32>(header->sectionOffset) + 0x14;
    for (s32 index = 0; index < requested; ++index)
        cursor += *gabi::at<const be<u32>>(cursor);
    return gabi::at<void>(cursor);
}
/* 027BC68C */
void* ShaderBinaryCursor::blockAfterThree() const {
    const ShaderBinaryHeader_l* header = binary;
    u32 cursor = gabi::ea(header) + static_cast<u32>(header->sectionOffset) + 0x14;
    for (u32 index = 0; index < 3; ++index)
        cursor += *gabi::at<const be<u32>>(cursor);
    return gabi::at<void>(cursor);
}
/* WWHD glue */
