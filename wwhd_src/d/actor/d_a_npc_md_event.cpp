/**
 * d_a_npc_md_event.cpp (WWHD)
 * Player - Medli: event cuts, messages, movement helpers (0228AB60..0228EB87)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline void* dComIfGp_evmng_getMyVec3dP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 1); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyStringP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 4); }
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(int start, int range) (out of line copy) */
static inline s32 cLib_getRndValue(s32 start, s32 range) { return gabi::call<s32>(0x021E1E78, start, range); }
/* 025E1A7C mDoAud_monsSeStart(id, pos, procId, reverb): HD fopAcM_monsSeStart with param 0 */
static inline void mDoAud_monsSeStart4(u32 id, cXyz* pos, u32 pid, s32 reverb) { gabi::call(0x025E1A7C, id, pos, pid, reverb); }
/* 0244307C daPy_lk_c::checkEndTactMusic */
static inline bool daPy_lk_checkEndTactMusic(fopAc_ac_c* pl) { return gabi::call<bool>(0x0244307C, pl); }
/* 028F040C strcpy */
static inline void strcpy_g(u32 dst, u32 src) { gabi::call(0x028F040C, dst, src); }
/* daPy_getPlayerLinkActorClass(): play+0x5B34 */
static inline fopAc_ac_c* daPy_getPlayerLinkActorClass() { return dComIfGp_getLinkPlayer(); }
/* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: mNoResetFlg0 (HD +0x3B8) bit 0x08000000 */
/* CPad (port 0): stick angle, A/B trigger */
static inline s16 CPad_GET_STICK_ANGLE(s32 port) { return gabi::call<s16>(0x02007A1C, port); }
static inline bool CPad_CHECK_TRIG_A(s32 port) { return gabi::call<bool>(0x02007898, port); }
static inline BOOL CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
/* 024F8018 dCam_getControledAngleY(camera_class*); camera 0 at play+0x5AF8 */
static inline s16 dCam_getControledAngleY(u32 cam) { return gabi::call<s16>(0x024F8018, cam); }
/* dAttention_c (play+0x5804) */
static inline u32 dAttention_GetLockonList(dAttention_c* a, s32 i) { return gabi::call<u32>(0x024EE058, a, i); }
static inline u32 dAttention_GetActionList(dAttention_c* a, s32 i) { return gabi::call<u32>(0x024EE020, a, i); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttList_getActor(u32 l) { return gabi::call<fopAc_ac_c*>(0x024EBA14, l); }
/* 0200ECD4 cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 024EF09C dBgS::GetSpecialCode(cBgS_PolyInfo&) */
static inline s32 dBgS_GetSpecialCode(dBgS* bgs, u32 poly) { return gabi::call<s32>(0x024EF09C, bgs, poly); }
/* 02544950 dEvent_manager_c::ChkPresentEnd */
static inline bool dComIfGp_evmng_ChkPresentEnd() { return gabi::call<bool>(0x02544950, dComIfGp_getPEvtManager()); }
/* dComIfGs_isEventBit: dSv_event_c at save info + 0x624 */
static inline BOOL dComIfGs_isEventBit(u16 flag) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag); }
/* HD messages: the manager *(0x101F4B5C); 025F795C its current status, 025F74D0 sets it,
 * 025F7DB0 messageSet(mng, msgNo, pos), messageSendOn: mng+0x921 = 1 */
static inline u32 l_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 fopMsgM_getStatus(u32 mng) { return gabi::call<u16>(0x025F795C, mng); }
static inline void fopMsgM_setStatus(u32 mng, u32 st) { gabi::call(0x025F74D0, mng, st); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
static inline void fopMsgM_messageSendOn(u32 mng) { gabi::store<u8>(mng + 0x921, 1); }
/* dComIfGp_getMesgAnimeAttrInfo: play+0x5BC5; dComIfGp_checkMesgCancelButton: play+0x5BD3 */
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) */
static inline s16 fpcLf_GetPriority(void* a) { return gabi::call<s16>(0x025DF2B8, a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { gabi::call(0x025DA874, tag, prio); }
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s16 prio = fpcLf_GetPriority(a);
    fopDwTg_ToDrawQ(gabi::ea(a) + 0xDC, prio);
}
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
static inline void dComIfGs_onEventBit(u16 flag) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag); }
/* fopAcM_checkCarryNow: actor_status & 0x2000 */
static inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & 0x2000) != 0; }
#define MD_M_SEATALK 0x101D5F40 /* bool daNpc_Md_c::m_seaTalk */
/* inline strcmp(a, b) == 0 (GHS inlines it as a byte loop or a word loop; the result is the same) */
static inline bool md_streq(u32 a, u32 b) {
    u8 ca, cb;
    do {
        ca = gabi::load<u8>(a++);
        cb = gabi::load<u8>(b++);
    } while (ca == cb && ca != 0);
    return ca == cb;
}
/* dVibration_c (play+0x599C): StartQuake(int, int, cXyz) (cXyz by value: pointer to a copy), StopQuake(int) */
static inline BOOL dComIfGp_getVibration_StartQuake(s32 a, s32 b, cXyz* pos) { return gabi::call<BOOL>(0x025CB408, dComIfGp_getVibration(), a, b, pos); }
static inline BOOL dComIfGp_getVibration_StopQuake(s32 a) { return gabi::call<BOOL>(0x025CB610, dComIfGp_getVibration(), a); }
/* dEvent_manager_c::getMyActName(staffId) */
static inline u32 dComIfGp_evmng_getMyActName(s32 staffId) { return gabi::call<u32>(0x025447EC, dComIfGp_getPEvtManager(), staffId); }
/* dComIfGp_event_runCheck(): play+0x5292 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, bool headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* chkNpcAction(func) with a constant {0, -1, func} (GHS pointer-to-member compare) */
static inline bool md_chkAction(ProcFunc_l* p, u32 f) {
    s16 i = p->i;
    if (i != -1)
        return false;
    if (i == 0)
        return true;
    return p->d == 0 && p->f == f;
}

/* 0228B3CC */
void daNpc_Md_c::setHarpPlayNum(int) {
    WWHD_FUNC(0x0228B3CC, void, this, 0);
    s32 i = cLib_getRndValue(0, 4);
    setAnm(gabi::load<s32>(0x1001DDF0 + i * 4)); /* l_harp_play_anm */
}
VERIFY(0x0228B3CC, &daNpc_Md_c::setHarpPlayNum);

/* 0228B610 */
BOOL daNpc_Md_c::actionMsgEndEvent(int staffIdx) {
    WWHD_FUNC(0x0228B610, BOOL, this, staffIdx);
    BOOL ret = talk(0);
    if (ret) {
        initialEndEvent(staffIdx);
    }
    lookBack(1, 0, 0);
    setAttention(true);
    return ret;
}
VERIFY(0x0228B610, &daNpc_Md_c::actionMsgEndEvent);

/* 0228C104 */
void daNpc_Md_c::particle_set(be<u32>* pEmitter, u16 particleID) {
    WWHD_FUNC(0x0228C104, void, this, pEmitter, particleID);
    if (*pEmitter != 0) {
        return;
    }
    s8 roomNo = fopAcM_GetRoomNo(this);
    GXColor* k0 = gabi::at<GXColor>(gabi::ea(this) + 0x1A8); /* tevStr.mColorK0 */
    JPABaseEmitter* e = dComIfGp_particle_set(particleID, &current.pos, &current.angle, nullptr, 0xFF, nullptr, roomNo, k0, k0);
    *pEmitter = gabi::ea(e);
    if (e != nullptr) {
        u32 p = gabi::ea(e) + 0x254; /* becomeImmortalEmitter: status |= 0x40 */
        gabi::store<u32>(p, gabi::load<u32>(p) | 0x40);
    }
}
VERIFY(0x0228C104, &daNpc_Md_c::particle_set);

/* 0228C1B4 */
void daNpc_Md_c::setWingEmitter() {
    WWHD_FUNC(0x0228C1B4, void, this);
    particle_set(&m0508[0], 0x819B);
}
VERIFY(0x0228C1B4, &daNpc_Md_c::setWingEmitter);

/* 0228C1C4 */
void daNpc_Md_c::initialFlyEvent(int) {
    WWHD_FUNC(0x0228C1C4, void, this, 0);
    setAnm(0x19);
    setBitStatus(daMdStts_UNK1);
    setWingEmitter();
    speed.y = 20.0f;
    gravity = l_HIO().m188;
    maxFallSpeed = 10.0f;
    m3118 = 60 * 30;
}
VERIFY(0x0228C1C4, &daNpc_Md_c::initialFlyEvent);

/* 0228C234 */
BOOL daNpc_Md_c::actionFlyEvent(int staffIdx) {
    WWHD_FUNC(0x0228C234, BOOL, this, staffIdx);
    be<f32>* p = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE94) /* "HIGH" */);
    f32 high = 0.0f;
    if (p != nullptr) {
        high = *p;
    }
    if (current.pos.y > high) {
        emitterDelete(&m0508[0]);
        return TRUE;
    }
    if (m3104 == 0x19) {
        if (m312A != 0) {
            setAnm(9);
        }
    }
    setAttention(true);
    if (mAcch.ChkRoofHit()) {
        return TRUE;
    }
    return cLib_calcTimer(&m3118) == 0 ? TRUE : FALSE;
}
VERIFY(0x0228C234, &daNpc_Md_c::actionFlyEvent);

