/**
 * d_a_lbridge.cpp (WWHD)
 * Object - Tower of the Gods - Glowing light bridge
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_lbridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10013D9C /* this TU's sead::SafeString vtable */
#define LBRIDGE_VTBL 0x10013DB4
#define m_arcname STR(0x10013E68) /* "Gbrg00" */
#define FILE_NAME STR(0x10013DCC) /* "d_a_lbridge.cpp" */

enum {
    dRes_INDEX_GBRG00_BDL_GBRG00_e = 7,
    dRes_INDEX_GBRG00_BPK_GBRG00_e = 0xA,
    dRes_INDEX_GBRG00_BRK_GBRG00_e = 0xD,
    dRes_INDEX_GBRG00_BTK_GBRG00_e = 0x10,
    dRes_INDEX_GBRG00_DZB_HHASHI1_e = 0x13,
};
enum { JA_SE_OBJ_L_BRIDGE_ON = 0x6956, JA_SE_OBJ_L_BRIDGE_OFF = 0x6957 };
enum { ID_IT_SN_RBRIDGE_FLSH00 = 0x810F, ID_IT_SN_RBRIDGE_APP00 = 0x8119 };
enum { UNK_0E01 = 0xE01, UNK_0F40 = 0xF40 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* mDoExt_bpkAnm (HD 0x74): constructor 025E7480, init 025E74FC (entry on the stack), entry 025E779C */
static inline void mDoExt_bpkAnm_ct(void* p) { gabi::call(0x025E7480, p); }
static inline BOOL mDoExt_bpkAnm_init(void* a, J3DModelData* d, void* bpk, s32 play, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E74FC, a, d, bpk, play, mode, rate, start, end, modify, entry);
}
static inline void mDoExt_bpkAnm_entry(void* a, J3DModelData* d, f32 frame) { gabi::call(0x025E779C, a, d, frame); }
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 (entry on the stack); entry 025E83FC (shared) */
static inline void mDoExt_brkAnm_ct_l(void* p) { gabi::call(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 play, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, play, mode, rate, start, end, modify, entry);
}
/* 0200F4FC cLib_chaseUC(u8* v, u8 target, u8 step); 0207A9A0 cLib_calcTimer<u8> */
static inline void cLib_chaseUC(be<u8>* v, u8 target, u8 step) { gabi::call(0x0200F4FC, v, target, step); }
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) (as in d_a_npc_md) */
static inline s16 fpcLf_GetPriority(void* a) { return gabi::call<s16>(0x025DF2B8, a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { gabi::call(0x025DA874, tag, prio); }
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s16 prio = fpcLf_GetPriority(a);
    fopDwTg_ToDrawQ(gabi::ea(a) + 0xDC, prio);
}
/* JPABaseEmitter (HD): status flags at +0x254, global alpha (u8) at +0x247 */
static inline void emitter_flags_or(u32 e, u32 v) { gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | v); }
static inline void emitter_flags_and(u32 e, u32 v) { gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & v); }
static inline dSv_event_c* dComIfGs_getEvent_l() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }

struct daLbridge_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    void setMoveBGMtx();
    bool _execute();
    void sw_check();
    void demo();
    void appear_bridge();
    void disappear_bridge();
    void set_on_se();
    void set_off_se();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ Mtx34 mMtx;
    /* 0x3EC */ mDoExt_btkAnm mBtkAnm;
    /* 0x460 */ u8 mBpkAnm[0x74];  /* J3DFrameCtrl at +0: rate +0, frame +4, start +8, end +0xA */
    /* 0x4D4 */ u8 mBrkAnm[0x78];
    /* 0x54C */ be<u32> mpEmitter;
    /* 0x550 */ be<s32> mSwitchNo;
    /* 0x554 */ be<s16> mAppearEventIdx;
    /* 0x556 */ be<s16> mDisappearEventIdx;
    /* 0x558 */ be<s16> unk31C;
    /* 0x55A */ be<u8> mTimer;
    /* 0x55B */ be<u8> mIsSw;

    J3DFrameCtrl& bpkCtrl() { return *gabi::at<J3DFrameCtrl>(gabi::ea(this) + 0x460); }
};
WWHD_OFFSET(daLbridge_c, mMtx, 0x3BC);
WWHD_OFFSET(daLbridge_c, mBtkAnm, 0x3EC);
WWHD_OFFSET(daLbridge_c, mBpkAnm, 0x460);
WWHD_OFFSET(daLbridge_c, mBrkAnm, 0x4D4);
WWHD_OFFSET(daLbridge_c, mpEmitter, 0x54C);
WWHD_OFFSET(daLbridge_c, mIsSw, 0x55B);
WWHD_SIZE(daLbridge_c, 0x55C);

