/**
 * d_a_npc_cb1_exec.cpp (WWHD)
 * NPC - Makar (Korok cellist): execute, orders, animation, talk, fly/walk movement
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_cb1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_cb1.h"

/* ---- this TU's statics (HD addresses) ---- */
#define CB1_L_HIO 0x10466B98          /* daNpc_Cb1_HIO_c l_HIO (0xF0, layout as on GameCube) */
#define CB1_M_FLYING_TIMER 0x101BDC28 /* s16 daNpc_Cb1_c::m_flyingTimer */
#define CB1_M_STATUS 0x101BDC2A       /* u16 daNpc_Cb1_c::m_status */
#define CB1_M_FLYING 0x101BDC2D       /* bool daNpc_Cb1_c::m_flying */
#define CB1_ANM_TYPE_TBL 0x101BD9F4   /* getAnmType table (11 bytes) */
#define CB1_ANM_PRM 0x101BDA54        /* s8 anmPrmData[24] */
#define CB1_ANM_TBL 0x101BDA6C        /* AnmData anmTblData[] (6 bytes each) */

/* member functions (actions) by address */
enum : u32 {
    CB1_waitNpcAction = 0x02223048,
    CB1_talkNpcAction = 0x02223500,
    CB1_carryNpcAction = 0x02223694,
    CB1_flyNpcAction = 0x02223A88,
    CB1_searchNpcAction = 0x022244A0,
    CB1_hitNpcAction = 0x022249C0,
    CB1_jumpNpcAction = 0x02224AC0,
    CB1_rescueNpcAction = 0x02224BC0,
    CB1_musicNpcAction = 0x02224C20,
    CB1_shipNpcAction = 0x02224CFC,
    CB1_waitPlayerAction = 0x02224E38,
    CB1_walkPlayerAction = 0x022250AC,
    CB1_hitPlayerAction = 0x02225388,
    CB1_jumpPlayerAction = 0x02225464,
    CB1_flyPlayerAction = 0x0222555C,
    CB1_carryPlayerAction = 0x0222566C,
};

/* daNpc_Cb1_HIO_c (0xF0; vtable at 0 as on GameCube, dNpc_HIO_c with its vtable last) */
struct daNpc_Cb1_HIO_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ dNpc_HIO_l mNpc;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> mPlayerChaseDistance;
    /* 0x38 */ be<f32> mChaseDistScale;
    /* 0x3C */ be<f32> mMaxWalkSpeed;
    /* 0x40 */ be<f32> mMinWalkSpeed;
    /* 0x44 */ be<f32> mForwardAccel;
    /* 0x48 */ be<f32> mDecelScale;
    /* 0x4C */ be<f32> mMaxDecel;
    /* 0x50 */ be<f32> mDecel;
    /* 0x54 */ be<f32> mWalkAnmSpeedScale;
    /* 0x58 */ be<f32> mMaxWalkAnmSpeed;
    /* 0x5C */ be<f32> mNpcFlyLaunchSpeedF;
    /* 0x60 */ be<f32> mNpcFlyLaunchSpeedY;
    /* 0x64 */ be<f32> field_0x64;
    /* 0x68 */ be<f32> mHitSpeedScaleF;
    /* 0x6C */ be<f32> mHitSpeedScaleY;
    /* 0x70 */ be<f32> field_0x70;
    /* 0x74 */ be<f32> field_0x74;
    /* 0x78 */ be<f32> field_0x78;
    /* 0x7C */ be<f32> field_0x7C;
    /* 0x80 */ be<f32> field_0x80;
    /* 0x84 */ be<f32> field_0x84;
    /* 0x88 */ be<f32> mStickWalkSpeedScale;
    /* 0x8C */ be<f32> field_0x8C;
    /* 0x90 */ be<f32> field_0x90;
    /* 0x94 */ u8 field_0x94[4];
    /* 0x98 */ be<f32> field_0x98;
    /* 0x9C */ be<f32> field_0x9C;
    /* 0xA0 */ be<f32> field_0xA0;
    /* 0xA4 */ be<f32> field_0xA4;
    /* 0xA8 */ be<f32> field_0xA8;
    /* 0xAC */ be<f32> mStickFlySpeedScale;
    /* 0xB0 */ be<f32> field_0xB0;
    /* 0xB4 */ be<f32> field_0xB4;
    /* 0xB8 */ be<f32> field_0xB8;
    /* 0xBC */ be<f32> mFlyLaunchSpeedY;
    /* 0xC0 */ be<f32> field_0xC0;
    /* 0xC4 */ be<f32> field_0xC4;
    /* 0xC8 */ be<s16> field_0xC8;
    /* 0xCA */ be<s16> mPlayerFlyTimer;
    /* 0xCC */ be<s16> field_0xCC;
    /* 0xCE */ be<s16> field_0xCE;
    /* 0xD0 */ be<s16> field_0xD0;
    /* 0xD2 */ be<s16> field_0xD2;
    /* 0xD4 */ be<s16> field_0xD4;
    /* 0xD6 */ be<s16> field_0xD6;
    /* 0xD8 */ be<s16> field_0xD8;
    /* 0xDA */ be<s16> field_0xDA;
    /* 0xDC */ be<s16> mNpcFlyTimer;
    /* 0xDE */ be<s16> field_0xDE;
    /* 0xE0 */ be<s16> field_0xE0;
    /* 0xE2 */ be<s16> field_0xE2;
    /* 0xE4 */ be<s16> field_0xE4;
    /* 0xE6 */ be<s16> field_0xE6;
    /* 0xE8 */ be<s16> field_0xE8;
    /* 0xEA */ be<s16> field_0xEA;
    /* 0xEC */ be<s16> field_0xEC;
    /* 0xEE */ be<u8> mDamageTimer;
    /* 0xEF */ be<u8> field_0xEF;
};
WWHD_SIZE(daNpc_Cb1_HIO_l, 0xF0);
static inline daNpc_Cb1_HIO_l& cb1_HIO() { return *gabi::at<daNpc_Cb1_HIO_l>(CB1_L_HIO); }

/* AnmData (6 bytes) */
struct cb1_AnmData_l {
    /* 0x00 */ be<s8> mAnmFileIdx;
    /* 0x01 */ be<u8> mLoopMode;
    /* 0x02 */ be<s8> field_0x02;
    /* 0x03 */ be<s8> mSpeed;
    /* 0x04 */ be<s8> field_0x04;
    /* 0x05 */ be<s8> field_0x05;
};
WWHD_SIZE(cb1_AnmData_l, 6);

static inline be<u16>& cb1_m_status() { return *gabi::at<be<u16>>(CB1_M_STATUS); }
static inline be<s16>& cb1_m_flyingTimer() { return *gabi::at<be<s16>>(CB1_M_FLYING_TIMER); }
static inline void cb1_offFlying() { gabi::store<u8>(CB1_M_FLYING, 0); }
static inline void cb1_onFlying() { gabi::store<u8>(CB1_M_FLYING, 1); }

/* GHS pointer-to-member equality with a plain (non-virtual) member: index first */
static inline BOOL cb1_pmfIs(ProcFunc_l* p, u32 f) {
    if (p->i != -1) return FALSE;
    if (p->i == 0) return TRUE;
    if (p->d != 0) return FALSE;
    return p->f == f;
}
static inline void cb1_pmfSet(ProcFunc_l* p, u32 f) {
    p->d = 0;
    p->i = -1;
    p->f = f;
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* calls into the other parts of this unit */
static inline void cb1_setNpcAction(daNpc_Cb1_c* a, ProcFunc_l* f, void* arg) { gabi::call(0x0221F2C4, a, f, arg); }
/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
static inline void cb1_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1AA4, id, &a->eyePos, gabi::load<u32>(gabi::ea(a) + 4), param, reverb);
}
/* 024EF09C dBgS::GetSpecialCode(cBgS_PolyInfo&) */
static inline s32 cb1_GetSpecialCode(void* poly) { return gabi::call<s32>(0x024EF09C, dComIfG_Bgsp(), poly); }
/* 025A8BFC dPa_control_c::setSimpleLand(poly, pos, angle, f32, f32, f32, tevstr, int*, int) */
static inline void cb1_setSimpleLand(void* poly, cXyz* pos, csXyz* ang, f32 a, f32 b, f32 c, void* tev, s32* out, s32 n) {
    gabi::call(0x025A8BFC, gabi::load<u32>(dComIfGp_ea() + 0x5AB0), poly, pos, ang, a, b, c, tev, out, n);
}
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, file, sound, arc) */
static inline void cb1_dNpc_setAnm(mDoExt_McaMorf* m, s32 loop, f32 morf, f32 speed, s32 file, s32 snd, u32 arc) {
    gabi::call(0x0259D454, m, loop, morf, speed, file, snd, arc);
}
/* 025E1FC0 mDoAud_cbPracticeStop(), 025E1FB0 mDoAud_cbPracticePlay(pos), 025E18EC mDoAud_bgmStart(id) */
static inline void cb1_cbPracticeStop() { gabi::call(0x025E1FC0); }
static inline void cb1_cbPracticePlay(cXyz* p) { gabi::call(0x025E1FB0, p); }
static inline void cb1_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* 023D4688 daPy_py_c::changePlayer(fopAc_ac_c*) */
static inline void cb1_changePlayer(fopAc_ac_c* self, fopAc_ac_c* a) { gabi::call(0x023D4688, self, a); }

