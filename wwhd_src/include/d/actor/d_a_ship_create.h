// King of Red Lions creation-only HD views. Shared ship members follow tww.
// HD: texture entries are 0x24 bytes, packet 0xA4, sight state 0x30;
// wave callbacks are 0x64 and track callback 0x50. Unused bytes stay opaque.
#pragma once
#include "d/actor/d_a_ship.h"

// HD: private views of the added texture, sight and particle fields. The shared
// ship layout remains the owner of the actor and its GameCube member names.
struct ShipCreateTextureEntry {
    be<u32> mHeader[3];
    be<u32> mPaletteOffset;
    be<u32> mHeaderTail[3];
    be<u32> mImageOffset;
    be<u32> mHDImage;
};
WWHD_SIZE(ShipCreateTextureEntry, 0x24);
struct ShipCreateTextureTable {
    be<u16> mNum;
    u8 mPadding[2];
    gptr<ShipCreateTextureEntry> mEntries;
    be<u32> mReplacedMask[4];
    be<u32> mMaskHigh;
    be<u32> mMaskLow;
};
WWHD_SIZE(ShipCreateTextureTable, 0x20);
struct ShipCreateModelTextures {
    u8 mPrefix[0x30];
    gptr<ShipCreateTextureTable> mTexture;
    be<u32> mTextureName;
};
WWHD_SIZE(ShipCreateModelTextures, 0x38);
struct ShipCreateFormattedString {
    be<u32> mStr;
    be<u32> __vtbl;
    u8 mBuffer[0x24];
};
WWHD_SIZE(ShipCreateFormattedString, 0x2C);
struct ShipCreateSightPacket {
    u8 mPrefix[0xC];
    be<u32> __vtbl;
    u8 mBody[0x98 - 0x10];
    gptr<J3DModel> mBeacon;
    be<u32> mModel;
    be<u32> mSegments;
};
WWHD_SIZE(ShipCreateSightPacket, 0xA4);
WWHD_OFFSET(ShipCreateSightPacket, mBeacon, 0x98);
struct ShipCreateSight {
    be<s16> mAngle[3];
    u8 mPadding[2];
    be<f32> mTime;
    be<f32> mTrajectory[7];
    be<s16> mSegment;
    u8 mPadding2[2];
    be<f32> mHeight;
};
WWHD_SIZE(ShipCreateSight, 0x30);
struct ShipCreateModelFlags {
    u8 mPrefix[0x74];
    be<u32> mFlags;
};
struct ShipCreateWave {
    be<u32> __vtbl;
    u8 mPrefix[0x14];
    be<f32> mMaxSpeed;
    u8 mBody[0x20];
    cXyz mPosition[3];
    u8 mTail[4];
};
WWHD_SIZE(ShipCreateWave, 0x64);
WWHD_OFFSET(ShipCreateWave, mPosition, 0x3C);
struct ShipCreateTrack {
    be<u32> __vtbl;
    u8 mPrefix[0xC];
    cXyz mPosition[3];
    u8 mBody[0x48 - 0x34];
    be<f32> mLimitSpeed;
    u8 mTail[4];
};
WWHD_SIZE(ShipCreateTrack, 0x50);
struct ShipCreateFollow {
    u8 mPrefix[0x10];
    be<f32> mRate;
};
WWHD_SIZE(ShipCreateFollow, 0x14);

const char* ShipCreate_getTextureName(u32, u32);
void ShipCreate_assureTermination(daShip_SafeString*);
void ShipCreate_formatTextureName(ShipCreateFormattedString*, const char*, u32);
void* ShipCreate_getResource(u32, daShip_SafeString*, daShip_SafeString*);
u32 ShipCreate_resourceReady(u32, daShip_SafeString*, u32, u32);
u32 ShipCreate_new(u32, u32, u32);
u32 ShipCreate_newArray(u32, u32, u32);
u32 ShipCreate_modelConstruct(u32);
void ShipCreate_modelCreate(u32, u32, u32);
u32 ShipCreate_constructNewArray(u32, u32, u32, u32, u32);
void ShipCreate_segmentCreate(u32, u32);
BOOL ShipCreate_ropeInit(void*, u32, u32, u32, u32);
void ShipCreate_modelUnlock(u32, u32);
BOOL ShipCreate_shadowInit(void*, J3DModelData*, u32, u32, u32, f32, u32, s32, u32, u32);
void ShipCreate_ortho(Mtx34*, f32, f32, f32, f32, f32, f32);
void ShipCreate_setMin(daShip_c*, f32, f32, f32);
void ShipCreate_setMax(daShip_c*, f32, f32, f32);
void ShipCreate_createWindArrow(u32, daShip_c*, u32, u32, u32);
void ShipCreate_createChild(u32, u32, u32, cXyz*, s32, u32, u32, s32, u32);

