/**
 * d_a_fm_b.cpp (WWHD)
 * Enemy - Floormaster, part B (02145070..02147408): hit check, target search, grab position,
 * appear/disappear/wait/attack modes, inverse kinematics.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Other parts: d_a_fm.cpp, d_a_fm_a.cpp, d_a_fm_c.cpp.
 */
#include "d/actor/d_a_fm_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daFm_c members outside this part (called by address) */
static inline void fm_setAnm(daFm_c* a, s32 idx, u32 force) { gabi::call(0x021418D0, a, idx, force); } /* matcher: "checkPlayerGrabBomb" */
static inline void fm_modeProcInit(daFm_c* a, s32 mode) { gabi::call(0x021412BC, a, 0, mode); }
static inline BOOL fm_isLink(daFm_c* a, fopAc_ac_c* t) { return gabi::call<BOOL>(0x02141280, a, t); }
static inline BOOL fm_isNpc(daFm_c* a, fopAc_ac_c* t) { return gabi::call<BOOL>(0x02142BC4, a, t); }
static inline BOOL fm_checkHeight(daFm_c* a, fopAc_ac_c* t) { return gabi::call<BOOL>(0x02140CC0, a, t); }
static inline BOOL fm_checkPlayerGrabTarget(daFm_c* a) { return gabi::call<BOOL>(0x02141FD8, a); }
static inline BOOL fm_isLinkControl(daFm_c* a) { return gabi::call<BOOL>(0x021446EC, a); } /* unnamed */
static inline void fm_moveRndBack(daFm_c* a) { gabi::call(0x02144DE4, a); }
static inline void fm_resetInvKine(daFm_c* a) { gabi::call(0x021441D0, a); }
static inline void fm_cancelGrab(daFm_c* a) { gabi::call(0x02142028, a); }
static inline BOOL fm_areaCheck(daFm_c* a) { return gabi::call<BOOL>(0x0214264C, a); }
static inline BOOL fm_isGrabFoot(daFm_c* a) { return gabi::call<BOOL>(0x0214472C, a); }
static inline s32 fm_setRnd(daFm_c* a, s32 p1, s32 p2) { return gabi::call<s32>(0x021444FC, a, p1, p2); }
static inline void fm_turnToBaseTarget(daFm_c* a) { gabi::call(0x02144A04, a); }
#define FM_SEARCH_NEAR_OTHER_ACTOR_CB 0x02140F04u

/* fopAcM / player inlines */
static inline s16 fm_GetName(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 8); }
static inline bool fm_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & 0x2000u) != 0; }
static inline void fm_setCarryNow(fopAc_ac_c* a, u32 b) { gabi::call(0x025D9D0C, a, b); }
/* fopAcM_SearchByName(name): fopAcIt_Judge(fpcSch_JudgeForPName, &name); GHS gives the two inlined
 * calls of searchTarget adjacent s16 slots (sp+8, sp+0xA) */
struct fm_nameKeys {
    be<s16> k0, k1;
};
static inline fopAc_ac_c* fm_SearchByName(be<s16>* key, s16 name) {
    *key = name;
    return fopAcIt_Judge(0x025E121C, key);
}
static inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(gabi::load<u32>(p->__vtbl + 0xBC), p); }
/* daPy_lk_c::checkCarryActionNow (inline): current proc +0x65F0 is 0x72, 0x6F or 0x71 */
static inline bool daPy_checkCarryActionNow(fopAc_ac_c* p) {
    u32 e = gabi::ea(p);
    if (gabi::load<s32>(e + 0x65F0) == 0x72) return true;
    s32 v = gabi::load<s32>(e + 0x65F0);
    return v == 0x6F || v == 0x71;
}
static inline void daPy_voiceStart(fopAc_ac_c* p, u32 id) { gabi::call_ptr(gabi::load<u32>(p->__vtbl + 0xE4), p, id); }
static inline void daPy_setOutPower(fopAc_ac_c* p, f32 pw, s16 ang, s32 n) { gabi::call_ptr(gabi::load<u32>(p->__vtbl + 0xEC), p, pw, ang, n); }
/* dComIfGp_checkPlayerStatus0(0, mask): play+0x5CD8 */
static inline bool fm_checkPlayerStatus0(u32 mask) { return (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & mask) != 0; }
/* 020CB92C daBomb_c::chk_state(state), 02048038 daObj::PrmAbstract */
static inline BOOL fm_bomb_chk_state(fopAc_ac_c* b, u32 s) { return gabi::call<BOOL>(0x020CB92C, b, s); }
static inline u32 fm_PrmAbstract(fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x02048038, a, w, s); }
/* 023127F8 daObj::quat_rotVec(Quaternion*, const cXyz&, const cXyz&), 028E9BC0 C_QUATSlerp */
static inline void quat_rotVec(Quaternion_fm* q, cXyz* a, cXyz* b) { gabi::call(0x023127F8, q, a, b); }
static inline void C_QUATSlerp(Quaternion_fm* p, Quaternion_fm* q, Quaternion_fm* r, f32 t) { gabi::call(0x028E9BC0, p, q, r, t); }
/* |(a - b).xz|: cXyz::operator-, then {x, 0, z} (float copies) through PSVECSquareMag and sqrtf */
static inline f32 fm_absXZ_mi(const cXyz* a, const cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    f32 x = d->x;
    f32 z = d->z;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    return std_sqrtf(PSVECSquareMag(xz));
}
static inline f32 fm_fdivs_ppc(f32 a, f32 b) {
    if (a != a) return gabi::ppc_qnan(a);
    if (b != b) return gabi::ppc_qnan(b);
    return a / b;
}
static inline bool fm_morfIsStop(mDoExt_McaMorf* m) { return m->isStop(); }

