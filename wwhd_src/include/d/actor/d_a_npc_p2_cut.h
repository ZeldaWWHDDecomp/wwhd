// Pirate rope-course cuts. Non-owning WWHD views; shared pirate layout is unchanged.
#pragma once
#include "d/actor/d_a_npc_p2.h"
namespace P2Cut {
enum Cut {
    CUT_TALK = 0,
    CUT_RIDE_SWITCH = 1,
    CUT_RUN_WAIT = 2,
    CUT_JUMP_TO_LIFT = 3,
    CUT_LIFT_TO_ROPE = 4,
    CUT_ROPE_TALK = 5,
    CUT_ROPE_TO_LIFT = 6,
    CUT_JUMP_TO_GOAL = 7,
    CUT_SET_ANM = 8,
    CUT_JUMP = 9,
    CUT_SW_ON = 10,
    CUT_SW_OFF = 11,
    CUT_SURPRISE = 12,
    CUT_OMAMORI_INIT = 13,
    CUT_OMAMORI_END = 14,
};
enum Effect {
    JA_SE_CV_NK_JUMP_UP = 0x4898,
    JA_SE_CV_NK_GRAB = 0x4899,
    JA_SE_CV_NK_JUMP_DOWN = 0x489A,
    JA_SE_CV_NK_LANDING = 0x489B,
    JA_SE_LK_ROPE_SWING_F = 0x201E,
    JA_SE_ITM_OMAMORI_BLINK = 0x852,
    JA_SE_ITM_OMAMORI_TETLA = 0x854,
    JA_SE_ITM_OMAMORI_ED_TALK = 0x855,
    ID_AK_JT_ELEMENTSMOKE00 = 0x2022,
};
// HD: process IDs used by the verified rope-course actor searches.
enum CourseActor { PROC_COURSE_SWITCH = 0x1B, PROC_COURSE_LIFT = 0x2E, PROC_COURSE_ROPE = 0x1BF };
enum Animation {
    ANM_WAIT = 1,
    ANM_JUMP_PREPARE = 5,
    ANM_RUN_WAIT = 6,
    ANM_SURPRISE = 7,
    ANM_JUMP_UP = 8,
    ANM_JUMP_DOWN = 9,
    ANM_LAND = 10,
    ANM_ROPE_BACK = 11,
    ANM_ROPE_FRONT = 12,
    ANM_KYORO = 17,
    ANM_THINK = 18,
    ANM_NOD = 19,
};
// Guest callback supplied by the process scheduler, outside this unit.
constexpr u32 JudgeByName = 0x025E121C;
struct RopeState { u8 mPadding[0xB0]; be<u32> mHangState; };
WWHD_OFFSET(RopeState, mHangState, 0xB0);
WWHD_SIZE(RopeState, 0xB4);
struct Rope : fopAc_ac_c {
    u8 mPadding[0x1914 - sizeof(fopAc_ac_c)];
    be<f32> mMaxLength;
};
WWHD_OFFSET(Rope, mMaxLength, 0x1914);
WWHD_SIZE(Rope, 0x1918);
struct PlayerVtable { u8 mPadding[0xE4]; gfn changeDemoMode; };
WWHD_OFFSET(PlayerVtable, changeDemoMode, 0xE4);
WWHD_SIZE(PlayerVtable, 0xE8);
struct Player : fopAc_ac_c {
    void changeDemoMode(s32 mode) { gabi::call_ptr(gabi::at<PlayerVtable>(__vtbl)->changeDemoMode.get(), this, mode); }
};
WWHD_SIZE(Player, 0x3AC);
struct SinCos { be<f32> sin, cos; };
WWHD_SIZE(SinCos, 8);
struct SinCosTable { SinCos entries[8192]; };
WWHD_SIZE(SinCosTable, 0x10000);
struct CutNames { gptr<char> names[15]; };
WWHD_SIZE(CutNames, 0x3C);
struct SmokeEmitter {
    u8 mPadding[0x34]; be<f32> mRate;
    u8 mPad38[0x58-0x38]; be<f32> mSpread;
    u8 mPad5C[0x68-0x5C]; be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir;
    u8 mPad74[0x220-0x74]; cXyz mGlobalDynamicsScale;
    u8 mPad22C[0x238-0x22C]; cXyz mGlobalParticleScale;
};
WWHD_OFFSET(SmokeEmitter, mRate, 0x34);
WWHD_OFFSET(SmokeEmitter, mSpread, 0x58);
WWHD_OFFSET(SmokeEmitter, mInitialVelOmni, 0x68);
WWHD_OFFSET(SmokeEmitter, mInitialVelAxis, 0x6C);
WWHD_OFFSET(SmokeEmitter, mInitialVelDir, 0x70);
WWHD_OFFSET(SmokeEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(SmokeEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(SmokeEmitter, 0x244);
struct SmokeCallback { be<u32> mVtable; gptr<SmokeEmitter> mpEmitter; u8 mPadding[0x18]; };
WWHD_OFFSET(SmokeCallback, mpEmitter, 4);
WWHD_SIZE(SmokeCallback, 0x20);
}
s32 p2Cut_calcTimer(be<s32>* timer);
u32 p2Cut_seStart(u32 sound);
f32 p2Cut_atan2f(f32 y, f32 x);
void p2Cut_normalize(const cXyz* vector, cXyz* out);
void p2Cut_getPosFromMtx(u32 matrix, cXyz* out);
void p2Cut_ZrotS(Mtx34* matrix, s32 angle);
BOOL p2Cut_chaseS(be<s16>* value, s16 target, s16 step);
void p2Cut_seStartPos(u32 sound, cXyz* pos);
s32 p2Cut_getMyActIdx(void* manager, s32 staffId, P2Cut::CutNames* names, s32 count, s32 force, s32 unused);

s16 p2Cut_rad2s(f32 radians);


struct P2CutMatrixBuffer { u8 mPadding[4]; be<u16> mFlags; u8 mPad6[0xA]; gptr<Mtx34> mpMatrices; };
WWHD_OFFSET(P2CutMatrixBuffer, mFlags, 4);
WWHD_OFFSET(P2CutMatrixBuffer, mpMatrices, 0x10);
WWHD_SIZE(P2CutMatrixBuffer, 0x14);
struct P2CutModel { u8 mPadding[0x2C]; gptr<P2CutMatrixBuffer> mpMatrixBuffer; };
WWHD_OFFSET(P2CutModel, mpMatrixBuffer, 0x2C);
WWHD_SIZE(P2CutModel, 0x30);
inline Mtx34* p2Cut_getAnmMtx(J3DModel* model, s32 joint) {
    auto* buffer = reinterpret_cast<P2CutModel*>(model)->mpMatrixBuffer.get();
    buffer->mFlags |= 0x10;
    return &buffer->mpMatrices[joint];
}
void p2Cut_monsSeStartCall(u32 sound, cXyz* position, u32 processId, u32 param, s32 reverb);
inline void p2Cut_monsSeStart(fopAc_ac_c* actor, u32 sound) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    p2Cut_monsSeStartCall(sound, &actor->eyePos, actor->mBsPcId, 0, reverb);
}
