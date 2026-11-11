/* daNpc_Ji1_c (Orca), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the constructor (0224D280, inlined in _create) and the verified
 * functions:
 * - fopEn_enemy_c ends at 0x3C8 (GameCube 0x2AC + 0x11C).
 * - The pointers to member functions (Action_t, SubAction_t) are 8 bytes (GHS) instead of 12, so
 *   mSmokeCb is at 0x3EC (+0x10C after the four PTMFs).
 * - dPa_smokeEcallBack is 0x20 in both; mDoExt_btpAnm grew to 0x74, mDoExt_brkAnm to 0x78,
 *   mDoExt_btkAnm to 0x74; mShadowId is gone (HD: no blob shadow).
 * - From the colliders on everything is +0x228 (mAcch 0x65C, the dCcD_Cps at 0xC68), except
 *   pad_0xC20 (gone), so mHitCutType..mAtHitCount are +0x224, and a new HD byte at 0xE64.
 * Size 0xFB0 (GameCube 0xD88). */
#pragma once
#include "bindings.h"
#ifdef DA_NPC_JI1_USE_SHARED_GAME_API
#include "gc_api/d/d_com_inf_game.h"
#endif

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6 */
struct ProcFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(ProcFunc_l, 8);

/* dPa_smokeEcallBack (HD 0x20): vtable, emitter, ... */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x20 - 0x08];
};
WWHD_SIZE(dPa_smokeEcallBack_l, 0x20);
/* mDoExt_btpAnm (HD 0x74), mDoExt_brkAnm (HD 0x78): J3DFrameCtrl first */
struct mDoExt_btpAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x74 - 0x10];
};
struct mDoExt_brkAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x78 - 0x10];
};
WWHD_SIZE(mDoExt_brkAnm_l, 0x78);
struct Quaternion_l {
    be<f32> x, y, z, w;
};

struct daNpc_Ji1_c : fopEn_enemy_c {
    /* methods (GameCube names) */
    BOOL isGuardAnim();
    BOOL isAttackAnim();
    int isAttackFrame();
    BOOL isItemWaitAnim();
    BOOL isClearRecord(s16);
    void setClearRecord(s16);
    void normalSubActionHarpoonGuard(s16);
    void normalSubActionGuard(s16);
    BOOL normalAction(void*);
    BOOL kaitenExpAction(void*);
    BOOL kaitenspeakAction(void*);
    BOOL kaitenwaitAction(void*);
    BOOL kaitenAction(void*);
    u32 getMsg1stType();
    u32 getMsg2ndType();
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    BOOL talkAction(void*);
    BOOL speakAction(void*);
    BOOL speakBadAction(void*);
    static void* initPosObject(void*, void*);
    void initPos(int);
    void createItem();
    void set_mtx();
    s32 getEventActionNo(int);
    BOOL eventAction(void*);
    u32 evn_init_pos_init(int);
    u32 evn_setAnm_init(int);
    u32 evn_talk_init(int);
    u32 evn_talk();
    u32 evn_continue_talk_init(int);
    u32 evn_continue_talk();
    u32 evn_setAngle_init(int);
    u32 evn_sound_proc_init(int);
    u32 evn_head_swing_init(int);
    u32 evn_harpoon_proc_init(int);
    u32 evn_RollAtControl_init(int);
    u32 evn_RollAtControl();
    u32 evn_game_mode_init(int);
    u32 evn_turn_to_player();
    u32 evn_hide_init(int);
    void AnimeControlToWait();
    u32 privateCut();
    u32 setParticle(int, f32, f32);
    void dtParticle();
    u32 setParticleAT(int, f32, f32);
    void dtParticleAT();
    BOOL startspeakAction(void*);
    BOOL endspeakAction(void*);
    BOOL reiAction(void*);
    BOOL plmoveAction(void*);
    BOOL teachMove(f32);
    BOOL teachSpRollCutMove(f32);
    f32 calcCoCorrectValue();
    f32 calcBgCorrectValue();
    BOOL MoveToPlayer(f32, u8);
    void teachSubActionAttackInit();
    BOOL teachSubActionAttack();
    void teachSubActionJumpInit();
    BOOL teachSubActionJump();
    BOOL teachAction(void*);
    BOOL teachSPRollCutAction(void*);
    void battleGameSetTimer();
    BOOL battleMove(f32);
    void battleSubActionWaitInit();
    BOOL battleSubActionWait();
    void battleSubActionNockBackInit(int);
    BOOL battleSubActionNockBack();
    void battleSubActionAttackInit();
    BOOL battleSubActionAttack();
    void battleSubActionTateAttackInit();
    BOOL battleSubActionTateAttack();
    void battleSubActionYokoAttackInit();
    BOOL battleSubActionYokoAttack();
    void battleSubActionJumpInit();
    BOOL battleSubActionJump();
    void battleSubActionDamageInit();
    BOOL battleSubActionDamage();
    void battleSubActionJpGuardInit();
    BOOL battleSubActionJpGuard();
    void battleSubActionGuardInit();
    BOOL battleSubActionGuard();
    BOOL battleAtSet();
    BOOL battleGuardCheck();
    BOOL battleAction(void*);
    BOOL checkCutType(int, int);
    void setAnimFromMsgNo(u32);
    BOOL setAnm(int, f32, int);
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL CreateInit();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    BOOL chkAttention(cXyz*, s16); /* cXyz by value: pointer to a copy */
    BOOL lookBack();
    void setHitParticle(cXyz*, u32);
    void setGuardParticle();
    void BackSlideInit();
    void BackSlide(f32, f32);
    void harpoonRelease(cXyz*);
    void harpoonMove();

