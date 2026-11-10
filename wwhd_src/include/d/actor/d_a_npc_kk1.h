/* daNpc_Kk1_c (Mila, poor, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" and its header has almost no members: the layout is measured
 * from the WWHD code. Size 0xAD0 (profile at 0x101BF654, process name 0x161; GameCube 0x824).
 * Base fopNpc_npc_c (0x7DC, d/d_npc.h). Fields whose role is not known are named by their HD
 * offset (mXXX); signedness: cast at use ((s8)mOnPath) rather than retyping.
 *
 * Includes d_a_npc_ko1.h for its local bindings (SAFESTRING_VTBL must be defined first). */
#pragma once
#include "d/actor/d_a_npc_ko1.h"

/* mDoExt_bckAnm (HD 0x8C, inline constructor: J3DFrameCtrl, vtables 1016E54C/1016D820) */
struct mDoExt_bckAnm_kk1 {
    u8 _[0x8C];
};

/* daNpc_Kk1_HIO_c (HD 0x60: vtable +0, mNo +4, field_0x8 +8, parameter table +0xC (0x54 bytes,
 * from .data 0x101BF5EC); the static l_HIO is at 0x10467698) */
struct daNpc_Kk1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ u8 mPrm[0x14];
    /* 0x20 */ be<f32> mAttentionYOffset;  /* setAttention */
    /* 0x24 */ u8 mPrm24[0x60 - 0x24];
};
WWHD_SIZE(daNpc_Kk1_HIO_c, 0x60);
static inline daNpc_Kk1_HIO_c& l_HIO_kk1() { return *gabi::at<daNpc_Kk1_HIO_c>(0x10467698); }

