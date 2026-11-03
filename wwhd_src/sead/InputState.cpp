// WWHD controller input state. All behavior and layouts reconstructed from the binary.
#include "sead/InputState.h"

void* allocateInputObject(u32 size);
u32 inputAtan(f32);
void moveInputBytes(void*, const void*, u32, u32);

static sead::InputPair sInvalidPointer;
static sead::InputPair sInvalidFloatPointer;
static sead::InputPair sIdleStick;

static u32 buttonBit(u32 bit) {
    // PowerPC variable shifts inspect six count bits, producing zero for counts 32..63.
    bit &= 63;
    return bit < 32 ? 1u << bit : 0;
}

template <class T>
static T* inputStorage(T* object, u32 size) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocateInputObject(size));
    return object;
}

static void clearInputWord(be<u32>* word) {
    word = inputStorage(word, 4);
    if (word != NULL) *word = 0;
}

/* 0273BBD0 */
sead::InputState* constructInputState(sead::InputState* state, s32 buttons,
                                           s32 stickBit, s32 crossBit, u32 directionFlags) {
    state = inputStorage(state, sizeof(sead::InputState));
    if (state == NULL) return NULL;
    clearInputWord(&state->mRelease);
    clearInputWord(&state->mTrigger);
    clearInputWord(&state->mRepeat);
    clearInputWord(&state->mPointerState);
    state->mPointer.mX = (u32)sInvalidPointer.mX;
    state->mPointer.mY = (u32)sInvalidPointer.mY;
    sead::InputRect* bounds = inputStorage(&state->mPointerBounds, sizeof(sead::InputRect));
    if (bounds != NULL) {
        bounds->mMinX = 3.4028234663852886e38f;
        bounds->mMaxX = -3.4028234663852886e38f;
        bounds->mMinY = 3.4028234663852886e38f;
        bounds->mMaxY = -3.4028234663852886e38f;
    }
    state->mPointerBit = directionFlags;
    state->mStickThreshold = 0.5f;
    state->mCrossReleaseThreshold = 0.25f;
    state->mIdleFrames = 0;
    state->mStickReleaseThreshold = 0.25f;
    state->mButtonCount = buttons;
    state->mCrossThreshold = 0.5f;
    state->mCrossBit = crossBit;
    state->mStickBit = stickBit;
    clearInputWord(&state->mButtons);
    state->mPointerFloat.mX = (u32)sInvalidFloatPointer.mX;
    state->mPointerFloat.mY = (u32)sInvalidFloatPointer.mY;
    sead::InputPair* stick = inputStorage(reinterpret_cast<sead::InputPair*>(&state->mStickX), 8);
    if (stick != NULL) {
        stick->mY = 0;
        stick->mX = 0;
    }
    sead::InputPair* cross = inputStorage(reinterpret_cast<sead::InputPair*>(&state->mCrossX), 8);
    if (cross != NULL) {
        cross->mY = 0;
        cross->mX = 0;
    }
    state->mLeftTrigger = 0.0f;
    state->mRightTrigger = 0.0f;
    if (buttons > 32) state->mButtonCount = 32;
    for (u32 index = 0; index < 32; ++index) {
        state->mRepeatStart[index] = 30;
        state->mRepeatStep[index] = 1;
        state->mRepeatCounts[index] = 0;
    }
    return state;
}

/* 0273BDF4 */
void sead::InputState::configureRepeat(u32 mask, u8 start, u8 step) {
    s32 count = mButtonCount;
    for (s32 index = 0; index < count; ++index) {
        if (mask & buttonBit(index)) {
            mRepeatStart[index] = start;
            mRepeatStep[index] = step;
            count = mButtonCount;
        }
    }
}

/* 0273C310 */
u32 sead::InputState::directionMask() {
    s32 first = mStickBit;
    u32 mask = 0;
    if (first >= 0) {
        mask = buttonBit(first) | buttonBit((u32)first + 1) |
               buttonBit((u32)first + 2) | buttonBit((u32)first + 3);
    }
    first = mCrossBit;
    if (first >= 0) {
        mask |= buttonBit(first) | buttonBit((u32)first + 1) |
                buttonBit((u32)first + 2) | buttonBit((u32)first + 3);
    }
    return mask;
}

/* 0273C5A8 */
u32 sead::InputState::isIdle() {
    if (mButtons != 0 || ((u32)mPointerState & 1)) return 0;
    f32 idleX = reinterpret_cast<be<f32>&>(sIdleStick.mX);
    if ((f32)mStickX != idleX) return 0;
    f32 idleY = reinterpret_cast<be<f32>&>(sIdleStick.mY);
    if ((f32)mStickY != idleY || (f32)mCrossX != idleX || (f32)mCrossY != idleY) return 0;
    if ((f32)mLeftTrigger != 0.0f || (f32)mRightTrigger != 0.0f) return 0;
    return 1;
}

