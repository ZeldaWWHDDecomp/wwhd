/* Ice platform: GameCube class/method names, descriptive HD members (tww layout is opaque).
 * HD actor prefix ends at 0x3AC; original verified member offsets and outgoing linkage retained. */
#pragma once
#include "gc_api/gc_api.h"
// Guest callees need a linkage area below addressable caller temporaries.
// GABI retains register packing and, for arguments beyond r10, the stack
// argument area at callee SP+8. This extra aligned frame protects our locals.
template <class R = void, class... A> static R iliftAbiCall(u32 target, A... args) {
    u32 callerSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[4]> linkage;
    (*linkage)[0] = callerSp;
    return gabi::call_ptr<R>(target, args...);
}
// Natural calls between this actor's functions use the same outgoing frame.
#undef WWHD_GLUE_FUNC
#define WWHD_GLUE_FUNC(addr, R, ...)                                                               \
    if (gabi::Activation::nested())                                                                \
        return iliftAbiCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                   \
    gabi::Activation wwhd_activation_

struct IliftPath;
struct IliftQuat {
    be<f32> x, y, z, w;
};
struct daIlift_c : fopAc_ac_c {
    enum MoveState { NEXT_POINT, ACCELERATE, CRUISE, DECELERATE };
    BOOL CreateHeap();
    void set_mtx();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    void set_next_pnt();
    void lift_normal_move();
    void path_move();
    void lift_wave();
    BOOL _execute();
    request_of_phase_process_class mPhase;
    be<u32> mpModel, mpBgW;
    be<f32> mBgMtx[12];
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    be<u8> mRiderPresent, mRiderPrevious;
    be<s16> mWaveAngle, mWaveTarget;
    u8 _5f6[2];
    IliftQuat mTargetQuat, mCurrentQuat;
    cXyz mRiderOffset;
    be<f32> mWaveDecay;
    be<u32> mFrameCount;
    be<s32> mWaveTime;
    u8 _630[4];
    be<u8> mType;
    u8 _635[3];
    cXyz mTargetPoint, mPreviousPoint;
    gptr<IliftPath> mpPath;
    be<f32> mTargetSpeed, mMoveSpeed;
    be<u16> mMoveTimer;
    be<u8> mPathIndex;
    be<s8> mPointIndex, mPointDirection;
    be<u8> mMoveState;
    u8 _662[6];
    be<f32> mRiderRadius;
    u8 _66c[4];
    be<u32> mSwitchIndex;
    cXyz mPreviousPlayerPosition;
};
WWHD_OFFSET(daIlift_c, mpModel, 0x3B4);
WWHD_OFFSET(daIlift_c, mAcch, 0x3EC);
WWHD_OFFSET(daIlift_c, mRiderPresent, 0x5F0);
WWHD_OFFSET(daIlift_c, mTargetQuat, 0x5F8);
WWHD_OFFSET(daIlift_c, mRiderOffset, 0x618);
WWHD_OFFSET(daIlift_c, mType, 0x634);
WWHD_OFFSET(daIlift_c, mpPath, 0x650);
WWHD_OFFSET(daIlift_c, mRiderRadius, 0x668);
WWHD_SIZE(daIlift_c, 0x680);

