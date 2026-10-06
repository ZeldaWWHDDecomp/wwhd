/**
 * d_a_npc_zl1.cpp (WWHD)
 * NPC - Tetra
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * The largest functions are in d_a_npc_zl1_<part>.cpp (separate verification units).
 */
#include "d/actor/d_a_npc_zl1.h"

/* 023035B8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023035B8, BOOL, i_this);
    return static_cast<daNpc_Zl1_c*>(i_this)->CreateHeap();
}
VERIFY(0x023035B8, CheckCreateHeap);

/* 023053E8 */
static cPhs_State daNpc_Zl1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023053E8, cPhs_State, i_this);
    return ((daNpc_Zl1_c*)i_this)->_create();
}
VERIFY(0x023053E8, daNpc_Zl1_Create);

/* 023053EC */
BOOL daNpc_Zl1_c::_delete() {
    WWHD_FUNC(0x023053EC, BOOL, this);
    /* HD: dComIfG_resDelete (GameCube: dComIfG_resDeleteDemo) */
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    dKy_plight_cut(&mLightInfluence1);
    gabi::call(0x0255BA9C, &mLightInfluence2); /* dKy_efplight_cut */
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)mRippleCallBack);
    return TRUE;
}
VERIFY(0x023053EC, &daNpc_Zl1_c::_delete);

