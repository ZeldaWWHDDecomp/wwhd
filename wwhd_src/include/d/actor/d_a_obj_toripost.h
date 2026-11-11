/* Rito postbox guest layout. HD: the NPC base grows from 0x6C4 to 0x7DC;
 * postbox fields shift by 0x118. */
#pragma once
#include "d/d_letter.h"
#include "gc_api/gc_api.h"

// HD: archive indices and postbox sounds retain the GameCube values.
enum {
    dRes_INDEX_TORIPOST_BCK_POST_GET_e = 4,
    dRes_INDEX_TORIPOST_BCK_POST_PUTOUT_e = 5,
    dRes_INDEX_TORIPOST_BCK_POST_WAIT_e = 6,
    dRes_INDEX_TORIPOST_BDL_VPOST_e = 9,
    JA_SE_OBJ_POST_EAT_LUGGAGE = 0x6973,
    JA_SE_OBJ_POST_LUGGAGE_OUT = 0x6974
};
struct daObjTpost_c : fopNpc_npc_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode {
        MODE_WAIT = 0x0,
        MODE_TALK = 0x1,
        MODE_TALK_XY = 0x2,
        MODE_RECEIVE = 0x3,
        MODE_RECEIVE_DEMO = 0x4,
        MODE_NULL,
    };
    enum BckIdx {
        BckIdx_POST_GET = 0x0,
        BckIdx_POST_PUTOUT = 0x1,
        BckIdx_POST_WAIT = 0x2,
    };
    enum AnmPrm {
        AnmPrm_POST_GET0 = 0x1,
        AnmPrm_POST_GET1 = 0x2,
        AnmPrm_POST_PUTOUT = 0x3,
        AnmPrm_POST_WAIT = 0x4,
        AnmPrm_NULL,
    };
    typedef void (daObjTpost_c::*mode_func_t)();
    struct mode_entry_t {
        gabi::ptmf<mode_func_t> init, run;
        gptr<const char> name;
    };
    /* 0x7DC */ be<u32> mCurMode;
    /* 0x7E0 */ be<s8> mBckIdx;
    /* 0x7E1 */ be<s8> mAnmPrmIdx;
    /* 0x7E2 */ be<s8> mOldAnmPrmIdx;
    u8 _7E3;
    /* 0x7E4 */ request_of_phase_process_class mPhs;
    /* 0x7EC */ gptr<mDoExt_McaMorf> mMorf;
    /* 0x7F0 */ dBgS_ObjAcch mAcch;
    /* 0x9B4 */ dBgS_AcchCir mAcchCir;
    /* 0x9F4 */ be<s32> mXYEndTimer;  /* was field_0x8DC: modeTalkXYInit 023A4E34 loads l_HIO mXYEndWaitTime; modeTalkXY 023A4E8C ends the item-show talk (back to MODE_WAIT) only once it has counted down and the animation has stopped (tww the same) */
    /* 0x9F8 */ be<s32> mPresentTimer;  /* was field_0x8E0: modeTalkXYInit 023A4E34 loads l_HIO mPresentWaitTime; modeTalkXY 023A4E8C cancels the present and plays animation 2 once the present has ended and it has counted down (tww the same) */
    /* 0x9FC */ be<s32> mPutOutDemoTimer;  /* was field_0x8E4: modeTalkXY 023A4E8C sets 10 when the put-out animation starts (mPutOutStart) and at 0 puts the player into demo mode 24, then -1 (tww the same) */
    /* 0xA00 */ be<u8> mPayType;
    /* 0xA01 */ be<u8> mReceiveReq;  /* was field_0x8E9: next_msgStatus 023A55BC sets 1 when the letter is to be received (no postage due, or paid); modeTalk 023A4D9C then goes to MODE_RECEIVE instead of MODE_WAIT and clears it (tww the same) */
    /* 0xA02 */ be<u8> mPutOutStart;  /* was field_0x8EA: getMsgXY 023A52B8 and next_msgStatus 023A55BC set it with the POST_PUTOUT animation; modeTalkXY 023A4E8C clears it and starts mPutOutDemoTimer (tww the same) */
    /* 0xA03 */ be<u8> mNextLetterMsg;  /* was field_0x8EB: modeReceiveDemo 023A50CC sets it when more readable letters wait; getMsgNormal 023A5500 then opens with message 0xCF7 for the next letter (count mLetterOrdinal) and clears it (tww the same) */
    /* 0xA04 */ be<s32> mNumReadable;
    union {
        /* 0xA08 */ be<u32> mLetterOrdinal;  /* was field_0x8F0: the ordinal of the letter being handed out (its low half is mLetterOrdinalLow, the message count number): createInit 023A418C / modeWaitInit 023A4C68 set 1, modeReceiveDemo 023A50CC and next_msgStatus 023A55BC add 1 per letter (tww dComIfGp_setMessageCountNumber(it)) */
        struct {
            be<s16> mLetterOrdinalHigh, mLetterOrdinalLow;
        };
    };
    /* 0xA0C */ be<u8> mTalkReq;  /* was field_0x8F4: checkOrder 023A4548 sets it when the normal talk event starts; modeWait 023A4CD8 goes to MODE_TALK while it is set; modeTalk 023A4D9C clears it at the end (tww the same) */
    /* 0xA0D */ be<u8> mTalkXYReq;  /* was field_0x8F5: checkOrder 023A4548 sets it when the item-show (XY) talk event starts; modeWait 023A4CD8 goes to MODE_TALK_XY; modeTalkXY 023A4E8C clears it at the end (tww the same) */
    /* 0xA0E */ be<u8> mPreItemNo;
    /* 0xA0F */ be<s8> mEventIdx;
    BOOL _createHeap();
    void setAnm(s32, BOOL);
    void setMtx();
    u32 checkSendPrice();
    void modeProc(s32, s32);
    void createInit();
    s32 _create();
    BOOL _delete();
    s32 getReadableLetterNum();
    void checkOrder();
    void setAttention();
    void cutSetAnmStart(s32);
    void cutDispLetterStart(s32);
    void cutPresentProc(s32);
    void cutSetAnmProc(s32);
    void cutDispLetterProc(s32);
    void cutProc();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    void modeWaitInit();
    BOOL checkTalk();
    void modeWait();
    void modeTalkInit();
    void modeTalk();
    void modeTalkXYInit();
    void modeTalkXY();
    void modeReceiveInit();
    void modeReceive();
    void modeReceiveDemoInit();
    void modeReceiveDemo();
    void deliverLetter();
    s16 getReceiveLetterNum();
    u32 getMsgXY();
    u32 getMsgNormal();
    u32 getMsg();
    u16 next_msgStatus(be<u32> *);
};
WWHD_OFFSET(daObjTpost_c, mMorf, 0x7EC);
WWHD_OFFSET(daObjTpost_c, mAcchCir, 0x9B4);
WWHD_OFFSET(daObjTpost_c, mXYEndTimer, 0x9F4);
WWHD_OFFSET(daObjTpost_c, mLetterOrdinal, 0xA08);
WWHD_OFFSET(daObjTpost_c, mNumReadable, 0xA04);
WWHD_SIZE(daObjTpost_c, 0xA10);

