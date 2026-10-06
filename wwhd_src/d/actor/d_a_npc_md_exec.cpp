/**
 * d_a_npc_md_exec.cpp (WWHD)
 * Player - Medli: animation, collision, action dispatch, execute/draw, first event actions
 * (WWHD 02286B5C..0228AB5F).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define MD_M_MIRROR 0x101D5F3F /* bool daNpc_Md_c::m_mirror (returnLinkPlayer) */
/* 0259D344 dNpc_setAnmFNDirect(morf, loopMode, morf, speed, filename, sound, arc) */
static inline bool dNpc_setAnmFNDirect(mDoExt_McaMorf* m, int loop, f32 morf, f32 speed, const char* fn, const char* snd, const char* arc) {
    return gabi::call<bool>(0x0259D344, m, loop, morf, speed, fn, snd, arc);
}
/* 023D4688 daPy_py_c::changePlayer(fopAc_ac_c*) */
static inline void daPy_changePlayer(fopAc_ac_c* self, fopAc_ac_c* pl) { gabi::call(0x023D4688, self, pl); }
/* 02007840: HD pad check, GameCube CPad_CHECK_TRIG_R(0) || CPad_CHECK_TRIG_START(0) */
static inline BOOL md_padTrigRorStart(s32 port) { return gabi::call<BOOL>(0x02007840, port); }
/* 028245AC JPAGetXYZRotateMtx(x, y, z, Mtx) */
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx) { gabi::call(0x028245AC, x, y, z, mtx); }
static inline u8 md_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }

/* JPABaseEmitter (HD): flags at +0x254, global translation at +0x22C, rotation matrix at +0x1F0,
 * emitter callback at +0x1E4, max frame at +0x5C, global scale-y sign byte at +0x262 */
static inline void JPA_becomeInvalidEmitter(u32 e) {
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
}

/* 02286B5C checkPlayerRoom (not named by the matcher) */
void daNpc_Md_c::checkPlayerRoom() {
    WWHD_FUNC(0x02286B5C, void, this);
    gabi::store<u8>(MD_M_PLAYERROOM, 0); /* offPlayerRoom() */
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if ((u32)(s32)current.roomNo == (u32)(s32)link->current.roomNo) {
        gabi::store<u8>(MD_M_PLAYERROOM, 1);
    }
}
VERIFY(0x02286B5C, &daNpc_Md_c::checkPlayerRoom);

/* 02286D04 dNpc_Md_setAnm(mDoExt_McaMorf*, ...) (not named by the matcher) */
bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf* pMorf, int loopMode, f32 morf, f32 speed, const char* animFilename, const char* arcName) {
    WWHD_FUNC(0x02286D04, bool, this, pMorf, loopMode, morf, speed, animFilename, arcName);
    return dNpc_setAnmFNDirect(pMorf, loopMode, morf, speed, animFilename, nullptr, arcName);
}
VERIFY(0x02286D04, (bool (daNpc_Md_c::*)(mDoExt_McaMorf*, int, f32, f32, const char*, const char*)) & daNpc_Md_c::dNpc_Md_setAnm);

/* 0228735C playLightBtkAnm (not named by the matcher) */
u32 daNpc_Md_c::playLightBtkAnm() {
    WWHD_FUNC(0x0228735C, u32, this);
    return mLightBtkAnm.play();
}
VERIFY(0x0228735C, &daNpc_Md_c::playLightBtkAnm);

/* 02287684 isFallAction (not named by the matcher) */
BOOL daNpc_Md_c::isFallAction() {
    WWHD_FUNC(0x02287684, BOOL, this);
    /* chkPlayerAction(jumpPlayerAction) || chkNpcAction(fallNpcAction) || chkNpcAction(fall02NpcAction);
     * GHS compares pointers to member as: same i, and (i == 0 or same d and f); the constants are
     * {d = 0, i = -1, f} */
    s16 pi = mCurrPlayerActionFunc.i;
    if (pi == -1) {
        if (pi == 0) return TRUE;
        if (mCurrPlayerActionFunc.d == 0 && mCurrPlayerActionFunc.f == 0x02293FA8u) return TRUE;
    }
    s16 ni = mCurrNpcActionFunc.i;
    if (ni == -1) {
        if (ni == 0) return TRUE;
        s16 nd = mCurrNpcActionFunc.d;
        if (nd == 0 && mCurrNpcActionFunc.f == 0x02291644u) return TRUE;
        if (nd == 0 && mCurrNpcActionFunc.f == 0x02291764u) return TRUE;
    }
    return FALSE;
}
VERIFY(0x02287684, &daNpc_Md_c::isFallAction);

/* 02287720 */
void daNpc_Md_followEcallBack_c::end() {
    WWHD_FUNC(0x02287720, void, this);
    if (mpEmitter.get() != nullptr) {
        JPA_becomeInvalidEmitter(gabi::ea(mpEmitter.get()));
        gabi::store<u32>(gabi::ea(mpEmitter.get()) + 0x1E4, 0); /* setEmitterCallBackPtr(NULL) */
        mpEmitter = nullptr;
    }
}
VERIFY(0x02287720, &daNpc_Md_followEcallBack_c::end);

/* 02287EAC */
void daNpc_Md_c::returnLinkPlayer() {
    WWHD_FUNC(0x02287EAC, void, this);
    daPy_changePlayer(this, dComIfGp_getLinkPlayer());
    gabi::store<u8>(MD_M_MIRROR, 0); /* offMirror() */
    gabi::store<u8>(MD_M_FLYING, 0); /* offFlying() */
}
VERIFY(0x02287EAC, &daNpc_Md_c::returnLinkPlayer);

/* 02288240 */
void daNpc_Md_c::setPlayerAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x02288240, void, this, actionFunc, arg);
    mCurrNpcActionFunc.d = 0;
    mCurrNpcActionFunc.i = 0;
    mCurrNpcActionFunc.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    setAction(&mCurrPlayerActionFunc, fn, arg);
}
VERIFY(0x02288240, &daNpc_Md_c::setPlayerAction);

/* 022884D4 */
BOOL daNpc_Md_c::returnLinkCheck() {
    WWHD_FUNC(0x022884D4, BOOL, this);
    if (!md_event_runCheck()) {
        /* HD: one pad call (GameCube CPad_CHECK_TRIG_R(0) || CPad_CHECK_TRIG_START(0)) */
        if (md_padTrigRorStart(0)) {
            if (mAcch.ChkGroundHit()) {
                return TRUE;
            }
        }
    }
    return FALSE;
}
VERIFY(0x022884D4, &daNpc_Md_c::returnLinkCheck);

/* 022886B4 */
void daNpc_Md_c::carryCheck() {
    WWHD_FUNC(0x022886B4, void, this);
    if (actor_status & 0x2000) { /* fopAcM_checkCarryNow */
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, PMF_carryNpcAction);
        setNpcAction(fn, nullptr);
    }
}
VERIFY(0x022886B4, &daNpc_Md_c::carryCheck);

/* 022887DC */
void daNpc_Md_c::npcAction(void* arg) {
    WWHD_FUNC(0x022887DC, void, this, arg);
    if (mCurrNpcActionFunc.i == 0) { /* mCurrNpcActionFunc == NULL */
        speedF = 0.0f;
        if (m3104 == 0x20) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, PMF_mwaitNpcAction);
            setNpcAction(fn, nullptr);
        } else {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, PMF_waitNpcAction);
            setNpcAction(fn, nullptr);
        }
    }
    md_pmf_call<void>(this, &mCurrNpcActionFunc, arg);
}
VERIFY(0x022887DC, &daNpc_Md_c::npcAction);

/* 02288AB4 emitterTrace (the matcher's emitterTrace 0228A174 is followEcallBack_c::execute) */
void daNpc_Md_c::emitterTrace(JPABaseEmitter* emitter, Mtx34* mtx, csXyz* angle) {
    WWHD_FUNC(0x02288AB4, void, this, emitter, mtx, angle);
    if (emitter == nullptr) {
        return;
    }
    /* lfs/stfs pairs: the recompiled code keeps signalling NaNs bit for bit, so the words are
     * copied as integers; the fneg below quiets them */
    u32 e = gabi::ea(emitter);
    u32 m = gabi::ea(mtx);
    u8 flag = gabi::load<u8>(e + 0x262);
    u32 y = gabi::load<u32>(m + 0x1C);
    u32 x = gabi::load<u32>(m + 0x0C);
    u32 z = gabi::load<u32>(m + 0x2C);
    gabi::store<u32>(e + 0x22C, x);
    gabi::store<u32>(e + 0x234, z);
    gabi::store<u32>(e + 0x230, y);
    /* HD: setGlobalTranslation negates y for emitters with a flag byte >= 7 */
    if (flag >= 7) {
        gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
    }
    if (angle == nullptr) {
        return;
    }
    JPAGetXYZRotateMtx(angle->x, angle->y, angle->z, e + 0x1F0);
}
VERIFY(0x02288AB4, &daNpc_Md_c::emitterTrace);

/* 02289E28 */
static BOOL daNpc_Md_Execute(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x02289E28, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x02289E28, daNpc_Md_Execute);

/* 0228A168 */
static BOOL daNpc_Md_Draw(daNpc_Md_c* i_this) {
    WWHD_FUNC(0x0228A168, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x0228A168, daNpc_Md_Draw);

/* 0228A16C */
static BOOL daNpc_Md_IsDelete(daNpc_Md_c*) {
    WWHD_FUNC(0x0228A16C, BOOL, (daNpc_Md_c*)nullptr);
    return TRUE;
}
VERIFY(0x0228A16C, daNpc_Md_IsDelete);

/* 0228A174 daNpc_Md_followEcallBack_c::execute (the matcher calls it emitterTrace) */
static void daNpc_Md_followEcallBack_execute(daNpc_Md_followEcallBack_c* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x0228A174, void, cb, emitter);
    /* lfs/stfs copies keep signalling NaNs (integer copies); fneg quiets them */
    u32 e = gabi::ea(emitter);
    u32 p = gabi::ea(&cb->mPos);
    u32 x = gabi::load<u32>(p + 0);
    u8 flag = gabi::load<u8>(e + 0x262);
    if (flag >= 7) {
        gabi::store<f32>(e + 0x230, -cb->mPos.y);
    } else {
        gabi::store<u32>(e + 0x230, gabi::load<u32>(p + 4));
    }
    gabi::store<u32>(e + 0x22C, x);
    gabi::store<u32>(e + 0x234, gabi::load<u32>(p + 8));
    JPAGetXYZRotateMtx(cb->mAngle.x, cb->mAngle.y, cb->mAngle.z, e + 0x1F0);
}
VERIFY(0x0228A174, daNpc_Md_followEcallBack_execute);

