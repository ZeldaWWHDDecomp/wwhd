/**
 * d_a_tag_etc.cpp (WWHD)
 * Tag - Medli (Md1) flying camera / event trigger area.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_etc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_etc.h"

// HD: inline construction installs this actor's virtual table.
daTag_Etc_c::daTag_Etc_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1003EFC4;
}

/* 024A6394 */
u8 daTag_Etc_c::getEventNo() {
    return fopAcM_GetParam(this) >> 24;
}

/* 024A5D90 */
u8 daTag_Etc_c::getType2() {
    return fopAcM_GetParam(this) >> 8 & 0xF;
}

/* 024A6030 */
BOOL daTag_Etc_c::rangeCheck(fopAc_ac_c* pActor) {
    gabi::Local<cXyz> delta;
    cXyz_mi(&pActor->current.pos, delta, &current.pos);
    f32 dx = delta->x, dy = delta->y, dz = delta->z;
    if (dy < 0.0f)
        return FALSE;

    /* delta.absXZ() */
    gabi::Local<cXyz> xz;
    xz->x = dx;
    xz->y = 0.0f;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    // HD: retain the distinct NaN behavior of the two range comparisons.
    if (!(dist < scale.x * 100.0f))
        return FALSE;
    if (dy > scale.y * 100.0f)
        return FALSE;
    return TRUE;
}

/* 024A6104 */
BOOL daTag_Etc_c::otherCheck(fopAc_ac_c* pActor) {
    BOOL result;

    switch (getType2()) {
    case 0:
        if (pActor != NULL && static_cast<TagEtcMedli*>(pActor)->checkStatusFly()) {
            result = TRUE;
        } else {
            result = FALSE;
        }
        break;
    default:
        result = TRUE;
        break;
    }
    return result;
}