/* 02305458 */
static BOOL daNpc_Zl1_Delete(daNpc_Zl1_c* i_this) {
    WWHD_FUNC(0x02305458, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x02305458, daNpc_Zl1_Delete);

/* 0230808C */
static BOOL daNpc_Zl1_Execute(daNpc_Zl1_c* i_this) {
    WWHD_FUNC(0x0230808C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0230808C, daNpc_Zl1_Execute);

/* 023086F0 */
static BOOL daNpc_Zl1_Draw(daNpc_Zl1_c* i_this) {
    WWHD_FUNC(0x023086F0, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x023086F0, daNpc_Zl1_Draw);

/* 023086F4 */
static BOOL daNpc_Zl1_IsDelete(daNpc_Zl1_c*) {
    WWHD_FUNC(0x023086F4, BOOL, (daNpc_Zl1_c*)nullptr);
    return TRUE;
}
VERIFY(0x023086F4, daNpc_Zl1_IsDelete);

/* 02304C44 */
void daNpc_Zl1_c::setAttention(u32 param_1) {
    WWHD_FUNC(0x02304C44, void, this, param_1);
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + l_HIO().mPrmTbl.field_1C;
    attPos->x = x;
    attPos->y = y;
    attPos->z = current.pos.z;
    if (field_0x79C == 0 && !param_1) {
        return;
    }
    eyePos.z = field_0x74C.z;
    eyePos.y = field_0x74C.y;
    eyePos.x = field_0x74C.x;
}
VERIFY(0x02304C44, &daNpc_Zl1_c::setAttention);

/* 023069E8 */
void daNpc_Zl1_c::init_Light() {
    WWHD_FUNC(0x023069E8, void, this);
    /* HD: other colours and powers (GameCube: 0x5C/0xAE/0xFF 139.7, 0xBA/0xFF/0xFF 270.7) */
    mLightInfluence2.mColorB = 0xE9;
    mLightInfluence2.mPower = 60.0f;
    mLightInfluence1.mColorR = 0;
    mLightInfluence1.mColorG = 0xBC;
    mLightInfluence2.mFluctuation = 93.8f;
    mLightInfluence1.mFluctuation = 93.8f;
    mLightInfluence1.mColorB = 0x49;
    mLightInfluence2.mColorR = 0;
    mLightInfluence2.mColorG = 0x2F;
    mLightInfluence1.mPower = 45.0f;
}
VERIFY(0x023069E8, &daNpc_Zl1_c::init_Light);

/* 023079F8 */
void daNpc_Zl1_c::incEnvironment() {
    WWHD_FUNC(0x023079F8, void, this);
    f32 r = mRatio + 0.05f;
    r = (r - 1.0f >= 0.0f) ? 1.0f : r; /* cLib_maxLimit (fsel) */
    mRatio = r;
    if (!(r < 1.0f)) {
        field_0x7C2 = 0;
    }
}
VERIFY(0x023079F8, &daNpc_Zl1_c::incEnvironment);

/* 02307A30 */
void daNpc_Zl1_c::decEnvironment() {
    WWHD_FUNC(0x02307A30, void, this);
    f32 r = mRatio - 0.05f;
    mRatio = (r - 0.15f >= 0.0f) ? r : 0.15f; /* cLib_minLimit (fsel) */
}
VERIFY(0x02307A30, &daNpc_Zl1_c::decEnvironment);

/* 0230683C get_prmFloat (not named by the matcher) */
f32 daNpc_Zl1_c::get_prmFloat(be<f32>* param_1, f32 param_2) {
    WWHD_FUNC(0x0230683C, f32, this, param_1, param_2);
    if (param_1 == nullptr) {
        return param_2;
    }
    return *param_1;
}
VERIFY(0x0230683C, &daNpc_Zl1_c::get_prmFloat);

/* 02306D34 cut_move_OKIRU (not named by the matcher) */
u32 daNpc_Zl1_c::cut_move_OKIRU() {
    WWHD_FUNC(0x02306D34, u32, this);
    return field_0x7C3 != 0 ? TRUE : FALSE;
}
VERIFY(0x02306D34, &daNpc_Zl1_c::cut_move_OKIRU);

/* 023066B4 */
void daNpc_Zl1_c::cut_init_OKIRU(int) {
    WWHD_FUNC(0x023066B4, void, this, 0);
    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0x98, 1.0f); /* mpMorf->setPlaySpeed(1.0f) */
    field_0x7C3 = 0;
}
VERIFY(0x023066B4, &daNpc_Zl1_c::cut_init_OKIRU);

/* 02306740 */
void daNpc_Zl1_c::cut_init_OKIRU_2(int) {
    WWHD_FUNC(0x02306740, void, this, 0);
    setAnm_NUM(0xE, 1);
}
VERIFY(0x02306740, &daNpc_Zl1_c::cut_init_OKIRU_2);

/* 0230545C */
u8 daNpc_Zl1_c::partner_search_sub(u32 i_judgeFunc) {
    WWHD_FUNC(0x0230545C, u8, this, i_judgeFunc);
    bool ret = false;
    mProcId1 = 0xFFFFFFFF;
    l_check_wrk() = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf()[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    if (l_check_wrk() != 0) {
        mProcId1 = fopAcM_GetID(l_check_inf()[0].get());
        ret = true;
    }
    return ret;
}
VERIFY(0x0230545C, &daNpc_Zl1_c::partner_search_sub);

/* 02305508 */
void daNpc_Zl1_c::partner_search() {
    WWHD_FUNC(0x02305508, void, this);
    bool temp;
    if (field_0x850 == 1) {
        switch ((u32)(s32)field_0x84F) {
        case 1:
            temp = partner_search_sub(0x023035BC /* searchActor_Branch */);
            break;
        case 2:
            temp = partner_search_sub(0x0230363C /* searchActor_Bm1 */);
            break;
        default:
            temp = true;
            break;
        }
        if (temp) {
            field_0x850 = field_0x850 + 1;
        }
    }
}
VERIFY(0x02305508, &daNpc_Zl1_c::partner_search);

/* 0230634C searchByID (the matcher calls it cLib_getRndValue<int> of d_a_npc_aj1) */
static fopAc_ac_c* daNpc_Zl1_searchByID(daNpc_Zl1_c* i_this, fpc_ProcID i_pid, be<s32>* o_wasDeleted) {
    WWHD_FUNC(0x0230634C, fopAc_ac_c*, i_this, i_pid, o_wasDeleted);
    gabi::Local<gptr<fopAc_ac_c>> actor;
    *actor = nullptr;
    *o_wasDeleted = FALSE;
    if (!gabi::call<BOOL>(0x025D54C4, i_pid, actor.get())) { /* fopAcM_SearchByID(id, &actor) */
        *o_wasDeleted = TRUE;
    }
    return *actor;
}
VERIFY(0x0230634C, daNpc_Zl1_searchByID);

/* 023037F4 */
u8 daNpc_Zl1_c::decideType(int param_1) {
    WWHD_FUNC(0x023037F4, u8, this, param_1);
    if (field_0x84E > 0) {
        return true;
    }
    field_0x84E = 1;
    /* HD: the switch is folded into a range check */
    if ((u32)param_1 > 7) {
        field_0x84F = -1;
        return false;
    }
    field_0x84F = (s8)param_1;
    switch ((u32)(s32)field_0x84F) {
    case 0:
    case 6:
    case 7:
        /* strcpy(mArcName, "Zl2") (4 bytes from .rodata 0x10023EE0) */
        for (int i = 0; i < 4; i++) gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x10023EE0 + i));
        break;
    default:
        /* strcpy(mArcName, "Zl") */
        for (int i = 0; i < 3; i++) gabi::store<u8>(gabi::ea(mArcName) + i, gabi::load<u8>(0x10023EE4 + i));
        break;
    }
    bool ret = false;
    if (field_0x84E != -1 && field_0x84F != -1) {
        ret = true;
    }
    return ret;
}
VERIFY(0x023037F4, &daNpc_Zl1_c::decideType);

/* 023038B8 */
BOOL daNpc_Zl1_c::set_action(ProcFunc_l* i_action, void* param_2) {
    WWHD_FUNC(0x023038B8, BOOL, this, i_action, param_2);
    ProcFunc_l* cur = &mCurrActionFunc;
    s16 newI = i_action->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_action->d;
        newF = i_action->f;
        if (!((u16)cur->d != (u16)newD || cur->f != newF))
            return TRUE;
    } else {
        newF = i_action->f;
        newD = i_action->d;
        if (cur->i == 0)
            goto set;
    }
    field_0x850 = 9;
    pmf_call(this, cur, param_2);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x850 = 0;
    pmf_call(this, cur, param_2);
    return TRUE;
}
VERIFY(0x023038B8, &daNpc_Zl1_c::set_action);