/* 0228C338 */
void daNpc_Md_c::initialGlidingEvent(int) {
    WWHD_FUNC(0x0228C338, void, this, 0);
    gravity = l_HIO().m0F4;
    maxFallSpeed = -100.0f;
}
VERIFY(0x0228C338, &daNpc_Md_c::initialGlidingEvent);

/* 0228C354 */
BOOL daNpc_Md_c::actionGlidingEvent(int) {
    WWHD_FUNC(0x0228C354, BOOL, this, 0);
    if (mAcch.ChkGroundHit()) {
        /* fopAcM_monsSeStart(this, JA_SE_CV_MD_LANDING, &current.pos, reverb) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_monsSeStart4(0x48A7, &current.pos, fopAcM_GetID(this), reverb);
        return TRUE;
    }
    if (!checkStatus(daMdStts_CAM_TAG_IN)) {
        speedF = 0.0f;
    }
    setAttention(true);
    return FALSE;
}
VERIFY(0x0228C354, &daNpc_Md_c::actionGlidingEvent);

/* 0228C3E8 */
void daNpc_Md_c::initialLandingEvent(int) {
    WWHD_FUNC(0x0228C3E8, void, this, 0);
    maxFallSpeed = -100.0f;
    speedF = 0.0f;
    shape_angle.x = 0;
    shape_angle.z = 0;
    clearStatus(daMdStts_UNK1);
    setAnm(0x10);
    m312A = 0;
}
VERIFY(0x0228C3E8, &daNpc_Md_c::initialLandingEvent);

/* 0228C454 */
BOOL daNpc_Md_c::actionLandingEvent(int) {
    WWHD_FUNC(0x0228C454, BOOL, this, 0);
    if (m312A != 0) {
        setAnm(0);
        return TRUE;
    }
    setAttention(true);
    return FALSE;
}
VERIFY(0x0228C454, &daNpc_Md_c::actionLandingEvent);

/* 0228C4A4 initialWalkEvent (not named by the matcher) */
void daNpc_Md_c::initialWalkEvent(int) {
    WWHD_FUNC(0x0228C4A4, void, this, 0);
    /* HD: also sets the wall circle (GameCube: speedF = 0, m3118 = 20 s only) */
    m3118 = 20 * 30;
    speedF = 0.0f;
    mAcchCir[1].SetWall(60.0f, 20.0f);
}
VERIFY(0x0228C4A4, &daNpc_Md_c::initialWalkEvent);

/* 0228C4D0 */
int daNpc_Md_c::wallHitCheck() {
    WWHD_FUNC(0x0228C4D0, int, this);
    if (mAcch.ChkWallHit()) {
        for (int i = 0; i < 2; i++) {
            if (mAcchCir[i].ChkWallHit()) {
                return i;
            }
        }
    }
    return -1;
}
VERIFY(0x0228C4D0, &daNpc_Md_c::wallHitCheck);

/* 0228CCC0 */
void daNpc_Md_c::initialTakeOffEvent(int) {
    WWHD_FUNC(0x0228CCC0, void, this, 0);
    setAnm(0x18);
    m312A = 0;
}
VERIFY(0x0228CCC0, &daNpc_Md_c::initialTakeOffEvent);

/* 0228CCF8 */
BOOL daNpc_Md_c::actionTakeOffEvent(int) {
    WWHD_FUNC(0x0228CCF8, BOOL, this, 0);
    if (m312A != 0) {
        return TRUE;
    }
    setAttention(true);
    return FALSE;
}
VERIFY(0x0228CCF8, &daNpc_Md_c::actionTakeOffEvent);

/* 0228CD40 */
void daNpc_Md_c::initialOnetimeEvent(int staffIdx) {
    WWHD_FUNC(0x0228CD40, void, this, staffIdx);
    /* char acStack_18[16] at sp+8 of the 0x20 frame (placed as GHS does: a name longer than 15
     * characters overflows strcpy into the same bytes) */
    gabi::Local<u8[0x20]> frame;
    u32 buf = gabi::ea(frame.get()) + 8;
    s32 anm = 0x1A;
    u32 name = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DEC4) /* "Name" */));
    if (name != 0) {
        strcpy_g(buf, name);
        /* strcmp(buf, cut_anm_tbl[0]) (inline byte loop) */
        u32 a = buf;
        u32 b = gabi::load<u32>(0x101C1EE8);
        u8 ca, cb;
        do {
            ca = gabi::load<u8>(a++);
            cb = gabi::load<u8>(b++);
        } while (ca == cb && ca != 0);
        if (ca == cb) {
            anm = gabi::load<s32>(0x1001DEC0); /* cut_anm_idx_tbl[0] */
        }
    }
    setAnm(anm);
    m312A = 0;
}
VERIFY(0x0228CD40, &daNpc_Md_c::initialOnetimeEvent);

/* 0228CDFC */
BOOL daNpc_Md_c::actionOnetimeEvent(int) {
    WWHD_FUNC(0x0228CDFC, BOOL, this, 0);
    if (m312A != 0) {
        if (m3104 == 0x1A) {
            setAnm(0);
        } else {
            return TRUE;
        }
    }
    setAttention(true);
    return FALSE;
}
VERIFY(0x0228CDFC, &daNpc_Md_c::actionOnetimeEvent);

/* 0228D0A4 initialHarpPlayEvent (not named by the matcher) */
void daNpc_Md_c::initialHarpPlayEvent(int) {
    WWHD_FUNC(0x0228D0A4, void, this, 0);
    setHarpPlayNum(0);
}
VERIFY(0x0228D0A4, &daNpc_Md_c::initialHarpPlayEvent);

/* 0228D0AC */
BOOL daNpc_Md_c::actionHarpPlayEvent(int) {
    WWHD_FUNC(0x0228D0AC, BOOL, this, 0);
    if (m312A != 0) {
        setHarpPlayNum(0);
    }
    return daPy_lk_checkEndTactMusic(daPy_getPlayerLinkActorClass()) ? TRUE : FALSE;
}
VERIFY(0x0228D0AC, &daNpc_Md_c::actionHarpPlayEvent);

/* 0228D0F0 */
void daNpc_Md_c::initialOffLinkEvent(int) {
    WWHD_FUNC(0x0228D0F0, void, this, 0);
    u32 p = gabi::ea(daPy_getPlayerLinkActorClass()) + 0x3B8; /* onPlayerNoDraw: mNoResetFlg0 */
    gabi::store<u32>(p, gabi::load<u32>(p) | 0x08000000);
}
VERIFY(0x0228D0F0, &daNpc_Md_c::initialOffLinkEvent);

/* 0228D120 */
void daNpc_Md_c::initialOnLinkEvent(int) {
    WWHD_FUNC(0x0228D120, void, this, 0);
    u32 p = gabi::ea(daPy_getPlayerLinkActorClass()) + 0x3B8; /* offPlayerNoDraw */
    gabi::store<u32>(p, gabi::load<u32>(p) & ~0x08000000u);
}
VERIFY(0x0228D120, &daNpc_Md_c::initialOnLinkEvent);

/* 0228D150 */
void daNpc_Md_c::initialTurnEvent(int staffIdx) {
    WWHD_FUNC(0x0228D150, void, this, staffIdx);
    be<s32>* t = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001DEF0) /* "Timer" */);
    if (t != nullptr) {
        m3118 = (s16)*t;
    } else {
        m3118 = 0;
    }
    cXyz* pos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, STR(0x1001DEEC) /* "Pos" */);
    if (pos != nullptr) {
        m30B8.x = pos->x;
        m30B8.y = pos->y;
        m30B8.z = pos->z;
    } else {
        f32 z = 0.0f;
        m30B8.y = z;
        m30B8.x = z;
        m30B8.z = z;
    }
}
VERIFY(0x0228D150, &daNpc_Md_c::initialTurnEvent);

/* 0228D840 initialLookDown (not named by the matcher) */
u32 daNpc_Md_c::initialLookDown(int) {
    WWHD_FUNC(0x0228D840, u32, this, 0);
    return setAnm(0x2F);
}
VERIFY(0x0228D840, &daNpc_Md_c::initialLookDown);

/* 0228D948 initialLookUp (not named by the matcher) */
u32 daNpc_Md_c::initialLookUp(int) {
    WWHD_FUNC(0x0228D948, u32, this, 0);
    return setAnm(5);
}
VERIFY(0x0228D948, &daNpc_Md_c::initialLookUp);

/* 0228D950 isTagCheckOK (virtual; not named by the matcher) */
BOOL daNpc_Md_c::isTagCheckOK() {
    WWHD_FUNC(0x0228D950, BOOL, this);
    BOOL ret = FALSE;
    if (md_chkAction(&mCurrNpcActionFunc, 0x0228EB88 /* waitNpcAction */) ||
        md_chkAction(&mCurrNpcActionFunc, 0x02292C48 /* searchNpcAction */)) {
        ret = TRUE;
    } else if (md_chkAction(&mCurrNpcActionFunc, 0x022904DC /* carryNpcAction */)) {
        /* !daPy_lk_c::checkCarryActionNow(): mCurProc (HD +0x65F0) not 0x72/0x6F/0x71 */
        s32 proc = gabi::load<s32>(gabi::ea(daPy_getPlayerLinkActorClass()) + 0x65F0);
        if (proc != 0x72 && proc != 0x6F && proc != 0x71) {
            ret = TRUE;
        }
    }
    return ret;
}
VERIFY(0x0228D950, &daNpc_Md_c::isTagCheckOK);