/* mAcch.m_gnd as a cBgS_PolyInfo (the dBgS_GndChk polygon info at +0x14) */
/* save info: the event flags (dSv_event_c) are at *(0x101F84DC) + 0x644; re-read at every use */
static inline dSv_event_c* cb1_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL cb1_isEventBit(u16 f) { return dSv_event_isEventBit(cb1_event(), f); }
static inline void cb1_onEventBit(u16 f) { dSv_event_onEventBit(cb1_event(), f); }
static inline u8 cb1_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
/* dAttention_c (play + 0x5804) */
static inline u32 cb1_attention() { return dComIfGp_ea() + 0x5804; }
static inline u32 cb1_GetLockonList(u32 at, s32 i) { return gabi::call<u32>(0x024EE058, at, i); }
static inline u32 cb1_GetActionList(u32 at, s32 i) { return gabi::call<u32>(0x024EE020, at, i); }
static inline BOOL cb1_LockonTruth(u32 at) { return gabi::call<BOOL>(0x024EDFCC, at); }
static inline u32 cb1_getActionBtnB(u32 at) { return gabi::call<u32>(0x024EE090, at); }
static inline fopAc_ac_c* cb1_AttList_getActor(u32 l) { return gabi::call<fopAc_ac_c*>(0x024EBA14, l); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void cb1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, BOOL headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}

static inline u8* cb1_gnd(daNpc_Cb1_c* a) { return a->mAcch.m_gnd + 0x14; }

/* 0221FBD4 */
void daNpc_Cb1_c::setWaitNpcAction(void* arg) {
    WWHD_FUNC(0x0221FBD4, void, this, arg);
    u16 st = cb1_m_status();
    gabi::Local<ProcFunc_l> fn;
    fn->d = 0;
    fn->i = -1;
    if (st & daCbStts_SHIP_RIDE) {
        fn->f = CB1_shipNpcAction;
    } else {
        fn->f = (st & daCbStts_MUSIC) ? CB1_musicNpcAction : CB1_waitNpcAction;
    }
    cb1_setNpcAction(this, fn, nullptr);
}
VERIFY(0x0221FBD4, &daNpc_Cb1_c::setWaitNpcAction);

/* 0221FC6C */
BOOL daNpc_Cb1_c::isFlyAction() {
    WWHD_FUNC(0x0221FC6C, BOOL, this);
    return cb1_pmfIs(&mPlayerAction, CB1_flyPlayerAction) || cb1_pmfIs(&mNpcAction, CB1_flyNpcAction);
}
VERIFY(0x0221FC6C, &daNpc_Cb1_c::isFlyAction);

/* 0221FCE4 */
void daNpc_Cb1_c::checkLanding() {
    WWHD_FUNC(0x0221FCE4, void, this);
    gabi::Local<s32> temp;
    if (m900 > 200.0f) {
        if (cb1_GetSpecialCode(cb1_gnd(this)) != 1) {
            cb1_monsSeStart(this, 0x48C1 /* JA_SE_CV_CB_LANDING */, 0);
        }
        cb1_setSimpleLand(cb1_gnd(this), &current.pos, &shape_angle, 1.25f, 1.5f, 1.0f, &tevStr, temp, 7);
    } else {
        cb1_setSimpleLand(cb1_gnd(this), &current.pos, &shape_angle, 0.625f, 0.75f, 0.5f, &tevStr, temp, 7);
    }
}
VERIFY(0x0221FCE4, &daNpc_Cb1_c::checkLanding);

/* 0221FDF0 */
void daNpc_Cb1_c::setCollision() {
    WWHD_FUNC(0x0221FDF0, void, this);
    mCyl.SetC(&current.pos);
    mCyl.SetR(20.0f);
    mCyl.SetH(60.0f);
    dComIfG_Ccsp_Set(&mCyl);
    mWindCyl.SetC(&current.pos);
    mWindCyl.SetR(20.0f);
    mWindCyl.SetH(60.0f);
    dComIfG_Ccsp_Set(&mWindCyl);
}
VERIFY(0x0221FDF0, &daNpc_Cb1_c::setCollision);

/* 0221FECC */
void daNpc_Cb1_c::musicStop() {
    WWHD_FUNC(0x0221FECC, void, this);
    if (fopAcM_GetParam(this) != 5) { /* !isTypeKazeBoss() */
        cb1_cbPracticeStop();
    }
}
VERIFY(0x0221FECC, &daNpc_Cb1_c::musicStop);

/* 0221FEDC */
void daNpc_Cb1_c::musicPlay() {
    WWHD_FUNC(0x0221FEDC, void, this);
    if (fopAcM_GetParam(this) != 5) {
        cb1_cbPracticePlay(&eyePos);
    } else {
        cb1_bgmStart(0x8000005F /* JA_BGM_MAKORE_TAKT_8 */);
    }
}
VERIFY(0x0221FEDC, &daNpc_Cb1_c::musicPlay);

/* 0221FEFC */
void daNpc_Cb1_c::initAnm(s8 param_1, BOOL param_2) {
    WWHD_FUNC(0x0221FEFC, void, this, param_1, param_2);
    if (param_1 >= 0) {
        m8DC = param_1;
        cb1_AnmData_l& data = *gabi::at<cb1_AnmData_l>(CB1_ANM_TBL + param_1 * 6);
        m8B0 = 0.0f;
        f32 morf = 0.0f;
        if (param_2) {
            morf = (f32)(s8)data.field_0x02;
        }
        s8 file = data.mAnmFileIdx;
        cb1_dNpc_setAnm(mpMorf.get(), data.mLoopMode, morf, (f32)(s8)data.mSpeed, file < 0 ? -file : file, -1, 0x10018F74 /* "Cb" */);
        m8D7 = 0;
        m8D8 = 0;
        if (data.mAnmFileIdx < 0) {
            cb1_m_status() = cb1_m_status() | daCbStts_MUSIC;
        } else {
            cb1_m_status() = cb1_m_status() & ~daCbStts_MUSIC;
        }
    }
}
VERIFY(0x0221FEFC, &daNpc_Cb1_c::initAnm);

