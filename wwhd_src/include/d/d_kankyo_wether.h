// Weather controls and HD packet rendering.
#pragma once
#include "bindings.h"
#include "d/d_kankyo.h"

void weatherDestroySampler(u32, u32);
namespace d_kankyo_wether_cpp {
// HD: only the address of these external tables or text copies is consumed
// here.
struct WeatherOpaqueObject {
    u8 mPrefix;
};
WWHD_SIZE(WeatherOpaqueObject, 1);
struct WeatherStarPalette {
    GXColor mColors[8];
};
WWHD_SIZE(WeatherStarPalette, 32);
struct WeatherProjection {
    union {
        be<f32> values[16];
        be<u32> words[16];
    };
};
WWHD_SIZE(WeatherProjection, 64);

struct WeatherMatrix {
    gabi::be<float> values[12];
};
WWHD_SIZE(WeatherMatrix, 48);

struct WeatherRenderState {
    be<u8> mCullEnabled, mDepthEnabled;
    u8 padHead[2];
    be<u32> mBlendMode, mDepthCompare, mAlphaMode;
    u8 padBlend[0xD0];
    be<u8> mBlendEnabled;
    u8 padDepth[3];
    be<u32> mDepthFunction;
    be<f32> mDepthBias;
    be<u32> mBlendFactors;
    u8 padEnd[0x2C];
};
WWHD_OFFSET(WeatherRenderState, mBlendEnabled, 0xE0);
WWHD_OFFSET(WeatherRenderState, mDepthFunction, 0xE4);
WWHD_OFFSET(WeatherRenderState, mDepthBias, 0xE8);
WWHD_OFFSET(WeatherRenderState, mBlendFactors, 0xEC);
WWHD_SIZE(WeatherRenderState, 0x11C);

union WeatherFloatBits {
    be<f32> value;
    be<u32> bits;
};
WWHD_SIZE(WeatherFloatBits, 4);
// HD: vector assignments retain integer copies, including signaling NaNs.
struct WeatherVector {
    union {
        struct {
            be<f32> x, y, z;
        };
        be<u32> words[3];
        be<f32> channels[3];
    };
};
struct ColorFloat {
    union {
        struct {
            be<f32> red, green, blue, alpha;
        };
        be<f32> channels[4];
        be<u32> words[4];
    };
};
WWHD_SIZE(ColorFloat, 16);
struct WeatherColorBytes {
    be<u8> red, green, blue, alpha;
};
WWHD_SIZE(WeatherColorBytes, 4);
struct SafeString {
    be<u32> text, vtable;
};
WWHD_SIZE(SafeString, 8);
struct WeatherRoomVtable {
    u8 padFileList[0x1DC];
    be<u32> mpGetFileList;
};
WWHD_OFFSET(WeatherRoomVtable, mpGetFileList, 0x1DC);
WWHD_SIZE(WeatherRoomVtable, 0x1E0);
struct WeatherRoomStatus {
    be<u32> mpVtable;
};
WWHD_SIZE(WeatherRoomStatus, 4);
struct WeatherRoomFileList {
    be<u32> mFlags;
};
WWHD_SIZE(WeatherRoomFileList, 4);
struct WeatherTextureResource {
    u8 padTexture[0x20];
    be<u32> mpTexture;
};
WWHD_OFFSET(WeatherTextureResource, mpTexture, 0x20);
WWHD_SIZE(WeatherTextureResource, 0x24);
struct WeatherWindlineEffect {
    be<u32> mStatus;
    u8 padActive[0x20];
    be<u32> mActive;
    u8 padEnd[0xC];
};
WWHD_OFFSET(WeatherWindlineEffect, mActive, 0x24);
WWHD_SIZE(WeatherWindlineEffect, 0x34);
struct WeatherWindlinePacket {
    WeatherWindlineEffect mEffects[30];
};
WWHD_SIZE(WeatherWindlinePacket, 0x618);
struct WeatherSave {
    u8 padWind[0x4A];
    be<u16> mWindX, mWindY;
};
using WeatherSaveLink = gptr<WeatherSave>;
struct WeatherSinCos {
    WeatherFloatBits sine, cosine;
};
WWHD_SIZE(WeatherSinCos, 8);
WWHD_OFFSET(WeatherSinCos, cosine, 4);
struct WindDirectionRange {
    be<u16> min_angle, max_angle;
    be<s32> direction;
};
WWHD_SIZE(WeatherVector, 12);
WWHD_OFFSET(WeatherVector, y, 4);
WWHD_OFFSET(WeatherVector, z, 8);
WWHD_SIZE(WeatherSave, 0x4E);
WWHD_OFFSET(WeatherSave, mWindX, 0x4A);
WWHD_OFFSET(WeatherSave, mWindY, 0x4C);
struct WindDirectionTable {
    WindDirectionRange mRanges[9];
};
struct WeatherSinCosTable {
    WeatherSinCos mEntries[8192];
};
WWHD_SIZE(WindDirectionTable, 72);
WWHD_SIZE(WeatherSinCosTable, 65536);
WWHD_SIZE(WindDirectionRange, 8);
WWHD_OFFSET(WindDirectionRange, direction, 4);
} // namespace d_kankyo_wether_cpp

