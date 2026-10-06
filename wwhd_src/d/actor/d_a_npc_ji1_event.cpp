/**
 * d_a_npc_ji1_event.cpp (WWHD)
 * NPC - Orca: event functions (eventAction, privateCut and the evn_* cuts), initPos, createItem,
 * particles.
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* dComIfGp_event_setItemPartnerId: play + 0x52A0 */
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
/* mini games (HD): play + 0x5CE8 flags (u16), +0x5CEA type (u8), +0x5CEE (u8) */
static inline void dComIfGp_endMiniGame(u16 bit) {
    u32 p = dComIfGp_ea();
    u16 f = gabi::load<u16>(p + 0x5CE8);
    gabi::store<u8>(p + 0x5CEE, 0);
    gabi::store<u8>(p + 0x5CEA, 0);
    gabi::store<u16>(p + 0x5CE8, (u16)(f ^ bit));
}
static inline void dComIfGp_startMiniGame(u8 type, u16 bit) {
    u32 p = dComIfGp_ea();
    gabi::store<u8>(p + 0x5CEA, type);
    gabi::store<u16>(p + 0x5CE8, (u16)(gabi::load<u16>(p + 0x5CE8) | bit));
}
/* HD messages: the message manager at *(0x101F4B5C) replaces the GameCube msg_class* */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
/* 025F7DB0 fopMsgM_messageSet (HD: manager, msgNo, pos) -> process id */
static inline u32 fopMsgM_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
static inline void fopMsgM_demoMsgFlagOn() { gabi::call(0x025DB58C); }
/* 025F795C (matcher: fopMsgM_SearchByID): the message status, GameCube l_msg->mStatus */
static inline u32 msg_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
/* 025F74D0: sets the message status (reads r4 only) */
static inline void msg_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10467228); }
/* audio */
static inline void mDoAud_bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* items */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, argFlag, bitNo, roomNo, angle, scale);
}
/* 0255F554 dKy_SordFlush_set(cXyz pos (by value: pointer to a copy), int) */
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { gabi::call(0x0255F554, pos, p); }
/* dNpc_HeadAnm_c */
static inline void swing_horizone_init(void* h, s32 a, s32 b, s32 c, s32 d) { gabi::call(0x0259F514, h, a, b, c, d); }
static inline void swing_vertical_init(void* h, s32 a, s32 b, s32 c, s32 d) { gabi::call(0x0259F36C, h, a, b, c, d); }
/* cSAngle::Val(const cSAngle&) / Val(s16) */
static inline void cSAngle_Val(u32 a, u32 src) { gabi::call(0x02006638, a, src); }
static inline void cSAngle_Val_s(u32 a, s16 v) { gabi::call(0x02006584, a, v); }
static inline void PSMTXIdentity(u32 m) { gabi::call(0x028E9098, m); }
/* JPABaseEmitter setters (HD offsets) */
static inline void emitter_setMaxFrame(u32 e, s32 v) { gabi::store<s32>(e + 0x5C, v); }
static inline void emitter_setRate(u32 e, f32 v) { gabi::store<f32>(e + 0x34, v); }
static inline void emitter_setSpread(u32 e, f32 v) { gabi::store<f32>(e + 0x58, v); }
static inline void emitter_setRandomDirectionSpeed(u32 e, f32 v) { gabi::store<f32>(e + 0x74, v); }
static inline void emitter_setVolumeSweep(u32 e, f32 v) { gabi::store<f32>(e + 0x7C, v); }

enum { fpcNm_TSUBO_e = 0x1C5 };
enum { JA_SE_CV_JI_FUTTOBI = 0x4932, JA_SE_CM_JI_DAMAGE = 0x5824, JA_SE_VS_JI_ENDING = 0x85C };

