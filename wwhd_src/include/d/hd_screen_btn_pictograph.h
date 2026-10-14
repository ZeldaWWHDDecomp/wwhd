/** HD BtnPictograph_00 layout, fade and thumbnail material. Private binary-derived source. */
#pragma once
#include "d/hd_screen_best_shot_mark.h"
#include "d/hd_parts_basic_button.h"
struct hdPictoTextureExtent_c { be<u32> mTexture; be<u16> mWidth, mHeight; };
WWHD_SIZE(hdPictoTextureExtent_c, 8);
struct hdPictoMaterial_l { u8 _reserved00[0x10]; gptr<void> mpTextureMap; };
WWHD_OFFSET(hdPictoMaterial_l, mpTextureMap, 0x10);
struct hdPictographButton_c : hdBasicButton_c {
    hdPictoTextureExtent_c mTexture;
    u8 _reserved94[0x134 - 0x94];
    static hdPictographButton_c* construct(hdPictographButton_c*);
    static void destroy(hdPictographButton_c*, s32);
    u32 initialize(u32, u32, hdPartsDesc_c*);
    void setHorizontalPosition(f32);
    BOOL fadeOut();
    void resetFade(); void hide();
    void setThumbnail(hdPictoTextureExtent_c*);
    void restoreThumbnail();
    void showBestShot(); void hideBestShot();
    u32 createChild(hdPartsDesc_c*);
};
WWHD_OFFSET(hdPictographButton_c, mTexture, 0x8C);
WWHD_SIZE(hdPictographButton_c, 0x134);
void hdPictoBasicButtonDestroy(hdBasicButton_c*, s32);
void hdPictoCopyTexture(void*, hdPictoTextureExtent_c*);
void hdPictoSetPaneTexture(hdPartsPaneSRT_c*, hdPictoTextureExtent_c*, u32, u32);
u32 hdPictoCreateDefaultChild(hdParts_c*, u32, hdPartsDesc_c*);
f64 hdPictoSine(f64);