// HD: heap callback lives in another translation unit. Keep its guest address
// until autoglue can register external static callbacks.
constexpr heapCallbackFunc daShip_createHeap = 0x0247BA34;

struct ShipCreateAcchBase {
    u8 mPrefix[0x10];
    be<u32> mPolyVtable;
    be<u32> mCheckVtable;
    be<u8> mObject;
    u8 mPadding[7];
    be<u32> mObjectVtable;
};
WWHD_SIZE(ShipCreateAcchBase, 0x24);
struct ShipCreateVtable {
    be<u32> __vtbl;
};
struct ShipCreateResourceHeader {
    u8 mPrefix[0x18];
    be<u32> mMaterialNameOffset;
};
WWHD_SIZE(ShipCreateResourceHeader, 0x1C);
struct ShipCreateVirtualTable {
    u8 mPrefix[0x14];
    be<u32> mMethod;
};
WWHD_SIZE(ShipCreateVirtualTable, 0x18);
struct ShipCreateTexGenBlock {
    gptr<ShipCreateVirtualTable> __vtbl;
    u32 getTexMtx(u32 index) { return gabi::call_ptr<u32>(__vtbl->mMethod, this, index); }
};
struct ShipCreateMaterial {
    u8 mPrefix[0x14];
    gptr<ShipCreateTexGenBlock> mTexGenBlock;
};
WWHD_SIZE(ShipCreateMaterial, 0x18);
struct ShipCreatePlay {
    u8 mPrefix[0x5B3C];
    gptr<daShip_c> mShipActor;
};
WWHD_SIZE(ShipCreatePlay, 0x5B40);
void ShipCreate_frameInit(void*, u32);
void ShipCreate_lineConstruct(void*);
void ShipCreate_btkConstruct(void*);
void ShipCreate_constructArray(void*, u32, u32, u32);
void ShipCreate_acchConstruct(void*);
void ShipCreate_sphereConstruct(void*);
void ShipCreate_packetConstruct(void*);
void ShipCreate_rippleConstruct(void*);
void ShipCreate_followConstruct(void*, u32, u32);

// HD: this TU's GHS vtables and callbacks defined in the other ship sources.
namespace ShipCreateTU {
constexpr u32 shipVtable = 0x1003A2EC;
constexpr u32 polyVtable = 0x1003A27C;
constexpr u32 objectVtable = 0x1003A28C;
constexpr u32 checkVtable = 0x1003A29C;
constexpr u32 sightVtable = 0x1003A7F8;
constexpr u32 waveVtable = 0x100521A8;
constexpr u32 splashVtable = 0x100521E8;
constexpr u32 trackVtable = 0x10052268;
constexpr u32 segmentConstructor = 0x02482E30;
constexpr u32 acchCirConstructor = 0x024EFE94;
constexpr u32 cylinderConstructor = 0x02482EA0;
constexpr u32 statusConstructor = 0x02482F2C;
constexpr u32 sphereConstructor = 0x025166F0;
constexpr u32 positionConstructor = 0x02482F94;
constexpr u32 bodyJointCallback = 0x0247187C;
constexpr u32 headJointCallback0 = 0x02471C50;
constexpr u32 headJointCallback1 = 0x02471E54;
constexpr u32 cannonJointCallback = 0x02471AAC;
constexpr u32 craneJointCallback = 0x02471BF4;
} // namespace ShipCreateTU

struct ShipCreateResourceArc {
    be<char> mName[1];
};
WWHD_SIZE(ShipCreateResourceArc, 1);

