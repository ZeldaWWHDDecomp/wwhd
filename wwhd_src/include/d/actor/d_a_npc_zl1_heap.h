/** Tetra heap views. HD uses relative resource offsets and name-based material lookup.
 * Actor and HIO layouts retain the GameCube field names from d_a_npc_zl1.h. */
#pragma once
/* daNpc_Zl1_c (Tetra), WWHD layout.
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_zl1 (HD constructor allocates
 * 0xD74; GameCube 0x900):
 * - fopNpc_npc_c is 0x7DC (see d/d_npc.h): members +0x118 up to mArcName (0x7F4);
 * - mShadowId (GameCube 0x6E0) is gone (HD shadows): mBtkAnm (0x7FC) is +0x114;
 * - mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14 to 0x74, and HD adds a second btp
 *   animation (0x8E8, set up together with the first by setBtp): +0x174 for mBtkAnmFrame /
 *   mBtpAnm, +0x248 for mBtpAnmFrame / mTimer;
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: +0x244 from mProcId1
 *   up to mLightInfluence1;
 * - LIGHT_INFLUENCE grew from 0x20 to 0x24 (an HD f32 at +0x20, 1.0 by the constructor): +0x248
 *   for mLightInfluence2, +0x24C from field_0x830 to field_0x8A8;
 * - HD: two s32 material indices at 0xB0C/0xB10 (bodyCreateHeap); the four
 *   mDoExt_*CupOn*AupPacket members grew from 0x10 to 0x98 each (0xB14..0xD74).
 * Field names are the GameCube ones (field_0xNNN = GameCube offset). */
#pragma once
#include "d/d_npc.h"
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
struct ProcFunc_l { be<s16> d,i; be<u32> f; };
WWHD_SIZE(ProcFunc_l,8);

#define ZL1_SAFESTRING_VTBL 0x10023B10 /* this TU's sead::SafeString vtable */


/* daNpc_Zl1_matAnm_c : J3DMaterialAnm (HD size and layout as daNpc_Ls1_matAnm_c: vtable at +0x68,
 * to be confirmed by the HD constructor) */
struct daNpc_Zl1_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ cXy mOffset;
    /* 0x74 */ cXy field_0x74;
    /* 0x7C */ be<u8> field_0x7C; /* move flag */
    /* 0x7D */ u8 _7D[3];
};
WWHD_SIZE(daNpc_Zl1_matAnm_c, 0x80);

/* LIGHT_INFLUENCE: the shared one (bindings.h, HD 0x24) */

