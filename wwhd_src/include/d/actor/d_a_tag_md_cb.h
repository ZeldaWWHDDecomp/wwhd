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
    MdCbAction m290; // HD GHS member pointer is eight bytes.
    be<s32> m29C, m2A0;
    be<u32> m2A4, m2A8, m2AC;
    be<u16> m2B0;
    be<s16> m2B2;
    be<s8> m2B4, m2B5, m2B6;
    be<u8> m2B7, m2B8, m2B9, m2BA, m2BB;
    be<s16> m2BC[7], m2CA, m2CC, m2CE, m2D0;
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
WWHD_OFFSET(daTag_MdCb_c, m290, 0x3AC);
WWHD_OFFSET(daTag_MdCb_c, m29C, 0x3B4);
WWHD_OFFSET(daTag_MdCb_c, m2B2, 0x3CA);
WWHD_OFFSET(daTag_MdCb_c, m2BC, 0x3D4);
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
            be<f32> m00, m04;
            be<s16> m08, m0A;
            be<u8> m0C, m0D, m0E, m0F, m10, m11, m12;
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
    be<u32> m30F0;
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
