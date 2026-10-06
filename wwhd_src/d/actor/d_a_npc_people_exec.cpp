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

#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz, f32* dist, s16* angle): the cXyz by value are
 * pointers to copies */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* angle) {
    gabi::call(0x0259D624, a, b, dist, angle);
}
/* dNpc_PathRun_c (8 bytes) */
static inline void dNpc_PathRun_getPoint_x(dNpc_PathRun_l* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
/* 0259E838 chkPointPass(cXyz (pointer to a copy), bool) */
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_l* r, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, r, pos, dir); }
static inline u8 dNpc_PathRun_pointArg(dNpc_PathRun_l* r, u8 idx) { return gabi::call<u8>(0x0259EE4C, r, idx); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* r) { return gabi::call<BOOL>(0x0259ED58, r); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem_x(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* 025D9F38 fopAcM_searchFromName(name, paramMask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
static inline BOOL dKy_moon_look_chk() { return gabi::call<BOOL>(0x02560944); }
static inline BOOL dKy_orion_look_chk() { return gabi::call<BOOL>(0x02560A60); }
static inline BOOL dKy_hokuto_look_chk() { return gabi::call<BOOL>(0x02560B78); }
static inline u32 dComIfGp_getPlayerStatus0() { return gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0); }
/* eventInfo (fopAc_ac_c + 0xF8): mCommand u16 at 0xF8, mEventId s16 at 0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) { gabi::store<s16>(gabi::ea(a) + 0xFC, id); }
/* GHS pointer-to-member call without arguments: (self->*pmf)() for the PTMF at `entry` */
static inline void people_pmf_call0(void* self, u32 entry) {
    s16 i = gabi::load<s16>(entry + 2);
    u32 thisp = gabi::ea(self) + gabi::load<s16>(entry);
    if (i < 0) {
        gabi::call_ptr(gabi::load<u32>(entry + 4), thisp);
    } else {
        u32 vt = gabi::load<u32>(thisp + gabi::load<s16>(entry + 6));
        gabi::call_ptr(gabi::load<u32>(vt + i * 8 + 4), thisp);
    }
}

/* .data */
enum : u32 {
    l_npc_anm_wait = 0x101C31B4,
    l_npc_anm_walk = 0x101C31BA,
    l_npc_anm_bikkuri = 0x101C3114,
    l_npc_anm_furue = 0x101C31BD,
    l_npc_anm_ub1_tbl = 0x101C3460,
    l_npc_anm_ub2_tbl = 0x101C3480,
    l_npc_anm_um3_tbl = 0x101C34A0,
    l_npc_anm_sa3_tbl = 0x101C34C0,
    l_npc_anm_sa5_tbl = 0x101C34D0,
    l_joint_dat_kyoro = 0x101C3E50,
    moveProc = 0x101C4BE4, /* PTMF[15] */
    l_pig_para = 0x1002014C,
};
static inline sPeopleAnmDat* anm_dat(u32 a) { return gabi::at<sPeopleAnmDat>(a); }
static inline sPeopleAnmDat* anm_tbl(u32 tbl, u32 i) { return gabi::at<sPeopleAnmDat>(gabi::load<u32>(tbl + i * 4)); }

/* 022C305C */
bool daNpcPeople_c::_execute() {
    WWHD_FUNC(0x022C305C, bool, this);
    m7A6 = 0xFF; /* HD: reset here */
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
            gabi::store<u8>(gabi::ea(&mStts) + 0x14, (u8)gabi::ftoi(50.0f * mag / f3 + 80.0f));
        }
    }

    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || (eventInfo_getCommand(this) == 1 && m79C != 0)) {
        people_pmf_call0(this, moveProc + m78F * 8); /* (this->*moveProc[m78F])() */
    } else {
        eventMove();
    }

    eventOrder();
    if (eventInfo_getCommand(this) != 1) {
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
                    setAnmTbl(anm_tbl(l_npc_anm_ub1_tbl, rand), 1);
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
                    setAnmTbl(anm_tbl(l_npc_anm_ub2_tbl, rand), 1);
                    m794 = rand;
                }
                break;
            case NPC_UM3:
                if (!dComIfGs_checkGetItem_x(0xF0 /* dItemNo_COLLECT_MAP_15_e */) && (mAnmFlag & 0x3)) {
                    mAnmFlag = mAnmFlag & ~0x3;
                    u8 rand = getRand(8);
                    if (rand == m794) {
                        rand = 0;
                    }
                    setAnmTbl(anm_tbl(l_npc_anm_um3_tbl, rand), 1);
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
                    setAnmTbl(anm_tbl(l_npc_anm_sa3_tbl, rand), 1);
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
                    setAnmTbl(anm_tbl(l_npc_anm_sa5_tbl, rand), 1);
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
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        dBgS_Acch_CrrPos(&mObjAcch, dComIfG_Bgsp());
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
    gabi::at<cXyz>(gabi::ea(this) + 0x388 + 8)->copy(*temp); /* attention_info.position = temp */
    eyePos.set(current.pos.x, current.pos.y + mpNpcDat->field_0x24, current.pos.z);
    lookBack();
    setMtx();
    return false;
}
VERIFY(0x022C305C, &daNpcPeople_c::_execute);

