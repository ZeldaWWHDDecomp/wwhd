/**
 * d_a_npc_zl1_demo.cpp (WWHD)
 * NPC - Tetra: demo, _execute, lookBack, event cuts, darkness (part of d_a_npc_zl1)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_zl1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_zl1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1988 HD: mDoAud_seStart(id) (sound without position) */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
/* d_kankyo colour ratios (HD addresses, single float argument) */
static inline void dKy_set_vrboxcol_ratio(f32 r) { gabi::call(0x0255A51C, r); }
static inline void dKy_set_fogcol_ratio(f32 r) { gabi::call(0x0255A458, r); }
static inline void dKy_set_bgcol_ratio(f32 r) { gabi::call(0x0255A418, r); }
static inline void dKy_fog_startendz_set(f32 s, f32 e, f32 r) { gabi::call(0x0255671C, s, e, r); }
/* acch flags (mObjAcch.m_flags, +0x28) */
static inline u32 acch_flags(fopNpc_npc_c* a) { return gabi::load<u32>(gabi::ea(&a->mObjAcch) + 0x28); }
enum { ACCH_GROUND_HIT = 0x20, ACCH_WATER_IN = 0x1000 };

/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: sead::SafeString compare (the virtual at
 * vtable +0x14 twice on the literal, once on the stage name at play+0x5134, then at most 0x40001 bytes) */
static inline void ss_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static bool zl1_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(name);
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = stage;
    ss_vcall(a);
    ss_vcall(a);
    u32 pa = a->mStringTop;
    ss_vcall(b);
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

/* 02306D48 */
u32 daNpc_Zl1_c::cut_move_OKIRU_2() {
    WWHD_FUNC(0x02306D48, u32, this);
    if (field_0x7C3) {
        setAnm_NUM(0x10, 1);
        mpMorf->setMorf(16.0f);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02306D48, &daNpc_Zl1_c::cut_move_OKIRU_2);

/* 02306DB8 */
u8 daNpc_Zl1_c::move_jmp() {
    WWHD_FUNC(0x02306DB8, u8, this);
    mDoExt_McaMorf* morf = mpMorf;
    f32 end = (f32)(s32)morf->mFrameCtrl.mEnd - 1.0f;
    if (!(morf->getFrame() < end)) {
        setAnm_NUM(8, 1);
        return false;
    }
    if (!field_0x7C7 && (acch_flags(this) & ACCH_GROUND_HIT)) {
        morf->setPlaySpeed(1.0f);
        gravity = -4.5f;
        speedF = 0.0f;
        speed.y = 0.0f;
        if (field_0x7C8) {
            return true;
        }
    } else if (field_0x7C8 || !(acch_flags(this) & ACCH_WATER_IN)) {
        return true;
    }
    field_0x744.x = current.angle.x;
    field_0x744.y = current.angle.y;
    field_0x744.z = current.angle.z;
    field_0x77C.copy(current.pos);
    field_0x7C6 = true;
    return true;
}
VERIFY(0x02306DB8, &daNpc_Zl1_c::move_jmp);

/* 02306ED4 */
u32 daNpc_Zl1_c::cut_move_JMP_OFF() {
    WWHD_FUNC(0x02306ED4, u32, this);
    if (!(speedF < 0.1f)) {
        cLib_chaseF(&speedF, 0.1f, 0.05f);
    }
    return (u8)(move_jmp() ^ 1);
}
VERIFY(0x02306ED4, &daNpc_Zl1_c::cut_move_JMP_OFF);

/* 02306CDC */
void daNpc_Zl1_c::cut_init_SURPRISED(int) {
    WWHD_FUNC(0x02306CDC, void, this, 0);
    mDoAud_seStart_id(0x852); /* JA_SE_ITM_OMAMORI_BLINK */
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> pos;
    pos->z = 0.0f;
    pos->y = 1.0f;
    pos->x = 0.0f;
    gabi::call(0x025CB374, vib, 4, 1, pos.get()); /* StartShock(4, 1, cXyz(0, 1, 0)) */
}
VERIFY(0x02306CDC, &daNpc_Zl1_c::cut_init_SURPRISED);

/* 0230741C: returns a cXyz through the hidden result pointer (HD: a NULL result pointer
 * allocates the object) */
void daNpc_Zl1_c::kyoroPos(cXyz* o_pos, int param_1) {
    WWHD_FUNC(0x0230741C, void, this, o_pos, param_1);
    /* a_tgt_offst (.data 0x101C74E4) */
    u32 src = 0x101C74E4 + param_1 * 12;
    gabi::Local<cXyz> temp1;
    gabi::Local<cXyz> temp2;
    f32 x = eyePos.x, y = eyePos.y, z = eyePos.z;
    temp1->z = gabi::load<f32>(src + 8);
    temp1->y = gabi::load<f32>(src + 4);
    temp1->x = gabi::load<f32>(src);
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp1, temp2);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->x = temp2->x;
    o_pos->y = temp2->y;
    o_pos->z = temp2->z;
}
VERIFY(0x0230741C, &daNpc_Zl1_c::kyoroPos);