struct daObjTpost_HIO_c {
    be<s8> mNo;
    be<u8> debug_draw;
    be<u8> field_0x06;
    be<u8> mForcePay;  /* was field_0x07: next_msgStatus 023A55BC asks for postage (message 0xCF3) for every letter while it is set, else only for a letter with mPayOnReceive; createHIO 023A5AB4 sets 0 (tww the same) */
    be<f32> attn_pos_offset, eye_pos_offset, talk_distance;
    be<s16> mXYEndWaitTime;  /* was field_0x14: modeTalkXYInit 023A4E34 loads it into mXYEndTimer; createHIO 023A5AB4 sets 30 (tww the same) */
    be<s16> mPresentWaitTime;  /* was field_0x16: modeTalkXYInit 023A4E34 loads it into mPresentTimer; createHIO 023A5AB4 sets 30 (tww the same) */
    be<u32> vt;
};

WWHD_SIZE(daObjTpost_HIO_c, 0x18);

struct daObjTpost_c__letter_data {
    be<u8> mPayOnReceive;  /* was field_0x00: next_msgStatus 023A55BC: a letter with it set asks for postage on delivery (message 0xCF3 -> 0xCF4 / 0xCF8, 10 or 201 rupees) before it is received (tww m_letter[i].mPayOnReceive the same) */
    u8 _01[3];
    be<u32> mMsgNo;
    be<u8> mItemNo;
    u8 _09;
    be<u16> mEventReg;
};
WWHD_OFFSET(daObjTpost_c__letter_data, mMsgNo, 4);
WWHD_OFFSET(daObjTpost_c__letter_data, mEventReg, 0xA);
WWHD_SIZE(daObjTpost_c__letter_data, 0xC);
struct PostStringVtable {
    u8 _00[0x14];
    gfn assureTermination;
};
struct PostString {
    gptr<const be<u8>> text;
    gptr<PostStringVtable> vt;
};
WWHD_OFFSET(PostString, vt, 4);
WWHD_SIZE(PostString, 8);
inline void postTerminate(PostString *str) {
    gabi::call_ptr(str->vt->assureTermination.get(), str);
}
inline dComIfG_inf_c *postInfo() { return reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get()); }
inline bool postTalkXY(dComIfG_inf_c *info) {
    return u32(info->play.mEvtCtrl.mTalkButton - 1) <= 3;
}
struct PostPlayerVtable {
    u8 _00[0x9C];
    gfn getBaseAnimeFrameRate;
};
struct PostPlayerPrefix {
    u8 _00[0xB4];
    gptr<PostPlayerVtable> vt;
};
inline f32 postPlayerFrameRate(daPy_py_c *p) {
    return gabi::call_ptr<f32>(
        reinterpret_cast<PostPlayerPrefix *>(p)->vt->getBaseAnimeFrameRate.get(), p);
}
WWHD_OFFSET(PostPlayerVtable, getBaseAnimeFrameRate, 0x9C);
WWHD_OFFSET(PostPlayerPrefix, vt, 0xB4);
J3DModelData *postGetObjectRes(dRes_control_c *, PostString *, s32);
void postSeStart(u32);
void postParticleSet(dPa_control_c *, u32, u32, cXyz *, csXyz *, cXyz *, u32, void *, s32,
                     GXColor *, void *, void *);
