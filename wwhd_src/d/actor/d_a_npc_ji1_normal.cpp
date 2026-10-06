/**
 * d_a_npc_ji1_normal.cpp (WWHD)
 * NPC - Orca: guard sub-actions, normal and spin-attack (kaiten) actions
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* the absXZ of (a - b): cXyz::operator- into a temporary, then sqrtf(PSVECSquareMag({x, 0, z})) */
static inline f32 ji1_absXZ_diff(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* cM_atan2s of (a - b).x, .z */
static inline s16 ji1_atan2_diff(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    return cM_atan2s(d->x, d->z);
}
/* mDoExt_McaMorf::checkFrame(getEndFrame() - d) */
static inline BOOL ji1_checkEndFrame(daNpc_Ji1_c* i_this, f32 d) {
    mDoExt_McaMorf* m = i_this->mpOrcaMorf;
    return m->checkFrame(m->getEndFrame() - d);
}
/* dComIfGp_particle_set(ID_AK_JN_NG, field_0x7E0.GetTgHitPosP()) */
static inline void ji1_ng_particle(daNpc_Ji1_c* i_this) {
    dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, i_this->field_0x7E0.GetTgHitPosP());
}
/* eventInfo.checkCommandDemoAccrpt() / onCondition() */
static inline bool ji1_demoAccept(fopAc_ac_c* a) { return eventInfo_command(a) == 2; }
static inline void ji1_onCondition(fopAc_ac_c* a, u16 c) {
    u32 p = gabi::ea(a) + 0xFA;
    gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | c));
}
/* daPy_py_c::checkFrontRoll(): virtual (HD vtable at +0xB4, slot 0x5C) */
static inline BOOL daPy_checkFrontRoll(fopAc_ac_c* pl) {
    return gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x5C), pl);
}
/* daPy_py_c::checkFrontRollCrash(): mModeFlg2? (+0x3C0) & 0x2000 */
static inline bool daPy_checkFrontRollCrash(fopAc_ac_c* pl) { return (gabi::load<u32>(gabi::ea(pl) + 0x3C0) & 0x2000) != 0; }
/* 02529D7C dDetect_c::set_quake(const cXyz*) (dDetect_c at play+0x5A20) */
static inline void dComIfGp_getDetect_set_quake(cXyz* p) { gabi::call(0x02529D7C, gabi::at<u8>(dComIfGp_ea() + PLAY_DETECT), p); }
/* HD message manager (*(0x101F4B5C)): GameCube's msg_class* is gone, the status lives in the play
 * object (play+0x5BB2) */
static inline u32 fopMsgM_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* 025F795C (matcher: fopMsgM_SearchByID): the current message status */
static inline u8 msg_getStatus(u32 mgr) { return gabi::call<u8>(0x025F795C, mgr); }
/* 025F74D0: sets the message status (play+0x5BB2) */
static inline void msg_setStatus(u32 mgr, u32 s) { gabi::call(0x025F74D0, mgr, s); }
/* dComIfGp_checkMesgSendButton(): play+0x5BD2 */
static inline u8 dComIfGp_checkMesgSendButton() { return gabi::load<u8>(dComIfGp_ea() + 0x5BD2); }
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form, no reverb) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 025E1904 mDoAud_bgmStop(u32) */
static inline void mDoAud_bgmStop(u32 t) { gabi::call(0x025E1904, t); }
static inline be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10467228); }

enum {
    JA_SE_CV_JI_ATTACK = 0x4843,
    JA_SE_CV_JI_DEFENCE = 0x4833,
    JA_SE_OBJ_COL_SWS_NMTLP = 0x6817,
    JA_SE_CM_JI_SHOTEI = 0x58A0,
    JA_SE_CM_AJ_ANGRY_FOOT = 0x58A1,
    JA_SE_VS_JI_OPENING = 0x85B,
};
enum {
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_CONTINUES_e = 0xF,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
};

