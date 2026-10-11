/* Camera shake and controller rumble. */
#include "d/d_vibration_state.h"
// HD: six-bit PPC variable shifts, including the zero result for bit 5.
static u32 ppcShiftLeft(u32 v, u32 n) { return (n & 32) ? 0 : v << (n & 31); }
static u32 ppcShiftRight(u32 v, u32 n) { return (n & 32) ? 0 : v >> (n & 31); }
static const f32 unsignedThreshold = 2147483648.0f;
static const f32 piMin = -3.1415927410125732f;
static const f32 piMax = 3.1415927410125732f;
enum : u32 { vibrationVtable = 0x10056530 };
#define platform (*gabi::at<gptr<VibrationPlatform>>(0x101F5088))
#define saveInfo (*gabi::at<gptr<VibrationSave>>(0x101F84DC))
#define motorData (*gabi::at<VibrationMotorData>(0x104872A8))
static const vib_pattern CS_patt[26] = {
    {0, 0, 0x00000000},  {0, 3, 0xC0000000},  {0, 4, 0xE0000000}, {0, 6, 0xE8000000},
    {0, 7, 0xEC000000},  {0, 8, 0xF6000000},  {0, 9, 0xFB000000}, {0, 10, 0xF6800000},
    {0, 11, 0xFB400000}, {0, 3, 0xE0000000},  {0, 5, 0xF8000000}, {0, 23, 0xE0202000},
    {0, 13, 0xF0380000}, {0, 13, 0xF0380000}, {0, 3, 0xE0000000}, {0, 8, 0xD0000000},
    {0, 7, 0xE0000000},  {0, 8, 0xF0000000},  {0, 5, 0xF8000000}, {0, 2, 0xC0000000},
    {0, 8, 0xF5000000},  {0, 12, 0xF8D00000}, {0, 6, 0xC0000000}, {0, 9, 0xE8000000},
    {0, 10, 0xF4000000}, {0, 15, 0xFAA00000}};
static const vib_pattern MS_patt[26] = {
    {0, 0, 0x00000000},  {0, 3, 0xC0000000},  {0, 4, 0xE0000000}, {0, 6, 0xE8000000},
    {0, 7, 0xEC000000},  {0, 8, 0xF6000000},  {0, 9, 0xFB000000}, {0, 10, 0xF6800000},
    {0, 11, 0xFB400000}, {0, 3, 0xE0000000},  {0, 5, 0xF8000000}, {0, 23, 0xE0202000},
    {0, 13, 0xF0380000}, {0, 13, 0xF0380000}, {0, 3, 0xE0000000}, {0, 8, 0xD0000000},
    {0, 7, 0xE0000000},  {0, 8, 0xF0000000},  {0, 5, 0xF8000000}, {0, 2, 0xC0000000},
    {0, 8, 0xF5000000},  {0, 12, 0xF8D00000}, {0, 6, 0xC0000000}, {0, 9, 0xE8000000},
    {0, 10, 0xF4000000}, {0, 15, 0xFAA00000}};
static const vib_pattern CQ_patt[12] = {
    {0, 0, 0x00000000},  {1, 32, 0x10202020}, {1, 32, 0x42104208}, {2, 32, 0x22222222},
    {2, 32, 0x22492249}, {3, 32, 0x52525252}, {3, 32, 0x4A94AA95}, {4, 32, 0x36555655},
    {4, 32, 0x6B6D6B6D}, {0, 3, 0x80000000},  {0, 3, 0xC0000000},  {0, 32, 0xAAAAA000}};
static const vib_pattern MQ_patt[12] = {
    {0, 0, 0x00000000},  {1, 32, 0x10202020}, {1, 32, 0x42104208}, {2, 32, 0x22222222},
    {2, 32, 0x22492249}, {3, 32, 0x52525252}, {3, 32, 0x4A94AA95}, {4, 32, 0x36555655},
    {4, 32, 0x6B6D6B6D}, {0, 3, 0x80000000},  {0, 3, 0xC0000000},  {0, 32, 0xAAAAA000}};

