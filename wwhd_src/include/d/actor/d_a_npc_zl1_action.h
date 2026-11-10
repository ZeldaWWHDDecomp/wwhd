/* Tetra action slice: GameCube field names on the asserted HD layout.
 * HD base 0x7DC, animation/lighting growth places the actor tail at 0xD74. */
#pragma once
#include "d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

struct daNpc_Zl1_c : fopNpc_npc_c {
    u8 chk_parts_notMov();
    u8 chkAttention();
    u8 chk_talk();
    u32 getMsg_ZL1_2();
    u32 getMsg_ZL1_4();
    u32 getMsg();
    u16 next_msgStatus(be<u32>* pMsgNo);
    void chngAnmAtr(u8 param_1);
    void anmAtr(u16 mesgNo);
    u8 chk_areaIN(f32 param_1, f32 param_2, s16 param_3, cXyz* param_4);
    BOOL wait_1();
    BOOL talk_1();
    BOOL demo_1();
    BOOL demo_2();
    BOOL demo_3();
    BOOL demo_4();
    BOOL optn_1();
    BOOL optn_2();
    BOOL optn_3();
    BOOL wait_action1(void*);
    BOOL demo_action1(void*);
    BOOL demo_action2(void*);
    BOOL optn_action1(void*);
    void setStt(s8);
    void setAnm_ATR();
    void setAnm_NUM(int, int);
    void endEvent();
    void eventOrder();
    u8 move_jmp();
    BOOL setFrontWallType();
    void kyoroPos(cXyz*, int);
    u8 _7DC[0x1A0];
    /* 0x97C */ csXyz mInitialAngle;  /* was field_0x738: _execute 02307D18 copies current.angle on the first run, with mInitialPos, then mbRanExecute = 1; event_proc 023071C4 copies it again; wait_1 02309394 turns current.angle.y back to its y (type 4) (tww the same); same code as daNpc_Hi1_c::mInitialAngle */
    u8 _982[0x1A];
    /* 0x99C */ cXyz mLookPos;  /* was field_0x758: lookBack 023075B0 / kyorokyoro 023074E4 look at it; cut_init_MAJYU_START 02306624 and chngAnmAtr 0230883C set it */
    /* 0x9A8 */ cXyz field_0x764;
    u8 _9B4[0x24];
    /* 0x9D8 */ csXyz mPartsAngle;  /* was field_0x794: lookBack 023075B0 stores x = current.angle.y, y = the head Y angle, z = the backbone Y angle; chk_parts_notMov 02308DFC compares them with the current ones (tww the same); same code as daNpc_Hi1_c::mPartsAngle */
    u8 _9DE[0x2];
    /* 0x9E0 */ be<s32> mbSetEyePos;  /* was field_0x79C: wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 store the step's result (wait_1(), talk_1(), demo_1() ..); checkOrder 023055A4 sets 1; setAttention 02304C44 tests it with its param (if (it == 0 && !param_1)) (tww the same); same code as daNpc_Hi1_c::mbSetEyePos */
    u8 _9E4[0xE];
    /* 0x9F2 */ be<s16> field_0x7AE;
    /* 0x9F4 */ be<s16> mKyoroTimer;  /* was field_0x7B0: optn_1 02309794 sets HIO prm 0x2C / 0x2E; kyorokyoro 023074E4 counts it down, then picks the next look position */
    /* 0x9F6 */ be<s16> mKyoroPosNo;  /* was field_0x7B2: kyorokyoro 023074E4 passes it to kyoroPos and sets cLib_getRndValue(1, 10); optn_1 02309794 clears it */
    u8 _9F8[0x2];
    /* 0x9FA */ be<s16> mWaitTimer;  /* was field_0x7B6: wait_1 02309394 / talk_1 02309478 set a random 15..30 and count it down */
    /* 0x9FC */ be<s16> field_0x7B8;
    /* 0x9FE */ be<s16> field_0x7BA;
    /* 0xA00 */ be<s16> field_0x7BC;
    u8 _A02[0x5];
    /* 0xA07 */ be<s8> mbMorfAnimStopped;  /* was field_0x7C3: play_animation 023045E0: the morf's play() result, or 1 when the frame wrapped; setAnm_anm 0230607C / cut_init_OKIRU 023066B4 clear it; cut_move_OKIRU 02306D34 returns it, cut_move_OKIRU_2 02306D48 / optn_1 02309794 test it (tww the same); same code as daNpc_Hi1_c::mbMorfAnimStopped (tww daNpc_Ls1_c mbMorfAnimStopped) */
    u8 _A08[0x1];
    /* 0xA09 */ be<u8> mItemNo;
    u8 _A0A[0x6];
    /* 0xA10 */ be<u8> field_0x7CC;
    u8 _A11[0x3];
    /* 0xA14 */ be<u8> field_0x7D0;
    u8 _A15[0x5];
    /* 0xA1A */ be<u8> mHasAttention;
    /* 0xA1B */ be<u8> mTalking;  /* was field_0x7D7: checkOrder 023055A4 sets 1 on the talk command (dEvtCmd_INTALK); wait_1 02309394 starts the talk (setStt(2)) while it is set; talk_1 02309478 talks while it is set and clears it when the message ends; optn_1 02309794 tests it (tww the same); same code as daNpc_Hi1_c::mTalking */
    /* 0xA1C */ be<u8> mLookHeadOnly;  /* was field_0x7D8: lookBack 023075B0 passes it to dNpc_JntCtrl_c::lookAtTarget_2 as headOnly (d_npc.h; tww lookAtTarget_2 turns the body only if (mbTrn && !it)); wait_1 02309394 / optn_1 02309794 / checkOrder 023055A4 / cut_init_LOK_PLYER 023062C4 / cut_init_LOK_PARTNER 023063A0 set it, wait_1 / optn_2 023099F4 / cut_init_JMP_OFF 0230684C clear it (tww the same); same role as daNpcPhoto_c::mLookHeadOnly */
    u8 _A1D[0x5F];
    /* 0xA7C */ cXyz field_0x830;
    u8 _A88[0x9];
    /* 0xA91 */ be<u8> mAnmAtrNo;  /* was field_0x845: setAnm_ATR 02306458 plays a_anm_prm_tbl[it]; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 reset it to 0xFF */
    /* 0xA92 */ be<u8> mMesgAnimeTag;  /* was field_0x846: anmAtr 02308AC0: the message's animation tag (dComIfGp_getMesgAnimeTagInfo) when it changes; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 0xFF (tww the same); same code as daNpc_Hi1_c::mMesgAnimeTag (tww daNpc_Ls1_c mMesgAnimeTag) */
    u8 _A93[0x2];
    /* 0xA95 */ be<s8> mAnmNo;  /* was field_0x849: setAnm_anm 0230607C: skips when the entry's animation equals it, else stores the entry's animation number; createInit 0230503C sets 0x11 */
    /* 0xA96 */ be<s8> mOrderEvent;  /* was field_0x84A: eventOrder 02307988: 1 / 2 the talk (1 orders the speak event), 3+ orders mEventIdx[it - 3] (mEventNo); checkOrder 023055A4 clears it once that event or the talk starts; init_ZL1_5 02303E98, wait_1 02309394, demo_2..4, optn_1 / optn_3, setStt 02306184 set it (tww the same); same code as daNpc_Hi1_c::mOrderEvent */
    /* 0xA97 */ be<s8> mStt;  /* was field_0x84B: setStt 02306184 stores the new state (and switches on it); setAnm 02306114 plays a_anm_prm_tbl[it] */
    /* 0xA98 */ be<s8> mPrevStt;  /* was field_0x84C: setStt 02306184 keeps the previous mStt here; talk_1 02309478 returns to it (setStt(it)) */
    /* 0xA99 */ be<s8> mLookMode;  /* was field_0x84D: lookBack 023075B0 switches on it: 1 the player's eyes, 2 mLookPos, 3 the angle at 0xA04, 4 the partner, 5 kyorokyoro (0 while mAnmNo is 0xD / 0xE); chngAnmAtr 0230883C, wait_1 02309394, optn_1 / optn_2, cut_init_LOK_PLYER 023062C4 / LOK_PARTNER 023063A0 / MAJYU_START 02306624 set it (tww the same); same code as daNpc_Hi1_c::mLookMode */
    u8 _A9A[0x1];
    /* 0xA9B */ be<s8> mType;  /* was field_0x84F: decideType 023037F4 stores the type argument (0-7, else -1) and picks the archive by it (Zl2 for 0 / 6 / 7, else Zl); createInit 0230503C / partner_search 02305508 / getMsg 02308D08 switch on it, itemCreateHeap 02303400, wait_1 02309394, demo_1 023095E4, wait_action1 02309CEC, demo_action1 02309E10, demo 02305778, _draw 02308090 test it (tww decideType the same); after daNpc_Hi1_c::mType (same code) */
    /* 0xA9C */ be<s8> mActionStatus;  /* was field_0x850: set_action 023038B8 sets 9 for the leaving action and 0 for the new one; wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 switch on it and step it (it + 1); partner_search 02305508 tests 1 and steps it (tww the same); same code as daNpc_Hi1_c::mActionStatus */
    /* 0xA9D */ be<s8> mAnmAtrSetCnt;  /* was field_0x851: anmAtr 02308AC0: while the message types, applies the message's animation attribute (chngAnmAtr) only when it is 0, then adds 1; MSG_DISPLAYED (0xE) and setStt 02306184 reset it (tww the same); same code as daNpc_Hi1_c::mAnmAtrSetCnt */
    u8 _A9E[0x2D6];
};
WWHD_OFFSET(daNpc_Zl1_c, mInitialAngle, 0x97C);
WWHD_OFFSET(daNpc_Zl1_c, mLookPos, 0x99C);
WWHD_OFFSET(daNpc_Zl1_c, field_0x764, 0x9A8);
WWHD_OFFSET(daNpc_Zl1_c, mPartsAngle, 0x9D8);
WWHD_OFFSET(daNpc_Zl1_c, mbSetEyePos, 0x9E0);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7AE, 0x9F2);
WWHD_OFFSET(daNpc_Zl1_c, mKyoroTimer, 0x9F4);
WWHD_OFFSET(daNpc_Zl1_c, mKyoroPosNo, 0x9F6);
WWHD_OFFSET(daNpc_Zl1_c, mWaitTimer, 0x9FA);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7B8, 0x9FC);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7BA, 0x9FE);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7BC, 0xA00);
WWHD_OFFSET(daNpc_Zl1_c, mbMorfAnimStopped, 0xA07);
WWHD_OFFSET(daNpc_Zl1_c, mItemNo, 0xA09);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7CC, 0xA10);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7D0, 0xA14);
WWHD_OFFSET(daNpc_Zl1_c, mHasAttention, 0xA1A);
WWHD_OFFSET(daNpc_Zl1_c, mTalking, 0xA1B);
WWHD_OFFSET(daNpc_Zl1_c, mLookHeadOnly, 0xA1C);
WWHD_OFFSET(daNpc_Zl1_c, field_0x830, 0xA7C);
WWHD_OFFSET(daNpc_Zl1_c, mAnmAtrNo, 0xA91);
WWHD_OFFSET(daNpc_Zl1_c, mMesgAnimeTag, 0xA92);
WWHD_OFFSET(daNpc_Zl1_c, mAnmNo, 0xA95);
WWHD_OFFSET(daNpc_Zl1_c, mOrderEvent, 0xA96);
WWHD_OFFSET(daNpc_Zl1_c, mStt, 0xA97);
WWHD_OFFSET(daNpc_Zl1_c, mPrevStt, 0xA98);
WWHD_OFFSET(daNpc_Zl1_c, mLookMode, 0xA99);
WWHD_OFFSET(daNpc_Zl1_c, mType, 0xA9B);
WWHD_OFFSET(daNpc_Zl1_c, mActionStatus, 0xA9C);
WWHD_OFFSET(daNpc_Zl1_c, mAnmAtrSetCnt, 0xA9D);
WWHD_SIZE(daNpc_Zl1_c, 0xD74);

