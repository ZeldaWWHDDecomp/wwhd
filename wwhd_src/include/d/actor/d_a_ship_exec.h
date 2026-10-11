/* Ship execution views: GameCube actor fields retain their shared names and offsets.
 * HD adds the cannon sight at 0x1B24, expands play state and rope material storage,
 * and keeps the compact 8-byte GHS procedure descriptor in a 12-byte actor slot.
 * Private partial views describe only fields used here; shared layouts remain unchanged.
 */
#pragma once
#include "d/actor/d_a_ship.h"
#include "gc_api/f_op/f_op_actor.h"

// The creation source owns this TU-wide mapped object.
extern cXyz& tiller_top_offset;

// HD: cannon trajectory state added after the GameCube actor's collision sphere.
struct ShipExecSight {
    /* 0x00 */ csXyz mAngle;
    /* 0x06 */ u8 mPadding[2];
    /* 0x08 */ cXyz mPosition;
    /* 0x14 */ cXyz mVelocity;
    /* 0x20 */ be<f32> mSpeed;
    /* 0x24 */ be<f32> mGravity;
    /* 0x28 */ be<s16> mDelay;
    /* 0x2A */ u8 mPadding2[2];
    /* 0x2C */ be<f32> mMinVelocity;
};
WWHD_SIZE(ShipExecSight, 0x30);
WWHD_OFFSET(ShipExecSight, mPosition, 0x08);
WWHD_OFFSET(ShipExecSight, mDelay, 0x28);
struct ShipExecSightActor {
    u8 mActor[0x1B24];
    ShipExecSight mSight;
};
WWHD_OFFSET(ShipExecSightActor, mSight, 0x1B24);
WWHD_SIZE(ShipExecSightActor, 0x1B54);
static inline ShipExecSight& ship_exec_sight(daShip_c* ship) {
    return gabi::at<ShipExecSightActor>(gabi::ea(ship))->mSight;
}

// HD: only the play-state fields read or written by the ship execution path.
struct ShipExecPlay {
    u8 mPrefix[0x5292];
    be<u8> mEventRun;
    u8 mPadding0[0x52C4 - 0x5293];
    u8 mEventManager[1];
    u8 mPadding1[0x5B00 - 0x52C5];
    be<u32> mCameraAttentionStatus;
    u8 mPadding2[0x5B6C - 0x5B04];
    be<s16> mItemBombNumCount;
    u8 mPadding3[0x5BE4 - 0x5B6E];
    be<u8> mMenuCollect;
    u8 mPadding4[0x5CDC - 0x5BE5];
    be<u32> mPlayerStatus1;
    u8 mPadding5[0x5CEA - 0x5CE0];
    be<u8> mMiniGameType;
    u8 mTail;
};
WWHD_SIZE(ShipExecPlay, 0x5CEC);
WWHD_OFFSET(ShipExecPlay, mEventRun, 0x5292);
WWHD_OFFSET(ShipExecPlay, mEventManager, 0x52C4);
WWHD_OFFSET(ShipExecPlay, mCameraAttentionStatus, 0x5B00);
WWHD_OFFSET(ShipExecPlay, mItemBombNumCount, 0x5B6C);
WWHD_OFFSET(ShipExecPlay, mMenuCollect, 0x5BE4);
WWHD_OFFSET(ShipExecPlay, mPlayerStatus1, 0x5CDC);
WWHD_OFFSET(ShipExecPlay, mMiniGameType, 0x5CEA);
static inline ShipExecPlay* ship_exec_play() { return gabi::at<ShipExecPlay>(dComIfGp_ea()); }
// The callback prefix is shared by the four GameCube particle callbacks in HD.
struct ShipExecCallbackState {
    be<u32> __vtbl;
    be<s16> mState;
    u8 mPadding[2];
    void stop() { mState = 1; }
};
WWHD_SIZE(ShipExecCallbackState, 8);
WWHD_OFFSET(ShipExecCallbackState, mState, 4);
struct ShipExecPlayer {
    u8 mPrefix[0x3BC];
    be<u32> mNoResetFlg1;
};
WWHD_SIZE(ShipExecPlayer, 0x3C0);
WWHD_OFFSET(ShipExecPlayer, mNoResetFlg1, 0x3BC);
struct ShipExecSave {
    u8 mPrefix[0x8A];
    be<u8> mBombNum;
};
WWHD_SIZE(ShipExecSave, 0x8B);
WWHD_OFFSET(ShipExecSave, mBombNum, 0x8A);
void ShipExec_clearCollisionHit(dCcD_Sph*);
void ShipExec_removeCollision(void*, dCcD_Sph*);
u32 ShipExec_getMyNowCutName(void*, s32);
f64 ShipExec_getStickValue(u32);
s32 ShipExec_getStickAngle(u32);
void ShipExec_updateFrame(void*);
s16 ShipExec_radToAngle(f32);
BOOL ShipExec_getPolyRoomPathVec(u32, cXyz*, be<s32>*);
void ShipExec_normalizeZP(cXyz*, cXyz*);
u32 ShipExec_makeBombParameter(u32, u32, u32);
fopAc_ac_c* ShipExec_fastCreate(u32, u32, cXyz*, s32, csXyz*, u32, s32, u32, u32);
void ShipExec_forceCameraLockOn(u32, u32);
void ShipExec_setBombNoGravityTime(fopAc_ac_c*, s32);
void ShipExec_playCruiseSound(cXyz*, f32);
s32 ShipExec_seenActorAngleY(fopAc_ac_c*, fopAc_ac_c*);
void ShipExec_resetTargetHit(dCcD_Sph*);
void ShipExec_setSailSoundState(u32);

