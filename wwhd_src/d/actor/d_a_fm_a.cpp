/**
 * d_a_fm_a.cpp (WWHD)
 * Enemy - Floormaster, part A (02142988..02145070): attack data, grab helpers, collision, execute,
 * draw, the hide/under-foot/path/goal-keeper modes, isGrabFoot and moveRndBack.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fm_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* functions of the other d_a_fm parts, by address (matcher names in this unit are wrong, see the report) */
static inline void fm_setAnm(daFm_c* a, s8 idx, bool force) { gabi::call(0x021418D0, a, (s32)idx, (u32)force); }
static inline bool fm_setHoleScale(daFm_c* a, f32 x, f32 y, f32 z) { return gabi::call<bool>(0x02142450, a, x, y, z); }
static inline void fm_modeProc(daFm_c* a, s32 proc, s32 mode) { gabi::call(0x021412BC, a, proc, mode); }
static inline void fm_holeExecute(daFm_c* a) { gabi::call(0x02142200, a); }
static inline void fm_iceProc(daFm_c* a) { gabi::call(0x021424D0, a); }
static inline void fm_setBaseTarget(daFm_c* a) { gabi::call(0x02141714, a); }
static inline void fm_bodySetMtx(daFm_c* a) { gabi::call(0x0214153C, a); }
static inline void fm_holeSetMtx(daFm_c* a) { gabi::call(0x0214162C, a); }
static inline u8 fm_areaCheck(daFm_c* a) { return gabi::call<u8>(0x0214264C, a); }
static inline u32 fm_lineCheck(daFm_c* a, cXyz* p1, cXyz* p2) { return gabi::call<u32>(0x021425BC, a, p1, p2); }
static inline u32 fm_isLink(daFm_c* a, fopAc_ac_c* t) { return gabi::call<u32>(0x02141280, a, t); }
static inline void fm_setAttention(daFm_c* a) { gabi::call(0x021422A4, a); }
static inline void fm_spAttackVJump(daFm_c* a) { gabi::call(0x02142870, a); }
static inline void fm_spAttackJump(daFm_c* a) { gabi::call(0x021428FC, a); }
/* 020CB92C daBomb_c::chk_state(state) */
static inline u32 daBomb_chk_state(fopAc_ac_c* b, u32 st) { return gabi::call<u32>(0x020CB92C, b, st); }
/* 025E535C mDoExt_McaMorf::play(cXyz*, u32, s8) */
static inline void morf_play(mDoExt_McaMorf* m, cXyz* pos, u32 a, s32 b) { gabi::call(0x025E535C, m, pos, a, b); }
/* 028E9BC0 C_QUATSlerp(p, q, r, t) */
static inline void C_QUATSlerp(Quaternion_fm* p, u32 q, Quaternion_fm* r, f32 t) { gabi::call(0x028E9BC0, p, q, r, t); }
#define ZeroQuat 0x101E9C38u
static inline void morf_calc(mDoExt_McaMorf* m) { gabi::call(0x025E55A0, m); }

enum { fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F };

/* daPy_lk_c virtuals through the HD vtable (+0xB4): checkPlayerFly +0x4C, checkFrontRoll +0x5C,
 * getGrabActorID +0xBC (results tested as full words) */
