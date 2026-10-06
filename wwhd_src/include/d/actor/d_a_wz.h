/* d_a_wz.h (WWHD): Wizzrobe. 
 * Layout from the WWHD code (GameCube fields +0x11C); names by WWHD offset where the GameCube
 * header has none. Size 0x10B0 (GameCube 0xF8C, the HD dCcD/J3D helpers add the rest). */
#pragma once
#include "bindings.h"

struct wz_class : fopEn_enemy_c {
    /* 0x3C8 */ u8 mPhase[0x8]; /* request_of_phase_process_class */
    /* 0x3D0 */ be<u32> mpMorf; /* GameCube m2B4: body */
    /* 0x3D4 */ be<u32> mpRodMorf; /* the rod */
    /* 0x3D8 */ be<u32> mpMorf2; /* mini-boss second body */
    /* 0x3DC */ be<u32> mpBrkAnm;
    /* 0x3E0 */ be<u32> mpBtkAnm;
    /* 0x3E4 */ u8 _3E4[0x3E8 - 0x3E4];
    /* 0x3E8 */ Mtx34 mBallMtx; /* summon door / ball matrix */
    /* 0x418 */ cXyz mHandPos; /* joint 0x11 */
    /* 0x424 */ be<f32> mRodScaleX;
    /* 0x428 */ be<f32> mRodScaleY;
    /* 0x42C */ be<f32> mRodScaleZ;
    /* 0x430 */ cXyz mTamaTarget; /* GameCube m314 */
    /* 0x43C */ cXyz mRodTipPos;
    /* 0x448 */ u8 _448[0x44C - 0x448];
    /* 0x44C */ be<f32> mBaseY;
    /* 0x450 */ u8 _450[0x454 - 0x450];
    /* 0x454 */ cXyz mSummonPos;
    /* 0x460 */ be<u32> mParentId;
    /* 0x464 */ be<u8> mBehaviorType; /* GameCube 0x348 */
    /* 0x465 */ be<u8> mEnemySummonTableIndex;
    /* 0x466 */ be<u8> mEnableSpawnSwitch;
    /* 0x467 */ be<u8> mDisableSpawnOnDeathSwitch;
    /* 0x468 */ be<u8> m468;
    /* 0x469 */ be<u8> m469;
    /* 0x46A */ be<u8> mHitKind;
    /* 0x46B */ be<u8> mHitLock;
    /* 0x46C */ be<u8> mIsMiniBoss; /* GameCube 0x350 */
    /* 0x46D */ be<u8> m46D;
    /* 0x46E */ u8 _46E[0x46F - 0x46E];
    /* 0x46F */ be<u8> m46F;
    /* 0x470 */ be<u8> m470;
    /* 0x471 */ u8 _471[0x478 - 0x471];
    /* 0x478 */ be<u32> mChildId[20]; /* summoner: the summoned enemies */
    /* 0x4C8 */ be<u8> mChildFlag[20];
    /* 0x4DC */ u8 _4DC[0x4F0 - 0x4DC];
    /* 0x4F0 */ be<s16> m4F0;
    /* 0x4F2 */ u8 _4F2[0x4FA - 0x4F2];
    /* 0x4FA */ be<s16> m4FA[4];
    /* 0x502 */ u8 _502[0x504 - 0x502];
    /* 0x504 */ be<s16> mFuwaAngle;
    /* 0x506 */ be<s16> mAlpha;
    /* 0x508 */ be<s32> mSummonWave; /* mini-boss: the summon table row */
    /* 0x50C */ be<s32> mSummonSlot;
    /* 0x510 */ be<s16> m510;
    /* 0x512 */ be<s16> mSummonCount;
    /* 0x514 */ u8 _514[0x518 - 0x514];
    /* 0x518 */ be<s32> mCurrBckIdx;
    /* 0x51C */ be<f32> mCorrectionOffsetY;
    /* 0x520 */ be<f32> mAcchWallH;
    /* 0x524 */ be<f32> mAcchWallR;
    /* 0x528 */ u8 _528[0x540 - 0x528];
    /* 0x540 */ be<u32> m540;
    /* 0x544 */ u8 _544[0x551 - 0x544];
    /* 0x551 */ be<u8> mSummonSw;
    /* 0x552 */ u8 _552[0x580 - 0x552];
    /* 0x580 */ u8 mAcchCir[0x40];
    /* 0x5C0 */ u8 mAcch[0x1C4];
    /* 0x784 */ u8 mStts[0x3C];
    /* 0x7C0 */ u8 mBodyCyl[0x130];
    /* 0x8F0 */ u8 mBallSph[0x12C]; /* GameCube m7D8 (dCcD_Sph) */
    /* 0xA1C */ u8 mLight[0x24]; /* LIGHT_INFLUENCE */
    /* 0xA40 */ u8 _A40[0xA44 - 0xA40];
    /* 0xA44 */ be<s16> mA44;
    /* 0xA46 */ be<u8> mA46;
    /* 0xA47 */ u8 _A47[0xA48 - 0xA47];
    /* 0xA48 */ be<f32> mA48;
    /* 0xA4C */ be<u8> mA4C;
    /* 0xA4D */ u8 _A4D[0xA4E - 0xA4D];
    /* 0xA4E */ be<s16> mIceTimer;
    /* 0xA50 */ u8 _A50[0xA70 - 0xA50];
    /* 0xA70 */ u8 mStts2[0x3C];
    /* 0xAAC */ u8 mCyl2[0x130];
    /* 0xBDC */ u8 _BDC[0xBEC - 0xBDC];
    /* 0xBEC */ be<f32> mBEC;
    /* 0xBF0 */ u8 _BF0[0xBF4 - 0xBF0];
    /* 0xBF4 */ u8 mAcchCir2[0x40];
    /* 0xC34 */ u8 mAcch2[0x1C4];
    /* 0xDF8 */ u8 mEnemyFire[0x22C];
    /* 0x1024 */ u8 _1024[0x1038 - 0x1024];
    /* 0x1038 */ u8 mFollowCb[0x64];
    /* 0x109C */ u8 mInvisModel[0x8];
    /* 0x10A4 */ u8 mInvisModel2[0x8];
    /* 0x10AC */ u8 _10AC[0x10B0 - 0x10AC];
};
WWHD_SIZE(wz_class, 0x10B0);
WWHD_OFFSET(wz_class, mPhase, 0x3C8);
WWHD_OFFSET(wz_class, mpMorf, 0x3D0);
WWHD_OFFSET(wz_class, mpRodMorf, 0x3D4);
WWHD_OFFSET(wz_class, mpMorf2, 0x3D8);
WWHD_OFFSET(wz_class, mpBrkAnm, 0x3DC);
WWHD_OFFSET(wz_class, mpBtkAnm, 0x3E0);
WWHD_OFFSET(wz_class, mBallMtx, 0x3E8);
WWHD_OFFSET(wz_class, mHandPos, 0x418);
WWHD_OFFSET(wz_class, mRodScaleX, 0x424);
WWHD_OFFSET(wz_class, mRodScaleY, 0x428);
WWHD_OFFSET(wz_class, mRodScaleZ, 0x42C);
WWHD_OFFSET(wz_class, mTamaTarget, 0x430);
WWHD_OFFSET(wz_class, mRodTipPos, 0x43C);
WWHD_OFFSET(wz_class, mBaseY, 0x44C);
WWHD_OFFSET(wz_class, mSummonPos, 0x454);
WWHD_OFFSET(wz_class, mParentId, 0x460);
WWHD_OFFSET(wz_class, mBehaviorType, 0x464);
WWHD_OFFSET(wz_class, mEnemySummonTableIndex, 0x465);
WWHD_OFFSET(wz_class, mEnableSpawnSwitch, 0x466);
WWHD_OFFSET(wz_class, mDisableSpawnOnDeathSwitch, 0x467);
WWHD_OFFSET(wz_class, m468, 0x468);
WWHD_OFFSET(wz_class, m469, 0x469);
WWHD_OFFSET(wz_class, mHitKind, 0x46A);
WWHD_OFFSET(wz_class, mHitLock, 0x46B);
WWHD_OFFSET(wz_class, mIsMiniBoss, 0x46C);
WWHD_OFFSET(wz_class, m46D, 0x46D);
WWHD_OFFSET(wz_class, m46F, 0x46F);
WWHD_OFFSET(wz_class, m470, 0x470);
WWHD_OFFSET(wz_class, mChildId, 0x478);
WWHD_OFFSET(wz_class, mChildFlag, 0x4C8);
WWHD_OFFSET(wz_class, m4F0, 0x4F0);
WWHD_OFFSET(wz_class, m4FA, 0x4FA);
WWHD_OFFSET(wz_class, mSummonWave, 0x508);
WWHD_OFFSET(wz_class, mSummonSlot, 0x50C);
WWHD_OFFSET(wz_class, mSummonCount, 0x512);
WWHD_OFFSET(wz_class, mFuwaAngle, 0x504);
WWHD_OFFSET(wz_class, mAlpha, 0x506);
WWHD_OFFSET(wz_class, m510, 0x510);
WWHD_OFFSET(wz_class, mCurrBckIdx, 0x518);
WWHD_OFFSET(wz_class, mCorrectionOffsetY, 0x51C);
WWHD_OFFSET(wz_class, mAcchWallH, 0x520);
WWHD_OFFSET(wz_class, mAcchWallR, 0x524);
WWHD_OFFSET(wz_class, m540, 0x540);
WWHD_OFFSET(wz_class, mSummonSw, 0x551);
WWHD_OFFSET(wz_class, mAcchCir, 0x580);
WWHD_OFFSET(wz_class, mAcch, 0x5C0);
WWHD_OFFSET(wz_class, mStts, 0x784);
WWHD_OFFSET(wz_class, mBodyCyl, 0x7C0);
WWHD_OFFSET(wz_class, mBallSph, 0x8F0);
WWHD_OFFSET(wz_class, mLight, 0xA1C);
WWHD_OFFSET(wz_class, mA44, 0xA44);
WWHD_OFFSET(wz_class, mA46, 0xA46);
WWHD_OFFSET(wz_class, mA48, 0xA48);
WWHD_OFFSET(wz_class, mA4C, 0xA4C);
WWHD_OFFSET(wz_class, mIceTimer, 0xA4E);
WWHD_OFFSET(wz_class, mStts2, 0xA70);
WWHD_OFFSET(wz_class, mCyl2, 0xAAC);
WWHD_OFFSET(wz_class, mBEC, 0xBEC);
WWHD_OFFSET(wz_class, mAcchCir2, 0xBF4);
WWHD_OFFSET(wz_class, mAcch2, 0xC34);
WWHD_OFFSET(wz_class, mEnemyFire, 0xDF8);
WWHD_OFFSET(wz_class, mFollowCb, 0x1038);
WWHD_OFFSET(wz_class, mInvisModel, 0x109C);
WWHD_OFFSET(wz_class, mInvisModel2, 0x10A4);
