/* daObjMagmarock::Act_c, WWHD layout (0x768).
 * GC Act_c has no completed member layout. HD fields use descriptive names;
 * actor members beyond the base follow the +0x11C actor-size delta.
 */
#pragma once
#include "f_op/f_op_actor.h"
// HD layout: three quaternions separate rider tilt from the lava-ground normal.

// HD: J3D light information and its two extended copies in the TEV record.
struct MagmarockLightInfo {
    cXyz mPosition, mDirection;
    GXColor mColor;
    be<s16> mControls[4];
    be<f32> mAttenuation[8];
};
WWHD_OFFSET(MagmarockLightInfo, mColor, 0x18);
WWHD_OFFSET(MagmarockLightInfo, mControls, 0x1C);
WWHD_OFFSET(MagmarockLightInfo, mAttenuation, 0x24);
WWHD_SIZE(MagmarockLightInfo, 0x44);
struct MagmarockTevStr {
    MagmarockLightInfo mLightInfo;
    u8 mLightObjectTail[0x40];
    cXyz mLightPosWorld;
    GXColorS10 mColorC0;
    GXColor mColorK0, mColorK1;
    GXColorS10 mFogColor;
    be<f32> mFogStartZ, mFogEndZ, mColpatBlend;
    be<u8> mInitTimer, mEnvrIdxCurr, mEnvrIdxPrev, mColpatCurr, mColpatPrev;
    be<s8> mRoomNo;
    be<u8> mEnvrIdxOverride, mLightMode, mInitType;
    u8 padBD[3];
    MagmarockLightInfo mHdLightInfo;
    u8 pad104[0x40];
    MagmarockLightInfo mHdLightInfo2;
    u8 pad188[0x40];
};
WWHD_OFFSET(MagmarockTevStr, mLightPosWorld, 0x84);
WWHD_OFFSET(MagmarockTevStr, mColorC0, 0x90);
WWHD_OFFSET(MagmarockTevStr, mColorK0, 0x98);
WWHD_OFFSET(MagmarockTevStr, mHdLightInfo, 0xC0);
WWHD_OFFSET(MagmarockTevStr, mHdLightInfo2, 0x144);
WWHD_SIZE(MagmarockTevStr, 0x1C8);