static inline s32 daPy_vcall(fopAc_ac_c* p, u32 off) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + off), p); }
static inline s32 daPy_checkPlayerFly(fopAc_ac_c* p) { return daPy_vcall(p, 0x4C); }
static inline s32 daPy_checkFrontRoll(fopAc_ac_c* p) { return daPy_vcall(p, 0x5C); }
static inline s32 daPy_getGrabActorID(fopAc_ac_c* p) { return daPy_vcall(p, 0xBC); }
/* 02587D24 dLib_pathMove(cXyz* pos, s8* pntNo, dPath*, f32 speed, cb, void* data) */
static inline void dLib_pathMove(cXyz* pos, be<s8>* pnt, dPath* path, f32 spd, u32 cb, void* data) {
    gabi::call(0x02587D24, pos, pnt, path, spd, cb, data);
}
#define pathMove_CB 0x02140CA8u
/* 025BEBB8 dSnap_RegistFig(u8 type, fopAc_ac_c*, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_pos(u32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angY, x, y, z);
}
enum { DSNAP_TYPE_FM = 0xB5 };
enum { fpcNm_BOMB_e = 0x126, fpcNm_TSUBO_e = 0x1C5, FM_JNT_TE_e = 5 };
/* 02048038 daObj::PrmAbstract(actor, width, shift) */
static inline u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh) { return gabi::call<u32>(0x02048038, a, w, sh); }
/* 02018F3C cM3dGSph::SetC(f32, f32, f32) */
static inline void cM3dGSph_SetC_xyz(cM3dGSph* s, f32 x, f32 y, f32 z) { gabi::call(0x02018F3C, s, x, y, z); }
/* 0x101FF558 g_Counter.mCounter0 (frame counter) */
static inline u32 g_Counter0() { return gabi::load<u32>(0x101FF558); }
/* daPy_py_c::setPlayerPosAndAngle(MtxP) through the HD vtable (+0x124) */
static inline void daPy_setPlayerPosAndAngle_mtx(fopAc_ac_c* p, Mtx34* m) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 0xB4) + 0x124), p, m);
}
/* daPy_py_c::getHeadTopPos (+0x3D8) */
static inline cXyz* fm_headTopPos(fopAc_ac_c* p) { return gabi::at<cXyz>(gabi::ea(p) + 0x3D8); }
/* cXyz::abs() on a copy: sqrtf(PSVECSquareMag) */
static inline f32 fm_abs_copy(cXyz* v) {
    gabi::Local<cXyz> c;
    c->copy(*v);
    return std_sqrtf(PSVECSquareMag(c.get()));
}
/* mDoMtx_stack_c::multVecZero(out): the translation column (float copies, bit-exact) */
static inline void fm_multVecZero(cXyz* out) {
    u32 m = gabi::ea(mDoMtx_stack_c::get());
    gabi::store<u32>(gabi::ea(out) + 0, gabi::load<u32>(m + 0x0C));
    gabi::store<u32>(gabi::ea(out) + 4, gabi::load<u32>(m + 0x1C));
    gabi::store<u32>(gabi::ea(out) + 8, gabi::load<u32>(m + 0x2C));
}
static inline bool dComIfGp_checkPlayerStatus0(u32 bits) { return (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & bits) != 0; }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* (a - b).absXZ(): cXyz::operator- into a temporary, then sqrtf(PSVECSquareMag({x, 0, z})) */
static inline f32 fm_absXZ_mi(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    cXyz_mi(a, diff.get(), b);
    gabi::Local<cXyz> xz;
    gabi::store<f32>(gabi::ea(xz.get()) + 4, 0.0f);
    gabi::store<u32>(gabi::ea(xz.get()) + 0, gabi::load<u32>(gabi::ea(diff.get()) + 0));
    gabi::store<u32>(gabi::ea(xz.get()) + 8, gabi::load<u32>(gabi::ea(diff.get()) + 8));
    return std_sqrtf(PSVECSquareMag(xz.get()));
}

/* ---- functions ---- */

/* 02142988 */
void daFm_c::spAttackNone() {
    WWHD_FUNC(0x02142988, void, this);
    /* setBtAttackData(100.0f, 100.0f, 10000.0f, 0); setBtNowFrame(0.0f) */
    mBtStartFrame = 100.0f;
    mBtNowFrame = 0.0f;
    mBtEndFrame = 100.0f;
    mBtAttackType = 0;
    mBtMaxDis = 10000.0f;
}
VERIFY(0x02142988, &daFm_c::spAttackNone);

/* 02142BC4 */
bool daFm_c::isNpc(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02142BC4, bool, this, i_actor);
    if (i_actor == nullptr) {
        return false;
    }
    /* HD: fopAcM_GetName checks for NULL (inline, twice) */
    return (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_CB1_e) ||
           (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_NPC_MD_e);
}
VERIFY(0x02142BC4, &daFm_c::isNpc);

/* 02142C0C */
void daFm_c::MtxToRot(Mtx34* i_mtx, csXyz* angle) {
    WWHD_FUNC(0x02142C0C, void, this, i_mtx, angle);
    gabi::Local<Mtx34> mtx;
    mDoMtx_inverseTranspose(i_mtx, mtx);
    mDoMtx_MtxToRot(mtx, angle);
}
VERIFY(0x02142C0C, &daFm_c::MtxToRot);

/* 0214399C */
static bool daFmExecute(daFm_c* i_this) {
    WWHD_FUNC(0x0214399C, bool, i_this);
    return i_this->_execute();
}
VERIFY(0x0214399C, daFmExecute);

/* 0214418C */
static bool daFmDraw(daFm_c* i_this) {
    WWHD_FUNC(0x0214418C, bool, i_this);
    return i_this->_draw();
}
VERIFY(0x0214418C, daFmDraw);

/* 02144190 */
void daFm_c::modeSwWaitInit() {
    WWHD_FUNC(0x02144190, void, this);
    fm_setAnm(this, 8, false);
    field_0x3E0 = hio_f(0x120);
}
VERIFY(0x02144190, &daFm_c::modeSwWaitInit);

