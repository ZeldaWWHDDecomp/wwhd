/* Wii U graphics utility views reconstructed from binary field accesses. */
#pragma once
#include "wwhd.h"

namespace agl { namespace gx2util {
struct TickValue { be<u32> mHigh; be<u32> mLow; };
struct TimedCounter {
    u8 mPropertyBase[0x18];
    gptr<void> mVtable;
    be<u32> mCount;
    be<u32> mRate;
    u8 mPadding[4];
    TickValue mStart;
    TickValue mEnd;
    be<u8> mActive;
    u8 mTail[7];
    void copyTo(TimedCounter* destination);
    u32 difference(const TimedCounter* other) const;
};
struct FrameController {
    be<f32> mCurrent;
    be<f32> mStart;
    be<f32> mEnd;
    be<f32> mRate;
    be<u32> mPolicy;
    be<u32> mCycle;
    void configure(u32 policy, f32 start, f32 end);
};
struct BindingTable {
    gptr<be<u32>> mSlots;
    be<u16> mFlags;
    be<u16> mCapacity;
    be<u16> mSourceCount;
    be<u16> mTargetCount;
    void configure(be<u32>* slots, u32 capacity);
    void reset(u32 targetCount);
    void setTargets(const be<u16>* targets);
};
struct BufferView {
    gptr<void> mData;
    be<u32> mSize;
    be<u32> mCount;
    void configure(void* data, u32 size);
};
struct AnimationClock {
    gptr<FrameController> mController;
    FrameController mDefault;
    BufferView mCursors;
    be<u32> mLastFrameBits;
    gptr<void> mResults;
    be<u32> mStorage;
    gptr<void> mVtable;
    void setRange(s32 end, u32 repeat);
};
f32 clampFrame(f32 value, f32 start, f32 end);
f32 wrapFrame(f32 value, f32 start, f32 end);
WWHD_SIZE(TickValue, 8);
WWHD_OFFSET(TimedCounter, mVtable, 0x18);
WWHD_OFFSET(TimedCounter, mCount, 0x1C);
WWHD_OFFSET(TimedCounter, mRate, 0x20);
WWHD_OFFSET(TimedCounter, mStart, 0x28);
WWHD_OFFSET(TimedCounter, mEnd, 0x30);
WWHD_OFFSET(TimedCounter, mActive, 0x38);
WWHD_SIZE(TimedCounter, 0x40);
WWHD_OFFSET(FrameController, mPolicy, 0x10);
WWHD_OFFSET(FrameController, mCycle, 0x14);
WWHD_SIZE(FrameController, 0x18);
WWHD_OFFSET(BindingTable, mSourceCount, 8);
WWHD_OFFSET(BindingTable, mTargetCount, 0xA);
WWHD_SIZE(BindingTable, 0xC);
WWHD_OFFSET(BufferView, mSize, 4);
WWHD_SIZE(BufferView, 0xC);
WWHD_OFFSET(AnimationClock, mDefault, 4);
WWHD_OFFSET(AnimationClock, mCursors, 0x1C);
WWHD_OFFSET(AnimationClock, mLastFrameBits, 0x28);
WWHD_OFFSET(AnimationClock, mResults, 0x2C);
WWHD_OFFSET(AnimationClock, mStorage, 0x30);
WWHD_OFFSET(AnimationClock, mVtable, 0x34);
WWHD_SIZE(AnimationClock, 0x38);
} }