/* 0228E2D4 */
void daNpc_Md_c::setHane02Emitter() {
    WWHD_FUNC(0x0228E2D4, void, this);
    particle_set(&m0508[2], 0x8217);
    particle_set(&m0508[3], 0x8217);
}
VERIFY(0x0228E2D4, &daNpc_Md_c::setHane02Emitter);

/* 0228E328 */
void daNpc_Md_c::setHane03Emitter() {
    WWHD_FUNC(0x0228E328, void, this);
    particle_set(&m0508[4], 0x827D);
    particle_set(&m0508[5], 0x827D);
}
VERIFY(0x0228E328, &daNpc_Md_c::setHane03Emitter);

/* 0228E37C */
void daNpc_Md_c::setNormalSpeedF(f32 f1, f32 f2, f32 i_scale, f32 i_maxStep, f32 i_minStep) {
    WWHD_FUNC(0x0228E37C, void, this, f1, f2, i_scale, i_maxStep, i_minStep);
    f32 f31 = mMaxNormalSpeed * f1;
    int r3 = wallHitCheck();
    if (r3 >= 0) {
        s16 angle = (s16)(current.angle.y + 0x8000 - mAcchCir[r3].GetWallAngleY());
        if (abs((int)angle) < 0x4000) {
            f31 *= gabi::fnmsubs(l_HIO().m008.m1C, cM_scos(angle), 1.0f); /* 1 - m1C * cos */
        }
    }
    f32 targetSpeed;
    f32 maxStep;
    if (f31 < m3108) {
        f32 temp2 = m3108 - f31;
        if (temp2 > i_maxStep) {
            maxStep = i_maxStep;
        } else {
            maxStep = temp2;
        }
        if (maxStep < i_minStep) {
            maxStep = i_minStep;
        }
        f2 = 0.0f;
        targetSpeed = f31;
    } else {
        maxStep = i_maxStep;
        targetSpeed = 0.0f;
    }
    if (!(std::fabs(f2) < 3.814697265625e-06f)) { /* !cM3d_IsZero(f2) */
        f32 v = m3108 + f2;
        if (v > f31) {
            m3108 = f31;
        } else {
            m3108 = v;
        }
    } else {
        cLib_addCalc(&m3108, targetSpeed, i_scale, maxStep, i_minStep);
    }
}
VERIFY(0x0228E37C, &daNpc_Md_c::setNormalSpeedF);

/* 0228E4C0 */
void daNpc_Md_c::setSpeedAndAngleNormal(f32 f1, s16 r4) {
    WWHD_FUNC(0x0228E4C0, void, this, f1, r4);
    f32 f2;
    if (f1 > 0.05f) {
        cLib_addCalcAngleS(&current.angle.y, r4, l_HIO().m008.m24, l_HIO().m008.m20, l_HIO().m008.m22);
        f32 temp = cM_scos((s16)(r4 - current.angle.y));
        temp = (temp >= 0.0f) ? temp : 0.0f; /* if (temp < 0) temp = 0 (fsel) */
        f2 = l_HIO().m008.m04 * f1 * temp;
    } else {
        f2 = 0.0f;
    }
    setNormalSpeedF(f1, f2, l_HIO().m008.m14, l_HIO().m008.m0C, l_HIO().m008.m10);
}
VERIFY(0x0228E4C0, &daNpc_Md_c::setSpeedAndAngleNormal);

/* 0228E5CC */
void daNpc_Md_c::walkProc(f32 f1, s16 r3) {
    WWHD_FUNC(0x0228E5CC, void, this, f1, r3);
    mMaxNormalSpeed = l_HIO().m008.m08;
    setSpeedAndAngleNormal(f1, r3);
    f32 temp = std::fabs(m3108 / mMaxNormalSpeed);
    f32 temp2 = temp * l_HIO().m008.m18;
    gabi::store<f32>(gabi::ea(mpMorf.get()) + 0xA4, temp2);    /* mpMorf->setPlaySpeed */
    gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xA4, temp2); /* mpArmMorf->setPlaySpeed */
    /* setRunRate(temp) */
    u32 morf = gabi::ea(mpMorf.get());
    mRunRate = temp;
    gabi::store<f32>(morf + 0xC0, temp); /* setAnmRate */
    gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xC0, mRunRate);
    speedF = m3108;
}
VERIFY(0x0228E5CC, &daNpc_Md_c::walkProc);

/* 0228E658 */
s16 daNpc_Md_c::getStickAngY(int param_1) {
    WWHD_FUNC(0x0228E658, s16, this, param_1);
    if (param_1) {
        return (s16)(CPad_GET_STICK_ANGLE(0) + 0x8000);
    }
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
    s32 stick = CPad_GET_STICK_ANGLE(0) + 0x8000;
    return (s16)(stick + dCam_getControledAngleY(cam));
}
VERIFY(0x0228E658, &daNpc_Md_c::getStickAngY);

/* 0228E6CC */
int daNpc_Md_c::calcStickPos(s16 param_1, cXyz* param_2) {
    WWHD_FUNC(0x0228E6CC, int, this, param_1, param_2);
    dAttention_c* attention = dComIfGp_getAttention();
    u32 attList = dAttention_GetLockonList(attention, 0);
    /* dAttention_c::Lockon(): LockonTruth() || chkFlag(0x20000000) */
    bool r26 = true;
    if (!dAttention_LockonTruth(attention) && !(gabi::load<u32>(gabi::ea(attention) + 0x20) & 0x20000000)) {
        r26 = false;
    }
    int r31 = !r26 ? 0 : dAttention_LockonTruth(attention) ? 1 : -1;
    if (attList == 0) {
        attList = dAttention_GetActionList(attention, 0);
    }
    if (attList != 0) {
        fopAc_ac_c* ac = dAttList_getActor(attList);
        gabi::store<u32>(gabi::ea(param_2), gabi::load<u32>(gabi::ea(&ac->eyePos)));
        gabi::store<u32>(gabi::ea(param_2) + 4, gabi::load<u32>(gabi::ea(&ac->eyePos) + 4));
        gabi::store<u32>(gabi::ea(param_2) + 8, gabi::load<u32>(gabi::ea(&ac->eyePos) + 8));
        return r31;
    }
    if (r26) {
        param_1 = shape_angle.y;
    }
    param_2->y = current.pos.y;
    param_2->x = gabi::fmadds(100.0f, cM_ssin(param_1), current.pos.x);
    param_2->z = gabi::fmadds(100.0f, cM_scos(param_1), current.pos.z);
    return r31;
}
VERIFY(0x0228E6CC, &daNpc_Md_c::calcStickPos);

/* 0228E804 */
BOOL daNpc_Md_c::flyCheck() {
    WWHD_FUNC(0x0228E804, BOOL, this);
    if (!CPad_CHECK_TRIG_A(0)) {
        return FALSE;
    }
    gabi::Local<ProcFunc_l> fn;
    if (checkStatus(daMdStts_LIGHT_BODY_HIT)) {
        md_pmf_load(fn, PMF_mkamaePlayerAction);
    } else {
        md_pmf_load(fn, PMF_flyPlayerAction);
    }
    setPlayerAction(fn, nullptr);
    return TRUE;
}
VERIFY(0x0228E804, &daNpc_Md_c::flyCheck);

/* 0228E8AC */
BOOL daNpc_Md_c::mirrorCancelCheck() {
    WWHD_FUNC(0x0228E8AC, BOOL, this);
    return CPad_CHECK_TRIG_B(0) ? TRUE : FALSE;
}
VERIFY(0x0228E8AC, &daNpc_Md_c::mirrorCancelCheck);

/* 0228E8D8 */
void daNpc_Md_c::NpcCall(be<s32>* r31) {
    WWHD_FUNC(0x0228E8D8, void, this, r31);
    if (!dComIfGs_isEventBit(0x1620)) {
        return;
    }
    f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
    if (!(mNoResetFlg1 & 2)) { /* !checkNpcCallCommand() */
        if (dist_sq < l_HIO().m0C8 * l_HIO().m0C8) {
            u32 p = gabi::ea(daPy_getPlayerLinkActorClass()) + 0x3BC; /* onNpcCall */
            gabi::store<u32>(p, gabi::load<u32>(p) | 2);
            *r31 = 1;
        }
    } else {
        f32 temp = l_HIO().m0C4 + l_HIO().m0C4;
        if (!(dist_sq < temp * temp)) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, PMF_searchNpcAction);
            setNpcAction(fn, nullptr);
        }
        *r31 = 1;
    }
}
VERIFY(0x0228E8D8, &daNpc_Md_c::NpcCall);

