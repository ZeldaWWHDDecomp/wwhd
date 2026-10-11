/**
 * d_a_npc_kk1_c.cpp (WWHD)
 * NPC - Mila (poor, Windfall): part C (talk animation attributes, messages, checks, the
 * wait/walk/comment actions).
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names.
 */
#define SAFESTRING_VTBL 0x1001C250 /* this TU's sead::SafeString vtable */
#include <math.h>
#include "d/actor/d_a_npc_kk1_c.h"

// HD: these are aliases into TU-owned data; this part defines no TU static object.
static Kk1ActionParameters& l_action_parameters = gabi::object_at<Kk1ActionParameters>(0x10467698);
static daNpc_Kk1_c::anm_prm_c (&a_anm_prm_tbl)[13] = gabi::object_at<daNpc_Kk1_c::anm_prm_c[13]>(0x101BF51C);
static be<u32>& l_frame_counter = gabi::object_at<be<u32>>(0x101FF558);
static be<f32>& l_player_detection_offset = gabi::object_at<be<f32>>(0x1047BB20);
static inline fopAc_ac_c* linkPlayer() { return dComIfGp_getLinkPlayer(); }
static inline s32 abs_s16(s16 v) { return v < 0 ? -(s32)v : (s32)v; }

/* 0226EF4C */
void daNpc_Kk1_c::setAnm_ATR() {
    Kk1ActionFields& fields = Kk1_fields(this);
    anm_prm_c* tbl = a_anm_prm_tbl; /* a_anm_prm_tbl (0x10 bytes each) */
    init_texPttrnAnm(tbl[(u8)fields.mTalkAnimation].mTexNo, true);
    setAnm_anm(&tbl[(u8)fields.mTalkAnimation]);
}

/* 0226EFB4 */
void daNpc_Kk1_c::chngAnmAtr(u8 i_attr) {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (mCurrMsgNo == 0x1CAC) {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* partner = searchByID(fields.mPartnerId, notFound);
        if (partner != nullptr && *notFound == 0) {
            fields.mLookTarget.x = partner->current.pos.x;
            f32 y = partner->current.pos.y;
            fields.mLookTarget.y = y;
            fields.mLookTarget.z = partner->current.pos.z;
            fields.mUseLookTarget = 1;
            fields.mLookMode = 2;
            fields.mLookTarget.y = y + (l_action_parameters.mAttentionYOffset + 200.0f);
            fields.mTalkLookTimer = l_action_parameters.mTalkLookTimer;
        }
    }
    if (i_attr != fields.mTalkAnimation && i_attr < 0xD) {
        fields.mTalkAnimation = i_attr;
        setAnm_ATR();
    }
}

/* 0226F088 */
void daNpc_Kk1_c::ctrlAnmAtr() {
    Kk1ActionFields& fields = Kk1_fields(this);
    switch ((u8)fields.mTalkAnimation) {
    case 8:
        if ((s8)fields.mAnimationEnded != 0) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            setAnm_NUM(0, 1);
            mpMorf->setMorf(0.0f);
            fields.mTalkAnimation = 0;
        }
        break;
    case 10:
        if ((s8)fields.mAnimationEnded != 0) {
            setAnm_NUM(0, 1);
            fields.mTalkAnimation = 0;
        }
        break;
    case 11:
        if ((s8)fields.mAnimationEnded != 0) {
            setAnm_NUM(6, 1);
            fields.mTalkAnimation = 6;
        }
        break;
    }
}

