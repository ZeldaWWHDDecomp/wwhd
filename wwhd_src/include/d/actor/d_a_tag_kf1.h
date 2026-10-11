/* Kf1 event tag: GC has no recovered fields. HD extends fopNpc_npc_c to 0x880.
 */
#pragma once
#include "bindings.h"
struct daTag_Kf1_c;
struct KfAction : gabi::ptmf<u32 (daTag_Kf1_c::*)(u32)> {
    s16 virtualOffset() const {
        return (s16)(u32)f;
    }
};
WWHD_SIZE(KfAction, 8);
struct KfActionWords {
    be<u32> words[2];
};
struct daTag_Kf1_c : fopNpc_npc_c {
    enum : u32 { __vt = 0x1003F648 };
    u32 set_action(const KfAction *descriptor, u32 context);
    u32 createInit();
    u32 _create();
    u32 _delete();
    void checkOrder();
    void setStt(u32 state);
    void event_talkInit(s32 staff);
    void bensyoInit();
    void goto_nextStage();
    s32 checkPartner();
    void event_cntTsubo();
    u32 event_mesSet();
    u32 event_mesEnd();
    u32 event_bensyo();
    void privateCut();
    void event_proc();
    void eventOrder();
    u32 _execute();
    u32 _draw();
    u32 next_msgStatus(be<u32> *message);
    u32 chkAttention(const cXyz *position);
    u32 partner_srch();
    u32 wait01();
    u32 wait_action1(u32 context);
    /* 0x7DC */ KfAction mAction;
    /* 0x7E4 */ dNpc_EventCut_c mTagEventCut;
    /* 0x850 */ be<u8> mbAttention;
    u8 pad851;
    /* 0x852 */ be<s16> mMissingPartners;
    u8 pad854[2];
    /* 0x856 */ be<u16> mRupees;
    /* 0x858 */ be<u32> mPartnerIds[8];
    /* 0x878 */ be<s16> mPartnerCount;
    /* 0x87A */ be<s8> mCutIndex, mOrder, mState;
    /* 0x87D */ be<u8> mbInitialized;
    /* 0x87E */ be<s8> mActionPhase;
    u8 pad87F;
};
WWHD_OFFSET(daTag_Kf1_c, mAction, 0x7DC);
WWHD_OFFSET(daTag_Kf1_c, mTagEventCut, 0x7E4);
WWHD_OFFSET(daTag_Kf1_c, mbAttention, 0x850);
WWHD_OFFSET(daTag_Kf1_c, mPartnerIds, 0x858);
WWHD_OFFSET(daTag_Kf1_c, mActionPhase, 0x87E);
WWHD_SIZE(daTag_Kf1_c, 0x880);
struct daTag_Kf1_HIO_c {
    enum : u32 { __vt = 0x1003F5A8 };
    be<s8> mNo;
    u8 pad[3];
    union {
        be<f32> mAttentionDistance;
        be<u32> mAttentionDistanceBits;
    };
    union {
        be<f32> mAttentionHeight;
        be<u32> mAttentionHeightBits;
    };
    be<u32> mDebugDrawWord, __vtbl;
};
WWHD_SIZE(daTag_Kf1_HIO_c, 0x14);
struct KfPlay {
    u8 pad0[0x5134];
    u8 mStartStage[0x5292 - 0x5134];
    be<u8> mEventState;
    u8 pad5293[0x52C4 - 0x5293];
    u8 mEventManager[0x5B2C - 0x52C4];
    be<u32> mPlayer;
    u8 pad5B30[0x5B48 - 0x5B30];
    be<u32> mRupeeBalance;
    u8 pad5B4C[0x5BA0 - 0x5B4C];
    be<u16> mRupeeDebt;
};
WWHD_OFFSET(KfPlay, mEventManager, 0x52C4);
WWHD_OFFSET(KfPlay, mRupeeBalance, 0x5B48);
WWHD_OFFSET(KfPlay, mRupeeDebt, 0x5BA0);
WWHD_SIZE(KfPlay, 0x5BA2);
struct KfSave {
    u8 pad[0x24];
    be<u16> mRupees;
};
WWHD_SIZE(KfSave, 0x26);
struct KfPartnerList {
    be<u32> actors[100];
};
struct KfCutNames {
    be<u32> names[5];
};
struct KfEventNames {
    be<u32> names[8];
};
struct KfHioDefaults {
    be<u32> words[3];
};
struct KfGlobalNode {
    be<u32> words[3];
};
struct KfColorWord {
    be<u32> word;
};
struct KfAcchVtables {
    enum : u32 { ground = 0x1003F588, line = 0x1003F598 };
};
constexpr u32 kfJudgeByIDAddress = 0x025E1234;
KfPlay *kfGetPlay();
u32 kfIsActor(void *);
u32 kfGetActorParam(void *, u32, u32);
void kfSetActorInfo(void *, const char *, void *);
void kfNpcConstruct(void *);
void kfEventCutConstruct(void *);
u32 kfHioCreateChild(const char *, u32);
void kfHioDeleteChild(s32);
u32 kfIsPresent(void *, const char *);
u32 kfGetSubstance(void *, s32, const char *, u32);
void kfSetNextStage(void *, u32, s32, s32, u32, u32, u32, f32);
u32 kfSearchByJudge(u32, u32);
u32 kfTalk(void *, u32);
s32 kfGetMyStaffId(void *, const char *, u32, u32);
u32 kfGetMyActIdx(void *, s32, u32, u32, u32, u32);
void kfCutEnd(void *, s32);
u32 kfIsAdvance(void *, s32);
u32 kfIsEnd(void *, const char *);
u32 kfCutProc(void *);
void kfOrderSpeakEvent(void *);
void kfOrderOtherEvent(void *, u32, u32, u32);
void kfCopyBytes(u32, u32, u32);
f32 kfSquareDistance(const cXyz *, const cXyz *);
f32 kfSqrt(f32);
void kfSearchAll(u32, void *);
daTag_Kf1_HIO_c *kfAllocate(u32);
void kfRegisterGlobal(u32);
void kfHeaderByteConstruct(u32);
void kfHeaderFlagConstruct(u32);
void kfAcchDestroy(void *, u32);
void kfAcchCircleDestroy(void *, u32);
void kfLineCheckDestroy(void *, u32);
void kfActorAcchDestroy(void *, u32);
void kfActorDestroy(void *, u32);
void kfFree(void *);

