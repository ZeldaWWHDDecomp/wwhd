/**
 * d_a_npc_ji1_teach.cpp (WWHD)
 * NPC - Orca: sword training (teachAction and its helpers)
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0200F164 cLib_addCalcPos2(cXyz*, const cXyz&, f32 scale, f32 maxStep) */
static inline void cLib_addCalcPos2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep) {
    gabi::call(0x0200F164, pos, target, scale, maxStep);
}
/* 0200EF78 cLib_addCalcPosXZ(cXyz*, const cXyz&, f32, f32, f32) */
static inline void cLib_addCalcPosXZ(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep, f32 minStep) {
    gabi::call(0x0200EF78, pos, target, scale, maxStep, minStep);
}
/* daPy_py_c::checkCutCharge(): virtual, HD vtable (+0xB4) slot 0x6C */
static inline BOOL daPy_checkCutCharge(fopAc_ac_c* pl) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(pl->__vtbl + 0x6C), pl);
}
/* daPy_py_c::getCutType(): u8 at +0x3AC */
static inline u8 daPy_getCutType(fopAc_ac_c* pl) { return gabi::load<u8>(gabi::ea(pl) + 0x3AC); }
/* daPy_py_c::getCutAtFlg(): mModeFlg (+0x3B8) & 0x40 */
static inline u32 daPy_getCutAtFlg(fopAc_ac_c* pl) { return gabi::load<u32>(gabi::ea(pl) + 0x3B8) & 0x40; }
/* 025E1950 mDoAud_changeBgmStatus(u32) */
static inline void mDoAud_changeBgmStatus(u32 status) { gabi::call(0x025E1950, status); }
/* fopAcM_orderOtherEventId(this, idx) with the GameCube defaults (0xFF, 0xFFFF, 0, 1) */
static inline BOOL ji1_orderOtherEventId(fopAc_ac_c* a, s16 idx) { return fopAcM_orderOtherEventId(a, idx, 0xFF, 0xFFFF, 0, 1); }
/* HD-only file static (0x10467244): a failed training cut is forgiven once before the
 * "bad cut" event (set by the first failure, cleared when an event is ordered) */
static inline be<u8>& l_HD_badCutOnce() { return *gabi::at<be<u8>>(0x10467244); }
/* dCcD_GObjInf::OnTgShield / OffTgShield: mGObjTg.mSPrm bit 0 */
static inline void OnTgShield(dCcD_GObjInf* o) { o->mGObjTg.mSPrm |= 1u; }
static inline void OffTgShield(dCcD_GObjInf* o) { o->mGObjTg.mSPrm &= ~1u; }

enum { JA_SE_CV_JI_ATTACK = 0x4843 };

/* (f32)endFrame - d: the s16 end frame converted through the double magic constant */
static inline f32 endFrame(mDoExt_McaMorf* m) { return (f32)m->mFrameCtrl.mEnd.get(); }