/* 0226F170 */
void daNpc_Kk1_c::anmAtr(u16 i_msgStatus) {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (i_msgStatus == KK1_MSG_TYPING) {
        if ((s8)fields.mMessageAnimationStarted == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            fields.mMessageAnimationStarted = fields.mMessageAnimationStarted + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo();
        if (tag != 0xFF && tag != fields.mMessageTag) {
            dComIfGp_clearMesgAnimeTagInfo();
            fields.mMessageTag = tag;
        }
    } else if (i_msgStatus == KK1_MSG_DISPLAYED) {
        fields.mMessageAnimationStarted = 0;
    }
    ctrlAnmAtr();
}

/* 0226F230 */
u16 daNpc_Kk1_c::next_msgStatus(be<u32>* pMsgNo) {
    Kk1ActionFields& fields = Kk1_fields(this);
    switch ((u32)*pMsgNo) {
    case 0x1C85: *pMsgNo = 0x1C86; break;
    case 0x1C86: *pMsgNo = 0x1CA7; break;
    case 0x1C88: *pMsgNo = 0x1C89; break;
    case 0x1C89: *pMsgNo = 0x1C8A; break;
    case 0x1C8A: *pMsgNo = 0x1CA9; break;
    case 0x1C8B:
        *pMsgNo = 0x1CAC;
        fields.mPartnerEventEnabled = 1;
        break;
    case 0x1C8D: *pMsgNo = 0x1C8E; break;
    case 0x1C8E: *pMsgNo = 0x1C8F; break;
    case 0x1C8F: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = 0x1C90;
        } else if (sel == 1) {
            *pMsgNo = 0x1CA5;
            fields.mRefusedAnswerCount = fields.mRefusedAnswerCount + 1;
        }
        break;
    }
    case 0x1C90: *pMsgNo = 0x1C91; break;
    case 0x1C91: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = 0x1C93;
        } else if (sel == 1) {
            *pMsgNo = 0x1C92;
            fields.mRefusedAnswerCount = fields.mRefusedAnswerCount + 1;
        }
        break;
    }
    case 0x1C92: *pMsgNo = 0x1C91; break;
    case 0x1C93: *pMsgNo = 0x1C94; break;
    case 0x1C94: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = 0x1C96;
        } else if (sel == 1) {
            *pMsgNo = 0x1C95;
            fields.mRefusedAnswerCount = fields.mRefusedAnswerCount + 1;
        }
        break;
    }
    case 0x1C95: *pMsgNo = 0x1C96; break;
    case 0x1C96: *pMsgNo = 0x1C97; break;
    case 0x1C97: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = 0x1C98;
        } else if (sel == 1) {
            *pMsgNo = fields.mRefusedAnswerCount > 1 ? 0x1C9A : 0x1C9B;
        }
        break;
    }
    case 0x1C98: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = 0x1C99;
        } else if (sel == 1) {
            return KK1_MSG_ENDS; /* fopMsgStts_MSG_ENDS_e */
        }
        break;
    }
    case 0x1C99: {
        u32 sel = Kk1_selectedAnswer();
        if (sel == 0) {
            *pMsgNo = fields.mRefusedAnswerCount > 1 ? 0x1C9C : 0x1C9D;
        } else if (sel == 1) {
            *pMsgNo = 0x1C9A;
        }
        break;
    }
    case 0x1C9B: *pMsgNo = 0x1C9D; break;
    case 0x1C9D: *pMsgNo = 0x1C9E; break;
    case 0x1C9E: *pMsgNo = 0x1CAA; break;
    case 0x1CA1: *pMsgNo = 0x1CA2; break;
    case 0x1CA2: *pMsgNo = 0x1CA3; break;
    case 0x1CA3: *pMsgNo = 0x1CA4; break;
    case 0x1CA4: *pMsgNo = 0x1CAB; break;
    case 0x1CA5: *pMsgNo = 0x1C8F; break;
    case 0x1CA7: *pMsgNo = 0x1CA8; break;
    case 0x1CAA: *pMsgNo = 0x1C9F; break;
    default:
        return KK1_MSG_ENDS; /* fopMsgStts_MSG_ENDS_e */
    }
    return KK1_MSG_CONTINUES; /* fopMsgStts_MSG_CONTINUES_e */
}

/* 0226F538 */
u32 daNpc_Kk1_c::getMsg_KK1_0() {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (dKy_daynight_check() == 1) {
        if (fields.mNightMessageFlag != 0) {
            fields.mNightMessageFlag = 0;
            return 0x1CA1;
        }
        return 0x1C8B;
    }
    if (dComIfGs_isEventBit(0xE08)) {
        return 0x1C85;
    }
    if (dComIfGs_isEventBit(0xE10)) {
        return 0x1C87;
    }
    return 0x1C88;
}