struct daNpc_Zl1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> field_0x0; /* bck number */
        /* 0x01 */ be<s8> field_0x1; /* btp number */
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mPlaySpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    BOOL set_startPos(int);
    bool init_ZL1_0();
    bool init_ZL1_1();
    bool init_ZL1_2();
    bool init_ZL1_3();
    bool init_ZL1_4();
    bool init_ZL1_5();
    bool init_ZL1_6();
    bool init_ZL1_7();
    void setEyeCtrl();
    void clrEyeCtrl();
    bool createInit();
    void play_animation();
    bool swoon_OnShip();
    void setMtx(u32);
    u32 bckResID(int); /* char* */
    u32 btpResID(int);
    u32 btkResID(int);
    u32 setBtp(s32, u32); /* HD: the full register reaches btpResID */
    void setMat();
    u32 setBtk(s32, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void eye_ctrl();
    void play_btk_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_ZL1_2();
    u32 getMsg_ZL1_4();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    u8 partner_search_sub(u32);
    void partner_search();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    f32 get_prmFloat(be<f32>*, f32);
    void set_LightPos(cXyz*); /* returns a cXyz through a hidden result pointer */
    void init_Light();
    void incEnvironment();
    void decEnvironment();
    void darkProc();
    void cut_init_LOK_PLYER(int);
    void cut_init_LOK_PARTNER(int);
    void cut_init_CHG_ANM_ATR(int);
    void cut_init_PLYER_TRN_PARTNER(int);
    void cut_init_PLYER_TRN_TETRA(int);
    void cut_init_MAJYU_START(int);
    void cut_init_OKIRU(int);
    u32 cut_move_OKIRU();
    void cut_init_OKIRU_2(int);
    u32 cut_move_OKIRU_2();
    void cut_init_DRW_ONOFF(int);
    void cut_init_PLYER_DRW_ONOFF(int);
    void cut_init_JMP_OFF(int);
    u32 cut_move_JMP_OFF();
    void cut_init_OMAMORI_ONOFF(int);
    void cut_init_SURPRISED(int);
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, f32, s16, cXyz*);
    void setWaterRipple();
    void setWaterSplash();
    void set_simpleLand(u32);
    void setEff();
    BOOL setFrontWallType();
    u8 move_jmp();
    void kyoroPos(cXyz*, int); /* returns a cXyz through a hidden result pointer */
    BOOL kyorokyoro();
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
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;     /* GameCube 0x6C4 */
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ u8 field_0x6CE[2];
    /* 0x7E8 */ gptr<J3DModel> mpModel;                   /* GameCube 0x6D0 */
    /* 0x7EC */ gptr<daNpc_Zl1_matAnm_c> mpEyeMatAnm[2];  /* was field_0x6D4: bodyCreateHeap sets the two eye material animations; setEyeCtrl 023041F8 / clrEyeCtrl 02304230 / eye_ctrl 02304310 drive [i] */
    /* 0x7F4 */ char mArcName[3];                         /* GameCube 0x6DC */
    /* 0x7F7 */ u8 _7F7;
    /* 0x7F8 */ u8 field_0x6E4[4];                        /* HD: no mShadowId before it */
    /* 0x7FC */ u8 mBtkAnm[0x74];                         /* mDoExt_btkAnm (HD 0x74), GameCube 0x6E8 */
    /* 0x870 */ be<u8> mBtkAnmFrame;                      /* GameCube 0x6FC */
    /* 0x871 */ u8 _871[3];
    /* 0x874 */ u8 mBtpAnm[0x74];                         /* mDoExt_btpAnm (HD 0x74), GameCube 0x700 */
    /* 0x8E8 */ u8 mBtpAnm2[0x74];                        /* HD: a second mDoExt_btpAnm */
    /* 0x95C */ be<u8> mBtpAnmFrame;                      /* GameCube 0x714 */
    /* 0x95D */ u8 _95D;
    /* 0x95E */ be<s16> mTimer;                           /* GameCube 0x716 */
    /* 0x960 */ ProcFunc_l mCurrActionFunc;               /* GameCube 0x718 (12 bytes) */
    /* 0x968 */ be<u32> mProcId1;                         /* GameCube 0x724 */
    /* 0x96C */ be<u32> mProcId2;
    /* 0x970 */ cXyz mInitialPos;  /* was field_0x72C: _execute 02307D18 copies current.pos on the first run (while mbRanExecute is 0); event_proc 023071C4 copies it again (tww the same); written only, but the same code as daNpc_Hi1_c::mInitialPos (with mInitialAngle) */
    /* 0x97C */ csXyz mInitialAngle;  /* was field_0x738: _execute 02307D18 copies current.angle on the first run, with mInitialPos, then mbRanExecute = 1; event_proc 023071C4 copies it again; wait_1 02309394 turns current.angle.y back to its y (type 4) (tww the same); same code as daNpc_Hi1_c::mInitialAngle */
    /* 0x982 */ csXyz field_0x73E;
    /* 0x988 */ csXyz mEffAngle;  /* was field_0x744: move_jmp 02306DB8 copies current.angle; setWaterSplash 023048F4 / set_simpleLand 0230498C pass it to createLandEmitter as the angle */
    /* 0x98E */ u8 field_0x74A[2];
    /* 0x990 */ cXyz mEyePos;  /* was field_0x74C: _nodeCB_Head 023020F8: the head matrix times a_eye_pos_off; setAttention 02304C44 copies it to eyePos */
    /* 0x99C */ cXyz mLookPos;  /* was field_0x758: lookBack 023075B0 / kyorokyoro 023074E4 look at it; cut_init_MAJYU_START 02306624 and chngAnmAtr 0230883C set it */
    /* 0x9A8 */ cXyz field_0x764;
    /* 0x9B4 */ cXyz mHeadPos;  /* was field_0x770: _nodeCB_Head 023020F8: translation of the head joint matrix */
    /* 0x9C0 */ cXyz mEffPos;  /* was field_0x77C: move_jmp 02306DB8 copies current.pos; setWaterSplash 023048F4 / set_simpleLand 0230498C pass it to createLandEmitter as the position */
    /* 0x9CC */ be<f32> mRatio;                           /* GameCube 0x788 */
    /* 0x9D0 */ be<f32> mFrame;
    /* 0x9D4 */ u8 field_0x790[4];
    /* 0x9D8 */ csXyz mPartsAngle;  /* was field_0x794: lookBack 023075B0 stores x = current.angle.y, y = the head Y angle, z = the backbone Y angle; chk_parts_notMov 02308DFC compares them with the current ones (tww the same); same code as daNpc_Hi1_c::mPartsAngle */
    /* 0x9DE */ u8 _9DE[2];
    /* 0x9E0 */ be<s32> mbSetEyePos;  /* was field_0x79C: wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 store the step's result (wait_1(), talk_1(), demo_1() ..); checkOrder 023055A4 sets 1; setAttention 02304C44 tests it with its param (if (it == 0 && !param_1)) (tww the same); same code as daNpc_Hi1_c::mbSetEyePos */
    /* 0x9E4 */ be<s16> mEventIdx[4];                     /* GameCube 0x7A0 */
    /* 0x9EC */ be<s16> mEventNo;  /* was field_0x7A8: init_ZL1_5 02303E98 / eventOrder 02307988 set it and order mEventIdx[it] (fopAcM_orderOtherEventId) */
    /* 0x9EE */ u8 field_0x7AA[4];
    /* 0x9F2 */ be<s16> field_0x7AE;
    /* 0x9F4 */ be<s16> mKyoroTimer;  /* was field_0x7B0: optn_1 02309794 sets HIO prm 0x2C / 0x2E; kyorokyoro 023074E4 counts it down, then picks the next look position */
    /* 0x9F6 */ be<s16> mKyoroPosNo;  /* was field_0x7B2: kyorokyoro 023074E4 passes it to kyoroPos and sets cLib_getRndValue(1, 10); optn_1 02309794 clears it */
    /* 0x9F8 */ u8 field_0x7B4[2];
    /* 0x9FA */ be<s16> mWaitTimer;  /* was field_0x7B6: wait_1 02309394 / talk_1 02309478 set a random 15..30 and count it down */
    /* 0x9FC */ be<s16> field_0x7B8;
    /* 0x9FE */ be<s16> field_0x7BA;
    /* 0xA00 */ be<s16> field_0x7BC;
    /* 0xA02 */ u8 field_0x7BE[2];
    /* 0xA04 */ be<s16> field_0x7C0;
    /* 0xA06 */ be<s8> field_0x7C2;                       /* GameCube 0x7C2 */
    /* 0xA07 */ be<s8> mbMorfAnimStopped;  /* was field_0x7C3: play_animation 023045E0: the morf's play() result, or 1 when the frame wrapped; setAnm_anm 0230607C / cut_init_OKIRU 023066B4 clear it; cut_move_OKIRU 02306D34 returns it, cut_move_OKIRU_2 02306D48 / optn_1 02309794 test it (tww the same); same code as daNpc_Hi1_c::mbMorfAnimStopped (tww daNpc_Ls1_c mbMorfAnimStopped) */
    /* 0xA08 */ be<s8> field_0x7C4;
    /* 0xA09 */ be<u8> mItemNo;
    /* 0xA0A */ be<u8> field_0x7C6;
    /* 0xA0B */ be<u8> mbGroundHit;  /* was field_0x7C7: _execute 02307D18 stores mObjAcch.ChkGroundHit() (the flag of the previous CrrPos) each frame; move_jmp 02306DB8 lands when it was 0 and the acch now hits the ground; cut_init_JMP_OFF 0230684C sets 1 (tww it = mObjAcch.ChkGroundHit()) */
    /* 0xA0C */ be<u8> mbWaterIn;  /* was field_0x7C8: _execute 02307D18 stores mObjAcch.ChkWaterIn() each frame; move_jmp 02306DB8, setWaterRipple 02304824 and setEff 02304A6C test it (HD in place of ChkWaterIn()); cut_init_JMP_OFF 0230684C clears it, setMtx 02304C98 sets it in Demo37 (tww it = mObjAcch.ChkWaterIn()) */
    /* 0xA0D */ be<u8> field_0x7C9;
    /* 0xA0E */ be<u8> field_0x7CA;
    /* 0xA0F */ be<u8> field_0x7CB;
    /* 0xA10 */ be<u8> field_0x7CC;
    /* 0xA11 */ u8 field_0x7CD;
    /* 0xA12 */ be<u8> field_0x7CE;
    /* 0xA13 */ be<u8> mStateIsComplaete;
    /* 0xA14 */ be<u8> field_0x7D0;
    /* 0xA15 */ be<u8> mEyeCtrl;  /* was field_0x7D1: setEyeCtrl 023041F8 sets 1, clrEyeCtrl 02304230 sets 0; eye_ctrl 02304310 moves the eyes only while set */
    /* 0xA16 */ be<u8> field_0x7D2;
    /* 0xA17 */ be<u8> field_0x7D3;
    /* 0xA18 */ be<u8> mNoDraw;  /* was field_0x7D4: cut_init_DRW_ONOFF 0230674C sets it when the event's OnOff is 0; demo 02305778 sets it for frame ranges; _draw 02308090 skips drawing while set */
    /* 0xA19 */ be<u8> mbRanExecute;  /* was field_0x7D5: _execute 02307D18: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww if (!it) { ..; it = true; }); same code as daNpc_Hi1_c::mbRanExecute */
    /* 0xA1A */ be<u8> mHasAttention;
    /* 0xA1B */ be<u8> mTalking;  /* was field_0x7D7: checkOrder 023055A4 sets 1 on the talk command (dEvtCmd_INTALK); wait_1 02309394 starts the talk (setStt(2)) while it is set; talk_1 02309478 talks while it is set and clears it when the message ends; optn_1 02309794 tests it (tww the same); same code as daNpc_Hi1_c::mTalking */
    /* 0xA1C */ be<u8> mLookHeadOnly;  /* was field_0x7D8: lookBack 023075B0 passes it to dNpc_JntCtrl_c::lookAtTarget_2 as headOnly (d_npc.h; tww lookAtTarget_2 turns the body only if (mbTrn && !it)); wait_1 02309394 / optn_1 02309794 / checkOrder 023055A4 / cut_init_LOK_PLYER 023062C4 / cut_init_LOK_PARTNER 023063A0 set it, wait_1 / optn_2 023099F4 / cut_init_JMP_OFF 0230684C clear it (tww the same); same role as daNpcPhoto_c::mLookHeadOnly */
    /* 0xA1D */ be<u8> mInDemo;  /* was field_0x7D9: demo 02305778: 1 while demoActorID is set (resets the joint angles when it turns on), 0 otherwise, and returns it; _execute 02307D18 skips the collision while set, setMtx 02304C98 the ripple and effects (tww the same); same code as daNpc_Hi1_c::mInDemo */
    /* 0xA1E */ u8 _A1E[2];
    /* 0xA20 */ u8 mRippleCallBack[0x14];                 /* dPa_rippleEcallBack, GameCube 0x7DC */
    /* 0xA34 */ LIGHT_INFLUENCE mLightInfluence1;       /* GameCube 0x7F0 */
    /* 0xA58 */ LIGHT_INFLUENCE mLightInfluence2;       /* GameCube 0x810 */
    /* 0xA7C */ cXyz field_0x830;                         /* GameCube 0x830 */
    /* 0xA88 */ be<s16> mHeadAngleY;  /* was field_0x83C: lookBack 023075B0 stores the head Y angle; _nodeCB_Head 023020F8 Y-rotates the head by -it */
    /* 0xA8A */ be<s16> mHeadAngleZ;  /* was field_0x83E: lookBack 023075B0 stores the head X angle; _nodeCB_Head 023020F8 Z-rotates the head by -it */
    /* 0xA8C */ be<s16> mEyeAngleY;  /* was field_0x840: lookBack 023075B0 stores the head Y angle; eye_ctrl 02304310 moves the eye texture by it / div * 0.1 */
    /* 0xA8E */ be<s16> mEyeAngleX;  /* was field_0x842: lookBack 023075B0 stores the head X angle; eye_ctrl 02304310 moves the eye texture by it / div * 0.1 */
    /* 0xA90 */ be<s8> mActIdx;                           /* GameCube 0x844 */
    /* 0xA91 */ be<u8> mAnmAtrNo;  /* was field_0x845: setAnm_ATR 02306458 plays a_anm_prm_tbl[it]; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 reset it to 0xFF */
    /* 0xA92 */ be<u8> mMesgAnimeTag;  /* was field_0x846: anmAtr 02308AC0: the message's animation tag (dComIfGp_getMesgAnimeTagInfo) when it changes; checkOrder 023055A4 / endEvent 02306280 / setStt 02306184 0xFF (tww the same); same code as daNpc_Hi1_c::mMesgAnimeTag (tww daNpc_Ls1_c mMesgAnimeTag) */
    /* 0xA93 */ be<s8> mBtpNum;  /* was field_0x847: setBtp 023024F4 stores the btp number; play_btp_anm 02304268 blinks only while it is not 1; play_animation 023045E0 switches on it; demo 02305778 0x11 (tww the same); same code as daNpc_Hi1_c::mBtpNum (tww daNpc_Ls1_c mBtpNum) */
    /* 0xA94 */ be<s8> mBtkNum;  /* was field_0x848: setBtk 02302984 stores the btk number; play_animation 023045E0 switches on it (eye control while 1); demo 02305778 10 (tww the same); same code as daNpc_Hi1_c::mBtkNum (tww daNpc_Ls1_c mBtkNum) */
    /* 0xA95 */ be<s8> mAnmNo;  /* was field_0x849: setAnm_anm 0230607C: skips when the entry's animation equals it, else stores the entry's animation number; createInit 0230503C sets 0x11 */
    /* 0xA96 */ be<s8> mOrderEvent;  /* was field_0x84A: eventOrder 02307988: 1 / 2 the talk (1 orders the speak event), 3+ orders mEventIdx[it - 3] (mEventNo); checkOrder 023055A4 clears it once that event or the talk starts; init_ZL1_5 02303E98, wait_1 02309394, demo_2..4, optn_1 / optn_3, setStt 02306184 set it (tww the same); same code as daNpc_Hi1_c::mOrderEvent */
    /* 0xA97 */ be<s8> mStt;  /* was field_0x84B: setStt 02306184 stores the new state (and switches on it); setAnm 02306114 plays a_anm_prm_tbl[it] */
    /* 0xA98 */ be<s8> mPrevStt;  /* was field_0x84C: setStt 02306184 keeps the previous mStt here; talk_1 02309478 returns to it (setStt(it)) */
    /* 0xA99 */ be<s8> mLookMode;  /* was field_0x84D: lookBack 023075B0 switches on it: 1 the player's eyes, 2 mLookPos, 3 the angle at 0xA04, 4 the partner, 5 kyorokyoro (0 while mAnmNo is 0xD / 0xE); chngAnmAtr 0230883C, wait_1 02309394, optn_1 / optn_2, cut_init_LOK_PLYER 023062C4 / LOK_PARTNER 023063A0 / MAJYU_START 02306624 set it (tww the same); same code as daNpc_Hi1_c::mLookMode */
    /* 0xA9A */ be<s8> mHeapSizeIdx;  /* was mType (misnamed; tww field_0x84E): decideType 023037F4 returns while it is > 0, then sets it to 1 (the type argument goes to the next byte); _create 023052B4 creates the solid heap with a_siz_tbl[it] (tww decideType / _create the same); same code as daNpc_Hi1_c::mHeapSizeIdx */
    /* 0xA9B */ be<s8> mType;  /* was field_0x84F: decideType 023037F4 stores the type argument (0-7, else -1) and picks the archive by it (Zl2 for 0 / 6 / 7, else Zl); createInit 0230503C / partner_search 02305508 / getMsg 02308D08 switch on it, itemCreateHeap 02303400, wait_1 02309394, demo_1 023095E4, wait_action1 02309CEC, demo_action1 02309E10, demo 02305778, _draw 02308090 test it (tww decideType the same); after daNpc_Hi1_c::mType (same code) */
    /* 0xA9C */ be<s8> mActionStatus;  /* was field_0x850: set_action 023038B8 sets 9 for the leaving action and 0 for the new one; wait_action1 02309CEC / demo_action1 02309E10 / demo_action2 02309EB8 / optn_action1 02309FC8 switch on it and step it (it + 1); partner_search 02305508 tests 1 and steps it (tww the same); same code as daNpc_Hi1_c::mActionStatus */
    /* 0xA9D */ be<s8> mAnmAtrSetCnt;  /* was field_0x851: anmAtr 02308AC0: while the message types, applies the message's animation attribute (chngAnmAtr) only when it is 0, then adds 1; MSG_DISPLAYED (0xE) and setStt 02306184 reset it (tww the same); same code as daNpc_Hi1_c::mAnmAtrSetCnt */
    /* 0xA9E */ u8 _A9E[2];
    /* 0xAA0 */ gptr<J3DJoint> mJoint1;                   /* GameCube 0x854 */
    /* 0xAA4 */ gptr<J3DJoint> mJoint2;
    /* 0xAA8 */ gptr<J3DJoint> mJoint3;
    /* 0xAAC */ be<u32> field_0x860[6];                   /* J3DMaterial* */
    /* 0xAC4 */ be<u32> field_0x878[6];                   /* J3DMaterial* */
    /* 0xADC */ be<u32> field_0x890[6];                   /* J3DShape* */
    /* 0xAF4 */ be<u32> field_0x8A8[6];                   /* J3DShape* */
    /* 0xB0C */ be<s32> mHeadMaterialIndex;                             /* HD: material index (bodyCreateHeap) */
    /* 0xB10 */ be<s32> mHeadOverlayMaterialIndex;                             /* HD: material index (bodyCreateHeap) */
    /* 0xB14 */ u8 mOffCupOnAupPacket1[0x98];             /* GameCube 0x8C0 (0x10) */
    /* 0xBAC */ u8 mOffCupOnAupPacket2[0x98];
    /* 0xC44 */ u8 mOnCupOffAupPacket1[0x98];
    /* 0xCDC */ u8 mOnCupOffAupPacket2[0x98];
};
WWHD_OFFSET(daNpc_Zl1_c, mArcName, 0x7F4);
WWHD_OFFSET(daNpc_Zl1_c, mBtkAnm, 0x7FC);
WWHD_OFFSET(daNpc_Zl1_c, mBtpAnm, 0x874);
WWHD_OFFSET(daNpc_Zl1_c, mBtpAnmFrame, 0x95C);
WWHD_OFFSET(daNpc_Zl1_c, mCurrActionFunc, 0x960);
WWHD_OFFSET(daNpc_Zl1_c, mProcId1, 0x968);
WWHD_OFFSET(daNpc_Zl1_c, mEyePos, 0x990);
WWHD_OFFSET(daNpc_Zl1_c, mRatio, 0x9CC);
WWHD_OFFSET(daNpc_Zl1_c, mbSetEyePos, 0x9E0);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7B8, 0x9FC);
WWHD_OFFSET(daNpc_Zl1_c, field_0x7C2, 0xA06);
WWHD_OFFSET(daNpc_Zl1_c, mStateIsComplaete, 0xA13);
WWHD_OFFSET(daNpc_Zl1_c, mInDemo, 0xA1D);
WWHD_OFFSET(daNpc_Zl1_c, mRippleCallBack, 0xA20);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence1, 0xA34);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence2, 0xA58);
WWHD_OFFSET(daNpc_Zl1_c, field_0x830, 0xA7C);
WWHD_OFFSET(daNpc_Zl1_c, mOrderEvent, 0xA96);
WWHD_OFFSET(daNpc_Zl1_c, mType, 0xA9B);
WWHD_OFFSET(daNpc_Zl1_c, mJoint1, 0xAA0);
WWHD_OFFSET(daNpc_Zl1_c, mHeadMaterialIndex, 0xB0C);
WWHD_OFFSET(daNpc_Zl1_c, mOffCupOnAupPacket1, 0xB14);
WWHD_SIZE(daNpc_Zl1_c, 0xD74);