/* 023074E4 */
BOOL daNpc_Zl1_c::kyorokyoro() {
    WWHD_FUNC(0x023074E4, BOOL, this);
    if (cLib_calcTimer(&field_0x7B0) != 0) {
        gabi::Local<cXyz> pos;
        kyoroPos(pos, field_0x7B2);
        field_0x758.copy(*pos);
        return TRUE;
    }
    field_0x7B2 = (s16)cLib_getRndValue(1, 10);
    if (field_0x849 == 0xB) {
        field_0x7B0 = l_HIO().mPrmTbl.field_2E;
        field_0x7AE = l_HIO().mPrmTbl.field_32;
    } else {
        field_0x7B0 = l_HIO().mPrmTbl.field_2C;
        field_0x7AE = l_HIO().mPrmTbl.field_30;
    }
    return FALSE;
}
VERIFY(0x023074E4, &daNpc_Zl1_c::kyorokyoro);

/* 02306A40: returns a cXyz through the hidden result pointer (HD: NULL allocates) */
void daNpc_Zl1_c::set_LightPos(cXyz* o_pos) {
    WWHD_FUNC(0x02306A40, void, this, o_pos);
    /* player's getRightHandPos() at +0x3FC (HD), each access through dComIfGp_getPlayer(0) */
    f32 x = gabi::load<f32>(gabi::ea(dComIfGp_getPlayer(0)) + 0x3FC);
    f32 y = gabi::load<f32>(gabi::ea(dComIfGp_getPlayer(0)) + 0x400);
    f32 z = gabi::load<f32>(gabi::ea(dComIfGp_getPlayer(0)) + 0x404);
    y = y + 5.66f;
    f32 c = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
    f32 s = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
    x = x + gabi::fmadds(c, 10.25f, s * 11.13f);
    f32 s2 = cM_ssin(dComIfGp_getPlayer(0)->shape_angle.y);
    f32 c2 = cM_scos(dComIfGp_getPlayer(0)->shape_angle.y);
    z = z + gabi::fmsubs(c2, 11.13f, s2 * 10.25f);
    if (o_pos == nullptr) {
        o_pos = (cXyz*)operator_new(12);
        if (o_pos == nullptr)
            return;
    }
    o_pos->y = y;
    o_pos->x = x;
    o_pos->z = z;
}
VERIFY(0x02306A40, &daNpc_Zl1_c::set_LightPos);