/* 0226F61C */
u32 daNpc_Kk1_c::getMsg() {
    Kk1ActionFields& fields = Kk1_fields(this);
    u32 msg = 0;
    if ((s8)fields.mMessageType == 0) {
        msg = getMsg_KK1_0();
    }
    return msg;
}

/* 0226F654 */
bool daNpc_Kk1_c::chk_talk() {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            fields.mPresentedItem = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    fields.mPresentedItem = 0xFF;
    return true;
}

/* 0226F6EC */
u8 daNpc_Kk1_c::chk_parts_notMov() {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (fields.mPreviousLookAngles.y != m_jnt.mAngles[0][1] || fields.mPreviousLookAngles.z != m_jnt.mAngles[1][1] || fields.mPreviousLookAngles.x != current.angle.y) {
        return 1;
    }
    return 0;
}

/* 0226F72C */
BOOL daNpc_Kk1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}

/* 0226F7B4 */
void daNpc_Kk1_c::createTama(f32 i_speed) {
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, 0, 0);
    gabi::Local<cXyz> start;
    start->x = eyePos.x;
    start->y = eyePos.y + 15.0f;
    start->z = eyePos.z;
    gabi::Local<cXyz> dist;
    dNpc_playerEyePos(dist, -20.0f);
    gabi::Local<cXyz> target;
    target->copy(*dist);
    cXyz_mi(target, dist, &eyePos);
    gabi::Local<cXyz> xz;
    xz->x = dist->x;
    xz->z = dist->z;
    xz->y = 0.0f;
    std_sqrtf(PSVECSquareMag(xz)); /* dist.absXZ(): unused */
    angle->y = cLib_targetAngleY(start, target);
    angle->x = Kk1_targetAngleX(start, target);
    fopAc_ac_c* tama = Kk1_fastCreate(0x1D0 /* PROC_KANTERA? tama */, 0, &eyePos, current.roomNo, angle, nullptr, -1, 0, nullptr);
    if (tama != nullptr) {
        Kk1_view<Kk1Tama>(tama)->mSpeed = i_speed;
        Kk1_view<Kk1Tama>(tama)->mOwnerId = mBsPcId; /* fopAcM_GetID(this) */
        tama->speedF = 50.0f;
    }
}

/* 0226F908 */
bool daNpc_Kk1_c::chk_areaIN(f32 i_radius, cXyz* i_center) {
    gabi::Local<cXyz> diff;
    cXyz_mi(&linkPlayer()->current.pos, diff, i_center);
    gabi::Local<cXyz> xz;
    xz->z = diff->z;
    xz->y = 0.0f;
    xz->x = diff->x;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &linkPlayer()->current.pos) - current.angle.y);
    f32 tamaSpeed = i_radius;
    if (abs_s16(angle) > 0x4AAA) {
        tamaSpeed = i_radius * 0.5f;
    }
    bool in = dist < i_radius;
    if (in) {
        u32 link = gabi::ea(linkPlayer());
        if (link != gabi::ea(dComIfGp_getPlayer(0))) {
            u32 pl = gabi::ea(dComIfGp_getPlayer(0));
            Kk1_playerFlags(pl)->mFlags |= 1;
            return false;
        }
        if ((u32)l_frame_counter % 3 == 0) {
            createTama(tamaSpeed);
        }
    }
    return in;
}

/* 0226FABC */
bool daNpc_Kk1_c::startEvent_check() {
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (chk_areaIN(l_action_parameters.mDetectionRadius, pos)) {
        f32 dist = std_sqrtf(Kk1_squareDistance(&current.pos, &linkPlayer()->current.pos));
        if (dist < (f32)l_player_detection_offset + 210.0f || Kk1_fields(this).mPartnerEventStatus != 0) {
            return true;
        }
    }
    return false;
}

