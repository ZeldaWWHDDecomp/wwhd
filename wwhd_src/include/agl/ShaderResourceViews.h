/* Local shader binary string and block-stream views from assigned accesses. */
#pragma once
#include "wwhd.h"
namespace agl {
struct ShaderBinaryHeader_l {
    be<u32> tag, sectionOffset, stringCount;
};
struct ShaderBinaryCursor {
    gptr<ShaderBinaryHeader_l> binary;
    const char* stringAfterTable() const;
    const char* stringAt(u32 index) const;
    void* blockAt(s32 index) const;
    void* blockAfterThree() const;
};
struct ShaderRelativeBase {
    static void* add(void* base, u32 offset);
};
struct ShaderFlags {
    be<u8> flags;
    void clearDirty();
};
WWHD_SIZE(ShaderBinaryHeader_l, 12);
WWHD_OFFSET(ShaderBinaryHeader_l, sectionOffset, 4);
WWHD_OFFSET(ShaderBinaryHeader_l, stringCount, 8);
WWHD_SIZE(ShaderBinaryCursor, 4);
WWHD_OFFSET(ShaderBinaryCursor, binary, 0);
WWHD_SIZE(ShaderFlags, 1);
}
