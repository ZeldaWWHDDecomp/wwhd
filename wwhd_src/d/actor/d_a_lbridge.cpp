/**
 * d_a_lbridge.cpp (WWHD)
 * Object - Tower of the Gods - Glowing light bridge
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lbridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_lbridge.h"
const gc_tu_data gc_tu = {.safeString = 0x10013D9C};
static LbridgeText m_arcname;
static LbridgeText sourceName;
static LbridgeText modelAssert;
static LbridgeText btkAssert;
static LbridgeText bpkAssert;
static LbridgeText brkAssert;
static LbridgeText appearEvent;
static LbridgeText disappearEvent;

/* 021B2328 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return static_cast<daLbridge_c *>(i_this)->CreateHeap();
}

/* 021B20B4 */
BOOL daLbridge_c::CreateHeap() {
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
        m_arcname.bytes, dRes_INDEX_GBRG00_BDL_GBRG00_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(214, modelData != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0xD6, modelAssert.bytes);

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000U, 0x11000223U);
    if (!mpModel) {
        return FALSE;
    }

    J3DAnmTextureSRTKey *pbtk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(
        m_arcname.bytes, dRes_INDEX_GBRG00_BTK_GBRG00_e, gc_tu.safeString);
    if (pbtk == nullptr) /* JUT_ASSERT(232, pbtk != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0xE8, btkAssert.bytes);
    if (!mBtkAnm.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    void *pbpk =
        dComIfG_getObjectRes(m_arcname.bytes, dRes_INDEX_GBRG00_BPK_GBRG00_e, gc_tu.safeString);
    if (pbpk == nullptr) /* JUT_ASSERT(246, pbpk != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0xF6, bpkAssert.bytes);
    if (!lbridge_mDoExt_bpkAnm_init(&mBpkAnm, modelData, pbpk, TRUE, J3DFrameCtrl::EMode_NONE, 1.0f,
                                    0, -1, false, 0)) {
        return FALSE;
    }

    mBpkAnm.mFrameCtrl.setFrame(0.0f);
    mBpkAnm.mFrameCtrl.setRate(1.0f); /* setPlaySpeed */

    void *pbrk =
        dComIfG_getObjectRes(m_arcname.bytes, dRes_INDEX_GBRG00_BRK_GBRG00_e, gc_tu.safeString);
    if (pbrk == nullptr) /* JUT_ASSERT(262, pbrk != NULL) */
        JUT_ASSERT_fail(sourceName.bytes, 0x106, brkAssert.bytes);
    if (!lbridge_mDoExt_brkAnm_init(&mBrkAnm, modelData, pbrk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f,
                                    0, -1, false, 0)) {
        return FALSE;
    }

    setMoveBGMtx();

    mpBgW = new_dBgW();
    if (mpBgW) {
        cBgD_t *dzb = (cBgD_t *)dComIfG_getObjectRes(
            m_arcname.bytes, dRes_INDEX_GBRG00_DZB_HHASHI1_e, gc_tu.safeString);
        if (cBgW_Set(mpBgW, dzb, LBRIDGE_MOVE_BG, &mMtx) == true) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

/* 021B2404 */
void daLbridge_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -100.0f, -150.0f, 600.0f, 100.0f, 150.0f);
    cullSizeFar = 1.5f; /* fopAcM_setCullSizeFar */

    mpEmitter = reinterpret_cast<LbridgeEmitter *>(
        dComIfGp_particle_set(ID_IT_SN_RBRIDGE_FLSH00, &current.pos, &current.angle));
    if (mpEmitter != nullptr) {
        mpEmitter->stopDrawParticle(); /* stopDrawParticle */
        mpEmitter->mGlobalAlpha = 0;   /* setGlobalAlpha(0) */
    }

    mSwitchNo = daLbridge_prm::getSwitchNo(this);
    mIsSw = fopAcM_isSwitch(this, mSwitchNo) != 0;

    mAppearEventIdx = dComIfGp_evmng_getEventIdx(appearEvent.bytes /* "EFFAPPEAR" */, 0xFF);
    mDisappearEventIdx =
        dComIfGp_evmng_getEventIdx(disappearEvent.bytes /* "BRIDGE_DISAPPEAR" */, 0xFF);

    if (fopAcM_isSwitch(this, mSwitchNo) || mSwitchNo == 0xFF) {
        dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
        set_mtx();
        dBgW_Move(mpBgW);
        if (mpEmitter != nullptr) {
            mpEmitter->playDrawParticle();  /* playDrawParticle */
            mpEmitter->mGlobalAlpha = 0xFF; /* setGlobalAlpha(0xFF) */
        }
    }
}