struct daNpc_Zl1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackboneX;
        /* 0x0A */ be<s16> mMaxBackboneY;
        /* 0x0C */ be<s16> mMinBackboneX;
        /* 0x0E */ be<s16> mMinBackboneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> field_18;
        /* 0x14 */ be<f32> field_1C;
        /* 0x18 */ be<u8> field_20;
        /* 0x19 */ be<u8> field_21;
        /* 0x1A */ be<u8> field_22;
        /* 0x1B */ be<u8> field_23;
        /* 0x1C */ be<f32> field_24;
        /* 0x20 */ be<f32> field_28;
        /* 0x24 */ be<s16> field_2C;
        /* 0x26 */ be<s16> field_2E;
        /* 0x28 */ be<s16> field_30;
        /* 0x2A */ be<s16> field_32;
        /* 0x2C */ be<f32> field_34;
        /* 0x30 */ be<f32> field_38;
        /* 0x34 */ be<f32> field_3C;
        /* 0x38 */ be<f32> field_40;
        /* 0x3C */ be<f32> field_44;
        /* 0x40 */ be<f32> field_48;
        /* 0x44 */ be<f32> field_4C;
        /* 0x48 */ be<f32> field_50;
        /* 0x4C */ be<f32> field_54;
        /* 0x50 */ be<f32> field_58;
        /* 0x54 */ be<s16> field_5C;
        /* 0x56 */ be<s16> field_5E;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Zl1_HIO_c, 0x64);


