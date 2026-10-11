/**
 * d_a_npc_people_exec.cpp (WWHD)
 * NPC - Windfall townspeople: _execute, executeCommon and the Wait/Talk/Walk/Turn/Bikkuri/Furue
 * move procedures.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include <cmath>

#include "d/actor/d_a_npc_people_exec.h"
#include "gc_api/f_op/f_op_msg.h"

enum { dItemNo_COLLECT_MAP_15_e = 0xF0, dItemNo_PEARL_NAYRU_e = 0x69 };
enum { daPyStts0_TELESCOPE_LOOK_e = 0x00200000 };
enum { ENDLESS_NIGHT = 0x0A02 };

// GameCube animation data; scalar initializers are checked against the HD image.
// Named pointer tables and moveProc are separately audited against the image,
// since the glue checker cannot evaluate named array pointers or external methods.
static sPeopleAnmDat l_npc_anm_wait[] = {
    {
        0x00,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_walk[] = {
    {
        0x02,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_bikkuri[] = {
    {
        0x03,
        0x08,
        0x01,
    },
    {
        0xFF,
        0x00,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_furue[] = {
    {
        0x04,
        0x00,
        0x00,
    },
};
static s16 l_joint_dat_kyoro[] = {0, 0x2000, 0, 0, 0, -0x2000, 0, 0, 0x800};
static s32 l_pig_para[] = {0x400, 0x300, 0x200};
static sPeopleAnmDat l_npc_anm_ub1_wait1[] = {
    {
        0x00,
        0x08,
        0x04,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_ub1_wait2[] = {
    {
        0x03,
        0x08,
        0x04,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_ub1_yada[] = {
    {
        0x04,
        0x08,
        0x01,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_ub2_wait1[] = {
    {
        0x03,
        0x08,
        0x04,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_ub2_wait2[] = {
    {
        0x00,
        0x08,
        0x04,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_ub2_yada[] = {
    {
        0x04,
        0x08,
        0x01,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_um3_wait2[] = {
    {
        0x00,
        0x0F,
        0x01,
    },
    {
        0x00,
        0x08,
        0x03,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_um3_wait[] = {
    {
        0x03,
        0x0F,
        0x01,
    },
    {
        0x03,
        0x08,
        0x03,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_um3_tameiki[] = {
    {
        0x04,
        0x0F,
        0x01,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_sa3_wait3[] = {
    {
        0x00,
        0x08,
        0x04,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_sa3_laugh[] = {
    {
        0x03,
        0x08,
        0x03,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat l_npc_anm_sa5_drink[] = {
    {
        0x03,
        0x08,
        0x01,
    },
    {
        0xFF,
        0x08,
        0x00,
    },
};
static sPeopleAnmDat* l_npc_anm_ub1_tbl[] = {l_npc_anm_ub1_wait1, l_npc_anm_ub1_wait1, l_npc_anm_ub1_wait1, l_npc_anm_ub1_wait1, l_npc_anm_ub1_wait2, l_npc_anm_ub1_wait2, l_npc_anm_ub1_yada, l_npc_anm_ub1_yada};
static sPeopleAnmDat* l_npc_anm_ub2_tbl[] = {l_npc_anm_ub2_wait1, l_npc_anm_ub2_wait1, l_npc_anm_ub2_wait1, l_npc_anm_ub2_wait1, l_npc_anm_ub2_wait2, l_npc_anm_ub2_wait2, l_npc_anm_ub2_yada, l_npc_anm_ub2_yada};
static sPeopleAnmDat* l_npc_anm_um3_tbl[] = {l_npc_anm_um3_wait2, l_npc_anm_um3_wait2, l_npc_anm_um3_wait2, l_npc_anm_um3_wait2, l_npc_anm_um3_wait, l_npc_anm_um3_wait, l_npc_anm_um3_tameiki, l_npc_anm_um3_tameiki};
static sPeopleAnmDat* l_npc_anm_sa3_tbl[] = {l_npc_anm_sa3_wait3, l_npc_anm_sa3_wait3, l_npc_anm_sa3_wait3, l_npc_anm_sa3_laugh};
static sPeopleAnmDat* l_npc_anm_sa5_tbl[] = {l_npc_anm_sa3_wait3, l_npc_anm_sa3_wait3, l_npc_anm_sa3_wait3, l_npc_anm_sa5_drink};
static PeopleMoveProc moveProc[] = {{&daNpcPeople_c::executeWait}, {&daNpcPeople_c::executeTalk}, {&daNpcPeople_c::executeWalk}, {&daNpcPeople_c::executeTurn}, {&daNpcPeople_c::executeBikkuri}, {&daNpcPeople_c::executeFurue}, {&daNpcPeople_c::executeKyoro}, {&daNpcPeople_c::executeLetter}, {&daNpcPeople_c::executeLook}, {&daNpcPeople_c::executeLook2}, {&daNpcPeople_c::executeUgWalk}, {&daNpcPeople_c::executeUgTurn}, {&daNpcPeople_c::executeUgLook}, {&daNpcPeople_c::executeUgLook2}, {&daNpcPeople_c::executeUgSit}};

/* 022C305C */
bool daNpcPeople_c::_execute() {
    // HD: reset the photo object here.
    m7A6 = 0xFF;
    warp();

    switch (mNpcNo) {
    case NPC_UG1:
    case NPC_UG2:
        if (getPrmArg0() == 0) {
            gabi::Local<cXyz> diff;
            cXyz_mi(&current.pos, diff.get(), &home.pos);
            f32 mag = std_sqrtf(PSVECSquareMag(diff.get()));
            f32 f3 = 20.0f;
            mag -= 180.0f;
            if (mag < 0.0f) {
                mag = 0.0f;
            } else if (mag > f3) {
                mag = f3;
            }
            /* mStts.SetWeight(): the weight byte */
            mStts.SetWeight((u8)gabi::ftoi(50.0f * mag / f3 + 80.0f));
        }
    }

    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || (eventInfo.getCommand() == 1 && m79C != 0)) {
        (this->*moveProc[m78F].fn)();
    } else {
        eventMove();
    }

    eventOrder();
    if (eventInfo.getCommand() != 1) {
        mNoTalk = 0;
        if (!mbIsNight) {
            switch (mNpcNo) {
            case NPC_UB1:
                if (mAnmFlag & 0x3) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand = getRand(8);
                    if (rand == m794) {
                        rand = 0;
                    }
                    setAnmTbl(l_npc_anm_ub1_tbl[rand], 1);
                    m794 = rand;
                }
                break;
            case NPC_UB2:
                if (mAnmFlag & 0x3) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand = getRand(8);
                    if (rand == m794) {
                        rand = 0;
                    }
                    setAnmTbl(l_npc_anm_ub2_tbl[rand], 1);
                    m794 = rand;
                }
                break;
            case NPC_UM3:
                if (!dComIfGs_checkGetItem(dItemNo_COLLECT_MAP_15_e) && (mAnmFlag & 0x3)) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand = getRand(8);
                    if (rand == m794) {
                        rand = 0;
                    }
                    setAnmTbl(l_npc_anm_um3_tbl[rand], 1);
                    m794 = rand;
                }
                break;
            }
        } else {
            switch (mNpcNo) {
            case NPC_SA3:
                if (mAnmFlag & 0x3) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand;
                    if (m794 == 3) {
                        rand = 0;
                        m744 = 15.0f;
                    } else {
                        rand = getRand(4);
                    }
                    setAnmTbl(l_npc_anm_sa3_tbl[rand], 1);
                    m794 = rand;
                    if (m793 == 3) {
                        m79E = 0;
                    } else {
                        m79E = 1;
                    }
                }
                break;
            case NPC_SA5:
                if (mAnmFlag & 0x3) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand;
                    if (m794 == 3) {
                        rand = 0;
                        m744 = 15.0f;
                    } else {
                        rand = getRand(4);
                    }
                    setAnmTbl(l_npc_anm_sa5_tbl[rand], 1);
                    m794 = rand;
                    if (m793 == 3) {
                        m79E = 0;
                    } else {
                        m79E = 1;
                    }
                }
                break;
            }
        }
    }

    playTexPatternAnm();
    playAnm();
    if (m7A1 != 0) {
        if (m793 == 2) {
            cLib_chaseF(&speedF, m740, 0.1f);
            f32 speed = speedF * mpNpcDat->field_0x44;
            mDoExt_McaMorf* morf = mpMorf;
            if (speed < 0.5f) {
                speed = 0.5f;
            }
            morf->setPlaySpeed(speed);
        } else {
            speedF = 0.0f;
        }
        fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }

    {
        daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        setCollision(&mCyl, pos.get(), m74C, dat->field_0x40);
    }
    gabi::Local<cXyz> temp;
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    temp->set(dat->field_0x18, dat->field_0x1C, dat->field_0x20);
    mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), temp.get(), temp.get());
    PSVECAdd(temp.get(), &current.pos, temp.get()); /* temp += current.pos */
    attention_info.position.copy(*temp); /* attention_info.position = temp */
    eyePos.set(current.pos.x, current.pos.y + mpNpcDat->field_0x24, current.pos.z);
    lookBack();
    setMtx();
    return false;
}

