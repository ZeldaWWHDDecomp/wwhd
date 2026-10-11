// Blade Trap.
// HD: the GameCube actor is stubbed; these names describe the verified HD layout and movement.
#pragma once
#include "bindings.h"

struct TrapPathPoint {
    be<u8> mArg[4];
    cXyz mPosition;
};
struct TrapPath {
    be<u16> mCount, mNextId;
    be<u8> mArg[4];
    gptr<TrapPathPoint> mpPoints;
};
struct TrapCylinder : dCcD_GObjInf {
    u8 padF8[0x18];
    be<u32> mAttrVtable, mShapeVtable;
    cM3dGCyl mCyl;
};
struct TrapLineCheck {
    gptr<void> mpPolyPass, mpGroupPass;
    be<u32> mActorPid;
    u8 padC[4];
    be<u32> mCheckVtable;
    cBgS_PolyInfo mPoly;
    cXyz mStart, mEnd;
    be<u32> mLineVtable;
    u8 pad40[0x18];
    be<u32> mPolyPassVtable;
    be<u8> mPassFlags[7];
    u8 pad63;
    be<u32> mGroupPassVtable, mGroup;
};
struct TrapBlock : fopAc_ac_c {
    u8 pad3AC[0x41C - 0x3AC];
    be<u32> mState;
    u8 pad420[0x740 - 0x420];
    be<s32> mGridX, mGridZ;
    u8 pad748[4];
    be<u32> mDirection;
};
struct TrapGameInfo {
    u8 pad0[0x12A0];
    dBgS mBgS;
    u8 pad1[0x26A4 - 0x12A0 - sizeof(dBgS)];
    u8 mCollision[1];
    u8 pad2[0x5B2C - 0x26A5];
    gptr<fopAc_ac_c> mpPlayer;
};
struct TrapResourceName {
    gptr<char> mpText;
    be<u32> mVtable;
};
struct TrapSpeedTable {
    be<f32> mSpeeds[3];
};
struct TrapWaitTable {
    be<s16> mWait[3];
};
struct TrapWallAngles {
    be<s16> mAngles[3];
};
struct TrapBlockAngles {
    be<s16> mAngles[2];
};
struct TrapBlockDirections {
    be<s16> mAngles[4];
};
struct TrapWallDistances {
    be<f32> mDistances[3];
};
struct TrapTrigEntry {
    be<f32> mSin, mCos;
};
struct TrapTrigTable {
    TrapTrigEntry mEntries[8192];
};
struct TrapHeaderStatics {
    be<f32> mMin, mMax;
    union {
        be<u32> mRandom;
        be<u8> mRandomBytes[4];
    };
    be<u32> mSeed0, mSeed1, mSeed2, mSeed3;
};
struct daObjTrap_c : fopAc_ac_c {
    enum Mode { MODE_MOVE, MODE_CHASE, MODE_VIBRATE, MODE_WAIT };
    enum Parameter { PATH_NONE = 0xFF, SPEED_DEFAULT = 0xF };
    enum Sound { SE_TRAP_HIT = 0x6A02, SE_TRAP_MOVE = 0x704F };
    // HD: the movable block's process number is two lower than GameCube.
    enum { fpcNm_Obj_Movebox_e = 0x2B };
    enum Resource { RESOURCE_MODEL = 5, RESOURCE_TEXTURE = 8, RESOURCE_COLLISION = 11 };
    BOOL create_heap();
    s32 _create();
    BOOL _delete();
    void init_mtx();
    void set_co_pos();
    BOOL circle_search();
    void set_move_info();
    BOOL check_arrival();
    void check_wall(cXyz* output);
    BOOL check_block_target_pos(cXyz* position);
    void check_block(cXyz* output, cXyz* input);
    void set_vib_mode();
    void vibrate();
    void bound();
    void set_shine();
    void shine_move();
    BOOL _execute();
    BOOL _draw();

    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ mDoExt_btkAnm mBtk;
    /* 0x424 */ request_of_phase_process_class mPhase;
    /* 0x42C */ dCcD_Stts mStts;
    /* 0x468 */ TrapCylinder mCyl;
    /* 0x598 */ gptr<TrapPath> mpPath;
    /* 0x59C */ cXyz mTarget;
    /* 0x5A8 */ cXyz mOrigin;
    /* 0x5B4 */ cXyz mDirection;
    /* 0x5C0 */ cXyz mDisplacement;
    /* 0x5CC */ cXyz mSearchVelocity;
    /* 0x5D8 */ cXyz mSavedPosition;
    /* 0x5E4 */ cXyz mBounceOffset;
    /* 0x5F0 */ be<f32> mPathLength;
    /* 0x5F4 */ be<s16> mWaitTimer;
    /* 0x5F6 */ be<u16> mBounceTimer;
    /* 0x5F8 */ be<f32> mBounceAmplitude;
    /* 0x5FC */ be<u16> mVibrationTimer;
    /* 0x5FE */ be<s16> mSavedAngle;
    /* 0x600 */ be<u8> mPathPoint, mPathId, mDirectionValid, mMode, mShining, mSpeedType;
    u8 pad606[2];
    /* 0x608 */ be<f32> mMoveSpeed;
    /* 0x60C */ be<s16> mWaitDuration;
    u8 pad60E[2];
    /* 0x610 */ gptr<dBgW> mpBgW;
};
WWHD_SIZE(TrapPathPoint, 0x10);
WWHD_SIZE(TrapPath, 0xC);
WWHD_OFFSET(TrapPath, mpPoints, 8);
WWHD_SIZE(TrapLineCheck, 0x6C);
WWHD_OFFSET(TrapLineCheck, mPoly, 0x14);
WWHD_OFFSET(TrapLineCheck, mEnd, 0x30);
WWHD_OFFSET(TrapLineCheck, mPolyPassVtable, 0x58);
WWHD_OFFSET(TrapLineCheck, mGroupPassVtable, 0x64);
WWHD_SIZE(TrapCylinder, 0x130);
WWHD_OFFSET(TrapCylinder, mCyl, 0x118);
WWHD_OFFSET(TrapBlock, mState, 0x41C);
WWHD_OFFSET(TrapBlock, mGridX, 0x740);
WWHD_OFFSET(TrapBlock, mDirection, 0x74C);
WWHD_OFFSET(TrapGameInfo, mBgS, 0x12A0);
WWHD_OFFSET(TrapGameInfo, mCollision, 0x26A4);
WWHD_OFFSET(TrapGameInfo, mpPlayer, 0x5B2C);
WWHD_OFFSET(daObjTrap_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjTrap_c, mBtk, 0x3B0);
WWHD_OFFSET(daObjTrap_c, mPhase, 0x424);
WWHD_OFFSET(daObjTrap_c, mStts, 0x42C);
WWHD_OFFSET(daObjTrap_c, mCyl, 0x468);
WWHD_OFFSET(daObjTrap_c, mpPath, 0x598);
WWHD_OFFSET(daObjTrap_c, mTarget, 0x59C);
WWHD_OFFSET(daObjTrap_c, mMode, 0x603);
WWHD_OFFSET(daObjTrap_c, mpBgW, 0x610);
WWHD_SIZE(daObjTrap_c, 0x614);

