/**
 * d_a_npc_md_exec.cpp (WWHD)
 * Player - Medli: animation, collision, action dispatch, execute/draw, first event actions
 * WWHD execution and effect updates.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md_exec.h"
using namespace MdExec;
static RoomTable roomStatus;
static NamePointer l_staff_name;
static EventNames cut_name_tbl;
static EventActions event_init_tbl;
static EventActions event_action_tbl;

static u8 m_playerRoom;
static u8 m_mirror;
static u8 m_flying;
static u32 l_msgId;
static SavePointer saveInfo;
static MessagePointer messageManager;
static ActorPointer instance;
static HeapPointer rootHeap;
static anmTypeData_TalkTable anmTypeData_Talk;
static eventModesTable eventModes;
static l_anmPrmTable l_anmPrm;
static l_anmTblTable l_anmTbl;
static armAnmTblTable armAnmTbl;
static wingAnmTblTable wingAnmTbl;
static event_name_tblTable event_name_tbl;
static l_harp_play_seTable l_harp_play_se;
static cXyz l_ms_light_local_start;
static cXyz l_ms_light_local_vec;
static ActionWords action_mwaitNpcAction;
static ActionWords action_waitNpcAction;
static ActionWords action_waitPlayerAction;
static ActionWords action_hitPlayerAction;
static ActionWords action_hitNpcAction;
static ActionWords action_carryNpcAction;
static ActionWords action_talkNpcAction;
static ActionWords action_shipTalkNpcAction;
static ActionWords action_carryPlayerAction;
static ActionWords action_escapeNpcAction;

/* Inlined helpers. */
static inline bool dNpc_setAnmFNDirect(mDoExt_McaMorf* m, int loop, f32 morf, f32 speed, const char* fn, const char* snd, const char* arc) {
    return mdExec_setAnmFNDirect(m, loop, morf, speed, fn, snd, arc);
}
/*  daPy_py_c::changePlayer(fopAc_ac_c*) */
static inline void daPy_changePlayer(fopAc_ac_c* self, fopAc_ac_c* pl) { mdExec_changePlayer(self, pl); }
// HD: one pad check replaces the GameCube R/Start trigger tests.
static inline BOOL md_padTrigRorStart(s32 port) { return mdExec_padTrigRorStart(port); }
static inline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx) { mdExec_getXYZRotateMtx(x, y, z, mtx); }
static inline u8 md_event_runCheck() { return dComIfGp_event_runCheck(); }

/* JPABaseEmitter (HD): flags at +0x254, global translation at +0x22C, rotation matrix at +0x1F0,
 * emitter callback at +0x1E4, max frame at +0x5C, global scale-y sign byte at +0x262 */
static inline void JPA_becomeInvalidEmitter(u32 e) {
    emitter(e)->mMaxFrame = -1;
    emitter(e)->mFlags = emitter(e)->mFlags | 1;
}

/*  dNpc_Md_setAnm(mDoExt_McaMorf*, ...) (not named by the matcher) */

/* inlined into MdBodyAnimationActor::dNpc_Md_setAnm */
static inline void* md_getObjectRes(const char* arc, const char* name) {
    SafeString a;
    SafeString n;
    a.mStringTop = gabi::guest_addr(arc);
    a.__vtbl = SAFE_STRING_VTABLE;
    n.mStringTop = gabi::guest_addr(name);
    n.__vtbl = SAFE_STRING_VTABLE;
    return mdExec_getObjectRes(dComIfG_resControl(), &a, &n);
}
/* strcmp, inlined by GHS (byte loop) */
static inline s32 md_strcmp(const char* s1, const char* s2) {
    u32 a = gabi::guest_addr(s1), b = gabi::guest_addr(s2);
    u8 c1, c2;
    do {
        c1 = bytes(a++)->get();
        c2 = bytes(b++)->get();
    } while (c1 == c2 && c1 != 0);
    return (s32)c1 - (s32)c2;
}
static inline void McaMorf2_setAnm(mDoExt_McaMorf2* m, void* a1, void* a2, f32 blend, s32 loop, f32 morf, f32 speed, f32 start, f32 end, void* snd) {
    mdExec_setMorfAnm(m, a1, a2, blend, loop, morf, speed, start, end, snd);
}
static inline u8 cLib_calcTimer_u8(be<u8>* t) { return mdExec_calcTimerU8(t); }
static inline void mDoExt_baseAnm_initPlay(void* anm, s16 frameMax, s32 mode, f32 speed, s16 start, s16 end, bool modify) {
    mdExec_initAnmPlay(anm, frameMax, mode, speed, start, end, modify);
}

static inline u16 md_eventCommand(fopAc_ac_c* a) { return a->eventInfo.mCommand; } /* eventInfo.mCommand */
static inline BOOL md_event_chkTalkXY() { return dComIfGp_event_chkTalkXY(); }
static inline void dEvt_control_order(u16 type, u16 prio, u16 flag, u16 hind, fopAc_ac_c* a1, fopAc_ac_c* a2, s16 eventId, u8 infoIdx) {
    dEvt_control_c* ev = dComIfGp_getEvent();
    mdExec_orderEvent(ev, type, prio, flag, hind, a1, a2, eventId, infoIdx);
}

