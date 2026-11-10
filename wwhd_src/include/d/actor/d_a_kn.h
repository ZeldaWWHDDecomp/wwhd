/* Crab.
 * HD: actor members move by 0x11C, and the callback has its vtable at zero. */
#pragma once
#include "gc_api/gc_api.h"
enum { /* HD archive order */
       dRes_INDEX_KN_BCK_PATA_e = 4,
       dRes_INDEX_KN_BCK_WAIT01_e = 5,
       dRes_INDEX_KN_BCK_WAIT02_e = 6,
       dRes_INDEX_KN_BCK_WALK_e = 7,
       dRes_INDEX_KN_BDL_KN_e = 0xA,
};

enum Mode {
    Mode_10_e = 0,
    Mode_11_e,
    Mode_12_e,
    Mode_13_e,
    Mode_14_e,
    Mode_15_e,
    Mode_16_e,
    Mode_17_e,
    Mode_18_e,
    Mode_19_e,
    Mode_20_e,
};

struct kn_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 m2B6[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ be<u8> mMode;  /* was m2BD: kn_move (in daKN_Execute 021A23B8): switch over Mode_10_e .. Mode_19_e (wait, walk, turn home, burrow, emerge, flee); daKN_Create 021A30C0 starts in Mode_10_e; daKN_Execute 021A23B8 checks it for the water ground */
    /* 0x3DA */ be<u8> mOnWater;  /* was m2BE: daKN_Execute 021A23B8: 1 when the ground under the walking crab (Mode_13_e / Mode_19_e) is a water polygon; kn_move (in daKN_Execute 021A23B8) then splashes (shibuki_set) and burrows; 0 when a walk or flight starts */
    /* 0x3DB */ be<u8> mSandBurrow;  /* was m2BF: kn_move (in daKN_Execute 021A23B8): 1 when the fleeing crab burrows with smoke (wall hit or too far from mHomePos), 0 when it burrows in water; picks smoke_set or shibuki_set on emerging and ends mSmokeEcallBack */
    /* 0x3DC */ be<u8> mBurrowed;  /* was m2C0: kn_move (in daKN_Execute 021A23B8): 0 when burrowing starts (Mode_15_e), 1 once mBurrowY reached the depth (-7, -11 for the big crab); emerging waits for it */
    /* 0x3DD */ u8 m2C1;
    /* 0x3DE */ be<s16> mTimers[4];  /* was m2C2: daKN_Execute 021A23B8: 4 timers counted down every frame; kn_move (in daKN_Execute 021A23B8): [0] walk time (20..60) / delay before sinking (20), [1] walk-back steps (20 / 8), [2] 40 after emerging (no flight, walk home), [3] minimum time burrowed (30..60) */
    /* 0x3E6 */ be<s16> mWaitLoops;  /* was m2CA: kn_move (in daKN_Execute 021A23B8): 1..3 at the start of the wait (Mode_10_e), - 1 per wait animation loop (frame 43), the walk starts at 0 */
    /* 0x3E8 */ be<f32> mBurrowY;  /* was m2CC: kn_move (in daKN_Execute 021A23B8): eased to -7 (-11 for scale above 1.5) while burrowing, back to 0 on emerging; daKN_Execute 021A23B8 lowers the model by it, BG_check (in daKN_Execute 021A23B8) checks the ground without it; daKN_Draw 021A1FA0 draws nothing while it is below 0 */
    /* 0x3EC */ be<s16> mTargetAngleY;  /* was m2D0: kn_move (in daKN_Execute 021A23B8): random near current.angle.y, the angle home (mHomePos) or away from the player; current.angle.y turns to it with step mTurnStep */
    /* 0x3EE */ be<s16> mTurnStep;  /* was m2D2: kn_move (in daKN_Execute 021A23B8): 0x800 for wait / walk, 0x100 for the flight; step of the turn to mTargetAngleY */
    /* 0x3F0 */ be<s16> m2D4;
    /* 0x3F2 */ u8 m2D6[6];
    /* 0x3F8 */ be<f32> mAnmSpeed;  /* was m2DC: kn_move (in daKN_Execute 021A23B8): eased to mAnmSpeedTarget; the morf's play speed */
    /* 0x3FC */ be<f32> mAnmSpeedTarget;  /* was m2E0: kn_move (in daKN_Execute 021A23B8): 1, or 2 while the turn to mTargetAngleY is above 0x100 (walk modes) */
    /* 0x400 */ be<f32> mTargetSpeed;  /* was m2E4: kn_move (in daKN_Execute 021A23B8): mWalkSpeed, its negative (walking back) or 0; speedF is eased to it */
    /* 0x404 */ be<f32> mWalkSpeed;  /* was m2E8: kn_move (in daKN_Execute 021A23B8): 3 or -3 (sideways either way, 3 when heading home), 40 for the flight */
    /* 0x408 */ cXyz mHomePos;  /* was m2EC: daKN_Create 021A30C0: current.pos at creation; kn_move (in daKN_Execute 021A23B8) walks back toward it when farther than 200 and burrows when the flight takes it farther */
    /* 0x414 */ csXyz mGroundAngle;  /* was m2F8: daKN_Execute 021A23B8: fopAcM_getGroundAngle on ground hits; the model is rotated by its x / z */
    /* 0x41A */ u8 m2FE[2];
    /* 0x41C */ cXyz mSmokePos;  /* was m300: kn_move (in daKN_Execute 021A23B8): current.pos (+ 20 in y on emerging) before smoke_set; smoke_set 021A2160 emits ID_AK_JT_ELEMENTSMOKE01 there */
    /* 0x428 */ u8 m30C[8];
    /* 0x430 */ dPa_smokeEcallBack mSmokeEcallBack;  /* was m314: smoke_set 021A2160: the smoke's dPa_smokeEcallBack (ended before a new smoke); daKN_Create 021A30C0 constructs it, daKN_Delete 021A2F80 removes it */
    /* 0x450 */ gptr<JPABaseEmitter> mpSmokeEmitter;  /* was m334: smoke_set 021A2160: the ID_AK_JT_ELEMENTSMOKE01 emitter (max frame 0x19, rate 3, lifetime 25 set on it) */
    /* 0x454 */ dBgS_AcchCir mAcchCir;
    /* 0x494 */ dBgS_ObjAcch mAcch;
};
WWHD_OFFSET(kn_class, mpMorf, 0x3D4);
WWHD_OFFSET(kn_class, mBurrowY, 0x3E8);
WWHD_OFFSET(kn_class, mSmokePos, 0x41C);
WWHD_OFFSET(kn_class, mAcch, 0x494);
WWHD_SIZE(kn_class, 0x658);