/* the guard-hit counter (field_0xD74) leads to an event at 3, 5 and from 6 on */
static inline bool ji1_guard_event(daNpc_Ji1_c* i_this, u32 n) {
    i_this->setAnm(5, 4.0f, 0);
    i_this->field_0xC84 = n;
    ptmf_copy(i_this->field_0x2C8, i_this->mAction);
    ji1_setAction(i_this, ACT_eventAction, nullptr);
    return true;
}

/* 0224FCC4 */
void daNpc_Ji1_c::normalSubActionHarpoonGuard(s16 param_1) {
    WWHD_FUNC(0x0224FCC4, void, this, param_1);
    if (field_0xD6C == 0) {
        if (field_0x7E0.ChkTgHit()) {
            field_0xD6C = 1;
            field_0xD74 += 1;
            field_0xD68 = 0;
            setAnm(9, 0.0f, 1);
            ji1_ng_particle(this);
            ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
            ji1_seStart(this, JA_SE_OBJ_COL_SWS_NMTLP, 0);
        } else {
            if (mAnimation == 0xD) {
                if (ji1_checkEndFrame(this, 2.0f)) {
                    s32 n = field_0xD68 + 1;
                    field_0xD68 = n;
                    if (n > 1) {
                        setAnm(0, 12.0f, 0);
                    }
                }
            }
        }
    }

    if (field_0xD6C == 1) {
        cLib_addCalcAngleS2(&current.angle.y, param_1, 2, 0x2000);
        if (mAnimation == 0xB) {
            if (ji1_checkEndFrame(this, 14.0f)) {
                setParticle(3, 3.0f, 0.5f);
                ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
            } else if (ji1_checkEndFrame(this, 2.0f)) {
                dtParticle();
                s32 n = field_0xD68;
                field_0xD68 = n + 1;
                if (n > 2) {
                    field_0xD6C = 0;
                    setAnm(0, 16.0f, 0);
                } else {
                    setAnm(0xB, 4.0f, 1);
                    mpOrcaMorf->setPlaySpeed(2.0f);
                }
            }
        } else {
            if (ji1_checkEndFrame(this, 2.0f)) {
                field_0xD6C = 0;
                if (field_0xD74 == 3) {
                    ji1_guard_event(this, 0xC);
                } else if (field_0xD74 == 5) {
                    ji1_guard_event(this, 0xD);
                } else if (field_0xD74 > 5) {
                    ji1_guard_event(this, 0xE);
                    field_0xD74 = 0;
                } else {
                    setAnm(0xD, 8.0f, 0);
                }
            }
        }
    }
}
VERIFY(0x0224FCC4, &daNpc_Ji1_c::normalSubActionHarpoonGuard);

/* 022504E4 */
void daNpc_Ji1_c::normalSubActionGuard(s16 param_1) {
    WWHD_FUNC(0x022504E4, void, this, param_1);
    if (field_0xD6C == 0) {
        if (field_0x7E0.ChkTgHit()) {
            field_0xD6C = 1;
            field_0xD68 = 0;
            setAnm(0xF, 0.0f, 1);
            ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
        }
    }

    if (field_0xD6C == 1) {
        cLib_addCalcAngleS2(&current.angle.y, param_1, 2, 0x2000);
        if (ji1_checkEndFrame(this, 2.0f)) {
            s32 n = field_0xD68;
            field_0xD68 = n + 1;
            if (n > 1) {
                field_0xD6C = 0;
                if (field_0xD74 == 3) {
                    ji1_guard_event(this, 0xC);
                } else if (field_0xD74 == 5) {
                    ji1_guard_event(this, 0xD);
                } else if (field_0xD74 > 5) {
                    ji1_guard_event(this, 0xE);
                } else {
                    setAnm(0x1, 8.0f, 0);
                }
            }
        }
    }
}
VERIFY(0x022504E4, &daNpc_Ji1_c::normalSubActionGuard);