enum {
    JA_SE_LK_SW_HIT_S = 0x2803, JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_HS_SPIKE = 0x286F, JA_SE_LK_ARROW_HIT = 0x2879, JA_SE_CV_FM_DAMAGE = 0x48B1, JA_SE_CV_FM_DIE = 0x48B2,
    JA_SE_CM_FM_GRAB = 0x58AD, JA_SE_CM_FM_GRAB_HAND = 0x58B1,
};
enum { fpcNm_BOMB_e = 0x126, fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F, fpcNm_TSUBO_e = 0x1C5 };

/* ---- functions ---- */

/* 02145070 */
bool daFm_c::checkTgHit() {
    WWHD_FUNC(0x02145070, bool, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    mStts2.Move();
    if (cLib_calcTimer(&field_0x9C4) != 0 || !mSph.ChkTgHit()) {
        return false;
    }
    bool temp = true;
    cXyz* hitPos = mSph.GetTgHitPosP();
    void* hitObj = mSph.GetTgHitObj();
    field_0x9C4 = hio_s(0x9C);
    if (hitObj == nullptr) {
        return false;
    }

    switch (gabi::load<u32>(gabi::ea(hitObj) + 0x10)) { /* GetAtType */
    case 0x2:        /* AT_TYPE_SWORD */
    case 0x400:      /* AT_TYPE_MACHETE */
    case 0x800:      /* AT_TYPE_UNK800 */
    case 0x4000000:  /* AT_TYPE_DARKNUT_SWORD */
    case 0x10000000: /* AT_TYPE_MOBLIN_SPEAR */
    {
        fm_seStart(this, JA_SE_LK_SW_HIT_S, 0x20);
        u8 cut = daPy_getCutType(player);
        switch (cut) {
        case 0x10: /* CUT_TYPE_BT_VERTICALJUMPCUT */
            mHitType = 0xC;
            break;
        case 0x05: case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0C: case 0x0E: case 0x0F:
        case 0x15: case 0x17: case 0x19: case 0x1A: case 0x1B:
            mHitType = 1;
            /* HD: cut types 8 and 9 set the hit cooldown to 12 frames */
            if (cut == 8 || cut == 9) {
                field_0x9C4 = 0xC;
            }
            break;
        default:
            mHitType = 0;
            break;
        }
        break;
    }
    case 0x200000: /* AT_TYPE_WIND */
        temp = false;
        mHitType = 3;
        break;
    case 0x40: /* AT_TYPE_BOOMERANG */
    case 0x80: /* AT_TYPE_BOKO_STICK */
        fm_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
        temp = false;
        mHitType = 4;
        dComIfGp_particle_set(0x27B /* ID_IT_JN_PIYOHIT00 */, attention_pos(this));
        fm_monsSeStart(this, JA_SE_CV_FM_DAMAGE, 0);
        fm_modeProcInit(this, 0x13);
        break;
    case 0x8000: /* AT_TYPE_HOOKSHOT */
        fm_seStart(this, JA_SE_LK_HS_SPIKE, 0x44);
        temp = false;
        mHitType = 5;
        fm_modeProcInit(this, 10);
        break;
    case 0x10000:   /* AT_TYPE_SKULL_HAMMER */
    case 0x1000000: /* AT_TYPE_STALFOS_MACE */
        fm_seStart(this, JA_SE_LK_HAMMER_HIT, 0x20);
        mHitType = 8;
        if (daPy_getCutType(player) == 0x11 /* CUT_TYPE_HAMMER_SIDESWING */) {
            mHitType = 9;
        }
        break;
    case 0x20: /* AT_TYPE_BOMB */
        mHitType = 7;
        break;
    case 0x80000: /* AT_TYPE_ICE_ARROW */
        temp = false;
        fm_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
        mEnemyIce.mFreezeDuration = hio_s(0x62);
        mHitType = 10;
        fm_modeProcInit(this, 7);
        break;
    case 0x100000: /* AT_TYPE_LIGHT_ARROW */
        temp = false;
        fm_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
        mEnemyIce.mLightShrinkTimer = 1;
        mHitType = 0xB;
        break;
    case 0x40000: /* AT_TYPE_FIRE_ARROW */
    case 0x4000:  /* AT_TYPE_NORMAL_ARROW */
        fm_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
        mHitType = 6;
        break;
    case 0x8000000: /* AT_TYPE_GRAPPLING_HOOK */
        fm_monsSeStart(this, JA_SE_CV_FM_DAMAGE, 0);
        dComIfGp_particle_set(0x27B /* ID_IT_JN_PIYOHIT00 */, attention_pos(this));
        fm_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
        mHitType = 0xD;
        temp = false;
        break;
    }

    gabi::Local<CcAtInfo_l> atInfo;
    atInfo->pParticlePos = nullptr;
    atInfo->mpObj = gabi::ea(mSph.GetTgHitObj());

    if (temp) {
        cc_at_check(this, atInfo);
        u8 hitType = mHitType;
        if (hitType == 1 || hitType == 8 || hitType == 9 || hitType == 0xC || health <= 0) {
            dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, hitPos);
            gabi::Local<cXyz> scale;
            scale->x = 2.0f;
            scale->y = 2.0f;
            scale->z = 2.0f;
            dComIfGp_particle_set(0xF /* ID_AK_JN_CRITICALHIT */, hitPos, &player->shape_angle, scale);
            /* fopAcM_monsSeStart: the sound id and the process id are read before the reverb */
            u32 se = health > 0 ? JA_SE_CV_FM_DAMAGE : JA_SE_CV_FM_DIE;
            s8 roomNo = current.roomNo;
            u32 procId = gabi::load<u32>(gabi::ea(this) + 4);
            s32 reverb = dComIfGp_getReverb(roomNo);
            gabi::call(0x025E1AA4, se, &eyePos, procId, 0, reverb);

            if (mHitType == 0xC || mHitType == 8) {
                fm_modeProcInit(this, 0x14);
                return true;
            }
            if (health <= 0) {
                fm_modeProcInit(this, 0xC);
            } else {
                fm_modeProcInit(this, 10);
            }
        } else {
            dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, hitPos, &player->shape_angle);
            fm_monsSeStart(this, JA_SE_CV_FM_DAMAGE, 0);
            fm_modeProcInit(this, 10);
        }
    } else if (mHitType == 0xD) {
        fm_modeProcInit(this, 10);
        s8 oldHealth = health;
        health = 10;
        cc_at_check(this, atInfo);
        health = oldHealth;
        fm_setAnm(this, 1, 0);
        field_0x64C = 0x28;
        field_0x68C = 0x2B00;
    }
    return true;
}
VERIFY(0x02145070, &daFm_c::checkTgHit);