// HD: the renderer and archive entry points used by this translation unit.
void weatherAddCalc(u32, f32, f32, f32, f32);
s32 weatherAtan2Signed(f32, f32);
u32 weatherAtan2Register(f32, f32);
void weatherAddVectors(u32, u32, u32);
void weatherScaleVector(u32, u32, f32);
u32 weatherVectorNotEqual(u32, u32);
void weatherNormalizeVector(u32, u32);
u32 weatherGetPlayRegister();
void weatherGetPlayDiscard();
u32 weatherGetStageResource(u32, u32);
void weatherdDlst_list_c__entryZSortXluDrawList(u32, u32, u32, u32);
u32 weatherGetEnvironment();
f32 weatherGetDaytime(u32);
void weatherdKyr_get_vectle_calc(u32, u32, u32);
void weatherdKyr_wind_init();
void weatherdKyr_wind_move();
void weatherdKyr_lenzflare_move();
void weatherdKyr_sun_move();
void weatherdKyr_rain_init();
void weatherdKyr_rain_move();
void weatherdKyr_housi_move();
void weatherCall_dKyr_snow_init();
void weatherdKyr_snow_move();
void weatherdKyr_kazanbai_move();
void weatherdKyr_star_init();
void weatherCall_dKyr_star_move();
void weatherpoison_move();
void weatherpoison_init();
void weathervrkumo_move();
void weatherdKyr_drawSun(u32, u32, u32);
void weatherdKyr_drawLenzflare(u32, u32, u32);
void weatherdKyr_drawRain(u32, u32);
void weatherdKyr_drawSibuki(u32, u32);
void weatherCall_dKyr_thunder_init();
void weatherdKyr_thunder_move();
void weatherCall_dKyw_setDrawPacketList(u32, u32);
void weatherCall_dKyw_setDrawPacketListSky(u32, u32);
void weatherCall_dKyw_drawSun(u32);
void weatherCall_dKyw_drawSunlenz(u32);
void weatherCall_dKyw_drawRain(u32);
void weatherCall_dKyw_drawSnow(u32);
void weatherCall_dKyw_drawStar(u32);
void weatherCall_dKyw_drawPoison(u32);
void weatherCall_dKyw_drawHousi(u32);
void weatherCall_dKyw_drawCloud(u32);
void weatherCall_dKyw_drawVrkumo(u32);
void weatherCall_dKyw_drawWave(u32);
void weatherCall_dKyw_wind_init();
void weatherDeleteRainPacket(u32);
void weatherDeleteStarPacket(u32);
void weatherDeleteHousiPacket(u32);
void weatherDeleteCloudPacket(u32);
void weatherDeleteWavePacket(u32);
f32 weatherdKyw_get_wind_pow();
u32 weatherdKyw_wether_init(u32);
u32 weatherConstructSunlenzPacket(u32);
void weatherInitializeSunGeometry(u32, u32);
void weatherInitializeSunlenzGeometry(u32);
u32 weatherConstructRainPacket(u32);
void weatherNormalizeColor(u32, u32);
void weatherInitializeStarGeometry(u32);
u32 weatherConstructHousiPacket(u32);
void weatherInitializeHousiGeometry(u32);
void weatherInitializeVrkumoGeometry(u32);
u32 weatherConstructVrkumoPacket(u32);
void weatherRefreshVrkumoTextures(u32);
void weatherdKyw_pntwind_cut(u32);
u32 weatherdKyw_get_wind_vec();
void weatherpntwind_set(u32);
void weatherdKyw_pntwind_set(u32);
void weatherdKyw_pntwind_get_info(u32, u32, u32);
void weatherCopyMatrix(u32, u32);
u32 weatherConstructSafeString(u32, u32);
u32 weatherSafeStringEqual(u32, u32);
void weatherDestroySafeString(u32, u32);
void weatherwether_move_moya();
void weatherwether_move_wave();
u32 weatherdStage_roomControl_c__getStatusRoomDtRegister(u32, s32);
u32 weatherdStage_roomControl_c__getStatusRoomDtRegister78(u32, u32);
void weatherStartSound(u32);
void weatherPlayRainSound(u32);
void weatherDelete(u32);
void weatherBeginHeap(u32, u32, u32, u32);
void weatherEndHeap(u32, u32);
void weatherAbortHeap(u32);
u32 weatherGetArchiveResource(u32, u32, u32, u32);
u32 weatherAllocateRegister(u32);
void weatherAllocateDiscard(u32);
u32 weatherAllocateAligned(u32, u32);
void weatherFree(u32);
void weatherLockTextures(u32);
void weatherUnlockTextures(u32);
void weatherConstructRenderState(u32);
void weatherApplyRenderState(u32);
u32 weatherAllocateVertices(u32, u32);
u32 weatherGetAllocator(u32, u32);
void weatherImportTexture(u32, u32);
void weatherConstructVertexBuffer(u32);
void weatherDestroyVertexBuffer(u32, u32);
void weatherUploadVertices(u32, u32, u32);
u32 weatherFindMaterial(u32, u32);
void weatherBindProgram(u32);
void weatherUploadShader(u32);
void weatherPrepareShader(u32, u32);
void weatherCopyTextureDescriptor(u32, u32);
void weatherConstructTextureDescriptor(u32);
void weatherConstructTextureBinding(u32);
void weatherInitializeTextureBinding(u32);
void weatherDestroyTextureBinding(u32, u32);
void weatherBindGeometryMaterial(u32);
u32 weatherGetArchiveTexture(u32, u32);
u32 weatherGetArchive(u32);
void weatherJ3DDrawBuffer__entryImm(u32, u32, u32);
void weatherConstructPacket(u32);
void weatherDestroyPacket(u32, u32);
u32 weatherGetShaderCache(u32);
void weatherConstructTexture(u32);
void weatherDestroyTexture(u32, u32);
void weatherSetTextureMode(u32, u32);
void weatherConstructRenderer(u32);
void weatherDestroyRenderer(u32, u32);
void weatherSetRendererMode(u32, u32, u32);
void weatherSetMatrices(u32, u32, u32, u32, u32);
void weatherInitializeRenderer(u32, u32, u32);
void weatherPrepareRenderer(u32, u32);
void weatherBindRenderer(u32, u32, u32);
void weatherConstructTextureSet(u32);
void weatherInitializeTextureSet(u32);
void weatherSetTexture(u32, u32, u32, u32);
void weatherBindTextureSet(u32, u32);
u32 weatherGetTexture(u32, u32);
void weatherInitializeVertices(u32, u32, u32, u32);
void weatherInitializeGeometryMaterial(u32, u32, u32, u32, u32);
u32 weatherGetMaterialState(u32);
void weatherInitializeMaterial(u32, u32, u32);
void weatherJPABaseEmitter__deleteAllParticle(u32, u32);
f32 weatherVectorSquaredLength(u32);
f32 weatherVectorSquaredDistance(u32, u32);
void weatherConstructArray(u32, u32, u32, u32);
void weatherDestroyArray(u32, u32, u32, u32, u32, u32);
f32 weatherSqrt(f32);
void weatherClear(u32, u32);
void weatherBindShader(u32, u32);
void weatherDrawIndexed(u32, u32, u32, u32, u32, u32);
void weatherSetGeometryUniforms(u32, u32, u32);
void weatherSetPixelUniforms(u32, u32, u32);
void weatherSetVertexUniforms(u32, u32, u32);