/* daNpc_Zl1_HIO_c: HD vtable-first layout. */
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



#include "gc_api/tu.h"

namespace zl1Heap { inline u32 jointNames(J3DModelData* data); }
namespace zl1Heap {
template <class T> struct RelativeOffset {
    be<u32> mOffset;
    T* get() const {
        u32 offset = mOffset;
        return offset ? gabi::at<T>(gabi::ea(this) + offset) : nullptr;
    }
};
struct ResourceHeader {
    u8 mPad[0x10];
    RelativeOffset<void> mJointNames;
    u8 mPad14[4];
    RelativeOffset<void> mMaterialNames;
    u8 mPad1C[0x18];
    RelativeOffset<u8> mTexMatrices;
    RelativeOffset<void> mTexMatrixNames;
};
struct Material {
    gptr<ResourceHeader> mpResource;
    be<u16> mFlags;
    u8 mPad06[2];
    be<u32> mShape;
    gptr<be<u32>> mpDirtyBits;
    u8 mPad10[0x14];
    gptr<daNpc_Zl1_matAnm_c> mpMaterialAnm;
    gptr<u8> mpTextureSRT;
    u8 mPad2C[0x370];
};
struct Joint {
    be<u32> mData;
    u8 mPad04[4];
    be<u32> mCallback;
    u8 mPad0C[0x10];
};
struct ModelData {
    gptr<ResourceHeader> mpResource;
    be<u32> mJointCount;
    gptr<Joint> mpJoints;
    be<u32> mMaterialCount;
    gptr<Material> mpMaterials;
};
struct BtpEntry {
    u8 mPad[0xC];
    RelativeOffset<const char> mName;
    u8 mPad10[0xC];
};
struct BtpResource {
    u8 mPad[0x16];
    be<u16> mUpdateMaterialCount;
    u8 mPad18[0x14];
    RelativeOffset<BtpEntry> mMaterials;
};
struct BtpAnimation {
    u8 mPad[0x54];
    gptr<be<u32>> mpMaterialLinks;
    u8 mPad58[8];
    gptr<BtpResource> mpAnimation;
};
struct BtkMaterialList { u8 mPad[0x14]; be<u16> mCount; };
struct BtkResource { u8 mPad[0xC]; gptr<BtkMaterialList> mpMaterials; };
struct BtkAnimation { u8 mPad[0x68]; gptr<BtkResource> mpAnimation; };
struct TexMatrixEntry {
    u8 mPad[2];
    be<u16> mSRTOffset;
    be<s32> mAnimationIndex;
    u8 mPad08[4];
    be<u16> mRelatedMatrix;
    u8 mPad0E[6];
};
struct TextureSRT { u8 mPad[0x10]; be<f32> mTranslationX; be<f32> mTranslationY; };
struct Packet { u8 mPad[0xC]; be<u32> mVtable; };
struct SafeStringVtable { u8 mPad[0x14]; be<u32> mAssureTermination; };

inline ModelData* modelData(J3DModelData* data) { return reinterpret_cast<ModelData*>(data); }
inline Material* materialByIndex(J3DModelData* data, s32 index) {
    if (index < 0) return nullptr;
    ModelData* view = modelData(data);
    Material* material = view->mpMaterials.get();
    if ((u32)index < view->mMaterialCount) material += index;
    return material;
}
inline void assure(SafeString* string) {
    auto* vtable = gabi::at<SafeStringVtable>(string->__vtbl);
    gabi::call_ptr(vtable->mAssureTermination, string);
}
inline BtpAnimation* btp(void* animation) { return reinterpret_cast<BtpAnimation*>(animation); }
inline BtkAnimation* btk(void* animation) { return reinterpret_cast<BtkAnimation*>(animation); }
inline Packet* packet(void* value) { return reinterpret_cast<Packet*>(value); }
inline TexMatrixEntry* texMatrix(ResourceHeader* resource, s32 index) {
    return reinterpret_cast<TexMatrixEntry*>(resource->mTexMatrices.get()) + index;
}
constexpr u32 nodeCB_Head = 0x02302278;
constexpr u32 nodeCB_BackBone = 0x023023E4;
constexpr u32 CheckCreateHeap = 0x023035B8;
constexpr u32 materialAnmVtable = 0x10024298;
constexpr u32 hioVtable = 0x10023C18;
constexpr u32 offCupVtable = 0x10058D10;
constexpr u32 onCupVtable = 0x10058D40;
}
WWHD_OFFSET(zl1Heap::ResourceHeader, mMaterialNames, 0x18);
WWHD_OFFSET(zl1Heap::ResourceHeader, mTexMatrices, 0x34);
WWHD_SIZE(zl1Heap::ResourceHeader, 0x3C);
WWHD_OFFSET(zl1Heap::Material, mpMaterialAnm, 0x24);
WWHD_OFFSET(zl1Heap::Material, mpTextureSRT, 0x28);
WWHD_SIZE(zl1Heap::Material, 0x39C);
WWHD_SIZE(zl1Heap::ModelData, 0x14);
WWHD_SIZE(zl1Heap::BtpEntry, 0x1C);
WWHD_OFFSET(zl1Heap::BtpResource, mMaterials, 0x2C);
WWHD_OFFSET(zl1Heap::BtpAnimation, mpAnimation, 0x60);
WWHD_SIZE(zl1Heap::TexMatrixEntry, 0x14);
WWHD_OFFSET(zl1Heap::TextureSRT, mTranslationX, 0x10);
WWHD_OFFSET(zl1Heap::Packet, mVtable, 0xC);