/* 02306BD8 */
void daNpc_Zl1_c::cut_init_OMAMORI_ONOFF(int i_staffIdx) {
    WWHD_FUNC(0x02306BD8, void, this, i_staffIdx);
    be<s32>* pOnOff = (be<s32>*)dComIfGp_evmng_getMyIntegerP(i_staffIdx, STR(0x1002415C) /* "OnOff" */);
    field_0x7C2 = 0;
    if (pOnOff != nullptr) {
        if (*pOnOff == 0) {
            mDoAud_seStart_id(0x854); /* JA_SE_ITM_OMAMORI_TETLA */
            field_0x7C2 = -1;
            mRatio = 1.0f;
            init_Light();
            gabi::Local<cXyz> pos;
            set_LightPos(pos);
            u32 p = gabi::ea(pos.get());
            for (u32 i = 0; i < 12; i += 4) {
                u32 w = gabi::load<u32>(p + i);
                gabi::store<u32>(gabi::ea(&field_0x830) + i, w);
                gabi::store<u32>(gabi::ea(&mLightInfluence1.mPos) + i, w);
                gabi::store<u32>(gabi::ea(&mLightInfluence2.mPos) + i, w);
            }
        } else {
            field_0x7C2 = 1;
            mLightInfluence1.mPower = 0.0f;
            mRatio = 0.15f;
            mLightInfluence2.mPower = 0.0f;
        }
    }
}
VERIFY(0x02306BD8, &daNpc_Zl1_c::cut_init_OMAMORI_ONOFF);

/* 02307A58 */
void daNpc_Zl1_c::darkProc() {
    WWHD_FUNC(0x02307A58, void, this);
    /* HD: nothing in the stage "Hyrule" */
    if (zl1_isStartStage(STR(0x10024240) /* "Hyrule" */)) {
        return;
    }
    switch ((u32)(s32)field_0x7C2) {
    case 1:
        incEnvironment();
        dKy_set_vrboxcol_ratio(mRatio);
        break;
    case (u32)-1:
        decEnvironment();
        dKy_set_vrboxcol_ratio(mRatio);
        break;
    default:
        mRatio = 1.0f;
        dKy_set_vrboxcol_ratio(1.0f);
        break;
    }
    dKy_set_fogcol_ratio(mRatio);
    f32 temp = (mRatio - 0.15f) * 1.1764705f;
    if (temp > 0.5f) {
        dKy_fog_startendz_set(300.0f, 2000.0f, 2.0f - (temp + temp));
    } else {
        f32 t = temp * 0.85f;
        dKy_set_bgcol_ratio((t + t) + 0.15f);
        dKy_fog_startendz_set(300.0f, gabi::fmadds(3000.0f, temp, 500.0f), 1.0f);
    }
}
VERIFY(0x02307A58, &daNpc_Zl1_c::darkProc);

/* 0230684C */
void daNpc_Zl1_c::cut_init_JMP_OFF(int i_staffIdx) {
    WWHD_FUNC(0x0230684C, void, this, i_staffIdx);
    be<f32>* pSpeed = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x10024128) /* "Speed" */);
    be<f32>* pSpeedY = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x10024130) /* "Spd_y" */);
    be<f32>* pGrvty = (be<f32>*)dComIfGp_evmng_getMyFloatP(i_staffIdx, STR(0x10024138) /* "Grvty" */);
    f32 speedF1 = get_prmFloat(pSpeed, 0.0f);
    f32 speedY = get_prmFloat(pSpeedY, 0.0f);
    f32 grvty = get_prmFloat(pGrvty, 0.0f);
    setAnm_NUM(10, 1);
    field_0x7C8 = false;
    field_0x7CA = false;
    field_0x7CB = true;
    gravity = grvty;
    field_0x7C7 = true;
    speed.y = speedY;
    speedF = speedF1;
    current.angle.y = current.angle.y + 0x4000;
    /* daPy_getPlayerActorClass()->onPlayerNoDraw() */
    u32 pl = gabi::ea(dComIfGp_getPlayer(0));
    gabi::store<u32>(pl + 0x3B8, gabi::load<u32>(pl + 0x3B8) | 0x08000000);
    m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
    field_0x7D3 = false;
    m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
    m_jnt.mbBackBoneLock = 0; /* offBackBoneLock() */
    field_0x84D = 0;
    m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
    m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
    field_0x7D8 = false;
}
VERIFY(0x0230684C, &daNpc_Zl1_c::cut_init_JMP_OFF);