namespace daLbridge_prm {
inline u8 getSwitchNo(daLbridge_c* ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
};  // namespace daLbridge_prm

/* 021B2328 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B2328, BOOL, i_this);
    return static_cast<daLbridge_c*>(i_this)->CreateHeap();
}
VERIFY(0x021B2328, CheckCreateHeap);

/* 021B20B4 */
BOOL daLbridge_c::CreateHeap() {
    WWHD_FUNC(0x021B20B4, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_GBRG00_BDL_GBRG00_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(214, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xD6, STR(0x10013E00));

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000U, 0x11000223U);
    if (!mpModel) {
        return FALSE;
    }

    J3DAnmTextureSRTKey* pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_GBRG00_BTK_GBRG00_e, SAFESTRING_VTBL);
    if (pbtk == nullptr) /* JUT_ASSERT(232, pbtk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xE8, STR(0x10013DDC));
    if (!mBtkAnm.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    void* pbpk = dComIfG_getObjectRes(m_arcname, dRes_INDEX_GBRG00_BPK_GBRG00_e, SAFESTRING_VTBL);
    if (pbpk == nullptr) /* JUT_ASSERT(246, pbpk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xF6, STR(0x10013DE8));
    if (!mDoExt_bpkAnm_init(mBpkAnm, modelData, pbpk, TRUE, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    bpkCtrl().setFrame(0.0f);
    bpkCtrl().setRate(1.0f); /* setPlaySpeed */

    void* pbrk = dComIfG_getObjectRes(m_arcname, dRes_INDEX_GBRG00_BRK_GBRG00_e, SAFESTRING_VTBL);
    if (pbrk == nullptr) /* JUT_ASSERT(262, pbrk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x106, STR(0x10013DF4));
    if (!mDoExt_brkAnm_init(mBrkAnm, modelData, pbrk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    setMoveBGMtx();

    mpBgW = new_dBgW();
    if (mpBgW) {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_GBRG00_DZB_HHASHI1_e, SAFESTRING_VTBL);
        if (cBgW_Set(mpBgW, dzb, 1 /* cBgW::MOVE_BG_e */, &mMtx) == true) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x021B20B4, &daLbridge_c::CreateHeap);

/* 021B2404 */
void daLbridge_c::CreateInit() {
    WWHD_FUNC(0x021B2404, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -100.0f, -150.0f, 600.0f, 100.0f, 150.0f);
    cullSizeFar = 1.5f; /* fopAcM_setCullSizeFar */

    mpEmitter = gabi::ea(dComIfGp_particle_set(ID_IT_SN_RBRIDGE_FLSH00, &current.pos, &current.angle));
    if (mpEmitter != 0) {
        emitter_flags_or(mpEmitter, 4);        /* stopDrawParticle */
        gabi::store<u8>(mpEmitter + 0x247, 0); /* setGlobalAlpha(0) */
    }

    mSwitchNo = daLbridge_prm::getSwitchNo(this);
    mIsSw = fopAcM_isSwitch(this, mSwitchNo) != 0;

    mAppearEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10013E30) /* "EFFAPPEAR" */, 0xFF);
    mDisappearEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10013E3C) /* "BRIDGE_DISAPPEAR" */, 0xFF);

    if (fopAcM_isSwitch(this, mSwitchNo) || mSwitchNo == 0xFF) {
        dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
        set_mtx();
        dBgW_Move(mpBgW);
        if (mpEmitter != 0) {
            emitter_flags_and(mpEmitter, ~4u);        /* playDrawParticle */
            gabi::store<u8>(mpEmitter + 0x247, 0xFF); /* setGlobalAlpha(0xFF) */
        }
    }
}
VERIFY(0x021B2404, &daLbridge_c::CreateInit);

/* 021B25C8 */
cPhs_State daLbridge_c::_create() {
    WWHD_FUNC(0x021B25C8, cPhs_State, this);
    /* fopAcM_ct(this, daLbridge_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = LBRIDGE_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
            mDoExt_bpkAnm_ct(mBpkAnm);
            mDoExt_brkAnm_ct_l(mBrkAnm);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&mPhs, m_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x021B2328 /* CheckCreateHeap */, 0x2E10U)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return ret;
}
VERIFY(0x021B25C8, &daLbridge_c::_create);