/* 0228A1B8 daNpc_Md_followEcallBack_c::setup (not named by the matcher) */
static void daNpc_Md_followEcallBack_setup(daNpc_Md_followEcallBack_c* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x0228A1B8, void, cb, emitter);
    cb->mpEmitter = emitter;
}
VERIFY(0x0228A1B8, daNpc_Md_followEcallBack_setup);

/* 0228A1C0 */
void daNpc_Md_c::emitterDelete(be<u32>* pEmitter) {
    WWHD_FUNC(0x0228A1C0, void, this, pEmitter);
    if (*pEmitter == 0) {
        return;
    }
    u32 e = *pEmitter;
    gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~0x40u); /* quitImmortalEmitter */
    JPA_becomeInvalidEmitter(*pEmitter);
    *pEmitter = 0;
}
VERIFY(0x0228A1C0, &daNpc_Md_c::emitterDelete);

/* 0228A1FC deleteHane02Emitter (not named by the matcher) */
void daNpc_Md_c::deleteHane02Emitter() {
    WWHD_FUNC(0x0228A1FC, void, this);
    emitterDelete(&m0508[2]);
    emitterDelete(&m0508[3]);
}
VERIFY(0x0228A1FC, &daNpc_Md_c::deleteHane02Emitter);

/* 0228A228 deleteHane03Emitter (not named by the matcher) */
void daNpc_Md_c::deleteHane03Emitter() {
    WWHD_FUNC(0x0228A228, void, this);
    emitterDelete(&m0508[4]);
    emitterDelete(&m0508[5]);
}
VERIFY(0x0228A228, &daNpc_Md_c::deleteHane03Emitter);

/* 0228A4E4 */
void daNpc_Md_c::changeCaught02() {
    WWHD_FUNC(0x0228A4E4, void, this);
    setAnm(0x25);
    setBitStatus(daMdStts_UNK1);
}
VERIFY(0x0228A4E4, &daNpc_Md_c::changeCaught02);

/* 0228A520 initialDefault (not named by the matcher; in the event tables 0x101C0450/0x101C0490) */
void daNpc_Md_c::initialDefault(int) {
    WWHD_FUNC(0x0228A520, void, this, 0);
}
VERIFY(0x0228A520, &daNpc_Md_c::initialDefault);

/* 0228A67C */
BOOL daNpc_Md_c::actionDefault(int) {
    WWHD_FUNC(0x0228A67C, BOOL, this, 0);
    lookBack(0, 0, 1);
    return TRUE;
}
VERIFY(0x0228A67C, &daNpc_Md_c::actionDefault);

/* 0228AB40 */
int daNpc_Md_c::getAnmType(u8 r4) {
    WWHD_FUNC(0x0228AB40, int, this, r4);
    if (r4 < 0x2C) {
        return gabi::load<s32>(0x1001DD40 + r4 * 4); /* anmTypeData_Talk */
    }
    return -1;
}
VERIFY(0x0228AB40, &daNpc_Md_c::getAnmType);

/* ---- local bindings, part 2 (SHARED-CANDIDATE) ---- */
#define MD_SAFESTRING_VTBL 0x1001D6A0 /* this TU's sead::SafeString vtable */
/* 02606900 HD: dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* md_getObjectRes(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mStringTop = gabi::ea(arc);
    a->__vtbl = MD_SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    n->__vtbl = MD_SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* strcmp, inlined by GHS (byte loop) */
static inline s32 md_strcmp(const char* s1, const char* s2) {
    u32 a = gabi::ea(s1), b = gabi::ea(s2);
    u8 c1, c2;
    do {
        c1 = gabi::load<u8>(a++);
        c2 = gabi::load<u8>(b++);
    } while (c1 == c2 && c1 != 0);
    return (s32)c1 - (s32)c2;
}
/* 025E5D54 mDoExt_McaMorf2::setAnm(bck1, bck2, blend, loopMode, morf, speed, start, end, sound) */
static inline void McaMorf2_setAnm(mDoExt_McaMorf2* m, void* a1, void* a2, f32 blend, s32 loop, f32 morf, f32 speed, f32 start, f32 end, void* snd) {
    gabi::call(0x025E5D54, m, a1, a2, blend, loop, morf, speed, start, end, snd);
}
/* 0207A9A0 cLib_calcTimer<u8> */
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* 025E72D4 mDoExt_baseAnm::initPlay(s16 frameMax, int mode, f32 speed, s16 start, s16 end, bool modify) */
static inline void mDoExt_baseAnm_initPlay(void* anm, s16 frameMax, s32 mode, f32 speed, s16 start, s16 end, bool modify) {
    gabi::call(0x025E72D4, anm, frameMax, mode, speed, start, end, modify);
}

/* 02286BC0 */
bool daNpc_Md_c::dNpc_Md_setAnm(mDoExt_McaMorf2* pMorf, f32 param_f1, int loopMode, f32 morf, f32 speed, const char* fileName1,
                                const char* filename2, const char* arcName) {
    WWHD_FUNC(0x02286BC0, bool, this, pMorf, param_f1, loopMode, morf, speed, fileName1, filename2, arcName);
    bool ret = false;
    void* bck2 = nullptr;
    if (pMorf) {
        void* bck1 = md_getObjectRes(arcName, fileName1);
        if (md_strcmp(filename2, STR(0x1001DC88) /* "" */) != 0) {
            bck2 = md_getObjectRes(arcName, filename2);
        }
        McaMorf2_setAnm(pMorf, bck1, bck2, param_f1, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
        ret = true;
    }
    return ret;
}
VERIFY(0x02286BC0, (bool (daNpc_Md_c::*)(mDoExt_McaMorf2*, f32, int, f32, f32, const char*, const char*, const char*)) & daNpc_Md_c::dNpc_Md_setAnm);

/* 02286D18 */
void daNpc_Md_c::deletePiyoPiyo() {
    WWHD_FUNC(0x02286D18, void, this);
    u32 emitter = m0508[1];
    if (emitter == 0) {
        return;
    }
    gabi::store<u32>(emitter + 0x254, gabi::load<u32>(emitter + 0x254) & ~0x40u); /* quitImmortalEmitter */
    JPA_becomeInvalidEmitter(m0508[1]);
    /* every particle of the emitter's list (first link at +0x1AC, next at +0xC): setDeleteParticleFlag */
    for (u32 link = gabi::load<u32>(emitter + 0x1AC); link != 0; link = gabi::load<u32>(link + 0xC)) {
        u32 obj = gabi::load<u32>(link);
        gabi::store<u32>(obj + 0xCC, gabi::load<u32>(obj + 0xCC) | 2);
    }
    m0508[1] = 0;
}
VERIFY(0x02286D18, &daNpc_Md_c::deletePiyoPiyo);

/* 0228729C */
void daNpc_Md_c::playTexPatternAnm() {
    WWHD_FUNC(0x0228729C, void, this);
    if (m3136 == 1) {
        /* m0520.setFrame(m30D0): an lfs/stfs copy (bit exact) */
        gabi::store<u32>(gabi::ea(m0520) + 4, gabi::load<u32>(gabi::ea(&m30D0)));
        return;
    }
    if (mDoExt_baseAnm_play(m0520)) {
        if (cLib_calcTimer_u8(&m3133) == 0) {
            m3133 = (u8)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
            mDoExt_baseAnm_initPlay(m0520, m3112, 1, 1.0f, 0, -1, true);
        }
    }
}
VERIFY(0x0228729C, &daNpc_Md_c::playTexPatternAnm);

static inline u16 md_eventCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); } /* eventInfo.mCommand */
static inline BOOL md_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
/* 0253EC0C dEvt_control_c::order(type, priority, flag, hind, actor1, actor2, eventId, infoIdx) */
static inline void dEvt_control_order(u16 type, u16 prio, u16 flag, u16 hind, fopAc_ac_c* a1, fopAc_ac_c* a2, s16 eventId, u8 infoIdx) {
    dEvt_control_c* ev = dComIfGp_getEvent();
    gabi::call(0x0253EC0C, ev, type, prio, flag, hind, a1, a2, eventId, infoIdx);
}

/* 02287DF0 */
BOOL daNpc_Md_c::checkCommandTalk() {
    WWHD_FUNC(0x02287DF0, BOOL, this);
    if (md_eventCommand(this) == 1) { /* eventInfo.checkCommandTalk() */
        if (md_event_chkTalkXY()) {
            setBitStatus(daMdStts_XY_TALK); /* onXYTalk() */
            if (!checkStatus(daMdStts_DEFAULT_TALK_XY)) {
                if (mCurEventMode == 3) {
                    mCurEventMode = 0;
                }
                return FALSE;
            }
        } else {
            clearStatus(daMdStts_XY_TALK);
        }
        return TRUE;
    } else {
        clearStatus(daMdStts_XY_TALK);
        return FALSE;
    }
}
VERIFY(0x02287DF0, &daNpc_Md_c::checkCommandTalk);

/* 022886FC */
void daNpc_Md_c::checkOrder() {
    WWHD_FUNC(0x022886FC, void, this);
    if (md_eventCommand(this) == 1 && (mCurEventMode == 1 || mCurEventMode == 2 || mCurEventMode == 3)) {
        mCurEventMode = 0;
        gabi::Local<ProcFunc_l> fn;
        if (md_event_chkTalkXY()) {
            setBitStatus(daMdStts_XY_TALK);
            if (checkStatus(daMdStts_DEFAULT_TALK_XY)) {
                md_pmf_load(fn, PMF_talkNpcAction);
                setNpcAction(fn, nullptr);
            }
        } else if (checkStatus(daMdStts_SHIP_RIDE)) { /* isShipRide() */
            md_pmf_load(fn, PMF_shipTalkNpcAction);
            setNpcAction(fn, nullptr);
        } else {
            md_pmf_load(fn, PMF_talkNpcAction);
            setNpcAction(fn, nullptr);
        }
        fopAcM_cancelCarryNow(this);
    }
}
VERIFY(0x022886FC, &daNpc_Md_c::checkOrder);

