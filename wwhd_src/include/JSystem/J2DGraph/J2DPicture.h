#pragma once
#include "JSystem/J2DGraph/J2DPane.h"
#include "JSystem/JUtility/JUTTexture.h"
#include "JSystem/JUtility/JUTResource.h"
struct J2DPicture : J2DPane {
    gptr<JUTTexture> mpTexture[4];
    be<u8> mNumTexture,mValidTexture,mBinding,mFlag;
    be<f32> mBlendKonstColorF[4],mBlendKonstAlphaF[4];
    gptr<JUTPalette> mpPalette;
    J2DColor mColorWhite,mColorBlack,mCornerColor[4],mBlendKonstColor,mBlendKonstAlpha;
    void setBlendKonstColor();void setBlendKonstAlpha();void initinfo();
    bool insert(const ResTIMG*,u32,f32);bool insert(const char*,u32,f32);
    void loadTextures();const ResTIMG* changeTexture(const ResTIMG*,u32);const ResTIMG* changeTexture(const char*,u32);
    void swap(be<f32>*,be<f32>*);
    // HD adapter: four overflow floats use the typed caller parameter area.
    void drawTexCoord(f32,f32,f32,f32,f32,f32,f32,f32,const Mtx34*);
    void drawFullSet(f32,f32,f32,f32,u32,u32,u32,const Mtx34*);
    void drawSelf(f32,f32,const Mtx34*);
};
WWHD_OFFSET(J2DPicture,mpTexture,0xCC);WWHD_OFFSET(J2DPicture,mNumTexture,0xDC);
WWHD_OFFSET(J2DPicture,mBlendKonstColorF,0xE0);WWHD_OFFSET(J2DPicture,mBlendKonstAlphaF,0xF0);
WWHD_OFFSET(J2DPicture,mpPalette,0x100);WWHD_OFFSET(J2DPicture,mCornerColor,0x10C);
WWHD_OFFSET(J2DPicture,mBlendKonstColor,0x11C);WWHD_SIZE(J2DPicture,0x124);
void* pictureAllocate(u32);
void pictureDelete(void*);
void pictureTextureDestroy(JUTTexture*,u32);
J2DPane* picturePaneConstruct(J2DPane*);
void picturePaneDestroy(J2DPane*,u32);
void pictureClear(void*,u32);
void pictureConstructArray(void*,u32,u32,u32);
J2DColor* pictureColorConstruct(J2DColor*);
const ResTIMG* pictureFindResource(const char*,void*);
void pictureRegisterStatic(void*);

s32 pictureRead(JSUInputStream*,void*,s32);
s32 pictureSeek(JSURandomInputStream*,s32,u32);
void pictureMakePaneStream(J2DPane*,J2DPane*,JSURandomInputStream*);
void* pictureGetResource(JUTResReference*,JSUInputStream*,u32,void*);
void pictureStorePalette(JUTPalette*,u32,ResTLUT*);
void pictureAttachPalette(JUTTexture*,JUTPalette*);
