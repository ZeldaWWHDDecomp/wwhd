/* d_a_boomerang.h (WWHD) - WWHD layout of daBoomerang_c. */
#pragma once
#include "f_op/f_op_actor.h"

// The HD packets contain native GX2 buffer/declaration and uniform resources.
struct daBoomerang_renderPoint_l {
    gptr<void> mVertices;
    u8 mVertexBuffer[0x154];
    u8 mDeclaration[0xF4];
    be<u32> mAllocationBytes;
    gptr<void> mAllocation;
};
static_assert(sizeof(daBoomerang_renderPoint_l) == 0x254);

struct daBoomerang_blurRing_l {
    daBoomerang_renderPoint_l mPoints[60][2];
    be<u32> mWriteBuffer;
    u8 mDeclaration[12];
    gptr<void> mPrevious;
    be<u8> mInitialized;
    u8 _pad[3];
};
static_assert(sizeof(daBoomerang_blurRing_l) == 0x11778);
WWHD_OFFSET(daBoomerang_blurRing_l, mWriteBuffer, 0x11760);

struct daBoomerang_sightPacket_l {
    gptr<void> mVtable;
    be<f32> mMatrices[5][12];
    be<u8> mFrames[5];
    be<u8> mAlpha[5];
    u8 _padFE[2];
    be<u32> mVisible;
    gptr<void> mTexture, mImage, mResource;
    daBoomerang_renderPoint_l mPoints[2];
    be<u32> mWriteBuffer;
    u8 _renderResources[0x1CA4 - 0x5BC];
};
static_assert(sizeof(daBoomerang_sightPacket_l) == 0x1CA4);
WWHD_OFFSET(daBoomerang_sightPacket_l, mFrames, 0xF4);

struct daBoomerang_blurPacket_l {
    u8 _packetBase[0x98];
    be<s32> mSegments;
    cXyz mPosition;
    cXyz mTrail[4][60];
    gptr<void> mResource;
    daBoomerang_blurRing_l mRings[2];
    u8 _renderResources[0x24140 - 0x23ADC];
};
static_assert(sizeof(daBoomerang_blurPacket_l) == 0x24140);
WWHD_OFFSET(daBoomerang_blurPacket_l, mTrail, 0xA8);
WWHD_OFFSET(daBoomerang_blurPacket_l, mRings, 0xBEC);

struct daBoomerang_c : fopAc_ac_c {
    u8 _toModel[0x3AC - sizeof(fopAc_ac_c)];
    gptr<void> mModel;
    daBoomerang_sightPacket_l mSight;
    daBoomerang_blurPacket_l mBlur;
    u8 mKeep[0xB0];
    be<u32> mLockActorIDs[5];
    gptr<fopAc_ac_c> mLockActors[5];
    be<u8> mReturning, mUnused, mJustHit, mCatchAndDelete, mUnused2;
    be<u8> mLockCount, mCurrentLock, mThirdPerson, mCancel;
    u8 _tail[0x264C8 - 0x26275];
};
WWHD_OFFSET(daBoomerang_c, mModel, 0x3AC);
WWHD_OFFSET(daBoomerang_c, mSight, 0x3B0);
WWHD_OFFSET(daBoomerang_c, mBlur, 0x2054);
WWHD_OFFSET(daBoomerang_c, mLockActorIDs, 0x26244);
WWHD_OFFSET(daBoomerang_c, mLockActors, 0x26258);
WWHD_OFFSET(daBoomerang_c, mLockCount, 0x26271);
static_assert(sizeof(daBoomerang_c) == 0x264C8);
