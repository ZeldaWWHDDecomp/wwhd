/**
 * d_a_fm_c.cpp (WWHD)
 * Enemy - Floormaster: part C (02147408..0214996C): throw, damage, grab, demos, death, paralysis.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fm_local.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daFm_c functions in the other parts (WWHD addresses) */
static inline void fm_setAnm(daFm_c* t, s8 idx, bool force) { gabi::call(0x021418D0, t, idx, force); } /* matcher: checkPlayerGrabBomb */
static inline void fm_modeProc(daFm_c* t, int proc, int mode) { gabi::call(0x021412BC, t, proc, mode); }
static inline void fm_modeProcInit(daFm_c* t, int mode) { fm_modeProc(t, 0, mode); }
static inline u32 fm_isLink(daFm_c* t, fopAc_ac_c* a) { return gabi::call<u32>(0x02141280, t, a); }
static inline u32 fm_isNpc(daFm_c* t, fopAc_ac_c* a) { return gabi::call<u32>(0x02142BC4, t, a); }
static inline void fm_resetInvKine(daFm_c* t) { gabi::call(0x021441D0, t); }
static inline void fm_calcInvKine(daFm_c* t, fopAc_ac_c* a) { gabi::call(0x021468E8, t, a); }
static inline void fm_cancelGrab(daFm_c* t) { gabi::call(0x02142028, t); }
static inline void fm_searchTarget(daFm_c* t) { gabi::call(0x02145A60, t); }
static inline u32 fm_checkTgHit(daFm_c* t) { return gabi::call<u32>(0x02145070, t); }
static inline u32 fm_setHoleScale(daFm_c* t, f32 a, f32 b, f32 c) { return gabi::call<u32>(0x02142450, t, a, b, c); }
static inline void enemy_piyo_set(fopAc_ac_c* a) { gabi::call(0x02041CAC, a); }
/* fpcM_GetName with the HD null check */
static inline s16 fm_getName(fopAc_ac_c* a) { return a != nullptr ? gabi::load<s16>(gabi::ea(a) + 8) : (s16)-1; }

static inline void fm_setGrabPos(daFm_c* t) { gabi::call(0x02145DE4, t); }
static inline u32 fm_checkPlayerGrabBomb(daFm_c* t) { return gabi::call<u32>(0x0214594C, t); } /* unnamed by the matcher */
static inline u32 fm_isGrab(daFm_c* t) { return gabi::call<u32>(0x021467D8, t); }
static inline u32 fm_isGrabFoot(daFm_c* t) { return gabi::call<u32>(0x0214472C, t); }
static inline u32 fm_isLinkControl(daFm_c* t) { return gabi::call<u32>(0x021446EC, t); } /* unnamed by the matcher */
static inline void fm_moveRndBack(daFm_c* t) { gabi::call(0x02144DE4, t); }
#define FM_SEARCHNEARFM_CB 0x02141064u /* searchNearFm_CB (unnamed by the matcher) */
/* cXyz::absXZ (inline): |(x, 0, z)| through a stack vector */
static inline f32 fm_absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    return std_sqrtf(PSVECSquareMag(v));
}
/* daPy_py_c virtuals (HD vtable at +0xB4) */
static inline u32 daPy_vfn(fopAc_ac_c* p, u32 off) { return gabi::load<u32>(p->__vtbl + off); }
static inline void daPy_setOutPower(fopAc_ac_c* p, f32 pow, s16 angle, int a) { gabi::call_ptr(daPy_vfn(p, 0xEC), p, pow, angle, a); }
static inline u32 daPy_checkPlayerGuard(fopAc_ac_c* p) { return gabi::call_ptr<u32>(daPy_vfn(p, 0x3C), p); }

