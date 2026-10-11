/**
 * d_a_npc_ko1_d1.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part D1: setStt and the state functions wait_*, walk_*, swim_*, attk_*, down_1.
 */
#include "d/actor/d_a_npc_ko1_d1.h"

/* 02278134 */
void daNpc_Ko1_c::setStt(s8 i_status) {
    fopAc_ac_c* actor = Ko1_searchByID(this, mPartnerId);
    mTalkWaitTimer = 0;
    s8 oldStatus = mStatus;
    mStatus = i_status;
    // HD: preserve the sign-extended status for the switch, including invalid inputs.
    u32 status = (u32)(s32)i_status;
    switch (status) {
    case STATUS_WAIT_1:
    case STATUS_WAIT_2:
    case STATUS_WAIT_5:
    case STATUS_WAIT_5_ALT:
    case STATUS_WAIT_A:
        if (status == STATUS_WAIT_1) {
            mWaitTimer = (s16)gabi::ftoi(cM_rndF(90.0f) + 90.0f);
        } else if (status == STATUS_WAIT_2) {
            mBalloonTimer = (s16)((Ko1_counter().mCounter0 & 3) + 1);
        }
        if (oldStatus != STATUS_TALK) {
            m_jnt.mbTrn = 1;
            mTurnAngle = mInitialAngle.y;
            mLookTarget = 3;
            mbMoving = 0;
        }
        mLookMode = 0;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        Ko1_setAnm(this);
        break;
    case STATUS_TALK:
        mLookTarget = 1;
        mbMoving = 0;
        mLookMode = 0;
        m_jnt.mbTrn = 1;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        mAnimationSelector = 0xFF;
        mHeadAnimationSelector = 0xFF;
        mResumeStatus = oldStatus;
        Ko1_setAnm(this);
        break;
    case STATUS_WALK_1:
    case STATUS_WALK_3: {
        if (status == STATUS_WALK_1) {
            mWaitTimer = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        } else if (status == STATUS_WALK_3) {
            if (mSavedPath != 0) {
                mPathRun.setInfDrct(Ko1_savedPath(mSavedPath));
                mPathRun.setNearPathIndx(&current.pos, 0.0f);
                mSavedPath = 0;
            }
        }
        gabi::Local<cXyz> pnt;
        mPathRun.getPoint(pnt, mPathRun.mIdx);
        mTargetPos.copy(*pnt);
        mLookTarget = 0;
        mbMoving = 1;
        mLookMode = 0;
        mMoveMode = 1;
        mbMoveFinished = 0;
        gravity = -4.5f;
        mMoveSpeed = Ko1_hioChildren()[mType].mWalkSpeed;
        mMoveAcceleration = Ko1_hioChildren()[mType].mWalkAcceleration;
        mTurnRate = Ko1_hioChildren()[mType].mWalkTurnRate;
        Ko1_setAnm(this);
        break;
    }
    case STATUS_WAIT_3:
        mLookTarget = 0;
        mbMoving = 0;
        mLookMode = 0;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        Ko1_setAnm(this);
        break;
    case STATUS_ATTACK_1:
    case STATUS_ATTACK_2:
    case STATUS_ATTACK_2_ALT: {
        if (status == STATUS_ATTACK_1 && mPathRun.mPath.get() != nullptr) {
            mSavedPath = gabi::ea(mPathRun.mPath.get());
            mPathRun.setInfDrct(nullptr);
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        mTargetPos.copy(player->current.pos);
        mLookTarget = 1;
        mbMoving = 1;
        mLookMode = 0;
        mMoveMode = 2;
        mbMoveFinished = 0;
        gravity = -4.5f;
        mMoveSpeed = Ko1_hioChildren()[mType].mRunSpeed;
        mMoveAcceleration = Ko1_hioChildren()[mType].mRunAcceleration;
        mTurnRate = Ko1_hioChildren()[mType].mRunTurnRate;
        Ko1_setAnm(this);
        break;
    }
    case STATUS_SWIM_1: {
        if (mPathRun.mPath.get() != nullptr) {
            mSavedPath = gabi::ea(mPathRun.mPath.get());
            mPathRun.setInfDrct(nullptr);
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        mTargetPos.copy(player->current.pos);
        mLookTarget = 1;
        mbMoving = 1;
        mVerticalSpeed = -4.0f;
        mMoveAcceleration = 0.0f;
        mMoveSpeed = 0.0f;
        mLookMode = 0;
        mMoveMode = 3;
        mbMoveFinished = 0;
        speed.y = -4.0f;
        speedF = 0.0f;
        gravity = 0.0f;
        mTurnRate = Ko1_hioChildren()[mType].mSwimTurnRate;
        Ko1_setAnm(this);
        break;
    }
    case STATUS_SWIM_2: {
        if (mSavedPath != 0) {
            mPathRun.setInfDrct(Ko1_savedPath(mSavedPath));
            mPathRun.setNearPathIndx(&current.pos, 0.0f);
            mSavedPath = 0;
        }
        gabi::Local<cXyz> pnt;
        mPathRun.getPoint(pnt, mPathRun.mIdx);
        mTargetPos.copy(*pnt);
        mLookTarget = 0;
        mbMoving = 1;
        mLookMode = 0;
        mMoveMode = 3;
        speed.y = -4.0f;
        mVerticalSpeed = -4.0f;
        mMoveAcceleration = 0.0f;
        speedF = 0.0f;
        gravity = 0.0f;
        mbMoveFinished = 0;
        mMoveSpeed = 0.0f;
        mTurnRate = Ko1_hioChildren()[mType].mSwimTurnRate;
        Ko1_setAnm(this);
        break;
    }
    case STATUS_DOWN_1:
    case STATUS_DOWN_2:
    case STATUS_DOWN_3:
    case STATUS_DOWN_ALT:
        mLookTarget = 0;
        mbMoving = 0;
        speed.y = 10.0f;
        mLookMode = 0;
        mPreviousMoveMode = mMoveMode;
        mMoveMode = 4;
        speedF = -3.0f;
        gravity = -1.6f;
        mbJumping = 1;
        mMoveAcceleration = 0.1f;
        Ko1_setAnm(this);
        break;
    case STATUS_WAIT_4:
    case STATUS_WAIT_6:
    case STATUS_WAIT_9:
        mLookTarget = 1;
        mbMoving = 0;
        mLookMode = 0;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        Ko1_setAnm(this);
        break;
    case STATUS_WALK_2:
    case STATUS_WALK_2_ALT:
        mTargetPos.copy(mInitialPos);
        gravity = -4.5f;
        mLookTarget = 0;
        mbMoving = 1;
        mLookMode = 0;
        mMoveMode = 1;
        mbMoveFinished = 0;
        mMoveSpeed = Ko1_hioChildren()[mType].mWalkSpeed;
        mMoveAcceleration = Ko1_hioChildren()[mType].mWalkAcceleration;
        mTurnRate = Ko1_hioChildren()[mType].mWalkTurnRate;
        Ko1_setAnm(this);
        break;
    case STATUS_ATTACK_3:
        if (actor == nullptr)
            JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0xA77, "a_partner != (0)");
        mTargetPos.copy(actor->current.pos);
        mLookTarget = 2;
        mLookPos.copy(actor->current.pos);
        mLookPos.y = actor->eyePos.y;
        mbMoving = 1;
        mLookMode = 0;
        mMoveMode = 2;
        mbMoveFinished = 0;
        gravity = -4.5f;
        mMoveSpeed = Ko1_hioChildren()[mType].mRunSpeed;
        mMoveAcceleration = Ko1_hioChildren()[mType].mRunAcceleration;
        mTurnRate = Ko1_hioChildren()[mType].mRunTurnRate;
        Ko1_setAnm(this);
        break;
    case STATUS_WAIT_7:
        if (actor != nullptr) {
            mLookTarget = 2;
            mLookPos.copy(actor->current.pos);
            mLookPos.y = actor->eyePos.y;
            mbMoving = 0;
            mLookMode = 0;
            mMoveMode = 0;
        } else {
            mLookTarget = 0;
            mbMoving = 0;
            mLookMode = 0;
            mMoveMode = 0;
        }
        Ko1_clrSpd(this);
        Ko1_setAnm(this);
        break;
    case STATUS_MANZAI:
        mAnimationSelector = 0xFF;
        mHeadAnimationSelector = 0xFF;
        mResumeStatus = oldStatus;
        Ko1_setAnm(this);
        break;
    case STATUS_SLEEP_1:
    case STATUS_SLEEP_2:
        mWaitTimer = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        mBalloonTimer = (s16)Ko1_getRndValue(3, 10);
        mSleepState = 0;
        Ko1_setAnm(this);
        break;
    default:
        Ko1_setAnm(this);
        break;
    }
}

/* 0227896C */
BOOL daNpc_Ko1_c::wait_1() {
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    mTurnAngle = mInitialAngle.y;
    mLookMode = 2;
    mLookTarget = 3;
    if (mAnimationNum == 6) {
        if ((s8)mAnimationEnded != 0) {
            Ko1_calcTimer(&mBalloonTimer);
        }
        if (mBalloonTimer == 0 || mAttention != 0) {
            mBalloonTimer = (s16)((Ko1_counter().mCounter0 & 3) + 1);
            mWaitTimer = (s16)gabi::ftoi(cM_rndF(90.0f) + 90.0f);
            Ko1_setAnm_NUM(this, 0, 1);
        }
    } else {
        if (mAttention != 0) {
            mTalkWaitTimer = 0x3C;
        }
        if (Ko1_calcTimer(&mTalkWaitTimer)) {
            mLookTarget = 1;
            return TRUE;
        }
        if (!Ko1_calcTimer(&mWaitTimer)) {
            Ko1_setAnm_NUM(this, 6, 1);
        }
        m_jnt.mbTrn = 1;
    }
    return TRUE;
}

/* 02278AE0 */
BOOL daNpc_Ko1_c::wait_2() {
    if ((s8)mAnimationEnded != 0) {
        Ko1_calcTimer(&mBalloonTimer);
        if (mBalloonTimer == 0 || mAttention != 0 || mTalkFlag != 0) {
            setStt(STATUS_WAIT_1);
        }
    }
    return TRUE;
}

/* 02278B50 */
BOOL daNpc_Ko1_c::wait_3() {
    if ((s8)mAnimationEnded != 0) {
        setStt(STATUS_WALK_1);
    }
    return TRUE;
}

/* 02278B84 */
BOOL daNpc_Ko1_c::wait_4() {
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    if (Ko1_chk_start_swim(this)) {
        setStt(STATUS_SWIM_1);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(current.pos);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWaitAreaRadius, pos)) {
        mLookMode = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    pos2->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mSwimAreaRadius, pos2)) {
        setStt(STATUS_WALK_3);
    } else {
        setStt(STATUS_ATTACK_1);
    }
    return TRUE;
}

