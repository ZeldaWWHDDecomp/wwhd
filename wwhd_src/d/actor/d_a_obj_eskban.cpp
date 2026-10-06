/**
 * d_a_obj_eskban.cpp (WWHD)
 * Object - Bomb-breakable boulder blocking Medli's path (Earth Temple).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_eskban.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10027FF0) /* "Eskban" */
#define M_evname STR(0x10027FDC)  /* "Eskban" */
#define ALWAYS_ARC STR(0x10027FC4) /* "Always" */
#define SAFESTRING_VTBL 0x10027EF0
#define ACT_VTBL 0x10027FF8
#define AAB_VTBL 0x10027F08          /* cM3dGAab (per TU) */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046983C)
#define cyl_check_src 0x101C90E4
#define cyl_camera_src 0x101C9128
#define sph_check_src 0x101C90A4

enum { dRes_INDEX_ESKBAN_BDL_ESKBAN_e = 4, dRes_INDEX_ESKBAN_DZB_ESKBAN_e = 7 };
enum { dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e = 0x30, dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e = 0x66 };
enum { cPhs_STOP_e = 3 };
enum { fpcNm_NPC_MD_e = 0x16F, fpcNm_Bomb2_e = 0x127 };
enum { JA_SE_LK_W_WEP_CRT_HIT = 0x2835, JA_SE_READ_RIDDLE_1 = 0x806 };
enum { DESTROY_VIBRATION_LEN = 35, DESTROY_VIBRATION_SHOCK_FRAME_IDX = 28, DESTROY_SMOKE_ANM_LEN = 20 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) (allocates when this == NULL) */
static inline dPa_smokeEcallBack* new_dPa_smokeEcallBack(u8 a) { return gabi::call<dPa_smokeEcallBack*>(0x025A5B18, (u32)0, a); }
/* 025A3BDC dPa_J3DmodelEmitter_c::dPa_J3DmodelEmitter_c (allocates when this == NULL; HD: an extra
 * argument before the tevstr, 1 here) */
static inline void* new_dPa_J3DmodelEmitter(JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i) {
    return gabi::call<void*>(0x025A3BDC, (u32)0, e, d, hd, tev, btp, n, i);
}
/* 0200FE78 cLs_Addition(list, node): dComIfGp_particle_addModelEmitter (the list at particle+0x130) */
static inline void dComIfGp_particle_addModelEmitter(void* e) {
    u32 pa = gabi::ea(dComIfGp_getParticle());
    gabi::call(0x0200FE78, gabi::load<u32>(pa + 0x130), e);
}
/* 028249B0 JPASetRMtxTVecfromMtx(mtx, R, T): JPABaseEmitter::setGlobalRTMatrix (R at +0x1F0, T at +0x22C) */
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
/* 025163BC dCcD_GObjInf::GetCoHitObj */
static inline void* GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x025163BC, o); }
/* cCcD_Obj::GetAc (inline): the hit object's stts (+0x44) actor */
static inline fopAc_ac_c* cCcD_Obj_GetAc(void* obj) {
    u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
    return stts == 0 ? nullptr : gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC));
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline void fopAcM_orderOtherEvent(fopAc_ac_c* a, const char* name, u16 flag = 1, u16 hind = 0xFFFF) {
    gabi::call(0x025D77DC, a, name, flag, hind);
}
/* 025CB408 dVibration_c::StartQuake(int, int, cXyz) / 025CB610 StopQuake(int) */
static inline void dComIfGp_getVibration_StartQuake(s32 strength, s32 flags, cXyz* pos) {
    gabi::call(0x025CB408, dComIfGp_getVibration(), strength, flags, pos);
}
static inline void dComIfGp_getVibration_StopQuake(s32 flags) { gabi::call(0x025CB610, dComIfGp_getVibration(), flags); }
/* 0254457C dEvent_manager_c::endCheckOld(name) */
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* 025E1988 mDoAud_seStart(id) (HD: one argument) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
static inline BOOL cXyz_normalizeRS(cXyz* a) { return gabi::call<BOOL>(0x0201B47C, a); }

namespace daObjEskban {

struct Act_c : dBgS_MoveBgActor {
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