    /* 0x3C8 */ be<u32> mMsgNo;
    /* 0x3CC */ ProcFunc_l mAction;
    /* 0x3D4 */ ProcFunc_l mPrevAction;  /* was field_0x2BC: setAction (inline, e.g. in normalAction 02250A1C) copies mAction here before switching; talkAction 0225268C / speakAction 02252FD0 / speakBadAction 02254A64 switch back to it */
    /* 0x3DC */ ProcFunc_l mEventReturnAction;  /* was field_0x2C8: normalSubActionGuard 022504E4 (guard event) copies mAction here before eventAction; eventAction 022596CC switches back to it; evn_talk_init 022583A4 tests it */
    /* 0x3E4 */ ProcFunc_l mSubAction;
    /* 0x3EC */ dPa_smokeEcallBack_l mSmokeCb;
    /* 0x40C */ dPa_smokeEcallBack_l mSmokeCbAT;
    /* 0x42C */ cXyz mSmokePos;  /* was field_0x320: battleSubActionTateAttack 0225B85C copies the spear tip (y 0) / YokoAttack 0225BBA4 sets it 200 around the body; position of the ID_AK_JT_ELEMENTSMOKE00 emitter (0225B720) */
    /* 0x438 */ be<s16> mSmokeAngle;  /* was field_0x32C: battleSubActionYokoAttack 0225BBA4: steps -0x1400 per frame, mSmokePos = pos + 200 * (sin, cos) of it */
    /* 0x43A */ u8 _43A[2];
    /* 0x43C */ gptr<mDoExt_McaMorf> mpOrcaMorf;
    /* 0x440 */ dNpc_EventCut_c mEventCut;
    /* 0x4AC */ dNpc_JntCtrl_c m_jnt;
    /* 0x4E0 */ gptr<J3DAnmTexPattern> headTexPattern;
    /* 0x4E4 */ mDoExt_btpAnm_l mBlinkAnim;
    /* 0x558 */ be<u8> mBlinkFrame;
    /* 0x559 */ u8 _559;
    /* 0x55A */ be<s16> mBlinkTimer;
    /* 0x55C */ gptr<mDoExt_McaMorf> mpSpearMorf;     /* HD: no mShadowId before it */
    /* 0x560 */ gptr<J3DModel> mpTearsModel;
    /* 0x564 */ mDoExt_brkAnm_l mCryBrk;
    /* 0x5DC */ be<f32> mCryBrkFrame;
    /* 0x5E0 */ mDoExt_btkAnm mCryBtk;
    /* 0x654 */ be<f32> mCryBtkFrame;
    /* 0x658 */ gptr<JPABaseEmitter> mpTearEmitter;  /* was field_0x430: setAnm 0224DFF0 sets dPa_ID_AK_SN_JITEAR00 at current.pos when NULL; setAnimFromMsgNo 02251A8C ends it (becomeInvalidEmitter) */
    /* 0x65C */ dBgS_ObjAcch mAcch;
    /* 0x820 */ dBgS_AcchCir mAcchCir;
    /* 0x860 */ dCcD_Stts mStts;  /* was field_0x638: CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mCyl, mAtCyl and mAtCps; _execute 0224B268 moves by its m_cc_move (fopAcM_posMoveF) */
    /* 0x89C */ dCcD_Stts mGuardStts;  /* was field_0x674: CreateInit 0224C650: Init(0xFF, 0xFF, this), status of mGuardCyl */
    /* 0x8D8 */ dCcD_Cyl mCyl;  /* was field_0x6B0: CreateInit 0224C650: Set(l_cyl_src), mStts; _execute 0224B268 SetC(current.pos) every frame */
    /* 0xA08 */ dCcD_Cyl mGuardCyl;  /* was field_0x7E0: CreateInit 0224C650: Set(l_cyl2_src); battleSubActionNockBack 0225C208 OnTgShield/ChkTgShieldHit; setGuardParticle 02248D3C spark at its TG hit position */
    /* 0xB38 */ dCcD_Cyl mAtCyl;  /* was field_0x910: CreateInit 0224C650: Set(l_cylAt_src); MoveToPlayer 0224F628 SetC(current.pos) and sets it at the attack's last frames */
    /* 0xC68 */ dCcD_Cps mAtCps;  /* was field_0xA40: CreateInit 0224C650: Set(l_cpsAt_src) from mSpearButtPos to mSpearTipPos; battleAtSet 0225B4AC sets radius and start/end (mSpearTipOldPos -> mSpearTipPos) */
    /* 0xDA0 */ cXyz mSpearTipPos;  /* was field_0xB78: set_mtx 0224AEF0: the spear joint matrix times (0, 0, 100) (70 for the side attack) */
    /* 0xDAC */ cXyz mSpearButtPos;  /* was field_0xB84: set_mtx 0224AEF0: the spear joint matrix times (0, 0, -100); _delete 0224C550 deletes its sound object */
    /* 0xDB8 */ cXyz mSpearTipOldPos;  /* was field_0xB90: set_mtx 0224AEF0 copies mSpearTipPos here before updating it; battleAtSet 0225B4AC uses it as the capsule start */
    /* 0xDC4 */ be<s8> handRJointNo;
    /* 0xDC5 */ be<s8> hair1JointNo;
    /* 0xDC6 */ be<s8> hair2JointNo;
    /* 0xDC7 */ be<s8> hair3JointNo;
    /* 0xDC8 */ be<s8> armLJointNo;
    /* 0xDC9 */ be<s8> armRJointNo;
    /* 0xDCA */ be<s16> mArmShakeAmp;  /* was field_0xBA2: battleSubActionNockBack 0225C208 sets t * 0x100; nodeCallBack3 022494C8 X-rotates both arm joints by it * sin(mArmShakePhase) */
    /* 0xDCC */ be<s16> mArmShakePhase;  /* was field_0xBA4: battleSubActionNockBack 0225C208 sets t * 0x2800; nodeCallBack3 022494C8: sin phase of the arm shake */
    /* 0xDCE */ be<s16> mHeadPitch;  /* was field_0xBA6: setHairAngle (inlined in _execute 0224B268): atan2 of the head front vector's y against its xz length */
    /* 0xDD0 */ be<s16> mHeadYaw;  /* was field_0xBA8: setHairAngle (inlined in _execute 0224B268): atan2(x, z) of the head front vector */
    /* 0xDD2 */ be<s16> mHair1AngleZ;  /* was field_0xBAA: setHairAngle (in _execute 0224B268) eases and clamps it (HIO limits); nodeCallBack2 02249224 Z-rotates the hair1 joint by it */
    /* 0xDD4 */ be<s16> mHair1AngleY;  /* was field_0xBAC: setHairAngle (in _execute 0224B268) eases and clamps it; nodeCallBack2 02249224 Y-rotates the hair1 joint by it */
    /* 0xDD6 */ be<s16> mHair2AngleZ;  /* was field_0xBAE: setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair2 joint by it */
    /* 0xDD8 */ be<s16> mHair2AngleY;  /* was field_0xBB0: setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair2 joint by it */
    /* 0xDDA */ be<s16> mHair3AngleZ;  /* was field_0xBB2: setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Z-rotates the hair3 joint by it */
    /* 0xDDC */ be<s16> mHair3AngleY;  /* was field_0xBB4: setHairAngle (in _execute 0224B268); nodeCallBack2 02249224 Y-rotates the hair3 joint by it */
    /* 0xDDE */ be<s16> mHair1SpeedZ;  /* was field_0xBB6: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleZ, added next frame */
    /* 0xDE0 */ be<s16> mHair1SpeedY;  /* was field_0xBB8: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair1AngleY, added next frame */
    /* 0xDE2 */ be<s16> mHair2SpeedZ;  /* was field_0xBBA: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleZ, added next frame */
    /* 0xDE4 */ be<s16> mHair2SpeedY;  /* was field_0xBBC: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair2AngleY, added next frame */
    /* 0xDE6 */ be<s16> mHair3SpeedZ;  /* was field_0xBBE: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleZ, added next frame */
    /* 0xDE8 */ be<s16> mHair3SpeedY;  /* was field_0xBC0: setHairAngle (in _execute 0224B268): a tenth of this frame's change of mHair3AngleY, added next frame */
    /* 0xDEA */ be<s16> field_0xBC2;
    /* 0xDEC */ cXyz mHair1LastPos;  /* was field_0xBC4: setHairAngle (in _execute 0224B268): the hair1 joint position, kept for the next frame (its motion drives the hair) */
    /* 0xDF8 */ be<s16> mHairWindPhase;  /* was field_0xBD0: setHairAngle (in _execute 0224B268): advanced by 1500 + 4600 * wind each frame; cos phase of the wind sway */
    /* 0xDFA */ be<s16> mHair1WindAngle;  /* was field_0xBD2: setHairAngle (in _execute 0224B268): wind * 2280 * cos(mHairWindPhase); nodeCallBack2 02249224 adds it to the hair1 Z angle */
    /* 0xDFC */ be<s16> mHair2WindAngle;  /* was field_0xBD4: setHairAngle (in _execute 0224B268): wind * 3908 * cos(...); nodeCallBack2 02249224 adds it to the hair2 Z angle */
    /* 0xDFE */ be<s16> mHair3WindAngle;  /* was field_0xBD6: setHairAngle (in _execute 0224B268): wind * 7568 * cos(...); nodeCallBack2 02249224 adds it to the hair3 Z angle */
    /* 0xE00 */ cXyz mBackbonePos[3];  /* was field_0xBD8: nodeCallBack1 02249084: the backbone joint matrix times l_hio's three offsets (HIO 0x0C0) */
    /* 0xE24 */ u8 mHeadAnm[0x24];             /* dNpc_HeadAnm_c */
    /* 0xE48 */ be<s32> mHitCutType;  /* was field_0xC24: daJi1_TgHitCallback 02248CF0 stores the player's cut type; teachAction 02255908 / teachSPRollCutAction 02256F1C / battleSubActionGuard 0225C7EC switch on it; HD: pad_0xC20 gone */
    /* 0xE4C */ be<u32> mCoHit;  /* was field_0xC28: daJi1_CoHitCallback 02248CA4 sets 1; _execute 0224B268 clears it every frame */
    /* 0xE50 */ be<s32> mAttackEndWait;  /* was field_0xC2C: MoveToPlayer 0224F628 sets 0x14 at the attack animation's end, then counts it down returning TRUE while > 0 */
    /* 0xE54 */ be<s32> mBattleTimer;  /* was field_0xC30: battleGameSetTimer 02259A6C sets a random time; battleSubActionWait 0225B344 counts it down and acts at < 0; battleAction 0225AEC8 sets 30 + rnd(150) */
    /* 0xE58 */ be<s32> mNockBackTimer;  /* was field_0xC34: the attack inits (0225B2B8, 0225B22C, 0225B1A0) clear it; battleSubActionNockBack 0225C208 counts it and scales the arm shake by it */
    /* 0xE5C */ be<s32> field_0xC38;
    /* 0xE60 */ be<s32> mAtHitCount;  /* was field_0xC3C: daJi1_AtHitCallback 02248EA0 adds 1; battleAction 0225AEC8 clears it and shows 3 - it as life; battleGuardCheck 0225A534 ends the game when > 2 */
    /* 0xE64 */ union { be<u8> mHitCooldownHD; be<u8> mHD_E64; };                /* HD only */
    /* 0xE65 */ u8 _E65[3];
    /* 0xE68 */ cXyz mBgCorrectVec;  /* was field_0xC40: _execute 0224B268 copies the BG correction difference; calcBgCorrectValue 0224F59C dots it with the facing direction */
    /* 0xE74 */ be<f32> mCoCorrectSum;  /* was field_0xC4C: calcCoCorrectValue 0224F528 adds mStts' m_cc_move along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 */
    /* 0xE78 */ be<f32> mBgCorrectSum;  /* was field_0xC50: calcBgCorrectValue 0224F59C adds mBgCorrectVec along the facing direction each call; cleared by CreateInit 0224C650 / MoveToPlayer 0224F628 */
    /* 0xE7C */ be<s16> mEventIdx[0x12];
    /* 0xEA0 */ be<s8> mActionPhase;  /* was field_0xC78: setAction (inline) sets -1 before calling the old action, 0 for the new one; every action (e.g. battleAction 0225AEC8, eventAction 022596CC) inits at 0 and adds 1 */
    /* 0xEA1 */ u8 _EA1[3];
    /* 0xEA4 */ request_of_phase_process_class mPhs;
    /* 0xEAC */ be<u32> mEventNo;  /* was field_0xC84: eventAction 022596CC orders mEventIdx[it], then sets 0x12 (none); _create 0224D280 sets 0x12; the guard event (022504E4) / kaitenwaitAction 02250D54 set the event to play */
    /* 0xEB0 */ be<s16> mLookMaxVel;  /* was field_0xC88: lookBack 0224ABA4 eases it to HIO 0x18 while m_jnt turns, else clears it; passed as lookAtTarget's maxVel (TWW d_npc.h parameter name) */
    /* 0xEB2 */ u8 _EB2[2];
    /* 0xEB4 */ be<s32> mTeachCutType;  /* was field_0xC8C: teachAction 02255908 / teachSPRollCutAction 02256F1C copy mHitCutType here and test it (CUT_TYPE_CUT_ROLL) */
    /* 0xEB8 */ be<s32> mTeachResult;  /* was field_0xC90: the result of a lesson cut: teachAction 02255908 sets 1 for a right cut with hits left (orders event 1, then speakAction), 2 for a wrong cut (event 3, speakBadAction), 3 for the last needed cut (event 1 / 2 / 15, then speakAction or endspeakAction), and copies mRollTeachResult into it after a spin attack; speakAction 02252FD0 picks the reply message by it (1 / 3 per lesson step); CreateInit 0224C650 / teachSPRollCutAction 02256F1C clear it (tww the same) */
    /* 0xEBC */ be<s32> field_0xC94;
    /* 0xEC0 */ be<s32> mTeachHitsLeft;  /* was field_0xC98: CreateInit 0224C650 sets HIO 0x54[mTeachStep]; teachAction 02255908 counts it down per good cut, the lesson ends at 0 */
    /* 0xEC4 */ be<f32> mBackSlideDist;  /* was field_0xC9C: BackSlideInit 0224DE14 and every battle subaction init clear it; BackSlide 022525B4 eases it to the slide distance behind mBackSlideStartPos */
    /* 0xEC8 */ Mtx34 mHarpoonMtx;  /* was field_0xCA0: harpoonRelease 0224DE3C takes the held harpoon's matrix; harpoonMove 0224A2B8 concatenates it */
    /* 0xEF8 */ cXyz mHarpoonPos;  /* was field_0xCD0: harpoonRelease 0224DE3C: translation of mHarpoonMtx; harpoonMove 0224A2B8 adds mHarpoonSpeed each frame */
    /* 0xF04 */ cXyz mHarpoonSpeed;  /* was field_0xCDC: harpoonRelease 0224DE3C copies the release velocity; harpoonMove 0224A2B8 adds it to mHarpoonPos and gravity (HIO) to y */
    /* 0xF10 */ u8 pad_0xCE8[0xC];
    /* 0xF1C */ Quaternion_l mHarpoonRot;  /* was field_0xCF4: harpoonRelease 0224DE3C sets the identity quaternion; harpoonMove 0224A2B8 multiplies it by mHarpoonRotStep and normalizes */
    /* 0xF2C */ Quaternion_l mHarpoonRotStep;  /* was field_0xD04: harpoonRelease 0224DE3C builds it from mHarpoonSwing and the facing; harpoonMove 0224A2B8 multiplies mHarpoonRot by it */
    /* 0xF3C */ be<s16> mHarpoonSwing;  /* was field_0xD14: harpoonRelease 0224DE3C sets 0x400 and builds mHarpoonRotStep from sin of it */
    /* 0xF3E */ be<s16> mRollAtAngle;  /* was field_0xD16: evn_RollAtControl_init 02258A14 sets the start angle + 0x2000; evn_RollAtControl 02258E6C adds 0x540 per frame, mRollAtPos = pos + mRollAtRadius * (sin, cos) */
    /* 0xF40 */ be<f32> mRollAtRadius;  /* was field_0xD18: evn_RollAtControl_init 02258A14 sets 400; evn_RollAtControl 02258E6C chases it to 0 */
    /* 0xF44 */ cXyz mRollAtPos;  /* was field_0xD1C: evn_RollAtControl 02258E6C: position on the circle of mRollAtAngle / mRollAtRadius */
    /* 0xF50 */ cXyz mInitPos;  /* was field_0xD28: CreateInit 0224C650 / kaitenAction 02251ED0 copy home.pos; initPos 02254390 puts current.pos and old.pos there */
    /* 0xF5C */ be<s32> mRollTeachResult;  /* was field_0xD34: teachAction 02255908: the same result values (1 / 2 / 3) for a spin attack (CUT_TYPE_CUT_ROLL), held until Link's cut ends, then copied into mTeachResult (speakAction / speakBadAction); teachSPRollCutAction 02256F1C clears it (tww the same) */
    /* 0xF60 */ cXyz mBackSlideStartPos;  /* was field_0xD38: BackSlideInit 0224DE14 and the subaction inits copy current.pos; BackSlide 022525B4 slides back from it */
    /* 0xF6C */ u8 pad_0xD44[0xC];
    /* 0xF78 */ cXyz mHeldItemOffset;  /* was field_0xD50: CreateInit 0224C650 sets (-13, -4, 10); _execute 0224B268 eases it to the item/normal aim offset; set_mtx 0224AEF0 translates by it */
    /* 0xF84 */ csXyz mHeldItemAngle;  /* was field_0xD5C: CreateInit 0224C650 sets x 0x4000; _execute 0224B268 sets it to the aim angle; set_mtx 0224AEF0 rotates by it (XYZ) */
    /* 0xF8A */ u8 _F8A[2];
    /* 0xF8C */ be<s32> mAnimation;
    /* 0xF90 */ be<s32> field_0xD68;
    /* 0xF94 */ be<s32> mSubActionPhase;  /* was field_0xD6C: every battle subaction (0225C208, 0225CA04, 0225C660, 0225C7EC, ...) starts at 0 and steps it; normalSubActionGuard 022504E4 sets 1 on a guard hit */
    /* 0xF98 */ be<s32> mTeachStep;  /* was field_0xD70: CreateInit 0224C650 clears it and picks HIO 0x54[it]; teachAction 02255908 tests == 4; teachSPRollCutAction sets the message count number to it */
    /* 0xF9C */ be<s32> mGuardCount;  /* was field_0xD74: normalSubActionHarpoonGuard 0224FCC4 adds 1 per guarded hit; normalSubActionGuard 022504E4 plays the guard event at 3, 5 and > 5 (then 0) */
    /* 0xFA0 */ be<u8> mFirstMsgDone;  /* was field_0xD78: getMsg1stType 0224D8E0: returns 0x978 once and sets it, 0x97B after */
    /* 0xFA1 */ be<u8> field_0xD79;
    /* 0xFA2 */ be<u8> field_0xD7A;
    /* 0xFA3 */ be<u8> field_0xD7B;
    /* 0xFA4 */ be<u8> field_0xD7C;
    /* 0xFA5 */ be<u8> mCreateItemNo;
    /* 0xFA6 */ be<u8> field_0xD7E;
    /* 0xFA7 */ u8 _FA7;
    /* 0xFA8 */ be<u32> mEndMsgNo;
    /* 0xFAC */ be<u8> mHarpoonState;  /* was field_0xD84: _draw 02249638 / set_mtx 0224AEF0: Ji1Draw_HARPOON_HELD (1) / _RELEASED; harpoonRelease 0224DE3C and evn_harpoon_proc_init 0225896C set it */
    /* 0xFAD */ be<u8> mHide;
    /* 0xFAE */ u8 _FAE[2];
};
WWHD_OFFSET(daNpc_Ji1_c, mAction, 0x3CC);
WWHD_OFFSET(daNpc_Ji1_c, mSubAction, 0x3E4);
WWHD_OFFSET(daNpc_Ji1_c, mpOrcaMorf, 0x43C);
WWHD_OFFSET(daNpc_Ji1_c, m_jnt, 0x4AC);
WWHD_OFFSET(daNpc_Ji1_c, mCryBrk, 0x564);
WWHD_OFFSET(daNpc_Ji1_c, mCryBtk, 0x5E0);
WWHD_OFFSET(daNpc_Ji1_c, mAcch, 0x65C);
WWHD_OFFSET(daNpc_Ji1_c, mAcchCir, 0x820);
WWHD_OFFSET(daNpc_Ji1_c, mStts, 0x860);
WWHD_OFFSET(daNpc_Ji1_c, mAtCps, 0xC68);
WWHD_OFFSET(daNpc_Ji1_c, mSpearTipPos, 0xDA0);
WWHD_OFFSET(daNpc_Ji1_c, mHeadAnm, 0xE24);
WWHD_OFFSET(daNpc_Ji1_c, mHitCutType, 0xE48);
WWHD_OFFSET(daNpc_Ji1_c, mBgCorrectVec, 0xE68);
WWHD_OFFSET(daNpc_Ji1_c, mEventIdx, 0xE7C);
WWHD_OFFSET(daNpc_Ji1_c, mActionPhase, 0xEA0);
WWHD_OFFSET(daNpc_Ji1_c, mPhs, 0xEA4);
WWHD_OFFSET(daNpc_Ji1_c, mHarpoonMtx, 0xEC8);
WWHD_OFFSET(daNpc_Ji1_c, mHeldItemAngle, 0xF84);
WWHD_OFFSET(daNpc_Ji1_c, mAnimation, 0xF8C);
WWHD_OFFSET(daNpc_Ji1_c, mEndMsgNo, 0xFA8);
WWHD_SIZE(daNpc_Ji1_c, 0xFB0);

