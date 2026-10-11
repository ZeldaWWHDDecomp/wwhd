/* Wall lamp actor, GameCube field names and verified WWHD layout. Local only. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_kankyo.h"

static const u32 LAMP_VTBL = 0x10013CEC;
enum { dRes_INDEX_LAMP_BMD_LAMP_00_e = 3 };
enum { ID_AK_JN_TORCH = 0x01EA, ID_AK_JP_O_KAGEROU00 = 0x4004 };
enum { AT_TYPE_WIND = 0x200000, AT_TYPE_UNK400000 = 0x400000 };

struct lamp_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<u8> mParameters;
    /* 0x3B5 */ u8 _3B5;
    /* 0x3B6 */ be<s16> mCycleCtr;
    /* 0x3B8 */ gptr<J3DModel> mModel;
    /* 0x3BC */ dPa_followEcallBack mPa;
    /* 0x3D0 */ be<u8> mParticleInit;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ cXyz mPos;
    /* 0x3E0 */ dCcD_Stts mStts;
    /* 0x41C */ dCcD_Sph mSph;
    /* 0x548 */ be<s8> mOto;
    /* 0x549 */ u8 _549[3];
    /* 0x54C */ be<f32> mLength;
    /* 0x550 */ be<s8> mHitTimeoutLeft;
    /* 0x551 */ u8 _551[3];
    /* 0x554 */ be<f32> mHitReactCurZ;
    /* 0x558 */ be<s16> mHitAngle;
    /* 0x55A */ u8 _55A[2];
    /* 0x55C */ LIGHT_INFLUENCE mInf;  /* HD: 0x24 bytes */
    /* 0x580 */ be<f32> mParticlePower;
};
WWHD_OFFSET(lamp_class, mSph, 0x41C);
WWHD_OFFSET(lamp_class, mInf, 0x55C);
WWHD_SIZE(lamp_class, 0x584);


void dCcD_Sph__ct(dCcD_Sph*);
void dCcD_Sph__dt(dCcD_Sph*, s32);
