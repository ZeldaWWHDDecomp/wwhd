/**
 * d_a_npc_md_npc.cpp (WWHD)
 * Player - Medli: NPC action functions (waitNpcAction .. land03NpcAction, windProc)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_md.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_md_npc.h"
using namespace MdNpc;
static daNpc_Md_HIO_l npcHIO;
static u32 messageId;
static cXyz zeroVector;
static SavePointer saveInfo;
static inline dSv_event_c* mdNpc_event() { return reinterpret_cast<dSv_event_c*>(&saveInfo.mpInfo->mEvent); }
static ActionWords action_demoFlyNpcAction;
static ActionWords action_fall02NpcAction;
static ActionWords action_fallNpcAction;
static ActionWords action_glidingNpcAction;
static ActionWords action_land01NpcAction;
static ActionWords action_land02NpcAction;
static ActionWords action_land03NpcAction;
static ActionWords action_piyo2NpcAction;
static ActionWords action_searchNpcAction;
static ActionWords action_shipNpcAction;
static ActionWords action_squatdownNpcAction;
static ActionWords action_sqwait01NpcAction;
static ActionWords action_throwNpcAction;
static ActionWords action_waitNpcAction;
static ActionWords action_wallHitNpcAction;
// GHS copies each word to the guest stack before calling setNpcAction.
static inline void md_setNpcAction(daNpc_Md_c* actor, const ActionWords* action, void* arg = nullptr) {
    gabi::Local<ProcFunc_l> fn;
    auto* words = reinterpret_cast<ActionWords*>(fn.get());
    words->words[0] = action->words[0];
    words->words[1] = action->words[1];
    actor->setNpcAction(fn, arg);
}
static inline void md_camForceLockOff(fopAc_ac_c* actor) {
    auto* camera = play()->mpCamera.get();
    mdNpc_forceLockOff(&camera->mCamera, actor->mBsPcId);
}
static inline J3DFrameCtrl* md_morf2Frame(mDoExt_McaMorf2* m) { return &morf2(m)->mFrameCtrl; }
static inline bool md_morf2IsMorf(mDoExt_McaMorf2* m) { return morf2(m)->mCurMorf < 1.0f; }
static inline f32 md_morfFrame(mDoExt_McaMorf* m) { return m->getFrame(); }
static inline fopAc_ac_c* md_getShipActor() { return play()->mpShip; }
static inline void md_linkOnNpcCall() { play()->mpLink->mNoResetFlg1 |= 2; }
static inline be<u32>& md_attnFlags(fopAc_ac_c* actor) { return actor->attention_info.flags; }
static inline void mdNpc_sound(fopAc_ac_c* actor, u32 sound) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    mdNpc_monsSeStart(sound, &actor->current.pos, actor->mBsPcId, reverb);
}
static inline u8 md_chkAttentionSelf(daNpc_Md_c* actor) {
    gabi::Local<cXyz> pos;
    pos->x = actor->current.pos.x;
    pos->y = actor->current.pos.y;
    pos->z = actor->current.pos.z;
    s16 angle = (s16)(actor->shape_angle.y + actor->mJntCtrl.mAngles[0][1] + actor->mJntCtrl.mAngles[1][1]);
    // HD: preserve the full low byte of chkAttention instead of normalizing its bool result.
    return mdNpc_chkAttentionByte(actor, pos, angle, 0);
}
static inline void md_startShock(s32 strength, s32 flags) {
    gabi::Local<cXyz> up;
    up->x = 0.0f; up->y = 1.0f; up->z = 0.0f;
    dComIfGp_getVibration_StartShock(strength, flags, up);
}
static inline f32 daPy_getBaseAnimeFrame(fopAc_ac_c* player) { return reinterpret_cast<Player*>(player)->getBaseAnimeFrame(); }
// HD: frame setters truncate through s16 before storing the floating-point frame.
static inline f32 md_frameS16(f32 frame) { return (f32)(s16)gabi::ftoi(frame); }
static inline BOOL dComIfGs_isEventBit_md(u16 flag) { return dSv_event_isEventBit(mdNpc_event(), flag); }
static inline void dComIfGs_onEventBit_md(u16 flag) { dSv_event_onEventBit(mdNpc_event(), flag); }

/* 0228EB88 */
BOOL daNpc_Md_c::waitNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        if (mType == TYPE_SEA /* isTypeSea */) {
            s32 m = m312D; /* compared unsigned after sign extension */
            if (m3104 == 0x1E || m3104 == 0x29 || (u32)m == 0xE || (u32)m == 0x12 || (u32)m == 0x13 ||
                (u32)m == 0x1A || (u32)m == 0x25) {
                setHarpPlayNum(1);
            } else {
                setAnm(0x12);
            }
        } else if (mType == TYPE_M_DRA09 /* isTypeM_Dra09 */ && !dComIfGs_isEventBit_md(UNK_1140)) {
            setAnm(0x22);
        } else if (mType == TYPE_M_DAI || mType == TYPE_EDAICHI /* isTypeM_Dai || isTypeEdaichi */) {
            if (!checkStatus(daMdStts_UNK80)) {
                setAnm(0);
            }
        } else {
            setAnm(0);
        }
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        if (mType != TYPE_SEA) {
            setBitStatus(daMdStts_UNK4);
        } else {
            clearStatus(daMdStts_UNK4);
        }
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m3144 = 150;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            if (m3104 == 0x12) {
                setHarpPlayNum(1);
            } else if (m3104 == 0x13 || m3104 == 0x21) {
                setAnm(0);
            } else if (m3104 == 0x16 || m3104 == 0x17 || m3104 == 0x11) {
                setHarpPlayNum(1);
            }
        }
        if (mType == TYPE_ADANMAE /* isTypeAdanmae */ && dComIfGs_isEventBit_md(UNK_1102)) {
            fopAcM_delete(this);
            return TRUE;
        }
        gabi::Local<be<s32>> sp08;
        *sp08 = 0;
        if (mType == TYPE_SEA && !dComIfGs_isEventBit_md(UNK_1402)) {
            // HD: only test this invitation while Link is the current player.
            daPy_py_l* link = play()->mpLink;
            if (link == play()->mpPlayer.get()) {
                f32 distXZ2 = fopAcM_searchPlayerDistanceXZ2(this);
                f32 distY = link->current.pos.y - current.pos.y;
                if (distXZ2 < npcHIO.m0CC * npcHIO.m0CC && distY < npcHIO.m0BC && distY > npcHIO.m0C0) {
                    mCurEventMode = 0xB;
                }
            }
        } else {
            cXyz pos;
            pos.x = (f32)current.pos.x;
            pos.y = (f32)current.pos.y;
            pos.z = (f32)current.pos.z;
            s16 headAngle = (s16)(shape_angle.y + mJntCtrl.mAngles[0][1] + mJntCtrl.mAngles[1][1]);
            md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
            u32 attn = mdNpc_chkAttentionWord(this, &pos, headAngle, 1); /* chkAttention */
            *sp08 = attn;
            m312C = (u8)attn;
            if (attn != 0) {
                if (mType == TYPE_ATORIZK || mType == TYPE_ADANMAE || mType == TYPE_SEA) {
                    md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                }
                if (mType == TYPE_ATORIZK || mType == TYPE_ADANMAE) {
                    mCurEventMode = 2;
                } else if (mType == TYPE_M_DRA09) {
                    if (dComIfGs_isEventBit_md(UNK_1140)) {
                        if (dComIfGs_isEventBit_md(UNK_1101)) {
                            md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_SPEAK_e;
                            if (play()->mPlayerStatus0 & PLAYER_UNKNOWN_800000) {
                                dComIfGs_onEventBit_md(UNK_1280);
                            }
                            if (dComIfGs_isEventBit_md(UNK_1280)) {
                                mCurEventMode = 2;
                            } else {
                                mCurEventMode = 1;
                            }
                        } else {
                            md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                            mCurEventMode = 2;
                        }
                    } else {
                        setAnm(0x22);
                    }
                } else if (mType == TYPE_SEA) {
                    if (mCurEventMode == 0 && dComIfGs_isEventBit_md(UNK_1402)) {
                        mCurEventMode = 3;
                    }
                } else {
                    md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
                }
            }
            NpcCall(sp08);
        }
        if (mType == TYPE_M_DRA09 && dComIfGs_isEventBit_md(UNK_1101) && dComIfGs_isSwitch(m3100, current.roomNo)) {
            md_setNpcAction(this, &action_demoFlyNpcAction);
        }
        if (mType == TYPE_M_DAI || mType == TYPE_EDAICHI) {
            if (m3104 == 0) {
                if (mdNpc_calcTimer(&m3144) == 0) {
                    setAnm(40);
                }
            } else if (m3104 == 0x28 && m312A != 0) {
                setAnm(0);
                m3144 = (s16)(gabi::ftoi(cM_rndF(180.0f)) + 60);
            }
        }
        if (mType == TYPE_ATORIZK) {
            if (chkArea(&current.pos)) {
                lookBack(*sp08, 0, 1);
            } else {
                lookBack(0, 0, 0);
            }
        } else if (mType == TYPE_M_DRA09) {
            if (dComIfGs_isEventBit_md(UNK_1140)) {
                lookBack(1, 0, 0);
                setAnm(0);
            } else {
                lookBack(1, 0, 1);
            }
        } else {
            if (m3104 == 0x16 || m3104 == 0x17) {
                cXyz eye; /* cXyz::Zero by value (FPR copy) */
                const cXyz* zero = &zeroVector;
                f32 y = zero->y, x = zero->x, z = zero->z;
                eye.y = y;
                eye.z = z;
                eye.x = x;
                mdNpc_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &eye, shape_angle.y, 0, 0);
            }
            if (chkArea(&current.pos)) {
                if (mType == TYPE_SEA) {
                    lookBack(*sp08, 0, 1);
                } else {
                    lookBack(*sp08, 0, 0);
                }
            } else {
                lookBack(0, 0, 0);
            }
        }
        waitGroundCheck();
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}