/* 02220024 */
void daNpc_Cb1_c::playAnm() {
    WWHD_FUNC(0x02220024, void, this);
    BOOL r3;
    if ((cb1_m_status() & daCbStts_UNK_0100) && mAcch.ChkGroundHit()) {
        u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)cb1_gnd(this));
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        r3 = mpMorf->play(&eyePos, snd, reverb);
    } else {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        r3 = mpMorf->play(&eyePos, 0, reverb);
    }
    if (r3 || mpMorf->getFrame() < m8B0) {
        m8D7 = 1;
        m8D8 = (s8)(m8D8 + 1);
        cb1_AnmData_l& data = *gabi::at<cb1_AnmData_l>(CB1_ANM_TBL + (s8)m8DC * 6);
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
VERIFY(0x02220024, &daNpc_Cb1_c::playAnm);

/* 02220194 */
BOOL daNpc_Cb1_c::checkCommandTalk() {
    WWHD_FUNC(0x02220194, BOOL, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1) { /* eventInfo.checkCommandTalk() */
        if ((u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
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
VERIFY(0x02220194, &daNpc_Cb1_c::checkCommandTalk);

/* 02220238 */
void daNpc_Cb1_c::returnLinkPlayer() {
    WWHD_FUNC(0x02220238, void, this);
    cb1_changePlayer(this, dComIfGp_getLinkPlayer());
    cb1_offFlying();
    setWaitNpcAction(nullptr);
}
VERIFY(0x02220238, &daNpc_Cb1_c::returnLinkPlayer);

/* 02221958 */
u8 daNpc_Cb1_c::getAnmType(int param_1) {
    WWHD_FUNC(0x02221958, u8, this, param_1);
    /* l_talkAnmType[11] */
    if ((u32)param_1 < 11) {
        return gabi::load<u8>(CB1_ANM_TYPE_TBL + param_1);
    }
    return gabi::load<u8>(CB1_ANM_TYPE_TBL);
}
VERIFY(0x02221958, &daNpc_Cb1_c::getAnmType);

/* 02221978 */
BOOL daNpc_Cb1_c::setAnm(u8 param_1) {
    WWHD_FUNC(0x02221978, BOOL, this, param_1);
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
        initAnm(gabi::load<s8>(CB1_ANM_PRM + param_1), 1);
    }
    return TRUE;
}
VERIFY(0x02221978, &daNpc_Cb1_c::setAnm);

/* 02221A3C */
void daNpc_Cb1_c::setMessageAnimation(u8 param_1) {
    WWHD_FUNC(0x02221A3C, void, this, param_1);
    if (!(actor_status & 0x2000) /* !fopAcM_checkCarryNow(this) */ && gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x10018FAC) /* "Cb1" */, nullptr, 0);
        if (staffIdx != -1) {
            u32 name = gabi::call<u32>(0x025447EC, dComIfGp_getPEvtManager(), staffIdx); /* getMyActName */
            /* HD: null check on the act name */
            if (name != 0) {
                u32 p = name, q = 0x10018FB0; /* "WAIT" */
                u8 a, b;
                for (;;) {
                    a = gabi::load<u8>(p++);
                    b = gabi::load<u8>(q++);
                    if (a != b || a == 0) break;
                }
                if (a == b) {
                    setAnm(getAnmType(param_1));
                }
            }
        }
    }
}
VERIFY(0x02221A3C, &daNpc_Cb1_c::setMessageAnimation);

/* 02221B18 */
BOOL daNpc_Cb1_c::calcFlyingTimer() {
    WWHD_FUNC(0x02221B18, BOOL, this);
    if (cb1_m_flyingTimer() != 0 && gabi::call<s32>(0x02055B64, &cb1_m_flyingTimer()) == 0) { /* cLib_calcTimer<s16> */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02221B18, &daNpc_Cb1_c::calcFlyingTimer);

/* 02221B68 (unnamed; called from 026552B8): chkPlayerAction(jumpPlayerAction) || chkPlayerAction(flyPlayerAction) */
static BOOL daNpc_Cb1_chkJumpFlyPlayerAction(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x02221B68, BOOL, i_this);
    return cb1_pmfIs(&i_this->mPlayerAction, CB1_jumpPlayerAction) || cb1_pmfIs(&i_this->mPlayerAction, CB1_flyPlayerAction);
}
VERIFY(0x02221B68, daNpc_Cb1_chkJumpFlyPlayerAction);

/* 02222020 */
void daNpc_Cb1_c::setWaitAction(void* param_1) {
    WWHD_FUNC(0x02222020, void, this, param_1);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this)) { /* dComIfGp_getPlayer(0) == this */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x10018BF8); /* &daNpc_Cb1_c::waitPlayerAction */
        setPlayerAction(fn, nullptr);
    } else {
        setWaitNpcAction(param_1);
    }
}
VERIFY(0x02222020, &daNpc_Cb1_c::setWaitAction);

/* 022220A8 */
s16 daNpc_Cb1_c::getStickAngY() {
    WWHD_FUNC(0x022220A8, s16, this);
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    s32 stick = gabi::call<s32>(0x02007A1C, 0);            /* CPad_GET_STICK_ANGLE(0) */
    s32 cam = gabi::call<s32>(0x024F8018, camera);         /* dCam_getControledAngleY */
    return (s16)(0x8000 + stick + cam);
}
VERIFY(0x022220A8, &daNpc_Cb1_c::getStickAngY);

/* 02222234 */
BOOL daNpc_Cb1_c::flyCheck() {
    WWHD_FUNC(0x02222234, BOOL, this);
    if (!gabi::call<BOOL>(0x02007898, 0)) { /* CPad_CHECK_TRIG_A(0) */
        return FALSE;
    }
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, 0x10018C00); /* &daNpc_Cb1_c::flyPlayerAction */
    setPlayerAction(fn, nullptr);
    return TRUE;
}
VERIFY(0x02222234, &daNpc_Cb1_c::flyCheck);

/* 022222AC */
f32 daNpc_Cb1_c::breaking() {
    WWHD_FUNC(0x022222AC, f32, this);
    return gabi::call<f32>(0x0200ECD4, &speedF, 0.0f, (f32)cb1_HIO().mDecelScale, (f32)cb1_HIO().mMaxDecel, (f32)cb1_HIO().mDecel); /* cLib_addCalc */
}
VERIFY(0x022222AC, &daNpc_Cb1_c::breaking);

/* 022205DC */
void daNpc_Cb1_c::setPlayerAction(ProcFunc_l* param_1, void* param_2) {
    WWHD_FUNC(0x022205DC, void, this, param_1, param_2);
    mNpcAction.d = 0;
    mNpcAction.i = 0;
    mNpcAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(param_1));
    gabi::call(0x0221F140, this, &mPlayerAction, fn.get(), param_2); /* setAction */
}
VERIFY(0x022205DC, &daNpc_Cb1_c::setPlayerAction);

/* 0222062C */
void daNpc_Cb1_c::playerAction(void* param_1) {
    WWHD_FUNC(0x0222062C, void, this, param_1);
    if (mPlayerAction.i == 0) {
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x10018BF8); /* &daNpc_Cb1_c::waitPlayerAction */
        setPlayerAction(fn, nullptr);
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0x23); /* dComIfGp_setDoStatus(dActStts_FLY_e) */
    md_pmf_call<BOOL>(this, &mPlayerAction, param_1);
}
VERIFY(0x0222062C, &daNpc_Cb1_c::playerAction);

/* 022206F0 */
void daNpc_Cb1_c::carryCheck() {
    WWHD_FUNC(0x022206F0, void, this);
    if (actor_status & 0x2000) { /* fopAcM_checkCarryNow(this) */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x10018BF0); /* &daNpc_Cb1_c::carryNpcAction */
        cb1_setNpcAction(this, fn, nullptr);
    }
}
VERIFY(0x022206F0, &daNpc_Cb1_c::carryCheck);