/* events / stages */
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline void dComIfGp_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
static inline u32 dComIfGp_getStartStageName() { return dComIfGp_ea() + 0x5134; }
static inline void dLib_setNextStageBySclsNum(u8 no, s8 roomNo) { gabi::call(0x02587EFC, no, roomNo); }
static inline void fopAcM_orderChangeEvent(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) { gabi::call(0x025D78FC, a, name, flag, hind); }
static inline void dComIfGs_onEventBit(u16 bit) { gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x644, bit); }
static inline void daPy_npc_setPointRestart(fopAc_ac_c* npc, s16 point, s8 opt) { gabi::call(0x02445518, npc, point, opt); }
static inline void daNpc_Md_changeCaught02(fopAc_ac_c* md) { gabi::call(0x0228A4E4, md); }
static inline void cLib_chasePos(cXyz* p, const cXyz* t, f32 step) { gabi::call(0x0200F62C, p, t, step); }
/* daPy_py_c inlines */
static inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(daPy_vfn(p, 0xBC), p); }
static inline void daPy_voiceStart(fopAc_ac_c* p, u32 id) { gabi::call_ptr(daPy_vfn(p, 0xE4), p, id); }
#define STR_DEFAULT_FM_GRAB STR(0x1000F8D0)
#define STR_DEFAULT_FM_SUIKOMI_NPC STR(0x1000F8E0)
static inline u32 dComIfGp_evmng_getMyNowCutName(s32 staffId) { return gabi::call<u32>(0x02544830, dComIfGp_getPEvtManager(), staffId); }
/* strcmp(a, b) == 0 (inlined byte loop; only the equality is used) */
static inline bool fm_streq(u32 a, u32 b) {
    for (u32 i = 0;; i++) {
        u8 ca = gabi::load<u8>(a + i), cb = gabi::load<u8>(b + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
}
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, csXyz* angle) { gabi::call_ptr(daPy_vfn(p, 0x11C), p, pos, angle); }
#define STR_FMASTER STR(0x1000F8FC)
#define STR_DEFAULT_FM_GRAB_FOOT STR(0x1000F93C)
static inline void fopAcM_setCarryNow(fopAc_ac_c* a, int stageLayer) { gabi::call(0x025D9D0C, a, stageLayer); }
static inline void daPy_setThrowDamage(fopAc_ac_c* p, void* a, s16 angle, f32 speedF, f32 speedY, int b) {
    gabi::call_ptr(daPy_vfn(p, 0x12C), p, a, angle, speedF, speedY, b);
}
/* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw (inline): bit 0x08000000 of +0x3B8 */
static inline void daPy_onPlayerNoDraw(fopAc_ac_c* p) { gabi::store<u32>(gabi::ea(p) + 0x3B8, gabi::load<u32>(gabi::ea(p) + 0x3B8) | 0x08000000); }
static inline void daPy_offPlayerNoDraw(fopAc_ac_c* p) { gabi::store<u32>(gabi::ea(p) + 0x3B8, gabi::load<u32>(gabi::ea(p) + 0x3B8) & ~0x08000000u); }
enum { fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F };

enum { fpcNm_BOMB_e = 0x126, fpcNm_TSUBO_e = 0x1C5 };

/* ---- functions ---- */

/* daObj::PrmAbstract(actor, width, shift) (02048038): daTsubo::Act_c::prm_get_type() */
static inline u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh) { return gabi::call<u32>(0x02048038, a, w, sh); }
static inline BOOL morf_isStop(mDoExt_McaMorf* m) { return m->isStop(); }
static inline s8 fm_tevStr_roomNo(fopAc_ac_c* a) { return gabi::load<s8>(gabi::ea(a) + 0x1C9); } /* tevStr.mRoomNo */

/* 02147408 */
void daFm_c::modeThrow() {
    WWHD_FUNC(0x02147408, void, this);
    fm_resetInvKine(this);
    if (mAnmPrmIdx == 1 && cLib_calcTimer(&field_0x650) == 0) {
        fm_modeProcInit(this, 7);
    }
    if (field_0x684 == 0) {
        return;
    }
    if (mpActorTarget == nullptr) {
        fm_modeProcInit(this, 10);
        return;
    }
    if (fm_checkTgHit(this)) {
        return;
    }

    if (field_0x2D0 != 2 || !fm_isNpc(this, mpActorTarget)) {
        s16 angle = fopAcM_searchPlayerAngleY(this);
        cLib_addCalcAngleS2(&shape_angle.y, angle, 4, 0x800);
    }

    if (mpMorf->getFrame() == hio_f(0xFC)) {
        fopAc_ac_c* target = mpActorTarget;
        if (fm_isNpc(this, target)) {
            target->speedF = hio_f(0xF0);
            mpActorTarget->speed.y = hio_f(0xF4);
            fopAcM_cancelCarryNow(mpActorTarget);
            mGrabPos.copy(current.pos);
            field_0x684 = 0;
        } else if (target != nullptr && (fm_getName(target) == fpcNm_BOMB_e || fm_getName(target) == fpcNm_TSUBO_e)) {
            target->current.angle.y = shape_angle.y;
            mpActorTarget->shape_angle.y = shape_angle.y;
            mpActorTarget->speedF = hio_f(0xF0);

            f32 temp = field_0x9D4 / (REG_F(12, 13) + 1000.0f);
            /* HD branches: written as the original tests them */
            if (!(temp > 0.0f)) {
                temp = 0.0f;
            } else if (!(temp < 1.0f)) {
                temp = 1.0f;
            }
            f32 dropspeed = temp * hio_f(0xF4);
            fopAc_ac_c* tsubo = mpActorTarget;
            if (tsubo != nullptr && fm_getName(tsubo) == fpcNm_TSUBO_e) {
                switch (daObj_PrmAbstract(tsubo, 4, 0x18)) { /* prm_get_type() */
                case 0: case 1: case 2: case 4: case 5: case 6:
                    tsubo->speed.y = dropspeed;                         /* set_drop_spd_y0 */
                    gabi::store<u8>(gabi::ea(tsubo) + 0x92C, 1);        /* m810 */
                    break;
                }
            } else {
                tsubo->speed.y = dropspeed; /* set_drop_spd_y0 */
            }
            mpActorTarget->gravity = hio_f(0xF8);
            fopAcM_cancelCarryNow(mpActorTarget);
            mGrabPos.copy(current.pos);
            field_0x684 = 0;
        }
    }

    if (mAnmPrmIdx == 0xB && morf_isStop(mpMorf)) {
        field_0x650 = 0x3C;
    }
}
VERIFY(0x02147408, &daFm_c::modeThrow);

