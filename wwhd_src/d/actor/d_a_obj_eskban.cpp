/** Bomb-breakable boulder blocking Medli's path. WWHD behaviour verified against the original;
 * GameCube names and structure from zeldaret/tww. */
#include "d/actor/d_a_obj_eskban.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10027EF0,
    .headerStatics = {0x104697F0, 0x101C916C, 0x104697FC},
};
using daObjEskban::Act_c;

static const char M_arcname[] = "Eskban";
static const char M_evname[] = "Eskban";
static Mtx34 M_tmp_mtx;
static cXyz offset_vec;
static cXyz d_scale;
static cXyz p_scale;
static u32 offsetGuard;
static u32 dynamicsGuard;
static u32 particleGuard;

/* Collision flags match tww: bomb target, other-object contact, no collision correction. */
static const eskban_layout::CylinderSource cyl_check_src = {
    {0, 0, 0, 0, 0x20, 0x9, 0, 0, 0, 0, 0, 0},
    {0.0f, 0.0f, 0.0f}, 200.0f, 600.0f,
};
static const eskban_layout::CylinderSource cyl_camera_src = {
    {0, 0, 0, 0, 0, 0, 0x149, 0, 0, 0, 0, 0},
    {0.0f, 0.0f, 0.0f}, 1000.0f, 100.0f,
};
static const eskban_layout::SphereSource sph_check_src = {
    {0, 0, 0, 0, 0x20, 0x9, 0x139, 0, 0, 0, 0, 0},
    {0.0f, 0.0f, 0.0f}, 300.0f,
};

/* 0233C60C */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_ESKBAN_BDL_ESKBAN_e);
    if (model_data == nullptr) /* JUT_ASSERT(261, model_data != NULL) */
        JUTAssertion::showAssert("d_a_obj_eskban.cpp", 261, "model_data != 0");
    mpModel = (eskban_layout::Model*)mDoExt_J3DModel__create(model_data, 0, 0x11020203U);
    M_smoke = newSmokeCallback(nullptr, 1);
    if (M_smoke == nullptr) /* JUT_ASSERT(264, M_smoke != NULL) */
        JUTAssertion::showAssert("d_a_obj_eskban.cpp", 264, "M_smoke != 0");
    return mpModel != nullptr;
}

/* 0233C7EC */
BOOL Act_c::Create() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -500.0f, -1.0f, -500.0f, 500.0f, 500.0f, 500.0f);
    mCheckStts.Init(0xFF, 0xFF, this);
    mCheckCyl.Set((const dCcD_SrcCyl*)&cyl_check_src);
    mCheckCyl.SetC(&current.pos);
    mCheckCyl.SetStts(&mCheckStts);

    mCameraStts.Init(0xFF, 0xFF, this);
    mCameraCyl.Set((const dCcD_SrcCyl*)&cyl_camera_src);
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> center;
    offset->set(300.0f, 450.0f, 300.0f);
    cXyz_pl(&current.pos, center, offset);
    mCameraCyl.SetC(center);
    mCameraCyl.SetStts(&mCameraStts);

    mCheckSphStts.Init(0xFF, 0xFF, this);
    mCheckSph.Set((const dCcD_SrcSph*)&sph_check_src);
    offset->set(0.0f, 400.0f, 0.0f);
    cXyz_pl(&current.pos, center, offset);
    mCheckSph.SetC(center);
    mCheckSph.SetStts(&mCheckSphStts);

    mActorState = ST_WAIT;
    mRemainingSmokeAnm = 0;
    mIsVisible = 1;
    return TRUE;
}

/* 0233C3D4 */
cPhs_State Act_c::Mthd_Create() {
    cPhs_State phase_state;
    /* fopAcM_ct(this, Act_c): base constructor, vtable, inline collider constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = __vt;
            dCcD_Stts_ct(&mCheckStts);
            dCcD_Cyl_ct(&mCheckCyl, __aab);
            dCcD_Stts_ct(&mCameraStts);
            dCcD_Cyl_ct(&mCameraCyl, __aab);
            dCcD_Stts_ct(&mCheckSphStts);
            constructSphere(&mCheckSph);
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
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) // HD: this assert reports line 335.
            JUTAssertion::showAssert("d_a_obj_eskban.cpp", 335, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase_state;
}

/* 0233D4D0 */
BOOL Act_c::Delete() {
    if (M_smoke) {
        M_smoke->remove();
        M_smoke = nullptr;
    }
    return TRUE;
}

/* 0233C5B4 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    if ((u8)mCreateResult != cPhs_STOP_e) { /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, M_arcname);                 /* dComIfG_resDeleteDemo */
    }
    return result;
}