/* setAction(p, arg) with a PTMF read from memory (eventAction: setAction(field_0x2C8, 0)) */
static inline void ji1_setActionP(daNpc_Ji1_c* t, ProcFunc_l& src, void* arg) {
    s16 ad = src.d;
    s16 ai = src.i;
    u32 af = src.f;
    s16 mi = t->mAction.i;
    bool eq;
    if (mi == ai) {
        eq = mi == 0 || (t->mAction.d == ad && t->mAction.f == af);
    } else {
        eq = false;
    }
    if (!eq) {
        if (mi != 0) {
            t->field_0xC78 = -1;
            ptmf_invoke(t->mAction, t, arg);
        }
        ptmf_copy(t->field_0x2BC, t->mAction);
        t->mAction.d = ad;
        t->mAction.i = ai;
        t->mAction.f = af;
        t->field_0xC78 = 0;
        ptmf_invoke(t->mAction, t, arg);
    }
}

/* 0224FBAC */
u32 daNpc_Ji1_c::setParticle(int max, f32 rate, f32 spread) {
    WWHD_FUNC(0x0224FBAC, u32, this, max, rate, spread);
    dtParticle();
    if (mSmokeCb.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(this);
        /* dComIfGp_particle_setToon(ID_AK_JT_ELEMENTSMOKE00, &current.pos, 0, 0, 0xB9, &mSmokeCb, roomNo) */
        JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &current.pos, nullptr, nullptr, 0xB9,
                                                  (dPa_levelEcallBack*)&mSmokeCb, roomNo, nullptr, nullptr, nullptr);
        if (emitter) {
            u32 e = gabi::ea(emitter);
            emitter_setRate(e, rate);
            emitter_setSpread(e, spread);
            emitter_setRandomDirectionSpeed(e, 1.5f);
            emitter_setMaxFrame(e, max);
            return true;
        }
    }
    return false;
}
VERIFY(0x0224FBAC, &daNpc_Ji1_c::setParticle);

/* 0225B720 */
u32 daNpc_Ji1_c::setParticleAT(int max, f32 rate, f32 spread) {
    WWHD_FUNC(0x0225B720, u32, this, max, rate, spread);
    if (mSmokeCbAT.mpEmitter.get() == nullptr) {
        s8 roomNo = fopAcM_GetRoomNo(this);
        JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &field_0x320, nullptr, nullptr, 0xB9,
                                                  (dPa_levelEcallBack*)&mSmokeCbAT, roomNo, nullptr, nullptr, nullptr);
        if (mSmokeCbAT.mpEmitter.get()) {
            u32 e = gabi::ea(emitter);
            /* setGlobalParticleScale(2, 2, 2) */
            gabi::store<f32>(e + 0x238, 2.0f);
            gabi::store<f32>(e + 0x23C, 2.0f);
            gabi::store<f32>(e + 0x240, 2.0f);
            emitter_setVolumeSweep(e, 0.15f);
            emitter_setRate(e, rate);
            emitter_setSpread(e, spread);
            emitter_setRandomDirectionSpeed(e, 1.5f);
            emitter_setMaxFrame(e, max);
            return true;
        }
    }
    return false;
}
VERIFY(0x0225B720, &daNpc_Ji1_c::setParticleAT);

/* 02255358 */
void daNpc_Ji1_c::setHitParticle(cXyz* param_1, u32 param_2) {
    WWHD_FUNC(0x02255358, void, this, param_1, param_2);
    cXyz* pRVec = &field_0x7E0.mGObjTg.mRVec;
    /* angle and scale adjacent as in the original frame (without `callee 025A847C r7=6` the
     * harness compares 12 bytes behind the csXyz* argument) */
    struct AngScale {
        csXyz angle;
        u8 _6[2];
        cXyz scale;
    };
    gabi::Local<AngScale> loc;
    cXyz* scale = &loc->scale;
    csXyz* angle = &loc->angle;
    scale->set(0.75f, 0.75f, 0.75f);
    angle->y = cM_atan2s(pRVec->x, pRVec->z);
    angle->x = cM_atan2s(pRVec->z, pRVec->y);
    if (param_1) {
        scale->copy(*param_1);
    }
    dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, field_0x7E0.GetTgHitPosP(), angle, scale);
    dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, field_0x7E0.GetTgHitPosP(), nullptr, scale);
    ji1_seStart(this, JA_SE_CM_JI_DAMAGE, 0);
    ji1_seStart(this, param_2, 0);
    gabi::Local<cXyz> pos;
    cXyz* hit = field_0x7E0.GetTgHitPosP();
    pos->z = (f32)hit->z;
    pos->y = (f32)hit->y;
    pos->x = (f32)hit->x;
    dKy_SordFlush_set(pos, 0);
}
VERIFY(0x02255358, &daNpc_Ji1_c::setHitParticle);

