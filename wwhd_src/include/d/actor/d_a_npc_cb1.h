/* daNpc_Cb1_c (Makar, the Korok cellist), WWHD layout. 
 *
 * GameCube -> WWHD, measured from create 0221F700 (inlined constructor) and the profile
 * 0x101BDA48 (size 0xB40, GameCube 0x938):
 * - daPy_npc_c base: HD 0x608 (layout daPy_npc_l from d_a_npc_md.h; constructor 024450B4).
 * - mPhs/mpMorf: +0x11C (0x608/0x610);
 * - mShadowId (GameCube 0x4F8) is gone (HD shadows): the models are +0x118 (0x614..0x624);
 * - mDoExt_bckAnm grew from 0x10 to 0x8C (+0x7C each): mAcchCir..mJntCtrl..mPrevAnmFrame are +0x210;
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
    /* 0xA9C */ cXyz mSlideSpeed;  /* was m88C: execute 0222091C adds its x/z to current.pos each frame and eases it (cLib_addCalcPosXZ) toward the ground normal's x/z * 40 on slide-code ground, else toward zero (tww current.pos.x += it.x; cLib_addCalcPosXZ(&it, temp2, ..)) */
    /* 0xAA8 */ cXyz mNutPos;
    /* 0xAB4 */ cXyz mNusSpeed;
    /* 0xAC0 */ be<f32> mPrevAnmFrame;  /* was m8B0: playAnm 02220024: a morf frame below it means the animation wrapped; it then takes the current frame (tww if (r3 || mpMorf->getFrame() < it) ..; it = mpMorf->getFrame()); initAnm 0221FEFC 0 */
    /* 0xAC4 */ ProcFunc_l mPlayerAction;                 /* GameCube 0x8B4 (12 bytes) */
    /* 0xACC */ ProcFunc_l mNpcAction;                    /* GameCube 0x8C0 */
    /* 0xAD4 */ be<u32> mMsgNo;                           /* GameCube 0x8CC */
    /* 0xAD8 */ be<s16> mLookAtMaxVel;  /* was m8D0: lookBack 02221EF8 eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel (tww mJntCtrl.lookAtTarget(.., desiredY, it, 0)); after tww daNpc_Bm1_c / daNpc_Ah_c mLookAtMaxVel */
    /* 0xADA */ be<s8> m_backbone_jnt_num;
    /* 0xADB */ be<s8> m_armRend_jnt_num;
    /* 0xADC */ be<s8> m_armL2_jnt_num;
    /* 0xADD */ be<s8> m_nut_jnt_num;
    /* 0xADE */ be<s8> m_center_jnt_num;
    /* 0xADF */ be<s8> mAnmLooped;  /* was m8D7: playAnm 02220024 sets 1 when the animation wraps; initAnm 0221FEFC 0; flyAction 022222D0 / evActSow 022265AC act once it is set (tww) */
    /* 0xAE0 */ be<s8> mAnmLoopCount;  /* was m8D8: playAnm 02220024 adds 1 per wrap and switches the animation once it reaches the table's loop count (tww it++; if (it >= data.field_0x04)); initAnm 0221FEFC 0 */
    /* 0xAE1 */ be<u8> mAttnSetCount;
    /* 0xAE2 */ be<u8> mHasAttention;
    /* 0xAE3 */ be<u8> mAnmPrmIdx;  /* was m8DB: setAnm 02221978 stores the ANM_ number and indexes anmPrmData with it, skipping a repeat (tww if (it == param_1) ..; it = param_1; &anmPrmData[it]); create 0221F700 0xFF (tww constructor) */
    /* 0xAE4 */ be<s8> mAnmTblIdx;  /* was m8DC: initAnm 0221FEFC stores the animation table index; playAnm 02220024 reads anmTblData[it] (tww); flyAction 022222D0 / draw 022213BC / waitNpcAction 02223048 test it; same name as daNpc_Md_c::mAnmTblIdx (npcs.md) */
    /* 0xAE5 */ be<s8> mOrderEvent;  /* was m8DD: eventOrder 02220884: 0-4 order mEventIdx[it], 5/6 the talk, 7 the talk-with-item (tww); eventProc 02220288 / checkOrder 02220738 / checkCommandTalk 02220194 reset it to -1 once accepted; init 0221F3C0 -1; the actions set it */
    /* 0xAE6 */ be<u8> mSearchSide;  /* was m8DE: searchNpcAction 022244A0 flips it (^= 1) and turns by -/+ HIO field_0xE6 by it (tww it ^= 1; temp += it ? -l_HIO.field_0xE6 : ..) */
    /* 0xAE7 */ be<u8> mSearchTimer;  /* was m8DF: searchNpcAction 022244A0: counted down (cLib_calcTimer) and reloaded with cLib_getRndValue(HIO field_0xE2, field_0xE4) at 0 (tww) */
    /* 0xAE8 */ be<u8> mGroundHit;  /* was m8E0: execute 0222091C: mAcch.ChkGroundHit() taken before CrrPos; while set, the moving-ground correction and the slope fall speed apply (tww); routeCheck 0222403C 1; flyAction 022222D0 / evActWalk 02225CB0 test it (cb1_execRoom, inlined into execute) */
    /* 0xAE9 */ be<s8> mMoveBlockSide;  /* was m8E1: execute 0222091C: chkMoveBlock(&mMoveBlockSpeed) (tww; daPy_npc_c::chkMoveBlock returns -1 / 1 for the side of a pushed block coming at it, else 0); searchNpcAction 022244A0 / waitNpcAction 02223048 step aside by its sign */
    /* 0xAEA */ be<u8> mDoStatus;  /* was m8E2: execute 0222091C: dComIfGp_getDoStatus() (tww it = dComIfGp_getDoStatus()), else 0; sowCheck 02222EF4 tests != 0x2D */
    /* 0xAEB */ be<s8> mOrderedEvent;  /* was m8E3: eventOrder 02220884 copies mOrderEvent and orders mEventIdx[it] (tww); eventProc 02220288 ends the event when dComIfGp_evmng_endCheck(mEventIdx[it]) (tww); evInitOffsetLink 02225B24 / evInitSetGoal 022266DC test 0; init 0221F3C0 -1 */
    /* 0xAEC */ be<s16> mEventIdx[5];
    /* 0xAF6 */ be<s16> mEvtTimer;  /* was m8EE: evInitWait 02225798 / evInitSow 022264E4 load the cut's Timer; evActWait 02225820 / evActSow 022265AC count it to 0 (tww cLib_chaseS(&it, 0, 1), cLib_calcTimer(&it)); evInitWarp 02226884 / evActWarp 022268E0 (tww) */
    /* 0xAF8 */ be<s8> mActionStatus;  /* was m8F0: setAction 0221F140 sets -1 for the leaving action and 0 for the new one; the actions step it (tww it++, if (it == 0) ..); after tww daNpc_Bs1_c / daNpc_Bmsw_c mActionStatus */
    /* 0xAF9 */ be<s8> mFlyLiftCycle;  /* was m8F1: flyNpcAction 02223A88: flyAction gets lift (param_1 TRUE) only when it is 0, then it = (it + 1) % HIO field_0xDE (tww); setAction 0221F140 0 */
    /* 0xAFA */ be<s8> m8F2;
    /* 0xAFB */ be<u8> mSetGoalWaitCount;  /* was m8F3: WWHD only (no tww use): evActSetGoal 02226784 adds 1 while Link is more than 50 from the goal and puts Link there once it reaches 150 */
    /* 0xAFC */ be<s16> mCarryAngle;  /* was m8F4: carryNpcAction 02223694: the angle to the carrying player's shape angle, chased to 0 (tww it = fopAcM_toPlayerShapeAngleY(this); cLib_chaseAngleS(&it, 0, 0x800); shape_angle.y = player->shape_angle.y + it); setAction 0221F140 0 */
    /* 0xAFE */ be<s16> mLandIdleCount;  /* was m8F6: flyAction 022222D0: counts frames on the ground with the propeller at or below HIO field_0xD2 and ends the flight past HIO field_0xCC (tww ++it > l_HIO.field_0xCC); 0 while lifting; setAction 0221F140 0 */
    /* 0xB00 */ be<s16> mPropellerSpeed;  /* was m8F8: flyAction 022222D0: chased up to HIO field_0xD0 while lifting, down otherwise, added to mPropellerAngle and scales JA_SE_CM_PRAPELLO_ROLLING (tww cLib_chaseS(&it, ..); m8FA += it; fopAcM_seStart(.., it * (100.0f / l_HIO.field_0xD0))); flyNpcAction 02223A88 20000; setAction 0221F140 0 */
    /* 0xB02 */ be<s16> mPropellerAngle;  /* was m8FA: flyAction 022222D0 adds mPropellerSpeed each frame (tww it += m8F8); ppNodeCallBack 0221E5BC turns the propeller joint by it (TWW getWork3()); setAction 0221F140 0 */
    /* 0xB04 */ be<f32> mFlyHeight;  /* was m8FC: flyAction 022222D0 adds the frame's rise (tww it += current.pos.y - old.pos.y) and tests it against HIO field_0xD8 / field_0xA4; flyNpcAction 02223A88 HIO field_0xD8 + 1; execute 0222091C (cb1_execRoom) > -400; setAction 0221F140 0 */
    /* 0xB08 */ be<f32> mFallDist;  /* was m900: execute 0222091C adds the frame's drop (tww it += temp4 - current.pos.y); checkLanding 0221FCE4 tests > 200; flyAction / jumpNpcAction 02224AC0 / jumpPlayerAction 02225464 / flyNpcAction 02223A88 reset it to 0 */
    /* 0xB0C */ cXyz mCarryOffset;  /* was m904: carryNpcAction 02223694 chases it to HIO field_0x74/78/7C (tww cLib_chaseF(&it.x, ..)); setBaseMtx 0221ED08 translates the model by it (tww mDoMtx_stack_c::transM(it.x, it.y, it.z)) */
    /* 0xB18 */ cXyz mGroundNormal;  /* was m910: execute 0222091C (cb1_execRoom): the ground triangle's normal (tww it = pla->mNormal); sets the slope fall speed and the slide speed (tww); routeAngCheck 02223E64 crosses it with the route direction */
    /* 0xB24 */ cXyz mMoveBlockSpeed;  /* was m91C: execute 0222091C: chkMoveBlock's out vector (tww chkMoveBlock(&it); daPy_npc_c::chkMoveBlock(cXyz* outBlockVel) = block current.pos - old.pos); searchNpcAction turns by its atan2 (tww cM_atan2s(it.x, it.z)) */
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
WWHD_OFFSET(daNpc_Cb1_c, mAnmPrmIdx, 0xAE3);
WWHD_OFFSET(daNpc_Cb1_c, mPolyInfo, 0xB30);
WWHD_SIZE(daNpc_Cb1_c, 0xB40);