/* 0273C62C */
void sead::InputState::clear() {
    s32 count = mButtonCount;
    mRelease = 0;
    mTrigger = 0;
    mButtons = 0;
    mRepeat = 0;
    mPointerState = 0;
    for (s32 index = 0; index < count; ++index) {
        mRepeatCounts[index] = 0;
        count = mButtonCount;
    }
    mPointerFloat.mX = (u32)sInvalidFloatPointer.mX;
    mPointerFloat.mY = (u32)sInvalidFloatPointer.mY;
    mPointer.mX = (u32)sInvalidPointer.mX;
    mPointer.mY = (u32)sInvalidPointer.mY;
    mStickX = 0.0f;
    mCrossX = 0.0f;
    mRightTrigger = 0.0f;
    mCrossY = 0.0f;
    mStickY = 0.0f;
    mLeftTrigger = 0.0f;
}

/* 0273BB88 */
static u32 controllerIsIdle(sead::InputState* state) { return state->isIdle(); }

/* 0273D2C8 */
static void wrapperClear(sead::InputState* state) { state->clear(); }

/* 0273D2CC */
static u32 wrapperIsIdle(sead::InputState* state) { return state->isIdle(); }

/* 0273BBC8 */
static u32 controllerIsConnected() { return 1; }

/* 0273C6B8 */
static void initializeInvalidInputPoints() {
    sInvalidFloatPointer.mX = 0xFF7FFFFF;
    sInvalidPointer.mX = 0x80000000;
    sInvalidFloatPointer.mY = 0xFF7FFFFF;
    sInvalidPointer.mY = 0x80000000;
}

/* 0273D1EC */
static void copyWrapperRepeat(sead::InputState* destination, const sead::InputState* source) {
    moveInputBytes(destination->mRepeatStart, source->mRepeatStart, 32, 0);
    moveInputBytes(destination->mRepeatStep, source->mRepeatStep, 32, 0);
}

/* 0273BE34 */
void sead::InputState::updatePointer(u32 valid, u32 contact, const InputPair* point) {
    u32 state;
    s32 bit;
    if (valid != 0) {
        f32 minX = mPointerBounds.mMinX;
        f32 maxX = mPointerBounds.mMaxX;
        bool bounded = !(minX > maxX);
        f32 minY = 0.0f;
        f32 maxY = 0.0f;
        if (bounded) {
            minY = mPointerBounds.mMinY;
            maxY = mPointerBounds.mMaxY;
            bounded = !(minY > maxY);
        }
        if (bounded) {
            f32 x = reinterpret_cast<const be<f32>&>(point->mX);
            bool inside = !(minX > x) && !(x > maxX);
            if (inside) {
                f32 y = reinterpret_cast<const be<f32>&>(point->mY);
                inside = !(minY > y) && !(y > maxY);
            }
            if (inside) {
                reinterpret_cast<be<f32>&>(mPointerFloat.mX) = gabi::fsubs_ppc(x,minX);
                f32 originY = mPointerBounds.mMinY;
                f32 y = reinterpret_cast<const be<f32>&>(point->mY);
                state = mPointerState;
                f32 relativeY = gabi::fsubs_ppc(y,originY);
                bit = mPointerBit;
                reinterpret_cast<be<f32>&>(mPointerFloat.mY) = relativeY;
                state |= 1;
                mPointerState = state;
            } else {
                state = (u32)mPointerState & ~1u;
                valid = 0;
                bit = mPointerBit;
                mPointerState = state;
            }
        } else {
            mPointerFloat.mX = (u32)point->mX;
            bit = mPointerBit;
            mPointerFloat.mY = (u32)point->mY;
            state = (u32)mPointerState | 1;
            mPointerState = state;
        }
    } else {
        state = (u32)mPointerState & ~1u;
        bit = mPointerBit;
        mPointerState = state;
    }
    if (bit >= 0) {
        u32 mask = buttonBit(bit);
        u32 buttons = mButtons;
        mButtons = valid != 0 && contact != 0 ? buttons | mask : buttons & ~mask;
        state = mPointerState;
    }
    if ((state & 8) == 0) return;
    if ((state & 1) == 0) {
        mPointerFloat.mX = (u32)sInvalidFloatPointer.mX;
        mPointerFloat.mY = (u32)sInvalidFloatPointer.mY;
        state = mPointerState;
    }
    mPointerState = state & ~8u;
}