/* 021441D0 */
void daFm_c::resetInvKine() {
    WWHD_FUNC(0x021441D0, void, this);
    if (isBodyAppear()) {
        field_0x390 = 0;
        for (int i = 0; i < 6; i++) {
            C_QUATSlerp(&field_0x330[i], ZeroQuat, &field_0x330[i], 0.05f);
        }
        morf_calc(mpMorf);
    }
}
VERIFY(0x021441D0, &daFm_c::resetInvKine);

/* 021444FC */
int daFm_c::setRnd(int param_1, int param_2) {
    WWHD_FUNC(0x021444FC, int, this, param_1, param_2);
    f32 rnd = cM_rndF(std::fabs((f32)(param_2 - param_1)));
    return param_1 + gabi::ftoi(rnd);
}
VERIFY(0x021444FC, &daFm_c::setRnd);

/* 02144568 */
void daFm_c::modeHideInit() {
    WWHD_FUNC(0x02144568, void, this);
    fm_setAnm(this, 8, false);
    field_0x650 = setRnd(hio_s(0x084), hio_s(0x086));
}
VERIFY(0x02144568, &daFm_c::modeHideInit);

/* 021445B8 */
void daFm_c::modeHide() {
    WWHD_FUNC(0x021445B8, void, this);
    field_0x690.copy(current.pos);
    resetInvKine();
    if (fm_setHoleScale(this, hio_f(0x120), 0.1f, hio_f(0x128))) {
        current.pos.x = mBaseTarget->current.pos.x;
        current.pos.z = mBaseTarget->current.pos.z;
        if (cLib_calcTimer(&field_0x650) == 0) {
            /* HD: field_0xAE4 != 0 (GameCube: == 1) */
            if (!(std::fabs(field_0x69C.y - mBaseTarget->current.pos.y) > hio_f(0x0A8)) && field_0xAE4 != 0) {
                fm_modeProc(this, PROC_INIT_e, 2);
            }
        }
    }
}
VERIFY(0x021445B8, &daFm_c::modeHide);

/* 0214468C */
void daFm_c::modeUnderFootInit() {
    WWHD_FUNC(0x0214468C, void, this);
    fm_setAnm(this, 8, false);
    field_0x650 = hio_s(0x066);
    current.pos.x = mBaseTarget->current.pos.x;
    current.pos.z = mBaseTarget->current.pos.z;
    field_0x3E0 = hio_f(0x120);
}
VERIFY(0x0214468C, &daFm_c::modeUnderFootInit);

/* 021446EC */
bool daFm_c::isLinkControl() {
    WWHD_FUNC(0x021446EC, bool, this);
    fopAc_ac_c* p0 = dComIfGp_getPlayer(0);
    return p0 != dComIfGp_getLinkPlayer();
}
VERIFY(0x021446EC, &daFm_c::isLinkControl);

/* 021449C4 */
void daFm_c::modePathMoveInit() {
    WWHD_FUNC(0x021449C4, void, this);
    fm_setAnm(this, 8, false);
    field_0x3E0 = hio_f(0x124);
}
VERIFY(0x021449C4, &daFm_c::modePathMoveInit);

/* 02144A04 */
void daFm_c::turnToBaseTarget() {
    WWHD_FUNC(0x02144A04, void, this);
    cLib_addCalcAngleS2(&shape_angle.y, field_0x9D0, 4, 0x800);
}
VERIFY(0x02144A04, &daFm_c::turnToBaseTarget);

/* 02144B54 */
void daFm_c::modeGoalKeeperInit() {
    WWHD_FUNC(0x02144B54, void, this);
    fm_setAnm(this, 8, false);
    field_0x3E0 = hio_f(0x124);
}
VERIFY(0x02144B54, &daFm_c::modeGoalKeeperInit);

/* 02144D88 */
void daFm_c::modeAppearInit() {
    WWHD_FUNC(0x02144D88, void, this);
    fm_setAnm(this, 2, false);
    field_0x660.copy(current.pos);
    shape_angle.y = field_0x9D0;
    field_0x650 = 0x1E;
}
VERIFY(0x02144D88, &daFm_c::modeAppearInit);

