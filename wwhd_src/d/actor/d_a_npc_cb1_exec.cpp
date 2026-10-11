/**
 * d_a_npc_cb1_exec.cpp (WWHD)
 * NPC - Makar (Korok cellist): execute, orders, animation, talk, fly/walk movement
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1_execute.h"

using namespace cb1_execute;
static cb1_execute::ActionPtmf cbExec_waitNpcAction;
static cb1_execute::ActionPtmf cbExec_searchNpcAction;
static cb1_execute::ActionPtmf cbExec_carryNpcAction;
static cb1_execute::ActionPtmf cbExec_waitPlayerAction;
static cb1_execute::ActionPtmf cbExec_flyPlayerAction;
static cb1_execute::ActionPtmf cbExec_flyNpcAction;
static cb1_execute::ActionPtmf cbExec_hitNpcAction;
static cb1_execute::ActionPtmf cbExec_talkNpcAction;
static cb1_execute::ActionPtmf cbExec_carryPlayerAction;
static cb1_execute::ActionPtmf cbExec_hitPlayerAction;
// HD: these aliases refer to the original shared guest storage.
static cb1_execute::daNpc_Cb1_HIO_c cbExec_HIO;
static u16 cbExec_status;
static s16 cbExec_flyingTimer;
static u8 cbExec_flying;
static cb1_execute::AnmTable cbExec_anmTable;
static cXyz cbExec_zero;
static cb1_execute::AnmTypeTable cbExec_anmTypes;
static cb1_execute::AnmParameterTable cbExec_anmParameters;
static inline cb1_execute::daNpc_Cb1_HIO_c &cb1_HIO() { return cbExec_HIO; }
static inline be<u16> &cb1_m_status() { return *gabi::at<be<u16>>(gabi::ea(&cbExec_status)); }
static inline be<s16> &cb1_m_flyingTimer() {
    return *gabi::at<be<s16>>(gabi::ea(&cbExec_flyingTimer));
}
static inline void cb1_offFlying() { cbExec_flying = 0; }
static inline void cb1_onFlying() { cbExec_flying = 1; }

/* GHS pointer-to-member equality with a plain (non-virtual) member: index first */
static inline BOOL cb1_pmfIs(ProcFunc_l *p, u32 f) {
    if (p->i != -1) {
        return FALSE;
    }
    if (p->i == 0) {
        return TRUE;
    }
    if (p->d != 0) {
        return FALSE;
    }
    return p->f == f;
}
static inline void cb1_pmfSet(ProcFunc_l *p, u32 f) {
    p->d = 0;
    p->i = -1;
    p->f = f;
}

// Private bindings preserve HD argument and result widths.
/* calls into the other parts of this unit */
static inline void cb1_setNpcAction(daNpc_Cb1_c *a, ProcFunc_l *f, void *arg) {
    a->setNpcAction(f, arg);
}
// HD: sound binding receives the actor ID explicitly.
static inline void cb1_monsSeStart(fopAc_ac_c *a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    CbExec_monsSeStart(id, &a->eyePos, (u32)a->mBsPcId, param, reverb);
}
// HD: dBgS::GetSpecialCode(cBgS_PolyInfo&)
static inline s32 cb1_GetSpecialCode(void *poly) {
    return CbExec_getSpecialCode(dComIfG_Bgsp(), poly);
}
// HD: dPa_control_c::setSimpleLand(poly, pos, angle, f32, f32, f32, tevstr, int*, int)
static inline void cb1_setSimpleLand(void *poly, cXyz *pos, csXyz *ang, f32 a, f32 b, f32 c,
                                     void *tev, s32 *out, s32 n) {
    CbExec_setSimpleLand(gabi::ea(dComIfGp_getParticle()), poly, pos, ang, a, b, c, tev, out, n);
}
// HD: dNpc_setAnm(morf, loopMode, morf, speed, file, sound, arc)
static inline void cb1_dNpc_setAnm(mDoExt_McaMorf *m, s32 loop, f32 morf, f32 speed, s32 file,
                                   s32 snd, u32 arc) {
    CbExec_setAnm(m, loop, morf, speed, file, snd, arc);
}
// Cello practice and boss music use different sound entry points.
static inline void cb1_cbPracticeStop() { CbExec_practiceStop(); }
static inline void cb1_cbPracticePlay(cXyz *p) { CbExec_practicePlay(p); }
static inline void cb1_bgmStart(u32 id) { CbExec_bgmStart(id); }
// HD: daPy_py_c::changePlayer(fopAc_ac_c*)
static inline void cb1_changePlayer(fopAc_ac_c *self, fopAc_ac_c *a) {
    CbExec_changePlayer(self, a);
}

/* mAcch.m_gnd as a cBgS_PolyInfo (the dBgS_GndChk polygon info at +0x14) */
// HD: save accessors reread the singleton on every use.
static inline BOOL cb1_isEventBit(u16 f) { return dComIfGs_isEventBit(f); }
static inline void cb1_onEventBit(u16 f) { dComIfGs_onEventBit(f); }
static inline u8 cb1_event_runCheck() {
    return (u8) reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mEvtCtrl.mMode;
}
/* dAttention_c (play + 0x5804) */
static inline u32 cb1_attention() {
    return gabi::ea(&reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mAttention);
}
static inline u32 cb1_GetLockonList(u32 at, s32 i) { return CbExec_getLockonList(at, i); }
static inline u32 cb1_GetActionList(u32 at, s32 i) { return CbExec_getActionList(at, i); }
static inline BOOL cb1_LockonTruth(u32 at) { return CbExec_lockonTruth(at); }
static inline u32 cb1_getActionBtnB(u32 at) { return CbExec_getActionBtnB(at); }
static inline fopAc_ac_c *cb1_AttList_getActor(u32 l) { return CbExec_getAttentionActor(l); }
// HD: dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16,
// bool)
static inline void cb1_lookAtTarget(dNpc_JntCtrl_c *j, be<s16> *outY, cXyz *target, cXyz *eye,
                                    s16 yrot, s16 vel, BOOL headOnly) {
    CbExec_lookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
}

static inline u8 *cb1_gnd(daNpc_Cb1_c *a) {
    return reinterpret_cast<u8 *>(static_cast<cBgS_PolyInfo *>(&a->mAcch.m_gnd));
}

/* 0221FBD4 */
void daNpc_Cb1_c::setWaitNpcAction(void *arg) {

    u16 st = cb1_m_status();
    ProcFunc_l fn;
    fn.d = 0;
    fn.i = -1;
    if (st & daCbStts_SHIP_RIDE) {
        fn.f = CB1_shipNpcAction;
    } else {
        fn.f = (st & daCbStts_MUSIC) ? CB1_musicNpcAction : CB1_waitNpcAction;
    }
    cb1_setNpcAction(this, &fn, nullptr);
}

/* 0221FC6C */
BOOL daNpc_Cb1_c::isFlyAction() {

    return cb1_pmfIs(&mPlayerAction, CB1_flyPlayerAction) ||
           cb1_pmfIs(&mNpcAction, CB1_flyNpcAction);
}

/* 0221FCE4 */
void daNpc_Cb1_c::checkLanding() {

    gabi::Local<s32> temp;
    if (m900 > 200.0f) {
        if (cb1_GetSpecialCode(cb1_gnd(this)) != 1) {
            cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_LANDING, 0);
        }
        cb1_setSimpleLand(cb1_gnd(this), &current.pos, &shape_angle, 1.25f, 1.5f, 1.0f, &tevStr,
                          temp, 7);
    } else {
        cb1_setSimpleLand(cb1_gnd(this), &current.pos, &shape_angle, 0.625f, 0.75f, 0.5f, &tevStr,
                          temp, 7);
    }
}

/* 0221FDF0 */
void daNpc_Cb1_c::setCollision() {

    mCyl.SetC(&current.pos);
    mCyl.SetR(20.0f);
    mCyl.SetH(60.0f);
    dComIfG_Ccsp_Set(&mCyl);
    mWindCyl.SetC(&current.pos);
    mWindCyl.SetR(20.0f);
    mWindCyl.SetH(60.0f);
    dComIfG_Ccsp_Set(&mWindCyl);
}

/* 0221FECC */
void daNpc_Cb1_c::musicStop() {

    if (fopAcM_GetParam(this) != 5) { /* !isTypeKazeBoss() */
        cb1_cbPracticeStop();
    }
}

/* 0221FEDC */
void daNpc_Cb1_c::musicPlay() {

    if (fopAcM_GetParam(this) != 5) {
        cb1_cbPracticePlay(&eyePos);
    } else {
        cb1_bgmStart(cb1_execute::JA_BGM_MAKORE_TAKT_8);
    }
}

/* 0221FEFC */
void daNpc_Cb1_c::initAnm(s8 param_1, BOOL param_2) {

    if (param_1 >= 0) {
        m8DC = param_1;
        cb1_execute::AnmData &data =
            *gabi::at<cb1_execute::AnmData>(gabi::ea(&cbExec_anmTable) + param_1 * 6);
        m8B0 = 0.0f;
        f32 morf = 0.0f;
        if (param_2) {
            morf = (f32)(s8)data.field_0x02;
        }
        s8 file = data.mAnmFileIdx;
        cb1_dNpc_setAnm(mpMorf.get(), data.mLoopMode, morf, (f32)(s8)data.mSpeed,
                        file < 0 ? -file : file, -1, gabi::guest_str("Cb"));
        m8D7 = 0;
        m8D8 = 0;
        if (data.mAnmFileIdx < 0) {
            cb1_m_status() = cb1_m_status() | daCbStts_MUSIC;
        } else {
            cb1_m_status() = cb1_m_status() & ~daCbStts_MUSIC;
        }
    }
}