/* 02306F30 */
void daNpc_Zl1_c::privateCut(int i_staffIdx) {
    WWHD_FUNC(0x02306F30, void, this, i_staffIdx);
    /* a_cut_tbl (.data 0x101C74AC): LOK_PLYER, LOK_PARTNER, CHG_ANM_ATR, PLYER_TRN_PARTNER,
     * PLYER_TRN_TETRA, MAJYU_START, MAJYU_END, OKIRU, OKIRU_2, DRW_ONOFF, PLYER_DRW_ONOFF, JMP_OFF,
     * OMAMORI_ONOFF, SURPRISED */
    if (i_staffIdx == -1) {
        return;
    }
    s8 idx = dComIfGp_evmng_getMyActIdx(i_staffIdx, 0x101C74AC, 14, TRUE, 0);
    mActIdx = idx;
    dEvent_manager_c* evmng = dComIfGp_getPEvtManager();
    if (idx == -1) {
        gabi::call(0x02543280, evmng, i_staffIdx); /* cutEnd */
        return;
    }
    if (gabi::call<BOOL>(0x025447C8, evmng, i_staffIdx)) { /* getIsAddvance */
        switch ((u32)(s32)mActIdx) {
        case 0: cut_init_LOK_PLYER(i_staffIdx); break;
        case 1: cut_init_LOK_PARTNER(i_staffIdx); break;
        case 2: cut_init_CHG_ANM_ATR(i_staffIdx); break;
        case 3: cut_init_PLYER_TRN_PARTNER(i_staffIdx); break;
        case 4: cut_init_PLYER_TRN_TETRA(i_staffIdx); break;
        case 5: cut_init_MAJYU_START(i_staffIdx); break;
        case 7: cut_init_OKIRU(i_staffIdx); break;
        case 8: cut_init_OKIRU_2(i_staffIdx); break;
        case 9: cut_init_DRW_ONOFF(i_staffIdx); break;
        case 10: cut_init_PLYER_DRW_ONOFF(i_staffIdx); break;
        case 11: cut_init_JMP_OFF(i_staffIdx); break;
        case 12: cut_init_OMAMORI_ONOFF(i_staffIdx); break;
        case 13: cut_init_SURPRISED(i_staffIdx); break;
        }
    }
    /* the other cut_move_* return TRUE (inlined) */
    u32 end;
    switch ((u32)(s32)mActIdx) {
    case 7: end = cut_move_OKIRU(); break;
    case 8: end = cut_move_OKIRU_2(); break;
    case 11: end = cut_move_JMP_OFF(); break;
    default: end = TRUE; break;
    }
    if (end) {
        dComIfGp_evmng_cutEnd(i_staffIdx);
    }
}
VERIFY(0x02306F30, &daNpc_Zl1_c::privateCut);

/* 023071C4 */
void daNpc_Zl1_c::event_proc(int i_staffIdx) {
    WWHD_FUNC(0x023071C4, void, this, i_staffIdx);
    if (dComIfGp_evmng_endCheck(mEventIdx[field_0x7A8])) {
        switch ((u32)(s32)field_0x7A8) {
        case 0:
            field_0x7D3 = false;
            dComIfGs_onEventBit(0x802);
            field_0x738.z = current.angle.z;
            field_0x738.x = current.angle.x;
            field_0x72C.copy(current.pos);
            field_0x738.y = current.angle.y;
            setStt(1);
            break;
        case 1:
            dComIfGs_onEventBit(0x804);
            field_0x738.z = current.angle.z;
            current.pos.x = -37692.0f;
            current.angle.y = -0x5C72;
            current.pos.z = 8016.0f;
            current.pos.y = 2200.0f;
            field_0x764.y = 2200.0f;
            field_0x738.y = current.angle.y;
            field_0x738.x = current.angle.x;
            field_0x84D = 0;
            field_0x72C.copy(current.pos);
            setStt(8);
            break;
        case 2:
            dComIfGs_onEventBit(0x801);
            field_0x7C9 = false;
            break;
        case 3:
            dComIfGs_onEventBit(0x3804); /* HYRULE_COURTYARD_CUTSCENE */
            setStt(3);
            field_0x7CB = false;
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staffIdx);
        }
    }
}
VERIFY(0x023071C4, &daNpc_Zl1_c::event_proc);