void zl1Heap_strcpy(void* destination, const char* source);
s32 zl1Heap_nameIndex(u32 table, const char* name);
u32 zl1Heap_btkResID(daNpc_Zl1_c* actor, s32 number);
u32 zl1Heap_btpResID(daNpc_Zl1_c* actor, s32 number);
u32 zl1Heap_materialNameHeader(u32 data);
u32 zl1Heap_patternNameHeader(J3DModelData* data);
void zl1Heap_lightPriority(LIGHT_INFLUENCE* light);
void zl1Heap_effectLight(LIGHT_INFLUENCE* light);
void zl1Heap_npcConstructor(daNpc_Zl1_c* actor);
void zl1Heap_btkConstructor(void* animation);
void zl1Heap_btpConstructor(void* animation);
void zl1Heap_rippleConstructor(void* callback);
void zl1Heap_packetConstructor(void* packet);
void zl1Heap_packetDestructor(void* packet, s32 flags);
void zl1Heap_circleDestructor(u32 circle, s32 flags);
void zl1Heap_actorDestructor(daNpc_Zl1_c* actor, s32 flags);
void zl1Heap_destroyArray(u32 array, s32 count, s32 stride, u32 destructor, s32 flag1, s32 flag2);

namespace zl1Heap {
struct HeaderStatics { be<f32> mNegativePi; be<f32> mPi; be<u8> mFirst; be<u8> mSecond; };
struct DestructorRecord { be<u32> mWords[3]; };
inline J3DJoint* jointByName(J3DModelData* data, const char* name) {
    s32 result = gabi::call<s32>(0x027DF9B0, jointNames(data), name);
    u32 index = (u16)result;
    ModelData* view = modelData(data);
    Joint* joint = view->mpJoints.get();
    if (index < view->mJointCount) joint += index;
    return reinterpret_cast<J3DJoint*>(joint);
}
inline void setJointCallback(daNpc_Zl1_c* actor, s8 number, u32 callback) {
    ModelData* view = modelData(actor->mpMorf->getModel()->getModelData());
    u32 index = (u16)(s16)number;
    u32 count = view->mJointCount;
    Joint* joint = view->mpJoints.get();
    if (index < count) joint += index;
    joint->mCallback = callback;
}
}
void zl1Heap_headerFirst(void* object);
void zl1Heap_headerSecond(void* object);

