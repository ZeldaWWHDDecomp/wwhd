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
#include "d/actor/d_a_npc_zl1_main.h"

const gc_tu_data gc_tu = {.safeString=ZL1_SAFESTRING_VTBL};

static const ProcFunc_l demo_action1_record = {0, -1, 0x02309E10};
static const ProcFunc_l wait_action1_record = {0, -1, 0x02309CEC};
static const ProcFunc_l demo_action2_record = {0, -1, 0x02309EB8};
static const ProcFunc_l optn_action1_record = {0, -1, 0x02309FC8};
static daNpc_Zl1_c::anm_prm_c a_anm_prm_tbl_setAnm[] = {
    {-1, -1, 0.0f, 0.0f, -1},
    {0, 1, 0.0f, 1.0f, 2},
    {-1, -1, 0.0f, 0.0f, -1},
    {8, 1, 8.0f, 1.0f, 2},
    {9, 1, 8.0f, 1.0f, 2},
    {13, 13, 8.0f, 0.0f, 0},
    {-1, -1, 0.0f, 0.0f, -1},
    {0, 1, 0.0f, 1.0f, 2},
    {0, 1, 0.0f, 1.0f, 2},
    {0, 1, 0.0f, 1.0f, 2},
};
static daNpc_Zl1_c::anm_prm_c a_anm_prm_tbl_setAnm_ATR[] = {
    {0, 1, 8.0f, 1.0f, 2},
    {1, 1, 8.0f, 1.0f, 2},
    {2, 1, 8.0f, 1.0f, 2},
    {3, 1, 8.0f, 1.0f, 2},
    {4, 1, 8.0f, 1.0f, 2},
    {0, 2, 8.0f, 1.0f, 2},
    {3, 3, 8.0f, 1.0f, 2},
    {0, 4, 8.0f, 1.0f, 2},
    {4, 5, 8.0f, 1.0f, 2},
    {0, 6, 8.0f, 1.0f, 2},
    {3, 7, 8.0f, 1.0f, 2},
    {2, 7, 8.0f, 1.0f, 2},
    {15, 4, 8.0f, 1.0f, 2},
    {16, 15, 8.0f, 1.0f, 2},
    {15, 1, 8.0f, 1.0f, 2},
    {15, 7, 8.0f, 1.0f, 2},
    {15, 16, 8.0f, 1.0f, 2},
    {8, 1, 8.0f, 1.0f, 2},
    {8, 7, 8.0f, 1.0f, 2},
};
static daNpc_Zl1_c::anm_prm_c a_anm_prm_tbl_setAnm_NUM[] = {
    {0, 1, 8.0f, 1.0f, 2},
    {1, 1, 8.0f, 1.0f, 2},
    {2, 1, 8.0f, 1.0f, 2},
    {3, 1, 8.0f, 1.0f, 2},
    {4, 1, 8.0f, 1.0f, 2},
    {5, 8, 8.0f, 1.0f, 0},
    {6, 0, 8.0f, 1.0f, 2},
    {7, 0, 8.0f, 1.0f, 2},
    {8, 1, 8.0f, 1.0f, 2},
    {9, 1, 8.0f, 1.0f, 2},
    {10, 1, 0.0f, 0.0f, 0},
    {11, 1, 8.0f, 1.0f, 2},
    {12, 12, 8.0f, 1.0f, 2},
    {13, 13, 0.0f, 0.0f, 0},
    {14, 14, 0.0f, 1.0f, 0},
    {15, 1, 8.0f, 1.0f, 2},
    {16, 15, 8.0f, 1.0f, 2},
};
static const cXyz a_chk_playerPos[2][3] = {
    {{0.0f, -1145.0f, -4005.0f}, {0.0f, -2099.0f, 3913.0f}, {0.0f, -1145.0f, -3542.0f}},
    {{-4.0f, 0.0f, 2695.0f}, {0.0f, 149.0f, -955.0f}, {0.0f, -1997.0f, -7758.0f}},
};
static const cXyz a_set_tetoraPos[2][3] = {
    {{-75.0f, -1146.0f, -4160.0f}, {80.0f, -2100.0f, 4193.0f}, {-75.0f, -1146.0f, -3700.0f}},
    {{92.0f, 0.0f, 2842.0f}, {-129.0f, 149.0f, -1132.0f}, {107.0f, -1978.0f, -7559.0f}},
};
static const f32 a_set_tetoraAng[2][3] = {{16.0f, -171.0f, 16.0f}, {-159.0f, 15.0f, -153.0f}};
static const cXyz a_start_pos = {-229.76634216308594f, 37.46316909790039f, 305.2134094238281f};
static const char archiveZl2[] = "Zl2";
static const char archiveZl[] = "Zl";
static const char* l_evn_tbl[] = {"yuukaigo", "ooi", "majyuu_shinnyuu", "nakaniwa"};
static s32 l_check_wrk = 0;
static fopAc_ac_c* l_check_inf[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

enum {
    dSv_event_flag_UNK_0001 = 0x0001,
    dSv_event_flag_UNK_0101 = 0x0101,
    dSv_event_flag_UNK_0E20 = 0x0E20,
    dSv_event_flag_UNK_2401 = 0x2401,
    dSv_event_flag_UNK_0801 = 0x0801,
};
enum { dSv_event_flag_ZELDA_AWAKENED = 0x2D02, dSv_event_flag_HYRULE_COURTYARD_CUTSCENE = 0x3804 };

// HD: SafeString compares call assureTermination twice on the literal and once on the stage.
static bool zl1_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(name);
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    u32 stage = gabi::ea(dComIfGp_getStartStageName());
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = stage;
    zl1_main::assureTermination(a);
    zl1_main::assureTermination(a);
    const be<u8>* pa = zl1_main::safeStringBytes(a);
    zl1_main::assureTermination(b);
    const be<u8>* pb = zl1_main::safeStringBytes(b);
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = pa[i];
        u8 cb = pb[i];
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* 023035B8 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daNpc_Zl1_c*>(i_this)->CreateHeap();
}

