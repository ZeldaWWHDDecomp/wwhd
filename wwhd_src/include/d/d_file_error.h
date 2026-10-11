/* File-error MyScreen/MyPicture helpers: HD layout, GameCube names.
 * HD: the J2DPane vtable is at 0xC8; MyPicture's tail remains at 0x124. */
#pragma once
#include "gc_api/JSystem/J2DScreen.h"
#include "gc_api/JSystem/J2DPicture.h"

class MyPicture : public J2DPicture {
public:
    enum : u32 { __vt = 0x10057C88 }; // HD: this class's vtable
    /* 0x124 */ be<f32> m124;
    /* 0x128 */ be<f32> m128;
    /* 0x12C */ be<f32> m12C;
    /* 0x130 */ be<f32> m130;
    /* 0x134 */ be<u8> m134;
    /* 0x135 */ u8 _135[3];
};
WWHD_OFFSET(MyPicture, m134, 0x134);
WWHD_SIZE(MyPicture, 0x138);

class MyScreen : public J2DScreen {
public:
    void calcMtx();
    J2DPane* createPane(const J2DScrnBlockHeader*, JSURandomInputStream*, J2DPane*);
};