/* 022513D8 */
s32 daNpc_Ji1_c::getEventActionNo(int staffIdx) {
    WWHD_FUNC(0x022513D8, s32, this, staffIdx);
    /* static char* ActionNames[7] = {"00_dummy", ...} at 0x101BEBA4 */
    return dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BEBA4, 7, FALSE, 0);
}
VERIFY(0x022513D8, &daNpc_Ji1_c::getEventActionNo);

/* 022542A0 */
void* daNpc_Ji1_c::initPosObject(void* pActor, void* pData) {
    WWHD_FUNC(0x022542A0, void*, pActor, pData);
    if (fopAc_IsActor(pActor) && pActor != nullptr && fpcM_GetName(pActor) == fpcNm_TSUBO_e) {
        /* daTsubo::Act_c::pos_init() (inline; m678 at 0x794) */
        fopAc_ac_c* a = (fopAc_ac_c*)pActor;
        u32 b = gabi::ea(a);
        if (gabi::load<s32>(b + 0x794) == 2) {
            a->current.pos.set(a->home.pos.x, a->home.pos.y, a->home.pos.z);
            a->current.angle.x = (s16)a->home.angle.x;
            a->current.angle.y = (s16)a->home.angle.y;
            a->current.angle.z = (s16)a->home.angle.z;
            a->shape_angle.x = (s16)a->home.angle.x;
            a->shape_angle.y = (s16)a->home.angle.y;
            a->shape_angle.z = (s16)a->home.angle.z;
            u32 zero = 0x101FF354; /* cSAngle::_0 */
            cSAngle_Val(b + 0x7A4, zero);
            cSAngle_Val(b + 0x7A6, zero);
            cSAngle_Val(b + 0x7A8, zero);
            cSAngle_Val(b + 0x7AA, zero);
            cSAngle_Val(b + 0x7AC, zero);
            cSAngle_Val_s(b + 0x7AE, a->current.angle.y);
            PSMTXIdentity(b + 0x7D8);
        }
    }
    return 0;
}
VERIFY(0x022542A0, &daNpc_Ji1_c::initPosObject);

/* 02254390 */
void daNpc_Ji1_c::initPos(int param_1) {
    WWHD_FUNC(0x02254390, void, this, param_1);
    if (param_1 >= 0) {
        setAnm(param_1, 0.0f, 0);
    }
    fopAcIt_Judge(0x022542A0 /* initPosObject */, this); /* fopAcM_Search */
    old.pos.copy(field_0xD28);
    current.pos.copy(field_0xD28);
    gabi::Local<csXyz> a;
    csXyz* r = gabi::call<csXyz*>(0x0201A478, a.get(), (s16)home.angle.y, (s16)0, (s16)0); /* csXyz::csXyz returns this */
    current.angle.x = (s16)r->x;
    current.angle.y = (s16)r->y;
    current.angle.z = (s16)r->z;
}
VERIFY(0x02254390, &daNpc_Ji1_c::initPos);

