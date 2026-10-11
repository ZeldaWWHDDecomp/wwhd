/**
 * d_a_npc_md_event.cpp (WWHD)
 * Player - Medli: event cuts, messages, movement helpers (0228AB60..0228EB87)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md_event.h"
using namespace MdEvent;
static TurnTargetWords turnTarget;
static Mtx34 mtxStack;
static SavePointer saveInfo;
static MessagePointer messageManager;
static u8 m_seaTalk;
static HarpAnimationTable l_harp_play_anm;
static MessageTable l_msg_num;
static NamePointer l_staff_name;
static NamePointer cut_anm_wait;
static NamePointer cut_anm_onetime;
static s32 cut_anm_wait_index;
static s32 cut_anm_onetime_index;
static ActionWords action_mkamaePlayerAction;
static ActionWords action_flyPlayerAction;
static ActionWords action_searchNpcAction;
static ActionWords action_fallNpcAction;
static ActionWords action_waitNpcAction;
static ActionWords action_deleteNpcAction;
static ActionWords action_squatdownNpcAction;

// Inlined helpers preserve the HD call signatures.
/* cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return mdEvent_calcTimer(t); }
/* cLib_getRndValue<int>(int start, int range) (out of line copy) */
static inline s32 cLib_getRndValue(s32 start, s32 range) { return mdEvent_getRndValue(start, range); }
/* mDoAud_monsSeStart(id, pos, procId, reverb): HD fopAcM_monsSeStart with param 0 */
static inline void mDoAud_monsSeStart4(u32 id, cXyz* pos, u32 pid, s32 reverb) { mdEvent_monsSeStart(id, pos, pid, reverb); }
/* daPy_lk_c::checkEndTactMusic */
static inline bool daPy_lk_checkEndTactMusic(fopAc_ac_c* pl) { return mdEvent_checkEndTactMusic(pl); }
/* strcpy */
static inline void strcpy_g(u32 dst, u32 src) { mdEvent_strcpy(dst, src); }
static inline fopAc_ac_c* daPy_getPlayerLinkActorClass() { return dComIfGp_getLinkPlayer(); }
/* CPad (port 0): stick angle, A/B trigger */
static inline s16 CPad_GET_STICK_ANGLE(s32 port) { return mdEvent_stickAngle(port); }
static inline bool CPad_CHECK_TRIG_A(s32 port) { return mdEvent_trigA(port); }
static inline BOOL CPad_CHECK_TRIG_B(s32 port) { return mdEvent_trigB(port); }
/* 024F8018 dCam_getControledAngleY(camera_class*); camera 0 at play+0x5AF8 */
static inline s16 dCam_getControledAngleY(u32 cam) { return mdEvent_cameraAngleY(cam); }
static inline u32 dAttention_GetLockonList(dAttention_c* a, s32 i) { return mdEvent_lockonList(a, i); }
static inline u32 dAttention_GetActionList(dAttention_c* a, s32 i) { return mdEvent_actionList(a, i); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return mdEvent_lockonTruth(a); }
static inline fopAc_ac_c* dAttList_getActor(u32 l) { return mdEvent_listActor(l); }
/* cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return mdEvent_addCalc(v, target, scale, maxStep, minStep);
}
/* dBgS::GetSpecialCode(cBgS_PolyInfo&) */
static inline s32 dBgS_GetSpecialCode(dBgS* bgs, u32 poly) { return mdEvent_specialCode(bgs, poly); }
// HD: normalize this call to bool before returning the BOOL result.
static inline bool md_presentEnd() { return mdEvent_presentEnd(dComIfGp_getPEvtManager()); }
// HD: messages use a manager and out-of-line status accessors.
static inline u32 l_msgMng() { return gabi::ea(messageManager.mpManager.get()); }
static inline u16 fopMsgM_getStatus(u32 mng) { return mdEvent_messageStatus(mng); }
static inline void fopMsgM_setStatus(u32 mng, u32 st) { mdEvent_setMessageStatus(mng, st); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return mdEvent_messageSet(mng, msgNo, pos); }
static inline void fopMsgM_messageSendOn(u32 mng) { message(mng)->mSendOn = 1; }
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag (+0xDC), fpcLf_GetPriority(this)) */
static inline s16 fpcLf_GetPriority(void* a) { return mdEvent_priority(a); }
static inline void fopDwTg_ToDrawQ(u32 tag, s16 prio) { mdEvent_drawQueue(tag, prio); }
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s16 prio = fpcLf_GetPriority(a);
    fopDwTg_ToDrawQ(gabi::ea(&a->draw_tag), prio);
}
/* dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    mdEvent_nextStage(stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
static inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & fopAcStts_CARRY_e) != 0; }

/* inline strcmp(a, b) == 0 (GHS inlines it as a byte loop or a word loop; the result is the same) */
static inline bool md_streq(u32 a, u32 b) {
    u8 ca, cb;
    do {
        ca = (u8)*bytes(a++);
        cb = (u8)*bytes(b++);
    } while (ca == cb && ca != 0);
    return ca == cb;
}
/* dVibration_c (play+0x599C): StartQuake(int, int, cXyz) (cXyz by value: pointer to a copy), StopQuake(int) */
static inline BOOL dComIfGp_getVibration_StartQuake(s32 a, s32 b, cXyz* pos) { return mdEvent_startQuake(dComIfGp_getVibration(), a, b, pos); }
static inline BOOL dComIfGp_getVibration_StopQuake(s32 a) { return mdEvent_stopQuake(dComIfGp_getVibration(), a); }
// HD: keep the full guest address returned by getMyActName.
static inline u32 md_actName(s32 staffId) { return mdEvent_actName(dComIfGp_getPEvtManager(), staffId); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, bool headOnly) {
    mdEvent_lookAtTarget(j, outY, target, eye, yrot, vel, headOnly);
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

    word(i_this->current.pos.y) = word(local_24->y);
    word(i_this->current.pos.z) = word(local_24->z);
    word(i_this->current.pos.x) = word(local_24->x);
}
/* mJntCtrl.setTrn(); mJntCtrl.lookAtTarget(&shape_angle.y, &local_24, current.pos, current.angle.y, m3110, false) */
static inline void md_walk_look(daNpc_Md_c* i_this, cXyz* local_24, cXyz* eye) {
    eye->z = i_this->current.pos.z;
    eye->y = i_this->current.pos.y;
    eye->x = i_this->current.pos.x;
    i_this->mJntCtrl.mbTrn = 1;
    dNpc_JntCtrl_lookAtTarget(&i_this->mJntCtrl, &i_this->shape_angle.y, local_24, eye, i_this->current.angle.y, i_this->m3110, false);
}