/* 02220738 */
void daNpc_Cb1_c::checkOrder() {
    WWHD_FUNC(0x02220738, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1) { /* eventInfo.checkCommandTalk() */
        if (m8DD == 5 || m8DD == 6 || m8DD == 7) {
            m8DD = -1;
            if ((u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3) { /* dComIfGp_event_chkTalkXY() */
                cb1_m_status() = cb1_m_status() | daCbStts_TACT;
            } else {
                gabi::Local<ProcFunc_l> fn;
                md_pmf_load(fn, 0x10018C40); /* &daNpc_Cb1_c::talkNpcAction */
                cb1_setNpcAction(this, fn, nullptr);
            }
            fopAcM_cancelCarryNow(this);
        }
    }
}
VERIFY(0x02220738, &daNpc_Cb1_c::checkOrder);

/* 022207E4 */
void daNpc_Cb1_c::npcAction(void* param_1) {
    WWHD_FUNC(0x022207E4, void, this, param_1);
    if (mNpcAction.i == 0) {
        speedF = 0.0f;
        setWaitNpcAction(nullptr);
    }
    md_pmf_call<BOOL>(this, &mNpcAction, param_1);
}
VERIFY(0x022207E4, &daNpc_Cb1_c::npcAction);

/* 02220884 */
void daNpc_Cb1_c::eventOrder() {
    WWHD_FUNC(0x02220884, void, this);
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
VERIFY(0x02220884, &daNpc_Cb1_c::eventOrder);

/* 02221BD0 */
BOOL daNpc_Cb1_c::chkAttention(f32 param_1, s32 param_2) {
    WWHD_FUNC(0x02221BD0, BOOL, this, param_1, param_2);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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
VERIFY(0x02221BD0, &daNpc_Cb1_c::chkAttention);

/* 02221CF8 */
u16 daNpc_Cb1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02221CF8, u16, this, pMsgNo);
    u32 no = *pMsgNo;
    if (no == 0x5AC) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    } else if (no == 0x1520 || (0x1519 <= no && no <= 0x151D) || no == 0x1523 || (0x14C2 <= no && no <= 0x14C3)) {
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
VERIFY(0x02221CF8, &daNpc_Cb1_c::next_msgStatus);

/* 02221DE4 */
u32 daNpc_Cb1_c::getMsg() {
    WWHD_FUNC(0x02221DE4, u32, this);
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
VERIFY(0x02221DE4, &daNpc_Cb1_c::getMsg);

/* 02221EF8 */
void daNpc_Cb1_c::lookBack(BOOL param_1) {
    WWHD_FUNC(0x02221EF8, void, this, param_1);
    cXyz* dstPos = nullptr;
    gabi::Local<cXyz> vec2;
    gabi::Local<cXyz> vec;
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = 0.0f;
    s16 desiredY = shape_angle.y;
    if (mJntCtrl.mbTrn) { /* trnChk() */
        cLib_addCalcAngleS2(&m8D0, cb1_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
    } else {
        m8D0 = 0;
    }
    if (param_1) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, cb1_HIO().mNpc.m04);
        *vec2 = *eye;
        dstPos = vec2;
        vec->x = current.pos.x;
        vec->y = eyePos.y;
        vec->z = current.pos.z;
    }
    gabi::Local<cXyz> eyeCopy;
    *eyeCopy = *vec;
    cb1_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, eyeCopy, desiredY, m8D0, 0);
}
VERIFY(0x02221EF8, &daNpc_Cb1_c::lookBack);

/* 022220FC */
int daNpc_Cb1_c::calcStickPos(s16 param_1, cXyz* param_2) {
    WWHD_FUNC(0x022220FC, int, this, param_1, param_2);
    u32 attention = cb1_attention();
    u32 attList = cb1_GetLockonList(attention, 0);
    /* attention.Lockon(): LockonTruth() || (flags & 0x20000000) */
    bool r26 = cb1_LockonTruth(attention) || (gabi::load<u32>(attention + 0x20) & 0x20000000);
    int r31 = !r26 ? 0 : (cb1_LockonTruth(attention) ? 1 : -1);
    if (attList == 0) {
        attList = cb1_GetActionList(attention, 0);
    }
    if (attList) {
        fopAc_ac_c* a = cb1_AttList_getActor(attList);
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
VERIFY(0x022220FC, &daNpc_Cb1_c::calcStickPos);

/* 02222DD0 */
BOOL daNpc_Cb1_c::walkAction(f32 targetSpeed, f32 accel, s16 targetAngle) {
    WWHD_FUNC(0x02222DD0, BOOL, this, targetSpeed, accel, targetAngle);
    cLib_chaseAngleS(&current.angle.y, targetAngle, 0x400);
    lookBack(1);
    shape_angle.y = current.angle.y;
    if (cLib_chaseF(&speedF, targetSpeed, accel) && targetSpeed == 0.0f) {
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x10018BE0); /* &daNpc_Cb1_c::waitNpcAction */
        cb1_setNpcAction(this, fn, nullptr);
        return TRUE;
    }
    f32 playSpeed = fabsf(speedF) * cb1_HIO().mWalkAnmSpeedScale;
    f32 mx = cb1_HIO().mMaxWalkAnmSpeed;
    playSpeed = (playSpeed - mx >= 0.0f) ? playSpeed : mx; /* cLib_minLimit (fsel) */
    mpMorf->setPlaySpeed(playSpeed);
    return FALSE;
}
VERIFY(0x02222DD0, &daNpc_Cb1_c::walkAction);

/* 02222EF4 */
BOOL daNpc_Cb1_c::sowCheck() {
    WWHD_FUNC(0x02222EF4, BOOL, this);
    if (!cb1_event_runCheck() && !(m4E4 & 1) /* !isReturnLink() */) {
        u32 list = cb1_getActionBtnB(cb1_attention());
        /* HD: null check on the list's actor */
        fopAc_ac_c* actor;
        if (list && (actor = cb1_AttList_getActor(list)) != nullptr) {
            u32 type = gabi::load<u32>(list + 8);
            if ((type == 3 || type == 1) && (gabi::load<u32>(gabi::ea(actor) + 0x39C) & 0x10000000)) {
                if (m8E2 != 0x2D) {
                    gabi::Local<cXyz> pos;
                    pos->x = 0.0f;
                    pos->y = 1.0f;
                    pos->z = 0.0f;
                    dComIfGp_getVibration_StartShock(4, -0x21, pos);
                }
                gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0x2D); /* dComIfGp_setDoStatus */
                if (gabi::call<BOOL>(0x02007898, 0)) { /* CPad_CHECK_TRIG_A(0) */
                    eventInfo_onCondition(this, 1);
                    gabi::call(0x025D744C, this, actor); /* fopAcM_orderTalkEvent */
                    return TRUE;
                }
                return FALSE;
            }
        }
    }
    return flyCheck();
}
VERIFY(0x02222EF4, &daNpc_Cb1_c::sowCheck);

/* HD message flow: the message manager at *(0x101F4B5C); status 025F795C, setStatus 025F74D0,
 * messageSet 025F7DB0 (mgr, msgNo, pos), tactMessageSet 025F8088 (mgr); l_msgId at 0x10466B28 */
#define CB1_L_MSG_ID 0x10466B28
static inline u32 cb1_msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 cb1_msgStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void cb1_msgSetStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }

/* 02223308 HD: the message id is only set here; no fopMsgM_SearchByID */
BOOL daNpc_Cb1_c::initTalk() {
    WWHD_FUNC(0x02223308, BOOL, this);
    if (gabi::load<s32>(CB1_L_MSG_ID) == -1) {
        u32 mgr = cb1_msgMgr();
        if (mMsgNo == 0x5AC) {
            gabi::store<u32>(CB1_L_MSG_ID, gabi::call<u32>(0x025F8088, mgr)); /* fopMsgM_tactMessageSet */
        } else {
            gabi::store<u32>(CB1_L_MSG_ID, gabi::call<u32>(0x025F7DB0, mgr, (u32)mMsgNo, &eyePos)); /* fopMsgM_messageSet */
        }
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02223308, &daNpc_Cb1_c::initTalk);

/* 02223380 */
BOOL daNpc_Cb1_c::execTalk(BOOL param_1) {
    WWHD_FUNC(0x02223380, BOOL, this, param_1);
    u32 mgr = cb1_msgMgr();
    u16 st = (u16)cb1_msgStatus(mgr);
    if (st == 0xE) { /* fopMsgStts_MSG_DISPLAYED_e */
        cb1_msgSetStatus(mgr, next_msgStatus(&mMsgNo));
        if (cb1_msgStatus(mgr) == 0xF) {
            gabi::call(0x025F7DB0, mgr, (u32)mMsgNo, 0); /* fopMsgM_messageSet */
        }
    } else if (st == 0x15) { /* fopMsgStts_INPUT_e */
        if (param_1) {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BD3) != 0) { /* dComIfGp_checkMesgCancelButton() */
                cb1_msgSetStatus(mgr, 0x10);
                gabi::store<u8>(mgr + 0x921, 1); /* fopMsgM_messageSendOn() */
                cb1_m_status() = cb1_m_status() | daCbStts_TACT_CANCEL;
            }
            if (cb1_m_status() & daCbStts_TACT_CORRECT) {
                cb1_msgSetStatus(mgr, 0x10);
                gabi::store<u8>(mgr + 0x921, 1);
            }
        }
    } else if (st == 6) { /* fopMsgStts_MSG_TYPING_e */
        if (mMsgNo != 0x5AC) {
            setAnm(getAnmType(gabi::load<u8>(dComIfGp_ea() + 0x5BC5) /* dComIfGp_getMesgAnimeAttrInfo() */));
        }
    } else if (st == 0x12) { /* fopMsgStts_BOX_CLOSED_e */
        cb1_msgSetStatus(mgr, 0x13);
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02223380, &daNpc_Cb1_c::execTalk);

