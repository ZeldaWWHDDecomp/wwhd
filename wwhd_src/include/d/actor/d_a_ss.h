/* d_a_ss (WWHD layout). 
 * The GameCube header has no members: the layout here is measured from the WWHD code
 * (constructor 0248D390 allocates 0x3E20). */
#pragma once
#include "bindings.h"

/* one segment of a tentacle line */
struct ss_seg {
    /* 0x0 */ cXyz mPos;
    /* 0xC */ be<u8> mSize;            /* line width index */
    /* 0xD */ u8 _D[3];
};
WWHD_SIZE(ss_seg, 0x10);

/* one tentacle ("hand"), 0x58C bytes (constructor 0248D724, destructor 0248D77C) */
struct ss_s {
    /* 0x000 */ be<u8> mMode;           /* 0 set, 1 move, 2 cut */
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ cXyz mPos;              /* root position */
    /* 0x010 */ u8 _010[2];
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngleZ;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;            /* cut: flight speed */
    /* 0x024 */ be<f32> mHeight;
    /* 0x028 */ be<f32> mSwing;
    /* 0x02C */ be<f32> mGround;
    /* 0x030 */ be<s16> mSwingPhase;
    /* 0x032 */ be<s16> mSwingStep;
    /* 0x034 */ be<s16> mRotSpeed;
    /* 0x036 */ be<s16> mHoldTimer;
    /* 0x038 */ be<s16> mLandTimer;
    /* 0x03A */ be<s8> mLen;            /* visible segments */
    /* 0x03B */ be<s8> mBurn;
    /* 0x03C */ dCcD_Sph mSph[4];
    /* 0x4EC */ ss_seg mSeg[10];
};
WWHD_OFFSET(ss_s, mSph, 0x3C);
WWHD_OFFSET(ss_s, mSeg, 0x4EC);
WWHD_SIZE(ss_s, 0x58C);

/* mDoExt_3DlineMat0_c (HD 0x148): vtable at +0x130, lines at +0x144 */
struct mDoExt_3DlineMat0_ss {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x144 - 0x134];
    /* 0x144 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat0_ss, 0x148);

struct ss_class : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<s8> m3D0;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> mType;         /* param & 0xFF */
    /* 0x3D9 */ be<u8> mKind;         /* (param >> 8) & 0xFF */
    /* 0x3DA */ be<u8> mSwNo;         /* param >> 24 (0xFF -> 0) */
    /* 0x3DB */ u8 _3DB;
    /* 0x3DC */ be<s16> mCounter;
    /* 0x3DE */ u8 _3DE[2];
    /* 0x3E0 */ be<s16> mMode;
    /* 0x3E2 */ be<s16> mPrevMode;
    /* 0x3E4 */ be<s16> mTimer[4];
    /* 0x3EC */ be<s16> mDamageTimer;
    /* 0x3EE */ be<s16> mHeadRotX;
    /* 0x3F0 */ be<s16> mHeadRotY;
    /* 0x3F2 */ u8 _3F2[2];
    /* 0x3F4 */ be<u8> m3F4;
    /* 0x3F5 */ u8 _3F5[3];
    /* 0x3F8 */ ss_s mHand[10];
    /* 0x3B70 */ mDoExt_3DlineMat0_ss mLineMat;
    /* 0x3CB8 */ dCcD_Stts mStts;
    /* 0x3CF4 */ dCcD_Sph mSph;
};
WWHD_OFFSET(ss_class, mPhs, 0x3C8);
WWHD_OFFSET(ss_class, mHand, 0x3F8);
WWHD_OFFSET(ss_class, mLineMat, 0x3B70);
WWHD_OFFSET(ss_class, mStts, 0x3CB8);
WWHD_OFFSET(ss_class, mSph, 0x3CF4);
WWHD_SIZE(ss_class, 0x3E20);