/* 0230634C searchByID(fpc_ProcID, BOOL*) (the matcher calls it cLib_getRndValue<int> of
 * d_a_npc_aj1; not inlined in HD, as in d_a_npc_ls1) */
static fopAc_ac_c* zl1_searchByID(daNpc_Zl1_c* i_this, fpc_ProcID i_PID, be<s32>* o_wasDeleted) {
    WWHD_FUNC(0x0230634C, fopAc_ac_c*, i_this, i_PID, o_wasDeleted);
    gabi::Local<gptr<fopAc_ac_c>> actor_p;
    *actor_p = nullptr;
    *o_wasDeleted = 0;
    if (!gabi::call<BOOL>(0x025D54C4, i_PID, actor_p.get())) { /* fopAcM_SearchByID(id, &actor) */
        *o_wasDeleted = 1;
    }
    return *actor_p;
}
/* verified in d_a_npc_zl1.cpp (daNpc_Zl1_searchByID) */

/* 023075B0 */
void daNpc_Zl1_c::lookBack() {
    WWHD_FUNC(0x023075B0, void, this);
    gabi::Local<cXyz> temp3;
    gabi::Local<cXyz> temp; /* passed by value: a copy */
    s16 head_y = m_jnt.mAngles[0][1];
    u8 temp5 = field_0x7D8;
    temp3->x = 0.0f;
    s16 targetY = current.angle.y;
    s16 backbone_y = m_jnt.mAngles[1][1];
    temp3->z = 0.0f;
    temp3->y = 0.0f;
    field_0x794.x = targetY;
    s8 temp2 = field_0x84D;
    f32 srcZ = current.pos.z;
    field_0x794.y = head_y;
    f32 srcX = current.pos.x;
    f32 srcY = eyePos.y;
    field_0x794.z = backbone_y;
    cXyz* temp4 = nullptr;
    if (field_0x849 == 0xD || field_0x849 == 0xE) {
        temp2 = 0;
    }
    switch ((u32)(s32)temp2) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        field_0x7D0 = true;
        field_0x758.copy(*eye);
        temp3->copy(*eye);
        temp4 = temp3;
        break;
    }
    case 2:
        temp3->copy(field_0x758);
        temp4 = temp3;
        break;
    case 3:
        targetY = field_0x7C0;
        field_0x7D0 = false;
        break;
    case 4: {
        gabi::Local<be<s32>> wasDeleted;
        fopAc_ac_c* actor = zl1_searchByID(this, mProcId2, wasDeleted);
        if (actor != nullptr && *wasDeleted == 0) {
            field_0x758.copy(actor->current.pos);
            f32 x = field_0x758.x;
            f32 z = field_0x758.z;
            f32 y = actor->eyePos.y;
            temp3->z = z;
            temp3->y = y;
            field_0x758.y = y;
            temp3->x = x;
            field_0x7D0 = true;
            temp4 = temp3;
        }
        break;
    }
    case 5:
        kyorokyoro();
        temp3->copy(field_0x758);
        field_0x7D0 = true;
        temp4 = temp3;
        break;
    default:
        field_0x7D0 = false;
        break;
    }
    temp->x = srcX;
    temp->z = srcZ;
    temp->y = srcY;
    dNpc_JntCtrl_lookAtTarget_2(&m_jnt, &current.angle.y, temp4, temp, targetY, l_HIO().mPrmTbl.field_18, temp5);
    s16 hx = (s16)(m_jnt.mAngles[0][0] / 2);
    field_0x83E = hx;
    s16 hy = (s16)(m_jnt.mAngles[0][1] / 2);
    field_0x842 = hx;
    field_0x840 = hy;
    field_0x83C = hy;
}
VERIFY(0x023075B0, &daNpc_Zl1_c::lookBack);

