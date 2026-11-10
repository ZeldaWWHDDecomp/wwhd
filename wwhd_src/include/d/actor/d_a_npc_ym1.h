/* daNpc_Ym1_c (Mesa & Abe, Outset), WWHD layout. 
 *
 * GameCube -> WWHD: members +0x118 up to mArcName (fopNpc_npc_c is 0x7DC: GameCube 0x6C4 +
 * 0x11C - 4); mDoExt_btpAnm grew from 0x14 to 0x74 and the pointer to member mCurrProcFunc is 8
 * bytes, so from mCyl (GameCube 0x704) on everything is +0x170. Size 0xA24 (profiles NPC_YM1/
 * NPC_YM2 at 0x101C6D38/0x101C6D68; GameCube 0x8B4). Fields whose role is not known are named
 * by WWHD offset. The local bindings are shared with d_a_npc_ko1 (d_a_npc_ko1.h). */
#pragma once
#include "d/actor/d_a_npc_ko1.h" /* local bindings, ProcFunc_l */

struct daNpc_Ym1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
    };

    /* Methods, one section per part file. Each part owns its section: it may fix the
     * declarations there (return types, parameter types) and add new ones (unnamed functions).
     * Return types marked unchecked come from mkbind.py hints. */
    /* ---- part A (d_a_npc_ym1.cpp) ---- */
    void setKariFlg(); /* 022F68AC */
    void _nodeCB_Head(J3DNode*, J3DModel*); /* 022F68C4 */
    void _nodeCB_BackBone(J3DNode*, J3DModel*); /* 022F6A7C */
    bool bodyCreateHeap(); /* 022F6BE0 */
    s32 btpResID(s32); /* 022F6EF4 */
    bool init_texPttrnAnm(s8, u32); /* 022F6F38 */
    bool headCreateHeap(); /* 022F7024 */
    bool itemCreateHeap(); /* 022F7114 */
    bool CreateHeap(); /* 022F71E0 */
    bool decideType(int); /* 022F72B0 */
    BOOL set_action(ProcFunc_l*, void*); /* 022F7388 */
    bool init_YM1_0(); /* 022F74B4 */
    bool init_YM1_1(); /* 022F75E4 */
    bool init_YM2_0(); /* 022F7668 */
    bool init_YM2_1(); /* 022F7710 */
    bool init_YM2_2(); /* 022F77B8 */
    bool init_YM2_3(); /* 022F7848 */
    bool init_YMx_error(); /* 022F78D8 */
    void play_texPttrnAnm(); /* 022F7918 */
    void play_animation(); /* 022F79D0 (the r3 left is McaMorf::play's result: not a return value) */
    u8 chk_nbt_attn(); /* 022F7A3C */
    void setAttention(u32); /* 022F7A60 */
    void setMtx(u32); /* 022F7B28 */
    /* ---- end of part A ---- */
    /* ---- part A2 (d_a_npc_ym1_a2.cpp) ---- */
    bool createInit(); /* 022F7D68 */
    cPhs_State _create(); /* 022F8108 */
    BOOL _delete(); /* 022F82AC */
    void checkOrder(); /* 022F8304 */
    u8 demo(); /* 022F8344 */
    s32 isEventEntry(); /* 022F8504 */
    void endEvent(); /* 022F8544 */
    void privateCut(int); /* 022F8588 */
    void event_proc(int); /* 022F8620 */
    void lookBack(); /* 022F8670 */
    void eventOrder(); /* 022F8890 */
    void set_cutGrass(); /* 022F88C8 */
    void set_collision_sp(); /* 022F89CC */
    BOOL _execute(); /* 022F8C24 */
    /* ---- end of part A2 ---- */
    /* ---- part B (d_a_npc_ym1_b.cpp) ---- */
    BOOL _draw(); /* 022F8F30 */
    s32 bckResID(int); /* 022F915C */
    void setAnm_anm(anm_prm_c*); /* 022F9170 */
    void setAnm_NUM(int, int); /* 022F9230 */
    void setAnm(); /* 022F92A0 */
    void setAnm_ATR(); /* 022F9310 */
    void chngAnmAtr(u8); /* 022F9378 */
    void anmAtr(u16); /* 022F9394 */
    u16 next_msgStatus(be<u32>*); /* 022F9464 */
    u32 getMsg_YM1_0(); /* 022F9758 */
    u32 getMsg_YM1_1(); /* 022F980C */
    bool chk_BlackPig(); /* 022F9858 */
    u32 getMsg_YM2_0(); /* 022F9890 */
    u32 getMsg_YM2_1(); /* 022F9914 */
    u32 getMsg_YM2_2(); /* 022F9960 */
    u32 getMsg_YM2_3(); /* 022F9A44 */
    u32 getMsg(); /* 022F9A48 */
    bool chk_talk(); /* 022F9ACC */
    u8 chk_parts_notMov(); /* 022F9B64 */
    u8 chkAttention(); /* 022F9BA4 */
    void setStt(s8); /* 022F9C2C */
    bool chk_areaIN(f32, cXyz*); /* 022F9D04 (cXyz by value: pointer to a copy) */
    BOOL kari_1(); /* 022F9E00 */
    BOOL wait_1(); /* 022F9EF4 */
    BOOL wait_2(); /* 022FA0A0 */
    u8 talk_1(); /* 022FA288 */
    BOOL turn_1(); /* 022FA4E4 */
    BOOL NBTwai(); /* 022FA5B8 */
    BOOL SITwai(); /* 022FA70C */
    BOOL wait_action1(void*); /* 022FA828 */
    BOOL wait_action2(void*); /* 022FA940 */
    BOOL wait_action3(void*); /* 022FAA28 */
    BOOL wait_action4(void*); /* 022FAB88 */
    BOOL demo_action1(void*); /* 022FAC34 */
    /* ---- end of part B ---- */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> mHeadJointIdx;
    /* 0x7E5 */ be<s8> mBboneJointIdx;
    /* 0x7E6 */ be<s8> mHandLJointIndex;
    /* 0x7E7 */ be<s8> mHandRJointIndex;
    /* 0x7E8 */ gptr<J3DModel> mItemModel;  /* was m7E8: itemCreateHeap 022F7114: mDoExt_J3DModel__create(a_mdl_dat, ..) (nullptr without one); setMtx 022F7B28 places it on the hand; _draw 022F8F30 draws it while set; view Ym1PartAState mItemModel (d_a_npc_ym1_readable.h); GameCube m6D0; GameCube m6D0 */
    /* 0x7EC */ char mArcName[4];
    /* 0x7F0 */ gptr<J3DModel> mpHeadModel;
    /* 0x7F4 */ u8 mBtpAnm[0x74];                 /* mDoExt_btpAnm (HD 0x74) */
    /* 0x868 */ be<u8> mBlinkFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ ProcFunc_l mCurrProcFunc;
    /* 0x874 */ dCcD_Cyl mCyl;                    /* hides fopNpc_npc_c::mCyl (GameCube 0x704) */
    /* 0x9A4 */ be<u32> m9A4;
    /* 0x9A8 */ be<u32> m9A8;
    /* 0x9AC */ be<u32> m9AC;
    /* 0x9B0 */ be<s16> m9B0;
    /* 0x9B2 */ be<s16> mRotYTarget;              /* GameCube 0x842 */
    /* 0x9B4 */ be<s16> m9B4;
    /* 0x9B6 */ csXyz mModelAngle;  /* was m9B6: _execute 022F8C24 / createInit 022F7D68: current.angle every frame; setMtx 022F7B28: mDoMtx_ZXYrotM by it; view Ym1PartAState mModelAngle; GameCube m846; GameCube m846 */
    /* 0x9BC */ cXyz mEyePosition;  /* was m9BC: _nodeCB_Head 022F68C4: the head matrix times a_eye_pos_off; setAttention 022F7A60 copies it to eyePos; view Ym1PartAState mEyePosition */
    /* 0x9C8 */ cXyz mLookPos;  /* was m9C8: lookBack 022F8670: the player's eye position in look mode 1, the look-at target in mode 2 (after our decomp's daNpc_Pf1_c mLookPos, same code) */
    /* 0x9D4 */ cXyz mNbtAttentionPosition;  /* was m9D4: setAttention 022F7A60: the attention position for animations 5-7 (chk_nbt_attn); views ym1_b::State and Ym1PartAState mNbtAttentionPosition */
    /* 0x9E0 */ cXyz mHeadPosition;  /* was m9E0, m9E4, m9E8 (three f32, named mHeadPositionX..Z before this type fix): _nodeCB_Head 022F68C4: the head joint's position; setAttention 022F7A60: y - 210 for animation 6; view Ym1PartAState mHeadPosition (d_a_npc_ym1_readable.h) */
    /* 0x9EC */ be<f32> mPreviousMorfFrame;  /* was m9EC: play_animation 022F79D0: the morf frame of the last frame (a smaller one means the animation looped); setAnm_anm 022F9170 0; views State / Ym1PartAState mPreviousMorfFrame */
    /* 0x9F0 */ csXyz mPreviousAngles;  /* was m9F0: lookBack 022F8670: x = the target angle, y / z = the head / backbone angle; chk_parts_notMov 022F9B64 compares them with the current ones; view State mPreviousAngles */
    /* 0x9F6 */ u8 _9F6[2];
    /* 0x9F8 */ be<u32> mEyePositionEnabled;  /* was m9F8: setAttention 022F7A60 copies mEyePosition to eyePos while set; the wait/talk actions store their result; views State / Ym1PartAState mEyePositionEnabled */
    /* 0x9FC */ be<s16> m9FC;
    /* 0x9FE */ be<s16> mAttentionTimer;  /* was m9FE: wait_1 022F9EF4 / wait_2 022FA0A0: cLib_getRndValue(15, 30) and counted down before turning; talk_1 022FA288 resets it; view State mAttentionTimer */
    /* 0xA00 */ be<s16> mKariTimer;               /* GameCube 0x890 */
    /* 0xA02 */ be<s16> mA02;
    /* 0xA04 */ be<s16> mA04;
    /* 0xA06 */ be<s16> mLookAngleY;  /* was mA06: lookBack 022F8670: the target angle in look mode 3 (after our decomp's daNpc_Pf1_c mLookAngleY, same code) */
    /* 0xA08 */ be<u8> mAnimationStopped;  /* was mA08: play_animation 022F79D0: mpMorf->play()'s result, or 1 when the frame went back; setAnm_anm 022F9170 0; NBTwai 022FA5B8 tests it; view State mAnimationStopped (Ym1PartAState: mMorfAnimationStopped) */
    /* 0xA09 */ be<u8> mAnimationWrapped;  /* was mA09: setAnm_anm 022F9170: 0 with a new animation; view State mAnimationWrapped */
    /* 0xA0A */ be<u8> mPresentItem;  /* was mA0A: chk_talk 022F9ACC: dComIfGp_event_getPreItemNo() on a talk with an item, else 0xFF; talk_1 022FA288 0xFF; view State mPresentItem */
    /* 0xA0B */ be<u8> mKariFlag;                 /* GameCube 0x89B */
    /* 0xA0C */ be<u8> mPrevKariFlag;  /* was mA0C: _draw 022F8F30: mKariFlag of this frame (mKariFlag is then cleared); kari_1 022F9E00 goes back to waiting at 0, turn_1 022FA4E4 starts kari again while set (GameCube m89C) */
    /* 0xA0D */ be<u8> mbResLoaded;  /* was mA0D: _create 022F8108: dComIfG_resLoad(..) == cPhs_COMPLEATE_e, create waits while 0 (after our decomp's daNpc_Pf1_c mbResLoaded, same code; GameCube m89D) */
    /* 0xA0E */ be<u8> mbWaitForDemo;  /* was mA0E: _execute 022F8C24: no execution while set and no demo actor, cleared while a demo runs; _draw 022F8F30 draws nothing while set (after our decomp's daNpc_Pf1_c mbWaitForDemo, same code; GameCube m89E) */
    /* 0xA0F */ be<u8> mbNoShapeAngle;  /* was mA0F: _execute 022F8C24: shape_angle follows current.angle only while 0; demo 022F8344 clears it (after our decomp's daNpc_Pf1_c mbNoShapeAngle, same code; GameCube m89F) */
    /* 0xA10 */ be<u8> mA10;
    /* 0xA11 */ be<u8> mbHomeSet;  /* was mA11: _execute 022F8C24: 0 stores the home position words, angles and mRotYTarget once, then 1 (after our decomp's daNpc_Pf1_c mbHomeSet, same code) */
    /* 0xA12 */ be<s8> mPlayerAttending;  /* was mA12: wait_1 022F9EF4 / wait_2 022FA0A0 / SITwai 022FA70C: the attention result, resets mAttentionTimer while set; view State mPlayerAttending */
    /* 0xA13 */ be<s8> mTalkRequested;  /* was mA13: checkOrder 022F8304: 1 on the talk command while mActionSubstate is 1/2; the wait actions start the talk while set, talk_1 022FA288 clears it; set_collision_sp 022F89CC skips while set; view State mTalkRequested */
    /* 0xA14 */ be<s8> mbHeadOnly;  /* was mA14: lookBack 022F8670: the head-only flag of the look-at; kari_1 022F9E00, wait_1 and the other actions set 1 / 0 with the look mode (after our decomp's daNpc_Pf1_c mbHeadOnly, same code; GameCube m8A4) */
    /* 0xA15 */ be<s8> mTurnToPlayer;  /* was mA15: wait_1 022F9EF4 / wait_2 022FA0A0: turn current.angle.y to mRotYTarget while set; NBTwai 022FA5B8 sets it; view State mTurnToPlayer */
    /* 0xA16 */ be<s8> mbDemo;  /* was mA16: demo 022F8344: 1 while demoActorID is set (returned), else 0; _execute 022F8C24 / set_cutGrass 022F88C8 skip while set (after our decomp's daNpc_Pf1_c mbDemo, same code) */
    /* 0xA17 */ be<s8> mActionIndex;  /* was mA17: privateCut 022F8588: dComIfGp_evmng_getMyActIdx(staff, a_cut_tbl, ..), -1 ends (after our decomp's daNpc_Pf1_c mActionIndex, same code) */
    /* 0xA18 */ be<s8> mAnimationAttribute;  /* was mA18: chngAnmAtr 022F9378: the message animation attribute (< 0xD); setAnm_ATR 022F9310 plays its table entry; endEvent 022F8544 / setStt 022F9C2C -1; view State mAnimationAttribute */
    /* 0xA19 */ be<s8> mAnimationTag;  /* was mA19: anmAtr 022F9394: the last message animation tag taken; endEvent 022F8544 / setStt 022F9C2C -1; view State mAnimationTag */
    /* 0xA1A */ be<s8> mBtpNum;  /* was mA1A: init_texPttrnAnm 022F6F38: the btp number; play_texPttrnAnm 022F7918 blinks only at 0; demo 022F8344 1 with the demo's btp; view Ym1PartAState mBtpNum */
    /* 0xA1B */ be<s8> mAnmNum;  /* was mA1B: setAnm_anm 022F9170: anm_prm_c::mAnmNum of the animation set (no change when equal); createInit 022F7D68 0xD; set_cutGrass 022F88C8 / set_collision_sp 022F89CC switch on it; chk_nbt_attn 022F7A3C tests 5-7 (GameCube m8AB) */
    /* 0xA1C */ be<s8> mActionSubstate;  /* was mA1C: the wait actions set 2; checkOrder 022F8304 starts the talk at 1/2 and clears it; eventOrder 022F8890 orders the speak event at 1, can-talk at 1/2; view State mActionSubstate */
    /* 0xA1D */ be<s8> mStatus;  /* was mA1D: setStt 022F9C2C: the state; wait_action1 022FA828 .. run by it; setAnm 022F92A0 plays its table entry; view State mStatus */
    /* 0xA1E */ be<s8> mReturnStatus;  /* was mA1E: setStt 022F9C2C: the state before a talk; talk_1 022FA288 goes back to it; view State mReturnStatus */
    /* 0xA1F */ be<s8> mLookMode;  /* was mA1F: lookBack 022F8670: 1 player's eyes, 2 mLookPos, 3 mLookAngleY; the actions set it (after our decomp's daNpc_Pf1_c mLookMode, same code; GameCube m8AF) */
    /* 0xA20 */ be<s8> mSubType;                  /* GameCube 0x8B0 */
    /* 0xA21 */ be<s8> mStaff;                    /* GameCube 0x8B1 */
    /* 0xA22 */ be<s8> mActionPhase;  /* was mA22: set_action 022F7388: 9 when the old action ends, 0 for the new; wait_action1 022FA828 .. set up at 0 and step to 1; view State mActionPhase (Ym1PartAState: mActionState) */
    /* 0xA23 */ be<s8> mMessageAnimationState;  /* was mA23: anmAtr 022F9394: 0 takes the message's animation attribute once and steps to 1, reset when the message is displayed; view State mMessageAnimationState */
};
WWHD_OFFSET(daNpc_Ym1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ym1_c, mArcName, 0x7EC);
WWHD_OFFSET(daNpc_Ym1_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Ym1_c, mCurrProcFunc, 0x86C);
WWHD_OFFSET(daNpc_Ym1_c, mCyl, 0x874);
WWHD_OFFSET(daNpc_Ym1_c, mHeadPosition, 0x9E0);
WWHD_OFFSET(daNpc_Ym1_c, mKariTimer, 0xA00);
WWHD_OFFSET(daNpc_Ym1_c, mSubType, 0xA20);
WWHD_SIZE(daNpc_Ym1_c, 0xA24);
