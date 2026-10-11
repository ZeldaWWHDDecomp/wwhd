/**
 * d_a_fm_c.cpp (WWHD)
 * Enemy - Floormaster: part C (02147408..0214996C): throw, damage, grab, demos, death, paralysis.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fm_c.h"

/* inlined into daFm_c::modeGrab */
// HD: the float-copy vector and sqrtf preserve the verified arithmetic.
static inline f32 fm_absXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    return std_sqrtf(PSVECSquareMag(v));
}

// HD: these strcmp uses were compiled to byte-by-byte equality loops.
static inline bool fm_streq(const char* a, const char* b) {
    const be<u8>* left = fm_c::stringBytes(a);
    const be<u8>* right = fm_c::stringBytes(b);
    for (u32 i = 0;; i++) {
        u8 ca = left[i], cb = right[i];
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
}

// Numeric actor and HIO fields retain their names from zeldaret/tww.

/* 02147408 */
void daFm_c::modeThrow() {
    fm_resetInvKine(this);
    if (mAnmPrmIdx == 1 && fm_calcTimer(&field_0x650) == 0) {
        fm_modeProcInit(this, fm_c::MODE_WAIT);
    }
    if (field_0x684 == 0) {
        return;
    }
    if (mpActorTarget == nullptr) {
        fm_modeProcInit(this, fm_c::MODE_DAMAGE);
        return;
    }
    if (fm_checkTgHit(this)) {
        return;
    }

    if (field_0x2D0 != 2 || !fm_isNpc(this, mpActorTarget)) {
        s16 angle = fopAcM_searchPlayerAngleY(this);
        cLib_addCalcAngleS2(&shape_angle.y, angle, 4, 0x800);
    }

    if (mpMorf->getFrame() == fm_c::hio().field_0x0FC) {
        fopAc_ac_c* target = mpActorTarget;
        if (fm_isNpc(this, target)) {
            target->speedF = fm_c::hio().field_0x0F0;
            mpActorTarget->speed.y = fm_c::hio().field_0x0F4;
            fopAcM_cancelCarryNow(mpActorTarget);
            mGrabPos.copy(current.pos);
            field_0x684 = 0;
        } else if (target != nullptr && (fopAcM_GetName(target) == fpcNm_BOMB_e || fopAcM_GetName(target) == fpcNm_TSUBO_e)) {
            target->current.angle.y = shape_angle.y;
            mpActorTarget->shape_angle.y = shape_angle.y;
            mpActorTarget->speedF = fm_c::hio().field_0x0F0;

            f32 temp = field_0x9D4 / (REG_F(12, 13) + 1000.0f);
            // HD: unordered floating-point tests follow the HD branch conditions.
            if (!(temp > 0.0f)) {
                temp = 0.0f;
            } else if (!(temp < 1.0f)) {
                temp = 1.0f;
            }
            f32 dropspeed = temp * fm_c::hio().field_0x0F4;
            fopAc_ac_c* tsubo = mpActorTarget;
            if (tsubo != nullptr && fopAcM_GetName(tsubo) == fpcNm_TSUBO_e) {
                switch (daObj_PrmAbstract(tsubo, 4, 0x18)) { /* prm_get_type() */
                case 0: case 1: case 2: case 4: case 5: case 6:
                    tsubo->speed.y = dropspeed;                         /* set_drop_spd_y0 */
                    static_cast<fm_c::Tsubo*>(tsubo)->m810 = 1;        /* m810 */
                    break;
                }
            } else {
                tsubo->speed.y = dropspeed; /* set_drop_spd_y0 */
            }
            mpActorTarget->gravity = fm_c::hio().field_0x0F8;
            fopAcM_cancelCarryNow(mpActorTarget);
            mGrabPos.copy(current.pos);
            field_0x684 = 0;
        }
    }

    if (mAnmPrmIdx == 0xB && mpMorf->isStop()) {
        field_0x650 = 0x3C;
    }
}

/* 02147724 */
void daFm_c::modeDamageInit() {
    fm_setAnm(this, 7, true);
    field_0x650 = fm_c::hio().field_0x0A6;
}

/* 02147764 */
void daFm_c::modeDamage() {
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (mAnmPrmIdx == 1 && field_0x64C == 0) {
        fm_searchTarget(this);
        fopAc_ac_c* target = mpActorTarget;
        if (target != nullptr && !fm_isNpc(this, target)) {
            fm_modeProcInit(this, fm_c::MODE_WAIT);
        }
        if (!fm_calcTimer(&field_0x650)) {
            fm_modeProcInit(this, fm_c::MODE_WAIT);
        }
    }
    fm_checkTgHit(this);
}

/* 02147810 */
void daFm_c::modeGrabInit() {
    u32 link = fm_isLink(this, mpActorTarget);
    fopAc_ac_c* target = mpActorTarget;
    if (link) {
        mSinkTimer = fm_c::hio().field_0x090;
    } else {
        mSinkTimer = fm_c::hio().field_0x09E;
    }
    field_0x650 = fm_c::hio().field_0x092;
    if (fm_isNpc(this, target)) {
        fm_setAnm(this, 0xC, false);
    } else {
        s16 procName = fopAcM_GetName(target);
        if (target != nullptr && (procName == fpcNm_BOMB_e || fopAcM_GetName(target) == fpcNm_TSUBO_e)) {
            fm_setAnm(this, 9, false);
        }
    }
    field_0x660.copy(field_0x690);
}

/* 02147914 */
void daFm_c::moveRndEscape() {
    f32 dist = fopAcM_searchPlayerDistanceXZ(this);

    if (fm_calcTimer(&field_0x648) == 0 || field_0xAE4 == 0 || mObjAcch.ChkWallHit()) {
        if (dist < fm_c::hio().field_0x0B4) {
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
            field_0x660.z = gabi::fmadds(fm_c::hio().field_0x0B8, cM_scos(field_0x680), z);
            field_0x660.x = gabi::fmadds(fm_c::hio().field_0x0B8, cM_ssin(field_0x680), x);
            field_0x648 = fm_c::hio().field_0x0A0;
        }

        if (cM_rndF(100.0f) < 33.0f) {
            fm_setAnm(this, 0xC, false);
        }
    }

    if (mAnmPrmIdx != 0xC && field_0xAE4 != 0 && !mObjAcch.ChkWallHit()) {
        fm_addCalcPosXZ2(&current.pos, &field_0x660, 0.005f * (fm_c::hio().field_0x0CC + 1.0f) * field_0x394, fm_c::hio().field_0x0B0);
        field_0x690.copy(current.pos);
    }
}

/* 02147BE0 */
void daFm_c::modeGrab() {
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
        fm_modeProcInit(this, fm_c::MODE_DAMAGE);
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
        if (cLib_distanceAngleS(shape_angle.y, angle) > fm_c::hio().field_0x09A) {
            cLib_addCalcAngleS2(&shape_angle.y, angle, 4, fm_c::hio().field_0x096);
        }
    } else {
        f32 dist = fopAcM_searchPlayerDistanceXZ(this);
        if (field_0x2D0 == 2 && fm_isNpc(this, mpActorTarget) && dist < fm_c::hio().field_0x0C4) {
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
            fm_modeProcInit(this, fm_c::MODE_WAIT);
            return;
        }
        if (fm_calcTimer(&mSinkTimer) == 0 && !fm_c::hio().field_0x00D && !dComIfGp_event_runCheck()) {
            fm_modeProcInit(this, fm_c::MODE_GRAB_DEMO);
            field_0x684 = 1;
            return;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&mGrabPos, diff, &field_0x63C);
        f32 dist = fm_absXZ(diff->x, diff->z);
        if (dist > fm_c::hio().field_0x110 && dist < fm_c::hio().field_0x114) {
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            angle = cLib_targetAngleY(&field_0x63C, &mGrabPos);
            f32 lo = fm_c::hio().field_0x110;
            f32 temp4 = std::fabs(fm_c::hio().field_0x114 - lo);
            if (temp4 == 0.0f) {
                temp4 = 1.0f;
            }
            f32 temp5 = fm_c::hio().field_0x118 * (std::fabs(dist - lo) / temp4);
            daPy_setOutPower(link, temp5, angle, 0);
        } else if (!fm_isGrab(this)) {
            fm_setAnm(this, 5, false);
            fm_modeProcInit(this, fm_c::MODE_WAIT);
        }
        if (fm_isGrabFoot(this)) {
            fm_modeProcInit(this, fm_c::MODE_DISAPPEAR);
        }
        return;
    }

    fopAc_ac_c* t2 = mpActorTarget;
    if (fm_isNpc(this, t2)) {
        mGrabPos.copy(field_0x61C);
        if (field_0x2E4 != 0) {
            if (!fm_isLinkControl(this)) {
                fm_modeProcInit(this, fm_c::MODE_GRAB_NPC_DEMO);
            }
            return;
        }
        f32 dist = fopAcM_searchPlayerDistanceXZ(this);
        if (dist < fm_c::hio().field_0x0B4 && field_0x2D0 == 2) {
            angle = cLib_targetAngleY(&current.pos, &field_0x3E4);
            if (cLib_distanceAngleS(shape_angle.y, angle) < 0x500 && dist < fm_c::hio().field_0x0C4) {
                fm_modeProcInit(this, fm_c::MODE_THROW);
                return;
            }
        }
        if (fm_calcTimer(&mSinkTimer) == 0) {
            if (!fm_c::hio().field_0x00D && !dComIfGp_event_runCheck()) {
                fm_modeProcInit(this, fm_c::MODE_GRAB_DEMO);
            }
        } else if (field_0x2DC == 1) {
            fm_modeProcInit(this, fm_c::MODE_THROW);
        }
    } else if (t2 != nullptr && (fopAcM_GetName(t2) == fpcNm_BOMB_e || fopAcM_GetName(t2) == fpcNm_TSUBO_e)) {
        fm_moveRndBack(this);
        mGrabPos.copy(field_0x61C);
        if (!daPy_checkPlayerGuard(dComIfGp_getLinkPlayer())) {
            if (fm_calcTimer(&field_0x650) == 0 && !fm_c::hio().field_0x00D) {
                fm_modeProcInit(this, fm_c::MODE_THROW);
            }
        } else {
            field_0x650 = fm_c::hio().field_0x092;
        }
    }
}

