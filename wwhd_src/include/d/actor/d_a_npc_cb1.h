/* daNpc_Cb1_c (Makar, the Korok cellist), WWHD layout. 
 *
 * GameCube -> WWHD, measured from create 0221F700 (inlined constructor) and the profile
 * 0x101BDA48 (size 0xB40, GameCube 0x938):
 * - daPy_npc_c base: HD 0x608 (layout daPy_npc_l from d_a_npc_md.h; constructor 024450B4).
 * - mPhs/mpMorf: +0x11C (0x608/0x610);
 * - mShadowId (GameCube 0x4F8) is gone (HD shadows): the models are +0x118 (0x614..0x624);
 * - mDoExt_bckAnm grew from 0x10 to 0x8C (+0x7C each): mAcchCir..mJntCtrl..m8B0 are +0x210;
 * - the two pointers to member are 8 bytes (GHS) instead of 12: mMsgNo..mPolyInfo are +0x208.
 * Field names are the GameCube ones (mNNN = GameCube offset).
 *
 * Coordination: this header holds the layout and the class declaration only. Each part
 * file keeps its own statics/bindings locally; add a member declaration here only with a small
 * python replace of one line. */
#pragma once
#include "d/actor/d_a_npc_md.h" /* daPy_npc_l, ProcFunc_l, cBgS_PolyInfo_l */

#define CB1_VTBL 0x100191F0 /* daNpc_Cb1_c vtable (create 0221F700, stored at +0xB4) */

struct daNpc_Cb1_c : daPy_npc_l {
    enum daNpc_Cb1_StatusBit_e {
        daCbStts_NO_CARRY_ACTION = 0x0001,
        daCbStts_TACT = 0x0002,
        daCbStts_TACT_CORRECT = 0x0004,
        daCbStts_TACT_CANCEL = 0x0008,
        daCbStts_MUSIC = 0x0010,
        daCbStts_NUT = 0x0020,
        daCbStts_SHIP_RIDE = 0x0040,
        daCbStts_PLAYER_FIND = 0x0080,
        daCbStts_UNK_0100 = 0x0100,
    };

    /* member functions (GameCube signatures; pointers to member are ProcFunc_l) */
    BOOL isTagCheckOK();
    void setMessageAnimation(u8);
    cPhs_State create();
    BOOL createHeap();
    BOOL setAction(ProcFunc_l*, ProcFunc_l*, void*);
    void setWaitAction(void*);
    void setWaitNpcAction(void*);
    void npcAction(void*);
    void setNpcAction(ProcFunc_l*, void*);
    void playerAction(void*);
    void setPlayerAction(ProcFunc_l*, void*);
    s16 getStickAngY();
    int calcStickPos(s16, cXyz*);
    BOOL flyCheck();
    void checkLanding();
    f32 breaking();
    BOOL flyAction(BOOL, f32, s16, BOOL);
    BOOL walkAction(f32, f32, s16);
    void returnLinkPlayer();
    BOOL isFlyAction();
    BOOL sowCheck();
    BOOL shipRideCheck();
    BOOL eventProc();
    void evCheckDisp(int);
    void evInitWait(int);
    BOOL evActWait(int);
    void evInitMsgSet(int);
    BOOL evActMsgSet(int);
    void evInitMsgEnd(int);
    BOOL evActMsgEnd(int);
    void evInitMovePos(int);
    BOOL evActMovePos(int);
    void evInitOffsetLink(int);
    BOOL evActOffsetLink(int);
    void evInitWalk(int);
    BOOL evActWalk(int);
    void evInitToLink(int);
    BOOL evActToLink(int);
    u32 evInitTact(int);
    BOOL evActTact(int);
    u32 evInitCelloPlay(int);
    BOOL evActCelloPlay(int);
    void evInitTurn(int);
    BOOL evActTurn(int);
    void evInitSow(int);
    BOOL evActSow(int);
    void evInitSetAnm(int);
    BOOL evActSetAnm(int);
    void evInitSetGoal(int);
    BOOL evActSetGoal(int);
    void evInitWarp(int);
    BOOL evActWarp(int);
    void evInitEnd(int);
    BOOL evActEnd(int);
    u8 getAnmType(int);
    BOOL initTalk();
    BOOL execTalk(BOOL);
    BOOL waitNpcAction(void*);
    BOOL talkNpcAction(void*);
    BOOL carryNpcAction(void*);
    BOOL flyNpcAction(void*);
    void routeAngCheck(cXyz*, be<s16>*); /* GameCube cXyz&, s16* */
    void routeWallCheck(cXyz*, cXyz*, be<s16>*); /* GameCube cXyz&, cXyz&, s16* */
    f32 checkForwardGroundY(s16);
    f32 checkWallJump(s16);
    BOOL chkWallHit();
    BOOL routeCheck(f32, be<s16>*);
    BOOL searchNpcAction(void*);
    BOOL hitNpcAction(void*);
    BOOL jumpNpcAction(void*);
    BOOL rescueNpcAction(void*);
    BOOL musicNpcAction(void*);
    BOOL shipNpcAction(void*);
    BOOL waitPlayerAction(void*);
    BOOL walkPlayerAction(void*);
    BOOL hitPlayerAction(void*);
    BOOL jumpPlayerAction(void*);
    BOOL flyPlayerAction(void*);
    BOOL carryPlayerAction(void*);
    BOOL calcFlyingTimer();
    void initAnm(s8, BOOL);
    void musicPlay();
    void musicStop();
    BOOL setAnm(u8);
    void playAnm();
    BOOL chkAttention(f32, s32);
    void carryCheck();
    void eventOrder();
    void checkOrder();
    BOOL checkCommandTalk();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setCollision();
    void lookBack(BOOL);
    void setBaseMtx();
    BOOL init();
    BOOL draw();
    BOOL execute();