enum : u32 { PMF_demo_action1 = 0x10023AD8, PMF_wait_action1 = 0x10023AE0, PMF_demo_action2 = 0x10023AE8, PMF_optn_action1 = 0x10023AF0 };
enum {
    dSv_event_flag_UNK_0001 = 0x0001,
    dSv_event_flag_UNK_0101 = 0x0101,
    dSv_event_flag_UNK_0E20 = 0x0E20,
    dSv_event_flag_UNK_2401 = 0x2401,
    dSv_event_flag_UNK_0801 = 0x0801,
};

/* 023039E4 */
bool daNpc_Zl1_c::init_ZL1_0() {
    WWHD_FUNC(0x023039E4, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_demo_action1);
        set_action(pmf, nullptr);
        field_0x7D2 = true;
        actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
        return true;
    }
    return false;
}
VERIFY(0x023039E4, &daNpc_Zl1_c::init_ZL1_0);

/* 02303A7C */
bool daNpc_Zl1_c::init_ZL1_1() {
    WWHD_FUNC(0x02303A7C, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0101)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_demo_action1);
        set_action(pmf, nullptr);
        field_0x7CE = true;
        actor_status &= ~0x180u; /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        return true;
    }
    return false;
}
VERIFY(0x02303A7C, &daNpc_Zl1_c::init_ZL1_1);

/* 02303B14 */
bool daNpc_Zl1_c::init_ZL1_2() {
    WWHD_FUNC(0x02303B14, bool, this);
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20) && !dComIfGs_isEventBit(dSv_event_flag_UNK_2401)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_wait_action1);
        set_action(pmf, nullptr);
        return true;
    }
    return false;
}
VERIFY(0x02303B14, &daNpc_Zl1_c::init_ZL1_2);

/* 02303BBC */
bool daNpc_Zl1_c::init_ZL1_3() {
    WWHD_FUNC(0x02303BBC, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0801)) {
        gabi::Local<cXyz> temp;
        temp->x = -123.0f;
        temp->y = 0.0f;
        temp->z = -100.0f;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), temp, &field_0x764);
        field_0x764.y = 400.0f;
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_demo_action2);
        set_action(pmf, nullptr);
        actor_status &= ~0x180u;
        return true;
    }
    return false;
}
VERIFY(0x02303BBC, &daNpc_Zl1_c::init_ZL1_3);

