/**
 * d_a_npc_ko1_d2.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part D2
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part D2: talk_1/talk_2/manzai/neru_*, the action functions (hana_action*, wait_action*) and the
 * compiler-generated tail of the translation unit (HIO constructors, __sinit, destructors).
 */
#include "d/actor/d_a_npc_ko1_d2.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1001C87C,
    .objAcchDt = {0x1001C8F4, 0x1001C904},
    .headerStatics = {0x104677B4, 0x101BFD24, 0x104677C0},
};

static const u8 a_prm_tbl[2][0x58] = {
    {32, 0, 31, 64, 224, 192, 224, 192, 0, 0, 31, 64, 0, 0, 224, 192, 7, 208, 6, 64, 66, 180, 0, 0, 0, 0, 0, 4, 8, 0, 0, 0, 63, 102, 102, 102, 63, 128, 0, 0, 63, 0, 0, 0, 65, 168, 0, 0, 61, 204, 204, 205, 65, 32, 0, 0, 62, 153, 153, 154, 66, 32, 0, 0, 66, 160, 0, 0, 67, 72, 0, 0, 67, 150, 0, 0, 67, 22, 0, 0, 68, 137, 128, 0, 68, 187, 128, 0},
    {32, 0, 31, 64, 224, 192, 224, 192, 0, 0, 31, 64, 0, 0, 224, 192, 7, 208, 6, 64, 66, 180, 0, 0, 0, 0, 0, 6, 8, 0, 0, 0, 62, 153, 153, 154, 64, 192, 0, 0, 62, 204, 204, 205, 65, 168, 0, 0, 61, 204, 204, 205, 65, 64, 0, 0, 63, 192, 0, 0, 66, 200, 0, 0, 0, 0, 0, 0, 67, 175, 0, 0, 67, 250, 0, 0, 67, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/* 02279BC0 */
BOOL daNpc_Ko1_c::talk_1() {
    u32 msgMgr = gabi::ea(fopNpc_npc_c::mpCurrMsg.get());
    BOOL ret = Ko1_chk_partsNotMove(this); /* chk_partsNotMove() */
    u16 status = 0;
    if (mbHasMsg) {
        status = (u16)Ko1_messageStatus(msgMgr);
    }
    mMsgStatus = status;
    if ((u8)mTalkState == 2 && mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        // HD: the first speaker of a two-NPC conversation opens the message itself
        mCurrMsgNo = Ko1_getMsgVirtual(this);
        Ko1TalkActorSlots* actors = Ko1_talkActorSlots(dComIfGp_ea() + 0x5C34);
        fopAc_ac_c* talkActor = Ko1_actorPointer(actors->mIndexAndActors[1]);
        u32 id = Ko1_messageSet(msgMgr, mCurrMsgNo, &talkActor->eyePos);
        mbHasMsg = 0;
        mCurrMsgBsPcId = id;
        return ret;
    }
    mPartnerMsgNo = 0xFFFFFFFF;
    talk(1);
    if (mbHasMsg && Ko1_messageStatus(msgMgr) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
        switch (mCurrMsgNo) {
        case 0xAF1:
            dComIfGs_onEventBit(0x0210);
            break;
        case 0xAFF:
            dComIfGs_onEventBit(0x3801);
            break;
        case 0xB06:
            dComIfGs_onEventBit(0x3340);
            break;
        case 0xB55:
            dComIfGs_onEventBit(0x0220);
            break;
        case 0xB57:
            dComIfGs_onEventBit(0x0208);
            dComIfGs_onEventBit(0x0240);
            break;
        case 0xB62:
            dComIfGs_onEventBit(0x3101);
            break;
        case 0xB64:
            dComIfGs_onEventBit(0x2C04);
            mbTalkTriggered = 1;
            break;
        }
        mTalkFlag = 0;
        mMessageSelector = 0xFF;
        switch (mSpecificType) {
        case 1:
        case 3:
        case 6:
        case 7:
            Ko1_setStt(this, (s32)STATUS_TALK_END); /* setStt(0x15) */
            return ret;
        }
        Ko1_setStt(this, (s32)mResumeStatus); /* setStt(mResumeStatus) */
        mTalkWaitTimer = 60;
        Ko1_endEvent(this); /* endEvent() */
    }
    return ret;
}

/* 02279EF8 */
BOOL daNpc_Ko1_c::talk_2() {
    /* the partners (process ids at 0x924, mPartners.mMetadata.mCount of them) still talking are told to end */
    u32 nIdle = 0;
    s32 i = 0;
    u32 n;
    while ((s32)i < (s32)(n = mPartners.mMetadata.mCount)) {
        fopNpc_npc_c* partner = Ko1_searchByID(this, (u32)mPartners.mIds[i]);
        // HD: assert text is "(0) != a_actor".
        if (partner == nullptr)
            JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0xD5F, "partner != NULL");
        if ((u8)partner->mTalkState != 0) {
            partner->mTalkState = 3;
            i++;
        } else {
            i++;
            nIdle++;
        }
    }
    if (nIdle == n) {
        mTalkState = 0;
        Ko1_setStt(this, (s32)mResumeStatus); /* setStt(mResumeStatus) */
        Ko1_endEvent(this);          /* endEvent() */
    }
    return TRUE;
}

/* 02279FC0 */
BOOL daNpc_Ko1_c::manzai() {
    Ko1TalkActorSlots* actors = Ko1_talkActorSlots(dComIfGp_ea() + 0x5C34);
    switch ((u8)mTalkState) {
    case 2: {
        fopNpc_npc_c* partner = Ko1_searchByID(this, (u32)mPartnerProcId);
        /* play + 0x5C34: index into the talk actor list (HD) */
        u32 idx = actors->mIndexAndActors[0];
        if (gabi::ea(this) != (u32)actors->mIndexAndActors[idx]) {
            /* the partner speaks: follow its message status */
            if (mAnimationSelector == 0xFF) {
                return TRUE;
            }
            if (mSpecificType == 1 || mSpecificType == 6) {
                mAnimationMode = 1;
                mAnimationSelector = 0xFF;
                m_jnt.mbTrn = 1;
                return TRUE;
            }
            s8 prev = mResumeStatus;
            mAnimationMode = 3;
            mTurnAngle = mInitialAngle.y;
            mStatus = prev;
            m_jnt.mbTrn = 1;
            Ko1_setAnm(this); /* setAnm() */
            mStatus = 0x14;
            mAnimationSelector = 0xFF;
            return TRUE;
        }
        if (partner != nullptr) {
            mPartnerMsgNo = partner->mCurrMsgNo;
            Ko1_anmAtrVirtual(this, (u16)partner->mMsgStatus);
        }
        return TRUE;
    }
    case 3:
        actor_status &= ~fopAcStts_UNK4000_e;
        Ko1_setStt(this, (s32)mResumeStatus); /* setStt(mResumeStatus) */
        mTalkState = 0;
        return TRUE;
    default:
        return TRUE;
    }
}

/* 0227A110 */
BOOL daNpc_Ko1_c::neru_1() {
    s32 a_anm_num_tbl[2] = {7, 8};
    if (Ko1_calcTimer(&mSleepTimer) == 0) {
        u8 n = mSleepState ^ 1;
        mSleepState = n;
        Ko1_setAnm_NUM(this, a_anm_num_tbl[n & 1], (s32)1); /* setAnm_NUM(.., 1) */
        mSleepTimer = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
    }
    return TRUE;
}

/* 0227A1C8 */
BOOL daNpc_Ko1_c::neru_2() {
    s32 a_anm_num_tbl[3] = {9, 0xC, 0xA};
    switch (mSleepState) {
    case 0:
        if (Ko1_calcTimer(&mSleepTimer) == 0) {
            if (Ko1_calcTimer(&mBalloonTimer) == 0) {
                mSleepState = 1;
                Ko1_setAnm_NUM(this, a_anm_num_tbl[1], (s32)1);
                Ko1_set_balloonAnm_NUM(this, (s32)1);
                mBalloonTimer = (s16)Ko1_getRndValue(3, 10);
            }
            mSleepTimer = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        }
        return TRUE;
    case 1:
        if ((s8)mAnimationEnded != 0) {
            mSleepState = 2;
            Ko1_setAnm_NUM(this, a_anm_num_tbl[2], (s32)1);
        } else if (mpMorf->mFrameCtrl.checkPass(2.0f)) {
            Ko1_setPrtcl_HanaPachi(this); /* setPrtcl_HanaPachi() */
            mDoAud_seStart(0x592C, &eyePos, 0, dComIfGp_getReverb(current.roomNo));
        }
        return TRUE;
    case 2:
        if ((s8)mAnimationEnded != 0) {
            mSleepState = 0;
            Ko1_setAnm_NUM(this, a_anm_num_tbl[0], (s32)1);
            Ko1_set_balloonAnm_NUM(this, (s32)0);
        }
        return TRUE;
    default:
        return TRUE;
    }
}

/* the action functions: mActionPhase is the phase (0: init, 1..3: run, else (e.g. 9 on exit): nothing);
 * mStatus is the status set by setStt; mStatusResult keeps the status function's result */
/* 0227A374 */
BOOL daNpc_Ko1_c::hana_action1(void*) {
    if (mActionPhase == 0) {
        Ko1_ko_setPthPos(this); /* ko_setPthPos() */
        Ko1_setStt(this, (s32)STATUS_WALK_1);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) {
        return TRUE;
    }
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_WALK_1:
        mStatusResult = Ko1_walk_1(this);
        break;
    case STATUS_WAIT_3:
        mStatusResult = Ko1_wait_3(this);
        break;
    case STATUS_ATTACK_1:
        mStatusResult = Ko1_attk_1(this);
        break;
    case STATUS_SWIM_1:
        mStatusResult = Ko1_swim_1(this);
        break;
    case STATUS_SWIM_2:
        mStatusResult = Ko1_swim_2(this);
        break;
    case STATUS_DOWN_1:
        mStatusResult = Ko1_down_1(this, (s32)10);
        break;
    case STATUS_WAIT_4:
        mStatusResult = Ko1_wait_4(this);
        break;
    case STATUS_WALK_3:
        mStatusResult = Ko1_walk_3(this);
        break;
    }
    Ko1_lookBack(this); /* lookBack() */
    return TRUE;
}