// HD: packets add renderer, material and double-buffered geometry storage.
namespace d_kankyo_wether_cpp {
struct WeatherPacketBase {
    u8 pad__vtbl[0xC];
    /* 0x000C */ be<u32> __vtbl;
    u8 padEnd[0x88];
};
WWHD_OFFSET(WeatherPacketBase, __vtbl, 0xC);
WWHD_SIZE(WeatherPacketBase, 0x98);
struct WeatherVertexBuffer {
    u8 mState[0x14C];
    be<u32> mUploadSize;
    u8 padEnd[4];
};
WWHD_OFFSET(WeatherVertexBuffer, mUploadSize, 0x14C);
WWHD_SIZE(WeatherVertexBuffer, 0x154);
struct WeatherGeometry {
    /* 0x0000 */ be<u32> mpVertices;
    /* 0x0004 */ WeatherVertexBuffer mVertexBuffer;
    /* 0x0158 */ u8 mMaterialBinding[0xF4];
    /* 0x024C */ be<u32> mVertexCount;
    /* 0x0250 */ be<u32> mpAllocation;
};
WWHD_OFFSET(WeatherGeometry, mpVertices, 0x0);
WWHD_OFFSET(WeatherGeometry, mVertexBuffer, 0x4);
WWHD_OFFSET(WeatherGeometry, mMaterialBinding, 0x158);
WWHD_OFFSET(WeatherGeometry, mVertexCount, 0x24C);
WWHD_OFFSET(WeatherGeometry, mpAllocation, 0x250);
WWHD_SIZE(WeatherGeometry, 0x254);
struct WeatherStarGeometry {
    WeatherGeometry mGeometry[4];
    be<u32> mBufferIndex, mPrimitive, mAttributeCount, mpAttributes, mpMaterial;
    be<u8> mbInitialized;
    u8 padEnd[3];
};
WWHD_OFFSET(WeatherStarGeometry, mPrimitive, 0x954);
WWHD_OFFSET(WeatherStarGeometry, mpMaterial, 0x960);
WWHD_OFFSET(WeatherStarGeometry, mbInitialized, 0x964);
WWHD_SIZE(WeatherStarGeometry, 0x968);
struct WeatherVrkumoGeometry {
    WeatherGeometry mGeometry[600];
    be<u32> mBufferIndex, mPrimitive, mAttributeCount, mpAttributes, mpMaterial;
    be<u8> mbInitialized;
    u8 padEnd[3];
};
WWHD_OFFSET(WeatherVrkumoGeometry, mPrimitive, 0x574E4);
WWHD_OFFSET(WeatherVrkumoGeometry, mpMaterial, 0x574F0);
WWHD_OFFSET(WeatherVrkumoGeometry, mbInitialized, 0x574F4);
WWHD_SIZE(WeatherVrkumoGeometry, 0x574F8);
struct WeatherGeometryPair {
    /* 0x0000 */ WeatherGeometry mGeometry[2];
    /* 0x04A8 */ be<u32> mBufferIndex;
    /* 0x04AC */ be<u32> mPrimitive;
    /* 0x04B0 */ be<u32> mAttributeCount;
    /* 0x04B4 */ be<u32> mpAttributes;
    /* 0x04B8 */ be<u32> mpMaterial;
    /* 0x04BC */ be<u8> mbInitialized;
    u8 padEnd[0x3];
};
WWHD_OFFSET(WeatherGeometryPair, mGeometry, 0x0);
WWHD_OFFSET(WeatherGeometryPair, mBufferIndex, 0x4A8);
WWHD_OFFSET(WeatherGeometryPair, mPrimitive, 0x4AC);
WWHD_OFFSET(WeatherGeometryPair, mAttributeCount, 0x4B0);
WWHD_OFFSET(WeatherGeometryPair, mpAttributes, 0x4B4);
WWHD_OFFSET(WeatherGeometryPair, mpMaterial, 0x4B8);
WWHD_OFFSET(WeatherGeometryPair, mbInitialized, 0x4BC);
WWHD_SIZE(WeatherGeometryPair, 0x4C0);
struct WeatherTextureSmall {
    u8 mState[0xC];
    be<u32> __vtbl;
    u8 mTextureState[0x64];
    u8 mParameters[0x34];
};
WWHD_OFFSET(WeatherTextureSmall, __vtbl, 0xC);
WWHD_OFFSET(WeatherTextureSmall, mParameters, 0x74);
WWHD_SIZE(WeatherTextureSmall, 0xA8);
struct WeatherRendererPrefix {
    be<u32> mState;
    be<u32> mpInfo;
    be<u32> mFlags;
    WeatherTextureSmall mTexture;
};
WWHD_OFFSET(WeatherRendererPrefix, mpInfo, 4);
WWHD_OFFSET(WeatherRendererPrefix, mTexture, 0xC);
WWHD_SIZE(WeatherRendererPrefix, 0xB4);
struct WeatherTextureParameters {
    union {
        be<f32> mValues[44];
        struct {
            u8 padColors[0x40];
            ColorFloat mPrimaryColor, mSecondaryColor;
            u8 padValues[0x50];
        };
    };
    union {
        u8 mArrayStorage[0x240];
        struct {
            ColorFloat mColorPairs[3][2];
            u8 mMatrixStorage[8][0x30];
            ColorFloat mVectorStorage[6];
        };
    };
};
WWHD_OFFSET(WeatherTextureParameters, mColorPairs, 0xB0);
WWHD_OFFSET(WeatherTextureParameters, mMatrixStorage, 0x110);
WWHD_OFFSET(WeatherTextureParameters, mVectorStorage, 0x290);
WWHD_OFFSET(WeatherTextureParameters, mPrimaryColor, 0x40);
WWHD_OFFSET(WeatherTextureParameters, mSecondaryColor, 0x50);
WWHD_SIZE(WeatherTextureParameters, 0x2F0);
struct WeatherTextureExtended {
    u8 mState[0xC];
    be<u32> __vtbl;
    u8 mTextureState[0x64];
    WeatherTextureParameters mParameters;
};
WWHD_OFFSET(WeatherTextureExtended, __vtbl, 0xC);
WWHD_OFFSET(WeatherTextureExtended, mParameters, 0x74);
WWHD_SIZE(WeatherTextureExtended, 0x364);
struct WeatherRenderer {
    WeatherRendererPrefix mPrefix;
    WeatherTextureExtended mExtendedTexture;
    be<u8> mStatus;
    u8 padEnd[3];
};
WWHD_OFFSET(WeatherRenderer, mPrefix, 0);
WWHD_OFFSET(WeatherRenderer, mExtendedTexture, 0xB4);
WWHD_OFFSET(WeatherRenderer, mStatus, 0x418);
WWHD_SIZE(WeatherRenderer, 0x41C);
struct WeatherTexture {
    u8 pad__vtbl[0xC];
    /* 0x000C */ be<u32> __vtbl;
    u8 padmParameters[0x64];
    /* 0x0074 */ WeatherTextureParameters mParameters;
    u8 padmBufferIndex[0x4];
    /* 0x0368 */ be<u32> mBufferIndex;
    /* 0x036C */ be<u32> mFlags;
};
WWHD_OFFSET(WeatherTexture, __vtbl, 0xC);
WWHD_OFFSET(WeatherTexture, mParameters, 0x74);
WWHD_OFFSET(WeatherTexture, mBufferIndex, 0x368);
WWHD_OFFSET(WeatherTexture, mFlags, 0x36C);
WWHD_SIZE(WeatherTexture, 0x370);
struct WeatherTextureSet {
    /* 0x0000 */ be<u32> mState;
    /* 0x0004 */ be<u32> mpInfo;
    u8 padEnd[0x44];
};
WWHD_OFFSET(WeatherTextureSet, mState, 0x0);
WWHD_OFFSET(WeatherTextureSet, mpInfo, 0x4);
WWHD_SIZE(WeatherTextureSet, 0x4C);
struct WeatherCloudGeometry {
    /* 0x0000 */ WeatherGeometry mGeometry[32];
    /* 0x4A80 */ be<u32> mBufferIndex;
    /* 0x4A84 */ be<u32> mPrimitive;
    /* 0x4A88 */ be<u32> mAttributeCount;
    /* 0x4A8C */ be<u32> mpAttributes;
    /* 0x4A90 */ be<u32> mpMaterial;
    /* 0x4A94 */ be<u8> mbInitialized;
    u8 padEnd[0x3];
};
WWHD_OFFSET(WeatherCloudGeometry, mGeometry, 0x0);
WWHD_OFFSET(WeatherCloudGeometry, mBufferIndex, 0x4A80);
WWHD_OFFSET(WeatherCloudGeometry, mPrimitive, 0x4A84);
WWHD_OFFSET(WeatherCloudGeometry, mAttributeCount, 0x4A88);
WWHD_OFFSET(WeatherCloudGeometry, mpAttributes, 0x4A8C);
WWHD_OFFSET(WeatherCloudGeometry, mpMaterial, 0x4A90);
WWHD_OFFSET(WeatherCloudGeometry, mbInitialized, 0x4A94);
WWHD_SIZE(WeatherCloudGeometry, 0x4A98);
struct dKankyo_sun_Packet {
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void draw();
    void destroy(u32 flags);
    void initializeGeometry(u32 alternateTexture);
    void setColors(u32 index, u32 colorA, u32 colorB);
    void render(u32 firstTexture, u32 enabled, u32 allRows);
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ WeatherVector mPos[2];
    /* 0x00B0 */ be<u32> mVizChkData[5];
    /* 0x00C4 */ be<u8> mOcclusionDelayHD;
    /* 0x00C5 */ be<u8> mHasVisiblePointHD;
    u8 padmVisibility[0x2];
    /* 0x00C8 */ be<f32> mVisibility;
    /* 0x00CC */ be<f32> mSunAlpha;
    /* 0x00D0 */ be<f32> mMoonAlpha;
    /* 0x00D4 */ be<u32> mpTextureData[5];
    /* 0x00E8 */ be<u32> mFullyVisibleDelayHD;
    /* 0x00EC */ be<u32> mpMaterial;
    /* 0x00F0 */ WeatherGeometryPair mGeometry[4];
    /* 0x13F0 */ WeatherRenderer mRenderer[4];
    /* 0x2460 */ WeatherTextureSet mTextureSet[6];
};
WWHD_OFFSET(dKankyo_sun_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_sun_Packet, mPos, 0x98);
WWHD_OFFSET(dKankyo_sun_Packet, mVizChkData, 0xB0);
WWHD_OFFSET(dKankyo_sun_Packet, mOcclusionDelayHD, 0xC4);
WWHD_OFFSET(dKankyo_sun_Packet, mHasVisiblePointHD, 0xC5);
WWHD_OFFSET(dKankyo_sun_Packet, mVisibility, 0xC8);
WWHD_OFFSET(dKankyo_sun_Packet, mSunAlpha, 0xCC);
WWHD_OFFSET(dKankyo_sun_Packet, mMoonAlpha, 0xD0);
WWHD_OFFSET(dKankyo_sun_Packet, mpTextureData, 0xD4);
WWHD_OFFSET(dKankyo_sun_Packet, mFullyVisibleDelayHD, 0xE8);
WWHD_OFFSET(dKankyo_sun_Packet, mpMaterial, 0xEC);
WWHD_OFFSET(dKankyo_sun_Packet, mGeometry, 0xF0);
WWHD_OFFSET(dKankyo_sun_Packet, mRenderer, 0x13F0);
WWHD_OFFSET(dKankyo_sun_Packet, mTextureSet, 0x2460);
WWHD_SIZE(dKankyo_sun_Packet, 0x2628);
struct dKankyo_sunlenz_Packet {
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void draw();
    void destroy(u32 flags);
    void initializeGeometry();
    void render();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTextures[3];
    /* 0x00A4 */ WeatherVector mPositions[8];
    u8 padmPositionScale[0xC];
    /* 0x0110 */ be<f32> mPositionScale;
    /* 0x0114 */ be<f32> mPositionOffset;
    /* 0x0118 */ be<f32> mDepthLimit;
    /* 0x011C */ be<f32> mVisibility;
    /* 0x0120 */ be<f32> mAngleDeg;
    /* 0x0124 */ be<f32> mDistFalloff;
    /* 0x0128 */ be<s16> mRotation;
    /* 0x012A */ be<u8> mbDrawLenzInSky;
    u8 padmpMaterial[0x1];
    /* 0x012C */ be<u32> mpMaterial;
    /* 0x0130 */ WeatherGeometryPair mGeometry[9];
    /* 0x2BF0 */ WeatherRenderer mRenderer[9];
    /* 0x50EC */ WeatherTextureSet mTextureSet[3];
    /* 0x51D0 */ be<u32> mpCloudMaterial;
    /* 0x51D4 */ WeatherCloudGeometry mCloudGeometry;
    /* 0x9C6C */ WeatherRenderer mCloudRenderer;
};
WWHD_OFFSET(dKankyo_sunlenz_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mpTextures, 0x98);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mPositions, 0xA4);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mPositionScale, 0x110);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mPositionOffset, 0x114);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mDepthLimit, 0x118);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mVisibility, 0x11C);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mAngleDeg, 0x120);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mDistFalloff, 0x124);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mRotation, 0x128);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mbDrawLenzInSky, 0x12A);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mpMaterial, 0x12C);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mGeometry, 0x130);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mRenderer, 0x2BF0);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mTextureSet, 0x50EC);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mpCloudMaterial, 0x51D0);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mCloudGeometry, 0x51D4);
WWHD_OFFSET(dKankyo_sunlenz_Packet, mCloudRenderer, 0x9C6C);
WWHD_SIZE(dKankyo_sunlenz_Packet, 0xA088);
struct RAIN_EFF {
    /* 0x0000 */ be<s8> mStatus;
    u8 padmPos[0x3];
    /* 0x0004 */ WeatherVector mPos;
    /* 0x0010 */ WeatherVector mBasePos;
    /* 0x001C */ be<f32> mSpeed;
    /* 0x0020 */ be<f32> mWindScale;
    /* 0x0024 */ be<f32> mGravity;
    /* 0x0028 */ be<f32> mAlpha;
    /* 0x002C */ be<f32> mSplashRadius;
    /* 0x0030 */ be<f32> mSplashHeight;
    /* 0x0034 */ be<s16> mTimer;
    u8 padEnd[0x2];
};
WWHD_OFFSET(RAIN_EFF, mStatus, 0x0);
WWHD_OFFSET(RAIN_EFF, mPos, 0x4);
WWHD_OFFSET(RAIN_EFF, mBasePos, 0x10);
WWHD_OFFSET(RAIN_EFF, mSpeed, 0x1C);
WWHD_OFFSET(RAIN_EFF, mWindScale, 0x20);
WWHD_OFFSET(RAIN_EFF, mGravity, 0x24);
WWHD_OFFSET(RAIN_EFF, mAlpha, 0x28);
WWHD_OFFSET(RAIN_EFF, mSplashRadius, 0x2C);
WWHD_OFFSET(RAIN_EFF, mSplashHeight, 0x30);
WWHD_OFFSET(RAIN_EFF, mTimer, 0x34);
WWHD_SIZE(RAIN_EFF, 0x38);
struct dKankyo_rain_Packet {
    void renderStreaks();
    void renderSplashes();
    void initializeGeometry();
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void draw();
    void destroy(u32 flags);
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTextures[2];
    /* 0x00A0 */ RAIN_EFF mEff[250];
    /* 0x3750 */ be<f32> mWindSpeed;
    /* 0x3754 */ be<f32> mWindScale;
    /* 0x3758 */ be<s16> mRainCount;
    u8 padmFrame[0x2];
    /* 0x375C */ be<u32> mFrame;
    /* 0x3760 */ WeatherVector mCamEyePos;
    /* 0x376C */ WeatherVector mCamCenterPos;
    /* 0x3778 */ WeatherVector mCenterDelta;
    /* 0x3784 */ be<f32> mCenterDeltaMul;
    /* 0x3788 */ be<f32> mSplashSpeed;
    /* 0x378C */ be<f32> mSibukiAlpha;
    /* 0x3790 */ be<f32> mOverheadFade;
    /* 0x3794 */ be<f32> mFwdFade1;
    /* 0x3798 */ be<f32> mFwdFade2;
    /* 0x379C */ be<u8> mStatus;
    /* 0x379D */ be<u8> mSplashActive[250];
    u8 padmpMaterial[0x1];
    /* 0x3898 */ be<u32> mpMaterial;
    /* 0x389C */ WeatherGeometryPair mSplashGeometry[1000];
    /* 0x12C69C */ WeatherGeometryPair mRainGeometry[250];
    u8 padmSplashTextures[0x4];
    /* 0x176A20 */ WeatherTexture mSplashTextures[250];
    /* 0x1AC580 */ WeatherTexture mRainTexture;
    /* 0x1AC8F0 */ WeatherRendererPrefix mRenderer;
    /* 0x1AC9A4 */ WeatherTextureSet mTextureSet[2];
    u8 padEnd[0x4];
};
WWHD_OFFSET(dKankyo_rain_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_rain_Packet, mpTextures, 0x98);
WWHD_OFFSET(dKankyo_rain_Packet, mEff, 0xA0);
WWHD_OFFSET(dKankyo_rain_Packet, mWindSpeed, 0x3750);
WWHD_OFFSET(dKankyo_rain_Packet, mWindScale, 0x3754);
WWHD_OFFSET(dKankyo_rain_Packet, mRainCount, 0x3758);
WWHD_OFFSET(dKankyo_rain_Packet, mFrame, 0x375C);
WWHD_OFFSET(dKankyo_rain_Packet, mCamEyePos, 0x3760);
WWHD_OFFSET(dKankyo_rain_Packet, mCamCenterPos, 0x376C);
WWHD_OFFSET(dKankyo_rain_Packet, mCenterDelta, 0x3778);
WWHD_OFFSET(dKankyo_rain_Packet, mCenterDeltaMul, 0x3784);
WWHD_OFFSET(dKankyo_rain_Packet, mSplashSpeed, 0x3788);
WWHD_OFFSET(dKankyo_rain_Packet, mSibukiAlpha, 0x378C);
WWHD_OFFSET(dKankyo_rain_Packet, mOverheadFade, 0x3790);
WWHD_OFFSET(dKankyo_rain_Packet, mFwdFade1, 0x3794);
WWHD_OFFSET(dKankyo_rain_Packet, mFwdFade2, 0x3798);
WWHD_OFFSET(dKankyo_rain_Packet, mStatus, 0x379C);
WWHD_OFFSET(dKankyo_rain_Packet, mSplashActive, 0x379D);
WWHD_OFFSET(dKankyo_rain_Packet, mpMaterial, 0x3898);
WWHD_OFFSET(dKankyo_rain_Packet, mSplashGeometry, 0x389C);
WWHD_OFFSET(dKankyo_rain_Packet, mRainGeometry, 0x12C69C);
WWHD_OFFSET(dKankyo_rain_Packet, mSplashTextures, 0x176A20);
WWHD_OFFSET(dKankyo_rain_Packet, mRainTexture, 0x1AC580);
WWHD_OFFSET(dKankyo_rain_Packet, mRenderer, 0x1AC8F0);
WWHD_OFFSET(dKankyo_rain_Packet, mTextureSet, 0x1AC9A4);
WWHD_SIZE(dKankyo_rain_Packet, 0x1ACA40);
struct CLOUD_EFF {
    be<s8> mStatus;
    u8 padState[0x4B];
};
struct WAVE_EFF {
    WeatherVector mPos, mBasePos;
    be<f32> mSpeed, mScale, mCounterSpeed, mCounter, mAlpha, mStrengthEnv;
    be<s16> field_0x30, field_0x32;
    be<s8> mStatus;
    u8 padEnd[3];
};
WWHD_OFFSET(CLOUD_EFF, mStatus, 0);
WWHD_SIZE(CLOUD_EFF, 0x4C);
WWHD_OFFSET(WAVE_EFF, mPos, 0);
WWHD_OFFSET(WAVE_EFF, mStatus, 0x34);
WWHD_SIZE(WAVE_EFF, 0x38);
struct dKankyo_cloud_Packet {
    void draw();
    void render(s32 count);
    void destroy(u32 flags);
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void initializeGeometry();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTexture;
    /* 0x009C */ be<u32> mCount;
    /* 0x00A0 */ CLOUD_EFF mEff[30];
    /* 0x0988 */ be<s16> mActiveCount;
    u8 padmAlpha[0x2];
    /* 0x098C */ be<f32> mAlpha;
    /* 0x0990 */ be<u32> mpMaterial;
    /* 0x0994 */ WeatherGeometryPair mGeometry[30];
    u8 padmTextures[0x4];
    /* 0x9818 */ WeatherTexture mTextures[30];
    /* 0xFF38 */ WeatherRendererPrefix mRenderer;
    /* 0xFFEC */ WeatherTextureSet mTextureSet;
};
WWHD_OFFSET(dKankyo_cloud_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_cloud_Packet, mpTexture, 0x98);
WWHD_OFFSET(dKankyo_cloud_Packet, mCount, 0x9C);
WWHD_OFFSET(dKankyo_cloud_Packet, mEff, 0xA0);
WWHD_OFFSET(dKankyo_cloud_Packet, mActiveCount, 0x988);
WWHD_OFFSET(dKankyo_cloud_Packet, mAlpha, 0x98C);
WWHD_OFFSET(dKankyo_cloud_Packet, mpMaterial, 0x990);
WWHD_OFFSET(dKankyo_cloud_Packet, mGeometry, 0x994);
WWHD_OFFSET(dKankyo_cloud_Packet, mTextures, 0x9818);
WWHD_OFFSET(dKankyo_cloud_Packet, mRenderer, 0xFF38);
WWHD_OFFSET(dKankyo_cloud_Packet, mTextureSet, 0xFFEC);
WWHD_SIZE(dKankyo_cloud_Packet, 0x10038);
struct dKankyo_housi_Packet {
    void draw();
    void render();
    void destroy(u32 flags);
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void initializeGeometry();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTex;
    /* 0x009C */ u8 mEffect[300][0x50];
    /* 0x5E5C */ be<f32> mWindSpeed;
    /* 0x5E60 */ be<f32> mWindScale;
    /* 0x5E64 */ be<f32> mAlpha;
    /* 0x5E68 */ be<s16> mCount;
    u8 padmFrame[0x2];
    /* 0x5E6C */ be<u32> mFrame;
    /* 0x5E70 */ be<u8> mActive[600];
    /* 0x60C8 */ be<u32> mpMaterial;
    /* 0x60CC */ WeatherGeometryPair mGeometry[600];
    u8 padmTextures[0x4];
    /* 0xB82D0 */ WeatherTexture mTextures[600];
    /* 0x139150 */ WeatherRendererPrefix mRenderer;
    /* 0x139204 */ WeatherTextureSet mTextureSet;
};
WWHD_OFFSET(dKankyo_housi_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_housi_Packet, mpTex, 0x98);
WWHD_OFFSET(dKankyo_housi_Packet, mEffect, 0x9C);
WWHD_OFFSET(dKankyo_housi_Packet, mWindSpeed, 0x5E5C);
WWHD_OFFSET(dKankyo_housi_Packet, mWindScale, 0x5E60);
WWHD_OFFSET(dKankyo_housi_Packet, mAlpha, 0x5E64);
WWHD_OFFSET(dKankyo_housi_Packet, mCount, 0x5E68);
WWHD_OFFSET(dKankyo_housi_Packet, mFrame, 0x5E6C);
WWHD_OFFSET(dKankyo_housi_Packet, mActive, 0x5E70);
WWHD_OFFSET(dKankyo_housi_Packet, mpMaterial, 0x60C8);
WWHD_OFFSET(dKankyo_housi_Packet, mGeometry, 0x60CC);
WWHD_OFFSET(dKankyo_housi_Packet, mTextures, 0xB82D0);
WWHD_OFFSET(dKankyo_housi_Packet, mRenderer, 0x139150);
WWHD_OFFSET(dKankyo_housi_Packet, mTextureSet, 0x139204);
WWHD_SIZE(dKankyo_housi_Packet, 0x139250);
struct dKankyo_vrkumo_Packet {
    void draw();
    void destroy(u32 flags);
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void initializeGeometry();
    void refreshTextures();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpCloudTextures[3];
    /* 0x00A4 */ u8 mInst[100][0x2C];
    /* 0x11D4 */ be<f32> mCloudPhase;
    /* 0x11D8 */ be<u32> mpMaterial;
    /* 0x11DC */ WeatherVrkumoGeometry mGeometry[2];
    /* 0xAFBCC */ WeatherRenderer mRenderer;
    /* 0xAFFE8 */ WeatherTexture mTextures[100];
    /* 0xC57A8 */ u8 mTextureDescriptors[3][0x90];
    /* 0xC5958 */ u8 mTextureBindings[3][0x198];
};
WWHD_OFFSET(dKankyo_vrkumo_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mpCloudTextures, 0x98);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mInst, 0xA4);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mCloudPhase, 0x11D4);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mpMaterial, 0x11D8);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mGeometry, 0x11DC);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mRenderer, 0xAFBCC);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mTextures, 0xAFFE8);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mTextureDescriptors, 0xC57A8);
WWHD_OFFSET(dKankyo_vrkumo_Packet, mTextureBindings, 0xC5958);
WWHD_SIZE(dKankyo_vrkumo_Packet, 0xC5E20);
struct dKankyo_wave_Packet {
    void setColors(u32 index, u32 colorA, u32 colorB);
    void render(u32 context);
    void draw(u32 context);
    void destroy(u32 flags);
    // HD: renderer methods preserve the original packet ABI.
    u32 construct();
    void initializeGeometry();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTextures[2];
    /* 0x00A0 */ WAVE_EFF mEff[300];
    /* 0x4240 */ be<f32> mSkewWidth;
    /* 0x4244 */ be<f32> mSkewDir;
    /* 0x4248 */ be<u32> mpMaterial;
    /* 0x424C */ WeatherGeometryPair mGeometry[300];
    u8 padmTextures[0x4];
    /* 0x5D350 */ WeatherTexture mTextures[300];
    /* 0x9DA90 */ WeatherRendererPrefix mRenderer;
    /* 0x9DB44 */ WeatherTextureSet mTextureSet;
    /* 0x9DB90 */ be<u8> mActive[300];
    u8 padEnd[0x4];
};
WWHD_OFFSET(dKankyo_wave_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_wave_Packet, mpTextures, 0x98);
WWHD_OFFSET(dKankyo_wave_Packet, mEff, 0xA0);
WWHD_OFFSET(dKankyo_wave_Packet, mSkewWidth, 0x4240);
WWHD_OFFSET(dKankyo_wave_Packet, mSkewDir, 0x4244);
WWHD_OFFSET(dKankyo_wave_Packet, mpMaterial, 0x4248);
WWHD_OFFSET(dKankyo_wave_Packet, mGeometry, 0x424C);
WWHD_OFFSET(dKankyo_wave_Packet, mTextures, 0x5D350);
WWHD_OFFSET(dKankyo_wave_Packet, mRenderer, 0x9DA90);
WWHD_OFFSET(dKankyo_wave_Packet, mTextureSet, 0x9DB44);
WWHD_OFFSET(dKankyo_wave_Packet, mActive, 0x9DB90);
WWHD_SIZE(dKankyo_wave_Packet, 0x9DCC0);
struct dKankyo_star_Packet {
    void draw();
    void render();
    void setColor(u32 colorIndex);
    void destroy(u32 flags);
    u32 construct();
    // HD: renderer methods preserve the original packet ABI.
    void initializeGeometry();
    /* 0x0000 */ WeatherPacketBase mPacket;
    /* 0x0098 */ be<u32> mpTexture;
    /* 0x009C */ u8 mEffect[0x30];
    /* 0x00CC */ be<s16> mEffectNum;
    u8 padmFrame[0x2];
    /* 0x00D0 */ be<u32> mFrame;
    /* 0x00D4 */ be<u8> mActive[2000];
    /* 0x08A4 */ be<u32> mpMaterial;
    /* 0x08A8 */ WeatherStarGeometry mGeometry[2000];
    /* 0x498528 */ WeatherRenderer mRenderer[8];
    /* 0x49A608 */ be<u8> mColorIndex[2000];
};
WWHD_OFFSET(dKankyo_star_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_star_Packet, mpTexture, 0x98);
WWHD_OFFSET(dKankyo_star_Packet, mEffect, 0x9C);
WWHD_OFFSET(dKankyo_star_Packet, mEffectNum, 0xCC);
WWHD_OFFSET(dKankyo_star_Packet, mFrame, 0xD0);
WWHD_OFFSET(dKankyo_star_Packet, mActive, 0xD4);
WWHD_OFFSET(dKankyo_star_Packet, mpMaterial, 0x8A4);
WWHD_OFFSET(dKankyo_star_Packet, mGeometry, 0x8A8);
WWHD_OFFSET(dKankyo_star_Packet, mRenderer, 0x498528);
WWHD_OFFSET(dKankyo_star_Packet, mColorIndex, 0x49A608);
WWHD_SIZE(dKankyo_star_Packet, 0x49ADD8);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
struct WeatherWindInfluence {
    WeatherVector mPos, mDir;
    be<f32> mRadius, mStrength, mInitialRadius;
    be<u32> mRegistIdx;
    be<u8> mbConstant;
    u8 padEnd[3];
};
struct WeatherWindEntity {
    be<u8> mbInUse;
    u8 padState[3];
    be<f32> mMinRadius, mSpeed, mStrengthMaxVel;
    WeatherWindInfluence mInfluence;
};
WWHD_SIZE(WeatherWindInfluence, 0x2C);
WWHD_OFFSET(WeatherWindInfluence, mDir, 0xC);
WWHD_OFFSET(WeatherWindInfluence, mRadius, 0x18);
WWHD_OFFSET(WeatherWindInfluence, mStrength, 0x1C);
WWHD_OFFSET(WeatherWindInfluence, mRegistIdx, 0x24);
WWHD_OFFSET(WeatherWindInfluence, mbConstant, 0x28);
WWHD_SIZE(WeatherWindEntity, 0x3C);
WWHD_OFFSET(WeatherWindEntity, mMinRadius, 4);
WWHD_OFFSET(WeatherWindEntity, mSpeed, 8);
WWHD_OFFSET(WeatherWindEntity, mStrengthMaxVel, 0xC);
WWHD_OFFSET(WeatherWindEntity, mInfluence, 0x10);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
struct WeatherPlay {
    u8 padmStageName[0x5134];
    /* 0x5134 */ char mStageName[8];
    u8 padmRoomControl[0x90];
    /* 0x51CC */ u8 mRoomControl[0x5ACA - 0x51CC];
    /* 0x5ACA */ be<u16> mStatus;
    u8 padmpCamera[0x2C];
    /* 0x5AF8 */ be<u32> mpCamera;
    u8 padmDrawLists[0x234];
    /* 0x5D30 */ u8 mDrawLists[0x1C];
    /* 0x5D4C */ be<u32> mpSkyOpaque;
    /* 0x5D50 */ be<u32> mpSkyTranslucent;
    u8 padmpBackgroundTranslucent[0x20];
    /* 0x5D74 */ be<u32> mpBackgroundTranslucent;
    /* 0x5D78 */ be<u32> mpOpaque;
    /* 0x5D7C */ be<u32> mpTranslucent;
    /* 0x5D80 */ be<u32> mpFilter;
    u8 padmpVrkumoTranslucent[0x20];
    /* 0x5DA4 */ be<u32> mpVrkumoTranslucent;
};
WWHD_OFFSET(WeatherPlay, mStageName, 0x5134);
WWHD_OFFSET(WeatherPlay, mRoomControl, 0x51CC);
WWHD_OFFSET(WeatherPlay, mStatus, 0x5ACA);
WWHD_OFFSET(WeatherPlay, mpCamera, 0x5AF8);
WWHD_OFFSET(WeatherPlay, mDrawLists, 0x5D30);
WWHD_OFFSET(WeatherPlay, mpSkyOpaque, 0x5D4C);
WWHD_OFFSET(WeatherPlay, mpSkyTranslucent, 0x5D50);
WWHD_OFFSET(WeatherPlay, mpBackgroundTranslucent, 0x5D74);
WWHD_OFFSET(WeatherPlay, mpOpaque, 0x5D78);
WWHD_OFFSET(WeatherPlay, mpTranslucent, 0x5D7C);
WWHD_OFFSET(WeatherPlay, mpFilter, 0x5D80);
WWHD_OFFSET(WeatherPlay, mpVrkumoTranslucent, 0x5DA4);
WWHD_SIZE(WeatherPlay, 0x5DA8);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
struct WeatherTextureDescriptor {
    u8 padmWidth[0x4];
    /* 0x4 */ be<u32> mWidth;
    /* 0x8 */ be<u32> mHeight;
    /* 0xC */ be<u32> mDepth;
    /* 0x10 */ be<u32> mMipCount;
    /* 0x14 */ be<u32> mFormat;
    /* 0x18 */ be<u32> mSampleMode;
    /* 0x1C */ be<u32> mUsage;
    u8 padmpPixelData[0x8];
    /* 0x28 */ be<u32> mpPixelData;
    u8 padmPixelBytes[0x4];
    /* 0x30 */ be<u32> mPixelBytes;
    /* 0x34 */ be<u32> mTileMode;
    /* 0x38 */ be<u32> mAlignment;
    u8 padEnd[0x54];
};
WWHD_OFFSET(WeatherTextureDescriptor, mWidth, 0x4);
WWHD_OFFSET(WeatherTextureDescriptor, mHeight, 0x8);
WWHD_OFFSET(WeatherTextureDescriptor, mDepth, 0xC);
WWHD_OFFSET(WeatherTextureDescriptor, mMipCount, 0x10);
WWHD_OFFSET(WeatherTextureDescriptor, mFormat, 0x14);
WWHD_OFFSET(WeatherTextureDescriptor, mSampleMode, 0x18);
WWHD_OFFSET(WeatherTextureDescriptor, mUsage, 0x1C);
WWHD_OFFSET(WeatherTextureDescriptor, mpPixelData, 0x28);
WWHD_OFFSET(WeatherTextureDescriptor, mPixelBytes, 0x30);
WWHD_OFFSET(WeatherTextureDescriptor, mTileMode, 0x34);
WWHD_OFFSET(WeatherTextureDescriptor, mAlignment, 0x38);
WWHD_SIZE(WeatherTextureDescriptor, 0x90);
struct WeatherTextureBinding {
    /* 0x0 */ WeatherTextureDescriptor mDescriptor;
    u8 padmpCachedPixelData[0x44];
    /* 0xD4 */ be<u32> mpCachedPixelData;
    u8 padmCachedPixelBytes[0x4];
    /* 0xDC */ be<u32> mCachedPixelBytes;
    u8 padmPixelStateDirty[0x70];
    /* 0x150 */ be<u32> mPixelStateDirty[3];
    /* 0x15C */ be<u32> mSamplerStateDirty[3];
    u8 padmFlags[0x28];
    /* 0x190 */ be<u8> mFlags;
    u8 padEnd[0x7];
};
WWHD_OFFSET(WeatherTextureBinding, mDescriptor, 0x0);
WWHD_OFFSET(WeatherTextureBinding, mpCachedPixelData, 0xD4);
WWHD_OFFSET(WeatherTextureBinding, mCachedPixelBytes, 0xDC);
WWHD_OFFSET(WeatherTextureBinding, mPixelStateDirty, 0x150);
WWHD_OFFSET(WeatherTextureBinding, mSamplerStateDirty, 0x15C);
WWHD_OFFSET(WeatherTextureBinding, mFlags, 0x190);
WWHD_SIZE(WeatherTextureBinding, 0x198);
} // namespace d_kankyo_wether_cpp

