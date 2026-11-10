/**
 * d_a_npc_ji1_teach.cpp (WWHD)
 * NPC - Orca: sword training (teachAction and its helpers)
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_teach.h"

// HD: one failed training cut is forgiven before the bad-cut event. Other Ji1
// sources also clear this TU-wide byte; access its existing storage without a duplicate definition.
static be<u8>& l_HD_badCutOnce() { return *gabi::at<be<u8>>(0x10467244); }

static void ji1_orderOtherEventId(fopAc_ac_c* actor, s16 event) {
    fopAcM_orderOtherEventId(actor, event, 0xFF, 0xFFFF, 0, 1);
}
static void OnTgShield(dCcD_GObjInf* collider) { collider->mGObjTg.mSPrm |= 1u; }
static void OffTgShield(dCcD_GObjInf* collider) { collider->mGObjTg.mSPrm &= ~1u; }

enum {
    JA_SE_CV_JI_ATTACK = 0x4843,
    JA_SE_CM_JI_SLIP = 0x5021,
    JA_SE_CV_JI_DEFENCE = 0x4833,
};

/* 0224EF14 */
BOOL daNpc_Ji1_c::teachMove(f32 param_1) {
    Ji1Player* player = static_cast<Ji1Player*>(daPy_getPlayerActorClass());
    cXyz temp1;
    temp1 = home.pos - current.pos;
    cXyz temp2;
    temp2 = player->current.pos - current.pos;
    std_sqrtf(PSVECSquareMag(&temp1));
    f32 temp3 = std_sqrtf(PSVECSquareMag(&temp2));
    cXyz d3;
    d3 = player->current.pos - home.pos;
    std_sqrtf(PSVECSquareMag(&d3));

    cM_atan2s(temp1.x, temp1.z);
    s16 temp5 = cM_atan2s(temp2.x, temp2.z);
    BOOL temp13 = FALSE;
    BOOL temp20 = FALSE;
    cXyz a;
    cXyz_ml(&home.pos, &a, 0.3f);
    cXyz b;
    cXyz_ml(&player->current.pos, &b, 0.7f);
    cXyz temp8;
    temp8 = a + b;
    temp8.y = 0.0f;

    f32 temp10 = 150.0f;
    if (mTeachStep == 3 && !Ji1_checkCutCharge(player)) {
        temp10 = 280.0f;
    }

    if (ptmf_eq(mAction, ACT_speakBadAction)) {
        temp3 = 10.0f;
    } else {
        if (temp3 > temp10) {
            temp3 = gabi::fmadds(5.0f, temp3 / temp10 - 1.0f, 0.5f);
        } else {
            f32 px = player->current.pos.x;
            temp8.x = px;
            f32 py = player->current.pos.y;
            temp8.y = py;
            f32 pz = player->current.pos.z;
            temp8.x = gabi::fnmsubs(temp10, cM_ssin(temp5), px);
            temp8.z = gabi::fnmsubs(temp10, cM_scos(temp5), pz);
            temp3 = 2.5f;
        }
    }

    cXyz d;
    d = temp8 - current.pos;
    cXyz xz;
    xz.x = (f32)d.x;
    xz.y = 0.0f;
    xz.z = (f32)d.z;
    if (std_sqrtf(PSVECSquareMag(&xz)) > 25.0f) {
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
        if (!isGuardAnim() || mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
            setAnm(6, 4.0f, 0);
        }
        if (mAnimation == 6) {
            mpOrcaMorf->setPlaySpeed(l_HIO().field_0x48 * param_1);
        }
        return TRUE;
    }
    return FALSE;
}

