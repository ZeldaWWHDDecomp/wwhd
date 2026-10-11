/* Treasure chest environment texture, private HD views.
 * This separate TU leaves the shared daTbox_c layout unchanged. */
#pragma once
#include "d/actor/d_a_tbox.h"
#include "gc_api/gc_api.h"

struct TboxModelInfo {
    be<s16> modelId, openBckId, btkId, brkId, closedColId, openColId;
};
WWHD_SIZE(TboxModelInfo, 12);

// HD: ResTIMG gains a live graphics texture after the GameCube image header.
struct TboxResTIMG {
    be<u8> format, alphaEnabled;
    be<u16> width, height;
    be<u8> wrapS, wrapT, indexTexture, colorFormat;
    be<u16> numColors;
    be<u32> paletteOffset;
    be<u8> mipmapEnabled, doEdgeLOD, biasClamp, maxAnisotropy;
    be<u8> minFilter, magFilter;
    be<s8> minLOD, maxLOD;
    be<u8> mipmapCount, unknown;
    be<s16> LODBias;
    be<u32> imageOffset;
    gptr<void> mpTexture;
};
WWHD_OFFSET(TboxResTIMG, width, 2);
WWHD_OFFSET(TboxResTIMG, height, 4);
WWHD_OFFSET(TboxResTIMG, indexTexture, 8);
WWHD_OFFSET(TboxResTIMG, paletteOffset, 12);
WWHD_OFFSET(TboxResTIMG, imageOffset, 28);
WWHD_OFFSET(TboxResTIMG, mpTexture, 32);
WWHD_SIZE(TboxResTIMG, 36);
union TboxImageWords {
    TboxResTIMG image;
    be<u32> words[9];
};
WWHD_SIZE(TboxImageWords, 36);
struct TboxName {
    be<u32> text, vtable;
};
WWHD_SIZE(TboxName, 8);
struct TboxTextureFrame {
    u8 linkage[8];
    TboxImageWords image;
    TboxName name;
    u8 tail[0x1C];
};
WWHD_OFFSET(TboxTextureFrame, image, 8);
WWHD_OFFSET(TboxTextureFrame, name, 44);
WWHD_SIZE(TboxTextureFrame, 0x50);
struct TboxTexture {
    be<u16> mNum;
    u8 padding[2];
    gptr<TboxImageWords> mpRes;
    be<u32> mUsed[2][2];
    be<u32> mUseMask[2];
};
WWHD_OFFSET(TboxTexture, mpRes, 4);
WWHD_OFFSET(TboxTexture, mUsed, 8);
WWHD_OFFSET(TboxTexture, mUseMask, 24);
WWHD_SIZE(TboxTexture, 32);
struct TboxModelData {
    u8 base[0x30];
    gptr<TboxTexture> mpTexture;
    gptr<JUTNameTab> mpTextureName;
};
WWHD_OFFSET(TboxModelData, mpTexture, 0x30);
WWHD_OFFSET(TboxModelData, mpTextureName, 0x34);
struct TboxGraphicsTexture {
    u8 base[8];
    be<u32> width, height;
};
WWHD_OFFSET(TboxGraphicsTexture, width, 8);
WWHD_OFFSET(TboxGraphicsTexture, height, 12);

TboxModelInfo* Tbox_getModelInfo(daTbox_c*);
TboxModelData* Tbox_getRes(void*, TboxName*, s32);
const char* Tbox_getTextureName(JUTNameTab*, u16);
void* Tbox_getGraphicsImage(void*, s32);
void* Tbox_allocateTexture(u32);
TboxGraphicsTexture* Tbox_constructTexture(void*, void*);

// HD: PPC shifts wrap to six bits and return zero when bit five is set.
inline u32 tboxShiftLeft(u32 x, u32 count) {
    count &= 63;
    return count < 32 ? x << count : 0;
}
inline u32 tboxShiftRight(u32 x, u32 count) {
    count &= 63;
    return count < 32 ? x >> count : 0;
}

using TboxControlPtr = gptr<void>;