/* 022C39F0 */
BOOL daNpcPeople_c::chkSurprise() {
    if (mNpcNo == NPC_UO2 && m78F != 1) {
        daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getPlayer(0));
        if (player->checkResetFlg0(daPy_py_c::daPyRFlg0_FRONT_ROLL_CRASH)) { /* checkFrontRollCrash() */
            gabi::Local<cXyz> delta;
            cXyz_mi(&current.pos, delta.get(), &player->current.pos);
            if (!(std_sqrtf(PSVECSquareMag(delta.get())) > 2000.0f)) { /* ble: NaN counts as near */
                return true;
            }
        }
        if (fopAcM_otoCheck(this, 2000.0f)) {
            return true;
        }
    }
    return false;
}

/* 022C3AC4 */
bool daNpcPeople_c::executeCommon() {
    if (m78B != 0 && mNoTalk == 0) {
        mOrderEventNum = 1;
        switch (mNpcNo) {
        case NPC_UG1:
        case NPC_UG2:
            if (eventInfo.getCommand() != 1) {
                gabi::Local<cXyz> diff;
                cXyz_mi(&current.pos, diff.get(), &home.pos);
                diff->y = 0.0f;
                f32 mag = std_sqrtf(PSVECSquareMag(diff.get()));
                if (mag < 200.0f && dComIfGs_isEventBit(0x2880) && dComIfGs_isEventBit(0x2B08) &&
                    !dComIfGs_isEventBit(0x2B04) && !dComIfGs_isTmpBit(0x0208)) {
                    m79C = 0;
                    actor_status = actor_status & ~fopAcStts_NOCULLEXEC_e; /* fopAcM_OffStatus(NOCULLEXEC) */
                    eventInfo.setEventId(m766[0]);
                } else {
                    eventInfo.setEventId(-1);
                    actor_status = actor_status | fopAcStts_NOCULLEXEC_e;
                    m79C = 1;
                }
            }
        }
    } else {
        mOrderEventNum = 0;
    }

    if (chkSurprise()) {
        executeSetMode(4);
        return true;
    }
    if ((mEtcFlag & 0x10) && !(mEtcFlag & 0x20)) {
        executeSetMode(9);
        return true;
    }
    if ((mEtcFlag & 0x4000) && m78F != 0xD) {
        executeSetMode(0xD);
        return true;
    }
    if (mEtcFlag & 0x20000) {
        mEtcFlag = mEtcFlag & ~0x20000;
        executeSetMode(0xB);
        return true;
    }
    if (mTalk == 1 && m79C != 0 && m78F != 1) {
        executeSetMode(1);
    }
    bool ret = false;
    if (mTalk != 0 && m79C != 0) {
        ret = true;
    }
    return ret;
}