/* 02250A1C */
BOOL daNpc_Ji1_c::normalAction(void* arg) {
    WWHD_FUNC(0x02250A1C, BOOL, this, arg);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    f32 temp = ji1_absXZ_diff(&player->current.pos, &current.pos);

    if (field_0xC78 == 0) {
        field_0x7E0.mGObjTg.mSPrm |= 1; /* OnTgShield (dCcD_GObjTg SPrm) */
        if (field_0xD84 == 1) {
            setAnm(0, 8.0f, 0);
        } else {
            setAnm(1, 8.0f, 0);
        }
        if (dComIfGs_isEventBit(0x0520 /* UNK_0520 */)) {
            eventInfo_setXyCheckCB(this, 0x02248C64 /* daNpc_Ji1_XyCheckCB */);
        }
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        dComIfGp_get(); /* HD: a daPy_getPlayerActorClass() whose result is unused */
        s16 temp2 = ji1_atan2_diff(&player->current.pos, &current.pos);

        if (eventInfo_checkCommandTalk(this)) {
            s16 temp3 = cLib_targetAngleY(&current.pos, &player->current.pos);
            temp3 -= m_jnt.mAngles[0][1]; /* getHead_y */
            temp3 -= m_jnt.mAngles[1][1]; /* getBackbone_y */
            cLib_addCalcAngleS2(&current.angle.y, temp3, 8, 0x800);
            if (cLib_distanceAngleS(current.angle.y, temp3) < 0x100) {
                ji1_setAction(this, ACT_talkAction, nullptr);
            }
        }

        if (temp < l_HIO().field_0x2C && isGuardAnim() == 0 && mAnimation != 0xD) {
            ji1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
            if (dComIfGs_isEventBit(0x0520)) {
                ji1_onCondition(this, 0x20 /* dEvtCnd_CANTALKITEM_e */);
            }
        }

        if (field_0xD84 == 1) {
            normalSubActionHarpoonGuard(temp2);
        } else {
            normalSubActionGuard(temp2);
        }
    }
    return true;
}
VERIFY(0x02250A1C, &daNpc_Ji1_c::normalAction);

/* 02250D54 */
BOOL daNpc_Ji1_c::kaitenwaitAction(void* arg) {
    WWHD_FUNC(0x02250D54, BOOL, this, arg);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    f32 temp = ji1_absXZ_diff(&player->current.pos, &current.pos);

    if (field_0xC78 == 0) {
        if (!dComIfGs_isEventBit(0x0501 /* UNK_0501 */)) {
            setAnm(0x12, 16.0f, 0);
        } else {
            setAnm(1, 4.0f, 0);
        }
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        if (!dComIfGs_isEventBit(0x0501) && daNpc_Ji1_plRoomOutCheck()) {
            field_0xC84 = 9;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_kaitenwaitAction);
            return true;
        }

        fopAc_ac_c* r27 = daPy_getPlayerActorClass();
        s16 temp2 = ji1_atan2_diff(&player->current.pos, &current.pos);

        if (!dComIfGs_isEventBit(0x0501) && isGuardAnim() == 0) {
            s16 temp3 = ji1_atan2_diff(&r27->current.pos, &current.pos);
            cLib_addCalcAngleS2(&current.angle.y, temp3, 0x10, 0x800);
            if (daPy_checkFrontRoll(r27)) {
                setAnm(1, 6.0f, 0);
            } else {
                setAnm(0x12, 16.0f, 0);
            }
        }

        if (eventInfo_checkCommandTalk(this)) {
            s16 temp3 = cLib_targetAngleY(&current.pos, &player->current.pos);
            temp3 -= m_jnt.mAngles[0][1];
            temp3 -= m_jnt.mAngles[1][1];
            cLib_addCalcAngleS2(&current.angle.y, temp3, 8, 0x800);
            if (cLib_distanceAngleS(current.angle.y, temp3) < 0x100) {
                ji1_setAction(this, ACT_talkAction, nullptr);
            }
        }

        if (temp < l_HIO().field_0x2C && isGuardAnim() == 0 && mAnimation != 0xD && r27->speedF < 1.0f) {
            ji1_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        }

        if (daPy_checkFrontRollCrash(player) && (!dComIfGs_isEventBit(0x0501) || l_HIO().field_0x30 != 0)) {
            field_0xD79 = 1;
            dComIfGp_getDetect_set_quake(nullptr);
            ji1_setAction(this, ACT_kaitenAction, nullptr);
        }

        if (field_0xD84 == 1) {
            normalSubActionHarpoonGuard(temp2);
        } else {
            normalSubActionGuard(temp2);
        }
    }
    return true;
}
VERIFY(0x02250D54, &daNpc_Ji1_c::kaitenwaitAction);

