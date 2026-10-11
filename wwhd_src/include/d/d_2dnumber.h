/* Battery and paired-picture HUD draw lists. GameCube fields and sizes retained.
 * HD: J2DPane members before its vtable are shifted by -4. Own partial views
 * avoid changing shared J2D headers. */
#pragma once
#include "bindings.h"

struct d2d_cXy {
    be<f32> x, y;
};
WWHD_SIZE(d2d_cXy, 8);
struct ResTIMG {
    u8 mUnaccessed[2];
    be<u16> width, height;
    u8 mRemaining[0x20 - 6];
};
WWHD_SIZE(ResTIMG, 0x20);
WWHD_OFFSET(ResTIMG, width, 2);
WWHD_OFFSET(ResTIMG, height, 4);
struct d2d_picture {
    struct VTable {
        u8 mUnaccessed[0x3C];
        gfn makeMatrix;
    };
    u8 mUnaccessed[0x98];
    /* 0x098 */ d2d_cXy mBasePosition;
    /* 0x0A0 */ be<f32> mRotation;
    /* 0x0A4 */ be<u8> mRotationAxis;
    u8 mPaneFlags[3];
    /* 0x0A8 */ be<u8> mAlpha;
    u8 mPaneRemaining[0xC8 - 0xA9];
    /* 0x0C8 */ gptr<VTable> __vtbl;
    u8 mTextureState[0x104 - 0xCC];
    /* 0x104 */ be<u32> mColorWhite;
    /* 0x108 */ be<u32> mColorBlack;
    /* 0x10C */ be<u32> mCornerColor[4];
    u8 mBlendColors[8];
};
WWHD_OFFSET(d2d_picture, mBasePosition, 0x98);
WWHD_OFFSET(d2d_picture, mRotation, 0xA0);
WWHD_OFFSET(d2d_picture, mAlpha, 0xA8);
WWHD_OFFSET(d2d_picture, __vtbl, 0xC8);
WWHD_OFFSET(d2d_picture, mColorWhite, 0x104);
WWHD_OFFSET(d2d_picture, mCornerColor, 0x10C);
WWHD_SIZE(d2d_picture, 0x124);
struct d2d_graf {
    struct VTable {
        u8 mUnaccessed[0x24];
        gfn setPort;
    };
    u8 mUnaccessed[0xB8];
    gptr<VTable> __vtbl;
};
WWHD_OFFSET(d2d_graf, __vtbl, 0xB8);
WWHD_SIZE(d2d_graf, 0xBC);
struct d2d_gameInfo {
    u8 mUnaccessed[0x5D10];
    gptr<d2d_graf> mpCurrentGrafPort;
};
WWHD_OFFSET(d2d_gameInfo, mpCurrentGrafPort, 0x5D10);
WWHD_SIZE(d2d_gameInfo, 0x5D14);
class dDlst_2DBattery_c {
  public:
    dDlst_2DBattery_c();
    u32 init(ResTIMG *rule, ResTIMG *battery, ResTIMG *batteryBase, ResTIMG *degree);
    void draw();
    void setRotate(f32 rot);
    be<u32> __vtbl;
    gptr<d2d_picture> mRule, mBattery, mBatteryBase;
    gptr<d2d_picture> mNum[3][2];
    d2d_cXy mPicturePos, mNumPos, mRuleSize, mBatterySize, mBatteryBaseSize, mNumSize;
    be<f32> mRotation;
};
WWHD_SIZE(dDlst_2DBattery_c, 0x5C);
WWHD_OFFSET(dDlst_2DBattery_c, mNum, 0x10);
WWHD_OFFSET(dDlst_2DBattery_c, mPicturePos, 0x28);
WWHD_OFFSET(dDlst_2DBattery_c, mNumPos, 0x30);
WWHD_OFFSET(dDlst_2DBattery_c, mRuleSize, 0x38);
WWHD_OFFSET(dDlst_2DBattery_c, mRotation, 0x58);
class dDlst_2DObject_c {
  public:
    dDlst_2DObject_c();
    u32 init(ResTIMG *img1, ResTIMG *img2);
    void draw();
    be<u32> __vtbl;
    gptr<d2d_picture> mPicture[2];
    d2d_cXy mPos, mSize[2];
    be<f32> mScale;
    be<u8> mCurrentNo;
    u8 mPadding[3];
};
WWHD_SIZE(dDlst_2DObject_c, 0x2C);
WWHD_OFFSET(dDlst_2DObject_c, mPos, 0xC);
WWHD_OFFSET(dDlst_2DObject_c, mSize, 0x14);
WWHD_OFFSET(dDlst_2DObject_c, mScale, 0x24);
WWHD_OFFSET(dDlst_2DObject_c, mCurrentNo, 0x28);
struct HudName16 {
    u8 bytes[16];
};
void dDlst_base_construct(void *object);
void initializeStorage(void *storage, u32 size);
d2d_picture *J2DPicture_createImage(d2d_picture *object, ResTIMG *image);
d2d_picture *J2DPicture_createName(d2d_picture *object, const char *name);
void J2DPicture_draw(d2d_picture *picture, u32 mirrorX, u32 mirrorY, u32 transpose, f32 x, f32 y,
                     f32 width, f32 height);
void J2DPicture_changeTexture(d2d_picture *picture, const char *name, u32 index);
void formatTextureName(void *buffer, u32 size, const char *format, u32 digit);
void assertRotation(const char *file, u32 line, const char *condition);