/* 02220024 */
void daNpc_Cb1_c::playAnm() {

    BOOL r3;
    if ((cb1_m_status() & daCbStts_UNK_0100) && mAcch.ChkGroundHit()) {
        u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo *)cb1_gnd(this));
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        r3 = mpMorf->play(&eyePos, snd, reverb);
    } else {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        r3 = mpMorf->play(&eyePos, 0, reverb);
    }
    if (r3 || mpMorf->getFrame() < m8B0) {
        m8D7 = 1;
        m8D8 = (s8)(m8D8 + 1);
        cb1_execute::AnmData &data =
            *gabi::at<cb1_execute::AnmData>(gabi::ea(&cbExec_anmTable) + (s8)m8DC * 6);
        s8 temp = data.field_0x04;
        if (fopAcM_GetParam(this) == 5 && temp == 0x18) { /* isTypeKazeBoss() */
            temp = 0xD;
        }
        if (m8D8 >= temp) {
            if (data.field_0x05 == 0x13) {
                musicStop();
            } else if (data.field_0x05 == 0x11) {
                musicPlay();
            }
            initAnm(data.field_0x05, 1);
        }
    }
    m8B0 = mpMorf->getFrame();
}

/* 02220194 */
BOOL daNpc_Cb1_c::checkCommandTalk() {

    if (eventInfo.checkCommandTalk()) { /* eventInfo.checkCommandTalk() */
        if ((u32)((u8) reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())
                      ->play.mEvtCtrl.mTalkButton -
                  1) <= 3) { /* dComIfGp_event_chkTalkXY() */
            if (m8DD == 7) {
                m8DD = -1;
            }
            cb1_m_status() = cb1_m_status() | daCbStts_TACT; /* onTact() */
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/* 02220238 */
void daNpc_Cb1_c::returnLinkPlayer() {

    cb1_changePlayer(this, dComIfGp_getLinkPlayer());
    cb1_offFlying();
    setWaitNpcAction(nullptr);
}

/* 02221958 */
u8 daNpc_Cb1_c::getAnmType(int param_1) {

    /* l_talkAnmType[11] */
    if ((u32)param_1 < 11) {
        return (u8)cbExec_anmTypes.mData[param_1];
    }
    return (u8)cbExec_anmTypes.mData[0];
}

/* 02221978 */
BOOL daNpc_Cb1_c::setAnm(u8 param_1) {

    if (m8DB == param_1) {
        return FALSE;
    }
    if (m8DB == 9) {
        musicStop();
    }
    m8DB = param_1;
    if (param_1 == 9) {
        actor_status = actor_status | 0x20000; /* fopAcM_OnStatus(fopAcStts_NOPAUSE_e) */
    } else {
        actor_status = actor_status & ~0x20000u;
    }
    /* HD: bounds check on anmPrmData */
    if (param_1 < 0x18) {
        initAnm((s8)cbExec_anmParameters.mData[param_1], 1);
    }
    return TRUE;
}

/* 02221A3C */
void daNpc_Cb1_c::setMessageAnimation(u8 param_1) {

    if (!(actor_status & 0x2000) /* !fopAcM_checkCarryNow(this) */ &&
        (u8) reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mEvtCtrl.mMode !=
            0 /* dComIfGp_event_runCheck() */) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId("Cb1", nullptr, 0);
        if (staffIdx != -1) {
            u32 name = CbExec_getMyActName(dComIfGp_getPEvtManager(), staffIdx); /* getMyActName */
            /* HD: null check on the act name */
            if (name != 0) {
                u32 p = name, q = gabi::guest_str("WAIT"); /* "WAIT" */
                u8 a, b;
                for (;;) {
                    a = (u8)*gabi::at<be<u8>>(p++);
                    b = (u8)*gabi::at<be<u8>>(q++);
                    if (a != b || a == 0) {
                        break;
                    }
                }
                if (a == b) {
                    setAnm(getAnmType(param_1));
                }
            }
        }
    }
}

/* 02221B18 */
BOOL daNpc_Cb1_c::calcFlyingTimer() {

    if (cb1_m_flyingTimer() != 0 &&
        CbExec_calcTimerS16(&cb1_m_flyingTimer()) == 0) { /* cLib_calcTimer<s16> */
        return TRUE;
    }
    return FALSE;
}

// HD: (unnamed; called from 026552B8): chkPlayerAction(jumpPlayerAction) ||
// chkPlayerAction(flyPlayerAction)
/* 02221B68 */
static BOOL daNpc_Cb1_chkJumpFlyPlayerAction(daNpc_Cb1_c *i_this) {

    return cb1_pmfIs(&i_this->mPlayerAction, CB1_jumpPlayerAction) ||
           cb1_pmfIs(&i_this->mPlayerAction, CB1_flyPlayerAction);
}

/* 02222020 */
void daNpc_Cb1_c::setWaitAction(void *param_1) {

    if (dComIfGp_getPlayer(0) == this) { /* dComIfGp_getPlayer(0) == this */
        ProcFunc_l fn;
        md_pmf_load(&fn, gabi::ea(&cbExec_waitPlayerAction)); /* &daNpc_Cb1_c::waitPlayerAction */
        setPlayerAction(&fn, nullptr);
    } else {
        setWaitNpcAction(param_1);
    }
}

/* 022220A8 */
s16 daNpc_Cb1_c::getStickAngY() {

    u32 camera = gabi::ea(dComIfGp_getCamera(0)); /* dComIfGp_getCamera(0) */
    s32 stick = CbExec_stickAngle(0);             /* CPad_GET_STICK_ANGLE(0) */
    s32 cam = CbExec_controlledAngleY(camera);    /* dCam_getControledAngleY */
    return (s16)(0x8000 + stick + cam);
}

/* 02222234 */
BOOL daNpc_Cb1_c::flyCheck() {

    if (!CbExec_trigA(0)) { /* CPad_CHECK_TRIG_A(0) */
        return FALSE;
    }
    ProcFunc_l fn;
    md_pmf_load(&fn, gabi::ea(&cbExec_flyPlayerAction)); /* &daNpc_Cb1_c::flyPlayerAction */
    setPlayerAction(&fn, nullptr);
    return TRUE;
}

/* 022222AC */
f32 daNpc_Cb1_c::breaking() {

    return CbExec_addCalc(&speedF, 0.0f, (f32)cb1_HIO().mDecelScale, (f32)cb1_HIO().mMaxDecel,
                          (f32)cb1_HIO().mDecel); /* cLib_addCalc */
}

/* 022205DC */
void daNpc_Cb1_c::setPlayerAction(ProcFunc_l *param_1, void *param_2) {

    mNpcAction.d = 0;
    mNpcAction.i = 0;
    mNpcAction.f = 0;
    ProcFunc_l fn; /* by-value copy */
    md_pmf_load(&fn, gabi::ea(param_1));
    setAction(&mPlayerAction, (&fn), param_2); /* setAction */
}

/* 0222062C */
void daNpc_Cb1_c::playerAction(void *param_1) {

    if (mPlayerAction.i == 0) {
        speedF = 0.0f;
        ProcFunc_l fn;
        md_pmf_load(&fn, gabi::ea(&cbExec_waitPlayerAction)); /* &daNpc_Cb1_c::waitPlayerAction */
        setPlayerAction(&fn, nullptr);
    }
    dComIfGp_setDoStatus(0x23); /* dComIfGp_setDoStatus(dActStts_FLY_e) */
    md_pmf_call<BOOL>(this, &mPlayerAction, param_1);
}

/* 022206F0 */
void daNpc_Cb1_c::carryCheck() {

    if (actor_status & 0x2000) { /* fopAcM_checkCarryNow(this) */
        ProcFunc_l fn;
        md_pmf_load(&fn, gabi::ea(&cbExec_carryNpcAction)); /* &daNpc_Cb1_c::carryNpcAction */
        cb1_setNpcAction(this, &fn, nullptr);
    }
}

/* 02220738 */
void daNpc_Cb1_c::checkOrder() {

    if (eventInfo.checkCommandTalk()) { /* eventInfo.checkCommandTalk() */
        if (m8DD == 5 || m8DD == 6 || m8DD == 7) {
            m8DD = -1;
            if ((u32)((u8) reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())
                          ->play.mEvtCtrl.mTalkButton -
                      1) <= 3) { /* dComIfGp_event_chkTalkXY() */
                cb1_m_status() = cb1_m_status() | daCbStts_TACT;
            } else {
                ProcFunc_l fn;
                md_pmf_load(&fn, gabi::ea(&cbExec_talkNpcAction)); /* &daNpc_Cb1_c::talkNpcAction */
                cb1_setNpcAction(this, &fn, nullptr);
            }
            fopAcM_cancelCarryNow(this);
        }
    }
}

/* 022207E4 */
void daNpc_Cb1_c::npcAction(void *param_1) {

    if (mNpcAction.i == 0) {
        speedF = 0.0f;
        setWaitNpcAction(nullptr);
    }
    md_pmf_call<BOOL>(this, &mNpcAction, param_1);
}