WWHD_OFFSET(daObjTrap_c, mOrigin, 0x5A8);
WWHD_OFFSET(daObjTrap_c, mDirection, 0x5B4);
WWHD_OFFSET(daObjTrap_c, mDisplacement, 0x5C0);
WWHD_OFFSET(daObjTrap_c, mSearchVelocity, 0x5CC);
WWHD_OFFSET(daObjTrap_c, mSavedPosition, 0x5D8);
WWHD_OFFSET(daObjTrap_c, mBounceOffset, 0x5E4);
WWHD_OFFSET(daObjTrap_c, mPathLength, 0x5F0);
WWHD_OFFSET(daObjTrap_c, mWaitTimer, 0x5F4);
WWHD_OFFSET(daObjTrap_c, mBounceTimer, 0x5F6);
WWHD_OFFSET(daObjTrap_c, mBounceAmplitude, 0x5F8);
WWHD_OFFSET(daObjTrap_c, mVibrationTimer, 0x5FC);
WWHD_OFFSET(daObjTrap_c, mSavedAngle, 0x5FE);
WWHD_OFFSET(daObjTrap_c, mPathPoint, 0x600);
WWHD_OFFSET(daObjTrap_c, mPathId, 0x601);
WWHD_OFFSET(daObjTrap_c, mDirectionValid, 0x602);
WWHD_OFFSET(daObjTrap_c, mShining, 0x604);
WWHD_OFFSET(daObjTrap_c, mSpeedType, 0x605);
WWHD_OFFSET(daObjTrap_c, mMoveSpeed, 0x608);
WWHD_OFFSET(daObjTrap_c, mWaitDuration, 0x60C);