/* 021482AC */
void daFm_c::modeGrabDemoInit() {
    fm_setAnm(this, 6, false);
    field_0x3FC = (u8)fm_isLink(this, mpActorTarget); /* the result byte as returned */
}

/* 021482F0 */
void daFm_c::modeGrabDemo() {
    fm_resetInvKine(this);

    if (eventInfo_checkCommandDemoAccrpt(this)) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        fm_c::player(link)->mDemo.mDemoType = fm_c::Demo::TYPE_ORIGINAL_e; /* changeOriginalDemo() */
        fm_c::player(link)->mDemo.mParam0 = 0;
        if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 15.0f) {
            fm_seStart(this, JA_SE_CM_FM_PULL_IN, 0);
        }
        if (field_0x3FC) {
            fopAc_ac_c* actor = fopAcM_SearchByID(daPy_getGrabActorID(link));
            if (actor != nullptr) {
                fopAcM_cancelCarryNow(actor);
            }
            if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 40.0f) {
                daPy_voiceStart(link, 0xC);
            }
            fm_c::player(link)->mDemo.mDemoMode = fm_c::Demo::DEMO_UNK_030_e; /* changeDemoMode(0x1E) */
            fm_c::player(link)->mDemo.mParam0 = 1;    /* changeDemoParam0(1) */
            gabi::Local<cXyz> temp;
            temp->set(0.0f, 0.0f, 0.0f);
            cLib_chasePos(&field_0x610, temp, 5.0f);

            if (fm_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_GRAB")) {
                if (field_0x2C7 != 0xFF) {
                    dLib_setNextStageBySclsNum(field_0x2C7, current.roomNo);
                } else {
                    fm_setNextStage(gabi::ea(dComIfGp_getStartStageName()), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
                }
            }
        } else {
            if (mAnmPrmIdx == 6 && mpMorf->getFrame() == 40.0f) {
                fopAc_ac_c* t = mpActorTarget;
                if (fopAcM_GetName(t) == fpcNm_NPC_MD_e) {
                    fm_monsSeStart(this, JA_SE_CV_MD_DAMAGE, 0);
                } else if (fopAcM_GetName(t) == fpcNm_NPC_CB1_e) {
                    fm_monsSeStart(this, JA_SE_CV_CB_DAMAGE, 0);
                }
            }
            fopAc_ac_c* md = mpActorTarget;
            if (md != nullptr && fopAcM_GetName(md) == fpcNm_NPC_MD_e) {
                daNpc_Md_changeCaught02(md);
                field_0x688 = 1;
            }
            if (fm_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_SUIKOMI_NPC")) {
                if (field_0x2C8 != 0xFF) {
                    fm_cancelGrab(this);
                    fopAc_ac_c* t = mpActorTarget;
                    if (t != nullptr && fopAcM_GetName(t) == fpcNm_NPC_CB1_e) {
                        dComIfGs_onEventBit(0x3408);
                        s16 point = field_0x2C8;
                        daPy_npc_setPointRestart(mpActorTarget, point, 1);
                    }
                    t = mpActorTarget;
                    if (t != nullptr && fopAcM_GetName(t) == fpcNm_NPC_MD_e) {
                        dComIfGs_onEventBit(0x3404);
                        s16 point = field_0x2C8;
                        daPy_npc_setPointRestart(mpActorTarget, point, 2);
                    }
                }
                dComIfGp_event_reset();
                fm_modeProcInit(this, fm_c::MODE_DELETE);
            }
        }
    } else if (fm_isLink(this, mpActorTarget)) {
        fopAcM_orderOtherEvent2(this, "DEFAULT_FM_GRAB", 1, 0xFFFF);
    } else if (fm_isNpc(this, mpActorTarget)) {
        if (field_0x2E4 != 0) {
            fopAcM_orderChangeEvent(this, "DEFAULT_FM_SUIKOMI_NPC", 0, 0xFFFF);
        } else {
            fopAcM_orderOtherEvent2(this, "DEFAULT_FM_SUIKOMI_NPC", 1, 0xFFFF);
        }
    }
}