/* HD member accesses and profile101B7940 prove these offsets. */
WWHD_OFFSET(daIlift_c, mPhase, 0x3AC);
WWHD_OFFSET(daIlift_c, mpBgW, 0x3B8);
WWHD_OFFSET(daIlift_c, mBgMtx, 0x3BC);
WWHD_OFFSET(daIlift_c, mAcchCir, 0x5B0);
WWHD_OFFSET(daIlift_c, mRiderPrevious, 0x5F1);
WWHD_OFFSET(daIlift_c, mWaveAngle, 0x5F2);
WWHD_OFFSET(daIlift_c, mWaveTarget, 0x5F4);
WWHD_OFFSET(daIlift_c, mCurrentQuat, 0x608);
WWHD_OFFSET(daIlift_c, mWaveDecay, 0x624);
WWHD_OFFSET(daIlift_c, mFrameCount, 0x628);
WWHD_OFFSET(daIlift_c, mWaveTime, 0x62C);
WWHD_OFFSET(daIlift_c, mTargetPoint, 0x638);
WWHD_OFFSET(daIlift_c, mPreviousPoint, 0x644);
WWHD_OFFSET(daIlift_c, mTargetSpeed, 0x654);
WWHD_OFFSET(daIlift_c, mMoveSpeed, 0x658);
WWHD_OFFSET(daIlift_c, mMoveTimer, 0x65C);
WWHD_OFFSET(daIlift_c, mPathIndex, 0x65E);
WWHD_OFFSET(daIlift_c, mPointIndex, 0x65F);
WWHD_OFFSET(daIlift_c, mPointDirection, 0x660);
WWHD_OFFSET(daIlift_c, mMoveState, 0x661);
WWHD_OFFSET(daIlift_c, mSwitchIndex, 0x670);
WWHD_OFFSET(daIlift_c, mPreviousPlayerPosition, 0x674);