/* 0228F424 */
BOOL daNpc_Md_c::harpWaitNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        setHarpPlayNum(1);
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        m30F0 = daMdStts_UNK80; /* clearStatus(); setBitStatus(UNK80) */
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = 0;
        m3146 = 0xC;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (m312A != 0) {
                m3146 = m3146 - 1;
                if (m3146 != 0) {
                    setHarpPlayNum(1);
                } else {
                    setAnm(0x11);
                    mActionStatus = mActionStatus + 1;
                    m3146 = 0x78;
                }
            }
        } else {
            m3146 = m3146 - 1;
            if (m3146 == 0) {
                setHarpPlayNum(1);
                m3146 = 0xC;
                mActionStatus = ACTION_ONGOING_1;
            }
        }
        if (mType == TYPE_M_DAIB /* isTypeM_DaiB */ && m3146 == 0xC) {
            BOOL play = FALSE;
            if (m3104 == 0x16) {
                if (md_morf2Frame(mpMorf.get())->checkPass(11.0f)) {
                    play = TRUE;
                }
            } else if (m3104 == 0x17 && md_morf2Frame(mpMorf.get())->checkPass(11.0f)) {
                play = TRUE;
            }
            if (play) {
                mdNpc_bgmStart(JA_BGM_MEDORI_TAKT_8);
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 0228F610 */
BOOL daNpc_Md_c::talkNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        messageId = fpcM_ERROR_PROCESS_ID_e;
        mMsgNo = getMsg();
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        m312B = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if (mActionStatus == ACTION_ONGOING_1) {
            if (XYTalkCheck()) {
                mActionStatus = ACTION_ONGOING_2;
            }
        } else if (mActionStatus == ACTION_ONGOING_2) {
            if (talk_init()) {
                mActionStatus = ACTION_ONGOING_3;
            }
        } else if (mActionStatus == ACTION_ONGOING_3) {
            if (talk(0)) {
                if (mType == TYPE_ATORIZK /* isTypeAtorizk */ || mType == TYPE_M_DRA09 /* isTypeM_Dra09 */) {
                    mActionStatus = 4;
                } else if (mType == TYPE_ADANMAE /* isTypeAdanmae */) {
                    if (dComIfGs_isEventBit_md(UNK_1104)) {
                        md_setNpcAction(this, &action_squatdownNpcAction);
                    }
                } else {
                    md_setNpcAction(this, &action_waitNpcAction);
                }
                dComIfGp_event_reset();
            }
        } else if (mType == TYPE_ATORIZK) {
            if (dComIfGs_isEventBit_md(MEDLI_GAVE_FATHERS_LETTER)) {
                md_setNpcAction(this, &action_waitNpcAction);
            }
        } else if (mType == TYPE_M_DRA09 && dComIfGs_isEventBit_md(UNK_1101)) {
            md_setNpcAction(this, &action_waitNpcAction);
        }
        mJntCtrl.mbTrn = 1; /* setTrn() */
        lookBack(1, 0, 0);
        current.angle.y = shape_angle.y;
        setAttention(md_morf2IsMorf(mpMorf.get()));
    }
    return TRUE;
}