/* 02307D18 */
BOOL daNpc_Zl1_c::_execute() {
    WWHD_FUNC(0x02307D18, BOOL, this);
    if (!field_0x7D5) {
        field_0x72C.copy(current.pos);
        field_0x738.x = current.angle.x;
        field_0x738.y = current.angle.y;
        field_0x738.z = current.angle.z;
        field_0x7D5 = true;
    }
    daNpc_Zl1_HIO_c::hio_prm_c& prm = l_HIO().mPrmTbl;
    s16 temp = field_0x7BC < 0 ? (s16)prm.mMaxTurnStep : (s16)prm.field_5C;
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, temp);
    if (field_0x7D2 && demoActorID == 0) {
        return TRUE;
    }
    partner_search();
    checkOrder();
    if (!demo()) {
        s32 staff_id = -1;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            staff_id = isEventEntry();
        }
        if (staff_id >= 0 || field_0x7C9) {
            event_proc(staff_id);
        } else {
            pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        lookBack();
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        u32 flags = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28);
        field_0x7C7 = (flags >> 5) & 1;  /* ChkGroundHit() */
        field_0x7C8 = (flags >> 12) & 1; /* ChkWaterIn() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        play_animation();
    } else {
        /* HD (as GameCube USA/PAL): CrrPos and the ground/water flags while in a demo */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        u32 flags = gabi::load<u32>(gabi::ea(&mObjAcch) + 0x28);
        field_0x7D2 = false;
        field_0x7C7 = (flags >> 5) & 1;
        field_0x7C8 = (flags >> 12) & 1;
    }
    eventOrder();
    field_0x73E.x = current.angle.x;
    field_0x73E.y = current.angle.y;
    field_0x73E.z = current.angle.z;
    if (!field_0x7D3) {
        shape_angle.z = current.angle.z;
        shape_angle.y = current.angle.y;
        shape_angle.x = current.angle.x;
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), daNpc_gndPoly(this)));
    darkProc();
    setMtx(false);
    if (!field_0x7D9 && !field_0x7CA && !field_0x7CB) {
        setCollision(50.0f, 140.0f);
    }
    return TRUE;
}
VERIFY(0x02307D18, &daNpc_Zl1_c::_execute);

static inline u32 selfrel(u32 base) { u32 off = gabi::load<u32>(base); return off != 0 ? base + off : 0; }
/* SafeString equality of a literal with the string at `other` (HD inline: the TU's
 * assureTerminationImpl_ (0230A308) called directly, then the virtual twice / once) */