struct ShipExecEffectModels {
    u8 mPrefix[0x5AC];
    gptr<J3DModel> mpBeaconModel;
    gptr<J3DModel> mpFallPointModel;
    gptr<mDoExt_McaMorf> mpFallPointAnm;
};
WWHD_SIZE(ShipExecEffectModels, 0x5B8);
WWHD_OFFSET(ShipExecEffectModels, mpFallPointModel, 0x5B0);
WWHD_OFFSET(ShipExecEffectModels, mpFallPointAnm, 0x5B4);
struct ShipExecGrid {
    fopAc_ac_c mActor;
    u8 mPadding[0x2B78 - sizeof(fopAc_ac_c)];
    be<f32> field_0x2200;
    u8 mPadding2[0x2B9A - 0x2B7C];
    be<s16> mForceWindRelAngle;
    be<u8> mIsForceWindRelAngle;
    u8 mTail[3];
    void force_calc_wind_rel_angle(s16 angle) {
        mForceWindRelAngle = angle;
        mIsForceWindRelAngle = 1;
    }
};
WWHD_SIZE(ShipExecGrid, 0x2BA0);
WWHD_OFFSET(ShipExecGrid, field_0x2200, 0x2B78);
WWHD_OFFSET(ShipExecGrid, mForceWindRelAngle, 0x2B9A);
WWHD_OFFSET(ShipExecGrid, mIsForceWindRelAngle, 0x2B9C);
struct ShipExecTrack {
    ShipExecCallbackState mCallback;
    be<f32> mBaseY;
    be<f32> mMinY;
    u8 mTail[0x50 - 0x10];
};
WWHD_SIZE(ShipExecTrack, 0x50);
WWHD_OFFSET(ShipExecTrack, mBaseY, 8);
WWHD_OFFSET(ShipExecTrack, mMinY, 0xC);
struct ShipExecProcedure {
    u8 mPrefix[0xD190];
    gabi::ptmf<void (daShip_c::*)()> mProc;
    u8 mTail[4];
};
WWHD_SIZE(ShipExecProcedure, 0xD19C);
WWHD_OFFSET(ShipExecProcedure, mProc, 0xD190);
struct ShipExecLineHit {
    u8 mPrefix[0x30];
    cXyz mCross;
};
WWHD_SIZE(ShipExecLineHit, 0x3C);
WWHD_OFFSET(ShipExecLineHit, mCross, 0x30);
namespace ShipExecProcess {
enum { fpcNm_BOMB_e = 0x126, fpcNm_RD_e = 0xE1, fpcNm_DAIOCTA_e = 0xE2, fpcNm_WZ_e = 0xD1 };
}
// HD: named guest targets retain the compact GHS member descriptor comparison.
namespace ShipExecTarget {
constexpr u32 procCannon = 0x0247F0D0;
constexpr u32 procCrane = 0x0247F7CC;
constexpr u32 procCraneUp = 0x0247FBC4;
} // namespace ShipExecTarget

// HD: the verified animation update writes the leading frame scalar directly.
struct ShipExecAnimation {
    be<f32> mFrame;
};
WWHD_SIZE(ShipExecAnimation, 4);
WWHD_OFFSET(ShipExecAnimation, mFrame, 0);

struct ShipExecRopePositions {
    gptr<cXyz> mpPosition;
};
WWHD_SIZE(ShipExecRopePositions, 4);
WWHD_OFFSET(ShipExecRopePositions, mpPosition, 0);
struct ShipExecRopeActor {
    u8 mPrefix[0x5A8];
    gptr<ShipExecRopePositions> mpRopePositions;
};
WWHD_SIZE(ShipExecRopeActor, 0x5AC);
WWHD_OFFSET(ShipExecRopeActor, mpRopePositions, 0x5A8);
namespace ShipExecSound {
enum { JA_SE_LK_SHIP_CANNON_FIRE = 0x2852, JA_SE_LK_SHIP_RUDDER_OUT = 0x2011 };
}