/* 025CB898 */
// HD: controller mode expands each rumble bit to a nibble and permits 120 bits.
VibrationMotorData *makedata(VibrationMotorData *dest, u32 pattern, u32 length) {
    u32 state = (u32)platform->mExpandedMotorPattern;
    if (!state) {
        dest->length = u16(length);
        dest->words[0] = u16(pattern >> 16);
        dest->words[6] = 0;
        dest->words[5] = 0;
        dest->words[2] = 0;
        dest->words[7] = 0;
        dest->words[1] = u16(pattern);
        dest->words[4] = 0;
        dest->words[3] = 0;
    } else {
        u32 expanded = (length << 2) & 0xFFFC;
        dest->length = u16(expanded > 120 ? 120 : expanded);
        for (u32 group = 0; group < 8; ++group) {
            u16 packed = 0;
            for (u32 bit = 0; bit < 4; ++bit)
                if (pattern & (u32(1) << (31 - group * 4 - bit)))
                    packed |= u16(0xF000 >> (bit * 4));
            dest->words[group] = packed;
        }
    }
    return dest;
}

/* 025CB878 */
u32 rollshift(u32 pattern, u32 length, u32 frame) {
    u32 quotient = ppc_divw(frame, length), pos = frame - quotient * length;
    return ppcShiftRight(pattern, pos) | ppcShiftLeft(pattern, length - pos);
}

/* 025CB4A8 */
u32 makebits(u32 pattern, u32 length, u32 desired) {
    u32 masked = pattern & ppcShiftLeft(~u32(0), 32 - length), result = masked;
    for (u32 i = length; s32(i) < s32(desired);) {
        i += length;
        result = ppcShiftRight(result, length) | masked;
    }
    return result;
}

/* 025CB77C */
// HD: retain double multiplication and the unsigned conversion correction.
u32 randombit(s32 rounds, s32 length) {
    u32 pattern = 0;
    if (rounds <= 0)
        return pattern;
    f32 size = f32(length), threshold = unsignedThreshold;
    for (s32 i = 0; i < rounds; ++i) {
        f32 v = f32(f64(cM_rnd()) * f64(size));
        u32 n;
        if (v < threshold)
            n = u32(gabi::ftoi(v));
        else
            n = u32(gabi::ftoi(f32(f64(v) - f64(threshold)))) + 0x80000000;
        pattern |= (n & 32) ? 0 : (u32(0x40000000) >> (n & 31));
    }
    return pattern;
}