/* 0224EF14 */
BOOL daNpc_Ji1_c::teachMove(f32 param_1) {
    WWHD_FUNC(0x0224EF14, BOOL, this, param_1);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    gabi::Local<cXyz> temp1;
    cXyz_mi(&home.pos, temp1, &current.pos);
    gabi::Local<cXyz> temp2;
    cXyz_mi(&player->current.pos, temp2, &current.pos);
    std_sqrtf(PSVECSquareMag(temp1));
    f32 temp3 = std_sqrtf(PSVECSquareMag(temp2));
    gabi::Local<cXyz> d3;
    cXyz_mi(&player->current.pos, d3, &home.pos);
    std_sqrtf(PSVECSquareMag(d3));

    cM_atan2s(temp1->x, temp1->z);
    s16 temp5 = cM_atan2s(temp2->x, temp2->z);
    BOOL temp13 = FALSE;
    BOOL temp20 = FALSE;
    gabi::Local<cXyz> a;
    cXyz_ml(&home.pos, a, 0.3f);
    gabi::Local<cXyz> b;
    cXyz_ml(&player->current.pos, b, 0.7f);
    gabi::Local<cXyz> temp8;
    cXyz_pl(a, temp8, b);
    temp8->y = 0.0f;

    f32 temp10 = 150.0f;
    if (field_0xD70 == 3 && !daPy_checkCutCharge(player)) {
        temp10 = 280.0f;
    }

    if (ptmf_eq(mAction, ACT_speakBadAction)) {
        temp3 = 10.0f;
    } else {
        if (temp3 > temp10) {
            temp3 = gabi::fmadds(5.0f, temp3 / temp10 - 1.0f, 0.5f);
        } else {
            f32 px = player->current.pos.x;
            temp8->x = px;
            f32 py = player->current.pos.y;
            temp8->y = py;
            f32 pz = player->current.pos.z;
            temp8->x = gabi::fnmsubs(temp10, cM_ssin(temp5), px);
            temp8->z = gabi::fnmsubs(temp10, cM_scos(temp5), pz);
            temp3 = 2.5f;
        }
    }

    gabi::Local<cXyz> d;
    cXyz_mi(temp8, d, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    if (std_sqrtf(PSVECSquareMag(xz)) > 25.0f) {
        cLib_addCalcPos2(&current.pos, temp8, 0.1f, temp3 * param_1);
        if (temp3 > 1.0f) {
            temp13 = TRUE;
        }
    }

    s16 dang = (s16)(temp5 - current.angle.y);
    if (abs(dang) > 0x1000) {
        temp20 = TRUE;
    }
    cLib_addCalcAngleS2(&current.angle.y, temp5, 8, 0x1000);

    if (temp20 == TRUE || temp13 == TRUE) {
        if (!isGuardAnim() || mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
            setAnm(6, 4.0f, 0);
        }
        if (mAnimation == 6) {
            mpOrcaMorf->setPlaySpeed(l_HIO().field_0x48 * param_1);
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0224EF14, &daNpc_Ji1_c::teachMove);

/* 0224F628 */
BOOL daNpc_Ji1_c::MoveToPlayer(f32 param_1, u8 param_2) {
    WWHD_FUNC(0x0224F628, BOOL, this, param_1, param_2);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    gabi::Local<cXyz> temp;
    cXyz_mi(&player->current.pos, temp, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = (f32)temp->x;
    xz->y = 0.0f;
    xz->z = (f32)temp->z;
    f32 temp2 = std_sqrtf(PSVECSquareMag(xz));
    s16 temp3 = cM_atan2s(temp->x, temp->z);
    BOOL temp13 = FALSE;
    BOOL temp5 = FALSE;

    if (calcCoCorrectValue() < -200.0f) {
        if (mAnimation != 0xE) {
            setAnm(0xE, 4.0f, 0);
            ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
        }
        if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
            field_0xC4C = 0.0f;
            field_0xC2C = 0x14;
            field_0x910.SetC(&current.pos);
            dComIfG_Ccsp_Set(&field_0x910);
            dVibration_c* vib = dComIfGp_getVibration();
            gabi::Local<cXyz> up;
            up->x = 0.0f;
            up->y = 1.0f;
            up->z = 0.0f;
            gabi::call<BOOL>(0x025CB374, vib, 5, -0x11, up.get()); /* StartShock */
        }
        return TRUE;
    }
    if (calcBgCorrectValue() < -200.0f) {
        if (mAnimation != 0xE) {
            setAnm(0xE, 4.0f, 0);
            ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
        }
        if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
            field_0xC50 = 0.0f;
            return FALSE;
        }
        return TRUE;
    }

    if (field_0xC2C > 0) {
        field_0xC2C = field_0xC2C - 1;
        return TRUE;
    }

    if (temp2 > param_1) {
        gabi::Local<cXyz> t;
        f32 q = temp2 / param_1;
        f32 px = player->current.pos.x;
        t->x = px;
        t->y = (f32)player->current.pos.y;
        f32 pz = player->current.pos.z;
        t->x = gabi::fnmsubs(param_1, cM_ssin(temp3), px);
        temp2 = gabi::fmadds(12.0f, q - 1.0f, 2.0f);
        t->z = gabi::fnmsubs(param_1, cM_scos(temp3), pz);
        gabi::Local<cXyz> d;
        cXyz_mi(t, d, &current.pos);
        gabi::Local<cXyz> dxz;
        dxz->x = (f32)d->x;
        dxz->y = 0.0f;
        dxz->z = (f32)d->z;
        if (std_sqrtf(PSVECSquareMag(dxz)) > 25.0f) {
            cLib_addCalcPos2(&current.pos, t, 0.1f, temp2);
            if (temp2 > 1.0f) {
                temp13 = TRUE;
            }
        }
    }

    s16 dang = (s16)(temp3 - current.angle.y);
    if (abs(dang) > 0x1000) {
        temp5 = TRUE;
    }
    cLib_addCalcAngleS2(&current.angle.y, temp3, 8, 0x1000);
    if (temp5 == TRUE || temp13 == TRUE) {
        if (mAnimation != 6 || mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
            setAnm(6, 4.0f, 0);
        }
        if (mAnimation == 6) {
            mpOrcaMorf->setPlaySpeed(l_HIO().field_0x48);
        }
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0224F628, &daNpc_Ji1_c::MoveToPlayer);

/* 02255654 */
BOOL daNpc_Ji1_c::teachSubActionAttack() {
    WWHD_FUNC(0x02255654, BOOL, this);
    s16 target = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    f32 frame = mpOrcaMorf->getFrame();
    /* setBtAttackData(0.0f, 12.0f, 300.0f, 2); setBtNowFrame(frame) */
    mBtStartFrame = 0.0f;
    mBtEndFrame = 12.0f;
    mBtMaxDis = 300.0f;
    mBtAttackType = 2;
    mBtNowFrame = frame;

    /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
    if (!(field_0xA40.mGObjAt.mRPrm & 1) && field_0xA40.ChkAtHit()) {
        field_0xA40.ClrAtHit();
    }
    if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 1.0f)) {
        setAnm(5, 4.0f, 0);
        return FALSE;
    }
    if (mpOrcaMorf->checkFrame(10.0f)) {
        mpOrcaMorf->setPlaySpeed(2.0f);
    }
    if (frame > 12.0f && frame < 28.0f) {
        gabi::Local<cXyz> temp;
        f32 y = field_0xD38.y;
        f32 x = field_0xD38.x;
        f32 z = field_0xD38.z;
        temp->x = gabi::fmadds(40.0f, cM_ssin(current.angle.y), x);
        temp->y = y;
        temp->z = gabi::fmadds(40.0f, cM_scos(current.angle.y), z);
        cLib_addCalcPos2(&current.pos, temp, 0.25f, 20.0f);
    } else if (frame < 12.0f) {
        cLib_addCalcAngleS2(&current.angle.y, target, 4, 0x200);
        field_0xD38.copy(current.pos);
    }
    return TRUE;
}
VERIFY(0x02255654, &daNpc_Ji1_c::teachSubActionAttack);

/* 022554D0 */
BOOL daNpc_Ji1_c::teachSubActionJump() {
    WWHD_FUNC(0x022554D0, BOOL, this);
    gabi::Local<cXyz> temp;
    cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, temp, &current.pos);
    s16 temp2 = cM_atan2s(temp->x, temp->z);
    cLib_addCalc2(&field_0xC9C, 120.0f, 0.25f, 50.0f);
    cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x1000);
    gabi::Local<cXyz> temp3;
    f32 c9c = field_0xC9C;
    f32 y = field_0xD38.y;
    f32 x = field_0xD38.x;
    f32 z = field_0xD38.z;
    temp3->y = y;
    temp3->x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
    temp3->z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
    f32 temp4 = 25.0f;
    if (mAcch.ChkWallHit()) {
        temp4 = 10.0f;
    }
    cLib_addCalcPosXZ(&current.pos, temp3, 0.8f, temp4, 1.0f);
    if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 10.0f)) {
        setParticle(3, 3.0f, 0.5f);
    }
    return TRUE;
}
VERIFY(0x022554D0, &daNpc_Ji1_c::teachSubActionJump);