/* 02258574 HD: the item's room is the current room (GameCube demo: -1) */
void daNpc_Ji1_c::createItem() {
    WWHD_FUNC(0x02258574, void, this);
    u8 itemNo;
    if (field_0xD7C) {
        itemNo = 0x38; /* dItemNo_SWORD_e */
    } else if (field_0xD7B == 1) {
        itemNo = 0xAA; /* dItemNo_HURRICANE_SPIN_e */
    } else if (dComIfGs_getEventReg(0xD003) == 1) {
        itemNo = 5; /* dItemNo_PURPLE_RUPEE_e */
    } else if (dComIfGs_getEventReg(0xD003) == 2) {
        itemNo = 6; /* dItemNo_ORANGE_RUPEE_e */
    } else if (field_0xD70 >= l_HIO().field_0x60[3] && dComIfGs_isEventBit(0x0F10)) {
        itemNo = 0xF; /* dItemNo_SILVER_RUPEE_e */
    } else {
        itemNo = 7; /* dItemNo_HEART_PIECE_e */
        dComIfGs_onEventBit(0x0F10);
    }
    u32 itemPID = fopAcM_createItemForPresentDemo(&current.pos, itemNo, 0, -1, current.roomNo, nullptr, nullptr);
    if (itemPID != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(itemPID);
    }
    mCreateItemNo = itemNo;
}
VERIFY(0x02258574, &daNpc_Ji1_c::createItem);

/* 022596CC */
BOOL daNpc_Ji1_c::eventAction(void* arg) {
    WWHD_FUNC(0x022596CC, BOOL, this, arg);
    if (field_0xC78 == 0) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
        if (!eventInfo_checkCommandDemoAccrpt(this) && staffIdx == -1) {
            fopAcM_orderOtherEventId(this, mEventIdx[field_0xC84], 0xFF, 0xFFFF, 0, 1);
            eventInfo_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
            return false;
        }
        field_0xC78 += 1;
    } else if (field_0xC78 == -1) {
        field_0xC84 = 0x12;
    } else {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
        privateCut();
        AnimeControlToWait();
        if (dComIfGp_evmng_endCheck(mEventIdx[field_0xC84])) {
            dComIfGp_event_reset();
            ji1_setActionP(this, field_0x2C8, nullptr);
        } else if (staffIdx == -1) {
            ji1_setActionP(this, field_0x2C8, nullptr);
        }
    }
    return true;
}
VERIFY(0x022596CC, &daNpc_Ji1_c::eventAction);

/* 02258508 */
u32 daNpc_Ji1_c::evn_init_pos_init(int staffIdx) {
    WWHD_FUNC(0x02258508, u32, this, staffIdx);
    void* data = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2E0) /* "AnmNo" */);
    u32 value = 0;
    if (data) {
        value = gabi::load<s32>(gabi::ea(data));
    }
    initPos(value);
    return 1;
}
VERIFY(0x02258508, &daNpc_Ji1_c::evn_init_pos_init);

/* 022582E4 */
u32 daNpc_Ji1_c::evn_setAnm_init(int staffIdx) {
    WWHD_FUNC(0x022582E4, u32, this, staffIdx);
    void* iData = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2BC) /* "AnmNo" */);
    void* fData = dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001B2C4) /* "hokan" */);
    if (iData) {
        int iVal = gabi::load<s32>(gabi::ea(iData));
        f32 fVal = 0.0f;
        if (fData) {
            fVal = gabi::load<f32>(gabi::ea(fData));
        }
        if (field_0xD84 != 1 && iVal == 5) {
            iVal = 1;
        }
        setAnm(iVal, fVal, 0);
    }
    return 1;
}
VERIFY(0x022582E4, &daNpc_Ji1_c::evn_setAnm_init);

