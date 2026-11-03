/* ResTIMG: the GameCube texture image header (tww JUTTexture.h), unchanged in WWHD (0x20).
 * Canonical layout for JUTTexture and the game code that reads it. */
#pragma once
#include "wwhd.h"
struct ResTIMG {
    be<u8> format;
    be<u8> alphaEnabled;
    be<u16> width;
    be<u16> height;
    be<u8> wrapS;
    be<u8> wrapT;
    be<u8> indexTexture;
    be<u8> colorFormat;
    be<u16> numColors;
    be<u32> paletteOffset;
    u8 mMipmap[4];
    be<u8> minFilter;
    be<u8> magFilter;
    be<s8> minLOD;
    be<s8> maxLOD;
    u8 mImageCount[2];
    be<s16> LODBias;
    be<u32> imageOffset;
};
WWHD_OFFSET(ResTIMG, width, 2);
WWHD_OFFSET(ResTIMG, height, 4);
WWHD_OFFSET(ResTIMG, indexTexture, 8);
WWHD_OFFSET(ResTIMG, paletteOffset, 0xc);
WWHD_OFFSET(ResTIMG, imageOffset, 0x1c);
WWHD_SIZE(ResTIMG, 0x20);