/* 02147724 */
void daFm_c::modeDamageInit() {
    WWHD_FUNC(0x02147724, void, this);
    fm_setAnm(this, 7, true);
    field_0x650 = hio_s(0xA6);
}
VERIFY(0x02147724, &daFm_c::modeDamageInit);

/* 02147764 */
void daFm_c::modeDamage() {
    WWHD_FUNC(0x02147764, void, this);
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (mAnmPrmIdx == 1 && field_0x64C == 0) {
        fm_searchTarget(this);
        fopAc_ac_c* target = mpActorTarget;
        if (target != nullptr && !fm_isNpc(this, target)) {
            fm_modeProcInit(this, 7);
        }
        if (!cLib_calcTimer(&field_0x650)) {
            fm_modeProcInit(this, 7);
        }
    }
    fm_checkTgHit(this);
}
VERIFY(0x02147764, &daFm_c::modeDamage);

/* 02147810 */
void daFm_c::modeGrabInit() {
    WWHD_FUNC(0x02147810, void, this);
    u32 link = fm_isLink(this, mpActorTarget);
    fopAc_ac_c* target = mpActorTarget;
    if (link) {
        mSinkTimer = hio_s(0x90);
    } else {
        mSinkTimer = hio_s(0x9E);
    }
    field_0x650 = hio_s(0x92);
    if (fm_isNpc(this, target)) {
        fm_setAnm(this, 0xC, false);
    } else {
        s16 procName = fm_getName(target);
        if (target != nullptr && (procName == fpcNm_BOMB_e || fm_getName(target) == fpcNm_TSUBO_e)) {
            fm_setAnm(this, 9, false);
        }
    }
    field_0x660.copy(field_0x690);
}
VERIFY(0x02147810, &daFm_c::modeGrabInit);

/* 02147914 */
void daFm_c::moveRndEscape() {
    WWHD_FUNC(0x02147914, void, this);
    f32 dist = fopAcM_searchPlayerDistanceXZ(this);

    if (cLib_calcTimer(&field_0x648) == 0 || field_0xAE4 == 0 || mObjAcch.ChkWallHit()) {
        if (dist < hio_f(0xB4)) {
            if (field_0xAE4 == 0 || mObjAcch.ChkWallHit()) {
                s16 old = field_0x680;
                f32 rnd = cM_rndF(3.0f);
                field_0x680 = (s16)(old + (s16)gabi::ftoi(gabi::fmadds(rnd, 4096.0f, (f32)(old + 0x5000))));
            } else {
                f32 base = (f32)(shape_angle.y + 0x3000);
                f32 rnd = cM_rndF(5.0f);
                field_0x680 = (s16)gabi::ftoi(gabi::fmadds(rnd, 4096.0f, base));
            }
            /* field_0x660 = field_0x690 (through the FPRs) */
            f32 x = field_0x690.x;
            f32 y = field_0x690.y;
            f32 z = field_0x690.z;
            field_0x660.x = x;
            field_0x660.y = y;
            field_0x660.z = z;
            field_0x660.z = gabi::fmadds(hio_f(0xB8), cM_scos(field_0x680), z);
            field_0x660.x = gabi::fmadds(hio_f(0xB8), cM_ssin(field_0x680), x);
            field_0x648 = hio_s(0xA0);
        }

        if (cM_rndF(100.0f) < 33.0f) {
            fm_setAnm(this, 0xC, false);
        }
    }

    if (mAnmPrmIdx != 0xC && field_0xAE4 != 0 && !mObjAcch.ChkWallHit()) {
        cLib_addCalcPosXZ2(&current.pos, &field_0x660, 0.005f * (hio_f(0xCC) + 1.0f) * field_0x394, hio_f(0xB0));
        field_0x690.copy(current.pos);
    }
}
VERIFY(0x02147914, &daFm_c::moveRndEscape);