// HD: partial guest objects used by GHS member dispatch and the nullable process lookup.
struct KfActionObject {
    u32 mAddress;
    KfActionObject(daTag_Kf1_c *actor, s32 adjustment) : mAddress(gabi::ea(actor) + adjustment) {
    }
    u32 address() const {
        return mAddress;
    }
    u32 virtualTarget(s16 offset, s32 index) const {
        u32 table = *gabi::at<be<u32>>(mAddress + offset);
        return *gabi::at<be<u32>>(table + 8 * index + 4);
    }
};
struct KfGuestMemory {
    static fopAc_ac_c *actor(u32 address) {
        return gabi::at<fopAc_ac_c>(address);
    }
    static KfSave *save(u32 address) {
        return gabi::at<KfSave>(address);
    }
    static u32 word(u32 address) {
        return *gabi::at<be<u32>>(address);
    }
};
enum KfMessage {
    KfMsg_DEBT = 0x1C2D,
    KfMsg_CHECK_RUPEES = 0x1C2E,
    KfMsg_CAN_PAY = 0x1C2F,
    KfMsg_CANNOT_PAY = 0x1C30,
    KfMsg_PAYMENT_END = 0x1C31,
    KfMsg_PAYMENT_NEXT = 0x1C35
};
#include "gc_api/f_op/f_op_actor_mng.h"

// HD process ID, absent from the merged shared process-name subset.
enum { fpcNm_TAG_KF1_e = 0x19E };

WWHD_OFFSET(daTag_Kf1_c,mMissingPartners,0x852);
WWHD_OFFSET(daTag_Kf1_c,mRupees,0x856);
WWHD_OFFSET(daTag_Kf1_c,mPartnerCount,0x878);
WWHD_OFFSET(daTag_Kf1_c,mCutIndex,0x87A);
WWHD_OFFSET(daTag_Kf1_c,mOrder,0x87B);
WWHD_OFFSET(daTag_Kf1_c,mState,0x87C);
WWHD_OFFSET(daTag_Kf1_c,mbInitialized,0x87D);
WWHD_OFFSET(daTag_Kf1_HIO_c,mAttentionDistance,4);
WWHD_OFFSET(daTag_Kf1_HIO_c,mAttentionHeight,8);
WWHD_OFFSET(daTag_Kf1_HIO_c,mDebugDrawWord,0xC);
WWHD_OFFSET(daTag_Kf1_HIO_c,__vtbl,0x10);
WWHD_OFFSET(KfPlay,mStartStage,0x5134);
WWHD_OFFSET(KfPlay,mEventState,0x5292);
WWHD_OFFSET(KfPlay,mPlayer,0x5B2C);
WWHD_OFFSET(KfSave,mRupees,0x24);
WWHD_SIZE(KfActionWords,8);
WWHD_SIZE(KfPartnerList,400);
WWHD_SIZE(KfCutNames,20);
WWHD_SIZE(KfEventNames,32);
WWHD_SIZE(KfHioDefaults,12);
WWHD_SIZE(KfGlobalNode,12);
WWHD_SIZE(KfColorWord,4);