namespace Kn {
// HD: view of the additional emitter fields used by the crab's smoke and splash.
struct Emitter {
    u8 _00[0x34];
    be<f32> mRate;
    u8 _38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 _62[6];
    be<f32> mInitialVelOmni;
    be<f32> mInitialVelAxis;
    be<f32> mInitialVelDir;
    u8 _74[0x1AC];
    cXyz mGlobalScale;
    u8 _22C[0xC];
    cXyz mGlobalParticleScale;
    void setGlobalScale(f32 x, f32 y, f32 z) {
        mGlobalScale.x = x;
        mGlobalScale.y = y;
        mGlobalScale.z = z;
        mGlobalParticleScale.x = x;
        mGlobalParticleScale.y = y;
        mGlobalParticleScale.z = z;
    }
};
WWHD_OFFSET(Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Emitter, mInitialVelOmni, 0x68);
WWHD_OFFSET(Emitter, mInitialVelDir, 0x70);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(Emitter, 0x244);
} // namespace Kn
enum {
    DSNAP_TYPE_KN = 0x56,
    dBgS_Attr_WATER_e = 0x13,
    ID_AK_JT_ELEMENTSMOKE01 = 0x2027,
    ID_AK_JN_ELEMENTSHIBUKI00 = 0x23
};

// HD: full-width attribute result, with the ground polygon base passed by address.
s32 knGetAttributeCode(dBgS*, cBgS_PolyInfo*);