/* HD frame controls: mDoExt_McaMorf2 (play speed +0xA4, frame +0xA8, end +0xAE, loop frame +0xB0),
 * mDoExt_McaMorf (play speed +0x98, frame +0x9C, end +0xA2, loop frame +0xA4) */

/* sead::SafeString operator== (HD inline: assureTermination, then strcmp bounded by 0x40001) */
static inline bool md_SafeString_eq(SafeString* a, SafeString* b) {
    assureTermination(a);
    assureTermination(a);
    assureTermination(b);
    u32 s1 = (u32)a->mStringTop;
    u32 s2 = (u32)b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = (u8)bytes(s1)[i];
        u8 c2 = (u8)bytes(s2)[i];
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
    setPlayerPosition(pl,pos,angle);
}

/* 0228E658 */
s16 daNpc_Md_c::getStickAngY(int param_1) {
    if (param_1) {
        return (s16)(CPad_GET_STICK_ANGLE(0) + 0x8000);
    }
    u32 cam = (u32)play()->mpCamera; /* dComIfGp_getCamera(0) */
    s32 stick = CPad_GET_STICK_ANGLE(0) + 0x8000;
    return (s16)(stick + dCam_getControledAngleY(cam));
}

/* 0228E6CC */
int daNpc_Md_c::calcStickPos(s16 param_1, cXyz* param_2) {
    dAttention_c* attention = dComIfGp_getAttention();
    u32 attList = dAttention_GetLockonList(attention, 0);
    /* dAttention_c::Lockon(): LockonTruth() || chkFlag(AttnFlag_20000000) */
    bool r26 = true;
    if (!dAttention_LockonTruth(attention) && !((u32)MdEvent::attention(attention)->mFlags & AttnFlag_20000000)) {
        r26 = false;
    }
    int r31 = !r26 ? 0 : dAttention_LockonTruth(attention) ? 1 : -1;
    if (attList == 0) {
        attList = dAttention_GetActionList(attention, 0);
    }
    if (attList != 0) {
        fopAc_ac_c* ac = dAttList_getActor(attList);
        word(param_2->x) = word(ac->eyePos.x);
        word(param_2->y) = word(ac->eyePos.y);
        word(param_2->z) = word(ac->eyePos.z);
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

/* 0228E804 */
BOOL daNpc_Md_c::flyCheck() {
    if (!CPad_CHECK_TRIG_A(0)) {
        return FALSE;
    }
    gabi::Local<ProcFunc_l> fn;
    if (checkStatus(daMdStts_LIGHT_BODY_HIT)) {
        copyAction(fn, action_mkamaePlayerAction);
    } else {
        copyAction(fn, action_flyPlayerAction);
    }
    setPlayerAction(fn, nullptr);
    return TRUE;
}

/* 0228E8AC */
BOOL daNpc_Md_c::mirrorCancelCheck() {
    return CPad_CHECK_TRIG_B(0) ? TRUE : FALSE;
}

/* 0228C1B4 */
void daNpc_Md_c::setWingEmitter() {
    particle_set(&m0508[0], ID_IT_SN_MD_HANE00);
}

/* 0228E2D4 */
void daNpc_Md_c::setHane02Emitter() {
    particle_set(&m0508[2], ID_IT_SN_MD_HANE02);
    particle_set(&m0508[3], ID_IT_SN_MD_HANE02);
}

/* 0228E328 */
void daNpc_Md_c::setHane03Emitter() {
    particle_set(&m0508[4], ID_IT_SN_MD_HANE03);
    particle_set(&m0508[5], ID_IT_SN_MD_HANE03);
}

/* 0228C4D0 */
int daNpc_Md_c::wallHitCheck() {
    if (mAcch.ChkWallHit()) {
        for (int i = 0; i < 2; i++) {
            if (mAcchCir[i].ChkWallHit()) {
                return i;
            }
        }
    }
    return -1;
}

/* 0228E8D8 */
void daNpc_Md_c::NpcCall(be<s32>* r31) {
    if (!dComIfGs_isEventBit(UNK_1620)) {
        return;
    }
    f32 dist_sq = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
    if (!(mNoResetFlg1 & 2)) { /* !checkNpcCallCommand() */
        if (dist_sq < l_HIO().m0C8 * l_HIO().m0C8) {
            Link* player = link(daPy_getPlayerLinkActorClass());
            player->mNoResetFlg1 = player->mNoResetFlg1 | 2;
            *r31 = 1;
        }
    } else {
        f32 temp = l_HIO().m0C4 + l_HIO().m0C4;
        if (!(dist_sq < temp * temp)) {
            gabi::Local<ProcFunc_l> fn;
            copyAction(fn, action_searchNpcAction);
            setNpcAction(fn, nullptr);
        }
        *r31 = 1;
    }
}

/* 0228D950 */
BOOL daNpc_Md_c::isTagCheckOK() {
    BOOL ret = FALSE;
    if (md_chkAction(&mCurrNpcActionFunc, WAIT_NPC_ACTION /* waitNpcAction */) ||
        md_chkAction(&mCurrNpcActionFunc, SEARCH_NPC_ACTION /* searchNpcAction */)) {
        ret = TRUE;
    } else if (md_chkAction(&mCurrNpcActionFunc, CARRY_NPC_ACTION /* carryNpcAction */)) {
        /* !daPy_lk_c::checkCarryActionNow(): mCurProc (HD +0x65F0) not 0x72/0x6F/0x71 */
        s32 proc = (s32)link(daPy_getPlayerLinkActorClass())->mCurProc;
        if (proc != daPyProc_GRAB_PUT_e && proc != daPyProc_GRAB_UP_e && proc != daPyProc_GRAB_THROW_e) {
            ret = TRUE;
        }
    }
    return ret;
}

/* 0228DA10 */
void daNpc_Md_c::setMessageAnimation(u8 msgAnmAtr) {
    if (fopAcM_checkCarryNow(this)) {
        return;
    }
    if (!dComIfGp_event_runCheck()) {
        return;
    }
    u32 staffName = l_staff_name.mpName; /* l_staff_name */
    int staffIdx = dComIfGp_evmng_getMyStaffId(nameAt(staffName), nullptr, 0);
    if (staffIdx != -1) {
        u32 actName = md_actName(staffIdx);
        // HD: NULL check
        if (actName != 0 && md_streq(actName, gabi::guest_addr("WAIT"))) {
            int anmType = getAnmType(msgAnmAtr);
            if (anmType >= 0) {
                setAnm(anmType);
            }
        }
    }
}

/* 0228E9C0 */
void daNpc_Md_c::waitGroundCheck() {
    if (mAcch.ChkGroundHit()) {
        if (mType != 2) { /* !isTypeM_Dra09() */
            /* GetSpecialCode(mAcch.m_gnd): its cBgS_PolyInfo at mAcch+0xE8 */
            if (dBgS_GetSpecialCode(dComIfG_Bgsp(), gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) == 1) {
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
            copyAction(fn, action_fallNpcAction);
            setNpcAction(fn, nullptr);
        }
    }
}

/* 0228EACC */
BOOL daNpc_Md_c::chkAdanmaeDemoOrder() {
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

/* 0228EB50 */
BOOL daNpc_Md_c::XYTalkCheck() {
    BOOL ret = TRUE;
    if (checkStatus(daMdStts_XY_TALK)) {
        ret = md_presentEnd();
    }
    return ret;
}

/* 0228E37C */
void daNpc_Md_c::setNormalSpeedF(f32 f1, f32 f2, f32 i_scale, f32 i_maxStep, f32 i_minStep) {
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

/* 0228E4C0 */
void daNpc_Md_c::setSpeedAndAngleNormal(f32 f1, s16 r4) {
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

/* 0228E5CC */
void daNpc_Md_c::walkProc(f32 f1, s16 r3) {
    mMaxNormalSpeed = l_HIO().m008.m08;
    setSpeedAndAngleNormal(f1, r3);
    f32 temp = std::fabs(m3108 / mMaxNormalSpeed);
    f32 temp2 = temp * l_HIO().m008.m18;
    MdEvent::morf(mpMorf.get())->mPlaySpeed = temp2;    /* mpMorf->setPlaySpeed */
    MdEvent::morf(mpArmMorf.get())->mPlaySpeed = temp2; /* mpArmMorf->setPlaySpeed */
    /* setRunRate(temp) */

    mRunRate = temp;
    MdEvent::morf(mpMorf.get())->mAnmRate = temp; /* setAnmRate */
    MdEvent::morf(mpArmMorf.get())->mAnmRate = mRunRate;
    speedF = m3108;
}

/* 0228B610 */
BOOL daNpc_Md_c::actionMsgEndEvent(int staffIdx) {
    BOOL ret = talk(0);
    if (ret) {
        initialEndEvent(staffIdx);
    }
    lookBack(1, 0, 0);
    setAttention(true);
    return ret;
}

/* 0228B68C */
void daNpc_Md_c::initialMovePosEvent(int staffIdx) {
    // HD: preserve the complete event frame because strcpy can overwrite adjacent locals.
    gabi::Local<MoveFrame> frame;
    u32 buf = gabi::ea(frame->acStack_ac);
    fopAc_ac_c* link = nullptr;
    u32 target = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Target"));
    if (target != 0) {
        strcpy_g(buf, target);
        if (md_streq(buf, gabi::guest_addr("@PLAYER"))) {
            link = daPy_getPlayerLinkActorClass();
        }
        if (link != nullptr) {
            u32 type = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Type"));
            if (type != 0) {
                strcpy_g(buf, type);
            }
            if (md_streq(buf, gabi::guest_addr("LINK_DIST"))) {
                s16 uVar9 = fopAcM_searchActorAngleY(this, link);
                be<f32>* pDist = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Dist");
                if (pDist != nullptr) {
                    cXyz* pos = &frame->pos;
                    f32 dist = *pDist;
                    pos->y = current.pos.y + 50.0f;
                    pos->x = gabi::fmadds(dist, cM_ssin(uVar9), current.pos.x);
                    pos->z = gabi::fmadds(dist, cM_scos(uVar9), current.pos.z);
                    /* dBgS_GndChk local_a0 (inline constructor); local_a0.SetPos(&pos) */
                    dBgS_GndChk* check = &frame->local_a0;
                    u32 g = gabi::ea(check);
                    mdEvent_groundCheckCtor(g); /* cBgS_GndChk::cBgS_GndChk */
                    check->mGrp = 1;
                    check->dBgS_GrpPassChk::__vtbl = GROUP_PASS_VTABLE;
                    check->cBgS_PolyInfo::__vtbl = POLY_INFO_VTABLE;
                    check->dBgS_PolyPassChk::__vtbl = POLY_PASS_VTABLE;
                    check->cBgS_Chk::__vtbl = GROUND_VTABLE;
                    word(check->m_pos.x) = word(pos->x);
                    for (int i = 0; i < 7; i++) {
                        passFlags(check)[i] = 0;
                    }
                    word(check->m_pos.y) = word(pos->y);
                    word(check->m_pos.z) = word(pos->z);
                    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check));
                    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check));
                    f32 y = cBgS_GroundCross(dComIfG_Bgsp(), check);
                    if (y != -1000000000.0f) { /* -G_CM3D_F_INF */
                        pos->y = y;
                    } else {
                        pos->y = current.pos.y;
                    }
                    daPy_setPlayerPosAndAngle(link, pos, link->shape_angle.y);
                    /* ~dBgS_GndChk */
                    check->cBgS_PolyInfo::__vtbl = POLY_INFO_VTABLE;
                    check->dBgS_PolyPassChk::__vtbl = POLY_PASS_VTABLE;
                    check->dBgS_GrpPassChk::__vtbl = GROUP_PASS_BASE_VTABLE;
                    mdEvent_checkDtor(g, 0); /* cBgS_Chk::~cBgS_Chk */
                }
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Angle");
                if (pAngle != nullptr) {
                    daPy_setPlayerPosAndAngle(link, nullptr, (s16)(md_deg2s(*pAngle) + (-0x8000 + uVar9)));
                }
            } else if (md_streq(buf, gabi::guest_addr("DIST"))) {
                be<f32>* pDist = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Dist");
                if (pDist != nullptr) {
                    s16 a = link->shape_angle.y;
                    f32 dist = *pDist;
                    f32 x = gabi::fmadds(dist, cM_ssin(a), link->current.pos.x);
                    f32 z = gabi::fmadds(dist, cM_scos(a), link->current.pos.z);
                    current.pos.x = x;
                    current.pos.y = link->current.pos.y;
                    current.pos.z = z;
                }
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Angle");
                if (pAngle != nullptr) {
                    s16 angle = (s16)(md_deg2s(*pAngle) + (-0x8000 + link->shape_angle.y));
                    current.angle.y = angle;
                    shape_angle.y = angle;
                }
            } else {
                be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Angle");
                if (pAngle != nullptr) {
                    s16 angle = (s16)(md_deg2s(*pAngle) + link->shape_angle.y);
                    current.angle.y = angle;
                    shape_angle.y = angle;
                }
                cXyz* pPos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, "Pos");
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
        be<f32>* pAngle = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Angle");
        if (pAngle != nullptr) {
            s16 angle = md_deg2s(*pAngle);
            current.angle.y = angle;
            shape_angle.y = angle;
        }
        cXyz* pPos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, "Pos");
        if (pPos != nullptr) {
            current.pos.x = (f32)pPos->x;
            current.pos.y = (f32)pPos->y;
            current.pos.z = (f32)pPos->z;
            // HD: placed by the event "ARRIVAL_GND": start in the player's initial room
            SafeString* arrival = &frame->arrival;
            SafeString* evName = &frame->eventName;
            arrival->__vtbl = SAFE_STRING_VTABLE; /* this TU's sead::SafeString vtable */
            arrival->mStringTop = gabi::guest_addr("ARRIVAL_GND"); /* "ARRIVAL_GND" */
            u32 name = mdEvent_runEventName(dComIfGp_getPEvtManager()); /* dEvent_manager_c run event name */
            evName->__vtbl = SAFE_STRING_VTABLE;
            evName->mStringTop = name;
            if (md_SafeString_eq(arrival, evName)) {
                mdEvent_checkInitialRoom(this); /* daPy_npc_c::check_initialRoom */
                setAnm(0);
                m3131 = 1;
            }
        }
    }
    gravity = l_HIO().m0F4;
    maxFallSpeed = -100.0f;
    speedF = 0.0f;
    speed.y = 0.0f;
    u32 pName = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Name"));
    if (pName != 0) {
        strcpy_g(buf, pName);
        if (md_streq(buf, cut_anm_wait.mpName /* cut_anm_tbl[0] "WAIT" */)) {
            setAnm(cut_anm_wait_index); /* cut_anm_idx_tbl[0] */
        }
    }
    u32 pShipRide = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "ShipRide"));
    if (pShipRide != 0) {
        strcpy_g(buf, pShipRide);
        if (md_streq(buf, gabi::guest_addr("on"))) {
            setBitStatus(daMdStts_SHIP_RIDE);
        } else if (md_streq(buf, gabi::guest_addr("off"))) {
            clearStatus(daMdStts_SHIP_RIDE);
        }
    }
}