/* 022583A4 HD: no l_msg pointer (the message manager replaces it) */
u32 daNpc_Ji1_c::evn_talk_init(int staffIdx) {
    WWHD_FUNC(0x022583A4, u32, this, staffIdx);
    void* pMsgNo = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2CC) /* "MsgNo" */);
    void* pEndMsgNo = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2D4) /* "EndMsgNo" */);
    l_msgId() = fpcM_ERROR_PROCESS_ID_e;
    if (pMsgNo) {
        u32 no = gabi::load<u32>(gabi::ea(pMsgNo));
        mMsgNo = no;
        if (no == 0x9A4) {
            if (field_0xC38 == 2) {
                mMsgNo = 0x9A9;
            } else if (mCreateItemNo == 7) {
                mMsgNo = 0x9A6;
            }
        } else if (no == 0x9A1) {
            if (field_0xC38 == 2) {
                mMsgNo = 0x9A7;
            }
        } else if (no == 0x9B7) {
            if (ptmf_eq(field_0x2C8, ACT_kaitenwaitAction)) {
                mMsgNo = 0x95E;
            } else if (ptmf_eq(field_0x2C8, ACT_teachAction)) {
                mMsgNo = 0x95D;
            }
        }
    } else {
        mMsgNo = 0;
    }
    if (pEndMsgNo) {
        mEndMsgNo = gabi::load<u32>(gabi::ea(pEndMsgNo));
    } else {
        mEndMsgNo = 0;
    }
    return 1;
}
VERIFY(0x022583A4, &daNpc_Ji1_c::evn_talk_init);

/* The two talk cuts are the same code. privateCut calls 02258CB0 for TALKMSG and 02259178 for
 * CONTINUETALK, so 02258CB0 is evn_talk and 02259178 evn_continue_talk (the matcher has them
 * swapped). HD: the message manager replaces l_msg; a failed messageSet skips demoMsgFlagOn. */
static inline u32 ji1_talk_proc(daNpc_Ji1_c* t) {
    u32 mgr = msgMgr();
    if (l_msgId() == fpcM_ERROR_PROCESS_ID_e) {
        u32 id = fopMsgM_messageSet(mgr, t->mMsgNo, &t->eyePos);
        l_msgId() = id;
        if (id != fpcM_ERROR_PROCESS_ID_e) {
            fopMsgM_demoMsgFlagOn();
        }
        return false;
    }
    t->setAnimFromMsgNo(t->mMsgNo);
    if (msg_getStatus(mgr) == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        /* next_msgStatus's r3 goes to the status setter as is (no u16 re-extension) */
        msg_setStatus(mgr, gabi::call<u32>(0x0224DA74, t, &t->mMsgNo));
        if (msg_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
            fopMsgM_messageSet(mgr, t->mMsgNo, nullptr);
        } else if (msg_getStatus(mgr) == 0x10 /* fopMsgStts_MSG_ENDS_e */) {
            if (t->mMsgNo == 0x9BA) {
                t->field_0xD7B = 0;
                t->field_0xD6C = 0;
            } else if (t->mMsgNo == 0x95A) {
                t->field_0xD7C = 0;
                t->field_0xD6C = 0;
            }
        }
        return false;
    }
    if (msg_getStatus(mgr) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        msg_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
        l_msgId() = fpcM_ERROR_PROCESS_ID_e;
        return true;
    }
    if ((msg_getStatus(mgr) == 2 /* BOX_OPENING */ || msg_getStatus(mgr) == 6 /* MSG_TYPING */) && t->mMsgNo == t->mEndMsgNo) {
        t->mEndMsgNo = 0;
        return true;
    }
    return false;
}

/* 02258CB0 (matcher: evn_continue_talk) */
u32 daNpc_Ji1_c::evn_talk() {
    WWHD_FUNC(0x02258CB0, u32, this);
    return ji1_talk_proc(this);
}
VERIFY(0x02258CB0, &daNpc_Ji1_c::evn_talk);

/* 02259178 (matcher: evn_talk) */
u32 daNpc_Ji1_c::evn_continue_talk() {
    WWHD_FUNC(0x02259178, u32, this);
    return ji1_talk_proc(this);
}
VERIFY(0x02259178, &daNpc_Ji1_c::evn_continue_talk);

/* 02258BE8 */
u32 daNpc_Ji1_c::evn_continue_talk_init(int staffIdx) {
    WWHD_FUNC(0x02258BE8, u32, this, staffIdx);
    void* data = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B300) /* "EndMsgNo" */);
    if (data) {
        mEndMsgNo = gabi::load<u32>(gabi::ea(data));
    } else {
        mEndMsgNo = 0;
    }
    return 1;
}
VERIFY(0x02258BE8, &daNpc_Ji1_c::evn_continue_talk_init);

