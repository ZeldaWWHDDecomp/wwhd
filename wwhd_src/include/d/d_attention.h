#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"

// HD: private image views retain the 32-bit verified ABI. The parameter block grows by eight
// bytes after the GameCube members; drawing uses mDoExt_bpkAnm instead of a color-animation array.
namespace AttentionHD {
// HD: raw entry points also receive address zero; it denotes the mapped guest page here.
template <class T> inline T* view(u32 address) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}

struct Globe {
    be<f32> mRadius;
    be<s16> mV, mU;
};
struct List {
    be<f32> mWeight, mDistance;
    be<u32> mType, mActorID;
};
struct Hint {
    be<u32> mHintActorID;
    be<s32> mPriority;
    be<u32> mZHintTargetID;
};
struct Catch {
    be<u32> mRequestActorID;
    be<s32> field_0x4;
    be<f32> mDistance;
    be<u8> mCatchItemNo, field_0xd, field_0xe, field_0xf;
    be<u32> mCatghTargetID;
    be<u8> mChangeItem;
    u8 padding[3];
};
struct Look {
    be<u32> mRequestActorID;
    be<s32> field_0x4;
    be<f32> mDistance;
    be<u32> mLookTargetID;
};
struct Param {
    be<u16> mFlags;
    u8 padding[2];
    be<f32> field_0x04, field_0x08, field_0x0c;
    be<f32> mDangerBGMDistance, mBGMDistMargin, mSWModeDisable;
    u8 hdParameters[8];
    be<u32> __vtbl;
};
struct Draw {
    be<u32> anm, mpAnmClr, mpAnmMatClr;
};
struct Attention {
    be<u32> mpPlayer, mLockonTargetID, mCallBack, mPadNo, mFlagMask;
    u8 field_0x014[4];
    be<u8> mLockOnState, field_0x019, field_0x01a, field_0x01b;
    be<s16> field_0x01c;
    u8 padding[2];
    be<u32> mFlags, heap;
    be<s8> field_0x028;
    u8 padding2[3];
    cXyz mDrawAttnPos;
    Draw draw[2];
    be<u32> mDrawLockonTargetID;
    List mLockOnList[8];
    be<s32> mLockonCount, mLockOnOffset;
    List mActionList[4];
    be<s32> mActionCount, mActionOffset;
    Hint mHint;
    Catch mCatch;
    Look mLook[2];
    be<u32> mEnemyID;
    be<f32> mEnemyDistance;
    Param mAttParam;
};
struct DistTbl {
    be<f32> mDistXZMax, mDistXZMaxRelease, mDistXZAngleAdjust, mDeltaYMax, mDeltaYMin;
    be<f32> mWeightDivisor;
    be<u32> mFrontAngleCheckBits;
};
struct LocTbl {
    be<s16> mType;
    be<u16> mMask;
};
struct Morf {
    u8 padding[0x90];
    be<u32> mpModel, mpAnm;
    u8 padding2[4];
    be<f32> mFrame;
    be<s16> mStart, mEnd;
};
struct Model {
    u8 padding[0xAC];
    be<u32> mpModelData;
    u8 padding2[0x18];
    be<f32> mBaseTransform[12];
};
struct ColorAnimation {
    be<u32> mFrameBits, __vtbl;
    u8 padding[4];
    be<u32> mpUpdateMaterialTable;
};
struct UpdateMaterialTable {
    u8 padding[0x14];
    be<u16> mUpdateMaterialNum;
};
struct AnimationResource {
    u8 padding[8];
    be<u32> mpNameBlock;
};
struct ResourceNameBlock {
    u8 padding[4];
    be<s32> mNameOffset;
};
struct Heap {
    u8 padding[0xC];
    be<u32> __vtbl, mName;
};
WWHD_OFFSET(Attention, mLockOnList, 0x54);
WWHD_OFFSET(Attention, mActionList, 0xDC);
WWHD_OFFSET(Attention, mHint, 0x124);
WWHD_OFFSET(Attention, mCatch, 0x130);
WWHD_OFFSET(Attention, mLook, 0x148);
WWHD_OFFSET(Attention, mAttParam, 0x170);
WWHD_OFFSET(Param, __vtbl, 0x24);
WWHD_OFFSET(Morf, mFrame, 0x9C);
WWHD_OFFSET(Model, mBaseTransform, 0xC8);
static_assert(sizeof(Globe) == 8);
static_assert(sizeof(List) == 0x10);
static_assert(sizeof(Hint) == 0xC);
static_assert(sizeof(Catch) == 0x18);
static_assert(sizeof(Look) == 0x10);
static_assert(sizeof(Param) == 0x28);
static_assert(sizeof(Draw) == 0xC);
static_assert(sizeof(Attention) == 0x198);
static_assert(sizeof(DistTbl) == 0x1C);
} // namespace AttentionHD
