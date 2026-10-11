#pragma once
#include "d/actor/d_a_bdk.h"
#include "d/d_save.h"
#include "d/actor/d_a_bdk_obj.h"
extern BdkObjectSavePointer& l_save;

// HD: each joint owns a linked mesh list; shapes have a separate visibility byte.
struct BdkDrawShape { gptr<void> mpShape; be<u8> mVisible; };
struct BdkDrawMesh { u8 mPrefix[4]; gptr<BdkDrawMesh> mpNext; gptr<BdkDrawShape> mpShape; };
struct BdkDrawJoint { u8 mPrefix[0x10]; gptr<BdkDrawMesh> mpMesh; u8 mSuffix[8]; };
struct BdkDrawModelData { u8 mPrefix[4]; be<u32> mJointCount; gptr<BdkDrawJoint> mpJoints; };
struct BdkDrawModel { u8 mPrefix[0xAC]; gptr<BdkDrawModelData> mpModelData; u8 mPadding[0x18]; Mtx34 mBaseTRMtx; };
struct BdkDrawMorf { u8 mPrefix[0x90]; gptr<J3DModel> mpModel; };
struct BdkDrawJointRegistry { u8 mPrefix[8]; be<u16> mJointCount; };
struct BdkDrawTailScales { be<f32> mScale[9]; };
struct BdkDrawMaskStages { be<u32> mJoint[4]; };
struct BdkDrawBlur { be<u8> mEnabled; be<u8> mRate; };
struct BdkDrawSaveSystem { u8 mPrefix[0x20]; dSv_info_c mSave; };
struct BdkDrawSavePointer { gptr<BdkDrawSaveSystem> mpSave; };
WWHD_SIZE(BdkDrawJoint, 0x1C);
WWHD_OFFSET(BdkDrawMesh, mpShape, 8);
WWHD_OFFSET(BdkDrawShape, mVisible, 4);
WWHD_OFFSET(BdkDrawModel, mpModelData, 0xAC);
WWHD_OFFSET(BdkDrawModel, mBaseTRMtx, 0xC8);
WWHD_SIZE(BdkDrawModel, 0xF8);
WWHD_OFFSET(BdkDrawMorf, mpModel, 0x90);
WWHD_OFFSET(BdkDrawJointRegistry, mJointCount, 8);
WWHD_SIZE(BdkDrawTailScales, 0x24);
WWHD_SIZE(BdkDrawMaskStages, 0x10);
WWHD_SIZE(BdkDrawBlur, 2);
WWHD_OFFSET(BdkDrawSaveSystem, mSave.mMemory.mMembit, 0x798);
WWHD_SIZE(BdkDrawSavePointer, 4);

// HD: lighting copies omit the three 0x40-byte renderer caches and padding.
// Words holding floats are copied as bits, preserving signaling NaNs.
struct BdkDrawLight {
    be<u32> mPositionAndDirectionBits[6];
    be<u8> mColor[4];
    be<u16> mParameters[4];
    be<u32> mFactorBits[8];
};
struct BdkDrawTev {
    BdkDrawLight mLight; u8 mLightCache[0x40];
    be<u32> mLightPosBits[3]; be<u16> mColorC0[4]; be<u32> mColorFogBlendBits[7];
    be<u8> mFlags[9]; u8 mPadding[3];
    BdkDrawLight mHdLight; u8 mHdLightCache[0x40];
    BdkDrawLight mHdLight2; u8 mHdLightCache2[0x40];
};
WWHD_SIZE(BdkDrawLight, 0x44);
WWHD_OFFSET(BdkDrawTev, mLightPosBits, 0x84);
WWHD_OFFSET(BdkDrawTev, mFlags, 0xB4);
WWHD_OFFSET(BdkDrawTev, mHdLight, 0xC0);
WWHD_OFFSET(BdkDrawTev, mHdLight2, 0x144);
WWHD_SIZE(BdkDrawTev, 0x1C8);
static inline void bdkDraw_copyLight(BdkDrawLight* dst, const BdkDrawLight* src) {
    for (u32 i = 0; i < 6; ++i) dst->mPositionAndDirectionBits[i] = src->mPositionAndDirectionBits[i];
    for (u32 i = 0; i < 4; ++i) dst->mColor[i] = src->mColor[i];
    for (u32 i = 0; i < 4; ++i) dst->mParameters[i] = src->mParameters[i];
    for (u32 i = 0; i < 8; ++i) dst->mFactorBits[i] = src->mFactorBits[i];
}
static inline void bdkDraw_copyTev(dKy_tevstr_c* destination, const dKy_tevstr_c* source) {
    BdkDrawTev* dst = reinterpret_cast<BdkDrawTev*>(destination);
    const BdkDrawTev* src = reinterpret_cast<const BdkDrawTev*>(source);
    bdkDraw_copyLight(&dst->mLight, &src->mLight);
    for (u32 i = 0; i < 3; ++i) dst->mLightPosBits[i] = src->mLightPosBits[i];
    for (u32 i = 0; i < 4; ++i) dst->mColorC0[i] = src->mColorC0[i];
    for (u32 i = 0; i < 7; ++i) dst->mColorFogBlendBits[i] = src->mColorFogBlendBits[i];
    for (u32 i = 0; i < 9; ++i) dst->mFlags[i] = src->mFlags[i];
    bdkDraw_copyLight(&dst->mHdLight, &src->mHdLight);
    bdkDraw_copyLight(&dst->mHdLight2, &src->mHdLight2);
}
static inline J3DModel* bdkDraw_getModel(mDoExt_McaMorf* morf) { return reinterpret_cast<BdkDrawMorf*>(morf)->mpModel; }
static inline Mtx34* bdkDraw_getAnmMtx(J3DModel_bdk* model, s32 joint) {
    J3DMtxBlock_bdk* matrices = model->mpMtxBlock;
    matrices->mFlags = matrices->mFlags | 0x10;
    return matrices->mpMtx.get() + joint;
}
static inline void bdkDraw_setBaseTRMtx(J3DModel* model, const Mtx34* source) {
    Mtx34* destination = &reinterpret_cast<BdkDrawModel*>(model)->mBaseTRMtx;
    f32 elements[3][4];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            elements[row][column] = source->m[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->m[row][column] = elements[row][column];
}
BdkDrawJointRegistry* BdkDraw_getJointRegistry(BdkDrawModelData*);
void BdkDraw_onBlure();
BOOL BdkDraw_isDungeonItem(dSv_memBit_c*, s32);
enum { DK_JNT_J_DK_ATAMA1_e = 0x18, DSNAP_TYPE_BDK = 0xCA, TEV_TYPE_BG3 = 4 };
WWHD_SIZE(BdkDrawModelData, 0xC);
