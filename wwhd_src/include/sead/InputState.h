// WWHD controller state and repeat configuration, from local constructors and accesses.
#pragma once
#include "sead/Foundation.h"
namespace sead {
struct InputPair { be<u32> mX; be<u32> mY; };
struct InputRect { be<f32> mMinX; be<f32> mMinY; be<f32> mMaxX; be<f32> mMaxY; };
struct InputState {
    be<u32> mRelease;
    be<u32> mTrigger;
    be<u32> mRepeat;
    be<u32> mPointerState;
    InputPair mPointer;
    InputRect mPointerBounds;
    be<u32> mRepeatCounts[32];
    be<u8> mRepeatStart[32];
    be<u8> mRepeatStep[32];
    be<f32> mStickThreshold;
    be<f32> mCrossThreshold;
    be<f32> mStickReleaseThreshold;
    be<f32> mCrossReleaseThreshold;
    be<s32> mButtonCount;
    be<s32> mStickBit;
    be<s32> mCrossBit;
    be<s32> mPointerBit;
    be<u32> mIdleFrames;
    be<u32> mButtons;
    InputPair mPointerFloat;
    be<f32> mStickX;
    be<f32> mStickY;
    be<f32> mCrossX;
    be<f32> mCrossY;
    be<f32> mLeftTrigger;
    be<f32> mRightTrigger;
    void configureRepeat(u32 mask, u8 start, u8 step);
    u32 directionMask();
    u32 isIdle();
    void clear();
    void updatePointer(u32 valid,u32 contact,const InputPair* point);
    u32 stickDirections(u32 previous,const InputPair* stick,u32 firstBit,f32 onThreshold,f32 offThreshold);
    void update(u32 previous,u32 previousPointer);
};
}
WWHD_OFFSET(sead::InputState,mRelease,0);
WWHD_OFFSET(sead::InputState,mTrigger,4);
WWHD_OFFSET(sead::InputState,mRepeat,8);
WWHD_OFFSET(sead::InputState,mPointerState,12);
WWHD_OFFSET(sead::InputState,mPointer,16);
WWHD_OFFSET(sead::InputState,mPointerBounds,24);
WWHD_OFFSET(sead::InputState,mRepeatCounts,40);
WWHD_OFFSET(sead::InputState,mRepeatStart,168);
WWHD_OFFSET(sead::InputState,mRepeatStep,200);
WWHD_OFFSET(sead::InputState,mStickThreshold,232);
WWHD_OFFSET(sead::InputState,mButtonCount,248);
WWHD_OFFSET(sead::InputState,mStickBit,252);
WWHD_OFFSET(sead::InputState,mCrossBit,256);
WWHD_OFFSET(sead::InputState,mPointerBit,260);
WWHD_OFFSET(sead::InputState,mIdleFrames,264);
WWHD_OFFSET(sead::InputState,mButtons,268);
WWHD_OFFSET(sead::InputState,mPointerFloat,272);
WWHD_OFFSET(sead::InputState,mStickX,280);
WWHD_OFFSET(sead::InputState,mStickY,284);
WWHD_OFFSET(sead::InputState,mCrossX,288);
WWHD_OFFSET(sead::InputState,mCrossY,292);
WWHD_OFFSET(sead::InputState,mLeftTrigger,296);
WWHD_OFFSET(sead::InputState,mRightTrigger,300);
WWHD_SIZE(sead::InputState,304);
WWHD_SIZE(sead::InputPair,8);
WWHD_SIZE(sead::InputRect,16);

sead::InputState* constructInputState(sead::InputState*, s32, s32, s32, u32);
