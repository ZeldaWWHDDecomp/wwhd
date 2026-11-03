/* Partial picture view, from the picture constructor's four color objects at +0xA8. */
#pragma once
#include "nw/lyt/GlyphWriter.h"
namespace nw { namespace lyt {
struct PictureColors {
    u8 baseState[0xA4];
    gptr<void> material;
    ColorWord colors[4];
    void setColorByte(u32 index, u32 value);
    bool hasMaterial() const;
    void* materialAt(s32 index) const;
};
WWHD_OFFSET(PictureColors, material, 0xA4);
WWHD_OFFSET(PictureColors, colors, 0xA8);
WWHD_SIZE(PictureColors, 0xB8);
} }