/* 02220884 */
void daNpc_Cb1_c::eventOrder() {

    if (m8DD == 5 || m8DD == 6) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (m8DD == 5) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (m8DD == 7) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALKITEM_e | dEvtCnd_CANTALK_e */
    } else if (m8DD != -1 && m8DD < 5) {
        m8E3 = m8DD;
        if (m8E3 != -1 && mEventIdx[(s8)m8E3] != -1) {
            fopAcM_orderOtherEventId(this, mEventIdx[(s8)m8E3], 0xFF, 0xFFFF, 0, 1);
        }
    }
}

/* 02221BD0 */
BOOL daNpc_Cb1_c::chkAttention(f32 param_1, s32 param_2) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - current.pos.z;
    f32 dx = player->current.pos.x - current.pos.x;
    f32 diff = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 angle = cM_atan2s(dx, dz);
    if (mHasAttention) {
        param_1 += 40.0f;
        param_2 += 0x71C;
    }
    s16 temp2 = shape_angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1];
    angle -= temp2;
    bool result = false;
    if (abs(angle) < param_2 && param_1 > diff) {
        result = true;
    }
    return result;
}

/* 02221CF8 */
u16 daNpc_Cb1_c::next_msgStatus(be<u32> *pMsgNo) {

    u32 no = *pMsgNo;
    if (no == 0x5AC) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    } else if (no == 0x1520 || (0x1519 <= no && no <= 0x151D) || no == 0x1523 ||
               (0x14C2 <= no && no <= 0x14C3)) {
        *pMsgNo = no + 1;
    } else if (no == 0x151E) {
        cb1_onEventBit(0x1880);
        return 0x10;
    } else if (no == 0x1521) {
        cb1_onEventBit(0x1840);
        return 0x10;
    } else if (no == 0x14C4) {
        cb1_onEventBit(0x1904);
        return 0x10;
    } else {
        return 0x10;
    }
    return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
}

/* 02221DE4 */
u32 daNpc_Cb1_c::getMsg() {

    u32 prm = fopAcM_GetParam(this);
    if (prm == 0) { /* isTypeBossDie() */
        return 0x1456;
    }
    if (prm == 1) { /* isTypeForest() */
        return cb1_isEventBit(0x1904) ? 0x14C5 : 0x14C2;
    }
    if (prm == 2) { /* isTypeWaterFall() */
        if (cb1_m_status() & daCbStts_TACT) {
            return cb1_isEventBit(0x1840) ? 0x1522 : 0x1520;
        }
        return cb1_isEventBit(0x1880) ? 0x151F : 0x1519;
    }
    if (cb1_m_status() & daCbStts_SHIP_RIDE) {
        return 0x152F;
    }
    return 0;
}

/* 02221EF8 */
void daNpc_Cb1_c::lookBack(BOOL param_1) {

    cXyz *dstPos = nullptr;
    cXyz vec2;
    cXyz vec;
    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = 0.0f;
    s16 desiredY = shape_angle.y;
    if (mJntCtrl.mbTrn) { /* trnChk() */
        cLib_addCalcAngleS2(&m8D0, cb1_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
    } else {
        m8D0 = 0;
    }
    if (param_1) {
        cXyz eye;
        dNpc_playerEyePos(&eye, cb1_HIO().mNpc.m04);
        vec2 = eye;
        dstPos = &vec2;
        vec.x = current.pos.x;
        vec.y = eyePos.y;
        vec.z = current.pos.z;
    }
    cXyz eyeCopy;
    eyeCopy = vec;
    cb1_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, &eyeCopy, desiredY, m8D0, 0);
}

/* 022220FC */
int daNpc_Cb1_c::calcStickPos(s16 param_1, cXyz *param_2) {

    u32 attention = cb1_attention();
    u32 attList = cb1_GetLockonList(attention, 0);
    /* attention.Lockon(): LockonTruth() || (flags & 0x20000000) */
    bool r26 = cb1_LockonTruth(attention) ||
               ((u32)gabi::at<cb1_execute::Attention>(attention)->mFlags & 0x20000000);
    int r31 = !r26 ? 0 : (cb1_LockonTruth(attention) ? 1 : -1);
    if (attList == 0) {
        attList = cb1_GetActionList(attention, 0);
    }
    if (attList) {
        fopAc_ac_c *a = cb1_AttList_getActor(attList);
        *param_2 = a->eyePos;
        return r31;
    }
    if (r26) {
        param_1 = shape_angle.y;
    }
    param_2->x = gabi::fmadds(100.0f, cM_ssin(param_1), current.pos.x);
    param_2->y = current.pos.y;
    param_2->z = gabi::fmadds(100.0f, cM_scos(param_1), current.pos.z);
    return r31;
}

/* 02222DD0 */
BOOL daNpc_Cb1_c::walkAction(f32 targetSpeed, f32 accel, s16 targetAngle) {

    cLib_chaseAngleS(&current.angle.y, targetAngle, 0x400);
    lookBack(1);
    shape_angle.y = current.angle.y;
    if (cLib_chaseF(&speedF, targetSpeed, accel) && targetSpeed == 0.0f) {
        ProcFunc_l fn;
        md_pmf_load(&fn, gabi::ea(&cbExec_waitNpcAction)); /* &daNpc_Cb1_c::waitNpcAction */
        cb1_setNpcAction(this, &fn, nullptr);
        return TRUE;
    }
    f32 playSpeed = fabsf(speedF) * cb1_HIO().mWalkAnmSpeedScale;
    f32 mx = cb1_HIO().mMaxWalkAnmSpeed;
    playSpeed = (playSpeed - mx >= 0.0f) ? playSpeed : mx; /* cLib_minLimit (fsel) */
    mpMorf->setPlaySpeed(playSpeed);
    return FALSE;
}

/* 02222EF4 */
BOOL daNpc_Cb1_c::sowCheck() {

    if (!cb1_event_runCheck() && !(m4E4 & 1) /* !isReturnLink() */) {
        u32 list = cb1_getActionBtnB(cb1_attention());
        /* HD: null check on the list's actor */
        fopAc_ac_c *actor;
        if (list && (actor = cb1_AttList_getActor(list)) != nullptr) {
            u32 type = (u32)gabi::at<cb1_execute::AttentionList>(list)->mType;
            if ((type == 3 || type == 1) && ((u32)actor->attention_info.flags & 0x10000000)) {
                if (m8E2 != 0x2D) {
                    cXyz pos;
                    pos.x = 0.0f;
                    pos.y = 1.0f;
                    pos.z = 0.0f;
                    dComIfGp_getVibration_StartShock(4, -0x21, &pos);
                }
                dComIfGp_setDoStatus(0x2D); /* dComIfGp_setDoStatus */
                if (CbExec_trigA(0)) {      /* CPad_CHECK_TRIG_A(0) */
                    eventInfo_onCondition(this, 1);
                    CbExec_orderTalkEvent(this, actor); /* fopAcM_orderTalkEvent */
                    return TRUE;
                }
                return FALSE;
            }
        }
    }
    return flyCheck();
}

// HD: message flow uses the original manager singleton and shared message ID.
static u32 cbExec_msgId;
static u32 cbExec_msgManager;
static inline u32 cb1_msgMgr() { return (u32)cbExec_msgManager; }
static inline u32 cb1_msgStatus(u32 mgr) { return CbExec_messageStatus(mgr); }
static inline void cb1_msgSetStatus(u32 mgr, u32 st) { CbExec_setMessageStatus(mgr, st); }

// HD: HD: the message id is only set here; no fopMsgM_SearchByID
/* 02223308 */
BOOL daNpc_Cb1_c::initTalk() {

    if ((s32)cbExec_msgId == -1) {
        u32 mgr = cb1_msgMgr();
        if (mMsgNo == 0x5AC) {
            cbExec_msgId = CbExec_tactMessageSet(mgr); /* fopMsgM_tactMessageSet */
        } else {
            cbExec_msgId = CbExec_messageSet(mgr, (u32)mMsgNo, &eyePos); /* fopMsgM_messageSet */
        }
        return FALSE;
    }
    return TRUE;
}

/* 02223380 */
BOOL daNpc_Cb1_c::execTalk(BOOL param_1) {

    u32 mgr = cb1_msgMgr();
    u16 st = (u16)cb1_msgStatus(mgr);
    if (st == 0xE) { /* fopMsgStts_MSG_DISPLAYED_e */
        cb1_msgSetStatus(mgr, next_msgStatus(&mMsgNo));
        if (cb1_msgStatus(mgr) == 0xF) {
            CbExec_messageSet(mgr, (u32)mMsgNo, 0); /* fopMsgM_messageSet */
        }
    } else if (st == 0x15) { /* fopMsgStts_INPUT_e */
        if (param_1) {
            if (dComIfGp_checkMesgCancelButton() != 0) { /* dComIfGp_checkMesgCancelButton() */
                cb1_msgSetStatus(mgr, 0x10);
                gabi::at<cb1_execute::MessageManager>(mgr)->mMessageSendOn =
                    1; /* fopMsgM_messageSendOn() */
                cb1_m_status() = cb1_m_status() | daCbStts_TACT_CANCEL;
            }
            if (cb1_m_status() & daCbStts_TACT_CORRECT) {
                cb1_msgSetStatus(mgr, 0x10);
                gabi::at<cb1_execute::MessageManager>(mgr)->mMessageSendOn = 1;
            }
        }
    } else if (st == 6) { /* fopMsgStts_MSG_TYPING_e */
        if (mMsgNo != 0x5AC) {
            setAnm(
                getAnmType(dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeAttrInfo() */));
        }
    } else if (st == 0x12) { /* fopMsgStts_BOX_CLOSED_e */
        cb1_msgSetStatus(mgr, 0x13);
        return TRUE;
    }
    return FALSE;
}