/* 0227A4DC */
BOOL daNpc_Ko1_c::hana_action2(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_7);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_ATTACK_3:
        mStatusResult = Ko1_attk_3(this);
        break;
    case STATUS_DOWN_3:
        mStatusResult = Ko1_down_1(this, (s32)0x13);
        break;
    case STATUS_WAIT_7:
        mStatusResult = Ko1_wait_7(this);
        break;
    case STATUS_MANZAI:
        mStatusResult = manzai();
        break;
    case STATUS_TALK_END:
        mStatusResult = talk_2();
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227A60C */
BOOL daNpc_Ko1_c::hana_action3(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_5_ALT);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_WAIT_5_ALT:
        mStatusResult = Ko1_wait_5(this, (s32)0x17);
        break;
    case STATUS_ATTACK_2_ALT:
        mStatusResult = Ko1_attk_2(this, (s32)0x19, (s32)0x18);
        break;
    case STATUS_WALK_2_ALT:
        mStatusResult = Ko1_walk_2(this, (s32)0x16, (s32)0x17);
        break;
    case STATUS_DOWN_ALT:
        mStatusResult = Ko1_down_1(this, (s32)0x1A);
        break;
    case STATUS_WAIT_9:
        mStatusResult = Ko1_wait_9(this);
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227A750 */
BOOL daNpc_Ko1_c::hana_action4(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_A);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_MANZAI:
        mStatusResult = manzai();
        break;
    case STATUS_TALK_END:
        mStatusResult = talk_2();
        break;
    case STATUS_WAIT_A:
        mStatusResult = Ko1_wait_a(this);
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227A830 */
BOOL daNpc_Ko1_c::hana_action5(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_SLEEP_2);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    if (mStatus == STATUS_SLEEP_2) {
        mStatusResult = neru_2();
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227A8C8 */
BOOL daNpc_Ko1_c::wait_action1(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_1);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) {
        return TRUE;
    }
    if (!dComIfGs_isEventBit(0x0104)) {
        Ko1_check_landOn(this); /* check_landOn() */
    }
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_WAIT_1:
        mStatusResult = Ko1_wait_1(this);
        break;
    case STATUS_WAIT_2:
        mStatusResult = Ko1_wait_2(this);
        break;
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227A9C0 */
BOOL daNpc_Ko1_c::wait_action2(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_5);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_WAIT_5:
        mStatusResult = Ko1_wait_5(this, (s32)0xD);
        break;
    case STATUS_ATTACK_2:
        mStatusResult = Ko1_attk_2(this, (s32)0x10, (s32)0xF);
        break;
    case STATUS_WAIT_6:
        mStatusResult = Ko1_wait_6(this);
        break;
    case STATUS_WALK_2:
        mStatusResult = Ko1_walk_2(this, (s32)0xC, (s32)0xD);
        break;
    case STATUS_DOWN_2:
        mStatusResult = Ko1_down_1(this, (s32)0xE);
        break;
    case STATUS_MANZAI:
        mStatusResult = manzai();
        break;
    case STATUS_TALK_END:
        mStatusResult = talk_2();
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227AB28 */
BOOL daNpc_Ko1_c::wait_action3(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_WAIT_A);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    switch ((u32)(s32)mStatus) {
    case STATUS_TALK:
        mStatusResult = talk_1();
        break;
    case STATUS_MANZAI:
        mStatusResult = manzai();
        break;
    case STATUS_TALK_END:
        mStatusResult = talk_2();
        break;
    case STATUS_WAIT_A:
        mStatusResult = Ko1_wait_a(this);
        break;
    }
    Ko1_lookBack(this);
    return TRUE;
}

