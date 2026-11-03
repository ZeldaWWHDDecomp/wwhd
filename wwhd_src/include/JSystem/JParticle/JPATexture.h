// JPA texture objects retain the Cafe-expanded JUTTexture storage.
#pragma once
#include "JSystem/JUtility/JUTTexture.h"
/* Texture blob header (tww JPATexture.h name field; getter 028282FC returns data + 0xC). The opaque texture payload
 * follows the 0x20-byte prefix; no GameCube ResTIMG tail without HD evidence. */
struct JPATextureData {
    u8 mHeader[0xC];
    char mName[0x14];
};
WWHD_OFFSET(JPATextureData, mName, 0xC);
WWHD_SIZE(JPATextureData, 0x20);
struct JPATextureArc {
    gptr<const void> __vtbl;
    JUTTexture mTexture;
    gptr<const JPATextureData> mpData;
};
struct JPADefaultTexture {
    void initialize(void*);
    gptr<be<u8>> imgBuf;
    u8 mTexObj[0x40];
};
WWHD_OFFSET(JPATextureArc, mTexture, 4);
WWHD_OFFSET(JPATextureArc, mpData, 0x64);
WWHD_SIZE(JPATextureArc, 0x68);
WWHD_SIZE(JPADefaultTexture, 0x44);
