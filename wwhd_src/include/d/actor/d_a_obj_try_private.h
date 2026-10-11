/* Unit-private HD tables and native views. No shared API additions. */
#pragma once
#include "d/actor/d_a_obj_try.h"
namespace try_actor {
using Act_c = daObjTry::Act_c;
// HD: integer load/store pairs retain signaling NaNs rather than copying float values.
inline be<u32>& bits(be<f32>& value) { return reinterpret_cast<be<u32>&>(value); }
inline be<u32>& bits(be<u32>& value) { return value; }
struct AttrHD {
    /* 0x00 */ be<f32> mGravity, mWaterOffset, mAttentionOffset;
    /* 0x0C */ be<u8> mWeight;
    u8 paddingDrop[3];
    /* 0x10 */ be<f32> mDropSpeedY, mDropSpeedBase, mDropSpeedRate;
    /* 0x1C */ be<f32> mAirDrag, mAirStream, mBuoyancy, mWaterDrag, mWaterStream, mMaxSinkSpeed;
    u8 paddingSound[0x42 - 0x34];
    /* 0x42 */ be<u8> mHitEnvironment, mHitContext, mWaterEnvironment, mWaterContext;
    /* 0x46 */ be<s16> mModelIndex, mBrkIndex;
    u8 paddingRadius;
    /* 0x4B */ be<u8> mRadius, mHeight, mGroundOffset;
    u8 paddingEffects[6];
    /* 0x54 */ be<u32> mLandSound, mWaterSound, mHitSound, mHeapSize;
    u8 paddingFlags[0x70 - 0x64];
    /* 0x70 */ be<u8> mSwitchAppear, mUnique, mClearSaveSwitch, mChaseCorrection, mAllowCorrection, mSkipCull;
    u8 paddingEnd[2];
};
WWHD_OFFSET(AttrHD, mWeight, 0xC);
WWHD_OFFSET(AttrHD, mHitEnvironment, 0x42);
WWHD_OFFSET(AttrHD, mModelIndex, 0x46);
WWHD_OFFSET(AttrHD, mBrkIndex, 0x48);
WWHD_OFFSET(AttrHD, mRadius, 0x4B);
WWHD_OFFSET(AttrHD, mLandSound, 0x54);
WWHD_OFFSET(AttrHD, mSwitchAppear, 0x70);
WWHD_SIZE(AttrHD, 0x78);
struct AttributesHD {
    AttrHD mRows[13];
};
WWHD_SIZE(AttributesHD, 13 * 0x78);
struct EmitterHD {
    u8 prefix[0x5C];
    be<s32> mMaxFrame;
    u8 paddingFlags[0x254 - 0x60];
    be<u32> mFlags;
};
WWHD_OFFSET(EmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(EmitterHD, mFlags, 0x254);
WWHD_SIZE(EmitterHD, 0x258);
struct CameraHD {
    u8 prefix[0x248];
    u8 mCamera[1];
};
WWHD_OFFSET(CameraHD, mCamera, 0x248);
WWHD_SIZE(CameraHD, 0x249);
struct PlayHD {
    u8 prefix[0x12A0];
    u8 mBgS[0x26A4 - 0x12A0];
    u8 mCcS[0x4EF8 - 0x26A4];
    u8 mMass[0x5292 - 0x4EF8];
    be<u8> mEventRunning;
    u8 paddingVibration[0x599C - 0x5293];
    u8 mVibration[0x5AB0 - 0x599C];
    gptr<void> mpParticle;
    u8 paddingCamera[0x5AF8 - 0x5AB4];
    gptr<CameraHD> mpCamera;
    u8 paddingPlayer[0x5B2C - 0x5AFC];
    gptr<fopAc_ac_c> mpPlayer;
};
WWHD_OFFSET(PlayHD, mBgS, 0x12A0);
WWHD_OFFSET(PlayHD, mCcS, 0x26A4);
WWHD_OFFSET(PlayHD, mMass, 0x4EF8);
WWHD_OFFSET(PlayHD, mEventRunning, 0x5292);
WWHD_OFFSET(PlayHD, mVibration, 0x599C);
WWHD_OFFSET(PlayHD, mpParticle, 0x5AB0);
WWHD_OFFSET(PlayHD, mpCamera, 0x5AF8);
WWHD_OFFSET(PlayHD, mpPlayer, 0x5B2C);
WWHD_SIZE(PlayHD, 0x5B30);
inline PlayHD* playView() { return reinterpret_cast<PlayHD*>(dComIfGp_get()); }
struct SaveHD {
    u8 prefix[0x20];
    u8 mSwitch[0x1178 - 0x20];
    u8 mEvent[1];
};
WWHD_OFFSET(SaveHD, mSwitch, 0x20);
WWHD_OFFSET(SaveHD, mEvent, 0x1178);
WWHD_SIZE(SaveHD, 0x1179);
struct WordHD {
    be<u32> mValue;
};
struct ByteHD {
    be<u8> mValue;
};
WWHD_SIZE(WordHD, 4);
WWHD_SIZE(ByteHD, 1);
struct WaterHD : cBgS_PolyInfo, cBgS_Chk, dBgS_Chk {
    cXyz mPosition;
    be<f32> mTop, mHeight;
    u8 padding[4];
};
WWHD_OFFSET(WaterHD, mHeight, 0x48);
WWHD_SIZE(WaterHD, 0x50);
inline WaterHD& waterView(dBgS_WtrChk& water) { return reinterpret_cast<WaterHD&>(water); }
struct ModeHD {
    be<s16> mAdjustment, mSlot;
    be<u32> mTarget;
};
WWHD_SIZE(ModeHD, 8);
struct ModesHD {
    gabi::ptmf<void (Act_c::*)()> mEntries[5];
};
WWHD_SIZE(ModesHD, 40);
struct ColorsHD {
    GXColor mPrimary, mEnvironment;
};
WWHD_SIZE(ColorsHD, 8);
struct TrigHD {
    be<f32> mCos, mSin;
};
WWHD_SIZE(TrigHD, 8);
struct TrigTableHD {
    TrigHD mEntries[8192];
};
WWHD_SIZE(TrigTableHD, 65536);
struct VtableHD {
    u8 prefix[0x44];
    be<u32> mEnd;
};
WWHD_OFFSET(VtableHD, mEnd, 0x44);
WWHD_SIZE(VtableHD, 0x48);
inline void endCallback(dPa_followEcallBack* callback) {
    auto* vtable = gabi::at<VtableHD>(callback->__vtbl);
    gabi::call_ptr(vtable->mEnd, callback);
}
struct ModelHD {
    u8 prefix[0xAC];
    gptr<J3DModelData> mpModelData;
    u8 paddingScale[0xBC - 0xB0];
    cXyz mScale;
    Mtx34 mBaseMtx;
};
WWHD_OFFSET(ModelHD, mpModelData, 0xAC);
WWHD_OFFSET(ModelHD, mScale, 0xBC);
WWHD_OFFSET(ModelHD, mBaseMtx, 0xC8);
WWHD_SIZE(ModelHD, 0xF8);
inline ModelHD* modelView(u32 address) { return gabi::at<ModelHD>(address); }
inline EmitterHD* emitterView(u32 address) { return gabi::at<EmitterHD>(address); }
struct HeaderStaticsHD {
    be<f32> mLower, mUpper;
    be<u8> mMinusOne, mPlusOne;
};
WWHD_SIZE(HeaderStaticsHD, 10);
struct RegistersHD {
    be<u32> mHeadA, mHeadB, mReadyA, mReadyB;
};
WWHD_SIZE(RegistersHD, 16);
struct BoundsHD {
    be<u32> mLower, mUpper;
};
WWHD_SIZE(BoundsHD, 8);
struct RecordHD {
    be<u32> words[3];
};
WWHD_SIZE(RecordHD, 12);
} // namespace try_actor
namespace try_actor {
struct CylinderCtorHD {
    u8 prefix[0x110];
    be<u32> mShapeVtable, mCylinderVtable;
};
WWHD_OFFSET(CylinderCtorHD, mShapeVtable, 0x110);
WWHD_OFFSET(CylinderCtorHD, mCylinderVtable, 0x114);
WWHD_SIZE(CylinderCtorHD, 0x118);
} // namespace try_actor

namespace try_actor {
struct SavePointerHD {
    gptr<SaveHD> mPointer;
};
struct ResourcePointerHD {
    gptr<void> mPointer;
};
WWHD_SIZE(SavePointerHD, 4);
WWHD_SIZE(ResourcePointerHD, 4);
} // namespace try_actor
