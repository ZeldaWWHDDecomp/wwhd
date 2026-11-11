#pragma once
#include "f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_msg.h"
struct daTag_MdCb_c;
// HD: GHS member pointers occupy eight bytes, rather than GC's twelve.
struct MdCbAction {
    union {
        struct {
            be<s16> delta, index;
            be<u32> target;
        };
        gabi::ptmf<s32 (daTag_MdCb_c::*)(u32)> fn;
        be<u32> words[2];
    };
};
// HD: actor base is GC +0x11C; the shorter GHS action pointer makes later members GC +0x118.
struct daTag_MdCb_c : fopAc_ac_c {
    MdCbAction mActionFn;  /* was m290: setAction 024AEFA0 stores the action (member-function pointer with delta / index) and calls it; action 024AF0E0 calls it each frame unless its index is 0 (tww the same); HD GHS member pointer is eight bytes */
    be<s32> mParamMsgNo;  /* was m29C: init 024AF468 sets it to the low 16 parameter bits; initialMsgSetEvent 024AF844 takes it as the message number when the event gives none (tww the same) */
    be<s32> mSwitchStart;  /* was m2A0: init 024AF468 sets it to parameter bits 16..23; checkCondition 024AEA28 tests the switches mSwitchStart .. + mSwitchCount - 1 (tww the same) */
    be<u32> mSwitchCount;  /* was m2A4: init 024AF468 sets it to parameter bits 24..31; checkCondition 024AEA28 loops over that many switches from mSwitchStart (tww the same) */
    be<u32> mMsgNo;  /* was m2A8: initialMsgSetEvent 024AF844 sets the message number (event substance or mParamMsgNo) and picks the sound by it; talk_init 024AF974 / talk 024AFB68 set it as the message, next_msgStatus 024AF9E4 steps it (tww the same) */
    be<u32> m2AC;
    be<u16> mSwitchNo;  /* was m2B0: init 024AF468 sets it to the low byte of current.angle.x; checkCondition 024AEA28 tests (and clears) that switch per tag kind (tww the same) */
    be<s16> mTimer;  /* was m2B2: checkTimer 024AE9FC counts it down (cLib_calcTimer) and is true at 0; init 024AF468 sets 0 / 60 / l_HIO mInitTime by kind, next_msgStatus 024AF9E4 l_HIO mNextMsgTime (tww the same) */
    be<s8> mOrderEventIdx;  /* was m2B4: messageAction 024AFD80 sets the index (0 .. 6) of the event to order by the partner (Medli / Makar) and its carry state; eventOrder 024AF2CC orders mEventIdxTbl[it] while it is not -1; eventProc 024AE748 / init 024AF468 set -1 (tww the same) */
    be<s8> mEventIdx;  /* was m2B5: eventOrder 024AF2CC copies mOrderEventIdx into it when it orders; eventProc 024AE748 ends the event mEventIdxTbl[it], talk 024AFB68 picks the partner animation by it, eventEnd 024AE6FC sets -1 (tww the same) */
    be<s8> mActionPhase;  /* was m2B6: setAction 024AEFA0 sets -1 for the old action's last call and 0 for the new one; warpAction 024AFD10 / messageAction 024AFD80 turn 0 into 1 on their first call and run only after that (tww the same) */
    be<u8> m2B7;
    be<u8> m2B8;
    be<u8> mAreaIn;  /* was m2B9: checkAreaIn 024AE968 sets 1 while the partner is inside the tag's cylinder; execute 024AF360 copies it to m2B8 and clears it each frame (tww the same) */
    be<u8> mSwitchSeen;  /* was m2BA: checkCondition 024AEA28 sets 1 once a switch of the mSwitchStart range is on; then entering the area starts mTimer (checkTimer); init 024AF468 clears it (tww the same) */
    be<u8> m2BB;
    be<s16> mEventIdxTbl[7];  /* was m2BC: init 024AF468 and execute 024AF360 fill [i] with getEventIdx(event_name_tbl[i]) for the 7 events; eventOrder 024AF2CC orders [mOrderEventIdx], eventProc 024AE748 checks [mEventIdx] for the end (tww the same) */
    be<s16> m2CA;
    be<s16> m2CC;
    be<s16> m2CE;
    be<s16> m2D0;
    u8 m2D2[6];
    be<f32> m2D8;
    BOOL checkCondition(), checkAreaIn(fopAc_ac_c *), checkTimer(), checkEventFinish();
    BOOL eventProc(), checkCommandTalk(), setAction(MdCbAction *, void *), warpAction(void *),
        messageAction(void *);
    BOOL init(), execute(), talk(), talk_init();
    s32 getMyStaffId();
    void initialDefault(s32);
    BOOL actionDefault(s32), actionMsgSetEvent(s32), actionMessageEvent(s32);
    u16 next_msgStatus(be<u32> *);
    void eventEnd(), eventOrder(), action(void *), initialInitEvent(s32), initialMsgSetEvent(s32),
        initialPlayerOffDrow(s32), initialPlayerOnDrow(s32);
};
WWHD_OFFSET(daTag_MdCb_c, mActionFn, 0x3AC);
WWHD_OFFSET(daTag_MdCb_c, mParamMsgNo, 0x3B4);
WWHD_OFFSET(daTag_MdCb_c, mTimer, 0x3CA);
WWHD_OFFSET(daTag_MdCb_c, mEventIdxTbl, 0x3D4);
WWHD_OFFSET(daTag_MdCb_c, m2D8, 0x3F0);