/* 02303CBC */
bool daNpc_Zl1_c::init_ZL1_4() {
    WWHD_FUNC(0x02303CBC, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_wait_action1);
    set_action(pmf, nullptr);
    actor_status &= ~0x180u;
    return true;
}
VERIFY(0x02303CBC, &daNpc_Zl1_c::init_ZL1_4);

/* 02303D14 */
BOOL daNpc_Zl1_c::set_startPos(int param_1) {
    WWHD_FUNC(0x02303D14, BOOL, this, param_1);
    /* a_chk_playerPos (.rodata 0x10023F14), a_set_tetoraPos (0x10023F5C), a_set_tetoraAng (0x10023EFC) */
    int temp2 = 0; /* HD: initialised; no temp2 < 3 check */
    f32 temp = 1000000000.0f; /* G_CM3D_F_INF */
    if (param_1 == 0 || param_1 == 1) {
        for (int i = 0; i < 3; i++) {
            gabi::Local<cXyz> chkPlayerPos;
            u32 src = 0x10023F14 + param_1 * 0x24 + i * 0xC;
            f32 x = gabi::load<f32>(src), y = gabi::load<f32>(src + 4);
            chkPlayerPos->x = x;
            chkPlayerPos->y = y;
            chkPlayerPos->z = gabi::load<f32>(src + 8);
            gabi::Local<cXyz> diff;
            cXyz_mi(chkPlayerPos, diff, &dComIfGp_getPlayer(0)->current.pos);
            gabi::Local<cXyz> xz;
            f32 dx = diff->x, dz = diff->z;
            xz->x = dx;
            xz->y = 0.0f;
            xz->z = dz;
            f32 absDiff = std_sqrtf(PSVECSquareMag(xz));
            if (temp > absDiff) {
                temp = absDiff;
                temp2 = i;
            }
        }
        int idx = param_1 * 3 + temp2;
        u32 pos = 0x10023F5C + idx * 0xC;
        current.pos.x = gabi::load<f32>(pos);
        current.pos.y = gabi::load<f32>(pos + 4);
        current.pos.z = gabi::load<f32>(pos + 8);
        current.angle.y = (s16)gabi::ftoi(gabi::load<f32>(0x10023EFC + idx * 4) * 182.04445f); /* cM_deg2s */
        /* mObjAcch.SetOld(): *pOld = *pPos (bit copy) */
        u32 oldp = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x30);
        u32 posp = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x2C);
        for (int k = 0; k < 12; k += 4) gabi::store<u32>(oldp + k, gabi::load<u32>(posp + k));
    }
    return TRUE;
}
VERIFY(0x02303D14, &daNpc_Zl1_c::set_startPos);

/* 02303E98 */
static inline void SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: inline sead::SafeString compare with the
 * start stage name at play+0x5134 (the virtual at vtable +0x14 twice on the literal, once on the
 * stage name, then at most 0x40001 bytes) */
static bool zl1_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(name);
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = stage;
    SafeString_vcall(a);
    SafeString_vcall(a);
    u32 pa = a->mStringTop;
    SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
