/* J2DPane API used by the file-error helpers. GameCube names, HD layout.
 * HD: bounds start at 0x08 (GC 0x0C), and the vtable is at 0xC8.
 * Unaccessed base members remain padding. */
#pragma once
#include "bindings.h"

class JSURandomInputStream;
class J2DPane {
public:
    struct J2DScrnBlockHeader {
        be<u32> mMagic;
        be<u32> mSize;
    };
    struct Bounds {
        struct Point { be<f32> x, y; } i, f;
    };
    struct VTable {
        /* 0x00 */ u8 _00[0x64];
        /* 0x64 */ be<u32> makeMatrix;
    };
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ Bounds mBounds;
    /* 0x18 */ u8 _18[0xC8 - 0x18];
    /* 0xC8 */ gptr<VTable> __vtbl;


};
WWHD_OFFSET(J2DPane, mBounds, 0x08);
WWHD_OFFSET(J2DPane, __vtbl, 0xC8);
WWHD_SIZE(J2DPane, 0xCC);
WWHD_SIZE(J2DPane::J2DScrnBlockHeader, 8);

WWHD_OFFSET(J2DPane::VTable, makeMatrix, 0x64);
WWHD_SIZE(J2DPane::VTable, 0x68);