/* 022888C4 */
void daNpc_Md_c::eventOrder() {
    WWHD_FUNC(0x022888C4, void, this);
    if (mCurEventMode == 1) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (mCurEventMode == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (mCurEventMode == 2 || mCurEventMode == 3) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
    } else if (mCurEventMode >= 4) {
        switch ((u32)(s32)mCurEventMode) {
        case 4: case 5: case 6: case 7: case 8: case 9:
            mCurEvent = gabi::load<s8>(0x1001DCF0 + (s32)mCurEventMode); /* {4: 0, ..., 9: 5} */
            break;
        case 10:
            mCurEvent = 6;
            dEvt_control_order(2 /* dEvtType_OTHER_e */, 0xFF, 1 /* dEvtFlag_NOPARTNER_e */, 0xFFFF,
                               dComIfGp_getPlayer(0), this, mEventIdxTable[mCurEvent], 0xFF);
            return;
        case 11:
            mCurEvent = 7;
            break;
        case 12:
            mCurEvent = 8;
            break;
        }
        fopAcM_orderOtherEventId(this, mEventIdxTable[mCurEvent], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x022888C4, &daNpc_Md_c::eventOrder);

/* GHS pointer-to-member equality against a constant {d = 0, i = -1, f} */
static inline bool md_pmf_is(ProcFunc_l* p, u32 f) {
    s16 i = p->i;
    if (i != -1) return false;
    if (i == 0) return true;
    return p->d == 0 && p->f == f;
}
/* play-object button status bytes (GameCube dComIfGp_set*Status) */
static inline void md_setDoStatus(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB7, s); }
static inline void md_setAStatus(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB6, s); }
static inline void md_setRStatusForce(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB8, s); }
/* 0201B3C0 cXyz::normalizeZP (HD: a result slot in r4) */
static inline void cXyz_normalizeZP(cXyz* a, cXyz* out) { gabi::call(0x0201B3C0, a, out); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void md_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 f2u(f32 f) { u32 u; memcpy(&u, &f, 4); return u; }

/* 02288290 */
void daNpc_Md_c::playerAction(void* arg) {
    WWHD_FUNC(0x02288290, void, this, arg);
    if (mCurrPlayerActionFunc.i == 0) { /* mCurrPlayerActionFunc == NULL */
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, PMF_waitPlayerAction);
        setPlayerAction(fn, nullptr);
    }
    if (mAcch.ChkGroundHit() && checkStatus(daMdStts_LIGHT_BODY_HIT)) {
        md_setRStatusForce(7); /* dActStts_RETURN_e */
        if (md_pmf_is(&mCurrPlayerActionFunc, 0x02294984 /* mkamaePlayerAction */)) {
            md_setDoStatus(0);    /* dActStts_BLANK_e */
            md_setAStatus(8);     /* dActStts_PUT_AWAY_e */
        } else {
            md_setDoStatus(0x2E); /* dActStts_ba_motu__dupe_2E */
            md_setAStatus(0x3E);  /* dActStts_HIDDEN_e */
            if (!mOldLightBodyHit) {
                gabi::Local<cXyz> up;
                up->x = 0.0f;
                up->y = 1.0f;
                up->z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, up);
            }
        }
    } else {
        md_setDoStatus(0x23); /* dActStts_FLY_e */
        if (md_pmf_is(&mCurrPlayerActionFunc, 0x02294160 /* flyPlayerAction */)) {
            md_setRStatusForce(0x3E); /* dActStts_HIDDEN_e */
            md_setAStatus(6);         /* dActStts_LET_GO_e */
        } else {
            md_setRStatusForce(7);
            md_setAStatus(0x3E);
        }
    }
    md_pmf_call<void>(this, &mCurrPlayerActionFunc, arg);
}
VERIFY(0x02288290, &daNpc_Md_c::playerAction);

/* 02288544 */
BOOL daNpc_Md_c::checkCollision(int r30) {
    WWHD_FUNC(0x02288544, BOOL, this, r30);
    if ((mType == 5 || mType == 4) && mCyl1.ChkTgHit()) { /* isTypeM_Dai() || isTypeEdaichi() */
        fopAc_ac_c* hit_actor = mCyl1.GetTgHitAc();
        if (hit_actor) {
            gabi::Local<cXyz> sp3C;
            gabi::Local<cXyz> norm;
            gabi::Local<be<s16>> sp08;
            cXyz_mi(&current.pos, sp3C, &hit_actor->current.pos);
            sp3C->y = 0.0f;
            cXyz_normalizeZP(sp3C, norm);
            f32 x = sp3C->x;
            f32 z = sp3C->z;
            s16 ang;
            if (std::fabs(x) < 0.001f && std::fabs(z) < 0.001f) {
                ang = 0;
            } else {
                ang = cM_atan2s(x, z);
            }
            *sp08.get() = ang;
            gabi::Local<ProcFunc_l> fn;
            if (r30 != 0) {
                md_pmf_load(fn, PMF_hitPlayerAction);
                setPlayerAction(fn, nullptr);
            } else {
                md_pmf_load(fn, PMF_hitNpcAction);
                setNpcAction(fn, sp08.get());
            }
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02288544, &daNpc_Md_c::checkCollision);

/* 0228A524 */
void daNpc_Md_c::lookBack(int param_1, int param_2, int param_3) {
    WWHD_FUNC(0x0228A524, void, this, param_1, param_2, param_3);
    gabi::Local<cXyz> local_30;
    gabi::Local<cXyz> local_3c;
    gabi::Local<cXyz> eye;
    cXyz* dstPos = nullptr;
    /* local_3c: setall(0.0f), or current.pos with eyePos.y (lfs/stfs copies, bit exact) */
    u32 lx = f2u(0.0f), ly = f2u(0.0f), lz = f2u(0.0f);
    s16 desiredYRot = shape_angle.y;
    if (mJntCtrl.mbTrn != 0) { /* trnChk() */
        s16 target;
        if (param_2 != 0) {
            target = l_HIO().m1BC;
        } else {
            target = l_HIO().mNpc.mMaxHeadTurnVel;
        }
        cLib_addCalcAngleS2(&m3110, target, 4, 0x800);
    } else {
        m3110 = 0;
    }
    if (param_1 != 0) {
        dNpc_playerEyePos(eye, l_HIO().mNpc.m04);
        u32 e = gabi::ea(eye.get()), d = gabi::ea(local_30.get());
        gabi::store<u32>(d + 0, gabi::load<u32>(e + 0));
        gabi::store<u32>(d + 8, gabi::load<u32>(e + 8));
        gabi::store<u32>(d + 4, gabi::load<u32>(e + 4));
        lx = gabi::load<u32>(gabi::ea(&current.pos.x));
        ly = gabi::load<u32>(gabi::ea(&eyePos.y));
        lz = gabi::load<u32>(gabi::ea(&current.pos.z));
        dstPos = local_30;
    }
    u32 l = gabi::ea(local_3c.get());
    gabi::store<u32>(l + 0, lx);
    gabi::store<u32>(l + 4, ly);
    gabi::store<u32>(l + 8, lz);
    md_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, local_3c, desiredYRot, m3110, param_3 != 0);
}
VERIFY(0x0228A524, (void (daNpc_Md_c::*)(int, int, int)) & daNpc_Md_c::lookBack);

/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) */
static inline s16 fpcLf_GetPriority(void* a) { return gabi::call<s16>(0x025DF2B8, a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { gabi::call(0x025DA874, tag, prio); }
static inline void md_onDraw(fopAc_ac_c* a) {
    s16 prio = fpcLf_GetPriority(a);
    fopDwTg_ToDrawQ(gabi::ea(a) + 0xDC, prio);
}
/* 02055B64 cLib_calcTimer<s16> */
static inline s16 cLib_calcTimer_s16(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline dSv_event_c* md_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline fpc_ProcID fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D7DEC, pos, itemNo, argFlag, bitNo, roomNo, angle, scale);
}
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void md_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
#define MD_L_MSGID 0x10467A34 /* static fpc_ProcID l_msgId */
static inline u32 md_msgManager() { return gabi::load<u32>(0x101F4B5C); } /* HD message manager */

/* 0228A6AC */
void daNpc_Md_c::initialWaitEvent(int staffIdx) {
    WWHD_FUNC(0x0228A6AC, void, this, staffIdx);
    be<s32>* pDisp = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, STR(0x1001DD28) /* "Disp" */, 3);
    if (pDisp) {
        if (*pDisp) {
            md_onDraw(this);
        } else {
            fopAcM_offDraw(this);
        }
    } else {
        md_onDraw(this);
    }
    be<s32>* pTimer = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, STR(0x1001DD20) /* "Timer" */, 3);
    if (pTimer) {
        m3118 = (s16)*pTimer;
    } else {
        m3118 = 0;
    }
    speedF = 0.0f;
}
VERIFY(0x0228A6AC, &daNpc_Md_c::initialWaitEvent);

/* 0228A7D4 actionWaitEvent (not named by the matcher) */
BOOL daNpc_Md_c::actionWaitEvent(int) {
    WWHD_FUNC(0x0228A7D4, BOOL, this, 0);
    return cLib_calcTimer_s16(&m3118) == 0 ? TRUE : FALSE;
}
VERIFY(0x0228A7D4, &daNpc_Md_c::actionWaitEvent);

/* 0228A800 */
void daNpc_Md_c::initialLetterEvent(int staffIdx) {
    WWHD_FUNC(0x0228A800, void, this, staffIdx);
    u8 itemNo;
    clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
    setBitStatus(daMdStts_UNK4);
    speedF = 0.0f;
    m30F8 = 75.0f;
    if (mCurEvent == 0) {
        itemNo = 0x98;
    } else if (mCurEvent == 1) {
        itemNo = 0x25;
    } else if (mCurEvent == 2) {
        itemNo = 0x50;
    } else {
        itemNo = 0x98;
    }
    fpc_ProcID itemID = fopAcM_createItemForPresentDemo(&current.pos, itemNo, 0, -1, current.roomNo, nullptr, nullptr);
    if (itemID != fpcM_ERROR_PROCESS_ID_e) {
        gabi::store<u32>(dComIfGp_ea() + 0x52A0, itemID); /* dComIfGp_event_setItemPartnerId */
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}
VERIFY(0x0228A800, &daNpc_Md_c::initialLetterEvent);

/* 0228A928 */
void daNpc_Md_c::initialMsgSetEvent(int staffIdx) {
    WWHD_FUNC(0x0228A928, void, this, staffIdx);
    if (mMsgNo == 0x19DD) {
        dSv_event_onEventBit(md_event(), 0x1620);
        dSv_event_onEventBit(md_event(), 0x1608);
        md_setNextStage(STR(0x1001DD34) /* "sea" */, 0xE3, 0xD /* dIsleRoom_DragonRoostIsland_e */, 8, 0.0f, 0, 1, 0);
    } else {
        gabi::store<u32>(MD_L_MSGID, fpcM_ERROR_PROCESS_ID_e);
        be<s32>* pMsgNo = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, STR(0x1001DD38) /* "MsgNo" */, 3);
        if (pMsgNo != nullptr) {
            mMsgNo = *pMsgNo;
            if (mMsgNo == 0x19D6 && dSv_event_isEventBit(md_event(), 0x1504)) {
                mMsgNo = 0x19D9;
                return;
            }
        }
        /* HD: the tact message (0x5AC) sets a play-object byte */
        if (mMsgNo == 0x5AC) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BDA, 0xFF);
        }
    }
}
VERIFY(0x0228A928, &daNpc_Md_c::initialMsgSetEvent);