/* 02303D14 */
BOOL daNpc_Zl1_c::set_startPos(int param_1) {

    // HD: initialize the selection and omit the GameCube temp2 < 3 check.
    int temp2 = 0;
    f32 temp = 1000000000.0f; /* G_CM3D_F_INF */
    if (param_1 == 0 || param_1 == 1) {
        for (int i = 0; i < 3; i++) {
            gabi::Local<cXyz> chkPlayerPos;
            const cXyz* src = &a_chk_playerPos[param_1][i];
            f32 x = src->x, y = src->y;
            chkPlayerPos->x = x;
            chkPlayerPos->y = y;
            chkPlayerPos->z = src->z;
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
        const cXyz* pos = &a_set_tetoraPos[param_1][temp2];
        current.pos.x = pos->x;
        current.pos.y = pos->y;
        current.pos.z = pos->z;
        current.angle.y = (s16)gabi::ftoi(a_set_tetoraAng[param_1][temp2] * 182.04445f); /* cM_deg2s */
        /* mObjAcch.SetOld(): *pOld = *pPos (bit copy) */
        cXyz* oldPos = mObjAcch.pm_old_pos;
        cXyz* newPos = mObjAcch.pm_pos;
        oldPos->copy(*newPos);
    }
    return TRUE;
}

/* 023039E4 */
bool daNpc_Zl1_c::init_ZL1_0() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0001)) {
        gabi::Local<ProcFunc_l> pmf;
        zl1_main::copyAction(pmf, &demo_action1_record);
        set_action(pmf, nullptr);
        field_0x7D2 = true;
        actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
        return true;
    }
    return false;
}