namespace daObjMagmarock {
struct Act_c : fopAc_ac_c {
    BOOL CreateHeap();
    void set_mtx();
    void stay_proc_init();
    void appear_proc_init();
    void wait_proc_init();
    void vanish_proc_init();
    void calc_ground_quat();
    void demo_move();
    void ControlEffect();
    void play_anim();
    BOOL LiftUpRequest(u8 *position);
    BOOL BeforeLiftRequest(u8 *position);
    void appear_proc();
    void quake_proc_init();
    void wait_proc();
    void stay_proc();
    void quake_proc();
    void vanish_proc();
    BOOL CreateInit();
    BOOL _execute();
    u8 _3AC[4];
    be<s16> mTiltAngle, mUnusedAngle, mRiderFrames;
    be<u8> mHasRider, mStartStay;
    gptr<u8> mpAppearEffect, mpAppearEffect2, mpBeforeLiftEffect, mpLiftEffect;
    be<f32> mCurrentQuat[4], mRiderQuat[4], mGroundQuat[4];
    gabi::ptmf<void (Act_c::*)()> mProcess;
    u8 mPhase[8];
    gptr<u8> mpModel, mpBrkData;
    u8 mBrkController[0x78];
    gptr<u8> mpBckData;
    u8 mBckController[0x8C];
    be<f32> mBackgroundMatrix[12];
    gptr<u8> mpBackground;
    MagmarockTevStr mLighting;
    be<f32> mGroundTriangle[9];
    be<f32> mWobbleAmplitude, mBckFrame, mBrkFrame, mLiftTarget[3];
    be<s32> mTimer, mElapsedFrames;
    be<u32> mUnused;
    be<s16> mWobblePhase, mWobbleStep, mUnusedShort, mDemoState, mLifted, mBeforeLift;
};
} // namespace daObjMagmarock
using daObjMagmarock_c = daObjMagmarock::Act_c;
WWHD_OFFSET(daObjMagmarock_c, mTiltAngle, 0x3B0);
WWHD_OFFSET(daObjMagmarock_c, mpModel, 0x408);
WWHD_OFFSET(daObjMagmarock_c, mpBckData, 0x488);
WWHD_OFFSET(daObjMagmarock_c, mBackgroundMatrix, 0x518);
WWHD_OFFSET(daObjMagmarock_c, mGroundTriangle, 0x714);
WWHD_OFFSET(daObjMagmarock_c, mBeforeLift, 0x766);
WWHD_SIZE(daObjMagmarock_c, 0x768);
WWHD_OFFSET(daObjMagmarock_c, mTiltAngle, 0x3B0);
WWHD_OFFSET(daObjMagmarock_c, mUnusedAngle, 0x3B2);
WWHD_OFFSET(daObjMagmarock_c, mRiderFrames, 0x3B4);
WWHD_OFFSET(daObjMagmarock_c, mHasRider, 0x3B6);
WWHD_OFFSET(daObjMagmarock_c, mStartStay, 0x3B7);
WWHD_OFFSET(daObjMagmarock_c, mpAppearEffect, 0x3B8);
WWHD_OFFSET(daObjMagmarock_c, mpAppearEffect2, 0x3BC);
WWHD_OFFSET(daObjMagmarock_c, mpBeforeLiftEffect, 0x3C0);
WWHD_OFFSET(daObjMagmarock_c, mpLiftEffect, 0x3C4);
WWHD_OFFSET(daObjMagmarock_c, mpModel, 0x408);
WWHD_OFFSET(daObjMagmarock_c, mpBrkData, 0x40C);
WWHD_OFFSET(daObjMagmarock_c, mpBckData, 0x488);
WWHD_OFFSET(daObjMagmarock_c, mpBackground, 0x548);
WWHD_OFFSET(daObjMagmarock_c, mWobbleAmplitude, 0x738);
WWHD_OFFSET(daObjMagmarock_c, mBckFrame, 0x73C);
WWHD_OFFSET(daObjMagmarock_c, mBrkFrame, 0x740);
WWHD_OFFSET(daObjMagmarock_c, mTimer, 0x750);
WWHD_OFFSET(daObjMagmarock_c, mElapsedFrames, 0x754);
WWHD_OFFSET(daObjMagmarock_c, mUnused, 0x758);
WWHD_OFFSET(daObjMagmarock_c, mWobblePhase, 0x75C);
WWHD_OFFSET(daObjMagmarock_c, mWobbleStep, 0x75E);
WWHD_OFFSET(daObjMagmarock_c, mUnusedShort, 0x760);
WWHD_OFFSET(daObjMagmarock_c, mDemoState, 0x762);
WWHD_OFFSET(daObjMagmarock_c, mLifted, 0x764);
WWHD_OFFSET(daObjMagmarock_c, mBeforeLift, 0x766);