/* 0226FB70 */
BOOL daNpc_Kk1_c::chkHitPlayer() {
    BOOL hit = FALSE;
    if (mCyl.ChkCoHit()) {
        u32 obj = Kk1_coHitObject(&mCyl);
        if (obj != 0) {
            u32 stts = gabi::ea(Kk1_hitObject(obj)->mpStts.get());
            if (stts == 0) {
                return FALSE;
            }
            u32 actor = gabi::ea(Kk1_status(stts)->mp_actor.get());
            if (actor != 0) {
                hit = Kk1_actor(actor)->mProcName == 0xA8; /* fopAcM_GetName(actor) == PROC_PLAYER */
            }
        }
    }
    return hit;
}

/* 0226FC1C */
u8 daNpc_Kk1_c::chk_attn() {
    Kk1ActionFields& fields = Kk1_fields(this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = diff->x;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    fopAc_ac_c* link = linkPlayer();
    f32 dy = current.pos.y - link->current.pos.y;
    s16 angle = (s16)(cLib_targetAngleY(&current.pos, &linkPlayer()->current.pos) - current.angle.y);
    u8 ret = 0;
    f32 maxAngle = (s8)fields.mLookMode == 1 ? 90.0f : 60.0f;
    if (dist < 200.0f) {
        f32 deg = (f32)abs_s16(angle) / 182.04444885253906f;
        if (deg < maxAngle && fabsf(dy) < 300.0f) {
            ret = 1;
        }
    }
    return ret;
}

/* 0226FE78 */
BOOL daNpc_Kk1_c::wait_1() {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (fields.mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(KK1_ACTION_TALK);
            setAnm_NUM(0, 1);
            fields.mUseLookTarget = 0;
            m_jnt.mbTrn = 1;
            fields.mLookMode = 1;
        }
        return TRUE;
    }
    u8 onPath = fields.mOnPath;
    fields.mLookMode = 0;
    fields.mUseLookTarget = 1;
    fields.mEventState = 2;
    if (onPath != 0) {
        gabi::Local<cXyz> pnt;
        Kk1_pathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        if (abs_s16((s16)(target - current.angle.y)) < 0x1800) {
            setStt(KK1_ACTION_WALK);
            fields.mWaitingAtPathEnd = 0;
        }
        return TRUE;
    }
    if ((s8)fields.mAnimationNo == 0xB) {
        if ((s8)fields.mAnimationEnded != 0) {
            setAnm_NUM(0, 1);
            fields.mActionTimer = cLib_getRndValue(60, 30);
            fields.mWaitingAtPathEnd = 1;
        }
        return TRUE;
    }
    gabi::Local<cXyz> offset;
    offset->z = 0.0f;
    offset->x = -100.0f;
    offset->y = 0.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(fields.mHomeAngle.y);
    gabi::Local<cXyz> pnt;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, pnt);
    s16 target = cLib_targetAngleY(&current.pos, pnt);
    cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
    s16 diff = (s16)(target - current.angle.y);
    if (fields.mWaitingAtPathEnd == 0) {
        if (diff == 0) {
            setAnm_NUM(0xB, 1);
        }
        return TRUE;
    }
    if (cLib_calcTimer(&fields.mActionTimer) != 0) {
        return TRUE;
    }
    if (mPathRun.mPath.get() != nullptr && Kk1_pathMaxPoint(&mPathRun) > 2 && Kk1_shopCreateCount() > 1) {
        fields.mOnPath = 1;
        return TRUE;
    }
    fields.mWaitingAtPathEnd = 0;
    return TRUE;
}