/* 02303A7C */
bool daNpc_Zl1_c::init_ZL1_1() {
    if (!dComIfGs_isEventBit(dSv_event_flag_UNK_0101)) {
        gabi::Local<ProcFunc_l> pmf;
        zl1_main::copyAction(pmf, &demo_action1_record);
        set_action(pmf, nullptr);
        field_0x7CE = true;
        actor_status &= ~u32(fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e); /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        return true;
    }
    return false;
}

/* 02303B14 */
bool daNpc_Zl1_c::init_ZL1_2() {
    if (dComIfGs_isEventBit(dSv_event_flag_UNK_0E20) && !dComIfGs_isEventBit(dSv_event_flag_UNK_2401)) {
        gabi::Local<ProcFunc_l> pmf;
        zl1_main::copyAction(pmf, &wait_action1_record);
        set_action(pmf, nullptr);
        return true;
    }
    return false;
}

/* 02303BBC */
bool daNpc_Zl1_c::init_ZL1_3() {
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
        zl1_main::copyAction(pmf, &demo_action2_record);
        set_action(pmf, nullptr);
        actor_status &= ~u32(fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e);
        return true;
    }
    return false;
}

/* 02303CBC */
bool daNpc_Zl1_c::init_ZL1_4() {
    gabi::Local<ProcFunc_l> pmf;
    zl1_main::copyAction(pmf, &wait_action1_record);
    set_action(pmf, nullptr);
    actor_status &= ~u32(fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e);
    return true;
}

/* 02303E98 */
bool daNpc_Zl1_c::init_ZL1_5() {
    if (!dComIfGs_isEventBit(dSv_event_flag_ZELDA_AWAKENED)) {
        gabi::Local<ProcFunc_l> pmf;
        zl1_main::copyAction(pmf, &optn_action1_record);
        set_action(pmf, nullptr);
        actor_status &= ~u32(fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e); /* fopAcM_OffStatus(this, fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e) */
        if (!dComIfGs_isEventBit(dSv_event_flag_HYRULE_COURTYARD_CUTSCENE)) {
            actor_status |= fopAcStts_UNK4000_e; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            field_0x84A = 6;
            field_0x7A8 = field_0x84A + -3;
            fopAcM_orderOtherEventId(this, mEventIdx[field_0x7A8], 0xFF, 0xFFFF, 0, 1);
            Zl1main_cancelStartDemo(zl1_main::eventException()); /* dComIfGp_evmng_cancelStartDemo(): dEvent_exception_c::init */
        } else {
            if (zl1_isStartStage("Hyrule" /* "Hyrule" */)) {
                set_startPos(1);
            }
            if (zl1_isStartStage("Hyroom" /* "Hyroom" */)) {
                set_startPos(0);
            }
        }
        gravity = -4.5f;
        return true;
    }
    return false;
}

/* 02304148 */
bool daNpc_Zl1_c::init_ZL1_6() {
    gabi::Local<ProcFunc_l> pmf;
    zl1_main::copyAction(pmf, &demo_action1_record);
    set_action(pmf, nullptr);
    actor_status &= ~0x3Fu; /* fopAcM_ClearStatusMap(this) */
    return true;
}

/* 023041A0 */
bool daNpc_Zl1_c::init_ZL1_7() {
    gabi::Local<ProcFunc_l> pmf;
    zl1_main::copyAction(pmf, &demo_action1_record);
    set_action(pmf, nullptr);
    actor_status &= ~0x3Fu;
    return true;
}