/* 0228C1C4 */
void daNpc_Md_c::initialFlyEvent(int) {
    setAnm(0x19);
    setBitStatus(daMdStts_UNK1);
    setWingEmitter();
    speed.y = 20.0f;
    gravity = l_HIO().m188;
    maxFallSpeed = 10.0f;
    m3118 = 60 * 30;
}

/* 0228C234 */
BOOL daNpc_Md_c::actionFlyEvent(int staffIdx) {
    be<f32>* p = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "HIGH");
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

/* 0228C338 */
void daNpc_Md_c::initialGlidingEvent(int) {
    gravity = l_HIO().m0F4;
    maxFallSpeed = -100.0f;
}

/* 0228C354 */
BOOL daNpc_Md_c::actionGlidingEvent(int) {
    if (mAcch.ChkGroundHit()) {
        /* fopAcM_monsSeStart(this, JA_SE_CV_MD_LANDING, &current.pos, reverb) */
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_monsSeStart4(JA_SE_CV_MD_LANDING, &current.pos, fopAcM_GetID(this), reverb);
        return TRUE;
    }
    if (!checkStatus(daMdStts_CAM_TAG_IN)) {
        speedF = 0.0f;
    }
    setAttention(true);
    return FALSE;
}

/* 0228C3E8 */
void daNpc_Md_c::initialLandingEvent(int) {
    maxFallSpeed = -100.0f;
    speedF = 0.0f;
    shape_angle.x = 0;
    shape_angle.z = 0;
    clearStatus(daMdStts_UNK1);
    setAnm(0x10);
    m312A = 0;
}

