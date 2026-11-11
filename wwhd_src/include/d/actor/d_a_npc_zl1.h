/* daNpc_Zl1_c (Tetra), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_zl1 (constructor 02303728 allocates
 * 0xD74; GameCube 0x900):
 * - fopNpc_npc_c is 0x7DC (see d/d_npc.h): members +0x118 up to mArcName (0x7F4);
 * - mShadowId (GameCube 0x6E0) is gone (HD shadows): mBtkAnm (0x7FC) is +0x114;
 * - mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14 to 0x74, and HD adds a second btp
 *   animation (0x8E8, set up together with the first by setBtp): +0x174 for mBtkAnmFrame /
 *   mBtpAnm, +0x248 for mBtpAnmFrame / mTimer;
 * - the pointer-to-member mCurrActionFunc is 8 bytes (GHS) instead of 12: +0x244 from mProcId1
 *   up to mLightInfluence1;
 * - LIGHT_INFLUENCE grew from 0x20 to 0x24 (an HD f32 at +0x20, 1.0 by the constructor): +0x248
 *   for mLightInfluence2, +0x24C from mLightPos to mpMayuShape;
 * - HD: two s32 material indices at 0xB0C/0xB10 (bodyCreateHeap); the four
 *   mDoExt_*CupOn*AupPacket members grew from 0x10 to 0x98 each (0xB14..0xD74).
 * Field names are the GameCube ones (field_0xNNN = GameCube offset). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member) */

#define ZL1_SAFESTRING_VTBL 0x10023B10 /* this TU's sead::SafeString vtable */
#define ZL1_VTBL 0x100242B0           /* daNpc_Zl1_c vtable (constructor 02303728) */

/* daNpc_Zl1_matAnm_c : J3DMaterialAnm (HD size and layout as daNpc_Ls1_matAnm_c: vtable at +0x68,
 * to be confirmed by the constructor 0230242C) */
struct daNpc_Zl1_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ be<f32> mOffsetX; /* mOffset.x */
    /* 0x70 */ be<f32> mOffsetY; /* mOffset.y */
    /* 0x74 */ u8 field_0x74[8];
    /* 0x7C */ be<u8> mMoveFlag;  /* was field_0x7C: daNpc_Zl1_matAnm_c_ct 0230242C clears it; daNpc_Zl1_matAnm_c_calc 023086FC writes mOffset into the texture SRT only while it is set; demo 02305778 clears it (tww clrMoveFlag / setMoveFlag); as d_a_pz.h's matAnm mMoveFlag; move flag */
    /* 0x7D */ u8 _7D[3];
};
WWHD_SIZE(daNpc_Zl1_matAnm_c, 0x80);

/* LIGHT_INFLUENCE: the shared one (bindings.h, HD 0x24) */