/* 0214472C (USA path: the link player is used throughout) */
bool daFm_c::isGrabFoot() {
    WWHD_FUNC(0x0214472C, bool, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    if (std::fabs(current.pos.y - pLink->current.pos.y) > 10.0f) {
        return false;
    }
    if (fopAcM_searchActorDistanceXZ(this, pLink) > hio_f(0x0DC)) {
        return false;
    }
    if (pLink->speedF > 4.0f) {
        return false;
    }
    if (daPy_checkPlayerFly(pLink) || daPy_checkFrontRoll(pLink) || daPy_getGrabActorID(pLink) != -1 || isLinkControl()) {
        return false;
    }
    return true;
}
VERIFY(0x0214472C, &daFm_c::isGrabFoot);

/* 02144838 */
void daFm_c::modeUnderFoot() {
    WWHD_FUNC(0x02144838, void, this);
    if (cLib_calcTimer(&field_0x648) == 0) {
        field_0x660.copy(mBaseTarget->current.pos);
        field_0x648 = hio_s(0x08A);
    }
    cLib_addCalcPosXZ2(&current.pos, &field_0x660, 0.1f, hio_f(0x0C0));

    field_0x690.copy(current.pos);
    resetInvKine();
    if (std::fabs(field_0x69C.y - mBaseTarget->current.pos.y) > hio_f(0x0A8) || field_0xAE4 == 0) {
        fm_modeProc(this, PROC_INIT_e, 1);
    } else if (fm_setHoleScale(this, hio_f(0x124), 0.1f, hio_f(0x12C))) {
        if (cLib_calcTimer(&field_0x650) == 0) {
            if (isGrabFoot() && !dComIfGp_event_runCheck()) {
                fm_modeProc(this, PROC_INIT_e, 0xF);
            } else if (field_0x9D4 < hio_f(0x0E4) && field_0xAE5 == 0) {
                fm_modeProc(this, PROC_INIT_e, 5);
            } else {
                fm_modeProc(this, PROC_INIT_e, 1);
            }
        }
    }
}
VERIFY(0x02144838, &daFm_c::modeUnderFoot);

/* 02144A18 */
void daFm_c::modePathMove() {
    WWHD_FUNC(0x02144A18, void, this);
    field_0x690.copy(current.pos);
    resetInvKine();
    turnToBaseTarget();
    if (isGrabFoot() && !dComIfGp_event_runCheck()) {
        fm_modeProc(this, PROC_INIT_e, 0xF);
    } else {
        dLib_pathMove(&field_0x3B0, &field_0x3BC, mpPath, 3.0f, pathMove_CB, this);
        cLib_addCalcPosXZ2(&current.pos, &field_0x3B0, (hio_f(0x0D0) + 1.0f) * 0.005f * field_0x394, 20.0f);
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        if (field_0x9D4 < hio_f(0x0E4) && std::fabs(pLink->current.pos.y - current.pos.y) < hio_f(0x0A8) &&
            field_0xAE5 == 0) {
            fm_modeProc(this, PROC_INIT_e, 5);
        }
    }
}
VERIFY(0x02144A18, &daFm_c::modePathMove);

/* 02144B94 */
void daFm_c::modeGoalKeeper() {
    WWHD_FUNC(0x02144B94, void, this);
    if (field_0x2E5 && dComIfGp_evmng_endCheckOld(STR(0x1000F878) /* "DEFAULT_FM_SUIKOMI_NPC" */)) {
        fm_modeProc(this, PROC_INIT_e, 6);
        return;
    }

    resetInvKine();
    turnToBaseTarget();
    if (isGrabFoot() && !dComIfGp_event_runCheck()) {
        fm_modeProc(this, PROC_INIT_e, 0xF);
    } else if (field_0x2E4 != 0) {
        fm_modeProc(this, PROC_INIT_e, 5);
    } else {
        f32 temp = std::fabs(current.pos.y - mBaseTarget->current.pos.y);
        if (field_0x9D4 < hio_f(0x0E8) && temp < hio_f(0x0A8)) {
            if (fm_absXZ_mi(&mBaseTarget->current.pos, &field_0x69C) < field_0x2E0 && field_0xAE5 == 0) {
                fm_modeProc(this, PROC_INIT_e, 5);
            }
        }
        field_0x690.copy(field_0x69C);
        cLib_addCalcPosXZ2(&current.pos, &field_0x690, (hio_f(0x0CC) + 1.0f) * 0.005f * field_0x394, 40.0f);
    }
}
VERIFY(0x02144B94, &daFm_c::modeGoalKeeper);

/* 021442A8 */
void daFm_c::modeSwWait() {
    WWHD_FUNC(0x021442A8, void, this);
    resetInvKine();
    if (!dComIfGs_isSwitch(field_0x2D4, current.roomNo)) {
        return;
    }

    if (field_0x2E4 != 0) {
        fm_setHoleScale(this, hio_f(0x124), 0.1f, hio_f(0x128));
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            if (field_0x2E4 != 0 && cLib_calcTimer(&field_0x650) != 0) {
                return;
            }
        } else {
            /* HD: event by name (startCheckOld) and fopAcM_orderOtherEvent2(name, 1, 0xFFFF) */
            if (dComIfGp_evmng_startCheckOld(STR(0x1000F854) /* "DEFAULT_FM_SW_APEEAR" */) == 0) {
                fopAcM_orderOtherEvent2(this, STR(0x1000F854), 1, 0xFFFF);
                return;
            }
        }
    } else {
        if (dComIfGp_evmng_startCheckOld(STR(0x1000F854)) != 0) {
            field_0x2E5 = 1;
        }
        actor_status |= 0x4000u; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
    }

    switch ((u32)field_0x2D0) {
    case 0:
        fm_modeProc(this, PROC_INIT_e, 2);
        break;
    case 1:
        if (fm_setHoleScale(this, hio_f(0x124), 0.1f, hio_f(0x128))) {
            fm_modeProc(this, PROC_INIT_e, 3);
        }
        break;
    case 2:
        if (fm_setHoleScale(this, hio_f(0x124), 0.1f, hio_f(0x128))) {
            fm_modeProc(this, PROC_INIT_e, 4);
        }
        break;
    }
}
VERIFY(0x021442A8, &daFm_c::modeSwWait);

