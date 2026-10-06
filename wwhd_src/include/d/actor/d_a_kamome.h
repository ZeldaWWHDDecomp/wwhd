/* kamome_class (seagull), WWHD layout. 
 *
 * GameCube -> WWHD: +0x11C up to mpMorf; mShadowId (GameCube 0x29C) is gone (HD shadows), so the
 * fields after it are +0x118; four more bytes go before the collision members (+0x114 from
 * mAcchCir on). Offsets from the verified functions. */
#pragma once
#include "bindings.h"

struct dBgS_AcchCir_l { u8 _[0x40]; };
struct dBgS_ObjAcch_l {
    /* 0x00 */ u8 _00[0x28];
    /* 0x28 */ be<u32> m_flags; /* dBgS_Acch::m_flags; WALL_HIT 0x10, GROUND_HIT 0x20 */
    /* 0x2C */ u8 _2C[0x94 - 0x2C];
    /* 0x94 */ be<f32> m_ground_h; /* GetGroundH */
    /* 0x98 */ u8 _98[0x1C4 - 0x98];
    bool ChkGroundHit() { return (m_flags & 0x20) != 0; }
    bool ChkWallHit() { return (m_flags & 0x10) != 0; }
};
struct dCcD_Stts_l { u8 _[0x3C]; };
struct dCcD_Sph_l {
    /* 0x00 */ u8 _00[0x44];
    /* 0x44 */ gptr<dCcD_Stts_l> mpStts; /* SetStts */
    /* 0x48 */ u8 _48[0x12C - 0x48];
};

struct kamome_class {
    /* 0x000 */ fopAc_ac_c actor;
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<mDoExt_McaMorf_c> mpMorf;
    /* 0x3B8 */ be<u8> mType;
    /* 0x3B9 */ be<u8> mKoMaxCount;
    /* 0x3BA */ be<u8> mPathIdx;
    /* 0x3BB */ be<u8> mSwitchNoPrm;
    /* 0x3BC */ be<s32> mGlobalTimer;
    /* 0x3C0 */ be<s8> mAnimState;
    /* 0x3C1 */ be<s8> mMoveState;
    /* 0x3C2 */ be<u8> mSwitchNo;
    /* 0x3C3 */ be<s8> m2AB;
    /* 0x3C4 */ be<s16> mJointRotY;
    /* 0x3C6 */ be<s16> mJointRotZ;
    /* 0x3C8 */ be<s16> mJointRotYTarget;
    /* 0x3CA */ be<s16> mJointRotZTarget;
    /* 0x3CC */ be<s16> m2B4;
    /* 0x3CE */ u8 _3CE[2];
    /* 0x3D0 */ be<f32> m2B8;
    /* 0x3D4 */ be<s8> mbNoDraw;
    /* 0x3D5 */ u8 _3D5[3];
    /* 0x3D8 */ cXyz mTargetPos;
    /* 0x3E4 */ be<f32> mVelocityFwdTarget;
    /* 0x3E8 */ be<f32> mVelocityFwdTargetMaxVel;
    /* 0x3EC */ be<f32> mRotVelFade;
    /* 0x3F0 */ be<f32> mRotVel;
    /* 0x3F4 */ u8 _3F4[4];
    /* 0x3F8 */ be<s16> mTimers[6];
    /* 0x404 */ be<s16> mGroundKeepTimer; /* HD: kamome_auto_move keeps the gull above the ground while it runs */
    /* 0x406 */ be<s16> mRiseTimer;
    /* 0x408 */ be<f32> mScale;
    /* 0x40C */ be<u32> mEsaProcID;
    /* 0x410 */ gptr<fopAc_ac_c> mpTargetActor;
    /* 0x414 */ be<s16> m2FC;
    /* 0x416 */ be<s8> mbUsePathMovement;
    /* 0x417 */ be<s8> mCurPointIdx;
    /* 0x418 */ be<s8> mPathIdxIncr;
    /* 0x419 */ u8 _419[3];
    /* 0x41C */ gptr<dPath> mpPath;
    /* 0x420 */ be<u8> m308;
    /* 0x421 */ u8 _421[0x430 - 0x421];
    /* 0x430 */ dBgS_AcchCir_l mAcchCir;
    /* 0x470 */ dBgS_ObjAcch_l mAcch;
    /* 0x634 */ dCcD_Stts_l mStts;
    /* 0x670 */ dCcD_Sph_l mSph;
    /* 0x79C */ be<u8> m688;
    /* 0x79D */ u8 _79D[3];
};
WWHD_OFFSET(kamome_class, mTargetPos, 0x3D8);
WWHD_OFFSET(kamome_class, mScale, 0x408);
WWHD_OFFSET(kamome_class, mpPath, 0x41C);
WWHD_OFFSET(kamome_class, mAcch, 0x470);
WWHD_OFFSET(kamome_class, mSph, 0x670);
WWHD_OFFSET(kamome_class, m688, 0x79C);