/* 0228AA7C */
BOOL daNpc_Md_c::talk_init() {
    WWHD_FUNC(0x0228AA7C, BOOL, this);
    /* HD: no fopMsgM_SearchByID; the message is "ready" as soon as an id is set */
    u32 msgId = gabi::load<u32>(MD_L_MSGID);
    u32 mgr = md_msgManager();
    if (msgId == fpcM_ERROR_PROCESS_ID_e) {
        if (mMsgNo == 0x5AC) {
            gabi::store<u32>(MD_L_MSGID, gabi::call<u32>(0x025F8088, mgr)); /* fopMsgM_tactMessageSet */
        } else {
            gabi::store<u32>(MD_L_MSGID, gabi::call<u32>(0x025F7DB0, mgr, (u32)mMsgNo, &eyePos)); /* fopMsgM_messageSet */
        }
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0228AA7C, &daNpc_Md_c::talk_init);

/* 0228AAF4 */
BOOL daNpc_Md_c::actionMsgSetEvent(int) {
    WWHD_FUNC(0x0228AAF4, BOOL, this, 0);
    lookBack(1, 0, 0);
    setAttention(true);
    return talk_init();
}
VERIFY(0x0228AAF4, &daNpc_Md_c::actionMsgSetEvent);

/* 02542E94 dEvent_manager_c::getRunEventName (the matcher calls it dummy1): the running event's name */
static inline u32 dEvent_getRunEventName(dEvent_manager_c* m) { return gabi::call<u32>(0x02542E94, m); }
/* sead::SafeString::cstr(): virtual at +0x14 (assure termination) */
static inline void SafeString_assure(SafeString* s) { gabi::call_ptr<void>(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* sead::SafeString operator== (inlined: same pointer, or a byte loop of at most 0x40001 chars) */
static inline bool md_safeStringEq(SafeString* a, SafeString* b) {
    SafeString_assure(a);
    SafeString_assure(a);
    u32 pa = a->mStringTop;
    SafeString_assure(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c1 = gabi::load<u8>(pa + n);
        u8 c2 = gabi::load<u8>(pb + n);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}

/* 02286D7C */
BOOL daNpc_Md_c::setAnm(int anmIdx) {
    WWHD_FUNC(0x02286D7C, BOOL, this, anmIdx);
    /* static anm_prm l_anmPrm[] at 0x101C11C4 {s8 anmTblIdx; u8 armAnmTblIdx; u8 btpAnmTblIdx; int loopMode; f32 morf; f32 speed}
     * l_anmTbl 0x101C0804, armAnmTbl 0x101C14E4, wingAnmTbl 0x101C1CE4 ({char[0x20], char[0x20]}) */
    u32 prm = 0x101C11C4 + anmIdx * 0x10;
    BOOL ret = FALSE;
    m3104 = anmIdx;
    f32 speed = gabi::load<f32>(prm + 0xC);
    f32 morf = gabi::load<f32>(prm + 0x8);
    /* HD: no morphing into the wait animation during the "ARRIVAL_GND" event */
    if (anmIdx == 0) {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->__vtbl = MD_SAFESTRING_VTBL;
        a->mStringTop = 0x1001DC8C; /* "ARRIVAL_GND" */
        u32 name = dEvent_getRunEventName(dComIfGp_getPEvtManager());
        b->__vtbl = MD_SAFESTRING_VTBL;
        b->mStringTop = name;
        if (md_safeStringEq(a, b)) {
            morf = 0.0f;
        }
    }
    if (anmIdx == 6) {
        speed = l_HIO().m118;
    } else if (anmIdx == 9 || anmIdx == 0xA) {
        speed = l_HIO().m11C;
    } else if (anmIdx == 0x2E) {
        speed = l_HIO().m124;
    } else if (anmIdx == 0x28) {
        morf = l_HIO().m068.m4;
        speed = l_HIO().m068.m8;
    } else if (anmIdx == 7 || anmIdx == 8) {
        morf = l_HIO().m1A8;
    }

    s8 anmTblIdx = gabi::load<s8>(prm + 0);
    if (anmTblIdx != m312D || gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xA4) != gabi::load<f32>(prm + 0xC)) {
        /* mpMorf->getPlaySpeed() != prm->speed */
        m312A = 0;
        m312D = anmTblIdx;
        u32 bodyAnm = 0x101C0804 + anmTblIdx * 0x40;
        dNpc_Md_setAnm(mpMorf, mRunRate, gabi::load<s32>(prm + 4), morf, speed, STR(bodyAnm), STR(bodyAnm + 0x20), mModelArcName);
        /* m30D0 = mpMorf->getFrame() (lfs/stfs copy) */
        gabi::store<u32>(gabi::ea(&m30D0), gabi::load<u32>(gabi::ea(mpMorf.get()) + 0xA8));
        u8 armIdx = gabi::load<u8>(prm + 1);
        if (armIdx < 0x80) {
            u32 armAnm = 0x101C14E4 + armIdx * 0x40;
            dNpc_Md_setAnm(mpArmMorf, mRunRate, gabi::load<s32>(prm + 4), morf, speed, STR(armAnm), STR(armAnm + 0x20), mModelArcName);
            ret = FALSE;
        } else {
            if (!isTypeShipRide()) {
                u32 wingAnm = 0x101C1CE4 + (armIdx - 0x80) * 0x40;
                dNpc_Md_setAnm((mDoExt_McaMorf*)mpWingMorf.get(), gabi::load<s32>(prm + 4), morf, speed, STR(wingAnm), mModelArcName);
            }
            ret = TRUE;
        }
        /* fopAcM_seStartCurrent */
        if (m3104 == 0x12 || m3104 == 0x1F) {
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(0x59A1 /* JA_SE_CM_MD_HARP_SET_UP */, &current.pos, 0, reverb);
        } else if (m3104 == 0x13 || m3104 == 0x21) {
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(0x59A2 /* JA_SE_CM_MD_HARP_TAKE_OFF */, &current.pos, 0, reverb);
        }
    }

    u8 btpIdx = gabi::load<u8>(prm + 2);
    if (btpIdx != m3137) {
        initTexPatternAnm(btpIdx, true);
    }
    if (m312D == 0x0E || m312D == 0x12 || m312D == 0x13 || m312D == 0x1A || m312D == 0x1C || m312D == 0x25) {
        setBitStatus(daMdStts_UNK80);
    } else if (m312D != 0x0F && m312D != 0x1B) {
        clearStatus(daMdStts_UNK80);
    }
    if (m312D == 0x1B || m312D == 0x1C) {
        setBitStatus(daMdStts_UNK4000);
    } else {
        clearStatus(daMdStts_UNK4000);
    }
    if (m312D == 0x10) {
        if (m0508[1] == 0) {
            s8 roomNo = current.roomNo;
            JPABaseEmitter* e = dComIfGp_particle_set(0x819D /* ID_IT_SN_MD_PIYOPIYO00 */, &current.pos, nullptr, nullptr, 0xFF, nullptr,
                                                      roomNo, gabi::at<GXColor>(gabi::ea(this) + 0x1A8), gabi::at<GXColor>(gabi::ea(this) + 0x1A8));
            m0508[1] = gabi::ea(e);
            if (e) {
                u32 p = gabi::ea(e) + 0x254;
                gabi::store<u32>(p, gabi::load<u32>(p) | 0x40); /* becomeImmortalEmitter */
            }
        }
    } else {
        deletePiyoPiyo();
    }
    return ret;
}
VERIFY(0x02286D7C, &daNpc_Md_c::setAnm);

/* 02008254 cBgS::ChkPolySafe(cBgS_PolyInfo&) */
static inline BOOL cBgS_ChkPolySafe(dBgS* bgs, u32 poly) { return gabi::call<BOOL>(0x02008254, bgs, poly); }
/* 025E65FC mDoExt_McaMorf2::play(cXyz* pos, u32 mtrlSndId, s8 reverb) */
static inline u8 McaMorf2_play(mDoExt_McaMorf2* m, cXyz* pos, u32 se, s32 reverb) { return gabi::call<u8>(0x025E65FC, m, pos, se, reverb); }
/* 027F2BF8 J3DFrameCtrl::checkPass (mpMorf's frame control at +0xA4) */
static inline BOOL McaMorf2_checkFrame(mDoExt_McaMorf2* m, f32 f) { return gabi::call<BOOL>(0x027F2BF8, gabi::ea(m) + 0xA4, f); }
/* 025445B8 / 0254457C dEvent_manager_c::startCheckOld / endCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(u32 name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline BOOL dComIfGp_evmng_endCheckOld(u32 name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline s32 md_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 025E1A7C mDoAud_monsSeStart(se, pos, actorId, reverb) (fopAcM_monsSeStart) */
static inline void md_monsSeStart(u32 se, cXyz* pos, u32 id, s32 reverb) { gabi::call(0x025E1A7C, se, pos, id, reverb); }
#define MD_EVENT_NAME_TBL 0x101C0284 /* event_name_tbl[10] */

/* 02287364 */
void daNpc_Md_c::animationPlay() {
    WWHD_FUNC(0x02287364, void, this);
    /* l_harp_play_se[12] at 0x1001DCAC */
    u32 mtrlSndId = 0;
    if (checkStatus(daMdStts_UNK20000) && mAcch.ChkGroundHit() &&
        cBgS_ChkPolySafe(dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8 /* m_gnd's poly info */)) {
        mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(gabi::ea(&mAcch) + 0xE8));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    m312A = McaMorf2_play(mpMorf, &eyePos, mtrlSndId, reverb);
    bool bVar3 = true;
    if (md_event_runCheck() && (dComIfGp_evmng_startCheckOld(gabi::load<u32>(MD_EVENT_NAME_TBL + 5 * 4)) ||
                                dComIfGp_evmng_endCheckOld(gabi::load<u32>(MD_EVENT_NAME_TBL + 5 * 4)))) {
        bVar3 = false;
    }
    if (mType == 3 && bVar3 && (m3104 == 0x16 || m3104 == 0x17)) { /* isTypeSea() */
        s32 anm = m3104;
        bool bVar2 = false;
        if (McaMorf2_checkFrame(mpMorf, 11.0f)) {
            bVar2 = true;
        } else if (anm == 0x16) {
            if (McaMorf2_checkFrame(mpMorf, 38.0f)) bVar2 = true;
        } else {
            if (McaMorf2_checkFrame(mpMorf, 35.0f)) bVar2 = true;
        }
        if (bVar2) {
            u32 se;
            if (m313E == 0) {
                s32 i = md_getRndValue(0, 10);
                se = gabi::load<u32>(0x1001DCAC + i * 4);
                m313E = 2;
                m30F4 = se;
            } else {
                m313E = m313E - 1;
                se = m30F4;
            }
            s32 rv = dComIfGp_getReverb(current.roomNo);
            md_monsSeStart(se, &current.pos, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID */, rv);
        }
    } else {
        m313E = 0;
    }
    if (!isTypeShipRide() && checkStatus(daMdStts_UNK1)) {
        mpWingMorf->play(nullptr, 0, 0);
    } else {
        McaMorf2_play(mpArmMorf, nullptr, 0, 0);
    }
    u32 morf = gabi::ea(mpMorf.get());
    f32 playSpeed = gabi::load<f32>(morf + 0xA4);
    if (playSpeed < 0.0f) {
        if (m30D0 < gabi::load<f32>(morf + 0xA8)) {
            m312A = 1;
        }
    } else if (m30D0 > gabi::load<f32>(morf + 0xA8)) {
        m312A = 1;
    }
    /* m30D0 = mpMorf->getFrame() (lfs/stfs copy) */
    gabi::store<u32>(gabi::ea(&m30D0), gabi::load<u32>(morf + 0xA8));
    if (m312D == 0x1B || m312D == 0xF) {
        if (m30D0 < 10.0f) {
            clearStatus(daMdStts_UNK80);
        } else {
            setBitStatus(daMdStts_UNK80);
        }
    }
    playTexPatternAnm();
    playLightBtkAnm();
}
VERIFY(0x02287364, &daNpc_Md_c::animationPlay);

/* 0252A038 dDetect_c::chk_light(const cXyz*) (dDetect_c at play+0x5A20) */
static inline BOOL md_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT), pos); }
/* 0201794C cM3d_lineVsPosSuisenCross(const cXyz& start, const cXyz& end, const cXyz& pos, cXyz* out) */
static inline void cM3d_lineVsPosSuisenCross(u32 s, u32 e, u32 p, cXyz* out) { gabi::call(0x0201794C, s, e, p, out); }
/* 023D457C daPy_mtxFollowEcallBack_c::makeEmitter(u16 id, MtxP, const cXyz* pos, const cXyz* scale) */
static inline void mtxFollow_makeEmitter(daPy_mtxFollowEcallBack_l* cb, u16 id, Mtx34* mtx, cXyz* pos, cXyz* scale) {
    gabi::call(0x023D457C, cb, id, mtx, pos, scale);
}
/* 023D4538 daPy_mtxFollowEcallBack_c::end */
static inline void mtxFollow_end(daPy_mtxFollowEcallBack_l* cb) { gabi::call(0x023D4538, cb); }
#define MD_L_MS_LIGHT_LOCAL_VEC 0x10467A54 /* static cXyz l_ms_light_local_vec */