/* 0273BF88 */
u32 sead::InputState::stickDirections(u32 previous, const InputPair* stick, u32 firstBit,
                                      f32 onThreshold, f32 offThreshold) {
    f32 y = reinterpret_cast<const be<f32>&>(stick->mY);
    f32 x = reinterpret_cast<const be<f32>&>(stick->mX);
    f32 squared = gabi::fmadds(x,x,gabi::fmuls_ppc(y,y));
    f32 magnitude;
    if (squared > 0.0f) {
        f64 estimate = gabi::frsqrte(squared);
        // The estimate remains double; Espresso rounds only the multiplier to25 bits.
        f32 squareInverse = (f32)(estimate * round25(estimate));
        f32 halfInverse = (f32)(estimate * 0.5);
        f32 inverse;
        f32 correction = gabi::fnmsubs(squareInverse,squared,3.0f);
        inverse = gabi::fmuls_ppc(correction,halfInverse);
        magnitude = gabi::fmuls_ppc(inverse,squared);
        if (magnitude < offThreshold) return 0;
    } else {
        magnitude = gabi::fmuls_ppc(0.0f,squared);
        if (magnitude < offThreshold) return 0;
    }
    if (magnitude < onThreshold) {
        u32 directions = buttonBit(firstBit) | buttonBit(firstBit + 1) |
                         buttonBit(firstBit + 2) | buttonBit(firstBit + 3);
        if ((previous & directions) == 0) return 0;
    }
    x = reinterpret_cast<const be<f32>&>(stick->mX);
    y = reinterpret_cast<const be<f32>&>(stick->mY);
    u32 angle = 0;
    if (x == 0.0f && y == 0.0f) return buttonBit(firstBit + 3);
    if (!(x < 0.0f)) {
        if (!(y < 0.0f)) {
            if (!(x < y)) angle = inputAtan(gabi::fdivs_ppc(y,x));
            else angle = 0x40000000u - inputAtan(gabi::fdivs_ppc(x,y));
        } else {
            if (!(x < -y)) angle = 0u - inputAtan(-(gabi::fdivs_ppc(y,x)));
            else angle = inputAtan(-(gabi::fdivs_ppc(x,y))) - 0x40000000u;
        }
    } else {
        if (!(y < 0.0f)) {
            if (!(-x < y)) angle = 0x80000000u - inputAtan(-(gabi::fdivs_ppc(y,x)));
            else angle = inputAtan(-(gabi::fdivs_ppc(x,y))) + 0x40000000u;
        } else {
            if (!(x > y)) angle = inputAtan(gabi::fdivs_ppc(y,x)) + 0x80000000u;
            else angle = 0xC0000000u - inputAtan(gabi::fdivs_ppc(x,y));
        }
    }
    if (angle < 0x10000000u || angle >= 0xF0000000u) return buttonBit(firstBit + 3);
    if (angle < 0x30000000u) return buttonBit(firstBit) | buttonBit(firstBit + 3);
    if (angle < 0x50000000u) return buttonBit(firstBit);
    if (angle < 0x70000000u) return buttonBit(firstBit) | buttonBit(firstBit + 2);
    if (angle < 0x90000000u) return buttonBit(firstBit + 2);
    if (angle < 0xB0000000u) return buttonBit(firstBit + 2) | buttonBit(firstBit + 1);
    if (angle < 0xD0000000u) return buttonBit(firstBit + 1);
    return buttonBit(firstBit + 3) | buttonBit(firstBit + 1);
}

/* 0273C390 */
void sead::InputState::update(u32 previous, u32 previousPointer) {
    s32 first = mStickBit;
    u32 directions = 0;
    if (first >= 0) {
        f32 offThreshold = mStickReleaseThreshold;
        f32 onThreshold = mStickThreshold;
        directions = stickDirections(previous,reinterpret_cast<InputPair*>(&mStickX),first,onThreshold,offThreshold);
    }
    first = mCrossBit;
    if (first >= 0) {
        f32 offThreshold = mCrossReleaseThreshold;
        f32 onThreshold = mCrossThreshold;
        directions |= stickDirections(previous,reinterpret_cast<InputPair*>(&mCrossX),first,onThreshold,offThreshold);
    }
    u32 mask = directionMask();
    u32 buttons = ((u32)mButtons & ~mask) | directions;
    mButtons = buttons;
    mTrigger = previous & ~buttons;
    s32 count = mButtonCount;
    mRelease = buttons & ~previous;
    mRepeat = 0;
    for (s32 index = 0; index < count; ++index) {
        u32 bit = buttonBit(index);
        if ((u32)mButtons & bit) {
            u32 interval = mRepeatStep[index];
            u32 frames;
            if (interval != 0) {
                frames = mRepeatCounts[index];
                u32 delay = mRepeatStart[index];
                if (frames >= delay && (frames == delay || (frames - delay) % interval == 0)) {
                    mRepeat = (u32)mRepeat | bit;
                }
            }
            frames = mRepeatCounts[index];
            mRepeatCounts[index] = frames + 1;
        } else mRepeatCounts[index] = 0;
        count = mButtonCount;
    }
    u32 state = mPointerState;
    if (previousPointer == 0 && (state & 1) != 0) state = (state | 2) & ~4u;
    else {
        state &= ~2u;
        if (previousPointer != 0 && (state & 1) == 0) state |= 4;
        else state &= ~4u;
    }
    f32 x = reinterpret_cast<be<f32>&>(mPointerFloat.mX);
    f32 y = reinterpret_cast<be<f32>&>(mPointerFloat.mY);
    mPointerState = state;
    s32 ix = gabi::ftoi(x);
    s32 iy = gabi::ftoi(y);
    mPointer.mX = ix;
    mPointer.mY = iy;
}

/* WWHD glue
 data sInvalidPointer 1049FFA8
 data sInvalidFloatPointer 1049FFA0
 data sIdleStick 104A0908
 ext 02756D88 u32 inputAtan(f32)
 ext 0273AD10 void* allocateInputObject(u32)
 ext C0009988 void moveInputBytes(void*, const void*, u32, u32)
*/