enum { dSv_event_flag_ZELDA_AWAKENED = 0x2D02, dSv_event_flag_HYRULE_COURTYARD_CUTSCENE = 0x3804 };
bool daNpc_Zl1_c::init_ZL1_5() {
    WWHD_FUNC(0x02303E98, bool, this);
    if (!dComIfGs_isEventBit(dSv_event_flag_ZELDA_AWAKENED)) {
        gabi::Local<ProcFunc_l> pmf;
        pmf_load(pmf, PMF_optn_action1);
        set_action(pmf, nullptr);
        actor_status &= ~0x180u; /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        if (!dComIfGs_isEventBit(dSv_event_flag_HYRULE_COURTYARD_CUTSCENE)) {
            actor_status |= 0x4000u; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            field_0x84A = 6;
            field_0x7A8 = field_0x84A + -3;
            fopAcM_orderOtherEventId(this, mEventIdx[field_0x7A8], 0xFF, 0xFFFF, 0, 1);
            gabi::call(0x0254351C, dComIfGp_ea() + 0x52E8); /* dComIfGp_evmng_cancelStartDemo(): dEvent_exception_c::init */
        } else {
            if (zl1_isStartStage(STR(0x10023FA8) /* "Hyrule" */)) {
                set_startPos(1);
            }
            if (zl1_isStartStage(STR(0x10023FB0) /* "Hyroom" */)) {
                set_startPos(0);
            }
        }
        gravity = -4.5f;
        return true;
    }
    return false;
}
VERIFY(0x02303E98, &daNpc_Zl1_c::init_ZL1_5);

/* 02304148 init_ZL1_6 (not named by the matcher) */
bool daNpc_Zl1_c::init_ZL1_6() {
    WWHD_FUNC(0x02304148, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_demo_action1);
    set_action(pmf, nullptr);
    actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
    return true;
}
VERIFY(0x02304148, &daNpc_Zl1_c::init_ZL1_6);

/* 023041A0 init_ZL1_7 (not named by the matcher) */
bool daNpc_Zl1_c::init_ZL1_7() {
    WWHD_FUNC(0x023041A0, bool, this);
    gabi::Local<ProcFunc_l> pmf;
    pmf_load(pmf, PMF_demo_action1);
    set_action(pmf, nullptr);
    actor_status &= ~0x3Fu;
    return true;
}
VERIFY(0x023041A0, &daNpc_Zl1_c::init_ZL1_7);

/* 023041F8 setEyeCtrl (not named by the matcher) */
void daNpc_Zl1_c::setEyeCtrl() {
    WWHD_FUNC(0x023041F8, void, this);
    for (int i = 0; i < 2; i++) {
        daNpc_Zl1_matAnm_c* p = field_0x6D4[i];
        if (p) {
            p->field_0x7C = 1;
        }
    }
    field_0x7D1 = true;
}
VERIFY(0x023041F8, &daNpc_Zl1_c::setEyeCtrl);

/* 02304230 clrEyeCtrl (not named by the matcher) */
void daNpc_Zl1_c::clrEyeCtrl() {
    WWHD_FUNC(0x02304230, void, this);
    for (int i = 0; i < 2; i++) {
        daNpc_Zl1_matAnm_c* p = field_0x6D4[i];
        if (p) {
            p->field_0x7C = 0;
        }
    }
    field_0x7D1 = false;
}
VERIFY(0x02304230, &daNpc_Zl1_c::clrEyeCtrl);

/* 0230503C */
bool daNpc_Zl1_c::createInit() {
    WWHD_FUNC(0x0230503C, bool, this);
    bool ret;
    int temp = 0xFF;
    /* l_evn_tbl (.data 0x101C70C0): "yuukaigo", "ooi", "majyuu_shinnyuu", "nakaniwa" */
    for (int i = 0; i < 4; i++) {
        const char* name = gabi::at<const char>(gabi::load<u32>(0x101C70C0 + i * 4));
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2(STR(0x10024018) /* "Zl1" */, this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA);  /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB);  /* attention_info.distances[TALK] */
    field_0x849 = 0x11;
    gravity = 0.0f;
    field_0x7BC = -1;
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB);  /* attention_info.distances[SPEAK] */
    switch ((u32)(s32)field_0x84F) {
    case 0:
        ret = init_ZL1_0();
        break;
    case 1:
        ret = init_ZL1_1();
        break;
    case 2:
        ret = init_ZL1_2();
        break;
    case 3:
        ret = init_ZL1_3();
        break;
    case 4:
        ret = init_ZL1_4();
        break;
    case 5:
        ret = init_ZL1_5();
        temp = 0x8C;
        break;
    case 6:
        ret = init_ZL1_6();
        break;
    case 7:
        ret = init_ZL1_7();
        break;
    default:
        ret = false;
        break;
    }
    if (!ret) {
        return false;
    }
    field_0x73E.x = current.angle.x;
    field_0x73E.y = current.angle.y;
    field_0x73E.z = current.angle.z;
    shape_angle.x = field_0x73E.x;
    shape_angle.y = field_0x73E.y;
    shape_angle.z = field_0x73E.z;
    mStts.Init(temp, 0xFF, this);
    mCyl.SetStts(&mStts);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}