/* 022C3D88 */
s32 daNpcPeople_c::executeWaitInit() {
    speedF = 0.0f;
    setWaitAnm();
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    f32 rnd = cM_rndF((f32)(dat->field_0x52 - dat->field_0x50));
    dat = mpNpcDat;
    m76E = (s16)gabi::ftoi(rnd + (f32)dat->field_0x50);
    m_jnt.setParam(dat->field_0x04, dat->field_0x06, dat->field_0x0C, dat->field_0x0E, dat->field_0x00, dat->field_0x02,
                   dat->field_0x08, dat->field_0x0A, dat->field_0x10);
    return 0;
}

/* 022C3E78 */
BOOL daNpcPeople_c::checkPig() {
    if (dComIfGs_isTmpBit(0x0280) && (m789 != 0 || m78A != 0)) {
        for (int i = 0; i < 3; i++) {
            PeoplePigView* pPig = static_cast<PeoplePigView*>(peopleSearchFromName("Pig", 0xF00, l_pig_para[i]));
            if (pPig != nullptr && pPig->m408 == 0 /* m408 */) {
                gabi::Local<cXyz> a, b;
                gabi::Local<be<f32>> dist;
                a->x = current.pos.x;
                a->z = current.pos.z;
                a->y = current.pos.y;
                b->x = pPig->current.pos.x;
                b->y = pPig->current.pos.y;
                b->z = pPig->current.pos.z;
                peopleCalcDisXZAngY(a.get(), b.get(), dist.get(), nullptr);
                if (*dist < 400.0f) {
                    f32 y = current.pos.y;
                    if (!(std::fabs(y - pPig->current.pos.y) > 10.0f)) {
                        /* pPig->taura_pos_set(current.pos) */
                        f32 x = current.pos.x, z = current.pos.z;
                        pPig->m408 = 1;
                        cXyz* tp = &pPig->m450;
                        tp->x = x;
                        tp->z = z;
                        tp->y = y;
                        mOrderEventNum = 2;
                        mEtcFlag = mEtcFlag | 0x80;
                        dComIfGs_setTmpReg(0xFC03, (u8)i);
                        u8 reg = dComIfGs_getTmpReg(0xFD07);
                        dComIfGs_setTmpReg(0xFD07, (u8)(reg | (1 << i)));
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/* 022C4074 */
void daNpcPeople_c::executeWait() {
    if (!executeCommon()) {
        if (mPathRun.mPath.get() != nullptr && m76E != 0 && m789 == 0 && m78A == 0) {
            m76E = m76E - 1;
            if (m76E == 0) {
                executeSetMode(3);
            }
        }

        switch (mNpcNo) {
        // HD: no m7A6 (photo object) setting for UB4/UW2 here.
        case NPC_UM3:
            if (mbIsNight && (!dComIfGs_isEventBit(ENDLESS_NIGHT) || dComIfGs_checkGetItem(dItemNo_PEARL_NAYRU_e))) {
                u32 status = dComIfGp_getPlayerStatus0();
                f32 f = mpNpcDat->field_0x28;
                if (status & daPyStts0_TELESCOPE_LOOK_e) {
                    m748 = (f32)(s16)gabi::ftoi(f + f);
                } else {
                    m748 = f;
                    mEtcFlag = mEtcFlag & ~0x80000700;
                }

                if (m789) {
                    if (!dComIfGs_isEventBit(0x2320)) {
                        mOrderEventNum = 2;
                    } else if (dComIfGp_getPlayerStatus0() & daPyStts0_TELESCOPE_LOOK_e) {
                        // HD: as in PAL, set the scope type only with no message.
                        if (peopleExecGame()->play.mMesgStatus == 0) {
                            peopleExecGame()->play.mScopeType = 1; /* dComIfGp_setScopeType(1) */
                        }
                        if (peopleExecGame()->play.mScopeType != 1) {
                            break;
                        }
                        if (dKy_moon_look_chk() && !(mEtcFlag & 0x100)) {
                            dComIfGs_onEventBit(0x2308);
                            mEtcFlag = mEtcFlag | 0x100;
                            m7A7 = 0;
                            mOrderEventNum = 0xB;
                        } else if (dKy_orion_look_chk() && !(mEtcFlag & 0x200)) {
                            mEtcFlag = mEtcFlag | 0x200;
                            m7A7 = 1;
                            mOrderEventNum = 0xB;
                        } else if (dKy_hokuto_look_chk() && !(mEtcFlag & 0x400)) {
                            mEtcFlag = mEtcFlag | 0x400;
                            m7A7 = 2;
                            mOrderEventNum = 0xB;
                        }
                    }
                }
            }
            break;
        case NPC_SA5:
            checkPig();
            break;
        case NPC_UG1:
        case NPC_UG2:
            if ((mEtcFlag & 0x1000) && m76E != 0) {
                m76E = m76E - 1;
                if (m76E == 0) {
                    executeSetMode(0xB);
                }
            }
            break;
        default:
            break;
        }
    }
}

/* 022C43A8 */
s32 daNpcPeople_c::executeTalkInit() {
    if (m79C) {
        if (m78F == 4) {
            setAnmTbl(l_npc_anm_wait, 1);
        }
        return true;
    }
    return m78F;
}

/* 022C4404 */
void daNpcPeople_c::executeTalk() {
    executeCommon();

    if (!dComIfGp_event_chkTalkXY() || dComIfGp_evmng_ChkPresentEnd()) {
        mEtcFlag = mEtcFlag & ~0x200000;
        if (talk2(1, this) == fopMsgStts_BOX_CLOSED_e) {
            mTalk = 0;
            mEtcFlag = mEtcFlag & ~0x1;
            executeSetMode(0);

            if (m77E & 0x1) {
                m77E = m77E & ~0x1;
                mOrderEventNum = 3;
            } else if (m77E & 0x2) {
                m77E = m77E & ~0x2;
                mOrderEventNum = 7;
            } else if (m77E & 0x4) {
                m77E = m77E & ~0x4;
                mOrderEventNum = 8;
            } else if (m77E & 0x8) {
                m77E = m77E & ~0x8;
                mOrderEventNum = 9;
            } else if (m77E & 0x10) {
                m77E = m77E & ~0x10;
                mOrderEventNum = 0xC;
            } else if (m77E & 0x20) {
                m77E = m77E & ~0x20;
                mOrderEventNum = 0xD;
            } else if (m77E & 0x40) {
                m77E = m77E & ~0x40;
                mOrderEventNum = 0xF;
            } else {
                m77E = m77E | 0x8000;
                if (mNpcNo == NPC_UB1 || mNpcNo == NPC_UB2) {
                    m79C = 0;
                }
            }

            if (mEtcFlag & 0x100000) {
                mEtcFlag = mEtcFlag & ~0x100000;
                peopleExecGame()->play.mStartItemTimer = 1; /* dComIfGp_startItemTimer() */
            }
        }
    }
}

/* 022C4600 */
s32 daNpcPeople_c::executeWalkInit() {
    mEtcFlag = mEtcFlag & ~0x00400008;
    setAnmTbl(l_npc_anm_walk, 1);
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    f32 rnd = cM_rndF((f32)(dat->field_0x56 - dat->field_0x54));
    m76E = (s16)gabi::ftoi(rnd + (f32)mpNpcDat->field_0x54);
    return 2;
}

/* 022C46D0 */
void daNpcPeople_c::executeWalk() {
    if (!executeCommon()) {
        bool temp = false;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (peoplePathChkPointPass(&mPathRun, pos.get(), mPathRun.mbDir != 0)) {
            if (peoplePathPointArg(&mPathRun, mPathRun.mIdx) == 0) {
                executeSetMode(8);
            }
            if (!peoplePathNextIdxAuto(&mPathRun)) {
                temp = true;
            }
        }

        if (m789 != 0 || m78A != 0) {
            executeSetMode(0);
        } else if (!temp) {
            if (m78F != 8) {
                gabi::Local<cXyz> point, a, b;
                gabi::Local<be<s16>> angle;
                peoplePathGetPoint(&mPathRun, point.get(), mPathRun.mIdx);
                b->x = point->x;
                b->y = point->y;
                b->z = point->z;
                a->y = current.pos.y;
                a->x = current.pos.x;
                a->z = current.pos.z;
                peopleCalcDisXZAngY(a.get(), b.get(), nullptr, angle.get());
                s16 ang = *angle;
                m77A = ang;
                m786 = ang;
                m764 = false;
                m782 = mpNpcDat->field_0x3A;
                m799 = 2;
                m_jnt.mbTrn = 1; /* setTrn() */
                m740 = mpNpcDat->field_0x48;
                if (m76E != 0) {
                    m76E = m76E - 1;
                    if (m76E == 0) {
                        executeSetMode(0);
                    }
                }
            }
        } else if (mNpcNo != NPC_UO3 || mPathRun.mbDir == 0) {
            mEtcFlag = mEtcFlag | 4;
            mPathRun.mbDir = mPathRun.mbDir ^ 1;
            executeSetMode(0);
        } else {
            mEtcFlag = mEtcFlag & ~0x4;
            executeSetMode(6);
        }
    }
}

/* 022C48B8 */
s32 daNpcPeople_c::executeTurnInit() {
    gabi::Local<cXyz> point, a, b;
    gabi::Local<be<s16>> angle;
    peoplePathGetPoint(&mPathRun, point.get(), mPathRun.mIdx);
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = point->x;
    b->y = point->y;
    b->z = point->z;
    peopleCalcDisXZAngY(a.get(), b.get(), nullptr, angle.get());
    if (*angle == current.angle.y && (mEtcFlag & 8) == 0) {
        setAnmTbl(l_npc_anm_walk, 1);
        daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
        f32 rnd = cM_rndF((f32)(dat->field_0x56 - dat->field_0x54));
        m76E = (s16)gabi::ftoi(rnd + (f32)mpNpcDat->field_0x54);
        return 2;
    }
    return 3;
}

/* 022C4A04 */
void daNpcPeople_c::executeTurn() {
    if (!executeCommon()) {
        s16 temp;
        if (mEtcFlag & 0x400008) {
            temp = m77A;
        } else {
            gabi::Local<cXyz> point, a, b;
            gabi::Local<be<s16>> angle;
            peoplePathGetPoint(&mPathRun, point.get(), mPathRun.mIdx);
            a->x = current.pos.x;
            a->y = current.pos.y;
            b->x = point->x;
            a->z = current.pos.z;
            b->z = point->z;
            b->y = point->y;
            peopleCalcDisXZAngY(a.get(), b.get(), nullptr, angle.get());
            temp = *angle;
        }
        m786 = temp;
        m764 = false;
        m799 = 2;
        m_jnt.mbTrn = 1; /* setTrn() */
        if (current.angle.y == temp) {
            if (mEtcFlag & 0x400000) {
                executeSetMode(2);
            } else if (mEtcFlag & 8) {
                executeSetMode(7);
            } else {
                executeSetMode(2);
            }
        }
    }
}

/* 022C4B30 */
s32 daNpcPeople_c::executeBikkuriInit() {
    m79A = m79A | 0x1;
    setAnmTbl(l_npc_anm_bikkuri, 1);
    m772 = 0x15;
    return 4;
}

/* 022C4B80 */
void daNpcPeople_c::executeBikkuri() {
    if (!executeCommon() && m772 != 0) {
        m799 = 0;
        m772 = m772 - 1;
        if (m772 == 0) {
            m744 = -1.0f;
            executeSetMode(5);
        }
    }
}

/* 022C4BF4 */
s32 daNpcPeople_c::executeFurueInit() {
    setAnmTbl(l_npc_anm_furue, 1);
    // HD: 150 frames; GameCube uses 0x78.
    m772 = 0x96;
    m774 = (s16)gabi::ftoi(cM_rndF(20.0f) + 10.0f);
    m786 = current.angle.y;
    be<s16>* k = l_joint_dat_kyoro;
    m_jnt.setParam(k[2], k[3], k[6], k[7], k[0], k[1], k[4], k[5], k[8]);
    return 5;
}

/* 022C4CA8 */
void daNpcPeople_c::executeFurue() {
    if (!executeCommon()) {
        if (m772) {
            if (m774 != 0) {
                m774 = m774 - 1;
            } else {
                f32 r = cM_rndF(20.0f);
                m79A = m79A ^ 2;
                m774 = (s16)gabi::ftoi(r + 10.0f);
            }
            if (m79A & 2) {
                m786 = current.angle.y + 0x4000;
            } else {
                m786 = current.angle.y - 0x4000;
            }
            m764 = true;
            m799 = 2;
            m772 = m772 - 1;
            if (m772 == 0) {
                m744 = 8.0f;
                executeSetMode(0);
                m79A = m79A & ~0x1;
            }
        }
        m7A6 = 0;
    }
}

/* WWHD glue
data l_npc_anm_wait 101C31B4
data l_npc_anm_walk 101C31BA
data l_npc_anm_bikkuri 101C3114
data l_npc_anm_furue 101C31BD
data l_joint_dat_kyoro 101C3E50
data l_pig_para 1002014C
data l_npc_anm_ub1_wait1 101C314C
data l_npc_anm_ub1_wait2 101C3154
data l_npc_anm_ub1_yada 101C315C
data l_npc_anm_ub2_wait1 101C3164
data l_npc_anm_ub2_wait2 101C316C
data l_npc_anm_ub2_yada 101C3174
data l_npc_anm_um3_wait2 101C3E64
data l_npc_anm_um3_wait 101C3E70
data l_npc_anm_um3_tameiki 101C3194
data l_npc_anm_sa3_wait3 101C319C
data l_npc_anm_sa3_laugh 101C31A4
data l_npc_anm_sa5_drink 101C31AC
data l_npc_anm_ub1_tbl 101C3460 nocheck
data l_npc_anm_ub2_tbl 101C3480 nocheck
data l_npc_anm_um3_tbl 101C34A0 nocheck
data l_npc_anm_sa3_tbl 101C34C0 nocheck
data l_npc_anm_sa5_tbl 101C34D0 nocheck
data moveProc 101C4BE4 nocheck
str 100207E0 "Pig"
ext 0259D624 void peopleCalcDisXZAngY(cXyz*, cXyz*, be<f32>*, be<s16>*)
ext 0259E778 void peoplePathGetPoint(dNpc_PathRun_l*, cXyz*, u8)
ext 0259E838 BOOL peoplePathChkPointPass(dNpc_PathRun_l*, cXyz*, u32)
ext 0259EE4C u8 peoplePathPointArg(dNpc_PathRun_l*, u8)
ext 0259ED58 BOOL peoplePathNextIdxAuto(dNpc_PathRun_l*)
ext 025D9F38 fopAc_ac_c* peopleSearchFromName(const char*, u32, u32)
ext 02560944 BOOL dKy_moon_look_chk()
ext 02560A60 BOOL dKy_orion_look_chk()
ext 02560B78 BOOL dKy_hokuto_look_chk()
*/