/* 0214594C (unnamed; matcher name "checkPlayerGrabBomb" is on 021418D0 setAnm) */
u8 daFm_c::checkPlayerGrabBomb() {
    WWHD_FUNC(0x0214594C, u8, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAc_ac_c* ac = fopAcM_SearchByID(daPy_getGrabActorID(link));
    if (ac == nullptr) {
        return false;
    }
    return fm_GetName(ac) == fpcNm_BOMB_e;
}
VERIFY(0x0214594C, &daFm_c::checkPlayerGrabBomb);

/* 021459C8 */
u8 daFm_c::checkPlayerGrabNpc() {
    WWHD_FUNC(0x021459C8, u8, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAc_ac_c* ac = fopAcM_SearchByID(daPy_getGrabActorID(link));
    if (ac == nullptr) {
        return false;
    }
    return fm_isNpc(this, ac) != 0;
}
VERIFY(0x021459C8, &daFm_c::checkPlayerGrabNpc);

/* 02145A60 */
void daFm_c::searchTarget() {
    WWHD_FUNC(0x02145A60, void, this);
    if (dComIfGp_event_runCheck() && field_0x2E4 == 0) {
        return;
    }
    fopAc_ac_c* actor = fopAcIt_Judge(FM_SEARCH_NEAR_OTHER_ACTOR_CB, this); /* fopAcM_Search(searchNearOtherActor_CB, this) */
    mpActorTarget = actor;
    if (actor == nullptr) {
        gabi::Local<fm_nameKeys> keys;
        actor = fm_SearchByName(&keys->k0, fpcNm_NPC_CB1_e);
        if (actor == nullptr) {
            actor = fm_SearchByName(&keys->k1, fpcNm_NPC_MD_e);
        }
        if (actor != nullptr) {
            if (fopAcM_searchActorDistanceXZ(this, actor) < hio_f(0xE4) || fm_checkHeight(this, actor)) {
                if (field_0x2DC == 1) {
                    if (fm_checkCarryNow(actor)) {
                        mpActorTarget = actor;
                    }
                } else {
                    mpActorTarget = actor;
                }
            }
        }

        if (mpActorTarget == nullptr && field_0x2DC != 2) {
            fopAc_ac_c* link_actor = dComIfGp_getLinkPlayer();
            fopAc_ac_c* link_player = dComIfGp_getLinkPlayer(); /* daPy_getPlayerLinkActorClass() */
            if (link_actor != nullptr && !fm_checkPlayerStatus0(0x4100 /* HOOKSHOT_AIM | HANG */)) {
                if (!checkPlayerGrabBomb()) {
                    if (!checkPlayerGrabNpc() && !fm_isLinkControl(this)) {
                        if (!daPy_checkCarryActionNow(dComIfGp_getLinkPlayer()) && daPy_getGrabActorID(link_player) == 0xFFFFFFFFu) {
                            if (fopAcM_searchActorDistanceXZ(this, link_actor) < hio_f(0xE4) || fm_checkHeight(this, mpActorTarget)) {
                                mpActorTarget = link_actor;
                            }
                        }
                    }
                }
            }
        }
    }

    actor = mpActorTarget;
    if (actor != nullptr) {
        mProcId = fopAcM_GetID(actor);
    }
}
VERIFY(0x02145A60, &daFm_c::searchTarget);

/* 02145C70: returns cXyz through the hidden pointer (GHS: allocated when NULL) */
void daFm_c::getOffsetPos(cXyz* out) {
    WWHD_FUNC(0x02145C70, void, this, out);
    f32 y = 0.0f;
    if (fm_isLink(this, mpActorTarget)) {
        y = -10.0f;
    } else {
        fopAc_ac_c* t = mpActorTarget;
        if (t != nullptr) {
            s16 procName = fm_GetName(t);
            if (procName == fpcNm_NPC_CB1_e) {
                y = fm_checkCarryNow(t) ? 20.0f : 80.0f;
            } else if (procName == fpcNm_NPC_MD_e) {
                y = fm_checkCarryNow(t) ? 20.0f : 140.0f;
            } else if (procName == fpcNm_BOMB_e) {
                y = 80.0f;
            } else if (procName == fpcNm_TSUBO_e) {
                switch (fm_PrmAbstract(t, 4, 0x18) /* daTsubo::Act_c::prm_get_type */) {
                case 5:
                case 0:
                    y = 80.0f;
                    break;
                case 4:
                case 6:
                    y = 80.0f;
                    break;
                case 1:
                case 2:
                    y = 90.0f;
                    break;
                }
            }
        }
    }
    if (out == nullptr) {
        out = (cXyz*)operator_new(0xC);
        if (out == nullptr) {
            return;
        }
    }
    out->y = y;
    out->x = 0.0f;
    out->z = 0.0f;
}
VERIFY(0x02145C70, &daFm_c::getOffsetPos);

/* 02145DE4 */
void daFm_c::setGrabPos() {
    WWHD_FUNC(0x02145DE4, void, this);
    fopAc_ac_c* bomb = mpActorTarget;
    if (bomb == nullptr) {
        mGrabPos.copy(current.pos);
        return;
    }
    bool link;
    if (fm_GetName(bomb) == fpcNm_BOMB_e) {
        if (fm_bomb_chk_state(bomb, 0 /* STATE_0 */)) {
            mGrabPos.copy(current.pos);
            return;
        }
        link = fm_isLink(this, mpActorTarget);
    } else {
        link = fm_isLink(this, bomb);
    }

    if (link) {
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        cXyz* head = daPy_getHeadTopPos(pLink);
        gabi::Local<cXyz> headPos;
        headPos->x = (f32)head->x;
        headPos->y = (f32)head->y;
        headPos->z = (f32)head->z;
        gabi::Local<cXyz> offset;
        getOffsetPos(offset);
        gabi::Local<cXyz> sum;
        cXyz_pl(headPos, sum, offset);
        mGrabPos.copy(*sum);
    } else if (fm_isNpc(this, mpActorTarget)) {
        gabi::Local<cXyz> offset;
        getOffsetPos(offset);
        gabi::Local<cXyz> sum;
        cXyz_pl(&mpActorTarget->current.pos, sum, offset);
        mGrabPos.copy(*sum);
    } else {
        gabi::Local<cXyz> offset;
        getOffsetPos(offset);
        gabi::Local<cXyz> sum;
        cXyz_pl(&mpActorTarget->current.pos, sum, offset);
        mGrabPos.copy(*sum);
    }
}
VERIFY(0x02145DE4, &daFm_c::setGrabPos);

/* 02145F90 (matcher: unnamed; "isGrabPos" is this function) */
bool daFm_c::isGrabPos() {
    WWHD_FUNC(0x02145F90, bool, this);
    if (mpActorTarget == nullptr) {
        return false;
    }

    f32 abs = fm_absXZ_mi(&mGrabPos, &current.pos);

    fopAc_ac_c* t = mpActorTarget;
    if (fm_checkCarryNow(t)) {
        if (checkPlayerGrabBomb() != 0) {
            return false;
        }
        if (fm_checkPlayerGrabTarget(this) == 0) {
            return false;
        }
        t = mpActorTarget;
    }

    if (fm_isLink(this, t)) {
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        if (daPy_checkCarryActionNow(dComIfGp_getLinkPlayer()) || daPy_getGrabActorID(pLink) != 0xFFFFFFFFu) {
            return false;
        }
        if (fm_isLinkControl(this)) {
            return false;
        }
    }

    if (fm_isLink(this, mpActorTarget)) {
        if (!(abs > hio_f(0xE0))) {
            return false;
        }
    } else if (!(abs > hio_f(0xE0)) || abs > hio_f(0xE4)) {
        return false;
    }

    f32 dy = mGrabPos.y - current.pos.y;
    if (std::fabs(dy) > hio_f(0xA8)) {
        return false;
    }
    s16 target = cLib_targetAngleY(&current.pos, &mGrabPos);
    if (cLib_distanceAngleS(shape_angle.y, target) > hio_s(0x98)) {
        return false;
    }
    if (field_0x2D0 != 0) {
        return true;
    }
    if (fm_absXZ_mi(&field_0x69C, &mGrabPos) > field_0x2E0) {
        return false;
    }
    return true;
}
VERIFY(0x02145F90, &daFm_c::isGrabPos);

/* 021461D0 */
void daFm_c::modeAppear() {
    WWHD_FUNC(0x021461D0, void, this);
    if (field_0x2D0 == 0) {
        fm_moveRndBack(this);
    }
    fm_resetInvKine(this);
    if (checkTgHit()) {
        return;
    }
    searchTarget();
    if (mpActorTarget != nullptr) {
        setGrabPos();
        if (field_0x2E4 != 0 && cLib_calcTimer(&field_0x650) != 0) {
            return;
        }
        if (isGrabPos()) {
            fm_modeProcInit(this, 8);
            return;
        }
    }
    if (mAnmPrmIdx == 1) {
        fm_modeProcInit(this, 7);
    }
}
VERIFY(0x021461D0, &daFm_c::modeAppear);

/* 021462A0 */
void daFm_c::modeDisappearInit() {
    WWHD_FUNC(0x021462A0, void, this);
    if (mAnmPrmIdx != 8 && mAnmPrmIdx != 6) {
        fm_setAnm(this, 3, 0);
    }
    if (mAnmPrmIdx == 6) {
        fm_setAnm(this, 8, 0);
    }
}
VERIFY(0x021462A0, &daFm_c::modeDisappearInit);

/* 02146310 */
void daFm_c::modeDisappear() {
    WWHD_FUNC(0x02146310, void, this);
    if (field_0x2D0 != 2) {
        field_0x690.copy(current.pos);
    }
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (mAnmPrmIdx == 8 && fm_morfIsStop(mpMorf)) {
        if (field_0x2E4 != 0 || field_0x2E5 != 0) {
            fm_modeProcInit(this, 0x12);
        } else {
            fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0)); /* fopAcM_searchPlayerDistanceXZ (result unused) */
            if (fm_isGrabFoot(this) && !dComIfGp_event_runCheck()) {
                fm_modeProcInit(this, 0xF);
            } else {
                switch ((s32)field_0x2D0) {
                case 0:
                    fm_modeProcInit(this, 1);
                    break;
                case 1:
                    fm_modeProcInit(this, 3);
                    break;
                case 2:
                    fm_modeProcInit(this, 4);
                    break;
                }
            }
        }
    }
}
VERIFY(0x02146310, &daFm_c::modeDisappear);