/* 021B25C8 */
cPhs_State daLbridge_c::_create() {
    /* fopAcM_ct(this, daLbridge_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = LBRIDGE_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
            lbridge_mDoExt_bpkAnm_ct(&mBpkAnm);
            lbridge_mDoExt_brkAnm_construct(&mBrkAnm);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&mPhs, m_arcname.bytes);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)CheckCreateHeap), 0x2E10U)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}

/* 021B232C */
void daLbridge_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 021B2044 */
void daLbridge_c::setMoveBGMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* 021B2E90 */
bool daLbridge_c::_execute() {
    bool isSw = fopAcM_isSwitch(this, mSwitchNo) != 0;

    sw_check();
    demo();
    mBtkAnm.play();
    mDoExt_baseAnm_play(&mBpkAnm);
    mDoExt_baseAnm_play(&mBrkAnm);
    set_mtx();

    mIsSw = isSw;
    return TRUE;
}

/* 021B2AD0 */
void daLbridge_c::sw_check() {
    u8 isSw = fopAcM_isSwitch(this, mSwitchNo) != 0;

    if (mSwitchNo == 0xFF) {
        return;
    }

    if (isSw != mIsSw) {
        if (isSw) {
            appear_bridge();
        } else {
            disappear_bridge();
        }
    }

    if (!isSw) {
        if (mBpkAnm.mFrameCtrl.getFrame() == (f32)mBpkAnm.mFrameCtrl.getStart()) {
            fopAcM_offDraw(this);
        }
        if (mpEmitter != nullptr) {
            gabi::Local<be<u8>> alpha;
            *alpha = (u8)mpEmitter->mGlobalAlpha;
            lbridge_cLib_chaseUC(alpha, 0x0, 0x8);
            mpEmitter->mGlobalAlpha = (u8)*alpha;
        }
    } else {
        lbridge_fopAcM_onDraw(this);
        lbridge_cLib_calcTimer_u8(&mTimer);
        if (mTimer == 0 && mpEmitter != nullptr) {
            gabi::Local<be<u8>> alpha;
            *alpha = (u8)mpEmitter->mGlobalAlpha;
            lbridge_cLib_chaseUC(alpha, 0xFF, 0x8);
            mpEmitter->mGlobalAlpha = (u8)*alpha;
        }
    }
}

/* 021B2C8C */
void daLbridge_c::demo() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        if (dComIfGp_evmng_startCheck(mAppearEventIdx) && unk31C == APPEAR_DEMO) {
            unk31C = NO_DEMO;
        }
        if (dComIfGp_evmng_startCheck(mDisappearEventIdx) && unk31C == DISAPPEAR_DEMO) {
            unk31C = NO_DEMO;
        }
        if (dComIfGp_evmng_endCheck(mAppearEventIdx)) {
            dComIfGp_event_reset();
            dSv_event_onEventBit(dComIfGs_getEvent(), UNK_0E01);
        }
        if (dComIfGp_evmng_endCheck(mDisappearEventIdx)) {
            dComIfGp_event_reset();
            dSv_event_onEventBit(dComIfGs_getEvent(), UNK_0F40);
        }
    } else {
        if (dSv_event_isEventBit(dComIfGs_getEvent(), UNK_0E01) == FALSE && unk31C == APPEAR_DEMO) {
            fopAcM_orderOtherEventId(this, mAppearEventIdx, 0xFF, 0xFFFF, 0, 1);
            eventInfo.onCondition(dEvtCnd_UNK2_e); /* onCondition(dEvtCnd_UNK2_e) */
        } else if (dSv_event_isEventBit(dComIfGs_getEvent(), UNK_0F40) == FALSE &&
                   unk31C == DISAPPEAR_DEMO) {
            fopAcM_orderOtherEventId(this, mDisappearEventIdx, 0xFF, 0xFFFF, 0, 1);
            eventInfo.onCondition(dEvtCnd_UNK2_e);
        }
    }
}