/* 021B232C */
void daLbridge_c::set_mtx() {
    WWHD_FUNC(0x021B232C, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x021B232C, &daLbridge_c::set_mtx);

/* 021B2044 */
void daLbridge_c::setMoveBGMtx() {
    WWHD_FUNC(0x021B2044, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
VERIFY(0x021B2044, &daLbridge_c::setMoveBGMtx);

/* 021B2E90 */
bool daLbridge_c::_execute() {
    WWHD_FUNC(0x021B2E90, bool, this);
    bool isSw = fopAcM_isSwitch(this, mSwitchNo) != 0;

    sw_check();
    demo();
    mBtkAnm.play();
    mDoExt_baseAnm_play(mBpkAnm);
    mDoExt_baseAnm_play(mBrkAnm);
    set_mtx();

    mIsSw = isSw;
    return TRUE;
}
VERIFY(0x021B2E90, &daLbridge_c::_execute);

/* 021B2AD0 */
void daLbridge_c::sw_check() {
    WWHD_FUNC(0x021B2AD0, void, this);
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
        if (bpkCtrl().getFrame() == (f32)bpkCtrl().getStart()) {
            fopAcM_offDraw(this);
        }
        if (mpEmitter != 0) {
            gabi::Local<be<u8>> alpha;
            *alpha = gabi::load<u8>(mpEmitter + 0x247);
            cLib_chaseUC(alpha, 0x0, 0x8);
            gabi::store<u8>(mpEmitter + 0x247, *alpha);
        }
    } else {
        fopAcM_onDraw(this);
        cLib_calcTimer_u8(&mTimer);
        if (mTimer == 0 && mpEmitter != 0) {
            gabi::Local<be<u8>> alpha;
            *alpha = gabi::load<u8>(mpEmitter + 0x247);
            cLib_chaseUC(alpha, 0xFF, 0x8);
            gabi::store<u8>(mpEmitter + 0x247, *alpha);
        }
    }
}
VERIFY(0x021B2AD0, &daLbridge_c::sw_check);

/* 021B2C8C */
void daLbridge_c::demo() {
    WWHD_FUNC(0x021B2C8C, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mAppearEventIdx) && unk31C == 1) {
            unk31C = 0;
        }
        if (dComIfGp_evmng_startCheck(mDisappearEventIdx) && unk31C == 2) {
            unk31C = 0;
        }
        if (dComIfGp_evmng_endCheck(mAppearEventIdx)) {
            dComIfGp_event_reset();
            dSv_event_onEventBit(dComIfGs_getEvent_l(), UNK_0E01);
        }
        if (dComIfGp_evmng_endCheck(mDisappearEventIdx)) {
            dComIfGp_event_reset();
            dSv_event_onEventBit(dComIfGs_getEvent_l(), UNK_0F40);
        }
    } else {
        if (dSv_event_isEventBit(dComIfGs_getEvent_l(), UNK_0E01) == FALSE && unk31C == 1) {
            fopAcM_orderOtherEventId(this, mAppearEventIdx, 0xFF, 0xFFFF, 0, 1);
            gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 2); /* onCondition(dEvtCnd_UNK2_e) */
        } else if (dSv_event_isEventBit(dComIfGs_getEvent_l(), UNK_0F40) == FALSE && unk31C == 2) {
            fopAcM_orderOtherEventId(this, mDisappearEventIdx, 0xFF, 0xFFFF, 0, 1);
            gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 2);
        }
    }
}
VERIFY(0x021B2C8C, &daLbridge_c::demo);