namespace zl1Heap {
inline const char* text(u32 address) { return gabi::at<const char>(address); }
inline const be<u8>* bytes(u32 address) { return gabi::at<const be<u8>>(address); }
inline ResourceHeader* resourceHeader(u32 address) { return gabi::at<ResourceHeader>(address); }
inline void destroyAcch(dBgS_ObjAcch* acch) {
    acch->dBgS_GrpPassChk::__vtbl = 0x10023BB8;
    acch->dBgS_PolyPassChk::__vtbl = 0x10023BC8;
    gabi::call(0x024EFD9C, acch, 0);
}
enum { Branch = 0x1A9, Bm1 = 0x146, SearchMax = 20 };
}

namespace zl1Heap {
inline void* objectResource(const char* archive, const char* name) {
    gabi::Local<SafeString> arc;
    gabi::Local<SafeString> resource;
    arc->mTextPtr = gabi::ea(archive);
    arc->__vtbl = ZL1_SAFESTRING_VTBL;
    resource->mTextPtr = gabi::guest_addr(name);
    resource->__vtbl = ZL1_SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), arc.get(), resource.get());
}
constexpr u32 actorVtable = 0x100242B0;
}
s32 zl1Heap_btpInit(void* animation, J3DModelData* data, void* resource, s32 play,
                    s32 attribute, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
s32 zl1Heap_btkInit(void* animation, J3DModelData* data, void* resource, s32 play,
                    s32 attribute, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
u32 zl1Heap_jointHeader(J3DModelData* data);
s8 zl1Heap_jointNameIndex(u32 table, const char* name);
namespace zl1Heap {
inline u32 jointNames(J3DModelData* data) {
    ResourceHeader* header = resourceHeader(zl1Heap_jointHeader(data));
    return gabi::ea(header->mJointNames.get());
}
}

WWHD_OFFSET(zl1Heap::ModelData, mpJoints, 8);
WWHD_OFFSET(zl1Heap::ModelData, mpMaterials, 0x10);
WWHD_OFFSET(zl1Heap::BtpEntry, mName, 0xC);
WWHD_SIZE(zl1Heap::BtpResource, 0x30);
WWHD_SIZE(zl1Heap::BtpAnimation, 0x64);
WWHD_SIZE(zl1Heap::BtkMaterialList, 0x16);
WWHD_OFFSET(zl1Heap::BtkResource, mpMaterials, 0xC);
WWHD_SIZE(zl1Heap::BtkResource, 0x10);
WWHD_OFFSET(zl1Heap::BtkAnimation, mpAnimation, 0x68);
WWHD_SIZE(zl1Heap::BtkAnimation, 0x6C);
WWHD_SIZE(zl1Heap::TextureSRT, 0x18);
WWHD_SIZE(zl1Heap::Packet, 0x10);
WWHD_OFFSET(zl1Heap::SafeStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(zl1Heap::SafeStringVtable, 0x18);
WWHD_SIZE(zl1Heap::HeaderStatics, 0xA);
WWHD_SIZE(zl1Heap::DestructorRecord, 0xC);
WWHD_OFFSET(daNpc_Zl1_HIO_c, mPrmTbl, 0xC);
WWHD_SIZE(daNpc_Zl1_HIO_c::hio_prm_c, 0x58);

WWHD_OFFSET(daNpc_Zl1_matAnm_c, mOffset, 0x6C);
WWHD_OFFSET(daNpc_Zl1_c, mHeadOverlayMaterialIndex, 0xB10);

WWHD_OFFSET(zl1Heap::Joint, mCallback, 8);
WWHD_SIZE(zl1Heap::Joint, 0x1C);