/* 02223500 */
BOOL daNpc_Cb1_c::talkNpcAction(void*) {
    WWHD_FUNC(0x02223500, BOOL, this, (void*)nullptr);
    if (m8F0 == 0) {
        gabi::store<s32>(CB1_L_MSG_ID, -1);
        mMsgNo = getMsg();
        gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) & ~0x10u); /* attention_info.flags */
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
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
            if (ship != 0) {
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                gabi::Local<cXyz> d;
                cXyz_mi(&link->current.pos, d, &current.pos);
                gabi::Local<cXyz> temp;
                cXyz_mi(&current.pos, temp, d);
                /* daShip_c::setAtnPos */
                gabi::store<u32>(ship + 0x644, gabi::load<u32>(ship + 0x644) | 0x1000000);
                gabi::store<f32>(ship + 0x1350, temp->x);
                gabi::store<f32>(ship + 0x1354, temp->y);
                gabi::store<f32>(ship + 0x1358, temp->z);
            }
        } else {
            mJntCtrl.mbTrn = 1; /* setTrn() */
            lookBack(1);
        }
    }
    return TRUE;
}
VERIFY(0x02223500, &daNpc_Cb1_c::talkNpcAction);

/* 02223048 */
BOOL daNpc_Cb1_c::waitNpcAction(void* param_1) {
    WWHD_FUNC(0x02223048, BOOL, this, param_1);
    if (m8F0 == 0) {
        if (cb1_isEventBit(0x1610)) {
            gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) | 0x10); /* fopAc_Attn_ACTION_CARRY_e */
        }
        u8 anm = 0;
        if (param_1) {
            if (cb1_m_status() & daCbStts_PLAYER_FIND) {
                anm = 0xF;
                cb1_monsSeStart(this, 0x491A /* JA_SE_CV_CB_LEFT_ALONE */, 0);
            } else {
                anm = 0x14;
                cb1_monsSeStart(this, 0x491C /* JA_SE_CV_CB_TROUBLE */, 0);
            }
        }
        setAnm(anm);
        cb1_m_status() = cb1_m_status() & ~daCbStts_PLAYER_FIND;
        speedF = 0.0f;
    } else if (m8F0 != -1) {
        if (m8E1) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, 0x10018BE8); /* &daNpc_Cb1_c::searchNpcAction */
            cb1_setNpcAction(this, fn, nullptr);
        } else {
            mHasAttention = chkAttention(m8DC == 0x14 ? 4000.0f : (f32)cb1_HIO().mNpc.mMaxAttnDistXZ, 0x10000);
            BOOL temp = mHasAttention;
            u32 flags = gabi::ea(this) + 0x39C;
            if (temp && getMsg()) {
                m8DD = 6;
                gabi::store<u32>(flags, gabi::load<u32>(flags) | 0xA); /* ACTION_SPEAK | LOCKON_TALK */
            } else {
                gabi::store<u32>(flags, gabi::load<u32>(flags) & ~0xAu);
            }
            f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
            if (!(mNoResetFlg1 & 2)) { /* !checkNpcCallCommand() */
                if (cb1_isEventBit(0x1610) && dist_sq < (f32)cb1_HIO().field_0xC0 * (f32)cb1_HIO().field_0xC0) {
                    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                    gabi::store<u32>(gabi::ea(link) + 0x3BC, gabi::load<u32>(gabi::ea(link) + 0x3BC) | 2); /* onNpcCall() */
                    temp = TRUE;
                }
            } else {
                if (!(dist_sq < (f32)cb1_HIO().mPlayerChaseDistance * (f32)cb1_HIO().mPlayerChaseDistance)) {
                    gabi::Local<ProcFunc_l> fn;
                    md_pmf_load(fn, 0x10018BE8);
                    cb1_setNpcAction(this, fn, nullptr);
                }
                temp = TRUE;
            }
            lookBack(temp);
            current.angle.y = shape_angle.y;
        }
    }
    return TRUE;
}
VERIFY(0x02223048, &daNpc_Cb1_c::waitNpcAction);

/* 02223694 */
BOOL daNpc_Cb1_c::carryNpcAction(void*) {
    WWHD_FUNC(0x02223694, BOOL, this, (void*)nullptr);
    if (m8F0 == 0) {
        gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) & ~0x10u);
        mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
        fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0); /* fopAcM_toPlayerShapeAngleY: the player first */
        s16 myAngle = shape_angle.y;
        m8F4 = (s16)(myAngle - pPlayer->shape_angle.y);
        if (cb1_m_status() & daCbStts_NO_CARRY_ACTION) {
            cb1_monsSeStart(this, 0x48BF /* JA_SE_CV_CB_DAMAGE */, 0);
            setAnm(0xB);
        } else {
            cb1_monsSeStart(this, 0x48BC /* JA_SE_CV_CB_LIFT_UP */, 0);
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
            gabi::call(0x02445438, this, 1); /* daPy_npc_c::setRestart */
            fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
            /* mpMorf is re-read after each virtual call, as the original does */
            mDoExt_McaMorf* morf0 = mpMorf;
            f32 oldPlaySpeed = morf0->getPlaySpeed();
            if (0.0f == pPlayer->speedF) {
                /* getBaseAnimeFrame: HD vtable slot 0xA4; its f1 is used unrounded (compare, fctiwz) */
                f64 baseFrame = gabi::call_ptr<f64>(gabi::load<u32>(pPlayer->__vtbl + 0xA4), pPlayer);
                mDoExt_McaMorf* morf1 = mpMorf;
                if (!(baseFrame > (f64)morf0->getEndFrame())) { /* HD: ble into this block (unordered taken) */
                    f64 f = gabi::call_ptr<f64>(gabi::load<u32>(pPlayer->__vtbl + 0xA4), pPlayer);
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
            gabi::Local<ProcFunc_l> fn;
            if (speedF > 0.0f) {
                md_pmf_load(fn, 0x10018C08); /* &daNpc_Cb1_c::flyNpcAction */
            } else {
                md_pmf_load(fn, 0x10018BE0); /* &daNpc_Cb1_c::waitNpcAction */
            }
            cb1_setNpcAction(this, fn, nullptr);
        }
        gabi::Local<cXyz> zero;
        *zero = *gabi::at<cXyz>(0x101FFBA8); /* cXyz::Zero */
        cb1_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, zero, shape_angle.y, 0, 0);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x02223694, &daNpc_Cb1_c::carryNpcAction);

/* 02223A88 */
BOOL daNpc_Cb1_c::flyNpcAction(void*) {
    WWHD_FUNC(0x02223A88, BOOL, this, (void*)nullptr);
    s8 temp = m8F0;
    if (temp == 0) {
        speedF = cb1_HIO().mNpcFlyLaunchSpeedF;
        speed.y = cb1_HIO().mNpcFlyLaunchSpeedY;
        setAnm(4);
        cb1_m_flyingTimer() = cb1_HIO().mNpcFlyTimer;
        m8FC = (f32)(cb1_HIO().field_0xD8 + 1);
        cb1_monsSeStart(this, 0x48BD /* JA_SE_CV_CB_THROW */, 0);
    } else if (temp != -1) {
        flyAction(m8F1 ? FALSE : TRUE, 0.0f, 0, FALSE);
        if (temp != 1) {
            m8F1 = (s8)((m8F1 + 1) % cb1_HIO().field_0xDE);
        }
    }
    return TRUE;
}
VERIFY(0x02223A88, &daNpc_Cb1_c::flyNpcAction);