/* 02287754 */
BOOL daNpc_Md_c::lightHitCheck() {
    WWHD_FUNC(0x02287754, BOOL, this);
    BOOL lightHit = FALSE;
    gabi::Local<cXyz> lightVec;
    PSMTXMultVecSR(J3DModel_getBaseTRMtx(mpHarpModel), gabi::at<cXyz>(MD_L_MS_LIGHT_LOCAL_VEC), lightVec);
    clearStatus(daMdStts_LIGHT_BODY_HIT);

    if (md_chk_light(&current.pos)) {
        setBitStatus(daMdStts_LIGHT_BODY_HIT);
        lightHit = TRUE;
    } else {
        void* hitObj = mCyl3.GetTgHitObj();
        if (hitObj && (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x00800000) /* ChkAtType(AT_TYPE_LIGHT) */) {
            setBitStatus(daMdStts_LIGHT_BODY_HIT);
            if (PSVECDotProduct(&mCyl3.mGObjTg.mRVec, lightVec) < 0.0f) {
                lightHit = TRUE;
            }
        }
    }

    if (lightHit) {
        setBitStatus(daMdStts_LIGHT_HIT);
        if (mCps.ChkAtHit()) {
            fopAc_ac_c* hitActor = mCps.GetAtHitAc();
            if ((actor_status & 0x2000) /* fopAcM_checkCarryNow */ && !checkStatus(daMdStts_CARRY_ACTION)) {
                if (hitActor != dComIfGp_getLinkPlayer() && m3058.mpEmitter.get() == nullptr) {
                    dComIfGp_particle_set(0x8232 /* ID_AK_SN_HITSHIELDLIGHT00 */, &current.pos, nullptr, nullptr, 0xFF,
                                          (dPa_levelEcallBack*)(void*)&m3058);
                }
            } else if (m3058.mpEmitter.get() == nullptr) {
                dComIfGp_particle_set(0x8232, &current.pos, nullptr, nullptr, 0xFF, (dPa_levelEcallBack*)(void*)&m3058);
            }
            u32 cps = gabi::ea(&mCps);
            cM3d_lineVsPosSuisenCross(cps + 0x118, cps + 0x124, gabi::ea(&mCps.mGObjAt.mHitPos), &m3058.mPos);
            /* lightVec.absXZ(): sqrt(PSVECSquareMag({x, 0, z})) */
            gabi::Local<cXyz> xz;
            u32 lv = gabi::ea(lightVec.get());
            gabi::store<u32>(gabi::ea(xz.get()) + 0, gabi::load<u32>(lv + 0));
            xz->y = 0.0f;
            gabi::store<u32>(gabi::ea(xz.get()) + 8, gabi::load<u32>(lv + 8));
            f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
            s16 angleX = cM_atan2s(-lightVec->y, absXZ);
            s16 angleY = cM_atan2s(lightVec->x, lightVec->z);
            m3058.mAngle.y = angleY;
            m3058.mAngle.x = angleX;
            m3058.mAngle.z = 0;
        } else {
            m3058.end();
        }
        /* fopAcM_seStartCurrent */
        u32 se;
        if (!(mCps.mObjAt.mSPrm & 1)) { /* !ChkAtSet() */
            se = 0x6961; /* JA_SE_OBJ_MIRROR_REFLECT */
        } else {
            se = 0x7028; /* JA_SE_OBJ_MIRROR_LIGHT */
        }
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(se, &current.pos, 0, reverb);

        if (m304C.mpEmitter.get() == nullptr) {
            mtxFollow_makeEmitter(&m304C, 0x8226 /* ID_AK_SN_MIRRORSHIELD00 */, J3DModel_getBaseTRMtx(mpHarpLightModel), &current.pos, nullptr);
            u32 emitter = gabi::ea(m304C.mpEmitter.get());
            gabi::store<f32>(emitter + 0x08, 1.0f); /* setEmitterScale(1, 1, 1) */
            gabi::store<f32>(emitter + 0x0C, 1.0f);
            gabi::store<f32>(emitter + 0x10, 1.0f);
            gabi::store<f32>(emitter + 0x14, 0.0f); /* setEmitterTranslation(0, 4, 0) */
            gabi::store<f32>(emitter + 0x18, 4.0f);
            gabi::store<f32>(emitter + 0x1C, 0.0f);
        }
    } else {
        clearStatus(daMdStts_LIGHT_HIT);
        m3058.end();
        if (m304C.mpEmitter.get() != nullptr) {
            gabi::store<u8>(gabi::ea(m304C.mpEmitter.get()) + 0x247, 0); /* setGlobalAlpha(0) */
            mtxFollow_end(&m304C);
        }
    }
    return lightHit;
}
VERIFY(0x02287754, &daNpc_Md_c::lightHitCheck);

static inline void md_mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 02018808 cM3dGCps::SetStartEnd / cM3dGLin::SetStartEnd(const cXyz&, const cXyz&) */
static inline void cM3dGLin_SetStartEnd(u32 lin, cXyz* s, cXyz* e) { gabi::call(0x02018808, lin, s, e); }
static inline void md_dCcMassS_Set(void* obj, u8 p) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, p); }
/* integer cXyz copy (GHS struct copy) */
static inline void md_xyz_copy(cXyz* dst, const cXyz* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    u32 x = gabi::load<u32>(s), y = gabi::load<u32>(s + 4), z = gabi::load<u32>(s + 8);
    gabi::store<u32>(d, x);
    gabi::store<u32>(d + 4, y);
    gabi::store<u32>(d + 8, z);
}
#define MD_L_MS_LIGHT_LOCAL_START 0x10467A60 /* static cXyz l_ms_light_local_start */

/* 02287A74 */
void daNpc_Md_c::setCollision() {
    WWHD_FUNC(0x02287A74, void, this);
    gabi::Local<cXyz> local_44;
    gabi::Local<cXyz> local_50;
    gabi::Local<cXyz> local_2c;
    gabi::Local<cXyz> local_38;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> tmp2;
    md_xyz_copy(local_2c, &current.pos);
    f32 radius = 30.0f;
    if (!checkStatus(daMdStts_SHIP_RIDE) && !(actor_status & 0x2000) /* fopAcM_checkCarryNow */) {
        mCyl1.SetC(local_2c);
        mCyl1.SetR(radius);
        mCyl1.SetH(m30F8);
        dComIfG_Ccsp_Set(&mCyl1);
    } else {
        gabi::call(0x0251641C, &mCyl1); /* ClrCoHit */
        mCyl1.ClrTgHit();
    }
    mCyl2.SetC(local_2c);
    mCyl2.SetR(radius);
    mCyl2.SetH(m30F8);
    dComIfG_Ccsp_Set(&mCyl2);
    mCyl3.SetC(local_2c);
    mCyl3.SetR(radius);
    mCyl3.SetH(m30F8);
    dComIfG_Ccsp_Set(&mCyl3);
    if (checkStatus(daMdStts_LIGHT_HIT)) {
        PSMTXCopy(J3DModel_getBaseTRMtx(mpHarpModel), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(l_HIO().m034.m0C, l_HIO().m034.m10, l_HIO().m034.m14);
        md_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), l_HIO().m034.m18, l_HIO().m034.m1A, l_HIO().m034.m1C);
        PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(MD_L_MS_LIGHT_LOCAL_START), local_44);
        PSMTXMultVecSR(mDoMtx_stack_c::get(), gabi::at<cXyz>(MD_L_MS_LIGHT_LOCAL_VEC), local_38);
        cXyz_pl(local_44, tmp, local_38);
        md_xyz_copy(local_50, tmp);
        dBgS_LinChk_Set(mLinChk, local_44, local_50, this);
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            md_xyz_copy(local_50, gabi::at<cXyz>(gabi::ea(this) + 0x7D4)); /* mLinChk.GetCross() */
            cXyz_mi(local_50, tmp2, local_44);
            md_xyz_copy(local_38, tmp2);
        }
        cM3dGLin_SetStartEnd(gabi::ea(&mCps) + 0x118, local_44, local_50); /* mCps.SetStartEnd */
        md_xyz_copy(&mCps.mGObjAt.mVec, local_38);                       /* mCps.SetAtVec */
        mCps.mObjAt.mSPrm = mCps.mObjAt.mSPrm | 1;                       /* OnAtSetBit */
        dComIfG_Ccsp_Set(&mCps);
        md_dCcMassS_Set(&mCps, 1);
        actor_status &= ~0x100u; /* fopAcM_OffStatus(this, fopAcStts_CULL_e) */
    } else {
        gabi::call(0x02516138, &mCps); /* ResetAtHit */
        mCps.mObjAt.mSPrm = mCps.mObjAt.mSPrm & ~1u; /* OffAtSetBit */
        actor_status |= 0x100;
    }
}
VERIFY(0x02287A74, &daNpc_Md_c::setCollision);