/* 025CBB90 */
s32 dVibrationState_c::Run() {
    VibrationPatternValue pattern;
    cXyz norm;
    cXyz sum;
    mRumbleState = RUMBLE_STATE_RUNNING;
    s32 option = checkVibration(&saveInfo->mConfig);
    if (option != 1) {
        mMotor.mQuake.mPatternIdx = PATTERN_OFF;
        mMotor.mShock.mCurrentFrame = RESET_FRAME;
        mMotor.mShock.mPatternIdx = PATTERN_OFF;
        mMotor.mQuake.mCurrentFrame = RESET_FRAME;
    }
    s32 shockFrame = (s32)mCamera.mShock.mCurrentFrame;
    if (shockFrame == 0 || (s32)mCamera.mQuake.mCurrentFrame == 0) {
        u32 mode = 0;
        s32 quakeIndex = (s32)mCamera.mQuake.mPatternIdx;
        if ((s32)mCamera.mShock.mPatternIdx == PATTERN_OFF)
            mCamera.mShock.mCurrentFrame = RESET_FRAME;
        else if (shockFrame >= 0)
            mode = RUMBLE_SHOCK;
        if (quakeIndex == -1)
            mCamera.mQuake.mCurrentFrame = RESET_FRAME;
        else if ((s32)mCamera.mQuake.mCurrentFrame >= 0)
            mode |= RUMBLE_QUAKE;
        switch (mode) {
        case RUMBLE_SHOCK: {
            u32 length = (u32)mCamera.mShock.mLength, bits = (u32)mCamera.mShock.mPattern;
            bits |= randombit((s32)mCamera.mShock.mRounds, length);
            pattern.bits = bits;
            normalizeVector(&mCamera.mShock.mCoord, &norm);
            void *camera = dCam_getBody();
            StartCameraShake(camera, length, &pattern.bits, (u32)mCamera.mShock.mFlags, &norm);
            break;
        }
        case RUMBLE_QUAKE: {
            u32 length = (u32)mCamera.mQuake.mLength;
            u32 bits = rollshift((u32)mCamera.mQuake.mPattern, length, (u32)mFrameIdx);
            bits |= randombit((s32)mCamera.mQuake.mRounds, length);
            pattern.bits = bits;
            normalizeVector(&mCamera.mQuake.mCoord, &norm);
            void *camera = dCam_getBody();
            StartCameraShake(camera, length, &pattern.bits, (u32)mCamera.mQuake.mFlags, &norm);
            break;
        }
        case RUMBLE_SHOCK | RUMBLE_QUAKE: {
            u32 frame = (u32)mCamera.mShock.mCurrentFrame,
                bits = ppcShiftLeft((u32)mCamera.mShock.mPattern, frame),
                length = (u32)mCamera.mShock.mLength - frame;
            u32 extended =
                makebits((u32)mCamera.mQuake.mPattern, (u32)mCamera.mQuake.mLength, length);
            bits |= rollshift(extended, length, (u32)mFrameIdx);
            s32 rounds = (s32)mCamera.mQuake.mRounds, shockRounds = (s32)mCamera.mShock.mRounds;
            if (shockRounds > rounds)
                rounds = shockRounds;
            bits |= randombit(rounds, length);
            pattern.bits = bits;
            addVectors(&mCamera.mShock.mCoord, &sum, &mCamera.mQuake.mCoord);
            normalizeVector(&sum, &norm);
            void *camera = dCam_getBody();
            u32 flags = (u32)mCamera.mQuake.mFlags | (u32)mCamera.mShock.mFlags;
            StartCameraShake(camera, length, &pattern.bits, flags, &norm);
            mCamera.mQuake.mCurrentFrame = 0;
            mCamera.mShock.mCurrentFrame = 0;
            break;
        }
        default: {
            void *camera = dCam_getBody();
            StopCameraShake(camera);
            break;
        }
        }
    }
    s32 motorQuakeFrame = (s32)mMotor.mQuake.mCurrentFrame;
    if (motorQuakeFrame >= MAX_MOTOR_QUAKE_FRAME) {
        stopMotorWave(u32(0));
        stopMotor();
        mMotor.mQuake.mCurrentFrame = -1;
        motorQuakeFrame = -1;
    }
    s32 motorShockFrame = (s32)mMotor.mShock.mCurrentFrame;
    if (motorShockFrame == 0 || motorQuakeFrame == 0) {
        u32 mode = 0;
        s32 quakeIndex = (s32)mMotor.mQuake.mPatternIdx;
        if ((s32)mMotor.mShock.mPatternIdx == PATTERN_OFF)
            mMotor.mShock.mCurrentFrame = RESET_FRAME;
        else if (motorShockFrame >= 0)
            mode = RUMBLE_SHOCK;
        if (quakeIndex == -1)
            mMotor.mQuake.mCurrentFrame = RESET_FRAME;
        else if ((s32)mMotor.mQuake.mCurrentFrame >= 0)
            mode |= RUMBLE_QUAKE;
        switch (mode) {
        case RUMBLE_SHOCK: {
            u32 length = (u32)mMotor.mShock.mLength, rounds = (u32)mMotor.mShock.mRounds,
                bits = (u32)mMotor.mShock.mPattern;
            bits |= randombit(rounds, length);
            mMotor.mShock.mStopFrame = length;
            VibrationMotorData *data = makedata(&motorData, bits, length);
            startMotorWave(0, data, LOOP_ONCE);
            break;
        }
        case RUMBLE_QUAKE: {
            u32 length = (u32)mMotor.mQuake.mLength;
            u32 bits = rollshift((u32)mMotor.mQuake.mPattern, length, (u32)mFrameIdx);
            bits |= randombit((u32)mMotor.mQuake.mRounds, length);
            mMotor.mQuake.mStopFrame = INT32_MAX;
            VibrationMotorData *data = makedata(&motorData, bits, length);
            startMotorWave(0, data, LOOP_FOREVER);
            break;
        }
        case RUMBLE_SHOCK | RUMBLE_QUAKE: {
            u32 frame = (u32)mMotor.mShock.mCurrentFrame,
                length = (u32)mMotor.mShock.mLength - frame,
                bits = ppcShiftLeft((u32)mMotor.mShock.mPattern, frame);
            u32 extended =
                makebits((u32)mMotor.mQuake.mPattern, (u32)mMotor.mQuake.mLength, length);
            bits |= rollshift(extended, length, (u32)mFrameIdx);
            s32 rounds = (s32)mMotor.mQuake.mRounds, shockRounds = (s32)mMotor.mShock.mRounds;
            if (shockRounds > rounds)
                rounds = shockRounds;
            bits |= randombit(rounds, length);
            mMotor.mQuake.mStopFrame = length;
            mMotor.mQuake.mCurrentFrame = 0;
            mMotor.mShock.mStopFrame = length;
            mMotor.mShock.mCurrentFrame = 0;
            VibrationMotorData *data = makedata(&motorData, bits, length);
            startMotorWave(0, data, LOOP_ONCE);
            break;
        }
        default: {
            stopMotorWave(u32(0));
            stopMotor();
            mMotor.mShock.mStopFrame = RESET_FRAME;
            mMotor.mQuake.mStopFrame = RESET_FRAME;
            break;
        }
        }
    }
    u32 frame = (u32)mCamera.mShock.mCurrentFrame;
    if (s32(frame) >= -1) {
        ++frame;
        mCamera.mShock.mCurrentFrame = frame;
        if (s32(frame) > (s32)mCamera.mShock.mLength) {
            mCamera.mShock.mCurrentFrame = 0;
            mCamera.mShock.mPatternIdx = PATTERN_OFF;
        }
    }
    frame = (u32)mMotor.mShock.mCurrentFrame;
    if (s32(frame) >= -1) {
        ++frame;
        mMotor.mShock.mCurrentFrame = frame;
        if (s32(frame) > (s32)mMotor.mShock.mStopFrame) {
            mMotor.mShock.mCurrentFrame = 0;
            mMotor.mShock.mPatternIdx = PATTERN_OFF;
        }
    }
    frame = (u32)mCamera.mQuake.mCurrentFrame;
    if (s32(frame) >= -1) {
        ++frame;
        mCamera.mQuake.mCurrentFrame = frame;
        if (s32(frame) > (s32)mCamera.mQuake.mLength)
            mCamera.mQuake.mCurrentFrame = 0;
    }
    frame = (u32)mMotor.mQuake.mCurrentFrame;
    if (s32(frame) >= -1) {
        ++frame;
        mMotor.mQuake.mCurrentFrame = frame;
        if (s32(frame) > (s32)mMotor.mQuake.mStopFrame)
            mMotor.mQuake.mCurrentFrame = 0;
    }
    mFrameIdx = (u32)mFrameIdx + 1;
    return 1;
}