/* 0228E9C0 */
void daNpc_Md_c::waitGroundCheck() {
    WWHD_FUNC(0x0228E9C0, void, this);
    if (mAcch.ChkGroundHit()) {
        if (mType != 2) { /* !isTypeM_Dra09() */
            /* GetSpecialCode(mAcch.m_gnd): its cBgS_PolyInfo at mAcch+0xE8 */
            if (dBgS_GetSpecialCode(dComIfG_Bgsp(), gabi::ea(this) + 0x524) == 1) {
                cLib_addCalcAngleS(&shape_angle.y, cM_atan2s(m32A4.x, m32A4.z), 8, 0x2000, 0x400);
                f32 s = speedF + 0.5f;
                speedF = s;
                f32 lim = l_HIO().m008.m08;
                speedF = (s - lim >= 0.0f) ? lim : s; /* cLib_maxLimit (fsel) */
            } else {
                speedF = 0.0f;
            }
        }
    } else {
        if (dComIfGp_getPlayer(0) != this) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, PMF_fallNpcAction);
            setNpcAction(fn, nullptr);
        }
    }
}
VERIFY(0x0228E9C0, &daNpc_Md_c::waitGroundCheck);

/* 0228EACC */
BOOL daNpc_Md_c::chkAdanmaeDemoOrder() {
    WWHD_FUNC(0x0228EACC, BOOL, this);
    if (mType == 1) { /* isTypeAdanmae() */
        f32 groundH = mAcch.GetGroundH();
        /* cLib_checkMinMaxLimit(groundH, 600, 700), as the branches test it */
        if (checkStatus(daMdStts_CAM_TAG_IN) && !(groundH < 600.0f) && !(groundH > 700.0f)) {
            mCurEventMode = 6;
            gravity = l_HIO().m0F4;
            maxFallSpeed = -100.0f;
            if (mAcch.ChkGroundHit()) {
                speedF = 0.0f;
            }
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0228EACC, &daNpc_Md_c::chkAdanmaeDemoOrder);

/* 0228EB50 */
BOOL daNpc_Md_c::XYTalkCheck() {
    WWHD_FUNC(0x0228EB50, BOOL, this);
    BOOL ret = TRUE;
    if (checkStatus(daMdStts_XY_TALK)) {
        ret = dComIfGp_evmng_ChkPresentEnd();
    }
    return ret;
}
VERIFY(0x0228EB50, &daNpc_Md_c::XYTalkCheck);

/* 0228DD60 */
bool daNpc_Md_c::chkArea(cXyz* param_1) {
    WWHD_FUNC(0x0228DD60, bool, this, param_1);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = l_HIO().mNpc.mMaxAttnDistXZ;
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    maxDist += 40.0f;
    return maxDist > dist;
}
VERIFY(0x0228DD60, &daNpc_Md_c::chkArea);

/* 0228B418 */
void daNpc_Md_c::initialEndEvent(int staffIdx) {
    WWHD_FUNC(0x0228B418, void, this, staffIdx);
    be<s32>* p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001DE08) /* "EndMode" */);
    s32 mode = p == nullptr ? 0 : (s32)*p;
    gabi::Local<ProcFunc_l> fn;
    switch (mode) {
    case -2:
        if (mMsgNo == 0x19DE) {
            dComIfGs_onEventBit(0x1620);
            dComIfGs_onEventBit(0x1608);
            dComIfGp_setNextStage(STR(0x1001DE10) /* "sea" */, 0xE3, 13 /* dIsleRoom_DragonRoostIsland_e */, 8, 0.0f, 0, 1, 0);
        }
        /* fall-through */
    case -1:
        dComIfGp_event_reset();
        if (checkStatus(daMdStts_UNK80)) {
            setHarpPlayNum(0);
        }
        md_pmf_load(fn, PMF_waitNpcAction);
        setNpcAction(fn, nullptr);
        break;
    case 3:
        md_pmf_load(fn, PMF_deleteNpcAction);
        setNpcAction(fn, nullptr);
        break;
    case 2:
        clearStatus(daMdStts_UNK1);
        md_pmf_load(fn, PMF_squatdownNpcAction);
        setNpcAction(fn, nullptr);
        break;
    default:
        clearStatus(daMdStts_UNK1);
        if (checkStatus(daMdStts_UNK80)) {
            setHarpPlayNum(0);
        } else {
            setAnm(0);
        }
        md_pmf_load(fn, PMF_waitNpcAction);
        setNpcAction(fn, nullptr);
        break;
    }
}
VERIFY(0x0228B418, &daNpc_Md_c::initialEndEvent);

/* 0228CB88 */
BOOL daNpc_Md_c::actionTactEvent(int staffIdx) {
    WWHD_FUNC(0x0228CB88, BOOL, this, staffIdx);
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001DEB8) /* "prm0" */);
    s32 prm = 0;
    if (pPrm0 != nullptr) {
        prm = *pPrm0;
    }
    /* daPy_py_c::getTactMusic(): virtual, vtable (+0xB4) slot +0x2C */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 song = gabi::call_ptr<s32>(gabi::load<u32>(player->__vtbl + 0x2C), player);
    if (song >= 0) {
        setBitStatus(daMdStts_UNK400);
    }
    BOOL result = talk(1);
    if (result) {
        if (checkStatus(daMdStts_UNK400)) {
            clearStatus(daMdStts_UNK400);
            if (song == prm) {
                mMsgNo = 0x19DD;
            } else {
                mMsgNo = 0x19DA;
            }
        } else {
            mCurEventMode = 10;
            if (checkStatus(daMdStts_UNK200)) {
                mMsgNo = 0x19DC;
                clearStatus(daMdStts_UNK200);
            }
            fopAcM_onDraw(this);
            dComIfGp_event_reset();
        }
    }
    return result;
}
VERIFY(0x0228CB88, &daNpc_Md_c::actionTactEvent);

/* 0228B1B8 */
BOOL daNpc_Md_c::talk(int r4) {
    WWHD_FUNC(0x0228B1B8, BOOL, this, r4);
    u32 mng = l_msgMng();
    u16 msgStatus = fopMsgM_getStatus(mng);
    u8 msgAnmAtr = gabi::load<u8>(dComIfGp_ea() + 0x5BC5); /* dComIfGp_getMesgAnimeAttrInfo() */
    if (msgStatus == 0xE) { /* fopMsgStts_MSG_DISPLAYED_e */
        /* next_msgStatus's u16 result is passed on as the full register (GHS does not re-extend) */
        fopMsgM_setStatus(mng, gabi::call<u32>(0x0228AB60, this, &mMsgNo));
        if (fopMsgM_getStatus(mng) == 0xF) { /* fopMsgStts_MSG_CONTINUES_e */
            fopMsgM_messageSet(mng, mMsgNo, nullptr);
            m313A = 0;
        }
    } else if (msgStatus == 0x15) { /* fopMsgStts_INPUT_e */
        if (r4 != 0) {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BD3) != 0) { /* dComIfGp_checkMesgCancelButton() */
                fopMsgM_setStatus(mng, 0x10); /* fopMsgStts_MSG_ENDS_e */
                fopMsgM_messageSendOn(mng);
                setBitStatus(daMdStts_UNK200);
            }
            if (checkStatus(daMdStts_UNK400)) {
                fopMsgM_setStatus(mng, 0x10);
                fopMsgM_messageSendOn(mng);
            }
        }
    } else if (msgStatus == 6) { /* fopMsgStts_MSG_TYPING_e */
        if (m313A == 0 && !fopAcM_checkCarryNow(this) && !checkStatus(daMdStts_SHIP_RIDE) && mMsgNo != 0x05AC) {
            int anmType = getAnmType(msgAnmAtr);
            if (anmType >= 0) {
                setAnm(anmType);
            }
        }
    } else if (msgStatus == 0x12) { /* fopMsgStts_BOX_CLOSED_e */
        fopMsgM_setStatus(mng, 0x13); /* fopMsgStts_MSG_DESTROYED_e */
        clearStatus(daMdStts_XY_TALK);
        return TRUE;
    }
    if (!fopAcM_checkCarryNow(this) && !checkStatus(daMdStts_SHIP_RIDE) && mMsgNo != 0x05AC) {
        if (m313A == 1 && msgAnmAtr == 0x14 && m312A != 0) {
            setAnm(0x1B);
            m313A = 1;
        }
    }
    return FALSE;
}
VERIFY(0x0228B1B8, &daNpc_Md_c::talk);

