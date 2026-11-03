/* Graphics utility counters, frame policies and index bindings. Binary-only reconstruction. */
#include "agl/gx2/RuntimeUtility.h"
using namespace agl::gx2util;

extern void* graphicsAllocate(u32 size);
extern void graphicsDelete(void* object);
extern void graphicsPropertyConstruct(TimedCounter* object);
extern void graphicsPropertyDestroy(TimedCounter* object, u32 flags);
extern void graphicsPropertyCopy(TimedCounter* object, TimedCounter* destination);
extern void graphicsTickConstruct(TickValue* value);
static u32 sCounterVtable;
static u32 sClockVtable;

/* 027DA694 */
static TimedCounter* constructTimedCounter(TimedCounter* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<TimedCounter*>(graphicsAllocate(sizeof(TimedCounter)));
    if (object != NULL) {
        graphicsPropertyConstruct(object);
        object->mCount = 0;
        object->mRate = 0;
        object->mVtable = &sCounterVtable;
        graphicsTickConstruct(&object->mStart);
        graphicsTickConstruct(&object->mEnd);
        object->mActive = 0;
    }
    return object;
}

/* 027DA710 */
static void destroyTimedCounter(TimedCounter* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) return;
    graphicsPropertyDestroy(object, 0);
    if (flags & 1) graphicsDelete(object);
}

/* 027DA764 */
void agl::gx2util::TimedCounter::copyTo(TimedCounter* destination) {
    gabi::AutoFrame frame{gabi::current_function()};
    graphicsPropertyCopy(this, destination);
    u32 high = mStart.mHigh;
    u32 low = mStart.mLow;
    destination->mStart.mHigh = high;
    destination->mStart.mLow = low;
    high = mEnd.mHigh;
    low = mEnd.mLow;
    destination->mEnd.mHigh = high;
    destination->mEnd.mLow = low;
    destination->mCount = mCount;
    destination->mActive = mActive;
    destination->mRate = mRate;
}

/* 027DA7D4 */
u32 agl::gx2util::TimedCounter::difference(const TimedCounter* other) const {
    gabi::AutoFrame frame{gabi::current_function()};
    return (u32)mCount - (u32)other->mCount;
}

/* 027DA7E4 */
static FrameController* constructFrameController(FrameController* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<FrameController*>(graphicsAllocate(sizeof(FrameController)));
    if (object != NULL) {
        object->mCycle = 0;
        object->mStart = 0.0f;
        object->mPolicy = gabi::guest_fn((const void*)&clampFrame);
        object->mCurrent = 0.0f;
        object->mEnd = 0.0f;
        object->mRate = 1.0f;
    }
    return object;
}

/* 027DA84C */
void agl::gx2util::FrameController::configure(u32 policy, f32 start, f32 end) {
    gabi::AutoFrame frame{gabi::current_function()};
    mStart = start;
    mCycle = 0;
    mCurrent = 0.0f;
    mEnd = end;
    mPolicy = policy;
    mRate = 1.0f;
}

/* 027DA87C */
void agl::gx2util::BindingTable::configure(be<u32>* slots, u32 capacity) {
    gabi::AutoFrame frame{gabi::current_function()};
    mSlots = slots;
    mFlags = 0;
    mTargetCount = 0;
    mCapacity = capacity;
    mSourceCount = 0;
}

/* 027DA898 */
void agl::gx2util::BindingTable::reset(u32 targetCount) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mSourceCount;
    targetCount = (u16)targetCount;
    if (targetCount > count) count = targetCount;
    mTargetCount = targetCount;
    for (u32 index = 0; index < count; ++index) mSlots[index] = 0xFFFFFFFF;
}