/* 025CB374 */
u8 dVibrationState_c::StartShock(u32 pattern, u32 flags, const cXyz *coord) {
    u8 result = 0;
    if (flags & MASK_CAMERA_SHAKE) {
        mCamera.mShock.mCurrentFrame = 0;
        mCamera.mShock.mFlags = flags;
        mCamera.mShock.mPatternIdx = pattern;
        mCamera.mShock.mCoord.copy(*coord);
        const vib_pattern *p = &CS_patt[pattern];
        mCamera.mShock.mPattern = (u32)p->pattern;
        mCamera.mShock.mLength = (u16)p->length;
        mCamera.mShock.mRounds = (u16)p->rounds;
        result = 1;
    }
    if (flags & FLAG_MOTOR_SHAKE) {
        const vib_pattern *p = &MS_patt[pattern];
        mMotor.mShock.mPatternIdx = pattern;
        mMotor.mShock.mCurrentFrame = 0;
        mMotor.mShock.mPattern = (u32)p->pattern;
        mMotor.mShock.mLength = (u16)p->length;
        result = 1;
    }
    return result;
}

/* 025CB408 */
u8 dVibrationState_c::StartQuake(u32 pattern, u32 flags, const cXyz *coord) {
    u8 result = 0;
    if (flags & MASK_CAMERA_SHAKE) {
        mCamera.mQuake.mPatternIdx = pattern;
        mCamera.mQuake.mCurrentFrame = 0;
        mCamera.mQuake.mFlags = flags;
        mCamera.mQuake.mCoord.copy(*coord);
        const vib_pattern *p = &CQ_patt[pattern];
        mCamera.mQuake.mPattern = (u32)p->pattern;
        mCamera.mQuake.mLength = (u16)p->length;
        mCamera.mQuake.mRounds = (u16)p->rounds;
        result = 1;
    }
    if (flags & FLAG_MOTOR_SHAKE) {
        const vib_pattern *p = &MQ_patt[pattern];
        mMotor.mQuake.mPatternIdx = pattern;
        mMotor.mQuake.mCurrentFrame = 0;
        mMotor.mQuake.mPattern = (u32)p->pattern;
        mMotor.mQuake.mLength = (u16)p->length;
        mMotor.mQuake.mRounds = (u16)CQ_patt[pattern].rounds;
        result = 1;
    }
    return result;
}