// HD: named unit-local bindings retain the extra outgoing linkage frame.
namespace ilift_call {
template <class... A> inline void vectorSubtract(A... args) {
    constexpr u32 target = 0x0201ADE0;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void vectorCross(A... args) {
    constexpr u32 target = 0x0201B080;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline f32 vectorSquaredMag(A... args) {
    constexpr u32 target = 0x028E8DD0;
    return iliftAbiCall<f32>(target, args...);
}
template <class... A> inline f32 sqrtf(A... args) {
    constexpr u32 target = 0x028F4384;
    return iliftAbiCall<f32>(target, args...);
}
template <class... A> inline s32 vectorNormalizeRS(A... args) {
    constexpr u32 target = 0x0201B47C;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline void addCalcAngleS2(A... args) {
    constexpr u32 target = 0x0200F428;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline u32 getResource(A... args) {
    constexpr u32 target = 0x026066C4;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline void showAssert(A... args) {
    constexpr u32 target = 0x0273AA24;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline u32 createModel(A... args) {
    constexpr u32 target = 0x025E38E0;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline u32 createBgW(A... args) {
    constexpr u32 target = 0x024F23F4;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline s32 setBgW(A... args) {
    constexpr u32 target = 0x0200A030;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline void translateMatrix(A... args) {
    constexpr u32 target = 0x028E93CC;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void multiplyQuaternion(A... args) {
    constexpr u32 target = 0x025F25CC;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void scaleMatrix(A... args) {
    constexpr u32 target = 0x025F2518;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void copyMatrix(A... args) {
    constexpr u32 target = 0x028E90D4;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void setCullBox(A... args) {
    constexpr u32 target = 0x025D674C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void setWall(A... args) {
    constexpr u32 target = 0x024EFF44;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void setAcch(A... args) {
    constexpr u32 target = 0x024F06B4;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline u32 getRoomPath(A... args) {
    constexpr u32 target = 0x025AAF88;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline u32 getPlay(A... args) {
    constexpr u32 target = 0x025200D4;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline void registerBgW(A... args) {
    constexpr u32 target = 0x024EEA6C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void moveBgW(A... args) {
    constexpr u32 target = 0x024F43DC;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void constructActor(A... args) {
    constexpr u32 target = 0x025D4ED0;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void constructAcch(A... args) {
    constexpr u32 target = 0x024F0474;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void constructAcchCircle(A... args) {
    constexpr u32 target = 0x024EFE94;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline s32 loadResource(A... args) {
    constexpr u32 target = 0x02520460;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline s32 createSolidHeap(A... args) {
    constexpr u32 target = 0x025D63E8;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline void releaseBgW(A... args) {
    constexpr u32 target = 0x020087EC;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void deleteResource(A... args) {
    constexpr u32 target = 0x025204C8;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline u32 getEnvLight(A... args) {
    constexpr u32 target = 0x02555D0C;
    return iliftAbiCall<u32>(target, args...);
}
template <class... A> inline void settingTev(A... args) {
    constexpr u32 target = 0x025626A4;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void setLightTev(A... args) {
    constexpr u32 target = 0x02562F5C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void updateModel(A... args) {
    constexpr u32 target = 0x025E2DE0;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline f32 addCalc(A... args) {
    constexpr u32 target = 0x0200ECD4;
    return iliftAbiCall<f32>(target, args...);
}
template <class... A> inline void addCalcPosXZ(A... args) {
    constexpr u32 target = 0x0200F164;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void addCalcAngleS(A... args) {
    constexpr u32 target = 0x0200F378;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void vectorNormalizeZP(A... args) {
    constexpr u32 target = 0x0201B12C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline s32 vectorNotEqual(A... args) {
    constexpr u32 target = 0x0201AFD8;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline void quaternionLerp(A... args) {
    constexpr u32 target = 0x028E9BC0;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline s32 isSwitch(A... args) {
    constexpr u32 target = 0x025BA0C0;
    return iliftAbiCall<s32>(target, args...);
}
template <class... A> inline void changeAttributeCode(A... args) {
    constexpr u32 target = 0x024F43EC;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void registerGlobalObject(A... args) {
    constexpr u32 target = 0x028F026C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void constructGlobalByteA(A... args) {
    constexpr u32 target = 0x028ED6F8;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void constructGlobalByteB(A... args) {
    constexpr u32 target = 0x028EAB2C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void deleteObject(A... args) {
    constexpr u32 target = 0x0273AF40;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void destroyAcchCirArray(A... args) {
    constexpr u32 target = 0x02018034;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void destroyAcch(A... args) {
    constexpr u32 target = 0x024EFD9C;
    return iliftAbiCall<void>(target, args...);
}
template <class... A> inline void destroyActor(A... args) {
    constexpr u32 target = 0x025D50BC;
    return iliftAbiCall<void>(target, args...);
}
} // namespace ilift_call

struct SinCosEntry {
    be<f32> sin, cos;
};
struct IliftModel {
    u8 _00[0xBC];
    cXyz mBaseScale;
    Mtx34 mBaseTransform;
};
struct IliftPoint {
    u8 mArgs[4];
    cXyz m_position;
};
struct IliftPath {
    be<u16> m_num, m_nextID;
    u8 mArg0;
    be<u8> m_closed;
    u8 _06[2];
    gptr<IliftPoint> m_points;
};
WWHD_SIZE(SinCosEntry, 8);
WWHD_OFFSET(IliftModel, mBaseScale, 0xBC);
WWHD_OFFSET(IliftModel, mBaseTransform, 0xC8);
WWHD_SIZE(IliftModel, 0xF8);
WWHD_SIZE(IliftQuat, 0x10);
WWHD_SIZE(IliftPoint, 0x10);
WWHD_OFFSET(IliftPath, m_points, 8);
WWHD_SIZE(IliftPath, 0xC);

// HD: player process shifted by one from GameCube's fpcNm_PLAYER_e.
constexpr s16 IliftPlayerProcess = 0xA8;
struct IliftBgW {
    u8 _00[0xA8];
    be<u32> __vtbl;
    u8 _AC[4];
    be<u32> mRideCallback;
};
struct IliftPlayLists {
    u8 _00[0x5D70];
    be<u32> mOpaList, mXluList, mOpaDefault, mXluDefault;
};
WWHD_OFFSET(IliftBgW, __vtbl, 0xA8);
WWHD_OFFSET(IliftBgW, mRideCallback, 0xB0);
WWHD_SIZE(IliftBgW, 0xB4);
WWHD_OFFSET(IliftPlayLists, mOpaList, 0x5D70);
WWHD_SIZE(IliftPlayLists, 0x5D80);
