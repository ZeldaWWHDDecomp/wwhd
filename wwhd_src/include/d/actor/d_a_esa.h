/** All-Purpose Bait: tww members on the larger HD actor base (+0x11C).
 * HD: the unused GC byte after the ripple callback is absent; the following flags move
 * by +0x11B before alignment restores +0x11C. The bob angle is appended at 0x3E0.
 */
#pragma once
#include "bindings.h"

struct fopAcM_prmBase_class {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ be<u16> setID;
};
WWHD_OFFSET(fopAcM_prmBase_class, position, 4);
WWHD_OFFSET(fopAcM_prmBase_class, angle, 0x10);
WWHD_SIZE(fopAcM_prmBase_class, 0x18);
struct fopAcM_prmScale_class {
    be<u8> x, y, z;
};
WWHD_SIZE(fopAcM_prmScale_class, 3);
struct fopAcM_prm_class {
    /* 0x00 */ fopAcM_prmBase_class base;
    /* 0x18 */ fopAcM_prmScale_class scale;
    /* 0x1B */ be<u8> gbaName;
    /* 0x1C */ be<u32> parent_id;
    /* 0x20 */ be<s8> argument;
    /* 0x21 */ be<s8> room_no;
    /* 0x22 */ u8 _22[2];
};
WWHD_OFFSET(fopAcM_prm_class, base, 0);
WWHD_OFFSET(fopAcM_prm_class, scale, 0x18);
WWHD_OFFSET(fopAcM_prm_class, parent_id, 0x1C);
WWHD_OFFSET(fopAcM_prm_class, room_no, 0x21);
WWHD_SIZE(fopAcM_prm_class, 0x24);
struct EsaRippleCallback {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
    JPABaseEmitter* getEmitter() {
        return mpEmitter;
    }
    void setRate(f32 r) {
        mRate = r;
    }
};
WWHD_OFFSET(EsaRippleCallback, mpEmitter, 4);
WWHD_OFFSET(EsaRippleCallback, mRate, 0x10);
WWHD_SIZE(EsaRippleCallback, 0x14);

struct esa_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ u8 _3B5[3];
    /* 0x3B8 */ be<f32> mGroundHeight; /* HD: the water height of the last bg_check */
    /* 0x3BC */ be<s8> mActionState;
    /* 0x3BD */ be<s8> mState;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ EsaRippleCallback field_0x2A4;
    /* 0x3D4 */ be<u8> field_0x2B9;
    /* 0x3D5 */ be<u8> field_0x2BA;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ gptr<J3DModel> mpModel;
    /* 0x3DC */ be<s16> mTimer[2];
    /* 0x3E0 */ be<s16> mBobAngle; /* HD: bobbing phase on the water */
};
WWHD_OFFSET(esa_class, field_0x2A4, 0x3C0);
WWHD_OFFSET(esa_class, mpModel, 0x3D8);
WWHD_OFFSET(esa_class, mBobAngle, 0x3E0);

WWHD_SIZE(esa_class, 0x3E2);
WWHD_OFFSET(esa_class, mPhase, 0x3AC);
WWHD_OFFSET(esa_class, field_0x298, 0x3B4);
WWHD_OFFSET(esa_class, mGroundHeight, 0x3B8);
WWHD_OFFSET(esa_class, mActionState, 0x3BC);
WWHD_OFFSET(esa_class, mState, 0x3BD);
WWHD_OFFSET(esa_class, field_0x2B9, 0x3D4);
WWHD_OFFSET(esa_class, field_0x2BA, 0x3D5);
WWHD_OFFSET(esa_class, mTimer, 0x3DC);

struct EsaPlayerView : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3F0 - 0x3AC];
    /* 0x3F0 */ cXyz mLeftHandPos;
};
WWHD_OFFSET(EsaPlayerView, mLeftHandPos, 0x3F0);
WWHD_SIZE(EsaPlayerView, 0x3FC);
struct EsaHeaderStatics {
    /* 0x00 */ be<f32> mNegativePi;
    /* 0x04 */ be<f32> mPositivePi;
    /* 0x08 */ be<u32> mRippleGuard;
    /* 0x0C */ be<u8> mFirstObject;
    /* 0x0D */ be<u8> mSecondObject;
    /* 0x0E */ u8 _0E[2];
    /* 0x10 */ be<u32> mZero[4];
};
WWHD_OFFSET(EsaHeaderStatics, mRippleGuard, 8);
WWHD_OFFSET(EsaHeaderStatics, mFirstObject, 0xC);
WWHD_OFFSET(EsaHeaderStatics, mZero, 0x10);
WWHD_SIZE(EsaHeaderStatics, 0x20);
struct EsaDestructorRecord {
    u8 mRecord[0xC];
};
WWHD_SIZE(EsaDestructorRecord, 0xC);

struct EsaDestructorRecords {
    EsaDestructorRecord mRecords[3];
};
WWHD_OFFSET(EsaDestructorRecords, mRecords, 0);
WWHD_SIZE(EsaDestructorRecords, 0x24);