/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, const char** tbl, int n, BOOL force, int nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, s32 force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* dComIfGp_event_setTalkPartner: dEvt_control_c (play+0x51D0) +0xCC = getPId(actor) (0253F124) */
static inline void md_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = gabi::ea(dComIfGp_getEvent());
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
#define MD_L_STAFF_NAME 0x101C0280   /* static const char* l_staff_name ("Md1") */
#define MD_CUT_NAME_TBL 0x101C0594   /* cut_name_tbl[0x16] */
#define MD_EVENT_INIT_TBL 0x101C0434 /* event_init_tbl[]: pointers to member (8 bytes) */
#define MD_EVENT_ACTION_TBL 0x101C04E4

/* 02287EFC */
BOOL daNpc_Md_c::eventProc() {
    WWHD_FUNC(0x02287EFC, BOOL, this);
    if (eventInfo_checkCommandDemoAccrpt(this) && mCurEventMode != 0) {
        if (mCurEventMode == 0xC) {
            if (dComIfGp_evmng_startCheckOld(0x1001DCE0 /* "OPTION_CHAR_END" */) || dComIfGp_evmng_endCheckOld(0x1001DCE0)) {
                md_event_setTalkPartner(dComIfGp_getLinkPlayer());
                gabi::call(0x025E1988, 0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
            } else {
                m4E4 = m4E4 & ~1u; /* offReturnLink() */
                mCurEventMode = 0;
            }
        }
        if (mCurEventMode != 0) {
            m4E4 = m4E4 | 2; /* onEventAccept() */
            mCurEventMode = 0;
        }
    }
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(gabi::load<u32>(MD_L_STAFF_NAME)), nullptr, 0);
    if (md_event_runCheck() && !checkCommandTalk()) {
        if (staffIdx != -1) {
            s32 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, MD_CUT_NAME_TBL, 0x16, TRUE, 0);
            if (actIdx == -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            } else {
                if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                    md_pmf_call<void>(this, gabi::at<ProcFunc_l>(MD_EVENT_INIT_TBL + actIdx * 8), staffIdx);
                }
                if (md_pmf_call<BOOL>(this, gabi::at<ProcFunc_l>(MD_EVENT_ACTION_TBL + actIdx * 8), staffIdx)) {
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
            }
        }
        if (m4E4 & 2) { /* isEventAccept() */
            if (dComIfGp_evmng_endCheck(mEventIdxTable[mCurEvent])) {
                dComIfGp_event_reset();
                m4E4 = m4E4 & ~2u; /* offEventAccept() */
                if (mCurEvent == 8) {
                    returnLinkPlayer();
                    m4E4 = m4E4 & ~1u; /* offReturnLink() */
                }
                mCurEvent = -1;
            }
            return TRUE;
        }
        if (staffIdx != -1) {
            return TRUE;
        } else if (gabi::load<u16>(gabi::ea(dComIfGp_getLinkPlayer()) + 0xF8) != 3) { /* !checkCommandDoor() */
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02287EFC, &daNpc_Md_c::eventProc);

#define MD_INSTANCE 0x101CEF74 /* HD: a static pointer to the Medli actor, cleared by her destructor */
/* 0228A254 */
static void daNpc_Md_dt(daNpc_Md_c* i_this, s32 flags) {
    WWHD_FUNC(0x0228A254, void, i_this, flags);
    if (i_this == nullptr) {
        return;
    }
    u32 t = gabi::ea(i_this);
    i_this->__vtbl = MD_VTBL;
    dComIfG_resDelete(&i_this->mPhase, i_this->mModelArcName);
    if (i_this->heap.get() != nullptr) {
        gabi::call(0x025E6794, i_this->mpMorf.get()); /* mDoExt_McaMorf2::stopZelAnime */
    }
    i_this->deletePiyoPiyo();
    i_this->emitterDelete(&i_this->m0508[0]);
    i_this->deleteHane02Emitter();
    i_this->deleteHane03Emitter();
    i_this->m3058.end();
    mtxFollow_end(&i_this->m304C);
    gabi::call(0x025A9270, &i_this->m3074); /* dPa_rippleEcallBack::end (GameCube remove()) */
    /* HD: no l_HIO child to delete; a static instance pointer is cleared instead */
    gabi::store<u8>(MD_M_FLYING, 0);     /* offFlying() */
    gabi::store<u8>(MD_M_MIRROR, 0);     /* offMirror() */
    gabi::store<u8>(MD_M_PLAYERROOM, 0); /* offPlayerRoom() */
    if (gabi::load<u32>(MD_INSTANCE) == t) {
        gabi::store<u32>(MD_INSTANCE, 0);
    }

    /* member destructors (inlined), last member first */
    /* m0B70 (dDlst_mirrorPacket, HD 0x3428): its dBgS_LinChk at +0x33BC (0x4104) */
    gabi::store<u32>(t + 0x415C, 0x1001D788);
    gabi::store<u32>(t + 0x4168, 0x1001D708);
    gabi::store<u32>(t + 0x4124, 0x1001D6F8);
    gabi::call(0x02008B4C, t + 0x4104, 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::call(0x027FB528, t + 0x3D9C, 0);
    gabi::call(0x027FB528, t + 0x3CF4, 0);
    gabi::call(0x027FD764, t + 0x3CE8, 2);
    if (t + 0x3828 != 0) {
        /* two 0x254-byte elements at 0x3828 */
        gabi::call(0x027BF7E8, t + 0x3980);
        gabi::store<u32>(t + 0x3828, 0);
        if (gabi::load<u32>(t + 0x3A78) != 0) {
            u32 heap = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), gabi::load<u32>(t + 0x3A78));
            gabi::call_ptr<void>(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C), heap, gabi::load<u32>(t + 0x3A78));
            gabi::store<u32>(t + 0x3A74, 0);
            gabi::store<u32>(t + 0x3A78, 0);
        }
        gabi::call(0x027BF7E8, t + 0x3BD4);
        gabi::store<u32>(t + 0x3A7C, 0);
        if (gabi::load<u32>(t + 0x3CCC) != 0) {
            u32 heap = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), gabi::load<u32>(t + 0x3CCC));
            gabi::call_ptr<void>(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C), heap, gabi::load<u32>(t + 0x3CCC));
            gabi::store<u32>(t + 0x3CC8, 0);
            gabi::store<u32>(t + 0x3CCC, 0);
        }
        gabi::store<u32>(t + 0x3CE0, 0);
        gabi::call(0x028F0164, t + 0x3828, 2, 0x254, 0x02295490, 0, 0); /* __destroy_arr */
    }
    gabi::call(0x027BE2B0, t + 0x368C, 2);
    gabi::call(0x027BE2B0, t + 0x34F4, 2);
    gabi::call(0x027BE2B0, t + 0x335C, 2);
    gabi::call(0x027F13DC, t + 0xD48, 0); /* J3DPacket::~J3DPacket */
    gabi::call(0x02515980, &i_this->mCps, 2);  /* ~dCcD_Cps */
    gabi::call(0x02515A70, &i_this->mCyl3, 2); /* ~dCcD_Cyl (the matcher calls it dBgS_Acch::~dBgS_Acch) */
    gabi::call(0x02515A70, &i_this->mCyl2, 2);
    gabi::call(0x02515A70, &i_this->mCyl1, 2);
    gabi::call(0x02515860, &i_this->mStts, 2); /* ~dCcD_Stts */
    /* mLinChk (dBgS_MirLightLinChk) */
    gabi::store<u32>(t + 0x7FC, 0x1001D788);
    gabi::store<u32>(t + 0x808, 0x1001D708);
    gabi::store<u32>(t + 0x7C4, 0x1001D6F8);
    gabi::call(0x02008B4C, t + 0x7A4, 0);
    gabi::call(0x028F0164, &i_this->mAcchCir, 2, 0x40, 0x0229543C, 0, 0); /* __destroy_arr(mAcchCir) */
    gabi::call(0x0244513C, i_this, 0); /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}
VERIFY(0x0228A254, daNpc_Md_dt);

/* 025E7B3C mDoExt_btpAnm::entry(J3DModelData*, s16 frame) */
static inline void btpAnm_entry(void* anm, J3DModelData* d, s16 frame) { gabi::call(0x025E7B3C, anm, d, frame); }
/* 025E66D4 mDoExt_McaMorf2::entryDL */
static inline void McaMorf2_entryDL(mDoExt_McaMorf2* m) { gabi::call(0x025E66D4, m); }
/* 02445A00 daPy_npc_c::drawDamageFog */
static inline void daPy_npc_drawDamageFog(void* a) { gabi::call(0x02445A00, a); }
/* 028E9108 PSMTXConcat(a, b, ab) */
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* 0252DA64 dDlst_mirrorPacket::update(Mtx, u8 alpha, f32) */
static inline void mirrorPacket_update(u32 pkt, Mtx34* m, u8 alpha, f32 f) { gabi::call(0x0252DA64, pkt, m, alpha, f); }
/* 027F0E04 J3DDrawBuffer::entryImm(J3DPacket*, u16); the XLU list is at play+0x5D7C */
static inline void md_xluList_entryImm(u32 pkt, u16 idx) { gabi::call(0x027F0E04, gabi::load<u32>(dComIfGp_ea() + 0x5D7C), pkt, idx); }