/* 02146434 */
void daFm_c::modeWaitInit() {
    WWHD_FUNC(0x02146434, void, this);
    field_0x658 = hio_s(0x88);
    field_0x650 = fm_setRnd(this, hio_s(0x8C), hio_s(0x8E));
    mSinkTimer = hio_s(0x8A);
    field_0x65C = hio_s(0x100);
    field_0x6B4 = 0;
    field_0x660.copy(current.pos);
}
VERIFY(0x02146434, &daFm_c::modeWaitInit);

/* 021464B4 */
void daFm_c::modeWait() {
    WWHD_FUNC(0x021464B4, void, this);
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (checkTgHit()) {
        return;
    }
    if (!fm_areaCheck(this)) {
        fm_modeProcInit(this, 6);
        return;
    }
    if (fm_isGrabFoot(this)) {
        fm_modeProcInit(this, 6);
        return;
    }

    searchTarget();
    s32 angle = cLib_distanceAngleS(shape_angle.y, field_0x9D0);
    if (mpActorTarget != nullptr) {
        if (field_0x9D4 > hio_f(0xE0)) {
            s16 angle2 = cLib_targetAngleY(&current.pos, &mGrabPos);
            fopAc_ac_c* t = mpActorTarget;
            if (fm_isNpc(this, t) || fm_isLink(this, t)) {
                cLib_addCalcAngleS2(&shape_angle.y, angle2, 8, 0x300);
            } else {
                cLib_addCalcAngleS2(&shape_angle.y, angle2, 4, 0x800);
            }
        }
        setGrabPos();
        if (isGrabPos()) {
            if (fm_checkPlayerGrabTarget(this)) {
                if (cLib_calcTimer(&field_0x65C) == 0) {
                    fm_modeProcInit(this, 8);
                    return;
                }
            } else {
                if (fm_isNpc(this, mpActorTarget)) {
                    if (cLib_calcTimer(&field_0x65C) != 0) {
                        return;
                    }
                }
                fm_modeProcInit(this, 8);
                return;
            }
        }
    } else {
        if (angle > hio_s(0x9A)) {
            cLib_addCalcAngleS2(&shape_angle.y, field_0x9D0, 10, 0x100);
        }
    }

    if (field_0x2D0 == 2 && fm_absXZ_mi(&mBaseTarget->current.pos, &field_0x69C) > field_0x2E0) {
        fm_modeProcInit(this, 6);
    } else {
        fm_moveRndBack(this);
    }
    if (field_0x9D4 > hio_f(0xE4) && field_0x9D4 < hio_f(0xE8)) {
        fm_turnToBaseTarget(this);
    }
    if (field_0x9D4 > hio_f(0xE8)) {
        if (cLib_calcTimer(&field_0x658) == 0) {
            fm_modeProcInit(this, 6);
        }
    } else {
        field_0x658 = hio_s(0x88);
    }
}
VERIFY(0x021464B4, &daFm_c::modeWait);