/* 02258C4C */
u32 daNpc_Ji1_c::evn_setAngle_init(int staffIdx) {
    WWHD_FUNC(0x02258C4C, u32, this, staffIdx);
    void* data = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B30C) /* "angle" */);
    if (data) {
        current.angle.y = (s16)gabi::load<s32>(gabi::ea(data));
    }
    return 1;
}
VERIFY(0x02258C4C, &daNpc_Ji1_c::evn_setAngle_init);

/* 02258794 */
u32 daNpc_Ji1_c::evn_sound_proc_init(int staffIdx) {
    WWHD_FUNC(0x02258794, u32, this, staffIdx);
    void* data = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2E8) /* "prm" */);
    if (data) {
        switch (gabi::load<u32>(gabi::ea(data))) {
        case 0:
            mDoAud_bgmStop(45);
            break;
        case 1:
            mDoAud_bgmStart(0x80000018 /* JA_BGM_HOUSE_G */);
            break;
        case 2:
            ji1_seStart(this, JA_SE_VS_JI_ENDING, 0);
            break;
        case 3: {
            gabi::Local<cXyz> v;
            v->set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(5, -0x11, v);
            break;
        }
        }
    }
    return 1;
}
VERIFY(0x02258794, &daNpc_Ji1_c::evn_sound_proc_init);

/* 02258894 */
u32 daNpc_Ji1_c::evn_head_swing_init(int staffIdx) {
    WWHD_FUNC(0x02258894, u32, this, staffIdx);
    void* pData = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2EC) /* "prm" */);
    u32 value;
    if (!pData) {
        value = 0;
    } else {
        value = gabi::load<u32>(gabi::ea(pData));
    }
    switch (value) {
    case 0:
        swing_horizone_init(mHeadAnm, 2, 0x1000, 0x1000, 1);
        break;
    case 1:
        swing_vertical_init(mHeadAnm, 2, 0x1000, 0x800, 1);
        break;
    case 2:
        swing_vertical_init(mHeadAnm, 1, 0x1400, 0x1000, 1);
        break;
    }
    return true;
}
VERIFY(0x02258894, &daNpc_Ji1_c::evn_head_swing_init);

/* 0225896C */
u32 daNpc_Ji1_c::evn_harpoon_proc_init(int staffIdx) {
    WWHD_FUNC(0x0225896C, u32, this, staffIdx);
    void* pData = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2F0) /* "prm" */);
    u32 value;
    if (!pData) {
        value = 0;
    } else {
        value = gabi::load<u32>(gabi::ea(pData));
    }
    switch (value) {
    case 0:
        harpoonRelease(nullptr);
        break;
    case 1:
        field_0xD84 = 0;
        break;
    case 2:
        field_0xD84 = 1;
        break;
    }
    return true;
}
VERIFY(0x0225896C, &daNpc_Ji1_c::evn_harpoon_proc_init);

/* 02258A14 */
u32 daNpc_Ji1_c::evn_RollAtControl_init(int staffIdx) {
    WWHD_FUNC(0x02258A14, u32, this, staffIdx);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    field_0xD18 = 400.0f;
    s16 a = cLib_targetAngleY(&current.pos, &player->current.pos);
    field_0xD16 = (s16)(a + 0x2000);
    field_0xC30 = 30;
    field_0xD68 = 0;
    return 1;
}
VERIFY(0x02258A14, &daNpc_Ji1_c::evn_RollAtControl_init);

