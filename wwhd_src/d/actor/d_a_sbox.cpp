/** Salvaged treasure box. Ported from zeldaret/tww, checked against WWHD cking.rpx.
 * HD animation/light layouts and emitter inline writes differ. */
#include "d/actor/d_a_sbox.h"
// HD: per-TU SafeString and common-header static copies.
const gc_tu_data gc_tu = {
    .safeString = 0x10039CB4,
    .headerStatics = {0x1046D86C, 0x101D000C, 0},
};
const char daSbox_c::M_arcname[] = "Dalways";
static u32 action_table[3] = {};
static dEvent_manager_c *eventManager() { return dComIfGp_getPEvtManager(); }
static void cutEnd(s32 staff) { sboxCutEnd(eventManager(), staff); }
static void invalidateEmitter(SboxEmitter *emitter) {
    u32 flags = emitter->mStatus;
    emitter->mMaxFrame = -1;
    emitter->mStatus = flags | 1;
}
static daSbox_c *daSbox_ctor(daSbox_c *);

/* 02469DA0 */
static BOOL CheckCreateHeap(daSbox_c *self) { return self->CreateHeap(); }
/* 02469B40 */
BOOL daSbox_c::CreateHeap() {
    auto res = [](s32 index) { return dComIfG_getObjectRes("Dalways", index); };
    J3DModelData *data = (J3DModelData *)res(dRes_INDEX_DALWAYS_BDL_BOXSEA_e);
    if (!data)
        JUT_ASSERT_fail("d_a_sbox.cpp", 113, "modelData != (0)");
    J3DAnmTransform *anm = (J3DAnmTransform *)res(dRes_INDEX_DALWAYS_BCK_BOXOPENBOX_e);
    if (!mBck1.init(data, anm, true, 0, 1.0f, 0, -1, false))
        return FALSE;
    mpModel1 = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    if (!mpModel1)
        return FALSE;
    data = (J3DModelData *)res(dRes_INDEX_DALWAYS_BDL_IT_TAKARA_FLASH_e);
    if (!data)
        JUT_ASSERT_fail("d_a_sbox.cpp", 140, "flashModelData != (0)");
    mpModel2 = mDoExt_J3DModel__create(data, 0x80000, 0x01000200);
    if (!mpModel2)
        return FALSE;
    anm = (J3DAnmTransform *)res(dRes_INDEX_DALWAYS_BCK_IT_TAKARA_FLASH2_e);
    if (!mBck2.init(data, anm, true, 0, 1.0f, 0, -1, false))
        return FALSE;
    auto btk = (J3DAnmTextureSRTKey *)res(dRes_INDEX_DALWAYS_BTK_IT_TAKARA_FLASH_e);
    if (!mBtk.init(data, btk, true, 0, 1.0f, 0, -1, false, 0))
        return FALSE;
    auto *brk = reinterpret_cast<J3DAnmTevRegKey *>(res(dRes_INDEX_DALWAYS_BRK_IT_TAKARA_FLASH_e));
    if (!mBrk.init(data, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;
    calcMtx();
    return TRUE;
}
/* 024699BC */
void daSbox_c::calcMtx() {
    J3DModel_setBaseScale(mpModel1, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel1, mDoMtx_stack_c::get());
    J3DModel_setBaseScale(mpModel2, &scale);
    mDoMtx_stack_c::transS(current.pos.x, (f32)current.pos.y + 50.0f, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel2, mDoMtx_stack_c::get());
}
/* 02469EB0 */
void daSbox_c::shipMtx() {
    auto *ship = reinterpret_cast<SboxShip *>(dComIfGp_getShipActor());
    if (!ship)
        JUT_ASSERT_fail("d_a_sbox.cpp", 220, "0");
    auto body = [ship]() { return ship->mMorf->getModel()->getBaseTRMtx(); };
    Mtx34 *matrix = mDoMtx_stack_c::get();
    PSMTXCopy(body(), matrix);
    mDoMtx_stack_c::transM(0.0f, 22.35f, 20.0f);
    mDoMtx_stack_c::YrotM(0x7FFF);
    J3DModel_setBaseTRMtx(mpModel1, matrix);
    /* HD recompiled lfs/stfs transport preserves SNaN bits for x/y here,
     * whereas z is quieted. Keep those bits before the integer eyePos copies;
     * normal gabi::load<f32> would quiet x/y prematurely. */
    auto rawFloat = [](const be<f32> *a) {
        u32 bits = *reinterpret_cast<const be<u32> *>(a);
        f32 f;
        memcpy(&f, &bits, 4);
        return f;
    };
    f32 x = rawFloat(&matrix->m[0][3]);
    current.pos.x = x;
    f32 y = rawFloat(&matrix->m[1][3]);
    current.pos.y = y;
    f32 z = matrix->m[2][3];
    attention_info.position.x = x;
    u32 ybits = *reinterpret_cast<be<u32> *>(&current.pos.y);
    current.pos.z = z;
    u32 zbits = *reinterpret_cast<be<u32> *>(&current.pos.z);
    *reinterpret_cast<be<u32> *>(&eyePos.y) = ybits;
    *reinterpret_cast<be<u32> *>(&eyePos.z) = zbits;
    u32 xbits = *reinterpret_cast<be<u32> *>(&current.pos.x);
    attention_info.position.y = y;
    *reinterpret_cast<be<u32> *>(&eyePos.x) = xbits;
    attention_info.position.z = z;
    current.angle.y = ship->shape_angle.y + 0x7FFF;
    shape_angle.y = ship->shape_angle.y;
    PSMTXCopy(body(), matrix);
    mDoMtx_stack_c::transM(0.0f, 72.35f, 20.0f);
    J3DModel_setBaseTRMtx(mpModel2, matrix);
}
/* 0246A744 */
BOOL daSbox_c::volmProc() {
    if (!mEmitter)
        return TRUE;
    s16 timer = (s16)(mVolumeTimer + 1);
    mVolumeTimer = timer;
    if (timer == 36)
        mEmitter->mGlobalAlpha = 255;
    else if (timer >= 181) {
        mEmitter->mGlobalAlpha = 0;
        invalidateEmitter(mEmitter);
        mEmitter = nullptr;
        return TRUE;
    } else if (timer > 156)
        mEmitter->mGlobalAlpha = (181 - timer) * 10;
    return FALSE;
}
/* 0246A7E0 */
BOOL daSbox_c::darkProc() {
    if (!chkFlag(DARKEN_SCENE))
        return TRUE;
    s16 timer = (s16)(mDarkTimer + 1);
    mDarkTimer = timer;
    if (timer > 150) {
        mDarkRatio = 1.0f;
        sboxSetAllColorRatio(1.0f);
        clrFlag(DARKEN_SCENE);
        return TRUE;
    }
    if (timer > 120)
        mDarkRatio = gabi::fmadds(((f32)timer - 120.0f) / 30.0f, 0.6f, 0.4f);
    sboxSetAllColorRatio((f32)mDarkRatio);
    return FALSE;
}
/* 0246A8EC */
BOOL daSbox_c::lightProc() {
    if (!chkFlag(EFFECT_LIGHT))
        return TRUE;
    s16 timer = (s16)(mLightTimer + 1);
    f32 p = mLight.mPower;
    mLightTimer = timer;
    if (timer < 156) {
        if (p < 130.0f)
            mLight.mPower = p + 13.0f;
        f32 q = mEffectLight.mPower;
        if (q < 120.0f)
            mEffectLight.mPower = q + 12.0f;
    } else {
        f32 q = mEffectLight.mPower;
        mLight.mPower = p > 5.2f ? p - 5.2f : 0.0f;
        f32 next = q > 4.8f ? q - 4.8f : 0.0f;
        f32 first = mLight.mPower;
        mEffectLight.mPower = next;
        if (first == 0.0f && next == 0.0f) {
            clrFlag(EFFECT_LIGHT);
            return TRUE;
        }
    }
    return FALSE;
}
/* 0246A1EC */
void daSbox_c::lightInit() {
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mLight.mPos.set(x, y + 55.0f, z);
    mEffectLight.mPos.set(x, y + 50.0f, z);
    mLight.mColorR = 255;
    mLight.mColorG = 255;
    mLight.mColorB = 255;
    mEffectLight.mColorR = 255;
    mEffectLight.mColorG = 255;
    mEffectLight.mColorB = 100;
    mLight.mPower = 0;
    mLight.mFluctuation = 0;
    mEffectLight.mPower = 0;
    mEffectLight.mFluctuation = 0;
    mLightTimer = 0;
    setFlag(EFFECT_LIGHT);
    sboxSetPriorityLight(&mLight);
    dKy_efplight_set(&mEffectLight);
}
/* 0246AE28 */
BOOL daSbox_c::CreateInit() {
    s8 room = current.roomNo;
    mAction = 0;
    tevStr.mRoomNo = room;
    calcMtx();
    return TRUE;
}
/* 0246AE5C */
cPhs_State daSbox_c::create() {
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0)
            daSbox_ctor(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap), 0x15C0))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