/* 027DA8D8 */
void agl::gx2util::BindingTable::setTargets(const be<u16>* targets) {
    gabi::AutoFrame frame{gabi::current_function()};
    enum : u32 { InvalidTarget = 0x7FFF, TargetMask = 0x7FFF, SourceMask = 0x3FFF8000 };
    s32 count = mSourceCount;
    for (s32 source = 0; source < count; ++source) {
        u32 target = targets[source];
        if (target < InvalidTarget) {
            mSlots[source] = (u32)mSlots[source] & SourceMask;
            mSlots[source] = (u32)mSlots[source] | (target & TargetMask);
            mSlots[target] = (u32)mSlots[target] & ~SourceMask;
            mSlots[target] = (u32)mSlots[target] | (((u32)source << 15) & SourceMask);
            count = mSourceCount;
        }
    }
}

/* 027DA960 */
void agl::gx2util::BufferView::configure(void* data, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    mData = data;
    mSize = data == NULL ? 0 : size;
    mCount = 0;
}

/* 027DA984 */
static AnimationClock* constructAnimationClock(AnimationClock* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<AnimationClock*>(graphicsAllocate(sizeof(AnimationClock)));
    if (object != NULL) {
        object->mVtable = &sClockVtable;
        constructFrameController(&object->mDefault);
        object->mStorage = 0;
        object->mController = &object->mDefault;
    }
    return object;
}

struct IntegerConversionWords { be<u32> mExponent; be<u32> mEncoded; };
WWHD_OFFSET(IntegerConversionWords, mExponent, 0);
WWHD_OFFSET(IntegerConversionWords, mEncoded, 4);
WWHD_SIZE(IntegerConversionWords, 8);

/* 027DA9E8 */
f32 agl::gx2util::wrapFrame(f32 value, f32 start, f32 end) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 range, distance, base, direction;
    if (!(value < end)) {
        direction = 1.0f;
        range = gabi::fsubs_ppc(end, start);
        distance = gabi::fsubs_ppc(value, end);
        base = start;
    } else {
        if (!(value < start)) return value;
        direction = -1.0f;
        range = gabi::fsubs_ppc(end, start);
        distance = gabi::fsubs_ppc(start, value);
        base = end;
    }
    if (range == 0.0f) return start;
    f32 quotient = gabi::fdivs_ppc(distance, range);
    s32 wholeCycles = gabi::ftoi(quotient);
    gabi::FrameLocal<IntegerConversionWords> conversionStorage(0x08);
    conversionStorage->mExponent = 0x43300000;
    conversionStorage->mEncoded = (u32)wholeCycles ^ 0x80000000;
    f32 cycles = (f32)wholeCycles;
    f32 remainder = gabi::fnmsubs(range, cycles, distance);
    return gabi::fmadds(direction, remainder, base);
}

/* 027DAAA0 */
f32 agl::gx2util::clampFrame(f32 value, f32 start, f32 end) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!(value < end)) return end;
    f32 difference = gabi::fsubs_ppc(start, value);
    return difference >= 0.0f ? start : value;
}

/* 027DAAB8 */
void agl::gx2util::AnimationClock::setRange(s32 end, u32 repeat) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 policy = repeat != 0 ? gabi::guest_fn((const void*)&wrapFrame) : gabi::guest_fn((const void*)&clampFrame);
    gabi::FrameLocal<IntegerConversionWords> conversionStorage(0x08);
    conversionStorage->mExponent = 0x43300000;
    conversionStorage->mEncoded = (u32)end ^ 0x80000000;
    mDefault.configure(policy, 0.0f, (f32)end);
}

/* WWHD glue
 data sCounterVtable 1016C480 nocheck
 data sClockVtable 1016C4C0 nocheck
 ext 0273AD10 void* graphicsAllocate(u32)
 ext 0273AF40 void graphicsDelete(void*)
 ext 027C05F4 void graphicsPropertyConstruct(agl::gx2util::TimedCounter*)
 ext 027C06C4 void graphicsPropertyDestroy(agl::gx2util::TimedCounter*, u32)
 ext 027C0720 void graphicsPropertyCopy(agl::gx2util::TimedCounter*, agl::gx2util::TimedCounter*)
 ext 02760DE8 void graphicsTickConstruct(agl::gx2util::TickValue*)
*/