/* 02147BE0 */
void daFm_c::modeGrab() {
    WWHD_FUNC(0x02147BE0, void, this);
    fopAc_ac_c* target;
    if (!fm_isLink(this, mpActorTarget) && (field_0x2D0 == 0 || field_0x2D0 == 1)) {
        target = mpActorTarget;
        if (fm_isNpc(this, target)) {
            moveRndEscape();
            target = mpActorTarget;
        }
    } else {
        target = mpActorTarget;
    }
    if (target == nullptr) {
        fm_modeProcInit(this, 10);
        return;
    }

    if (fm_isLink(this, target)) {
        fm_calcInvKine(this, mpActorTarget);
    } else {
        fm_resetInvKine(this);
    }
    if (fm_checkTgHit(this)) {
        return;
    }

    s16 angle;
    if (fm_isLink(this, mpActorTarget)) {
        angle = cLib_targetAngleY(&current.pos, &mGrabPos);
        if (cLib_distanceAngleS(shape_angle.y, angle) > hio_s(0x9A)) {
            cLib_addCalcAngleS2(&shape_angle.y, angle, 4, hio_s(0x96));
        }
    } else {
        f32 dist = fopAcM_searchPlayerDistanceXZ(this);
        if (field_0x2D0 == 2 && fm_isNpc(this, mpActorTarget) && dist < hio_f(0xC4)) {
            fopAcIt_Judge(FM_SEARCHNEARFM_CB, this); /* fopAcM_Search(searchNearFm_CB, this) */
            if (fm_absXZ(field_0x3E4.x, field_0x3E4.z) != 0.0f) {
                angle = cLib_targetAngleY(&current.pos, &field_0x3E4);
            } else {
                angle = fopAcM_searchPlayerAngleY(this);
            }
        } else {
            angle = fopAcM_searchPlayerAngleY(this);
        }
        cLib_addCalcAngleS2(&shape_angle.y, angle, 4, 0x800);
    }

    if (fm_isLink(this, mpActorTarget)) {
        fm_setGrabPos(this);
        if (fm_checkPlayerGrabBomb(this)) {
            fm_setAnm(this, 7, false);
            fm_modeProcInit(this, 7);
            return;
        }
        if (cLib_calcTimer(&mSinkTimer) == 0 && !hio_u8(0xD) && !dComIfGp_event_runCheck()) {
            fm_modeProcInit(this, 0xE);
            field_0x684 = 1;
            return;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&mGrabPos, diff, &field_0x63C);
        f32 dist = fm_absXZ(diff->x, diff->z);
        if (dist > hio_f(0x110) && dist < hio_f(0x114)) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            angle = cLib_targetAngleY(&field_0x63C, &mGrabPos);
            f32 lo = hio_f(0x110);
            f32 temp4 = std::fabs(hio_f(0x114) - lo);
            if (temp4 == 0.0f) {
                temp4 = 1.0f;
            }
            f32 temp5 = hio_f(0x118) * (std::fabs(dist - lo) / temp4);
            daPy_setOutPower(link, temp5, angle, 0);
        } else if (!fm_isGrab(this)) {
            fm_setAnm(this, 5, false);
            fm_modeProcInit(this, 7);
        }
        if (fm_isGrabFoot(this)) {
            fm_modeProcInit(this, 6);
        }
        return;
    }

    fopAc_ac_c* t2 = mpActorTarget;
    if (fm_isNpc(this, t2)) {
        mGrabPos.copy(field_0x61C);
        if (field_0x2E4 != 0) {
            if (!fm_isLinkControl(this)) {
                fm_modeProcInit(this, 0x10);
            }
            return;
        }
        f32 dist = fopAcM_searchPlayerDistanceXZ(this);
        if (dist < hio_f(0xB4) && field_0x2D0 == 2) {
            angle = cLib_targetAngleY(&current.pos, &field_0x3E4);
            if (cLib_distanceAngleS(shape_angle.y, angle) < 0x500 && dist < hio_f(0xC4)) {
                fm_modeProcInit(this, 9);
                return;
            }
        }
        if (cLib_calcTimer(&mSinkTimer) == 0) {
            if (!hio_u8(0xD) && !dComIfGp_event_runCheck()) {
                fm_modeProcInit(this, 0xE);
            }
        } else if (field_0x2DC == 1) {
            fm_modeProcInit(this, 9);
        }
    } else if (t2 != nullptr && (fm_getName(t2) == fpcNm_BOMB_e || fm_getName(t2) == fpcNm_TSUBO_e)) {
        fm_moveRndBack(this);
        mGrabPos.copy(field_0x61C);
        if (!daPy_checkPlayerGuard(dComIfGp_getLinkPlayer())) {
            if (cLib_calcTimer(&field_0x650) == 0 && !hio_u8(0xD)) {
                fm_modeProcInit(this, 9);
            }
        } else {
            field_0x650 = hio_s(0x92);
        }
    }
}
VERIFY(0x02147BE0, &daFm_c::modeGrab);