/* 0224F628 */
BOOL daNpc_Ji1_c::MoveToPlayer(f32 param_1, u8 param_2) {
    Ji1Player* player = static_cast<Ji1Player*>(daPy_getPlayerActorClass());
    cXyz temp;
    temp = player->current.pos - current.pos;
    cXyz xz;
    xz.x = (f32)temp.x;
    xz.y = 0.0f;
    xz.z = (f32)temp.z;
    f32 temp2 = std_sqrtf(PSVECSquareMag(&xz));
    s16 temp3 = cM_atan2s(temp.x, temp.z);
    BOOL temp13 = FALSE;
    BOOL temp5 = FALSE;

    if (calcCoCorrectValue() < -200.0f) {
        if (mAnimation != 0xE) {
            setAnm(0xE, 4.0f, 0);
            ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
        }
        if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
            mCoCorrectSum = 0.0f;
            mAttackEndWait = 0x14;
            mAtCyl.SetC(&current.pos);
            dComIfG_Ccsp_Set(&mAtCyl);
            dVibration_c* vib = dComIfGp_getVibration();
            cXyz up;
            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;
            vib->StartShock(5, -0x11, &up);
        }
        return TRUE;
    }
    if (calcBgCorrectValue() < -200.0f) {
        if (mAnimation != 0xE) {
            setAnm(0xE, 4.0f, 0);
            ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
        }
        if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
            mBgCorrectSum = 0.0f;
            return FALSE;
        }
        return TRUE;
    }

    if (mAttackEndWait > 0) {
        mAttackEndWait = mAttackEndWait - 1;
        return TRUE;
    }

    if (temp2 > param_1) {
        cXyz t;
        f32 q = temp2 / param_1;
        f32 px = player->current.pos.x;
        t.x = px;
        t.y = (f32)player->current.pos.y;
        f32 pz = player->current.pos.z;
        t.x = gabi::fnmsubs(param_1, cM_ssin(temp3), px);
        temp2 = gabi::fmadds(12.0f, q - 1.0f, 2.0f);
        t.z = gabi::fnmsubs(param_1, cM_scos(temp3), pz);
        cXyz d;
        d = t - current.pos;
        cXyz dxz;
        dxz.x = (f32)d.x;
        dxz.y = 0.0f;
        dxz.z = (f32)d.z;
        if (std_sqrtf(PSVECSquareMag(&dxz)) > 25.0f) {
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
        if (mAnimation != 6 || mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
            setAnm(6, 4.0f, 0);
        }
        if (mAnimation == 6) {
            mpOrcaMorf->setPlaySpeed(l_HIO().field_0x48);
        }
        return TRUE;
    }
    return FALSE;
}

/* 02255654 */
BOOL daNpc_Ji1_c::teachSubActionAttack() {
    s16 target = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    f32 frame = mpOrcaMorf->getFrame();
    /* setBtAttackData(0.0f, 12.0f, 300.0f, 2); setBtNowFrame(frame) */
    mBtStartFrame = 0.0f;
    mBtEndFrame = 12.0f;
    mBtMaxDis = 300.0f;
    mBtAttackType = 2;
    mBtNowFrame = frame;

    /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
    if (!(mAtCps.mGObjAt.mRPrm & 1) && mAtCps.ChkAtHit()) {
        mAtCps.ClrAtHit();
    }
    if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 1.0f)) {
        setAnm(5, 4.0f, 0);
        return FALSE;
    }
    if (mpOrcaMorf->checkFrame(10.0f)) {
        mpOrcaMorf->setPlaySpeed(2.0f);
    }
    if (frame > 12.0f && frame < 28.0f) {
        cXyz temp;
        f32 y = mBackSlideStartPos.y;
        f32 x = mBackSlideStartPos.x;
        f32 z = mBackSlideStartPos.z;
        temp.x = gabi::fmadds(40.0f, cM_ssin(current.angle.y), x);
        temp.y = y;
        temp.z = gabi::fmadds(40.0f, cM_scos(current.angle.y), z);
        cLib_addCalcPos2(&current.pos, temp, 0.25f, 20.0f);
    } else if (frame < 12.0f) {
        cLib_addCalcAngleS2(&current.angle.y, target, 4, 0x200);
        mBackSlideStartPos.copy(current.pos);
    }
    return TRUE;
}

