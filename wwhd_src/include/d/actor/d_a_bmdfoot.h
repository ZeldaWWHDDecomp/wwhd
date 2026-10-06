/* bmdfoot_class (Kalle Demos floor tentacle), WWHD layout. 
 *
 * GameCube members +0x11C up to mTevstr; dKy_tevstr_c is 0x1C8 in HD (GameCube 0xB0), so the
 * members after it move by a further +0x118 (size 0xE08, GameCube 0xBD4). */
#pragma once
#include "bindings.h"

/* dPa_smokeEcallBack (HD 0x20): vtable +0, emitter +4, follow-off flag +0x12 */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x12 - 0x08];
    /* 0x12 */ be<u8> mFollowOff;
    /* 0x13 */ u8 _13[0x20 - 0x13];
    JPABaseEmitter* getEmitter() { return mpEmitter; }
};
WWHD_SIZE(dPa_smokeEcallBack_l, 0x20);

struct bmdfoot_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyVineMorf;
    /* 0x3D4 */ be<s16> m2B8;
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0[4];
    /* 0x3E4 */ be<s16> m2C8[2];
    /* 0x3E8 */ cXyz m2CC[18];
    /* 0x4C0 */ be<f32> m3A4[17];        /* GameCube m3A4[16]; the loops write [2..16] */
    /* 0x504 */ u8 _504[0x508 - 0x504];
    /* 0x508 */ be<s16> m3EC;
    /* 0x50A */ u8 _50A[0x510 - 0x50A];
    /* 0x510 */ be<u8> m3F4;
    /* 0x511 */ u8 _511[3];
    /* 0x514 */ cXyz m3F8[3];
    /* 0x538 */ dPa_followEcallBack mAsiWaitFollowCB[3];
    /* 0x574 */ dPa_smokeEcallBack_l mLAttackSmoke00CB[2];
    /* 0x5B4 */ cXyz m498[2];
    /* 0x5CC */ dPa_smokeEcallBack_l mLAttackSmoke01CB;
    /* 0x5EC */ dCcD_Stts mStts;
    /* 0x628 */ dCcD_Sph mSph[5];
    /* 0xC04 */ cXyz mAE8;
    /* 0xC10 */ gptr<mDoExt_McaMorf> mpFloorVineMorf;
    /* 0xC14 */ dKy_tevstr_c mTevstr;
    /* 0xDDC */ be<u8> mBA8;
    /* 0xDDD */ u8 _DDD[3];
    /* 0xDE0 */ cXyz mBAC;
    /* 0xDEC */ csXyz mBB8;
    /* 0xDF2 */ u8 _DF2[2];
    /* 0xDF4 */ be<f32> mBC0;
    /* 0xDF8 */ be<f32> mBC4;
    /* 0xDFC */ be<u8> mBC8;
    /* 0xDFD */ u8 _DFD[3];
    /* 0xE00 */ gptr<mDoExt_btkAnm> btk;
    /* 0xE04 */ be<s16> mBD0;
    /* 0xE06 */ be<u8> mBD2;
    /* 0xE07 */ u8 _E07;
};
WWHD_OFFSET(bmdfoot_class, m3F8, 0x514);
WWHD_OFFSET(bmdfoot_class, mStts, 0x5EC);
WWHD_OFFSET(bmdfoot_class, mSph, 0x628);
WWHD_OFFSET(bmdfoot_class, mAE8, 0xC04);
WWHD_OFFSET(bmdfoot_class, mTevstr, 0xC14);
WWHD_OFFSET(bmdfoot_class, mBA8, 0xDDC);
WWHD_OFFSET(bmdfoot_class, btk, 0xE00);
WWHD_SIZE(bmdfoot_class, 0xE08);

/* daBmdfoot_HIO_c (HD: vtable after the members, size 0xC) */
struct daBmdfoot_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<s8> m05;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<u32> __vtbl;
};
WWHD_SIZE(daBmdfoot_HIO_c, 0xC);