/* 021482AC */
void daFm_c::modeGrabDemoInit() {
    WWHD_FUNC(0x021482AC, void, this);
    fm_setAnm(this, 6, false);
    field_0x3FC = (u8)fm_isLink(this, mpActorTarget); /* the result byte as returned */
}
VERIFY(0x021482AC, &daFm_c::modeGrabDemoInit);

/* 021482F0 */
void daFm_c::modeGrabDemo() {
    WWHD_FUNC(0x021482F0, void, this);
    fm_resetInvKine(this);

    if (eventInfo_checkCommandDemoAccrpt(this)) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        gabi::store<u16>(gabi::ea(link) + 0x420, 3); /* changeOriginalDemo() */
        gabi::store<u32>(gabi::ea(link) + 0x428, 0);
        if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 15.0f) {
            fm_seStart(this, 0x58B0 /* JA_SE_CM_FM_PULL_IN */, 0);
        }
        if (field_0x3FC) {
            fopAc_ac_c* actor = fopAcM_SearchByID(daPy_getGrabActorID(link));
            if (actor != nullptr) {
                fopAcM_cancelCarryNow(actor);
            }
            if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 40.0f) {
                daPy_voiceStart(link, 0xC);
            }
            gabi::store<u32>(gabi::ea(link) + 0x430, 0x1E); /* changeDemoMode(0x1E) */
            gabi::store<u32>(gabi::ea(link) + 0x428, 1);    /* changeDemoParam0(1) */
            gabi::Local<cXyz> temp;
            temp->set(0.0f, 0.0f, 0.0f);
            cLib_chasePos(&field_0x610, temp, 5.0f);

            if (dComIfGp_evmng_endCheckOld(STR_DEFAULT_FM_GRAB)) {
                if (field_0x2C7 != 0xFF) {
                    dLib_setNextStageBySclsNum(field_0x2C7, current.roomNo);
                } else {
                    dComIfGp_setNextStage(dComIfGp_getStartStageName(), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
                }
            }
        } else {
            if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 40.0f) {
                fopAc_ac_c* t = mpActorTarget;
                if (fm_getName(t) == fpcNm_NPC_MD_e) {
                    fm_monsSeStart(this, 0x48AA /* JA_SE_CV_MD_DAMAGE */, 0);
                } else if (fm_getName(t) == fpcNm_NPC_CB1_e) {
                    fm_monsSeStart(this, 0x48BF /* JA_SE_CV_CB_DAMAGE */, 0);
                }
            }
            fopAc_ac_c* md = mpActorTarget;
            if (md != nullptr && fm_getName(md) == fpcNm_NPC_MD_e) {
                daNpc_Md_changeCaught02(md);
                field_0x688 = 1;
            }
            if (dComIfGp_evmng_endCheckOld(STR_DEFAULT_FM_SUIKOMI_NPC)) {
                if (field_0x2C8 != 0xFF) {
                    fm_cancelGrab(this);
                    fopAc_ac_c* t = mpActorTarget;
                    if (t != nullptr && fm_getName(t) == fpcNm_NPC_CB1_e) {
                        dComIfGs_onEventBit(0x3408);
                        s16 point = field_0x2C8;
                        daPy_npc_setPointRestart(mpActorTarget, point, 1);
                    }
                    t = mpActorTarget;
                    if (t != nullptr && fm_getName(t) == fpcNm_NPC_MD_e) {
                        dComIfGs_onEventBit(0x3404);
                        s16 point = field_0x2C8;
                        daPy_npc_setPointRestart(mpActorTarget, point, 2);
                    }
                }
                dComIfGp_event_reset();
                fm_modeProcInit(this, 0x12);
            }
        }
    } else if (fm_isLink(this, mpActorTarget)) {
        fopAcM_orderOtherEvent2(this, STR_DEFAULT_FM_GRAB, 1, 0xFFFF);
    } else if (fm_isNpc(this, mpActorTarget)) {
        if (field_0x2E4 != 0) {
            fopAcM_orderChangeEvent(this, STR_DEFAULT_FM_SUIKOMI_NPC, 0, 0xFFFF);
        } else {
            fopAcM_orderOtherEvent2(this, STR_DEFAULT_FM_SUIKOMI_NPC, 1, 0xFFFF);
        }
    }
}
VERIFY(0x021482F0, &daFm_c::modeGrabDemo);