/* 025CB4E0 */
// HD: the custom StartQuake overload has a separate glue name; preserve its length assertion and byte loads.
u8 dVibrationState_c::StartQuakeCustom(const VibrationCustomPattern *pattern, u32 rounds, u32 flags,
                                       const cXyz *coord) {
    u32 length = pattern->length.value, bits = 0;
    u8 result = 0;
    if (length > 32)
        assertion("d_vibration.cpp", u32(0x1B3), "n <= 32");
    if (length) {
        bits = u32((u8)pattern->bits[0]) << 24;
        if (length >= 9) {
            bits |= u32((u8)pattern->bits[1]) << 16;
            if (length >= 17) {
                bits |= u32((u8)pattern->bits[2]) << 8;
                if (length >= 25)
                    bits |= (u8)pattern->bits[3];
            }
        }
    }
    if (flags & MASK_CAMERA_SHAKE) {
        mCamera.mQuake.mPatternIdx = 0;
        mCamera.mQuake.mCurrentFrame = 0;
        mCamera.mQuake.mFlags = flags;
        mCamera.mQuake.mCoord.copy(*coord);
        u32 extended = makebits(bits, u32((u8)pattern->length.bytes.low), u32(32));
        mCamera.mQuake.mLength = 32;
        mCamera.mQuake.mRounds = rounds;
        mCamera.mQuake.mPattern = extended;
        result = 1;
    }
    if (flags & FLAG_MOTOR_SHAKE) {
        mMotor.mQuake.mPatternIdx = 0;
        mMotor.mQuake.mCurrentFrame = 0;
        u32 extended = makebits(bits, u32((u8)pattern->length.bytes.low), u32(32));
        mMotor.mQuake.mRounds = rounds;
        mMotor.mQuake.mLength = 32;
        mMotor.mQuake.mPattern = extended;
        result = 1;
    }
    return result;
}

/* 025CB610 */
s32 dVibrationState_c::StopQuake(u32 flags) {
    s32 result = 0;
    if (flags & MASK_CAMERA_SHAKE) {
        u32 remaining = (u32)mCamera.mQuake.mFlags & ~flags;
        mCamera.mQuake.mFlags = remaining;
        if (!remaining)
            mCamera.mQuake.mPatternIdx = PATTERN_OFF;
        mCamera.mQuake.mCurrentFrame = 0;
        result = 1;
    }
    if ((flags & FLAG_MOTOR_SHAKE) && (s32)mMotor.mQuake.mPatternIdx != PATTERN_OFF) {
        mMotor.mQuake.mCurrentFrame = 0;
        mMotor.mQuake.mPatternIdx = PATTERN_OFF;
        result = 1;
    }
    return result;
}

/* 025CB694 */
// HD: both hard-stop operations resolve to the same function and are called separately.
void dVibrationState_c::Kill() {
    stopMotorWave(u32(0));
    stopMotorWave(u32(0));
    setDefault();
}

/* 025CB66C */
u8 dVibrationState_c::CheckQuake() {
    return (s32)mCamera.mQuake.mPatternIdx != PATTERN_OFF ||
           (s32)mMotor.mQuake.mPatternIdx != PATTERN_OFF;
}