/* 02289E2C */
BOOL daNpc_Md_c::draw() {
    WWHD_FUNC(0x02289E2C, BOOL, this);
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
        if (ship != 0 && (gabi::load<u32>(ship + 0x644) & 0x200000) /* checkHeadNoDraw() */) {
            return TRUE;
        }
    } else {
        if (home.roomNo < 0) { /* fopAcM_GetHomeRoomNo */
            return TRUE;
        }
        /* dComIfGp_roomControl_checkStatusFlag(current.roomNo, 0x10): room status table at 0x1047E8E8 (0x22C each) */
        s8 roomNo = current.roomNo;
        dComIfGp_get();
        if (!(gabi::load<u8>(0x1047E8E8 + roomNo * 0x22C) & 0x10)) {
            return TRUE;
        }
    }
    J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(mpMorf.get()) + 0x90)); /* getModel() */
    J3DModelData* modelData = J3DModel_getModelData(model);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    daPy_npc_drawDamageFog(this);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    btpAnm_entry(m0520, modelData, (s16)gabi::ftoi(gabi::load<f32>(gabi::ea(m0520) + 4)));
    McaMorf2_entryDL(mpMorf);
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* m0520.remove(modelData) */

    if (!isTypeShipRide() && checkStatus(daMdStts_UNK1)) {
        J3DModel* limbModel = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(mpWingMorf.get()) + 0x90));
        setLightTevColorType(dKy_getEnvlight(), limbModel, &tevStr);
        mpWingMorf->entryDL();
    } else {
        J3DModel* limbModel = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(mpArmMorf.get()) + 0x90));
        setLightTevColorType(dKy_getEnvlight(), limbModel, &tevStr);
        McaMorf2_entryDL(mpArmMorf);
    }
    setLightTevColorType(dKy_getEnvlight(), mpHarpModel, &tevStr);
    mDoExt_modelUpdateDL(mpHarpModel, 0);

    if (checkStatus(daMdStts_LIGHT_HIT)) {
        mLightBtkAnm.entry(J3DModel_getModelData(mpHarpLightModel), mLightBtkAnm.mFrameCtrl.mFrame);
        mDoExt_modelUpdateDL(mpHarpLightModel, 0);
        gabi::store<u32>(gabi::ea(J3DModel_getModelData(mpHarpLightModel)) + 0x44, 0); /* mLightBtkAnm.remove() */

        gabi::Local<Mtx34> mtx;
        PSMTXCopy(J3DModel_getBaseTRMtx(mpHarpModel), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(l_HIO().m034.m0C, l_HIO().m034.m10, l_HIO().m034.m14);
        PSMTXCopy(mDoMtx_stack_c::get(), mtx);
        cXyz* start = gabi::at<cXyz>(MD_L_MS_LIGHT_LOCAL_START);
        PSMTXTrans(mDoMtx_stack_c::get(), start->x, start->y, start->z); /* transS */
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -0x8000);
        f32 len = std_sqrtf(PSVECSquareMag(&mCps.mGObjAt.mVec)); /* GetAtVecP()->abs() */
        /* HD: scale (1, 1, len / 960) (GameCube 0.1, 0.1, len / 9600) */
        mDoMtx_stack_c::scaleM(1.0f, 1.0f, len * (1.0f / 960.0f));
        PSMTXConcat(mtx, mDoMtx_stack_c::get(), mDoMtx_stack_c::get()); /* revConcat */
        mirrorPacket_update(gabi::ea(m0B70), mDoMtx_stack_c::get(), 0xFF, 90.0f);
        md_xluList_entryImm(gabi::ea(m0B70), 0x1F);
    }
    /* HD: no real shadow (GameCube dComIfGd_setShadow + addRealShadow) */
    dSnap_RegistFig(0x8A /* DSNAP_TYPE_NPC_MD */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02289E2C, &daNpc_Md_c::draw);