VERIFY(0x0230503C, &daNpc_Zl1_c::createInit);

static inline fopAc_ac_c* searchByID(daNpc_Zl1_c* i_this, fpc_ProcID pid, be<s32>* o_wasDeleted) {
    return gabi::call<fopAc_ac_c*>(0x0230634C, i_this, pid, o_wasDeleted);
}
/* daPy_py_c (HD): mNoResetFlg0 at +0x3B8 (onPlayerNoDraw: 0x08000000), mDemo.mMoveAngle at +0x422 */
static inline void daPy_onPlayerNoDraw(fopAc_ac_c* pl) { u32 a = gabi::ea(pl) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) | 0x08000000u); }
static inline void daPy_offPlayerNoDraw(fopAc_ac_c* pl) { u32 a = gabi::ea(pl) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u); }
static inline void daPy_changeDemoMoveAngle(fopAc_ac_c* pl, s16 ang) { gabi::store<s16>(gabi::ea(pl) + 0x422, ang); }
/* 0253F124 dEvt_control_c::getPId(actor) (play+0x51D0) */
static inline u32 dEvt_control_getPId(u32 evt, fopAc_ac_c* a) { return gabi::call<u32>(0x0253F124, evt, a); }

/* 023055A4 */
void daNpc_Zl1_c::checkOrder() {
    WWHD_FUNC(0x023055A4, void, this);
    /* a_start_pos (.rodata 0x10024028) */
    u16 command = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdx[field_0x7A8]) && field_0x84A >= 3) {
            switch ((u32)(s32)field_0x7A8) {
            case 0:
                field_0x7D3 = true;
                break;
            case 2:
                /* dComIfGs_setRestartRoom(a_start_pos, 0x4000, 0): dSv_restart_c::setRoom (save + 0x1148) */
                gabi::call(0x025B9810, gabi::load<u32>(0x101F84DC) + 0x1148, 0x10024028, 0x4000, 0);
                field_0x7C9 = true;
                break;
            case 3:
                daPy_onPlayerNoDraw(dComIfGp_getPlayer(0));
                actor_status &= ~0x4000u; /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
                field_0x7D8 = true;
                field_0x79C = 1;
                break;
            }
            field_0x84A = 0;
            field_0x846 = 0xFF;
            field_0x845 = 0xFF;
        }
    } else if (command == 1 /* dEvtCmd_INTALK_e */ && (field_0x84A == 1 || field_0x84A == 2)) {
        field_0x84A = 0;
        field_0x7D7 = true;
    }
}
VERIFY(0x023055A4, &daNpc_Zl1_c::checkOrder);

/* 02305FCC */
s32 daNpc_Zl1_c::isEventEntry() {
    WWHD_FUNC(0x02305FCC, s32, this);
    const char* name = gabi::at<const char>(mEventCut.mpEvtStaffName);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}
VERIFY(0x02305FCC, &daNpc_Zl1_c::isEventEntry);

/* 0259D344 dNpc_setAnmFNDirect(morf, loopMode, morf, speed, name, soundId, arc) */
static inline BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, u32 name, s32 snd, const char* arc) {
    return gabi::call<BOOL>(0x0259D344, morf, loopMode, morfF, speed, name, snd, arc);
}

/* 0230607C */
void daNpc_Zl1_c::setAnm_anm(anm_prm_c* param_1) {
    WWHD_FUNC(0x0230607C, void, this, param_1);
    s8 temp = param_1->field_0x0;
    if (temp < 0 || field_0x849 == temp) {
        return;
    }
    u32 name = bckResID(temp);
    dNpc_setAnmFNDirect(mpMorf, param_1->mLoopMode, param_1->mMorf, param_1->mPlaySpeed, name, 0, mArcName);
    field_0x849 = param_1->field_0x0;
    mFrame = 0.0f;
    field_0x7C3 = 0;
    field_0x7C4 = 0;
}
VERIFY(0x0230607C, &daNpc_Zl1_c::setAnm_anm);

