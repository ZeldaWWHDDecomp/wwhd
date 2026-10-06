/**
 * d_a_tag_etc.cpp (WWHD)
 * Tag - Medli (Md1) flying camera / event trigger area.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_etc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define TAG_ETC_VTBL 0x1003EFC4

enum Action {
    ACT_WAIT,
    ACT_SEARCH,
    ACT_HUNT,
    ACT_READY,
    ACT_EVENT,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) (as in d_a_npc_photo.h) */
static inline fopAc_ac_c* tag_etc_searchFromName(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
/* dComIfGp_event_setItemPartner(actor): dEvt_control_c (play+0x51D0) mPtItem (+0xD0) = getPId(actor);
 * 0253F124 dEvt_control_c::getPId */
static inline void dComIfGp_event_setItemPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xD0, gabi::call<u32>(0x0253F124, evt, a));
}
/* daNpc_Md_c::m30F0 (HD +0x420C): checkStatusFly() bit 0x10, onBitCamTagIn/offBitCamTagIn bit 0x20 */
static inline bool md_checkStatusFly(fopAc_ac_c* md) { return (gabi::load<u32>(gabi::ea(md) + 0x420C) & 0x10) != 0; }

struct daTag_Etc_c : fopAc_ac_c {
    void setActio(u8 action) { mAction = action; }

    u8 getEventNo();
    u8 getType2();
    BOOL rangeCheck(fopAc_ac_c*);
    BOOL otherCheck(fopAc_ac_c*);
    void demoProc();
    void demoInitProc();
    cPhs_State create();

    /* 0x3AC */ be<u8> mAction;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<u32> mMedliPID;
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ be<s16> field_0x29A;
};
WWHD_OFFSET(daTag_Etc_c, mMedliPID, 0x3B0);
WWHD_OFFSET(daTag_Etc_c, field_0x29A, 0x3B6);
WWHD_SIZE(daTag_Etc_c, 0x3B8);

/* 024A6394 */
u8 daTag_Etc_c::getEventNo() {
    WWHD_FUNC(0x024A6394, u8, this);
    return fopAcM_GetParam(this) >> 24;
}
VERIFY(0x024A6394, &daTag_Etc_c::getEventNo);

/* 024A5D90 */
u8 daTag_Etc_c::getType2() {
    WWHD_FUNC(0x024A5D90, u8, this);
    return fopAcM_GetParam(this) >> 8 & 0xF;
}
VERIFY(0x024A5D90, &daTag_Etc_c::getType2);

/* 024A6030 */
BOOL daTag_Etc_c::rangeCheck(fopAc_ac_c* pActor) {
    WWHD_FUNC(0x024A6030, BOOL, this, pActor);
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
    /* GHS: bge (not less, NaN included) / bgt */
    if (!(dist < scale.x * 100.0f))
        return FALSE;
    if (dy > scale.y * 100.0f)
        return FALSE;
    return TRUE;
}
VERIFY(0x024A6030, &daTag_Etc_c::rangeCheck);

