/* Swinging chain platform: WWHD members move +0x11C from GameCube. */
#pragma once
#include "bindings.h"
struct msw_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s16> mSwayTimer;  /* was m298: msw_move (in daMsw_Execute 021D6880): +1 per frame; phase of the sine sway (mSwayOffset) and tilt (mTiltAngle); chain_Draw (in daMsw_Draw 021D6518): phase of a hit chain's shake */
    /* 0x3B6 */ be<s16> m29A;
    /* 0x3B8 */ gptr<J3DModel> mpModel;
    /* 0x3BC */ be<u8> mScaleType;  /* was m2A0: daMsw_Create 021D6F48: parameter bits 0-7 (0xFF read as 0); its switch sets scale.x/z (1: 1.5, 2: 2, 3: 3, else 1) */
    /* 0x3BD */ u8 _3BD[3];
    /* 0x3C0 */ cXyz mSwayOffset;  /* was m2A4: msw_move (in daMsw_Execute 021D6880): x / z = sine of mSwayTimer times mSwayAmp; current.pos = home.pos + it */
    /* 0x3CC */ cXyz mSwayAmp;  /* was m2B0: ride_call_back 021D6270: raised toward the rider's step (old to new position, x / z) above 10; msw_move (in daMsw_Execute 021D6880): amplitude of mSwayOffset, eased back to 0 */
    /* 0x3D8 */ cXyz mTiltAmp;  /* was m2BC: ride_call_back 021D6270: raised to the rider's step (z into x, x into z); msw_move (in daMsw_Execute 021D6880): amplitude of mTiltAngle, eased to REG0_F(9) */
    /* 0x3E4 */ csXyz mTiltAngle;  /* was m2C8: msw_move (in daMsw_Execute 021D6880): x / z = sine of mSwayTimer times mTiltAmp; shape_angle = current.angle + it */
    /* 0x3EA */ u8 _3EA[2];
    /* 0x3EC */ gptr<J3DModel> mpChainModels[4];
    /* 0x3FC */ cXyz mChainLowerPos[4];  /* was m2E0: daMsw_Execute 021D6880: [i] = the platform corner (xd / zd times 200) through the platform matrix; centre of chain i's cylinder and its hit sound; chain_Draw (in daMsw_Draw 021D6518) draws chain i from it to mChainUpperPos[i]; daMsw_IsDelete 021D6D50 deletes its sound object */
    /* 0x42C */ cXyz mChainUpperPos[4];  /* was m310: daMsw_Execute 021D6880: [i] = mChainLowerPos[i] 1000 higher while mChainInitTimer runs; chain_Draw (in daMsw_Draw 021D6518): the chain's other end */
    /* 0x45C */ dCcD_Stts mStts;
    /* 0x498 */ dCcD_Cyl mChainCyls[4];
    /* 0x958 */ be<s16> mChainHitTimer[4];  /* was m83C: daMsw_Execute 021D6880: [i] = 15 when chain i's cylinder is hit while below 10, counted down; chain_Draw (in daMsw_Draw 021D6518) shakes chain i by it */
    /* 0x960 */ be<s16> mChainInitTimer;  /* was m844: daMsw_Create 021D6F48: 10; daMsw_Execute 021D6880: counted down in the chain loop, and while not 0 the chains' upper ends are placed (mChainUpperPos) */
    /* 0x962 */ u8 m846[0x968 - 0x962];
    /* 0x968 */ Mtx34 mMtx;
    /* 0x998 */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(msw_class, mpModel, 0x3B8);
WWHD_OFFSET(msw_class, mTiltAngle, 0x3E4);
WWHD_OFFSET(msw_class, mStts, 0x45C);
WWHD_OFFSET(msw_class, mChainCyls, 0x498);
WWHD_OFFSET(msw_class, mChainHitTimer, 0x958);
WWHD_OFFSET(msw_class, mMtx, 0x968);
WWHD_OFFSET(msw_class, mpBgW, 0x998);
WWHD_SIZE(msw_class, 0x99C);

// HD: callback slots follow the opaque collision-model base.
struct MswBgCallbacks {
    u8 base[0xA8];
    be<u32> mpCrrFunc;
    u8 padAC[4];
    be<u32> mpRideCallback;
};
WWHD_OFFSET(MswBgCallbacks, mpCrrFunc, 0xA8);
WWHD_OFFSET(MswBgCallbacks, mpRideCallback, 0xB0);
WWHD_SIZE(MswBgCallbacks, 0xB4);
inline MswBgCallbacks *mswCallbacks(dBgW *bgw) {
    return reinterpret_cast<MswBgCallbacks *>(bgw);
}
enum {
    dRes_INDEX_MSW_BDL_MSWNG_e = 4,
    dRes_INDEX_MSW_BDL_OBM_CHAIN1_e = 5,
    dRes_INDEX_MSW_DZB_MSWING_e = 9
};
enum { JA_SE_LK_HIT_SBRIDGE_CHAIN = 0x2839, ID_AK_JN_NG = 0xC };
enum : u32 {
    mswActorVtable = 0x100153FC,
    mswAabVtable = 0x100153EC,
    mswSafeStringVtable = 0x100153D4
};
void fopAcM_SetMin(fopAc_ac_c *, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c *, f32, f32, f32);
BOOL mswSetBg(dBgW *, cBgD_t *, u32, Mtx34 *);
u32 mswAddAngleWords(csXyz *, csXyz *);
// HD: GHS returns the six-byte angle in r3:r4; retain the second return register.
inline void mswAddAngles(csXyz *dst, csXyz *a, csXyz *b) {
    u32 hi = mswAddAngleWords(a, b);
    u32 lo = gabi::cpu->r[4];
    dst->x = (s16)(hi >> 16);
    dst->y = (s16)hi;
    dst->z = (s16)(lo >> 16);
}
void mswConstructArray(void *, u32, u32, u32);
void mswDestroyArray(void *, u32, u32, u32, u32, u32);
void JAIZelBasic_seStart(u32, cXyz *, u32, s32);
void mswActorDestroy(void *, s32);
constexpr u32 mswCylDestructor = 0x02515A70;
constexpr u32 dBgS_MoveBGProc_Typical = 0x024EE658;