/* 02278CB4 */
BOOL daNpc_Ko1_c::wait_5(s8 i_nextStatus) {
    if (mTalkState == 1) {
        mTalkState = 2;
        setStt(STATUS_MANZAI);
        mLookTarget = 1;
        mbMoving = 0;
        mLookMode = 0;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mNearAreaRadius, pos)) {
        setStt(i_nextStatus);
    }
    return TRUE;
}

/* 02278D7C */
BOOL daNpc_Ko1_c::wait_6() {
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this) && Ko1_chk_manzai_1(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    if (mTalkState == 1) {
        mTalkState = 2;
        setStt(STATUS_MANZAI);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(current.pos);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWaitAreaRadius, pos)) {
        mLookMode = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    pos2->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mOuterAreaRadius, pos2)) {
        setStt(STATUS_WALK_2);
    } else {
        setStt(STATUS_ATTACK_2);
    }
    return TRUE;
}

/* 02278EC0 */
BOOL daNpc_Ko1_c::wait_7() {
    fopAc_ac_c* actor = Ko1_searchByID(this, mPartnerId);
    if (actor == nullptr)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0xB6F, "a_partner != (0)");
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this) && Ko1_chk_manzai_1(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    if (mTalkState == 1) {
        mTalkState = 2;
        setStt(STATUS_MANZAI);
        mLookTarget = 1;
        mbMoving = 0;
        mLookMode = 0;
        mMoveMode = 0;
        Ko1_clrSpd(this);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(current.pos);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWaitAreaRadius, pos)) {
        mLookTarget = 1;
    } else {
        mLookTarget = 2;
        mLookPos.copy(actor->current.pos);
        mLookPos.y = actor->eyePos.y;
    }
    /* distance in XZ from this actor to the partner */
    cXyz_mi(&actor->current.pos, pos, &current.pos);
    gabi::Local<cXyz> xz;
    xz->x = pos->x;
    xz->y = 0.0f;
    xz->z = pos->z;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    if (dist < Ko1_hioChildren()[mType].mWaitAreaRadius) {
        mLookMode = 2;
    } else {
        setStt(STATUS_ATTACK_3);
    }
    return TRUE;
}