/* GHS pointer-to-member equality against a constant {d = 0, i = -1, f} */
static inline bool md_pmf_is(ProcFunc_l* p, u32 f) {
    s16 i = p->i;
    if (i != -1) return false;
    if (i == 0) return true;
    return p->d == 0 && p->f == f;
}
/* play-object button status bytes (GameCube dComIfGp_set*Status) */
static inline void md_setDoStatus(u8 s) { dComIfGp_setDoStatus(s); }
static inline void md_setAStatus(u8 s) { dComIfGp_setAStatus(s); }
static inline void md_setRStatusForce(u8 s) { dComIfGp_setRStatusForce(s); }
static inline void cXyz_normalizeZP(cXyz* a, cXyz* out) { mdExec_normalizeZP(a, out); }
/*  dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void md_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    mdExec_lookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 f2u(f32 f) { u32 u; memcpy(&u, &f, 4); return u; }

/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) */
static inline s16 fpcLf_GetPriority(void* a) { return mdExec_getPriority(a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { mdExec_toDrawQueue(tag, prio); }
static inline void md_onDraw(fopAc_ac_c* a) {
    s16 prio = fpcLf_GetPriority(a);
    fopDwTg_ToDrawQ(gabi::ea(&a->draw_tag), prio);
}
static inline s16 cLib_calcTimer_s16(be<s16>* t) { return mdExec_calcTimerS16(t); }
static inline dSv_event_c* md_event() { return reinterpret_cast<dSv_event_c*>(&saveInfo.mpInfo->mEvent); }
static inline fpc_ProcID fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return mdExec_createItemForPresentDemo(pos, itemNo, argFlag, bitNo, roomNo, angle, scale);
}
static inline void md_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    mdExec_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
static inline u32 md_msgManager() { return messageManager.mpManager; } /* HD message manager */

static inline u32 dEvent_getRunEventName(dEvent_manager_c* m) { return mdExec_getRunEventName(m); }
/* sead::SafeString::cstr(): virtual at +0x14 (assure termination) */
static inline void SafeString_assure(SafeString* s) { assureString(s); }
/* sead::SafeString operator== (inlined: same pointer, or a byte loop of at most 0x40001 chars) */
static inline bool md_safeStringEq(SafeString* a, SafeString* b) {
    SafeString_assure(a);
    SafeString_assure(a);
    u32 pa = a->mStringTop;
    SafeString_assure(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 c1 = bytes(pa)[n];
        u8 c2 = bytes(pb)[n];
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}

static inline BOOL cBgS_ChkPolySafe(dBgS* bgs, u32 poly) { return mdExec_checkPolySafe(bgs, poly); }
/*  mDoExt_McaMorf2::play(cXyz* pos, u32 mtrlSndId, s8 reverb) */
static inline u8 McaMorf2_play(mDoExt_McaMorf2* m, cXyz* pos, u32 se, s32 reverb) { return mdExec_playMorf(m, pos, se, reverb); }
static inline BOOL McaMorf2_checkFrame(mDoExt_McaMorf2* m, f32 f) { return mdExec_checkFrame(gabi::ea(&MdExec::morf(m)->mPlaySpeed), f); }
/*  /  dEvent_manager_c::startCheckOld / endCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(u32 name) { return mdExec_startCheckOld(dComIfGp_getPEvtManager(), name); }
static inline BOOL dComIfGp_evmng_endCheckOld(u32 name) { return mdExec_endCheckOld(dComIfGp_getPEvtManager(), name); }
static inline s32 md_getRndValue(s32 base, s32 range) { return mdExec_getRndValue(base, range); }
static inline void md_monsSeStart(u32 se, cXyz* pos, u32 id, s32 reverb) { mdExec_monsSeStart(se, pos, id, reverb); }

/*  dDetect_c::chk_light(const cXyz*) (dDetect_c at play+0x5A20) */
static inline BOOL md_chk_light(cXyz* pos) { return mdExec_checkLight(detect(), pos); }
/*  cM3d_lineVsPosSuisenCross(const cXyz& start, const cXyz& end, const cXyz& pos, cXyz* out) */
static inline void cM3d_lineVsPosSuisenCross(u32 s, u32 e, u32 p, cXyz* out) { mdExec_lineVsPosSuisenCross(s, e, p, out); }
/*  daPy_mtxFollowEcallBack_c::makeEmitter(u16 id, MtxP, const cXyz* pos, const cXyz* scale) */
static inline void mtxFollow_makeEmitter(daPy_mtxFollowEcallBack_l* cb, u16 id, Mtx34* mtx, cXyz* pos, cXyz* scale) {
    mdExec_makeEmitter(cb, id, mtx, pos, scale);
}
static inline void mtxFollow_end(daPy_mtxFollowEcallBack_l* cb) { mdExec_endMtxFollow(cb); }

static inline void md_mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { mdExec_XYZrotM(m, x, y, z); }
static inline void cM3dGLin_SetStartEnd(u32 lin, cXyz* s, cXyz* e) { mdExec_setStartEnd(lin, s, e); }
static inline void md_dCcMassS_Set(void* obj, u8 p) { mdExec_setMass(mass(), obj, p); }
/* integer cXyz copy (GHS struct copy) */
static inline void md_xyz_copy(cXyz* dst, const cXyz* src) {

    u32 x = word(src->x), y = word(src->y), z = word(src->z);
    word(dst->x) = x;
    word(dst->y) = y;
    word(dst->z) = z;
}

/*  dEvent_manager_c::getMyActIdx(staffId, const char** tbl, int n, BOOL force, int nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, s32 force, s32 nameType) {
    return mdExec_getMyActIdx(dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* dComIfGp_event_setTalkPartner: dEvt_control_c (play+0x51D0) +0xCC = getPId(actor) () */
static inline void md_event_setTalkPartner(fopAc_ac_c* a) {
    dComIfGp_event_setTalkPartner(a);
}

/*  mDoExt_btpAnm::entry(J3DModelData*, s16 frame) */
static inline void btpAnm_entry(void* anm, J3DModelData* d, s16 frame) { mdExec_entryBtp(anm, d, frame); }
static inline void McaMorf2_entryDL(mDoExt_McaMorf2* m) { mdExec_entryMorfDL(m); }
static inline void daPy_npc_drawDamageFog(void* a) { mdExec_drawDamageFog(a); }
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { mdExec_concatMtx(a, b, ab); }
static inline void mirrorPacket_update(u32 pkt, Mtx34* m, u8 alpha, f32 f) { mdExec_updateMirror(pkt, m, alpha, f); }
/*  J3DDrawBuffer::entryImm(J3DPacket*, u16); the XLU list is at play+0x5D7C */
static inline void md_xluList_entryImm(u32 pkt, u16 idx) { mdExec_entryImmediate(MdExec::play()->mpXluList, pkt, idx); }

/* ---- execute ---- */
static inline BOOL daPy_npc_check_initialRoom(void* a) { return mdExec_checkInitialRoom(a); }
static inline BOOL daPy_npc_initialRestartOption(void* a, s8 p, BOOL b) { return mdExec_initialRestartOption(a, p, b); }
static inline BOOL daPy_npc_check_moveStop(void* a) { return mdExec_checkMoveStop(a); }
static inline BOOL daPy_npc_checkNowPosMove(void* a, u32 name) { return mdExec_checkNowPosMove(a, name); }
static inline s8 daPy_npc_chkMoveBlock(void* a, cXyz* p) { return mdExec_checkMoveBlock(a, p); }
static inline void daPy_py_objWindHitCheck(void* a, dCcD_Cyl* c) { mdExec_objWindHitCheck(a, c); }
static inline BOOL dBgS_ChkMoveBG(dBgS* bgs, u32 poly) { return mdExec_checkMoveBg(bgs, poly); }
static inline void dBgS_MoveBgCrrPos(dBgS* bgs, u32 poly, bool b, cXyz* pos, csXyz* a1, csXyz* a2) { mdExec_moveBgCrrPos(bgs, poly, b, pos, a1, a2); }
static inline s8 md_GetRoomId(dBgS* bgs, u32 poly) { return mdExec_getRoomId(bgs, poly); }
static inline u8 md_GetPolyColor(dBgS* bgs, u32 poly) { return mdExec_getPolyColor(bgs, poly); }
static inline s32 md_GetGroundCode(dBgS* bgs, u32 poly) { return mdExec_getGroundCode(bgs, poly); }
/*  dPa_control_c::setSimpleLand(poly, pos, angle, f32, f32, f32, tevstr, int* out, int) */
static inline void md_setSimpleLand(u32 poly, cXyz* pos, csXyz* ang, f32 a, f32 b, f32 c, dKy_tevstr_c* tev, s32* out, s32 n) {
    dPa_control_c* pa = dComIfGp_getParticle();
    mdExec_setSimpleLand(pa, poly, pos, ang, a, b, c, tev, out, n);
}
static inline void daPy_lk_startRestartRoom(fopAc_ac_c* lk, u32 mode, s32 idx, f32 f, s32 p) { mdExec_startRestartRoom(lk, mode, idx, f, p); }
static inline void md_setStatusMap(fopAc_ac_c* a, u32 v) { a->actor_status = (a->actor_status & ~0x3Fu) | v; }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* md_getAnmMtx(u32 model, s32 jnt) {
    J3DMtxBuffer_hd* blk = modelAt(model)->mpMtxBuffer;
    u32 mtx = blk->mpAnmMtx;
    blk->mFlags = (u16)(blk->mFlags | 0x10);
    return matrixAt(mtx + jnt * 0x30);
}
/* the room status flag 0x10 (dComIfGp_roomControl_checkStatusFlag): table at , 0x22C each */
static inline bool md_roomStatus10(s8 roomNo) {
    dComIfGp_get();
    return (roomStatus.mRooms[roomNo].mStatus & 0x10) != 0;
}
/* the stage name (play+0x5134) as a SafeString, compared with a literal */
/* inlined into daNpc_Md_c::execute */
static inline bool md_stageNameIs(u32 lit) {
    SafeString a;
    SafeString b;
    a.__vtbl = SAFE_STRING_VTABLE;
    a.mStringTop = lit;
    u32 stage = dComIfGp_ea() + 0x5134;
    b.mStringTop = stage;
    b.__vtbl = SAFE_STRING_VTABLE;
    return md_safeStringEq(&a, &b);
}
/* the ground polygon: GetTriPla normal, room, colour, mPolyInfo */
static inline void md_setGroundInfo(daNpc_Md_c* t) {
    auto* gnd = static_cast<cBgS_PolyInfo*>(&t->mAcch.m_gnd); /* mAcch.m_gnd's cBgS_PolyInfo */
    u32 pla = gabi::ea(cBgS_GetTriPla(dComIfG_Bgsp(), gnd->mBgIndex, gnd->mPolyIndex));
    if (pla != 0) {
        md_xyz_copy(&t->m32A4, normalAt(pla)); /* *triPla->GetNP() */
    }
    s8 roomNo = md_GetRoomId(dComIfG_Bgsp(), gabi::ea(gnd));
    t->current.roomNo = roomNo;
    t->tevStr.mRoomNo = roomNo; /* tevStr.mRoomNo */
    u8 color = md_GetPolyColor(dComIfG_Bgsp(), gabi::ea(gnd));
    t->tevStr.mEnvrIdxOverride = color;  /* tevStr.mEnvrIdxOverride */
    t->mStts.mRoomId = roomNo;          /* mStts.SetRoomId */
    /* mPolyInfo.SetPolyInfo(mAcch.m_gnd) */
    t->mPolyInfo.mPolyIndex = gnd->mPolyIndex;
    t->mPolyInfo.mBgIndex = gnd->mBgIndex;
    t->mPolyInfo.mpBgW = gnd->mpBgW;
    t->mPolyInfo.mProcId = gnd->mActorId;
}

/* 022887DC */
void daNpc_Md_c::npcAction(void* arg) {
    if (mCurrNpcActionFunc.i == 0) { /* mCurrNpcActionFunc == NULL */
        speedF = 0.0f;
        if (m3104 == 0x20) {
            gabi::Local<ProcFunc_l> fn;
            copyAction(fn, action_mwaitNpcAction);
            setNpcAction(fn, nullptr);
        } else {
            gabi::Local<ProcFunc_l> fn;
            copyAction(fn, action_waitNpcAction);
            setNpcAction(fn, nullptr);
        }
    }
    md_pmf_call<void>(this, &mCurrNpcActionFunc, arg);
}

/* 02288290 */
void daNpc_Md_c::playerAction(void* arg) {
    if (mCurrPlayerActionFunc.i == 0) { /* mCurrPlayerActionFunc == NULL */
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        copyAction(fn, action_waitPlayerAction);
        setPlayerAction(fn, nullptr);
    }
    if (mAcch.ChkGroundHit() && checkStatus(daMdStts_LIGHT_BODY_HIT)) {
        md_setRStatusForce(7); /* dActStts_RETURN_e */
        if (md_pmf_is(&mCurrPlayerActionFunc, MKAMAE_PLAYER_ACTION /* mkamaePlayerAction */)) {
            md_setDoStatus(0);    /* dActStts_BLANK_e */
            md_setAStatus(8);     /* dActStts_PUT_AWAY_e */
        } else {
            md_setDoStatus(0x2E); /* dActStts_ba_motu__dupe_2E */
            md_setAStatus(0x3E);  /* dActStts_HIDDEN_e */
            if (!mOldLightBodyHit) {
                cXyz up;
                up.x = 0.0f;
                up.y = 1.0f;
                up.z = 0.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, &up);
            }
        }
    } else {
        md_setDoStatus(0x23); /* dActStts_FLY_e */
        if (md_pmf_is(&mCurrPlayerActionFunc, FLY_PLAYER_ACTION /* flyPlayerAction */)) {
            md_setRStatusForce(0x3E); /* dActStts_HIDDEN_e */
            md_setAStatus(6);         /* dActStts_LET_GO_e */
        } else {
            md_setRStatusForce(7);
            md_setAStatus(0x3E);
        }
    }
    md_pmf_call<void>(this, &mCurrPlayerActionFunc, arg);
}

/* 02288240 */
void daNpc_Md_c::setPlayerAction(ProcFunc_l* actionFunc, void* arg) {
    mCurrNpcActionFunc.d = 0;
    mCurrNpcActionFunc.i = 0;
    mCurrNpcActionFunc.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    copyAction(fn, *reinterpret_cast<ActionWords*>(actionFunc));
    setAction(&mCurrPlayerActionFunc, fn, arg);
}

/* 0228A1FC */
void daNpc_Md_c::deleteHane02Emitter() {
    emitterDelete(&m0508[2]);
    emitterDelete(&m0508[3]);
}

/* 0228A228 */
void daNpc_Md_c::deleteHane03Emitter() {
    emitterDelete(&m0508[4]);
    emitterDelete(&m0508[5]);
}

/* 02287EAC */
void daNpc_Md_c::returnLinkPlayer() {
    daPy_changePlayer(this, dComIfGp_getLinkPlayer());
    m_mirror = 0; /* offMirror() */
    m_flying = 0; /* offFlying() */
}

/* 02287684 */
BOOL daNpc_Md_c::isFallAction() {
    /* chkPlayerAction(jumpPlayerAction) || chkNpcAction(fallNpcAction) || chkNpcAction(fall02NpcAction);
     * GHS compares pointers to member as: same i, and (i == 0 or same d and f); the constants are
     * {d = 0, i = -1, f} */
    s16 pi = mCurrPlayerActionFunc.i;
    if (pi == -1) {
        if (pi == 0) return TRUE;
        if (mCurrPlayerActionFunc.d == 0 && mCurrPlayerActionFunc.f == JUMP_PLAYER_ACTION) return TRUE;
    }
    s16 ni = mCurrNpcActionFunc.i;
    if (ni == -1) {
        if (ni == 0) return TRUE;
        s16 nd = mCurrNpcActionFunc.d;
        if (nd == 0 && mCurrNpcActionFunc.f == FALL_NPC_ACTION) return TRUE;
        if (nd == 0 && mCurrNpcActionFunc.f == FALL02_NPC_ACTION) return TRUE;
    }
    return FALSE;
}

/* 022884D4 */
BOOL daNpc_Md_c::returnLinkCheck() {
    if (!md_event_runCheck()) {
        // HD: one pad call (GameCube CPad_CHECK_TRIG_R(0) || CPad_CHECK_TRIG_START(0))
        if (md_padTrigRorStart(0)) {
            if (mAcch.ChkGroundHit()) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* 02287754 */
BOOL daNpc_Md_c::lightHitCheck() {
    BOOL lightHit = FALSE;
    cXyz lightVec;
    PSMTXMultVecSR(J3DModel_getBaseTRMtx(mpHarpModel), &l_ms_light_local_vec, &lightVec);
    clearStatus(daMdStts_LIGHT_BODY_HIT);

    if (md_chk_light(&current.pos)) {
        setBitStatus(daMdStts_LIGHT_BODY_HIT);
        lightHit = TRUE;
    } else {
        void* hitObj = mCyl3.GetTgHitObj();
        if (hitObj && (reinterpret_cast<HitObject*>(hitObj)->mAtType & 0x00800000) /* ChkAtType(AT_TYPE_LIGHT) */) {
            setBitStatus(daMdStts_LIGHT_BODY_HIT);
            if (PSVECDotProduct(&mCyl3.mGObjTg.mRVec, &lightVec) < 0.0f) {
                lightHit = TRUE;
            }
        }
    }

    if (lightHit) {
        setBitStatus(daMdStts_LIGHT_HIT);
        if (mCps.ChkAtHit()) {
            fopAc_ac_c* hitActor = mCps.GetAtHitAc();
            if ((actor_status & fopAcStts_CARRY_e) /* fopAcM_checkCarryNow */ && !checkStatus(daMdStts_CARRY_ACTION)) {
                if (hitActor != dComIfGp_getLinkPlayer() && m3058.mpEmitter.get() == nullptr) {
                    dComIfGp_particle_set(0x8232 /* ID_AK_SN_HITSHIELDLIGHT00 */, &current.pos, nullptr, nullptr, 0xFF,
                                          (dPa_levelEcallBack*)(void*)&m3058);
                }
            } else if (m3058.mpEmitter.get() == nullptr) {
                dComIfGp_particle_set(0x8232, &current.pos, nullptr, nullptr, 0xFF, (dPa_levelEcallBack*)(void*)&m3058);
            }
            auto* cps = reinterpret_cast<Capsule*>(&mCps);
            cM3d_lineVsPosSuisenCross(gabi::ea(&cps->mGeometry.mStart), gabi::ea(&cps->mGeometry.mEnd), gabi::ea(&mCps.mGObjAt.mHitPos), &m3058.mPos);
            /* lightVec.absXZ(): sqrt(PSVECSquareMag({x, 0, z})) */
            cXyz xz;

            word(xz.x) = word(lightVec.x);
            xz.y = 0.0f;
            word(xz.z) = word(lightVec.z);
            f32 absXZ = std_sqrtf(PSVECSquareMag(&xz));
            s16 angleX = cM_atan2s(-lightVec.y, absXZ);
            s16 angleY = cM_atan2s(lightVec.x, lightVec.z);
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
            MdExec::emitter(emitter)->mEmitterScale.x = 1.0f; /* setEmitterScale(1, 1, 1) */
            MdExec::emitter(emitter)->mEmitterScale.y = 1.0f;
            MdExec::emitter(emitter)->mEmitterScale.z = 1.0f;
            MdExec::emitter(emitter)->mEmitterTranslation.x = 0.0f; /* setEmitterTranslation(0, 4, 0) */
            MdExec::emitter(emitter)->mEmitterTranslation.y = 4.0f;
            MdExec::emitter(emitter)->mEmitterTranslation.z = 0.0f;
        }
    } else {
        clearStatus(daMdStts_LIGHT_HIT);
        m3058.end();
        if (m304C.mpEmitter.get() != nullptr) {
            MdExec::emitter(m304C.mpEmitter.get())->mGlobalAlpha = 0; /* setGlobalAlpha(0) */
            mtxFollow_end(&m304C);
        }
    }
    return lightHit;
}

/* 02288544 */
BOOL daNpc_Md_c::checkCollision(int r30) {
    if ((mType == 5 || mType == 4) && mCyl1.ChkTgHit()) { /* isTypeM_Dai() || isTypeEdaichi() */
        fopAc_ac_c* hit_actor = mCyl1.GetTgHitAc();
        if (hit_actor) {
            cXyz sp3C;
            cXyz norm;
            gabi::Local<be<s16>> sp08;
            cXyz_mi(&current.pos, &sp3C, &hit_actor->current.pos);
            sp3C.y = 0.0f;
            cXyz_normalizeZP(&sp3C, &norm);
            f32 x = sp3C.x;
            f32 z = sp3C.z;
            s16 ang;
            if (std::fabs(x) < 0.001f && std::fabs(z) < 0.001f) {
                ang = 0;
            } else {
                ang = cM_atan2s(x, z);
            }
            *sp08.get() = ang;
            gabi::Local<ProcFunc_l> fn;
            if (r30 != 0) {
                copyAction(fn, action_hitPlayerAction);
                setPlayerAction(fn, nullptr);
            } else {
                copyAction(fn, action_hitNpcAction);
                setNpcAction(fn, sp08.get());
            }
            return TRUE;
        }
    }
    return FALSE;
}

/* 0228A4E4 */
void daNpc_Md_c::changeCaught02() {
    setAnm(0x25);
    setBitStatus(daMdStts_UNK1);
}

/* 02287EFC */
BOOL daNpc_Md_c::eventProc() {
    if (eventInfo_checkCommandDemoAccrpt(this) && mCurEventMode != 0) {
        if (mCurEventMode == 0xC) {
            if (dComIfGp_evmng_startCheckOld(OPTION_CHAR_END_NAME /* "OPTION_CHAR_END" */) || dComIfGp_evmng_endCheckOld(OPTION_CHAR_END_NAME)) {
                md_event_setTalkPartner(dComIfGp_getLinkPlayer());
                mdExec_startControlSound(0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
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
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(l_staff_name.mpName.get(), nullptr, 0);
    if (md_event_runCheck() && !checkCommandTalk()) {
        if (staffIdx != -1) {
            s32 actIdx = dComIfGp_evmng_getMyActIdx(staffIdx, gabi::ea(&cut_name_tbl), 0x16, TRUE, 0);
            if (actIdx == -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            } else {
                if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
                    md_pmf_call<void>(this, &event_init_tbl.values[actIdx], staffIdx);
                }
                if (md_pmf_call<BOOL>(this, &event_action_tbl.values[actIdx], staffIdx)) {
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
        } else if (dComIfGp_getLinkPlayer()->eventInfo.mCommand != 3) { /* !checkCommandDoor() */
            return TRUE;
        }
    }
    return FALSE;
}

/* 0228A520 */
void daNpc_Md_c::initialDefault(int) {
}

/* 0228A67C */
BOOL daNpc_Md_c::actionDefault(int) {
    lookBack(0, 0, 1);
    return TRUE;
}

/* 0228A6AC */
void daNpc_Md_c::initialWaitEvent(int staffIdx) {
    be<s32>* pDisp = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "Disp", 3);
    if (pDisp) {
        if (*pDisp) {
            md_onDraw(this);
        } else {
            fopAcM_offDraw(this);
        }
    } else {
        md_onDraw(this);
    }
    be<s32>* pTimer = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "Timer", 3);
    if (pTimer) {
        m3118 = (s16)*pTimer;
    } else {
        m3118 = 0;
    }
    speedF = 0.0f;
}

/* 0228A7D4 */
BOOL daNpc_Md_c::actionWaitEvent(int) {
    return cLib_calcTimer_s16(&m3118) == 0 ? TRUE : FALSE;
}

/* 0228A800 */
void daNpc_Md_c::initialLetterEvent(int staffIdx) {
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
        dComIfGp_event_setItemPartnerId(itemID); /* dComIfGp_event_setItemPartnerId */
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}

/* 0228A928 */
void daNpc_Md_c::initialMsgSetEvent(int staffIdx) {
    if (mMsgNo == 0x19DD) {
        dSv_event_onEventBit(md_event(), 0x1620);
        dSv_event_onEventBit(md_event(), 0x1608);
        md_setNextStage("sea", 0xE3, 0xD /* dIsleRoom_DragonRoostIsland_e */, 8, 0.0f, 0, 1, 0);
    } else {
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        be<s32>* pMsgNo = (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffIdx, "MsgNo", 3);
        if (pMsgNo != nullptr) {
            mMsgNo = *pMsgNo;
            if (mMsgNo == 0x19D6 && dSv_event_isEventBit(md_event(), 0x1504)) {
                mMsgNo = 0x19D9;
                return;
            }
        }
        // HD: the tact message (0x5AC) sets a play-object byte
        if (mMsgNo == 0x5AC) {
            MdExec::play()->mTactMessageMode = 0xFF;
        }
    }
}

/* 0228AAF4 */
BOOL daNpc_Md_c::actionMsgSetEvent(int) {
    lookBack(1, 0, 0);
    setAttention(true);
    return talk_init();
}

/* 0228AA7C */
BOOL daNpc_Md_c::talk_init() {
    // HD: no fopMsgM_SearchByID; the message is "ready" as soon as an id is set
    u32 msgId = l_msgId;
    u32 mgr = md_msgManager();
    if (msgId == fpcM_ERROR_PROCESS_ID_e) {
        if (mMsgNo == 0x5AC) {
            l_msgId = mdExec_setTactMessage(mgr); /* fopMsgM_tactMessageSet */
        } else {
            l_msgId = mdExec_setMessage(mgr, (u32)mMsgNo, &eyePos); /* fopMsgM_messageSet */
        }
        return FALSE;
    }
    return TRUE;
}

/* 0228AB40 */
int daNpc_Md_c::getAnmType(u8 r4) {
    if (r4 < 0x2C) {
        return anmTypeData_Talk.values[r4]; /* anmTypeData_Talk */
    }
    return -1;
}

/* 0228729C */
void daNpc_Md_c::playTexPatternAnm() {
    if (m3136 == 1) {
        /* m0520.setFrame(m30D0): an lfs/stfs copy (bit exact) */
        word(reinterpret_cast<TexturePatternAnm*>(m0520)->mFrame) = word(m30D0);
        return;
    }
    if (mDoExt_baseAnm_play(m0520)) {
        if (cLib_calcTimer_u8(&m3133) == 0) {
            m3133 = (u8)gabi::ftoi(cM_rndF(60.0f) + 30.0f);
            mDoExt_baseAnm_initPlay(m0520, m3112, 1, 1.0f, 0, -1, true);
        }
    }
}

/* 0228735C */
u32 daNpc_Md_c::playLightBtkAnm() {
    return mLightBtkAnm.play();
}

/* 02286D7C */
BOOL daNpc_Md_c::setAnm(int anmIdx) {
    /* static anm_prm l_anmPrm.values[] at  {s8 anmTblIdx; u8 armAnmTblIdx; u8 btpAnmTblIdx; int loopMode; f32 morf; f32 speed}
     * l_anmTbl , armAnmTbl , wingAnmTbl  ({char[0x20], char[0x20]}) */
    AnmPrm* prm = &l_anmPrm.values[anmIdx];
    BOOL ret = FALSE;
    m3104 = anmIdx;
    f32 speed = prm->speed;
    f32 morf = prm->morf;
    // HD: no morphing into the wait animation during the "ARRIVAL_GND" event
    if (anmIdx == 0) {
        SafeString a;
        SafeString b;
        a.__vtbl = SAFE_STRING_VTABLE;
        a.mStringTop = ARRIVAL_GND_NAME; /* "ARRIVAL_GND" */
        u32 name = dEvent_getRunEventName(dComIfGp_getPEvtManager());
        b.__vtbl = SAFE_STRING_VTABLE;
        b.mStringTop = name;
        if (md_safeStringEq(&a, &b)) {
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

    s8 anmTblIdx = prm->anmTblIdx;
    if (anmTblIdx != m312D || MdExec::morf(mpMorf)->mPlaySpeed != prm->speed) {
        /* mpMorf->getPlaySpeed() != prm->speed */
        m312A = 0;
        m312D = anmTblIdx;
        AnmTbl* bodyAnm = &l_anmTbl.values[anmTblIdx];
        dNpc_Md_setAnm(mpMorf, mRunRate, prm->loopMode, morf, speed, bodyAnm->m00, bodyAnm->m20, mModelArcName);
        /* m30D0 = mpMorf->getFrame() (lfs/stfs copy) */
        word(m30D0) = word(MdExec::morf(mpMorf)->mFrame);
        u8 armIdx = prm->armAnmTblIdx;
        if (armIdx < 0x80) {
            AnmTbl* armAnm = &armAnmTbl.values[armIdx];
            dNpc_Md_setAnm(mpArmMorf, mRunRate, prm->loopMode, morf, speed, armAnm->m00, armAnm->m20, mModelArcName);
            ret = FALSE;
        } else {
            if (!isTypeShipRide()) {
                AnmTbl* wingAnm = &wingAnmTbl.values[armIdx - 0x80];
                dNpc_Md_setAnm((mDoExt_McaMorf*)mpWingMorf.get(), prm->loopMode, morf, speed, wingAnm->m00, mModelArcName);
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

    u8 btpIdx = prm->btpAnmTblIdx;
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
                                                      roomNo, &tevStr.mColorK0, &tevStr.mColorK0);
            m0508[1] = gabi::ea(e);
            if (e) {
                MdExec::emitter(e)->mFlags = MdExec::emitter(e)->mFlags | 0x40; /* becomeImmortalEmitter */
            }
        }
    } else {
        deletePiyoPiyo();
    }
    return ret;
}

/* 02286BC0 */
bool MdBodyAnimationActor::dNpc_Md_setAnm(mDoExt_McaMorf2* pMorf, f32 param_f1, int loopMode, f32 morf, f32 speed, const char* fileName1,
                                const char* filename2, const char* arcName) {
    bool ret = false;
    void* bck2 = nullptr;
    if (pMorf) {
        void* bck1 = md_getObjectRes(arcName, fileName1);
        if (md_strcmp(filename2, "") != 0) {
            bck2 = md_getObjectRes(arcName, filename2);
        }
        McaMorf2_setAnm(pMorf, bck1, bck2, param_f1, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
        ret = true;
    }
    return ret;
}

/* 02286D04 */
bool MdWingAnimationActor::dNpc_Md_setAnm(mDoExt_McaMorf* pMorf, int loopMode, f32 morf, f32 speed, const char* animFilename, const char* arcName) {
    return dNpc_setAnmFNDirect(pMorf, loopMode, morf, speed, animFilename, nullptr, arcName);
}

/* 022886B4 */
void daNpc_Md_c::carryCheck() {
    if (actor_status & fopAcStts_CARRY_e) { /* fopAcM_checkCarryNow */
        gabi::Local<ProcFunc_l> fn;
        copyAction(fn, action_carryNpcAction);
        setNpcAction(fn, nullptr);
    }
}

/* 022888C4 */
void daNpc_Md_c::eventOrder() {
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
            mCurEvent = eventModes.values[(s32)mCurEventMode]; /* {4: 0, ..., 9: 5} */
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

/* 022886FC */
void daNpc_Md_c::checkOrder() {
    if (md_eventCommand(this) == 1 && (mCurEventMode == 1 || mCurEventMode == 2 || mCurEventMode == 3)) {
        mCurEventMode = 0;
        gabi::Local<ProcFunc_l> fn;
        if (md_event_chkTalkXY()) {
            setBitStatus(daMdStts_XY_TALK);
            if (checkStatus(daMdStts_DEFAULT_TALK_XY)) {
                copyAction(fn, action_talkNpcAction);
                setNpcAction(fn, nullptr);
            }
        } else if (checkStatus(daMdStts_SHIP_RIDE)) { /* isShipRide() */
            copyAction(fn, action_shipTalkNpcAction);
            setNpcAction(fn, nullptr);
        } else {
            copyAction(fn, action_talkNpcAction);
            setNpcAction(fn, nullptr);
        }
        fopAcM_cancelCarryNow(this);
    }
}

/* 02287DF0 */
BOOL daNpc_Md_c::checkCommandTalk() {
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

/* 02287A74 */
void daNpc_Md_c::setCollision() {
    cXyz local_44;
    cXyz local_50;
    cXyz local_2c;
    cXyz local_38;
    cXyz tmp;
    cXyz tmp2;
    md_xyz_copy(&local_2c, &current.pos);
    f32 radius = 30.0f;
    if (!checkStatus(daMdStts_SHIP_RIDE) && !(actor_status & fopAcStts_CARRY_e) /* fopAcM_checkCarryNow */) {
        mCyl1.SetC(&local_2c);
        mCyl1.SetR(radius);
        mCyl1.SetH(m30F8);
        dComIfG_Ccsp_Set(&mCyl1);
    } else {
        mdExec_clearCoHit(&mCyl1); /* ClrCoHit */
        mCyl1.ClrTgHit();
    }
    mCyl2.SetC(&local_2c);
    mCyl2.SetR(radius);
    mCyl2.SetH(m30F8);
    dComIfG_Ccsp_Set(&mCyl2);
    mCyl3.SetC(&local_2c);
    mCyl3.SetR(radius);
    mCyl3.SetH(m30F8);
    dComIfG_Ccsp_Set(&mCyl3);
    if (checkStatus(daMdStts_LIGHT_HIT)) {
        PSMTXCopy(J3DModel_getBaseTRMtx(mpHarpModel), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(l_HIO().m034.m0C, l_HIO().m034.m10, l_HIO().m034.m14);
        md_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), l_HIO().m034.m18, l_HIO().m034.m1A, l_HIO().m034.m1C);
        PSMTXMultVec(mDoMtx_stack_c::get(), &l_ms_light_local_start, &local_44);
        PSMTXMultVecSR(mDoMtx_stack_c::get(), &l_ms_light_local_vec, &local_38);
        cXyz_pl(&local_44, &tmp, &local_38);
        md_xyz_copy(&local_50, &tmp);
        dBgS_LinChk_Set(mLinChk, &local_44, &local_50, this);
        if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk)) {
            md_xyz_copy(&local_50, &reinterpret_cast<dBgS_LinChk*>(mLinChk)->mLin.mEnd); /* mLinChk.GetCross() */
            cXyz_mi(&local_50, &tmp2, &local_44);
            md_xyz_copy(&local_38, &tmp2);
        }
        cM3dGLin_SetStartEnd(gabi::ea(&reinterpret_cast<Capsule*>(&mCps)->mGeometry), &local_44, &local_50); /* mCps.SetStartEnd */
        md_xyz_copy(&mCps.mGObjAt.mVec, &local_38);                       /* mCps.SetAtVec */
        mCps.mObjAt.mSPrm = mCps.mObjAt.mSPrm | 1;                       /* OnAtSetBit */
        dComIfG_Ccsp_Set(&mCps);
        md_dCcMassS_Set(&mCps, 1);
        actor_status &= ~fopAcStts_CULL_e; /* fopAcM_OffStatus(this, fopAcStts_CULL_e) */
    } else {
        mdExec_resetAtHit(&mCps); /* ResetAtHit */
        mCps.mObjAt.mSPrm = mCps.mObjAt.mSPrm & ~1u; /* OffAtSetBit */
        actor_status |= fopAcStts_CULL_e;
    }
}

/* 0228A524 */
void MdLookBackActor::lookBack(int param_1, int param_2, int param_3) {
    cXyz local_30;
    cXyz local_3c;
    cXyz eye;
    cXyz* dstPos = nullptr;
    /* &local_3c: setall(0.0f), or current.pos with eyePos.y (lfs/stfs copies, bit exact) */
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
        dNpc_playerEyePos(&eye, l_HIO().mNpc.m04);

        word(local_30.x) = word(eye.x);
        word(local_30.z) = word(eye.z);
        word(local_30.y) = word(eye.y);
        lx = word(current.pos.x);
        ly = word(eyePos.y);
        lz = word(current.pos.z);
        dstPos = &local_30;
    }

    word(local_3c.x) = lx;
    word(local_3c.y) = ly;
    word(local_3c.z) = lz;
    md_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, &local_3c, desiredYRot, m3110, param_3 != 0);
}

/* 02286D18 */
void daNpc_Md_c::deletePiyoPiyo() {
    u32 emitter = m0508[1];
    if (emitter == 0) {
        return;
    }
    MdExec::emitter(emitter)->mFlags = MdExec::emitter(emitter)->mFlags & ~0x40u; /* quitImmortalEmitter */
    JPA_becomeInvalidEmitter(m0508[1]);
    /* every particle of the emitter's list (first link at +0x1AC, next at +0xC): setDeleteParticleFlag */
    for (u32 link = MdExec::emitter(emitter)->mpFirstParticle; link != 0; link = particleLink(link)->mpNext) {
        u32 obj = particleLink(link)->mpObject;
        particle(obj)->mFlags = particle(obj)->mFlags | 2;
    }
    m0508[1] = 0;
}

/* 02289E2C */
BOOL daNpc_Md_c::draw() {
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        u32 ship = gabi::ea(dComIfGp_getShipActor()); /* dComIfGp_getShipActor() */
        if (ship != 0 && (shipState(ship)->mHeadFlags & 0x200000) /* checkHeadNoDraw() */) {
            return TRUE;
        }
    } else {
        if (home.roomNo < 0) { /* fopAcM_GetHomeRoomNo */
            return TRUE;
        }
        /* dComIfGp_roomControl_checkStatusFlag(current.roomNo, 0x10): room status table at  (0x22C each) */
        s8 roomNo = current.roomNo;
        dComIfGp_get();
        if (!(roomStatus.mRooms[roomNo].mStatus & 0x10)) {
            return TRUE;
        }
    }
    J3DModel* model = modelAt(gabi::ea(MdExec::morf(mpMorf)->mpModel.get())); /* getModel() */
    J3DModelData* modelData = J3DModel_getModelData(model);
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    daPy_npc_drawDamageFog(this);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    btpAnm_entry(m0520, modelData, (s16)gabi::ftoi(reinterpret_cast<TexturePatternAnm*>(m0520)->mFrame));
    McaMorf2_entryDL(mpMorf);
    reinterpret_cast<ModelAnmState*>(modelData)->mpBtp = 0; /* m0520.remove(modelData) */

    if (!isTypeShipRide() && checkStatus(daMdStts_UNK1)) {
        J3DModel* limbModel = modelAt(gabi::ea(mpWingMorf->mpModel.get()));
        setLightTevColorType(dKy_getEnvlight(), limbModel, &tevStr);
        mpWingMorf->entryDL();
    } else {
        J3DModel* limbModel = modelAt(gabi::ea(MdExec::morf(mpArmMorf)->mpModel.get()));
        setLightTevColorType(dKy_getEnvlight(), limbModel, &tevStr);
        McaMorf2_entryDL(mpArmMorf);
    }
    setLightTevColorType(dKy_getEnvlight(), mpHarpModel, &tevStr);
    mDoExt_modelUpdateDL(mpHarpModel, 0);

    if (checkStatus(daMdStts_LIGHT_HIT)) {
        mLightBtkAnm.entry(J3DModel_getModelData(mpHarpLightModel), mLightBtkAnm.mFrameCtrl.mFrame);
        mDoExt_modelUpdateDL(mpHarpLightModel, 0);
        reinterpret_cast<ModelAnmState*>(J3DModel_getModelData(mpHarpLightModel))->mpBtk = 0; /* mLightBtkAnm.remove() */

        Mtx34 mtx;
        PSMTXCopy(J3DModel_getBaseTRMtx(mpHarpModel), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(l_HIO().m034.m0C, l_HIO().m034.m10, l_HIO().m034.m14);
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
        cXyz* start = &l_ms_light_local_start;
        PSMTXTrans(mDoMtx_stack_c::get(), start->x, start->y, start->z); /* transS */
        mDoMtx_YrotM(mDoMtx_stack_c::get(), -0x8000);
        f32 len = std_sqrtf(PSVECSquareMag(&mCps.mGObjAt.mVec)); /* GetAtVecP()->abs() */
        // HD: scale (1, 1, len / 960) (GameCube 0.1, 0.1, len / 9600)
        mDoMtx_stack_c::scaleM(1.0f, 1.0f, len * (1.0f / 960.0f));
        PSMTXConcat(&mtx, mDoMtx_stack_c::get(), mDoMtx_stack_c::get()); /* revConcat */
        mirrorPacket_update(gabi::ea(m0B70), mDoMtx_stack_c::get(), 0xFF, 90.0f);
        md_xluList_entryImm(gabi::ea(m0B70), 0x1F);
    }
    // HD: no real shadow (GameCube dComIfGd_setShadow + addRealShadow)
    dSnap_RegistFig(0x8A /* DSNAP_TYPE_NPC_MD */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02287364 */
void daNpc_Md_c::animationPlay() {
    /* l_harp_play_se.values[12] at  */
    u32 mtrlSndId = 0;
    if (checkStatus(daMdStts_UNK20000) && mAcch.ChkGroundHit() &&
        cBgS_ChkPolySafe(dComIfG_Bgsp(), gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)) /* m_gnd's poly info */)) {
        mtrlSndId = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    m312A = McaMorf2_play(mpMorf, &eyePos, mtrlSndId, reverb);
    bool bVar3 = true;
    if (md_event_runCheck() && (dComIfGp_evmng_startCheckOld(event_name_tbl.values[5]) ||
                                dComIfGp_evmng_endCheckOld(event_name_tbl.values[5]))) {
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
                se = l_harp_play_se.values[i];
                m313E = 2;
                m30F4 = se;
            } else {
                m313E = m313E - 1;
                se = m30F4;
            }
            s32 rv = dComIfGp_getReverb(current.roomNo);
            md_monsSeStart(se, &current.pos, mBsPcId /* fopAcM_GetID */, rv);
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
    f32 playSpeed = morfState(morf)->mPlaySpeed;
    if (playSpeed < 0.0f) {
        if (m30D0 < morfState(morf)->mFrame) {
            m312A = 1;
        }
    } else if (m30D0 > morfState(morf)->mFrame) {
        m312A = 1;
    }
    /* m30D0 = mpMorf->getFrame() (lfs/stfs copy) */
    word(m30D0) = word(morfState(morf)->mFrame);
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

/* 02286B5C */
void daNpc_Md_c::checkPlayerRoom() {
    m_playerRoom = 0; /* offPlayerRoom() */
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if ((u32)(s32)current.roomNo == (u32)(s32)link->current.roomNo) {
        m_playerRoom = 1;
    }
}

/* 02288B08 */
BOOL daNpc_Md_c::execute() {
    clearStatus(daMdStts_UNK20000);
    mOldLightBodyHit = checkStatus(daMdStts_LIGHT_BODY_HIT) ? 1 : 0; /* setOldLightBodyHit() */
    actor_status &= ~fopAcStts_SHOWMAP_e;                                         /* fopAcM_OffStatus(SHOWMAP) */
    checkPlayerRoom();
    cLib_calcTimer_u8(&mDamageFogTimer); /* executeDamageFog() */
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        daNpc_Md_HIO_l& h = l_HIO();
        mJntCtrl.setParam(h.m074.m10, h.m074.m12, h.m074.m14, h.m074.m16, h.m074.m08, h.m074.m0A, h.m074.m0C, h.m074.m0E,
                          h.mNpc.mMaxTurnStep);
    } else if (mType != 6) { /* !isTypeM_DaiB() */
        if (!daPy_npc_check_initialRoom(this)) {
            if (gabi::ea(MdExec::play()->mpCb1Player.get()) == gabi::ea(this)) { /* dComIfGp_getCb1Player() */
                MdExec::play()->mpCb1Player = nullptr;
            }
            return TRUE;
        }
        daPy_npc_initialRestartOption(this, 2, mType == 5);
        if (gabi::ea(MdExec::play()->mpCb1Player.get()) == gabi::ea(this) &&
            (!dSv_event_isEventBit(md_event(), 0x1620) || mType == 7 || mType == 6)) {
            MdExec::play()->mpCb1Player = nullptr;
        }
        fopAcM_setStageLayer(this);
        if (daPy_npc_check_moveStop(this)) {
            if (!isTypeShipRide()) {
                gabi::Local<ProcFunc_l> fn;
                copyAction(fn, action_waitNpcAction);
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
    } else if (!checkStatus(daMdStts_SHIP_RIDE) && !(m4E4 & 1) /* isReturnLink() */ && !(actor_status & fopAcStts_CARRY_e)) {
        if (daPy_npc_checkNowPosMove(this, gabi::ea(l_staff_name.mpName.get()))) {
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
            fopAcM_posMove(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }
        if (checkStatus(daMdStts_UNK4)) {
            daPy_py_objWindHitCheck(this, &mCyl2);
        }
        mAcch.CrrPos(dComIfG_Bgsp());
        setBitStatus(daMdStts_UNK20000);
        bool bVar10 = false;
        /* mAcch.m_wtr.GetHeight() (+0x1BC) */
        if (mAcch.ChkWaterIn() && mAcch.m_wtr.m_height > current.pos.y + 5.0f) {
            if (m3074.mpEmitter.get() == nullptr) {
                /* dComIfGp_particle_setShipTail (HD: group 5) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33 /* ID_AK_JN_HAMON00 */, &current.pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)(void*)&m3074, -1, nullptr, nullptr, nullptr);
            }
            m3074.mRate = 0.0f;
            bVar10 = true;
        }
        if (!bVar10 && m3074.mpEmitter.get() != nullptr) {
            mdExec_endRipple(&m3074); // HD: dPa_rippleEcallBack::end (GameCube remove())
        }
        if (!mAcch.ChkGroundHit()) {
            if (md_pmf_is(&mCurrPlayerActionFunc, WALK_PLAYER_ACTION /* walkPlayerAction */) ||
                md_pmf_is(&mCurrNpcActionFunc, SEARCH_NPC_ACTION /* searchNpcAction */) ||
                md_pmf_is(&mCurrNpcActionFunc, WAIT_NPC_ACTION /* waitNpcAction */)) {
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
            md_setSimpleLand(gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)), &current.pos, &shape_angle, 1.25f, 1.5f, 1.0f, &tevStr, (s32*)(void*)iStack_88.get(), 7);
        }
        m3131 = mAcch.ChkGroundHit();
        if (mAcch.GetGroundH() == -1000000000.0f || md_GetGroundCode(dComIfG_Bgsp(), gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) == 4) {
            // HD: the unnamed action  falls too
            if (isFallAction() || md_pmf_is(&mCurrNpcActionFunc, RETURN_TO_PLAYER_NPC_ACTION)) {
                if (m4E8 < 30) {
                    m4E8 = m4E8 + 1;
                } else if (md_pmf_is(&mCurrNpcActionFunc, RETURN_TO_PLAYER_NPC_ACTION) || !md_roomStatus10(home.roomNo)) {
                    /* current = home (struct copy), shape_angle = home.angle */
                    word(current.pos.x) = word(home.pos.x);
                    placeWords(&current)->mAngleZRoom = placeWords(&home)->mAngleZRoom;
                    shape_angle.y = home.angle.y;
                    word(current.pos.z) = word(home.pos.z);
                    word(current.pos.y) = word(home.pos.y);
                    shape_angle.x = home.angle.x;
                    placeWords(&current)->mAngleXY = placeWords(&home)->mAngleXY;
                    shape_angle.z = home.angle.z;
                    m4E8 = 0;
                    speedF = 0.0f;
                    // HD: back to the wait action
                    gabi::Local<ProcFunc_l> fn;
                    copyAction(fn, action_waitNpcAction);
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
    // HD: also animated on the Dragon Roost spring stage (M_Dra09)
    if (daPy_npc_checkNowPosMove(this, gabi::ea(l_staff_name.mpName.get())) || md_stageNameIs(DRAGON_ROOST_SPRING_NAME /* "M_Dra09" */)) {
        // HD: back to the wait animation after the tact animation there (event bit 0x1140)
        if (m3104 == 0x22 && dSv_event_isEventBit(md_event(), 0x1140) && md_stageNameIs(DRAGON_ROOST_SPRING_NAME)) {
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
                    copyAction(fn, action_carryPlayerAction);
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
                copyAction(fn, action_escapeNpcAction);
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
    u32 model = gabi::ea(MdExec::morf(mpMorf)->mpModel.get()); /* getModel() */
    emitterTrace(nativeEmitter(m0508[0]), md_getAnmMtx(model, m_neck_jnt_num), &current.angle);
    emitterTrace(nativeEmitter(m0508[1]), md_getAnmMtx(model, m_neck_jnt_num), nullptr);
    if (!isTypeShipRide()) {
        model = gabi::ea(mpWingMorf->mpModel.get());
        emitterTrace(nativeEmitter(m0508[2]), md_getAnmMtx(model, m_wingR3_jnt_num), nullptr);
        emitterTrace(nativeEmitter(m0508[3]), md_getAnmMtx(model, m_wingL3_jnt_num), nullptr);
        emitterTrace(nativeEmitter(m0508[4]), md_getAnmMtx(model, m_wingR3_jnt_num), nullptr);
        emitterTrace(nativeEmitter(m0508[5]), md_getAnmMtx(model, m_wingL3_jnt_num), nullptr);
        if (m3135 & 1) { /* checkBitEffectStatus(1) */
            model = gabi::ea(mpWingMorf->mpModel.get());
            cXyz local_3c;

            for (int k = 0; k < 2; k++) {
                Mtx34* m = md_getAnmMtx(model, k == 0 ? m_wingR2_jnt_num : m_wingL2_jnt_num);
                s8 roomNo = current.roomNo;
                            /* local_3c.set(mtx[0][3], mtx[1][3], mtx[2][3]) (through FPRs: signalling NaNs are quieted here) */
                f32 z = m->m[2][3], x = m->m[0][3], y = m->m[1][3];
                local_3c.x = x;
                local_3c.y = y;
                local_3c.z = z;
                dComIfGp_particle_set(0x819C /* ID_IT_SN_MD_HANE01 */, &local_3c, nullptr, nullptr, 0xFF, nullptr, roomNo,
                                      &tevStr.mColorK0, &tevStr.mColorK0, nullptr);
            }
        }
    }
    m3135 = 0; /* setEffectStatus(0) */
    return TRUE;
}

/* 02288AB4 */
void daNpc_Md_c::emitterTrace(JPABaseEmitter* emitter, Mtx34* mtx, csXyz* angle) {
    if (emitter == nullptr) {
        return;
    }
    /* lfs/stfs pairs: the recompiled code keeps signalling NaNs bit for bit, so the words are
     * copied as integers; the fneg below quiets them */
    u32 e = gabi::ea(emitter);

    u8 flag = MdExec::emitter(e)->mScaleFlag;
    u32 y = word(mtx->m[1][3]);
    u32 x = word(mtx->m[0][3]);
    u32 z = word(mtx->m[2][3]);
    word(MdExec::emitter(e)->mGlobalTranslation.x) = x;
    word(MdExec::emitter(e)->mGlobalTranslation.z) = z;
    word(MdExec::emitter(e)->mGlobalTranslation.y) = y;
    // HD: setGlobalTranslation negates y for emitters with a flag byte >= 7
    if (flag >= 7) {
        MdExec::emitter(e)->mGlobalTranslation.y = -MdExec::emitter(e)->mGlobalTranslation.y;
    }
    if (angle == nullptr) {
        return;
    }
    JPAGetXYZRotateMtx(angle->x, angle->y, angle->z, gabi::ea(&MdExec::emitter(e)->mGlobalRotation));
}

/* 0228A1C0 */
void daNpc_Md_c::emitterDelete(be<u32>* pEmitter) {
    if (*pEmitter == 0) {
        return;
    }
    u32 e = *pEmitter;
    emitter(e)->mFlags = emitter(e)->mFlags & ~0x40u; /* quitImmortalEmitter */
    JPA_becomeInvalidEmitter(*pEmitter);
    *pEmitter = 0;
}

/* 02289E28 */
static BOOL daNpc_Md_Execute(daNpc_Md_c* i_this) {
    return i_this->execute();
}

/* 0228A168 */
static BOOL daNpc_Md_Draw(daNpc_Md_c* i_this) {
    return i_this->draw();
}

/* 0228A16C */
static BOOL daNpc_Md_IsDelete(daNpc_Md_c*) {
    return TRUE;
}

/* 0228A174 */
static void daNpc_Md_followEcallBack_execute(daNpc_Md_followEcallBack_c* cb, JPABaseEmitter* emitter) {
    /* lfs/stfs copies keep signalling NaNs (integer copies); fneg quiets them */
    u32 e = gabi::ea(emitter);
    u32 x = word(cb->mPos.x);
    u8 flag = MdExec::emitter(e)->mScaleFlag;
    if (flag >= 7) {
        MdExec::emitter(e)->mGlobalTranslation.y = -cb->mPos.y;
    } else {
        word(MdExec::emitter(e)->mGlobalTranslation.y) = word(cb->mPos.y);
    }
    word(MdExec::emitter(e)->mGlobalTranslation.x) = x;
    word(MdExec::emitter(e)->mGlobalTranslation.z) = word(cb->mPos.z);
    JPAGetXYZRotateMtx(cb->mAngle.x, cb->mAngle.y, cb->mAngle.z, gabi::ea(&MdExec::emitter(e)->mGlobalRotation));
}

/* 0228A1B8 */
static void daNpc_Md_followEcallBack_setup(daNpc_Md_followEcallBack_c* cb, JPABaseEmitter* emitter) {
    cb->mpEmitter = emitter;
}

/* 02287720 */
void daNpc_Md_followEcallBack_c::end() {
    if (mpEmitter.get() != nullptr) {
        JPA_becomeInvalidEmitter(gabi::ea(mpEmitter.get()));
        emitter(mpEmitter.get())->mpEmitterCallBack = 0; /* setEmitterCallBackPtr(NULL) */
        mpEmitter = nullptr;
    }
}

// HD: compiler-generated and template code of this translation unit.

/* 0228A254 */
static void daNpc_Md_dt(daNpc_Md_c* i_this, s32 flags) {
    if (i_this == nullptr) {
        return;
    }
    i_this->__vtbl = MD_VTBL;
    dComIfG_resDelete(&i_this->mPhase, i_this->mModelArcName);
    if (i_this->heap.get() != nullptr) {
        mdExec_stopZelAnime(i_this->mpMorf.get()); /* mDoExt_McaMorf2::stopZelAnime */
    }
    i_this->deletePiyoPiyo();
    i_this->emitterDelete(&i_this->m0508[0]);
    i_this->deleteHane02Emitter();
    i_this->deleteHane03Emitter();
    i_this->m3058.end();
    mtxFollow_end(&i_this->m304C);
    mdExec_endRipple(&i_this->m3074); /* dPa_rippleEcallBack::end (GameCube remove()) */
    // HD: no l_HIO child to delete; a static instance pointer is cleared instead
    m_flying = 0;     /* offFlying() */
    m_mirror = 0;     /* offMirror() */
    m_playerRoom = 0; /* offPlayerRoom() */
    if (instance.mpActor.get() == i_this) {
        instance.mpActor = nullptr;
    }

    auto* packet = reinterpret_cast<MirrorPacket*>(i_this->m0B70);
    /* member destructors (inlined), last member first */
    /* m0B70 (dDlst_mirrorPacket, HD 0x3428): its dBgS_LinChk at +0x33BC (0x4104) */
    packet->mLinChk.dBgS_PolyPassChk::__vtbl = LINE_POLY_PASS_VTABLE;
    packet->mLinChk.dBgS_GrpPassChk::__vtbl = GROUP_PASS_VTABLE;
    packet->mLinChk.cBgS_PolyInfo::__vtbl = LINE_POLY_INFO_VTABLE;
    mdExec_destroyLineCheck(gabi::ea(&packet->mLinChk), 0); /* cBgS_LinChk::~cBgS_LinChk */
    mdExec_destroyColorAnm(gabi::ea(&packet->mColorAnm[1]), 0);
    mdExec_destroyColorAnm(gabi::ea(&packet->mColorAnm[0]), 0);
    mdExec_destroyTevRegAnm(gabi::ea(&packet->mTevRegAnm), 2);
    if (gabi::ea(&packet->mModelData) != 0) {
        /* two 0x254-byte elements at 0x3828 */
        mdExec_destroyModelDataArray(gabi::ea(&packet->mModelData[0].mModelData));
        packet->mModelData[0].mState = 0;
        if (packet->mModelData[0].mpBuffer != 0) {
            u32 heap = mdExec_findHeap(rootHeap.mpHeap, packet->mModelData[0].mpBuffer);
            freeHeapBlock(heap, packet->mModelData[0].mpBuffer);
            packet->mModelData[0].mBufferState = 0;
            packet->mModelData[0].mpBuffer = 0;
        }
        mdExec_destroyModelDataArray(gabi::ea(&packet->mModelData[1].mModelData));
        packet->mModelData[1].mState = 0;
        if (packet->mModelData[1].mpBuffer != 0) {
            u32 heap = mdExec_findHeap(rootHeap.mpHeap, packet->mModelData[1].mpBuffer);
            freeHeapBlock(heap, packet->mModelData[1].mpBuffer);
            packet->mModelData[1].mBufferState = 0;
            packet->mModelData[1].mpBuffer = 0;
        }
        packet->mControl = 0;
        mdExec_destroyArray(gabi::ea(&packet->mModelData), 2, 0x254, MODEL_MEMBER_DESTRUCTOR, 0, 0); /* __destroy_arr */
    }
    mdExec_destroyModel(gabi::ea(&packet->mModels[2]), 2);
    mdExec_destroyModel(gabi::ea(&packet->mModels[1]), 2);
    mdExec_destroyModel(gabi::ea(&packet->mModels[0]), 2);
    mdExec_destroyPacket(gabi::ea(packet), 0); /* J3DPacket::~J3DPacket */
    mdExec_destroyCapsule(&i_this->mCps, 2);  /* ~dCcD_Cps */
    mdExec_destroyCylinder(&i_this->mCyl3, 2); /* ~dCcD_Cyl (the matcher calls it dBgS_Acch::~dBgS_Acch) */
    mdExec_destroyCylinder(&i_this->mCyl2, 2);
    mdExec_destroyCylinder(&i_this->mCyl1, 2);
    mdExec_destroyCollisionStatus(&i_this->mStts, 2); /* ~dCcD_Stts */
    /* mLinChk (dBgS_MirLightLinChk) */
    auto* line = reinterpret_cast<dBgS_LinChk*>(i_this->mLinChk);
    line->dBgS_PolyPassChk::__vtbl = LINE_POLY_PASS_VTABLE;
    line->dBgS_GrpPassChk::__vtbl = GROUP_PASS_VTABLE;
    line->cBgS_PolyInfo::__vtbl = LINE_POLY_INFO_VTABLE;
    mdExec_destroyLineCheck(gabi::ea(line), 0);
    mdExec_destroyArray(gabi::ea(&i_this->mAcchCir), 2, 0x40, CIRCLE_MEMBER_DESTRUCTOR, 0, 0); /* __destroy_arr(mAcchCir) */
    mdExec_destroyPlayerNpc(i_this, 0); /* daPy_npc_c::~daPy_npc_c */
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* WWHD glue
ext 0259D344 bool mdExec_setAnmFNDirect(mDoExt_McaMorf* m, int loop, f32 morf, f32 speed, const char* fn, const char* snd, const char* arc)
ext 023D4688 void mdExec_changePlayer(fopAc_ac_c* self, fopAc_ac_c* player)
ext 02007840 BOOL mdExec_padTrigRorStart(s32 port)
ext 028245AC void mdExec_getXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx)
ext 02606900 void* mdExec_getObjectRes(dRes_control_c* control, SafeString* arc, SafeString* name)
ext 025E5D54 void mdExec_setMorfAnm(mDoExt_McaMorf2* m, void* a1, void* a2, f32 blend, s32 loop, f32 morf, f32 speed, f32 start, f32 end, void* snd)
ext 0207A9A0 u8 mdExec_calcTimerU8(be<u8>* timer)
ext 025E72D4 void mdExec_initAnmPlay(void* anm, s16 frameMax, s32 mode, f32 speed, s16 start, s16 end, bool modify)
ext 0253EC0C void mdExec_orderEvent(dEvt_control_c* event, u16 type, u16 prio, u16 flag, u16 hind, fopAc_ac_c* a1, fopAc_ac_c* a2, s16 eventId, u8 infoIdx)
ext 0201B3C0 void mdExec_normalizeZP(cXyz* src, cXyz* out)
ext 0259DED0 void mdExec_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 025DF2B8 s16 mdExec_getPriority(void* actor)
ext 025DA874 void mdExec_toDrawQueue(u32 tag, s16 priority)
ext 02055B64 s16 mdExec_calcTimerS16(be<s16>* timer)
ext 025D7DEC fpc_ProcID mdExec_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale)
ext 0252012C void mdExec_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe)
ext 025F8088 u32 mdExec_setTactMessage(u32 manager)
ext 025F7DB0 u32 mdExec_setMessage(u32 manager, u32 message, cXyz* eye)
ext 02542E94 u32 mdExec_getRunEventName(dEvent_manager_c* manager)
ext 02008254 BOOL mdExec_checkPolySafe(dBgS* bgs, u32 poly)
ext 025E65FC u8 mdExec_playMorf(mDoExt_McaMorf2* m, cXyz* pos, u32 sound, s32 reverb)
ext 027F2BF8 BOOL mdExec_checkFrame(u32 control, f32 frame)
ext 025445B8 BOOL mdExec_startCheckOld(dEvent_manager_c* manager, u32 name)
ext 0254457C BOOL mdExec_endCheckOld(dEvent_manager_c* manager, u32 name)
ext 021E1E78 s32 mdExec_getRndValue(s32 base, s32 range)
ext 025E1A7C void mdExec_monsSeStart(u32 sound, cXyz* pos, u32 process, s32 reverb)
ext 0252A038 BOOL mdExec_checkLight(dDetect_c* detect, cXyz* pos)
ext 0201794C void mdExec_lineVsPosSuisenCross(u32 start, u32 end, u32 pos, cXyz* out)
ext 023D457C void mdExec_makeEmitter(daPy_mtxFollowEcallBack_l* cb, u16 id, Mtx34* mtx, cXyz* pos, cXyz* scale)
ext 023D4538 void mdExec_endMtxFollow(daPy_mtxFollowEcallBack_l* cb)
ext 025F19F8 void mdExec_XYZrotM(Mtx34* mtx, s16 x, s16 y, s16 z)
ext 02018808 void mdExec_setStartEnd(u32 line, cXyz* start, cXyz* end)
ext 02516C14 void mdExec_setMass(u8* mass, void* obj, u8 param)
ext 0251641C void mdExec_clearCoHit(dCcD_Cyl* cyl)
ext 02516138 void mdExec_resetAtHit(dCcD_Cps* capsule)
ext 02542EDC s32 mdExec_getMyActIdx(dEvent_manager_c* manager, s32 staffId, u32 table, s32 count, s32 force, s32 nameType)
ext 0253F124 u32 mdExec_getPartnerId(u32 event, fopAc_ac_c* actor)
ext 025E1988 void mdExec_startControlSound(s32 sound)
ext 025E6794 void mdExec_stopZelAnime(mDoExt_McaMorf2* m)
ext 025A9270 void mdExec_endRipple(dPa_rippleEcallBack_md* cb)
ext 02008B4C void mdExec_destroyLineCheck(u32 check, s32 flags)
ext 027FB528 void mdExec_destroyColorAnm(u32 anm, s32 flags)
ext 027FD764 void mdExec_destroyTevRegAnm(u32 anm, s32 flags)
ext 027BF7E8 void mdExec_destroyModelDataArray(u32 data)
ext 02755FEC u32 mdExec_findHeap(u32 heap, u32 block)
ext 028F0164 void mdExec_destroyArray(u32 array, s32 count, s32 stride, u32 destructor, s32 reserved1, s32 reserved2)
ext 027BE2B0 void mdExec_destroyModel(u32 model, s32 flags)
ext 027F13DC void mdExec_destroyPacket(u32 packet, s32 flags)
ext 02515980 void mdExec_destroyCapsule(dCcD_Cps* capsule, s32 flags)
ext 02515A70 void mdExec_destroyCylinder(dCcD_Cyl* cylinder, s32 flags)
ext 02515860 void mdExec_destroyCollisionStatus(dCcD_Stts* status, s32 flags)
ext 0244513C void mdExec_destroyPlayerNpc(daNpc_Md_c* actor, s32 flags)
ext 025E7B3C void mdExec_entryBtp(void* anm, J3DModelData* data, s16 frame)
ext 025E66D4 void mdExec_entryMorfDL(mDoExt_McaMorf2* m)
ext 02445A00 void mdExec_drawDamageFog(void* actor)
ext 028E9108 void mdExec_concatMtx(const Mtx34* a, const Mtx34* b, Mtx34* out)
ext 0252DA64 void mdExec_updateMirror(u32 packet, Mtx34* mtx, u8 alpha, f32 angle)
ext 027F0E04 void mdExec_entryImmediate(u32 list, u32 packet, u16 index)
ext 024451B4 BOOL mdExec_checkInitialRoom(void* actor)
ext 0244586C BOOL mdExec_initialRestartOption(void* actor, s8 option, BOOL isDungeon)
ext 024452A0 BOOL mdExec_checkMoveStop(void* actor)
ext 02445950 BOOL mdExec_checkNowPosMove(void* actor, u32 name)
ext 02445AA4 s8 mdExec_checkMoveBlock(void* actor, cXyz* pos)
ext 023D4890 void mdExec_objWindHitCheck(void* actor, dCcD_Cyl* cylinder)
ext 024EEABC BOOL mdExec_checkMoveBg(dBgS* bgs, u32 poly)
ext 024EF968 void mdExec_moveBgCrrPos(dBgS* bgs, u32 poly, bool moving, cXyz* pos, csXyz* angle1, csXyz* angle2)
ext 024EF130 s8 mdExec_getRoomId(dBgS* bgs, u32 poly)
ext 024EEEB8 u8 mdExec_getPolyColor(dBgS* bgs, u32 poly)
ext 024EF0BC s32 mdExec_getGroundCode(dBgS* bgs, u32 poly)
ext 025A8BFC void mdExec_setSimpleLand(dPa_control_c* particle, u32 poly, cXyz* pos, csXyz* angle, f32 a, f32 b, f32 c, dKy_tevstr_c* tev, s32* out, s32 count)
ext 023FD4E4 void mdExec_startRestartRoom(fopAc_ac_c* link, u32 mode, s32 index, f32 speed, s32 option)

data m_playerRoom 101D5F41
data m_mirror 101D5F3F
data m_flying 101D5F3E
data l_msgId 10467A34
data saveInfo 101F84DC
data messageManager 101F4B5C
data instance 101CEF74
data rootHeap 101F8B4C
data roomStatus 1047E8E8
data l_staff_name 101C0280
data cut_name_tbl 101C0594
data event_init_tbl 101C0434
data event_action_tbl 101C04E4
data anmTypeData_Talk 1001DD40
data eventModes 1001DCF0
data l_anmPrm 101C11C4
data l_anmTbl 101C0804
data armAnmTbl 101C14E4
data wingAnmTbl 101C1CE4
data event_name_tbl 101C0284
data l_harp_play_se 1001DCAC
data l_ms_light_local_start 10467A60
data l_ms_light_local_vec 10467A54
data action_mwaitNpcAction 1001D488
data action_waitNpcAction 1001D490
data action_waitPlayerAction 1001D498
data action_hitPlayerAction 1001D4D8
data action_hitNpcAction 1001D4E0
data action_carryNpcAction 1001D4E8
data action_talkNpcAction 1001D570
data action_shipTalkNpcAction 1001D578
data action_carryPlayerAction 1001D588
data action_escapeNpcAction 1001D590

str 1001DC88 ""
str 1001DD28 "Disp"
str 1001DD20 "Timer"
str 1001DD34 "sea"
str 1001DD38 "MsgNo"
*/