/* 0233C6E0 */
void Act_c::set_mtx() {
    s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
    shape_angle.x = ax;
    shape_angle.y = ay;
    shape_angle.z = az;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0233C7CC */
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 0233C970 */
void Act_c::eff_m_break(u16 particleID, u16 prm_b) {
    J3DModelData* mdlData = (J3DModelData*)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_MPI_KOISHI_e);
    void* txPattern = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTP_MPI_KOISHI_e);

    gabi::Local<cXyz> scl;
    scl->set(3.0f, 3.0f, 3.0f);
    eskban_layout::Emitter* pBEmtr = (eskban_layout::Emitter*)dComIfGp_particle_set(particleID, &current.pos, &shape_angle, nullptr, 0xFF, nullptr, -1,
                                                   nullptr, nullptr, scl);
    if (!pBEmtr) {
        return;
    }
    pBEmtr->setGlobalRTMatrix(mpModel->getBaseTRMtx());
    // HD: the model-emitter constructor has an extra flag before tevStr.
    void* pMdlEmtr = newModelEmitter(nullptr, pBEmtr, mdlData, 1, &tevStr, txPattern, prm_b, 0);
    if (!pMdlEmtr) {
        return;
    }
    dComIfGp_particle_addModelEmitter(pMdlEmtr);
}

/* 0233CAA4 */
void Act_c::eff_b_break(u16 particleID) {
    gabi::Local<GXColor> c0;
    gabi::Local<cXyz> scl;
    c0->r = (u8)tevStr.mColorC0.r;
    c0->g = (u8)tevStr.mColorC0.g;
    c0->b = (u8)tevStr.mColorC0.b;
    c0->a = (u8)tevStr.mColorC0.a;
    scl->set(1.0f, 1.0f, 1.0f);
    eskban_layout::Emitter* pBEmtr = (eskban_layout::Emitter*)dComIfGp_particle_set(particleID, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                                   &tevStr.mColorK0, c0, scl);
    if (!pBEmtr) {
        return;
    }
    pBEmtr->setGlobalRTMatrix(mpModel->getBaseTRMtx());
}

/* 0233CB78 */
void Act_c::daObjEskban_effect_set() {
    eff_m_break(ID_AK_SN_BREAKRDMROCK00, 2);
    eff_b_break(ID_AK_SN_M_BREAKRDMROCK00);

    // HD: GHS sets each static guard before storing the vector components.
    if (offsetGuard == 0) {
        offsetGuard = 1;
        offset_vec.set(0.0f, 250.0f, 0.0f);
    }
    PSMTXCopy(mpModel->getBaseTRMtx(), mDoMtx_stack_c::get());
    PSMTXMultVec(mDoMtx_stack_c::get(), &offset_vec, &mSmokePos);
    eskban_layout::SmokeCallback* smoke = M_smoke;
    if (!smoke) {
        return;
    }
    /* dComIfGp_particle_setToon (group 2) */
    eskban_layout::Emitter* pBEmtr = (eskban_layout::Emitter*)dPa_control_set(dComIfGp_getParticle(), 2, ID_AK_JT_ELEMENTSMOKE01, &mSmokePos,
                                             nullptr, nullptr, 0xC8, (dPa_levelEcallBack*)smoke, -1, nullptr, nullptr, nullptr);
    if (!pBEmtr) {
        return;
    }
    // HD: max frame is stored before rate.
    pBEmtr->setMaxFrame(1);
    pBEmtr->setRate(30.0f);
    if (dynamicsGuard == 0) {
        dynamicsGuard = 1;
        d_scale.set(3.0f, 3.0f, 3.0f);
    }
    if (particleGuard == 0) {
        particleGuard = 1;
        p_scale.set(7.0f, 7.0f, 7.0f);
    }
    pBEmtr->mGlobalDynamicsScale.x = d_scale.x;
    pBEmtr->mGlobalDynamicsScale.y = d_scale.y;
    pBEmtr->mGlobalDynamicsScale.z = d_scale.z;
    pBEmtr->mGlobalParticleScale.x = p_scale.x;
    pBEmtr->mGlobalParticleScale.y = p_scale.y;
    pBEmtr->mGlobalParticleScale.z = p_scale.z;
}