// HD: fixed ABI renderer/lifecycle entries used by part 2.
void weatherSetFog();
void weatherMoveWave();
void weatherMoveCloudShadow();
void weatherDrawPoison(u32, u32);
void weatherDrawHousi(u32, u32);
void weatherDrawKazanbai(u32, u32);
void weatherDrawStar(u32, u32);
void weatherDrawWave(u32, u32, u32);
void weatherDrawCloudShadow(u32, u32);
void weatherDrawVrkumo(u32, u32);
u32 weatherConstructCloudPacket(u32);
void weatherInitializeCloudGeometry(u32);
u32 weatherConstructWavePacket(u32);
void weatherInitializeWaveGeometry(u32);
void weatherDrawStarColor(u32, u32);
void weatherScaleColor(u32, u32, f32);
void weatherApplyBlendState(u32);
void weatherUpdateTextureUniforms(u32);
void weatherSetPrimaryColor(u32, u32, u32);
void weatherSetSecondaryColor(u32, u32, u32);
void weatherUpdateRendererUniforms(u32);
void weatherDestroyTextureSet(u32, u32);
void weatherApplyContextMaterial(u32, u32);
void weatherConstructArchiveFlag(u32);
void weatherConstructHeaderFlag(u32);
void weatherRegisterGlobalObject(u32);

