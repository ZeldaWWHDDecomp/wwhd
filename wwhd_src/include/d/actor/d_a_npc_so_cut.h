/* Fishman event-cut views. HD actor fields use the shared +0x118/+0x178 layout.
 * Private implementation layouts. */
#pragma once
#include "d/actor/d_a_esa.h"
#include "d/actor/d_a_npc_so.h"
#include "gc_api/d/d_com_inf_game.h"

struct SoCutRegisters {
    u8 reserved0[0x18];
    /* 0x18 */ be<f32> REG0_F4;
    /* 0x1C */ be<f32> REG0_F5;
    /* 0x20 */ be<f32> REG0_F6;
    /* 0x24 */ be<f32> REG0_F7;
    u8 reserved28[0x488];
    /* 0x4B0 */ be<f32> REG8_F10;
    /* 0x4B4 */ be<f32> REG8_F11;
    u8 reserved4B8[0x4A];
    /* 0x502 */ be<s16> REG8_S1;
    u8 reserved504[0x8];
    /* 0x50C */ be<s16> REG8_S6;
    u8 reserved50E[0x1D6];
    /* 0x6E4 */ be<f32> REG12_F7;
    u8 reserved6E8[0x8];
    /* 0x6F0 */ be<f32> REG12_F10;
    u8 reserved6F4[0x4C];
    /* 0x740 */ be<s16> REG12_S0;
    /* 0x742 */ be<s16> REG12_S1;
    /* 0x744 */ be<s16> REG12_S2;
    u8 reserved746[0x6];
    /* 0x74C */ be<s16> REG12_S6;
};
WWHD_OFFSET(SoCutRegisters, REG0_F4, 0x18);
WWHD_OFFSET(SoCutRegisters, REG0_F5, 0x1C);
WWHD_OFFSET(SoCutRegisters, REG0_F6, 0x20);
WWHD_OFFSET(SoCutRegisters, REG0_F7, 0x24);
WWHD_OFFSET(SoCutRegisters, REG8_F10, 0x4B0);
WWHD_OFFSET(SoCutRegisters, REG8_F11, 0x4B4);
WWHD_OFFSET(SoCutRegisters, REG8_S1, 0x502);
WWHD_OFFSET(SoCutRegisters, REG8_S6, 0x50C);
WWHD_OFFSET(SoCutRegisters, REG12_F7, 0x6E4);
WWHD_OFFSET(SoCutRegisters, REG12_F10, 0x6F0);
WWHD_OFFSET(SoCutRegisters, REG12_S0, 0x740);
WWHD_OFFSET(SoCutRegisters, REG12_S1, 0x742);
WWHD_OFFSET(SoCutRegisters, REG12_S2, 0x744);
WWHD_OFFSET(SoCutRegisters, REG12_S6, 0x74C);
WWHD_SIZE(SoCutRegisters, 0x74E);

inline SoCutRegisters& soCutRegisters() { return gabi::object_at<SoCutRegisters>(0x1047B608); }

struct SoCutHIO {
    u8 reserved0[0x3C];
    /* 0x3C */ be<f32> m3C;
    u8 reserved40[0x10];
    /* 0x50 */ be<f32> m50;
    u8 reserved54[0x14];
    /* 0x68 */ be<s16> m68;
    u8 reserved6A[0x16];
    /* 0x80 */ be<f32> m80;
    /* 0x84 */ be<f32> m84;
    /* 0x88 */ be<f32> m88;
    /* 0x8C */ be<f32> m8C;
    /* 0x90 */ be<f32> m90;
    /* 0x94 */ be<u8> m94;
    u8 tail[0x3];
};
WWHD_OFFSET(SoCutHIO, m3C, 0x3C);
WWHD_OFFSET(SoCutHIO, m50, 0x50);
WWHD_OFFSET(SoCutHIO, m68, 0x68);
WWHD_OFFSET(SoCutHIO, m80, 0x80);
WWHD_OFFSET(SoCutHIO, m84, 0x84);
WWHD_OFFSET(SoCutHIO, m88, 0x88);
WWHD_OFFSET(SoCutHIO, m8C, 0x8C);
WWHD_OFFSET(SoCutHIO, m90, 0x90);
WWHD_OFFSET(SoCutHIO, m94, 0x94);
WWHD_SIZE(SoCutHIO, 0x98);

inline SoCutHIO& soCutHIO() { return gabi::object_at<SoCutHIO>(0x104686CC); }

struct SoCutPlayer {
    u8 reserved0[0x3C0];
    /* 0x3C0 */ be<u32> mStatus;
    u8 reserved3C4[0x5C];
    /* 0x420 */ be<u16> mDemoType;
    u8 reserved422[0x6];
    /* 0x428 */ be<u32> mDemoParam0;
    u8 reserved42C[0x4];
    /* 0x430 */ be<u32> mDemoMode;
};
WWHD_OFFSET(SoCutPlayer, mStatus, 0x3C0);
WWHD_OFFSET(SoCutPlayer, mDemoType, 0x420);
WWHD_OFFSET(SoCutPlayer, mDemoParam0, 0x428);
WWHD_OFFSET(SoCutPlayer, mDemoMode, 0x430);
WWHD_SIZE(SoCutPlayer, 0x434);

inline SoCutPlayer& soCutPlayer(fopAc_ac_c* player) {
    return *reinterpret_cast<SoCutPlayer*>(player);
}