/* 0228C454 */
BOOL daNpc_Md_c::actionLandingEvent(int) {
    if (m312A != 0) {
        setAnm(0);
        return TRUE;
    }
    setAttention(true);
    return FALSE;
}

/* 0228C4A4 */
void daNpc_Md_c::initialWalkEvent(int) {
    // HD: also sets the wall circle (GameCube: speedF = 0, m3118 = 20 s only)
    m3118 = 20 * 30;
    speedF = 0.0f;
    mAcchCir[1].SetWall(60.0f, 20.0f);
}

/* 0228C50C */
BOOL daNpc_Md_c::actionWalkEvent(int staffIdx) {
    cXyz local_24;
    cXyz xz;
    cXyz eye;
    cXyz local_3c;
    cXyz* pfVar2 = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, "Pos");
    be<f32>* pfVar3 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "Speed");
    be<f32>* pfVar4 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "RunRate");
    if (pfVar2 != nullptr) {
        f32 dx, dz;
        if (md_walk_target(this, &local_24, pfVar2, &local_3c, &xz, &dx, &dz) < 10.0f) {
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
        md_walk_look(this, &local_24, &eye);
        setAttention(true);
        if (wallHitCheck() >= 0) {
            md_set_pos(this, &local_24);
            speedF = 0.0f;
            return TRUE;
        }
        if (cLib_calcTimer(&m3118) != 0) {
            return FALSE;
        }
        md_set_pos(this, &local_24);
        speedF = 0.0f;
    }
    return TRUE;
}

/* 0228C898 */
BOOL daNpc_Md_c::actionDashEvent(int staffIdx) {
    cXyz local_24;
    cXyz xz;
    cXyz eye;
    cXyz local_3c;
    cXyz* pfVar2 = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, "Pos");
    if (pfVar2 != nullptr) {
        f32 dx, dz;
        if (md_walk_target(this, &local_24, pfVar2, &local_3c, &xz, &dx, &dz) < 10.0f) {
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
        md_walk_look(this, &local_24, &eye);
        if (wallHitCheck() >= 0) {
            md_set_pos(this, &local_24);
            speedF = 0.0f;
            return TRUE;
        }
        setAttention(true);
        if (cLib_calcTimer(&m3118) != 0) {
            return FALSE;
        }
        md_set_pos(this, &local_24);
        speedF = 0.0f;
    }
    return TRUE;
}

