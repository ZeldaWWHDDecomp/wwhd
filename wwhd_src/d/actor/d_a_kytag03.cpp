/**
 * d_a_kytag03.cpp (WWHD)
 * Environment tag 03: contrast colour pattern by switch and the "M_door" model shown behind the
 * player during events.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag03.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG03_VTBL 0x10013B1C    /* kytag03_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x10013B04 /* this TU's sead::SafeString vtable */

enum { dRes_INDEX_M_DOOR_BDL_MYAMIF_e = 3 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dStage_roomControl_c::mStayNo (s8 at 0x1047E6C8) */
static inline s8 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* 0255FD48 dKy_change_colpat(u8) */
static inline void dKy_change_colpat(u8 pat) { gabi::call(0x0255FD48, pat); }
/* 02560704 dKy_contrast_flg_get(), 025606D4 dKy_contrast_flg_set(bool) */
static inline BOOL dKy_contrast_flg_get() { return gabi::call<BOOL>(0x02560704); }
static inline void dKy_contrast_flg_set(bool f) { gabi::call(0x025606D4, f); }
/* dComIfGp_event_runCheck(): play+0x5292 (u8) != 0 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* dComIfGp_event_chkEventFlag(flag): the event control's flag word (play+0x52B8, u16) */
static inline bool dComIfGp_event_chkEventFlag(u16 flag) { return (gabi::load<u16>(dComIfGp_ea() + 0x52B8) & flag) != 0; }
enum { dEvtFlag_STAFF_ALL_e = 2 };

struct kytag03_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpModel;
    /* 0x3B8 */ u8 m3B8[4];
    /* 0x3BC */ be<u8> field_0x2a0;
    /* 0x3BD */ be<u8> mSwitchNo;
    /* 0x3BE */ be<u8> mbRoomActive;
    /* 0x3BF */ be<u8> mbVisible;
    /* 0x3C0 */ be<u8> mbIsActive;
    /* 0x3C1 */ u8 _3C1[3];
    /* 0x3C4 */ be<f32> field_0x2a8;
};
WWHD_SIZE(kytag03_class, 0x3C8);

/* 021AFD48 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021AFD48, BOOL, i_ac);
    kytag03_class* i_this = (kytag03_class*)i_ac;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10013B30) /* "M_door" */, dRes_INDEX_M_DOOR_BDL_MYAMIF_e, SAFESTRING_VTBL);
    i_this->mpModel = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, 2 /* J3DFrameCtrl::EMode_LOOP */,
                                             0.0f, 0, -1, 1, nullptr, 0x0, 0x11020203);

    if (i_this->mpModel == nullptr || i_this->mpModel->getModel() == nullptr)
        return FALSE;

    return TRUE;
}
VERIFY(0x021AFD48, useHeapInit);

/* 021AFE18 */
static BOOL daKytag03_Draw(kytag03_class* i_this) {
    WWHD_FUNC(0x021AFE18, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    J3DModel* model = i_this->mpModel->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &actor->tevStr);
    if (i_this->mbVisible)
        i_this->mpModel->updateDL();
    return TRUE;
}
VERIFY(0x021AFE18, daKytag03_Draw);

/* draw_SUB (inlined into Execute) */
static inline void draw_SUB(kytag03_class* i_this) {
    fopAc_ac_c* actor = i_this;
    J3DModel* model = i_this->mpModel->getModel();
    J3DModel_setBaseScale(model, &actor->scale);
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}

static inline void contrast_on(kytag03_class* i_this) {
    if (!dKy_contrast_flg_get()) {
        dKy_contrast_flg_set(true);
        dKy_change_colpat(4);
    }
    i_this->mbIsActive = true;
}
static inline void contrast_off(kytag03_class* i_this) {
    if (dKy_contrast_flg_get()) {
        dKy_contrast_flg_set(false);
        dKy_change_colpat(0);
    }
    i_this->mbIsActive = false;
}