/* 025CB334 */
void dVibrationState_c::setDefault() {
    mCamera.mQuake.mPatternIdx = PATTERN_OFF;
    mMotor.mQuake.mCurrentFrame = RESET_FRAME;
    mMotor.mShock.mPatternIdx = PATTERN_OFF;
    mCamera.mQuake.mCurrentFrame = RESET_FRAME;
    mMotor.mQuake.mPatternIdx = PATTERN_OFF;
    mCamera.mShock.mPatternIdx = PATTERN_OFF;
    mMotor.mShock.mCurrentFrame = RESET_FRAME;
    mRumbleState = RUMBLE_STATE_WAITING;
    mMotor.mShock.mStopFrame = RESET_FRAME;
    mFrameIdx = 0;
    mCamera.mShock.mCurrentFrame = RESET_FRAME;
    mMotor.mQuake.mStopFrame = RESET_FRAME;
}

/* 025CC0C8 */
void dVibrationState_c::Init() { setDefault(); }

/* 025CB6D4 */
void dVibrationState_c::Pause() {
    if ((s32)mRumbleState == RUMBLE_STATE_PAUSED)
        return;
    if ((s32)mMotor.mShock.mPatternIdx != PATTERN_OFF ||
        (s32)mMotor.mQuake.mPatternIdx != PATTERN_OFF) {
        stopMotorWave(u32(0));
        stopMotorWave(u32(0));
    }
    s32 camera = (s32)mCamera.mQuake.mPatternIdx, motor = (s32)mMotor.mQuake.mPatternIdx;
    mCamera.mShock.mCurrentFrame = RESET_FRAME;
    mMotor.mShock.mPatternIdx = PATTERN_OFF;
    mCamera.mShock.mPatternIdx = PATTERN_OFF;
    mMotor.mShock.mCurrentFrame = RESET_FRAME;
    if (camera != PATTERN_OFF)
        mCamera.mQuake.mCurrentFrame = 0;
    if (motor != PATTERN_OFF)
        mMotor.mQuake.mCurrentFrame = 0;
    mRumbleState = RUMBLE_STATE_PAUSED;
}

// HD: compiler-generated construction, destruction and header initialization.

/* 025CC0CC */
// HD: GHS allocation, vtable and unextended tail return.
static dVibrationState_c *dVibrationState_c__ct(dVibrationState_c *self) {
    if (!self)
        self = (dVibrationState_c *)operator_new(sizeof(dVibrationState_c));
    if (self) {
        self->__vtbl = vibrationVtable;
        return setDefaultResult(self);
    }
    return self;
}

/* 025CC110 */
// HD: deleting destructor restores the vtable, kills rumble, then optionally deletes.
static void dVibrationState_c__dt(dVibrationState_c *self, u32 flags) {
    if (self) {
        self->__vtbl = vibrationVtable;
        self->Kill();
        if (flags & 1)
            operator_delete(self);
    }
}

struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1048728C))

/* 025CC16C */
static void __sinit_d_vibration_cpp() {
    headerStatics.words[2] = 0;
    headerStatics.words[0] = 0;
    headerStatics.words[3] = 0;
    headerStatics.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101F12D4));
    f32 first = piMin, second = piMax;
    headerStatics.piMin = first;
    headerStatics.piMax = second;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101F12E0));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101F12EC));
}

/* WWHD glue (tools/verify/autoglue.py)
data unsignedThreshold 10056514
data piMin 10056524
data piMax 10056528
data CS_patt 10056298
data MS_patt 10056368
data CQ_patt 10056438
data MQ_patt 10056498
str 10056504 in 025CB4E0
str 100564FC in 025CB4E0
ext 0273AD10 void* operator_new(u32)
ext 0273AF40 void operator_delete(void*)
ext 025F2AAC void stopMotorWave(u32)
ext 025F2C88 void stopMotor()
ext 025F2A30 void startMotorWave(u32,VibrationMotorData*,u32)
ext 025B8850 s32 checkVibration(VibrationConfig*)
ext 024F8044 void* dCam_getBody()
ext 02515298 void StartCameraShake(void*,u32,be<u32>*,u32,cXyz*)
ext 025153D4 void StopCameraShake(void*)
ext 0201B084 void normalizeVector(cXyz*,cXyz*)
ext 0201AD78 void addVectors(cXyz*,cXyz*,cXyz*)
ext 02019788 f32 cM_rnd()
ext 0273AA24 void assertion(const char*,u32,const char*)
ext 025CB334 dVibrationState_c* setDefaultResult(dVibrationState_c*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