/* 0228B418 */
void daNpc_Md_c::initialEndEvent(int staffIdx) {
    be<s32>* p = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "EndMode");
    s32 mode = p == nullptr ? 0 : (s32)*p;
    gabi::Local<ProcFunc_l> fn;
    switch (mode) {
    case -2:
        if (mMsgNo == 0x19DE) {
            dComIfGs_onEventBit(UNK_1620);
            dComIfGs_onEventBit(UNK_1608);
            dComIfGp_setNextStage("sea", 0xE3, 13 /* dIsleRoom_DragonRoostIsland_e */, 8, 0.0f, 0, 1, 0);
        }
        /* fall-through */
    case -1:
        dComIfGp_event_reset();
        if (checkStatus(daMdStts_UNK80)) {
            setHarpPlayNum(0);
        }
        copyAction(fn, action_waitNpcAction);
        setNpcAction(fn, nullptr);
        break;
    case 3:
        copyAction(fn, action_deleteNpcAction);
        setNpcAction(fn, nullptr);
        break;
    case 2:
        clearStatus(daMdStts_UNK1);
        copyAction(fn, action_squatdownNpcAction);
        setNpcAction(fn, nullptr);
        break;
    default:
        clearStatus(daMdStts_UNK1);
        if (checkStatus(daMdStts_UNK80)) {
            setHarpPlayNum(0);
        } else {
            setAnm(0);
        }
        copyAction(fn, action_waitNpcAction);
        setNpcAction(fn, nullptr);
        break;
    }
}

/* 0228CB88 */
BOOL daNpc_Md_c::actionTactEvent(int staffIdx) {
    be<s32>* pPrm0 = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "prm0");
    s32 prm = 0;
    if (pPrm0 != nullptr) {
        prm = *pPrm0;
    }
    /* daPy_py_c::getTactMusic(): virtual, vtable (+0xB4) slot +0x2C */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 song = tactMusic(player);
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

/* 0228CCC0 */
void daNpc_Md_c::initialTakeOffEvent(int) {
    setAnm(0x18);
    m312A = 0;
}

/* 0228CCF8 */
BOOL daNpc_Md_c::actionTakeOffEvent(int) {
    if (m312A != 0) {
        return TRUE;
    }
    setAttention(true);
    return FALSE;
}

/* 0228CD40 */
void daNpc_Md_c::initialOnetimeEvent(int staffIdx) {
    // HD: retain the complete frame for the event string copy.
    gabi::Local<OnetimeFrame> frame;
    u32 buf = gabi::ea(frame->acStack_18);
    s32 anm = 0x1A;
    u32 name = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Name"));
    if (name != 0) {
        strcpy_g(buf, name);
        /* strcmp(buf, cut_anm_tbl[0]) (inline byte loop) */
        u32 a = buf;
        u32 b = cut_anm_onetime.mpName;
        u8 ca, cb;
        do {
            ca = (u8)*bytes(a++);
            cb = (u8)*bytes(b++);
        } while (ca == cb && ca != 0);
        if (ca == cb) {
            anm = cut_anm_onetime_index; /* cut_anm_idx_tbl[0] */
        }
    }
    setAnm(anm);
    m312A = 0;
}