/* 024A6104 */
BOOL daTag_Etc_c::otherCheck(fopAc_ac_c* pActor) {
    WWHD_FUNC(0x024A6104, BOOL, this, pActor);
    BOOL result;

    switch (getType2()) {
    case 0:
        if (pActor != NULL && md_checkStatusFly(pActor)) {
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
VERIFY(0x024A6104, &daTag_Etc_c::otherCheck);

/* 024A5E0C */
void daTag_Etc_c::demoProc() {
    WWHD_FUNC(0x024A5E0C, void, this);
    fopAc_ac_c* pMedli = fopAcM_SearchByID(mMedliPID);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1003EFAC) /* "TAG_ETC_D" */, NULL, 0);

    if (staffIdx != -1) {
        switch (getType2()) {
        case 0:
            if (pMedli == NULL || !md_checkStatusFly(pMedli)) {
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
VERIFY(0x024A5E0C, &daTag_Etc_c::demoProc);

/* 024A5FA4 */
void daTag_Etc_c::demoInitProc() {
    WWHD_FUNC(0x024A5FA4, void, this);
    fopAc_ac_c* pActor;

    switch (getType2()) {
    case 0:
        pActor = fopAcM_SearchByID(mMedliPID);
        dComIfGp_event_setItemPartner(pActor);
        field_0x29A = 15;
        break;
    }
}
VERIFY(0x024A5FA4, &daTag_Etc_c::demoInitProc);

/* 024A63A0 */
cPhs_State daTag_Etc_c::create() {
    WWHD_FUNC(0x024A63A0, cPhs_State, this);
    /* fopAcM_ct(this, daTag_Etc_c): HD fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TAG_ETC_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

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
    gabi::store<u32>(gabi::ea(this) + 0x39C, 8); /* attention_info.flags = fopAc_Attn_ACTION_SPEAK_e */
    u32 attnY = gabi::ea(this) + 0x394;          /* attention_info.position.y */
    gabi::store<f32>(attnY, gabi::load<f32>(attnY) + 150.0f);
    eyePos.y = eyePos.y + 150.0f;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A63A0, &daTag_Etc_c::create);

/* 024A5D88 */
static BOOL daTag_Etc_action_wait(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A5D88, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A5D88, daTag_Etc_action_wait);

/* 024A5D9C */
static BOOL daTag_Etc_action_search(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A5D9C, BOOL, i_this);
    fopAc_ac_c* actor = NULL;
    switch (i_this->getType2()) {
    case 0:
        actor = tag_etc_searchFromName(STR(0x1003EFA8) /* "Md1" */, 0, 0);
        break;
    }

    if (actor != NULL) {
        i_this->mMedliPID = fopAcM_GetID(actor);
        i_this->setActio(ACT_HUNT);
    }

    return TRUE;
}
VERIFY(0x024A5D9C, daTag_Etc_action_search);

/* 024A5F24 */
static BOOL daTag_Etc_action_event(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A5F24, BOOL, i_this);
    if (dComIfGp_evmng_endCheck(i_this->mEventIdx)) {
        i_this->mMedliPID = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        i_this->setActio(ACT_SEARCH);
        dComIfGp_event_reset();
    } else {
        i_this->demoProc();
    }

    return TRUE;
}
VERIFY(0x024A5F24, daTag_Etc_action_event);

/* 024A6158 */
static BOOL daTag_Etc_action_ready(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A6158, BOOL, i_this);
    fopAc_ac_c* actor = fopAcM_SearchByID(i_this->mMedliPID);
    if (eventInfo_checkCommandDemoAccrpt(i_this)) {
        i_this->demoInitProc();
        i_this->setActio(ACT_EVENT);
        daTag_Etc_action_event(i_this);
    } else if (actor == NULL || !i_this->rangeCheck(actor) || !i_this->otherCheck(actor)) {
        i_this->mMedliPID = 0xFFFFFFFF;
        i_this->setActio(ACT_SEARCH);
    } else {
        fopAcM_orderOtherEventId(i_this, i_this->mEventIdx, 0xFF, 0xFFFF, 0, 1);
    }

    return TRUE;
}
VERIFY(0x024A6158, daTag_Etc_action_ready);

/* 024A6244 */
static BOOL daTag_Etc_action_hunt(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A6244, BOOL, i_this);
    fopAc_ac_c* actor = fopAcM_SearchByID(i_this->mMedliPID);
    if (actor == NULL) {
        i_this->mMedliPID = 0xFFFFFFFF;
        i_this->setActio(ACT_SEARCH);
        return TRUE;
    }

    u32 bits = gabi::ea(actor) + 0x420C; /* daNpc_Md_c::m30F0 */
    if (i_this->rangeCheck(actor) && i_this->otherCheck(actor)) {
        gabi::store<u32>(bits, gabi::load<u32>(bits) | 0x20); /* onBitCamTagIn */
    } else {
        gabi::store<u32>(bits, gabi::load<u32>(bits) & ~0x20u); /* offBitCamTagIn */
    }

    return TRUE;
}
VERIFY(0x024A6244, daTag_Etc_action_hunt);

/* 024A6568: daTag_Etc_c::draw() inlined */
static BOOL daTag_Etc_Draw(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A6568, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A6568, daTag_Etc_Draw);

/* daTag_Etc_c::execute() inlined.
 * HD: the function-local l_action table is initialised on first use (guard 0x101FDCA0, table
 * 0x101FDCA4, copied from .data 0x101D1870). */
static be<u32>& l_action_guard() { return *gabi::at<be<u32>>(0x101FDCA0); }
static gfn* l_action() { return gabi::at<gfn>(0x101FDCA4); }

/* 024A6304 */
static BOOL daTag_Etc_Execute(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A6304, BOOL, i_this);
    if (l_action_guard() == 0) {
        l_action_guard() = 1;
        memcpy_g(l_action(), gabi::at<void>(0x101D1870), 0x14);
    }
    gabi::call_ptr<BOOL>(l_action()[i_this->mAction].get(), i_this);
    return TRUE;
}
VERIFY(0x024A6304, daTag_Etc_Execute);

/* 024A6384 */
static BOOL daTag_Etc_IsDelete(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A6384, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A6384, daTag_Etc_IsDelete);

/* 024A638C. HD: the destructor call (~daTag_Etc_c(), nothing to destroy) is gone */
static BOOL daTag_Etc_Delete(daTag_Etc_c* i_this) {
    WWHD_FUNC(0x024A638C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A638C, daTag_Etc_Delete);

/* 024A64D0 */
static cPhs_State daTag_Etc_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A64D0, cPhs_State, i_this);
    return static_cast<daTag_Etc_c*>(i_this)->create();
}
VERIFY(0x024A64D0, daTag_Etc_Create);

/* 024A64D4: __sinit_d_a_tag_etc_cpp (compiler-generated: header statics only) */
static void __sinit_d_a_tag_etc_cpp() {
    WWHD_FUNC(0x024A64D4, void);
    sinit_header_statics(0x1046E264, 0x101D1884);
}
VERIFY(0x024A64D4, __sinit_d_a_tag_etc_cpp);

/* 024A6570: daTag_Etc_c deleting destructor (vtable 0x1003EFC4 slot 3) */
static void daTag_Etc_c_dt(daTag_Etc_c* p, s32 flags) {
    WWHD_FUNC(0x024A6570, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x024A6570, daTag_Etc_c_dt);