inline daNpc_Zl1_HIO_c& Zl1_hio() { return *gabi::at<daNpc_Zl1_HIO_c>(0x10468C20); }
inline u32 Zl1_counter() { return *gabi::at<be<u32>>(0x101FF558); }

f32 PSVECSquareDistance(const cXyz*, const cXyz*);
s16 Zl1_calcTimer(be<s16>*);
s32 Zl1_getRndValue(s32, s32);
bool Zl1_LockonTruth(dAttention_c*);
fopAc_ac_c* Zl1_LockonTarget(dAttention_c*, s32);
fopAc_ac_c* Zl1_ActionTarget(dAttention_c*, s32);
u32 Zl1_messageStatus(u32);
void Zl1_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8);

WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMaxHeadX, 0x00);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMaxHeadY, 0x02);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMinHeadX, 0x04);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMinHeadY, 0x06);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMaxBackboneX, 0x08);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMaxBackboneY, 0x0A);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMinBackboneX, 0x0C);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMinBackboneY, 0x0E);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, mMaxTurnStep, 0x10);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_18, 0x12);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_1C, 0x14);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_20, 0x18);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_21, 0x19);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_22, 0x1A);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_23, 0x1B);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_24, 0x1C);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_28, 0x20);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_2C, 0x24);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_2E, 0x26);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_30, 0x28);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_32, 0x2A);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_34, 0x2C);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_38, 0x30);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_3C, 0x34);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_40, 0x38);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_44, 0x3C);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_48, 0x40);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_4C, 0x44);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_50, 0x48);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_54, 0x4C);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_58, 0x50);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_5C, 0x54);
WWHD_OFFSET(daNpc_Zl1_HIO_c::hio_prm_c, field_5E, 0x56);
WWHD_SIZE(daNpc_Zl1_HIO_c::hio_prm_c, 0x58);
WWHD_OFFSET(daNpc_Zl1_HIO_c, mPrmTbl, 0xC);
