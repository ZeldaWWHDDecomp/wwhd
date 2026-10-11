/** HD Miiverse bottle-spot manager. No GameCube layout exists.
 * Private HD views retain the room lists, singleton disposer, renderer state,
 * and the frame-faithful background query objects.
 */
#pragma once
#include "bindings.h"
#include "d/d_bg_s_spl_grp_chk.h"
#include <cstddef>

enum BottleSpotLimits {
    ROOM_COUNT = 50,
    ROOM_SPOT_CAPACITY = 20,
    SEA_POSITION_TRIES = 11,
    SEA_COLLISION_TRIES = 10
};
enum BottleSpotParameters {
    BOTTLE_LISTED_PARAMETERS = 0x03FF7F14,
    BOTTLE_SEA_PARAMETERS = 0x3BFF7F14
};
constexpr u32 ROOM_SPOT_CONSTRUCTOR = 0x0203857C;
struct BottleSpotManager;
struct BottleRoomSpots {
    cXyz mPositions[ROOM_SPOT_CAPACITY];
    be<u8> mCount;
    u8 pad[3];
};
struct BottleSpotDisposer {
    u8 prefix[12];
    be<u32> __vtbl;
};
struct BottleSpotManagerVtable {
    u8 prefix[0x14];
    gfn mDestructor;
};
struct BottleSpotManager {
    /* 0x0000 */ be<u32> __vtbl;
    /* 0x0004 */ BottleSpotDisposer mDisposer;
    /* 0x0014 */ BottleRoomSpots mRooms[ROOM_COUNT];
    /* 0x2FBC */ cXyz mSpawnPositions[ROOM_SPOT_CAPACITY];
    /* 0x30AC */ be<u8> mSpawnCount;
    u8 pad30AD[3];
    /* 0x30B0 */ be<u8> mDebugDraw, mEnabled, mSpawnFailed, mSpawnAll;
    /* 0x30B4 */ be<f32> mSideMax, mSideMin, mSectorMargin;
    /* 0x30C0 */ be<f32> mForwardMin[2];
    /* 0x30C8 */ be<f32> mForwardMax[2];
    /* 0x30D0 */ be<u8> mDebugSpawnCount;
    u8 pad30D1[3];
    // HD: room/position inputs are guest integers; retain 32-bit wraparound before dereference.
    BottleRoomSpots* room(u32 index) {
        return gabi::at<BottleRoomSpots>(gabi::ea(&mRooms[0]) + index * sizeof(BottleRoomSpots));
    }
    cXyz* position(u32 roomIndex, u32 posIndex) {
        return gabi::at<cXyz>(gabi::ea(room(roomIndex)->mPositions) + posIndex * sizeof(cXyz));
    }
    u32 destroy(u32 flags) {
        auto* vt = gabi::at<BottleSpotManagerVtable>(__vtbl);
        return gabi::call_ptr<u32>(vt->mDestructor.get(), this, flags);
    }
    void setParams();
    void load();
    void spawnAt(u32 pos, u32 spot);
    void spawnListed(u32 spot);
    void randomPos(fopAc_ac_c* pl, u32 spot, cXyz* out);
    u32 bgCheck(cXyz* pos, be<f32>* out);
    u32 trySpawnSea(u32 spot);
    u32 inSector(cXyz* pos);
    void debugDraw();
};
struct BottleSpotVectorWords {
    be<u32> x, y, z;
};
struct BottleSpotStringVtable {
    u8 prefix[0x14];
    gfn mAssureTermination;
};
struct BottleSpotString {
    gptr<const char> mpData;
    be<u32> __vtbl;
    u32 assureTermination() {
        auto* vt = gabi::at<BottleSpotStringVtable>(__vtbl);
        return gabi::call_ptr<u32>(vt->mAssureTermination.get(), this);
    }
};
struct BottleSpotFixedString : BottleSpotString {
    be<u32> mCapacity;
    be<u8> mBuffer[0x40];
};
struct BottleSpotArchive {
    u8 prefix[0x4C];
    be<u32> mFileTableOffset;
};
struct BottleSpotFile {
    be<u32> mDataOffset;
};
struct BottleSpotFileData {
    be<s32> mCount;
    cXyz mPositions[ROOM_SPOT_CAPACITY];
};
struct BottleSpotAppend {
    be<u32> mParameters;
    cXyz mPosition;
    csXyz mAngle;
    be<u16> mSetID;
    u8 mScale[3];
    be<u8> mGbaName;
    be<u32> mParentID;
    be<u8> mArgument, mRoomNo;
    u8 pad[2];
};
struct BottleSpotHeapVtable {
    u8 prefix[0x34];
    gfn mAllocate;
    u8 gap[4];
    gfn mFree;
};
struct BottleSpotHeap {
    u8 prefix[12];
    gptr<BottleSpotHeapVtable> mpVtable;
    be<u32>* allocate(u32 size, u32 alignment) {
        return gabi::call_ptr<be<u32>*>(mpVtable->mAllocate.get(), this, size, alignment);
    }
    u32 free(be<u32>* buffer) { return gabi::call_ptr<u32>(mpVtable->mFree.get(), this, buffer); }
};
struct BottleSpotWaterCheck : dBgS_SplGrpChk {};
struct BottleSpotCheckFrame {
    u8 pad[8];
    cXyz mEnd, mStart;
    dBgS_GndChk mGround;
    dBgS_LinChk mLine;
};
struct BottleSpotGameInfo {
    u8 prefix[0x12A0];
    u8 mBgS;
    u8 gap[0x5134 - 0x12A1];
    char mStageName[8];
    u8 gap2[0x5B2C - 0x513C];
    gptr<fopAc_ac_c> mpPlayer;
};
struct BottleSpotDrawSystem {
    u8 prefix[0x38];
    Mtx34 mView;
    u8 gap[0x14C - 0x68];
    be<u32> mProjection[16];
};
struct BottleSpotDrawState {
    be<u32> mCurrent, mDirty;
};
struct BottleSpotDestructorRecord {
    u8 bytes[12];
};
struct BottleSpotDestructorRecords {
    BottleSpotDestructorRecord mRecords[3];
};
struct BottleSpotHeader {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mByte, mFlag;
    u8 pad[2];
    be<u32> mWords[4];
};
struct BottleSpotAngles {
    be<f32> mMin, mMax;
};
struct BottleSpotTuning {
    be<f32> mZero, mDefaultMin, mDefaultMax, mForwardMin, mForwardSecond, mSectorHalf;
};
struct BottleSpotSector {
    be<f32> mSize, mIndexBias, mOriginBias;
};
struct BottleSpotRandomScale {
    be<f32> mInverse32, mHalf;
};
struct BottleSpotProbe {
    be<f32> mGroundHeight, mWaterMin, mWaterMax;
};
struct BottleSpotSinCos {
    be<f32> mSin, mCos;
};
struct BottleSpotTrig {
    BottleSpotSinCos mEntries[8192];
};
struct BottleSpotLineOffsets {
    be<f32> mX[4], mZ[4];
};
WWHD_OFFSET(BottleRoomSpots, mCount, 0xF0);
WWHD_SIZE(BottleRoomSpots, 0xF4);
WWHD_OFFSET(BottleSpotDisposer, __vtbl, 0xC);
WWHD_SIZE(BottleSpotDisposer, 0x10);
WWHD_OFFSET(BottleSpotManager, mDisposer, 0x4);
WWHD_OFFSET(BottleSpotManager, mRooms, 0x14);
WWHD_OFFSET(BottleSpotManager, mSpawnPositions, 0x2FBC);
WWHD_OFFSET(BottleSpotManager, mSpawnCount, 0x30AC);
WWHD_OFFSET(BottleSpotManager, mDebugDraw, 0x30B0);
WWHD_OFFSET(BottleSpotManager, mEnabled, 0x30B1);
WWHD_OFFSET(BottleSpotManager, mSpawnFailed, 0x30B2);
WWHD_OFFSET(BottleSpotManager, mSpawnAll, 0x30B3);
WWHD_OFFSET(BottleSpotManager, mSideMax, 0x30B4);
WWHD_OFFSET(BottleSpotManager, mSideMin, 0x30B8);
WWHD_OFFSET(BottleSpotManager, mSectorMargin, 0x30BC);
WWHD_OFFSET(BottleSpotManager, mForwardMin, 0x30C0);
WWHD_OFFSET(BottleSpotManager, mForwardMax, 0x30C8);
WWHD_OFFSET(BottleSpotManager, mDebugSpawnCount, 0x30D0);
WWHD_SIZE(BottleSpotManager, 0x30D4);
WWHD_OFFSET(BottleSpotString, mpData, 0x0);
WWHD_OFFSET(BottleSpotString, __vtbl, 0x4);
WWHD_SIZE(BottleSpotString, 0x8);
WWHD_OFFSET(BottleSpotFixedString, mCapacity, 0x8);
WWHD_OFFSET(BottleSpotFixedString, mBuffer, 0xC);
WWHD_SIZE(BottleSpotFixedString, 0x4C);
WWHD_OFFSET(BottleSpotArchive, mFileTableOffset, 0x4C);
WWHD_SIZE(BottleSpotArchive, 0x50);
WWHD_OFFSET(BottleSpotFileData, mPositions, 0x4);
WWHD_SIZE(BottleSpotFileData, 0xF4);
WWHD_OFFSET(BottleSpotAppend, mPosition, 0x4);
WWHD_OFFSET(BottleSpotAppend, mAngle, 0x10);
WWHD_OFFSET(BottleSpotAppend, mGbaName, 0x1B);
WWHD_OFFSET(BottleSpotAppend, mArgument, 0x20);
WWHD_OFFSET(BottleSpotAppend, mRoomNo, 0x21);
WWHD_SIZE(BottleSpotAppend, 0x24);
WWHD_OFFSET(BottleSpotHeapVtable, mAllocate, 0x34);
WWHD_OFFSET(BottleSpotHeapVtable, mFree, 0x3C);
WWHD_SIZE(BottleSpotHeapVtable, 0x40);
WWHD_OFFSET(BottleSpotHeap, mpVtable, 0xC);
WWHD_SIZE(BottleSpotHeap, 0x10);
WWHD_SIZE(BottleSpotWaterCheck, 0x50);
WWHD_OFFSET(BottleSpotCheckFrame, mEnd, 0x8);
WWHD_OFFSET(BottleSpotCheckFrame, mStart, 0x14);
WWHD_OFFSET(BottleSpotCheckFrame, mGround, 0x20);
WWHD_OFFSET(BottleSpotCheckFrame, mLine, 0x74);
WWHD_SIZE(BottleSpotCheckFrame, 0xE0);
WWHD_OFFSET(BottleSpotGameInfo, mBgS, 0x12A0);
WWHD_OFFSET(BottleSpotGameInfo, mStageName, 0x5134);
WWHD_OFFSET(BottleSpotGameInfo, mpPlayer, 0x5B2C);
WWHD_SIZE(BottleSpotGameInfo, 0x5B30);
WWHD_OFFSET(BottleSpotDrawSystem, mView, 0x38);
WWHD_OFFSET(BottleSpotDrawSystem, mProjection, 0x14C);
WWHD_SIZE(BottleSpotDrawSystem, 0x18C);
WWHD_OFFSET(BottleSpotHeader, mByte, 0x8);
WWHD_OFFSET(BottleSpotHeader, mFlag, 0x9);
WWHD_OFFSET(BottleSpotHeader, mWords, 0xC);
WWHD_SIZE(BottleSpotHeader, 0x1C);

