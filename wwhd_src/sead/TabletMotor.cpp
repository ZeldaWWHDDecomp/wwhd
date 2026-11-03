// WWHD tablet motor sequence playback, reconstructed from local instructions.
#include "sead/TabletMotor.h"

static MotorSpan sMotorTickRate;
void clearMotorPattern(void*,u32,u32);
void playMotorPattern(u32,void*,u8);
void stopTabletMotor(u32);
Pair32 motorSystemTime();
Pair32 divideMotorTicks(u32,u32,u32,u32);
void setMotorDuration(void*,MotorSpan*);
Pair32 motorRemaining(void*);

static void setPatternDuration(PadMotor* motor) {
    u32 factor = (u32)motor->mLength * 0x208Du;
    u32 lowRate = sMotorTickRate.mLow;
    u32 highRate = sMotorTickRate.mHigh;
    u64 product = (u64)factor * lowRate;
    u32 high = (product >> 32) + factor * highRate;
    auto divided = divideMotorTicks(high,product,0,1000);
    divided = divideMotorTicks(divided.r3,divided.r4,0,1000);
    gabi::Local<MotorSpan> duration;
    duration->mHigh = divided.r3;
    duration->mLow = divided.r4;
    setMotorDuration(motor->mDeadline,duration.get());
}

/* 0273EB3C */
static u32 updateTabletMotor(PadMotor* motor) {
    be<u8>* pattern = motor->mPattern;
    u32 frames = motor->mFrames;
    if (pattern == NULL) {
        if (frames != 0) {
            stopTabletMotor(0);
            motor->mFrames = 0;
        }
        return 0;
    }
    if (frames == 0) {
        u32 length = 0;
        clearMotorPattern(motor->mBits,0,15);
        pattern = motor->mPattern;
        u32 character = pattern[0];
        while (character != 0) {
            if (character == '*') {
                u32 index = length >> 3;
                motor->mBits[index] = (u8)motor->mBits[index] | (1u << (length & 7));
            }
            ++length;
            if (length == 120) break;
            pattern = motor->mPattern;
            character = pattern[length];
        }
        motor->mLength = length;
        playMotorPattern(0,motor->mBits,length);
        auto time = motorSystemTime();
        motor->mDeadline[1] = time.r4;
        motor->mDeadline[0] = time.r3;
        setPatternDuration(motor);
    }
    auto remaining = motorRemaining(motor->mDeadline);
    if ((s32)remaining.r3 >= 0) {
        u32 repeats = motor->mRepeats;
        motor->mFrames = 0;
        if (repeats != 0) {
            if (repeats == 1) {
                motor->mPattern = NULL;
                motor->mRepeats = 0;
                return 0;
            }
            motor->mRepeats = repeats - 1;
        }
        u8 length = reinterpret_cast<be<u8>*>(&motor->mLength)[3];
        playMotorPattern(0,motor->mBits,length);
        auto time = motorSystemTime();
        motor->mDeadline[0] = time.r3;
        motor->mDeadline[1] = time.r4;
        setPatternDuration(motor);
    }
    motor->mFrames = (u32)motor->mFrames + 1;
    return 0;
}

/* WWHD glue
 data sMotorTickRate 104A11C8
 ext C0009990 void clearMotorPattern(void*,u32,u32)
 ext C0007510 void playMotorPattern(u32,void*,u8)
 ext C0007720 void stopTabletMotor(u32)
 ext C0009C98 Pair32 motorSystemTime()
 ext 028F5B8C Pair32 divideMotorTicks(u32,u32,u32,u32)
 ext 02760E94 void setMotorDuration(void*,MotorSpan*)
 ext 02760E58 Pair32 motorRemaining(void*)
*/