static bool zl1_safestring_equal(u32 lit, u32 other) {
    gabi::Local<SafeString> b;
    b->__vtbl = ZL1_SAFESTRING_VTBL;
    b->mStringTop = other;
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    gabi::call(0x0230A308, a.get()); /* sead::SafeString::assureTerminationImpl_ (empty) */
    ss_vcall(a);
    u32 pa = a->mStringTop;
    ss_vcall(b);
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

/* 02305778 */
u8 daNpc_Zl1_c::demo() {
    WWHD_FUNC(0x02305778, u8, this);
    u8 id = demoActorID;
    if (id == 0) {
        if (field_0x7D9) {
            field_0x7D9 = FALSE;
        }
        return field_0x7D9;
    }
    if (!field_0x7D9) {
        for (int i = 0; i < 2; i++) {
            if (field_0x6D4[i].get() != nullptr) {
                field_0x6D4[i]->field_0x7C = 0; /* clrMoveFlag() */
            }
        }
        m_jnt.mAngles[0][1] = 0; /* setHead_y(0) */
        m_jnt.mAngles[0][0] = 0; /* setHead_x(0) */
        m_jnt.mAngles[1][1] = 0; /* setBackBone_y(0) */
        field_0x7D9 = TRUE;
        field_0x7D3 = false;
        id = demoActorID;
        m_jnt.mAngles[1][0] = 0; /* setBackBone_x(0) */
    }
    /* dComIfGp_demo_getActor(demoActorID) (HD inline with a range check) */
    void* demo_actor = nullptr;
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x10023C60), 0x23A, STR(0x10023C38));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor = gabi::call<void*>(0x02526E70, obj, id);
    }
    void* btp = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtpAnm) + 0x10)); /* mBtpAnm.getBtpAnm() */
    if (btp != nullptr) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btp);
        u8 frame = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor != nullptr) {
        void* demo_btp = gabi::call<void*>(0x02527828, demo_actor, mArcName); /* getP_BtpData */
        if (demo_btp != nullptr) {
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            mDoExt_btpAnm_init(mBtpAnm, md, demo_btp, 1, 0, 1.0f, 0, -1, 1, 0);
            /* HD: the second btp animation, and the eye materials ("zelda_mouth") linked
             * through the animation's material table */
            mDoExt_btpAnm_init(mBtpAnm2, md, demo_btp, 1, 0, 1.0f, 0, -1, 1, 0);
            u32 anm2 = gabi::load<u32>(gabi::ea(mBtpAnm2) + 0x60);
            u16 n = gabi::load<u16>(anm2 + 0x16);
            u32 arrp = gabi::ea(mBtpAnm2) + 0x54;
            for (u32 i = 0; i < n; i++) {
                u32 name = selfrel(selfrel(anm2 + 0x2C) + i * 0x1C + 0xC);
                gabi::Local<SafeString> s1;
                s1->__vtbl = ZL1_SAFESTRING_VTBL;
                s1->mStringTop = name;
                u32 h = gabi::call<u32>(0x027F3F8C, md); /* the material name table header */
                ss_vcall(s1);
                s32 idx = gabi::call<s32>(0x027DF9B0, selfrel(h + 0x18), (u32)s1->mStringTop); /* JUTNameTab::getIndex */
                gabi::Local<SafeString> s2;
                s2->mStringTop = 0x10024050; /* "zelda_mouth" */
                s2->__vtbl = ZL1_SAFESTRING_VTBL;
                ss_vcall(s1);
                ss_vcall(s1);
                u32 pa = s1->mStringTop;
                ss_vcall(s2);
                u32 pb = s2->mStringTop;
                bool equal = true;
                if (pa != pb) {
                    equal = false;
                    for (u32 k = 0; k < 0x40001; k++) {
                        u8 ca = gabi::load<u8>(pa + k);
                        u8 cb = gabi::load<u8>(pb + k);
                        if (ca != cb) break;
                        if (ca == 0) { equal = true; break; }
                    }
                }
                u32 arr = gabi::load<u32>(arrp);
                u32 w = gabi::load<u32>(arr + i * 4);
                if (!equal) {
                    gabi::store<u32>(arr + i * 4, w & 0x3FFF8000);
                    arr = gabi::load<u32>(arrp);
                    u32 j = (u32)idx + 1;
                    gabi::store<u32>(arr + i * 4, gabi::load<u32>(arr + i * 4) | (j & 0x7FFF));
                    arr = gabi::load<u32>(arrp);
                    gabi::store<u32>(arr + j * 4, gabi::load<u32>(arr + j * 4) & 0xC0007FFF);
                    arr = gabi::load<u32>(arrp);
                    gabi::store<u32>(arr + j * 4, gabi::load<u32>(arr + j * 4) | ((i << 15) & 0x3FFF8000));
                } else {
                    gabi::store<u32>(arr + i * 4, w | 0xC0007FFF);
                    arr = gabi::load<u32>(arrp);
                    u32 j = (u32)idx;
                    gabi::store<u32>(arr + j * 4, gabi::load<u32>(arr + j * 4) | 0x3FFF8000);
                }
            }
            mBtpAnmFrame = 0;
            field_0x847 = 0x11;
        }
    }
    void* btk = gabi::at<u8>(gabi::load<u32>(gabi::ea(mBtkAnm) + 0x68)); /* mBtkAnm.getBtkAnm() */
    if (btk != nullptr) {
        u8 frame_max = (u8)J3DAnm_getFrameMax(btk);
        u8 frame = (u8)(mBtkAnmFrame + 1);
        mBtkAnmFrame = frame < frame_max ? frame : frame_max;
    }
    if (demo_actor != nullptr) {
        void* demo_btk = gabi::call<void*>(0x025279C8, demo_actor, mArcName); /* getP_BtkData */
        if (demo_btk != nullptr) {
            mDoExt_btkAnm_init(mBtkAnm, J3DModel_getModelData_l(mpMorf->getModel()), demo_btk, 1, 0, 1.0f, 0, -1, 1, 0);
            mBtkAnmFrame = 0;
            field_0x848 = 10;
        }
        if (field_0x7CE) {
            u32 da = gabi::ea(demo_actor);
            f32 x = gabi::load<f32>(da + 8); /* *demo_actor->getTrans() */
            f32 y = gabi::load<f32>(da + 0xC);
            f32 z = gabi::load<f32>(da + 0x10);
            if (gabi::ftoi(x) != 0 || gabi::ftoi(y) != 0 || gabi::ftoi(z) != 0) {
                field_0x7CE = false;
                mpModel = nullptr;
            }
        }
    }
    /* HD: in the stage "Demo17" after frame 0x606 the demo actor's flag 0x40 is cleared and the
     * animation runs at 1.5x */
    if (zl1_safestring_equal(0x10024038 /* "Demo17" */, 0x1047E6B8 /* start stage name */) &&
        gabi::load<u32>(0x101D600C) >= 0x606) {
        if (demo_actor != nullptr) {
            u32 da = gabi::ea(demo_actor);
            gabi::store<u16>(da + 4, gabi::load<u16>(da + 4) & 0xFFBF);
        }
        mpMorf->setPlaySpeed(1.5f);
    }
    gabi::Local<cXyz> pos; /* passed by value */
    f32 z = current.pos.z;
    pos->z = z;
    f32 y = current.pos.y;
    pos->y = y;
    pos->x = current.pos.x;
    u32 snd = gabi::call<u32>(0x024F1914, pos.get(), 10.0f); /* dBgS_GetGndMtrlSndId_Func(pos, 10.0f) */
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    dDemo_setDemoData(this, 0x6A, mpMorf, mArcName, 0, nullptr, snd, (s8)reverb);
    /* HD: type 6 hides Tetra in parts of the demos "Demo05" and "Demo23" */
    if (field_0x84F == 6) {
        if (zl1_safestring_equal(0x10024040 /* "Demo05" */, 0x1047E6B8)) {
            u32 f = gabi::load<u32>(0x101D600C);
            field_0x7D4 = (u32)(f - 0x492) < 0x182;
        } else if (zl1_safestring_equal(0x10024048 /* "Demo23" */, 0x1047E6B8)) {
            u32 f = gabi::load<u32>(0x101D600C);
            field_0x7D4 = (u32)(f - 0xD66) < 0xB;
        }
    }
    return field_0x7D9;
}
VERIFY(0x02305778, &daNpc_Zl1_c::demo);