BottleSpotManager* bottleSpotAllocate(u32);
void bottleSpotConstructRooms(BottleRoomSpots*, u32, u32, u32);
BottleSpotManager* bottleSpotAllocateInHeap(u32, void*, u32);
void bottleSpotConstructDisposer(BottleSpotDisposer*, void*, u32);
void* bottleSpotGetResource(void*, BottleSpotString*, BottleSpotString*, u32);
BottleSpotArchive* bottleSpotGetArchive(void*);
void bottleSpotFormat(BottleSpotFixedString*, const char*, u32);
BottleSpotFile* bottleSpotFindFile(void*, const char*);
BottleSpotAppend* bottleSpotCreateAppend();
u32 bottleSpotGetLayer();
void bottleSpotCreateActor(u32, u32, u32, u32, BottleSpotAppend*);
void bottleSpotConsumeComment(void*);
u32 bottleSpotCheckItem(u32);
u32 bottleSpotReadyComments(void*);
BottleSpotHeap* bottleSpotGetCurrentHeap(void*);
u32 bottleSpotRandom(void*);
BottleSpotHeap* bottleSpotFindHeap(void*, void*);
void bottleSpotAdd(cXyz*, cXyz*, cXyz*);
void bottleSpotSubtract(cXyz*, cXyz*, cXyz*);
f32 bottleSpotSquaredMagnitude(cXyz*);
void bottleSpotConstructGround(dBgS_GndChk*);
f32 bottleSpotGroundCross(void*, dBgS_GndChk*);
void bottleSpotDestroyGround(dBgS_GndChk*, u32);
u32 bottleSpotCheckSeaArea(f32, f32);
f32 bottleSpotSeaHeight(f32, f32);
void bottleSpotConstructWater(BottleSpotWaterCheck*);
void bottleSpotRegisterGlobal(void*);
u32 bottleSpotWaterCross(void*, BottleSpotWaterCheck*);
void bottleSpotConstructLine(dBgS_LinChk*);
void bottleSpotSetLine(dBgS_LinChk*, cXyz*, cXyz*, u32);
u32 bottleSpotLineCross(void*, dBgS_LinChk*);
void bottleSpotDestroyLine(dBgS_LinChk*, u32);
BottleSpotDrawState* bottleSpotGetDrawState(BottleSpotDrawSystem*);
u32 bottleSpotSetDrawState(Mtx34*, void*, u32);
f32 bottleSpotSqrt(f32);
void bottleSpotDestroyDisposer(BottleSpotDisposer*, u32);
void bottleSpotDelete(void*);
void bottleSpotInitHeaderByte(void*);
void bottleSpotInitHeaderFlag(void*);
BottleSpotGameInfo* bottleSpotGetGameInfo();