/* fopAcM_seStart as this TU inlines it (no NULL checks): 025E1A40 mDoAud_seStart */
static inline void cb1_seStart(fopAc_ac_c* a, u32 id, u32 param) {
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
static inline void cb1_flyAir(daNpc_Cb1_c* i_this, bool isMakarPlayer, f32 param_2, s16 param_3, f32 temp3, f32& ySpeedLimit, BOOL& temp4) {
    daNpc_Cb1_HIO_l& hio = cb1_HIO();
    BOOL temp7 = i_this->calcFlyingTimer();
    if (isMakarPlayer) {
        if (temp7) {
            cb1_monsSeStart(i_this, 0x4922 /* JA_SE_CV_CB_FLY_END */, 0);
        }
        cb1_onFlying();
    }
    i_this->m900 = 0.0f;
    gabi::Local<cXyz> temp;
    temp->x = 0.0f;
    temp->y = gabi::fadds_ppc(temp3, hio.field_0xA0);
    temp->z = 0.0f;
    f32 temp5 = hio.field_0x9C;
    if (i_this->m8F2) {
        temp->x = gabi::fmuls_ppc(param_2, cM_ssin(param_3));
        temp->z = gabi::fmuls_ppc(param_2, cM_scos(param_3));
    }
    if (!(cb1_m_status() & daNpc_Cb1_c::daCbStts_MUSIC) && i_this->mWindCyl.ChkTgHit()) {
        void* tg = i_this->mWindCyl.GetTgHitObj();
        if (tg && (gabi::load<u32>(gabi::ea(tg) + 0x10) & 0x200000)) { /* ChkAtType(AT_TYPE_WIND) */
            gabi::Local<cXyz> v;
            cXyz_ml(gabi::at<cXyz>(gabi::ea(&i_this->mWindCyl) + 0xC0) /* GetTgRVecP() */, v, 0.01f);
            PSVECAdd(temp, v, temp);
            temp5 = 30.0f;
            ySpeedLimit = 30.0f;
        }
        i_this->m8F8 = 20000;
        temp4 = TRUE;
    } else {
        gabi::Local<cXyz> wind;
        gabi::Local<be<f32>> power;
        gabi::call(0x0257E1B8, &i_this->current.pos, wind.get(), power.get()); /* dKyw_get_AllWind_vec */
        *power = gabi::fmuls_ppc(*power, hio.field_0xB8);
        gabi::Local<cXyz> v;
        cXyz_ml(wind, v, *power);
        PSVECAdd(temp, v, temp);
        temp5 = temp5 + *power;
        ySpeedLimit = ySpeedLimit + *power;
        s16 temp8 = (s16)gabi::ftoi(*power * 400.0f);
        if (i_this->m8F8 < temp8) {
            i_this->m8F8 = temp8;
        }
    }
    s16 angle = cM_atan2s(temp->x, temp->z);
    f32 mag = std_sqrtf(gabi::fmadds(temp->x, temp->x, temp->z * temp->z));
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
    PSVECAdd(&i_this->speed, temp, &i_this->speed);
    gabi::Local<be<f32>> temp8;
    *temp8 = std_sqrtf(PSVECSquareMag(&i_this->speed));
    if (!(fabsf(*temp8) < 3.814697265625e-06f) && *temp8 > temp5) { /* !cM3d_IsZero */
        f32 temp2 = *temp8;
        gabi::call<BOOL>(0x0200F5C8, temp8.get(), temp5, 1.0f); /* cLib_chaseF */
        if (*temp8 > 30.0f) {
            *temp8 = 30.0f;
        }
        PSVECScale(&i_this->speed, &i_this->speed, *temp8 / temp2);
    }
    i_this->current.angle.y = cM_atan2s(i_this->speed.x, i_this->speed.z);
    i_this->speedF = std_sqrtf(gabi::fmadds(i_this->speed.x, i_this->speed.x, i_this->speed.z * i_this->speed.z));
    if (i_this->speed.y > ySpeedLimit) {
        i_this->speed.y = ySpeedLimit;
    }
}

/* 022222D0 */
BOOL daNpc_Cb1_c::flyAction(BOOL param_1, f32 param_2, s16 param_3, BOOL param_4) {
    WWHD_FUNC(0x022222D0, BOOL, this, param_1, param_2, param_3, param_4);
    daNpc_Cb1_HIO_l& hio = cb1_HIO();
    bool isMakarPlayer = gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this);
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
            cb1_seStart(this, 0x58C0 /* JA_SE_CM_PRAPELLO_OPEN */, 0);
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x830B /* ID_AK_SN_PERAPROOPEN00 */, &current.pos);
            if (emitter) {
                /* setGlobalRTMatrix(mpPropellerModel->getAnmMtx(m_center_jnt_num)); HD: getAnmMtx marks the
                 * joint matrices dirty */
                u32 blk = gabi::load<u32>(gabi::ea(mpPropellerModel.get()) + 0x2C);
                u32 mtx = (s8)m_center_jnt_num * 0x30 + gabi::load<u32>(blk + 0x10);
                gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
                gabi::call(0x028249B0, mtx, gabi::ea(emitter) + 0x1F0, gabi::ea(emitter) + 0x22C); /* JPASetRMtxTVecfromMtx */
            }
        }
    } else if (m8F0 == 2) {
        if (isMakarPlayer && (temp4 || cb1_m_flyingTimer() < hio.field_0xCE)) {
            if (setAnm(0x15) && temp4) {
                cb1_monsSeStart(this, 0x4922 /* JA_SE_CV_CB_FLY_END */, 0);
            }
        } else if (m8DC != 6 && m8FC > (f32)(s16)hio.field_0xD8) {
            setAnm(7);
        }
        if (cb1_m_flyingTimer() != 0 && param_1) {
            gabi::call(0x0200F564, &m8F8, (s16)hio.field_0xD0, (s16)hio.field_0xD4); /* cLib_chaseS */
            m8F6 = 0;
        } else {
            gabi::call(0x0200F564, &m8F8, mAcch.ChkGroundHit() ? (s16)hio.field_0xD2 : (s16)0, (s16)hio.field_0xD6);
        }
        m8FA = (s16)(m8FA + m8F8);
        m8FC = gabi::fadds_ppc(m8FC, gabi::fsubs_ppc(current.pos.y, old.pos.y));
        if (m8FC > hio.field_0xA4) {
            m8F2 = 1;
        }
        cb1_seStart(this, 0x50BF /* JA_SE_CM_PRAPELLO_ROLLING */, cb1_ftou((f32)(s16)m8F8 * (100.0f / (f32)(s16)hio.field_0xD0)));
        if (param_4 || (!isMakarPlayer && mAcch.ChkWallHit()) || (!mAcch.ChkGroundHit() && m8F8 == 0) ||
            (mAcch.ChkGroundHit() && m8F8 <= hio.field_0xD2 &&
             (!(gabi::call<f64>(0x020079B4, 0) /* CPad_GET_STICK_VALUE(0), f1 unrounded */ < (f64)hio.field_0x80) || /* bge: unordered taken */ (m8F6 = (s16)(m8F6 + 1)) > hio.field_0xCC))) {
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
            cb1_seStart(this, 0x58C0 /* JA_SE_CM_PRAPELLO_OPEN */, 0);
        }
        if (mAcch.ChkGroundHit()) {
            if (m8E0 == 0 && m8F0 == 3) {
                if (cb1_GetSpecialCode(cb1_gnd(this)) != 1) {
                    speed.y = hio.field_0x64;
                    checkLanding();
                }
                m8F0 = (s8)(m8F0 + 1);
            } else if (m8DC != 4) {
                gabi::Local<ProcFunc_l> fn;
                if (isMakarPlayer) {
                    md_pmf_load(fn, 0x10018BF8); /* &daNpc_Cb1_c::waitPlayerAction */
                    setPlayerAction(fn, nullptr);
                } else {
                    md_pmf_load(fn, 0x10018BE0); /* &daNpc_Cb1_c::waitNpcAction */
                    cb1_setNpcAction(this, fn, nullptr);
                    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
                    gabi::call(0x025052BC, camera + 0x248, fopAcM_GetID(this)); /* mCamera.ForceLockOff */
                }
                return FALSE;
            }
        }
    }
    return TRUE;
}
VERIFY(0x022222D0, &daNpc_Cb1_c::flyAction);

#define CB1_CUT_NAME_TBL 0x101BD91C /* l_cutNameTbl[15] */
#define CB1_EV_PROC_TBL 0x101BDB14  /* m_evProcTbl[15] {init, run} (pointers to member, 8 bytes each) */