/* 0214879C */
void daFm_c::modeGrabFootDemoInit() {
    WWHD_FUNC(0x0214879C, void, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    field_0x610.set(0.0f, 0.0f, 0.0f);
    field_0x630.copy(link->current.pos);
}
VERIFY(0x0214879C, &daFm_c::modeGrabFootDemoInit);

/* 021487F8 */
void daFm_c::modeGrabFootDemo() {
    WWHD_FUNC(0x021487F8, void, this);
    fm_resetInvKine(this);
    if (hio_u8(0xD)) {
        fm_modeProcInit(this, 5);
        return;
    }
    if (!eventInfo_checkCommandDemoAccrpt(this)) {
        fopAcM_orderOtherEvent2(this, STR_DEFAULT_FM_GRAB_FOOT, 1, 0xFFFF);
        return;
    }

    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR_FMASTER, nullptr, 0);
    u32 cutName = dComIfGp_evmng_getMyNowCutName(staffIdx);
    if (cutName == 0) {
        JUT_ASSERT_fail(STR(0x1000F930), 0xB0A, STR(0x1000F914)); /* strcmp argument check */
    } else {
        if (fm_streq(cutName, 0x1000F904 /* "Dummy" */) || fm_streq(cutName, 0x1000F90C /* "WAIT" */)) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (fm_streq(cutName, 0x1000F924 /* "GRAB_FOOT" */) && mAnmPrmIdx == 10 && mpMorf->getFrame() == 10.0f) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAc_ac_c* ac = fopAcM_SearchByID(daPy_getGrabActorID(link));
    if (ac != nullptr) {
        fopAcM_cancelCarryNow(ac);
    }
    gabi::store<u16>(gabi::ea(link) + 0x420, 3); /* changeOriginalDemo() */
    gabi::store<u32>(gabi::ea(link) + 0x428, 0);

    if (mAnmPrmIdx != 10) {
        if (fopAcM_searchPlayerDistanceXZ(this) < 3.0f) {
            fm_setAnm(this, 10, false);
        } else {
            cLib_addCalcPosXZ2(&field_0x630, &current.pos, 0.3f, 10.0f);
            daPy_setPlayerPosAndAngle(link, &field_0x630, &link->current.angle);
        }
    } else {
        if (mpMorf->getFrame() == 25.0f) {
            fm_seStart(this, 0x58B0 /* JA_SE_CM_FM_PULL_IN */, 0);
        }
        if (mpMorf->getFrame() == 10.0f) {
            fm_seStart(this, 0x58AF /* JA_SE_CM_FM_GRAB_FOOT */, 0);
            daPy_voiceStart(link, 0x1C);
        }
        if (mpMorf->getFrame() == 10.0f) {
            gabi::store<u32>(gabi::ea(link) + 0x430, 0x11); /* changeDemoMode(0x11) */
        }
        /* HD branches: written as the original tests them */
        if (!(mpMorf->getFrame() < 10.0f) && !(mpMorf->getFrame() > 40.0f)) {
            daPy_setPlayerPosAndAngle(link, &field_0x61C, &link->current.angle);
        }
    }
    if (dComIfGp_evmng_endCheckOld(STR_DEFAULT_FM_GRAB_FOOT)) {
        if (field_0x2C7 != 0xFF) {
            dLib_setNextStageBySclsNum(field_0x2C7, current.roomNo);
        } else {
            dComIfGp_setNextStage(dComIfGp_getStartStageName(), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
        }
    }
}
VERIFY(0x021487F8, &daFm_c::modeGrabFootDemo);

/* 02148CB4: modeDeathInit (unnamed by the matcher) */
void daFm_c::modeDeathInit() {
    WWHD_FUNC(0x02148CB4, void, this);
    fm_setAnm(this, 3, false);
}
VERIFY(0x02148CB4, &daFm_c::modeDeathInit);

/* 02148CC0 */
void daFm_c::modeDeath() {
    WWHD_FUNC(0x02148CC0, void, this);
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (mAnmPrmIdx == 8 && fm_setHoleScale(this, hio_f(0x120), 0.1f, hio_f(0x128)) != 0) {
        dComIfGs_onActor(setID, home.roomNo); /* fopAcM_onActor */
        fopAcM_createDisappear(this, &current.pos, 5, 0 /* daDisItem_IBALL_e */, 0xFF);
        fopAcM_delete(this);
    }
}
VERIFY(0x02148CC0, &daFm_c::modeDeath);

/* 02148D68 */
void daFm_c::modePrepareItemInit() {
    WWHD_FUNC(0x02148D68, void, this);
    fm_setAnm(this, 3, false);
    field_0x650 = hio_s(0x92);
    fm_cancelGrab(this);
}
VERIFY(0x02148D68, &daFm_c::modePrepareItemInit);

/* 02148DB0 */
void daFm_c::modePrepareItem() {
    WWHD_FUNC(0x02148DB0, void, this);
    if (fm_checkTgHit(this)) {
        return;
    }
    s8 temp = mAnmPrmIdx;
    if (temp == 3) {
        if (morf_isStop(mpMorf)) {
            u32 skullPrm = 0x057F3F3F; /* daTsubo::Act_c::prm_make_skull() */
            mProcId2 = fopAcM_create(fpcNm_TSUBO_e, skullPrm, &field_0x61C, fm_tevStr_roomNo(this), nullptr, nullptr, -1, 0);
        }
        return;
    }
    if (temp == 8) {
        fopAc_ac_c* t = fopAcM_SearchByID(mProcId2);
        mpActorTarget = t;
        if (t == nullptr) {
            JUT_ASSERT_fail(STR(0x1000F954), 0xD99, STR(0x1000F960)); /* HD: assertion on the created pot */
            t = mpActorTarget;
        }
        fopAcM_setCarryNow(t, 0);
        field_0x684 = 3;
        fm_setAnm(this, 2, false);
        fm_setGrabPos(this);
    } else if (temp == 2) {
        if (morf_isStop(mpMorf)) {
            fm_setAnm(this, 9, false);
        }
    } else if (temp == 9) {
        if (!daPy_checkPlayerGuard(dComIfGp_getLinkPlayer())) {
            if (cLib_calcTimer(&field_0x650) == 0) {
                fm_modeProcInit(this, 9);
            }
        } else {
            field_0x650 = hio_s(0x92);
        }
    }
}
VERIFY(0x02148DB0, &daFm_c::modePrepareItem);

/* 02148F9C: modeGrabNpcDemoInit (unnamed by the matcher) */
void daFm_c::modeGrabNpcDemoInit() {
    WWHD_FUNC(0x02148F9C, void, this);
    fm_setAnm(this, 9, false);
}
VERIFY(0x02148F9C, &daFm_c::modeGrabNpcDemoInit);

/* 02148FA8 */
void daFm_c::modeGrabNpcDemo() {
    WWHD_FUNC(0x02148FA8, void, this);
    fm_resetInvKine(this);
    mGrabPos.copy(field_0x61C);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1000F970) /* "Fmaster" */, nullptr, 0);
        u32 cutName = dComIfGp_evmng_getMyNowCutName(staffIdx);
        if (cutName == 0) {
            JUT_ASSERT_fail(STR(0x1000F9A4), 0xDC1, STR(0x1000F980)); /* strcmp argument check */
            dComIfGp_evmng_cutEnd(staffIdx);
        } else if (fm_streq(cutName, 0x1000F978 /* "YAYU" */)) {
            fm_setAnm(this, 12, false);
            if (morf_isStop(mpMorf)) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        } else {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (dComIfGp_evmng_endCheckOld(STR(0x1000F990) /* "DEFAULT_FM_NPC_GRAB" */)) {
            u8 npc = field_0x2E4;
            dComIfGp_event_reset();
            if (npc != 0) {
                fm_modeProcInit(this, 0xE);
            } else {
                fm_modeProcInit(this, 0xD);
            }
        }
    } else if (field_0x2E4 != 0) {
        fopAcM_orderChangeEvent(this, STR(0x1000F990), 0, 0xFFFF);
    } else {
        fopAcM_orderOtherEvent2(this, STR(0x1000F990), 1, 0xFFFF);
    }
}
VERIFY(0x02148FA8, &daFm_c::modeGrabNpcDemo);