/* 02251424 */
BOOL daNpc_Ji1_c::kaitenspeakAction(void* arg) {
    WWHD_FUNC(0x02251424, BOOL, this, arg);
    if (field_0xC78 == 0) {
        setAnm(1, 8.0f, 0);
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        fopAc_ac_c* player = daPy_getPlayerActorClass();
        s16 temp2 = (s16)(ji1_atan2_diff(&player->current.pos, &current.pos) - current.angle.y);
        m_jnt.mbTrn = 1; /* setTrn */
        if (abs(temp2) < 0x1000) {
            setAnm(3, 8.0f, 0);
        }
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B258) /* "Ji1" */, nullptr, 0);
        if (getEventActionNo(staffIdx) == 2) {
            if (temp2 == 0) {
                m_jnt.mbTrn = 0; /* clrTrn */
                ji1_setAction(this, ACT_talkAction, nullptr);
            }
        } else {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    return true;
}
VERIFY(0x02251424, &daNpc_Ji1_c::kaitenspeakAction);

/* 02251690 */
BOOL daNpc_Ji1_c::kaitenExpAction(void* arg) {
    WWHD_FUNC(0x02251690, BOOL, this, arg);
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    ji1_absXZ_diff(&player->current.pos, &current.pos); /* unused */

    if (mAnimation == 0x10) {
        if (mpOrcaMorf->checkFrame(19.0f)) {
            if (field_0xD68 == 1) {
                gabi::Local<cXyz> v;
                v->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(5, -0x11, v);
            }
            ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
            ji1_seStart(this, JA_SE_CM_JI_SHOTEI, 0);
            setParticle(3, 3.0f, 0.5f);
        }
        if (ji1_checkEndFrame(this, 2.0f)) {
            s32 n = field_0xD68 + 1;
            if (n > 1) {
                field_0xD68 = 0;
                setAnm(0x11, 4.0f, 0);
            } else {
                field_0xD68 = n;
            }
            dtParticle();
        }
    }

    if (field_0xC78 == 0) {
        setAnm(0x10, 4.0f, 0);
        if (!ji1_demoAccept(this)) {
            fopAcM_orderOtherEventId(this, mEventIdx[6], 0xFF, 0xFFFF, 0, 1);
            ji1_onCondition(this, 2 /* dEvtCmd_INDEMO_e */);
            return false;
        }
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B260) /* "Ji1" */, nullptr, 0);
        if (getEventActionNo(staffIdx) == 2) {
            ji1_setAction(this, ACT_kaitenspeakAction, nullptr);
        } else {
            dComIfGp_evmng_cutEnd(staffIdx);
        }
    }
    return true;
}
VERIFY(0x02251690, &daNpc_Ji1_c::kaitenExpAction);

/* 02251ED0 HD: the message is handled through the HD message manager (status get/set), and the
 * message box search of the second step is gone (it always proceeds) */
