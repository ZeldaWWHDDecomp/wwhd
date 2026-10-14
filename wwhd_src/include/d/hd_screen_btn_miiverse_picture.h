/* HD-only Miiverse picture button. Pane dimensions are layout-space width and height. */
#pragma once
#include "d/hd_screen_btn_miiverse_new_comment.h"
struct hdMiiversePicturePane_l {
    u8 mPanePrefix[0x3C];
    be<f32> mWidth;
    be<f32> mHeight;
    be<u8> mFlags;
    u8 mAlignment[3];
};
WWHD_OFFSET(hdMiiversePicturePane_l, mWidth, 0x3C);
WWHD_OFFSET(hdMiiversePicturePane_l, mHeight, 0x40);
WWHD_OFFSET(hdMiiversePicturePane_l, mFlags, 0x44);
WWHD_SIZE(hdMiiversePicturePane_l, 0x48);
struct hdBtnMiiversePicture_c : hdBasicButton_c {
    gptr<hdPartsPane_c> mpDecorations[3];
    gptr<hdMiiversePicturePane_l> mpPicture;
    static hdBtnMiiversePicture_c* construct(hdBtnMiiversePicture_c*);
    static void destroy(hdBtnMiiversePicture_c*, s32);
    u32 init(void* resource, u32 mode, hdPartsDesc_c* desc);
    f32 getWidth();
    f32 getHeight();
    void setPicture(void* texture);
    static void initializeStatics();
};
WWHD_OFFSET(hdBtnMiiversePicture_c, mpDecorations, 0x8C);
WWHD_OFFSET(hdBtnMiiversePicture_c, mpPicture, 0x98);
WWHD_SIZE(hdBtnMiiversePicture_c, 0x9C);
struct hdMiiversePaneLookupVtbl_l { u8 mPrefix[0x5C]; gfn mFind; };
struct hdMiiversePaneLookup_l { u8 mPrefix[8]; gptr<hdMiiversePaneLookupVtbl_l> mVtable; };
WWHD_OFFSET(hdMiiversePaneLookup_l, mVtable, 8);
WWHD_OFFSET(hdMiiversePaneLookupVtbl_l, mFind, 0x5C);
void setMiiversePictureTexture(hdMiiversePaneLookup_l*, void*, u32, u32);

hdPartsTypeInfo_c* miiversePictureType();
hdPartsPane_c* castMiiversePane(hdPartsPane_c*, hdPartsTypeInfo_c*);