struct daNpc_Kk1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* methods (drafts from mkbind: each part's owner fixes the signatures of its functions) */
    void _nodeCB_Head(J3DNode* a0, J3DModel* a1); /* 0226AAF8 */
    void _nodeCB_BackBone(J3DNode* a0, J3DModel* a1); /* 0226ACB8 */
    BOOL setBtp(s8 i_num, u32 i_modify); /* 0226AE30 */
    s32 btpResID(int i_num); /* 0226AE24 (unnamed by the matcher) */
    BOOL init_texPttrnAnm(s8 i_num, u32 i_modify); /* 0226AF1C */
    bool bodyCreateHeap(); /* 0226AF20 */
    bool effcCreateHeap(); /* 0226B1E0 */
    bool CreateHeap(); /* 0226B3E8 */
    bool decideType(int i_type); /* 0226B550 */
    void set_pthPoint(u8 i_idx); /* 0226B690 (unnamed by the matcher) */
    BOOL set_action(ProcFunc_l* a0, void* a1); /* 0226B704 */
    bool init_KK1_0(); /* 0226B830 */
    void play_btp_anm(); /* 0226B8E4 */
    void setBikon(cXyz* a0 /* by value: a copy */); /* 0226B984 */
    void play_eff_anm(); /* 0226BA48 */
    void play_animation(); /* 0226BB5C */
    void flwAse(); /* 0226BCD4 */
    void setAttention(u32 i_force); /* 0226BD78 */
    void setMtx(u32 i_force); /* 0226BDCC */
    bool createInit(); /* 0226BFD0 */
    cPhs_State _create(); /* 0226C28C */
    void delAse(); /* 0226C438 */
    BOOL _delete(); /* 0226C464 */
    bool partner_search_sub(u32 /* fn */ a0); /* 0226C4C4 */
    void partner_search(); /* 0226C570 */
    void setAse(); /* 0226C5F0 */
    void setAnm_anm(anm_prm_c* a0); /* 0226C660 */
    void setAnm_NUM(s32 a0, s32 a1); /* 0226C730 */
    bool checkCommandTalk(); /* 0226C7A0 */
    void checkOrder(); /* 0226C7DC */
    u8 demo(); /* 0226C8D8 */
    s32 isEventEntry(); /* 0226CA94 */
    s32 bckResID(int i_num); /* 0226C5DC (unnamed by the matcher) */
    fopAc_ac_c* searchByID(fpc_ProcID i_id, be<s32>* o_res); /* 0226CC2C (matcher: cLib_getRndValue<i>) */
    void setAnm(); /* 0226CAD4 */
    void setStt(s8 a0); /* 0226CB44 */
    void endEvent(); /* 0226CBE8 */
    bool cut_init_RUN_START(s32 a0); /* 0226CC80 */
    void cut_init_RUN(s32 a0); /* 0226CD04 */
    void cut_init_CATCH_START(s32 a0); /* 0226CD94 */
    void cut_init_CATCH_END(s32 a0); /* 0226CDF0 */
    void cut_init_TRN(s32 a0); /* 0226CE6C */
    void cut_init_BYE_START(s32 a0); /* 0226D0A4 */
    void cut_init_BYE(s32 a0); /* 0226D0E8 */
    void cut_init_BYE_END(s32 a0); /* 0226D1F0 */
    void cut_init_OTOBOKE(s32 a0); /* 0226D2B8 */
    void cut_init_PLYER_MOV(s32 a0); /* 0226D34C */
    void cut_init_RUNAWAY_START(s32 a0); /* 0226D454 */
    void cut_init_RUNAWAY_END(s32 a0); /* 0226D60C */
    void cut_init_BYE_CONTINUE(s32 a0); /* 0226D664 */
    bool cut_move_RUN_START(); /* 0226D6CC */
    bool event_move(bool a0); /* 0226D83C */
    s32 cut_move_RUN(); /* 0226DB64 */
    bool cut_move_CATCH_START(); /* 0226DBB0 */
    bool cut_move_TRN(); /* 0226DBD4 */
    bool cut_move_BYE(); /* 0226DC70 */
    bool cut_move_OTOBOKE(); /* 0226DD7C */
    bool cut_move_RUNAWAY_START(); /* 0226DDD8 */
    bool cut_move_BYE_CONTINUE(); /* 0226DE88 */
    void privateCut(s32 a0); /* 0226DEF0 */
    void event_proc(s32 a0); /* 0226E21C */
    void kyoroPos(cXyz* o_pos, s32 i_idx); /* 0226E498: returns a cXyz (hidden result pointer) */
    bool kyorokyoro(); /* 0226E560 */
    void lookBack(); /* 0226E604 */
    void eventOrder(); /* 0226E92C */
    BOOL _execute(); /* 0226E99C */
    BOOL _draw(); /* 0226ECA8 */
    void setAnm_ATR(); /* 0226EF4C */
    void chngAnmAtr(u8 a0); /* 0226EFB4 */
    void ctrlAnmAtr(); /* 0226F088 */
    void anmAtr(u16 a0); /* 0226F170 */
    u16 next_msgStatus(be<u32>* a0); /* 0226F230 */
    u32 getMsg_KK1_0(); /* 0226F538 */
    u32 getMsg(); /* 0226F61C */
    bool chk_talk(); /* 0226F654 */
    u8 chk_parts_notMov(); /* 0226F6EC */
    BOOL chkAttention(); /* 0226F72C */
    void createTama(f32 a0); /* 0226F7B4 */
    bool chk_areaIN(f32 a0, cXyz* a1 /* by value: a copy */); /* 0226F908 */
    bool startEvent_check(); /* 0226FABC */
    BOOL chkHitPlayer(); /* 0226FB70 */
    u8 chk_attn(); /* 0226FC1C */
    s32 wait_1(); /* 0226FE78 */
    s32 walk_1(); /* 022700E0 */
    s32 wait_2(); /* 022703B4 */
    void init_CMT_WAI(); /* 0227051C */
    void move_CMT_WAI(); /* 02270570 */
    void init_CMT_TRN(); /* 0227066C */
    void move_CMT_TRN(); /* 022706E4 */
    void init_CMT_PCK(); /* 022708B8 */
    void move_CMT_PCK(); /* 02270918 */
    s32 cmmt_1(); /* 02270AE0 */
    s32 wait_3(); /* 02270C7C */
    s32 wait_4(); /* 02270DA8 */
    BOOL talk_1(); /* 02270F28 */
    BOOL wait_action1(void* a0); /* 02271084 */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_backbone_jnt_num;
    /* 0x7E6 */ char mArcName[6];                /* "Kk" (decideType) */
    /* 0x7EC */ u8 mBtpAnm[0x74];                /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> m861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ ProcFunc_l mAction;
    /* 0x86C */ be<u32> mPartnerId;  /* was m86C: partner_search_sub 0226C4C4 stores the partner's process id (-1 when none); chngAnmAtr 0226EFB4 / wait_2 022703B4 / cut_init_RUN_START 0226CC80 search it; same name as the views Kk1CreationFields / Kk1EventState / Kk1ActionFields */
    /* 0x870 */ be<u32> mLookActorId;  /* was m870: lookBack 0226E604 looks at the actor with this id; same name as the view Kk1EventState */
    /* 0x874 */ dNpc_PathRun_l mPathRun;
    /* 0x87C */ cXyz mHomePos;  /* was m87C: _execute 0226E99C copies current.pos on the first run (mHomeInitialized); same name as the view Kk1EventState */
    /* 0x888 */ csXyz mHomeAngle;  /* was m888: _execute 0226E99C copies current.angle on the first run; wait_1 0226FE78 / wait_2 022703B4 turn back to its y; same name as the views Kk1EventState / Kk1ActionFields */
    /* 0x88E */ csXyz mModelAngle;  /* was m88E: createInit 0226BFD0 sets it, setMtx 0226BDCC rotates the model by it; _execute 0226E99C copies current.angle into it while mPreserveShapeAngle is clear; same name as the view Kk1CreationFields; same code as daNpc_Ko1_c::mModelAngle */
    /* 0x894 */ u8 _894[0x89C - 0x894];
    /* 0x89C */ cXyz mAttPos;                    /* attention offset position (nodeCB_Head) */
    /* 0x8A8 */ cXyz mLookPos;  /* was m8A8: lookBack 0226E604 sets it to the player's eye or the look actor's position and looks at it; chngAnmAtr 0226EFB4 sets the partner's position; same code as daNpc_Bm1_c / daNpc_Ko1_c::mLookPos; same name as the view Kk1EventState */
    /* 0x8B4 */ u8 _8B4[0x8C0 - 0x8B4];
    /* 0x8C0 */ cXyz mEyePos;                    /* (nodeCB_Head) */
    /* 0x8CC */ u8 _8CC[0x8D8 - 0x8CC];
    /* 0x8D8 */ be<f32> mPreviousFrame;  /* was m8D8: play_animation 0226BB5C: a morf frame below it means the animation wrapped (mAnimationEnded = 1), then it takes the frame; same name as the view Kk1CreationFields */
    /* 0x8DC */ u8 _8DC[4];
    /* 0x8E0 */ csXyz mPreviousLookAngles;  /* was m8E0: lookBack 0226E604 stores the look yaw (x), the joint control's head Y (y) and backbone Y (z); chk_parts_notMov 0226F6EC compares them; same name as the view Kk1ActionFields */
    /* 0x8E6 */ u8 _8E6[2];
    /* 0x8E8 */ be<u32> mbSetEyePos;  /* was m8E8: wait_action1 02271084 stores the state function's result; setAttention 0226BD78 copies the eye position to eyePos only while it or the argument is set; cut_move_CATCH_START 0226DBB0 0; same code as daNpc_Bm1_c / daNpc_Ko1_c::mbSetEyePos (tww Bm1 mbSetEyePos = wait_1()) */
    /* 0x8EC */ be<s16> mEventIds;  /* was m8EC: eventOrder 0226E92C orders the event mEventIds[idx]; event_proc 0226E21C reads the current one; same name as the view Kk1EventState */
    /* 0x8EE */ u8 _8EE[2];
    /* 0x8F0 */ be<s16> mPickEventId;  /* was m8F0: init_CMT_PCK 022708B8 reads the event id it orders; same name as the view Kk1ActionFields */
    /* 0x8F2 */ u8 _8F2[0x8FC - 0x8F2];
    /* 0x8FC */ be<s16> mEventIdx;  /* was m8FC: eventOrder 0226E92C stores the index of the ordered event; checkOrder 0226C7DC / event_proc 0226E21C pick mEventIds by it; init_CMT_PCK 022708B8 sets 2; same code as daNpc_Bm1_c::mEventIdx (tww Bm1 mEventIdx = condition - 3; fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx])) */
    /* 0x8FE */ u8 _8FE[4];
    /* 0x902 */ be<s16> m902;
    /* 0x904 */ be<s16> m904;
    /* 0x906 */ be<s16> m906;
    /* 0x908 */ be<s16> mCutTimer;  /* was m908: cut_init_RUN 0226CD04 .. set the cut's timer (-1 without one) and cut_move_RUN 0226DB64 .. end the cut when it runs out; init_CMT_TRN 0227066C sets the HIO turn time and move_CMT_TRN 022706E4 ends that comment action when it runs out: the countdown of the current cut or comment action; same name as the view Kk1EventState */
    /* 0x90A */ be<s16> mTurnTimer;  /* was m90A: cut_init_BYE 0226D0E8 sets the cut's delay (-1 without one); same name as the view Kk1EventState */
    /* 0x90C */ u8 _90C[2];
    /* 0x90E */ be<s16> mAfterTalkTimer;  /* was m90E: talk_1 02270F28 sets 15 + rnd(30) when the talk ends; same name as the view Kk1ActionFields */
    /* 0x910 */ u8 _910[2];
    /* 0x912 */ be<s16> mTalkLookTimer;  /* was m912: chngAnmAtr 0226EFB4 sets the HIO time, talk_1 02270F28 / setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x914 */ be<s16> mActionTimer;  /* was m914: wait_1 0226FE78 .. set 60 + rnd(30) and count it down; checkCommandTalk 0226C7A0 tests it; setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x916 */ u8 _916[4];
    /* 0x91A */ be<s16> mTurnStartAngle;  /* was m91A: init_CMT_TRN 0227066C keeps current.angle.y; move_CMT_TRN 022706E4 turns to its back; same name as the view Kk1ActionFields */
    /* 0x91C */ be<s16> mLookYaw;  /* was m91C: lookBack 0226E604 uses it as the look-at's Y angle; same name as the view Kk1EventState */
    /* 0x91E */ be<s16> mRefusedAnswerCount;  /* was m91E: next_msgStatus 0226F230 adds 1 per refused answer and picks the message by it; same name as the view Kk1ActionFields */
    /* 0x920 */ be<s16> mKeepMoving;  /* was m920: cut_init_BYE 0226D0E8 sets the cut's integer (0 without one); same name as the view Kk1EventState */
    /* 0x922 */ be<u8> mAnimationEnded;  /* was m922: play_animation 0226BB5C: the morf's play() result, also set when the frame wraps; setAnm_anm 0226C660 0; ctrlAnmAtr 0226F088 / wait_1 / the cuts test it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x923 */ be<u8> mAnimationStarted;  /* was m923: setAnm_anm 0226C660 0 when the animation changes; written only; same name as the view Kk1CreationFields */
    /* 0x924 */ be<u8> mPresentedItem;  /* was m924: chk_talk 0226F654 stores the item shown (dComIfGp_event_getPreItemNo) or 0xFF; same name as the view Kk1ActionFields */
    /* 0x925 */ be<u8> mSwitchNo;  /* was m925: createInit 0226BFD0 takes it from the parameters ((prm >> 8) & 0xFF); searchActor_SWC00 0226B4A8 / wait_2 022703B4 test the switch; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x926 */ be<u8> mOnPath;  /* was m926: setStt 0226CB44 1 for the comment state; walk_1 022700E0 / wait_1 0226FE78 test and set it; cut_init_RUN 0226CD04 / event_move 0226D83C follow the path while it is set; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x927 */ be<u8> mWaitingAtPathEnd;  /* was m927: wait_1 0226FE78 .. set 1 at the path end and 0 when moving on; same name as the view Kk1ActionFields */
    /* 0x928 */ be<u8> mPartnerEventEnabled;  /* was m928: next_msgStatus 0226F230 sets 1; wait_2 022703B4 starts the partner event while it is set; same name as the view Kk1ActionFields */
    /* 0x929 */ u8 _929;
    /* 0x92A */ be<u8> m92A;
    /* 0x92B */ be<u8> mHideBody;  /* was m92B: cut_init_BYE_START 0226D0A4 sets 1; _execute 0226E99C skips the collision while it is set; same name as the view Kk1EventState */
    /* 0x92C */ be<u8> mResourceLoaded;  /* was m92C: _create 0226C28C: the phase result == cPhs_COMPLEATE_e; same name as the view Kk1CreationFields */
    /* 0x92D */ be<u8> mDemoSuspend;  /* was m92D: _draw 0226ECA8 draws nothing while it is set; _execute 0226E99C clears it outside a demo; same name as the view Kk1EventState */
    /* 0x92E */ be<u8> mEffectHidden;  /* was m92E: setAnm_anm 0226C660 sets 1 / 0 by the animation; _draw 0226ECA8 draws the effect model only while it is clear; same name as the view Kk1CreationFields */
    /* 0x92F */ be<u8> mPreserveShapeAngle;  /* was m92F: cut_init_RUNAWAY_START 0226D454 sets 1; _execute 0226E99C copies current.angle to the shape and mModelAngle only while it is clear; demo 0226C8D8 0; same name as the view Kk1EventState */
    /* 0x930 */ be<u8> mHide;  /* was m930: cut_init_BYE_END 0226D1F0 / cut_init_RUNAWAY_END 0226D60C set 1; _draw 0226ECA8 draws nothing while it is set; same name as the view Kk1EventState */
    /* 0x931 */ be<u8> mHomeInitialized;  /* was m931: _execute 0226E99C stores mHomePos / mHomeAngle once and sets it; same name as the view Kk1EventState */
    /* 0x932 */ be<u8> mAttentionSelected;  /* was m932: wait_action1 02271084 stores chkAttention(); same name as the view Kk1ActionFields */
    /* 0x933 */ be<u8> mTalkRequested;  /* was m933: checkOrder 0226C7DC sets 1 on the talk command; wait_1 0226FE78 / walk_1 022700E0 start the talk on it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0x934 */ be<u8> mHeadOnlyFollow;  /* was m934: lookBack 0226E604 passes it as the head-only flag of the joint look-at; the cuts (cut_init_TRN 0226CE6C ..) and the waits (wait_1 0226FE78 ..) set 1 / 0; same code as daNpc_Bm1_c / daNpc_Ko1_c::mHeadOnlyFollow (tww Bm1 bool headOnlyFollow = m896) */
    /* 0x935 */ be<u8> mPlayerFar;  /* was m935: wait_3 02270C7C stores whether the player is far; wait_2 022703B4 0; same name as the view Kk1ActionFields */
    /* 0x936 */ be<u8> mInDemo;  /* was m936: demo 0226C8D8 sets and clears it and returns by it; _execute 0226E99C skips the collision while it is set; same name as the view Kk1CreationFields; same name as daNpc_Ko1_c::mInDemo (demo sets and returns it) */
    /* 0x937 */ u8 _937;
    /* 0x938 */ u8 mBrkAnm[0x74];                /* mDoExt_brkAnm (HD 0x74, constructor 025E7480) */
    /* 0x9AC */ mDoExt_btkAnm mBtkAnm;           /* (HD 0x74, constructor 025E7C6C) */
    /* 0xA20 */ mDoExt_bckAnm_kk1 mBckAnm;       /* (HD 0x8C, inline constructor) */
    /* 0xAAC */ csXyz mEffectFrame;  /* was mAAC: play_eff_anm 0226BA48 steps the effect brk (x) / btk (y) / bck (z) frames; setMtx 0226BDCC / _draw 0226ECA8 set the effect animations to them; same name as the view Kk1CreationFields */
    /* 0xAB2 */ u8 _AB2[2];
    /* 0xAB4 */ be<u32> mEffectModel;  /* was mAB4: effcCreateHeap 0226B1E0 creates the effect model; _draw 0226ECA8 draws it with the brk / btk animations; same name as the views Kk1CreationFields / Kk1EventState */
    /* 0xAB8 */ u8 _AB8[4];
    /* 0xABC */ be<u32> mSweatEmitter;  /* was mABC: setAse 0226C5F0 keeps the sweat (0x819E) emitter; flwAse 0226BCD4 moves it; same name as the view Kk1CreationFields; sweat (Ase) emitter */
    /* 0xAC0 */ be<u8> mCutIndex;  /* was mAC0: privateCut 0226DEF0: the cut index from dComIfGp_evmng_getMyActIdx, switched on; same name as the view Kk1EventState */
    /* 0xAC1 */ be<u8> mMoveMode;  /* was mAC1: event_move 0226D83C: 2 walks (walk rate / step / speed), else runs; takes the path point's argument 2 or 3; cut_init_RUN 0226CD04 / cut_init_BYE 0226D0E8 / setStt 0226CB44 (comment state) set 2 */
    /* 0xAC2 */ be<u8> mCommentAction;  /* was mAC2: cmmt_1 02270AE0 switches on it; move_CMT_WAI 02270570 / move_CMT_TRN 022706E4 / move_CMT_PCK 02270918 / setStt 0226CB44 set 0; checkCommandTalk 0226C7A0 tests 5; event_move 0226D83C clears it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xAC3 */ be<u8> mTalkAnimation;  /* was mAC3: setAnm_ATR 0226EF4C picks the message animation by it; checkOrder 0226C7DC / endEvent 0226CBE8 0xFF; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xAC4 */ be<u8> mMessageTag;  /* was mAC4: anmAtr 0226F170: the message's animation tag when it changes; checkOrder 0226C7DC / endEvent 0226CBE8 0xFF; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xAC5 */ be<u8> mTextureAnimation;  /* was mAC5: setBtp 0226AE30 stores the texture animation number; play_btp_anm 0226B8E4 blinks only in number 0; same name as the view Kk1CreationFields */
    /* 0xAC6 */ be<u8> mAnmNum;  /* was mAC6: setAnm_anm 0226C660: the animation number (returns at once when it is unchanged); play_animation 0226BB5C switches on it; createInit 0226BFD0 0xC; same code as daNpc_Bm1_c / daNpc_Ko1_c::mAnmNum (tww Bm1 setAnm_anm mAnmNum) */
    /* 0xAC7 */ be<u8> mOrderEvent;  /* was mAC7: eventOrder 0226E92C orders by it (talk at 1 / 2, other events from 3); checkOrder 0226C7DC clears it once the talk / event starts; the waits set 2; same code as daNpc_Bm1_c / daNpc_Ko1_c::mOrderEvent (tww Bm1 m8FD) */
    /* 0xAC8 */ be<u8> mActionState;  /* was mAC8: setStt 0226CB44 stores the state; setAnm 0226CAD4 picks the state animation by it; wait_action1 02271084 switches on it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xAC9 */ be<u8> mPreviousState;  /* was mAC9: setStt 0226CB44 keeps the previous state; talk_1 02270F28 returns to it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xACA */ be<u8> mLookMode;  /* was mACA: lookBack 0226E604 switches on it; chngAnmAtr 0226EFB4 / the cuts / the waits set it; chk_attn 0226FC1C widens the angle in mode 1; same name as the views Kk1EventState / Kk1ActionFields */
    /* 0xACB */ be<s8> mType;                    /* decideType */
    /* 0xACC */ be<s8> mMessageType;  /* was mACC: createInit 0226BFD0 / partner_search 0226C570 / getMsg 0226F61C branch on it; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xACD */ be<u8> mActionStep;  /* was mACD: partner_search 0226C570 runs at 1 and steps it; wait_action1 02271084 steps it from 0; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xACE */ be<u8> mMessageAnimationStarted;  /* was mACE: anmAtr 0226F170: applies the message's animation attribute only while it is 0, then adds 1; setStt 0226CB44 0; same name as the views Kk1CreationFields / Kk1ActionFields */
    /* 0xACF */ u8 _ACF;
};
WWHD_OFFSET(daNpc_Kk1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Kk1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Kk1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Kk1_c, mPathRun, 0x874);
WWHD_OFFSET(daNpc_Kk1_c, mAttPos, 0x89C);
WWHD_OFFSET(daNpc_Kk1_c, mEyePos, 0x8C0);
WWHD_OFFSET(daNpc_Kk1_c, mCutTimer, 0x908);
WWHD_OFFSET(daNpc_Kk1_c, mAnimationEnded, 0x922);
WWHD_OFFSET(daNpc_Kk1_c, mBrkAnm, 0x938);
WWHD_OFFSET(daNpc_Kk1_c, mBtkAnm, 0x9AC);
WWHD_OFFSET(daNpc_Kk1_c, mBckAnm, 0xA20);
WWHD_OFFSET(daNpc_Kk1_c, mEffectFrame, 0xAAC);
WWHD_OFFSET(daNpc_Kk1_c, mSweatEmitter, 0xABC);
WWHD_OFFSET(daNpc_Kk1_c, mType, 0xACB);
WWHD_SIZE(daNpc_Kk1_c, 0xAD0);