/* 022700E0 */
BOOL daNpc_Kk1_c::walk_1() {
    Kk1ActionFields& fields = Kk1_fields(this);
    gabi::Local<cXyz> pnt;
    Kk1_pathPoint(pnt, &mPathRun);
    if (Kk1_view<Kk1PathFlags>(mPathRun.mPath.get())->mClosed & 1) { /* the path is a loop */
        return TRUE;
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, pnt);
    gabi::Local<cXyz> xz;
    xz->y = 0.0f;
    xz->x = diff->x;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    f32 speed;
    s16 oldAngle;
    if (fields.mOnPath == 0) {
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        oldAngle = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        speed = l_action_parameters.mWalkSpeed;
        if (fields.mOnPath == 0) {
            speed = 0.0f;
        } else if (fields.mTalkRequested != 0 || chk_attn()) {
            speed = 0.0f;
        }
    } else {
        if (dist < l_action_parameters.mPathPointRadius) {
            bool turn = true;
            if (Kk1_pathNextIdx(&mPathRun) == 1) {
                if (mPathRun.mbDir == 0) {
                    turn = false;
                } else {
                    s32 count = Kk1_shopCreateCount();
                    if ((s32)mPathRun.mIdx < count) {
                        turn = false;
                    } else {
                        Kk1_pathDecIdx(&mPathRun);
                        Kk1_pathDecIdx(&mPathRun);
                    }
                }
            }
            if (turn) {
                u8 dir = mPathRun.mbDir;
                fields.mOnPath = 0;
                mPathRun.mbDir = dir ^ 1;
            }
        }
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        oldAngle = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        speed = l_action_parameters.mWalkSpeed;
        if (fields.mOnPath == 0) {
            speed = 0.0f;
        } else if (fields.mTalkRequested != 0 || chk_attn()) {
            speed = 0.0f;
        }
    }
    cLib_chaseF(&speedF, speed, l_action_parameters.mWalkAcceleration);
    f32 rate = speedF * l_action_parameters.mAnimationSpeedScale;
    mpMorf->setPlaySpeed((rate - 0.5f >= 0.0f) ? rate : 0.5f);
    if (gabi::ftoi(speed) == 0) {
        s32 curSpeed = gabi::ftoi(speedF);
        current.angle.y = oldAngle;
        if (curSpeed == 0) {
            if (fields.mTalkRequested != 0) {
                if (chk_talk()) {
                    setStt(KK1_ACTION_WAIT);
                    setAnm_NUM(0, 1);
                    fields.mUseLookTarget = 0;
                    m_jnt.mbTrn = 1;
                    fields.mLookMode = 1;
                }
                return TRUE;
            }
            if (fields.mOnPath == 0) {
                setStt(KK1_ACTION_WAIT);
            } else {
                setStt(KK1_ACTION_WAIT_FAR);
            }
            fields.mWaitingAtPathEnd = 0;
            fields.mOnPath = 0;
            return TRUE;
        }
    }
    fields.mUseLookTarget = 1;
    fields.mEventState = 2;
    fields.mLookMode = 0;
    return TRUE;
}