struct daNpc_Zl1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;  /* was field_0x0: setAnm_anm 0230607C: the entry's bck animation number (bckResID), stored to mAnmNo (tww); after tww daNpc_Ls1_c::anm_prm_c bckNum and our daNpc_Hi1_c; bck number */
        /* 0x01 */ be<s8> btpNum;  /* was field_0x1: setAnm_NUM 023066D0 / setAnm / setAnm_ATR pass it to init_texPttrnAnm (tww); after tww daNpc_Ls1_c::anm_prm_c btpNum and our daNpc_Hi1_c; btp number */
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
    /* 0x982 */ csXyz mMtxAngle;  /* was field_0x73E: setMtx 02304C98 rotates the model matrix by it (mDoMtx_ZXYrotM) when she is not on the branch or ship; _execute 02307D18 copies current.angle into it each frame; createInit 0230503C sets it and shape_angle from current.angle (tww the same) */
    /* 0x988 */ csXyz mEffAngle;  /* was field_0x744: move_jmp 02306DB8 copies current.angle; setWaterSplash 023048F4 / set_simpleLand 0230498C pass it to createLandEmitter as the angle */
    /* 0x98E */ u8 field_0x74A[2];
    /* 0x990 */ cXyz mEyePos;  /* was field_0x74C: _nodeCB_Head 023020F8: the head matrix times a_eye_pos_off; setAttention 02304C44 copies it to eyePos */
    /* 0x99C */ cXyz mLookPos;  /* was field_0x758: lookBack 023075B0 / kyorokyoro 023074E4 look at it; cut_init_MAJYU_START 02306624 and chngAnmAtr 0230883C set it */
    /* 0x9A8 */ cXyz mDemoAreaPos;  /* was field_0x764: init_ZL1_3 02303BBC: a point in front of the start position (y 400), event_proc 023071C4 / demo_action2 02309EB8 set its y; demo_2 02309640 orders event 4 when the player is in the chk_areaIN area around it, demo_3 023096CC event 5 when the player is nearer than mPrmTbl.field_28 (tww the same) */
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
    /* 0x9F2 */ be<s16> mLookPlayerTimer;  /* was field_0x7AE: optn_1 02309794 / kyorokyoro 023074E4 load it from mPrmTbl.field_30 / field_32 with mKyoroTimer; optn_2 023099F4 keeps look mode 1 (the player) while it counts down, then look mode 5 (look around) (tww the same) */
    /* 0x9F4 */ be<s16> mKyoroTimer;  /* was field_0x7B0: optn_1 02309794 sets HIO prm 0x2C / 0x2E; kyorokyoro 023074E4 counts it down, then picks the next look position */
    /* 0x9F6 */ be<s16> mKyoroPosNo;  /* was field_0x7B2: kyorokyoro 023074E4 passes it to kyoroPos and sets cLib_getRndValue(1, 10); optn_1 02309794 clears it */
    /* 0x9F8 */ u8 field_0x7B4[2];
    /* 0x9FA */ be<s16> mWaitTimer;  /* was field_0x7B6: wait_1 02309394 / talk_1 02309478 set a random 15..30 and count it down */
    /* 0x9FC */ be<s16> mIdleAnmTimer;  /* was field_0x7B8: setStt 02306184 / optn_1 02309794: random 90..180; optn_1 counts it down and switches to the second idle animation (setAnm_NUM 0xC) at 0 (tww the same) */
    /* 0x9FE */ be<s16> mIdleAnmLoops;  /* was field_0x7BA: optn_1 02309794: 1 or 2 when animation 0xC starts, counted down at each animation end, back to animation 8 at 0; setStt 02306184 clears it (tww the same) */
    /* 0xA00 */ be<s16> mTurnStepMode;  /* was field_0x7BC: _execute 02307D18: the m_jnt turn step is mPrmTbl.mMaxTurnStep while it is < 0, else mPrmTbl.field_5C; createInit 0230503C -1, chngAnmAtr 0230883C 10 for message 0x15E3 and -1 for 0x15E4 (tww the same) */
    /* 0xA02 */ u8 field_0x7BE[2];
    /* 0xA04 */ be<s16> mLookAngleY;  /* was field_0x7C0: lookBack 023075B0: the target angle of look mode 3; never written in WWHD or tww */
    /* 0xA06 */ be<s8> mEnvRatioStep;  /* was field_0x7C2: cut_init_OMAMORI_ONOFF 02306BD8: -1 for OnOff 0 (darken, with the light at mLightPos) / 1 otherwise (brighten) / 0; darkProc 02307A58 runs decEnvironment / incEnvironment on mRatio for -1 / 1; incEnvironment 023079F8 clears it at ratio 1 (tww the same); GameCube 0x7C2 */
    /* 0xA07 */ be<s8> mbMorfAnimStopped;  /* was field_0x7C3: play_animation 023045E0: the morf's play() result, or 1 when the frame wrapped; setAnm_anm 0230607C / cut_init_OKIRU 023066B4 clear it; cut_move_OKIRU 02306D34 returns it, cut_move_OKIRU_2 02306D48 / optn_1 02309794 test it (tww the same); same code as daNpc_Hi1_c::mbMorfAnimStopped (tww daNpc_Ls1_c mbMorfAnimStopped) */
    /* 0xA08 */ be<s8> field_0x7C4;
    /* 0xA09 */ be<u8> mItemNo;
    /* 0xA0A */ be<u8> mJmpLandEff;  /* was field_0x7C6: move_jmp 02306DB8 sets it when the jump lands in water or on ground, after storing mEffPos / mEffAngle; setEff 02304A6C keeps those while set, plays set_simpleLand(it) and clears it (tww the same) */
    /* 0xA0B */ be<u8> mbGroundHit;  /* was field_0x7C7: _execute 02307D18 stores mObjAcch.ChkGroundHit() (the flag of the previous CrrPos) each frame; move_jmp 02306DB8 lands when it was 0 and the acch now hits the ground; cut_init_JMP_OFF 0230684C sets 1 (tww it = mObjAcch.ChkGroundHit()) */
    /* 0xA0C */ be<u8> mbWaterIn;  /* was field_0x7C8: _execute 02307D18 stores mObjAcch.ChkWaterIn() each frame; move_jmp 02306DB8, setWaterRipple 02304824 and setEff 02304A6C test it (HD in place of ChkWaterIn()); cut_init_JMP_OFF 0230684C clears it, setMtx 02304C98 sets it in Demo37 (tww it = mObjAcch.ChkWaterIn()) */
    /* 0xA0D */ be<u8> mForceEventProc;  /* was field_0x7C9: checkOrder 023055A4 sets it when event 2 starts; event_proc 023071C4 clears it at that event's end; _execute 02307D18 runs event_proc while set even without a staff index (tww the same) */
    /* 0xA0E */ be<u8> mSwoonOnShip;  /* was field_0x7CA: setStt 02306184 (state 5) sets it, cut_init_JMP_OFF 0230684C clears it; setMtx 02304C98 places her by swoon_OnShip while set; setWaterRipple 02304824 skips and _execute 02307D18 skips the collision while set (tww the same) */
    /* 0xA0F */ be<u8> mJumpingOff;  /* was field_0x7CB: cut_init_JMP_OFF 0230684C sets it, event_proc 023071C4 clears it when event 3 ends; _execute 02307D18 skips the collision while set (tww the same) */
    /* 0xA10 */ be<u8> mJumping;  /* was field_0x7CC: optn_2 023099F4 sets it when she jumps a wall or a drop and, while set, only runs move_jmp and keeps its result (tww the same; tww's comment there suggests mIsMoveJumped) */
    /* 0xA11 */ u8 field_0x7CD;
    /* 0xA12 */ be<u8> mOnTreeBranch;  /* was field_0x7CE: init_ZL1_1 02303A7C sets it; setMtx 02304C98 puts the model on the "wood2" branch of the mProcId1 actor while set; _draw 02308090 registers the snap figure at that matrix; demo 02305778 clears it once the demo moves her (tww the same) */
    /* 0xA13 */ be<u8> mStateIsComplaete;
    /* 0xA14 */ be<u8> mEyeFollow;  /* was field_0x7D0: lookBack 023075B0 sets it in look modes 1 / 4 / 5 (a target position), clears it in 3 and the rest; play_btk_anm 02304548 runs eye_ctrl instead of the btk while set, play_animation 023045E0 turns the eye control on with btk 1; chngAnmAtr 0230883C / cut_init_MAJYU_START 02306624 set it (tww the same) */
    /* 0xA15 */ be<u8> mEyeCtrl;  /* was field_0x7D1: setEyeCtrl 023041F8 sets 1, clrEyeCtrl 02304230 sets 0; eye_ctrl 02304310 moves the eyes only while set */
    /* 0xA16 */ be<u8> mHideUntilDemo;  /* was field_0x7D2: init_ZL1_0 023039E4 sets it; _execute 02307D18 returns early while it is set and no demo runs, then clears it in a demo; _draw 02308090 draws nothing while set (tww the same) */
    /* 0xA17 */ be<u8> mKeepShapeAngle;  /* was field_0x7D3: _execute 02307D18 copies current.angle into shape_angle only while it is 0; checkOrder 023055A4 (event 0) / setStt 02306184 (state 5) set it, cut_init_JMP_OFF 0230684C / event_proc 023071C4 / demo 02305778 clear it (tww the same) */
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
    /* 0xA7C */ cXyz mLightPos;  /* was field_0x830: cut_init_OMAMORI_ONOFF 02306BD8: set_LightPos, copied into mLightInfluence1 / 2 .mPos (tww the same); chngAnmAtr 0230883C: message 0x15E7 looks at a point beside it; GameCube 0x830 */
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
    /* 0xAAC */ be<u32> mpEyeMat[6];  /* was field_0x860: J3DMaterial* [6]: bodyCreateHeap 02302B08 stores the materials eyeLdamB, eyeLdamA, eyeL, eyeRdamB, eyeRdamA, eyeR (HD: found by name; tww walks the zl_eye joint's meshes); the GameCube material animation sharing in _draw is absent in HD */
    /* 0xAC4 */ be<u32> mpMayuMat[6];  /* was field_0x878: J3DMaterial* [6]: bodyCreateHeap 02302B08 stores the eyebrow materials mayuLdamB, mayuLdamA, mayuL, mayuRdamB, mayuRdamA, mayuR (HD: found by name; tww walks the zl_mayu joint's meshes) */
    /* 0xADC */ be<u32> mpEyeShape[6];  /* was field_0x890: J3DShape* [6]: bodyCreateHeap 02302B08 stores the shapes of mpEyeMat[i]; _draw 02308090 shows / hides them (shape_vis) around the zl_eye joint's two draw passes (tww the same) */
    /* 0xAF4 */ be<u32> mpMayuShape[6];  /* was field_0x8A8: J3DShape* [6]: bodyCreateHeap 02302B08 stores the shapes of mpMayuMat[i]; _draw 02308090 shows / hides them (shape_vis) around the zl_mayu joint's two draw passes (tww the same) */
    /* 0xB0C */ be<s32> mHeadMaterialIndex;  /* was mB0C: HD only: bodyCreateHeap 02302B08 stores the index of the material "zelda_zhead"; mesh_vis (_draw 02308090) sets the visibility of its shape against the other meshes of the world_root joint; the name d_a_npc_zl1_heap.h already gives it (one name per field) */
    /* 0xB10 */ be<s32> mHeadOverlayMaterialIndex;  /* was mB10: HD only: bodyCreateHeap 02302B08 stores the index of the material "zelda_zhead_2_"; mesh_vis (_draw 02308090) treats it like mHeadMaterialIndex; the name d_a_npc_zl1_heap.h already gives it (one name per field) */
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
WWHD_OFFSET(daNpc_Zl1_c, mIdleAnmTimer, 0x9FC);
WWHD_OFFSET(daNpc_Zl1_c, mEnvRatioStep, 0xA06);
WWHD_OFFSET(daNpc_Zl1_c, mStateIsComplaete, 0xA13);
WWHD_OFFSET(daNpc_Zl1_c, mInDemo, 0xA1D);
WWHD_OFFSET(daNpc_Zl1_c, mRippleCallBack, 0xA20);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence1, 0xA34);
WWHD_OFFSET(daNpc_Zl1_c, mLightInfluence2, 0xA58);
WWHD_OFFSET(daNpc_Zl1_c, mLightPos, 0xA7C);
WWHD_OFFSET(daNpc_Zl1_c, mOrderEvent, 0xA96);
WWHD_OFFSET(daNpc_Zl1_c, mType, 0xA9B);
WWHD_OFFSET(daNpc_Zl1_c, mJoint1, 0xAA0);
WWHD_OFFSET(daNpc_Zl1_c, mHeadMaterialIndex, 0xB0C);
WWHD_OFFSET(daNpc_Zl1_c, mOffCupOnAupPacket1, 0xB14);
WWHD_SIZE(daNpc_Zl1_c, 0xD74);