enum {
    CUT_TYPE_CUT_ROLL = 9,
    JA_SE_CM_JI_SLIP = 0x5021,
    JA_SE_CV_JI_DEFENCE = 0x4833,
    dEvtCnd_UNK2_e = 2,
};

/* 02255908 HD: the first failed cut of a lesson is forgiven (l_HD_badCutOnce) */
BOOL daNpc_Ji1_c::teachAction(void*) {
    WWHD_FUNC(0x02255908, BOOL, this, nullptr);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (field_0xC78 == 0) {
        setAnm(5, 0.0f, 0);
        attn_flags(this) |= 1u; /* fopAc_Attn_LOCKON_MISC_e */
        field_0xC90 = 0;
        field_0xD34 = 0;
        if (field_0xD70 == 4) {
            field_0xC30 = 90;
        } else {
            field_0xC30 = 0;
        }
        field_0xC78 = (s8)(field_0xC78 + 1);
    } else if (field_0xC78 != -1) {
        if (!dComIfGs_isEventBit(0x2F10 /* UNK_2F10 */) && daNpc_Ji1_plRoomOutCheck()) {
            field_0xC84 = 9;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_teachAction);
            return true;
        }

        gabi::Local<cXyz> temp;
        cXyz_mi(&player->current.pos, temp, &current.pos);
        cM_atan2s(temp->x, temp->z);
        gabi::Local<cXyz> xz;
        xz->x = (f32)temp->x;
        xz->y = 0.0f;
        xz->z = (f32)temp->z;
        std_sqrtf(PSVECSquareMag(xz));

        if (checkCutType(daPy_getCutType(player), field_0xD70)) {
            OffTgShield(&field_0x7E0);
        } else {
            OnTgShield(&field_0x7E0);
        }

        if (field_0x7E0.ChkTgHit() && field_0xD6C != 1) {
            switch ((u32)field_0xC24) {
            case 0xF: /* CUT_TYPE_BT_ROLLCUT */
                teachSubActionJumpInit();
                break;
            case 1:
            case 6:
            case 8:
            case 9:
                setAnm(8, 0.0f, 1);
                break;
            case 0xA: /* CUT_TYPE_JUMPCUT_SWORD */
                setAnm(10, 0.0f, 1);
                field_0xC9C = 0.0f;
                field_0xD38.copy(current.pos);
                break;
            default:
                setAnm(9, 0.0f, 1);
                break;
            }

            if (checkCutType(field_0xC24, field_0xD70)) {
                setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE);
                field_0xC9C = 0.0f;
                field_0xD38.copy(current.pos);
                setParticle(0x10, 1.0f, 0.1f);

                u32 status;
                s32 d70 = field_0xD70;
                s32 c98 = field_0xC98;
                if (d70 < 3) {
                    status = l_HIO().field_0x54[d70] - c98 + 1;
                } else if (d70 < 5) {
                    if (c98 == 2) {
                        status = 1;
                    } else {
                        status = 9;
                    }
                } else if (c98 == 2) {
                    status = 1;
                } else {
                    status = 0xA;
                }
                if (c98 > 0) {
                    mDoAud_changeBgmStatus(status);
                }
            } else {
                setGuardParticle();
            }
            field_0xD6C = 1;
            field_0xC8C = field_0xC24;
        }

        if (field_0xD6C == 1) {
            if ((!daPy_getCutAtFlg(player) || mpOrcaMorf->getFrame() > endFrame(mpOrcaMorf) - 2.0f) && field_0xC90 == 0) {
                if (checkCutType(field_0xC8C, field_0xD70)) {
                    field_0xC98 = field_0xC98 - 1;
                    if (field_0xC98 > 0) {
                        if (field_0xC94 == 0 && field_0xC98 > 0 && field_0xD34 == 0) {
                            if (field_0xC8C != CUT_TYPE_CUT_ROLL) {
                                ji1_orderOtherEventId(this, mEventIdx[1]);
                                eventInfo_onCondition(this, dEvtCnd_UNK2_e);
                                field_0xC90 = 1;
                            } else {
                                field_0xD34 = 1;
                            }
                        } else {
                            field_0xC90 = 0;
                            field_0xD6C = 0;
                        }
                    } else if (field_0xC8C != CUT_TYPE_CUT_ROLL && field_0xD34 == 0) {
                        if (field_0xD70 >= 5 || (field_0xD70 >= 2 && !dComIfGs_isEventBit(0x0001))) {
                            if (dComIfGs_isEventBit(0x0001) && dComIfGs_isEventBit(0x2F10) == 0) {
                                field_0xC84 = 0xF;
                                ji1_orderOtherEventId(this, mEventIdx[0xF]);
                            } else {
                                ji1_orderOtherEventId(this, mEventIdx[2]);
                            }
                        } else {
                            ji1_orderOtherEventId(this, mEventIdx[1]);
                        }
                        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
                        field_0xC90 = 3;
                        l_HD_badCutOnce() = 0;
                    } else {
                        field_0xD34 = 3;
                    }
                } else if (field_0xC8C != CUT_TYPE_CUT_ROLL && field_0xD34 == 0) {
                    if (l_HD_badCutOnce() == 0) {
                        l_HD_badCutOnce() = 1;
                        field_0xD6C = 0;
                    } else {
                        ji1_orderOtherEventId(this, mEventIdx[3]);
                        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
                        field_0xC90 = 2;
                        l_HD_badCutOnce() = 0;
                    }
                } else {
                    field_0xD34 = 2;
                }
            }

            if (mAnimation == 0xA && checkCutType(field_0xC8C, field_0xD70)) {
                f32 target = l_HIO().field_0x34;
                f32 step = l_HIO().field_0x38;
                if (field_0xC98 == 0) {
                    target = target + target; /* l_HIO.field_0x34 * 2.0f */
                }
                cLib_addCalc2(&field_0xC9C, target, 0.25f, step);
                gabi::Local<cXyz> t;
                f32 x = field_0xD38.x;
                f32 y = field_0xD38.y;
                f32 c9c = field_0xC9C;
                t->y = y;
                t->x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
                f32 z = field_0xD38.z;
                t->z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
                f32 temp2 = 45.0f;
                if (mAcch.ChkWallHit()) {
                    temp2 = 10.0f;
                }
                cLib_addCalcPos2(&current.pos, t, 0.8f, temp2);
                ji1_seStart(this, JA_SE_CM_JI_SLIP, 0);
            } else if (mAnimation == 0xB && checkCutType(field_0xC8C, field_0xD70)) {
                teachSubActionJump();
            } else if (mAnimation != 5 && mAnimation != 6 && checkCutType(field_0xC8C, field_0xD70)) {
                cLib_addCalc2(&field_0xC9C, l_HIO().field_0x3C, 0.25f, l_HIO().field_0x40);
                gabi::Local<cXyz> t;
                f32 x = field_0xD38.x;
                f32 c9c = field_0xC9C;
                f32 y = field_0xD38.y;
                t->y = y;
                t->x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
                f32 z = field_0xD38.z;
                t->z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
                cLib_addCalcPos2(&current.pos, t, 0.8f, 45.0f);
            }

            if (mpOrcaMorf->getFrame() > endFrame(mpOrcaMorf) - 2.0f) {
                OffTgShield(&field_0x6B0);
                OnTgShield(&field_0x7E0);

                if (field_0xC8C != CUT_TYPE_CUT_ROLL) {
                    if (field_0xC90 == 1 || field_0xC90 == 3) {
                        field_0xD6C = 0;
                        field_0xC9C = 0.0f;
                        field_0xD38.copy(current.pos);
                        if ((field_0xD70 >= 5 || (field_0xD70 >= 2 && !dComIfGs_isEventBit(0x0001))) && field_0xC90 == 3) {
                            if (dComIfGs_isEventBit(0x0001) && dComIfGs_isEventBit(0x2F10) == 0) {
                                dComIfGs_onEventBit(0x2F10);
                                field_0xD7C = 1;
                                field_0xC84 = 0xF;
                                ji1_setAction(this, ACT_eventAction, nullptr);
                                ptmf_set(field_0x2C8, ACT_normalAction);
                            } else {
                                ji1_setAction(this, ACT_endspeakAction, nullptr);
                            }
                        } else {
                            ji1_setAction(this, ACT_speakAction, nullptr);
                        }
                    } else if (field_0xC90 == 2) {
                        field_0xD6C = 0;
                        field_0xC9C = 0.0f;
                        field_0xD38.copy(current.pos);
                        field_0xC4C = 0.0f;
                        field_0xC50 = 0.0f;
                        field_0xC2C = 0;
                        ji1_setAction(this, ACT_speakBadAction, nullptr);
                    } else {
                        field_0xC90 = 0;
                        field_0xD6C = 0;
                        field_0xC9C = 0.0f;
                        field_0xD38.copy(current.pos);
                    }
                } else {
                    field_0xC90 = 0;
                    field_0xD6C = 0;
                    field_0xC9C = 0.0f;
                    field_0xD38.copy(current.pos);
                }
            }
        } else if (field_0xD70 == 4) {
            if (mAnimation == 7) {
                teachSubActionAttack();
            } else {
                s32 c30 = field_0xC30;
                field_0xC30 = c30 - 1;
                if (c30 < 0) {
                    field_0xC30 = 90;
                    teachSubActionAttackInit();
                } else if (!teachMove(1.0f)) {
                    if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
                        setAnm(5, 4.0f, 0);
                    }
                }
            }
        } else if (!teachMove(1.0f)) {
            if (mpOrcaMorf->checkFrame(endFrame(mpOrcaMorf) - 2.0f)) {
                setAnm(5, 4.0f, 0);
            }
        }

        if (field_0xD6C != 1) {
            dtParticle();
        }
        if (field_0xD6C != 1 && isGuardAnim()) {
            if (mpOrcaMorf->getFrame() > endFrame(mpOrcaMorf) - 2.0f) {
                setAnm(5, 4.0f, 0);
            }
        }

        if (!daPy_getCutAtFlg(player) && field_0xD34) {
            field_0xD6C = 0;
            field_0xC9C = 0.0f;
            field_0xD38.copy(current.pos);
            if (field_0xD34 == 1) {
                field_0xC90 = 1;
                ji1_setAction(this, ACT_speakAction, nullptr);
            } else if (field_0xD34 == 3) {
                field_0xC90 = 3;
                ji1_setAction(this, ACT_speakAction, nullptr);
                l_HD_badCutOnce() = 0;
            } else if (field_0xD34 == 2) {
                field_0xC90 = 2;
                field_0xC2C = 0;
                field_0xC4C = 0.0f;
                field_0xC50 = 0.0f;
                ji1_setAction(this, ACT_speakBadAction, nullptr);
            }
        }
    }
    return true;
}
VERIFY(0x02255908, &daNpc_Ji1_c::teachAction);
