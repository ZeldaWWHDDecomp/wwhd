/* Unused small bird, WWHD layout. GC field_0x29c[2] and mFrameCtrl are absent:
 * members move by+0x11C up to mpModel and by+0x100 from mpModelWing. Size0x420. */
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/f_pc/f_pc.h"
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_KT_BMD_KT_HANE_e = 0xA,
    dRes_INDEX_KT_BMD_KT_MODEL_e = 0xB,
};

/* HD: the GameCube field_0x29c[2] and mFrameCtrl (0x29C..0x2B8) are gone: GameCube +0x11C up to
 * mpModel, +0x100 from mpModelWing on. Size 0x420. */
struct kt_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<J3DModel> mpModelWing;
    /* 0x3BC */ be<f32> mGroundY;
    /* 0x3C0 */ be<f32> mLiftY;
    /* 0x3C4 */ be<f32> mLiftYTimer;
    /* 0x3C8 */ be<f32> mSpeedLerp;
    /* 0x3CC */ be<s32> mWingTimer;
    /* 0x3D0 */ be<f32> mSpeedFwd;
    /* 0x3D4 */ cXyz mSpeedVel;
    /* 0x3E0 */ be<f32> mWingScale;
    /* 0x3E4 */ cXyz field_0x2e4;
    /* 0x3F0 */ cXyz mTargetPos;
    /* 0x3FC */ cXyz mTargetPosHome;
    /* 0x408 */ cXyz mHomePos;
    /* 0x414 */ be<s16> mTimer[3];
    /* 0x41A */ be<s16> mAngleRoll;
    /* 0x41C */ be<s8> mMode;
    /* 0x41D */ be<u8> field_0x31d;
    /* 0x41E */ be<u8> mHitGround;
    /* 0x41F */ be<u8> field_0x31f;
};
WWHD_OFFSET(kt_class, mpModelWing, 0x3B8);
WWHD_OFFSET(kt_class, mWingScale, 0x3E0);
WWHD_OFFSET(kt_class, mTimer, 0x414);
WWHD_SIZE(kt_class, 0x420);

// HD: copies without arithmetic retain signalling-NaN bits.
inline void kt_copyFloatBits(be<f32>* dst, const be<f32>* src) {
    *reinterpret_cast<be<u32>*>(dst) = *reinterpret_cast<const be<u32>*>(src);
}
struct KtAppend {
    struct Base {
        be<u32> parameters;
        cXyz position;
        csXyz angle;
        be<u16> setID;
    } base;
};
WWHD_OFFSET(KtAppend, base.position, 4);
WWHD_OFFSET(KtAppend, base.angle, 0x10);
WWHD_SIZE(KtAppend, 0x18);
enum { fpcNm_KT_e = 0xB8 };
enum KtMode {
    KT_FLY = 0,
    KT_APPROACH_GROUND = 1,
    KT_APPROACH_PLAYER = 2,
    KT_LAND = 8,
    KT_APPROACH_HEAD = 9,
    KT_HOP = 10,
    KT_WAIT = 11,
    KT_PERCH = 20
};
void mDoExt_modelUpdate(J3DModel*);
BOOL CPad_CHECK_TRIG_LEFT(s32);
KtAppend* fopAcM_CreateAppend();
void fpcSCtRq_Request(layer_class*, s16, u32, u32, KtAppend*);
void kt_actor_dt(fopAc_ac_c*, s32);

WWHD_OFFSET(kt_class, mPhs, 0x3AC);
WWHD_OFFSET(kt_class, mpModel, 0x3B4);
WWHD_OFFSET(kt_class, mGroundY, 0x3BC);
WWHD_OFFSET(kt_class, mLiftY, 0x3C0);
WWHD_OFFSET(kt_class, mLiftYTimer, 0x3C4);
WWHD_OFFSET(kt_class, mSpeedLerp, 0x3C8);
WWHD_OFFSET(kt_class, mWingTimer, 0x3CC);
WWHD_OFFSET(kt_class, mSpeedFwd, 0x3D0);
WWHD_OFFSET(kt_class, mSpeedVel, 0x3D4);
WWHD_OFFSET(kt_class, field_0x2e4, 0x3E4);
WWHD_OFFSET(kt_class, mTargetPos, 0x3F0);
WWHD_OFFSET(kt_class, mTargetPosHome, 0x3FC);
WWHD_OFFSET(kt_class, mHomePos, 0x408);
WWHD_OFFSET(kt_class, mAngleRoll, 0x41A);
WWHD_OFFSET(kt_class, mMode, 0x41C);
WWHD_OFFSET(kt_class, field_0x31d, 0x41D);
WWHD_OFFSET(kt_class, mHitGround, 0x41E);
WWHD_OFFSET(kt_class, field_0x31f, 0x41F);
