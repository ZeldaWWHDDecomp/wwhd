/* nz_class (Rat), WWHD layout. 
 *
 * The GameCube header has no members (the actor is undecompiled there): the layout is from the WWHD
 * code. Derives from fopEn_enemy_c (+0x11C base). enemyice at 0xBE0, enemyfire (HD 0x22C) at 0xF98,
 * an 8-byte ice-material object at 0x11C4. Size 0x11CC (constructor 0230F190). */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_pt.h" /* enemyice / enemyfire / smoke callback layouts */

struct dPa_ecallBack_l { /* 0x00 */ be<u32> __vtbl; /* 0x04 */ u8 _04[0x10]; };
/* mDoExt_3DlineMat1_c (HD 0x188): vtable at +0x130, lines at +0x184 */
struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;   /* line array: +0 positions */
};
WWHD_SIZE(mDoExt_3DlineMat1_l, 0x188);

struct nz_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> mArg0;
    /* 0x3D1 */ be<u8> mType;
    /* 0x3D2 */ be<u8> mbBombLit;
    /* 0x3D3 */ u8 _3D3[0x3D4 - 0x3D3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m3D8;
    /* 0x3D9 */ be<u8> m3D9;
    /* 0x3DA */ be<u8> m3DA;
    /* 0x3DB */ u8 _3DB[0x3DC - 0x3DB];
    /* 0x3DC */ be<u8> mSmokeTimer;
    /* 0x3DD */ be<u8> mbStole;
    /* 0x3DE */ be<u8> mbFrozen;
    /* 0x3DF */ be<u8> mbInWater;
    /* 0x3E0 */ be<u32> mItemId;
    /* 0x3E4 */ be<u32> mHoleId;
    /* 0x3E8 */ be<s16> mLookCount;
    /* 0x3EA */ be<s16> mSpinSpeed;
    /* 0x3EC */ be<s16> mSearchCounter;
    /* 0x3EE */ be<s16> mDeadCounter;
    /* 0x3F0 */ be<s16> m3F0;
    /* 0x3F2 */ be<s16> m3F2;
    /* 0x3F4 */ be<s16> m3F4;
    /* 0x3F6 */ be<s16> m3F6;
    /* 0x3F8 */ be<s16> mRippleTimer;
    /* 0x3FA */ u8 _3FA[0x3FE - 0x3FA];
    /* 0x3FE */ be<s16> mTailCounter;
    /* 0x400 */ be<u32> m400;
    /* 0x404 */ be<s32> mAnmIdx;
    /* 0x408 */ be<f32> mBodyOffset;
    /* 0x40C */ be<f32> mBodyOffsetTarget;
    /* 0x410 */ be<f32> mAttnOffsetY;
    /* 0x414 */ be<s16> mHeadRotX;
    /* 0x416 */ be<s16> mHeadRotY;
    /* 0x418 */ be<s16> mHeadRotZ;
    /* 0x41A */ u8 _41A[0x41C - 0x41A];
    /* 0x41C */ be<s16> mHeadRotYTarget;
    /* 0x41E */ u8 _41E[0x420 - 0x41E];
    /* 0x420 */ cXyz mRipplePos;
    /* 0x42C */ u8 _42C[0x438 - 0x42C];
    /* 0x438 */ cXyz mHitPos;
    /* 0x444 */ cXyz mHandPos0;
    /* 0x450 */ cXyz mHandPos1;
    /* 0x45C */ be<f32> mBgOffsetY;
    /* 0x460 */ u8 _460[0x484 - 0x460];
    /* 0x484 */ cXyz mProbeEnd[8];      /* line-check ends (6 probes; [6], [7] debug copies) */
    /* 0x4E4 */ cXyz mProbeHit[8];      /* line-check hits */
    /* 0x544 */ be<s16> mWallAngle;
    /* 0x546 */ u8 _546;
    /* 0x547 */ be<u8> m547;
    /* 0x548 */ be<u8> m548;
    /* 0x549 */ u8 _549[0x54C - 0x549];
    /* 0x54C */ mDoExt_3DlineMat1_l mLineMat;
    /* 0x6D4 */ cXyz mTailPos[10];
    /* 0x74C */ cXyz mTailVel[10];
    /* 0x7C4 */ cXyz mTailEnd[2];
    /* 0x7DC */ cXyz mTailStep;
    /* 0x7E8 */ be<s16> mMoveAngle;
    /* 0x7EA */ u8 _7EA[0x7EC - 0x7EA];
    /* 0x7EC */ cXyz mFollowPos;
    /* 0x7F8 */ csXyz mFollowAngle;
    /* 0x7FE */ u8 _7FE[0x800 - 0x7FE];
    /* 0x800 */ cXyz mSmokePos;
    /* 0x80C */ csXyz mSmokeAngle;
    /* 0x812 */ u8 _812[0x814 - 0x812];
    /* 0x814 */ dPa_ecallBack_l mRippleCb;
    /* 0x828 */ dPa_smokeEcallBack_l mSmokeCb;
    /* 0x848 */ dPa_ecallBack_l mFollowCb;
    /* 0x85C */ dBgS_AcchCir mAcchCir;
    /* 0x89C */ dBgS_ObjAcch mAcch;
    /* 0xA60 */ dCcD_Stts mStts;
    /* 0xA9C */ dCcD_Cyl mCyl;
    /* 0xBCC */ u8 _BCC[0x11];
    /* 0xBDD */ be<u8> mArg3;
    /* 0xBDE */ u8 _BDE[2];
    /* 0xBE0 */ enemyice mEnemyIce;
    /* 0xF98 */ enemyfire mEnemyFire;
    /* 0x11C4 */ u8 mIceMat[8];
};
WWHD_OFFSET(nz_class, mpMorf, 0x3D4);
WWHD_OFFSET(nz_class, mTailPos, 0x6D4);
WWHD_OFFSET(nz_class, mSmokeCb, 0x828);
WWHD_OFFSET(nz_class, mCyl, 0xA9C);
WWHD_OFFSET(nz_class, mEnemyIce, 0xBE0);
WWHD_OFFSET(nz_class, mEnemyFire, 0xF98);
WWHD_SIZE(nz_class, 0x11CC);

/* daNZ_HIO_c (0x18, vtable first) */
struct daNZ_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<f32> m08;
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ be<f32> m10;
    /* 0x14 */ be<f32> m14;
};
WWHD_SIZE(daNZ_HIO_c, 0x18);