/* 022C39F0 */
BOOL daNpcPeople_c::chkSurprise() {
    WWHD_FUNC(0x022C39F0, BOOL, this);
    if (mNpcNo == NPC_UO2 && m78F != 1) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x2000) { /* checkFrontRollCrash() */
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
VERIFY(0x022C39F0, &daNpcPeople_c::chkSurprise);

/* 022C3AC4 */
bool daNpcPeople_c::executeCommon() {
    WWHD_FUNC(0x022C3AC4, bool, this);
    if (m78B != 0 && mNoTalk == 0) {
        mOrderEventNum = 1;
        switch (mNpcNo) {
        case NPC_UG1:
        case NPC_UG2:
            if (eventInfo_getCommand(this) != 1) {
                gabi::Local<cXyz> diff;
                cXyz_mi(&current.pos, diff.get(), &home.pos);
                diff->y = 0.0f;
                f32 mag = std_sqrtf(PSVECSquareMag(diff.get()));
                if (mag < 200.0f && dComIfGs_isEventBit(0x2880) && dComIfGs_isEventBit(0x2B08) &&
                    !dComIfGs_isEventBit(0x2B04) && !dComIfGs_isTmpBit(0x0208)) {
                    m79C = 0;
                    actor_status = actor_status & ~0x80; /* fopAcM_OffStatus(NOCULLEXEC) */
                    eventInfo_setEventId(this, m766[0]);
                } else {
                    eventInfo_setEventId(this, -1);
                    actor_status = actor_status | 0x80;
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
VERIFY(0x022C3AC4, &daNpcPeople_c::executeCommon);

/* 022C3D88 */
s32 daNpcPeople_c::executeWaitInit() {
    WWHD_FUNC(0x022C3D88, s32, this);
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
VERIFY(0x022C3D88, &daNpcPeople_c::executeWaitInit);

/* 022C3E78 */
BOOL daNpcPeople_c::checkPig() {
    WWHD_FUNC(0x022C3E78, BOOL, this);
    if (dComIfGs_isTmpBit(0x0280) && (m789 != 0 || m78A != 0)) {
        for (int i = 0; i < 3; i++) {
            fopAc_ac_c* pPig = fopAcM_searchFromName(STR(0x100207E0) /* "Pig" */, 0xF00, gabi::load<u32>(l_pig_para + i * 4));
            if (pPig != nullptr && gabi::load<u8>(gabi::ea(pPig) + 0x750) == 0 /* m408 */) {
                gabi::Local<cXyz> a, b;
                gabi::Local<be<f32>> dist;
                a->x = current.pos.x;
                a->z = current.pos.z;
                a->y = current.pos.y;
                b->x = pPig->current.pos.x;
                b->y = pPig->current.pos.y;
                b->z = pPig->current.pos.z;
                dNpc_calc_DisXZ_AngY(a.get(), b.get(), dist.get(), nullptr);
                if (*dist < 400.0f) {
                    f32 y = current.pos.y;
                    if (!(std::fabs(y - pPig->current.pos.y) > 10.0f)) {
                        /* pPig->taura_pos_set(current.pos) */
                        f32 x = current.pos.x, z = current.pos.z;
                        gabi::store<u8>(gabi::ea(pPig) + 0x750, 1);
                        cXyz* tp = gabi::at<cXyz>(gabi::ea(pPig) + 0x798);
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
VERIFY(0x022C3E78, &daNpcPeople_c::checkPig);

/* 022C4074 */
void daNpcPeople_c::executeWait() {
    WWHD_FUNC(0x022C4074, void, this);
    if (!executeCommon()) {
        if (mPathRun.mPath.get() != nullptr && m76E != 0 && m789 == 0 && m78A == 0) {
            m76E = m76E - 1;
            if (m76E == 0) {
                executeSetMode(3);
            }
        }

        switch (mNpcNo) {
        /* HD: no m7A6 (photo object) setting for UB4/UW2 here */
        case NPC_UM3:
            if (mbIsNight && (!dComIfGs_isEventBit(0x0A02 /* ENDLESS_NIGHT */) || dComIfGs_checkGetItem_x(0x69 /* PEARL_NAYRU */))) {
                u32 status = dComIfGp_getPlayerStatus0();
                f32 f = mpNpcDat->field_0x28;
                if (status & 0x00200000 /* daPyStts0_TELESCOPE_LOOK_e */) {
                    m748 = (f32)(s16)gabi::ftoi(f + f);
                } else {
                    m748 = f;
                    mEtcFlag = mEtcFlag & ~0x80000700;
                }

                if (m789) {
                    if (!dComIfGs_isEventBit(0x2320)) {
                        mOrderEventNum = 2;
                    } else if (dComIfGp_getPlayerStatus0() & 0x00200000) {
                        /* HD (as the PAL version): the scope type is set only with no message */
                        if (gabi::load<u8>(dComIfGp_ea() + 0x5BB2) == 0) {
                            gabi::store<u8>(dComIfGp_ea() + 0x5BCF, 1); /* dComIfGp_setScopeType(1) */
                        }
                        if (gabi::load<u8>(dComIfGp_ea() + 0x5BCF) != 1) {
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
VERIFY(0x022C4074, &daNpcPeople_c::executeWait);

/* 022C43A8 */
s32 daNpcPeople_c::executeTalkInit() {
    WWHD_FUNC(0x022C43A8, s32, this);
    if (m79C) {
        if (m78F == 4) {
            setAnmTbl(anm_dat(l_npc_anm_wait), 1);
        }
        return true;
    }
    return m78F;
}
VERIFY(0x022C43A8, &daNpcPeople_c::executeTalkInit);

/* 022C4404 */
void daNpcPeople_c::executeTalk() {
    WWHD_FUNC(0x022C4404, void, this);
    executeCommon();

    if (!dComIfGp_event_chkTalkXY() || dComIfGp_evmng_ChkPresentEnd()) {
        mEtcFlag = mEtcFlag & ~0x200000;
        if (talk2(1, this) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
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
                gabi::store<u8>(dComIfGp_ea() + 0x5BDF, 1); /* dComIfGp_startItemTimer() */
            }
        }
    }
}
VERIFY(0x022C4404, &daNpcPeople_c::executeTalk);

/* 022C4600 */
s32 daNpcPeople_c::executeWalkInit() {
    WWHD_FUNC(0x022C4600, s32, this);
    mEtcFlag = mEtcFlag & ~0x00400008;
    setAnmTbl(anm_dat(l_npc_anm_walk), 1);
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    f32 rnd = cM_rndF((f32)(dat->field_0x56 - dat->field_0x54));
    m76E = (s16)gabi::ftoi(rnd + (f32)mpNpcDat->field_0x54);
    return 2;
}
VERIFY(0x022C4600, &daNpcPeople_c::executeWalkInit);

/* 022C46D0 */
void daNpcPeople_c::executeWalk() {
    WWHD_FUNC(0x022C46D0, void, this);
    if (!executeCommon()) {
        bool temp = false;
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (dNpc_PathRun_chkPointPass(&mPathRun, pos.get(), mPathRun.mbDir != 0)) {
            if (dNpc_PathRun_pointArg(&mPathRun, mPathRun.mIdx) == 0) {
                executeSetMode(8);
            }
            if (!dNpc_PathRun_nextIdxAuto(&mPathRun)) {
                temp = true;
            }
        }

        if (m789 != 0 || m78A != 0) {
            executeSetMode(0);
        } else if (!temp) {
            if (m78F != 8) {
                gabi::Local<cXyz> point, a, b;
                gabi::Local<be<s16>> angle;
                dNpc_PathRun_getPoint_x(&mPathRun, point.get(), mPathRun.mIdx);
                b->x = point->x;
                b->y = point->y;
                b->z = point->z;
                a->y = current.pos.y;
                a->x = current.pos.x;
                a->z = current.pos.z;
                dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
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
VERIFY(0x022C46D0, &daNpcPeople_c::executeWalk);

/* 022C48B8 */
s32 daNpcPeople_c::executeTurnInit() {
    WWHD_FUNC(0x022C48B8, s32, this);
    gabi::Local<cXyz> point, a, b;
    gabi::Local<be<s16>> angle;
    dNpc_PathRun_getPoint_x(&mPathRun, point.get(), mPathRun.mIdx);
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = point->x;
    b->y = point->y;
    b->z = point->z;
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    if (*angle == current.angle.y && (mEtcFlag & 8) == 0) {
        setAnmTbl(anm_dat(l_npc_anm_walk), 1);
        daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
        f32 rnd = cM_rndF((f32)(dat->field_0x56 - dat->field_0x54));
        m76E = (s16)gabi::ftoi(rnd + (f32)mpNpcDat->field_0x54);
        return 2;
    }
    return 3;
}
VERIFY(0x022C48B8, &daNpcPeople_c::executeTurnInit);

/* 022C4A04 */
void daNpcPeople_c::executeTurn() {
    WWHD_FUNC(0x022C4A04, void, this);
    if (!executeCommon()) {
        s16 temp;
        if (mEtcFlag & 0x400008) {
            temp = m77A;
        } else {
            gabi::Local<cXyz> point, a, b;
            gabi::Local<be<s16>> angle;
            dNpc_PathRun_getPoint_x(&mPathRun, point.get(), mPathRun.mIdx);
            a->x = current.pos.x;
            a->y = current.pos.y;
            b->x = point->x;
            a->z = current.pos.z;
            b->z = point->z;
            b->y = point->y;
            dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
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
VERIFY(0x022C4A04, &daNpcPeople_c::executeTurn);

/* 022C4B30 */
s32 daNpcPeople_c::executeBikkuriInit() {
    WWHD_FUNC(0x022C4B30, s32, this);
    m79A = m79A | 0x1;
    setAnmTbl(anm_dat(l_npc_anm_bikkuri), 1);
    m772 = 0x15;
    return 4;
}
VERIFY(0x022C4B30, &daNpcPeople_c::executeBikkuriInit);

/* 022C4B80 */
void daNpcPeople_c::executeBikkuri() {
    WWHD_FUNC(0x022C4B80, void, this);
    if (!executeCommon() && m772 != 0) {
        m799 = 0;
        m772 = m772 - 1;
        if (m772 == 0) {
            m744 = -1.0f;
            executeSetMode(5);
        }
    }
}
VERIFY(0x022C4B80, &daNpcPeople_c::executeBikkuri);

/* 022C4BF4 */
s32 daNpcPeople_c::executeFurueInit() {
    WWHD_FUNC(0x022C4BF4, s32, this);
    setAnmTbl(anm_dat(l_npc_anm_furue), 1);
    m772 = 0x96; /* HD: 150 frames (GameCube 0x78) */
    m774 = (s16)gabi::ftoi(cM_rndF(20.0f) + 10.0f);
    m786 = current.angle.y;
    be<s16>* k = gabi::at<be<s16>>(l_joint_dat_kyoro);
    m_jnt.setParam(k[2], k[3], k[6], k[7], k[0], k[1], k[4], k[5], k[8]);
    return 5;
}
VERIFY(0x022C4BF4, &daNpcPeople_c::executeFurueInit);

/* 022C4CA8 */
void daNpcPeople_c::executeFurue() {
    WWHD_FUNC(0x022C4CA8, void, this);
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
VERIFY(0x022C4CA8, &daNpcPeople_c::executeFurue);