void magmaBgRelease(void *, void *);
void magmaAddCalc(void *, f32, f32, f32);
void magmaAddCalcPos(void *, void *, f32, f32);
void magmaAddCalcAngle(void *, s32, u32, u32);
void magmaVectorDifference(void *, void *, void *);
void magmaVectorCross(void *, void *, void *);
s32 magmaVectorNormalize(void *);
void magmaBgRegister(void *, void *, void *);
u32 magmaBgNew(void *, u32, void *);
void magmaBgMove(void *);
u32 magmaGetPlay();
s32 magmaResLoad(void *, const char *);
void magmaResDelete(void *, const char *);
s32 magmaGetReverb(s32);
s32 magmaGetStaff(void *, const char *, u32, u32);
void magmaCutEnd(void *, s32);
s32 magmaEndCheck(void *, const char *);
u32 magmaGetEnvironment();
void magmaSettingTev(void *, u32, void *, void *);
void magmaSetLightTev(void *, void *, void *);
void magmaQuatFromTriangle(void *, void *, void *, void *, f32);
f32 magmaCheckY(void *, void *);
u32 magmaParticleSet(void *, u32, u32, void *, u32, u32, u32, u32, s32, u32, u32, u32);
void magmaEventOn(void *, u32);
s32 magmaEventCheck(void *, u32);
void magmaStartShock(void *, u32, u32, void *);
void magmaActorConstruct(void *);
void magmaActorDestroy(void *, u32);
void magmaActorDelete(void *);
s32 magmaEntryHeap(void *, void *, u32);
void magmaCullBox(void *, f32, f32, f32, f32, f32, f32);
void magmaOrderEvent(void *, const char *, u32, u32);
void magmaSoundStart(u32, void *, u32, s32);
void magmaModelUpdate(void *, u32);
u32 magmaModelCreate(void *, u32, u32);
void magmaBrkConstruct(void *);
s32 magmaBrkInit(void *, void *, void *, u32, u32, u32, f32, s32, u32, u32);
void magmaBrkEntry(void *, void *, f32);
s32 magmaBckInit(void *, void *, void *, u32, u32, u32, f32, s32, u32);
void magmaBckEntry(void *, void *, f32);
void magmaMtxZXYrot(void *, s32, s32, s32);
void magmaMtxYrot(void *, s32);
void magmaMtxScale(f32, f32, f32);
void magmaMtxQuat(void *);
u32 magmaGetResource(void *, void *, s32);
void magmaAssertion(const char *, u32, const char *);
daObjMagmarock_c *magmaAllocate(u32);
void magmaDeallocate(void *);
void magmaTextureMatrixConstruct(void *);
void magmaFrameInit(void *, u32);
void magmaSoundObjectDestroy(void *, u32);
void magmaQuatMultiply(void *, void *, void *);
void magmaVectorAdd(void *, void *, void *);
f32 magmaVectorSquareMag(void *);
void magmaVectorScale(void *, void *, f32);
void magmaMtxCopy(void *, void *);
void magmaMtxTranslation(void *, f32, f32, f32);
void magmaQuatSlerp(void *, void *, void *, f32);
void magmaCircleConstruct(void *);
void magmaAngleConstruct(void *);
void magmaRegisterGlobal(void *);
f32 magmaSqrt(f32);