/* 02144DE4 */
void daFm_c::moveRndBack() {
    WWHD_FUNC(0x02144DE4, void, this);
    cLib_distanceAngleS(shape_angle.y, field_0x9D0); /* unused angle */
    f32 temp = 100000.0f;
    if (mpActorTarget != nullptr) {
        temp = fopAcM_searchActorDistanceXZ(this, mpActorTarget);
    }

    f32 e0 = hio_f(0x0E0);
    if (!(field_0x9D4 > e0) || !(temp > e0)) {
        if (!isGrabFoot()) {
            s32 timer = cLib_calcTimer(&field_0x648);
            if (field_0xAE4 == 0 || mObjAcch.ChkWallHit()) {
                s16 old = field_0x680;
                f32 r = cM_rndF(3.0f);
                field_0x680 = (s16)(old + (s16)gabi::ftoi(gabi::fmadds(r, 4096.0f, (f32)(old + 0x5000))));
            } else if (timer == 0) {
                f32 base = (f32)(shape_angle.y + 0x3000);
                f32 r = cM_rndF(5.0f);
                field_0x680 = (s16)gabi::ftoi(gabi::fmadds(r, 4096.0f, base));
            } else {
                goto move;
            }
            field_0x394 = cM_rndF(9.0f) + 1.0f;
            field_0x660.z = gabi::fmadds(hio_f(0x0AC), cM_scos(field_0x680), field_0x660.z);
            field_0x660.x = gabi::fmadds(hio_f(0x0AC), cM_ssin(field_0x680), field_0x660.x);
            field_0x648 = hio_s(0x0A0);
        }
    }

    if (field_0xAE4 != 0 && !mObjAcch.ChkWallHit()) {
    move:
        cLib_addCalcPosXZ2(&current.pos, &field_0x660, 0.1f, hio_f(0x0B0));
    }
}
VERIFY(0x02144DE4, &daFm_c::moveRndBack);

/* 02143FF0 */
void daFm_c::holeDraw() {
    WWHD_FUNC(0x02143FF0, void, this);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    J3DModelData* modelData = J3DModel_getModelData(mpModel);
    mBtkAnm.entry(modelData, mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    /* HD: mBtkAnm.remove inline: clear the model data's texture-SRT animation */
    gabi::store<u32>(gabi::ea(J3DModel_getModelData(mpModel)) + 0x44, 0);
}
VERIFY(0x02143FF0, &daFm_c::holeDraw);

/* 02144058 */
void daFm_c::bodyDraw() {
    WWHD_FUNC(0x02144058, void, this);
    J3DModel* model = mpMorf->getModel();
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &tevStr);
    if (mEnemyIce.mFreezeTimer > 0x14) {
        dMat_control_iceEntryDL(mpMorf, -1, mInvisibleModel);
    } else {
        mpMorf->entryDL();
    }
}
VERIFY(0x02144058, &daFm_c::bodyDraw);

/* 021440D8 */
bool daFm_c::_draw() {
    WWHD_FUNC(0x021440D8, bool, this);
    debugDraw();
    if (field_0x3E0 > 0.015f) {
        holeDraw();
    }
    if (isBodyAppear()) {
        bodyDraw();
        dSnap_RegistFig_pos(DSNAP_TYPE_FM, this, &field_0x61C, shape_angle.y, 1.0f, 1.0f, 1.0f);
    }
    return true;
}
VERIFY(0x021440D8, &daFm_c::_draw);

