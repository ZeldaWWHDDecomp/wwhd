/* Contrast environment tag and event door model. Private WWHD reconstruction. */
#include "d/actor/d_a_kytag03.h"
const gc_tu_data gc_tu = { .safeString = 0x10013B04 };
enum { dRes_INDEX_M_DOOR_BDL_MYAMIF_e = 3, dEvtFlag_STAFF_ALL_e = 2 };
static s8 l_stayNo;
static inline s8 kytag03_getStayNo() { return l_stayNo; }
/* 021AFD48 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    kytag03_class* i_this = (kytag03_class*)i_ac;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10013B30) /* "M_door" */, dRes_INDEX_M_DOOR_BDL_MYAMIF_e);
    i_this->mpModel = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, J3DFrameCtrl::EMode_LOOP,
                                             0.0f, 0, -1, 1, nullptr, 0x0, 0x11020203);

    if (i_this->mpModel == nullptr || i_this->mpModel->getModel() == nullptr)
        return FALSE;

    return TRUE;
}

/* 021AFE18 */
static BOOL daKytag03_Draw(kytag03_class* i_this) {
    fopAc_ac_c* actor = i_this;
    J3DModel* model = i_this->mpModel->getModel();
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    g_env_light.setLightTevColorType(model, &actor->tevStr);
    if (i_this->mbVisible)
        i_this->mpModel->updateDL();
    return TRUE;
}

/* draw_SUB (inlined into Execute) */
static inline void draw_SUB(kytag03_class* i_this) {
    fopAc_ac_c* actor = i_this;
    J3DModel* model = i_this->mpModel->getModel();
    // HD: this binding preserves the HD component load/store order.
    J3DModel_setBaseScale(model, &actor->scale);
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
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
    fopAc_ac_c* actor = i_this;
    if (dComIfGp_event_runCheck() == FALSE || !kytag03_chkEventFlag(dEvtFlag_STAFF_ALL_e)) {
        if (actor->tevStr.mRoomNo == kytag03_getStayNo()) {
            i_this->mbRoomActive = true;
            if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, kytag03_getStayNo())) {
                contrast_on(i_this);
            } else {
                contrast_off(i_this);
            }
        }
    } else if (actor->tevStr.mRoomNo != kytag03_getStayNo()) {
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

/* 021B0244 */
static BOOL daKytag03_IsDelete(kytag03_class* i_this) {
    return TRUE;
}

/* 021B024C */
// HD: HD: dComIfG_resDelete (GameCube resDeleteDemo)
static BOOL daKytag03_Delete(kytag03_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, STR(0x10013B3C) /* "M_door" */);
    return TRUE;
}

/* 021B027C */
static cPhs_State daKytag03_Create(fopAc_ac_c* i_ac) {
    kytag03_class* i_this = (kytag03_class*)i_ac;
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */

    fopAcM_ct(i_this, kytag03_class);

    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, STR(0x10013B44) /* "M_door" */);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x4c30)) {
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

/* 021B0364 */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_kytag03_cpp() {
    sinit_header_statics(0x10464D1C, 0x101B8C24);
}

/* 021B03F8 */
// HD: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013B04 +0xC)
static void SafeString_dt(SafeString* s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 021B040C */
// HD: kytag03_class deleting destructor (compiler-generated, HD virtual destructor)
static void kytag03_class_dt(kytag03_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        kytag03_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021B0460 */
// HD: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy)
static void SafeString_assureTermination(SafeString*) {
}

kytag03_class::kytag03_class() {
    // HD: actor base construction and this unit's virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue (tools/verify/autoglue.py): private HD bindings and room static
data l_stayNo 1047E6C8
ext 0255FD48 void dKy_change_colpat(u8)
ext 02560704 BOOL dKy_contrast_flg_get()
ext 025606D4 void dKy_contrast_flg_set(bool)
ext 025D50BC void kytag03_actor_dt(void*, s32)
*/
