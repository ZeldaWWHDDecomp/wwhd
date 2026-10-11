/* Circle geometry, WWHD layout.
 * HD stores mPosZ at 0x08 and mRadius at 0x10, unlike the GameCube base-class layout. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_cyl.h"

class cM3dGCir {
public:
    // HD: the verified scalar argument order is x, y, radius, z.
    void Set(f32 x, f32 y, f32 radius, f32 z);
    /* 0x00 */ be<f32> mPosX;
    /* 0x04 */ be<f32> mPosY;
    /* 0x08 */ be<f32> mPosZ;
    /* 0x0C */ be<u32> __vtbl;
    /* 0x10 */ be<f32> mRadius;
};
WWHD_OFFSET(cM3dGCir, mPosX, 0x00);
WWHD_OFFSET(cM3dGCir, mPosY, 0x04);
WWHD_OFFSET(cM3dGCir, mPosZ, 0x08);
WWHD_OFFSET(cM3dGCir, __vtbl, 0x0C);
WWHD_OFFSET(cM3dGCir, mRadius, 0x10);
WWHD_SIZE(cM3dGCir, 0x14);
void constructCircleBase(cM3dGCir* circle);
void deleteCircle(cM3dGCir* circle, s32 flags);
