/* Draw lists, depth peeks and mirror packets.
 * HD: draw lists shrink to 0x5C0, depth peeks have 64 entries, mirror packets grow to 0x3428.
 * Private views describe the verified BFRES/GX2 records without changing shared GameCube headers. */
#pragma once
#include "wwhd.h"
#include "gabi.h"
namespace dDlstHD {

struct dDlst_window_c {
    void setScissor(f32 x, f32 y, f32 width, f32 height);
    void setViewPort(f32 x, f32 y, f32 width, f32 height, f32 nearZ, f32 farZ);
    /* 0x000 */ be<f32> mXOrig;
    /* 0x004 */ be<f32> mYOrig;
    /* 0x008 */ be<f32> mWidth;
    /* 0x00C */ be<f32> mHeight;
    /* 0x010 */ be<f32> mNearZ;
    /* 0x014 */ be<f32> mFarZ;
    /* 0x018 */ be<f32> mScissorXOrig;
    /* 0x01C */ be<f32> mScissorYOrig;
    /* 0x020 */ be<f32> mScissorWidth;
    /* 0x024 */ be<f32> mScissorHeight;
    /* 0x028 */ be<u8> mCameraID;
    /* 0x029 */ be<u8> mMode;
    u8 mPadding42[2];
};
WWHD_SIZE(dDlst_window_c, 0x2C);
WWHD_OFFSET(dDlst_window_c, mXOrig, 0x0);
WWHD_OFFSET(dDlst_window_c, mYOrig, 0x4);
WWHD_OFFSET(dDlst_window_c, mWidth, 0x8);
WWHD_OFFSET(dDlst_window_c, mHeight, 0xC);
WWHD_OFFSET(dDlst_window_c, mNearZ, 0x10);
WWHD_OFFSET(dDlst_window_c, mFarZ, 0x14);
WWHD_OFFSET(dDlst_window_c, mScissorXOrig, 0x18);
WWHD_OFFSET(dDlst_window_c, mScissorYOrig, 0x1C);
WWHD_OFFSET(dDlst_window_c, mScissorWidth, 0x20);
WWHD_OFFSET(dDlst_window_c, mScissorHeight, 0x24);
WWHD_OFFSET(dDlst_window_c, mCameraID, 0x28);
WWHD_OFFSET(dDlst_window_c, mMode, 0x29);

struct cM_rnd_c {
    void init(u32 a, u32 b, u32 c);
    /* 0x000 */ be<u32> mSeed;
    /* 0x004 */ be<u32> mMultiplier;
    /* 0x008 */ be<u32> mIncrement;
};
WWHD_SIZE(cM_rnd_c, 0xC);
WWHD_OFFSET(cM_rnd_c, mSeed, 0x0);
WWHD_OFFSET(cM_rnd_c, mMultiplier, 0x4);
WWHD_OFFSET(cM_rnd_c, mIncrement, 0x8);

struct dDlst_base_c {
    /* 0x000 */ be<u32> mVtable;
};
WWHD_SIZE(dDlst_base_c, 0x4);
WWHD_OFFSET(dDlst_base_c, mVtable, 0x0);

struct dDlst_effectLine_c {
    void update(u32 vector, u32 colour, u32 a, u32 b, u32 c, u32 d, f32 x, f32 y, f32 z, f32 w);
    /* 0x000 */ be<u32> mVtable;
    /* 0x004 */ be<u32> mSeed;
    /* 0x008 */ be<u32> mMultiplier;
    /* 0x00C */ be<u32> mIncrement;
    /* 0x010 */ be<u32> mPositionX;
    /* 0x014 */ be<u32> mPositionY;
    /* 0x018 */ be<u32> mPositionZ;
    /* 0x01C */ be<u32> mColor;
    /* 0x020 */ be<u16> mLineCount;
    /* 0x022 */ be<u16> mLineLength;
    /* 0x024 */ be<u16> mLineSpeed;
    /* 0x026 */ be<u16> mLineMode;
    /* 0x028 */ be<f32> mWidth;
    /* 0x02C */ be<f32> mHeight;
    /* 0x030 */ be<f32> mDepth;
    /* 0x034 */ be<f32> mScale;
};
WWHD_SIZE(dDlst_effectLine_c, 0x38);
WWHD_OFFSET(dDlst_effectLine_c, mVtable, 0x0);
WWHD_OFFSET(dDlst_effectLine_c, mSeed, 0x4);
WWHD_OFFSET(dDlst_effectLine_c, mMultiplier, 0x8);
WWHD_OFFSET(dDlst_effectLine_c, mIncrement, 0xC);
WWHD_OFFSET(dDlst_effectLine_c, mPositionX, 0x10);
WWHD_OFFSET(dDlst_effectLine_c, mPositionY, 0x14);
WWHD_OFFSET(dDlst_effectLine_c, mPositionZ, 0x18);
WWHD_OFFSET(dDlst_effectLine_c, mColor, 0x1C);
WWHD_OFFSET(dDlst_effectLine_c, mLineCount, 0x20);
WWHD_OFFSET(dDlst_effectLine_c, mLineLength, 0x22);
WWHD_OFFSET(dDlst_effectLine_c, mLineSpeed, 0x24);
WWHD_OFFSET(dDlst_effectLine_c, mLineMode, 0x26);
WWHD_OFFSET(dDlst_effectLine_c, mWidth, 0x28);
WWHD_OFFSET(dDlst_effectLine_c, mHeight, 0x2C);
WWHD_OFFSET(dDlst_effectLine_c, mDepth, 0x30);
WWHD_OFFSET(dDlst_effectLine_c, mScale, 0x34);

struct dDlst_peekZ_entry {
    /* 0x000 */ be<s16> mX;
    /* 0x002 */ be<s16> mY;
    /* 0x004 */ be<u32> mpDst;
};
WWHD_SIZE(dDlst_peekZ_entry, 0x8);
WWHD_OFFSET(dDlst_peekZ_entry, mX, 0x0);
WWHD_OFFSET(dDlst_peekZ_entry, mY, 0x2);
WWHD_OFFSET(dDlst_peekZ_entry, mpDst, 0x4);

struct dDlst_peekZ_c {
    void peekData();
    u32 newData(u32 x, u32 y, u32 output);
    /* 0x000 */ be<u8> mCount;
    u8 mPadding1[3];
    /* 0x004 */ dDlst_peekZ_entry mEntries[64];
};
WWHD_SIZE(dDlst_peekZ_c, 0x204);
WWHD_OFFSET(dDlst_peekZ_c, mCount, 0x0);
WWHD_OFFSET(dDlst_peekZ_c, mEntries, 0x4);

struct BufferedText {
    void assureTermination();
    /* 0x000 */ be<u32> mpBuffer;
    /* 0x004 */ be<u32> mVtable;
    /* 0x008 */ be<u32> mBufferSize;
    /* 0x00C */ be<u8> mBuffer[64];
};
WWHD_SIZE(BufferedText, 0x4C);
WWHD_OFFSET(BufferedText, mpBuffer, 0x0);
WWHD_OFFSET(BufferedText, mVtable, 0x4);
WWHD_OFFSET(BufferedText, mBufferSize, 0x8);
WWHD_OFFSET(BufferedText, mBuffer, 0xC);

struct SafeString {
    /* 0x000 */ be<u32> mpText;
    /* 0x004 */ be<u32> mVtable;
};
WWHD_SIZE(SafeString, 0x8);
WWHD_OFFSET(SafeString, mpText, 0x0);
WWHD_OFFSET(SafeString, mVtable, 0x4);

struct DrawBuffer {
    u8 mPadding0[12];
    /* 0x00C */ be<u32> mDrawType;
};
WWHD_SIZE(DrawBuffer, 0x10);
WWHD_OFFSET(DrawBuffer, mDrawType, 0xC);

struct dDlst_list_c {
    void entryZSortXluDrawList(u32 buffer, u32 packet, u32 position);
    void draw(u32 begin, u32 end);
    u32 set(u32 current, u32 end, u32 packet);
    void reset();
    u32 init();
    /* 0x000 */ be<f32> mXOrig;
    /* 0x004 */ be<f32> mYOrig;
    /* 0x008 */ be<f32> mWidth;
    /* 0x00C */ be<f32> mHeight;
    /* 0x010 */ be<f32> mNearZ;
    /* 0x014 */ be<f32> mFarZ;
    /* 0x018 */ be<u8> mHasWindow;
    u8 mPadding25[3];
    /* 0x01C */ be<u32> mpOpaListSky;
    /* 0x020 */ be<u32> mpXluListSky;
    /* 0x024 */ be<u32> mpOpaListP0;
    /* 0x028 */ be<u32> mpOpaListP1;
    /* 0x02C */ be<u32> mpXluListP1;
    /* 0x030 */ be<u32> mpOpaListPlayerExtra;
    /* 0x034 */ be<u32> mpOpaListZelda0;
    /* 0x038 */ be<u32> mpOpaListZelda1;
    /* 0x03C */ be<u32> mpXluListZelda1;
    /* 0x040 */ be<u32> mpOpaListBG;
    /* 0x044 */ be<u32> mpXluListBG;
    /* 0x048 */ be<u32> mpOpaList;
    /* 0x04C */ be<u32> mpXluList;
    /* 0x050 */ be<u32> mpOpaListFilter;
    /* 0x054 */ be<u32> mpOpaListMaskOff;
    /* 0x058 */ be<u32> mpXluListMaskOff;
    /* 0x05C */ be<u32> mpOpaListInvisible;
    /* 0x060 */ be<u32> mpXluListInvisible;
    /* 0x064 */ be<u32> mpListShadowOn;
    /* 0x068 */ be<u32> mpOpaList2D;
    /* 0x06C */ be<u32> mpListSnapshot;
    /* 0x070 */ be<u32> mpListSea;
    /* 0x074 */ be<u32> mpListCloud;
    /* 0x078 */ be<u32> mpListDisableBloom;
    /* 0x07C */ be<u32> mp2DCopyList[4];
    /* 0x08C */ be<u32> mp2DCopyListCur;
    /* 0x090 */ be<u32> mp2DCopyListEnd;
    /* 0x094 */ be<u32> mp2DOpaTopList[16];
    /* 0x0D4 */ be<u32> mp2DOpaTopListCur;
    /* 0x0D8 */ be<u32> mp2DOpaTopListEnd;
    /* 0x0DC */ be<u32> mp2DOpaList[64];
    /* 0x1DC */ be<u32> mp2DOpaListCur;
    /* 0x1E0 */ be<u32> mp2DOpaListEnd;
    /* 0x1E4 */ be<u32> mp2DXluList[32];
    /* 0x264 */ be<u32> mp2DXluListCur;
    /* 0x268 */ be<u32> mp2DXluListEnd;
    /* 0x26C */ be<u32> mpWindow;
    /* 0x270 */ be<u32> mpViewPort;
    /* 0x274 */ be<u32> mpView;
    /* 0x278 */ be<u32> mpProjection;
    /* 0x27C */ be<u32> mpCamera;
    /* 0x280 */ be<u32> mpRenderTarget;
    /* 0x284 */ be<u8> m3DlineMatSortPacket[312];
    /* 0x3BC */ dDlst_peekZ_c mPeekZ;
};
WWHD_SIZE(dDlst_list_c, 0x5C0);
WWHD_OFFSET(dDlst_list_c, mXOrig, 0x0);
WWHD_OFFSET(dDlst_list_c, mYOrig, 0x4);
WWHD_OFFSET(dDlst_list_c, mWidth, 0x8);
WWHD_OFFSET(dDlst_list_c, mHeight, 0xC);
WWHD_OFFSET(dDlst_list_c, mNearZ, 0x10);
WWHD_OFFSET(dDlst_list_c, mFarZ, 0x14);
WWHD_OFFSET(dDlst_list_c, mHasWindow, 0x18);
WWHD_OFFSET(dDlst_list_c, mpOpaListSky, 0x1C);
WWHD_OFFSET(dDlst_list_c, mpXluListSky, 0x20);
WWHD_OFFSET(dDlst_list_c, mpOpaListP0, 0x24);
WWHD_OFFSET(dDlst_list_c, mpOpaListP1, 0x28);
WWHD_OFFSET(dDlst_list_c, mpXluListP1, 0x2C);
WWHD_OFFSET(dDlst_list_c, mpOpaListPlayerExtra, 0x30);
WWHD_OFFSET(dDlst_list_c, mpOpaListZelda0, 0x34);
WWHD_OFFSET(dDlst_list_c, mpOpaListZelda1, 0x38);
WWHD_OFFSET(dDlst_list_c, mpXluListZelda1, 0x3C);
WWHD_OFFSET(dDlst_list_c, mpOpaListBG, 0x40);
WWHD_OFFSET(dDlst_list_c, mpXluListBG, 0x44);
WWHD_OFFSET(dDlst_list_c, mpOpaList, 0x48);
WWHD_OFFSET(dDlst_list_c, mpXluList, 0x4C);
WWHD_OFFSET(dDlst_list_c, mpOpaListFilter, 0x50);
WWHD_OFFSET(dDlst_list_c, mpOpaListMaskOff, 0x54);
WWHD_OFFSET(dDlst_list_c, mpXluListMaskOff, 0x58);
WWHD_OFFSET(dDlst_list_c, mpOpaListInvisible, 0x5C);
WWHD_OFFSET(dDlst_list_c, mpXluListInvisible, 0x60);
WWHD_OFFSET(dDlst_list_c, mpListShadowOn, 0x64);
WWHD_OFFSET(dDlst_list_c, mpOpaList2D, 0x68);
WWHD_OFFSET(dDlst_list_c, mpListSnapshot, 0x6C);
WWHD_OFFSET(dDlst_list_c, mpListSea, 0x70);
WWHD_OFFSET(dDlst_list_c, mpListCloud, 0x74);
WWHD_OFFSET(dDlst_list_c, mpListDisableBloom, 0x78);
WWHD_OFFSET(dDlst_list_c, mp2DCopyList, 0x7C);
WWHD_OFFSET(dDlst_list_c, mp2DCopyListCur, 0x8C);
WWHD_OFFSET(dDlst_list_c, mp2DCopyListEnd, 0x90);
WWHD_OFFSET(dDlst_list_c, mp2DOpaTopList, 0x94);
WWHD_OFFSET(dDlst_list_c, mp2DOpaTopListCur, 0xD4);
WWHD_OFFSET(dDlst_list_c, mp2DOpaTopListEnd, 0xD8);
WWHD_OFFSET(dDlst_list_c, mp2DOpaList, 0xDC);
WWHD_OFFSET(dDlst_list_c, mp2DOpaListCur, 0x1DC);
WWHD_OFFSET(dDlst_list_c, mp2DOpaListEnd, 0x1E0);
WWHD_OFFSET(dDlst_list_c, mp2DXluList, 0x1E4);
WWHD_OFFSET(dDlst_list_c, mp2DXluListCur, 0x264);
WWHD_OFFSET(dDlst_list_c, mp2DXluListEnd, 0x268);
WWHD_OFFSET(dDlst_list_c, mpWindow, 0x26C);
WWHD_OFFSET(dDlst_list_c, mpViewPort, 0x270);
WWHD_OFFSET(dDlst_list_c, mpView, 0x274);
WWHD_OFFSET(dDlst_list_c, mpProjection, 0x278);
WWHD_OFFSET(dDlst_list_c, mpCamera, 0x27C);
WWHD_OFFSET(dDlst_list_c, mpRenderTarget, 0x280);
WWHD_OFFSET(dDlst_list_c, m3DlineMatSortPacket, 0x284);
WWHD_OFFSET(dDlst_list_c, mPeekZ, 0x3BC);

struct TexturePacket {
    /* 0x000 */ be<u32> mpVertices;
    /* 0x004 */ be<u8> mVertexBuffer[340];
    /* 0x158 */ be<u8> mVertexArray[244];
    /* 0x24C */ be<u32> mAlignment;
    /* 0x250 */ be<u32> mpAllocation;
};
WWHD_SIZE(TexturePacket, 0x254);
WWHD_OFFSET(TexturePacket, mpVertices, 0x0);
WWHD_OFFSET(TexturePacket, mVertexBuffer, 0x4);
WWHD_OFFSET(TexturePacket, mVertexArray, 0x158);
WWHD_OFFSET(TexturePacket, mAlignment, 0x24C);
WWHD_OFFSET(TexturePacket, mpAllocation, 0x250);

struct DoubleTexturePacket {
    /* 0x000 */ TexturePacket mPackets[2];
    /* 0x4A8 */ be<u32> mCurrent;
    /* 0x4AC */ be<u32> mVertexLayout;
    /* 0x4B0 */ be<u32> mVertexStride;
    u8 mPadding1204[4];
    /* 0x4B8 */ be<u32> mpMaterial;
    /* 0x4BC */ be<u8> mReady;
    u8 mPadding1213[3];
};
WWHD_SIZE(DoubleTexturePacket, 0x4C0);
WWHD_OFFSET(DoubleTexturePacket, mPackets, 0x0);
WWHD_OFFSET(DoubleTexturePacket, mCurrent, 0x4A8);
WWHD_OFFSET(DoubleTexturePacket, mVertexLayout, 0x4AC);
WWHD_OFFSET(DoubleTexturePacket, mVertexStride, 0x4B0);
WWHD_OFFSET(DoubleTexturePacket, mpMaterial, 0x4B8);
WWHD_OFFSET(DoubleTexturePacket, mReady, 0x4BC);

struct dDlst_mirrorPacket {
    void drawMaterial(u32 packet);
    void update(u32 matrix);
    void init(u32 texture);
    u8 mPadding0[12];
    /* 0x00C */ be<u32> mVtable;
    u8 mPadding16[136];
    /* 0x098 */ be<f32> mPosMtx[12];
    /* 0x0C8 */ be<f32> mTexMtx[16];
    /* 0x108 */ be<f32> mLightMtx[12];
    /* 0x138 */ be<u16> mShadowPolyCount;
    u8 mPadding314[2];
    /* 0x13C */ be<u32> mShadowPolyVtable;
    u8 mPadding320[9216];
    /* 0x2540 */ be<u8> mTex[64];
    /* 0x2580 */ be<u8> mAlpha;
    u8 mPadding9601[3];
    /* 0x2584 */ be<u8> mSourceTexture[144];
    /* 0x2614 */ be<u8> mTexObj[408];
    /* 0x27AC */ be<u8> mSceneTexture[408];
    /* 0x2944 */ be<u8> mIndirectTexture[408];
    /* 0x2ADC */ be<u32> mpMaterial;
    /* 0x2AE0 */ DoubleTexturePacket mPackets;
    /* 0x2FA0 */ be<u8> mShaderParam[12];
    /* 0x2FAC */ be<u8> mUniformBlock[168];
    /* 0x3054 */ be<u8> mViewUniformBlock[116];
    /* 0x30C8 */ be<f32> mUniformValues[44];
    /* 0x3178 */ be<u8> mLightParameters[32];
    /* 0x3198 */ be<u8> mSceneParameters[32];
    /* 0x31B8 */ be<u8> mShadowParameters[32];
    /* 0x31D8 */ be<f32> mProjectionMtx[12];
    /* 0x3208 */ be<f32> mShadowMtx[12];
    u8 mPadding12856[288];
    /* 0x3358 */ be<f32> mEyePosition[4];
    /* 0x3368 */ be<f32> mEyeDirection[4];
    u8 mPadding13176[64];
    /* 0x33B8 */ be<u8> mEnabled;
    u8 mPadding13241[3];
    /* 0x33BC */ be<u8> mLinChk[88];
    /* 0x3414 */ be<u32> mPolyPassVtable;
    /* 0x3418 */ be<u8> mPolyPassFlags[7];
    u8 mPadding13343[1];
    /* 0x3420 */ be<u32> mGrpPassVtable;
    /* 0x3424 */ be<u32> mGrpPassMask;
};
WWHD_SIZE(dDlst_mirrorPacket, 0x3428);
WWHD_OFFSET(dDlst_mirrorPacket, mVtable, 0xC);
WWHD_OFFSET(dDlst_mirrorPacket, mPosMtx, 0x98);
WWHD_OFFSET(dDlst_mirrorPacket, mTexMtx, 0xC8);
WWHD_OFFSET(dDlst_mirrorPacket, mLightMtx, 0x108);
WWHD_OFFSET(dDlst_mirrorPacket, mShadowPolyCount, 0x138);
WWHD_OFFSET(dDlst_mirrorPacket, mShadowPolyVtable, 0x13C);
WWHD_OFFSET(dDlst_mirrorPacket, mTex, 0x2540);
WWHD_OFFSET(dDlst_mirrorPacket, mAlpha, 0x2580);
WWHD_OFFSET(dDlst_mirrorPacket, mSourceTexture, 0x2584);
WWHD_OFFSET(dDlst_mirrorPacket, mTexObj, 0x2614);
WWHD_OFFSET(dDlst_mirrorPacket, mSceneTexture, 0x27AC);
WWHD_OFFSET(dDlst_mirrorPacket, mIndirectTexture, 0x2944);
WWHD_OFFSET(dDlst_mirrorPacket, mpMaterial, 0x2ADC);
WWHD_OFFSET(dDlst_mirrorPacket, mPackets, 0x2AE0);
WWHD_OFFSET(dDlst_mirrorPacket, mShaderParam, 0x2FA0);
WWHD_OFFSET(dDlst_mirrorPacket, mUniformBlock, 0x2FAC);
WWHD_OFFSET(dDlst_mirrorPacket, mViewUniformBlock, 0x3054);
WWHD_OFFSET(dDlst_mirrorPacket, mUniformValues, 0x30C8);
WWHD_OFFSET(dDlst_mirrorPacket, mLightParameters, 0x3178);
WWHD_OFFSET(dDlst_mirrorPacket, mSceneParameters, 0x3198);
WWHD_OFFSET(dDlst_mirrorPacket, mShadowParameters, 0x31B8);
WWHD_OFFSET(dDlst_mirrorPacket, mProjectionMtx, 0x31D8);
WWHD_OFFSET(dDlst_mirrorPacket, mShadowMtx, 0x3208);
WWHD_OFFSET(dDlst_mirrorPacket, mEyePosition, 0x3358);
WWHD_OFFSET(dDlst_mirrorPacket, mEyeDirection, 0x3368);
WWHD_OFFSET(dDlst_mirrorPacket, mEnabled, 0x33B8);
WWHD_OFFSET(dDlst_mirrorPacket, mLinChk, 0x33BC);
WWHD_OFFSET(dDlst_mirrorPacket, mPolyPassVtable, 0x3414);
WWHD_OFFSET(dDlst_mirrorPacket, mPolyPassFlags, 0x3418);
WWHD_OFFSET(dDlst_mirrorPacket, mGrpPassVtable, 0x3420);
WWHD_OFFSET(dDlst_mirrorPacket, mGrpPassMask, 0x3424);

struct Texture {
    /* 0x000 */ be<u32> mFormat;
    /* 0x004 */ be<u32> mDimension;
    /* 0x008 */ be<u32> mWidth;
    /* 0x00C */ be<u32> mHeight;
    /* 0x010 */ be<u32> mDepth;
    /* 0x014 */ be<u32> mNumMips;
    /* 0x018 */ be<u32> mSwizzle;
    /* 0x01C */ be<u32> mAlignment;
    u8 mPadding32[8];
    /* 0x028 */ be<u32> mpBuffer;
    u8 mPadding44[4];
    /* 0x030 */ be<u32> mpImage;
    /* 0x034 */ be<u32> mPitch;
    /* 0x038 */ be<u32> mTileMode;
    u8 mPadding60[60];
    /* 0x078 */ be<u8> mMinWidth;
    /* 0x079 */ be<u8> mMinHeight;
    u8 mPadding122[22];
    /* 0x090 */ be<u8> mNeedsFlush;
    u8 mPadding145[19];
    /* 0x0A4 */ be<u32> mHasGpuBuffer;
    /* 0x0A8 */ be<u8> mGpuBuffer[44];
    /* 0x0D4 */ be<u32> mpGpuBuffer;
    u8 mPadding216[4];
    /* 0x0DC */ be<u32> mpGpuImage;
    u8 mPadding224[184];
};
WWHD_SIZE(Texture, 0x198);
WWHD_OFFSET(Texture, mFormat, 0x0);
WWHD_OFFSET(Texture, mDimension, 0x4);
WWHD_OFFSET(Texture, mWidth, 0x8);
WWHD_OFFSET(Texture, mHeight, 0xC);
WWHD_OFFSET(Texture, mDepth, 0x10);
WWHD_OFFSET(Texture, mNumMips, 0x14);
WWHD_OFFSET(Texture, mSwizzle, 0x18);
WWHD_OFFSET(Texture, mAlignment, 0x1C);
WWHD_OFFSET(Texture, mpBuffer, 0x28);
WWHD_OFFSET(Texture, mpImage, 0x30);
WWHD_OFFSET(Texture, mPitch, 0x34);
WWHD_OFFSET(Texture, mTileMode, 0x38);
WWHD_OFFSET(Texture, mMinWidth, 0x78);
WWHD_OFFSET(Texture, mMinHeight, 0x79);
WWHD_OFFSET(Texture, mNeedsFlush, 0x90);
WWHD_OFFSET(Texture, mHasGpuBuffer, 0xA4);
WWHD_OFFSET(Texture, mGpuBuffer, 0xA8);
WWHD_OFFSET(Texture, mpGpuBuffer, 0xD4);
WWHD_OFFSET(Texture, mpGpuImage, 0xDC);

struct MirrorModel {
    u8 mPadding0[4];
    /* 0x004 */ be<u32> mpData;
    /* 0x008 */ be<u32> mMaterialCount;
    /* 0x00C */ be<u32> mpMaterials;
};
WWHD_SIZE(MirrorModel, 0x10);
WWHD_OFFSET(MirrorModel, mpData, 0x4);
WWHD_OFFSET(MirrorModel, mMaterialCount, 0x8);
WWHD_OFFSET(MirrorModel, mpMaterials, 0xC);

struct MirrorModelData {
    u8 mPadding0[28];
    /* 0x01C */ be<u32> mMaterialCount;
    /* 0x020 */ be<u32> mpMaterials;
};
WWHD_SIZE(MirrorModelData, 0x24);
WWHD_OFFSET(MirrorModelData, mMaterialCount, 0x1C);
WWHD_OFFSET(MirrorModelData, mpMaterials, 0x20);

struct MirrorMaterial {
    /* 0x000 */ be<u32> mpShape;
    u8 mPadding4[16];
    /* 0x014 */ be<u32> mTextureCount;
    /* 0x018 */ be<u32> mpTextures;
    u8 mPadding28[4];
    /* 0x020 */ be<u8> mInitialized;
    u8 mPadding33[3];
};
WWHD_SIZE(MirrorMaterial, 0x24);
WWHD_OFFSET(MirrorMaterial, mpShape, 0x0);
WWHD_OFFSET(MirrorMaterial, mTextureCount, 0x14);
WWHD_OFFSET(MirrorMaterial, mpTextures, 0x18);
WWHD_OFFSET(MirrorMaterial, mInitialized, 0x20);

struct Shape {
    /* 0x000 */ be<u8> mFlags;
    u8 mPadding1[3];
    /* 0x004 */ be<u32> mpDisplayList;
    u8 mPadding8[4];
    /* 0x00C */ be<u32> mDisplayListSize;
    u8 mPadding16[108];
    /* 0x07C */ be<u32> mpModel;
};
WWHD_SIZE(Shape, 0x80);
WWHD_OFFSET(Shape, mFlags, 0x0);
WWHD_OFFSET(Shape, mpDisplayList, 0x4);
WWHD_OFFSET(Shape, mDisplayListSize, 0xC);
WWHD_OFFSET(Shape, mpModel, 0x7C);

struct ShapeModel {
    u8 mPadding0[40];
    /* 0x028 */ be<u32> mpMaterial;
};
WWHD_SIZE(ShapeModel, 0x2C);
WWHD_OFFSET(ShapeModel, mpMaterial, 0x28);

struct RenderContext {
    /* 0x000 */ be<u32> mpMaterial;
    /* 0x004 */ be<u32> mpShape;
};
WWHD_SIZE(RenderContext, 0x8);
WWHD_OFFSET(RenderContext, mpMaterial, 0x0);
WWHD_OFFSET(RenderContext, mpShape, 0x4);

struct RenderState {
    /* 0x000 */ be<u8> mDepthTest;
    /* 0x001 */ be<u8> mDepthWrite;
    u8 mPadding2[6];
    /* 0x008 */ be<u32> mBlendMode;
    u8 mPadding12[212];
    /* 0x0E0 */ be<u8> mCullMode;
    u8 mPadding225[11];
    /* 0x0EC */ be<u32> mDepthConfig;
    u8 mPadding240[44];
};
WWHD_SIZE(RenderState, 0x11C);
WWHD_OFFSET(RenderState, mDepthTest, 0x0);
WWHD_OFFSET(RenderState, mDepthWrite, 0x1);
WWHD_OFFSET(RenderState, mBlendMode, 0x8);
WWHD_OFFSET(RenderState, mCullMode, 0xE0);
WWHD_OFFSET(RenderState, mDepthConfig, 0xEC);

struct Heap {
    u8 mPadding0[12];
    /* 0x00C */ be<u32> mVtable;
};
WWHD_SIZE(Heap, 0x10);
WWHD_OFFSET(Heap, mVtable, 0xC);

struct HeapVtable {
    u8 mPadding0[52];
    /* 0x034 */ be<u32> mAlloc;
    u8 mPadding56[4];
    /* 0x03C */ be<u32> mFree;
};
WWHD_SIZE(HeapVtable, 0x40);
WWHD_OFFSET(HeapVtable, mAlloc, 0x34);
WWHD_OFFSET(HeapVtable, mFree, 0x3C);

struct Matrix34 {
    be<f32> m[12];
};
WWHD_SIZE(Matrix34, 48);
WWHD_OFFSET(Matrix34, m, 0);
struct Matrix44 {
    be<u32> m[16];
};
WWHD_SIZE(Matrix44, 64);
WWHD_OFFSET(Matrix44, m, 0);
struct VectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(VectorWords, 12);
WWHD_OFFSET(VectorWords, x, 0);
WWHD_OFFSET(VectorWords, y, 4);
WWHD_OFFSET(VectorWords, z, 8);
struct DrawVtable {
    u8 mPadding0[20];
    /* 0x014 */ be<u32> mDraw;
};
WWHD_SIZE(DrawVtable, 0x18);
WWHD_OFFSET(DrawVtable, mDraw, 0x14);

struct Resource {
    u8 mPadding0[24];
    /* 0x018 */ be<u32> mVtable;
    u8 mPadding28[32];
    /* 0x03C */ be<u32> mpTexture;
};
WWHD_SIZE(Resource, 0x40);
WWHD_OFFSET(Resource, mVtable, 0x18);
WWHD_OFFSET(Resource, mpTexture, 0x3C);

struct ResourceVtable {
    u8 mPadding0[12];
    /* 0x00C */ be<u32> mIsValid;
};
WWHD_SIZE(ResourceVtable, 0x10);
WWHD_OFFSET(ResourceVtable, mIsValid, 0xC);

struct Packet {
    /* 0x000 */ be<u32> mpModel;
    u8 mPadding4[8];
    /* 0x00C */ be<u32> mKind;
};
WWHD_SIZE(Packet, 0x10);
WWHD_OFFSET(Packet, mpModel, 0x0);
WWHD_OFFSET(Packet, mKind, 0xC);

struct PacketModel {
    u8 mPadding0[8];
    /* 0x008 */ be<u32> mpResource;
};
WWHD_SIZE(PacketModel, 0xC);
WWHD_OFFSET(PacketModel, mpResource, 0x8);

struct SpecularModel {
    u8 mPadding0[132];
    /* 0x084 */ be<u8> mLightPosWorld[12];
};
WWHD_SIZE(SpecularModel, 0x90);
WWHD_OFFSET(SpecularModel, mLightPosWorld, 0x84);

struct SpecularModelData {
    u8 mPadding0[42];
    /* 0x02A */ be<u16> mMaterialCount;
    u8 mPadding44[8];
    /* 0x034 */ be<u32> mpMaterials;
    u8 mPadding56[192];
    /* 0x0F8 */ be<f32> mTexMtx[12];
};
WWHD_SIZE(SpecularModelData, 0x128);
WWHD_OFFSET(SpecularModelData, mMaterialCount, 0x2A);
WWHD_OFFSET(SpecularModelData, mpMaterials, 0x34);
WWHD_OFFSET(SpecularModelData, mTexMtx, 0xF8);

struct SpecularMaterial {
    /* 0x000 */ be<u32> mpDefinition;
    /* 0x004 */ be<u16> mFlags;
    u8 mPadding6[6];
    /* 0x00C */ be<u32> mpDirtyBits;
    u8 mPadding16[44];
};
WWHD_SIZE(SpecularMaterial, 0x3C);
WWHD_OFFSET(SpecularMaterial, mpDefinition, 0x0);
WWHD_OFFSET(SpecularMaterial, mFlags, 0x4);
WWHD_OFFSET(SpecularMaterial, mpDirtyBits, 0xC);

struct MaterialDefinition {
    u8 mPadding0[52];
    /* 0x034 */ be<u32> mTexMtxOffset;
};
WWHD_SIZE(MaterialDefinition, 0x38);
WWHD_OFFSET(MaterialDefinition, mTexMtxOffset, 0x34);

struct TextureMatrixInfo {
    u8 mPadding0[4];
    /* 0x004 */ be<s32> mIndex;
    u8 mPadding8[4];
    /* 0x00C */ be<u16> mAlternate;
    u8 mPadding14[6];
};
WWHD_SIZE(TextureMatrixInfo, 0x14);
WWHD_OFFSET(TextureMatrixInfo, mIndex, 0x4);
WWHD_OFFSET(TextureMatrixInfo, mAlternate, 0xC);

struct DepthManager {
    u8 mPadding0[4128];
    /* 0x1020 */ be<u32> mBufferCount;
    /* 0x1024 */ be<u32> mpBuffers;
};
WWHD_SIZE(DepthManager, 0x1028);
WWHD_OFFSET(DepthManager, mBufferCount, 0x1020);
WWHD_OFFSET(DepthManager, mpBuffers, 0x1024);

struct Renderer {
    u8 mPadding0[56];
    /* 0x038 */ be<f32> mViewMtx[12];
    u8 mPadding104[224];
    /* 0x148 */ be<u32> mpCamera;
    /* 0x14C */ be<u32> mProjectionMtx[16];
};
WWHD_SIZE(Renderer, 0x18C);
WWHD_OFFSET(Renderer, mViewMtx, 0x38);
WWHD_OFFSET(Renderer, mpCamera, 0x148);
WWHD_OFFSET(Renderer, mProjectionMtx, 0x14C);

struct Play {
    u8 mPadding0[4768];
    /* 0x12A0 */ be<u8> mBgS;
    u8 mPadding4769[19087];
    /* 0x5D30 */ be<u8> mDrawList;
    u8 mPadding23857[627];
    /* 0x5FA4 */ be<u32> mpView;
};
WWHD_SIZE(Play, 0x5FA8);
WWHD_OFFSET(Play, mBgS, 0x12A0);
WWHD_OFFSET(Play, mDrawList, 0x5D30);
WWHD_OFFSET(Play, mpView, 0x5FA4);

struct View {
    u8 mPadding0[204];
    /* 0x0CC */ be<f32> mNearZ;
    /* 0x0D0 */ be<f32> mFarZ;
    /* 0x0D4 */ be<f32> mFovy;
    /* 0x0D8 */ be<f32> mAspect;
};
WWHD_SIZE(View, 0xDC);
WWHD_OFFSET(View, mNearZ, 0xCC);
WWHD_OFFSET(View, mFarZ, 0xD0);
WWHD_OFFSET(View, mFovy, 0xD4);
WWHD_OFFSET(View, mAspect, 0xD8);

struct GlobalDestructor {
    /* 0x000 */ be<u32> mNext;
    /* 0x004 */ be<u32> mObject;
    /* 0x008 */ be<u32> mDestructor;
    /* 0x00C */ be<u32> mFlags;
};
WWHD_SIZE(GlobalDestructor, 0x10);
WWHD_OFFSET(GlobalDestructor, mNext, 0x0);
WWHD_OFFSET(GlobalDestructor, mObject, 0x4);
WWHD_OFFSET(GlobalDestructor, mDestructor, 0x8);
WWHD_OFFSET(GlobalDestructor, mFlags, 0xC);

struct DrawVector {
    /* 0x000 */ be<f32> x;
    /* 0x004 */ be<f32> y;
    /* 0x008 */ be<f32> z;
};
WWHD_SIZE(DrawVector, 0xC);
WWHD_OFFSET(DrawVector, x, 0x0);
WWHD_OFFSET(DrawVector, y, 0x4);
WWHD_OFFSET(DrawVector, z, 0x8);

struct DepthPair {
    /* 0x000 */ DrawVector mFirst;
    /* 0x00C */ DrawVector mSecond;
    /* 0x018 */ be<u8> mSecondIsNearer;
    u8 mPadding25[3];
};
WWHD_SIZE(DepthPair, 0x1C);
WWHD_OFFSET(DepthPair, mFirst, 0x0);
WWHD_OFFSET(DepthPair, mSecond, 0xC);
WWHD_OFFSET(DepthPair, mSecondIsNearer, 0x18);

constexpr u32 kDepthDiagnosticFormat = 0x1004CAB8;

struct MirrorFrame {
    /* 0x000 */ be<f32> mPoints[12];
    /* 0x030 */ be<f32> mDirection[3];
    /* 0x03C */ be<f32> mBounds[24];
    /* 0x09C */ be<f32> mViewEye[3];
    /* 0x0A8 */ be<f32> mViewMtx[12];
    /* 0x0D8 */ be<f32> mViewProjection[12];
    /* 0x108 */ be<f32> mFarPoints[12];
    /* 0x138 */ be<f32> mViewTarget[3];
    /* 0x144 */ be<f32> mWorldEye[3];
    /* 0x150 */ be<f32> mWorldTarget[3];
    /* 0x15C */ be<f32> mNearPoints[12];
    /* 0x18C */ be<f32> mTextureMtx[12];
    /* 0x1BC */ be<f32> mLightViewMtx[12];
    /* 0x1EC */ be<f32> mInverseViewMtx[12];
    /* 0x21C */ be<u32> mProjectionWords[16];
    /* 0x25C */ be<f32> mLightProjection[16];
    u8 mPadding668[4];
};
WWHD_SIZE(MirrorFrame, 0x2A0);
WWHD_OFFSET(MirrorFrame, mPoints, 0x0);
WWHD_OFFSET(MirrorFrame, mDirection, 0x30);
WWHD_OFFSET(MirrorFrame, mBounds, 0x3C);
WWHD_OFFSET(MirrorFrame, mViewEye, 0x9C);
WWHD_OFFSET(MirrorFrame, mViewMtx, 0xA8);
WWHD_OFFSET(MirrorFrame, mViewProjection, 0xD8);
WWHD_OFFSET(MirrorFrame, mFarPoints, 0x108);
WWHD_OFFSET(MirrorFrame, mViewTarget, 0x138);
WWHD_OFFSET(MirrorFrame, mWorldEye, 0x144);
WWHD_OFFSET(MirrorFrame, mWorldTarget, 0x150);
WWHD_OFFSET(MirrorFrame, mNearPoints, 0x15C);
WWHD_OFFSET(MirrorFrame, mTextureMtx, 0x18C);
WWHD_OFFSET(MirrorFrame, mLightViewMtx, 0x1BC);
WWHD_OFFSET(MirrorFrame, mInverseViewMtx, 0x1EC);
WWHD_OFFSET(MirrorFrame, mProjectionWords, 0x21C);
WWHD_OFFSET(MirrorFrame, mLightProjection, 0x25C);

struct DepthFrame {
    u8 mPadding0[8];
    /* 0x008 */ be<u32> mChannelWords[4];
    /* 0x018 */ be<u32> mFloatBits;
    /* 0x01C */ be<f32> mChannelValues[4];
    /* 0x02C */ BufferedText mText;
};
WWHD_SIZE(DepthFrame, 0x78);
WWHD_OFFSET(DepthFrame, mChannelWords, 0x8);
WWHD_OFFSET(DepthFrame, mFloatBits, 0x18);
WWHD_OFFSET(DepthFrame, mChannelValues, 0x1C);
WWHD_OFFSET(DepthFrame, mText, 0x2C);

struct PPCFormatFrame {
    u8 mPadding0[8];
    /* 0x008 */ be<u8> mGprCount;
    /* 0x009 */ be<u8> mFprCount;
    u8 mPadding10[2];
    /* 0x00C */ be<u32> mpOverflowArgs;
    /* 0x010 */ be<u32> mpRegisterSaveArea;
    u8 mPadding20[4];
    /* 0x018 */ be<u32> mGPR[8];
    /* 0x038 */ be<u32> mFPRWords[16];
    u8 mPadding120[8];
};
WWHD_SIZE(PPCFormatFrame, 0x80);
WWHD_OFFSET(PPCFormatFrame, mGprCount, 0x8);
WWHD_OFFSET(PPCFormatFrame, mFprCount, 0x9);
WWHD_OFFSET(PPCFormatFrame, mpOverflowArgs, 0xC);
WWHD_OFFSET(PPCFormatFrame, mpRegisterSaveArea, 0x10);
WWHD_OFFSET(PPCFormatFrame, mGPR, 0x18);
WWHD_OFFSET(PPCFormatFrame, mFPRWords, 0x38);
using TextureMember = be<u32> Texture::*;
enum DrawType : u32 { ZSort = 2, NonSort = 5 };
constexpr u32 kPacketDestructor = 0x0252F6E8;
constexpr u32 kTexturePacketConstructor = 0x0252F74C;
constexpr u32 kSmallPacketConstructor = 0x0252F7A8;
constexpr u32 kTexturePacketDestructor = 0x0252F7D8;
constexpr u32 kLineMatSortConstructor = 0x025EDC2C;
constexpr u32 kSafeStringVtable = 0x1004C8A0;
constexpr u32 kSafeStringBaseVtable = 0x1004C8B8;
constexpr u32 kBufferedStringVtable = 0x1004C8E0;
constexpr u32 kTextureResourceVtable = 0x1004C920;
constexpr u32 kFixedStringVtable = 0x1004C930;
constexpr u32 kLinChkBaseVtable = 0x1004C978;
constexpr u32 kGrpPassBaseVtable = 0x1004C988;
constexpr u32 kPolyPassBaseVtable = 0x1004C9C8;
constexpr u32 kPolyInfoVtable = 0x1004C9D8;
constexpr u32 kLinChkVtable = 0x1004C9E8;
constexpr u32 kGrpPassVtable = 0x1004C9F8;
constexpr u32 kPolyPassVtable = 0x1004CA08;
constexpr u32 kShadowPolyVtable = 0x1004CA18;
constexpr u32 kDrawBaseVtable = 0x1004CBA8;
constexpr u32 kGlobalDrawVtable = 0x1004CBC0;
constexpr u32 kEffectLineVtable = 0x1004CBF0;
constexpr u32 kMirrorPacketVtable = 0x1004CC08;
constexpr u32 kMirrorUniformDefinition = 0x1016EF84;
constexpr u32 kMirrorViewUniformDefinition = 0x1016EFB4;
constexpr u32 kDefaultWipeColor = 0x101D5E94;
constexpr u32 kSpecularBaseMtx = 0x101D610C;
constexpr u32 kSpecularEye = 0x101FFBA8;
constexpr u32 kSpecularUp = 0x101FFBC0;
constexpr u32 kJ3DSys = 0x104B45C0;
constexpr u32 kViewMtx = 0x104B45F8;
} // namespace dDlstHD
void drawHD_formattedTextEntry(void*, void*, u32, u32, u32, u32, u32, u32, f32, f32, f32);
u32 drawHD_TextureFormatChannelBits(u32, u32);
s32 drawHD_TextureFormatChannelType(u32, u32);
s32 drawHD_TextureFormatChannels(u32);
u32 drawHD_TextureFormatChannel(u32, u32);
u32 drawHD_TextureFormatBytes(u32);
void drawHD_ShaderParamCommit(void*, u32);
void drawHD_normalizeVector(void*);
void drawHD_PSMTXMultVec(void*, void*, void*);
void drawHD_PSMTXConcat(void*, void*, void*);
void drawHD_copyMirrorMatrixEntry(void*, void*);
void drawHD_PSMTXCopy(void*, void*);
void drawHD_mDoMTXConcatProjView(void*, void*, void*);
void drawHD_C_MTXLightOrtho(void*, f32, f32, f32, f32, f32, f32);
void drawHD_mDoMTXLookAt(void*, void*, void*, u32);
void drawHD_PSMTXInverse(void*, void*);
u32 drawHD_getPlay();
f64 drawHD_tan(f32);
void drawHD_ShaderParamSetCamera(void*, u32, void*, void*, void*);
void drawHD_VertexBufferFlush(void*, u32, u32);
u32 drawHD_dBgSLineCross(void*, void*);
void drawHD_dBgSLinChkSet(void*, void*, void*, u32);
void drawHD_dBgSLinChkConstruct(void*);
u32 drawHD_operatorNew(u32);
void drawHD_constructArray(void*, u32, u32, u32);
void drawHD_bzero(void*, u32);
void drawHD_UniformBlockConstruct(void*);
void drawHD_ShaderParamConstruct(void*);
void drawHD_TextureConstruct(void*);
void drawHD_ResourceTextureConstruct(void*);
void drawHD_J3DPacketConstruct(void*);
void drawHD_GX2DrawIndexedEx(u32, u32, u32, u32, u32, u32);
void drawHD_VertexArrayBind(void*);
u32 drawHD_TextureManagerGet(void*, u32);
void drawHD_TexturePrepare(void*);
void drawHD_GpuContextReset(void*);
void drawHD_GX2Invalidate(void*);
void drawHD_TextureFlush(void*);
void drawHD_TextureBind(void*, void*, u32, u32);
void drawHD_RenderStateApply(void*);
void drawHD_RenderStateConstruct(void*);
void drawHD_ShaderParamBindMaterial(void*, void*, u32);
void drawHD_ShapeBind(void*);
void drawHD_GX2CallDisplayList(void*, u32);
void drawHD_MaterialBind(void*);
void drawHD_ShapeClearDirty(void*, u32);
u32 drawHD_getRenderContext(void*);
void drawHD_TextureCopy(void*, void*);
u32 drawHD_MaterialGetTextureMatrix(void*, u32);
u32 drawHD_MaterialGetTexture(void*, u32, void*);
void drawHD_PSMTXScale(void*, f32, f32, f32);
void drawHD_C_MTXLookAt(void*, void*, void*, void*);
void drawHD_PSVECSubtract(void*, void*, void*);
void drawHD_transformLightPosition(void*, void*, void*);
void drawHD_positionToCamera(void*, void*, void*);
u32 drawHD_getCamera();
void drawHD_ShaderParamSetMode(void*, u32, u32);
void drawHD_MaterialSetVertexArray(void*, void*, void*, void*, u32);
void drawHD_VertexBufferSetMirrorVertices(void*, void*, u32, void*);
u32 drawHD_getAllocationHeap(void*);
void drawHD_MaterialInit(void*, void*, u32);
u32 drawHD_findMaterial(void*, void*);
u32 drawHD_getMirrorModel();
void drawHD_TextureSetResource(void*, void*);
u32 drawHD_getObjectResource(void*, void*, u32);
u32 drawHD_newDrawBuffer(u32, void*);
void drawHD_operatorDelete(void*);
void drawHD_destroyArray(void*, u32, u32, u32, u32, u32);
void drawHD_J3DDrawBufferDestruct(void*, u32);
void drawHD_BufferedTextFormat(void*, void*, void*);
void drawHD_J3DPacketDestruct(void*, u32);
void drawHD_TextureDestruct(void*, u32);
u32 drawHD_findAllocationHeap(void*, void*);
void drawHD_VertexBufferRelease(void*);
void drawHD_ShaderParamDestruct(void*, u32);
void drawHD_UniformBlockDestruct(void*, u32);
void drawHD_dBgSLinChkDestruct(void*, u32);
void drawHD_J3DDrawBufferEntryImm(void*, void*, u32);
f64 drawHD_PSMTXMultVecZ(void*, void*);
void drawHD_lineMatSortReset(void*);
void drawHD_J3DDrawBufferFrameInit(void*);
void drawHD_listSetEntry(void*, void*, void*, void*);
void drawHD_randomInitEntry(void*, u32, u32, u32);
void drawHD_drawBaseConstructEntry(void*);
void drawHD_randomConstructEntry(void*);
void drawHD_registerGlobal(void*);
void drawHD_constructArchiveByte(void*);
void drawHD_constructHeaderByte(void*);
void drawHD_operatorDeleteWithFlags(void*, u32);
void drawHD_VertexBufferDestruct(void*, u32);
void drawHD_VertexArrayDestruct(void*, u32);
void drawHD_VertexArrayConstruct(void*);
void drawHD_VertexBufferConstruct(void*);
void drawHD_wipeInEntry(void*, f32);