    /* 0x3E0 */ gptr<dPa_smokeEcallBack> M_smoke;
    /* 0x3E4 */ cXyz mSmokePos;
    /* 0x3F0 */ request_of_phase_process_class mPhs;
    /* 0x3F8 */ gptr<J3DModel> mpModel;
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

/* 0233D69C: daObj::PrmAbstract<Act_c::Prm_e> (per-TU copy) */
static u32 PrmAbstract(fopAc_ac_c* a, u32 width, u32 shift) {
    WWHD_FUNC(0x0233D69C, u32, a, width, shift);
    u32 param = fopAcM_GetParam(a);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? param >> shift : 0u) & mask;
}
VERIFY(0x0233D69C, PrmAbstract);
s32 Act_c::param_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0233C60C */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0233C60C, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ESKBAN_BDL_ESKBAN_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(261, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x10027F80), 0x105, STR(0x10027F70));
    mpModel = mDoExt_J3DModel__create(model_data, 0, 0x11020203U);
    M_smoke = new_dPa_smokeEcallBack(1);
    if (M_smoke == nullptr) /* JUT_ASSERT(264, M_smoke != NULL) */
        JUT_ASSERT_fail(STR(0x10027F80), 0x108, STR(0x10027F94));
    return mpModel != nullptr;
}
VERIFY(0x0233C60C, &Act_c::CreateHeap);

/* 0233C7EC */
BOOL Act_c::Create() {
    WWHD_FUNC(0x0233C7EC, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -1.0f, -500.0f, 500.0f, 500.0f, 500.0f);
    mCheckStts.Init(0xFF, 0xFF, this);
    mCheckCyl.Set((const dCcD_SrcCyl*)gabi::at<u8>(cyl_check_src));
    mCheckCyl.SetC(&current.pos);
    mCheckCyl.SetStts(&mCheckStts);

    mCameraStts.Init(0xFF, 0xFF, this);
    mCameraCyl.Set((const dCcD_SrcCyl*)gabi::at<u8>(cyl_camera_src));
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> center;
    offset->set(300.0f, 450.0f, 300.0f);
    cXyz_pl(&current.pos, center, offset);
    mCameraCyl.SetC(center);
    mCameraCyl.SetStts(&mCameraStts);

    mCheckSphStts.Init(0xFF, 0xFF, this);
    mCheckSph.Set((const dCcD_SrcSph*)gabi::at<u8>(sph_check_src));
    offset->set(0.0f, 400.0f, 0.0f);
    cXyz_pl(&current.pos, center, offset);
    mCheckSph.SetC(center);
    mCheckSph.SetStts(&mCheckSphStts);

    mActorState = 0;
    mRemainingSmokeAnm = 0;
    mIsVisible = 1;
    return TRUE;
}
VERIFY(0x0233C7EC, &Act_c::Create);

/* 0233C3D4 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0233C3D4, cPhs_State, this);
    cPhs_State phase_state;
    /* fopAcM_ct(this, Act_c): base constructor, vtable, inline collider constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mCheckStts);
            dCcD_Cyl_ct(&mCheckCyl, AAB_VTBL);
            dCcD_Stts_ct(&mCameraStts);
            dCcD_Cyl_ct(&mCameraCyl, AAB_VTBL);
            dCcD_Stts_ct(&mCheckSphStts);
            gabi::call(0x025166F0, &mCheckSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    M_smoke = nullptr;

    s32 swSave = param_get_swSave();
    if (fopAcM_isSwitch(this, swSave)) {
        return cPhs_STOP_e;
    }
    phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_ESKBAN_DZB_ESKBAN_e, 0, 0x1020);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(336, ...) */
            JUT_ASSERT_fail(STR(0x10027F18), 0x14F, STR(0x10027F2C));
    }
    return phase_state;
}
VERIFY(0x0233C3D4, &Act_c::Mthd_Create);

/* 0233D4D0 */
BOOL Act_c::Delete() {
    WWHD_FUNC(0x0233D4D0, BOOL, this);
    if (M_smoke) {
        /* M_smoke->remove() (virtual, slot +0x44) */
        u32 s = gabi::ea(M_smoke.get());
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(s) + 0x44), M_smoke.get());
        M_smoke = nullptr;
    }
    return TRUE;
}
VERIFY(0x0233D4D0, &Act_c::Delete);

/* 0233C5B4 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x0233C5B4, BOOL, this);
    BOOL result = MoveBGDelete();
    if (gabi::load<u8>(gabi::ea(this) + 0xD) != cPhs_STOP_e) { /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, M_arcname);                 /* dComIfG_resDeleteDemo */
    }
    return result;
}
VERIFY(0x0233C5B4, &Act_c::Mthd_Delete);