/* 022554D0 */
BOOL daNpc_Ji1_c::teachSubActionJump() {
    cXyz temp;
    temp = dComIfGp_getPlayer(0)->current.pos - current.pos;
    s16 temp2 = cM_atan2s(temp.x, temp.z);
    cLib_addCalc2(&mBackSlideDist, 120.0f, 0.25f, 50.0f);
    cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x1000);
    cXyz temp3;
    f32 c9c = mBackSlideDist;
    f32 y = mBackSlideStartPos.y;
    f32 x = mBackSlideStartPos.x;
    f32 z = mBackSlideStartPos.z;
    temp3.y = y;
    temp3.x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
    temp3.z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
    f32 temp4 = 25.0f;
    if (mAcch.ChkWallHit()) {
        temp4 = 10.0f;
    }
    cLib_addCalcPosXZ(&current.pos, temp3, 0.8f, temp4, 1.0f);
    if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 10.0f)) {
        setParticle(3, 3.0f, 0.5f);
    }
    return TRUE;
}


/* 02255908 */
// HD: forgive the first failed cut of a lesson (l_HD_badCutOnce).
BOOL daNpc_Ji1_c::teachAction(void*) {
    Ji1Player* player = static_cast<Ji1Player*>(dComIfGp_getPlayer(0));

    if (mActionPhase == 0) {
        setAnm(5, 0.0f, 0);
        attention_info.flags |= fopAc_Attn_LOCKON_MISC_e;
        field_0xC90 = 0;
        field_0xD34 = 0;
        if (mTeachStep == 4) {
            mBattleTimer = 90;
        } else {
            mBattleTimer = 0;
        }
        mActionPhase = (s8)(mActionPhase + 1);
    } else if (mActionPhase != -1) {
        if (!dComIfGs_isEventBit(0x2F10 /* UNK_2F10 */) && daNpc_Ji1_plRoomOutCheck()) {
            mEventNo = 9;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(mEventReturnAction, ACT_teachAction);
            return true;
        }

        cXyz temp;
        temp = player->current.pos - current.pos;
        cM_atan2s(temp.x, temp.z);
        cXyz xz;
        xz.x = (f32)temp.x;
        xz.y = 0.0f;
        xz.z = (f32)temp.z;
        std_sqrtf(PSVECSquareMag(&xz));

        if (checkCutType(player->getCutType(), mTeachStep)) {
            OffTgShield(&mGuardCyl);
        } else {
            OnTgShield(&mGuardCyl);
        }

        if (mGuardCyl.ChkTgHit() && mSubActionPhase != 1) {
            switch ((u32)mHitCutType) {
            case CUT_TYPE_BT_ROLLCUT: /* CUT_TYPE_BT_ROLLCUT */
                teachSubActionJumpInit();
                break;
            case CUT_TYPE_CUT_A:
            case CUT_TYPE_CUT_EA:
            case CUT_TYPE_CUT_TURN:
            case CUT_TYPE_CUT_ROLL:
                setAnm(8, 0.0f, 1);
                break;
            case CUT_TYPE_JUMPCUT_SWORD: /* CUT_TYPE_JUMPCUT_SWORD */
                setAnm(10, 0.0f, 1);
                mBackSlideDist = 0.0f;
                mBackSlideStartPos.copy(current.pos);
                break;
            default:
                setAnm(9, 0.0f, 1);
                break;
            }

            if (checkCutType(mHitCutType, mTeachStep)) {
                setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE);
                mBackSlideDist = 0.0f;
                mBackSlideStartPos.copy(current.pos);
                setParticle(0x10, 1.0f, 0.1f);

                u32 status;
                s32 d70 = mTeachStep;
                s32 c98 = mTeachHitsLeft;
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
            mSubActionPhase = 1;
            mTeachCutType = mHitCutType;
        }

        if (mSubActionPhase == 1) {
            if ((!player->getCutAtFlg() || mpOrcaMorf->getFrame() > mpOrcaMorf->getEndFrame() - 2.0f) && field_0xC90 == 0) {
                if (checkCutType(mTeachCutType, mTeachStep)) {
                    mTeachHitsLeft = mTeachHitsLeft - 1;
                    if (mTeachHitsLeft > 0) {
                        if (field_0xC94 == 0 && mTeachHitsLeft > 0 && field_0xD34 == 0) {
                            if (mTeachCutType != CUT_TYPE_CUT_ROLL) {
                                ji1_orderOtherEventId(this, mEventIdx[1]);
                                eventInfo.onCondition(dEvtCnd_UNK2_e);
                                field_0xC90 = 1;
                            } else {
                                field_0xD34 = 1;
                            }
                        } else {
                            field_0xC90 = 0;
                            mSubActionPhase = 0;
                        }
                    } else if (mTeachCutType != CUT_TYPE_CUT_ROLL && field_0xD34 == 0) {
                        if (mTeachStep >= 5 || (mTeachStep >= 2 && !dComIfGs_isEventBit(0x0001))) {
                            if (dComIfGs_isEventBit(0x0001) && dComIfGs_isEventBit(0x2F10) == 0) {
                                mEventNo = 0xF;
                                ji1_orderOtherEventId(this, mEventIdx[0xF]);
                            } else {
                                ji1_orderOtherEventId(this, mEventIdx[2]);
                            }
                        } else {
                            ji1_orderOtherEventId(this, mEventIdx[1]);
                        }
                        eventInfo.onCondition(dEvtCnd_UNK2_e);
                        field_0xC90 = 3;
                        l_HD_badCutOnce() = 0;
                    } else {
                        field_0xD34 = 3;
                    }
                } else if (mTeachCutType != CUT_TYPE_CUT_ROLL && field_0xD34 == 0) {
                    if (l_HD_badCutOnce() == 0) {
                        l_HD_badCutOnce() = 1;
                        mSubActionPhase = 0;
                    } else {
                        ji1_orderOtherEventId(this, mEventIdx[3]);
                        eventInfo.onCondition(dEvtCnd_UNK2_e);
                        field_0xC90 = 2;
                        l_HD_badCutOnce() = 0;
                    }
                } else {
                    field_0xD34 = 2;
                }
            }

            if (mAnimation == 0xA && checkCutType(mTeachCutType, mTeachStep)) {
                f32 target = l_HIO().field_0x34;
                f32 step = l_HIO().field_0x38;
                if (mTeachHitsLeft == 0) {
                    target = target + target; /* l_HIO.field_0x34 * 2.0f */
                }
                cLib_addCalc2(&mBackSlideDist, target, 0.25f, step);
                cXyz t;
                f32 x = mBackSlideStartPos.x;
                f32 y = mBackSlideStartPos.y;
                f32 c9c = mBackSlideDist;
                t.y = y;
                t.x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
                f32 z = mBackSlideStartPos.z;
                t.z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
                f32 temp2 = 45.0f;
                if (mAcch.ChkWallHit()) {
                    temp2 = 10.0f;
                }
                cLib_addCalcPos2(&current.pos, t, 0.8f, temp2);
                ji1_seStart(this, JA_SE_CM_JI_SLIP, 0);
            } else if (mAnimation == 0xB && checkCutType(mTeachCutType, mTeachStep)) {
                teachSubActionJump();
            } else if (mAnimation != 5 && mAnimation != 6 && checkCutType(mTeachCutType, mTeachStep)) {
                cLib_addCalc2(&mBackSlideDist, l_HIO().field_0x3C, 0.25f, l_HIO().field_0x40);
                cXyz t;
                f32 x = mBackSlideStartPos.x;
                f32 c9c = mBackSlideDist;
                f32 y = mBackSlideStartPos.y;
                t.y = y;
                t.x = gabi::fnmsubs(c9c, cM_ssin(current.angle.y), x);
                f32 z = mBackSlideStartPos.z;
                t.z = gabi::fnmsubs(c9c, cM_scos(current.angle.y), z);
                cLib_addCalcPos2(&current.pos, t, 0.8f, 45.0f);
            }

            if (mpOrcaMorf->getFrame() > mpOrcaMorf->getEndFrame() - 2.0f) {
                OffTgShield(&mCyl);
                OnTgShield(&mGuardCyl);

                if (mTeachCutType != CUT_TYPE_CUT_ROLL) {
                    if (field_0xC90 == 1 || field_0xC90 == 3) {
                        mSubActionPhase = 0;
                        mBackSlideDist = 0.0f;
                        mBackSlideStartPos.copy(current.pos);
                        if ((mTeachStep >= 5 || (mTeachStep >= 2 && !dComIfGs_isEventBit(0x0001))) && field_0xC90 == 3) {
                            if (dComIfGs_isEventBit(0x0001) && dComIfGs_isEventBit(0x2F10) == 0) {
                                dComIfGs_onEventBit(0x2F10);
                                field_0xD7C = 1;
                                mEventNo = 0xF;
                                ji1_setAction(this, ACT_eventAction, nullptr);
                                ptmf_set(mEventReturnAction, ACT_normalAction);
                            } else {
                                ji1_setAction(this, ACT_endspeakAction, nullptr);
                            }
                        } else {
                            ji1_setAction(this, ACT_speakAction, nullptr);
                        }
                    } else if (field_0xC90 == 2) {
                        mSubActionPhase = 0;
                        mBackSlideDist = 0.0f;
                        mBackSlideStartPos.copy(current.pos);
                        mCoCorrectSum = 0.0f;
                        mBgCorrectSum = 0.0f;
                        mAttackEndWait = 0;
                        ji1_setAction(this, ACT_speakBadAction, nullptr);
                    } else {
                        field_0xC90 = 0;
                        mSubActionPhase = 0;
                        mBackSlideDist = 0.0f;
                        mBackSlideStartPos.copy(current.pos);
                    }
                } else {
                    field_0xC90 = 0;
                    mSubActionPhase = 0;
                    mBackSlideDist = 0.0f;
                    mBackSlideStartPos.copy(current.pos);
                }
            }
        } else if (mTeachStep == 4) {
            if (mAnimation == 7) {
                teachSubActionAttack();
            } else {
                s32 c30 = mBattleTimer;
                mBattleTimer = c30 - 1;
                if (c30 < 0) {
                    mBattleTimer = 90;
                    teachSubActionAttackInit();
                } else if (!teachMove(1.0f)) {
                    if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
                        setAnm(5, 4.0f, 0);
                    }
                }
            }
        } else if (!teachMove(1.0f)) {
            if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 2.0f)) {
                setAnm(5, 4.0f, 0);
            }
        }

        if (mSubActionPhase != 1) {
            dtParticle();
        }
        if (mSubActionPhase != 1 && isGuardAnim()) {
            if (mpOrcaMorf->getFrame() > mpOrcaMorf->getEndFrame() - 2.0f) {
                setAnm(5, 4.0f, 0);
            }
        }

        if (!player->getCutAtFlg() && field_0xD34) {
            mSubActionPhase = 0;
            mBackSlideDist = 0.0f;
            mBackSlideStartPos.copy(current.pos);
            if (field_0xD34 == 1) {
                field_0xC90 = 1;
                ji1_setAction(this, ACT_speakAction, nullptr);
            } else if (field_0xD34 == 3) {
                field_0xC90 = 3;
                ji1_setAction(this, ACT_speakAction, nullptr);
                l_HD_badCutOnce() = 0;
            } else if (field_0xD34 == 2) {
                field_0xC90 = 2;
                mAttackEndWait = 0;
                mCoCorrectSum = 0.0f;
                mBgCorrectSum = 0.0f;
                ji1_setAction(this, ACT_speakBadAction, nullptr);
            }
        }
    }
    return true;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025E1950 void mDoAud_changeBgmStatus(u32)
*/