// HD: private typed views keep this unit independent of shared layouts.
struct MagmarockModel {
    u8 pad0[0xAC];
    be<u32> mpModelData;
    u8 padb0[0xC];
    cXyz mBaseScale;
    Mtx34 mBaseTRMtx;
};
WWHD_SIZE(MagmarockModel, 0xF8);
WWHD_OFFSET(MagmarockModel, mpModelData, 0xAC);
WWHD_OFFSET(MagmarockModel, mBaseScale, 0xBC);
WWHD_OFFSET(MagmarockModel, mBaseTRMtx, 0xC8);
struct MagmarockBackground {
    be<u32> mIndex;
    u8 pad4[0x74];
    be<u32> mFlags;
    u8 pad7c[0x2C];
    be<u32> mpCorrection;
    u8 padac[0x4];
    be<u32> mpRideCallback;
};
WWHD_SIZE(MagmarockBackground, 0xB4);
WWHD_OFFSET(MagmarockBackground, mIndex, 0x0);
WWHD_OFFSET(MagmarockBackground, mFlags, 0x78);
WWHD_OFFSET(MagmarockBackground, mpCorrection, 0xA8);
WWHD_OFFSET(MagmarockBackground, mpRideCallback, 0xB0);
struct MagmarockPlay {
    u8 pad0[0x12A0];
    be<u8> mBgS;
    u8 pad12a1[0x4017];
    be<u16> mEventFlags;
    u8 pad52ba[0xA];
    be<u8> mEventManager;
    u8 pad52c5[0x6D7];
    be<u8> mVibration;
    u8 pad599d[0x113];
    be<u32> mpParticleControl;
    be<u32> mpMagmaPacket;
};
WWHD_SIZE(MagmarockPlay, 0x5AB8);
WWHD_OFFSET(MagmarockPlay, mBgS, 0x12A0);
WWHD_OFFSET(MagmarockPlay, mEventFlags, 0x52B8);
WWHD_OFFSET(MagmarockPlay, mEventManager, 0x52C4);
WWHD_OFFSET(MagmarockPlay, mVibration, 0x599C);
WWHD_OFFSET(MagmarockPlay, mpParticleControl, 0x5AB0);
WWHD_OFFSET(MagmarockPlay, mpMagmaPacket, 0x5AB4);
struct MagmarockEmitter {
    u8 pad0[0x5C];
    be<u32> mFrameCount;
    u8 pad60[0x1CC];
    cXyz mGlobalTranslation;
    u8 pad238[0xC];
    GXColor mPrmColor;
    GXColor mEnvColor;
    u8 pad24c[0x8];
    be<u32> mStatus;
    u8 pad258[0xA];
    be<u8> mKind;
};
WWHD_SIZE(MagmarockEmitter, 0x263);
WWHD_OFFSET(MagmarockEmitter, mFrameCount, 0x5C);
WWHD_OFFSET(MagmarockEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(MagmarockEmitter, mPrmColor, 0x244);
WWHD_OFFSET(MagmarockEmitter, mEnvColor, 0x248);
WWHD_OFFSET(MagmarockEmitter, mStatus, 0x254);
WWHD_OFFSET(MagmarockEmitter, mKind, 0x262);
struct MagmarockAnimation {
    u8 pad0[0x4];
    be<u32> __vtbl;
};
WWHD_SIZE(MagmarockAnimation, 0x8);
WWHD_OFFSET(MagmarockAnimation, __vtbl, 0x4);
struct MagmarockAnimationVtable {
    u8 pad0[0x14];
    be<u32> mpGetFrameCount;
};
WWHD_SIZE(MagmarockAnimationVtable, 0x18);
WWHD_OFFSET(MagmarockAnimationVtable, mpGetFrameCount, 0x14);
struct MagmarockSoundObject {
    be<u32> __vtbl;
    u8 pad4[0x34];
    be<u32> mEmbeddedVtable;
    u8 pad3c[0xC];
    be<u32> mVoiceHandle;
    u8 pad4c[0x20];
    be<u32> mState;
    be<u32> mLink;
    be<u32> mParameter;
    be<u32> mFlags;
};
WWHD_SIZE(MagmarockSoundObject, 0x7C);
WWHD_OFFSET(MagmarockSoundObject, __vtbl, 0x0);
WWHD_OFFSET(MagmarockSoundObject, mEmbeddedVtable, 0x38);
WWHD_OFFSET(MagmarockSoundObject, mVoiceHandle, 0x48);
WWHD_OFFSET(MagmarockSoundObject, mState, 0x6C);
WWHD_OFFSET(MagmarockSoundObject, mLink, 0x70);
WWHD_OFFSET(MagmarockSoundObject, mParameter, 0x74);
WWHD_OFFSET(MagmarockSoundObject, mFlags, 0x78);

enum MagmarockEmitterFlags { MAGMAROCK_EMITTER_INVALID = 1 };

struct MagmarockWords4 {
    be<u32> mWords[4];
};
WWHD_SIZE(MagmarockWords4, 16);
struct MagmarockSincos {
    be<f32> mSin, mCos;
};
WWHD_SIZE(MagmarockSincos, 8);
struct MagmarockSincosTable {
    MagmarockSincos mEntries[8192];
};
WWHD_SIZE(MagmarockSincosTable, 0x10000);
struct MagmarockColor {
    be<u8> mChannels[4];
};
WWHD_SIZE(MagmarockColor, 4);
struct MagmarockDtorRecords {
    be<u32> mWords[9];
};
WWHD_SIZE(MagmarockDtorRecords, 36);
struct MagmarockMemberEntry {
    be<s16> mDelta, mSlot;
    be<u32> mTarget;
};
WWHD_SIZE(MagmarockMemberEntry, 8);
// HD: GHS process member identities, including the pointer's virtual form.
enum MagmarockProcess : u32 {
    MAGMAROCK_APPEAR = 0x0236E738,
    MAGMAROCK_WAIT = 0x0236E800,
    MAGMAROCK_STAY = 0x0236E810,
    MAGMAROCK_QUAKE = 0x0236E85C,
    MAGMAROCK_VANISH = 0x0236E8E8,
};
namespace magmarock_tu {
constexpr u32 safeString = 0x1002C1F0;
constexpr u32 actor = 0x1002C330;
constexpr u32 soundObjectBase = 0x1016E54C;
constexpr u32 soundObject = 0x1002C208;
constexpr u32 textureMatrix = 0x1016D820;
constexpr u32 typicalMoveBg = 0x024EE658;
} // namespace magmarock_tu

WWHD_OFFSET(daObjMagmarock_c, mProcess, 0x3F8);
