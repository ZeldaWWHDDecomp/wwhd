/* Joel and Zill action/talk slice. HD base grows to 0x7DC; actor size 0xA1C.
 * GC provides method names but only empty layout and nonmatching stubs here. */
#pragma once
#include "d/d_npc.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/tu.h"

struct Ko1PartnerList {
    union { be<u32> mIds[3]; struct { u8 _00[8]; be<u8> mCount; u8 _09[3]; } mMetadata; };
};
WWHD_SIZE(Ko1PartnerList, 12);
struct daNpc_Ko1_c : fopNpc_npc_c {
    enum Status_e {
        STATUS_WAIT_1 = 1,
        STATUS_WAIT_2 = 2,
        STATUS_TALK = 3,
        STATUS_WALK_1 = 4,
        STATUS_WAIT_3 = 5,
        STATUS_ATTACK_1 = 6,
        STATUS_SWIM_1 = 7,
        STATUS_SWIM_2 = 8,
        STATUS_DOWN_1 = 9,
        STATUS_WAIT_4 = 10,
        STATUS_WALK_3 = 11,
        STATUS_WAIT_5 = 12,
        STATUS_ATTACK_2 = 13,
        STATUS_WAIT_6 = 14,
        STATUS_WALK_2 = 15,
        STATUS_DOWN_2 = 16,
        STATUS_ATTACK_3 = 17,
        STATUS_DOWN_3 = 18,
        STATUS_WAIT_7 = 19,
        STATUS_MANZAI = 20,
        STATUS_TALK_END = 21,
        STATUS_WAIT_5_ALT = 22,
        STATUS_ATTACK_2_ALT = 23,
        STATUS_WALK_2_ALT = 24,
        STATUS_DOWN_ALT = 25,
        STATUS_WAIT_9 = 26,
        STATUS_SLEEP_1 = 27,
        STATUS_SLEEP_2 = 28,
        STATUS_WAIT_A = 29,
    };

    BOOL talk_1();
    BOOL talk_2();
    BOOL manzai();
    BOOL neru_1();
    BOOL neru_2();
    BOOL hana_action1(void*);
    BOOL hana_action2(void*);
    BOOL hana_action3(void*);
    BOOL hana_action4(void*);
    BOOL hana_action5(void*);
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    BOOL wait_action3(void*);
    BOOL wait_action4(void*);
    u8 _7DC[0x148];
    /* 0x924 */ Ko1PartnerList mPartners;
    u8 _930[0x10];
    /* 0x940 */ csXyz mInitialAngle;
    u8 _946[0x72];
    /* 0x9B8 */ be<u32> mPartnerMsgNo;
    u8 _9BC[0x2];
    /* 0x9BE */ be<s16> mTalkWaitTimer;
    u8 _9C0[0x2];
    /* 0x9C2 */ be<s16> mSleepTimer;
    /* 0x9C4 */ be<s16> mBalloonTimer;
    u8 _9C6[0x2];
    /* 0x9C8 */ be<s16> mTurnAngle;
    u8 _9CA[0x6];
    /* 0x9D0 */ be<u8> mAnimationEnded;
    u8 _9D1[0x2];
    /* 0x9D3 */ be<u8> mMessageSelector;
    u8 _9D4[0x1];
    /* 0x9D5 */ be<u8> mSleepState;
    u8 _9D6[0x7];
    /* 0x9DD */ be<u8> mbTalkTriggered;
    u8 _9DE[0x2];
    /* 0x9E0 */ be<u32> mStatusResult;
    /* 0x9E4 */ be<u8> mAttention;
    /* 0x9E5 */ be<u8> mTalkFlag;
    u8 _9E6[0x26];
    /* 0xA0C */ be<u8> mAnimationSelector;
    u8 _A0D[0x6];
    /* 0xA13 */ be<s8> mStatus;
    /* 0xA14 */ be<s8> mResumeStatus;
    /* 0xA15 */ be<s8> mAnimationMode;
    u8 _A16[0x1];
    /* 0xA17 */ be<s8> mSpecificType;
    /* 0xA18 */ be<s8> mActionPhase;
    u8 _A19[0x3];
};
WWHD_OFFSET(daNpc_Ko1_c, mPartners, 0x924);
WWHD_OFFSET(daNpc_Ko1_c, mInitialAngle, 0x940);
WWHD_OFFSET(daNpc_Ko1_c, mPartnerMsgNo, 0x9B8);
WWHD_OFFSET(daNpc_Ko1_c, mTalkWaitTimer, 0x9BE);
WWHD_OFFSET(daNpc_Ko1_c, mSleepTimer, 0x9C2);
WWHD_OFFSET(daNpc_Ko1_c, mBalloonTimer, 0x9C4);
WWHD_OFFSET(daNpc_Ko1_c, mTurnAngle, 0x9C8);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationEnded, 0x9D0);
WWHD_OFFSET(daNpc_Ko1_c, mMessageSelector, 0x9D3);
WWHD_OFFSET(daNpc_Ko1_c, mSleepState, 0x9D5);
WWHD_OFFSET(daNpc_Ko1_c, mbTalkTriggered, 0x9DD);
WWHD_OFFSET(daNpc_Ko1_c, mStatusResult, 0x9E0);
WWHD_OFFSET(daNpc_Ko1_c, mAttention, 0x9E4);
WWHD_OFFSET(daNpc_Ko1_c, mTalkFlag, 0x9E5);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationSelector, 0xA0C);
WWHD_OFFSET(daNpc_Ko1_c, mStatus, 0xA13);
WWHD_OFFSET(daNpc_Ko1_c, mResumeStatus, 0xA14);
WWHD_OFFSET(daNpc_Ko1_c, mAnimationMode, 0xA15);
WWHD_OFFSET(daNpc_Ko1_c, mSpecificType, 0xA17);
WWHD_OFFSET(daNpc_Ko1_c, mActionPhase, 0xA18);
WWHD_SIZE(daNpc_Ko1_c, 0xA1C);