/* 02258E6C */
u32 daNpc_Ji1_c::evn_RollAtControl() {
    WWHD_FUNC(0x02258E6C, u32, this);
    /* static cXyz hit_scale(1.25f, 1.25f, 1.25f): guard 0x104673C0, object 0x10467264 */
    cXyz* hit_scale = gabi::at<cXyz>(0x10467264);
    if (gabi::load<u32>(0x104673C0) == 0) {
        gabi::store<u32>(0x104673C0, 1);
        hit_scale->x = 1.25f;
        hit_scale->z = 1.25f;
        hit_scale->y = 1.25f;
    }
    field_0xD16 = (s16)(field_0xD16 + 0x540);
    cLib_chaseF(&field_0xD18, 0.0f, 1.0f);
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    f32 x = current.pos.x;
    field_0xD1C.z = z;
    field_0xD1C.x = x;
    field_0xD1C.y = y;
    f32 d18 = field_0xD18;
    field_0xD1C.x = gabi::fmadds(d18, cM_ssin(field_0xD16), x);
    field_0xD1C.z = gabi::fmadds(d18, cM_scos(field_0xD16), z);
    dComIfGp_evmng_setGoal(&field_0xD1C);
    field_0x6B0.SetC(&current.pos);
    dComIfG_Ccsp_Set(&field_0x6B0);
    field_0x7E0.SetC(&current.pos);
    dComIfG_Ccsp_Set(&field_0x7E0);
    if (field_0xC30 == 0xA) {
        setAnm(0x19, 0.0f, 0);
        BackSlideInit();
        setHitParticle(hit_scale, JA_SE_CV_JI_FUTTOBI);
    }
    if (mAnimation == 0x19) {
        if (mpOrcaMorf->getFrame() < 10.0f) {
            fopAc_ac_c* player = daPy_getPlayerActorClass();
            gabi::Local<cXyz> posDiff;
            cXyz_mi(&player->current.pos, posDiff, &current.pos);
            cLib_addCalcAngleS2(&current.angle.y, cM_atan2s(posDiff->x, posDiff->z), 2, 0x2000);
        } else {
            if (mpOrcaMorf->checkFrame(20.0f) && field_0xD68++ < 2) {
                setHitParticle(hit_scale, JA_SE_CV_JI_FUTTOBI);
                BackSlideInit();
                setAnm(0x19, 4.0f, 1);
            }
        }
        BackSlide(120.0f, 10.0f);
    }
    if (field_0xC30 > 0) {
        field_0xC30 -= 1;
    } else {
        return true;
    }
    return false;
}
VERIFY(0x02258E6C, &daNpc_Ji1_c::evn_RollAtControl);

/* 02258A78 */
u32 daNpc_Ji1_c::evn_game_mode_init(int staffIdx) {
    WWHD_FUNC(0x02258A78, u32, this, staffIdx);
    void* data = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2F8) /* "prm" */);
    u32 value;
    if (data) {
        value = gabi::load<u32>(gabi::ea(data));
    } else {
        value = 0;
    }
    switch (value) {
    case 0:
        dComIfGp_endMiniGame(1 << 5); /* dComIfGp_endMiniGame(6) */
        break;
    case 1:
        dComIfGp_startMiniGame(6, 1 << 5);
        game_life_point() = 3;
        /* fallthrough */
    case 2:
        dComIfGp_endMiniGame(1 << 1); /* dComIfGp_endMiniGame(2) */
        break;
    }
    return 1;
}
VERIFY(0x02258A78, &daNpc_Ji1_c::evn_game_mode_init);

/* 0225911C */
u32 daNpc_Ji1_c::evn_turn_to_player() {
    WWHD_FUNC(0x0225911C, u32, this);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    s16 target = cLib_targetAngleY(&current.pos, &player->current.pos);
    if (cLib_addCalcAngleS(&current.angle.y, target, 4, 0x1000, 0x100) == 0) {
        return 1;
    } else {
        return 0;
    }
}
VERIFY(0x0225911C, &daNpc_Ji1_c::evn_turn_to_player);