/* 024A5E0C */
void daTag_Etc_c::demoProc() {
    fopAc_ac_c* pMedli = fopAcM_SearchByID(mMedliPID);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("TAG_ETC_D", NULL, 0);

    if (staffIdx != -1) {
        switch (getType2()) {
        case 0:
            if (pMedli == NULL || !static_cast<TagEtcMedli*>(pMedli)->checkStatusFly()) {
                if (field_0x29A > 0) {
                    field_0x29A = field_0x29A - 1;
                } else {
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
            }
            break;
        default:
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
}

/* 024A5FA4 */
void daTag_Etc_c::demoInitProc() {
    fopAc_ac_c* pActor;

    switch (getType2()) {
    case 0:
        pActor = fopAcM_SearchByID(mMedliPID);
        TagEtc_setItemPartner(pActor);
        field_0x29A = 15;
        break;
    }
}

/* 024A63A0 */
cPhs_State daTag_Etc_c::create() {
    fopAcM_ct(this, daTag_Etc_c);

    u8 stageEVNTListIndex = getEventNo();
    mEventIdx = dComIfGp_evmng_getEventIdx(NULL, stageEVNTListIndex);
    if (mEventIdx == -1) {
        setActio(ACT_WAIT);
    } else {
        switch (getType2()) {
        case 0:
            setActio(ACT_SEARCH);
            break;
        default:
            setActio(ACT_WAIT);
            break;
        }
    }

    shape_angle.z = 0;
    shape_angle.x = 0;
    current.angle.z = 0;
    current.angle.x = 0;
    attention_info.flags = fopAc_Attn_ACTION_SPEAK_e;
    attention_info.position.y = attention_info.position.y + 150.0f;
    eyePos.y = eyePos.y + 150.0f;
    return cPhs_COMPLEATE_e;
}

/* 024A5D88 */
static BOOL daTag_Etc_action_wait(daTag_Etc_c* i_this) {
    return TRUE;
}

/* 024A5D9C */
static BOOL daTag_Etc_action_search(daTag_Etc_c* i_this) {
    fopAc_ac_c* actor = NULL;
    switch (i_this->getType2()) {
    case 0:
        actor = TagEtc_searchFromName("Md1", 0, 0);
        break;
    }

    if (actor != NULL) {
        i_this->mMedliPID = fopAcM_GetID(actor);
        i_this->setActio(ACT_HUNT);
    }

    return TRUE;
}

/* 024A5F24 */
static BOOL daTag_Etc_action_event(daTag_Etc_c* i_this) {
    if (dComIfGp_evmng_endCheck(i_this->mEventIdx)) {
        i_this->mMedliPID = fpcM_ERROR_PROCESS_ID_e;
        i_this->setActio(ACT_SEARCH);
        dComIfGp_event_reset();
    } else {
        i_this->demoProc();
    }

    return TRUE;
}

/* 024A6158 */
static BOOL daTag_Etc_action_ready(daTag_Etc_c* i_this) {
    fopAc_ac_c* actor = fopAcM_SearchByID(i_this->mMedliPID);
    if (i_this->eventInfo.checkCommandDemoAccrpt()) {
        i_this->demoInitProc();
        i_this->setActio(ACT_EVENT);
        daTag_Etc_action_event(i_this);
    } else if (actor == NULL || !i_this->rangeCheck(actor) || !i_this->otherCheck(actor)) {
        i_this->mMedliPID = fpcM_ERROR_PROCESS_ID_e;
        i_this->setActio(ACT_SEARCH);
    } else {
        fopAcM_orderOtherEventId(i_this, i_this->mEventIdx, 0xFF, 0xFFFF, 0, 1);
    }

    return TRUE;
}

/* 024A6244 */
static BOOL daTag_Etc_action_hunt(daTag_Etc_c* i_this) {
    fopAc_ac_c* actor = fopAcM_SearchByID(i_this->mMedliPID);
    if (actor == NULL) {
        i_this->mMedliPID = fpcM_ERROR_PROCESS_ID_e;
        i_this->setActio(ACT_SEARCH);
        return TRUE;
    }

    auto* medli = static_cast<TagEtcMedli*>(actor);
    if (i_this->rangeCheck(actor) && i_this->otherCheck(actor)) {
        medli->onBitCamTagIn();
    } else {
        medli->offBitCamTagIn();
    }

    return TRUE;
}

/* 024A6568 */
static BOOL daTag_Etc_Draw(daTag_Etc_c* i_this) {
    return TRUE;
}

// HD: the action table is copied on first use into its original BSS storage.
static u32 l_action_guard;
static u32 l_action[5] = {0, 0, 0, 0, 0};
// HD: checked guest function addresses preserve the original free-function dispatch.
static const u32 l_action_init[] = {0x024A5D88, 0x024A5D9C, 0x024A6244, 0x024A6158, 0x024A5F24};

/* 024A6304 */
static BOOL daTag_Etc_Execute(daTag_Etc_c* i_this) {
    if (l_action_guard == 0) {
        l_action_guard = 1;
        memcpy_g(l_action, l_action_init, sizeof(l_action_init));
    }
    gabi::call_ptr<BOOL>(l_action[i_this->mAction], i_this);
    return TRUE;
}

/* 024A6384 */
static BOOL daTag_Etc_IsDelete(daTag_Etc_c* i_this) {
    return TRUE;
}

/* 024A638C */
static BOOL daTag_Etc_Delete(daTag_Etc_c* i_this) {
    return TRUE;
}

/* 024A64D0 */
static cPhs_State daTag_Etc_Create(fopAc_ac_c* i_this) {
    return static_cast<daTag_Etc_c*>(i_this)->create();
}

/* 024A64D4 */
static void __sinit_d_a_tag_etc_cpp() {
    sinit_header_statics(0x1046E264, 0x101D1884);
}

/* 024A6570 */
static void daTag_Etc_c_dt(daTag_Etc_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025D9F38 fopAc_ac_c* TagEtc_searchFromName(const char*, u32, u32)
ext 0253F124 u32 TagEtc_getPId(dEvt_control_c*, fopAc_ac_c*)
str 1003EFAC "TAG_ETC_D"
str 1003EFA8 "Md1"
data l_action_guard 101FDCA0
data l_action 101FDCA4 nocheck # HD: BSS destination initialized from the checked source table before dispatch.
data l_action_init 101D1870
*/