/* 0230503C */
bool daNpc_Zl1_c::createInit() {
    bool ret;
    int temp = 0xFF;

    for (int i = 0; i < 4; i++) {
        const char* name = l_evn_tbl[i];
        mEventIdx[i] = dComIfGp_evmng_getEventIdx(name, 0xFF);
    }
    mEventCut.setActorInfo2("Zl1" /* "Zl1" */, this);
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;  /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;  /* attention_info.distances[TALK] */
    field_0x849 = 0x11;
    gravity = 0.0f;
    field_0x7BC = -1;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB;  /* attention_info.distances[SPEAK] */
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
    mCyl.Set(zl1_main::npcCylinderSource() /* dNpc_cyl_src */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    play_animation();
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    tevStr.mEnvrIdxOverride = dBgS_GetPolyColor(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mObjAcch.m_gnd));
    mpMorf->setMorf(0.0f);
    setMtx(true);
    return true;
}

/* 0230607C */
void daNpc_Zl1_c::setAnm_anm(anm_prm_c* param_1) {
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

/* 023066D0 */
void daNpc_Zl1_c::setAnm_NUM(int param_1, int param_2) {
    anm_prm_c* a_anm_prm_tbl = a_anm_prm_tbl_setAnm_NUM; /* [17] */
    if (param_2 != 0) {
        init_texPttrnAnm(a_anm_prm_tbl[param_1].field_0x1, true);
    }
    setAnm_anm(&a_anm_prm_tbl[param_1]);
}

/* 02306114 */
void daNpc_Zl1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = a_anm_prm_tbl_setAnm; /* [10] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x84B].field_0x1, true);
    setAnm_anm(&a_anm_prm_tbl[field_0x84B]);
}

/* 02306458 */
void daNpc_Zl1_c::setAnm_ATR() {
    anm_prm_c* a_anm_prm_tbl = a_anm_prm_tbl_setAnm_ATR; /* [19] */
    init_texPttrnAnm(a_anm_prm_tbl[field_0x845].field_0x1, true);
    setAnm_anm(&a_anm_prm_tbl[field_0x845]);
}

/* 02307988 */
void daNpc_Zl1_c::eventOrder() {
    if (field_0x84A == 1 || field_0x84A == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e); /* dEvtCnd_CANTALK_e */
        if (field_0x84A == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (field_0x84A >= 3) {
        field_0x7A8 = field_0x84A - 3;
        fopAcM_orderOtherEventId(this, mEventIdx[field_0x7A8], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 023055A4 */
void daNpc_Zl1_c::checkOrder() {

    u16 command = eventInfo.mCommand; /* eventInfo.mCommand */
    if (command == 2 /* checkCommandDemoAccrpt() */) {
        if (dComIfGp_evmng_startCheck(mEventIdx[field_0x7A8]) && field_0x84A >= 3) {
            switch ((u32)(s32)field_0x7A8) {
            case 0:
                field_0x7D3 = true;
                break;
            case 2:
                /* dComIfGs_setRestartRoom(a_start_pos, 0x4000, 0): dSv_restart_c::setRoom (save + 0x1148) */
                Zl1main_setRestartRoom(zl1_main::restart(), &a_start_pos, 0x4000, 0);
                field_0x7C9 = true;
                break;
            case 3:
                zl1_main::player(dComIfGp_getPlayer(0))->mNoResetFlg0 |= 0x08000000;
                actor_status &= ~u32(fopAcStts_UNK4000_e); /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
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

/* 0230634C */
static fopAc_ac_c* daNpc_Zl1_searchByID(daNpc_Zl1_c* i_this, fpc_ProcID i_pid, be<s32>* o_wasDeleted) {
    gabi::Local<gptr<fopAc_ac_c>> actor;
    *actor = nullptr;
    *o_wasDeleted = FALSE;
    if (!Zl1main_searchByIdOut(i_pid, actor.get())) { /* fopAcM_SearchByID(id, &actor) */
        *o_wasDeleted = TRUE;
    }
    return *actor;
}

/* 0230545C */
u8 daNpc_Zl1_c::partner_search_sub(u32 i_judgeFunc) {
    bool ret = false;
    mProcId1 = 0xFFFFFFFF;
    l_check_wrk = 0;
    for (int i = 0; i < 20; i++) {
        l_check_inf[i] = nullptr;
    }
    fpcM_Search(i_judgeFunc, this);
    if (l_check_wrk != 0) {
        mProcId1 = fopAcM_GetID(l_check_inf[0].get());
        ret = true;
    }
    return ret;
}

/* 02305508 */
void daNpc_Zl1_c::partner_search() {
    bool temp;
    if (field_0x850 == 1) {
        switch ((u32)(s32)field_0x84F) {
        case 1:
            temp = partner_search_sub(zl1_main::searchActor_Branch);
            break;
        case 2:
            temp = partner_search_sub(zl1_main::searchActor_Bm1);
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

/* 023041F8 */
void daNpc_Zl1_c::setEyeCtrl() {
    for (int i = 0; i < 2; i++) {
        daNpc_Zl1_matAnm_c* p = field_0x6D4[i];
        if (p) {
            p->field_0x7C = 1;
        }
    }
    field_0x7D1 = true;
}

/* 02304230 */
void daNpc_Zl1_c::clrEyeCtrl() {
    for (int i = 0; i < 2; i++) {
        daNpc_Zl1_matAnm_c* p = field_0x6D4[i];
        if (p) {
            p->field_0x7C = 0;
        }
    }
    field_0x7D1 = false;
}

/* 02304C44 */
void daNpc_Zl1_c::setAttention(u32 param_1) {
    cXyz* attPos = &attention_info.position; /* attention_info.position */
    f32 x = current.pos.x;
    f32 y = current.pos.y + zl1_main::hio().mPrmTbl.field_1C;
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

/* 023037F4 */
u8 daNpc_Zl1_c::decideType(int param_1) {
    if (field_0x84E > 0) {
        return true;
    }
    field_0x84E = 1;
    // HD: the switch is folded into a range check
    if ((u32)param_1 > 7) {
        field_0x84F = -1;
        return false;
    }
    field_0x84F = (s8)param_1;
    switch ((u32)(s32)field_0x84F) {
    case 0:
    case 6:
    case 7:

        for (int i = 0; i < 4; i++) zl1_main::archiveBytes(this)[i] = zl1_main::bytes(archiveZl2)[i];
        break;
    default:
        /* strcpy(mArcName, "Zl") */
        for (int i = 0; i < 3; i++) zl1_main::archiveBytes(this)[i] = zl1_main::bytes(archiveZl)[i];
        break;
    }
    bool ret = false;
    if (field_0x84E != -1 && field_0x84F != -1) {
        ret = true;
    }
    return ret;
}

/* 0230683C */
f32 daNpc_Zl1_c::get_prmFloat(be<f32>* param_1, f32 param_2) {
    if (param_1 == nullptr) {
        return param_2;
    }
    return *param_1;
}

/* 023069E8 */
void daNpc_Zl1_c::init_Light() {
    // HD: other colours and powers (GameCube: 0x5C/0xAE/0xFF 139.7, 0xBA/0xFF/0xFF 270.7)
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

/* 023079F8 */
void daNpc_Zl1_c::incEnvironment() {
    f32 r = mRatio + 0.05f;
    r = (r - 1.0f >= 0.0f) ? 1.0f : r; /* cLib_maxLimit (fsel) */
    mRatio = r;
    if (!(r < 1.0f)) {
        field_0x7C2 = 0;
    }
}

/* 02307A30 */
void daNpc_Zl1_c::decEnvironment() {
    f32 r = mRatio - 0.05f;
    mRatio = (r - 0.15f >= 0.0f) ? r : 0.15f; /* cLib_minLimit (fsel) */
}

/* 023062C4 */
void daNpc_Zl1_c::cut_init_LOK_PLYER(int i_staffIdx) {
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "prm_0" /* "prm_0" */);
    field_0x84D = 1;
    if (pPrm0 != nullptr && *pPrm0 == 1) {
        field_0x7D8 = true;
    } else {
        field_0x7D8 = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
}

/* 023063A0 */
void daNpc_Zl1_c::cut_init_LOK_PARTNER(int i_staffIdx) {
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "prm_0" /* "prm_0" */);
    gabi::Local<be<s32>> wasDeleted;
    if (Zl1main_searchByID(this, mProcId1, wasDeleted) != nullptr || !*wasDeleted) {
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

/* 023064C0 */
void daNpc_Zl1_c::cut_init_CHG_ANM_ATR(int i_staffIdx) {
    be<s32>* pAtrNo = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "AtrNo" /* "AtrNo" */);
    if (pAtrNo != nullptr) {
        field_0x845 = (u8)(s32)*pAtrNo;
        setAnm_ATR();
    }
}

/* 02306528 */
void daNpc_Zl1_c::cut_init_PLYER_TRN_PARTNER(int) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<be<s32>> wasDeleted;
    fopAc_ac_c* pActor = Zl1main_searchByID(this, mProcId1, wasDeleted);
    if (pActor != nullptr || !*wasDeleted) {
        dComIfGp_event_setItemPartner(pActor);
        dComIfGp_event_setTalkPartner(pActor);
        zl1_main::player(player)->mDemoMoveAngle =  cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &pActor->current.pos);
    }
}

/* 023065D4 */
void daNpc_Zl1_c::cut_init_PLYER_TRN_TETRA(int) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    zl1_main::player(player)->mDemoMoveAngle =  cLib_targetAngleY(&dComIfGp_getPlayer(0)->current.pos, &current.pos);
}

/* 02306624 */
void daNpc_Zl1_c::cut_init_MAJYU_START(int) {
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

/* 023066B4 */
void daNpc_Zl1_c::cut_init_OKIRU(int) {
    mpMorf->setPlaySpeed(1.0f); /* mpMorf->setPlaySpeed(1.0f) */
    field_0x7C3 = 0;
}

/* 02306D34 */
u32 daNpc_Zl1_c::cut_move_OKIRU() {
    return field_0x7C3 != 0 ? TRUE : FALSE;
}

/* 02306740 */
void daNpc_Zl1_c::cut_init_OKIRU_2(int) {
    setAnm_NUM(0xE, 1);
}

/* 0230674C */
void daNpc_Zl1_c::cut_init_DRW_ONOFF(int i_staffIdx) {
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "OnOff" /* "OnOff" */);
    if (pOnOff != nullptr) {
        field_0x7D4 = *pOnOff == 0;
    }
}

/* 023067B4 */
void daNpc_Zl1_c::cut_init_PLYER_DRW_ONOFF(int i_staffIdx) {
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, "OnOff" /* "OnOff" */);
    if (pOnOff != nullptr) {
        s32 onoff = *pOnOff;
        fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
        if (onoff == 0) {
            zl1_main::player(player)->mNoResetFlg0 |= 0x08000000;
        } else {
            zl1_main::player(player)->mNoResetFlg0 &= ~0x08000000u;
        }
    }
}

/* 02306280 */
void daNpc_Zl1_c::endEvent() {
    dComIfGp_event_reset();
    field_0x845 = 0xFF;
    field_0x846 = 0xFF;
}

/* 02305FCC */
s32 daNpc_Zl1_c::isEventEntry() {
    const char* name = zl1_main::staffName(mEventCut);
    return dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
}

/* 023038B8 */
BOOL daNpc_Zl1_c::set_action(ProcFunc_l* i_action, void* param_2) {
    ProcFunc_l* cur = &mCurrActionFunc;
    bool skipOldAction = false;
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
            skipOldAction = true;
    }
    if (!skipOldAction) {
        field_0x850 = 9;
        zl1_main::callAction(this, cur, param_2);
    }
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    field_0x850 = 0;
    zl1_main::callAction(this, cur, param_2);
    return TRUE;
}

/* 02306184 */
void daNpc_Zl1_c::setStt(s8 param_1) {
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
        field_0x7B8 = (s16)Zl1main_getRndValue(0x5A, 0xB4);
        field_0x7BA = 0;
        break;
    case 5:
        field_0x7CA = true;
        field_0x7D3 = true;
        break;
    }
    setAnm();
}

/* 023053EC */
BOOL daNpc_Zl1_c::_delete() {
    // HD: dComIfG_resDelete (GameCube: dComIfG_resDeleteDemo)
    dComIfG_resDelete(&mPhs, mArcName);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    dKy_plight_cut(&mLightInfluence1);
    Zl1main_cutEffectLight(&mLightInfluence2); /* dKy_efplight_cut */
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)mRippleCallBack);
    return TRUE;
}

/* 023053E8 */
static cPhs_State daNpc_Zl1_Create(fopAc_ac_c* i_this) {
    return ((daNpc_Zl1_c*)i_this)->_create();
}

/* 02305458 */
static BOOL daNpc_Zl1_Delete(daNpc_Zl1_c* i_this) {
    return i_this->_delete();
}

/* 0230808C */
static BOOL daNpc_Zl1_Execute(daNpc_Zl1_c* i_this) {
    return i_this->_execute();
}

/* 023086F0 */
static BOOL daNpc_Zl1_Draw(daNpc_Zl1_c* i_this) {
    return i_this->_draw();
}

/* 023086F4 */
static BOOL daNpc_Zl1_IsDelete(daNpc_Zl1_c*) {
    return TRUE;
}

/* WWHD glue
data demo_action1_record 10023AD8
data wait_action1_record 10023AE0
data demo_action2_record 10023AE8
data optn_action1_record 10023AF0
data a_anm_prm_tbl_setAnm 101C71CC
data a_anm_prm_tbl_setAnm_ATR 101C726C
data a_anm_prm_tbl_setAnm_NUM 101C739C
data a_chk_playerPos 10023F14
data a_set_tetoraPos 10023F5C
data a_set_tetoraAng 10023EFC
data a_start_pos 10024028
data l_evn_tbl 101C70C0
data l_check_wrk 10468B14 nocheck
data l_check_inf 10468C84 nocheck
ext 0255BA9C void Zl1main_cutEffectLight(LIGHT_INFLUENCE* light)
ext 025D54C4 BOOL Zl1main_searchByIdOut(fpc_ProcID id,gptr<fopAc_ac_c>* actor)
ext 0230634C fopAc_ac_c* Zl1main_searchByID(daNpc_Zl1_c* actor,fpc_ProcID id,be<s32>* deleted)
ext 0254351C void Zl1main_cancelStartDemo(void* exception)
ext 025B9810 void Zl1main_setRestartRoom(dSv_restart_c* restart,const cXyz* pos,s16 angle,s8 room)
ext 0259D344 BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* morf,s32 loopMode,f32 morfF,f32 speed,u32 name,s32 snd,const char* arc)
ext 021E1E78 s32 Zl1main_getRndValue(s32 base,s32 range)
ext 024EF130 s8 dBgS_GetRoomId(dBgS* bgs,void* poly)
ext 024EEEB8 u8 dBgS_GetPolyColor(dBgS* bgs,void* poly)
str 10023EE0 "Zl2"
str 10023EE4 "Zl"
str 10023C48 "yuukaigo"
str 10023B0C "ooi"
str 10023C28 "majyuu_shinnyuu"
str 10023C54 "nakaniwa"
str 10023FA8 "Hyrule" in 02303E98
str 10023FB0 "Hyroom" in 02303E98
str 10024018 "Zl1" in 0230503C
str 100240EC "prm_0" in 023062C4
str 100240F4 "prm_0" in 023063A0
str 100240FC "AtrNo" in 023064C0
str 10024118 "OnOff" in 0230674C
str 10024120 "OnOff" in 023067B4
*/