/* 0227AC08 */
BOOL daNpc_Ko1_c::wait_action4(void*) {
    if (mActionPhase == 0) {
        Ko1_setStt(this, (s32)STATUS_SLEEP_1);
        mActionPhase = mActionPhase + 1;
        return TRUE;
    }
    if ((u32)(s32)mActionPhase > 3) return TRUE;
    mAttention = (u8)Ko1_chkAttention(this);
    if (mStatus == STATUS_SLEEP_1) {
        mStatusResult = neru_1();
    }
    Ko1_lookBack(this);
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

// daNpc_Ko1_childHIO_c::daNpc_Ko1_childHIO_c (HD: allocates when this == NULL)
/* 0227ACA0 */
static daNpc_Ko1_childHIO_c* daNpc_Ko1_childHIO_c_ct(daNpc_Ko1_childHIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ko1_childHIO_c*)operator_new(0x60);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C954;
    return i_this;
}

// daNpc_Ko1_HIO_c::daNpc_Ko1_HIO_c (HD: allocates when this == NULL)
/* 0227ACE0 */
static daNpc_Ko1_HIO_c* daNpc_Ko1_HIO_c_ct(daNpc_Ko1_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpc_Ko1_HIO_c*)operator_new(0xCC);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1001C964;
    __construct_array(i_this->mChild, 2, 0x60, gabi::guest_fn((const void*)&daNpc_Ko1_childHIO_c_ct));
    for (int i = 0; i < 2; i++) {
        i_this->mChild[i].mNo = i;
        memcpy_g(i_this->mChild[i].mPrm, a_prm_tbl[i], 0x58);
    }
    i_this->mInitState = -1;
    i_this->mNo = -1;
    return i_this;
}