/* 0228CDFC */
BOOL daNpc_Md_c::actionOnetimeEvent(int) {
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

/* 0228CE84 */
void daNpc_Md_c::initialQuake(int staffIdx) {
    // HD: preserve the quake vector beside the event string buffer.
    gabi::Local<QuakeFrame> frame;
    u32 buf = gabi::ea(frame->acStack_1c);
    cXyz* tmp = &frame->quake;
    u32 src = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Mode"));
    if (src != 0) {
        strcpy_g(buf, src);
        if (md_streq(buf, gabi::guest_addr("Start"))) {
            tmp->x = 0.0f;
            tmp->y = 1.0f;
            tmp->z = 0.0f;
            dComIfGp_getVibration_StartQuake(7, 1, tmp);
        }
        if (md_streq(buf, gabi::guest_addr("Stop"))) {
            dComIfGp_getVibration_StopQuake(1);
        }
    }
}

/* 0228B3CC */
void daNpc_Md_c::setHarpPlayNum(int) {
    s32 i = cLib_getRndValue(0, 4);
    setAnm((s32)l_harp_play_anm.values[i]); /* l_harp_play_anm */
}

/* 0228D0A4 */
void daNpc_Md_c::initialHarpPlayEvent(int) {
    setHarpPlayNum(0);
}

/* 0228D0AC */
BOOL daNpc_Md_c::actionHarpPlayEvent(int) {
    if (m312A != 0) {
        setHarpPlayNum(0);
    }
    return daPy_lk_checkEndTactMusic(daPy_getPlayerLinkActorClass()) ? TRUE : FALSE;
}

/* 0228D0F0 */
void daNpc_Md_c::initialOffLinkEvent(int) {
    Link* player = link(daPy_getPlayerLinkActorClass());
    player->mNoResetFlg0 = player->mNoResetFlg0 | daPyFlg0_NO_DRAW;
}

/* 0228D120 */
void daNpc_Md_c::initialOnLinkEvent(int) {
    Link* player = link(daPy_getPlayerLinkActorClass());
    player->mNoResetFlg0 = player->mNoResetFlg0 & ~daPyFlg0_NO_DRAW;
}

/* 0228D150 */
void daNpc_Md_c::initialTurnEvent(int staffIdx) {
    be<s32>* t = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Timer");
    if (t != nullptr) {
        m3118 = (s16)*t;
    } else {
        m3118 = 0;
    }
    cXyz* pos = (cXyz*)dComIfGp_evmng_getMyVec3dP(staffIdx, "Pos");
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

/* 0228D32C */
BOOL daNpc_Md_c::actionTurnEvent(int staffIdx) {
    setAttention(true);
    // HD: retain the initialized target words and surrounding frame.
    gabi::Local<TurnFrame> frame;
    u32 buf = gabi::ea(&frame->local_20);
    u32 w2 = (u32)turnTarget.words[2];
    u32 w0 = (u32)turnTarget.words[0];
    u32 w1 = (u32)turnTarget.words[1];
    frame->local_20.words[0] = w0;
    frame->local_20.words[1] = w1;
    frame->local_20.words[2] = w2;
    u32 target = gabi::ea(dComIfGp_evmng_getMyStringP(staffIdx, "Target"));
    if (target != 0) {
        strcpy_g(buf, target);
    }
    be<s32>* type = (be<s32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "Type");
    int iVar4 = type == nullptr ? 0 : (s32)*type;
    dNpc_HIO_l& npc = l_HIO().mNpc;
    mJntCtrl.setParam(npc.mMaxBackboneX, npc.mMaxBackboneY, npc.mMinBackboneX, npc.mMinBackboneY, npc.mMaxHeadX,
                      npc.mMaxHeadY, -500, npc.mMinHeadY, npc.mMaxTurnStep);
    if (iVar4 == 2) {
        mJntCtrl.mbTrn = 1; /* setTrn() */
        iVar4 = 0;
    }
    if (md_streq(buf, gabi::guest_addr("@PLAYER"))) {
        lookBack(1, 0, iVar4);
    } else {
        lookBack(&m30B8, 1, iVar4);
    }
    if (cLib_calcTimer(&m3118) != 0) {
        return FALSE;
    }
    return TRUE;
}

/* 0228D564 */
void daNpc_Md_c::initialSetAnmEvent(int staffIdx) {
    u32 puVar1 = gabi::ea(dComIfGp_evmng_getMyIntegerP(staffIdx, "Number"));
    if (puVar1 != 0) {
        u8 r4 = (u8)bytes(puVar1)[3]; /* (u8)*puVar1 */
        if (setAnm(getAnmType(r4))) {
            setBitStatus(daMdStts_UNK1);
        } else {
            clearStatus(daMdStts_UNK1);
        }
    }
    speedF = 0.0f;
    be<f32>* pfVar2 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "PlaySpeed");
    if (pfVar2 != nullptr) {
        MdEvent::morf(mpMorf.get())->mPlaySpeed = *pfVar2;    /* setPlaySpeed */
        MdEvent::morf(mpArmMorf.get())->mPlaySpeed = *pfVar2;
        MdEvent::morf(mpWingMorf.get())->mPlaySpeed = *pfVar2;
        if (*pfVar2 < 0.0f) {
            toEnd(MdEvent::morf(mpMorf.get())->mFrame, MdEvent::morf(mpMorf.get())->mEndFrame);
            toEnd(MdEvent::morf(mpArmMorf.get())->mFrame, MdEvent::morf(mpArmMorf.get())->mEndFrame);
            toEnd(MdEvent::morf(mpWingMorf.get())->mFrame, MdEvent::morf(mpWingMorf.get())->mEndFrame);
        }
        loopAtFrame(MdEvent::morf(mpMorf.get())->mLoopFrame, MdEvent::morf(mpMorf.get())->mFrame);
        loopAtFrame(MdEvent::morf(mpArmMorf.get())->mLoopFrame, MdEvent::morf(mpArmMorf.get())->mFrame);
        loopAtFrame(MdEvent::morf(mpWingMorf.get())->mLoopFrame, MdEvent::morf(mpWingMorf.get())->mFrame);
        m30D0 = (f32)MdEvent::morf(mpMorf.get())->mFrame; /* mpMorf->getFrame() */
    }
    pfVar2 = (be<f32>*)dComIfGp_evmng_getMyFloatP(staffIdx, "MorfFrame");
    if (pfVar2 != nullptr) {
        mdEvent_setMorf2(mpMorf.get(), (f32)*pfVar2);    /* mDoExt_McaMorf2::setMorf */
        mdEvent_setMorf2(mpArmMorf.get(), (f32)*pfVar2);
        mdEvent_setMorf(mpWingMorf.get(), (f32)*pfVar2); /* mDoExt_McaMorf::setMorf */
    }
    /* clearJntAng() */
    mJntCtrl.mAngles[0][1] = 0;
    mJntCtrl.mAngles[0][0] = 0;
    mJntCtrl.mAngles[1][1] = 0;
    mJntCtrl.mAngles[1][0] = 0;
}

/* 0228D840 */
u32 daNpc_Md_c::initialLookDown(int) {
    return setAnm(0x2F);
}

/* 0228D948 */
u32 daNpc_Md_c::initialLookUp(int) {
    return setAnm(5);
}

/* 0228D848 */
BOOL daNpc_Md_c::actionLookDown(int) {
    cXyz local_14;
    cXyz local_38;
    cXyz cStack_20;
    f32 x = current.pos.x;
    f32 y = eyePos.y;
    f32 z = current.pos.z;
    s16 desiredYRot = shape_angle.y;
    m30AC.y = -50.0f;
    /* local_38.set(...); &local_14 = &local_38 */
    local_14.x = x;
    local_14.y = y;
    local_14.z = z;
    Mtx34* stack = &mtxStack; /* mDoMtx_stack_c::now */
    mDoMtx_YrotS(stack, desiredYRot);
    PSMTXMultVec(stack, &m30AC, &cStack_20);
    PSVECAdd(&local_14, &cStack_20, &local_14);
    local_38.x = x;
    local_38.y = y;
    local_38.z = z;
    dNpc_JntCtrl_lookAtTarget(&mJntCtrl, &shape_angle.y, &local_14, &local_38, desiredYRot, m3110, false);
    return TRUE;
}

/* 0228B1B8 */
BOOL daNpc_Md_c::talk(int r4) {
    // HD: message status and sending use the HD manager instead of GameCube msg_class.
    u32 mng = l_msgMng();
    u16 msgStatus = fopMsgM_getStatus(mng);
    u8 msgAnmAtr = (u8)play()->mMesgAnimeAttrInfo; /* dComIfGp_getMesgAnimeAttrInfo() */
    if (msgStatus == MSG_DISPLAYED) { /* fopMsgStts_MSG_DISPLAYED_e */
        /* next_msgStatus's u16 result is passed on as the full register (GHS does not re-extend) */
        fopMsgM_setStatus(mng, mdEvent_nextMessageStatus(this, &mMsgNo));
        if (fopMsgM_getStatus(mng) == fopMsgStts_MSG_CONTINUES_e) { /* fopMsgStts_MSG_CONTINUES_e */
            fopMsgM_messageSet(mng, mMsgNo, nullptr);
            m313A = 0;
        }
    } else if (msgStatus == INPUT) { /* fopMsgStts_INPUT_e */
        if (r4 != 0) {
            if ((u8)play()->mMesgCancelButton != 0) { /* dComIfGp_checkMesgCancelButton() */
                fopMsgM_setStatus(mng, fopMsgStts_MSG_ENDS_e); /* fopMsgStts_MSG_ENDS_e */
                fopMsgM_messageSendOn(mng);
                setBitStatus(daMdStts_UNK200);
            }
            if (checkStatus(daMdStts_UNK400)) {
                fopMsgM_setStatus(mng, fopMsgStts_MSG_ENDS_e);
                fopMsgM_messageSendOn(mng);
            }
        }
    } else if (msgStatus == MSG_TYPING) { /* fopMsgStts_MSG_TYPING_e */
        if (m313A == 0 && !fopAcM_checkCarryNow(this) && !checkStatus(daMdStts_SHIP_RIDE) && mMsgNo != 0x05AC) {
            int anmType = getAnmType(msgAnmAtr);
            if (anmType >= 0) {
                setAnm(anmType);
            }
        }
    } else if (msgStatus == fopMsgStts_BOX_CLOSED_e) { /* fopMsgStts_BOX_CLOSED_e */
        fopMsgM_setStatus(mng, MSG_DESTROYED); /* fopMsgStts_MSG_DESTROYED_e */
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

/* 0228DAF4 */
bool daNpc_Md_c::chkAttention(cXyz* pos, s16 angle, int param_3) {
    fopAc_ac_c* link = daPy_getPlayerLinkActorClass();
    f32 maxAttnDistXZ = l_HIO().mNpc.mMaxAttnDistXZ;
    f32 dVar8 = l_HIO().m0C0;
    int maxAttnAngleY = l_HIO().mNpc.mMaxAttnAngleY;
    f32 dVar9 = l_HIO().m0BC;
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        maxAttnDistXZ = l_HIO().m074.m04;
    } else if (mType == 2 && dComIfGs_isEventBit(UNK_1101)) { /* isTypeM_Dra09() */
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

/* 0228DD60 */
bool daNpc_Md_c::chkArea(cXyz* param_1) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 dz = player->current.pos.z - param_1->z;
    f32 dx = player->current.pos.x - param_1->x;
    f32 maxDist = l_HIO().mNpc.mMaxAttnDistXZ;
    f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    maxDist += 40.0f;
    return maxDist > dist;
}

/* 0228AB60 */
u16 daNpc_Md_c::next_msgStatus(be<u32>* pCurrMsgNo) {
    // HD: several GameCube intermediate messages fall directly through to message end.
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e /* fopMsgStts_MSG_CONTINUES_e */;
    switch (*pCurrMsgNo) {
        case 0x5AC:
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(MEDLI_GAVE_FATHERS_LETTER);
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0x17DD:
            *pCurrMsgNo = 0x17DE;
            break;
        case 0x17DE:
            mCurEventMode = 5;
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_1101);
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0x17E5:
            dComIfGs_onEventBit(UNK_1280);
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            if ((s32)message(l_msgMng())->mSelectNum == 1 /* mSelectNum */) {
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
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0x17F3:
            *pCurrMsgNo = 0x17F4;
            break;
        case 0x17F4:
            dComIfGs_onEventBit(UNK_1104);
            // Fall-through
        case 0x17F8:
            setBitStatus(daMdStts_UNK40);
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_1102);
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_1402);
            m_seaTalk = 1; /* onSeaTalk() */
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_1504);
        case 0x19D9:
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_2E40);
            m_seaTalk = 1; /* onSeaTalk() */
            // Fall-through
        case 0x1A01:
            dComIfGs_onEventBit(UNK_2C08);
            msgStatus = fopMsgStts_MSG_ENDS_e;
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
            dComIfGs_onEventBit(UNK_3B80);
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
    }
    return msgStatus;
}