namespace mdcb_detail {
enum {
    fpcNm_NPC_MD_e = 0x16F,
    fpcNm_NPC_CB1_e = 0x14E,
    dItemNo_HOOKSHOT_e = 0x2F,
    daMdStts_CARRY_ACTION = 0x800,
    daCbStts_NO_CARRY_ACTION = 1
};
enum {
    fopMsgStts_MSG_TYPING_e = 6,
    fopMsgStts_MSG_DISPLAYED_e = 14,
    fopMsgStts_MSG_DESTROYED_e = 19,
    fopMsgStts_INPUT_e = 21
};

// HD: HIO has its virtual pointer at the end. GC places it before mNo.
struct HioPrm {
    union {
        struct {
            be<f32> mAreaHeight;  /* was m00: daTag_MdCb_c::execute 024AF360: scale.y for kinds 0 / 1, the height of the tag area (the area check counts Link below scale.y) (tww execute the same); daTag_MdCb_HIO_ctor 024AFF08 fills it by a word copy from init_data, never written elsewhere in WWHD or tww (200) */
            be<f32> mAreaRadius;  /* was m04: daTag_MdCb_c::execute 024AF360: scale.x for kinds 0 / 1, the radius of the tag area (tww execute the same); daTag_MdCb_HIO_ctor 024AFF08 fills it by a word copy from init_data, never written elsewhere in WWHD or tww (200) */
            be<s16> mNextMsgTime;  /* was m08: daTag_MdCb_c::next_msgStatus 024AF9E4: mTimer after messages 0x19F0 / 0x19F1 / 0x19F2 / 0x19F4 (tww the same); daTag_MdCb_HIO_ctor 024AFF08 fills it by a word copy from init_data, never written elsewhere in WWHD or tww (900) */
            be<s16> mInitTime;  /* was m0A: daTag_MdCb_c::init 024AF468: mTimer at the start for every kind but 3 (tww the same); daTag_MdCb_HIO_ctor 024AFF08 fills it by a word copy from init_data, never written elsewhere in WWHD or tww (150) */
            be<u8> m0C;
            be<u8> m0D;
            be<u8> m0E;
            be<u8> m0F;
            be<u8> m10;
            be<u8> m11;
            be<u8> m12;
            u8 padding;
        };
        be<u32> words[5];
    };
};
struct Hio {
    be<s8> mNo;
    u8 padding[3];
    HioPrm mPrm;
    gptr<daTag_MdCb_c> mpActor;
    be<u32> mVtable;
};
struct WordBlock {
    be<u32> word[4];
};
struct FloatPair {
    be<f32> negative, positive;
};
struct HeaderObject {
    be<u8> value;
};
struct NpcVtable {
    u8 padding[0x14C];
    be<u32> isTagCheckOK;
    u8 pad150[4];
    be<u32> setMessageAnimation;
};
WWHD_OFFSET(NpcVtable, setMessageAnimation, 0x154);
struct MedliView : fopAc_ac_c {
    u8 padding[0x420C - 0x3AC];
    be<u32> mStatus;  /* was m30F0: daNpc_Md_c::mStatus (d_a_npc_md.h, offset 0x420C) */
};
struct MessageManager {
    u32 getStatus();
    void setStatus(u32);
    u32 messageSet(u32, cXyz *);
};
struct EventManager {
    s32 getMyStaffId(const char *, u32, u32);
    s32 getEventIdx(const char *, u32);
    s32 getMyActIdx(s32, const gptr<char> *, u32, u32, u32);
    void cutEnd(s32);
    BOOL getIsAddvance(s32);
    BOOL endCheck(s16);
    be<u32> *getMySubstanceP(s32, const char *, u32);
};
struct Registration {
    be<u32> words[3];
};
} // namespace mdcb_detail

void mdcb_deleteActorBase(fopAc_ac_c *, s32);
void mdcb_constructFirst(mdcb_detail::HeaderObject *);
void mdcb_constructSecond(mdcb_detail::HeaderObject *);
void mDoAud_monsSeStart(u32, cXyz *, u32, s32);

void PSVECScale(const cXyz *, cXyz *, f32);