/* 022790E4 */
BOOL daNpc_Ko1_c::wait_9() {
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(current.pos);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWaitAreaRadius, pos)) {
        mLookMode = 2;
        return TRUE;
    }
    gabi::Local<cXyz> pos2;
    pos2->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mOuterAreaRadius, pos2)) {
        setStt(STATUS_WALK_2_ALT);
    } else {
        setStt(STATUS_ATTACK_2_ALT);
    }
    return TRUE;
}

/* 022791F4 */
BOOL daNpc_Ko1_c::wait_a() {
    fopAc_ac_c* actor = Ko1_searchByID(this, mPartnerId);
    if (actor == nullptr)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0xBC7, "a_partner != (0)");
    if (mTalkFlag != 0) {
        if (Ko1_chk_talk(this) && Ko1_chk_manzai_1(this)) {
            setStt(STATUS_TALK);
        }
        return TRUE;
    }
    if (mTalkState == 1) {
        mTalkState = 2;
        setStt(STATUS_MANZAI);
        return TRUE;
    }
    mLookMode = 2;
    if (mAttention != 0) {
        mTalkWaitTimer = 0x3C;
    }
    if (Ko1_calcTimer(&mTalkWaitTimer)) {
        mLookTarget = 1;
        return TRUE;
    }
    mLookTarget = 3;
    mTurnAngle = mInitialAngle.y;
    m_jnt.mbTrn = 1;
    return TRUE;
}