/* 02142E0C */
void daFm_c::grabBomb() {
    WWHD_FUNC(0x02142E0C, void, this);
    fopAc_ac_c* target = mpActorTarget;
    /* HD: fopAcM_GetName checks for NULL */
    if (target != nullptr && fpcM_GetName(target) == fpcNm_BOMB_e) {
        gabi::Local<cXyz> temp_pos;
        temp_pos->x = 5.0f;
        temp_pos->y = -10.0f;
        temp_pos->z = 5.0f;
        gabi::Local<cXyz> pos;
        cXyz_pl(hio_xyz(0x038), pos, temp_pos);
        PSMTXCopy(fm_getAnmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        PSMTXMultVec(mDoMtx_stack_c::get(), pos, &mpActorTarget->current.pos);
        mpActorTarget->gravity = 0.0f;
        mpActorTarget->speedF = 0.0f;
        mpActorTarget->speed.x = 0.0f;
        mpActorTarget->speed.y = 0.0f;
        mpActorTarget->speed.z = 0.0f;
    }
}
VERIFY(0x02142E0C, &daFm_c::grabBomb);

/* 021432B8 */
void daFm_c::setCollision() {
    WWHD_FUNC(0x021432B8, void, this);
    if (isBodyAppear()) {
        if (dComIfGp_checkPlayerStatus0(0x402)) {
            /* HD: a fixed radius of 120 (GameCube: HIO radius * 1.5), set after the centre */
            mSph.SetC(&field_0x61C);
            mSph.SetR(120.0f);
        } else {
            /* HD: on odd frames the sphere is centred 30 above the actor's position */
            if (g_Counter0() & 1) {
                cM3dGSph_SetC_xyz(&mSph.mSph, current.pos.x, current.pos.y + 30.0f, current.pos.z);
            } else {
                mSph.SetC(&field_0x61C);
            }
            mSph.SetR(hio_f(0x0C8));
        }
        dComIfG_Ccsp_Set(&mSph);
    }

    if (field_0x3E0 > 0.015f) { /* isHoleAppear */
        mCyl.SetR(field_0x3E0);
        mCyl.SetH(150.0f);
        mCyl.SetC(&current.pos);
        dComIfG_Ccsp_Set(&mCyl);
    }
}
VERIFY(0x021432B8, &daFm_c::setCollision);

/* 021429BC */
void daFm_c::grabPlayer() {
    WWHD_FUNC(0x021429BC, void, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> head;
    head->copy(*fm_headTopPos(pLink));
    gabi::Local<cXyz> offset; /* unused */
    cXyz_mi(head, offset, &pLink->current.pos);
    gabi::Local<cXyz> temp;
    temp->x = 0.0f;
    temp->y = 0.0f;
    temp->z = 0.0f;
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    switch ((u32)(s32)mAnmPrmIdx) {
    case 4:
    case 6: {
        fopAc_ac_c* pLink2 = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> head2;
        head2->copy(*fm_headTopPos(pLink2));
        gabi::Local<cXyz> offset2;
        cXyz_mi(head2, offset2, &pLink2->current.pos);
        temp->y = 0.0f;
        temp->x = 5.0f;
        temp->z = 10.0f;
        angle->x = -3000;
        angle->y = 0;
        angle->z = 7000;
        gabi::Local<cXyz> temp2;
        cXyz_pl(hio_xyz(0x038), temp2, temp);
        f32 ty = temp2->y;
        f32 tz = temp2->z;
        f32 tx = temp2->x;
        ty = ty - fm_abs_copy(offset2);

        PSMTXCopy(fm_getAnmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        mDoMtx_stack_c::transM(tx, ty, tz);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        daPy_setPlayerPosAndAngle_mtx(pLink2, mDoMtx_stack_c::get());
    }
    case 5:
    default:
        break;
    }
}
VERIFY(0x021429BC, &daFm_c::grabPlayer);

/* 02142C4C */
void daFm_c::grabNPC() {
    WWHD_FUNC(0x02142C4C, void, this);
    if (isNpc(mpActorTarget) == true) {
        gabi::Local<cXyz> temp;
        temp->x = 0.0f;
        temp->y = 0.0f;
        temp->z = 0.0f;
        gabi::Local<csXyz> angle;
        csXyz_ct(angle, 0, 0, 0);
        fopAc_ac_c* target = mpActorTarget;
        /* HD: fopAcM_GetName checks for NULL */
        if (target != nullptr) {
            if (fpcM_GetName(target) == fpcNm_NPC_CB1_e) {
                temp->set(-15.0f, -10.0f, 8.0f);
                angle->x = 0;
                angle->y = 16000;
                angle->z = -4000;
            }
            if (target != nullptr && fpcM_GetName(target) == fpcNm_NPC_MD_e) {
                temp->set(10.0f, 0.0f, 5.0f);
                angle->x = 4000;
                angle->y = 18000;
                angle->z = -4000;
            }
        }
        gabi::Local<cXyz> temp2;
        cXyz_pl(hio_xyz(0x038), temp2, temp);
        PSMTXCopy(fm_getAnmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(temp2->x, temp2->y, temp2->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        fm_multVecZero(&mpActorTarget->current.pos);
        MtxToRot(mDoMtx_stack_c::get(), &mpActorTarget->shape_angle);
    }
}
VERIFY(0x02142C4C, &daFm_c::grabNPC);

/* 02142EF4 */
void daFm_c::grabTsubo() {
    WWHD_FUNC(0x02142EF4, void, this);
    fopAc_ac_c* tsubo = mpActorTarget;
    /* HD: fopAcM_GetName checks for NULL */
    if (tsubo != nullptr && fpcM_GetName(tsubo) == fpcNm_TSUBO_e) {
        gabi::Local<cXyz> pos;
        pos->x = 0.0f;
        pos->y = 0.0f;
        pos->z = 0.0f;
        gabi::Local<csXyz> angle;
        csXyz_ct(angle, 0, 0, 0);
        switch (daObj_PrmAbstract(tsubo, 4, 0x18)) { /* daTsubo::Act_c::prm_get_type */
        case 0:
            pos->set(25.0f, -25.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 5000;
            break;
        case 1:
        case 2:
            pos->set(55.0f, -55.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 5000;
            break;
        case 4:
            pos->set(50.0f, -45.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 7000;
            break;
        case 5:
            pos->set(0.0f, -15.0f, 0.0f);
            angle->x = 0; angle->y = 16000; angle->z = 0;
            break;
        case 6:
            pos->set(40.0f, -20.0f, 0.0f);
            angle->x = 0; angle->y = 0; angle->z = 7000;
            break;
        }

        gabi::Local<cXyz> pos2;
        cXyz_pl(hio_xyz(0x038), pos2, pos);
        PSMTXCopy(fm_getAnmMtx(mpMorf->getModel(), FM_JNT_TE_e), mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(pos2->x, pos2->y, pos2->z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angle->x, angle->y, angle->z);
        PSMTXCopy(mDoMtx_stack_c::get(), &field_0x6BC);
        fm_multVecZero(&mpActorTarget->current.pos);
        MtxToRot(mDoMtx_stack_c::get(), &mpActorTarget->shape_angle);
        /* tsubo->tevStr.mEnvrIdxOverride = tevStr.mEnvrIdxOverride */
        gabi::store<u8>(gabi::ea(tsubo) + 0x1CA, gabi::load<u8>(gabi::ea(this) + 0x1CA));
    }
}
VERIFY(0x02142EF4, &daFm_c::grabTsubo);

/* 02143418 */
bool daFm_c::_execute() {
    WWHD_FUNC(0x02143418, bool, this);
    fm_holeExecute(this);
    fopAcM_SearchByID_out(mProcId, &mpActorTarget);
    fopAc_ac_c* bomb = mpActorTarget;
    /* HD: fopAcM_GetName checks for NULL */
    if (bomb != nullptr && bomb != nullptr && fpcM_GetName(bomb) == fpcNm_BOMB_e && daBomb_chk_state(bomb, 0)) {
        mpActorTarget = nullptr;
        mProcId = 0;
        fm_modeProc(this, PROC_INIT_e, 7);
    }

    if (enemy_ice(&mEnemyIce)) {
        fm_iceProc(this);
        return true;
    }

    fm_setBaseTarget(this);
    if (mBaseTarget == nullptr) {
        fm_bodySetMtx(this);
        fm_holeSetMtx(this);
        morf_calc(mpMorf);
        return true;
    }

    u8 area = fm_areaCheck(this);
    field_0xAE5 = 0;
    field_0xAE4 = area;

    gabi::Local<cXyz> temp;
    temp->copy(current.pos);
    temp->y = current.pos.y + 100.0f;
    gabi::Local<cXyz> temp2;
    temp2->copy(mBaseTarget->current.pos);
    temp2->y = temp2->y + 100.0f;

    if (field_0x9D4 < hio_f(0x0E8) && fm_lineCheck(this, temp, temp2) != 0) {
        field_0xAE5 = 1;
    }
    if (field_0x2E4 != 0) {
        field_0xAE5 = 0;
    }

    if (mpActorTarget != nullptr) {
        if (mMode == 8 && mAnmPrmIdx == 4 && fm_isLink(this, mpActorTarget)) {
            fm_spAttackVJump(this);
        } else if (mAnmPrmIdx == 0xB || mAnmPrmIdx == 0xC) {
            fm_spAttackJump(this);
        } else {
            spAttackNone();
        }
    }
    fm_modeProc(this, PROC_EXEC_e, 0x15);
    fm_setAnm(this, 0xF, false);
    morf_play(mpMorf, &current.pos, 0, 0);
    morf_calc(mpMorf);

    if (field_0x684 != 0) {
        if (!fopAcM_SearchByID_out(mProcId, &mpActorTarget)) {
            field_0x684 = 0;
        }
        switch ((u32)field_0x684) {
        case 1:
            grabPlayer();
            break;
        case 4:
            grabNPC();
            break;
        case 2:
            grabBomb();
            break;
        case 3:
            grabTsubo();
            break;
        }
    }
    field_0x660.y = current.pos.y;
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());

    fm_bodySetMtx(this);
    fm_holeSetMtx(this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    fm_setAttention(this);
    setCollision();
    return true;
}
VERIFY(0x02143418, &daFm_c::_execute);

/* function-local static GXColor initialised on first use (guard, then memcpy of 4 bytes from .rodata) */
static inline void fm_static_color(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::call(0xC000A848, dst, src, 4); /* memcpy (import, through the thunk 028FEAC0) */
    }
}
#define FM_COLOR_A() fm_static_color(0x101FDAC0, 0x101FEBF8, 0x1000F23C)
#define FM_COLOR_B() fm_static_color(0x101FDA48, 0x101FEBEC, 0x1000F240)
#define FM_COLOR_C() fm_static_color(0x101FDA50, 0x101FEBF4, 0x1000F244)
#define FM_COLOR_D() fm_static_color(0x101FDAC4, 0x101FEBFC, 0x1000F248)
#define FM_COLOR_E() fm_static_color(0x101FDA4C, 0x101FEBF0, 0x1000F24C)
#define FM_COLOR_F() fm_static_color(0x101FDA44, 0x101FEBE8, 0x1000F250)

/* 021439A0
 * HD: every debug draw call except dLib_debugDrawAxis is compiled out; what is left are the
 * initialisations of the function-local static colours used by the removed calls (in the order of
 * the inlined code paths) and dead dComIfGp_get() calls. */
void daFm_c::debugDraw() {
    WWHD_FUNC(0x021439A0, void, this);
    if (hio_u8(0x004) != 0 && isBodyAppear()) {
        if (mpActorTarget != nullptr) {
            FM_COLOR_A();
        }
        if (mBaseTarget != nullptr) {
            FM_COLOR_B();
        }
    }
    if (hio_u8(0x00C) != 0) {
        for (int i = 0; i != 12; i++) {
        }
        FM_COLOR_A();
        FM_COLOR_A();
        FM_COLOR_C();
        FM_COLOR_B();
        FM_COLOR_D();
        dComIfGp_get();
    }
    if (hio_u8(0x00A) != 0) {
        FM_COLOR_E();
        if (mMode == 0xD && field_0x684 == 4) {
            dComIfGp_get();
            FM_COLOR_D();
        }
    }
    if (hio_u8(0x009) != 0) {
        FM_COLOR_A();
        FM_COLOR_B();
        FM_COLOR_C();
        FM_COLOR_F();
        FM_COLOR_C();
        FM_COLOR_C();
        FM_COLOR_B();
        FM_COLOR_B();
        FM_COLOR_C();
    }
    if (hio_u8(0x00B) != 0) {
        for (int i = 0; i < 6; i++) {
            if (i == 4) {
                continue;
            }
            FM_COLOR_C();
            FM_COLOR_B();
            FM_COLOR_B();
        }
    }
    if (hio_u8(0x006) != 0) {
        dLib_debugDrawAxis(&field_0x6BC, 100.0f);
    }
    if (hio_u8(0x007) != 0) {
        gabi::Local<Mtx34> mtx;
        J3DModel* mdl = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(this) + 0x368)); /* fopAc_ac_c::model */
        PSMTXCopy(fm_getAnmMtx(mdl, FM_JNT_TE_e), mtx);
        dLib_debugDrawAxis(mtx, 100.0f);
    }
    if (hio_u8(0x005) != 0) {
        dComIfGp_get(); /* daPy_getPlayerLinkActorClass()->getHeadTopPos() */
        FM_COLOR_B();
        FM_COLOR_A();
        FM_COLOR_C();
        FM_COLOR_B();
        FM_COLOR_B();
        FM_COLOR_C();
        FM_COLOR_B();
    }
}
VERIFY(0x021439A0, &daFm_c::debugDraw);