/* 021AFE8C */
static BOOL daKytag03_Execute(kytag03_class* i_this) {
    WWHD_FUNC(0x021AFE8C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    if (dComIfGp_event_runCheck() == FALSE || !dComIfGp_event_chkEventFlag(dEvtFlag_STAFF_ALL_e)) {
        if (actor->tevStr.mRoomNo == dComIfGp_roomControl_getStayNo()) {
            i_this->mbRoomActive = true;
            if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, dComIfGp_roomControl_getStayNo())) {
                contrast_on(i_this);
            } else {
                contrast_off(i_this);
            }
        }
    } else if (actor->tevStr.mRoomNo != dComIfGp_roomControl_getStayNo()) {
        /* GHS tests the room once for both event branches */
        if (!i_this->mbRoomActive) {
            if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, actor->tevStr.mRoomNo)) {
                contrast_on(i_this);
            } else {
                contrast_off(i_this);
            }
        } else {
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (!i_this->mbIsActive && dKy_contrast_flg_get()) {
                if (!i_this->mbVisible) {
                    gabi::Local<cXyz> pos;
                    const s16 y = player->shape_angle.y;
                    pos->x = cM_scos(0) * cM_ssin(y);
                    pos->y = cM_ssin(0);
                    pos->z = cM_scos(0) * cM_scos(y);
                    gabi::Local<cXyz> ofs;
                    gabi::Local<cXyz> res;
                    cXyz_ml(pos, ofs, -100.0f);
                    cXyz_pl(&player->current.pos, res, ofs);
                    actor->current.pos.copy(*res);
                    actor->current.angle.y = player->current.angle.y;
                    actor->shape_angle.y = player->shape_angle.y;
                }
                i_this->mbVisible = true;
            }
        }
    }

    if (i_this->mbVisible) {
        i_this->mpModel->play(nullptr, 0, 0);
        draw_SUB(i_this);
    }

    return TRUE;
}
VERIFY(0x021AFE8C, daKytag03_Execute);

/* 021B0244 */
static BOOL daKytag03_IsDelete(kytag03_class* i_this) {
    WWHD_FUNC(0x021B0244, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0244, daKytag03_IsDelete);

/* 021B024C: HD: dComIfG_resDelete (GameCube resDeleteDemo) */
static BOOL daKytag03_Delete(kytag03_class* i_this) {
    WWHD_FUNC(0x021B024C, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x10013B3C) /* "M_door" */);
    return TRUE;
}
VERIFY(0x021B024C, daKytag03_Delete);

/* 021B027C */
static cPhs_State daKytag03_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021B027C, cPhs_State, i_ac);
    kytag03_class* i_this = (kytag03_class*)i_ac;
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */

    /* fopAcM_ct_Retail(i_ac, kytag03_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = KYTAG03_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, STR(0x10013B44) /* "M_door" */);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, 0x021AFD48 /* useHeapInit */, 0x4c30)) {
            return cPhs_ERROR_e;
        }

        i_this->field_0x2a0 = 0;
        i_this->field_0x2a8 = 0.0f;
        i_this->mSwitchNo = fopAcM_GetParam(i_ac) & 0xFF;
        i_this->mbRoomActive = false;
        i_this->mbIsActive = false;
        i_this->mbVisible = false;
    }
    return ret;
}
VERIFY(0x021B027C, daKytag03_Create);

/* 021B0364: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag03_cpp() {
    WWHD_FUNC(0x021B0364, void, (u32)0);
    sinit_header_statics(0x10464D1C, 0x101B8C24);
}
VERIFY(0x021B0364, __sinit_d_a_kytag03_cpp);

/* 021B03F8: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013B04 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x021B03F8, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x021B03F8, SafeString_dt);

/* 021B040C: kytag03_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag03_class_dt(kytag03_class* i_this, s32 flags) {
    WWHD_FUNC(0x021B040C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B040C, kytag03_class_dt);

/* 021B0460: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x021B0460, void, (u32)0);
}
VERIFY(0x021B0460, SafeString_assureTermination);