/* 02279300 */
BOOL daNpc_Ko1_c::walk_1() {
    mbMoveFinished = 0;
    gabi::Local<cXyz> pnt;
    mPathRun.getPoint(pnt, mPathRun.mIdx);
    mTargetPos.copy(*pnt);
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWalkAreaRadius, pos)) {
        setStt(STATUS_ATTACK_1);
    } else if (!Ko1_calcTimer(&mWaitTimer)) {
        setStt(STATUS_WAIT_3);
    }
    return TRUE;
}

/* 022793C8 */
BOOL daNpc_Ko1_c::walk_2(s8 i_status1, s8 i_status2) {
    if (mbMoveFinished != 0) {
        setStt(i_status1);
        return TRUE;
    }
    if (mTalkState == 1) {
        setStt(STATUS_WAIT_6);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mNearAreaRadius, pos)) {
        setStt(i_status2);
        return TRUE;
    }
    mTargetPos.copy(mInitialPos);
    return TRUE;
}

/* 0227949C */
BOOL daNpc_Ko1_c::walk_3() {
    if (mbMoveFinished != 0) {
        setStt(STATUS_WALK_1);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWalkAreaRadius, pos)) {
        setStt(STATUS_ATTACK_1);
        return TRUE;
    }
    gabi::Local<cXyz> pnt;
    mPathRun.getPoint(pnt, mPathRun.mIdx);
    mTargetPos.copy(*pnt);
    if (Ko1_chk_start_swim(this)) {
        setStt(STATUS_SWIM_2);
    }
    return TRUE;
}

/* 02279578 */
BOOL daNpc_Ko1_c::swim_1() {
    f32 depth = mObjAcch.m_wtr.GetHeight() - current.pos.y;
    if (depth < 49.0f) {
        mVerticalSpeed = -4.0f;
        if (mbMoveFinished == 0) {
            speedF = 8.0f;
        }
    } else if (depth > 58.0f) {
        mVerticalSpeed = 4.0f;
    }
    if (!Ko1_chk_start_swim(this)) {
        setStt(STATUS_ATTACK_1);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mSwimAreaRadius, pos)) {
        setStt(STATUS_SWIM_2);
        return TRUE;
    }
    mbMoveFinished = 0;
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    mTargetPos.copy(player->current.pos);
    return TRUE;
}

/* 022796B4 */
BOOL daNpc_Ko1_c::swim_2() {
    f32 depth = mObjAcch.m_wtr.GetHeight() - current.pos.y;
    if (depth < 49.0f) {
        mVerticalSpeed = -4.0f;
        if (mbMoveFinished == 0) {
            speedF = 8.0f;
        }
    } else if (depth > 58.0f) {
        mVerticalSpeed = 4.0f;
    }
    if (!Ko1_chk_start_swim(this)) {
        setStt(STATUS_WALK_3);
        return TRUE;
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mWalkAreaRadius, pos)) {
        setStt(STATUS_SWIM_1);
        return TRUE;
    }
    mbMoveFinished = 0;
    gabi::Local<cXyz> pnt;
    mPathRun.getPoint(pnt, mPathRun.mIdx);
    mTargetPos.copy(*pnt);
    return TRUE;
}