    /* 0x608 */ request_of_phase_process_class mPhs;     /* GameCube 0x4EC */
    /* 0x610 */ gptr<mDoExt_McaMorf> mpMorf;
    /* HD: no mShadowId (GameCube 0x4F8) */
    /* 0x614 */ gptr<J3DModel> mpStickModel;              /* GameCube 0x4FC */
    /* 0x618 */ gptr<J3DModel> mpCelloModel;
    /* 0x61C */ gptr<J3DModel> mpFaceModel;
    /* 0x620 */ gptr<J3DModel> mpPropellerModel;
    /* 0x624 */ gptr<J3DModel> mpNutModel;
    /* 0x628 */ mDoExt_bckAnm mPropellerBckAnim;          /* GameCube 0x510 (HD 0x8C) */
    /* 0x6B4 */ mDoExt_bckAnm mNutBckAnim;                /* GameCube 0x520 */
    /* 0x740 */ dBgS_AcchCir mAcchCir[2];                 /* GameCube 0x530 */
    /* 0x7C0 */ dCcD_Stts mStts;                          /* GameCube 0x5B0 */
    /* 0x7FC */ dCcD_Cyl mCyl;                            /* GameCube 0x5EC */
    /* 0x92C */ dCcD_Cyl mWindCyl;                        /* GameCube 0x71C */
    /* 0xA5C */ dNpc_JntCtrl_c mJntCtrl;                  /* GameCube 0x84C */
    /* 0xA90 */ cXyz mEyePos;                             /* GameCube 0x880 */
    /* 0xA9C */ cXyz m88C;
    /* 0xAA8 */ cXyz mNutPos;
    /* 0xAB4 */ cXyz mNusSpeed;
    /* 0xAC0 */ be<f32> m8B0;
    /* 0xAC4 */ ProcFunc_l mPlayerAction;                 /* GameCube 0x8B4 (12 bytes) */
    /* 0xACC */ ProcFunc_l mNpcAction;                    /* GameCube 0x8C0 */
    /* 0xAD4 */ be<u32> mMsgNo;                           /* GameCube 0x8CC */
    /* 0xAD8 */ be<s16> m8D0;
    /* 0xADA */ be<s8> m_backbone_jnt_num;
    /* 0xADB */ be<s8> m_armRend_jnt_num;
    /* 0xADC */ be<s8> m_armL2_jnt_num;
    /* 0xADD */ be<s8> m_nut_jnt_num;
    /* 0xADE */ be<s8> m_center_jnt_num;
    /* 0xADF */ be<s8> m8D7;
    /* 0xAE0 */ be<s8> m8D8;
    /* 0xAE1 */ be<u8> mAttnSetCount;
    /* 0xAE2 */ be<u8> mHasAttention;
    /* 0xAE3 */ be<u8> m8DB;
    /* 0xAE4 */ be<s8> m8DC;
    /* 0xAE5 */ be<s8> m8DD;
    /* 0xAE6 */ be<u8> m8DE;
    /* 0xAE7 */ be<u8> m8DF;
    /* 0xAE8 */ be<u8> m8E0;
    /* 0xAE9 */ be<s8> m8E1;
    /* 0xAEA */ be<u8> m8E2;
    /* 0xAEB */ be<s8> m8E3;
    /* 0xAEC */ be<s16> mEventIdx[5];
    /* 0xAF6 */ be<s16> m8EE;
    /* 0xAF8 */ be<s8> m8F0;
    /* 0xAF9 */ be<s8> m8F1;
    /* 0xAFA */ be<s8> m8F2;
    /* 0xAFB */ be<u8> m8F3;
    /* 0xAFC */ be<s16> m8F4;
    /* 0xAFE */ be<s16> m8F6;
    /* 0xB00 */ be<s16> m8F8;
    /* 0xB02 */ be<s16> m8FA;
    /* 0xB04 */ be<f32> m8FC;
    /* 0xB08 */ be<f32> m900;
    /* 0xB0C */ cXyz m904;
    /* 0xB18 */ cXyz m910;
    /* 0xB24 */ cXyz m91C;
    /* 0xB30 */ cBgS_PolyInfo_l mPolyInfo;                /* GameCube 0x928 */
};
WWHD_OFFSET(daNpc_Cb1_c, mPhs, 0x608);
WWHD_OFFSET(daNpc_Cb1_c, mpStickModel, 0x614);
WWHD_OFFSET(daNpc_Cb1_c, mPropellerBckAnim, 0x628);
WWHD_OFFSET(daNpc_Cb1_c, mAcchCir, 0x740);
WWHD_OFFSET(daNpc_Cb1_c, mStts, 0x7C0);
WWHD_OFFSET(daNpc_Cb1_c, mCyl, 0x7FC);
WWHD_OFFSET(daNpc_Cb1_c, mWindCyl, 0x92C);
WWHD_OFFSET(daNpc_Cb1_c, mJntCtrl, 0xA5C);
WWHD_OFFSET(daNpc_Cb1_c, mPlayerAction, 0xAC4);
WWHD_OFFSET(daNpc_Cb1_c, mMsgNo, 0xAD4);
WWHD_OFFSET(daNpc_Cb1_c, m8DB, 0xAE3);
WWHD_OFFSET(daNpc_Cb1_c, mPolyInfo, 0xB30);
WWHD_SIZE(daNpc_Cb1_c, 0xB40);