/* 022703B4 */
BOOL daNpc_Kk1_c::wait_2() {
    Kk1ActionFields& fields = Kk1_fields(this);
    cLib_addCalcAngleS(&current.angle.y, fields.mHomeAngle.y, 4, 0x800, 0x80);
    if (fields.mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(KK1_ACTION_TALK);
            fields.mUseLookTarget = 0;
            fields.mPlayerFar = 0;
            fields.mLookMode = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    s8 state = (s8)fields.mEventState;
    fields.mLookMode = 0;
    fields.mUseLookTarget = 1;
    if (state == 3 || state == 4) {
        return TRUE;
    }
    if (fields.mPartnerEventEnabled != 0 && fields.mSwitchNo != 0xFF && dComIfGs_isSwitch(fields.mSwitchNo, current.roomNo)) {
        gabi::Local<be<s32>> notFound;
        fopAc_ac_c* partner = searchByID(fields.mPartnerId, notFound);
        if (partner != nullptr && *notFound == 0) {
            s16 angle = (s16)(cLib_targetAngleY(&partner->current.pos, &linkPlayer()->current.pos) - partner->current.angle.y);
            fields.mEventState = abs_s16(angle) >= 0x4000 ? 3 : 4;
            mStts.m_weight = 0xD9; /* mStts weight */
            return TRUE;
        }
    }
    fields.mEventState = 2;
    return TRUE;
}

/* 0227051C */
void daNpc_Kk1_c::init_CMT_WAI() {
    Kk1ActionFields& fields = Kk1_fields(this);
    fields.mActionTimer = cLib_getRndValue(0x5A, 0xB4);
    fields.mLookMode = 5;
    fields.mUseLookTarget = 1;
    setAnm_NUM(0, 1);
}

/* the actor's position, as a by-value cXyz argument */
static inline void copy_pos(cXyz* dst, cXyz* src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}
static inline bool not_talking(s8 state) { return state != 1 && state < 3; }

/* 02270570 */
void daNpc_Kk1_c::move_CMT_WAI() {
    Kk1ActionFields& fields = Kk1_fields(this);
    s16 timer = cLib_calcTimer(&fields.mActionTimer);
    s8 state = (s8)fields.mEventState;
    if (timer == 0) {
        if (not_talking(state)) {
            gabi::Local<cXyz> pos;
            copy_pos(pos, &current.pos);
            if (chk_areaIN(l_action_parameters.mDetectionRadius, pos)) {
                fields.mEventState = 8;
                return;
            }
        }
        fields.mCommentAction = 0;
        setAnm_NUM(3, 1);
        fields.mOnPath = 1;
        return;
    }
    if (not_talking(state) && startEvent_check()) {
        fields.mEventState = 9;
    }
}

/* 0227066C */
void daNpc_Kk1_c::init_CMT_TRN() {
    Kk1ActionFields& fields = Kk1_fields(this);
    fields.mTurnStartAngle = current.angle.y;
    fields.mActionTimer = cLib_getRndValue(0x5A, 0xB4);
    fields.mTurnTimer = l_action_parameters.mTurnTimer;
    fields.mTurnMaxStep = l_action_parameters.mTurnMaxStep;
    fields.mLookMode = 0;
    fields.mTurnRatio = l_action_parameters.mTurnRatio;
    fields.mUseLookTarget = 1;
    setAnm_NUM(0, 1);
}

/* 022706E4 */
void daNpc_Kk1_c::move_CMT_TRN() {
    Kk1ActionFields& fields = Kk1_fields(this);
    s16 backAngle = (s16)(fields.mTurnStartAngle + 0x8000);
    s16 oldAngle = current.angle.y;
    if (cLib_calcTimer(&fields.mTurnTimer) == 0) {
        if (fields.mActionTimer == 0) {
            gabi::Local<cXyz> pnt;
            Kk1_pathPoint(pnt, &mPathRun);
            s16 target = cLib_targetAngleY(&current.pos, pnt);
            cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
            if (!not_talking((s8)fields.mEventState)) {
                return;
            }
            if (startEvent_check()) {
                fields.mEventState = 9;
                return;
            }
            if (current.angle.y != target) {
                return;
            }
            fields.mCommentAction = 0;
            setAnm_NUM(3, 1);
            fields.mOnPath = 1;
            return;
        }
        cLib_addCalcAngleS(&current.angle.y, backAngle, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        s16 angle = current.angle.y;
        if (angle == backAngle) {
            if (angle != oldAngle) {
                fields.mLookMode = 5;
                fields.mUseLookTarget = 1;
            }
            if (cLib_calcTimer(&fields.mActionTimer) == 0) {
                if (not_talking((s8)fields.mEventState)) {
                    gabi::Local<cXyz> pos;
                    copy_pos(pos, &current.pos);
                    if (chk_areaIN(l_action_parameters.mDetectionRadius, pos)) {
                        fields.mEventState = 8;
                    }
                }
                fields.mLookMode = 0;
                fields.mUseLookTarget = 1;
            }
        }
    }
    if (not_talking((s8)fields.mEventState) && startEvent_check()) {
        fields.mEventState = 9;
    }
}

/* 022708B8 */
void daNpc_Kk1_c::init_CMT_PCK() {
    Kk1ActionFields& fields = Kk1_fields(this);
    setAnm_NUM(1, 1);
    s16 evId = fields.mPickEventId;
    fields.mEventAction = 2;
    fields.mActionTimer = l_action_parameters.mPickTimer;
    fields.mLookMode = 0;
    fields.mUseLookTarget = 1;
    eventInfo.mEventId = evId; /* eventInfo.setEventId() */
}

/* 02270918 */
void daNpc_Kk1_c::move_CMT_PCK() {
    Kk1ActionFields& fields = Kk1_fields(this);
    if (fields.mActionTimer == 0) {
        gabi::Local<cXyz> pnt;
        Kk1_pathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        if (!not_talking((s8)fields.mEventState)) {
            return;
        }
        if (startEvent_check()) {
            fields.mEventState = 9;
            return;
        }
        if (current.angle.y != target) {
            return;
        }
        fields.mCommentAction = 0;
        setAnm_NUM(3, 1);
        fields.mOnPath = 1;
        return;
    }
    if (fields.mTalkRequested != 0) {
        return;
    }
    if (not_talking((s8)fields.mEventState) && chkHitPlayer()) {
        fields.mEventState = 1;
        return;
    }
    if (cLib_calcTimer(&fields.mActionTimer) == 0) {
        setAnm_NUM(0, 1);
        fields.mEventState = 0;
        eventInfo.mEventId = -1; /* eventInfo.setEventId(-1) */
        return;
    }
    Kk1_soundStart(0x509B, &current.pos);
    fields.mEventState = 2;
}

/* 02270AE0 */
BOOL daNpc_Kk1_c::cmmt_1() {
    Kk1ActionFields& fields = Kk1_fields(this);
    switch ((s8)fields.mCommentAction) {
    case 1:
        move_CMT_WAI();
        return TRUE;
    case 4:
        move_CMT_TRN();
        return TRUE;
    case 5:
        move_CMT_PCK();
        return TRUE;
    }
    if (not_talking((s8)fields.mEventState) && (s8)fields.mAnimationNo != 1 && startEvent_check()) {
        fields.mEventState = 9;
    }
    fields.mLookMode = 0;
    fields.mUseLookTarget = 1;
    if (event_move(true)) {
        if (!not_talking((s8)fields.mEventState)) {
            return TRUE;
        }
        switch ((s8)fields.mCommentAction) {
        case 1:
            init_CMT_WAI();
            break;
        case 4:
            init_CMT_TRN();
            break;
        case 5:
            init_CMT_PCK();
            break;
        default:
            fields.mCommentAction = 0;
            setAnm_NUM(3, 1);
            fields.mOnPath = 1;
            break;
        }
    }
    if (not_talking((s8)fields.mEventState)) {
        fields.mEventState = 0;
    }
    return TRUE;
}

/* 02270C7C */
BOOL daNpc_Kk1_c::wait_3() {
    Kk1ActionFields& fields = Kk1_fields(this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    bool far = std_sqrtf(PSVECSquareMag(xz)) > 300.0f;
    fields.mPlayerFar = far;
    if (far) {
        cLib_addCalcAngleS(&current.angle.y, fields.mHomeAngle.y, 4, 0x800, 0x80);
    }
    if (fields.mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(KK1_ACTION_TALK);
            fields.mUseLookTarget = 0;
            fields.mPlayerFar = 0;
            fields.mLookMode = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    fields.mLookMode = 0;
    fields.mUseLookTarget = 1;
    u8 attn = chk_attn();
    s8 state = (s8)fields.mEventState;
    if (attn) {
        fields.mLookMode = 1;
    }
    if (not_talking(state)) {
        fields.mEventState = 2;
    }
    return TRUE;
}

/* 02270DA8 */
BOOL daNpc_Kk1_c::wait_4() {
    Kk1ActionFields& fields = Kk1_fields(this);
    gabi::Local<cXyz> diff;
    cXyz_mi(&current.pos, diff, &linkPlayer()->current.pos);
    gabi::Local<cXyz> xz;
    xz->x = diff->x;
    xz->y = 0.0f;
    xz->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (fields.mTalkRequested != 0) {
        if (chk_talk()) {
            setStt(KK1_ACTION_TALK);
            setAnm_NUM(0, 1);
            fields.mUseLookTarget = 0;
            m_jnt.mbTrn = 1;
            fields.mLookMode = 1;
        }
        return TRUE;
    }
    fields.mEventState = 2;
    fields.mUseLookTarget = 0;
    fields.mLookMode = 1;
    bool far = dist > 300.0f;
    fields.mOnPath = far;
    if (far) {
        gabi::Local<cXyz> pnt;
        Kk1_pathPoint(pnt, &mPathRun);
        s16 target = cLib_targetAngleY(&current.pos, pnt);
        cLib_addCalcAngleS(&current.angle.y, target, l_action_parameters.mAngleScale, l_action_parameters.mAngleMaxStep, 0x80);
        if (abs_s16((s16)(target - current.angle.y)) < 0x1800) {
            setStt(KK1_ACTION_WALK);
            fields.mWaitingAtPathEnd = 0;
        }
        fields.mLookMode = 0;
        fields.mUseLookTarget = 1;
    }
    return TRUE;
}

/* 02270F28 */
BOOL daNpc_Kk1_c::talk_1() {
    Kk1ActionFields& fields = Kk1_fields(this);
    BOOL moving = chk_parts_notMov();
    talk(1);
    if (mbHasMsg == 0) {
        return TRUE;
    }
    // HD: the message status comes from the message manager
    if (fopMsgM_getStatus() == KK1_MSG_DESTROYED) {
        s8 stt = (s8)fields.mPreviousState;
        fields.mTalkRequested = 0;
        fields.mPresentedItem = 0xFF;
        setStt(stt);
        fields.mAfterTalkTimer = cLib_getRndValue(0xF, 0x1E);
        switch ((u32)mCurrMsgNo) {
        case 0x1CA9:
            dComIfGs_onEventBit(0xE10);
            endEvent();
            break;
        case 0x1CAB:
            fields.mEventState = 7;
            endEvent();
            break;
        case 0x1CAC:
            fields.mTalkLookTimer = 0;
            fields.mUseLookTarget = 0;
            fields.mLookMode = 1;
            endEvent();
            break;
        default:
            endEvent();
            break;
        }
    }
    if (cLib_calcTimer(&fields.mTalkLookTimer) != 0 && fields.mTalkLookTimer == 1) {
        fields.mUseLookTarget = 0;
        fields.mLookMode = 1;
    }
    return moving;
}

/* 02271084 */
BOOL daNpc_Kk1_c::wait_action1(void*) {
    Kk1ActionFields& fields = Kk1_fields(this);
    s8 step = (s8)fields.mActionStep;
    if (step == 0) {
        if (dKy_daynight_check() == 0) {
            fields.mWaitingAtPathEnd = 0;
            setStt(KK1_ACTION_WAIT);
        } else {
            setStt(KK1_ACTION_WAIT_PARTNER);
        }
        fields.mActionStep = fields.mActionStep + 1;
        return TRUE;
    }
    if ((u32)(s32)step > 3) {
        return TRUE;
    }
    fields.mAttentionSelected = chkAttention();
    switch ((u32)(s32)(s8)fields.mActionState) {
    case KK1_ACTION_WAIT: fields.mActionResult = wait_1(); break;
    case KK1_ACTION_TALK: fields.mActionResult = talk_1(); break;
    case KK1_ACTION_WALK: fields.mActionResult = walk_1(); break;
    case KK1_ACTION_WAIT_PARTNER: fields.mActionResult = wait_2(); break;
    case KK1_ACTION_COMMENT: fields.mActionResult = cmmt_1(); break;
    case KK1_ACTION_WAIT_NEAR: fields.mActionResult = wait_3(); break;
    case KK1_ACTION_WAIT_FAR: fields.mActionResult = wait_4(); break;
    }
    return TRUE;
}

/* WWHD glue
ext 0200F974 s16 Kk1_targetAngleX(const cXyz*, const cXyz*)
ext 025D5928 fopAc_ac_c* Kk1_fastCreate(s16, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32, void*)
ext 025163BC u32 Kk1_coHitObject(dCcD_GObjInf*)
ext 028E8DE8 f32 Kk1_squareDistance(const cXyz*, const cXyz*)
ext 025E19CC void Kk1_soundStart(u32, cXyz*)
ext 0259EC0C void Kk1_pathDecIdx(dNpc_PathRun_l*)
ext 0259ED58 u8 Kk1_pathNextIdx(dNpc_PathRun_l*)
ext 0259EDB8 u8 Kk1_pathMaxPoint(dNpc_PathRun_l*)
ext 0252624C s32 Kk1_shopCreateCount()
ext 0226CC2C fopAc_ac_c* daNpc_Kk1_c::searchByID(fpc_ProcID, be<s32>*)
ext 0226B5B4 void Kk1_pathPoint(cXyz*, dNpc_PathRun_l*)
*/