/* 0228AB60 */
u16 daNpc_Md_c::next_msgStatus(be<u32>* pCurrMsgNo) {
    WWHD_FUNC(0x0228AB60, u16, this, pCurrMsgNo);
    u16 msgStatus = 0xF /* fopMsgStts_MSG_CONTINUES_e */;
    switch (*pCurrMsgNo) {
        case 0x5AC:
            msgStatus = 0x10;
            break;
        case 0x17D5:
        case 0x1805:
            *pCurrMsgNo = 0x17D6;
            break;
        case 0x17D6:
            *pCurrMsgNo = 0x17D7;
            break;
        case 0x17D7:
            *pCurrMsgNo = 0x17D8;
            break;
        case 0x17D8:
            mCurEventMode = 4;
            msgStatus = 0x10;
            break;
        case 0x17D9:
            *pCurrMsgNo = 0x1802;
            break;
        case 0x1802:
            *pCurrMsgNo = 0x17DA;
            break;
        case 0x17DA:
            *pCurrMsgNo = 0x17DB;
            break;
        case 0x17DB:
            dComIfGs_onEventBit(0x0E02);
            msgStatus = 0x10;
            break;
        case 0x17DD:
            *pCurrMsgNo = 0x17DE;
            break;
        case 0x17DE:
            mCurEventMode = 5;
            msgStatus = 0x10;
            break;
        case 0x17DF:
            *pCurrMsgNo = 0x17E0;
            break;
        case 0x17E0:
            *pCurrMsgNo = 0x17E1;
            break;
        case 0x17E1:
            *pCurrMsgNo = 0x17E2;
            break;
        case 0x17E2:
            *pCurrMsgNo = 0x17E3;
            break;
        case 0x17E4:
            dComIfGs_onEventBit(0x1101);
            msgStatus = 0x10;
            break;
        case 0x17E5:
            dComIfGs_onEventBit(0x1280);
            msgStatus = 0x10;
            break;
        case 0x17E6:
            *pCurrMsgNo = 0x17E7;
            break;
        case 0x17E7:
            *pCurrMsgNo = 0x17E8;
            break;
        case 0x17E8:
            *pCurrMsgNo = 0x17E9;
            break;
        case 0x17E9:
            *pCurrMsgNo = 0x17EA;
            break;
        case 0x17EA:
            *pCurrMsgNo = 0x17EB;
            break;
        case 0x17EB:
            *pCurrMsgNo = 0x17EC;
            break;
        case 0x17EC:
            *pCurrMsgNo = 0x17ED;
            break;
        case 0x17ED:
            *pCurrMsgNo = 0x17EE;
            break;
        case 0x17EE:
            if (gabi::load<s32>(l_msgMng() + 0x948) == 1 /* mSelectNum */) {
                *pCurrMsgNo = 0x17EF;
            } else {
                *pCurrMsgNo = 0x17F0;
            }
            break;
        case 0x17EF:
            *pCurrMsgNo = 0x17ED;
            break;
        case 0x17F0:
            *pCurrMsgNo = 0x17F1;
            break;
        case 0x17F1:
            mCurEventMode = 7;
            msgStatus = 0x10;
            break;
        case 0x17F3:
            *pCurrMsgNo = 0x17F4;
            break;
        case 0x17F4:
            dComIfGs_onEventBit(0x1104);
            // Fall-through
        case 0x17F8:
            setBitStatus(daMdStts_UNK40);
            msgStatus = 0x10;
            break;
        case 0x17F7:
            *pCurrMsgNo = 0x17F8;
            break;
        case 0x17FA:
            *pCurrMsgNo = 0x17FB;
            break;
        case 0x17FB:
            *pCurrMsgNo = 0x17FC;
            break;
        case 0x17FC:
            *pCurrMsgNo = 0x17FD;
            break;
        case 0x17FD:
            *pCurrMsgNo = 0x17FE;
            break;
        case 0x17FE:
            *pCurrMsgNo = 0x17FF;
            break;
        case 0x1800:
            *pCurrMsgNo = 0x1801;
            break;
        case 0x1801:
            dComIfGs_onEventBit(0x1102);
            msgStatus = 0x10;
            break;
        case 0x19C9:
            *pCurrMsgNo = 0x19CA;
            break;
        case 0x19CA:
            *pCurrMsgNo = 0x19CB;
            break;
        case 0x19CB:
            *pCurrMsgNo = 0x19CC;
            break;
        case 0x19CC:
            *pCurrMsgNo = 0x19CD;
            break;
        case 0x19CD:
            *pCurrMsgNo = 0x19CE;
            break;
        case 0x19CE:
            *pCurrMsgNo = 0x19CF;
            break;
        case 0x19CF:
            *pCurrMsgNo = 0x19D0;
            break;
        case 0x19D0:
            *pCurrMsgNo = 0x19D1;
            break;
        case 0x19D1:
            dComIfGs_onEventBit(0x1402);
            gabi::store<u8>(MD_M_SEATALK, 1); /* onSeaTalk() */
            msgStatus = 0x10;
            break;
        case 0x19D2:
            *pCurrMsgNo = 0x19D3;
            break;
        case 0x19D3:
            *pCurrMsgNo = 0x19D4;
            break;
        case 0x19D4:
            *pCurrMsgNo = 0x19D5;
            break;
        case 0x19D6:
            *pCurrMsgNo = 0x19D7;
            break;
        case 0x19D7:
            *pCurrMsgNo = 0x19D8;
            break;
        case 0x19D8:
            dComIfGs_onEventBit(0x1504);
        case 0x19D9:
            msgStatus = 0x10;
            break;
        case 0x19DA:
            *pCurrMsgNo = 0x19DB;
            break;
        case 0x19DD:
            *pCurrMsgNo = 0x19DE;
            break;
        case 0x19E0:
            *pCurrMsgNo = 0x19E1;
            break;
        case 0x19E1:
            *pCurrMsgNo = 0x19E2;
            break;
        case 0x19E2:
            *pCurrMsgNo = 0x19E3;
            break;
        case 0x19E3:
            *pCurrMsgNo = 0x19E4;
            break;
        case 0x19E5:
            *pCurrMsgNo = 0x19E6;
            break;
        case 0x19E7:
            *pCurrMsgNo = 0x19E8;
            break;
        case 0x19F6:
            *pCurrMsgNo = 0x19F7;
            break;
        case 0x19F7:
            *pCurrMsgNo = 0x19D2;
            break;
        case 0x19F8:
            *pCurrMsgNo = 0x19F9;
            break;
        case 0x19F9:
            *pCurrMsgNo = 0x19FA;
            break;
        case 0x19FA:
            *pCurrMsgNo = 0x19FB;
            break;
        case 0x19FB:
        case 0x19FF:
            dComIfGs_onEventBit(0x2E40);
            gabi::store<u8>(MD_M_SEATALK, 1); /* onSeaTalk() */
            // Fall-through
        case 0x1A01:
            dComIfGs_onEventBit(0x2C08);
            msgStatus = 0x10;
            break;
        case 0x19FC:
            *pCurrMsgNo = 0x19FD;
            break;
        case 0x19FD:
            *pCurrMsgNo = 0x19FE;
            break;
        case 0x19FE:
            *pCurrMsgNo = 0x19FF;
            break;
        case 0x1A00:
            *pCurrMsgNo = 0x1A01;
            break;
        case 0x1A02:
            dComIfGs_onEventBit(0x3B80);
            msgStatus = 0x10;
            break;
        default:
            msgStatus = 0x10;
            break;
    }
    return msgStatus;
}
VERIFY(0x0228AB60, &daNpc_Md_c::next_msgStatus);

/* 0228DDEC */
u32 daNpc_Md_c::getMsg() {
    WWHD_FUNC(0x0228DDEC, u32, this);
    u32 msgNo = 0;
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        msgNo = 0x19EF;
    } else if (mType == 0) { /* isTypeAtorizk() */
        if (dComIfGs_isEventBit(0x0E02 /* MEDLI_GAVE_FATHERS_LETTER */)) {
            msgNo = 0x17DC;
        } else if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0) { /* dComIfGs_getClearCount() */
            msgNo = 0x1805;
        } else {
            msgNo = 0x17D5;
        }
    } else if (mType == 1) { /* isTypeAdanmae() */
        if (checkStatus(daMdStts_UNK40)) {
            if (m312D == 0x10) {
                u8 count = m313C; /* getPiyo2TalkCNT() */
                msgNo = gabi::load<u32>(0x1001DF4C + count * 4); /* l_msg_num */
                /* countPiyo2TalkCNT() */
                u8 n = count + 1;
                if (n >= 3) {
                    m313C = 0;
                } else {
                    m313C = n;
                }
            } else {
                msgNo = 0x17F6;
            }
        } else if (dComIfGs_isEventBit(0x1104)) {
            msgNo = 0x17F7;
        } else {
            msgNo = 0x17E6;
        }
    } else if (mType == 2) { /* isTypeM_Dra09() */
        if (dComIfGs_isEventBit(0x1101)) {
            msgNo = 0x17E5;
        } else {
            msgNo = 0x17DD;
        }
    } else if (mType == 3) { /* isTypeSea() */
        if (checkStatus(daMdStts_XY_TALK)) {
            if (checkStatus(daMdStts_DEFAULT_TALK_XY)) {
                if (dComIfGs_isEventBit(0x2E40)) {
                    msgNo = 0x1A00;
                } else if (dComIfGs_isEventBit(0x2C08)) {
                    msgNo = 0x19F8;
                } else {
                    msgNo = 0x19FC;
                }
            } else {
                if (dComIfGs_isEventBit(0x1504)) {
                    msgNo = 0x19D9;
                } else {
                    msgNo = 0x19D6;
                }
            }
        } else {
            if (dComIfGs_isEventBit(0x1402)) {
                if (gabi::load<u8>(MD_M_SEATALK) != 0) { /* isSeaTalk() */
                    msgNo = 0x19D2;
                } else {
                    msgNo = 0x19F6;
                    gabi::store<u8>(MD_M_SEATALK, 1); /* onSeaTalk() */
                }
            } else {
                msgNo = 0x19C9;
            }
        }
    } else if (mType == 5 || mType == 4) { /* isTypeM_Dai() || isTypeEdaichi() */
        msgNo = 0x1A02;
    }
    return msgNo;
}
VERIFY(0x0228DDEC, &daNpc_Md_c::getMsg);