/* 02220288 */
BOOL daNpc_Cb1_c::eventProc() {
    WWHD_FUNC(0x02220288, BOOL, this);
    mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::WALL_NONE; /* ClrWallNone() */
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */ && m8DD != -1) {
        if (m8DD == 0) {
            /* fopAcM_onDraw(this) (HD) */
            u32 r = gabi::call<u32>(0x025DF2B8, this);
            gabi::call(0x025DA874, gabi::ea(this) + 0xDC, r);
        } else if (m8DD == 1) {
            u32 link = gabi::load<u32>(dComIfGp_ea() + 0x5B34); /* dComIfGp_getLinkPlayer() */
            u32 evt = dComIfGp_ea() + 0x51D0;                   /* dComIfGp_event_setTalkPartner */
            gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, link));
            gabi::call(0x025E1988, 0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
        }
        m4E4 = m4E4 | 2; /* onEventAccept() */
        m8DD = -1;
    }

    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x10018F78) /* "Cb1" */, nullptr, 0);
    if (cb1_event_runCheck() && (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this) || !checkCommandTalk())) {
        if (staffIdx != -1) {
            s32 actIdx = gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffIdx, CB1_CUT_NAME_TBL, 15, 1, 0); /* getMyActIdx */
            if (actIdx == -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            } else {
                u32 entry = CB1_EV_PROC_TBL + actIdx * 16;
                if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                    md_pmf_call<void>(this, gabi::at<ProcFunc_l>(entry), staffIdx);
                    speedF = 0.0f;
                }
                if (md_pmf_call<BOOL>(this, gabi::at<ProcFunc_l>(entry + 8), staffIdx)) {
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
        if (gabi::load<u16>(gabi::load<u32>(dComIfGp_ea() + 0x5B34) + 0xF8) != 3) { /* !checkCommandDoor() */
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02220288, &daNpc_Cb1_c::eventProc);

/* 022213B8 */
static BOOL daNpc_Cb1_Execute(daNpc_Cb1_c* i_this) {
    WWHD_FUNC(0x022213B8, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x022213B8, daNpc_Cb1_Execute);

#define CB1_M_PLAYER_ROOM 0x101BDC2C /* bool daNpc_Cb1_c::m_playerRoom */
#define CB1_STR_CB1_POS 0x10018FA0   /* "Cb1" (checkNowPosMove) */

/* execute: the room/ground part (not carried, not on the ship); returns TRUE when execute ends here */
static inline BOOL cb1_execRoom(daNpc_Cb1_c* i_this) {
    daNpc_Cb1_HIO_l& hio = cb1_HIO();
    u32 self = gabi::ea(i_this);
    BOOL initialRoom = gabi::call<BOOL>(0x024451B4, i_this); /* check_initialRoom */
    if (!initialRoom ||
        (fopAcM_GetParam(i_this) == 5 && !gabi::call<BOOL>(0x024EEB2C, dComIfG_Bgsp(), cb1_gnd(i_this)) /* ChkMoveBG_NoDABg */)) {
        i_this->home.roomNo = -1;
        i_this->current.roomNo = -1;
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == self) { /* dComIfGp_getCb1Player() */
            gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0);
        }
        return TRUE;
    }
    gabi::call(0x0244586C, i_this, 1, fopAcM_GetParam(i_this) == 4); /* initialRestartOption(1, isTypeKaze()) */
    if (!(cb1_m_status() & daNpc_Cb1_c::daCbStts_SHIP_RIDE) && gabi::call<BOOL>(0x024452A0, i_this) /* check_moveStop */) {
        i_this->setWaitNpcAction(nullptr);
        return TRUE;
    }
    if (i_this->m8E0 && gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), &i_this->mPolyInfo) /* ChkPolySafe */ &&
        gabi::call<BOOL>(0x024EEABC, dComIfG_Bgsp(), &i_this->mPolyInfo) /* ChkMoveBG */) {
        gabi::call(0x024EF968, dComIfG_Bgsp(), &i_this->mPolyInfo, 1, &i_this->old.pos, 0, 0); /* MoveBgCrrPos */
    }
    dNpc_HIO_l& n = hio.mNpc;
    i_this->mJntCtrl.setParam(n.mMaxBackboneX, n.mMaxBackboneY, n.mMinBackboneX, n.mMinBackboneY, n.mMaxHeadX, n.mMaxHeadY,
                              n.mMinHeadX, n.mMinHeadY, n.mMaxTurnStep);
    return FALSE;
}

/* execute: falling off the map (ground code 4 or no ground) */
static inline void cb1_execFallCheck(daNpc_Cb1_c* i_this) {
    if (i_this->isFlyAction() && i_this->m8FC > -400.0f) {
        i_this->m4E8 = 0;
    } else if (i_this->m4E8 < 0x1E) {
        i_this->m4E8 = (u8)(i_this->m4E8 + 1);
    } else {
        bool reset = cb1_pmfIs(&i_this->mNpcAction, 0x0222487C); /* HD: an extra NPC action (0222487C) always resets */
        if (!reset) {
            s8 room = i_this->home.roomNo;
            (void)dComIfGp_ea();
            /* dComIfGp_roomControl_checkStatusFlag(fopAcM_GetHomeRoomNo(this), 0x10) */
            reset = !(gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 0x10);
        }
        if (reset) {
            u32 a = gabi::ea(i_this);
            gabi::store<u32>(a + 0x324, gabi::load<u32>(a + 0x2FC)); /* current = home; shape_angle = home.angle */
            gabi::store<u32>(a + 0x314, gabi::load<u32>(a + 0x2EC));
            gabi::store<u16>(a + 0x328, gabi::load<u16>(a + 0x2F8));
            gabi::store<u16>(a + 0x32C, gabi::load<u16>(a + 0x2FC));
            gabi::store<u32>(a + 0x31C, gabi::load<u32>(a + 0x2F4));
            gabi::store<u16>(a + 0x32A, gabi::load<u16>(a + 0x2FA));
            gabi::store<u32>(a + 0x320, gabi::load<u32>(a + 0x2F8));
            i_this->speedF = 0.0f;
            i_this->m4E8 = 0;
            gabi::store<u32>(a + 0x318, gabi::load<u32>(a + 0x2F0));
            cb1_seStart(i_this, 0x491A /* JA_SE_CV_CB_LEFT_ALONE */, 0);
            /* HD: back to the wait action */
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, 0x10018BE0); /* &daNpc_Cb1_c::waitNpcAction */
            cb1_setNpcAction(i_this, fn, nullptr);
        } else {
            /* daPy_getPlayerLinkActorClass()->npcStartRestartRoom() */
            gabi::call(0x023FD4E4, gabi::load<u32>(dComIfGp_ea() + 0x5B34), 5, 0xC9, -1.0f, 0); /* startRestartRoom */
        }
    }
}