/* 0228FA44 */
BOOL daNpc_Md_c::shipTalkNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
        setBitStatus(daMdStts_SHIP_RIDE);  /* onShipRide() */
        messageId = fpcM_ERROR_PROCESS_ID_e;
        mMsgNo = getMsg();
        setAnm(0x2B);
        m312B = 0;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        clearStatus(daMdStts_SHIP_RIDE); /* offShipRide() */
    } else {
        m312C = md_chkAttentionSelf(this);
        if (mActionStatus == ACTION_ONGOING_1) {
            if (talk_init()) {
                mActionStatus = ACTION_ONGOING_2;
            }
        } else {
            if (talk(0)) {
                dComIfGp_event_reset();
                md_setNpcAction(this, &action_shipNpcAction);
            }
        }
        lookBack(1, 0, 1);
        current.angle.y = shape_angle.y;
        setAttention(md_morf2IsMorf(mpMorf.get()));
    }
    return TRUE;
}

/* 0228FC24 */
BOOL daNpc_Md_c::kyohiNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0x27);
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        m3144 = 2;
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            m3144 = m3144 - 1;
            if (m3144 == 0) {
                md_setNpcAction(this, &action_waitNpcAction);
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 0228FD20 */
BOOL daNpc_Md_c::shipNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        mNoResetFlg1 = mNoResetFlg1 | 0x40; /* onNpcNotChange() */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(0x23);
        speedF = 0.0f;
        m30F8 = 120.0f;
        if (md_getShipActor() != nullptr) {
            setBitStatus(daMdStts_SHIP_RIDE);
        }
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        clearStatus(daMdStts_SHIP_RIDE);
    } else {
        md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
        fopAc_ac_c* ship = md_getShipActor();
        if (ship != nullptr) {
            setBitStatus(daMdStts_SHIP_RIDE);
            if (std::fabs((f32)ship->speedF) < 0.001f) {
                setAnm(0x2A);
                if (!(play()->mPlayerStatus0 & PLAYER_SHIP_RIDE)) {
                    m312C = md_chkAttentionSelf(this);
                    if (m312C != 0) {
                        md_attnFlags(this) = md_attnFlags(this) | (fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                        mCurEventMode = 2;
                    }
                }
            } else {
                setAnm(0x23);
            }
        }
        if (m3104 == 0x23) {
            cXyz eye; /* cXyz::Zero by value (FPR copy) */
            const cXyz* zero = &zeroVector;
            f32 x = zero->x, z = zero->z, y = zero->y;
            eye.z = z;
            eye.x = x;
            eye.y = y;
            mdNpc_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &eye, shape_angle.y, 0, 0);
        } else {
            lookBack(1, 0, 1);
        }
        current.angle.y = shape_angle.y;
        setAttention(true);
    }
    return TRUE;
}