/* 02223500 */
BOOL daNpc_Cb1_c::talkNpcAction(void *) {

    if (m8F0 == 0) {
        cbExec_msgId = -1;
        mMsgNo = getMsg();
        attention_info.flags = attention_info.flags & ~0x10u; /* attention_info.flags */
        if (!(cb1_m_status() & daCbStts_MUSIC)) {
            setAnm((cb1_m_status() & daCbStts_SHIP_RIDE) ? 0xD : 0);
        }
    } else if (m8F0 != -1) {
        mHasAttention = 1;
        if (m8F0 == 1) {
            if (initTalk()) {
                m8F0 = 2;
            }
        } else if (m8F0 == 2) {
            if (execTalk(0)) {
                setWaitNpcAction(nullptr);
                dComIfGp_event_reset();
            }
        }
        if (cb1_m_status() & daCbStts_SHIP_RIDE) {
            u32 ship = gabi::ea(dComIfGp_getShipActor()); /* dComIfGp_getShipActor() */
            if (ship != 0) {
                fopAc_ac_c *link = dComIfGp_getLinkPlayer();
                cXyz d;
                cXyz_mi(&link->current.pos, &d, &current.pos);
                cXyz temp;
                cXyz_mi(&current.pos, &temp, &d);
                /* daShip_c::setAtnPos */
                gabi::at<cb1_execute::Ship>(ship)->mFlags =
                    gabi::at<cb1_execute::Ship>(ship)->mFlags | 0x1000000;
                gabi::at<cb1_execute::Ship>(ship)->mAtnPos.x = temp.x;
                gabi::at<cb1_execute::Ship>(ship)->mAtnPos.y = temp.y;
                gabi::at<cb1_execute::Ship>(ship)->mAtnPos.z = temp.z;
            }
        } else {
            mJntCtrl.mbTrn = 1; /* setTrn() */
            lookBack(1);
        }
    }
    return TRUE;
}

/* 02223048 */
BOOL daNpc_Cb1_c::waitNpcAction(void *param_1) {

    if (m8F0 == 0) {
        if (cb1_isEventBit(0x1610)) {
            attention_info.flags = attention_info.flags | 0x10; /* fopAc_Attn_ACTION_CARRY_e */
        }
        u8 anm = 0;
        if (param_1) {
            if (cb1_m_status() & daCbStts_PLAYER_FIND) {
                anm = 0xF;
                cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_LEFT_ALONE, 0);
            } else {
                anm = 0x14;
                cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_TROUBLE, 0);
            }
        }
        setAnm(anm);
        cb1_m_status() = cb1_m_status() & ~daCbStts_PLAYER_FIND;
        speedF = 0.0f;
    } else if (m8F0 != -1) {
        if (m8E1) {
            ProcFunc_l fn;
            md_pmf_load(&fn, gabi::ea(&cbExec_searchNpcAction)); /* &daNpc_Cb1_c::searchNpcAction */
            cb1_setNpcAction(this, &fn, nullptr);
        } else {
            mHasAttention =
                chkAttention(m8DC == 0x14 ? 4000.0f : (f32)cb1_HIO().mNpc.mMaxAttnDistXZ, 0x10000);
            BOOL temp = mHasAttention;
            if (temp && getMsg()) {
                m8DD = 6;
                attention_info.flags = attention_info.flags | 0xA; /* ACTION_SPEAK | LOCKON_TALK */
            } else {
                attention_info.flags = attention_info.flags & ~0xAu;
            }
            f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
            if (!(mNoResetFlg1 & 2)) { /* !checkNpcCallCommand() */
                if (cb1_isEventBit(0x1610) &&
                    dist_sq < (f32)cb1_HIO().field_0xC0 * (f32)cb1_HIO().field_0xC0) {
                    fopAc_ac_c *link = dComIfGp_getLinkPlayer();
                    gabi::at<daPy_py_l>(gabi::ea(link))->mNoResetFlg1 =
                        gabi::at<daPy_py_l>(gabi::ea(link))->mNoResetFlg1 | 2; /* onNpcCall() */
                    temp = TRUE;
                }
            } else {
                if (!(dist_sq <
                      (f32)cb1_HIO().mPlayerChaseDistance * (f32)cb1_HIO().mPlayerChaseDistance)) {
                    ProcFunc_l fn;
                    md_pmf_load(&fn, gabi::ea(&cbExec_searchNpcAction));
                    cb1_setNpcAction(this, &fn, nullptr);
                }
                temp = TRUE;
            }
            lookBack(temp);
            current.angle.y = shape_angle.y;
        }
    }
    return TRUE;
}

/* 02223694 */
BOOL daNpc_Cb1_c::carryNpcAction(void *) {

    if (m8F0 == 0) {
        attention_info.flags = attention_info.flags & ~0x10u;
        mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
        fopAc_ac_c *pPlayer =
            dComIfGp_getPlayer(0); /* fopAcM_toPlayerShapeAngleY: the player first */
        s16 myAngle = shape_angle.y;
        m8F4 = (s16)(myAngle - pPlayer->shape_angle.y);
        if (cb1_m_status() & daCbStts_NO_CARRY_ACTION) {
            cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_DAMAGE, 0);
            setAnm(0xB);
        } else {
            cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_LIFT_UP, 0);
            setAnm(3);
        }
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
    } else if (m8F0 == -1) {
        m904.y = 0.0f;
        m904.z = 0.0f;
        cb1_m_status() = cb1_m_status() & ~daCbStts_NO_CARRY_ACTION;
        mNoResetFlg1 = mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
    } else {
        if (cb1_m_status() & daCbStts_NO_CARRY_ACTION) {
            mpMorf->setPlaySpeed(2.0f);
        } else {
            CbExec_setRestart(this, 1); /* daPy_npc_c::setRestart */
            fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
            /* mpMorf is re-read after each virtual call, as the original does */
            mDoExt_McaMorf *morf0 = mpMorf;
            f32 oldPlaySpeed = morf0->getPlaySpeed();
            if (0.0f == pPlayer->speedF) {
                /* getBaseAnimeFrame: HD vtable slot 0xA4; its f1 is used unrounded (compare,
                 * fctiwz) */
                f64 baseFrame = gabi::call_ptr<f64>(
                    (u32)gabi::at<cb1_execute::PlayerVtable>(pPlayer->__vtbl)->mBaseAnimeFrame,
                    pPlayer);
                mDoExt_McaMorf *morf1 = mpMorf;
                if (!(baseFrame >
                      (f64)morf0->getEndFrame())) { /* HD: ble into this block (unordered taken) */
                    f64 f = gabi::call_ptr<f64>(
                        (u32)gabi::at<cb1_execute::PlayerVtable>(pPlayer->__vtbl)->mBaseAnimeFrame,
                        pPlayer);
                    morf1->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(f); /* setFrame */
                    mpMorf->setPlaySpeed(0.0f);
                } else {
                    morf1->setPlaySpeed(1.0f);
                }
            } else {
                morf0->setPlaySpeed(1.0f);
            }
            if (oldPlaySpeed != mpMorf->getPlaySpeed()) {
                mpMorf->setMorf(4.0f);
            }
            cLib_chaseF(&m904.x, cb1_HIO().field_0x74, 1.0f);
            cLib_chaseF(&m904.y, cb1_HIO().field_0x78, 1.0f);
            cLib_chaseF(&m904.z, cb1_HIO().field_0x7C, 1.0f);
            cLib_chaseAngleS(&m8F4, 0, 0x800);
            shape_angle.y = pPlayer->shape_angle.y + m8F4;
        }
        if (!(actor_status & 0x2000)) { /* !fopAcM_checkCarryNow(this) */
            ProcFunc_l fn;
            if (speedF > 0.0f) {
                md_pmf_load(&fn, gabi::ea(&cbExec_flyNpcAction)); /* &daNpc_Cb1_c::flyNpcAction */
            } else {
                md_pmf_load(&fn, gabi::ea(&cbExec_waitNpcAction)); /* &daNpc_Cb1_c::waitNpcAction */
            }
            cb1_setNpcAction(this, &fn, nullptr);
        }
        cXyz zero;
        zero = cbExec_zero; /* cXyz::Zero */
        cb1_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &zero, shape_angle.y, 0, 0);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}

/* 02223A88 */
BOOL daNpc_Cb1_c::flyNpcAction(void *) {

    s8 temp = m8F0;
    if (temp == 0) {
        speedF = cb1_HIO().mNpcFlyLaunchSpeedF;
        speed.y = cb1_HIO().mNpcFlyLaunchSpeedY;
        setAnm(4);
        cb1_m_flyingTimer() = cb1_HIO().mNpcFlyTimer;
        m8FC = (f32)(cb1_HIO().field_0xD8 + 1);
        cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_THROW, 0);
    } else if (temp != -1) {
        flyAction(m8F1 ? FALSE : TRUE, 0.0f, 0, FALSE);
        if (temp != 1) {
            m8F1 = (s8)((m8F1 + 1) % cb1_HIO().field_0xDE);
        }
    }
    return TRUE;
}