/* 021491B4 */
void daFm_c::modePlayerStartDemoInit() {
    WWHD_FUNC(0x021491B4, void, this);
    field_0x3E0 = hio_f(0x120);
    current.pos.copy(dComIfGp_getPlayer(0)->current.pos);
}
VERIFY(0x021491B4, &daFm_c::modePlayerStartDemoInit);

/* 02149208 */
void daFm_c::modePlayerStartDemo() {
    WWHD_FUNC(0x02149208, void, this);
    fm_resetInvKine(this);
    if (!dComIfGp_event_runCheck()) {
        return;
    }
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1000F9B0) /* "Fmaster" */, nullptr, 0);
    u32 cutName = dComIfGp_evmng_getMyNowCutName(staffIdx);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::store<u16>(gabi::ea(link) + 0x420, 3); /* changeOriginalDemo() */
    gabi::store<u32>(gabi::ea(link) + 0x428, 0);
    if (cutName == 0) {
        JUT_ASSERT_fail(STR(0x1000F9F8), 0xE06, STR(0x1000F9E8)); /* strcmp argument check */
        return;
    }

    if (fm_streq(cutName, 0x1000F9B8 /* "Dummy" */) || fm_streq(cutName, 0x1000F9C8 /* "WAIT" */)) {
        gabi::Local<cXyz> pos;
        f32 y = current.pos.y - REG_F(12, 2);
        pos->x = current.pos.x;
        pos->z = current.pos.z;
        pos->y = y;
        daPy_setPlayerPosAndAngle(link, pos, &link->shape_angle);
        daPy_onPlayerNoDraw(link);
        dComIfGp_evmng_cutEnd(staffIdx);
    } else if (fm_streq(cutName, 0x1000F9D0 /* "OPEN" */)) {
        gabi::Local<cXyz> pos2;
        f32 y = current.pos.y - REG_F(12, 2);
        pos2->x = current.pos.x;
        pos2->z = current.pos.z;
        pos2->y = y;
        daPy_setPlayerPosAndAngle(link, pos2, &link->shape_angle);
        daPy_onPlayerNoDraw(link);
        if (fm_setHoleScale(this, hio_f(0x124), 0.1f, hio_f(0x128))) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    } else if (fm_streq(cutName, 0x1000F9D8 /* "PL_OUT" */)) {
        daPy_offPlayerNoDraw(link);
        daPy_setThrowDamage(link, nullptr, link->shape_angle.y, REG_F(12, 0) + 10.0f, REG_F(12, 1) + 50.0f, 0);
        dComIfGp_evmng_cutEnd(staffIdx);
    } else if (fm_streq(cutName, 0x1000F9C0 /* "CLOSE" */)) {
        daPy_offPlayerNoDraw(link);
        if (fm_setHoleScale(this, hio_f(0x120), 0.1f, hio_f(0x128))) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    } else if (fm_streq(cutName, 0x1000F9E0 /* "DELETE" */)) {
        fm_modeProcInit(this, 0x12);
    }
}
VERIFY(0x02149208, &daFm_c::modePlayerStartDemo);