/* 02146764 */
void daFm_c::modeAttackInit() {
    WWHD_FUNC(0x02146764, void, this);
    if (fm_checkPlayerStatus0(0x402 /* UNK400 | UNK2 */)) {
        fm_modeProcInit(this, 7);
    } else {
        fm_setAnm(this, 4, 0);
        field_0x650 = hio_s(0x94);
    }
}
VERIFY(0x02146764, &daFm_c::modeAttackInit);

/* 021467D8 */
bool daFm_c::isGrab() {
    WWHD_FUNC(0x021467D8, bool, this);
    fopAc_ac_c* t = mpActorTarget;
    if (t == nullptr) {
        return false;
    }
    if (fm_isLink(this, t) && (fm_checkPlayerStatus0(0x100 /* HANG */) || checkPlayerGrabBomb())) {
        return false;
    }
    f32 dy = mGrabPos.y - current.pos.y;
    if (std::fabs(dy) > hio_f(0xA8)) {
        return false;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&mGrabPos, diff, &field_0x61C);
    f32 limit = hio_f(0xEC);
    if (field_0x2E4 != 0) {
        f32 v = hio_f(0xEC);
        limit = gabi::fadds_ppc(v, v); /* GameCube: * 2.0f */
    }
    if (std_sqrtf(PSVECSquareMag(diff)) > limit) {
        return false;
    }
    return true;
}
VERIFY(0x021467D8, &daFm_c::isGrab);