/* 0246A188 */
void daSbox_c::demoInitWait() {
    s32 staff = mStaff;
    dEvent_manager_c *mgr = eventManager();
    void *p = sboxGetSubstance(mgr, staff, "Timer", 3);
    mTimer = p ? reinterpret_cast<be<s16> *>(p)[1] : 0;
}
/* 0246A5FC */
BOOL daSbox_c::demoProcWait() {
    s16 timer = mTimer;
    if (timer > 0)
        mTimer = timer - 1;
    else
        cutEnd(mStaff);
    return FALSE;
}
/* 0246A2AC */
void daSbox_c::demoInitOpen() {
    s8 room = current.roomNo;
    mBck1.mFrameCtrl.mRate = 1.0f;
    mBck2.mFrameCtrl.mRate = 1.0f;
    mBrk.mFrameCtrl.mRate = 1.0f;
    mBtk.mFrameCtrl.mRate = 1.0f;
    s32 reverb = sboxGetReverb(room);
    mDoAud_seStart(JA_SE_OBJ_TBOX_OPEN_S1, &eyePos, 0, reverb);
    reverb = sboxGetReverb((s8)current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_TBOX_UNLOCK, &eyePos, 0, reverb);
    sboxStartSubBgm(JA_BGM_OPEN_BOX);
    setFlag(OPEN_EFFECT);
    auto particle = [this](u16 id) {
        return reinterpret_cast<SboxEmitter *>(
            dComIfGp_getParticle()->set(0, id, &current.pos, &current.angle, nullptr, 255, nullptr,
                                        -1, nullptr, nullptr, nullptr));
    };
    particle(dPa_name::ID_IT_JN_TAKARA_PAKAF);
    particle(dPa_name::ID_IT_JN_TAKARA_NAKAF);
    particle(dPa_name::ID_IT_JN_TAKARA_PAKAF_SOTO);
    SboxEmitter *e = particle(dPa_name::ID_IT_JN_TAKARA_UEF);
    if (e) {
        e->mGlobalParticleScale.y = 1.0f;
        e->mEmitterScale.z = 1.0f;
        e->mEmitterScale.x = 0.7f;
        e->mGlobalScale.x = 0.7f;
        e->mGlobalParticleScale.x = 0.7f;
        e->mGlobalScale.z = 1.0f;
        e->mEmitterScale.y = 1.0f;
        e->mGlobalParticleScale.z = 1.0f;
        e->mGlobalScale.y = 1.0f;
    }
    e = particle(dPa_name::ID_IT_JN_TAKARA_TSUBU);
    if (e) {
        e->mEmitterScale.y = 1.0f;
        e->mEmitterScale.x = 0.7f;
        e->mEmitterScale.z = 1.0f;
    }
    e = particle(dPa_name::ID_IT_JN_TAKARA_VOLM);
    mEmitter = e;
    if (e) {
        mVolumeTimer = 0;
        mEmitter->mGlobalAlpha = 0;
        mEmitter->mGlobalParticleScale.y = 0.7f;
    }
    setFlag(DARKEN_SCENE);
    mDarkTimer = 0;
    mDarkRatio = 0.4f;
    lightInit();
}
/* 0246A654 */
void daSbox_c::demoProcOpen() {
    if (chkFlag(OPEN_ANIMATION)) {
        if (mBck1.play()) {
            s32 reverb = sboxGetReverb((s8)current.roomNo);
            mDoAud_seStart(JA_SE_OBJ_TBOX_OPEN_S2, &eyePos, 0, reverb);
            cutEnd(mStaff);
            clrFlag(OPEN_ANIMATION);
        }
    } else
        cutEnd(mStaff);
}
/* 0246A56C */
void daSbox_c::demoInitDelete() {
    if (chkFlag(OPEN_EFFECT)) {
        if (mEmitter) {
            invalidateEmitter(mEmitter);
            mEmitter = nullptr;
        }
        if (chkFlag(DARKEN_SCENE)) {
            sboxSetAllColorRatio(1.0f);
            clrFlag(DARKEN_SCENE);
        }
    }
    dKy_plight_cut(&mLight);
    dKy_efplight_cut(&mEffectLight);
}
/* 0246A70C */
void daSbox_c::demoProcDelete() { cutEnd(mStaff); }
/* 0246A124 */
void daSbox_c::demoInitCom() {
    s32 staff = mStaff;
    dEvent_manager_c *mgr = eventManager();
    if (sboxGetSubstance(mgr, staff, "DISP", 3))
        setFlag(BOX_VISIBLE);
}
/* 0246AA0C */
void daSbox_c::demoProcCom() {
    if (chkFlag(OPEN_EFFECT)) {
        BOOL a = mBck2.play(), b = mBtk.play(), c = mBrk.play();
        if (gabi::ftoi(mBck2.getFrame()) == 36)
            setFlag(EFFECT_VISIBLE);
        BOOL d = volmProc(), e = darkProc(), f = lightProc();
        if (a && b && c && d && e && f)
            clrFlag(OPEN_EFFECT | EFFECT_VISIBLE);
    }
}
/* 0246A0D8 */
s32 daSbox_c::getNowEventAction() {
    s32 staff = mStaff;
    dEvent_manager_c *mgr = eventManager();
    return sboxGetAction(mgr, staff, action_table, 3, 0, 1);
}
/* 0246AAE8 */
BOOL daSbox_c::demoProc() {
    dEvent_manager_c *mgr = eventManager();
    s32 staff = sboxGetStaff(mgr, "tkrSlv", 0, 0);
    mStaff = staff;
    if (staff == -1)
        return FALSE;
    s32 action = getNowEventAction();
    staff = mStaff;
    mgr = eventManager();
    if (sboxIsAdvance(mgr, staff)) {
        setFlag(OPEN_ANIMATION);
        demoInitCom();
        switch (action) {
        case ACT_WAIT:
            demoInitWait();
            break;
        case ACT_OPEN:
            demoInitOpen();
            break;
        case ACT_DELETE:
            demoInitDelete();
            break;
        }
    }
    switch (action) {
    case ACT_WAIT:
        demoProcWait();
        break;
    case ACT_OPEN:
        demoProcOpen();
        break;
    case ACT_DELETE:
        demoProcDelete();
        break;
    default:
        cutEnd(mStaff);
    }
    demoProcCom();
    return TRUE;
}
/* 0246AC6C */
BOOL daSbox_c::actionWait() {
    shipMtx();
    if (!demoProc())
        fopAcM_delete(this);
    return TRUE;
}
/* 02469DA4 */
static BOOL daSbox_Draw(daSbox_c *self) {
    J3DModelData *data = self->mpModel1->getModelData();
    if (!self->chkFlag(BOX_VISIBLE))
        return TRUE;
    auto env = dKy_getEnvlight();
    settingTevStruct(env, 0, &self->current.pos, &self->tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, self->mpModel1, &self->tevStr);
    self->mBck1.entry(data, self->mBck1.getFrame());
    mDoExt_modelUpdateDL(self->mpModel1);
    if (self->chkFlag(EFFECT_VISIBLE)) {
        data = self->mpModel2->getModelData();
        self->mBck2.entry(data, self->mBck2.getFrame());
        self->mBtk.entry(data, self->mBtk.getFrame());
        self->mBrk.entry(data, self->mBrk.getFrame());
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdateDL(self->mpModel2);
        dComIfGd_setList();
    }
    return TRUE;
}
/* 0246ACB4 */
static BOOL daSbox_Execute(daSbox_c *self) {
    if (self->mAction == 0)
        self->actionWait();
    return TRUE;
}
/* 0246ACE4 */
static BOOL daSbox_IsDelete(daSbox_c *self) { return TRUE; }
/* 0246ACEC */
static BOOL daSbox_Delete(daSbox_c *self) {
    dComIfG_resDelete(&self->mPhase, daSbox_c::M_arcname);
    mDoAud_seDeleteObject(&self->eyePos);
    return TRUE;
}
/* 0246AF04 */
static cPhs_State daSbox_Create(daSbox_c *self) { return self->create(); }
// HD: compiler-generated and template code of this translation unit.
/* 0246AD30 */
static daSbox_c *daSbox_ctor(daSbox_c *self) {
    if (!self)
        self = sboxAllocate(sizeof(daSbox_c));
    if (self) {
        fopAc_ac_c_ct(self);
        self->__vtbl = SBOX_VTABLE;
        auto *first = reinterpret_cast<SboxBckAnm *>(&self->mBck1);
        first->mFrameCtrl.init(0);
        first->mVtable = SBOX_ANM_BASE_VTABLE;
        sboxInitTransformCache(first->mTransformCache);
        first->mCacheVtable = SBOX_CACHE_VTABLE;
        first->mFlags = 0;
        first->mCacheCount = 0;
        first->mResource = 0;
        first->mVtable = SBOX_BCK_VTABLE;
        first->mData = 0;
        first->mCacheData = 0;
        auto *second = reinterpret_cast<SboxBckAnm *>(&self->mBck2);
        second->mFrameCtrl.init(0);
        second->mVtable = SBOX_ANM_BASE_VTABLE;
        sboxInitTransformCache(second->mTransformCache);
        second->mCacheCount = 0;
        second->mResource = 0;
        second->mData = 0;
        second->mFlags = 0;
        second->mCacheData = 0;
        second->mCacheVtable = SBOX_CACHE_VTABLE;
        second->mVtable = SBOX_BCK_VTABLE;
        mDoExt_btkAnm::ct(&self->mBtk);
        mDoExt_brkAnm::ct(&self->mBrk);
        self->mEffectLight.mHD20 = 1.0f;
        self->mLight.mHD20 = 1.0f;
    }
    return self;
}
/* 0246AF08 */
static void __sinit_d_a_sbox_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}
/* 0246AF9C */
static void light_dtor_sbox(void *self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 0246AFB0 */
static void daSbox_dtor(daSbox_c *self, s32 flags) {
    if (self) {
        sboxDestroyBckCache(&reinterpret_cast<SboxBckAnm *>(&self->mBck2)->mVtable, 0);
        sboxDestroyBckCache(&reinterpret_cast<SboxBckAnm *>(&self->mBck1)->mVtable, 0);
        sboxDestroyActor(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}
/* 0246B01C */
static void daSbox_emptyVirtual(daSbox_c *self) {}

/* WWHD glue (tools/verify/autoglue.py)
data action_table 101D0000 nocheck
str 10039D0C in 02469B40
str 10039DE0 in 0246AE5C
str 10039DE0 in 0246ACEC
str 10039D2C in 02469B40
str 10039D3C
str 10039D14
str 10039D64 in 02469EB0
str 10039D60
str 10039D8C
str 10039D94
str 10039DD0
ext 02543280 void sboxCutEnd(dEvent_manager_c*, s32)
ext 0254487C void* sboxGetSubstance(dEvent_manager_c*, s32, const char*, s32)
ext 02542EDC s32 sboxGetAction(dEvent_manager_c*, s32, be<u32>*, s32, s32, s32)
ext 02542D88 s32 sboxGetStaff(dEvent_manager_c*, const char*, u32, s32)
ext 025447C8 BOOL sboxIsAdvance(dEvent_manager_c*, s32)
ext 02560444 void sboxSetAllColorRatio(f32)
ext 0255A2B8 void sboxSetPriorityLight(LIGHT_INFLUENCE*)
ext 02520540 s32 sboxGetReverb(s8)
ext 025E1918 void sboxStartSubBgm(u32)
ext 0273AD10 daSbox_c* sboxAllocate(u32)
ext 027DA984 void sboxInitTransformCache(void*)
ext 027F3628 void sboxDestroyBckCache(void*, s32)
ext 025D50BC void sboxDestroyActor(fopAc_ac_c*, s32)
*/