/* 02258B48 */
u32 daNpc_Ji1_c::evn_hide_init(int staffIdx) {
    WWHD_FUNC(0x02258B48, u32, this, staffIdx);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    void* pData = dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001B2FC) /* "prm" */);
    u32 value;
    if (!pData) {
        value = 0;
    } else {
        value = gabi::load<u32>(gabi::ea(pData));
    }
    mHide = value & 1;
    /* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: mModeFlg (+0x3B8) bit 0x08000000 */
    be<u32>& flg = *gabi::at<be<u32>>(gabi::ea(player) + 0x3B8);
    if (value & 2) {
        flg |= 0x08000000u;
    } else {
        flg &= ~0x08000000u;
    }
    return true;
}
VERIFY(0x02258B48, &daNpc_Ji1_c::evn_hide_init);

/* 022595EC */
void daNpc_Ji1_c::AnimeControlToWait() {
    WWHD_FUNC(0x022595EC, void, this);
    mDoExt_McaMorf* morf = mpOrcaMorf;
    u8 play_mode = morf->getPlayMode();
    if (play_mode == J3DFrameCtrl::EMode_NONE) {
        if (morf->checkFrame(morf->getEndFrame() - 1.0f)) {
            if (isAttackAnim() || isGuardAnim()) {
                setAnm(5, 4.0f, 0);
            } else if (isItemWaitAnim()) {
                setAnm(0, 4.0f, 0);
            }
        }
    }
}
VERIFY(0x022595EC, &daNpc_Ji1_c::AnimeControlToWait);

/* 02259334 (TALKMSG -> 02258CB0, CONTINUETALK -> 02259178: see evn_talk) */
u32 daNpc_Ji1_c::privateCut() {
    WWHD_FUNC(0x02259334, u32, this);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(mEventCut.mpEvtStaffName), nullptr, 0);
    if (staffIdx == -1) {
        return false;
    }
    enum {
        ACT_SETANM, ACT_TALKMSG, ACT_INITPOS, ACT_CREATEITEM, ACT_SOUND, ACT_HEADSWING, ACT_HARPOON,
        ACT_ROLLAT_CNT, ACT_GAME_MODE, ACT_TURN_TO_PLAYER, ACT_HIDE, ACT_CONTINUETALK, ACT_SETANGLE,
    };
    /* static char* cut_name_tbl[13] at 0x101BEBC0 */
    s32 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BEBC0, 13, TRUE, 0);
    dEvent_manager_c* mgr = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        gabi::call(0x02543280, mgr, staffIdx); /* cutEnd */
    } else {
        if (gabi::call<BOOL>(0x025447C8, mgr, staffIdx) /* getIsAddvance */) {
            switch (actIdx) {
            case ACT_SETANM: evn_setAnm_init(staffIdx); break;
            case ACT_TALKMSG: evn_talk_init(staffIdx); break;
            case ACT_INITPOS: evn_init_pos_init(staffIdx); break;
            case ACT_CREATEITEM: createItem(); break;
            case ACT_SOUND: evn_sound_proc_init(staffIdx); break;
            case ACT_HEADSWING: evn_head_swing_init(staffIdx); break;
            case ACT_HARPOON: evn_harpoon_proc_init(staffIdx); break;
            case ACT_ROLLAT_CNT: evn_RollAtControl_init(staffIdx); break;
            case ACT_GAME_MODE: evn_game_mode_init(staffIdx); break;
            case ACT_HIDE: evn_hide_init(staffIdx); break;
            case ACT_CONTINUETALK: evn_continue_talk_init(staffIdx); break;
            case ACT_SETANGLE: evn_setAngle_init(staffIdx); break;
            }
        }
        BOOL end;
        switch (actIdx) {
        case ACT_TALKMSG: end = evn_talk(); break;
        case ACT_ROLLAT_CNT: end = evn_RollAtControl(); break;
        case ACT_TURN_TO_PLAYER: end = evn_turn_to_player(); break;
        case ACT_CONTINUETALK: end = evn_continue_talk(); break;
        default: end = true; break;
        }
        if (end) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    return true;
}
VERIFY(0x02259334, &daNpc_Ji1_c::privateCut);