/* 0233C6E0 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x0233C6E0, void, this);
    s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x0233C6E0, &Act_c::set_mtx);

/* 0233C7CC */
void Act_c::init_mtx() {
    WWHD_FUNC(0x0233C7CC, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x0233C7CC, &Act_c::init_mtx);

/* 0233C970 */
void Act_c::eff_m_break(u16 particleID, u16 prm_b) {
    WWHD_FUNC(0x0233C970, void, this, particleID, prm_b);
    J3DModelData* mdlData = (J3DModelData*)dComIfG_getObjectRes(ALWAYS_ARC, dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e, SAFESTRING_VTBL);
    void* txPattern = dComIfG_getObjectRes(ALWAYS_ARC, dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e, SAFESTRING_VTBL);

    gabi::Local<cXyz> scl;
    scl->set(3.0f, 3.0f, 3.0f);
    JPABaseEmitter* pBEmtr = dComIfGp_particle_set(particleID, &current.pos, &shape_angle, nullptr, 0xFF, nullptr, -1,
                                                   nullptr, nullptr, scl);
    if (!pBEmtr) {
        return;
    }
    JPABaseEmitter_setGlobalRTMatrix(pBEmtr, J3DModel_getBaseTRMtx(mpModel));
    void* pMdlEmtr = new_dPa_J3DmodelEmitter(pBEmtr, mdlData, 1, &tevStr, txPattern, prm_b, 0);
    if (!pMdlEmtr) {
        return;
    }
    dComIfGp_particle_addModelEmitter(pMdlEmtr);
}
VERIFY(0x0233C970, &Act_c::eff_m_break);

/* 0233CAA4 */
void Act_c::eff_b_break(u16 particleID) {
    WWHD_FUNC(0x0233CAA4, void, this, particleID);
    /* tevStr.mColorC0 (GXColorS10 at tevStr+0x90), mColorK0 at tevStr+0x98 */
    u32 tev = gabi::ea(&tevStr);
    gabi::Local<GXColor> c0;
    gabi::Local<cXyz> scl;
    u8* c = (u8*)c0.get();
    gabi::store<u8>(gabi::ea(c) + 0, (u8)gabi::load<s16>(tev + 0x90));
    gabi::store<u8>(gabi::ea(c) + 1, (u8)gabi::load<s16>(tev + 0x92));
    gabi::store<u8>(gabi::ea(c) + 2, (u8)gabi::load<s16>(tev + 0x94));
    gabi::store<u8>(gabi::ea(c) + 3, (u8)gabi::load<s16>(tev + 0x96));
    scl->set(1.0f, 1.0f, 1.0f);
    JPABaseEmitter* pBEmtr = dComIfGp_particle_set(particleID, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                                   gabi::at<GXColor>(tev + 0x98), c0, scl);
    if (!pBEmtr) {
        return;
    }
    JPABaseEmitter_setGlobalRTMatrix(pBEmtr, J3DModel_getBaseTRMtx(mpModel));
}
VERIFY(0x0233CAA4, &Act_c::eff_b_break);

/* 0233CB78 */
void Act_c::daObjEskban_effect_set() {
    WWHD_FUNC(0x0233CB78, void, this);
    eff_m_break(0x82B1 /* ID_AK_SN_BREAKRDMROCK00 */, 2);
    eff_b_break(0x82B2 /* ID_AK_SN_M_BREAKRDMROCK00 */);

    /* static cXyz offset_vec(0, 250, 0) */
    be<u32>& guard = *gabi::at<be<u32>>(0x10469830);
    cXyz* offset_vec = gabi::at<cXyz>(0x1046980C);
    if (guard == 0) {
        guard = 1;
        offset_vec->set(0.0f, 250.0f, 0.0f);
    }
    PSMTXCopy(J3DModel_getBaseTRMtx(mpModel), mDoMtx_stack_c::get());
    PSMTXMultVec(mDoMtx_stack_c::get(), offset_vec, &mSmokePos);
    dPa_smokeEcallBack* smoke = M_smoke;
    if (!smoke) {
        return;
    }
    /* dComIfGp_particle_setToon (group 2) */
    JPABaseEmitter* pBEmtr = dPa_control_set(dComIfGp_getParticle(), 2, 0x2027 /* ID_AK_JT_ELEMENTSMOKE01 */, &mSmokePos,
                                             nullptr, nullptr, 0xC8, (dPa_levelEcallBack*)smoke, -1, nullptr, nullptr, nullptr);
    if (!pBEmtr) {
        return;
    }
    u32 e = gabi::ea(pBEmtr);
    gabi::store<u32>(e + 0x5C, 1);      /* setMaxFrame(1) */
    gabi::store<f32>(e + 0x34, 30.0f);  /* setRate(30) */
    /* static JGeometry::TVec3<f32> d_scale(3, 3, 3), p_scale(7, 7, 7) */
    be<u32>& dguard = *gabi::at<be<u32>>(0x10469834);
    cXyz* d_scale = gabi::at<cXyz>(0x10469818);
    if (dguard == 0) {
        dguard = 1;
        d_scale->set(3.0f, 3.0f, 3.0f);
    }
    be<u32>& pguard = *gabi::at<be<u32>>(0x10469838);
    cXyz* p_scale = gabi::at<cXyz>(0x10469824);
    if (pguard == 0) {
        pguard = 1;
        p_scale->set(7.0f, 7.0f, 7.0f);
    }
    /* setGlobalDynamicsScale / setGlobalParticleScale */
    gabi::store<f32>(e + 0x220, d_scale->x);
    gabi::store<f32>(e + 0x224, d_scale->y);
    gabi::store<f32>(e + 0x228, d_scale->z);
    gabi::store<f32>(e + 0x238, p_scale->x);
    gabi::store<f32>(e + 0x23C, p_scale->y);
    gabi::store<f32>(e + 0x240, p_scale->z);
}
VERIFY(0x0233CB78, &Act_c::daObjEskban_effect_set);

/* 0233CD6C */
BOOL Act_c::Execute(Mtx34** pMtx) {
    WWHD_FUNC(0x0233CD6C, BOOL, this, pMtx);
    dComIfG_Ccsp_Set(&mCheckCyl);
    dComIfG_Ccsp_Set(&mCameraCyl);
    dComIfG_Ccsp_Set(&mCheckSph);
    if (mCheckSph.ChkCoHit()) {
        void* hitObj = GetCoHitObj(&mCheckSph);
        if (hitObj) {
            fopAc_ac_c* hitAct = cCcD_Obj_GetAc(hitObj);
            /* HD: fopAcM_GetName checks the actor for NULL */
            if (hitAct && hitAct && fpcM_GetName(hitAct) == fpcNm_NPC_MD_e) {
                gabi::Local<cXyz> dist;
                cXyz_mi(&hitAct->current.pos, dist, &current.pos);
                dist->y = 0.0f;
                if (cXyz_normalizeRS(dist)) {
                    PSVECScale(dist, dist, 10.0f);
                } else {
                    dist->set(10.0f, 0.0f, 0.0f);
                }
                PSVECAdd(&hitAct->current.pos, dist, &hitAct->current.pos);
            }
        }
    }
    switch ((u32)(s32)mActorState) {
    case ST_WAIT: {
        if (!mCameraCyl.ChkCoHit()) {
            break;
        }
        void* hitObj = GetCoHitObj(&mCameraCyl);
        if (!hitObj) {
            break;
        }
        fopAc_ac_c* hitAct = cCcD_Obj_GetAc(hitObj);
        if (hitAct && hitAct && fpcM_GetName(hitAct) == fpcNm_Bomb2_e) {
            mActorID = fopAcM_GetID(hitAct);
            fopAcM_orderOtherEvent(this, M_evname);
            mActorState = ST_DESTROYED;
        }
        break;
    }
    case ST_DESTROYED:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            mActorState = ST_CUTSCENING;
            break;
        }
        fopAcM_orderOtherEvent(this, M_evname);
        break;
    case ST_CUTSCENING: {
        if (fopAcM_SearchByID(mActorID) == nullptr) {
            mActorState = ST_SMOKING;
            mRemainingSmokeAnm = DESTROY_SMOKE_ANM_LEN;
        }
        if (!mCheckCyl.ChkTgHit()) {
            break;
        }
        s32 staffID = dComIfGp_evmng_getMyStaffId(M_evname, nullptr, 0);
        dComIfGp_evmng_cutEnd(staffID);
        s8 roomNo = current.roomNo;
        mIsVisible = 0;
        mRemainingSmokeAnm = DESTROY_SMOKE_ANM_LEN;
        mActorState = ST_VIBRATING;
        mRemainingVibration = DESTROY_VIBRATION_LEN;
        /* fopAcM_seStartCurrent(this, JA_SE_LK_W_WEP_CRT_HIT, 0) */
        s32 reverb = dComIfGp_getReverb(roomNo);
        mDoAud_seStart(JA_SE_LK_W_WEP_CRT_HIT, &current.pos, 0, reverb);
        gabi::Local<cXyz> up;
        up->set(0.0f, 1.0f, 0.0f);
        dComIfGp_getVibration_StartQuake(7, ~0x20, up);
        mDoAud_seStart_1(JA_SE_READ_RIDDLE_1);
        daObjEskban_effect_set();
        break;
    }
    case ST_SMOKING:
        if (mRemainingSmokeAnm > 0) {
            mRemainingSmokeAnm = mRemainingSmokeAnm - 1;
            break;
        } else {
            mActorState = ST_WAIT;
            dComIfGp_event_reset();
            break;
        }
    case ST_VIBRATING:
        if (mRemainingVibration > 0) {
            mRemainingVibration = mRemainingVibration - 1;
            if (mRemainingVibration == DESTROY_VIBRATION_SHOCK_FRAME_IDX) {
                gabi::Local<cXyz> up;
                up->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(6, 0x4, up);
            }
        } else if (!fopAcM_isSwitch(this, param_get_swSave())) {
            s32 sw = param_get_swSave();
            dComIfGs_onSwitch(sw, home.roomNo);
            dComIfGp_getVibration_StopQuake(-1);
            gabi::Local<cXyz> up;
            up->set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartQuake(4, ~0x20, up);
        }
        if (mRemainingSmokeAnm > 0) {
            mRemainingSmokeAnm = mRemainingSmokeAnm - 1;
            break;
        }
        if (dComIfGp_evmng_endCheckOld(M_evname) && (gabi::load<u8>(gabi::ea(M_smoke.get()) + 0x10) & 1) /* M_smoke->isEnd() */) {
            dComIfGp_getVibration_StopQuake(-1);
            fopAcM_delete(this);
            dComIfGp_event_reset();
        }
        break;
    }
    set_mtx();
    *gabi::at<be<u32>>(gabi::ea(pMtx)) = gabi::ea(M_tmp_mtx);
    return TRUE;
}
VERIFY(0x0233CD6C, &Act_c::Execute);

