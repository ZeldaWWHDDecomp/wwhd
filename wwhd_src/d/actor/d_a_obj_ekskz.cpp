/**
 * d_a_obj_ekskz.cpp (WWHD)
 * Object - Stone statue blowing a strong gust of wind (Gale Isle); breaks when hit by the
 * Skull Hammer.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_obj_ekskz.cpp) has only "Nonmatching" stubs for this unit, so every function
 * here is written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_obj_ekskz.h"
using daObjEkskz::Act_c;

// HD: expanded animation controllers and light objects shift the retail actor fields.
static EkskzText M_arcname;
static EkskzText FILE_NAME;
static EkskzText phaseAssertFile;
static EkskzText phaseAssert;
static EkskzText modelAssert;
static EkskzText effectAssert;
static EkskzText bckAssert;
static EkskzText btkAssert;
static EkskzText brkAssert;
static dCcD_SrcCyl cylinderSource;
static Mtx34 M_tmp_mtx;

/* 0233C3B8 */
static u32 PrmAbstract(fopAc_ac_c *a, s32 width, s32 shift) {
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 daObjEkskz::Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0233B4A0 */
static u32 smokeEcallBack_ct(void *p) { return ekskz_createSmokeCallback(p); }

/* 0233B4A8 */
static Act_c *Act_c_ct(Act_c *self) {
    if (self == nullptr) {
        self = (Act_c *)operator_new(sizeof(Act_c));
        if (self == nullptr)
            return self;
    }
    dBgS_MoveBgActor::ct(self);
    self->__vtbl = ACT_VTBL;
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mCyl, CYL_AAB_VTBL);
    bckAnm_ct(gabi::ea(&self->mBckAnm));
    ekskz_mDoExt_brkAnm_ct(&self->mBrkAnm);
    mDoExt_btkAnm::ct(&self->mBtkAnm);
    ekskz___construct_array(self->mSmokeCallback, 4, 0x20, 0x0233B4A0);
    tevstr_ct(&self->mTevStr);
    return self;
}

/* 0233B74C */
cPhs_State daObjEkskz::Act_c::Mthd_Create() {
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr)
            Act_c_ct(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mpSmoke0 = nullptr;
    m510 = 0;
    m504[1] = 0;
    m504[0] = 0;
    if (fopAcM_isSwitch(this, prm_get_swSave()))
        return cPhs_STOP_e; /* cPhs_STOP_e: already broken */

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname.bytes);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname.bytes, dRes_INDEX_EKSKZ_DZB_e, 0, 0x28A0);
        if (!((phase_state == cPhs_COMPLEATE_e) ||
              (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x13D, ...) */
            JUT_ASSERT_fail(phaseAssertFile.bytes, 0x13D, phaseAssert.bytes);
    }
    return phase_state;
}

/* 0233B874 */
BOOL daObjEkskz::Act_c::Mthd_Delete() {
    BOOL ret = MoveBGDelete();
    /* base_process_class +0xD: the create result; 3 (cPhs_STOP_e) when Mthd_Create stopped
     * before loading the archive */
    if ((u8)mCreateResult != 3)
        dComIfG_resDelete(&mPhs, M_arcname.bytes);
    return ret;
}