/* 021B2880 */
void daLbridge_c::appear_bridge() {
    WWHD_FUNC(0x021B2880, void, this);
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    set_mtx();
    dBgW_Move(mpBgW);

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
    dPa_control_set(dComIfGp_getParticle(), 4, ID_IT_SN_RBRIDGE_APP00, pos1, &current.angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    dPa_control_set(dComIfGp_getParticle(), 4, ID_IT_SN_RBRIDGE_APP00, pos2, &current.angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);

    set_on_se();

    bpkCtrl().setFrame(0.0f);
    bpkCtrl().setRate(1.0f);

    if (mpEmitter != 0) {
        emitter_flags_and(mpEmitter, ~1u); /* playCreateParticle */
        emitter_flags_and(mpEmitter, ~4u); /* playDrawParticle */
    }

    mTimer = 31;
    unk31C = 1;
}
VERIFY(0x021B2880, &daLbridge_c::appear_bridge);

/* 021B2A38 */
void daLbridge_c::disappear_bridge() {
    WWHD_FUNC(0x021B2A38, void, this);
    cBgS_Release(dComIfG_Bgsp(), mpBgW);
    if (mpEmitter != 0) {
        emitter_flags_or(mpEmitter, 4); /* stopDrawParticle */
    }
    set_off_se();
    bpkCtrl().setFrame((f32)(s16)gabi::load<s16>(gabi::ea(this) + 0x46A) /* getEndFrame */);
    bpkCtrl().setRate(-1.0f);
    unk31C = 2;
}
VERIFY(0x021B2A38, &daLbridge_c::disappear_bridge);

/* 021B2838 */
void daLbridge_c::set_on_se() {
    WWHD_FUNC(0x021B2838, void, this);
    fopAcM_seStart(this, JA_SE_OBJ_L_BRIDGE_ON, 0);
}
VERIFY(0x021B2838, &daLbridge_c::set_on_se);

/* 021B29F0 */
void daLbridge_c::set_off_se() {
    WWHD_FUNC(0x021B29F0, void, this);
    fopAcM_seStart(this, JA_SE_OBJ_L_BRIDGE_OFF, 0);
}
VERIFY(0x021B29F0, &daLbridge_c::set_off_se);

/* 021B2760 */
bool daLbridge_c::_draw() {
    WWHD_FUNC(0x021B2760, bool, this);
    settingTevStruct(dKy_getEnvlight(), 1 /* TEV_TYPE_BG0 */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    mBtkAnm.entry(J3DModel_getModelData(mpModel), mBtkAnm.getFrame());
    mDoExt_bpkAnm_entry(mBpkAnm, J3DModel_getModelData(mpModel), bpkCtrl().getFrame());
    mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrkAnm, J3DModel_getModelData(mpModel), gabi::load<f32>(gabi::ea(this) + 0x4D8));

    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x021B2760, &daLbridge_c::_draw);

/* 021B26A4 */
static cPhs_State daLbridge_Create(void* i_this) {
    WWHD_FUNC(0x021B26A4, cPhs_State, i_this);
    return static_cast<daLbridge_c*>(i_this)->_create();
}
VERIFY(0x021B26A4, daLbridge_Create);

/* 021B26A8: _delete() inlined */
static BOOL daLbridge_Delete(void* p) {
    WWHD_FUNC(0x021B26A8, BOOL, p);
    daLbridge_c* i_this = static_cast<daLbridge_c*>(p);
    if (i_this->mpEmitter != 0) {
        /* becomeInvalidEmitter: max frame -1 (+0x5C), flag 1 */
        u32 e = i_this->mpEmitter;
        gabi::store<s32>(e + 0x5C, -1);
        emitter_flags_or(e, 1);
        i_this->mpEmitter = 0;
    }

    bool isSw = fopAcM_isSwitch(i_this, i_this->mSwitchNo) != 0;
    if ((i_this->mSwitchNo == 0xFF || isSw == true) && i_this->heap != nullptr) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }

    dComIfG_resDelete(&i_this->mPhs, m_arcname); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x021B26A8, daLbridge_Delete);

/* 021B2834 */
static BOOL daLbridge_Draw(void* i_this) {
    WWHD_FUNC(0x021B2834, BOOL, i_this);
    return static_cast<daLbridge_c*>(i_this)->_draw();
}
VERIFY(0x021B2834, daLbridge_Draw);

/* 021B2F1C */
static BOOL daLbridge_Execute(void* i_this) {
    WWHD_FUNC(0x021B2F1C, BOOL, i_this);
    return static_cast<daLbridge_c*>(i_this)->_execute();
}
VERIFY(0x021B2F1C, daLbridge_Execute);

/* 021B2FC8 */
static BOOL daLbridge_IsDelete(void*) {
    WWHD_FUNC(0x021B2FC8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B2FC8, daLbridge_IsDelete);

/* 021B2F20: __sinit_d_a_lbridge_cpp (HD header statics only) */
static void __sinit_d_a_lbridge_cpp() {
    WWHD_FUNC(0x021B2F20, void, (u32)0);
    sinit_header_statics(0x10464DD4, 0x101B8F1C);
}
VERIFY(0x021B2F20, __sinit_d_a_lbridge_cpp);

/* 021B2FB4: deleting destructor of an empty class (this TU's copy) */
static void deleting_dtor_empty(void* p, s32 flags) {
    WWHD_FUNC(0x021B2FB4, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021B2FB4, deleting_dtor_empty);

/* 021B2FD0: daLbridge_c::~daLbridge_c (deleting destructor; the anm members have no destructor) */
static void daLbridge_dtor(daLbridge_c* p, s32 flags) {
    WWHD_FUNC(0x021B2FD0, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x021B2FD0, daLbridge_dtor);

/* 021B3024: empty virtual (next to 021B2FB4 in the same per-TU vtable) */
static void lbridge_empty(void* p) {
    WWHD_FUNC(0x021B3024, void, p);
}
VERIFY(0x021B3024, lbridge_empty);