void postSetAnm(const char *, mDoExt_McaMorf *, be<s8> *, be<s8> *, be<s8> *, const be<s32> *,
                const dLib_anm_prm_c *, BOOL, u32);
const be<u8> *postGetSubstance(dEvent_manager_c *, s32, const char *, s32);
s32 postCreatePresent(cXyz *, u32, u32, s32, s32, u32, u32);
void postCutEnd(dEvent_manager_c *, s32);
s32 postGetStaff(dEvent_manager_c *, const char *, void *, s32);
s32 postGetAct(dEvent_manager_c *, s32, const gptr<char> *, s32, BOOL, s32);
BOOL postGetAdvance(dEvent_manager_c *, s32);
void postOrderEvent(fopAc_ac_c *, const char *, u32, u32);
void postHitSe(cXyz *, s32, dCcD_Cyl *, s32);
void postHitEffect(fopAc_ac_c *, dCcD_Cyl *);
void postRollPlayerCrash(fopAc_ac_c *, s32, f32);
void postCopyDebugColor(void *, const void *, u32);
f32 postPlayerDistance(fopAc_ac_c *, fopAc_ac_c *);
s32 postCalcTimer(be<s32> *);
void postCancelPresent(dEvent_manager_c *);
BOOL postEventEnd(dEvent_manager_c *, const char *);
f32 postPow(f32, f32);
void postNpcCtor(fopNpc_npc_c *);
void postAcchCtor(dBgS_Acch *);
void postAcchCirCtor(dBgS_AcchCir *);
void postMoveStatus(void *);
BOOL postPresentEnd(dEvent_manager_c *);
BOOL postChkTgHit(dCcD_Cyl *);
BOOL postDayNight();

void postCreateInitEffect(cXyz *);
struct PostMatrixBuffer {
    u8 _00[4];
    be<u16> flags;
    u8 _06[0xA];
    gptr<Mtx34> matrices;
};
WWHD_OFFSET(PostMatrixBuffer, flags, 4);
WWHD_OFFSET(PostMatrixBuffer, matrices, 0x10);
WWHD_SIZE(PostMatrixBuffer, 0x14);