// static initialisation of the translation unit
/* 0227AD90 */
static void __sinit_d_a_npc_ko1_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daNpc_Ko1_HIO_c_ct(Ko1_hioStorage());
}

// HD: this TU's SafeString deleting destructor.
/* 0227AE30 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

// HD: deleting destructor destroys only the inherited NPC members.
/* 0227AE44 */
static void daNpc_Ko1_c_dt(daNpc_Ko1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Ko1_circleDtor(&i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        Ko1_acchDtor(&i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        Ko1_actorDtor(i_this, 0);             /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// sead::SafeString::assureTerminationImpl_ (this TU's copy; empty)
/* 0227AEE0 */
static void SafeString_assureTerminationImpl(SafeString*) {
}

/* WWHD glue (tools/verify/autoglue.py)
data a_prm_tbl 101BFC74
str 1001CD5C "d_a_npc_ko1.cpp"
str 1001CD6C "partner != NULL" renamed
ext 02275738 void Ko1_endEvent(daNpc_Ko1_c*)
ext 022758B8 void Ko1_lookBack(daNpc_Ko1_c*)
ext 02276D18 void Ko1_setAnm_NUM(daNpc_Ko1_c*, s32, s32)
ext 02276D04 void Ko1_set_balloonAnm_NUM(daNpc_Ko1_c*, s32)
ext 02276D84 void Ko1_setAnm(daNpc_Ko1_c*)
ext 022770F0 fopNpc_npc_c* Ko1_searchByID(daNpc_Ko1_c*, u32)
ext 0227732C s32 Ko1_chk_partsNotMove(daNpc_Ko1_c*)
ext 02277A30 u32 Ko1_chkAttention(daNpc_Ko1_c*)
ext 02277AB8 void Ko1_check_landOn(daNpc_Ko1_c*)
ext 02277B9C void Ko1_ko_setPthPos(daNpc_Ko1_c*)
ext 02277FE8 void Ko1_setPrtcl_HanaPachi(daNpc_Ko1_c*)
ext 02278134 void Ko1_setStt(daNpc_Ko1_c*, s32)
ext 0227896C u32 Ko1_wait_1(daNpc_Ko1_c*)
ext 02278AE0 u32 Ko1_wait_2(daNpc_Ko1_c*)
ext 02278B50 u32 Ko1_wait_3(daNpc_Ko1_c*)
ext 02278B84 u32 Ko1_wait_4(daNpc_Ko1_c*)
ext 02278CB4 u32 Ko1_wait_5(daNpc_Ko1_c*, s32)
ext 02278D7C u32 Ko1_wait_6(daNpc_Ko1_c*)
ext 02278EC0 u32 Ko1_wait_7(daNpc_Ko1_c*)
ext 022790E4 u32 Ko1_wait_9(daNpc_Ko1_c*)
ext 022791F4 u32 Ko1_wait_a(daNpc_Ko1_c*)
ext 02279300 u32 Ko1_walk_1(daNpc_Ko1_c*)
ext 022793C8 u32 Ko1_walk_2(daNpc_Ko1_c*, s32, s32)
ext 0227949C u32 Ko1_walk_3(daNpc_Ko1_c*)
ext 02279578 u32 Ko1_swim_1(daNpc_Ko1_c*)
ext 022796B4 u32 Ko1_swim_2(daNpc_Ko1_c*)
ext 022797FC u32 Ko1_attk_1(daNpc_Ko1_c*)
ext 0227992C u32 Ko1_attk_2(daNpc_Ko1_c*, s32, s32)
ext 02279A58 u32 Ko1_attk_3(daNpc_Ko1_c*)
ext 02279B90 u32 Ko1_down_1(daNpc_Ko1_c*, s32)
ext 025F7DB0 u32 Ko1_messageSet(u32, u32, cXyz*)
ext 025F795C u32 Ko1_messageStatus(u32)
ext 02055B64 s16 Ko1_calcTimer(be<s16>*)
ext 021E1E78 s32 Ko1_getRndValue(s32, s32)
ext 028EFFD0 void __construct_array(void*, s32, u32, u32)
ext 02018034 void Ko1_circleDtor(cM3dGCir_l*, s32)
ext 024EFD9C void Ko1_acchDtor(dBgS_ObjAcch*, s32)
ext 025D50BC void Ko1_actorDtor(fopAc_ac_c*, s32)
*/