// HD: this inline sound call has no actor null check.
static inline void cb1_seStart(fopAc_ac_c *a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}
/* (u32)f as GHS converts it (values >= 2^31 through a subtraction) */
static inline u32 cb1_ftou(f32 v) {
    if (!(v < 2147483648.0f)) { /* bge: unordered taken */
        return (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
    }
    return (u32)gabi::ftoi(v);
}

/* flyAction, airborne part (m8F0 < 3): wind, tilt, speed */
// HD: inlined into the original actor method.
/* inlined into 022222D0 */
static inline void cb1_flyAir(daNpc_Cb1_c *i_this, bool isMakarPlayer, f32 param_2, s16 param_3,
                              f32 temp3, f32 &ySpeedLimit, BOOL &temp4) {
    cb1_execute::daNpc_Cb1_HIO_c &hio = cb1_HIO();
    BOOL temp7 = i_this->calcFlyingTimer();
    if (isMakarPlayer) {
        if (temp7) {
            cb1_monsSeStart(i_this, cb1_execute::JA_SE_CV_CB_FLY_END, 0);
        }
        cb1_onFlying();
    }
    i_this->m900 = 0.0f;
    cXyz temp;
    temp.x = 0.0f;
    temp.y = gabi::fadds_ppc(temp3, hio.field_0xA0);
    temp.z = 0.0f;
    f32 temp5 = hio.field_0x9C;
    if (i_this->m8F2) {
        temp.x = gabi::fmuls_ppc(param_2, cM_ssin(param_3));
        temp.z = gabi::fmuls_ppc(param_2, cM_scos(param_3));
    }
    if (!(cb1_m_status() & daNpc_Cb1_c::daCbStts_MUSIC) && i_this->mWindCyl.ChkTgHit()) {
        void *tg = i_this->mWindCyl.GetTgHitObj();
        if (tg && ((u32)gabi::at<cb1_execute::HitObject>(gabi::ea(tg))->mAtType &
                   0x200000)) { /* ChkAtType(AT_TYPE_WIND) */
            cXyz v;
            cXyz_ml(&i_this->mWindCyl.mGObjTg.mRVec /* GetTgRVecP() */, &v, 0.01f);
            PSVECAdd(&temp, &v, &temp);
            temp5 = 30.0f;
            ySpeedLimit = 30.0f;
        }
        i_this->m8F8 = 20000;
        temp4 = TRUE;
    } else {
        cXyz wind;
        gabi::Local<be<f32>> power;
        CbExec_allWindVector(&i_this->current.pos, (&wind), power.get()); /* dKyw_get_AllWind_vec */
        *power = gabi::fmuls_ppc(*power, hio.field_0xB8);
        cXyz v;
        cXyz_ml(&wind, &v, *power);
        PSVECAdd(&temp, &v, &temp);
        temp5 = temp5 + *power;
        ySpeedLimit = ySpeedLimit + *power;
        s16 temp8 = (s16)gabi::ftoi(*power * 400.0f);
        if (i_this->m8F8 < temp8) {
            i_this->m8F8 = temp8;
        }
    }
    s16 angle = cM_atan2s(temp.x, temp.z);
    f32 mag = std_sqrtf(gabi::fmadds(temp.x, temp.x, temp.z * temp.z));
    f32 temp6_2 = (mag - temp5 >= 0.0f) ? temp5 : mag; /* cLib_maxLimit (fsel) */
    angle = angle - i_this->shape_angle.y;
    f32 temp6 = temp6_2 * hio.field_0xA8;
    if (temp6 > 14000.0f) {
        temp6 = 14000.0f;
    }
    s16 cs = (s16)gabi::ftoi(cM_scos(angle) * temp6);
    s16 sn = (s16)gabi::ftoi(-(cM_ssin(angle) * temp6));
    cLib_chaseAngleS(&i_this->shape_angle.x, cs, hio.field_0xE0);
    cLib_chaseAngleS(&i_this->shape_angle.z, sn, hio.field_0xE0);
    i_this->lookBack(0);
    PSVECAdd(&i_this->speed, &temp, &i_this->speed);
    gabi::Local<be<f32>> temp8;
    *temp8 = std_sqrtf(PSVECSquareMag(&i_this->speed));
    if (!(fabsf(*temp8) < 3.814697265625e-06f) && *temp8 > temp5) { /* !cM3d_IsZero */
        f32 temp2 = *temp8;
        CbExec_chaseF(temp8.get(), temp5, 1.0f); /* cLib_chaseF */
        if (*temp8 > 30.0f) {
            *temp8 = 30.0f;
        }
        PSVECScale(&i_this->speed, &i_this->speed, *temp8 / temp2);
    }
    i_this->current.angle.y = cM_atan2s(i_this->speed.x, i_this->speed.z);
    i_this->speedF = std_sqrtf(
        gabi::fmadds(i_this->speed.x, i_this->speed.x, i_this->speed.z * i_this->speed.z));
    if (i_this->speed.y > ySpeedLimit) {
        i_this->speed.y = ySpeedLimit;
    }
}

/* 022222D0 */
BOOL daNpc_Cb1_c::flyAction(BOOL param_1, f32 param_2, s16 param_3, BOOL param_4) {

    cb1_execute::daNpc_Cb1_HIO_c &hio = cb1_HIO();
    bool isMakarPlayer = dComIfGp_getPlayer(0) == this;
    f32 ySpeedLimit = hio.field_0x98;
    f32 temp3 = gabi::fmuls_ppc((f32)(s16)m8F8, hio.field_0x8C);
    BOOL temp4 = FALSE;

    if (mAcch.ChkGroundHit()) {
        breaking();
        cLib_chaseAngleS(&shape_angle.x, 0, hio.field_0xE0);
        cLib_chaseAngleS(&shape_angle.z, 0, hio.field_0xE0);
        gravity = gabi::fadds_ppc(hio.field_0xA0, temp3);
    } else if (m8F0 < 3) {
        cb1_flyAir(this, isMakarPlayer, param_2, param_3, temp3, ySpeedLimit, temp4);
        gravity = 0.0f;
    }

    if (m8F0 == 1) {
        if (m8D7) {
            if (!mAcch.ChkGroundHit()) {
                m8F8 = hio.field_0xDA;
                speed.y = gabi::fadds_ppc(speed.y, hio.field_0xB0);
                setAnm(7);
            } else {
                setAnm(5);
            }
            m8F0 = (s8)(m8F0 + 1);
        } else if (mpMorf->checkFrame(6.0f)) {
            cb1_seStart(this, cb1_execute::JA_SE_CM_PRAPELLO_OPEN, 0);
            JPABaseEmitter *emitter =
                dComIfGp_particle_set(cb1_execute::ID_AK_SN_PERAPROOPEN00, &current.pos);
            if (emitter) {
                /* setGlobalRTMatrix(mpPropellerModel->getAnmMtx(m_center_jnt_num)); HD: getAnmMtx
                 * marks the joint matrices dirty */
                J3DMtxBuffer_hd *blk = mpPropellerModel->mpMtxBuffer;
                u32 mtx = (s8)m_center_jnt_num * 0x30 + (u32)blk->mpAnmMtx;
                blk->mFlags = (u16)(blk->mFlags | 0x10);
                CbExec_setEmitterMatrix(
                    mtx,
                    gabi::ea(&reinterpret_cast<cb1_execute::Emitter *>(emitter)->mGlobalRotation),
                    gabi::ea(&reinterpret_cast<cb1_execute::Emitter *>(emitter)
                                  ->mGlobalTranslation)); /* JPASetRMtxTVecfromMtx */
            }
        }
    } else if (m8F0 == 2) {
        if (isMakarPlayer && (temp4 || cb1_m_flyingTimer() < hio.field_0xCE)) {
            if (setAnm(0x15) && temp4) {
                cb1_monsSeStart(this, cb1_execute::JA_SE_CV_CB_FLY_END, 0);
            }
        } else if (m8DC != 6 && m8FC > (f32)(s16)hio.field_0xD8) {
            setAnm(7);
        }
        if (cb1_m_flyingTimer() != 0 && param_1) {
            CbExec_chaseS(&m8F8, (s16)hio.field_0xD0, (s16)hio.field_0xD4); /* cLib_chaseS */
            m8F6 = 0;
        } else {
            CbExec_chaseS(&m8F8, mAcch.ChkGroundHit() ? (s16)hio.field_0xD2 : (s16)0,
                          (s16)hio.field_0xD6);
        }
        m8FA = (s16)(m8FA + m8F8);
        m8FC = gabi::fadds_ppc(m8FC, gabi::fsubs_ppc(current.pos.y, old.pos.y));
        if (m8FC > hio.field_0xA4) {
            m8F2 = 1;
        }
        cb1_seStart(this, cb1_execute::JA_SE_CM_PRAPELLO_ROLLING,
                    cb1_ftou((f32)(s16)m8F8 * (100.0f / (f32)(s16)hio.field_0xD0)));
        if (param_4 || (!isMakarPlayer && mAcch.ChkWallHit()) ||
            (!mAcch.ChkGroundHit() && m8F8 == 0) ||
            (mAcch.ChkGroundHit() && m8F8 <= hio.field_0xD2 &&
             (!(CbExec_stickValue(0) /* CPad_GET_STICK_VALUE(0), f1 unrounded */ <
                (f64)hio.field_0x80) ||
              /* bge: unordered taken */ (m8F6 = (s16)(m8F6 + 1)) > hio.field_0xCC))) {
            setAnm(6);
            mpMorf->mFrameCtrl.mFrame = 8.0f; /* setFrame(8.0f) */
            m8F0 = (s8)(m8F0 + 1);
            cb1_offFlying();
        } else if (mAcch.ChkGroundHit() && m8DC == 6) {
            setAnm(5);
        }
        maxFallSpeed = hio.field_0x90;
    } else {
        if (m8DC == 4 && m8D7) {
            setAnm(2);
            cb1_seStart(this, cb1_execute::JA_SE_CM_PRAPELLO_OPEN, 0);
        }
        if (mAcch.ChkGroundHit()) {
            if (m8E0 == 0 && m8F0 == 3) {
                if (cb1_GetSpecialCode(cb1_gnd(this)) != 1) {
                    speed.y = hio.field_0x64;
                    checkLanding();
                }
                m8F0 = (s8)(m8F0 + 1);
            } else if (m8DC != 4) {
                ProcFunc_l fn;
                if (isMakarPlayer) {
                    md_pmf_load(
                        &fn,
                        gabi::ea(&cbExec_waitPlayerAction)); /* &daNpc_Cb1_c::waitPlayerAction */
                    setPlayerAction(&fn, nullptr);
                } else {
                    md_pmf_load(&fn,
                                gabi::ea(&cbExec_waitNpcAction)); /* &daNpc_Cb1_c::waitNpcAction */
                    cb1_setNpcAction(this, &fn, nullptr);
                    u32 camera = gabi::ea(dComIfGp_getCamera(0)); /* dComIfGp_getCamera(0) */
                    CbExec_forceLockOff(camera + 0x248,
                                        fopAcM_GetID(this)); /* mCamera.ForceLockOff */
                }
                return FALSE;
            }
        }
    }
    return TRUE;
}

static cb1_execute::CutNameTable cbExec_cutNames;
static cb1_execute::EventProcTable cbExec_eventProcs;

/* 02220288 */
BOOL daNpc_Cb1_c::eventProc() {

    mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::WALL_NONE; /* ClrWallNone() */
    if (eventInfo.checkCommandDemoAccrpt() /* eventInfo.checkCommandDemoAccrpt() */ && m8DD != -1) {
        if (m8DD == 0) {
            /* fopAcM_onDraw(this) (HD) */
            u32 r = CbExec_drawPriority(this);
            CbExec_enqueueDraw(gabi::ea(&draw_tag), r);
        } else if (m8DD == 1) {
            u32 link = gabi::ea(dComIfGp_getLinkPlayer()); /* dComIfGp_getLinkPlayer() */
            u32 evt = gabi::ea(&reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())
                                    ->play.mEvtCtrl); /* dComIfGp_event_setTalkPartner */
            gabi::at<dEvt_control_c>(evt)->mPtTalk = CbExec_convActor(evt, link);
            CbExec_controlSound(0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
        }
        m4E4 = m4E4 | 2; /* onEventAccept() */
        m8DD = -1;
    }

    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Cb1", nullptr, 0);
    if (cb1_event_runCheck() && (dComIfGp_getPlayer(0) == this || !checkCommandTalk())) {
        if (staffIdx != -1) {
            s32 actIdx = CbExec_getMyActIdx(dComIfGp_getPEvtManager(), staffIdx,
                                            gabi::ea(&cbExec_cutNames), 15, 1, 0); /* getMyActIdx */
            if (actIdx == -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            } else {
                u32 entry = gabi::ea(&cbExec_eventProcs) + actIdx * 16;
                if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                    md_pmf_call<void>(this, &gabi::at<cb1_execute::EventProc>(entry)->mInit,
                                      staffIdx);
                    speedF = 0.0f;
                }
                if (md_pmf_call<BOOL>(this, &gabi::at<cb1_execute::EventProc>(entry)->mRun,
                                      staffIdx)) {
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
            }
            mPlayerAction.i = 0;
            mPlayerAction.f = 0;
            mAcch.m_flags = mAcch.m_flags | dBgS_Acch::WALL_NONE; /* SetWallNone() */
            mNpcAction.d = 0;
            mPlayerAction.d = 0;
            mNpcAction.i = 0;
            mNpcAction.f = 0;
        }
        if (m4E4 & 2) { /* isEventAccept() */
            if (dComIfGp_evmng_endCheck(mEventIdx[(s8)m8E3])) {
                dComIfGp_event_reset();
                m4E4 = m4E4 & ~2u; /* offEventAccept() */
                if (m8E3 == 1) {
                    returnLinkPlayer();
                    m4E4 = m4E4 & ~1u; /* offReturnLink() */
                }
                m8E3 = -1;
            }
            return TRUE;
        }
        if (staffIdx != -1) {
            return TRUE;
        }
        if (dComIfGp_getLinkPlayer()->eventInfo.getCommand() !=
            dEvtCmd_INDOOR_e) { /* !checkCommandDoor() */
            return TRUE;
        }
    }
    return FALSE;
}

/* 022213B8 */
static BOOL daNpc_Cb1_Execute(daNpc_Cb1_c *i_this) { return i_this->execute(); }

static u8 cbExec_playerRoom;
static s8 cbExec_stayNo;
static cb1_execute::RoomStatusTable cbExec_roomStatus;

/* execute: the room/ground part (not carried, not on the ship); returns TRUE when execute ends here
 */
// HD: inlined into the original actor method.
/* inlined into 0222091C */
static inline BOOL cb1_execRoom(daNpc_Cb1_c *i_this) {
    cb1_execute::daNpc_Cb1_HIO_c &hio = cb1_HIO();
    u32 self = gabi::ea(i_this);
    BOOL initialRoom = CbExec_checkInitialRoom(i_this); /* check_initialRoom */
    if (!initialRoom ||
        (fopAcM_GetParam(i_this) == 5 &&
         !CbExec_checkMoveBgNoDaBg(dComIfG_Bgsp(), cb1_gnd(i_this)) /* ChkMoveBG_NoDABg */)) {
        i_this->home.roomNo = -1;
        i_this->current.roomNo = -1;
        if (gabi::ea(dComIfGp_getCb1Player()) == self) { /* dComIfGp_getCb1Player() */
            reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mpPlayerPtr[1] = nullptr;
        }
        return TRUE;
    }
    CbExec_initialRestartOption(
        i_this, 1, fopAcM_GetParam(i_this) == 4); /* initialRestartOption(1, isTypeKaze()) */
    if (!(cb1_m_status() & daNpc_Cb1_c::daCbStts_SHIP_RIDE) &&
        CbExec_checkMoveStop(i_this) /* check_moveStop */) {
        i_this->setWaitNpcAction(nullptr);
        return TRUE;
    }
    if (i_this->m8E0 &&
        CbExec_checkPolySafe(dComIfG_Bgsp(), &i_this->mPolyInfo) /* ChkPolySafe */ &&
        CbExec_checkMoveBg(dComIfG_Bgsp(), &i_this->mPolyInfo) /* ChkMoveBG */) {
        CbExec_moveBgCrrPos(dComIfG_Bgsp(), &i_this->mPolyInfo, 1, &i_this->old.pos, 0,
                            0); /* MoveBgCrrPos */
    }
    dNpc_HIO_l &n = hio.mNpc;
    i_this->mJntCtrl.setParam(n.mMaxBackboneX, n.mMaxBackboneY, n.mMinBackboneX, n.mMinBackboneY,
                              n.mMaxHeadX, n.mMaxHeadY, n.mMinHeadX, n.mMinHeadY, n.mMaxTurnStep);
    return FALSE;
}

/* execute: falling off the map (ground code 4 or no ground) */
// HD: inlined into the original actor method.
/* inlined into 0222091C */
static inline void cb1_execFallCheck(daNpc_Cb1_c *i_this) {
    if (i_this->isFlyAction() && i_this->m8FC > -400.0f) {
        i_this->m4E8 = 0;
    } else if (i_this->m4E8 < 0x1E) {
        i_this->m4E8 = (u8)(i_this->m4E8 + 1);
    } else {
        bool reset = cb1_pmfIs(&i_this->mNpcAction,
                               cb1_execute::HD_fallResetNpcAction); /* HD: an extra NPC action
                                                                       (0222487C) always resets */
        if (!reset) {
            s8 room = i_this->home.roomNo;
            (void)dComIfGp_ea();
            /* dComIfGp_roomControl_checkStatusFlag(fopAcM_GetHomeRoomNo(this), 0x10) */
            reset = !(
                (u8)gabi::at<cb1_execute::RoomStatus>(
                    gabi::ea(&cbExec_roomStatus) + (u32)(s32)room * sizeof(cb1_execute::RoomStatus))
                    ->mStatus &
                0x10);
        }
        if (reset) {
            auto *currentWords = reinterpret_cast<cb1_execute::WordPosition *>(&i_this->current);
            auto *homeWords = reinterpret_cast<cb1_execute::WordPosition *>(&i_this->home);
            currentWords->angleZRoom = homeWords->angleZRoom;
            currentWords->x = homeWords->x;
            i_this->shape_angle.x = i_this->home.angle.x;
            i_this->shape_angle.z = i_this->home.angle.z;
            currentWords->z = homeWords->z;
            i_this->shape_angle.y = i_this->home.angle.y;
            currentWords->angleXY = homeWords->angleXY;
            i_this->speedF = 0.0f;
            i_this->m4E8 = 0;
            currentWords->y = homeWords->y;
            cb1_seStart(i_this, cb1_execute::JA_SE_CV_CB_LEFT_ALONE, 0);
            /* HD: back to the wait action */
            ProcFunc_l fn;
            md_pmf_load(&fn, gabi::ea(&cbExec_waitNpcAction)); /* &daNpc_Cb1_c::waitNpcAction */
            cb1_setNpcAction(i_this, &fn, nullptr);
        } else {
            /* daPy_getPlayerLinkActorClass()->npcStartRestartRoom() */
            CbExec_startRestartRoom(gabi::ea(dComIfGp_getLinkPlayer()), 5, 0xC9, -1.0f,
                                    0); /* startRestartRoom */
        }
    }
}

/* execute: ground polygon (slope, slide, room) */
// HD: inlined into the original actor method.
/* inlined into 0222091C */
static inline void cb1_execGround(daNpc_Cb1_c *i_this, cXyz *temp2, f32 &temp3) {
    u8 *gnd = cb1_gnd(i_this);
    u32 pla = gabi::ea(dBgS_GetTriPla(dComIfG_Bgsp(), gnd));
    if (pla) {
        i_this->m910.x = gabi::at<cXyz>(pla)->x;
        i_this->m910.y = gabi::at<cXyz>(pla)->y;
        i_this->m910.z = gabi::at<cXyz>(pla)->z;
        if (i_this->m8E0) {
            i_this->maxFallSpeed =
                gabi::fnmsubs(100.0f, gabi::fsubs_ppc(1.0f, i_this->m910.y), -4.0f);
            if (cb1_GetSpecialCode(gnd) == 1) {
                cXyz xz;
                xz.x = i_this->m910.x;
                xz.y = 0.0f;
                xz.z = i_this->m910.z;
                f32 temp = PSVECSquareMag(&xz); /* m910.abs2XZ() */
                temp2->x = gabi::fmuls_ppc(i_this->m910.x, 40.0f);
                temp2->z = gabi::fmuls_ppc(i_this->m910.z, 40.0f);
                f32 sy = gabi::fmuls_ppc(std_sqrtf(1.0f - temp), -40.0f);
                i_this->speed.y = sy;
                temp3 = temp + temp;
                i_this->maxFallSpeed = sy;
                i_this->checkLanding();
            }
        }
    }
    s8 roomNo = (s8)CbExec_getRoomId(dComIfG_Bgsp(), gnd); /* GetRoomId */
    i_this->tevStr.mRoomNo = roomNo;                       /* tevStr.mRoomNo */
    i_this->current.roomNo = roomNo;
    u8 color = (u8)CbExec_getPolyColor(dComIfG_Bgsp(), gnd); /* GetPolyColor */
    i_this->mStts.mRoomId = roomNo;                          /* mStts.SetRoomId */
    /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
    i_this->mPolyInfo.mPolyIndex = gabi::at<cBgS_PolyInfo_l>(gabi::ea(gnd))->mPolyIndex;
    i_this->mPolyInfo.mProcId = gabi::at<cBgS_PolyInfo_l>(gabi::ea(gnd))->mProcId;
    i_this->mPolyInfo.mpBgW = gabi::at<cBgS_PolyInfo_l>(gabi::ea(gnd))->mpBgW;
    i_this->tevStr.mEnvrIdxOverride = color; /* tevStr.mEnvrIdxOverride */
    i_this->mPolyInfo.mBgIndex = gabi::at<cBgS_PolyInfo_l>(gabi::ea(gnd))->mBgIndex;
}

/* execute: the damage reaction (player or NPC) */
// HD: inlined into the original actor method.
/* inlined into 0222091C */
static inline void cb1_execDamage(daNpc_Cb1_c *i_this, u32 pmf, bool player) {
    if (i_this->mDamageFogTimer == 0 && !(cb1_m_status() & daNpc_Cb1_c::daCbStts_MUSIC) &&
        i_this->mCyl.ChkTgHit() && i_this->mCyl.GetTgHitObj()) {
        cXyz temp;
        cXyz_mi(&i_this->current.pos, &temp, i_this->mCyl.GetTgHitPosP() /* GetTgHitPosP() */);
        i_this->current.angle.y = cM_atan2s(temp.x, temp.z);
        i_this->speedF = cb1_HIO().field_0xC4;
        ProcFunc_l fn;
        md_pmf_load(&fn, pmf);
        if (player) {
            i_this->setPlayerAction(&fn, nullptr);
        } else {
            cb1_setNpcAction(i_this, &fn, nullptr);
        }
        i_this->mDamageFogTimer = cb1_HIO().mDamageTimer; /* setDamageFogTimer */
        cb1_monsSeStart(i_this, cb1_execute::JA_SE_CV_CB_DAMAGE, 0);
    }
}

/* 0222091C */
BOOL daNpc_Cb1_c::execute() {

    cb1_execute::daNpc_Cb1_HIO_c &hio = cb1_HIO();
    actor_status = actor_status & ~0x20u;     /* fopAcStts_SHOWMAP_e */
    CbExec_calcDamageTimer(&mDamageFogTimer); /* executeDamageFog(): cLib_calcTimer<u8> */
    fopAcM_setStageLayer(this);
    /* m_playerRoom = fopAcM_GetRoomNo(this) == dComIfGp_roomControl_getStayNo() */
    cbExec_playerRoom = (s8)current.roomNo == cbExec_stayNo;
    cb1_m_status() = cb1_m_status() & ~daCbStts_UNK_0100;

    if (!(cb1_m_status() & daCbStts_SHIP_RIDE) && !(actor_status & 0x2000)) {
        if (cb1_execRoom(this)) {
            return TRUE;
        }
    }

    cXyz temp2;
    temp2 = cbExec_zero; /* cXyz::Zero */
    f32 temp3 = 3.0f;

    if (!(actor_status & 0x2000) && !(cb1_m_status() & daCbStts_SHIP_RIDE)) {
        if (CbExec_checkNowPosMove(this, gabi::guest_str("Cb1")) /* checkNowPosMove("Cb1") */ &&
            !(m4E4 & 1)) {
            f32 temp4 = current.pos.y;
            fopAcM_posMoveF(this, &mStts.m_cc_move /* mStts.GetCCMoveP() */);
            m900 = gabi::fadds_ppc(m900, gabi::fsubs_ppc(temp4, current.pos.y));
            current.pos.x = gabi::fadds_ppc(current.pos.x, m88C.x);
            current.pos.z = gabi::fadds_ppc(current.pos.z, m88C.z);
            maxFallSpeed = -100.0f;
            gravity = hio.field_0x70;
        }
        m8E0 = mAcch.ChkGroundHit();
        mAcch.CrrPos(dComIfG_Bgsp());
        cb1_m_status() = cb1_m_status() | daCbStts_UNK_0100;
        if (mAcch.m_ground_h == -1000000000.0f ||
            CbExec_getGroundCode(dComIfG_Bgsp(), cb1_gnd(this)) == 4 /* GetGroundCode */) {
            cb1_execFallCheck(this);
        } else {
            m4E8 = 0;
        }
        if (mAcch.m_ground_h != -1000000000.0f) {
            cb1_execGround(this, &temp2, temp3);
        }
        setCollision();
    }

    if (CbExec_checkNowPosMove(this, gabi::guest_str("Cb1"))) { /* checkNowPosMove("Cb1") */
        playAnm();
    }

    if (dComIfGp_getPlayer(0) == this) {
        dComIfGp_setDoStatus(0);     /* dComIfGp_setDoStatus(dActStts_BLANK_e) */
        dComIfGp_setAStatus(0x3E);   /* dComIfGp_setAStatus(dActStts_HIDDEN_e) */
        dComIfGp_setRStatusForce(0); /* dComIfGp_setRStatusForce(dActStts_BLANK_e) */
    }

    if (!eventProc()) {
        cb1_m_status() = cb1_m_status() & ~daCbStts_TACT; /* offTact() */
        if (!isFlyAction() && !(cb1_m_status() & daCbStts_MUSIC) && mWindCyl.ChkTgHit()) {
            cXyz *rvec = &mWindCyl.mGObjTg.mRVec; /* GetTgRVecP() */
            cXyz xz;
            f32 rx = rvec->x;
            temp2.y = rvec->y;
            xz.z = rvec->z;
            xz.y = 0.0f;
            temp2.x = rx;
            xz.x = rx;
            temp2.z = xz.z;
            f32 temp = std_sqrtf(PSVECSquareMag(&xz)); /* absXZ() */
            if (temp < 1.0f) {
                cXyz d;
                cXyz_mi(&current.pos, &d, mWindCyl.GetTgHitPosP() /* GetTgHitPosP() */);
                cXyz m;
                cXyz_ml(&d, &m, 30.0f);
                temp2 = m;
            }
            if (temp > 30.0f) {
                PSVECScale(&temp2, &temp2, 30.0f / temp);
            }
            temp3 = 1.0f;
        }
        CbExec_addCalcPosXZ(&m88C, (&temp2), 0.5f, temp3, 0.5f); /* cLib_addCalcPosXZ */
        if (dComIfGp_getPlayer(0) == this) {
            if (mAcch.ChkGroundHit()) {
                dComIfGp_setRStatusForce(7); /* dComIfGp_setRStatusForce(dActStts_RETURN_e) */
            }
            actor_status =
                (actor_status & ~0x3Fu) | 0x32; /* fopAcM_SetStatusMap(this, 0x12) (HD 0x32) */
            if (m4E4 & 1) {                     /* isReturnLink() */
                m8DD = 1;
            } else {
                if (cb1_m_status() & daCbStts_NO_CARRY_ACTION) {
                    ProcFunc_l fn;
                    md_pmf_load(
                        &fn,
                        gabi::ea(&cbExec_carryPlayerAction)); /* &daNpc_Cb1_c::carryPlayerAction */
                    setPlayerAction(&fn, nullptr);
                    m4E4 = m4E4 | 1; /* returnLink() */
                }
                if (!cb1_event_runCheck() &&
                    (u8) reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mRStatusForce ==
                        7 &&
                    !cb1_event_runCheck() &&
                    CbExec_trigROrStart(0) /* CPad_CHECK_TRIG_R(0) || CPad_CHECK_TRIG_START(0) */) {
                    m4E4 = m4E4 | 1; /* returnLink() */
                }
                playerAction(nullptr);
                cb1_execDamage(this, gabi::ea(&cbExec_hitPlayerAction), true);
            }
            m8E2 = dComIfGp_getDoStatus(); /* dComIfGp_getDoStatus() */
        } else {
            m8E1 = (s8)CbExec_checkMoveBlock(this, &m91C); /* chkMoveBlock */
            u32 map = cb1_isEventBit(0x1610)
                          ? 0x2D
                          : 0x27; /* fopAcM_SetStatusMap 0xD / 0x7 (HD 0x2D / 0x27) */
            actor_status = (actor_status & ~0x3Fu) | map;
            carryCheck();
            checkOrder();
            npcAction(nullptr);
            if (!cb1_pmfIs(&mNpcAction, CB1_flyNpcAction)) {
                current.angle.y = shape_angle.y;
            }
            cb1_execDamage(this, gabi::ea(&cbExec_hitNpcAction), false);
            m8E2 = 0;
        }
    }
    eventOrder();
    setBaseMtx(); /* setBaseMtx */
    /* HD: attention_info.position and eyePos are not set here */
    return TRUE;
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data cbExec_HIO 10466B98
data cbExec_status 101BDC2A
data cbExec_flyingTimer 101BDC28
data cbExec_flying 101BDC2D
data cbExec_zero 101FFBA8
data cbExec_anmTable 101BDA6C
data cbExec_anmTypes 101BD9F4
data cbExec_anmParameters 101BDA54
data cbExec_waitNpcAction 10018BE0
data cbExec_searchNpcAction 10018BE8
data cbExec_carryNpcAction 10018BF0
data cbExec_waitPlayerAction 10018BF8
data cbExec_flyPlayerAction 10018C00
data cbExec_flyNpcAction 10018C08
data cbExec_hitNpcAction 10018C28
data cbExec_talkNpcAction 10018C40
data cbExec_carryPlayerAction 10018C50
data cbExec_hitPlayerAction 10018C58
ext 025E1AA4 void CbExec_monsSeStart(u32 arg0, cXyz* arg1, u32 arg2, u32 arg3, s32 arg4)
ext 024EF09C s32 CbExec_getSpecialCode(dBgS* arg0, void* arg1)
ext 025A8BFC void CbExec_setSimpleLand(u32 arg0, void* arg1, cXyz* arg2, csXyz* arg3, f32 arg4, f32 arg5, f32 arg6, void* arg7, s32* arg8, s32 arg9)
ext 0259D454 void CbExec_setAnm(mDoExt_McaMorf* arg0, s32 arg1, f32 arg2, f32 arg3, s32 arg4, s32 arg5, u32 arg6)
ext 025E1FC0 void CbExec_practiceStop()
ext 025E1FB0 void CbExec_practicePlay(cXyz* arg0)
ext 025E18EC void CbExec_bgmStart(u32 arg0)
ext 023D4688 void CbExec_changePlayer(fopAc_ac_c* arg0, fopAc_ac_c* arg1)
ext 024EE058 u32 CbExec_getLockonList(u32 arg0, s32 arg1)
ext 024EE020 u32 CbExec_getActionList(u32 arg0, s32 arg1)
ext 024EDFCC BOOL CbExec_lockonTruth(u32 arg0)
ext 024EE090 u32 CbExec_getActionBtnB(u32 arg0)
ext 024EBA14 fopAc_ac_c* CbExec_getAttentionActor(u32 arg0)
ext 0259DED0 void CbExec_lookAtTarget(dNpc_JntCtrl_c* arg0, be<s16>* arg1, cXyz* arg2, cXyz* arg3, s16 arg4, s16 arg5, BOOL arg6)
ext 025447EC u32 CbExec_getMyActName(dEvent_manager_c* arg0, s32 arg1)
ext 02055B64 s32 CbExec_calcTimerS16(be<s16>* arg0)
ext 02007A1C s32 CbExec_stickAngle(s32 arg0)
ext 024F8018 s32 CbExec_controlledAngleY(u32 arg0)
ext 02007898 BOOL CbExec_trigA(s32 arg0)
ext 0200ECD4 f32 CbExec_addCalc(be<f32>* arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4)
ext 025D744C void CbExec_orderTalkEvent(fopAc_ac_c* arg0, fopAc_ac_c* arg1)
ext 025F795C u32 CbExec_messageStatus(u32 arg0)
ext 025F74D0 void CbExec_setMessageStatus(u32 arg0, u32 arg1)
ext 025F8088 u32 CbExec_tactMessageSet(u32 arg0)
ext 025F7DB0 u32 CbExec_messageSet(u32 arg0, u32 arg1, cXyz* arg2)
ext 02445438 void CbExec_setRestart(daNpc_Cb1_c* arg0, s32 arg1)
ext 0257E1B8 void CbExec_allWindVector(cXyz* arg0, cXyz* arg1, be<f32>* arg2)
ext 0200F5C8 BOOL CbExec_chaseF(be<f32>* arg0, f32 arg1, f32 arg2)
ext 028249B0 void CbExec_setEmitterMatrix(u32 arg0, u32 arg1, u32 arg2)
ext 0200F564 void CbExec_chaseS(be<s16>* arg0, s16 arg1, s16 arg2)
ext 020079B4 f64 CbExec_stickValue(s32 arg0)
ext 025052BC void CbExec_forceLockOff(u32 arg0, u32 arg1)
ext 025DF2B8 u32 CbExec_drawPriority(fopAc_ac_c* arg0)
ext 025DA874 void CbExec_enqueueDraw(u32 arg0, u32 arg1)
ext 0253F124 u32 CbExec_convActor(u32 arg0, u32 arg1)
ext 025E1988 void CbExec_controlSound(u32 arg0)
ext 02542EDC s32 CbExec_getMyActIdx(dEvent_manager_c* arg0, s32 arg1, u32 arg2, s32 arg3, s32 arg4, s32 arg5)
ext 024451B4 BOOL CbExec_checkInitialRoom(daNpc_Cb1_c* arg0)
ext 024EEB2C BOOL CbExec_checkMoveBgNoDaBg(dBgS* arg0, void* arg1)
ext 0244586C void CbExec_initialRestartOption(daNpc_Cb1_c* arg0, s32 arg1, BOOL arg2)
ext 024452A0 BOOL CbExec_checkMoveStop(daNpc_Cb1_c* arg0)
ext 02008254 BOOL CbExec_checkPolySafe(dBgS* arg0, cBgS_PolyInfo_l* arg1)
ext 024EEABC BOOL CbExec_checkMoveBg(dBgS* arg0, cBgS_PolyInfo_l* arg1)
ext 024EF968 void CbExec_moveBgCrrPos(dBgS* arg0, cBgS_PolyInfo_l* arg1, s32 arg2, cXyz* arg3, u32 arg4, u32 arg5)
ext 023FD4E4 void CbExec_startRestartRoom(u32 arg0, s32 arg1, s32 arg2, f32 arg3, s32 arg4)
ext 024EF130 s32 CbExec_getRoomId(dBgS* arg0, void* arg1)
ext 024EEEB8 u32 CbExec_getPolyColor(dBgS* arg0, void* arg1)
ext 02445950 BOOL CbExec_checkNowPosMove(daNpc_Cb1_c* arg0, u32 arg1)
ext 024EF0BC s32 CbExec_getGroundCode(dBgS* arg0, void* arg1)
ext 0207A9A0 void CbExec_calcDamageTimer(be<u8>* arg0)
ext 0200EF78 void CbExec_addCalcPosXZ(cXyz* arg0, cXyz* arg1, f32 arg2, f32 arg3, f32 arg4)
ext 02007840 BOOL CbExec_trigROrStart(s32 arg0)
ext 02445AA4 s32 CbExec_checkMoveBlock(daNpc_Cb1_c* arg0, cXyz* arg1)
data cbExec_msgId 10466B28
data cbExec_msgManager 101F4B5C
str 10018F74 "Cb" in 0221FEFC
str 10018FAC "Cb1" in 02221A3C
str 10018FB0 "WAIT" in 02221A3C
str 10018F78 "Cb1" in 02220288
str 10018FA0 "Cb1" in 0222091C
data cbExec_cutNames 101BD91C
data cbExec_eventProcs 101BDB14
data cbExec_playerRoom 101BDC2C
data cbExec_stayNo 1047E6C8
data cbExec_roomStatus 1047E8E8
*/
// clang-format on