/* daNpc_Zl1_HIO_c (HD: vtable first, size 0x64; l_HIO at 0x10468C20, a_prm_tbl at 0x101C7574) */
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
    /* 0x08 */ be<s32> mRefCount;  /* was field_0x8: daNpc_Zl1_HIO_c_ct 0230A0EC -1; never read in WWHD or tww; after our decomp's daNpc_Hi1_HIO_c / daNpc_Ba1_HIO_c mRefCount: the same HIO layout (mNo, then the s32 instance counter) */
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Zl1_HIO_c, 0x64);

/* ---- file statics ---- */
static inline daNpc_Zl1_HIO_c& l_HIO() { return *gabi::at<daNpc_Zl1_HIO_c>(0x10468C20); }
static inline be<s32>& l_check_wrk() { return *gabi::at<be<s32>>(0x10468B14); }
static inline gptr<fopAc_ac_c>* l_check_inf() { return gabi::at<gptr<fopAc_ac_c>>(0x10468C84); } /* [20] */
/* l_BCKName 0x10468BC0, l_BTPName 0x10468BE0, l_BTKName 0x10468C00 (char[0x20]) */

/* ---- local bindings (SHARED-CANDIDATE; most as in d_a_npc_ba1.cpp / d_a_npc_ls1.cpp) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xD4 + i); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mTextPtr = gabi::ea(arc);
    key->__vtbl = ZL1_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (out-of-line copy in another TU) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 025E789C mDoExt_btpAnm::init / 025E7CE0 mDoExt_btkAnm::init */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_c* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16, s16, bool) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* HD message manager (*(0x101F4B5C)) */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, msgMgr()); }
/* cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
static inline u32 g_Counter_mCounter0() { return gabi::load<u32>(0x101FF558); }
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* 02606900 HD: dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* dComIfG_getObjectRes_name(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->mTextPtr = gabi::ea(arc);
    a->__vtbl = ZL1_SAFESTRING_VTBL;
    n->mTextPtr = gabi::ea(name);
    n->__vtbl = ZL1_SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}

/* GHS pointer to member function call */
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