/* 02306114 */
void daNpc_Zl1_c::setAnm() {
    WWHD_FUNC(0x02306114, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C71CC); /* [10] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x84B].field_0x1, true);
    setAnm_anm(&a_anm_prm_tbl[field_0x84B]);
}
VERIFY(0x02306114, &daNpc_Zl1_c::setAnm);

/* 02306184 */
void daNpc_Zl1_c::setStt(s8 param_1) {
    WWHD_FUNC(0x02306184, void, this, param_1);
    s8 temp = field_0x84B;
    field_0x84B = param_1;
    switch ((u32)(s32)param_1) {
    case 0:
    case 4:
        break;
    case 1:
    case 6:
    case 7:
    case 8:
    case 9:
        field_0x84A = 0;
        break;
    case 2:
        field_0x84A = 0;
        field_0x845 = 0xFF;
        field_0x84C = temp;
        field_0x846 = 0xFF;
        field_0x851 = 0;
        break;
    case 3:
        speedF = 0.0f;
        speed.y = 0.0f;
        field_0x7B8 = (s16)cLib_getRndValue(0x5A, 0xB4);
        field_0x7BA = 0;
        break;
    case 5:
        field_0x7CA = true;
        field_0x7D3 = true;
        break;
    }
    setAnm();
}
VERIFY(0x02306184, &daNpc_Zl1_c::setStt);

/* 02306280 */
void daNpc_Zl1_c::endEvent() {
    WWHD_FUNC(0x02306280, void, this);
    dComIfGp_event_reset();
    field_0x845 = 0xFF;
    field_0x846 = 0xFF;
}
VERIFY(0x02306280, &daNpc_Zl1_c::endEvent);

/* 023062C4 */
void daNpc_Zl1_c::cut_init_LOK_PLYER(int i_staffIdx) {
    WWHD_FUNC(0x023062C4, void, this, i_staffIdx);
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x100240EC) /* "prm_0" */);
    field_0x84D = 1;
    if (pPrm0 != nullptr && *pPrm0 == 1) {
        field_0x7D8 = true;
    } else {
        field_0x7D8 = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
}
VERIFY(0x023062C4, &daNpc_Zl1_c::cut_init_LOK_PLYER);

/* 023063A0 */
void daNpc_Zl1_c::cut_init_LOK_PARTNER(int i_staffIdx) {
    WWHD_FUNC(0x023063A0, void, this, i_staffIdx);
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x100240F4) /* "prm_0" */);
    gabi::Local<be<s32>> wasDeleted;
    if (searchByID(this, mProcId1, wasDeleted) != nullptr || !*wasDeleted) {
        field_0x84D = 4;
        mProcId2 = mProcId1;
        if (pPrm0 != nullptr && *pPrm0 == 1) {
            field_0x7D8 = true;
        } else {
            field_0x7D8 = false;
            m_jnt.mbTrn = 1; /* setTrn() */
        }
    }
}
VERIFY(0x023063A0, &daNpc_Zl1_c::cut_init_LOK_PARTNER);

/* 02306458 */
void daNpc_Zl1_c::setAnm_ATR() {
    WWHD_FUNC(0x02306458, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C726C); /* [19] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x845].field_0x1, true);
    setAnm_anm(&a_anm_prm_tbl[field_0x845]);
}
VERIFY(0x02306458, &daNpc_Zl1_c::setAnm_ATR);

/* 023064C0 */
void daNpc_Zl1_c::cut_init_CHG_ANM_ATR(int i_staffIdx) {
    WWHD_FUNC(0x023064C0, void, this, i_staffIdx);
    be<s32>* pAtrNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x100240FC) /* "AtrNo" */);
    if (pAtrNo != nullptr) {
        field_0x845 = (u8)(s32)*pAtrNo;
        setAnm_ATR();
    }
}
VERIFY(0x023064C0, &daNpc_Zl1_c::cut_init_CHG_ANM_ATR);