/* 0228FF5C */
BOOL daNpc_Md_c::mwaitNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
        mNoResetFlg1 = mNoResetFlg1 & ~2u; /* offNpcCallCommand() */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        setAnm(0x20);
        shape_angle.x = 0;
        shape_angle.z = 0;
        gravity = npcHIO.m0F4;
        maxFallSpeed = -100.0f;
        speedF = 0.0f;
        speed.y = 0.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mActionStatus == ACTION_ONGOING_1) {
            if (!checkStatus(daMdStts_LIGHT_BODY_HIT)) {
                mActionStatus = ACTION_ONGOING_3;
            } else {
                md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
                f32 dist2 = fopAcM_searchActorDistance2(this, dComIfGp_getPlayer(0));
                if (!(mNoResetFlg1 & 2) /* !checkNpcCallCommand() */) {
                    if (dist2 < npcHIO.m0C8 * npcHIO.m0C8) {
                        md_linkOnNpcCall();
                    }
                } else {
                    mActionStatus = mActionStatus + 1;
                    f32 r = npcHIO.m0C4 + npcHIO.m0C4; /* 2.0f * m0C4 */
                    if (dist2 < r * r) {
                        mActionStatus = mActionStatus + 1;
                    }
                }
            }
            if (mActionStatus != ACTION_ONGOING_1) {
                m311A = 0;
                m310C = 0.0f;
                m3114 = 0;
                m3116 = 0;
                setAnm(0x21);
                m312A = 0;
            }
        } else {
            if (lookBackWaist(m311A, m310C) && m312A != 0) {
                m3114 = 0;
                m3116 = 0;
                if (mActionStatus == ACTION_ONGOING_2) {
                    md_setNpcAction(this, &action_searchNpcAction);
                } else {
                    setAnm(0);
                    md_setNpcAction(this, &action_waitNpcAction);
                }
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02290234 */
BOOL daNpc_Md_c::squatdownNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(3);
        speedF = 0.0f;
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if (m312A != 0) {
            md_setNpcAction(this, &action_sqwait01NpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02290358 */
BOOL daNpc_Md_c::sqwait01NpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) | fopAc_Attn_ACTION_CARRY_e;
        if (dComIfGs_isEventBit_md(MEDLI_GAVE_FATHERS_LETTER)) {
            md_attnFlags(this) = md_attnFlags(this) & ~(fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e);
        }
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_FLY);
        setBitStatus(daMdStts_UNK4);
        setAnm(4);
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        speedF = 0.0f;
        m30F8 = 75.0f;
        shape_angle.x = 0;
        shape_angle.z = 0;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        m312C = md_chkAttentionSelf(this);
        if ((s8)(u8)mType >= 4) {
            md_setNpcAction(this, &action_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 022904DC */
BOOL daNpc_Md_c::carryNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        md_attnFlags(this) = md_attnFlags(this) & ~fopAc_Attn_ACTION_CARRY_e;
        mNoResetFlg1 = (mNoResetFlg1 | 0x40) & ~2u; /* onNpcNotChange(); offNpcCallCommand() */
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        m3144 = (s16)(shape_angle.y - pl->shape_angle.y); /* fopAcM_toPlayerShapeAngleY */
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        u32 se;
        if (checkStatus(daMdStts_CARRY_ACTION) /* isNoCarryAction */) {
            setAnm(0x24);
            setBitStatus(daMdStts_UNK1);
            setHane02Emitter();
            se = JA_SE_CV_MD_CRASH;
        } else {
            if (m3104 == 4) {
                setAnm(5);
            } else {
                setAnm(3);
                m312A = 0;
            }
            se = JA_SE_CV_MD_LIFT_UP;
        }
        mdNpc_sound(this, se);
        m30F8 = 75.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        m3298.y = 0.0f;
        m3298.z = 0.0f;
        mNoResetFlg1 = mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
        clearStatus(daMdStts_CARRY_ACTION);   /* offNoCarryAction() */
        deleteHane02Emitter();
        deleteHane03Emitter();
        emitterDelete(m0508);
    } else {
        m3131 = 0;
        BOOL landed = FALSE;
        if (actor_status & fopAcStts_CARRY_e /* fopAcM_checkCarryNow */) {
            mdNpc_setRestart(this, 2);
        }
        if (!checkStatus(daMdStts_CARRY_ACTION)) {
            s32 flying = mdNpc_checkAutoJumpFlying(play()->mpLink);
            fopAc_ac_c* player = dComIfGp_getPlayer(0);
            if (mActionStatus == ACTION_ONGOING_1) {
                if (flying > 0) {
                    mActionStatus = ACTION_ONGOING_2;
                    setAnm(9);
                    setBitStatus(daMdStts_UNK1);
                    setWingEmitter();
                    setHane03Emitter();
                    m312A = 0;
                    mdNpc_sound(this, JA_SE_CV_MD_FLY_WITH_LINK);
                    if (dComIfGs_isEventBit_md(UNK_4001)) {
                        dComIfGs_onEventBit_md(UNK_4180);
                    }
                } else if (m3104 == 5) {
                    f32 spdF = player->speedF;
                    Morf2* morf = morf2(mpMorf.get());
                    f32 oldRate = morf->mFrameCtrl.mRate; /* mpMorf->getPlaySpeed() */
                    if (spdF == 0.0f) {
                        f32 f = daPy_getBaseAnimeFrame(player);
                        f32 end = (f32)morf->mFrameCtrl.mEnd; /* mpMorf->getEndFrame() */
                        morf = morf2(mpMorf.get());
                        if (!(f > end)) {
                            f32 fr = daPy_getBaseAnimeFrame(player);
                            morf->mFrameCtrl.mFrame = md_frameS16(fr);
                            morf2(mpMorf.get())->mFrameCtrl.mRate = 0.0f;
                            Morf2* arm = morf2(mpArmMorf.get());
                            fr = daPy_getBaseAnimeFrame(player);
                            arm->mFrameCtrl.mFrame = md_frameS16(fr);
                            morf2(mpArmMorf.get())->mFrameCtrl.mRate = 0.0f;
                        } else {
                            morf->mFrameCtrl.mRate = 1.0f;
                            morf2(mpArmMorf.get())->mFrameCtrl.mRate = 1.0f;
                        }
                    } else {
                        morf->mFrameCtrl.mRate = 1.0f;
                        morf2(mpArmMorf.get())->mFrameCtrl.mRate = 1.0f;
                    }
                    mDoExt_McaMorf2* m = mpMorf.get();
                    if (!(morf2(m)->mFrameCtrl.mRate == oldRate)) {
                        mdNpc_setMorf(m, 4.0f);
                        mdNpc_setMorf(mpArmMorf.get(), 4.0f);
                    }
                    cLib_chaseF(&m3298.x, npcHIO.m0F8, 1.0f);
                    cLib_chaseF(&m3298.y, npcHIO.m0FC, 1.0f);
                    cLib_chaseF(&m3298.z, npcHIO.m100, 1.0f);
                } else if (m312A != 0) {
                    setAnm(5);
                }
            } else if (mActionStatus == ACTION_ONGOING_2) {
                if (flying < 0x1E) {
                    mActionStatus = ACTION_ONGOING_3;
                    morf2(mpMorf.get())->mFrameCtrl.mRate = npcHIO.m120;      /* setPlaySpeed */
                    mpWingMorf->mFrameCtrl.mRate = npcHIO.m120; /* setPlaySpeed */
                }
            } else if (flying <= 0) {
                mActionStatus = ACTION_ONGOING_1;
                m3135 = m3135 | 1; /* setBitEffectStatus(1) */
                deleteHane03Emitter();
                setAnm(5);
                clearStatus(daMdStts_UNK1);
                landed = TRUE;
                mdNpc_sound(this, JA_SE_CV_MD_FLY_END);
            }
        }
        if (!(actor_status & fopAcStts_CARRY_e)) {
            if (checkStatus(daMdStts_CARRY_ACTION)) {
                if (speedF > 0.0f) {
                    md_setNpcAction(this, &action_throwNpcAction);
                } else {
                    md_setNpcAction(this, &action_land03NpcAction);
                }
            } else if ((m30F0 & daMdStts_UNK1) || landed) {
                if (!(current.pos.y - mAcch.GetGroundH() < npcHIO.m1AC)) {
                    md_setNpcAction(this, &action_fall02NpcAction);
                } else {
                    md_setNpcAction(this, &action_fallNpcAction);
                }
            } else if (speedF > 0.0f) {
                md_setNpcAction(this, &action_throwNpcAction);
            } else {
                md_setNpcAction(this, &action_sqwait01NpcAction);
            }
        }
        cXyz eye; /* cXyz::Zero by value (FPR copy) */
        const cXyz* zero = &zeroVector;
        f32 x = zero->x, z = zero->z, y = zero->y;
        eye.x = x;
        eye.z = z;
        eye.y = y;
        mdNpc_lookAtTarget(&mJntCtrl, &shape_angle.y, nullptr, &eye, shape_angle.y, m3110, 0);
        setAttention(true); // HD: carry actions also refresh attention.
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}

/* 02291094 */
BOOL daNpc_Md_c::throwNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        mdNpc_sound(this, JA_SE_CV_MD_THROW);
        m314C = 1;
        m314E = npcHIO.m1B4;
        m3148 = 0;
        m3154 = 0.0f;
        speedF = npcHIO.m0E8;
        speed.y = npcHIO.m0EC;
        maxFallSpeed = npcHIO.m110;
        gravity = npcHIO.m114;
        m30A0.copy(current.pos);
        clearStatus(daMdStts_UNK4);
        setBitStatus(daMdStts_UNK1 | daMdStts_FLY);
        setAnm(6);
        setWingEmitter();
        mAcchCir[1].SetWall(60.0f, 60.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus == ACTION_ENDING) {
        emitterDelete(m0508);
    } else {
        f32 frame = md_morfFrame(mpWingMorf.get());
        if (frame == 23.0f && speed.y < npcHIO.m0F0) {
            speed.y = npcHIO.m0F0;
        }
        if (frame > npcHIO.m154) { // HD: keep this ordered comparison for NaN inputs.
            if (windProc() == 0) {
                m314C = 0;
            }
        }
        if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, &action_land01NpcAction);
            md_camForceLockOff(this);
        } else if (mAcch.ChkWallHit()) {
            md_setNpcAction(this, &action_wallHitNpcAction);
            md_camForceLockOff(this);
        } else if (m312A != 0) {
            md_setNpcAction(this, &action_glidingNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02291304 */
BOOL daNpc_Md_c::glidingNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        if (m314C == 1) {
            m3148 = 0;
            setAnm(0xB);
        } else {
            m3148 = 1;
            setAnm(0xA);
        }
        m314A = 2;
        clearStatus(daMdStts_UNK4);
        setBitStatus(daMdStts_UNK1 | daMdStts_FLY);
        mAcchCir[1].SetWall(60.0f, 60.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (!chkAdanmaeDemoOrder()) {
            if (mAcch.ChkGroundHit()) {
                md_setNpcAction(this, &action_land01NpcAction, mType == TYPE_ADANMAE /* isTypeAdanmae */ ? (void*)&m3144 : nullptr);
                md_camForceLockOff(this);
            } else if (mAcch.ChkWallHit()) {
                md_setNpcAction(this, &action_wallHitNpcAction, mType == TYPE_ADANMAE ? (void*)&m3144 : nullptr);
                md_camForceLockOff(this);
            } else {
                if (m3148 == 0) {
                    // HD: compare the full r3 from windProc with the sign-extended stored result.
                    s32 wind = mdNpc_windProcWord(this);
                    if ((u32)(s32)m314C != (u32)wind) {
                        if (wind == 0) {
                            setAnm(0xA);
                            m3148 = 1;
                        } else {
                            setAnm(0xB);
                        }
                    }
                    m314C = (s16)wind;
                } else if (m312A != 0) {
                    m314A = m314A - 1;
                    if (m314A != 0) {
                        setAnm(0xA);
                    } else {
                        md_setNpcAction(this, &action_fall02NpcAction, mType == TYPE_ADANMAE ? (void*)&m3144 : nullptr);
                        return TRUE;
                    }
                }
                cXyz tmp; /* cXyz cStack_68(current.pos - m30A0), unused */
                cXyz_mi(&current.pos, &tmp, &m30A0);
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02290CF8 */
s16 daNpc_Md_c::windProc() {
    s16 ret = 1;
    gabi::Local<be<f32>> pow;
    cXyz wind;
    if (checkStatus(daMdStts_UNK8)) {
        mdNpc_getAllWind(&current.pos, &wind, pow);
    } else {
        wind.z = 0.0f;
        wind.y = 0.0f;
        wind.x = 0.0f;
        *pow = 0.0f;
    }
    if (npcHIO.m1C6 != 0 && mdNpc_chkTgHit(&mCyl2)) {
        u32 obj = mdNpc_getTgHitObj(&mCyl2);
        if (obj != 0 && (gabi::at<CollisionObject>(obj)->mAtType & AT_TYPE_WIND)) {
            cXyz* rvec = &reinterpret_cast<CollisionVector*>(&mCyl2)->mTgRVec; /* GetTgRVecP */
            f32 z = rvec->z, y = rvec->y, x = rvec->x;
            wind.z = z;
            wind.y = y;
            wind.x = x;
            cXyz res;
            mdNpc_normalizeZP(&wind, &res);
            *pow = (f32)npcHIO.m148;
        }
    }
    f32 wx = wind.x;
    f32 sinY = cM_ssin(shape_angle.y);
    f32 cosY = cM_scos(shape_angle.y);
    f32 rate = npcHIO.m134;
    cXyz windXZ; /* cStack_64 */
    cXyz tmpXZ;  /* abs2XZ temporary */
    windXZ.y = 0.0f;
    tmpXZ.y = 0.0f;
    f32 wz = wind.z;
    windXZ.z = wz;
    tmpXZ.z = wz;
    tmpXZ.x = wx;
    windXZ.x = wx;
    f32 sq = PSVECSquareMag(&tmpXZ);
    if (!(std::fabs(sq) < 3.814697265625e-06f)) {
        cXyz res;
        mdNpc_normalizeZP(&windXZ, &res);
        f32 dot = gabi::fmadds(sinY, windXZ.x, cosY * windXZ.z);
        if (!(dot > 0.7071f)) {
            ret = 0;
            rate = npcHIO.m130;
        }
    }
    cXyz add;  /* local_34 * local_a4 */
    cXyz sum;  /* speed + add */
    cXyz sumXZ;
    if (ret == 0) {
        f32 limit = *pow * rate;
        f32 v = m3154 + npcHIO.m138;
        if (v > limit) {
            v = limit;
        }
        m3154 = v;
        *pow = v;
        cXyz_ml(&wind, &add, v);
    } else {
        f32 v = *pow * rate;
        *pow = v;
        cXyz_ml(&wind, &add, v);
    }
    cXyz_pl(&speed, &sum, &add);
    current.angle.y = cM_atan2s(sum.x, sum.z);
    sumXZ.x = (f32)sum.x;
    sumXZ.y = 0.0f;
    sumXZ.z = (f32)sum.z;
    f32 spd = std_sqrtf(PSVECSquareMag(&sumXZ));
    speedF = spd;
    f32 maxSpd = npcHIO.m13C;
    speed.y = (f32)sum.y;
    speedF = (spd - maxSpd >= 0.0f) ? maxSpd : spd; /* cLib_maxLimit (fsel) */
    if (m3148 == 0) {
        maxFallSpeed = npcHIO.m110;
        gravity = npcHIO.m114;
    }
    if (!(std::fabs((f32)*pow) < 3.814697265625e-06f)) {
        s16 t = mdNpc_calcTimer(&m314E);
        s16 lim = (s16)(npcHIO.m1B4 - npcHIO.m1B6);
        if (ret == 1 && t > 0 && t < lim && (npcHIO.m1C7 != 0 || m3148 == 0)) {
            maxFallSpeed = npcHIO.m140;
            gravity = npcHIO.m144;
        }
    }
    return ret;
}

/* 02291644 */
BOOL daNpc_Md_c::fallNpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_FLY);
        setAnm(0xC);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, &action_land01NpcAction);
            md_camForceLockOff(this);
        } else if (mAcch.ChkWallHit()) {
            md_setNpcAction(this, &action_wallHitNpcAction);
            md_camForceLockOff(this);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02291764 */
BOOL daNpc_Md_c::fall02NpcAction(void* param_1) {
    if (mActionStatus == ACTION_STARTING) {
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_FLY);
        maxFallSpeed = npcHIO.m110;
        gravity = npcHIO.m114;
        m30F8 = 120.0f;
        setAnm(0xD);
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = param_1 != nullptr ? 1 : 0;
        m3150 = current.pos.y;
    } else if (mActionStatus != ACTION_ENDING) {
        if (mType == TYPE_M_DAI /* isTypeM_Dai */ && mAcch.ChkGroundHit()) {
            if (!(m3150 - current.pos.y < npcHIO.m1B0)) {
                md_startShock(6, -0x21);
            }
        }
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder()) {
                if (mAcch.ChkGroundHit()) {
                    md_setNpcAction(this, &action_land03NpcAction);
                    md_camForceLockOff(this);
                } else if (mAcch.ChkWallHit()) {
                    md_setNpcAction(this, &action_wallHitNpcAction);
                    md_camForceLockOff(this);
                }
            }
        } else {
            if (mAcch.ChkGroundHit()) {
                md_setNpcAction(this, &action_land03NpcAction);
                md_camForceLockOff(this);
            } else if (mAcch.ChkWallHit()) {
                md_setNpcAction(this, &action_wallHitNpcAction);
                md_camForceLockOff(this);
            }
        }
        setAttention(true);
    }
    return TRUE;
}

/* 022919D4 */
BOOL daNpc_Md_c::wallHitNpcAction(void* param_1) {
    if (mActionStatus == ACTION_STARTING) {
        mdNpc_sound(this, JA_SE_CV_MD_CRASH);
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        setAnm(0xD);
        shape_angle.x = 0;
        shape_angle.z = 0;
        int i = wallHitCheck();
        if (i >= 0) {
            current.angle.y = mAcchCir[i].m_wall_angle_y;
        }
        speedF = npcHIO.m150;
        clearStatus(daMdStts_UNK1 | daMdStts_UNK4);
        setBitStatus(daMdStts_UNK2 | daMdStts_FLY);
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        md_startShock(5, -0x11);
        m3144 = param_1 != nullptr ? 1 : 0;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder() && mAcch.ChkGroundHit()) {
                md_setNpcAction(this, &action_land02NpcAction);
                md_camForceLockOff(this);
            }
        } else if (mAcch.ChkGroundHit()) {
            md_setNpcAction(this, &action_land02NpcAction);
            md_camForceLockOff(this);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02291BE4 */
BOOL daNpc_Md_c::land01NpcAction(void* param_1) {
    if (mActionStatus == ACTION_STARTING) {
        mdNpc_sound(this, JA_SE_CV_MD_LANDING);
        if (checkStatus(daMdStts_UNK1)) {
            m3135 = m3135 | 1; /* setBitEffectStatus(1) */
        }
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        setAnm(0xE);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
        m3144 = param_1 != nullptr ? 1 : 0;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m3144 != 0) {
            if (!chkAdanmaeDemoOrder() && m312A != 0) {
                md_setNpcAction(this, &action_waitNpcAction);
            }
        } else if (m312A != 0) {
            md_setNpcAction(this, &action_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02291D50 */
BOOL daNpc_Md_c::land02NpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        setAnm(0xF);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            md_setNpcAction(this, &action_piyo2NpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* 02291E50 */
BOOL daNpc_Md_c::land03NpcAction(void* i_actionArg) {
    if (mActionStatus == ACTION_STARTING) {
        maxFallSpeed = -100.0f;
        gravity = npcHIO.m0F4;
        setAnm(0xF);
        clearStatus(daMdStts_UNK1 | daMdStts_UNK2 | daMdStts_UNK4 | daMdStts_FLY);
        shape_angle.x = 0;
        shape_angle.z = 0;
        speedF = 0.0f;
        m30F8 = 120.0f;
        mAcchCir[1].SetWall(60.0f, 20.0f);
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        if (m312A != 0) {
            md_setNpcAction(this, &action_waitNpcAction);
        }
        setAttention(true);
    }
    return TRUE;
}

/* WWHD glue
data npcHIO 10467A90
data messageId 10467A34
data zeroVector 101FFBA8
data saveInfo 101F84DC
data action_demoFlyNpcAction 1001D4F0
data action_fall02NpcAction 1001D4C8
data action_fallNpcAction 1001D4C0
data action_glidingNpcAction 1001D528
data action_land01NpcAction 1001D518
data action_land02NpcAction 1001D530
data action_land03NpcAction 1001D510
data action_piyo2NpcAction 1001D538
data action_searchNpcAction 1001D4D0
data action_shipNpcAction 1001D4B0
data action_squatdownNpcAction 1001D4F8
data action_sqwait01NpcAction 1001D500
data action_throwNpcAction 1001D508
data action_waitNpcAction 1001D490
data action_wallHitNpcAction 1001D520
ext 025052BC void mdNpc_forceLockOff(void* camera, u32 actorId)
ext 025E18EC void mdNpc_bgmStart(u32 sound)
ext 025E1A7C void mdNpc_monsSeStart(u32 sound, cXyz* pos, u32 actorId, s32 reverb)
ext 0228DAF4 u8 mdNpc_chkAttentionByte(daNpc_Md_c* actor, cXyz* pos, s16 angle, s32 mode)
ext 0228DAF4 u32 mdNpc_chkAttentionWord(daNpc_Md_c* actor, cXyz* pos, s16 angle, s32 mode)
ext 0201B3C0 void mdNpc_normalizeZP(cXyz* vector, cXyz* result)
ext 0257E1B8 void mdNpc_getAllWind(cXyz* pos, cXyz* vector, be<f32>* power)
ext 025162A4 BOOL mdNpc_chkTgHit(dCcD_GObjInf* object)
ext 02516300 u32 mdNpc_getTgHitObj(dCcD_GObjInf* object)
ext 02055B64 s16 mdNpc_calcTimer(be<s16>* timer)
ext 0259DED0 void mdNpc_lookAtTarget(dNpc_JntCtrl_c* control, be<s16>* outY, cXyz* target, cXyz* eye, s16 angle, s16 velocity, u8 headOnly)
ext 024430A4 s32 mdNpc_checkAutoJumpFlying(daPy_py_l* link)
ext 02445438 void mdNpc_setRestart(fopAc_ac_c* actor, s8 option)
ext 025E5D1C void mdNpc_setMorf(mDoExt_McaMorf2* morf, f32 value)
ext 02290CF8 s32 mdNpc_windProcWord(daNpc_Md_c* actor)
*/