BOOL daNpc_Ji1_c::kaitenAction(void* arg) {
    WWHD_FUNC(0x02251ED0, BOOL, this, arg);
    u32 mgr = gabi::load<u32>(0x101F4B5C);
    if (field_0xC78 == 0) {
        if (!ji1_demoAccept(this)) {
            fopAcM_orderOtherEventId(this, mEventIdx[5], 0xFF, 0xFFFF, 0, 1);
            ji1_onCondition(this, 2 /* dEvtCmd_INDEMO_e */);
            return false;
        }
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B264) /* "Ji1" */, nullptr, 0);
        l_msgId() = 0xFFFFFFFF;
        mMsgNo = 0x974;
        dComIfGp_evmng_cutEnd(staffIdx);
        current.angle.y = 0;
        field_0xC78 += 1;
    } else if (field_0xC78 != -1) {
        s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B264), nullptr, 0);
        int actionNo = getEventActionNo(staffIdx);
        if (l_msgId() == 0xFFFFFFFF) {
            if (actionNo == 2) {
                u32 id = fopMsgM_messageSet(mgr, mMsgNo, &eyePos);
                l_msgId() = id;
                if (id != 0xFFFFFFFF) {
                    if (mMsgNo == 0x974) {
                        current.angle.y = -0x8000;
                    } else if (mMsgNo == 0x975) {
                        dComIfGp_evmng_cutEnd(staffIdx);
                    }
                }
            } else if (actionNo == 1) {
                m_jnt.mbHeadLock = 1;     /* onHeadLock */
                m_jnt.mbBackBoneLock = 1; /* onBackBoneLock */
                setAnm(4, 8.0f, 0);
                dComIfGp_evmng_cutEnd(staffIdx);
            }
        } else if (field_0xC78 == 1) {
            if (actionNo == 2) {
                gabi::Local<cXyz> v;
                v->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(5, -0x11, v);
                mDoAud_seStart2(JA_SE_CM_AJ_ANGRY_FOOT, nullptr);
            }
            field_0xC78 += 1;
        } else if (msg_getStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
            /* next_msgStatus's r3 is passed on as is (GHS does not re-extend the u16) */
            msg_setStatus(mgr, gabi::call<u32>(0x0224DA74, this, &mMsgNo));
            if (msg_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                fopMsgM_messageSet(mgr, mMsgNo, nullptr);
                setAnimFromMsgNo(mMsgNo);
            }
        } else if (msg_getStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
            if (actionNo == 2) {
                msg_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                l_msgId() = 0xFFFFFFFF;
                mMsgNo = 0x975;
                field_0xC78 = 1;
                return true;
            }
            if (staffIdx != -1) {
                dComIfGp_evmng_cutEnd(staffIdx);
            }
            if (dComIfGp_evmng_endCheck(mEventIdx[5])) {
                dComIfGs_onEventBit(0x0501);
                field_0xD28.copy(home.pos);
                home.angle.y = 0;
                msg_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                if (mMsgNo == 0x967) {
                    field_0xD70 = -1;
                    ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
                    mDoAud_bgmStop(45);
                    ji1_setAction(this, ACT_plmoveAction, nullptr);
                } else {
                    setAnm(1, 8.0f, 0);
                    ji1_setAction(this, ACT_normalAction, nullptr);
                }
                dComIfGp_event_reset();
            }
        } else if (dComIfGp_checkMesgSendButton() && actionNo == 3 && mMsgNo == 0x976) {
            setAnm(2, 4.0f, 0);
            m_jnt.mbHeadLock = 0;     /* offHeadLock */
            m_jnt.mbBackBoneLock = 0; /* offBackBoneLock */
            dComIfGp_evmng_cutEnd(staffIdx);
        }

        if (actionNo == 4) {
            fopAc_ac_c* player = daPy_getPlayerActorClass();
            s16 temp = ji1_atan2_diff(&player->current.pos, &current.pos);
            cLib_addCalcAngleS2(&current.angle.y, temp, 4, 0x1000);
        }
    }
    return true;
}
VERIFY(0x02251ED0, &daNpc_Ji1_c::kaitenAction);