/* 02306528 */
void daNpc_Zl1_c::cut_init_PLYER_TRN_PARTNER(int) {
    WWHD_FUNC(0x02306528, void, this, 0);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<be<s32>> wasDeleted;
    fopAc_ac_c* pActor = searchByID(this, mProcId1, wasDeleted);
    if (pActor != nullptr || !*wasDeleted) {
        u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
        gabi::store<u32>(evt + 0xD0, dEvt_control_getPId(evt, pActor)); /* dComIfGp_event_setItemPartner */
        evt = dComIfGp_ea() + PLAY_EVTCTRL;
        gabi::store<u32>(evt + 0xCC, dEvt_control_getPId(evt, pActor)); /* dComIfGp_event_setTalkPartner */
        daPy_changeDemoMoveAngle(player, cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &pActor->current.pos));
    }
}
VERIFY(0x02306528, &daNpc_Zl1_c::cut_init_PLYER_TRN_PARTNER);

/* 023065D4 */
void daNpc_Zl1_c::cut_init_PLYER_TRN_TETRA(int) {
    WWHD_FUNC(0x023065D4, void, this, 0);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    daPy_changeDemoMoveAngle(player, cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &current.pos));
}
VERIFY(0x023065D4, &daNpc_Zl1_c::cut_init_PLYER_TRN_TETRA);

/* 02306624 */
void daNpc_Zl1_c::cut_init_MAJYU_START(int) {
    WWHD_FUNC(0x02306624, void, this, 0);
    current.pos.y = 2200.0f;
    field_0x758.y = 400.0f;
    field_0x758.z = 7766.0f;
    field_0x758.x = -37978.0f;
    current.pos.z = 7852.0f;
    current.pos.x = -37927.0f;
    current.angle.y = cLib_targetAngleY(&current.pos, &field_0x758);
    field_0x84D = 2;
    field_0x7D0 = true;
}
VERIFY(0x02306624, &daNpc_Zl1_c::cut_init_MAJYU_START);

/* 023066D0 */
void daNpc_Zl1_c::setAnm_NUM(int param_1, int param_2) {
    WWHD_FUNC(0x023066D0, void, this, param_1, param_2);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C739C); /* [17] */
    if (param_2 != 0) {
        init_texPttrnAnm(a_anm_prm_tbl[param_1].field_0x1, true);
    }
    setAnm_anm(&a_anm_prm_tbl[param_1]);
}
VERIFY(0x023066D0, &daNpc_Zl1_c::setAnm_NUM);

/* 0230674C */
void daNpc_Zl1_c::cut_init_DRW_ONOFF(int i_staffIdx) {
    WWHD_FUNC(0x0230674C, void, this, i_staffIdx);
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x10024118) /* "OnOff" */);
    if (pOnOff != nullptr) {
        field_0x7D4 = *pOnOff == 0;
    }
}
VERIFY(0x0230674C, &daNpc_Zl1_c::cut_init_DRW_ONOFF);

/* 023067B4 */
void daNpc_Zl1_c::cut_init_PLYER_DRW_ONOFF(int i_staffIdx) {
    WWHD_FUNC(0x023067B4, void, this, i_staffIdx);
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x10024120) /* "OnOff" */);
    if (pOnOff != nullptr) {
        s32 onoff = *pOnOff;
        fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
        if (onoff == 0) {
            daPy_onPlayerNoDraw(player);
        } else {
            daPy_offPlayerNoDraw(player);
        }
    }
}
VERIFY(0x023067B4, &daNpc_Zl1_c::cut_init_PLYER_DRW_ONOFF);

/* 02307988 */
void daNpc_Zl1_c::eventOrder() {
    WWHD_FUNC(0x02307988, void, this);
    if (field_0x84A == 1 || field_0x84A == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (field_0x84A == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (field_0x84A >= 3) {
        field_0x7A8 = field_0x84A - 3;
        fopAcM_orderOtherEventId(this, mEventIdx[field_0x7A8], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02307988, &daNpc_Zl1_c::eventOrder);
