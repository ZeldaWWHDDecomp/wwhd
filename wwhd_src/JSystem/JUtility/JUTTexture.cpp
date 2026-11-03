/** JUTTexture platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JUtility/JUTTexture.h"
static JUTTextureStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 028433F4 */
static void __sinit_JUTTexture_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JUTTextureConstructRegister(&sStaticRegister);
}


/* 028431C0 */
void JUTTexture::initTexObj(u32 name) {
    // HD: GX texture initialization is deferred; this entry retains the TLUT index.
    mTlutName = name;
}
/* 028431C8 */
void JUTTexture::attachPalette(JUTPalette* palette) {
    if ((u8)mTexInfo->indexTexture == 0) return;
    if (palette == NULL) {
        // HD: always select the embedded palette, including a null pointer.
        palette = mEmbPalette;
        mPalette = palette;
    } else {
        mPalette = palette;
    }
    initTexObj(palette->mTlutName);
}
/* 028431FC */
void JUTTexture::storeTIMG(const ResTIMG* image,u32 paletteIndex) {
    if (image == NULL || paletteIndex >= 16) return;
    mTexInfo = image;
    mTexData = (u8*)image + (u32)image->imageOffset;
    if ((u32)image->imageOffset == 0) mTexData = (u8*)image + 0x20;
    image = mTexInfo;
    mPalette = NULL;
    mTlutName = 0;
    mWrapS = image->wrapS;
    mWrapT = image->wrapT;
    mMinFilter = image->minFilter;
    mMagFilter = image->magFilter;
    mMinLOD = (s8)image->minLOD;
    mMaxLOD = (s8)image->maxLOD;
    mLODBias = image->LODBias;
    u32 count = image->numColors;
    if (count == 0) return;
    JUTPalette* palette = mEmbPalette;
    if (count > 256) paletteIndex = (paletteIndex & 3) + 16;
    if (palette == NULL || !((u8)mFlags & 2)) {
        void* table = (u8*)image + (u32)image->paletteOffset;
        u32 alpha = image->alphaEnabled;
        u32 format = image->colorFormat;
        palette = (JUTPalette*)operator_new(sizeof(JUTPalette));
        if (palette != NULL) palette->storeTLUTRaw(paletteIndex,format,alpha,count,table);
        u32 flags = mFlags;
        mEmbPalette = palette;
        mFlags = (flags & 1) | 2;
        attachPalette(palette);
    } else {
        void* table = (u8*)image + (u32)image->paletteOffset;
        u32 format = image->colorFormat;
        u32 alpha = image->alphaEnabled;
        palette->storeTLUTRaw(paletteIndex,format,alpha,count,table);
        attachPalette(mEmbPalette);
    }
}
/* 02843374 */
static void JUTTexture__dt(JUTTexture* texture,u32 flags) {
    if (texture == NULL) return;
    u32 textureFlags = texture->mFlags;
    if (textureFlags & 1) {
        JUTTextureDeleteArray(texture->mCaptureData);
        textureFlags = texture->mFlags;
    }
    if (textureFlags & 2) operator_delete(texture->mEmbPalette);
    if (flags & 1) operator_delete(texture);
}
/* 028433E4 */
u32 JUTTexture::load() {
    JUTPalette* palette = mPalette;
    return palette != NULL ? palette->load() : 0;
}

/* WWHD glue
 ext 0273AFC8 void JUTTextureDeleteArray(void*)
 data sStaticBlock 104B6544
 data sStaticRegister 101FAFE4
 ext 028F026C void JUTTextureConstructRegister(void*)
*/