/* 0233B8CC */
BOOL daObjEkskz::Act_c::CreateHeap() {
    J3DModelData *model_data = (J3DModelData *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_EKSKZ_BDL_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x90, model_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME.bytes, 0x90, modelAssert.bytes);
    J3DModel *m = mDoExt_J3DModel__create(model_data, 0, 0x11020203);
    mpModel = m;
    if (m == nullptr)
        return FALSE;

    J3DModelData *model_data_eff = (J3DModelData *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_EKSKZ_BDL_EFF_e, SAFESTRING_VTBL);
    if (model_data_eff == nullptr) /* JUT_ASSERT(0x9B, model_data_eff != NULL) */
        JUT_ASSERT_fail(FILE_NAME.bytes, 0x9B, effectAssert.bytes);
    m = mDoExt_J3DModel__create(model_data_eff, 0, 0x11020203);
    mpModelEff = m;
    if (m == nullptr)
        return FALSE;

    J3DAnmTransform *bck = (J3DAnmTransform *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_EKSKZ_BCK_e, SAFESTRING_VTBL);
    if (bck == nullptr) /* JUT_ASSERT(0xA4, bck != NULL) */
        JUT_ASSERT_fail(FILE_NAME.bytes, 0xA4, bckAssert.bytes);
    if (!mBckAnm.init(model_data_eff, bck, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    J3DAnmTextureSRTKey *btk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(
        M_arcname.bytes, dRes_INDEX_EKSKZ_BTK_e, SAFESTRING_VTBL);
    if (btk == nullptr) /* JUT_ASSERT(0xAB, btk != NULL) */
        JUT_ASSERT_fail(FILE_NAME.bytes, 0xAB, btkAssert.bytes);
    if (!mBtkAnm.init(model_data_eff, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    void *brk = dComIfG_getObjectRes(M_arcname.bytes, dRes_INDEX_EKSKZ_BRK_e, SAFESTRING_VTBL);
    if (brk == nullptr) /* JUT_ASSERT(0xB2, brk != NULL) */
        JUT_ASSERT_fail(FILE_NAME.bytes, 0xB2, brkAssert.bytes);
    if (!ekskz_mDoExt_brkAnm_init(&mBrkAnm, model_data_eff, brk, true, J3DFrameCtrl::EMode_NONE,
                                  1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}

/* 0233BB30 */
void daObjEkskz::Act_c::set_mtx() {
    u16 ax = (u16)current.angle.x, ay = (u16)current.angle.y;
    shape_angle.x = (s16)ax;
    f32 x = current.pos.x;
    u16 az = (u16)current.angle.z;
    f32 y = current.pos.y, z = current.pos.z;
    shape_angle.y = (s16)ay;
    shape_angle.z = (s16)az;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mpModelEff, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}

/* 0233BC80 */
void daObjEkskz::Act_c::init_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    J3DModel_setBaseScale(mpModelEff, &scale);
    set_mtx();
}

/* 0233BCBC */
BOOL daObjEkskz::Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -4000.0f, -500.0f, -4000.0f, 4000.0f, 500.0f, 4000.0f);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&cylinderSource);
    mCyl.SetC(&current.pos);
    mCyl.SetStts(&mStts);
    m480 = 0;
    ekskz_dKy_tevstr_init(&mTevStr, home.roomNo, 0xFF);

    s8 room = current.roomNo;
    dPa_control_c *pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, 0xA2C2, &current.pos, &current.angle, nullptr, 0xA0,
                    (dPa_levelEcallBack *)&mSmokeCallback[0], room, nullptr, nullptr, nullptr);
    room = current.roomNo;
    pa = dComIfGp_getParticle();
    dPa_control_set(pa, 2, 0xA2C3, &current.pos, &current.angle, nullptr, 0xA0,
                    (dPa_levelEcallBack *)&mSmokeCallback[1], room, nullptr, nullptr, nullptr);

    /* the smoke takes the colour of the statue's light (mTevStr + 0x98) */
    if (mSmokeCallback[0].mpEmitter != 0)
        emitter_setColors(mSmokeCallback[0], mTevStr.mColorK0);
    if (mSmokeCallback[1].mpEmitter != 0)
        emitter_setColors(mSmokeCallback[1], mTevStr.mColorK0);

    attention_info.distances[0] = 0x31; /* attention_info.distances[0] */
    f32 y = current.pos.y + 200.0f;
    attention_info.position.y = y; /* attention_info.position.y */
    eyePos.y = y;
    return TRUE;
}

/* 0233BEC8 */
BOOL daObjEkskz::Act_c::Execute(Mtx34 **mtx) {
    attention_info.flags = (u32)attention_info.flags | 1; /* attention_info.flags */
    mBckAnm.play();
    mBtkAnm.play();
    dComIfG_Ccsp_Set(&mCyl);
    if (m480 == 0) {
        if (mCyl.ChkTgHit()) {
            ekskz_mDoAud_seStart_1(0x806);
            dComIfGs_onSwitch(prm_get_swSave(), home.roomNo);
            m480 = 1;
            dPa_smokeEcallBack_end(&mSmokeCallback[0]);
            dPa_smokeEcallBack_end(&mSmokeCallback[1]);
            u8 r = tevStr.mColorK0.r, g = tevStr.mColorK0.g, b = tevStr.mColorK0.b;
            JPABaseEmitter *e = dComIfGp_particle_set(0x82EC, &current.pos, &current.angle);
            mpSmoke0 = e;
            if (e != nullptr) {
                reinterpret_cast<EkskzEmitter *>(e)->mPrmColor.r = r;
                reinterpret_cast<EkskzEmitter *>(e)->mPrmColor.g = g;
                reinterpret_cast<EkskzEmitter *>(e)->mPrmColor.b = b;
            }
            // HD: smoke callback setup is inlined before the particle call.
            mSmokeCallback[3].mWindOff = 1;
            mSmokeCallback[3].m12 = 1;
            s8 room = current.roomNo;
            dPa_control_c *pa = dComIfGp_getParticle();
            dPa_control_set(pa, 2, 0xA2ED, &current.pos, &current.angle, nullptr, 0x80,
                            (dPa_levelEcallBack *)&mSmokeCallback[3], room, nullptr, nullptr,
                            nullptr);
            mSmokeCallback[3].mTevstr = gabi::ea(&mTevStr);
        }
    }
    if (m480 != 0) {
        mBrkAnm.play();
        if (mBrkAnm.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) ||
            mBrkAnm.mFrameCtrl.getRate() == 0.0f)
            fopAcM_delete(this);
    }
    set_mtx();
    *reinterpret_cast<gptr<Mtx34> *>(mtx) = &M_tmp_mtx;
    return TRUE;
}