/* 0233D42C */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0233D42C, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    if (!mIsVisible) {
        return TRUE;
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0233D42C, &Act_c::Draw);

/* method table entries (tail branches) */
/* 0233D520 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0233D520, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x0233D520, Mthd_Create);
/* 0233D524 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0233D524, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0233D524, Mthd_Delete);
/* 0233D528 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0233D528, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0233D528, Mthd_Execute);
/* 0233D52C: MoveBGDraw (virtual Draw) */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0233D52C, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x0233D52C, Mthd_Draw);
/* 0233D53C: MoveBGIsDelete (virtual IsDelete) */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0233D53C, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x0233D53C, Mthd_IsDelete);

/* 0233D54C */
static void __sinit_d_a_obj_eskban_cpp() {
    WWHD_FUNC(0x0233D54C, void, (u32)0);
    sinit_header_statics(0x104697F0, 0x101C916C);
}
VERIFY(0x0233D54C, __sinit_d_a_obj_eskban_cpp);

/* 0233D5E0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0233D5E0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0233D5E0, trivial_dt);

/* 0233D5F4: dBgS_MoveBgActor::IsDelete (per-TU copy; the matcher files it under d_a_fan) */
static BOOL MoveBgActor_IsDelete(void* i_this) {
    WWHD_FUNC(0x0233D5F4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0233D5F4, MoveBgActor_IsDelete);

/* 0233D5FC: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x0233D5FC, void, p);
}
VERIFY(0x0233D5FC, empty_virtual);

/* 0233D600: Act_c deleting destructor (inline member destructors) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0233D600, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mCheckSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mCheckSphStts, 2);
        dCcD_Cyl_dt(&i_this->mCameraCyl, 2);
        dCcD_Stts_dt(&i_this->mCameraStts, 2);
        dCcD_Cyl_dt(&i_this->mCheckCyl, 2);
        dCcD_Stts_dt(&i_this->mCheckStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0233D600, Act_c_dt);

} // namespace daObjEskban