struct SoCutEmitter {
    u8 reserved0[0x238];
    /* 0x238 */ cXyz mGlobalParticleScale;
};
WWHD_OFFSET(SoCutEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(SoCutEmitter, 0x244);

inline SoCutEmitter& soCutEmitter(u32 address) { return gabi::object_at<SoCutEmitter>(address); }

struct SoCutPlayerVtable {
    u8 reserved0[0x114];
    /* 0x114 */ be<u32> setPlayerPosAndAngle;
};
WWHD_OFFSET(SoCutPlayerVtable, setPlayerPosAndAngle, 0x114);
WWHD_SIZE(SoCutPlayerVtable, 0x118);

inline SoCutPlayerVtable& soCutPlayerVtable(u32 address) {
    return gabi::object_at<SoCutPlayerVtable>(address);
}

struct SoCutCameraEntry {
    /* 0x0 */ be<u32> mpCamera;
    u8 tail[0x30];
};
WWHD_OFFSET(SoCutCameraEntry, mpCamera, 0x0);
WWHD_SIZE(SoCutCameraEntry, 0x34);

struct SoCutPlay {
    u8 reserved0[0x5AF8];
    /* 0x5AF8 */ SoCutCameraEntry mCamera[1];
    u8 reserved5B2C[0xAF];
    /* 0x5BDB */ be<u8> mFmapFishFlag;
};
WWHD_OFFSET(SoCutPlay, mCamera, 0x5AF8);
WWHD_OFFSET(SoCutPlay, mFmapFishFlag, 0x5BDB);
WWHD_SIZE(SoCutPlay, 0x5BDC);

inline SoCutPlay& soCutPlay(u32 address) { return gabi::object_at<SoCutPlay>(address); }
inline const be<u8>* soCutBytes(u32 address) { return gabi::at<be<u8>>(address); }
inline u32 soCutCalcMtx() { return gabi::object_at<be<u32>>(0x1018C7B0); }

struct SoCutCirclePath {
    /* 0x00 */ cXyz mTranslation;
    /* 0x0C */ cXyz mPos;
    /* 0x18 */ be<f32> mRadius;
    /* 0x1C */ u8 _1C[4];
    /* 0x20 */ be<s16> mAngle;
    /* 0x22 */ be<s16> mAngleSpeed;
};
WWHD_SIZE(SoCutCirclePath, 0x24);

void soCut_offsetZero(daNpc_So_c*);
daNpc_So_c* soCut_offsetZeroResult(daNpc_So_c*);
void soCut_offsetDive(daNpc_So_c*);
void soCut_offsetSwim(daNpc_So_c*);
void soCut_offsetAppear(daNpc_So_c*);
void soCut_setAnm(daNpc_So_c*, s8, bool);
f32 soCut_getWaterY(cXyz*, dBgS_ObjAcch*);
s32 soCut_calcTimer(be<s32>*);
void soCut_subtract(cXyz*, cXyz*, cXyz*);
f64 soCut_squareMagnitude(cXyz*);
f64 soCut_sqrtUnrounded(f64);
void soCut_resetCamera(u32, cXyz*, cXyz*);
void soCut_startCamera(u32);
void soCut_stopCamera(u32);
void soCut_trimCamera(u32, s32);
void soCut_setCamera(u32, cXyz*, cXyz*);
void soCut_setCirclePath(SoCutCirclePath*);
void soCut_addCalcPos(cXyz*, cXyz*, f32, f32);
BOOL soCut_chasePosXZ(cXyz*, cXyz*, f32);
void soCut_matrixYrot(u32, s16);
void soCut_matrixMultVec(u32, cXyz*, cXyz*);
void soCut_vectorAdd(cXyz*, cXyz*, cXyz*);
void soCut_sphereRadius(cM3dGSph*, f32);
void soCut_sphereCenter(cM3dGSph*, cXyz*);
bool soCut_checkTargetHit(daNpc_So_c*);
s32 soCut_distanceAngle(s16, s16);
void soCut_shipInitStartPos(fopAc_ac_c*, cXyz*, s16);
void soCut_soundMiniGame(s32);
void soCut_assertion(const char*, s32, const char*);
void soCut_matrixPosition(cXyz*, cXyz*);
void soCut_add(cXyz*, cXyz*, cXyz*);

namespace SoCutConstants {
enum : u32 {
    fpcNm_SHIP_e = 0xA5,
    JA_SE_CM_SO_JUMP_L = 0x5938,
    JA_SE_CM_SO_LANDING_L = 0x5939,
    JA_SE_CM_SO_RISE = 0x593A,
    JA_SE_CM_SO_DIVE = 0x593B,
    ID_IT_SN_PF_BIKON00 = 0x8152,
    JA_SE_CM_CMN_NOTICE = 0x58BD,
    JA_SE_CM_SO_JUMP_DRAW = 0x593C,
    JA_SE_START_WHISTLE = 0x8F0,
    JA_SE_END_WHISTLE = 0x8F1,
    DEMO_BOW_MINIGAME_e = 0x44,
    DEMO_N_TALK_e = 6,
};
}

void* soCut_searchEsa_CB(void*, void*);
void* soCut_fpcSch_JudgeForPName(void*, void*);
static gabi::HostFn soCutSearchEsaBinding((const void*)&soCut_searchEsa_CB, 0x022DDCA0);
static gabi::HostFn soCutProcessNameBinding((const void*)&soCut_fpcSch_JudgeForPName, 0x025E121C);

typedef be<s16> SoCutProcessName;