/* 0233C0C8 */
BOOL daObjEkskz::Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &mTevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModelEff, &mTevStr);
    mBckAnm.entry(J3DModel_getModelData(mpModelEff), mBckAnm.getFrame());
    mBtkAnm.entry(J3DModel_getModelData(mpModelEff), mBtkAnm.getFrame());
    ekskz_mDoExt_brkAnm_entry(&mBrkAnm, J3DModel_getModelData(mpModelEff), mBrkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModelEff);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    if (m480 == 0)
        mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* 0233C1D8 */
BOOL daObjEkskz::Act_c::Delete() {
    for (int i = 0; i < 4; i++) {
        /* mSmokeCallback[i].remove() (virtual, slot 0x44) */
        mSmokeCallback[i].remove();
    }
    return TRUE;
}

/* method table (0x101C9084): Create, Delete, Execute, IsDelete, Draw (HD: tail branches) */
/* 0233C22C */
static cPhs_State Mthd_Create(void *i_this) { return ((Act_c *)i_this)->Mthd_Create(); }
/* 0233C230 */
static BOOL Mthd_Delete(void *i_this) { return ((Act_c *)i_this)->Mthd_Delete(); }
/* 0233C234 */
static BOOL Mthd_Execute(void *i_this) { return ((Act_c *)i_this)->MoveBGExecute(); }
/* 0233C238 */
static BOOL Mthd_Draw(void *i_this) { return ((Act_c *)i_this)->Draw_v(); }
/* 0233C248 */
static BOOL Mthd_IsDelete(void *i_this) { return ((Act_c *)i_this)->IsDelete_v(); }

/* 0233C258 */
static void __sinit_d_a_obj_ekskz_cpp() {
    sinit_header_statics(ekskzAbi::HeaderStatics, ekskzAbi::HeaderStaticsData);
}

/* 0233C2EC */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0233C300 */
static void smokeEcallBack_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0233C314 */
static BOOL dBgS_MoveBgActor_IsDelete(void *p) { return TRUE; }

/* 0233C31C */
static void SafeString_empty(void *p) {}

/* 0233C320 */
static void Act_c_dt(Act_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        ekskz___destroy_arr(i_this->mSmokeCallback, 4, 0x20, 0x0233C300);
        ekskz_destroyMtxCalc(&reinterpret_cast<EkskzBckView *>(&i_this->mBckAnm)
                                  ->mMtxCalcVtable); /* mDoExt_bckAnm's J3DMtxCalc part */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        ekskz_destroyActor(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data M_arcname 10027E90 nocheck
data FILE_NAME 10027E34 nocheck
data phaseAssertFile 10027DB8 nocheck
data phaseAssert 10027DCC nocheck
data modelAssert 10027E10 nocheck
data effectAssert 10027E20 nocheck
data bckAssert 10027E48 nocheck
data btkAssert 10027E54 nocheck
data brkAssert 10027E60 nocheck
data cylinderSource 101C8FEC nocheck
data M_tmp_mtx 104697C0
*/
