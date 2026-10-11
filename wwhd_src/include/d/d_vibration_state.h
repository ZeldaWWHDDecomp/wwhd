/* Private dVibration_c state view; shared caller-only d/d_vibration.h stays unchanged.
 * Vibration state: GameCube names and unchanged HD 0x84 layout.
 * Guest vectors/patterns retain word copies; the HD motor format has eight packed words. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct vib_pattern {
    be<u16> rounds, length;
    be<u32> pattern;
};
struct VibrationCustomPattern {
    union {
        be<u16> value;
        struct {
            be<u8> high, low;
        } bytes;
    } length;
    be<u8> bits[4];
};
struct VibrationMotorData {
    be<u16> length, words[8];
};
struct VibrationPatternValue {
    be<u32> bits;
};
class dVibrationState_c {
  public:
    enum { RUMBLE_STATE_PAUSED = -1, RUMBLE_STATE_WAITING = 0, RUMBLE_STATE_RUNNING = 1 };
    enum { RUMBLE_SHOCK = 1, RUMBLE_QUAKE = 2, MASK_CAMERA_SHAKE = 0x3E, FLAG_MOTOR_SHAKE = 1 };
    enum { LOOP_ONCE = 0, LOOP_FOREVER = 1 };
    enum { RESET_FRAME = -99, PATTERN_OFF = -1, MAX_MOTOR_QUAKE_FRAME = 900 };
    struct CameraPattern {
        /* 0x00 */ be<s32> mPatternIdx;
        /* 0x04 */ be<u32> mPattern;
        /* 0x08 */ be<s32> mLength;
        /* 0x0C */ be<s32> mRounds;
        /* 0x10 */ be<u32> mFlags;
        /* 0x14 */ cXyz mCoord;
        /* 0x20 */ be<s32> mCurrentFrame;
    };
    struct MotorPattern {
        /* 0x00 */ be<s32> mPatternIdx;
        /* 0x04 */ be<u32> mPattern;
        /* 0x08 */ be<s32> mLength;
        /* 0x0C */ be<s32> mRounds;
        /* 0x10 */ be<s32> mCurrentFrame;
        /* 0x14 */ be<s32> mStopFrame;
    };
    /* 0x00 */ struct {
        CameraPattern mShock, mQuake;
    } mCamera;
    /* 0x48 */ struct {
        MotorPattern mShock, mQuake;
    } mMotor;
    /* 0x78 */ be<s32> mFrameIdx;
    /* 0x7C */ be<s32> mRumbleState;
    /* 0x80 */ be<u32> __vtbl;
    s32 Run();
    u8 StartShock(u32, u32, const cXyz *);
    u8 StartQuake(u32, u32, const cXyz *);
    u8 StartQuakeCustom(const VibrationCustomPattern *, u32, u32, const cXyz *);
    s32 StopQuake(u32);
    u8 CheckQuake();
    void setDefault();
    void Kill();
    void Init();
    void Pause();
};
WWHD_SIZE(vib_pattern, 8);
WWHD_SIZE(dVibrationState_c::CameraPattern, 0x24);
WWHD_SIZE(dVibrationState_c::MotorPattern, 0x18);
WWHD_OFFSET(dVibrationState_c, mMotor, 0x48);
WWHD_OFFSET(dVibrationState_c, __vtbl, 0x80);
WWHD_SIZE(dVibrationState_c, 0x84);
struct VibrationPlatform {
    u8 state[0x1D0];
    be<u32> mExpandedMotorPattern;
};
struct VibrationConfig {
    u8 state;
};
struct VibrationSave {
    u8 state[0x1C4];
    VibrationConfig mConfig;
};
void *operator_new(u32);
void operator_delete(void *);
void stopMotorWave(u32);
void stopMotor();
void startMotorWave(u32, VibrationMotorData *, u32);
s32 checkVibration(VibrationConfig *);
void *dCam_getBody();
void StartCameraShake(void *, u32, be<u32> *, u32, cXyz *);
void StopCameraShake(void *);
void normalizeVector(cXyz *, cXyz *);
void addVectors(cXyz *, cXyz *, cXyz *);
f32 cM_rnd();
void assertion(const char *, u32, const char *);
// HD: constructor tail return forwards the full r3 from the void setDefault entry.
dVibrationState_c *setDefaultResult(dVibrationState_c *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
u32 makebits(u32, u32, u32);
u32 rollshift(u32, u32, u32);
u32 randombit(s32, s32);
VibrationMotorData *makedata(VibrationMotorData *, u32, u32);