/* daNpc_Ko1_childHIO_c (HD 0x60: vtable, parameter block copied from .data, index at +0x5C) */
struct daNpc_Ko1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 mPrm[0x58];
    /* 0x5C */ be<s32> mNo;
};
WWHD_SIZE(daNpc_Ko1_childHIO_c, 0x60);
/* daNpc_Ko1_HIO_c (HD 0xCC) */
struct daNpc_Ko1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mInitState;
    /* 0x0C */ daNpc_Ko1_childHIO_c mChild[2];
};
WWHD_SIZE(daNpc_Ko1_HIO_c, 0xCC);


void Ko1_endEvent(daNpc_Ko1_c*);
void Ko1_lookBack(daNpc_Ko1_c*);
void Ko1_setAnm_NUM(daNpc_Ko1_c*, s32, s32);
void Ko1_set_balloonAnm_NUM(daNpc_Ko1_c*, s32);
void Ko1_setAnm(daNpc_Ko1_c*);
fopNpc_npc_c* Ko1_searchByID(daNpc_Ko1_c*, u32);
s32 Ko1_chk_partsNotMove(daNpc_Ko1_c*);
u32 Ko1_chkAttention(daNpc_Ko1_c*);
void Ko1_check_landOn(daNpc_Ko1_c*);
void Ko1_ko_setPthPos(daNpc_Ko1_c*);
void Ko1_setPrtcl_HanaPachi(daNpc_Ko1_c*);
void Ko1_setStt(daNpc_Ko1_c*, s32);
u32 Ko1_wait_1(daNpc_Ko1_c*);
u32 Ko1_wait_2(daNpc_Ko1_c*);
u32 Ko1_wait_3(daNpc_Ko1_c*);
u32 Ko1_wait_4(daNpc_Ko1_c*);
u32 Ko1_wait_5(daNpc_Ko1_c*, s32);
u32 Ko1_wait_6(daNpc_Ko1_c*);
u32 Ko1_wait_7(daNpc_Ko1_c*);
u32 Ko1_wait_9(daNpc_Ko1_c*);
u32 Ko1_wait_a(daNpc_Ko1_c*);
u32 Ko1_walk_1(daNpc_Ko1_c*);
u32 Ko1_walk_2(daNpc_Ko1_c*, s32, s32);
u32 Ko1_walk_3(daNpc_Ko1_c*);
u32 Ko1_swim_1(daNpc_Ko1_c*);
u32 Ko1_swim_2(daNpc_Ko1_c*);
u32 Ko1_attk_1(daNpc_Ko1_c*);
u32 Ko1_attk_2(daNpc_Ko1_c*, s32, s32);
u32 Ko1_attk_3(daNpc_Ko1_c*);
u32 Ko1_down_1(daNpc_Ko1_c*, s32);
u32 Ko1_messageSet(u32, u32, cXyz*);
u32 Ko1_messageStatus(u32);
s16 Ko1_calcTimer(be<s16>*);
s32 Ko1_getRndValue(s32, s32);
void __construct_array(void*, s32, u32, u32);
void Ko1_circleDtor(cM3dGCir_l*, s32);
void Ko1_acchDtor(dBgS_ObjAcch*, s32);
void Ko1_actorDtor(fopAc_ac_c*, s32);

struct Ko1Vtable { u8 _00[0x1C]; be<u32> mGetMsg; u8 _20[4]; be<u32> mAnmAtr; };
WWHD_OFFSET(Ko1Vtable, mGetMsg, 0x1C);
WWHD_OFFSET(Ko1Vtable, mAnmAtr, 0x24);
WWHD_SIZE(Ko1Vtable, 0x28);
inline u32 Ko1_getMsgVirtual(daNpc_Ko1_c* actor) {
    return gabi::call_ptr<u32>(gabi::at<Ko1Vtable>(actor->__vtbl)->mGetMsg, actor);
}
inline void Ko1_anmAtrVirtual(daNpc_Ko1_c* actor, u16 status) {
    gabi::call_ptr<void>(gabi::at<Ko1Vtable>(actor->__vtbl)->mAnmAtr, actor, (u32)status);
}

struct Ko1TalkActorSlots { be<u32> mIndexAndActors[3]; };
WWHD_SIZE(Ko1TalkActorSlots, 12);
inline Ko1TalkActorSlots* Ko1_talkActorSlots(u32 address) {
    return gabi::at<Ko1TalkActorSlots>(address);
}
inline daNpc_Ko1_HIO_c* Ko1_hioStorage() {
    return gabi::at<daNpc_Ko1_HIO_c>(0x104677DC);
}

inline fopAc_ac_c* Ko1_actorPointer(u32 address) { return gabi::at<fopAc_ac_c>(address); }

WWHD_OFFSET(daNpc_Ko1_childHIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mPrm, 4);
WWHD_OFFSET(daNpc_Ko1_childHIO_c, mNo, 0x5C);
WWHD_OFFSET(daNpc_Ko1_HIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Ko1_HIO_c, mNo, 4);
WWHD_OFFSET(daNpc_Ko1_HIO_c, mInitState, 8);
WWHD_OFFSET(daNpc_Ko1_HIO_c, mChild, 0xC);