namespace d_kankyo_wether_cpp {
// HD: allocator vtable slots used by geometry buffers.
struct WeatherHeapVtable {
    u8 padAllocate[0x34];
    be<u32> mpAllocate;
    u8 padFree[4];
    be<u32> mpFree;
};
WWHD_OFFSET(WeatherHeapVtable, mpAllocate, 0x34);
WWHD_OFFSET(WeatherHeapVtable, mpFree, 0x3C);
WWHD_SIZE(WeatherHeapVtable, 0x40);
struct WeatherHeap {
    u8 padVtable[0xC];
    gptr<WeatherHeapVtable> mpVtable;
};
WWHD_OFFSET(WeatherHeap, mpVtable, 0xC);
WWHD_SIZE(WeatherHeap, 0x10);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
// HD: snow adds separate splash and flake geometry, buffered uniforms and a
// texture set.
struct dKankyo_snow_Packet {
    void draw();
    void renderParticles(u32 count, u32 blendStart);
    void renderSplashes();
    void initializeGeometry();
    void destroy(u32 flags);
    u32 construct();
    WeatherPacketBase mPacket;
    be<u32> mpTexture;
    u8 mEff[250][0x38];
    WeatherVector mOldEyePos;
    be<f32> field_0x36d0, field_0x36d4;
    be<s16> mEffCount, mTamariStart;
    be<u32> field_0x36dc, mpMaterial;
    WeatherGeometryPair mSplashGeometry[1000], mSnowGeometry[4000];
    be<u32> mGeometryPadding;
    WeatherTexture mSnowTexture;
    WeatherTexture mTextures[4000];
    WeatherRendererPrefix mRenderer;
    WeatherTextureSet mTextureSet;
};
// HD: poison keeps the GC effect/base-position fields and adds a material,
// geometry and uniforms.
struct dKankyo_poison_Packet {
    void draw();
    void render();
    void initializeGeometry();
    void destroy(u32 flags);
    u32 construct();
    WeatherPacketBase mPacket;
    u8 mEff[1000][0x30];
    WeatherVector mBasePos, field_0xbb9c;
    be<s32> mCount;
    be<u32> mpTexture;
    be<f32> mAlpha;
    be<u8> mActive[1000];
    be<u32> mpMaterial;
    WeatherGeometryPair mGeometry[1000];
    WeatherTexture mTextures[1000];
    WeatherRendererPrefix mRenderer;
    WeatherTextureSet mTextureSet;
};
WWHD_OFFSET(dKankyo_snow_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_snow_Packet, mpTexture, 0x98);
WWHD_OFFSET(dKankyo_snow_Packet, mEff, 0x9C);
WWHD_OFFSET(dKankyo_snow_Packet, mOldEyePos, 0x374C);
WWHD_OFFSET(dKankyo_snow_Packet, field_0x36d0, 0x3758);
WWHD_OFFSET(dKankyo_snow_Packet, field_0x36d4, 0x375C);
WWHD_OFFSET(dKankyo_snow_Packet, mEffCount, 0x3760);
WWHD_OFFSET(dKankyo_snow_Packet, mTamariStart, 0x3762);
WWHD_OFFSET(dKankyo_snow_Packet, field_0x36dc, 0x3764);
WWHD_OFFSET(dKankyo_snow_Packet, mpMaterial, 0x3768);
WWHD_OFFSET(dKankyo_snow_Packet, mSplashGeometry, 0x376C);
WWHD_OFFSET(dKankyo_snow_Packet, mSnowGeometry, 0x12C56C);
WWHD_OFFSET(dKankyo_snow_Packet, mSnowTexture, 0x5CFD70);
WWHD_OFFSET(dKankyo_snow_Packet, mTextures, 0x5D00E0);
WWHD_OFFSET(dKankyo_snow_Packet, mRenderer, 0x92B6E0);
WWHD_OFFSET(dKankyo_snow_Packet, mTextureSet, 0x92B794);
WWHD_OFFSET(dKankyo_poison_Packet, mPacket, 0x0);
WWHD_OFFSET(dKankyo_poison_Packet, mEff, 0x98);
WWHD_OFFSET(dKankyo_poison_Packet, mBasePos, 0xBC18);
WWHD_OFFSET(dKankyo_poison_Packet, field_0xbb9c, 0xBC24);
WWHD_OFFSET(dKankyo_poison_Packet, mCount, 0xBC30);
WWHD_OFFSET(dKankyo_poison_Packet, mpTexture, 0xBC34);
WWHD_OFFSET(dKankyo_poison_Packet, mAlpha, 0xBC38);
WWHD_OFFSET(dKankyo_poison_Packet, mActive, 0xBC3C);
WWHD_OFFSET(dKankyo_poison_Packet, mpMaterial, 0xC024);
WWHD_OFFSET(dKankyo_poison_Packet, mGeometry, 0xC028);
WWHD_OFFSET(dKankyo_poison_Packet, mTextures, 0x134E28);
WWHD_OFFSET(dKankyo_poison_Packet, mRenderer, 0x20BBA8);
WWHD_OFFSET(dKankyo_poison_Packet, mTextureSet, 0x20BC5C);
WWHD_SIZE(dKankyo_snow_Packet, 0x92B7E0);
WWHD_SIZE(dKankyo_poison_Packet, 0x20BCA8);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
// HD: shader cache and uniform metadata are separate from the GC J3D material.
struct WeatherShaderProgram {
    u8 padProgram[0x28];
    be<u32> mpProgram;
};
struct WeatherShader {
    be<u8> mFlags;
    u8 padBinary[3];
    be<u32> mpBinary;
    u8 padBinarySize[4];
    be<u32> mBinarySize;
    u8 padProgram[0x6C];
    gptr<WeatherShaderProgram> mpProgram;
};
struct WeatherShaderCache {
    be<u32> mpProgram;
    gptr<WeatherShader> mpShader;
};
struct WeatherUniformDescriptor {
    u8 padSlots[0xC];
    be<s16> mVertexSlot, mPixelSlot, mGeometrySlot;
    u8 padEnd[2];
};
struct WeatherMaterial {
    gptr<WeatherShader> mpShader;
    u8 padDescriptors[8];
    be<u32> mDescriptorCount;
    gptr<WeatherUniformDescriptor> mpDescriptors;
};
struct WeatherUniform {
    be<u32> mState, mpData, mCapacity, mSize;
    u8 padEnd[0xC];
};
struct WeatherUniformInfo {
    u8 padUniforms[0x10];
    WeatherUniform mUniforms[2];
    u8 padBuffer[4];
    be<u32> mBufferIndex;
};
struct WeatherUniformOwner {
    be<u32> mState;
    gptr<WeatherUniformInfo> mpInfo;
};
WWHD_OFFSET(WeatherShaderProgram, mpProgram, 0x28);
WWHD_SIZE(WeatherShaderProgram, 0x2C);
WWHD_OFFSET(WeatherShader, mpBinary, 4);
WWHD_OFFSET(WeatherShader, mBinarySize, 0xC);
WWHD_OFFSET(WeatherShader, mpProgram, 0x7C);
WWHD_SIZE(WeatherShader, 0x80);
WWHD_SIZE(WeatherShaderCache, 8);
WWHD_OFFSET(WeatherUniformDescriptor, mVertexSlot, 0xC);
WWHD_OFFSET(WeatherUniformDescriptor, mPixelSlot, 0xE);
WWHD_OFFSET(WeatherUniformDescriptor, mGeometrySlot, 0x10);
WWHD_SIZE(WeatherUniformDescriptor, 0x14);
WWHD_OFFSET(WeatherMaterial, mDescriptorCount, 0xC);
WWHD_OFFSET(WeatherMaterial, mpDescriptors, 0x10);
WWHD_SIZE(WeatherMaterial, 0x14);
WWHD_OFFSET(WeatherUniform, mpData, 4);
WWHD_OFFSET(WeatherUniform, mSize, 0xC);
WWHD_SIZE(WeatherUniform, 0x1C);
WWHD_OFFSET(WeatherUniformInfo, mUniforms, 0x10);
WWHD_OFFSET(WeatherUniformInfo, mBufferIndex, 0x4C);
WWHD_SIZE(WeatherUniformInfo, 0x50);
WWHD_OFFSET(WeatherUniformOwner, mpInfo, 4);
WWHD_SIZE(WeatherUniformOwner, 8);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
struct WeatherStringVtable {
    u8 padPrepare[0x14];
    be<u32> mpPrepare;
};
struct WeatherArchiveHeader {
    u8 padTextures[0x24];
    be<u32> mTextureDictionaryOffset;
};
struct WeatherMaterialEntry {
    u8 mState[0x20];
    be<u8> mbInitialized;
    u8 padEnd[3];
};
struct WeatherMaterialResource {
    u8 mState[0x84];
};
struct WeatherMaterialResources {
    u8 padCount[0x1C];
    be<u32> mCount;
    gptr<WeatherMaterialResource> mpResources;
};
struct WeatherMaterialRegistry {
    be<u32> mState;
    gptr<WeatherMaterialResources> mpMaterials;
    be<u32> mCount;
    gptr<WeatherMaterialEntry> mpEntries;
};
WWHD_OFFSET(WeatherStringVtable, mpPrepare, 0x14);
WWHD_SIZE(WeatherStringVtable, 0x18);
WWHD_OFFSET(WeatherArchiveHeader, mTextureDictionaryOffset, 0x24);
WWHD_SIZE(WeatherArchiveHeader, 0x28);
WWHD_OFFSET(WeatherMaterialEntry, mbInitialized, 0x20);
WWHD_SIZE(WeatherMaterialEntry, 0x24);
WWHD_SIZE(WeatherMaterialResource, 0x84);
WWHD_OFFSET(WeatherMaterialResources, mCount, 0x1C);
WWHD_OFFSET(WeatherMaterialResources, mpResources, 0x20);
WWHD_SIZE(WeatherMaterialResources, 0x24);
WWHD_OFFSET(WeatherMaterialRegistry, mpMaterials, 4);
WWHD_OFFSET(WeatherMaterialRegistry, mCount, 8);
WWHD_OFFSET(WeatherMaterialRegistry, mpEntries, 0xC);
WWHD_SIZE(WeatherMaterialRegistry, 0x10);
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
enum WeatherPhase { WEATHER_INITIALIZE, WEATHER_MOVE, WEATHER_DRAIN, WEATHER_DELETE };
enum WeatherWindSound {
    JA_SE_ATM_WIND_1 = 0x105A,
    JA_SE_ATM_WIND_HOT = 0x1061,
    JA_SE_ATM_WIND_GRASS = 0x1062
};
} // namespace d_kankyo_wether_cpp