/* 021495F8 */
void daFm_c::modeDelete() {
    WWHD_FUNC(0x021495F8, void, this);
    fm_resetInvKine(this);
    u32 done = 0;
    if (field_0x3E0 > 0.015f) { /* isHoleAppear() */
        done = fm_setHoleScale(this, hio_f(0x120), 0.1f, hio_f(0x128));
    }
    if (!dComIfGp_event_runCheck() && done != 0) {
        fopAcM_delete(this);
    }
}
VERIFY(0x021495F8, &daFm_c::modeDelete);

/* 02149688 */
void daFm_c::modeParalysisInit() {
    WWHD_FUNC(0x02149688, void, this);
    fm_setAnm(this, 0xD, true);
    field_0x650 = hio_s(0x11C);
}
VERIFY(0x02149688, &daFm_c::modeParalysisInit);

/* 021496C8 */
void daFm_c::modeParalysis() {
    WWHD_FUNC(0x021496C8, void, this);
    if (field_0x650 == hio_s(0x11C) - 0x14) {
        enemy_piyo_set(this);
    }
    if (field_0x650 <= hio_s(0x11C) - 0x14 && field_0x650 >= hio_s(0x11C) - 0x3C) {
        fm_seStart(this, 0x50BC /* JA_SE_CM_MD_PIYO */, 0);
    }
    if (field_0x650 == hio_s(0x11C) - 0x3C) {
        fm_setAnm(this, 1, false);
        field_0x64C = 0x28;
        field_0x68C = 0x2B00;
    }
    if (cLib_calcTimer(&field_0x650) == 0) {
        fm_modeProcInit(this, 7);
    }
    fm_cancelGrab(this);
    fm_checkTgHit(this);
}
VERIFY(0x021496C8, &daFm_c::modeParalysis);

/* 021497C0 */
void daFm_c::modeBikubikuInit() {
    WWHD_FUNC(0x021497C0, void, this);
    field_0x650 = hio_s(0x68);
    mSinkTimer = 5;
    fm_setAnm(this, 0xE, true);
    field_0x650 = hio_s(0x68);
}
VERIFY(0x021497C0, &daFm_c::modeBikubikuInit);

/* 02149818 */
void daFm_c::modeBikubiku() {
    WWHD_FUNC(0x02149818, void, this);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    mGrabPos.copy(link->current.pos);
    if (mAnmPrmIdx == 1) {
        fm_resetInvKine(this);
    } else if (mAnmPrmIdx != 0xE) {
        fm_calcInvKine(this, link);
    } else if (cLib_calcTimer(&mSinkTimer) != 0) {
        fm_calcInvKine(this, link);
    }

    if (mAnmPrmIdx == 0xE) {
        if (cLib_calcTimer(&field_0x650) == 0) {
            if (health <= 0) {
                fm_modeProcInit(this, 0xC);
                return;
            }
            fm_setAnm(this, 1, true);
            field_0x64C = REG_S(12, 3) + 0x28;
            field_0x68C = 0x2B00;
        }
    } else if (mAnmPrmIdx == 1 && field_0x64C == 0) {
        fm_modeProcInit(this, 7);
    }
}
VERIFY(0x02149818, &daFm_c::modeBikubiku);
