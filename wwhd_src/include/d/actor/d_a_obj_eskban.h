/* Medli's bomb-breakable boulder. WWHD retains the GameCube member names; the moving-background
 * actor base grows by 0x118, so the actor tail starts at 0x3E0. Local game-derived code. */
#pragma once
#include "gc_api/gc_api.h"

namespace eskban_layout {
/* HD: retain the GHS load/store order of the model's inline setters. */
struct Model : J3DModel {
    void setBaseScale(const cXyz& scale) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        mBaseScale.x = x; mBaseScale.y = y; mBaseScale.z = z;
    }
    void setBaseTRMtx(const Mtx34* matrix) { mtx_copy(&mBaseTransformMtx, matrix); }
};
struct SmokeCallback : dPa_smokeEcallBack {
    bool isEnd() { return (m10 & 1) != 0; }
};
WWHD_SIZE(Model, 0xF8);
WWHD_SIZE(SmokeCallback, 0x20);
/* The collision source object-info prefix is unchanged; the trailing shape is typed here. */
struct CylinderSource {
    /* 0x00 */ be<u32> mObjInf[12];
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
    /* 0x40 */ be<f32> mHeight;
};
struct SphereSource {
    /* 0x00 */ be<u32> mObjInf[12];
    /* 0x30 */ be<f32> mCenter[3];
    /* 0x3C */ be<f32> mRadius;
};
WWHD_OFFSET(CylinderSource, mCenter, 0x30);
WWHD_OFFSET(CylinderSource, mRadius, 0x3C);
WWHD_SIZE(CylinderSource, 0x44);
WWHD_OFFSET(SphereSource, mCenter, 0x30);
WWHD_OFFSET(SphereSource, mRadius, 0x3C);
WWHD_SIZE(SphereSource, 0x40);
struct Emitter : JPABaseEmitter {
    /* 0x070 */ u8 emitterState[0x1F0 - 0x70];
    /* 0x1F0 */ Mtx34 mGlobalRotation;
    /* 0x220 */ cXyz mGlobalDynamicsScale;
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalParticleScale;
    void setGlobalRTMatrix(const Mtx34* matrix) {
        JPASetRMtxTVecfromMtx(matrix, &mGlobalRotation, &mGlobalTranslation);
    }
};
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(Emitter, 0x244);
}

/* Unit-local bindings preserve the verified HD argument and return widths. */
eskban_layout::SmokeCallback* newSmokeCallback(void*, u8);
void* newModelEmitter(void*, JPABaseEmitter*, J3DModelData*, u32, dKy_tevstr_c*, void*, u16, s32);
dCcD_GObjInf* getCoHitObj(dCcD_GObjInf*);
void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16 = 1, u16 = 0xFFFF);
void startQuake(dVibration_c*, s32, s32, cXyz*);
void stopQuake(dVibration_c*, s32);
BOOL eventEndCheckOld(dEvent_manager_c*, const char*);
void startRiddleSound(u32);
BOOL normalizeRS(cXyz*);
void constructSphere(dCcD_Sph*);
void destroySphere(dCcD_Sph*, s32);

namespace daObjEskban {
struct Act_c : dBgS_MoveBgActor {
    enum : u32 { __vt = 0x10027FF8, __aab = 0x10027F08 };
    enum Prm_e { PRM_SWSAVE_W = 0x08, PRM_SWSAVE_S = 0x00 };
    enum actor_state { ST_WAIT = 0, ST_DESTROYED = 1, ST_CUTSCENING = 2, ST_VIBRATING = 3, ST_SMOKING = 4 };
    s32 param_get_swSave();

    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void eff_m_break(u16, u16);
    void eff_b_break(u16);
    void daObjEskban_effect_set();
    BOOL Execute(Mtx34**);
    BOOL Draw();

    /* 0x3E0 */ gptr<eskban_layout::SmokeCallback> M_smoke;
    /* 0x3E4 */ cXyz mSmokePos;
    /* 0x3F0 */ request_of_phase_process_class mPhs;
    /* 0x3F8 */ gptr<eskban_layout::Model> mpModel;
    /* 0x3FC */ dCcD_Stts mCheckStts;
    /* 0x438 */ dCcD_Cyl mCheckCyl;
    /* 0x568 */ dCcD_Stts mCameraStts;
    /* 0x5A4 */ dCcD_Cyl mCameraCyl;
    /* 0x6D4 */ be<u32> mActorID;
    /* 0x6D8 */ dCcD_Stts mCheckSphStts;
    /* 0x714 */ dCcD_Sph mCheckSph;
    /* 0x840 */ be<s32> mActorState;
    /* 0x844 */ be<s32> mRemainingSmokeAnm;
    /* 0x848 */ be<s32> mRemainingVibration;
    /* 0x84C */ be<u8> mIsVisible;
    /* 0x84D */ u8 _84D[3];
};
WWHD_OFFSET(Act_c, mCheckCyl, 0x438);
WWHD_OFFSET(Act_c, mCheckSph, 0x714);
WWHD_OFFSET(Act_c, mIsVisible, 0x84C);
WWHD_SIZE(Act_c, 0x850);
}
enum {
    dRes_INDEX_ESKBAN_BDL_ESKBAN_e = 4,
    dRes_INDEX_ESKBAN_DZB_ESKBAN_e = 7,
    dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e = 0x30,
    dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e = 0x66,
    fpcNm_NPC_MD_e = 0x16F, fpcNm_Bomb2_e = 0x127,
    JA_SE_LK_W_WEP_CRT_HIT = 0x2835, JA_SE_READ_RIDDLE_1 = 0x806,
    DESTROY_VIBRATION_LEN = 35, DESTROY_VIBRATION_SHOCK_FRAME_IDX = 28,
    DESTROY_SMOKE_ANM_LEN = 20,
    ID_AK_SN_BREAKRDMROCK00 = 0x82B1, ID_AK_SN_M_BREAKRDMROCK00 = 0x82B2,
    ID_AK_JT_ELEMENTSMOKE01 = 0x2027,
};