// HD: process and resource indices retain the WWHD archive/profile numbering.
namespace ShipCreateProcess {
enum { fpcNm_GRID_e = 0xAB, fpcNm_WIND_ARROW_e = 0x16, fpcNm_Coming2_e = 0x10E };
}
namespace ShipCreateResource {
enum { dRes_INDEX_LINK_BDL_ROPEEND_e = 0x2E, dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E };
}
enum { ShipCreateStageSea = 7, ShipCreateActorInitialized = 8 };

WWHD_OFFSET(ShipCreateTextureEntry, mPaletteOffset, 0xC);
WWHD_OFFSET(ShipCreateTextureEntry, mImageOffset, 0x1C);
WWHD_OFFSET(ShipCreateTextureEntry, mHDImage, 0x20);
WWHD_OFFSET(ShipCreateTextureTable, mEntries, 4);
WWHD_OFFSET(ShipCreateTextureTable, mReplacedMask, 8);
WWHD_OFFSET(ShipCreateTextureTable, mMaskHigh, 0x18);
WWHD_OFFSET(ShipCreateModelTextures, mTexture, 0x30);
WWHD_OFFSET(ShipCreateModelTextures, mTextureName, 0x34);
WWHD_OFFSET(ShipCreateFormattedString, __vtbl, 4);
WWHD_OFFSET(ShipCreateSightPacket, __vtbl, 0xC);
WWHD_OFFSET(ShipCreateSightPacket, mModel, 0x9C);
WWHD_OFFSET(ShipCreateSightPacket, mSegments, 0xA0);
WWHD_OFFSET(ShipCreateSight, mTime, 8);
WWHD_OFFSET(ShipCreateSight, mTrajectory, 0xC);
WWHD_OFFSET(ShipCreateSight, mSegment, 0x28);
WWHD_OFFSET(ShipCreateSight, mHeight, 0x2C);
WWHD_SIZE(ShipCreateModelFlags, 0x78);
WWHD_OFFSET(ShipCreateModelFlags, mFlags, 0x74);
WWHD_OFFSET(ShipCreateWave, mMaxSpeed, 0x18);
WWHD_OFFSET(ShipCreateTrack, mPosition, 0x10);
WWHD_OFFSET(ShipCreateTrack, mLimitSpeed, 0x48);
WWHD_OFFSET(ShipCreateFollow, mRate, 0x10);
WWHD_OFFSET(ShipCreateAcchBase, mPolyVtable, 0x10);
WWHD_OFFSET(ShipCreateAcchBase, mCheckVtable, 0x14);
WWHD_OFFSET(ShipCreateAcchBase, mObject, 0x18);
WWHD_OFFSET(ShipCreateAcchBase, mObjectVtable, 0x20);
WWHD_SIZE(ShipCreateVtable, 4);
WWHD_SIZE(ShipCreateTexGenBlock, 4);
WWHD_OFFSET(ShipCreateResourceHeader, mMaterialNameOffset, 0x18);
WWHD_OFFSET(ShipCreateVirtualTable, mMethod, 0x14);
WWHD_OFFSET(ShipCreateMaterial, mTexGenBlock, 0x14);
WWHD_OFFSET(ShipCreatePlay, mShipActor, 0x5B3C);
struct ShipCreateEffectModels {
    u8 mPrefix[0x5AC];
    gptr<J3DModel> mpBeaconModel;
    gptr<J3DModel> mpFallPointModel;
    gptr<mDoExt_McaMorf> mpFallPointAnm;
};
WWHD_SIZE(ShipCreateEffectModels, 0x5B8);
WWHD_OFFSET(ShipCreateEffectModels, mpBeaconModel, 0x5AC);
WWHD_OFFSET(ShipCreateEffectModels, mpFallPointModel, 0x5B0);
WWHD_OFFSET(ShipCreateEffectModels, mpFallPointAnm, 0x5B4);
struct ShipCreateSightActor {
    u8 mPrefix[0x1B24];
    ShipCreateSight mSight;
};
WWHD_SIZE(ShipCreateSightActor, 0x1B54);
WWHD_OFFSET(ShipCreateSightActor, mSight, 0x1B24);