/* 0228CE84 */
void daNpc_Md_c::initialQuake(int staffIdx) {
    WWHD_FUNC(0x0228CE84, void, this, staffIdx);
    /* the 0x28 frame: cXyz temporary at sp+8, char acStack_1c[12] at sp+0x14 (placed as GHS does:
     * a longer mode string overflows strcpy into the same bytes) */
    gabi::Local<u8[0x28]> frame;
    u32 sp = gabi::ea(frame.get()) + 0x30 - 0x28;
    u32 buf = sp + 0x14;
    cXyz* tmp = gabi::at<cXyz>(sp + 8);
    u32 src = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DEDC) /* "Mode" */));
    if (src != 0) {
        strcpy_g(buf, src);
        if (md_streq(buf, 0x1001DED4 /* "Start" */)) {
            tmp->x = 0.0f;
            tmp->y = 1.0f;
            tmp->z = 0.0f;
            dComIfGp_getVibration_StartQuake(7, 1, tmp);
        }
        if (md_streq(buf, 0x1001DEE4 /* "Stop" */)) {
            dComIfGp_getVibration_StopQuake(1);
        }
    }
}
VERIFY(0x0228CE84, &daNpc_Md_c::initialQuake);

/* 0228DA10 */
void daNpc_Md_c::setMessageAnimation(u8 msgAnmAtr) {
    WWHD_FUNC(0x0228DA10, void, this, msgAnmAtr);
    if (fopAcM_checkCarryNow(this)) {
        return;
    }
    if (!dComIfGp_event_runCheck()) {
        return;
    }
    u32 staffName = gabi::load<u32>(0x101C0280); /* l_staff_name */
    int staffIdx = dComIfGp_evmng_getMyStaffId(gabi::at<const char>(staffName), nullptr, 0);
    if (staffIdx != -1) {
        u32 actName = dComIfGp_evmng_getMyActName(staffIdx);
        /* HD: NULL check */
        if (actName != 0 && md_streq(actName, 0x1001DF40 /* "WAIT" */)) {
            int anmType = getAnmType(msgAnmAtr);
            if (anmType >= 0) {
                setAnm(anmType);
            }
        }
    }
}
VERIFY(0x0228DA10, &daNpc_Md_c::setMessageAnimation);

/* 0228DAF4 (cXyz pos by value: a pointer to the caller's copy) */
bool daNpc_Md_c::chkAttention(cXyz* pos, s16 angle, int param_3) {
    WWHD_FUNC(0x0228DAF4, bool, this, pos, angle, param_3);
    fopAc_ac_c* link = daPy_getPlayerLinkActorClass();
    f32 maxAttnDistXZ = l_HIO().mNpc.mMaxAttnDistXZ;
    f32 dVar8 = l_HIO().m0C0;
    int maxAttnAngleY = l_HIO().mNpc.mMaxAttnAngleY;
    f32 dVar9 = l_HIO().m0BC;
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        maxAttnDistXZ = l_HIO().m074.m04;
    } else if (mType == 2 && dComIfGs_isEventBit(0x1101)) { /* isTypeM_Dra09() */
        maxAttnDistXZ = l_HIO().m0B8;
    }
    f32 dz = link->current.pos.z - pos->z;
    f32 dx = link->current.pos.x - pos->x;
    f32 distXZ = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s16 targetAngleY = cM_atan2s(dx, dz);
    f32 dy = link->current.pos.y - pos->y;
    if (m312C != 0) {
        maxAttnDistXZ += 40.0f;
        maxAttnAngleY += 0x71C;
    }
    targetAngleY -= angle;
    if (param_3 != 0) {
        return (maxAttnDistXZ > distXZ) && (dy < dVar9) && (dy > dVar8);
    } else {
        return (maxAttnAngleY > abs((int)targetAngleY)) && (maxAttnDistXZ > distXZ) && (dy < dVar9) && (dy > dVar8);
    }
}
VERIFY(0x0228DAF4, &daNpc_Md_c::chkAttention);

/* 0228D208 */
void daNpc_Md_c::lookBack(cXyz* param_1, int param_2, int param_3) {
    WWHD_FUNC(0x0228D208, void, this, param_1, param_2, param_3);
    gabi::Local<cXyz> local_30;
    gabi::Local<cXyz> local_3c;
    cXyz* dstPos = nullptr;
    f32 x = 0.0f, y = 0.0f, z = 0.0f; /* local_3c.setall(0.0f) */
    s16 desiredYRot = shape_angle.y;
    if (mJntCtrl.mbTrn != 0) { /* mJntCtrl.trnChk() */
        cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
    } else {
        m3110 = 0;
    }
    if (param_2 != 0) {
        /* local_30 = *param_1 (integer copy) */
        gabi::store<u32>(gabi::ea(local_30.get()) + 8, gabi::load<u32>(gabi::ea(param_1) + 8));
        gabi::store<u32>(gabi::ea(local_30.get()), gabi::load<u32>(gabi::ea(param_1)));
        gabi::store<u32>(gabi::ea(local_30.get()) + 4, gabi::load<u32>(gabi::ea(param_1) + 4));
        dstPos = local_30;
        x = current.pos.x;
        y = eyePos.y;
        z = current.pos.z;
    }
    local_3c->x = x;
    local_3c->y = y;
    local_3c->z = z;
    dNpc_JntCtrl_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, local_3c, desiredYRot, m3110, param_3 != 0);
}
VERIFY(0x0228D208, (void (daNpc_Md_c::*)(cXyz*, int, int)) & daNpc_Md_c::lookBack);

/* 0228D32C */
BOOL daNpc_Md_c::actionTurnEvent(int staffIdx) {
    WWHD_FUNC(0x0228D32C, BOOL, this, staffIdx);
    setAttention(true);
    /* char local_20[10] = "@PLAYER" at sp+0x10 of the 0x28 frame (placed as GHS does: a longer
     * target name overflows strcpy into the same bytes); initialised from .rodata as 3 words */
    gabi::Local<u8[0x28]> frame;
    u32 buf = gabi::ea(frame.get()) + 8 + 0x10;
    u32 w2 = gabi::load<u32>(0x1001DF10 + 8);
    u32 w0 = gabi::load<u32>(0x1001DF10);
    u32 w1 = gabi::load<u32>(0x1001DF10 + 4);
    gabi::store<u32>(buf, w0);
    gabi::store<u32>(buf + 4, w1);
    gabi::store<u32>(buf + 8, w2);
    u32 target = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DF00) /* "Target" */));
    if (target != 0) {
        strcpy_g(buf, target);
    }
    be<s32>* type = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001DF08) /* "Type" */);
    int iVar4 = type == nullptr ? 0 : (s32)*type;
    dNpc_HIO_l& npc = l_HIO().mNpc;
    mJntCtrl.setParam(npc.mMaxBackboneX, npc.mMaxBackboneY, npc.mMinBackboneX, npc.mMinBackboneY, npc.mMaxHeadX,
                      npc.mMaxHeadY, -500, npc.mMinHeadY, npc.mMaxTurnStep);
    if (iVar4 == 2) {
        mJntCtrl.mbTrn = 1; /* setTrn() */
        iVar4 = 0;
    }
    if (md_streq(buf, 0x1001DEF8 /* "@PLAYER" */)) {
        lookBack(1, 0, iVar4);
    } else {
        lookBack(&m30B8, 1, iVar4);
    }
    if (cLib_calcTimer(&m3118) != 0) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0228D32C, &daNpc_Md_c::actionTurnEvent);

/* 0228D848 */
BOOL daNpc_Md_c::actionLookDown(int) {
    WWHD_FUNC(0x0228D848, BOOL, this, 0);
    gabi::Local<cXyz> local_14;
    gabi::Local<cXyz> local_38;
    gabi::Local<cXyz> cStack_20;
    f32 x = current.pos.x;
    f32 y = eyePos.y;
    f32 z = current.pos.z;
    s16 desiredYRot = shape_angle.y;
    m30AC.y = -50.0f;
    /* local_38.set(...); local_14 = local_38 */
    local_14->x = x;
    local_14->y = y;
    local_14->z = z;
    Mtx34* stack = gabi::at<Mtx34>(0x1048D0CC); /* mDoMtx_stack_c::now */
    mDoMtx_YrotS(stack, desiredYRot);
    PSMTXMultVec(stack, &m30AC, cStack_20);
    PSVECAdd(local_14, cStack_20, local_14);
    local_38->x = x;
    local_38->y = y;
    local_38->z = z;
    dNpc_JntCtrl_lookAtTarget(&mJntCtrl, &shape_angle.y, local_14, local_38, desiredYRot, m3110, false);
    return TRUE;
}
VERIFY(0x0228D848, &daNpc_Md_c::actionLookDown);