/* 021B2880 */
void daLbridge_c::appear_bridge() {
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    set_mtx();
    dBgW_Move(mpBgW);

    // HD: float stores use one position snapshot, preserving NaN payload behavior.
    gabi::Local<cXyz> pos1;
    gabi::Local<cXyz> pos2;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    pos1->x = x;
    pos1->y = y;
    pos1->z = z + 100.0f;
    pos2->x = x;
    pos2->y = y;
    pos2->z = z - 100.0f;

    /* dComIfGp_particle_setProjection: group 4 */
    dPa_control_set(dComIfGp_getParticle(), 4, ID_IT_SN_RBRIDGE_APP00, pos1, &current.angle,
                    nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dPa_control_set(dComIfGp_getParticle(), 4, ID_IT_SN_RBRIDGE_APP00, pos2, &current.angle,
                    nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);

    set_on_se();

    mBpkAnm.mFrameCtrl.setFrame(0.0f);
    mBpkAnm.mFrameCtrl.setRate(1.0f);

    if (mpEmitter != nullptr) {
        mpEmitter->playCreateParticle(); /* playCreateParticle */
        mpEmitter->playDrawParticle();   /* playDrawParticle */
    }

    mTimer = 31;
    unk31C = APPEAR_DEMO;
}

/* 021B2A38 */
void daLbridge_c::disappear_bridge() {
    cBgS_Release(dComIfG_Bgsp(), mpBgW);
    if (mpEmitter != nullptr) {
        mpEmitter->stopDrawParticle(); /* stopDrawParticle */
    }
    set_off_se();
    mBpkAnm.mFrameCtrl.setFrame((f32)(s16)mBpkAnm.mFrameCtrl.mEnd /* getEndFrame */);
    mBpkAnm.mFrameCtrl.setRate(-1.0f);
    unk31C = DISAPPEAR_DEMO;
}

/* 021B2838 */
void daLbridge_c::set_on_se() { fopAcM_seStart(this, JA_SE_OBJ_L_BRIDGE_ON, 0); }

/* 021B29F0 */
void daLbridge_c::set_off_se() { fopAcM_seStart(this, JA_SE_OBJ_L_BRIDGE_OFF, 0); }

/* 021B2760 */
bool daLbridge_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    mBtkAnm.entry(J3DModel_getModelData(mpModel), mBtkAnm.getFrame());
    lbridge_mDoExt_bpkAnm_entryFrame(&mBpkAnm, J3DModel_getModelData(mpModel),
                                     mBpkAnm.mFrameCtrl.getFrame());
    mDoExt_brkAnm_entry(&mBrkAnm, J3DModel_getModelData(mpModel), mBrkAnm.mFrameCtrl.mFrame);

    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* 021B26A4 */
static cPhs_State daLbridge_Create(void *i_this) {
    return static_cast<daLbridge_c *>(i_this)->_create();
}

/* 021B26A8 */
static BOOL daLbridge_Delete(void *p) {
    daLbridge_c *i_this = static_cast<daLbridge_c *>(p);
    if (i_this->mpEmitter != nullptr) {
        /* becomeInvalidEmitter: max frame -1 (+0x5C), flag 1 */
        LbridgeEmitter *e = i_this->mpEmitter;
        e->becomeInvalidEmitter();
        i_this->mpEmitter = nullptr;
    }

    bool isSw = fopAcM_isSwitch(i_this, i_this->mSwitchNo) != 0;
    if ((i_this->mSwitchNo == 0xFF || isSw == true) && i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhs, m_arcname.bytes); /* dComIfG_resDeleteDemo */
    return TRUE;
}

/* 021B2834 */
static BOOL daLbridge_Draw(void *i_this) { return static_cast<daLbridge_c *>(i_this)->_draw(); }

/* 021B2F1C */
static BOOL daLbridge_Execute(void *i_this) {
    return static_cast<daLbridge_c *>(i_this)->_execute();
}

/* 021B2FC8 */
static BOOL daLbridge_IsDelete(void *) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 021B2F20 */
static void __sinit_d_a_lbridge_cpp() { sinit_header_statics(0x10464DD4, 0x101B8F1C); }

/* 021B2FB4 */
static void deleting_dtor_empty(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021B2FD0 */
static void daLbridge_dtor(daLbridge_c *p, s32 flags) {
    if (p != nullptr) {
        lbridge_destroyActor(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 021B3024 */
static void lbridge_empty(void *p) {}

/* WWHD glue (tools/verify/autoglue.py)
data m_arcname 10013E68 nocheck
data sourceName 10013DCC nocheck
data modelAssert 10013E00 nocheck
data btkAssert 10013DDC nocheck
data bpkAssert 10013DE8 nocheck
data brkAssert 10013DF4 nocheck
data appearEvent 10013E30 nocheck
data disappearEvent 10013E3C nocheck
*/