/* execute: ground polygon (slope, slide, room) */
static inline void cb1_execGround(daNpc_Cb1_c* i_this, cXyz* temp2, f32& temp3) {
    u8* gnd = cb1_gnd(i_this);
    u32 pla = gabi::ea(dBgS_GetTriPla(dComIfG_Bgsp(), gnd));
    if (pla) {
        i_this->m910.x = gabi::load<f32>(pla);
        i_this->m910.y = gabi::load<f32>(pla + 4);
        i_this->m910.z = gabi::load<f32>(pla + 8);
        if (i_this->m8E0) {
            i_this->maxFallSpeed = gabi::fnmsubs(100.0f, gabi::fsubs_ppc(1.0f, i_this->m910.y), -4.0f);
            if (cb1_GetSpecialCode(gnd) == 1) {
                gabi::Local<cXyz> xz;
                xz->x = i_this->m910.x;
                xz->y = 0.0f;
                xz->z = i_this->m910.z;
                f32 temp = PSVECSquareMag(xz); /* m910.abs2XZ() */
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
    s8 roomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gnd); /* GetRoomId */
    u32 a = gabi::ea(i_this);
    gabi::store<u8>(a + 0x1C9, roomNo); /* tevStr.mRoomNo */
    i_this->current.roomNo = roomNo;
    u8 color = (u8)gabi::call<u32>(0x024EEEB8, dComIfG_Bgsp(), gnd); /* GetPolyColor */
    gabi::store<u8>(a + 0x7E2, roomNo); /* mStts.SetRoomId */
    /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
    i_this->mPolyInfo.mPolyIndex = gabi::load<u16>(gabi::ea(gnd));
    i_this->mPolyInfo.mProcId = gabi::load<u32>(gabi::ea(gnd) + 8);
    i_this->mPolyInfo.mpBgW = gabi::load<u32>(gabi::ea(gnd) + 4);
    gabi::store<u8>(a + 0x1CA, color); /* tevStr.mEnvrIdxOverride */
    i_this->mPolyInfo.mBgIndex = gabi::load<u16>(gabi::ea(gnd) + 2);
}

/* execute: the damage reaction (player or NPC) */
static inline void cb1_execDamage(daNpc_Cb1_c* i_this, u32 pmf, bool player) {
    if (i_this->mDamageFogTimer == 0 && !(cb1_m_status() & daNpc_Cb1_c::daCbStts_MUSIC) && i_this->mCyl.ChkTgHit() &&
        i_this->mCyl.GetTgHitObj()) {
        gabi::Local<cXyz> temp;
        cXyz_mi(&i_this->current.pos, temp, gabi::at<cXyz>(gabi::ea(&i_this->mCyl) + 0xCC) /* GetTgHitPosP() */);
        i_this->current.angle.y = cM_atan2s(temp->x, temp->z);
        i_this->speedF = cb1_HIO().field_0xC4;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, pmf);
        if (player) {
            i_this->setPlayerAction(fn, nullptr);
        } else {
            cb1_setNpcAction(i_this, fn, nullptr);
        }
        i_this->mDamageFogTimer = cb1_HIO().mDamageTimer; /* setDamageFogTimer */
        cb1_monsSeStart(i_this, 0x48BF /* JA_SE_CV_CB_DAMAGE */, 0);
    }
}

/* 0222091C */
BOOL daNpc_Cb1_c::execute() {
    WWHD_FUNC(0x0222091C, BOOL, this);
    daNpc_Cb1_HIO_l& hio = cb1_HIO();
    actor_status = actor_status & ~0x20u; /* fopAcStts_SHOWMAP_e */
    gabi::call(0x0207A9A0, &mDamageFogTimer); /* executeDamageFog(): cLib_calcTimer<u8> */
    fopAcM_setStageLayer(this);
    /* m_playerRoom = fopAcM_GetRoomNo(this) == dComIfGp_roomControl_getStayNo() */
    gabi::store<u8>(CB1_M_PLAYER_ROOM, (s8)current.roomNo == gabi::load<s8>(0x1047E6C8));
    cb1_m_status() = cb1_m_status() & ~daCbStts_UNK_0100;

    if (!(cb1_m_status() & daCbStts_SHIP_RIDE) && !(actor_status & 0x2000)) {
        if (cb1_execRoom(this)) {
            return TRUE;
        }
    }

    gabi::Local<cXyz> temp2;
    *temp2 = *gabi::at<cXyz>(0x101FFBA8); /* cXyz::Zero */
    f32 temp3 = 3.0f;

    if (!(actor_status & 0x2000) && !(cb1_m_status() & daCbStts_SHIP_RIDE)) {
        if (gabi::call<BOOL>(0x02445950, this, CB1_STR_CB1_POS) /* checkNowPosMove("Cb1") */ && !(m4E4 & 1)) {
            f32 temp4 = current.pos.y;
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts)) /* mStts.GetCCMoveP() */);
            m900 = gabi::fadds_ppc(m900, gabi::fsubs_ppc(temp4, current.pos.y));
            current.pos.x = gabi::fadds_ppc(current.pos.x, m88C.x);
            current.pos.z = gabi::fadds_ppc(current.pos.z, m88C.z);
            maxFallSpeed = -100.0f;
            gravity = hio.field_0x70;
        }
        m8E0 = mAcch.ChkGroundHit();
        mAcch.CrrPos(dComIfG_Bgsp());
        cb1_m_status() = cb1_m_status() | daCbStts_UNK_0100;
        if (mAcch.m_ground_h == -1000000000.0f || gabi::call<s32>(0x024EF0BC, dComIfG_Bgsp(), cb1_gnd(this)) == 4 /* GetGroundCode */) {
            cb1_execFallCheck(this);
        } else {
            m4E8 = 0;
        }
        if (mAcch.m_ground_h != -1000000000.0f) {
            cb1_execGround(this, temp2, temp3);
        }
        setCollision();
    }

    if (gabi::call<BOOL>(0x02445950, this, CB1_STR_CB1_POS)) { /* checkNowPosMove("Cb1") */
        playAnm();
    }

    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this)) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0);    /* dComIfGp_setDoStatus(dActStts_BLANK_e) */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 0x3E); /* dComIfGp_setAStatus(dActStts_HIDDEN_e) */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB8, 0);    /* dComIfGp_setRStatusForce(dActStts_BLANK_e) */
    }

    if (!eventProc()) {
        cb1_m_status() = cb1_m_status() & ~daCbStts_TACT; /* offTact() */
        if (!isFlyAction() && !(cb1_m_status() & daCbStts_MUSIC) && mWindCyl.ChkTgHit()) {
            cXyz* rvec = gabi::at<cXyz>(gabi::ea(&mWindCyl) + 0xC0); /* GetTgRVecP() */
            gabi::Local<cXyz> xz;
            f32 rx = rvec->x;
            temp2->y = rvec->y;
            xz->z = rvec->z;
            xz->y = 0.0f;
            temp2->x = rx;
            xz->x = rx;
            temp2->z = xz->z;
            f32 temp = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
            if (temp < 1.0f) {
                gabi::Local<cXyz> d;
                cXyz_mi(&current.pos, d, gabi::at<cXyz>(gabi::ea(&mWindCyl) + 0xCC) /* GetTgHitPosP() */);
                gabi::Local<cXyz> m;
                cXyz_ml(d, m, 30.0f);
                *temp2 = *m;
            }
            if (temp > 30.0f) {
                PSVECScale(temp2, temp2, 30.0f / temp);
            }
            temp3 = 1.0f;
        }
        gabi::call(0x0200EF78, &m88C, temp2.get(), 0.5f, temp3, 0.5f); /* cLib_addCalcPosXZ */
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this)) {
            if (mAcch.ChkGroundHit()) {
                gabi::store<u8>(dComIfGp_ea() + 0x5BB8, 7); /* dComIfGp_setRStatusForce(dActStts_RETURN_e) */
            }
            actor_status = (actor_status & ~0x3Fu) | 0x32; /* fopAcM_SetStatusMap(this, 0x12) (HD 0x32) */
            if (m4E4 & 1) { /* isReturnLink() */
                m8DD = 1;
            } else {
                if (cb1_m_status() & daCbStts_NO_CARRY_ACTION) {
                    gabi::Local<ProcFunc_l> fn;
                    md_pmf_load(fn, 0x10018C50); /* &daNpc_Cb1_c::carryPlayerAction */
                    setPlayerAction(fn, nullptr);
                    m4E4 = m4E4 | 1; /* returnLink() */
                }
                if (!cb1_event_runCheck() && gabi::load<u8>(dComIfGp_ea() + 0x5BB8) == 7 && !cb1_event_runCheck() &&
                    gabi::call<BOOL>(0x02007840, 0) /* CPad_CHECK_TRIG_R(0) || CPad_CHECK_TRIG_START(0) */) {
                    m4E4 = m4E4 | 1; /* returnLink() */
                }
                playerAction(nullptr);
                cb1_execDamage(this, 0x10018C58 /* &daNpc_Cb1_c::hitPlayerAction */, true);
            }
            m8E2 = gabi::load<u8>(dComIfGp_ea() + 0x5BB7); /* dComIfGp_getDoStatus() */
        } else {
            m8E1 = (s8)gabi::call<s32>(0x02445AA4, this, &m91C); /* chkMoveBlock */
            u32 map = cb1_isEventBit(0x1610) ? 0x2D : 0x27; /* fopAcM_SetStatusMap 0xD / 0x7 (HD 0x2D / 0x27) */
            actor_status = (actor_status & ~0x3Fu) | map;
            carryCheck();
            checkOrder();
            npcAction(nullptr);
            if (!cb1_pmfIs(&mNpcAction, CB1_flyNpcAction)) {
                current.angle.y = shape_angle.y;
            }
            cb1_execDamage(this, 0x10018C28 /* &daNpc_Cb1_c::hitNpcAction */, false);
            m8E2 = 0;
        }
    }
    eventOrder();
    gabi::call(0x0221ED08, this); /* setBaseMtx */
    /* HD: attention_info.position and eyePos are not set here */
    return TRUE;
}
VERIFY(0x0222091C, &daNpc_Cb1_c::execute);
