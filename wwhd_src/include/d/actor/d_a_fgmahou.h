#pragma once
#include "bindings.h"
struct fgmahou_class : fopAc_ac_c {
    /* 0x3AC */ u8 field_0x290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> mOrbNumber;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3E0 */ be<s16> mAge;
    /* 0x3E2 */ be<s16> mState;
    /* 0x3E4 */ cXyz mTargetPos;
    /* 0x3F0 */ be<s16> mWeaveAngleY;  /* was field_0x2D4: move (in daFgmahou_Execute 0213D594): starts at mOrbNumber * reg, + 0xDAC per frame; shape_angle.y = home.angle.y + sin(it) * the distance term (the ball's weave) */
    /* 0x3F2 */ be<s16> mWeaveAngleX;  /* was field_0x2D6: move (in daFgmahou_Execute 0213D594): random start, + 0xCE4 per frame; shape_angle.x = home.angle.x + sin(it) * 0.75 * the distance term */
    /* 0x3F4 */ be<s16> field_0x2D8;
    /* 0x3F6 */ be<s16> mTimers[2];
    /* 0x3FA */ be<s16> field_0x2DE;
    /* 0x3FC */ be<s16> mReflectFrame;  /* was field_0x2E0: move (in daFgmahou_Execute 0213D594): set 1 when the ball is sent back at Phantom Ganon (state 5), + 1 per frame and cleared after 8 */
    /* 0x3FE */ be<s16> field_0x2E2;
    /* 0x400 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x404 */ dBgS_AcchCir mCir;
    /* 0x444 */ dBgS_ObjAcch mAcch;
    /* 0x608 */ dCcD_Stts mStts;
    /* 0x644 */ dCcD_Sph mAtSph;
    /* 0x770 */ dCcD_Sph mTgSph;
    /* 0x89C */ be<s8> mDeleteTimer;  /* was field_0x780: move (in daFgmahou_Execute 0213D594) sets 50 when the ball hits or is lost; daFgmahou_Execute then stops moving it, ends its effect at 50 and deletes the actor at 0 */
    /* 0x89D */ u8 _89D[3];
};
WWHD_OFFSET(fgmahou_class, mPhs, 0x3C8);
WWHD_OFFSET(fgmahou_class, mpEmitter, 0x400);
WWHD_OFFSET(fgmahou_class, mAcch, 0x444);
WWHD_OFFSET(fgmahou_class, mTgSph, 0x770);
WWHD_OFFSET(fgmahou_class, mDeleteTimer, 0x89C);
WWHD_SIZE(fgmahou_class, 0x8A0);

// HD: private views cover only the emitter and boss fields this actor uses.
struct FgmahouEmitterHD {
    u8 pad0[0x5C];
    be<s32> mMaxFrame;
    u8 pad60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    u8 pad220[12];
    cXyz mGlobalTranslation;
    u8 pad238[0x254 - 0x238];
    be<u32> mFlags;
};
WWHD_OFFSET(FgmahouEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(FgmahouEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(FgmahouEmitterHD, mFlags, 0x254);
struct FgmahouBossHD : fopAc_ac_c {
    u8 pad3AC[0x8BF - 0x3AC];
    be<u8> mLaughRequest; /* was m68B: fganon_class mLaughRequest (0x8BF); move (in daFgmahou_Execute 0213D594, d_a_fgmahou) sets 1 when the dark ball hits the player; fganon's daFganon_Execute 02134B28 then starts the laugh */
};
WWHD_OFFSET(FgmahouBossHD, mLaughRequest, 0x8BF);
void fgmahouSetEmitterMatrix(const Mtx34 *, Mtx34 *, cXyz *);
dCcD_GObjInf *fgmahouGetAtHitObj(dCcD_GObjInf *);
void *fgmahouBtkConstruct(void *);
mDoExt_brkAnm *fgmahouBrkConstruct(void *);
void fgmahouSphereConstruct(dCcD_Sph *);
void fgmahouSphereDestroy(dCcD_Sph *, s32);
void fgmahouAcchDestroy(dBgS_Acch *, s32);
void fgmahouCircleDestroy(void *, s32);
void fgmahouActorDestroy(fopAc_ac_c *, s32);

// HD: retain guest callback/vtable targets; host member pointers have a different ABI.
enum : u32 {
    FGMAHOU_BOSS_SEARCH = 0x0213D544,
    FGMAHOU_HEAP_INIT = 0x0213E38C,
    FGMAHOU_ACCH_VTABLE = 0x1000F028,
    FGMAHOU_POLY_VTABLE = 0x1000F048,
    FGMAHOU_GROUP_VTABLE = 0x1000F038,
};
WWHD_OFFSET(FgmahouEmitterHD, mMaxFrame, 0x5C);
WWHD_SIZE(FgmahouEmitterHD, 0x258);
WWHD_SIZE(FgmahouBossHD, 0x8C0);