/* 022797FC */
BOOL daNpc_Ko1_c::attk_1() {
    mbMoveFinished = 0;
    u32 id = Ko1_get_crsActorID(this);
    mAttackActorId = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* actor = Ko1_searchByID(this, id);
        if (actor != nullptr && fpcM_GetName(actor) == fpcNm_ITEM_e) {
            setStt(STATUS_DOWN_1);
            return TRUE;
        }
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mSwimAreaRadius, pos)) {
        setStt(STATUS_WALK_3);
        return TRUE;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> ppos;
    ppos->set(player->current.pos);
    Ko1_set_tgtPos(this, pos, ppos);
    mTargetPos.copy(*pos);
    if (Ko1_chk_start_swim(this)) {
        setStt(STATUS_SWIM_1);
    }
    return TRUE;
}

/* 0227992C */
BOOL daNpc_Ko1_c::attk_2(s8 i_status1, s8 i_status2) {
    mbMoveFinished = 0;
    if (mTalkState == 1) {
        setStt(STATUS_WAIT_6);
        return TRUE;
    }
    u32 id = Ko1_get_crsActorID(this);
    mAttackActorId = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* actor = Ko1_searchByID(this, id);
        if (actor != nullptr && fpcM_GetName(actor) == fpcNm_ITEM_e) {
            setStt(i_status1);
            return TRUE;
        }
    }
    gabi::Local<cXyz> pos;
    pos->set(mAreaCenter);
    if (!Ko1_chk_areaIn(this, Ko1_hioChildren()[mType].mOuterAreaRadius, pos)) {
        setStt(i_status2);
        return TRUE;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    mTargetPos.copy(player->current.pos);
    return TRUE;
}

/* 02279A58 */
BOOL daNpc_Ko1_c::attk_3() {
    fopAc_ac_c* actor = Ko1_searchByID(this, mPartnerId);
    if (actor == nullptr)
        JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0xCD6, "a_partner != (0)");
    mbMoveFinished = 0;
    if (mTalkState == 1) {
        setStt(STATUS_WAIT_7);
        return TRUE;
    }
    u32 id = Ko1_get_crsActorID(this);
    mAttackActorId = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* target = Ko1_searchByID(this, id);
        if (target != nullptr && (fpcM_GetName(target) == KO1_ATTACK_TARGET_PROCESS_e || fpcM_GetName(target) == fpcNm_ITEM_e)) {
            setStt(STATUS_DOWN_3);
            return TRUE;
        }
    }
    gabi::Local<cXyz> apos;
    apos->x = actor->current.pos.x;
    apos->y = actor->current.pos.y;
    apos->z = actor->current.pos.z;
    gabi::Local<cXyz> tgt;
    Ko1_set_tgtPos(this, tgt, apos);
    mTargetPos.copy(*tgt);
    mLookPos.copy(*tgt);
    mLookPos.y = actor->eyePos.y;
    return TRUE;
}

/* 02279B90 */
BOOL daNpc_Ko1_c::down_1(s8 i_status) {
    if (mbMoveFinished != 0) {
        setStt(i_status);
    }
    return TRUE;
}

/* WWHD glue
ext 022770F0 fopAc_ac_c* Ko1_searchByID(daNpc_Ko1_c* actor, u32 id)
ext 02277070 bool Ko1_chk_talk(daNpc_Ko1_c* actor)
ext 02277124 s32 Ko1_chk_manzai_1(daNpc_Ko1_c* actor)
ext 02277DEC bool Ko1_chk_start_swim(daNpc_Ko1_c* actor)
ext 02277EC0 u32 Ko1_get_crsActorID(daNpc_Ko1_c* actor)
ext 02277F58 bool Ko1_chk_areaIn(daNpc_Ko1_c* actor, f32 r, cXyz* pos)
ext 02277C44 void Ko1_set_tgtPos(daNpc_Ko1_c* actor, cXyz* out, cXyz* pos)
ext 0227810C void Ko1_clrSpd(daNpc_Ko1_c* actor)
ext 02276D84 void Ko1_setAnm(daNpc_Ko1_c* actor)
ext 02276D18 void Ko1_setAnm_NUM(daNpc_Ko1_c* actor, s32 num, s32 tex)
ext 02055B64 s16 Ko1_calcTimer(be<s16>* timer)
ext 021E1E78 s32 Ko1_getRndValue(s32 base, s32 range)
str 1001CCBC "d_a_npc_ko1.cpp" in 02278134
str 1001CCCC "a_partner != (0)" in 02278134
str 1001CCE0 "d_a_npc_ko1.cpp" in 02278EC0
str 1001CCF0 "a_partner != (0)" in 02278EC0
str 1001CD04 "d_a_npc_ko1.cpp" in 022791F4
str 1001CD14 "a_partner != (0)" in 022791F4
str 1001CD38 "d_a_npc_ko1.cpp" in 02279A58
str 1001CD48 "a_partner != (0)" in 02279A58
*/