namespace d_kankyo_wether_cpp {
struct WeatherTextureVtable {
    u8 padBind[0x2C];
    be<u32> mpBind;
};
struct WeatherDrawContext {
    u8 padType[0xC];
    be<u32> mType;
    u8 padInfo[0x20];
    be<u32> mpInfo;
};
struct WeatherRegistration {
    u8 mStorage[12];
};
WWHD_OFFSET(WeatherTextureVtable, mpBind, 0x2C);
WWHD_SIZE(WeatherTextureVtable, 0x30);
WWHD_OFFSET(WeatherDrawContext, mType, 0xC);
WWHD_OFFSET(WeatherDrawContext, mpInfo, 0x30);
WWHD_SIZE(WeatherDrawContext, 0x34);
WWHD_SIZE(WeatherRegistration, 12);
// HD: only the eye and render-parameter address are consumed by weather draws.
struct WeatherCameraView {
    u8 padEye[0xDC];
    WeatherVector mEye;
    u8 padRenderParameters[0x158];
    u8 mRenderParameters[4];
};
WWHD_OFFSET(WeatherCameraView, mEye, 0xDC);
WWHD_OFFSET(WeatherCameraView, mRenderParameters, 0x240);
WWHD_SIZE(WeatherCameraView, 0x244);
} // namespace d_kankyo_wether_cpp