/* 021468E8 */
void daFm_c::calcInvKine(fopAc_ac_c* i_target) {
    WWHD_FUNC(0x021468E8, void, this, i_target);
    if (!isBodyAppear() || i_target == nullptr) {
        return;
    }
    /* HD branch: written as the original tests it (NaN returns) */
    if (!(fopAcM_searchActorDistanceXZ(this, i_target) > hio_f(0xDC))) {
        return;
    }
    for (int i = 5; i >= 0; i--) {
        field_0x390 = i;
        gabi::Local<cXyz> temp;
        temp->x = (f32)field_0x2E8[i].x;
        temp->y = (f32)field_0x2E8[i].y;
        temp->z = (f32)field_0x2E8[i].z;
        if (hio_u8(0xF + i) == 1) {
            gabi::Local<cXyz> diff;
            cXyz_mi(&mGrabPos, diff, temp);
            gabi::Local<cXyz> diff2;
            cXyz_mi(&field_0x61C, diff2, temp);
            gabi::Local<Quaternion_fm> quat;
            quat_rotVec(quat, diff2, diff);
            f32 t = gabi::fsubs_ppc(0.75f, gabi::load<f32>(0x1047BD34) /* REG12_F(0x19) */);
            C_QUATSlerp(&field_0x330[i], quat, &field_0x330[i], t); /* mDoMtx_quatSlerp */
        }
        mDoExt_McaMorf* morf = mpMorf;
        morf->calc();
    }
}
VERIFY(0x021468E8, &daFm_c::calcInvKine);