/* daNpc_Ji1_HIO_c, HD: the vtable pointer follows the members (0xFC), so every GameCube offset
 * is -4. l_HIO is at 0x104672AC. Names are the GameCube offsets. */
struct daNpc_Ji1_HIO_c {
    void genMessage(void* context);
    /* 0x000 */ be<s8> mNo;
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ be<f32> field_0x08;
    /* 0x008 */ be<s16> mMaxHeadX;  /* was field_0x0C: lookBack 0224ABA4 passes it to m_jnt.setParam as the head's max X angle (and its negative as the min); daNpc_Ji1_HIO_ct 0225CC44 7000; same role as dNpc_HIO_c::mMaxHeadX */
    /* 0x00A */ be<s16> mMaxBackboneX;  /* was field_0x0E: lookBack 0224ABA4 passes it to m_jnt.setParam as the backbone's max X angle (and its negative as the min); daNpc_Ji1_HIO_ct 0225CC44 4000; same role as dNpc_HIO_c::mMaxBackboneX */
    /* 0x00C */ be<s16> mMaxHeadY;  /* was field_0x10: lookBack 0224ABA4 passes it to m_jnt.setParam as the head's max Y angle (its negative as the head and backbone min; 0x3E80 instead during BTKAMASI / BTKAMAE); chkAttention 0224AA24 adds it to mMaxBackboneY as the attention angle; daNpc_Ji1_HIO_ct 0225CC44 9000; same role as dNpc_HIO_c::mMaxHeadY */
    /* 0x00E */ be<s16> mMaxBackboneY;  /* was field_0x12: lookBack 0224ABA4 passes it to m_jnt.setParam as the backbone's max Y angle (0x1F40 instead during BTKAMASI / BTKAMAE); chkAttention 0224AA24 adds it to mMaxHeadY as the attention angle; daNpc_Ji1_HIO_ct 0225CC44 7000; same role as dNpc_HIO_c::mMaxBackboneY */
    /* 0x010 */ be<s16> mMaxTurnStep;  /* was field_0x14: lookBack 0224ABA4 passes it to m_jnt.setParam as the turn step; daNpc_Ji1_HIO_ct 0225CC44 5000; same role as dNpc_HIO_c::mMaxTurnStep */
    /* 0x012 */ be<s16> mHeadTurnVelScale;  /* was field_0x16: lookBack 0224ABA4: the scale of cLib_addCalcAngleS2 that eases mLookMaxVel toward mMaxHeadTurnVel while turning; daNpc_Ji1_HIO_ct 0225CC44 1 */
    /* 0x014 */ be<s16> mMaxHeadTurnVel;  /* was field_0x18: lookBack 0224ABA4: cLib_addCalcAngleS2 eases mLookMaxVel toward it while turning (else 0); daNpc_Ji1_HIO_ct 0225CC44 2000; same role as dNpc_HIO_c::mMaxHeadTurnVel */
    /* 0x016 */ be<u8> field_0x1A;
    /* 0x017 */ u8 _017;
    /* 0x018 */ be<f32> field_0x1C;
    /* 0x01C */ be<f32> field_0x20;
    /* 0x020 */ be<f32> field_0x24;
    /* 0x024 */ be<u8> field_0x28;
    /* 0x025 */ u8 _025[3];
    /* 0x028 */ be<f32> mTalkDistXZ;  /* was field_0x2C: normalAction 02250A1C / kaitenwaitAction 02250D54: Orca can be talked to (dEvtCnd_CANTALK_e) only while Link is within this XZ distance; daNpc_Ji1_HIO_ct 0225CC44 200.0 */
    /* 0x02C */ be<u8> mbRepeatKaiten;  /* was field_0x30: CreateInit 0224C650 starts kaitenExpAction when it is set even with event bit 0x0640; kaitenwaitAction 02250D54 reacts to Link's front-roll crash when it is set even with event bit 0x0501; daNpc_Ji1_HIO_ct 0225CC44 0 */
    /* 0x02D */ u8 _02D[3];
    /* 0x030 */ be<f32> mJpGuardSlideDist;  /* was field_0x34: battleSubActionJpGuard 0225C660 / teachAction 02255908 (animation 0xA, twice it on the last hit) / teachSPRollCutAction 02256F1C (twice it): cLib_addCalc2 target of mBackSlideDist, the jump-guard slide back; daNpc_Ji1_HIO_ct 0225CC44 150.0 */
    /* 0x034 */ be<f32> mJpGuardSlideStep;  /* was field_0x38: battleSubActionJpGuard 0225C660 / teachAction 02255908 / teachSPRollCutAction 02256F1C: cLib_addCalc2 max step of mBackSlideDist toward mJpGuardSlideDist; daNpc_Ji1_HIO_ct 0225CC44 20.0 */
    /* 0x038 */ be<f32> mGuardSlideDist;  /* was field_0x3C: teachAction 02255908: cLib_addCalc2 target of mBackSlideDist for the other guard animations; daNpc_Ji1_HIO_ct 0225CC44 90.0 */
    /* 0x03C */ be<f32> mGuardSlideStep;  /* was field_0x40: teachAction 02255908 / battleSubActionGuard 0225C7EC: cLib_addCalc2 max step of mBackSlideDist on a guard; daNpc_Ji1_HIO_ct 0225CC44 10.0 */
    /* 0x040 */ be<f32> mInasiAnmSpeed;  /* was field_0x44: setAnm 0224DFF0: the play speed of animation 7 (ji_inasi.bck); daNpc_Ji1_HIO_ct 0225CC44 2.0 */
    /* 0x044 */ be<f32> mKrotAnmSpeed;  /* was field_0x48: setAnm 0224DFF0: the play speed of animation 6 (ji_krot.bck); battleMove 02259EC4 / teachMove 0224EF14 / teachSpRollCutMove 0224F368 (scaled) and MoveToPlayer 0224F628 set the body's play speed from it; daNpc_Ji1_HIO_ct 0225CC44 2.0 */
    /* 0x048 */ be<f32> mWaraiAnmSpeed;  /* was field_0x4C: setAnm 0224DFF0: the play speed of animation 13 (ji_warai.bck); daNpc_Ji1_HIO_ct 0225CC44 1.25 */
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<s16> mTeachHitCount[6];  /* was field_0x54: CreateInit 0224C650 / speakAction 02252FD0 / startspeakAction 02253880 load mTeachHitsLeft with [mTeachStep] (or [0]); teachAction 02255908 counts the hits against it; daNpc_Ji1_HIO_ct 0225CC44 {9, 9, 9, 2, 2, 2} */
    /* 0x05C */ be<s16> mLevelHits[4];  /* was field_0x60: the battle-game hit thresholds: isClearRecord 0224D5BC / setClearRecord 0224D654 compare the hits with [level]; battleGameSetTimer 02259A6C picks the timer segment by them; createItem 02258574 / battleGuardCheck 0225A534 end the game at [3]; daNpc_Ji1_HIO_ct 0225CC44 {100, 300, 500, 1000} */
    /* 0x064 */ be<u8> field_0x68;
    /* 0x065 */ u8 _065[3];
    /* 0x068 */ be<f32> mAttackAnmSpeed;  /* was field_0x6C: battleSubActionAttackInit 0225B2B8 / battleSubActionAttack 0225BF44: the attack animation's play speed up to frame 10; daNpc_Ji1_HIO_ct 0225CC44 0.7 (tww the same) */
    /* 0x06C */ be<f32> mAttackSwingAnmSpeed;  /* was field_0x70: battleSubActionAttack 0225BF44: the attack animation's play speed after frame 10; daNpc_Ji1_HIO_ct 0225CC44 1.5 (tww the same) */
    /* 0x070 */ be<f32> mTateYokoSwingAnmSpeed;  /* was field_0x74: battleSubActionTateAttack 0225B85C (after frame 26) / battleSubActionYokoAttack 0225BBA4 (after frame 25): the swing's play speed; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) */
    /* 0x074 */ be<f32> mTateAnmSpeed;  /* was field_0x78: battleSubActionTateAttackInit 0225B22C sets it; battleSubActionTateAttack 0225B85C / battleSubActionYokoAttack 0225BBA4: the play speed before the swing; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) */
    /* 0x078 */ be<f32> field_0x7C;
    /* 0x07C */ be<f32> mYokoAnmSpeed;  /* was field_0x80: battleSubActionYokoAttackInit 0225B1A0: the side attack's starting play speed; daNpc_Ji1_HIO_ct 0225CC44 1.4 (tww the same) */
    /* 0x080 */ be<f32> mNockBackAnmSpeed;  /* was field_0x84: battleSubActionAttack 0225BF44 / TateAttack 0225B85C / YokoAttack 0225BBA4 / NockBack 0225C208: the body's play speed while mNockBackTimer runs; daNpc_Ji1_HIO_ct 0225CC44 0.1 */
    /* 0x084 */ be<s16> mNockBackTime;  /* was field_0x88: the attack sub-actions load mNockBackTimer with it when Link's shield blocks the attack (ChkTgShieldHit); daNpc_Ji1_HIO_ct 0225CC44 11 */
    /* 0x086 */ be<s16> mBattleTimerBase0;  /* was field_0x8A: battleGameSetTimer 02259A6C: mBattleTimer base at 0 hits (segment 0's start); daNpc_Ji1_HIO_ct 0225CC44 50 (tww the same) */
    /* 0x088 */ be<s16> mBattleTimerBase1;  /* was field_0x8C: battleGameSetTimer 02259A6C: mBattleTimer base at mLevelHits[0] (segment 1's start, also segment 3's); daNpc_Ji1_HIO_ct 0225CC44 40 (tww the same) */
    /* 0x08A */ be<s16> mBattleTimerBase2;  /* was field_0x8E: battleGameSetTimer 02259A6C: mBattleTimer base, segment 0's end and segment 2's start; daNpc_Ji1_HIO_ct 0225CC44 30 (tww the same) */
    /* 0x08C */ be<s16> mBattleTimerBase3;  /* was field_0x90: battleGameSetTimer 02259A6C: mBattleTimer base, segment 1's end; daNpc_Ji1_HIO_ct 0225CC44 20 (tww the same) */
    /* 0x08E */ be<s16> mBattleTimerBase4;  /* was field_0x92: battleGameSetTimer 02259A6C: mBattleTimer base, segment 2's end; daNpc_Ji1_HIO_ct 0225CC44 15 (tww the same) */
    /* 0x090 */ be<s16> mBattleTimerBase5;  /* was field_0x94: battleGameSetTimer 02259A6C: mBattleTimer base, segment 3's end and past mLevelHits[3]; daNpc_Ji1_HIO_ct 0225CC44 10 (tww the same) */
    /* 0x092 */ be<s16> mBattleTimerRange0;  /* was field_0x96: battleGameSetTimer 02259A6C: the random part (cM_rndF) of mBattleTimer at 0 hits (segment 0's start); daNpc_Ji1_HIO_ct 0225CC44 180 (tww the same) */
    /* 0x094 */ be<s16> mBattleTimerRange1;  /* was field_0x98: battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 1's start (also segment 3's); daNpc_Ji1_HIO_ct 0225CC44 140 (tww the same) */
    /* 0x096 */ be<s16> mBattleTimerRange2;  /* was field_0x9A: battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 0's end and segment 2's start; daNpc_Ji1_HIO_ct 0225CC44 100 (tww the same) */
    /* 0x098 */ be<s16> mBattleTimerRange3;  /* was field_0x9C: battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 1's end; daNpc_Ji1_HIO_ct 0225CC44 80 (tww the same) */
    /* 0x09A */ be<s16> mBattleTimerRange4;  /* was field_0x9E: battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 2's end; daNpc_Ji1_HIO_ct 0225CC44 60 (tww the same) */
    /* 0x09C */ be<s16> mBattleTimerRange5;  /* was field_0xA0: battleGameSetTimer 02259A6C: the random part of mBattleTimer, segment 3's end and past mLevelHits[3]; daNpc_Ji1_HIO_ct 0225CC44 50 (tww the same) */
    /* 0x09E */ be<u8> field_0xA2;
    /* 0x09F */ u8 _09F;
    /* 0x0A0 */ be<f32> mBibiriAnmMorf;  /* was field_0xA4: setAnimFromMsgNo 02251A8C: the morf of animation 0x16 (ji_bibiri.bck); daNpc_Ji1_HIO_ct 0225CC44 6.0 */
    /* 0x0A4 */ be<f32> mOdorokuAnmMorf;  /* was field_0xA8: setAnimFromMsgNo 02251A8C: the morf of animation 0x15 (ji_odoroku.bck); daNpc_Ji1_HIO_ct 0225CC44 16.0 */
    /* 0x0A8 */ be<f32> mBibiriSlideDist;  /* was field_0xAC: talkAction 0225268C: BackSlide's target distance once the bibiri animation passes mBibiriSlideFrame; daNpc_Ji1_HIO_ct 0225CC44 70.0 */
    /* 0x0AC */ be<f32> mBibiriSlideStep;  /* was field_0xB0: talkAction 0225268C: BackSlide's max step during the bibiri animation; daNpc_Ji1_HIO_ct 0225CC44 10.0 */
    /* 0x0B0 */ be<f32> mBibiriSlideFrame;  /* was field_0xB4: talkAction 0225268C: the bibiri animation slides Orca back from this frame on; daNpc_Ji1_HIO_ct 0225CC44 1.0 */
    /* 0x0B4 */ be<u8> field_0xB8;
    /* 0x0B5 */ u8 _0B5[3];
    /* 0x0B8 */ be<f32> mHarpoonBounce;  /* was field_0xBC: harpoonMove 0224A2B8: the dropped harpoon's speed.y is multiplied by it when it hits the ground; daNpc_Ji1_HIO_ct 0225CC44 0.25 */
    /* 0x0BC */ be<f32> mHarpoonGravity;  /* was field_0xC0: harpoonMove 0224A2B8: added to the dropped harpoon's speed.y each frame; daNpc_Ji1_HIO_ct 0225CC44 -5.0 */
    /* 0x0C0 */ cXyz mBackboneOffsets[3];  /* was field_0xC4: nodeCallBack1 02249084: the backbone joint matrix times these three points gives mBackbonePos[0..2]; daNpc_Ji1_HIO_ct 0225CC44 (90, 5, 30), (90, 5, -30), (-20, -20, 0) */
    /* 0x0E4 */ be<s16> mHair1MinY;  /* was field_0xE8: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleY; daNpc_Ji1_HIO_ct 0225CC44 -0x3800 */
    /* 0x0E6 */ be<s16> mHair1MaxY;  /* was field_0xEA: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleY; daNpc_Ji1_HIO_ct 0225CC44 0x3800 */
    /* 0x0E8 */ be<s16> mHair2MinY;  /* was field_0xEC: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleY + mHair2AngleY; daNpc_Ji1_HIO_ct 0225CC44 -0x3A00 */
    /* 0x0EA */ be<s16> mHair2MaxY;  /* was field_0xEE: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleY + mHair2AngleY; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 */
    /* 0x0EC */ be<s16> mHair3MinY;  /* was field_0xF0: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of the three hair joints' Y angles together; daNpc_Ji1_HIO_ct 0225CC44 -0x3A00 */
    /* 0x0EE */ be<s16> mHair3MaxY;  /* was field_0xF2: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of the three hair joints' Y angles together; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 */
    /* 0x0F0 */ be<s16> mHair1MinZ;  /* was field_0xF4: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleZ; daNpc_Ji1_HIO_ct 0225CC44 -0x500 */
    /* 0x0F2 */ be<s16> mHair1MaxZ;  /* was field_0xF6: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleZ; daNpc_Ji1_HIO_ct 0225CC44 0x3800 */
    /* 0x0F4 */ be<s16> mHair2MinZ;  /* was field_0xF8: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of mHair1AngleZ + mHair2AngleZ; daNpc_Ji1_HIO_ct 0225CC44 -0xA00 */
    /* 0x0F6 */ be<s16> mHair2MaxZ;  /* was field_0xFA: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of mHair1AngleZ + mHair2AngleZ; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 */
    /* 0x0F8 */ be<s16> mHair3MinZ;  /* was field_0xFC: _execute 0224B268 (daNpc_Ji1_setHairAngle): the lower limit of the three hair joints' Z angles together; daNpc_Ji1_HIO_ct 0225CC44 -0xF00 */
    /* 0x0FA */ be<s16> mHair3MaxZ;  /* was field_0xFE: _execute 0224B268 (daNpc_Ji1_setHairAngle): the upper limit of the three hair joints' Z angles together; daNpc_Ji1_HIO_ct 0225CC44 0x3A00 */
    /* 0x0FC */ be<u32> __vtbl;
};
WWHD_OFFSET(daNpc_Ji1_HIO_c, mLevelHits, 0x5C);
WWHD_OFFSET(daNpc_Ji1_HIO_c, mBackboneOffsets, 0xC0);
WWHD_SIZE(daNpc_Ji1_HIO_c, 0x100);