/* local_24 = *pos (float copy); returns local_3c.absXZ() with local_3c = local_24 - current.pos */
static inline f32 md_walk_target(daNpc_Md_c* i_this, cXyz* local_24, cXyz* src, cXyz* local_3c, cXyz* xz,
                                 f32* dx, f32* dz) {
    local_24->x = (f32)src->x;
    local_24->y = (f32)src->y;
    local_24->z = (f32)src->z;
    cXyz_mi(local_24, local_3c, &i_this->current.pos);
    *dx = local_3c->x;
    *dz = local_3c->z;
    xz->x = *dx;
    xz->z = *dz;
    xz->y = 0.0f;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* current.pos = local_24 (integer copy) */
static inline void md_set_pos(daNpc_Md_c* i_this, cXyz* local_24) {
    u32 p = gabi::ea(&i_this->current.pos), l = gabi::ea(local_24);
    gabi::store<u32>(p + 4, gabi::load<u32>(l + 4));
    gabi::store<u32>(p + 8, gabi::load<u32>(l + 8));
    gabi::store<u32>(p, gabi::load<u32>(l));
}
/* mJntCtrl.setTrn(); mJntCtrl.lookAtTarget(&shape_angle.y, &local_24, current.pos, current.angle.y, m3110, false) */
static inline void md_walk_look(daNpc_Md_c* i_this, cXyz* local_24, cXyz* eye) {
    eye->z = i_this->current.pos.z;
    eye->y = i_this->current.pos.y;
    eye->x = i_this->current.pos.x;
    i_this->mJntCtrl.mbTrn = 1;
    dNpc_JntCtrl_lookAtTarget(&i_this->mJntCtrl, &i_this->shape_angle.y, local_24, eye, i_this->current.angle.y, i_this->m3110, false);
}

/* 0228C50C */
BOOL daNpc_Md_c::actionWalkEvent(int staffIdx) {
    WWHD_FUNC(0x0228C50C, BOOL, this, staffIdx);
    gabi::Local<cXyz> local_24;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> local_3c;
    cXyz* pfVar2 = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, STR(0x1001DEA4) /* "Pos" */);
    be<f32>* pfVar3 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DEA8) /* "Speed" */);
    be<f32>* pfVar4 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE9C) /* "RunRate" */);
    if (pfVar2 != nullptr) {
        f32 dx, dz;
        if (md_walk_target(this, local_24, pfVar2, local_3c, xz, &dx, &dz) < 10.0f) {
            speedF = 0.0f;
            return TRUE;
        }
        current.angle.y = cM_atan2s(dx, dz);
        if (mJntCtrl.mbTrn != 0) { /* mJntCtrl.trnChk() */
            cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
        } else {
            m3110 = 0;
            if (pfVar3 != nullptr) {
                speedF = *pfVar3;
            } else {
                speedF = 5.0f;
            }
            if (pfVar4 != nullptr) {
                mRunRate = *pfVar4;
            } else {
                mRunRate = 0.0f;
            }
            setAnm(2);
        }
        md_walk_look(this, local_24, eye);
        setAttention(true);
        if (wallHitCheck() >= 0) {
            md_set_pos(this, local_24);
            speedF = 0.0f;
            return TRUE;
        }
        if (cLib_calcTimer(&m3118) != 0) {
            return FALSE;
        }
        md_set_pos(this, local_24);
        speedF = 0.0f;
    }
    return TRUE;
}
VERIFY(0x0228C50C, &daNpc_Md_c::actionWalkEvent);

/* 0228C898 */
BOOL daNpc_Md_c::actionDashEvent(int staffIdx) {
    WWHD_FUNC(0x0228C898, BOOL, this, staffIdx);
    gabi::Local<cXyz> local_24;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> local_3c;
    cXyz* pfVar2 = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, STR(0x1001DEB4) /* "Pos" */);
    if (pfVar2 != nullptr) {
        f32 dx, dz;
        if (md_walk_target(this, local_24, pfVar2, local_3c, xz, &dx, &dz) < 10.0f) {
            speedF = 0.0f;
            return TRUE;
        }
        current.angle.y = cM_atan2s(dx, dz);
        if (mJntCtrl.mbTrn != 0) {
            cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
        } else {
            m3110 = 0;
            speedF = 17.0f;
            mRunRate = 1.0f;
            setAnm(2);
        }
        md_walk_look(this, local_24, eye);
        if (wallHitCheck() >= 0) {
            md_set_pos(this, local_24);
            speedF = 0.0f;
            return TRUE;
        }
        setAttention(true);
        if (cLib_calcTimer(&m3118) != 0) {
            return FALSE;
        }
        md_set_pos(this, local_24);
        speedF = 0.0f;
    }
    return TRUE;
}
VERIFY(0x0228C898, &daNpc_Md_c::actionDashEvent);

/* HD frame controls: mDoExt_McaMorf2 (play speed +0xA4, frame +0xA8, end +0xAE, loop frame +0xB0),
 * mDoExt_McaMorf (play speed +0x98, frame +0x9C, end +0xA2, loop frame +0xA4) */
static inline void md_morf_toEnd(u32 morf, u32 frameOff, u32 endOff) {
    /* setFrame(getEndFrame()): s16 end -> float -> s16 -> float */
    s16 end = gabi::load<s16>(morf + endOff);
    s16 f = (s16)gabi::ftoi((f32)end);
    gabi::store<f32>(morf + frameOff, (f32)f);
}
static inline void md_morf_loopAtFrame(u32 morf, u32 frameOff, u32 loopOff) {
    /* setLoopFrame(getFrame()) */
    gabi::store<s16>(morf + loopOff, (s16)gabi::ftoi(gabi::load<f32>(morf + frameOff)));
}

/* 0228D564 */
void daNpc_Md_c::initialSetAnmEvent(int staffIdx) {
    WWHD_FUNC(0x0228D564, void, this, staffIdx);
    u32 puVar1 = gabi::ea(dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x1001DF1C) /* "Number" */));
    if (puVar1 != 0) {
        u8 r4 = gabi::load<u8>(puVar1 + 3); /* (u8)*puVar1 */
        if (setAnm(getAnmType(r4))) {
            setBitStatus(daMdStts_UNK1);
        } else {
            clearStatus(daMdStts_UNK1);
        }
    }
    speedF = 0.0f;
    be<f32>* pfVar2 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DF24) /* "PlaySpeed" */);
    if (pfVar2 != nullptr) {
        gabi::store<f32>(gabi::ea(mpMorf.get()) + 0xA4, *pfVar2);    /* setPlaySpeed */
        gabi::store<f32>(gabi::ea(mpArmMorf.get()) + 0xA4, *pfVar2);
        gabi::store<f32>(gabi::ea(mpWingMorf.get()) + 0x98, *pfVar2);
        if (*pfVar2 < 0.0f) {
            md_morf_toEnd(gabi::ea(mpMorf.get()), 0xA8, 0xAE);
            md_morf_toEnd(gabi::ea(mpArmMorf.get()), 0xA8, 0xAE);
            md_morf_toEnd(gabi::ea(mpWingMorf.get()), 0x9C, 0xA2);
        }
        md_morf_loopAtFrame(gabi::ea(mpMorf.get()), 0xA8, 0xB0);
        md_morf_loopAtFrame(gabi::ea(mpArmMorf.get()), 0xA8, 0xB0);
        md_morf_loopAtFrame(gabi::ea(mpWingMorf.get()), 0x9C, 0xA4);
        m30D0 = gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xA8); /* mpMorf->getFrame() */
    }
    pfVar2 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DF30) /* "MorfFrame" */);
    if (pfVar2 != nullptr) {
        gabi::call(0x025E5D1C, mpMorf.get(), (f32)*pfVar2);    /* mDoExt_McaMorf2::setMorf */
        gabi::call(0x025E5D1C, mpArmMorf.get(), (f32)*pfVar2);
        gabi::call(0x025E4A54, mpWingMorf.get(), (f32)*pfVar2); /* mDoExt_McaMorf::setMorf */
    }
    /* clearJntAng() */
    mJntCtrl.mAngles[0][1] = 0;
    mJntCtrl.mAngles[0][0] = 0;
    mJntCtrl.mAngles[1][1] = 0;
    mJntCtrl.mAngles[1][0] = 0;
}
VERIFY(0x0228D564, &daNpc_Md_c::initialSetAnmEvent);

/* 0228DFF8 */
s32 daNpc_Md_c::lookBackWaist(s16 param_1, f32 param_2) {
    WWHD_FUNC(0x0228DFF8, s32, this, param_1, param_2);
    s16 iVar2 = (s16)(param_1 - shape_angle.y);
    f32 sn = cM_ssin(iVar2);
    f32 cs = cM_scos(iVar2);
    f32 fVar1 = cs * param_2 * l_HIO().m034.m04;
    f32 dVar6 = sn * param_2 * l_HIO().m034.m08;
    /* HD: an option of the save data (027200D0(save + 0x12C0) + 2) == 0 mirrors the waist turn */
    u32 opt = gabi::call<u32>(0x027200D0, gabi::load<u32>(0x101F84DC) + 0x12C0);
    if (gabi::load<u8>(opt + 2) == 0) {
        fVar1 = -fVar1;
    }
    s16 sVar3;
    if (fVar1 < 0.0f) {
        sVar3 = (s16)gabi::ftoi((f32)l_HIO().m034.m20 * fVar1);
    } else {
        sVar3 = (s16)gabi::ftoi((f32)l_HIO().m034.m1E * fVar1);
    }
    sVar3 = cLib_addCalcAngleS(&m3114, sVar3, 4, 0xC00, 0x180);
    s16 sVar4 = cLib_addCalcAngleS(&m3116, (s16)gabi::ftoi((f32)l_HIO().m034.m22 * dVar6), 4, 0xC00, 0x180);
    /* HD: while m310C > 0.9, a body turn towards a target far to the side */
    f32 rate = m310C;
    if (rate > 0.9f) {
        f32 s = cM_ssin(iVar2);
        if (std::fabs(s) > 0.91f || std::fabs(cM_ssin((u16)m311A)) > 0.95f) {
            s16 d = (s16)gabi::ftoi(512.0f * (rate - 0.5f) * s);
            cLib_addCalcAngleS(&shape_angle.y, (s16)(shape_angle.y + d), 4, 0x40, 0x10);
        }
    }
    return (sVar3 == 0 && sVar4 == 0) ? TRUE : FALSE;
}
VERIFY(0x0228DFF8, &daNpc_Md_c::lookBackWaist);