/* setOutPower toward the grab position (modeAttack, both grab states) */
static inline void fm_pushLink(daFm_c* i_this, fopAc_ac_c* pLink, f32 dist) {
    s16 targetAngle = cLib_targetAngleY(&i_this->field_0x63C, &i_this->mGrabPos);
    f32 h0 = hio_f(0x110);
    f32 h1 = hio_f(0x114);
    f32 range = std::fabs(gabi::fsubs_ppc(h1, h0));
    if (range == 0.0f) {
        range = 1.0f;
    }
    f32 ratio = fm_fdivs_ppc(std::fabs(gabi::fsubs_ppc(dist, h0)), range);
    f32 power = gabi::fmuls_ppc(hio_f(0x118), ratio);
    daPy_setOutPower(pLink, power, targetAngle, 0);
}

/* 02146A34 */
void daFm_c::modeAttack() {
    WWHD_FUNC(0x02146A34, void, this);
    if (mAnmPrmIdx != 5 && !isGrab()) {
        f32 dist = fm_absXZ_mi(&mGrabPos, &current.pos);
        if (!(dist > hio_f(0xE0) && dist < hio_f(0xC4)) && dist < hio_f(0xE4) + 10.0f) {
            field_0x660.copy(mGrabPos);
            f32 step = gabi::fmuls_ppc(gabi::fmuls_ppc(0.005f, gabi::fadds_ppc(hio_f(0xCC), 1.0f)), field_0x394);
            cLib_addCalcPosXZ2(&current.pos, &field_0x660, step, 40.0f);
        }
    }

    if (checkTgHit()) {
        return;
    }
    if (mAnmPrmIdx == 5 && fm_morfIsStop(mpMorf)) {
        fm_modeProcInit(this, 7);
        return;
    }
    if (mpActorTarget == nullptr) {
        fm_setAnm(this, 5, 0);
        return;
    }

    calcInvKine(mpActorTarget);
    s16 actorAngle = fopAcM_searchActorAngleY(this, mpActorTarget);
    if (cLib_distanceAngleS(shape_angle.y, actorAngle) > hio_s(0x9A)) {
        cLib_addCalcAngleS2(&shape_angle.y, actorAngle, 4, hio_s(0x96));
    }

    if (field_0xAE4 == 0) {
        fm_modeProcInit(this, 6);
        return;
    }
    if (fm_checkPlayerStatus0(0x402 /* UNK400 | UNK2 */)) {
        f32 oldGrabPosY = mGrabPos.y;
        fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
        mGrabPos.copy(pLink->current.pos);
        mGrabPos.y = oldGrabPosY;
        return;
    }
    if (!isGrabPos()) {
        fm_setAnm(this, 5, 0);
        return;
    }
    setGrabPos();

    if (isGrab()) {
        fopAc_ac_c* t = mpActorTarget;
        if (fm_isLink(this, t) && mAnmPrmIdx == 4) {
            if (fm_morfIsStop(mpMorf)) {
                fm_modeProcInit(this, 0xD);
                fm_seStart(this, JA_SE_CM_FM_GRAB_HAND, 0);
                daPy_voiceStart(dComIfGp_getLinkPlayer(), 0x1C);
            } else {
                fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
                if (pLink->speedF < 1.0f) {
                    return;
                }
                f32 dist = fm_absXZ_mi(&mGrabPos, &field_0x63C);
                if (!(dist > hio_f(0x110) && dist < hio_f(0x114))) {
                    return;
                }
                fm_pushLink(this, pLink, dist);
            }
        } else if (t = mpActorTarget, fm_isNpc(this, t)) {
            if (fm_checkCarryNow(t)) {
                fopAcM_cancelCarryNow(t);
                return;
            }
            fm_seStart(this, JA_SE_CM_FM_GRAB_HAND, 0);
            fm_setCarryNow(mpActorTarget, 0);
            fopAc_ac_c* npc = mpActorTarget;
            field_0x684 = 4;
            if (npc != nullptr && fm_GetName(npc) == fpcNm_NPC_CB1_e) {
                /* daNpc_Cb1_c::noCarryAction(): static flag word |= 1 */
                gabi::store<u16>(0x101BDC2A, (u16)(gabi::load<u16>(0x101BDC2A) | 1));
                npc = mpActorTarget;
            }
            if (npc != nullptr && fm_GetName(npc) == fpcNm_NPC_MD_e) {
                /* daNpc_Md_c::noCarryAction(): flags +0x420C |= 0x800 */
                gabi::store<u32>(gabi::ea(npc) + 0x420C, gabi::load<u32>(gabi::ea(npc) + 0x420C) | 0x800);
            }
            u8 sw = field_0x2E4;
            field_0x6B8 = 1;
            if (sw != 0) {
                if (dComIfGp_evmng_startCheckOld(STR(0x1000F8AC) /* "DEFAULT_FM_SW_APEEAR" */)) {
                    dComIfGp_event_reset();
                }
                fm_modeProcInit(this, 0xD);
            } else {
                f32 playerDist = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
                fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
                if ((playerDist > hio_f(0xBC) || std::fabs(pLink->current.pos.y - current.pos.y) > hio_f(0xA8)) &&
                    !dComIfGp_event_runCheck()) {
                    fm_modeProcInit(this, 0x10);
                } else {
                    fm_modeProcInit(this, 0xD);
                }
            }
        } else {
            if (t == nullptr) {
                return;
            }
            s16 procName = fm_GetName(t);
            if (procName == fpcNm_BOMB_e) {
                if (fm_checkCarryNow(t)) {
                    fm_modeProcInit(this, 7);
                    return;
                }
                fm_setCarryNow(t, 0);
                field_0x684 = 2;
                fm_seStart(this, JA_SE_CM_FM_GRAB, 0);
                fm_modeProcInit(this, 0xD);
            } else if (t != nullptr && fm_GetName(t) == fpcNm_TSUBO_e) {
                if (fm_checkCarryNow(t)) {
                    fopAcM_cancelCarryNow(t);
                    return;
                }
                fm_setCarryNow(t, 0);
                field_0x684 = 3;
                fm_seStart(this, JA_SE_CM_FM_GRAB, 0);
                fm_modeProcInit(this, 0xD);
            }
        }
    } else {
        fopAc_ac_c* t = mpActorTarget;
        if (fm_isLink(this, t)) {
            if (mAnmPrmIdx != 4) {
                return;
            }
            f32 dist = fm_absXZ_mi(&mGrabPos, &field_0x63C);
            if (dist > hio_f(0x110) && dist < hio_f(0x114)) {
                fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
                if (pLink->speedF < 1.0f) {
                    return;
                }
                fm_pushLink(this, pLink, dist);
            } else if (fm_morfIsStop(mpMorf) && cLib_calcTimer(&field_0x650) == 0) {
                fm_setAnm(this, 5, 0);
            }
        } else if (mAnmPrmIdx == 4) {
            if (fm_morfIsStop(mpMorf) && cLib_calcTimer(&field_0x650) == 0) {
                fm_setAnm(this, 5, 0);
            }
        }
    }
}
VERIFY(0x02146A34, &daFm_c::modeAttack);

/* 021473FC (unnamed) */
void daFm_c::modeThrowInit() {
    WWHD_FUNC(0x021473FC, void, this);
    fm_setAnm(this, 0xB, 0);
}
VERIFY(0x021473FC, &daFm_c::modeThrowInit);