/* ---- free functions of the translation unit ---- */
s16 daNpc_Ji1_XyCheckCB(void*, int);                                                 /* 02248C64 */
void daJi1_CoHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248CA4 */
void daJi1_TgHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248CF0 */
void daJi1_AtHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248EA0 */
BOOL daNpc_Ji1_plRoomOutCheck();                                                     /* 02248F58 */
u32 playerCutAtCheck();                                                              /* 02249058 */
BOOL nodeCallBack1(J3DNode*, int);                                                   /* 02249084 */
BOOL nodeCallBack2(J3DNode*, int);                                                   /* 02249224 */
BOOL nodeCallBack3(J3DNode*, int);                                                   /* 022494C8 */

/* ---- this TU's constants and statics ---- */
#define SAFESTRING_VTBL 0x1001AE20 /* this TU's sead::SafeString vtable */
static inline daNpc_Ji1_HIO_c& l_HIO() { return *gabi::at<daNpc_Ji1_HIO_c>(0x104672AC); }
static inline be<u8>& game_life_point() { return *gabi::at<be<u8>>(0x101D5F44); } /* daNpc_Ji1_c::game_life_point */

/* ---- pointers to member functions (GHS, 8 bytes) ---- */
/* `if (p)`: the virtual index is 0 only for a null PTMF */
static inline bool ptmf_nonnull(ProcFunc_l& p) { return p.i != 0; }
/* `p == &daNpc_Ji1_c::fn` for a non-virtual fn */
static inline bool ptmf_eq(ProcFunc_l& p, u32 fn) { return p.i == -1 && p.d == 0 && p.f == fn; }
static inline void ptmf_set(ProcFunc_l& p, u32 fn) {
    p.d = 0;
    p.i = -1;
    p.f = fn;
}
static inline void ptmf_copy(ProcFunc_l& dst, ProcFunc_l& src) {
    dst.d = (s16)src.d;
    dst.i = (s16)src.i;
    dst.f = (u32)src.f;
}
/* (self->*p)(args...) */
template <class... A> static inline BOOL ptmf_invoke(ProcFunc_l& p, void* self, A... a) {
    s16 d = p.d;
    s16 i = p.i;
    void* obj = gabi::at<void>(gabi::ea(self) + d);
    if (i < 0) return gabi::call_ptr<BOOL>(p.f, obj, a...);
    u32 vt = gabi::load<u32>(gabi::ea(obj) + gabi::load<s16>(gabi::ea(&p) + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), obj, a...);
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_getPlayerActorClass(): the play object's player 0 */
static inline fopAc_ac_c* daPy_getPlayerActorClass() { return dComIfGp_getPlayer(0); }
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 (re-read at each use) */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
// HD: the other split sources still use these legacy accessors.
#ifndef DA_NPC_JI1_USE_SHARED_GAME_API
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
#endif
/* fopAcM_seStart on this actor: GHS drops the inline's NULL checks (this and &eyePos non-NULL) */
static inline void ji1_seStart(fopAc_ac_c* a, u32 id, u32 param = 0) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* attention_info.flags (+0x39C), distances[] (+0x388) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
static inline be<u8>& attn_distance(fopAc_ac_c* a, int i) { return *gabi::at<be<u8>>(gabi::ea(a) + 0x388 + i); }
/* eventInfo (+0xF8): mCommand u16 +0xF8, mCondition u16 +0xFA, XyCheckCB +0x104 */
static inline u16 eventInfo_command(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline bool eventInfo_checkCommandTalk(fopAc_ac_c* a) { return eventInfo_command(a) == 1; }
static inline void eventInfo_setXyCheckCB(fopAc_ac_c* a, u32 cb) { gabi::store<u32>(gabi::ea(a) + 0x104, cb); }

/* setAction (inline in the GameCube header) */
static inline void ji1_setAction(daNpc_Ji1_c* i_this, u32 action, void* arg) {
    if (!ptmf_eq(i_this->mAction, action)) {
        if (ptmf_nonnull(i_this->mAction)) {
            i_this->mActionPhase = -1;
            ptmf_invoke(i_this->mAction, i_this, arg);
        }
        ptmf_copy(i_this->mPrevAction, i_this->mAction);
        ptmf_set(i_this->mAction, action);
        i_this->mActionPhase = 0;
        gabi::call_ptr<BOOL>(i_this->mAction.f, i_this, arg);
    }
}
static inline void ji1_setSubAction(daNpc_Ji1_c* i_this, u32 sub) { ptmf_set(i_this->mSubAction, sub); }

/* member function addresses (for PTMFs) */
enum : u32 {
    ACT_normalAction = 0x02250A1C,
    ACT_kaitenExpAction = 0x02251690,
    ACT_kaitenspeakAction = 0x02251424,
    ACT_kaitenwaitAction = 0x02250D54,
    ACT_kaitenAction = 0x02251ED0,
    ACT_talkAction = 0x0225268C,
    ACT_speakAction = 0x02252FD0,
    ACT_speakBadAction = 0x02254A64,
    ACT_eventAction = 0x022596CC,
    ACT_startspeakAction = 0x02253880,
    ACT_endspeakAction = 0x02253EE8,
    ACT_reiAction = 0x02254434,
    ACT_plmoveAction = 0x02257C9C,
    ACT_teachAction = 0x02255908,
    ACT_teachSPRollCutAction = 0x02256F1C,
    ACT_battleAction = 0x0225AEC8,
    SUB_teachSubActionAttack = 0x02255654,
    SUB_teachSubActionJump = 0x022554D0,
    SUB_battleSubActionWait = 0x0225B344,
    SUB_battleSubActionNockBack = 0x0225C208,
    SUB_battleSubActionAttack = 0x0225BF44,
    SUB_battleSubActionTateAttack = 0x0225B85C,
    SUB_battleSubActionYokoAttack = 0x0225BBA4,
    SUB_battleSubActionJump = 0x0225C358,
    SUB_battleSubActionDamage = 0x0225CA04,
    SUB_battleSubActionJpGuard = 0x0225C660,
    SUB_battleSubActionGuard = 0x0225C7EC,
};

/* HD: core-unit player accessors use a typed view; the full player header is
 * intentionally independent of this split NPC's common header. */
struct Ji1PlayerHD {
    u8 mUnaccessed[0x3AC];
    be<u8> mCutType;
    u8 mUnaccessed3AD[0xB];
    be<u32> mNoResetFlg0;
    enum : u32 { daPyFlg0_CUT_AT_FLG = 0x40 };
    u8 getCutType() const { return mCutType; }
    u32 getCutAtFlg() const { return mNoResetFlg0 & daPyFlg0_CUT_AT_FLG; }
};
WWHD_OFFSET(Ji1PlayerHD, mCutType, 0x3AC);
WWHD_OFFSET(Ji1PlayerHD, mNoResetFlg0, 0x3B8);
WWHD_SIZE(Ji1PlayerHD, 0x3BC);
WWHD_OFFSET(daNpc_Ji1_c, mHitCooldownHD, 0xE64);
enum Ji1AnimationIndexHD : s32 {
    Ji1_ANM_WAIT01=0, Ji1_ANM_INASI=7, Ji1_ANM_TATEGUARD=8,
    Ji1_ANM_YKGUARD=9, Ji1_ANM_JPGUARD=10, Ji1_ANM_TOKAMAE=11,
    Ji1_ANM_REI=12, Ji1_ANM_WARAI=13, Ji1_ANM_GUARD=15,
    Ji1_ANM_TATEATTACK=19, Ji1_ANM_YOKOATTACK=20,
    Ji1_ANM_NUGUI=24, Ji1_ANM_DAMAGE=25
};
namespace ji1Event {
 enum : u16 { UNK_0F20=0x0F20, UNK_D003=0xD003, UNK_0520=0x0520 };
}