/* 0214879C */
void daFm_c::modeGrabFootDemoInit() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    field_0x610.set(0.0f, 0.0f, 0.0f);
    field_0x630.copy(link->current.pos);
}

/* 021487F8 */
void daFm_c::modeGrabFootDemo() {
    fm_resetInvKine(this);
    if (fm_c::hio().field_0x00D) {
        fm_modeProcInit(this, fm_c::MODE_APPEAR);
        return;
    }
    if (!eventInfo_checkCommandDemoAccrpt(this)) {
        fopAcM_orderOtherEvent2(this, "DEFAULT_FM_GRAB_FOOT", 1, 0xFFFF);
        return;
    }

    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Fmaster", nullptr, 0);
    const char* cutName = fm_getMyNowCutName(dComIfGp_getPEvtManager(), staffIdx);
    if (cutName == 0) {
        JUT_ASSERT_fail("d_a_fm.cpp", 0xB0A, "cut_name != (0)"); /* strcmp argument check */
    } else {
        if (fm_streq(cutName, "Dummy") || fm_streq(cutName, "WAIT")) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (fm_streq(cutName, "GRAB_FOOT") && mAnmPrmIdx == 10 && mpMorf->getFrame() == 10.0f) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fopAc_ac_c* ac = fopAcM_SearchByID(daPy_getGrabActorID(link));
    if (ac != nullptr) {
        fopAcM_cancelCarryNow(ac);
    }
    fm_c::player(link)->mDemo.mDemoType = fm_c::Demo::TYPE_ORIGINAL_e; /* changeOriginalDemo() */
    fm_c::player(link)->mDemo.mParam0 = 0;

    if (mAnmPrmIdx != 10) {
        if (fopAcM_searchPlayerDistanceXZ(this) < 3.0f) {
            fm_setAnm(this, 10, false);
        } else {
            fm_addCalcPosXZ2(&field_0x630, &current.pos, 0.3f, 10.0f);
            daPy_setPlayerPosAndAngle(link, &field_0x630, &link->current.angle);
        }
    } else {
        if (mpMorf->getFrame() == 25.0f) {
            fm_seStart(this, JA_SE_CM_FM_PULL_IN, 0);
        }
        if (mpMorf->getFrame() == 10.0f) {
            fm_seStart(this, JA_SE_CM_FM_GRAB_FOOT, 0);
            daPy_voiceStart(link, 0x1C);
        }
        if (mpMorf->getFrame() == 10.0f) {
            fm_c::player(link)->mDemo.mDemoMode = fm_c::Demo::DEMO_PFALL_e; /* changeDemoMode(0x11) */
        }
        // HD: unordered floating-point tests follow the HD branch conditions.
        if (!(mpMorf->getFrame() < 10.0f) && !(mpMorf->getFrame() > 40.0f)) {
            daPy_setPlayerPosAndAngle(link, &field_0x61C, &link->current.angle);
        }
    }
    if (fm_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_GRAB_FOOT")) {
        if (field_0x2C7 != 0xFF) {
            dLib_setNextStageBySclsNum(field_0x2C7, current.roomNo);
        } else {
            fm_setNextStage(gabi::ea(dComIfGp_getStartStageName()), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
        }
    }
}

/* 02148CB4 */
void daFm_c::modeDeathInit() {
    fm_setAnm(this, 3, false);
}

/* 02148CC0 */
void daFm_c::modeDeath() {
    fm_resetInvKine(this);
    fm_cancelGrab(this);
    if (mAnmPrmIdx == 8 && fm_setHoleScale(this, fm_c::hio().field_0x120, 0.1f, fm_c::hio().field_0x128) != 0) {
        dComIfGs_onActor(setID, home.roomNo); /* fopAcM_onActor */
        fopAcM_createDisappear(this, &current.pos, 5, 0 /* daDisItem_IBALL_e */, 0xFF);
        fopAcM_delete(this);
    }
}

/* 02148D68 */
void daFm_c::modePrepareItemInit() {
    fm_setAnm(this, 3, false);
    field_0x650 = fm_c::hio().field_0x092;
    fm_cancelGrab(this);
}

/* 02148DB0 */
void daFm_c::modePrepareItem() {
    if (fm_checkTgHit(this)) {
        return;
    }
    s8 temp = mAnmPrmIdx;
    if (temp == 3) {
        if (mpMorf->isStop()) {
            u32 skullPrm = 0x057F3F3F; /* daTsubo::Act_c::prm_make_skull() */
            mProcId2 = fopAcM_create(fpcNm_TSUBO_e, skullPrm, &field_0x61C, tevStr.mRoomNo, nullptr, nullptr, -1, 0);
        }
        return;
    }
    if (temp == 8) {
        fopAc_ac_c* t = fopAcM_SearchByID(mProcId2);
        mpActorTarget = t;
        if (t == nullptr) {
            JUT_ASSERT_fail("d_a_fm.cpp", 0xD99, "m_grab_actor"); // HD: assertion on the created pot.
            t = mpActorTarget;
        }
        fm_setCarryNow(t, 0);
        field_0x684 = 3;
        fm_setAnm(this, 2, false);
        fm_setGrabPos(this);
    } else if (temp == 2) {
        if (mpMorf->isStop()) {
            fm_setAnm(this, 9, false);
        }
    } else if (temp == 9) {
        if (!daPy_checkPlayerGuard(dComIfGp_getLinkPlayer())) {
            if (fm_calcTimer(&field_0x650) == 0) {
                fm_modeProcInit(this, fm_c::MODE_THROW);
            }
        } else {
            field_0x650 = fm_c::hio().field_0x092;
        }
    }
}

/* 02148F9C */
void daFm_c::modeGrabNpcDemoInit() {
    fm_setAnm(this, 9, false);
}

/* 02148FA8 */
void daFm_c::modeGrabNpcDemo() {
    fm_resetInvKine(this);
    mGrabPos.copy(field_0x61C);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId("Fmaster" /* "Fmaster" */, nullptr, 0);
        const char* cutName = fm_getMyNowCutName(dComIfGp_getPEvtManager(), staffIdx);
        if (cutName == 0) {
            JUT_ASSERT_fail("d_a_fm.cpp", 0xDC1, "cut_name != (0)"); /* strcmp argument check */
            dComIfGp_evmng_cutEnd(staffIdx);
        } else if (fm_streq(cutName, "YAYU")) {
            fm_setAnm(this, 12, false);
            if (mpMorf->isStop()) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        } else {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (fm_endCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_FM_NPC_GRAB" /* "DEFAULT_FM_NPC_GRAB" */)) {
            u8 npc = field_0x2E4;
            dComIfGp_event_reset();
            if (npc != 0) {
                fm_modeProcInit(this, fm_c::MODE_GRAB_DEMO);
            } else {
                fm_modeProcInit(this, fm_c::MODE_GRAB);
            }
        }
    } else if (field_0x2E4 != 0) {
        fopAcM_orderChangeEvent(this, "DEFAULT_FM_NPC_GRAB", 0, 0xFFFF);
    } else {
        fopAcM_orderOtherEvent2(this, "DEFAULT_FM_NPC_GRAB", 1, 0xFFFF);
    }
}

/* 021491B4 */
void daFm_c::modePlayerStartDemoInit() {
    field_0x3E0 = fm_c::hio().field_0x120;
    current.pos.copy(dComIfGp_getPlayer(0)->current.pos);
}

/* 02149208 */
void daFm_c::modePlayerStartDemo() {
    fm_resetInvKine(this);
    if (!dComIfGp_event_runCheck()) {
        return;
    }
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Fmaster" /* "Fmaster" */, nullptr, 0);
    const char* cutName = fm_getMyNowCutName(dComIfGp_getPEvtManager(), staffIdx);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    fm_c::player(link)->mDemo.mDemoType = fm_c::Demo::TYPE_ORIGINAL_e; /* changeOriginalDemo() */
    fm_c::player(link)->mDemo.mParam0 = 0;
    if (cutName == 0) {
        JUT_ASSERT_fail("d_a_fm.cpp", 0xE06, "cut_name != (0)"); /* strcmp argument check */
        return;
    }

    if (fm_streq(cutName, "Dummy") || fm_streq(cutName, "WAIT")) {
        gabi::Local<cXyz> pos;
        f32 y = current.pos.y - REG_F(12, 2);
        pos->x = current.pos.x;
        pos->z = current.pos.z;
        pos->y = y;
        daPy_setPlayerPosAndAngle(link, pos, &link->shape_angle);
        daPy_onPlayerNoDraw(link);
        dComIfGp_evmng_cutEnd(staffIdx);
    } else if (fm_streq(cutName, "OPEN")) {
        gabi::Local<cXyz> pos2;
        f32 y = current.pos.y - REG_F(12, 2);
        pos2->x = current.pos.x;
        pos2->z = current.pos.z;
        pos2->y = y;
        daPy_setPlayerPosAndAngle(link, pos2, &link->shape_angle);
        daPy_onPlayerNoDraw(link);
        if (fm_setHoleScale(this, fm_c::hio().field_0x124, 0.1f, fm_c::hio().field_0x128)) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    } else if (fm_streq(cutName, "PL_OUT")) {
        daPy_offPlayerNoDraw(link);
        daPy_setThrowDamage(link, nullptr, link->shape_angle.y, REG_F(12, 0) + 10.0f, REG_F(12, 1) + 50.0f, 0);
        dComIfGp_evmng_cutEnd(staffIdx);
    } else if (fm_streq(cutName, "CLOSE")) {
        daPy_offPlayerNoDraw(link);
        if (fm_setHoleScale(this, fm_c::hio().field_0x120, 0.1f, fm_c::hio().field_0x128)) {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    } else if (fm_streq(cutName, "DELETE")) {
        fm_modeProcInit(this, fm_c::MODE_DELETE);
    }
}

/* 021495F8 */
void daFm_c::modeDelete() {
    fm_resetInvKine(this);
    u32 done = 0;
    if (field_0x3E0 > 0.015f) { /* isHoleAppear() */
        done = fm_setHoleScale(this, fm_c::hio().field_0x120, 0.1f, fm_c::hio().field_0x128);
    }
    if (!dComIfGp_event_runCheck() && done != 0) {
        fopAcM_delete(this);
    }
}

/* 02149688 */
void daFm_c::modeParalysisInit() {
    fm_setAnm(this, 0xD, true);
    field_0x650 = fm_c::hio().field_0x11C;
}

/* 021496C8 */
void daFm_c::modeParalysis() {
    if (field_0x650 == fm_c::hio().field_0x11C - 0x14) {
        enemy_piyo_set(this);
    }
    if (field_0x650 <= fm_c::hio().field_0x11C - 0x14 && field_0x650 >= fm_c::hio().field_0x11C - 0x3C) {
        fm_seStart(this, JA_SE_CM_MD_PIYO, 0);
    }
    if (field_0x650 == fm_c::hio().field_0x11C - 0x3C) {
        fm_setAnm(this, 1, false);
        field_0x64C = 0x28;
        field_0x68C = 0x2B00;
    }
    if (fm_calcTimer(&field_0x650) == 0) {
        fm_modeProcInit(this, fm_c::MODE_WAIT);
    }
    fm_cancelGrab(this);
    fm_checkTgHit(this);
}

/* 021497C0 */
void daFm_c::modeBikubikuInit() {
    field_0x650 = fm_c::hio().field_0x068;
    mSinkTimer = 5;
    fm_setAnm(this, 0xE, true);
    field_0x650 = fm_c::hio().field_0x068;
}

/* 02149818 */
void daFm_c::modeBikubiku() {
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    mGrabPos.copy(link->current.pos);
    if (mAnmPrmIdx == 1) {
        fm_resetInvKine(this);
    } else if (mAnmPrmIdx != 0xE) {
        fm_calcInvKine(this, link);
    } else if (fm_calcTimer(&mSinkTimer) != 0) {
        fm_calcInvKine(this, link);
    }

    if (mAnmPrmIdx == 0xE) {
        if (fm_calcTimer(&field_0x650) == 0) {
            if (health <= 0) {
                fm_modeProcInit(this, fm_c::MODE_DEATH);
                return;
            }
            fm_setAnm(this, 1, true);
            field_0x64C = REG_S(12, 3) + 0x28;
            field_0x68C = 0x2B00;
        }
    } else if (mAnmPrmIdx == 1 && field_0x64C == 0) {
        fm_modeProcInit(this, fm_c::MODE_WAIT);
    }
}

/* WWHD glue
ext 025D77DC void fopAcM_orderOtherEvent2(fopAc_ac_c* actor, const char* name, u16 flag, u16 hind)
ext 0252012C void fm_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe)
ext 021418D0 void fm_setAnm(daFm_c* t, s8 idx, bool force)
ext 021412BC void fm_modeProc(daFm_c* t, int proc, int mode)
ext 02141280 u32 fm_isLink(daFm_c* t, fopAc_ac_c* a)
ext 02142BC4 u32 fm_isNpc(daFm_c* t, fopAc_ac_c* a)
ext 021441D0 void fm_resetInvKine(daFm_c* t)
ext 021468E8 void fm_calcInvKine(daFm_c* t, fopAc_ac_c* a)
ext 02142028 void fm_cancelGrab(daFm_c* t)
ext 02145A60 void fm_searchTarget(daFm_c* t)
ext 02145070 u32 fm_checkTgHit(daFm_c* t)
ext 02142450 u32 fm_setHoleScale(daFm_c* t, f32 a, f32 b, f32 c)
ext 02041CAC void enemy_piyo_set(fopAc_ac_c* a)
ext 02145DE4 void fm_setGrabPos(daFm_c* t)
ext 0214594C u32 fm_checkPlayerGrabBomb(daFm_c* t)
ext 021467D8 u32 fm_isGrab(daFm_c* t)
ext 0214472C u32 fm_isGrabFoot(daFm_c* t)
ext 021446EC u32 fm_isLinkControl(daFm_c* t)
ext 02144DE4 void fm_moveRndBack(daFm_c* t)
ext 02587EFC void dLib_setNextStageBySclsNum(u8 no, s8 roomNo)
ext 025D78FC void fopAcM_orderChangeEvent(fopAc_ac_c* a, const char* name, u16 flag, u16 hind)
ext 02445518 void daPy_npc_setPointRestart(fopAc_ac_c* npc, s16 point, s8 opt)
ext 0228A4E4 void daNpc_Md_changeCaught02(fopAc_ac_c* md)
ext 0200F62C void cLib_chasePos(cXyz* p, const cXyz* t, f32 step)
ext 025D9D0C void fm_setCarryNow(fopAc_ac_c* a, int stageLayer)
ext 02048038 u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh)
ext 0211D2F8 s32 fm_calcTimer(be<s32>* timer)
ext 0200F268 void fm_addCalcPosXZ2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep)
ext 025E1AA4 void fm_monsSound(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb)
ext 0254457C BOOL fm_endCheckOld(void* manager, const char* name)
ext 02544830 const char* fm_getMyNowCutName(void* manager, s32 staffId)
str 1000F980 "cut_name != (0)" in 02148FA8
str 1000F904 "Dummy" in 021487F8
str 1000F90C "WAIT" in 021487F8
str 1000F990 "DEFAULT_FM_NPC_GRAB" in 02148FA8
str 1000F914 "cut_name != (0)" in 021487F8
str 1000F924 "GRAB_FOOT" in 021487F8
str 1000F9A4 "d_a_fm.cpp" in 02148FA8
str 1000F978 "YAYU" in 02148FA8
str 1000F930 "d_a_fm.cpp" in 021487F8
str 1000F9B0 "Fmaster" in 02149208
str 1000F9B8 "Dummy" in 02149208
str 1000F9C0 "CLOSE" in 02149208
str 1000F9C8 "WAIT" in 02149208
str 1000F8D0 "DEFAULT_FM_GRAB" in 021482F0
str 1000F93C "DEFAULT_FM_GRAB_FOOT" in 021487F8
str 1000F9D0 "OPEN" in 02149208
str 1000F954 "d_a_fm.cpp" in 02148DB0
str 1000F9D8 "PL_OUT" in 02149208
str 1000F8E0 "DEFAULT_FM_SUIKOMI_NPC" in 021482F0
str 1000F960 "m_grab_actor" in 02148DB0
str 1000F9E0 "DELETE" in 02149208
str 1000F9E8 "cut_name != (0)" in 02149208
str 1000F970 "Fmaster" in 02148FA8
str 1000F9F8 "d_a_fm.cpp" in 02149208
str 1000F8FC "Fmaster" in 021487F8
*/