/* 0233CD6C */
BOOL Act_c::Execute(Mtx34** pMtx) {
    dComIfG_Ccsp_Set(&mCheckCyl);
    dComIfG_Ccsp_Set(&mCameraCyl);
    dComIfG_Ccsp_Set(&mCheckSph);
    if (mCheckSph.ChkCoHit()) {
        dCcD_GObjInf* hitObj = getCoHitObj(&mCheckSph);
        if (hitObj) {
            dCcD_Stts* stts = hitObj->mStts;
            fopAc_ac_c* hitAct = stts ? stts->GetActor() : nullptr;
            /* HD: fopAcM_GetName checks the actor for NULL */
            if (hitAct && fpcM_GetName(hitAct) == fpcNm_NPC_MD_e) {
                gabi::Local<cXyz> dist;
                cXyz_mi(&hitAct->current.pos, dist, &current.pos);
                dist->y = 0.0f;
                if (normalizeRS(dist)) {
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
        dCcD_GObjInf* hitObj = getCoHitObj(&mCameraCyl);
        if (!hitObj) {
            break;
        }
        dCcD_Stts* stts = hitObj->mStts;
        fopAc_ac_c* hitAct = stts ? stts->GetActor() : nullptr;
        if (hitAct && fpcM_GetName(hitAct) == fpcNm_Bomb2_e) {
            mActorID = fopAcM_GetID(hitAct);
            fopAcM_orderOtherEvent(this, M_evname);
            mActorState = ST_DESTROYED;
        }
        break;
    }
    case ST_DESTROYED:
        if (eventInfo.checkCommandDemoAccrpt()) {
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
        // HD: snapshot the room before visibility/timer stores and the sound callbacks.
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
        startQuake(dComIfGp_getVibration(), 7, ~0x20, up);
        startRiddleSound(JA_SE_READ_RIDDLE_1);
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
                dComIfGp_getVibration()->StartShock(6, 0x4, up);
            }
        } else if (!fopAcM_isSwitch(this, param_get_swSave())) {
            s32 sw = param_get_swSave();
            dComIfGs_onSwitch(sw, home.roomNo);
            stopQuake(dComIfGp_getVibration(), -1);
            gabi::Local<cXyz> up;
            up->set(0.0f, 1.0f, 0.0f);
            startQuake(dComIfGp_getVibration(), 4, ~0x20, up);
        }
        if (mRemainingSmokeAnm > 0) {
            mRemainingSmokeAnm = mRemainingSmokeAnm - 1;
            break;
        }
        if (eventEndCheckOld(dComIfGp_getPEvtManager(), M_evname) && M_smoke->isEnd()) {
            stopQuake(dComIfGp_getVibration(), -1);
            fopAcM_delete(this);
            dComIfGp_event_reset();
        }
        break;
    }
    set_mtx();
    // HD: the output is a four-byte guest pointer, even in the host harness.
    *reinterpret_cast<be<u32>*>(pMtx) = gabi::ea(&M_tmp_mtx);
    return TRUE;
}

/* 0233D42C */
BOOL Act_c::Draw() {
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

/* method table entries (tail branches) */
/* 0233D520 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 0233D524 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0233D528 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 0233D52C */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 0233D53C */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0233D54C */
static void __sinit_d_a_obj_eskban_cpp() {
    __sinit_header_statics();
}

/* 0233D5E0 */
static void SafeString_dtor(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0233D5F4 */
static BOOL MoveBgActor_IsDelete(void* i_this) {
    return TRUE;
}

/* 0233D5FC */
static void SafeString_assureTerminationImpl(void* p) {
}

/* 0233D600 */
static void Act_c_dtor(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        destroySphere(&i_this->mCheckSph, 2);
        dCcD_Stts_dt(&i_this->mCheckSphStts, 2);
        dCcD_Cyl_dt(&i_this->mCameraCyl, 2);
        dCcD_Stts_dt(&i_this->mCameraStts, 2);
        dCcD_Cyl_dt(&i_this->mCheckCyl, 2);
        dCcD_Stts_dt(&i_this->mCheckStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0233D69C */
static u32 PrmAbstract(fopAc_ac_c* a, u32 width, u32 shift) {
    u32 param = fopAcM_GetParam(a);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? param >> shift : 0u) & mask;
}
s32 Act_c::param_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* WWHD glue
str 10027FF0 "Eskban"
str 10027FDC "Eskban" in 0233CD6C
str 10027FC4 "Always"
str 10027F80 "d_a_obj_eskban.cpp"
str 10027F18 "d_a_obj_eskban.cpp" in 0233C3D4
str 10027F70 "model_data != 0"
str 10027F94 "M_smoke != 0"
str 10027F2C "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046983C
data offset_vec 1046980C
data d_scale 10469818
data p_scale 10469824
data offsetGuard 10469830
data dynamicsGuard 10469834
data particleGuard 10469838
data cyl_check_src 101C90E4
data cyl_camera_src 101C9128
data sph_check_src 101C90A4
ext 025A5B18 eskban_layout::SmokeCallback* newSmokeCallback(void*, u8)
ext 025A3BDC void* newModelEmitter(void*, JPABaseEmitter*, J3DModelData*, u32, dKy_tevstr_c*, void*, u16, s32)
ext 025163BC dCcD_GObjInf* getCoHitObj(dCcD_GObjInf*)
ext 025D77DC void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16)
ext 025CB408 void startQuake(dVibration_c*, s32, s32, cXyz*)
ext 025CB610 void stopQuake(dVibration_c*, s32)
ext 0254457C BOOL eventEndCheckOld(dEvent_manager_c*, const char*)
ext 025E1988 void startRiddleSound(u32)
ext 0201B47C BOOL normalizeRS(cXyz*)
ext 025166F0 void constructSphere(dCcD_Sph*)
ext 02515AE8 void destroySphere(dCcD_Sph*, s32)
*/