/* sead::SafeString operator== (HD inline: assureTermination, then strcmp bounded by 0x40001) */
static inline bool md_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(b) + 4) + 0x14), b);
    u32 s1 = gabi::load<u32>(gabi::ea(a));
    u32 s2 = gabi::load<u32>(gabi::ea(b));
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}
/* cM_deg2s(x): (s16)(x * 182.04444) */
static inline s16 md_deg2s(f32 deg) { return (s16)gabi::ftoi(deg * 182.04444885253906f); }
/* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16): virtual, vtable (+0xB4) slot +0x114 */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    gabi::call_ptr(gabi::load<u32>(pl->__vtbl + 0x114), pl, pos, angle);
}

/* 0228B68C */
void daNpc_Md_c::initialMovePosEvent(int staffIdx) {
    WWHD_FUNC(0x0228B68C, void, this, staffIdx);
    /* the 0xC8 frame, as GHS lays it out (a strcpy overflow of acStack_ac reaches the same bytes):
     * acStack_ac[12] at sp+8, SafeStrings at sp+0x14/sp+0x34, cXyz pos at sp+0x28, dBgS_GndChk at sp+0x3C */
    gabi::Local<u8[0xC8]> frame;
    u32 sp = gabi::ea(frame.get()) + 8;
    u32 buf = sp + 8;
    fopAc_ac_c* link = nullptr;
    u32 target = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DE38) /* "Target" */));
    if (target != 0) {
        strcpy_g(buf, target);
        if (md_streq(buf, 0x1001DE1C /* "@PLAYER" */)) {
            link = daPy_getPlayerLinkActorClass();
        }
        if (link != nullptr) {
            u32 type = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DE40) /* "Type" */));
            if (type != 0) {
                strcpy_g(buf, type);
            }
            if (md_streq(buf, 0x1001DE70 /* "LINK_DIST" */)) {
                s16 uVar9 = fopAcM_searchActorAngleY(this, link);
                be<f32>* pDist = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE48) /* "Dist" */);
                if (pDist != nullptr) {
                    cXyz* pos = gabi::at<cXyz>(sp + 0x28);
                    f32 dist = *pDist;
                    pos->y = current.pos.y + 50.0f;
                    pos->x = gabi::fmadds(dist, cM_ssin(uVar9), current.pos.x);
                    pos->z = gabi::fmadds(dist, cM_scos(uVar9), current.pos.z);
                    /* dBgS_GndChk local_a0 (inline constructor); local_a0.SetPos(&pos) */
                    u32 g = sp + 0x3C;
                    gabi::call(0x02008E0C, g); /* cBgS_GndChk::cBgS_GndChk */
                    gabi::store<u32>(g + 0x50, 1);
                    gabi::store<u32>(g + 0x4C, 0x1001D738);
                    gabi::store<u32>(g + 0x20, 0x1001D728);
                    gabi::store<u32>(g + 0x40, 0x1001D748);
                    gabi::store<u32>(g + 0x10, 0x1001D718);
                    gabi::store<u32>(g + 0x24, gabi::load<u32>(sp + 0x28));
                    for (int i = 0; i < 7; i++) {
                        gabi::store<u8>(g + 0x44 + i, 0);
                    }
                    gabi::store<u32>(g + 0x28, gabi::load<u32>(sp + 0x2C));
                    gabi::store<u32>(g + 0x2C, gabi::load<u32>(sp + 0x30));
                    gabi::store<u32>(g + 0x4, g + 0x4C);
                    gabi::store<u32>(g + 0x0, g + 0x40);
                    f32 y = cBgS_GroundCross(dComIfG_Bgsp(), gabi::at<void>(g));
                    if (y != -1000000000.0f) { /* -G_CM3D_F_INF */
                        pos->y = y;
                    } else {
                        pos->y = current.pos.y;
                    }
                    daPy_setPlayerPosAndAngle(link, pos, link->shape_angle.y);
                    /* ~dBgS_GndChk */
                    gabi::store<u32>(g + 0x20, 0x1001D728);
                    gabi::store<u32>(g + 0x40, 0x1001D748);
                    gabi::store<u32>(g + 0x4C, 0x1001D708);
                    gabi::call(0x02008DAC, g, 0); /* cBgS_Chk::~cBgS_Chk */
                }
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE30) /* "Angle" */);
                if (pAngle != nullptr) {
                    daPy_setPlayerPosAndAngle(link, nullptr, (s16)(md_deg2s(*pAngle) + (-0x8000 + uVar9)));
                }
            } else if (md_streq(buf, 0x1001DE50 /* "DIST" */)) {
                be<f32>* pDist = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE48) /* "Dist" */);
                if (pDist != nullptr) {
                    s16 a = link->shape_angle.y;
                    f32 dist = *pDist;
                    f32 x = gabi::fmadds(dist, cM_ssin(a), link->current.pos.x);
                    f32 z = gabi::fmadds(dist, cM_scos(a), link->current.pos.z);
                    current.pos.x = x;
                    current.pos.y = link->current.pos.y;
                    current.pos.z = z;
                }
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE30) /* "Angle" */);
                if (pAngle != nullptr) {
                    s16 angle = (s16)(md_deg2s(*pAngle) + (-0x8000 + link->shape_angle.y));
                    current.angle.y = angle;
                    shape_angle.y = angle;
                }
            } else {
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE30) /* "Angle" */);
                if (pAngle != nullptr) {
                    s16 angle = (s16)(md_deg2s(*pAngle) + link->shape_angle.y);
                    current.angle.y = angle;
                    shape_angle.y = angle;
                }
                cXyz* pPos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, STR(0x1001DE28) /* "Pos" */);
                if (pPos != nullptr) {
                    f32 x = link->current.pos.x + pPos->x;
                    f32 y = link->current.pos.y + pPos->y;
                    f32 z = link->current.pos.z + pPos->z;
                    current.pos.x = x;
                    current.pos.y = y;
                    current.pos.z = z;
                }
            }
        }
    } else {
        be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, STR(0x1001DE30) /* "Angle" */);
        if (pAngle != nullptr) {
            s16 angle = md_deg2s(*pAngle);
            current.angle.y = angle;
            shape_angle.y = angle;
        }
        cXyz* pPos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, STR(0x1001DE28) /* "Pos" */);
        if (pPos != nullptr) {
            current.pos.x = (f32)pPos->x;
            current.pos.y = (f32)pPos->y;
            current.pos.z = (f32)pPos->z;
            /* HD: placed by the event "ARRIVAL_GND": start in the player's initial room */
            SafeString* arrival = gabi::at<SafeString>(sp + 0x14);
            SafeString* evName = gabi::at<SafeString>(sp + 0x34);
            arrival->__vtbl = 0x1001D6A0; /* this TU's sead::SafeString vtable */
            arrival->mStringTop = 0x1001DE64; /* "ARRIVAL_GND" */
            u32 name = gabi::call<u32>(0x02542E94, dComIfGp_getPEvtManager()); /* dEvent_manager_c run event name */
            evName->__vtbl = 0x1001D6A0;
            evName->mStringTop = name;
            if (md_SafeString_eq(arrival, evName)) {
                gabi::call<s32>(0x024451B4, this); /* daPy_npc_c::check_initialRoom */
                setAnm(0);
                m3131 = 1;
            }
        }
    }
    gravity = l_HIO().m0F4;
    maxFallSpeed = -100.0f;
    speedF = 0.0f;
    speed.y = 0.0f;
    u32 pName = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DE58) /* "Name" */));
    if (pName != 0) {
        strcpy_g(buf, pName);
        if (md_streq(buf, gabi::load<u32>(0x101C1EE4) /* cut_anm_tbl[0] "WAIT" */)) {
            setAnm(gabi::load<s32>(0x1001DE24)); /* cut_anm_idx_tbl[0] */
        }
    }
    u32 pShipRide = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, STR(0x1001DE7C) /* "ShipRide" */));
    if (pShipRide != 0) {
        strcpy_g(buf, pShipRide);
        if (md_streq(buf, 0x1001DE60 /* "on" */)) {
            setBitStatus(daMdStts_SHIP_RIDE);
        } else if (md_streq(buf, 0x1001DE2C /* "off" */)) {
            clearStatus(daMdStts_SHIP_RIDE);
        }
    }
}
VERIFY(0x0228B68C, &daNpc_Md_c::initialMovePosEvent);