/* ---- execute ---- */
static inline BOOL daPy_npc_check_initialRoom(void* a) { return gabi::call<BOOL>(0x024451B4, a); }
static inline BOOL daPy_npc_initialRestartOption(void* a, s8 p, BOOL b) { return gabi::call<BOOL>(0x0244586C, a, p, b); }
static inline BOOL daPy_npc_check_moveStop(void* a) { return gabi::call<BOOL>(0x024452A0, a); }
static inline BOOL daPy_npc_checkNowPosMove(void* a, u32 name) { return gabi::call<BOOL>(0x02445950, a, name); }
static inline s8 daPy_npc_chkMoveBlock(void* a, cXyz* p) { return gabi::call<s8>(0x02445AA4, a, p); }
static inline void daPy_py_objWindHitCheck(void* a, dCcD_Cyl* c) { gabi::call(0x023D4890, a, c); }
static inline BOOL dBgS_ChkMoveBG(dBgS* bgs, u32 poly) { return gabi::call<BOOL>(0x024EEABC, bgs, poly); }
static inline void dBgS_MoveBgCrrPos(dBgS* bgs, u32 poly, bool b, cXyz* pos, csXyz* a1, csXyz* a2) { gabi::call(0x024EF968, bgs, poly, b, pos, a1, a2); }
static inline s8 md_GetRoomId(dBgS* bgs, u32 poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 md_GetPolyColor(dBgS* bgs, u32 poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline s32 md_GetGroundCode(dBgS* bgs, u32 poly) { return gabi::call<s32>(0x024EF0BC, bgs, poly); }
/* 025A8BFC dPa_control_c::setSimpleLand(poly, pos, angle, f32, f32, f32, tevstr, int* out, int) */
static inline void md_setSimpleLand(u32 poly, cXyz* pos, csXyz* ang, f32 a, f32 b, f32 c, dKy_tevstr_c* tev, s32* out, s32 n) {
    dPa_control_c* pa = dComIfGp_getParticle();
    gabi::call(0x025A8BFC, pa, poly, pos, ang, a, b, c, tev, out, n);
}
/* 023FD4E4 daPy_lk_c::startRestartRoom(u32 mode, int eventInfoIdx, f32, int) */
static inline void daPy_lk_startRestartRoom(fopAc_ac_c* lk, u32 mode, s32 idx, f32 f, s32 p) { gabi::call(0x023FD4E4, lk, mode, idx, f, p); }
static inline void md_setStatusMap(fopAc_ac_c* a, u32 v) { a->actor_status = (a->actor_status & ~0x3Fu) | v; }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* md_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
/* the room status flag 0x10 (dComIfGp_roomControl_checkStatusFlag): table at 0x1047E8E8, 0x22C each */
static inline bool md_roomStatus10(s8 roomNo) {
    dComIfGp_get();
    return (gabi::load<u8>(0x1047E8E8 + roomNo * 0x22C) & 0x10) != 0;
}
/* the stage name (play+0x5134) as a SafeString, compared with a literal */
static inline bool md_stageNameIs(u32 lit) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = MD_SAFESTRING_VTBL;
    a->mStringTop = lit;
    u32 stage = dComIfGp_ea() + 0x5134;
    b->mStringTop = stage;
    b->__vtbl = MD_SAFESTRING_VTBL;
    return md_safeStringEq(a, b);
}
/* the ground polygon: GetTriPla normal, room, colour, mPolyInfo */
static inline void md_setGroundInfo(daNpc_Md_c* t) {
    u32 p = gabi::ea(t);
    u32 gnd = p + 0x524; /* mAcch.m_gnd's cBgS_PolyInfo */
    u32 pla = gabi::ea(cBgS_GetTriPla(dComIfG_Bgsp(), gabi::load<u16>(gnd + 2), gabi::load<u16>(gnd)));
    if (pla != 0) {
        md_xyz_copy(&t->m32A4, gabi::at<cXyz>(pla)); /* *triPla->GetNP() */
    }
    s8 roomNo = md_GetRoomId(dComIfG_Bgsp(), gnd);
    t->current.roomNo = roomNo;
    gabi::store<s8>(p + 0x1C9, roomNo); /* tevStr.mRoomNo */
    u8 color = md_GetPolyColor(dComIfG_Bgsp(), gnd);
    gabi::store<u8>(p + 0x1CA, color);  /* tevStr.mEnvrIdxOverride */
    t->mStts.mRoomId = roomNo;          /* mStts.SetRoomId */
    /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
    t->mPolyInfo.mPolyIndex = gabi::load<u16>(gnd);
    t->mPolyInfo.mBgIndex = gabi::load<u16>(gnd + 2);
    t->mPolyInfo.mpBgW = gabi::load<u32>(gnd + 4);
    t->mPolyInfo.mProcId = gabi::load<s32>(gnd + 8);
}

/* 02288B08 */
BOOL daNpc_Md_c::execute() {
    WWHD_FUNC(0x02288B08, BOOL, this);
    clearStatus(daMdStts_UNK20000);
    mOldLightBodyHit = checkStatus(daMdStts_LIGHT_BODY_HIT) ? 1 : 0; /* setOldLightBodyHit() */
    actor_status &= ~0x20u;                                         /* fopAcM_OffStatus(SHOWMAP) */
    checkPlayerRoom();
    cLib_calcTimer_u8(&mDamageFogTimer); /* executeDamageFog() */
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        daNpc_Md_HIO_l& h = l_HIO();
        mJntCtrl.setParam(h.m074.m10, h.m074.m12, h.m074.m14, h.m074.m16, h.m074.m08, h.m074.m0A, h.m074.m0C, h.m074.m0E,
                          h.mNpc.mMaxTurnStep);
    } else if (mType != 6) { /* !isTypeM_DaiB() */
        if (!daPy_npc_check_initialRoom(this)) {
            if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this)) { /* dComIfGp_getCb1Player() */
                gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0);
            }
            return TRUE;
        }
        daPy_npc_initialRestartOption(this, 2, mType == 5);
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this) &&
            (!dSv_event_isEventBit(md_event(), 0x1620) || mType == 7 || mType == 6)) {
            gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0);
        }
        fopAcM_setStageLayer(this);
        if (daPy_npc_check_moveStop(this)) {
            if (!isTypeShipRide()) {
                gabi::Local<ProcFunc_l> fn;
                md_pmf_load(fn, PMF_waitNpcAction);
                setNpcAction(fn, nullptr);
                setAnm(0x30);
                animationPlay();
                setBaseMtx();
            }
            m312B = 0;
            return TRUE;
        }
        u32 poly = gabi::ea(&mPolyInfo);
        if (m3131 != 0 && cBgS_ChkPolySafe(dComIfG_Bgsp(), poly) && dBgS_ChkMoveBG(dComIfG_Bgsp(), poly)) {
            dBgS_MoveBgCrrPos(dComIfG_Bgsp(), poly, true, &old.pos, nullptr, nullptr);
        }
        daNpc_Md_HIO_l& h = l_HIO();
        mJntCtrl.setParam(h.mNpc.mMaxBackboneX, h.mNpc.mMaxBackboneY, h.mNpc.mMinBackboneX, h.mNpc.mMinBackboneY, h.mNpc.mMaxHeadX,
                          h.mNpc.mMaxHeadY, h.mNpc.mMinHeadX, h.mNpc.mMinHeadY, h.mNpc.mMaxTurnStep);
    }
    if (mType == 6) { /* isTypeM_DaiB() */
        mAcch.CrrPos(dComIfG_Bgsp());
        setBitStatus(daMdStts_UNK20000);
        if (mAcch.GetGroundH() != -1000000000.0f /* -G_CM3D_F_INF */) {
            md_setGroundInfo(this);
        }
    } else if (!checkStatus(daMdStts_SHIP_RIDE) && !(m4E4 & 1) /* isReturnLink() */ && !(actor_status & 0x2000)) {
        if (daPy_npc_checkNowPosMove(this, gabi::load<u32>(MD_L_STAFF_NAME))) {
            f32 fVar1 = maxFallSpeed;
            f32 fVar2 = speed.y;
            if (fVar1 < fVar2) {
                speed.y = fVar2 - gravity;
                if (speed.y < maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            } else if (fVar1 > fVar2) {
                speed.y = fVar2 + gravity;
                if (speed.y > maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            }
            speed.x = speedF * cM_ssin(current.angle.y);
            speed.z = speedF * cM_scos(current.angle.y);
            fopAcM_posMove(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (checkStatus(daMdStts_UNK4)) {
            daPy_py_objWindHitCheck(this, &mCyl2);
        }
        mAcch.CrrPos(dComIfG_Bgsp());
        setBitStatus(daMdStts_UNK20000);
        bool bVar10 = false;
        /* mAcch.m_wtr.GetHeight() (+0x1BC) */
        if (mAcch.ChkWaterIn() && gabi::load<f32>(gabi::ea(&mAcch) + 0x1BC) > current.pos.y + 5.0f) {
            if (m3074.mpEmitter.get() == nullptr) {
                /* dComIfGp_particle_setShipTail (HD: group 5) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33 /* ID_AK_JN_HAMON00 */, &current.pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)(void*)&m3074, -1, nullptr, nullptr, nullptr);
            }
            m3074.mRate = 0.0f;
            bVar10 = true;
        }
        if (!bVar10 && m3074.mpEmitter.get() != nullptr) {
            gabi::call(0x025A9270, &m3074); /* HD: dPa_rippleEcallBack::end (GameCube remove()) */
        }
        if (!mAcch.ChkGroundHit()) {
            if (md_pmf_is(&mCurrPlayerActionFunc, 0x02293B5C /* walkPlayerAction */) ||
                md_pmf_is(&mCurrNpcActionFunc, 0x02292C48 /* searchNpcAction */) ||
                md_pmf_is(&mCurrNpcActionFunc, 0x0228EB88 /* waitNpcAction */)) {
                f32 gndY = mAcch.GetGroundH();
                f32 delta = gndY - current.pos.y;
                if (delta < 0.001f && delta >= -30.1f && speed.y < 0.001f) {
                    speed.y = 0.0f;
                    current.pos.y = gndY;
                    mAcch.m_flags = mAcch.m_flags | dBgS_Acch::GROUND_HIT; /* SetGroundHit() */
                }
            }
        }
        if (mAcch.GetGroundH() != -1000000000.0f) {
            md_setGroundInfo(this);
        }
        if (mAcch.ChkGroundHit() && m3131 == 0) {
            gabi::Local<be<s32>> iStack_88;
            md_setSimpleLand(gabi::ea(&mAcch) + 0xE8, &current.pos, &shape_angle, 1.25f, 1.5f, 1.0f, &tevStr, (s32*)(void*)iStack_88.get(), 7);
        }
        m3131 = mAcch.ChkGroundHit();
        if (mAcch.GetGroundH() == -1000000000.0f || md_GetGroundCode(dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xE8) == 4) {
            /* HD: the unnamed action 0229310C falls too */
            if (isFallAction() || md_pmf_is(&mCurrNpcActionFunc, 0x0229310C)) {
                if (m4E8 < 30) {
                    m4E8 = m4E8 + 1;
                } else if (md_pmf_is(&mCurrNpcActionFunc, 0x0229310C) || !md_roomStatus10(home.roomNo)) {
                    /* current = home (struct copy), shape_angle = home.angle */
                    u32 p = gabi::ea(this);
                    gabi::store<u32>(p + 0x314, gabi::load<u32>(p + 0x2EC));
                    gabi::store<u32>(p + 0x324, gabi::load<u32>(p + 0x2FC));
                    gabi::store<u16>(p + 0x32A, gabi::load<u16>(p + 0x2FA));
                    gabi::store<u32>(p + 0x31C, gabi::load<u32>(p + 0x2F4));
                    gabi::store<u32>(p + 0x318, gabi::load<u32>(p + 0x2F0));
                    gabi::store<u16>(p + 0x328, gabi::load<u16>(p + 0x2F8));
                    gabi::store<u32>(p + 0x320, gabi::load<u32>(p + 0x2F8));
                    gabi::store<u16>(p + 0x32C, gabi::load<u16>(p + 0x2FC));
                    m4E8 = 0;
                    speedF = 0.0f;
                    /* HD: back to the wait action */
                    gabi::Local<ProcFunc_l> fn;
                    md_pmf_load(fn, PMF_waitNpcAction);
                    setNpcAction(fn, nullptr);
                } else {
                    /* daPy_getPlayerLinkActorClass()->npcStartRestartRoom() */
                    daPy_lk_startRestartRoom(dComIfGp_getLinkPlayer(), 5, 0xC9, -1.0f, 0);
                }
            } else {
                m4E8 = 0;
            }
        } else {
            m4E8 = 0;
        }
    }
    lightHitCheck();
    setCollision();
    /* HD: also animated on the Dragon Roost spring stage (M_Dra09) */
    if (daPy_npc_checkNowPosMove(this, gabi::load<u32>(MD_L_STAFF_NAME)) || md_stageNameIs(0x1001DD10 /* "M_Dra09" */)) {
        /* HD: back to the wait animation after the tact animation there (event bit 0x1140) */
        if (m3104 == 0x22 && dSv_event_isEventBit(md_event(), 0x1140) && md_stageNameIs(0x1001DD10)) {
            setAnm(0);
        }
        animationPlay();
    }
    if (!eventProc()) {
        if (dComIfGp_getPlayer(0) == this) {
            md_setStatusMap(this, 0x31); /* GameCube 0x11 */
            if (m4E4 & 1) { /* isReturnLink() */
                mCurEventMode = 0xC;
            } else {
                if (checkStatus(daMdStts_CARRY_ACTION)) { /* isNoCarryAction() */
                    gabi::Local<ProcFunc_l> fn;
                    md_pmf_load(fn, PMF_carryPlayerAction);
                    setPlayerAction(fn, nullptr);
                    m4E4 = m4E4 | 1; /* returnLink() */
                }
                playerAction(nullptr);
                if (returnLinkCheck()) {
                    m4E4 = m4E4 | 1;
                } else {
                    checkCollision(1);
                }
            }
        } else {
            m313F = daPy_npc_chkMoveBlock(this, &m30C4);
            if (m313F != 0) {
                gabi::Local<ProcFunc_l> fn;
                md_pmf_load(fn, PMF_escapeNpcAction);
                setNpcAction(fn, nullptr);
            }
            u32 map = 0x28; /* GameCube 0x8 */
            if (dSv_event_isEventBit(md_event(), 0x1620)) {
                map = 0x2C; /* GameCube 0xC */
            }
            md_setStatusMap(this, map);
            carryCheck();
            checkOrder();
            npcAction(nullptr);
            checkCollision(0);
            if (!checkStatus(daMdStts_UNK2)) {
                if (checkStatus(daMdStts_UNK1)) {
                    s16 target = shape_angle.y - current.angle.y;
                    if (target < 0x6000 && target > -0x6000) {
                        if (target > 0x2000) {
                            target = 0x2000;
                        } else if (target < -0x2000) {
                            target = -0x2000;
                        }
                        cLib_addCalcAngleS(&shape_angle.z, target, 8, 0x2000, 0x400);
                        cLib_addCalcAngleS(&shape_angle.x, 0, 8, 0x2000, 0x400);
                    } else {
                        cLib_addCalcAngleS(&shape_angle.z, 0, 8, 0x2000, 0x400);
                        cLib_addCalcAngleS(&shape_angle.x, -0x2000, 8, 0x2000, 0x400);
                    }
                } else {
                    current.angle.y = shape_angle.y;
                }
            }
        }
    }
    eventOrder();
    setBaseMtx();
    u32 model = gabi::load<u32>(gabi::ea(mpMorf.get()) + 0x90); /* getModel() */
    emitterTrace(gabi::at<JPABaseEmitter>(m0508[0]), md_getAnmMtx(model, m_neck_jnt_num), &current.angle);
    emitterTrace(gabi::at<JPABaseEmitter>(m0508[1]), md_getAnmMtx(model, m_neck_jnt_num), nullptr);
    if (!isTypeShipRide()) {
        model = gabi::load<u32>(gabi::ea(mpWingMorf.get()) + 0x90);
        emitterTrace(gabi::at<JPABaseEmitter>(m0508[2]), md_getAnmMtx(model, m_wingR3_jnt_num), nullptr);
        emitterTrace(gabi::at<JPABaseEmitter>(m0508[3]), md_getAnmMtx(model, m_wingL3_jnt_num), nullptr);
        emitterTrace(gabi::at<JPABaseEmitter>(m0508[4]), md_getAnmMtx(model, m_wingR3_jnt_num), nullptr);
        emitterTrace(gabi::at<JPABaseEmitter>(m0508[5]), md_getAnmMtx(model, m_wingL3_jnt_num), nullptr);
        if (m3135 & 1) { /* checkBitEffectStatus(1) */
            model = gabi::load<u32>(gabi::ea(mpWingMorf.get()) + 0x90);
            gabi::Local<cXyz> local_3c;
            u32 l = gabi::ea(local_3c.get());
            for (int k = 0; k < 2; k++) {
                Mtx34* m = md_getAnmMtx(model, k == 0 ? m_wingR2_jnt_num : m_wingL2_jnt_num);
                s8 roomNo = current.roomNo;
                u32 me = gabi::ea(m);
                /* local_3c.set(mtx[0][3], mtx[1][3], mtx[2][3]) (through FPRs: signalling NaNs are quieted here) */
                f32 z = gabi::load<f32>(me + 0x2C), x = gabi::load<f32>(me + 0x0C), y = gabi::load<f32>(me + 0x1C);
                gabi::store<f32>(l + 0, x);
                gabi::store<f32>(l + 4, y);
                gabi::store<f32>(l + 8, z);
                dComIfGp_particle_set(0x819C /* ID_IT_SN_MD_HANE01 */, local_3c, nullptr, nullptr, 0xFF, nullptr, roomNo,
                                      gabi::at<GXColor>(gabi::ea(this) + 0x1A8), gabi::at<GXColor>(gabi::ea(this) + 0x1A8), nullptr);
            }
        }
    }
    m3135 = 0; /* setEffectStatus(0) */
    return TRUE;
}
VERIFY(0x02288B08, &daNpc_Md_c::execute);