/* 0228DDEC */
u32 daNpc_Md_c::getMsg() {
    u32 msgNo = 0;
    if (checkStatus(daMdStts_SHIP_RIDE)) {
        msgNo = 0x19EF;
    } else if (mType == 0) { /* isTypeAtorizk() */
        if (dComIfGs_isEventBit(MEDLI_GAVE_FATHERS_LETTER /* MEDLI_GAVE_FATHERS_LETTER */)) {
            msgNo = 0x17DC;
        } else if ((u8)saveInfo.mpInfo->mClearCount != 0) { /* dComIfGs_getClearCount() */
            msgNo = 0x1805;
        } else {
            msgNo = 0x17D5;
        }
    } else if (mType == 1) { /* isTypeAdanmae() */
        if (checkStatus(daMdStts_UNK40)) {
            if (m312D == 0x10) {
                u8 count = m313C; /* getPiyo2TalkCNT() */
                msgNo = (u32)l_msg_num.values[count]; /* l_msg_num */
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
        } else if (dComIfGs_isEventBit(UNK_1104)) {
            msgNo = 0x17F7;
        } else {
            msgNo = 0x17E6;
        }
    } else if (mType == 2) { /* isTypeM_Dra09() */
        if (dComIfGs_isEventBit(UNK_1101)) {
            msgNo = 0x17E5;
        } else {
            msgNo = 0x17DD;
        }
    } else if (mType == 3) { /* isTypeSea() */
        if (checkStatus(daMdStts_XY_TALK)) {
            if (checkStatus(daMdStts_DEFAULT_TALK_XY)) {
                if (dComIfGs_isEventBit(UNK_2E40)) {
                    msgNo = 0x1A00;
                } else if (dComIfGs_isEventBit(UNK_2C08)) {
                    msgNo = 0x19F8;
                } else {
                    msgNo = 0x19FC;
                }
            } else {
                if (dComIfGs_isEventBit(UNK_1504)) {
                    msgNo = 0x19D9;
                } else {
                    msgNo = 0x19D6;
                }
            }
        } else {
            if (dComIfGs_isEventBit(UNK_1402)) {
                if (m_seaTalk != 0) { /* isSeaTalk() */
                    msgNo = 0x19D2;
                } else {
                    msgNo = 0x19F6;
                    m_seaTalk = 1; /* onSeaTalk() */
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

/* 0228D208 */
void MdEventLookActor::lookBack(cXyz* param_1, int param_2, int param_3) {
    cXyz local_30;
    cXyz local_3c;
    cXyz* dstPos = nullptr;
    f32 x = 0.0f, y = 0.0f, z = 0.0f; /* local_3c.setall(0.0f) */
    s16 desiredYRot = shape_angle.y;
    if (mJntCtrl.mbTrn != 0) { /* mJntCtrl.trnChk() */
        cLib_addCalcAngleS2(&m3110, l_HIO().mNpc.mMaxHeadTurnVel, 4, 0x800);
    } else {
        m3110 = 0;
    }
    if (param_2 != 0) {
        /* &local_30 = *param_1 (integer copy) */
        word(local_30.z) = word(param_1->z);
        word(local_30.x) = word(param_1->x);
        word(local_30.y) = word(param_1->y);
        dstPos = &local_30;
        x = current.pos.x;
        y = eyePos.y;
        z = current.pos.z;
    }
    local_3c.x = x;
    local_3c.y = y;
    local_3c.z = z;
    dNpc_JntCtrl_lookAtTarget(&mJntCtrl, &shape_angle.y, dstPos, &local_3c, desiredYRot, m3110, param_3 != 0);
}

/* 0228DFF8 */
s32 daNpc_Md_c::lookBackWaist(s16 param_1, f32 param_2) {
    s16 iVar2 = (s16)(param_1 - shape_angle.y);
    f32 sn = cM_ssin(iVar2);
    f32 cs = cM_scos(iVar2);
    f32 fVar1 = cs * param_2 * l_HIO().m034.m04;
    f32 dVar6 = sn * param_2 * l_HIO().m034.m08;
    // HD: an option of the save data (027200D0(save + 0x12C0) + 2) == 0 mirrors the waist turn
    u32 opt = mdEvent_option(gabi::ea(&saveInfo.mpInfo->mOption));
    if ((u8)bytes(opt)[2] == 0) {
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
    // HD: while m310C > 0.9, a body turn towards a target far to the side
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

/* 0228C104 */
void daNpc_Md_c::particle_set(be<u32>* pEmitter, u16 particleID) {
    if (*pEmitter != 0) {
        return;
    }
    s8 roomNo = fopAcM_GetRoomNo(this);
    GXColor* k0 = &tevStr.mColorK0; /* tevStr.mColorK0 */
    JPABaseEmitter* e = dComIfGp_particle_set(particleID, &current.pos, &current.angle, nullptr, 0xFF, nullptr, roomNo, k0, k0);
    *pEmitter = gabi::ea(e);
    if (e != nullptr) {
        emitter(e)->mFlags = emitter(e)->mFlags | 0x40;
    }
}

/* WWHD glue
data saveInfo 101F84DC
data messageManager 101F4B5C
data m_seaTalk 101D5F40
data l_harp_play_anm 1001DDF0
data l_msg_num 1001DF4C
data l_staff_name 101C0280
data cut_anm_wait 101C1EE4
data cut_anm_onetime 101C1EE8
data cut_anm_wait_index 1001DE24
data cut_anm_onetime_index 1001DEC0
data action_mkamaePlayerAction 1001D4A0
data action_flyPlayerAction 1001D4A8
data action_searchNpcAction 1001D4D0
data action_fallNpcAction 1001D4C0
data action_waitNpcAction 1001D490
data action_deleteNpcAction 1001D568
data action_squatdownNpcAction 1001D4F8
data turnTarget 1001DF10
data mtxStack 1048D0CC
str 1001DE08 "EndMode" in 0228B418
str 1001DE10 "sea" in 0228B418
str 1001DE1C "@PLAYER" in 0228B68C
str 1001DE28 "Pos" in 0228B68C
str 1001DE2C "off" in 0228B68C
str 1001DE30 "Angle" in 0228B68C
str 1001DE38 "Target" in 0228B68C
str 1001DE40 "Type" in 0228B68C
str 1001DE48 "Dist" in 0228B68C
str 1001DE50 "DIST" in 0228B68C
str 1001DE58 "Name" in 0228B68C
str 1001DE60 "on" in 0228B68C
str 1001DE64 "ARRIVAL_GND" in 0228B68C
str 1001DE70 "LINK_DIST" in 0228B68C
str 1001DE7C "ShipRide" in 0228B68C
str 1001DE94 "HIGH" in 0228C234
str 1001DE9C "RunRate" in 0228C50C
str 1001DEA4 "Pos" in 0228C50C
str 1001DEA8 "Speed" in 0228C50C
str 1001DEB4 "Pos" in 0228C898
str 1001DEB8 "prm0" in 0228CB88
str 1001DEC4 "Name" in 0228CD40
str 1001DED4 "Start" in 0228CE84
str 1001DEDC "Mode" in 0228CE84
str 1001DEE4 "Stop" in 0228CE84
str 1001DEEC "Pos" in 0228D150
str 1001DEF0 "Timer" in 0228D150
str 1001DEF8 "@PLAYER" in 0228D32C
str 1001DF00 "Target" in 0228D32C
str 1001DF08 "Type" in 0228D32C
str 1001DF1C "Number" in 0228D564
str 1001DF24 "PlaySpeed" in 0228D564
str 1001DF30 "MorfFrame" in 0228D564
str 1001DF40 "WAIT" in 0228DA10
ext 02055B64 s16 mdEvent_calcTimer(be<s16>* timer)
ext 021E1E78 s32 mdEvent_getRndValue(s32 start, s32 range)
ext 025E1A7C void mdEvent_monsSeStart(u32 id, cXyz* pos, u32 pid, s32 reverb)
ext 0244307C bool mdEvent_checkEndTactMusic(fopAc_ac_c* player)
ext 028F040C void mdEvent_strcpy(u32 dst, u32 src)
ext 02007A1C s16 mdEvent_stickAngle(s32 port)
ext 02007898 bool mdEvent_trigA(s32 port)
ext 020078BC BOOL mdEvent_trigB(s32 port)
ext 024F8018 s16 mdEvent_cameraAngleY(u32 camera)
ext 024EE058 u32 mdEvent_lockonList(dAttention_c* attention, s32 index)
ext 024EE020 u32 mdEvent_actionList(dAttention_c* attention, s32 index)
ext 024EDFCC bool mdEvent_lockonTruth(dAttention_c* attention)
ext 024EBA14 fopAc_ac_c* mdEvent_listActor(u32 list)
ext 0200ECD4 f32 mdEvent_addCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 024EF09C s32 mdEvent_specialCode(dBgS* bgs, u32 poly)
ext 02544950 bool mdEvent_presentEnd(dEvent_manager_c* manager)
ext 025F795C u16 mdEvent_messageStatus(u32 manager)
ext 025F74D0 void mdEvent_setMessageStatus(u32 manager, u32 status)
ext 025F7DB0 u32 mdEvent_messageSet(u32 manager, u32 number, cXyz* pos)
ext 025DF2B8 s16 mdEvent_priority(void* actor)
ext 025DA874 void mdEvent_drawQueue(u32 tag, s16 priority)
ext 0252012C void mdEvent_nextStage(const char* stage, s16 point, s8 room, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe)
ext 025CB408 BOOL mdEvent_startQuake(dVibration_c* vibration, s32 mode, s32 id, cXyz* pos)
ext 025CB610 BOOL mdEvent_stopQuake(dVibration_c* vibration, s32 id)
ext 025447EC u32 mdEvent_actName(dEvent_manager_c* manager, s32 staff)
ext 0259DED0 void mdEvent_lookAtTarget(dNpc_JntCtrl_c* joint, be<s16>* angle, cXyz* target, cXyz* eye, s16 rotation, s16 velocity, bool headOnly)
ext 0228AB60 u32 mdEvent_nextMessageStatus(daNpc_Md_c* actor, be<u32>* number)
ext 025E5D1C void mdEvent_setMorf2(mDoExt_McaMorf2* morf, f32 frame)
ext 025E4A54 void mdEvent_setMorf(mDoExt_McaMorf* morf, f32 frame)
ext 027200D0 u32 mdEvent_option(u32 option)
ext 02008E0C void mdEvent_groundCheckCtor(u32 check)
ext 02008DAC void mdEvent_checkDtor(u32 check, s32 flag)
ext 02542E94 u32 mdEvent_runEventName(dEvent_manager_c* manager)
ext 024451B4 s32 mdEvent_checkInitialRoom(daNpc_Md_c* actor)
*/
