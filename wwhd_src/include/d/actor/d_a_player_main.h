/* daPy_lk_c (Link), WWHD layout. 
 *
 * Measured for the WWHD build (cking.rpx) from the constructor daPy_lk_c::daPy_lk_c 0240FE40 (it
 * allocates 0x8284 bytes; GameCube 0x4C28), from setAtnList 023DCD30 / resetSeAnime 023E07C0 /
 * small accessors, and from a statistical match of the GameCube members used by each function
 * against the this-relative offsets of the WWHD function of the same name (scratch notes in the
 * phase-1 report). GameCube -> WWHD:
 * - daPy_py_c base: 0x43C (d/actor/d_a_player.h; the GameCube daPy_py_c fields are +0x11C).
 * - 0x320..0x35C (+0x11C); HD replaced the GameCube ResTIMG mOtherLinktex by a 0x34 header list
 *   (mLinktexList) and ten 0x24 headers (mOtherLinktex) at 0x48C; 0x35C..0x3AC (+0x298); three HD
 *   0x58 texture animation controllers (mTexAnmCtrl) at 0x644..0x74C.
 * - 0x3AC..0x974 (+0x3A0): background checks, poly info, hand/sword models, mSwgripmsabBckAnim.
 * - 0x974..0x2FAC: re-laid out in HD (J3DAnm pointers became mDoExt_brk/btkAnm objects, the
 *   mirror packet grew); only constructor anchors are recorded (see mD90).
 * - 0x2FAC..0x3178: animation blending and the sight packet (HD 0xC08, GameCube 0x50).
 * - 0x3178..0x34B8 (+0x3410/+0x3414/+0x3418): HD adds a fourth item-button attention actor
 *   (0x65C8) and list entry (0x68A8) and a float at 0x68D0; mCurProcFunc is an 8-byte GHS pointer
 *   to member.
 * - 0x34B8..0x34C6 (+0x341C); 0x34C7..0x34CF (+0x3445); mProcVar0..5 (+0x3446); 0x34DC.. (+0x3448);
 *   mSeAnmIdx became a pointer (0x6938); 0x34F0..0x3598 (+0x3450); 0x3598..0x3688 (+0x3458);
 *   0x3688..0x37E4 (+0x3C08, after the per-joint saved state at 0x6AB0, 0x7E0 bytes); daPy_swBlur_c is allocated
 *   (pointer at 0x73EC); 0x3DB8..0x4C28 (+0x3638): foot data and colliders; HD tail 0x8260..0x8284.
 * "[?]" marks offsets inferred by order or from a single function; the range verifications refine
 * them. Field names are the GameCube ones. */
#pragma once
#include "d/actor/d_a_player.h"
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */

#define LK_VTBL 0x10037CF0 /* daPy_lk_c vtable (stored at +0xB4 by the constructor) */

/* daPy_actorKeep_c (8): process id + actor */
struct daPy_actorKeep_l {
    /* 0x0 */ be<u32> mID;
    /* 0x4 */ gptr<fopAc_ac_c> mActor;
};
WWHD_SIZE(daPy_actorKeep_l, 8);

struct daPy_lk_c : daPy_py_c {
    /* daPy_py_c fields not in d_a_player.h (GameCube +0x11C; SHARED-CANDIDATE for d_a_player.h) */
    u32 noResetFlg1() { return gabi::load<u32>(gabi::ea(this) + 0x3BC); }
    void setNoResetFlg1(u32 v) { gabi::store<u32>(gabi::ea(this) + 0x3BC, v); }
    u32 resetFlg0() { return gabi::load<u32>(gabi::ea(this) + 0x3C0); }
    void setResetFlg0(u32 v) { gabi::store<u32>(gabi::ea(this) + 0x3C0, v); }

    /* member functions (verified in phase 1; the ranges add theirs) */
    BOOL itemButton();
    BOOL checkSingleItemEquipAnime();
    BOOL checkItemEquipAnime();
    BOOL checkEquipAnime();
    BOOL checkBottleItem(int);
    BOOL checkPhotoBoxItem(int);
    BOOL checkBowItem(int);
    u32 seStartOnlyReverb(u32);
    u32 seStartMapInfo(u32);
    u32 seStartSwordCut(u32);
    void cancelNoDamageMode();
    BOOL checkMabaAnimeBtp(int);
    J3DModel* setItemHeap();
    void setHandModel(int);
    void resetSeAnime();
    int getDirectionFromAngle(s16);
    int getDirectionFromCurrentAngle();
    int getDirectionFromShapeAngle();
    void setSubjectMode();
    void resetCurse();
    BOOL checkSuccessGuard(int);
    u32 setParamData(int, int, int, int);
    BOOL procPolyDamage();
    BOOL checkNextMode(int);
    void endDamageEmitter();
    s16 checkAutoJumpFlying();
    void onDekuSpReturnFlg(u8);
    BOOL checkPlayerGuard();
    /* range #06 (02423D10..0242DAC7, d_a_player_main_06.cpp) */
    BOOL dProcPresent();
    BOOL dProcWindChange_init();
    BOOL dProcWindChange();
    BOOL dProcStandItemPut_init();
    BOOL dProcStandItemPut();
    BOOL dProcVorcanoFail_init();
    BOOL dProcVorcanoFail();
    BOOL dProcSlightSurprised_init();
    BOOL dProcSlightSurprised();
    BOOL dProcSmile_init();
    BOOL dProcSmile();
    BOOL dProcBossWarp_init();
    BOOL dProcBossWarp();
    BOOL dProcAgbUse_init();
    BOOL dProcAgbUse();
    BOOL dProcLookTurn_init();
    BOOL dProcLookTurn();
    BOOL dProcLetterOpen_init();
    BOOL dProcLetterOpen();
    BOOL dProcLetterRead_init();
    BOOL dProcLetterRead();
    BOOL dProcRedeadStop_init();
    BOOL dProcRedeadStop();
    BOOL dProcRedeadCatch_init();
    BOOL dProcRedeadCatch();
    BOOL dProcGetDance_init();
    BOOL dProcGetDance();
    BOOL dProcBottleOpenFairy_init();
    BOOL dProcBottleOpenFairy();
    BOOL dProcWarpShort_init();
    BOOL dProcWarpShort();
    BOOL dProcOpenSalvageTreasure_init();
    BOOL dProcOpenSalvageTreasure();
    BOOL dProcSurprisedWait_init();
    BOOL dProcSurprisedWait();
    BOOL dProcPowerUpWait_init();
    BOOL dProcPowerUp_init();
    BOOL dProcPowerUp();
    BOOL dProcShipSit_init();
    BOOL dProcShipSit();
    BOOL dProcLastCombo_init();
    BOOL dProcLastCombo();
    BOOL dProcHandUp_init();
    BOOL dProcHandUp();
    BOOL dProcIceSlip_init();
    BOOL dProcIceSlip();
    BOOL dProcHDLetterClose_init();
    BOOL dProcHDLetterWrite();
    BOOL dProcHDLetterEnd();
    BOOL dProcHDBottleThrow();
    void setBodyAngleXReadyAnime();
    BOOL checkItemModeActorPointer();
    void setLadderFootSe();
    BOOL procLadderDownEnd_init(int);
    int changeLadderMoveProc(int);
    int setMoveBGLadderCorrect();
    BOOL procLadderUpStart();
    BOOL procLadderUpEnd();
    BOOL procLadderDownStart();
    BOOL procLadderDownEnd();
    BOOL procLadderMove();
    f32 getHangMoveAnmSpeed();
    int getHangDirectionFromAngle();
    BOOL changeHangMoveProc(int);
    int changeHangEndProc(int);
    void setHangShapeOffset();
    BOOL procHangClimb_init(f32);
    BOOL procHangWait_init();
    BOOL procHangStart();
    BOOL procHangUp_init(int);
    BOOL procHangFallStart();
    BOOL procHangMove_init(int);
    BOOL procHangUp();
    BOOL procHangWait();
    BOOL procHangMove();
    BOOL procHangClimb();
    BOOL procHangWallCatch();
    f32 getClimbMoveAnmSpeed();
    int getClimbDirectionFromAngle();
    BOOL procClimbMoveSide_init(int);
    void changeClimbMoveProc(int);
    BOOL setMoveBGCorrectClimb();
    void checkBgCorrectClimbMove(cXyz*, cXyz*);
    void setClimbShapeOffset();
    void checkBgClimbMove();
    BOOL procClimbUpStart();
    BOOL procClimbDownStart();
    BOOL procClimbMoveUpDown();
    BOOL procClimbMoveSide();
    void setBlendWHideMoveAnime(f32);
    void getWHideBasePos(cXyz*);
    void getWHideNextPos(cXyz*, cXyz*);
    BOOL checkWHideBackWall(cXyz*);
    BOOL checkWHideFrontFloor(cXyz*);
    int checkWHideModeChange(cXyz*);
    int changeWHideEndProc(cXyz*);
    BOOL procWHideWait_init();
    BOOL procWHideReady();
    BOOL procWHideMove_init();
    BOOL procWHidePeep_init();
    BOOL procWHideWait();
    BOOL procWHideMove();
    BOOL procWHidePeep();
    f32 getCrawlMoveSpeed();
    void setCrawlMoveDirectionArrow();
    BOOL checkCrawlSideWall(cXyz*, cXyz*, cXyz*, cXyz*, s16*, s16*);
    BOOL procCrawlAutoMove_init(int, cXyz*);
    BOOL changeCrawlAutoMoveProc(cXyz*);
    void crawlBgCheck(cXyz*, cXyz*);
    void setDoStatusCrawl();
    BOOL checkNotCrawlStand(cXyz*);
    BOOL checkNotCrawlStand(cXyz*, cXyz*);
    BOOL procCrawlEnd_init(int, s16, s16);
    BOOL procCrawlStart();
    BOOL procCrawlMove();
    BOOL procCrawlAutoMove();
    BOOL procCrawlEnd();
    void setWeaponBlur();
    BOOL procGrabUp_init();
    BOOL procGrabMiss_init();
    /* ---- range #04 ---- (02400B14..02419FEB, d_a_player_main_04.cpp / _04b.cpp) */
    BOOL checkAttentionPosAngle(fopAc_ac_c*, cXyz**);
    fopAc_ac_c* getDemoLookActor();
    void checkOriginalHatAnimation();
    void setCutWaterSplash();
    f32 getBlurTopRate();
    int getSwordBlurColor();
    BOOL fanWindCrashEffectDraw();
    BOOL execute();
    BOOL playerDelete();
    void playerInit();
    BOOL makeBgWait();
    f32 getLadderMoveAnmSpeed();
    f32 getCrawlMoveAnmSpeed();
    void setSpeedAndAngleAtnBack();
    void setAngleAtnHD();
    void setSpeedAndAngleAtnNoShapeHD();
    void setBlendSubjectMoveAnimeHD();
    BOOL procControllWait();
    void setSpeedAndAngleAtn();
    BOOL procAtnMove();
    void setShapeAngleToAtnActor();
    void setSpeedAndAngleAtnActor();
    BOOL procAtnActorWait();
    BOOL procAtnActorMove();
    void checkNextActionBowFly();
    void checkNextActionBoomerangFly();
    BOOL procShipRestart_init();
    void setSwimMoveAnime(int);
    void setSwimTail();
    BOOL procSwimMove_init(int);
    BOOL procCrawlMove_init(s16, s16);
    BOOL procSmallJump_init(int);
    BOOL checkSubjectEnd(BOOL);
    BOOL procCrawlStart_init();
    int getCrawlMoveVec(cXyz*, cXyz*, cXyz*);
    void setNormalSpeedF(f32, f32, f32, f32);
    s16 checkBodyAngleX(s16);
    BOOL setBodyAngleToCamera();
    BOOL procSubjectivity();
    BOOL procCall();
    BOOL procIceSlipAlmostFall_init();
    BOOL procFreeWait_init();
    BOOL procFreeWait();
    BOOL procWait();
    BOOL procLadderUpEnd_init(int);
    void procClimbUpStart_init_sub();
    void procLadderUpStart_init_sub();
    void procLadderDownStart_init_sub();
    BOOL procVerticalJump_init();
    BOOL procClimbMoveUpDown_init(int);
    BOOL procClimbUpStart_init();
    BOOL procLadderMove_init(int, int, cXyz*);
    BOOL procLadderUpStart_init();
    BOOL procLadderDownStart_init();
    BOOL procHangWallCatch_init();
    BOOL procHangStart_init();
    BOOL procIceSlipFall_init();
    BOOL checkIceSlipFall();
    BOOL procMove();
    void setSpeedAndAngleNormal(s16);
    BOOL changeFrontWallTypeProc();
    BOOL procTactPlayEnd_init(int);
    void createAnimeHeap(JKRSolidHeap**, int);
    void entryMirrorObj9CHD(void*, u32);
    void entryMirrorObjA0HD(void*, u32);
    BOOL checkSelfieEndHD();
    BOOL procSelfieHD();
    void setFootMark(cXyz*);
    void setSwordAtCollision();
    void checkRoofRestart();
    void setSwimWaterDrop(void*);
    void setWaterDrop();
    void setHammerWaterSplash();
    void checkLightHit();
    void setAttentionPos();
    void setBootsModel(J3DModel**);
    BOOL jointCB1();
    void setFootEffectType(int, cXyz*, int, int);
    void setFootEffect();
    void setGrabItemPos();
    void setLightSaver();
    void setAuraEffect();
    void setWaterRipple();
    void setHatAngle();
    void setNeckAngle();
    s32 setItemModel();
    void setCollision();
    /* ---- end of range #04 ---- */
    /* ---- range #02 ---- (023E0E50..023EDF6F, d_a_player_main_02.cpp) */
    BOOL procScope_init(int);
    BOOL procBoomerangCatch_init();
    int checkShipRideUseItem(int);
    void setOldRootQuaternion(s16, s16, s16);
    void setShipRidePosUseItem();
    void initShipRideUseItem(int, int);
    BOOL procControllWait_init();
    BOOL changeWaitProc();
    u32 getSlidePolygon();
    BOOL procSlideFront_init(s16);
    BOOL procSlideBack_init(s16);
    int changeSlideProc();
    void endFlameDamageEmitter();
    BOOL procGrabWait_init();
    void setShipRidePos(int);
    BOOL procWait_init();
    BOOL procBtJump_init(fopEn_enemy_c*);
    BOOL procBtRoll_init(fopEn_enemy_c*);
    BOOL procBtVerticalJump_init(fopEn_enemy_c*);
    BOOL changeSpecialBattle();
    int getCutDirection();
    void setBlurPosResource(u16);
    void setAtParam(u32, int, int, u8, u8, u8, f32);
    void setFinishCutAtParam(u8);
    void setNormalCutAtParam(u8);
    BOOL procCutEA_init();
    BOOL procCutEB_init();
    BOOL procCutF_init(s16);
    BOOL procCutR_init(s16);
    BOOL procCutA_init(s16);
    BOOL procCutL_init(s16);
    BOOL procCutTurn_init(BOOL);
    void setExtraFinishCutAtParam(u8);
    BOOL procCutExB_init();
    void setExtraCutAtParam(u8);
    BOOL procCutExMJ_init(int);
    BOOL procCutKesa_init();
    BOOL procCutExA_init();
    int changeCutProc();
    void setFanModel();
    int setShapeFanLeaf();
    BOOL procFanSwing_init();
    void setPriTextureAnime(u16, int);
    void setBowReadyAnime();
    BOOL procBowSubject_init();
    s16 getGroundAngle(u32, s16);
    void setBlendAtnBackMoveAnime(f32);
    void setBlendAtnMoveAnime(f32);
    BOOL procBowMove_init();
    BOOL checkNextBowMode();
    BOOL procHookshotSubject_init();
    BOOL procHookshotMove_init();
    BOOL checkNextHookshotMode();
    BOOL procBoomerangSubject_init();
    BOOL procBoomerangMove_init();
    BOOL checkNextBoomerangMode();
    BOOL procRopeSubject_init();
    BOOL procRopeMove_init();
    BOOL checkNextRopeMode();
    BOOL procTactWait_init(int);
    int setHintActor();
    BOOL checkDrinkBottleItem(int);
    BOOL checkOpenBottleItem(int);
    BOOL checkGroupItem(int, int);
    BOOL checkSetItemTrigger(int, int);
    BOOL changeDragonShield(int);
    BOOL procBootsEquip_init(u16);
    BOOL procGrabThrow_init(int);
    void setAnimeEquipSword(BOOL);
    BOOL procGrabPut_init();
    BOOL checkNextActionGrab();
    int getReadyItem();
    BOOL itemTrigger();
    BOOL procHookshotFly_init();
    BOOL cancelItemUpperReadyAnime();
    BOOL bowButton();
    void setBowReloadAnime();
    void makeArrow();
    BOOL checkNextActionHookshotReady();
    BOOL checkNextActionBowReady();
    BOOL checkNextActionBoomerangReady();
    BOOL checkBossGomaStage();
    BOOL procRopeReady_init();
    BOOL procRopeThrowCatch_init();
    BOOL checkSightLine(f32, cXyz*);
    int throwRope();
    BOOL checkNextActionRopeReady();
    BOOL setTalkStatus();
    void setSpecialBattle(BOOL);
    void setDoStatusBasic();
    void setDoStatus();
    void setAnimeUnequipSword();
    void setAnimeEquipSingleItem(u16);
    void setAnimeUnequipItem(u16);
    void setAnimeUnequip();
    BOOL procPushPullWait_init(int);
    BOOL procGrabReady_init();
    void setAtCpsSpl(u8); /* HD-only (023ED2B0, unnamed by the matcher) */
    void setEnemyWeaponAtParam(BOOL);
    void setJumpCutAtParam();
    BOOL procJumpCut_init(int);
    BOOL procWHideReady_init(u32, cXyz*);
    BOOL procCutTurnCharge_init();
    BOOL procWeaponNormalSwing_init();
    BOOL procWeaponSideSwing_init();
    BOOL procWeaponFrontSwingReady_init();
    BOOL procWeaponThrow_init();
    BOOL procBottleSwing_init(int);
    BOOL procVomitJump_init(int);
    BOOL procShipReady_init();
    int orderTalk();
    int setMoveAnime(f32, f32, f32, int, int, int, f32);
    void setBlendMoveAnime(f32);
    u32 getWHideModePolygon(cXyz*, cXyz*, cXyz*, int);
    void setFrontWallType();
    /* ---- end of range #02 ---- */
    /* ---- range #05 ---- */
    /* range #05 (02419FEC..02423D0F, d_a_player_main_05.cpp) */
    void checkNextActionItemFly();
    BOOL procSideStepLand_init();
    BOOL checkJumpCutFromButton();
    void setParachuteFanModel(f32);
    BOOL procFanGlide_init(int);
    BOOL checkFanGlideProc(int);
    BOOL procSideStep();
    BOOL procSideStepLand();
    BOOL procCrouchDefense();
    BOOL procNockBackEnd_init();
    BOOL procCrouchDefenseSlip();
    BOOL procCrouch();
    BOOL procWaitTurn();
    BOOL procMoveTurn();
    BOOL procSlip();
    BOOL procSlideFrontLand_init();
    BOOL procSlideFront();
    BOOL procSlideBackLand_init();
    BOOL procSlideBack();
    BOOL procSlideFrontLand();
    BOOL procSlideBackLand();
    BOOL procFrontRollCrash_init();
    BOOL procFrontRoll();
    BOOL procFrontRollCrash();
    BOOL procNockBackEnd();
    BOOL procSideRoll_init();
    BOOL procSideRoll();
    BOOL procBackJumpLand_init();
    BOOL procBackJump();
    BOOL procBackJumpLand();
    BOOL procShipJumpRide_init();
    BOOL checkJumpRideShip();
    BOOL procLandDamage_init(int);
    BOOL procVomitLand_init();
    BOOL procLand_init(f32, int);
    BOOL changeLandProc(f32);
    BOOL checkSpecialRope();
    void setBlendRopeMoveAnime(int);
    BOOL procRopeSwing_init(fopAc_ac_c*, s16);
    int changeRopeSwingProc();
    void setFallVoice();
    BOOL procAutoJump();
    BOOL procLand();
    BOOL procLandDamage();
    BOOL procFall();
    BOOL procSlowFall();
    BOOL procSmallJump();
    BOOL procVerticalJump();
    BOOL procGuardCrash();
    BOOL procDamage();
    BOOL procLargeDamageWall_init(int, int, s16, s16);
    BOOL procLargeDamage();
    BOOL procLargeDamageUp();
    BOOL procLargeDamageWall();
    BOOL procLavaDamage();
    BOOL procElecDamage();
    BOOL procGuardSlip();
    BOOL procIceSlipFallUp_init(int, s16, s16);
    BOOL procIceSlipFall();
    BOOL procIceSlipFallUp();
    BOOL procIceSlipAlmostFall();
    BOOL procGrabHeavyWait_init();
    void initGrabNextMode();
    BOOL procBootsEquip();
    BOOL procNotUse();
    void setTalismanModel();
    void setLetterModel();
    void setShapeAngleToTalkActor();
    BOOL checkEndMessage(u32);
    void setDemoTextureAnime(u16, u16, int, u16);
    BOOL dProcTool();
    BOOL dProcTalk();
    BOOL dProcDamage_init();
    BOOL dProcDamage();
    BOOL dProcHoldup_init();
    BOOL dProcHoldup();
    BOOL dProcOpenTreasure_init();
    BOOL dProcOpenTreasure();
    BOOL dProcGetItem_init();
    BOOL dProcGetItem();
    BOOL dProcUnequip_init();
    BOOL dProcUnequip();
    BOOL dProcLavaDamage();
    BOOL dProcFreezeDamage();
    BOOL changeSwimUpProc();
    BOOL dProcDead();
    BOOL dProcLookAround_init();
    BOOL dProcLookAround();
    BOOL dProcSalute_init();
    BOOL dProcSalute();
    BOOL dProcLookAround2_init();
    BOOL dProcLookAround2();
    BOOL dProcTalismanPickup_init();
    BOOL dProcTalismanPickup();
    BOOL dProcTalismanWait_init();
    BOOL dProcTalismanWait();
    BOOL dProcSurprised_init();
    BOOL dProcSurprised();
    BOOL dProcTurnBack_init();
    BOOL dProcTurnBack();
    BOOL dProcLookUp_init();
    BOOL dProcLookUp();
    BOOL dProcQuakeWait_init();
    BOOL dProcQuakeWait();
    BOOL dProcDance_init();
    BOOL dProcDance();
    BOOL dProcCaught_init();
    BOOL dProcCaught();
    BOOL dProcLookWait();
    BOOL dProcPushPullWait_init();
    BOOL dProcPushPullWait();
    BOOL dProcPushMove_init();
    BOOL dProcPushMove();
    BOOL dProcDoorOpen_init();
    BOOL dProcDoorOpen();
    BOOL dProcNod_init();
    BOOL dProcNod();
    void dProcPresent_init_sub();
    BOOL dProcPresent_init();
    /* ---- range #08 ---- */
    /* (0243F058..02444F20, d_a_player_main_08.cpp) */
    BOOL procWeaponFrontSwingReady();
    BOOL procWeaponFrontSwingEnd_init();
    BOOL procWeaponFrontSwing();
    BOOL procWeaponFrontSwingEnd();
    BOOL procWeaponThrow();
    BOOL procFoodThrow();
    BOOL procFoodSet();
    BOOL procCutA();
    BOOL procCutF();
    BOOL procCutR();
    BOOL procCutL();
    BOOL procCutEA();
    BOOL procCutEB();
    BOOL procCutExA();
    BOOL procCutExB();
    BOOL procCutExMJ();
    BOOL procCutKesa();
    BOOL procCutTurn();
    BOOL procCutRoll_init();
    BOOL procCutRollEnd_init();
    BOOL procCutRoll();
    BOOL procCutRollEnd();
    BOOL procCutTurnMove_init();
    BOOL procCutTurnCharge();
    BOOL procCutTurnMove();
    BOOL procCutReverse();
    BOOL procJumpCutLand_init();
    BOOL procJumpCut();
    BOOL procJumpCutLand();
    BOOL returnBoomerang();
    BOOL shipSpecialDemoStart();
    BOOL checkEndTactMusic();
    f32 getTactMetronomeRate();
    BOOL checkTactLastInput();
    BOOL getTactTopPos(cXyz*);
    BOOL getTactNormalWait();
    BOOL checkTactPlayMelody();
    static BOOL setItemWaterEffect(fopAc_ac_c*, BOOL, BOOL);
    BOOL checkHookshotReturn();
    BOOL checkCutRollChange();
    BOOL checkGameOverStart();
    int getTactTimerCancel();
    s32 getTactMusic();
    fopAc_ac_c* getGrabMissActor();
    void setTactZev(u32, int, u32);
    BOOL getBokoFlamePos(cXyz*);
    void voiceStart(u32);
    void setOutPower(f32, s16, int);
    BOOL setHookshotCarryOffset(u32, const cXyz*);
    void setPlayerPosAndAngle(cXyz*, s16);
    void setPlayerPosAndAngle(cXyz*, csXyz*);
    void setPlayerPosAndAngle(Mtx34*);
    BOOL setThrowDamage(cXyz*, s16, f32, f32, int);
    void changeTextureAnime(u16, u16, int);
    /* per-TU inline accessor copies after __sinit (names by content, "_l") */
    f32 getGroundH_l();
    Mtx34* getAnmMtx8_l();
    Mtx34* getAnmMtx12_l();
    u32 checkModeFlg_10452822_l();
    BOOL check_02444DBC_l();
    BOOL checkProc1E_l();
    BOOL checkProcA5_l();
    BOOL checkCutTurnMoveProc_l();
    f32 getBaseAnimeRate_l();
    f32 getBaseAnimeFrame_l();
    u32 getEquipActorID_l();
    u32 getThrowActorID_l();
    u32 getGrabActorID_l();
    BOOL checkGrabBarrel_l();
    BOOL checkPlayerNoDraw();
    BOOL checkNoEquipActor_l();
    BOOL checkUpperAnimeE4_l();
    Mtx34* getModelJointMtx_l(s32);
    void setFlg8_02444F0C_l(u32);
    BOOL checkComboCutTurn();
    /* ---- end of range #08 ---- */
    /* ---- range #01 ---- (023D4BB8..023E0E4F, d_a_player_main_01.cpp) */
    BOOL checkBowReadyAnime();
    BOOL checkShipNotNormalMode();
    BOOL checkBoomerangAnime();
    BOOL checkBowAnime();
    BOOL checkRopeAnime();
    BOOL checkUpperReadyThrowAnime();
    BOOL checkUpperReadyAnime();
    BOOL checkCaughtShapeHide();
    BOOL checkDemoShieldNoDraw();
    void entryHDPacket(u32, u32);
    BOOL checkChanceMode();
    BOOL checkHDAnmNotEnd();
    BOOL checkGrabSpecialHeavyState();
    void deleteArrow();
    u32 setAnimeHeap(u32);
    u32 getAnimeResource(u32, u16, u32);
    void setFrameCtrl(J3DFrameCtrl*, u8, s16, s16, f32, f32);
    void setScopeModel();
    void setTinkleCeiverModel();
    void freeHookshotItem();
    void swimOutAfter(BOOL);
    u32 initModel(u32, int, u32);
    void offBodyEffect();
    void updateDLSetLight(J3DModel*, u32);
    void entryDLSetLight(J3DModel*, u32);
    BOOL checkMaskDraw();
    BOOL checkDemoSwordNoDraw(BOOL);
    void onBodyEffect();
    BOOL checkHeavyStateOn();
    u32 loadTextureAnimeResource(u32, BOOL);
    void setTextureScrollResource(u32, int);
    u32 loadTextureScrollResource(u32, BOOL);
    void resetPriTextureAnime();
    void freeRopeItem();
    void setActorPointer();
    BOOL checkGrabBarrelSearch(int);
    BOOL checkRestHPAnime();
    u32 getItemAnimeResource(u32);
    void returnKeepItemData();
    void resetFootEffect();
    u32 getAnmData(int);
    u32 initBrkAnm(u32, u32, u32);
    u32 initBtkAnm(u32, u32, u32);
    BOOL HDJointCB(int);
    void drawMirrorLightModel();
    void setGetItemSound(u32, BOOL);
    BOOL resetActAnimeUpper(int, f32);
    void setDamageCurseEmitter();
    void freeGrabItem();
    void animeUpdate();
    void setSmallFanModel();
    void setHookshotModel();
    void setPhotoBoxModel();
    void setTactModel();
    void setHammerModel();
    void getUnderUpperAnime(u32, u32, u32, int, u32);
    BOOL checkBossBgm();
    void resetDemoTextureAnime();
    void initSeAnime();
    BOOL bowJointCB(int);
    BOOL fanJointCB(int);
    BOOL parachuteJointCB(int);
    void setAtnList();
    void setTextureAnimeResource(u32, int);
    void deleteEquipItem(BOOL);
    BOOL setActAnimeUpper(u32, int, f32, f32, int, f32);
    void setSeAnime(u32, u32, u32);
    void setBgCheckParam();
    void hideHatAndBackle(u32);
    void makeItemType();
    BOOL setGetDemo();
    void drawShadow();
    void setBowModel();
    BOOL jointAfterCB(int, u32, u32);
    void setShipRideArmAngle(int, u32);
    void initTextureAnime();
    u16 checkNormalFace();
    BOOL setSingleMoveAnime(int, f32, f32, int, f32);
    void setBottleModel(u32);
    void setSwordModel(BOOL);
    void setTextureAnime(u32, int);
    void initTextureScroll();
    BOOL jointBeforeCB(int, u32, u32);
    BOOL commonProcInit(int);
    BOOL jointCB0(int);
    BOOL setDrawHandModel();
    BOOL createHeap();
    BOOL draw();
    /* ---- end of range #01 ---- */
    /* ---- range #07 ---- (0242DAC8..0243F057, d_a_player_main_07.cpp) */
    BOOL procGrabReady();
    BOOL procGrabRebound_init();
    BOOL procGrabUp();
    BOOL procGrabMiss();
    BOOL procGrabThrow();
    BOOL procGrabPut();
    BOOL procGrabWait();
    BOOL procGrabHeavyWait();
    BOOL procGrabRebound();
    void setSpeedAndAngleSwim();
    BOOL checkNextModeSwim();
    BOOL changeSwimOutProc();
    BOOL procSwimUp();
    BOOL procSwimWait();
    BOOL procSwimMove();
    BOOL procBtJumpCut_init(cXyz*);
    BOOL procBtJump();
    BOOL procBtJumpCut();
    BOOL procBtSlide();
    BOOL procBtRollCut_init(cXyz*);
    BOOL procBtRoll();
    BOOL procBtRollCut();
    BOOL procBtVerticalJumpCut_init();
    BOOL procBtVerticalJump();
    BOOL procBtVerticalJumpLand_init();
    BOOL procBtVerticalJumpCut();
    BOOL procBtVerticalJumpLand();
    void setShipAttentionAnmSpeed(f32);
    void setShipAttnetionBodyAngle();
    BOOL procShipGetOff_init();
    BOOL checkShipPutAwayTrigger(); /* 02431028 HD-only, unnamed (name ours) */
    BOOL procShipScope_init(int);
    BOOL procShipBow_init();
    BOOL procShipBoomerang_init();
    BOOL procShipHookshot_init();
    BOOL changeShipEndProc();
    BOOL procShipReady();
    BOOL procShipJumpRide();
    BOOL procShipSteer();
    BOOL procShipPaddle();
    BOOL procShipScope();
    BOOL procShipBoomerang();
    void setHookshotSight();
    BOOL procShipHookshot();
    BOOL procShipBow();
    BOOL procShipCannon();
    BOOL procShipCrane();
    BOOL procShipGetOff();
    BOOL procShipRestart();
    f32 checkRopeRoofHit(s16);
    int changeRopeEndProc(int);
    BOOL procRopeUpHang_init();
    int changeRopeToHangProc();
    BOOL checkRopeSwingWall(cXyz*, cXyz*, s16*, f32*);
    BOOL checkHangRopeActorNull();
    int specialRopeHangUp();
    BOOL procRopeSubject();
    BOOL procRopeReady();
    BOOL procRopeHangWait_init(int);
    BOOL procRopeUp_init();
    BOOL procRopeDown_init();
    BOOL procRopeSwingStart_init();
    BOOL procRopeHangWait();
    BOOL procRopeUp();
    BOOL procRopeDown();
    BOOL procRopeSwingStart();
    BOOL procRopeMove();
    BOOL procRopeThrowCatch();
    BOOL procRopeUpHang();
    BOOL procBoomerangSubject();
    BOOL procBoomerangMove();
    BOOL procBoomerangCatch();
    BOOL procBowSubject();
    BOOL procBowMove();
    BOOL procHookshotSubject();
    BOOL procHookshotMove();
    BOOL procHookshotFly();
    BOOL procCutReverse_init(int);
    int changeCutReverseProc(int);
    BOOL procFanSwing();
    BOOL procFanGlide();
    u16 getTactPlayRightArmAnm(s32);
    u16 getTactPlayLeftArmAnm(s32);
    BOOL checkNpcStatus(); /* 02439C9C, unnamed by the matcher */
    BOOL checkTactCancelTrigger(); /* 02439D24 HD-only, unnamed (name ours) */
    BOOL procTactPlay_init(s32, int, int);
    BOOL procTactWait();
    BOOL checkTactSongUsable(); /* 0243AAD4 HD-only, unnamed (name ours) */
    BOOL procTactPlay();
    u32 getDayNightParamData();
    BOOL procTactPlayEnd();
    BOOL procTactPlayOriginal_init();
    BOOL procTactPlayOriginal();
    BOOL procVomitReady();
    BOOL procVomitWait();
    BOOL procVomitJump();
    BOOL procVomitLand();
    void setHammerQuake(cBgS_PolyInfo*, const cXyz*, int);
    BOOL procHammerSideSwing();
    BOOL procHammerFrontSwing_init();
    BOOL procHammerFrontSwingReady();
    BOOL procHammerFrontSwingEnd_init();
    BOOL procHammerFrontSwing();
    BOOL procHammerFrontSwingEnd();
    BOOL setPushPullKeepData(int);
    BOOL procPushMove_init();
    BOOL procPullMove_init();
    BOOL procPushPullWait();
    BOOL procPushMove();
    BOOL procPullMove();
    BOOL procBottleDrink();
    BOOL procBottleOpen();
    BOOL procBottleGet_init();
    BOOL procBottleSwing();
    BOOL procBottleGet();
    BOOL procWeaponNormalSwing();
    BOOL procWeaponSideSwing();
    BOOL procWeaponFrontSwing_init();
    BOOL checkRopeSwingTurn(cXyz*, f32, f32); /* 0243460C HD-only */
    BOOL procRopeSwing();
    /* ---- end of range #07 ---- */
    s32 demoParam0() { return gabi::load<s32>(gabi::ea(this) + 0x428); } /* mDemo.getParam0() (GameCube 0x30C) [?] */
    /* ---- range #03 ---- (023EDF70..02400B13, d_a_player_main_03.cpp) */
    BOOL procHammerSideSwing_init();
    BOOL procHammerFrontSwingReady_init();
    BOOL procCall_init();
    BOOL procCrouch_init();
    BOOL checkGuardAccept();
    BOOL procCrouchDefense_init();
    BOOL procSideStep_init(int);
    BOOL procBackJump_init();
    BOOL procFrontRoll_init(f32);
    BOOL procSubjectivity_init(int);
    void setHyoiModel();
    void keepItemData();
    BOOL procFoodSet_init();
    BOOL procFoodThrow_init();
    int changeBottleDrinkFace(int);
    BOOL procBottleDrink_init(u16);
    BOOL procBottleOpen_init(u16);
    BOOL procNotUse_init(int);
    void setAnimeEquipItem();
    BOOL checkNewItemChange(u8);
    BOOL checkItemChangeFromButton();
    BOOL checkNextActionFromButton();
    BOOL checkAtnWaitAnime();
    BOOL procAtnActorWait_init();
    BOOL procAtnActorMove_init();
    BOOL procAtnMove_init();
    BOOL procWaitTurn_init();
    BOOL procMoveTurn_init(int);
    BOOL procSlip_init();
    BOOL procMove_init();
    BOOL procVomitWait_init();
    BOOL procVomitReady_init(s16, f32);
    BOOL checkJumpFlower();
    void initShipBaseAnime();
    BOOL procShipCannon_init();
    void initShipCraneAnime();
    BOOL procShipCrane_init();
    BOOL procShipSteer_init();
    BOOL procShipPaddle_init();
    void endDemoMode();
    BOOL dProcHDBottleThrow_init();
    BOOL dProcHDLetterWrite_init();
    BOOL dProcTool_init();
    int setTalkStartBack();
    BOOL dProcTalk_init();
    void setDamageRupee(f32); /* HD-only */
    BOOL setDamagePoint(f32);
    cXyz* getDamageVec(dCcD_GObjInf*);
    BOOL procLargeDamage_init(int, int, s16, s16);
    void dProcFreezeDamage_init_sub(int);
    BOOL procLargeDamageUp_init(int, int, s16, s16);
    BOOL procFall_init(int, f32);
    BOOL procSlowFall_init();
    BOOL dProcLookWait_init();
    BOOL changeDemoProc();
    fopAc_ac_c* makeFairy(cXyz*, u32);
    void dProcDead_init_sub();
    void dProcDead_init_sub2();
    BOOL dProcDead_init();
    BOOL changeDeadProc();
    BOOL procAutoJump_init();
    BOOL procClimbDownStart_init(s16);
    BOOL procHangFallStart_init(void*); /* cM3dGPla* */
    BOOL changeAutoJumpProc();
    f32 getSwimTimerRate();
    void setSwimTimerStartStop();
    BOOL procSwimWait_init(int);
    BOOL procSwimUp_init(int);
    BOOL changeSwimProc();
    BOOL dProcFreezeDamage_init();
    s32 checkWallAtributeDamage(dBgS_AcchCir*);
    void setDamageElecEmitter();
    void setDamageFlameEmitter();
    void setDamageEmitter();
    BOOL procElecDamage_init(const cXyz*);
    BOOL checkElecReturnDamage(dCcD_GObjInf*, cXyz*);
    BOOL procDamage_init();
    void setDamagePointWait(f32); /* HD-only helper */
    BOOL checkNormalDamage(int);
    void setDashDamage();
    BOOL procCrouchDefenseSlip_init();
    BOOL procGuardSlip_init();
    BOOL procPolyDamage_init();
    BOOL changeDamageProc();
    BOOL changeBoomerangCatchProc();
    void throwBoomerang();
    void checkItemAction();
    void setShieldGuard();
    BOOL checkAtHitEnemy(dCcD_GObjInf*);
    void playTextureAnime();
    void setAnimeRatioHD(); /* HD-only */
    void setBeltConveyerPower();
    void setWindAtPower();
    void posMoveFromFootPos();
    BOOL checkNoCollisionCorret();
    BOOL startRestartRoom(u32, int, f32, int);
    void posMove();
    void setWaterY();
    void autoGroundHit();
    BOOL procLavaDamage_init();
    void dProcLavaDamage_init_sub();
    BOOL dProcLavaDamage_init();
    BOOL checkLavaFace(cXyz*, int);
    BOOL checkSwimFallCheck();
    int setRoomInfo();
    void checkFallCode();
    void setShapeAngleOnGround();
    void setStepsOffset();
    void setWorldMatrix();
    void setWaistAngle();
    int setLegAngle(f32, int, s16*, s16*);
    void footBgCheck();
    void setMoveSlantAngle();
    void setStickData();
    void setDemoData();
    /* ---- end range #03 ---- */

    /* 0x043C */ request_of_phase_process_class mPhase;  /* GameCube 0x320 */
    /* 0x0444 */ gptr<J3DModelData> mpCLModelData;
    /* 0x0448 */ gptr<J3DModel> mpCLModel;
    /* 0x044C */ gptr<J3DModel> mpKatsuraModel;
    /* 0x0450 */ gptr<J3DModel> mpYamuModel;  /* [?] by order */
    /* 0x0454 */ be<u32> mpCurrLinktex;  /* ResTIMG* */
    /* 0x0458 */ u8 mLinktexList[0x34];  /* was m458: the list of Link's own texture headers that mpCurrLinktex points at (player04b::TextureOriginals: count, capacity, header pointers): the constructor 0240FE40 sets it up for 10 (lk_ct_listInit), playerInit 02410BE8 sets mpCurrLinktex to it and pushes the linktexS3TC headers; HD's form of what tww's mpCurrLinktex points at; HD: an object (0x34, constructor 0273B560 with 10 entries of 0xC) in place of the GameCube ResTIMG mOtherLinktex */
    /* 0x048C */ u8 mOtherLinktex[0x168];  /* was m48C: after TWW mOtherLinktex: the headers of the other outfit (HD keeps 10 of 0x24): playerInit 02410BE8 copies the originals here and swaps them with mLinktexList for the casual clothes (player04bCopyTextureList); playerDelete 0240EBF0 swaps them back (swapTexture) while the casual clothes are on; HD: 10 objects of 0x24 (constructor 02444280) */
    /* 0x05F4 */ be<u32> mpAnmTexPatternData;  /* GameCube 0x35C (+0x298) */
    /* 0x05F8 */ be<u32> m_texNoAnms;
    /* 0x05FC */ be<u32> mpTexScrollResData;
    /* 0x0600 */ be<u32> m_texMtxAnm;
    /* 0x0604 */ be<u32> m_tex_eye_scroll[2];  /* daPy_matAnm_c* */
    /* 0x060C */ be<u32> mpZOffBlendShape[4];
    /* 0x061C */ be<u32> mpZOffNoneShape[4];
    /* 0x062C */ be<u32> mpZOnShape[4];
    /* 0x063C */ be<u32> mpLhandShape;
    /* 0x0640 */ be<u32> mpRhandShape;
    /* 0x0644 */ u8 mTexAnmCtrl[0x108];  /* was m644: the three HD texture animation controllers (0x58 each): draw 023D9820 attaches [0] / [1] (texture patterns) and [2] (texture scroll) to the model data; procSelfieHD 02415BA4 (d_a_player_main_04b) evaluates them at mTexAnmFrame / mTexScrollFrame; frameBlend (d_a_player_main_07.h) indexes them; HD: three 0x58 animation objects (constructor 027DA984, vtables 0x1016D860..) at 0x644/0x69C/0x6F4 */
    /* 0x074C */ dBgS_AcchCir mAcchCir[3];  /* GameCube 0x3AC (+0x3A0) */
    /* 0x080C */ dBgS_Acch mAcch;  /* dBgS_LinkAcch */
    /* 0x09D0 */ u8 mLinkLinChk[0x6C];  /* dBgS_LinkLinChk */
    /* 0x0A3C */ u8 mRopeLinChk[0x6C];  /* dBgS_RopeLinChk */
    /* 0x0AA8 */ u8 mBoomerangLinChk[0x6C];  /* dBgS_BoomerangLinChk */
    /* 0x0B14 */ u8 mGndChk[0x54];  /* dBgS_LinkGndChk */
    /* 0x0B68 */ u8 mRoofChk[0x4C];  /* dBgS_LinkRoofChk */
    /* 0x0BB4 */ u8 mArrowLinChk[0x6C];  /* dBgS_ArrowLinChk */
    /* 0x0C20 */ u8 mMirLightLinChk[0x6C];  /* dBgS_MirLightLinChk */
    /* 0x0C8C */ u8 mLavaGndChk[0x54];  /* dBgS_ObjGndChk_Spl */
    /* 0x0CE0 */ u8 mPolyInfo[0x10];  /* cBgS_PolyInfo (s16 poly, s16 bg, ..., vtable at +0xC) */
    /* 0x0CF0 */ be<u32> m_HIO;  /* GameCube 0x950 (+0x3A0) [?] */
    /* 0x0CF4 */ gptr<J3DModel> mpHandsModel;
    /* 0x0CF8 */ gptr<J3DModel> mpEquippedSwordModel;
    /* 0x0CFC */ gptr<J3DModel> mpSwgripaModel;  /* [?] by order */
    /* 0x0D00 */ gptr<J3DModel> mpSwgripmsModel;  /* [?] by order */
    /* 0x0D04 */ mDoExt_bckAnm mSwgripmsabBckAnim;  /* GameCube 0x964 */
    /* 0x0D90 */ u8 mD90[0x4A60];  /* HD: GameCube 0x974..0x2FAC re-laid out (the J3DAnm pointers became mDoExt_brk/btkAnm objects, the mirror packet grew). Anchors measured from the constructor: brkAnm 0xD90, btkAnm 0xE08, bckAnm 0xE8C, btkAnm 0xF18, dDlst_mirrorPacket mMirrorPacket 0xF8C, btkAnm 0x43B8, mSwordAnim (bckAnm) 0x4444 with mpEquipItemModel 0x4440 before it, mpParachuteFanMorf 0x44D0, brk/btk animations 0x44D4..0x4B58, 0x4B58 an object of 0x90 (0273B560), 0x4BE8..0x5378 eleven 0xB0 objects (02080404), btk/brk animations 0x537C..0x57F0 */
    /* 0x57F0 */ be<u32> m_pbCalc[2];  /* GameCube 0x2FAC */
    /* 0x57F8 */ u8 mAnmRatioUnder[0x20];  /* mDoExt_AnmRatioPack[2] (HD 0x10 each) */
    /* 0x5818 */ u8 mAnmRatioUpper[0x30];  /* mDoExt_AnmRatioPack[3] */
    /* 0x5848 */ u8 m_anm_heap_under[0x20];  /* daPy_anmHeap_c[2] */
    /* 0x5868 */ u8 m_anm_heap_upper[0x30];  /* daPy_anmHeap_c[3] */
    /* 0x5898 */ J3DFrameCtrl mFrameCtrlUnder[2];
    /* 0x58B8 */ J3DFrameCtrl mFrameCtrlUpper[3];
    /* 0x58E8 */ u8 mSightPacket[0xC08];  /* daPy_sightPacket_c (HD 0xC08, constructor 0240EE9C; GameCube 0x50) */
    /* 0x64F0 */ u8 mJAIZelAnime[0x98];  /* JAIZelAnime (constructor JAIAnimeSound) */
    /* 0x6588 */ be<u32> m_sanm_buffer;  /* GameCube 0x3178 (+0x3410) */
    /* 0x658C */ u8 _658C[4];  /* [?] */
    /* 0x6590 */ daPy_actorKeep_l mActorKeepEquip;  /* GameCube 0x317C (+0x3414) */
    /* 0x6598 */ daPy_actorKeep_l mActorKeepThrow;
    /* 0x65A0 */ daPy_actorKeep_l mActorKeepGrab;
    /* 0x65A8 */ daPy_actorKeep_l mActorKeepRope;
    /* 0x65B0 */ gptr<fopAc_ac_c> mpAttnActorLockOn;  /* setAtnList 023DCD30 */
    /* 0x65B4 */ gptr<fopAc_ac_c> mpAttnActorAction;
    /* 0x65B8 */ gptr<fopAc_ac_c> mpAttnActorA;
    /* 0x65BC */ gptr<fopAc_ac_c> mpAttnActorX;
    /* 0x65C0 */ gptr<fopAc_ac_c> mpAttnActorY;
    /* 0x65C4 */ gptr<fopAc_ac_c> mpAttnActorZ;
    /* 0x65C8 */ gptr<fopAc_ac_c> mpAttnActorHD;  /* HD-only: a fourth item-button attention actor (cleared by setAtnList) */
    /* 0x65CC */ be<u32> m_old_fdata;  /* GameCube 0x31B4 (+0x3418) */
    /* 0x65D0 */ u8 m_tex_anm_heap[0x10];  /* daPy_anmHeap_c */
    /* 0x65E0 */ u8 m_tex_scroll_heap[0x10];  /* daPy_anmHeap_c */
    /* 0x65F0 */ be<s32> mCurProc;  /* GameCube 0x31D8 (+0x3418) */
    /* 0x65F4 */ ProcFunc_l mCurProcFunc;  /* GHS pointer to member (8 bytes, GameCube 12) */
    /* 0x65FC */ u8 mFootEffect[0x98];  /* daPy_footEffect_c[2], GameCube 0x31E8 (+0x3414 from here) */
    /* 0x6694 */ u8 mRippleEcallBack[0x14];  /* was m3280: dPa_rippleEcallBack of the ripple ring ID_AK_JN_HAMON00 around Link: setWaterRipple 0240AB88 starts it (setShipTail) in shallow water, sets its rate and removes it; procFanGlide 02438E78 tests its emitter; playerDelete 0240EBF0 removes it (tww the same); dPa_rippleEcallBack */
    /* 0x66A8 */ u8 mSwimTailEcallBack[0x50];  /* daPy_swimTailEcallBack_c[2] */
    /* 0x66F8 */ daPy_mtxFollowEcallBack_c mProcEffectEcallBack0;  /* was m32E4: daPy_mtxFollowEcallBack_c the procs use for their own effect and commonProcInit 023DFDD8 ends on every proc change: procCutTurn_init 023E6380 (ID_AK_JN_ROUNDATTACKTOE), procCutTurnMove_init 02442190 (CHARGEPOWER00), dProcLavaDamage_init 023FE6B8 / dProcVorcanoFail_init 02424348 (HIDARUMAFIRE), dProcTalismanWait_init 02422950, procBottleOpen 0243DBFC, procFanGlide, the Wind Waker procs; offBodyEffect / onBodyEffect stop / play it (tww the same) */
    /* 0x6704 */ daPy_mtxFollowEcallBack_c mProcEffectEcallBack1;  /* was m32F0: the second proc effect slot, also ended by commonProcInit 023DFDD8: setBottleModel 023DF0FC / procBottleGet_init 0243E534 (ID_AK_JN_FORESTWATER00), procCutTurn_init 023E6380 (ROUNDATTACKSHIBUKI), procCutTurnMove_init 02442190 (CHARGEPOWER01); procCutTurn 02441750 / playerDelete end it (tww the same) */
    /* 0x6710 */ u8 mSmokeEcallBack[0x20];  /* dPa_smokeEcallBack */
    /* 0x6730 */ u8 mCutTurnEcallBack0[0x10];  /* was m331C: dPa_cutTurnEcallBack_c of the spin-attack trail ID_AK_JN_ROUNDATTACK00: setCollision 024076CC starts it (with 01 / 02 below), feeds it the sword-blur positions while its alpha is 0xFF and removes it; procCutTurn 02441750 fades the alpha 0x80 -> 0; playerDelete 0240EBF0 removes it (tww the same); dPa_cutTurnEcallBack_c */
    /* 0x6740 */ u8 mCutTurnEcallBack1[0x10];  /* was m332C: the same for ID_AK_JN_ROUNDATTACK01: setCollision 024076CC, procCutTurn 02441750, playerDelete 0240EBF0 (tww the same) */
    /* 0x6750 */ u8 mCutTurnEcallBack2[0x10];  /* was m333C: the same for ID_AK_JN_ROUNDATTACK02: setCollision 024076CC, procCutTurn 02441750, playerDelete 0240EBF0 (tww the same) */
    /* 0x6760 */ u8 mNeckWaterDropEcallBack[0x20];  /* was m334C: daPy_waterDropEcallBack_c at the neck joint: setWaterDrop 0240CBE0 puts it at CL_JNT_NECK_JNT and runs setSwimWaterDrop on it; offBodyEffect 023D83D8 / onBodyEffect 023D8FF8 stop / play its particles; the constructor 0240FE40 sets its vtable (tww the same); daPy_waterDropEcallBack_c */
    /* 0x6780 */ u8 mBodyWaterDropEcallBack[0x20];  /* was m336C: the same at the body joint CL_JNT_BODY_CHN: setWaterDrop 0240CBE0, offBodyEffect 023D83D8 / onBodyEffect 023D8FF8; setFootMark 02405BC0 / setCutWaterSplash 02406DE0 test its emitter (tww the same) */
    /* 0x67A0 */ u8 mTalismanFlashEcallBack[0x1C];  /* was m338C: daPy_followEcallBack_c of ID_IT_JN_OMAMORI_FLASH00: dProcTalismanWait_init 02422950 starts it at the hand position; commonProcInit 023DFDD8 / playerDelete 0240EBF0 end it (tww the same, plus dProcTalismanWait); daPy_followEcallBack_c */
    /* 0x67BC */ u8 mProcEffectEcallBack2[0x10];  /* was m33A8: the third proc effect slot (daPy_mtxPosFollowEcallBack_c), faded and ended by commonProcInit 023DFDD8: procBottleDrink 0243D838, procCutRollEnd_init 02441C38 (ID_IT_JN_PIYOPIYO00 on the head), procVomitJump_init 023E47D4 (tww the same); daPy_mtxPosFollowEcallBack_c */
    /* 0x67CC */ u8 mDmEcallBack[0x30];  /* daPy_dmEcallBack_c[4] */
    /* 0x67FC */ daPy_mtxFollowEcallBack_c mFanWindEcallBack;  /* was m33E8: daPy_mtxFollowEcallBack_c of the Deku Leaf wind ID_AK_JN_UCHIWAWIND00: procFanSwing 024386C4 makes it on the YUCHW00 LWIND joint; setCollision 024076CC / playerDelete 0240EBF0 end it (tww the same) */
    /* 0x6808 */ u8 mFanSwingCb[0xC];  /* daPy_fanSwingEcallBack_c */
    /* 0x6814 */ u8 mWaterfallSplashEcallBack[0x10];  /* was m3400: daPy_mtxPosFollowEcallBack_c of the waterfall splash: setWaterRipple 0240AB88 makes ID_AK_JN_ELEMENTSHIBUKI00 on the head joint while Link stands or swims in a waterfall (or hangs on attribute 0x17), ends it otherwise and tints it with the sea colour (tww the same); daPy_mtxPosFollowEcallBack_c */
    /* 0x6824 */ u8 mWadeSplashEcallBack[0x1C];  /* was m3410: daPy_followEcallBack_c of the wading splash: setFootEffect 02406480 places ID_AK_JN_ELEMENTSHIBUKI00 45 in front of Link at the water surface, facing back, while he walks in water deeper than 25, and ends it otherwise (tww the same); daPy_followEcallBack_c */
    /* 0x6840 */ daPy_mtxFollowEcallBack_c mMirrorShieldEcallBack;  /* was m342C: daPy_mtxFollowEcallBack_c of ID_AK_SN_MIRRORSHIELD00 on the shield model: checkLightHit 0240584C makes it while the mirror shield reflects light and ends it (alpha 0) otherwise (tww the same) */
    /* 0x684C */ u8 mShieldLightHitEcallBack[0x1C];  /* was m3438: daPy_followEcallBack_c of ID_AK_SN_HITSHIELDLIGHT00: checkLightHit 0240584C puts it where the reflected beam (mFanLightCps) hits, larger on Jalhalla (fpcNm_BPW_e), and ends it when nothing is hit (tww the same); daPy_followEcallBack_c */
    /* 0x6868 */ daPy_mtxFollowEcallBack_c mLightSaverEcallBack;  /* was m3454: daPy_mtxFollowEcallBack_c of ID_AK_JN_LIGHTSAVER00 on the sword joint: setLightSaver 0240C32C makes it in chance mode or with the soup power-up and ends it; deleteEquipItem 023DC7AC ends it (tww the same) */
    /* 0x6874 */ u8 mRainDropEcallBack[0x20];  /* was m3460: two daPy_mtxPosFollowEcallBack_c of ID_AK_JN_LK_NURE_POTA01 on the neck / body joints: setWaterDrop 0240CBE0 makes them while nothing is overhead (dKyr_player_overhead_bg_chk, no roof, not a MISC stage) and ends them otherwise; offBodyEffect 023D83D8 / onBodyEffect 023D8FF8 (tww the same); daPy_mtxPosFollowEcallBack_c[2] */
    /* 0x6894 */ be<u32> mpAttention;  /* dAttention_c* (play + 0x5804) */
    /* 0x6898 */ be<u32> mpAttnEntryA;  /* dAttList_c* */
    /* 0x689C */ be<u32> mpAttnEntryX;
    /* 0x68A0 */ be<u32> mpAttnEntryY;
    /* 0x68A4 */ be<u32> mpAttnEntryZ;
    /* 0x68A8 */ be<u32> mpAttnEntryHD;  /* HD-only: the fourth item button entry (+4 from here) */
    /* 0x68AC */ be<u32> mTactZevEventName;  /* was m3494: TWW mTactZevEventName: setTactZev 02443768 stores the event name; procTactPlay 0243AC64 tests it, procTactPlayEnd_init 02412948 orders that event with the partner; GameCube 0x3494 (+0x3418) */
    /* 0x68B0 */ u8 mLightInfluence[0x20];  /* LIGHT_INFLUENCE */
    /* 0x68D0 */ be<f32> m68D0;  /* HD-only float (set by the constructor) */
    /* 0x68D4 */ be<u8> mDirection;  /* GameCube 0x34B8 (+0x341C) */
    /* 0x68D5 */ be<u8> mFrontWallType;
    /* 0x68D6 */ be<u8> m34BA;
    /* 0x68D7 */ be<u8> mCurrItemHeapIdx;
    /* 0x68D8 */ be<u8> mBaseFootIdx;  /* was m34BC: posMoveFromFootPos 023FCB9C: the foot that stands lower (0 right, 1 left; 2 without old-frame data); Link is moved by that foot's movement (tww the same) */
    /* 0x68D9 */ be<u8> mReadyItemBtn;
    /* 0x68DA */ be<u8> mFootEffectPosType;
    /* 0x68DB */ be<s8> mReverb;
    /* 0x68DC */ be<u8> mLeftHandIdx;
    /* 0x68DD */ be<u8> mRightHandIdx;
    /* 0x68DE */ be<u8> mRootMoveMode;  /* was m34C2: how the animation's root translation moves Link: the procs set a mode (0 .. 11) when they start their animation; posMove 023FDA70 switches on it (1 / 5 move current.pos by the root's change, 5 also in y; 2 / 6 keep the root; 3 / 7 and 4 restart into 1 / 5; 8 / 9 the other way round; 10 / 11 move the root by the position change); jointBeforeCB 023D6B30 holds the root in x / z (1) or at mRootTransY (5); commonProcInit 023DFDD8 turns 1 into 2 (tww the same) */
    /* 0x68DF */ be<u8> m34C3;
    /* 0x68E0 */ be<u8> mCutComboCount;  /* was m34C4: the sword combo count: changeCutProc 023E73FC adds 1 per cut and picks the cut by it (4 the finishing EA / EB cut, mCutCount from it); execute 0240CDD0 adds 2 on a hit at 2 / 3 and clears it when mCutComboTimer runs out; procCutTurn_init 023E6380 / procCutExMJ_init 023E6DC4 test 6; the finishing cuts clear it (tww the same) */
    /* 0x68E1 */ be<u8> m34C5;
    /* 0x68E2 */ u8 mJointFlag[0x2A];  /* was m68E2: per joint (42): bit 1 quaternion saved, bit 2 transform saved in mJointState (jointBeforeCB 023D6B30 / lk_saveTransInfo set them); jointAfterCB 023D7564 restores and clears them (the HD per-joint form of tww's m34C6); HD: m34C6 (GameCube 0x34C6) and HD-only bytes, not measured */
    /* 0x690C */ be<u8> mActivePlayerBombs;  /* GameCube 0x34C7 (+0x3445) */
    /* 0x690D */ be<u8> mItemTrigger;
    /* 0x690E */ be<u8> mItemButton;
    /* 0x690F */ be<u8> mOldItemButton;  /* was m34CA: setStickData 023F32E4 keeps the last mItemButton here before reading the buttons and puts it back in DEMO_KEEP (tww the same) */
    /* 0x6910 */ be<u8> mDekuSpRestartPoint;
    /* 0x6911 */ be<u8> mTactZevMelodyNum;  /* was m34CC: TWW mTactZevMelodyNum: setTactZev 02443768 stores the melody number; procTactPlay_init 02439D54 / procTactPlay 0243AC64 compare the played song with it */
    /* 0x6912 */ be<u8> mWaterfallRippleTimer;  /* was m34CD: setWaterRipple 0240AB88: when 0 it sets the ID_IT_JN_WP_HAMON03 ripple at the root joint, starts a quake and sets 15; otherwise counts down and stops the quake at 5 (tww the same) */
    /* 0x6913 */ be<u8> mGetDemoFlg;  /* was m34CE: setGetDemo 023DBDD0: bit 1 the DEFAULT_GETITEM event is run here (in the air or off the ground), bit 2 the get-item sound was started, bit 4 the item message closed (cut end); cleared when no get-item event runs (tww the same) */
    /* 0x6914 */ u8 _6914[2];  /* [?] */
    /* 0x6916 */ be<s16> mProcVar0;  /* GameCube 0x34D0 (+0x3446) */
    /* 0x6918 */ be<s16> mProcVar1;
    /* 0x691A */ be<s16> mProcVar2;
    /* 0x691C */ be<s16> mProcVar3;
    /* 0x691E */ be<s16> mProcVar4;
    /* 0x6920 */ be<s16> mProcVar5;
    /* 0x6922 */ u8 _6922[2];  /* HD-only [?] */
    /* 0x6924 */ be<s16> mStickAngle;  /* was m34DC: setStickData 023F32E4: the control stick angle (CPad stick angle, + 0x8000 in the camera modes that need it); the rope, ship-attention, vomit and Wind Waker procs take directions from it; the cut inits copy it (tww the same); GameCube 0x34DC (+0x3448) */
    /* 0x6926 */ be<s16> m34DE;
    /* 0x6928 */ be<s16> mWaistAngleZ;  /* was m34E0: jointBeforeCB 023D6B30: the Z rotation of the waist joint; procFanGlide 02438E78 leans it (clamped to +-0x2000), setWaistAngle 023FFB08 sets it; footBgCheck 0240005C / posMoveFromFootPos 023FCB9C correct the feet by it; commonProcInit 023DFDD8 clears it (tww the same) */
    /* 0x692A */ be<s16> mGroundAngle;  /* was m34E2: execute 0240CDD0: the slope of the ground along shape_angle.y (getGroundAngle, 0 in the air or on some grounds); setShapeAngleOnGround tilts shape_angle.x by it, setBlendMoveAnime 023E14A0 / checkNormalDamage 023F9D84 / setNeckAngle 02400C4C / procCrouch 0241AC94 read it (tww the same) */
    /* 0x692C */ be<s16> mWaistAngleY;  /* was m34E4: jointBeforeCB 023D6B30: the Y rotation of the waist joint; procFanGlide 02438E78 turns it (clamped to +-0x2000); commonProcInit 023DFDD8 clears it (tww the same) */
    /* 0x692E */ be<s16> mAtnShapeAngleY;  /* was m34E6: the shape angle kept while moving with attention: setAtnList 023DCD30, procWait 024177A8 and procCutTurnMove 02442470 store it, setSpeedAndAngleAtn 02417538 / setSpeedAndAngleAtnBack 0241699C put shape_angle.y back to it; procCutTurnMove takes the stick direction relative to it (tww the same) */
    /* 0x6930 */ be<s16> mMoveTargetAngleY;  /* was m34E8: setStickData 023F32E4: mStickAngle plus the camera angle (the demo move angle in demos, 0 without input); the move / swim / attention speed functions turn current.angle.y and shape_angle.y toward it, getDirectionFromShapeAngle 023E4E18 / getDirectionFromCurrentAngle 023E3398 compare with it (tww the same) */
    /* 0x6932 */ be<s16> mOldStickAngle;  /* was m34EA: execute 0240CDD0 copies mStickAngle here; setStickData 023F32E4 turns the difference into the spin input (mStickSpinAngle); setSpeedAndAngleNormal 0241650C / checkNextMode test a backward flip (tww the same) */
    /* 0x6934 */ be<s16> m34EC;
    /* 0x6936 */ u8 _6936[2];
    /* 0x6938 */ be<u32> mpSeAnm;  /* HD: a pointer to the SE animation data (GameCube u16 mSeAnmIdx at 0x34EE); resetSeAnime stores 0x10035574 */
    /* 0x693C */ be<f32> m693C;  /* [?] probably mSeAnmRate (GameCube 0x360C) */
    /* 0x6940 */ be<u16> mSeAnmDemoAnmNo;  /* was m34F0: setSeAnime 023E07E4 copies the animation heap's mDemoAnmNo here next to mSeAnmIdx and keeps the loaded sound animation only while both match; resetSeAnime 023E07C0 / playerInit 02410BE8 set 0xFFFF (tww the same); GameCube 0x34F0 (+0x3450) */
    /* 0x6942 */ be<s16> m34F2;
    /* 0x6944 */ be<s16> m34F4;
    /* 0x6946 */ be<s16> mHatAAngleZ;  /* was m34F6: jointBeforeCB 023D6B30: the Z angle of the HATA joint (with mHeadFrontAngleX - 0x8000 and mHatASwingZ); setHatAngle 024022D0 eases it toward the wind / motion direction (cLib_addCalcAngleS2), adds mHatASpeedZ and the head pitch change and clamps it (tww the same) */
    /* 0x6948 */ be<s16> mHatAAngleY;  /* was m34F8: jointBeforeCB 023D6B30: the Y angle of the HATA joint; setHatAngle 024022D0 eases it toward the sideways direction, adds mHatASpeedY minus the head yaw change, clamps it to +-0x3800 (tww the same) */
    /* 0x694A */ be<s16> mHatBAngleZ;  /* was m34FA: jointBeforeCB 023D6B30: the Z angle of the HATB joint (with mHatBSwingZ); setHatAngle 024022D0 takes off mHatAAngleZ's change, eases it to 0, adds mHatBSpeedZ, clamps it to -0x800..0x1000 (tww the same) */
    /* 0x694C */ be<s16> mHatBAngleY;  /* was m34FC: jointBeforeCB 023D6B30: the Y angle of the HATB joint; setHatAngle 024022D0 takes off mHatAAngleY's change, eases it to 0, adds mHatBSpeedY, keeps it + mHatAAngleY within +-0x3800 (tww the same) */
    /* 0x694E */ be<s16> mHatCAngleZ;  /* was m34FE: jointBeforeCB 023D6B30: the Z angle of the HATC joint (with mHatCSwingZ); setHatAngle 024022D0 takes off mHatBAngleZ's change, eases it to 0, adds mHatCSpeedZ, clamps it to -0x800..0x1000 (tww the same) */
    /* 0x6950 */ be<s16> mHatCAngleY;  /* was m3500: jointBeforeCB 023D6B30: the Y angle of the HATC joint; setHatAngle 024022D0 takes off mHatBAngleY's change, eases it to 0, adds mHatCSpeedY, keeps the sum of the three Y angles within +-0x3800 (tww the same) */
    /* 0x6952 */ be<s16> mHatASpeedZ;  /* was m3502: setHatAngle 024022D0: 0.2 x the frame's change of mHatAAngleZ, added to it on the next frame (tww the same) */
    /* 0x6954 */ be<s16> mHatASpeedY;  /* was m3504: setHatAngle 024022D0: 0.2 x the frame's change of mHatAAngleY, added to it on the next frame (tww the same) */
    /* 0x6956 */ be<s16> mHatBSpeedZ;  /* was m3506: setHatAngle 024022D0: 0.2 x the frame's change of mHatBAngleZ, added to it on the next frame (tww the same) */
    /* 0x6958 */ be<s16> mHatBSpeedY;  /* was m3508: setHatAngle 024022D0: 0.2 x the frame's change of mHatBAngleY, added to it on the next frame (tww the same) */
    /* 0x695A */ be<s16> mHatCSpeedZ;  /* was m350A: setHatAngle 024022D0: 0.2 x the frame's change of mHatCAngleZ, added to it on the next frame (tww the same) */
    /* 0x695C */ be<s16> mHatCSpeedY;  /* was m350C: setHatAngle 024022D0: 0.2 x the frame's change of mHatCAngleY, added to it on the next frame (tww the same) */
    /* 0x695E */ be<s16> mHatASwingZ;  /* was m350E: setHatAngle 024022D0: the wind swing of HATA, 2280 x strength x cos(mHatSwingPhase); jointBeforeCB 023D6B30 adds it to the HATA Z angle (tww the same) */
    /* 0x6960 */ be<s16> mHatBSwingZ;  /* was m3510: setHatAngle 024022D0: the wind swing of HATB, 3908 x strength x cos(mHatSwingPhase - 3 steps); jointBeforeCB 023D6B30 adds it to the HATB Z angle (tww the same) */
    /* 0x6962 */ be<s16> mHatCSwingZ;  /* was m3512: setHatAngle 024022D0: the wind swing of HATC, 7568 x strength x cos(mHatSwingPhase - 6 steps); jointBeforeCB 023D6B30 adds it to the HATC Z angle (tww the same) */
    /* 0x6964 */ be<s16> mHatSwingPhase;  /* was m3514: setHatAngle 024022D0: advanced by 1500 + 4060 x the wind strength each frame; the phase of mHatASwingZ / mHatBSwingZ / mHatCSwingZ (tww the same) */
    /* 0x6966 */ be<s16> mLMomiAngleY;  /* was m3516: setHatAngle 024022D0: -8192 x the sideways wind x strength, at most 0x1000, 0 while swimming; jointBeforeCB 023D6B30: the Y rotation of the LMOMI joint (tww the same) */
    /* 0x6968 */ be<s16> mRMomiAngleY;  /* was m3518: setHatAngle 024022D0: the same value as mLMomiAngleY, at least -0x1000, 0 while swimming; jointBeforeCB 023D6B30: the Y rotation of the RMOMI joint (tww the same) */
    /* 0x696A */ be<s16> mMomiAngleZ;  /* was m351A: setHatAngle 024022D0: -8192 x the forward wind x strength, 0 while swimming; jointBeforeCB 023D6B30: the Z rotation of both MOMI joints (tww the same) */
    /* 0x696C */ be<s16> mMoveSlantAngle;  /* was m351C: setMoveSlantAngle 02400990: the sideways lean while running (eased, shape_angle.z = half of it); setGrabItemPos 0240A41C tilts the held actor by it, setNeckAngle 02400C4C reads it (tww the same) */
    /* 0x696E */ be<s16> m351E;
    /* 0x6970 */ u8 m3520[2];
    /* 0x6972 */ be<s16> mCutComboTimer;  /* was m3522: the window for the next combo cut: the cut inits (procCutA_init 023E5F88, procCutF_init 023E5BD0, ...) set it from the HIO cut tables, the finishing cuts clear it; execute 0240CDD0 counts it down and clears mCutComboCount at 0 (tww the same) */
    /* 0x6974 */ be<s16> mStickSpinTimer;  /* was m3524: setStickData 023F32E4: set to the HIO mCutTurnR frame count whenever mStickSpinAngle grows, counted down otherwise; at 0 mStickSpinAngle is cleared (tww the same) */
    /* 0x6976 */ be<s16> mSlideStartTimer;  /* was m3526: changeSlideProc 023E4CD0: 8 off slide polygons; on one it counts down while the normal speed is eased to 0 and the slide starts only when it is 0 and the speed is 5 or less; procSlideFront_init 023E4B38 / procSlideBack_init 023E4C04 / playerInit 02410BE8 set 8 (tww the same) */
    /* 0x6978 */ be<s16> mHeadFrontAngleX;  /* was m3528: setHatAngle 024022D0: the pitch of the head's front vector (l_neck_front), whose change (halved, clamped to +-0x200) moves the hat Z angles; jointBeforeCB 023D6B30 adds it to the HATA Z angle (tww the same) */
    /* 0x697A */ be<s16> mHeadFrontAngleY;  /* was m352A: setHatAngle 024022D0: the yaw of the head's front vector (kept while it points steeply up or down), whose change (halved, clamped to +-0x800) moves the hat Y angles (tww the same) */
    /* 0x697C */ be<s16> m352C;
    /* 0x697E */ be<s16> mBgWaitTimer;  /* was m352E: makeBgWait 024131D0: 900 from the player init, counted down each call; while set the start waits for a treasure-chest (TBOX) ground or, with parameter bit 0x80, for a raft (OBJ_IKADA) ground (then cleared) (tww the same) */
    /* 0x6980 */ be<u16> mTexAnmFrame;  /* was m3530: the frame of the texture pattern (eyes / mouth) animation: playTextureAnime 023FBCEC advances it (blinks, demo frame); draw 023D9820 sets the pattern data frame from it; the talk / surprise / smile demo procs, procSelfieHD 02415BA4 and procShipScope 024327A4 set it (tww the same) */
    /* 0x6982 */ be<u16> mTexScrollFrame;  /* was m3532: the frame of the texture scroll animation: playTextureAnime 023FBCEC advances it; draw 023D9820 sets the scroll data frame from it; setTextureScrollResource 023DC43C and the surprise demo procs set it (tww the same) */
    /* 0x6984 */ be<s16> mFanWindTimer;  /* was m3534: procFanSwing 024386C4 sets 10; setCollision 024076CC scales the fan wind capsule by it / 10 and counts it down (tww the same) */
    /* 0x6986 */ be<s16> mFanWindAngleY;  /* was m3536: procFanSwing 024386C4: shape_angle.y at the swing; setCollision 024076CC aims the fan wind capsule by it (tww the same) */
    /* 0x6988 */ be<s16> mFanWindAngleX;  /* was m3538: procFanSwing 024386C4: the downward angle of the swing (0 or toward the ground); setCollision 024076CC updates it from the ground under the wind and aims the capsule by it (tww the same) */
    /* 0x698A */ be<s16> mFanWindSphTimer;  /* was m353A: procFanSwing 024386C4 sets 3, procFanGlide 02438E78 5; setCollision 024076CC sets the fan wind sphere at Link while it counts down (tww the same) */
    /* 0x698C */ be<s16> mShipRideAngleX;  /* was m353C: setShipRidePos 023E287C: follows the ship's shape_angle.x by mShipRideAngleXSpeed; setWorldMatrix 023FF6A0 rotates Link by it on the ship; procShipReady_init 023ECA94 / dProcBossWarp_init 024248C0 clear it (tww the same) */
    /* 0x698E */ be<s16> mShipRideAngleZ;  /* was m353E: setShipRidePos 023E287C: follows the ship's shape_angle.z by mShipRideAngleZSpeed; setWorldMatrix 023FF6A0 rotates Link by it; procShipReady_init 023ECA94 / dProcBossWarp_init 024248C0 clear it (tww the same) */
    /* 0x6990 */ be<s16> mShipRideAngleXSpeed;  /* was m3540: setShipRidePos 023E287C: the follow speed of mShipRideAngleX toward the ship's shape_angle.x (adds 0.75 x the difference, eased to 0, added to mShipRideAngleX each frame); procShipReady_init 023ECA94 clears it (tww the same) */
    /* 0x6992 */ be<s16> mShipRideAngleZSpeed;  /* was m3542: setShipRidePos 023E287C: the follow speed of mShipRideAngleZ toward the ship's shape_angle.z (adds half the difference, eased to 0, added to mShipRideAngleZ each frame); procShipReady_init 023ECA94 clears it (tww the same) */
    /* 0x6994 */ be<s16> mFrontWallTimer;  /* was m3544: changeFrontWallTypeProc 02418E00: frames Link keeps pushing against the front wall; above the HIO small-jump / wall-catch frame counts it starts the small jump, hang, or vertical jump, otherwise cleared; commonProcInit 023DFDD8 clears it (tww the same) */
    /* 0x6996 */ be<s16> mShieldFrontRangeYAngle;
    /* 0x6998 */ be<s16> m3548;
    /* 0x699A */ u8 m354A[2];
    /* 0x699C */ be<s16> mTinkleHoverTimer;
    /* 0x699E */ be<s16> mTinkleShieldTimer;
    /* 0x69A0 */ be<s16> m3550;
    /* 0x69A2 */ be<u16> mKeepItem;
    /* 0x69A4 */ be<s16> mForestWaterUseTimer;  /* was m3554: procBottleOpen 0243DBFC sets 3 when the Forest Water is used; setDemoData 023F22FC counts it down outside events and then clears daPyFlg1_FOREST_WATER_USE (tww the same) */
    /* 0x69A6 */ u8 m3556[2];
    /* 0x69A8 */ be<s16> mFanSmaAngleZ;  /* was m3558: fanJointCB 023D641C: ZrotS of the FANSMA joint of the small Deku Leaf model; setShapeFanLeaf 023E771C sets 0x600 x sin(m355C) (tww the same) */
    /* 0x69AA */ be<s16> mFanSmbAngleZ;  /* was m355A: fanJointCB 023D641C: ZrotS of the FANSMB joint; setShapeFanLeaf 023E771C sets 0x800 x sin(m355C - 0xE00) (tww the same) */
    /* 0x69AC */ be<s16> m355C;
    /* 0x69AE */ be<s16> m355E;
    /* 0x69B0 */ be<u16> mEquipItem;
    /* 0x69B2 */ be<u16> mAnimeEquipItem;  /* was m3562: TWW mAnimeEquipItem: the item of the equip animation: setAnimeEquipItem 023EF750 / setAnimeEquipSword 023E9F60 / setAnimeUnequip 023ECEF0 set it; checkItemAction 023FB230 copies it into mEquipItem */
    /* 0x69B4 */ csXyz mNeckAngle;  /* was m3564: jointBeforeCB 023D6B30 rotates the head joint by it (y, z, x); setNeckAngle 02400C4C eases it toward the look target (limited to the ship angle while riding); procDamage 0241E150 sets it from the hit direction; procWait 024177A8 tests it for 0 (tww the same) */
    /* 0x69BA */ u8 _69BA[2];
    /* 0x69BC */ be<s32> mCameraInfoIdx;  /* GameCube 0x356C (+0x3450) */
    /* 0x69C0 */ be<s32> mProcVar6;
    /* 0x69C4 */ be<s32> mProcVar7;
    /* 0x69C8 */ be<s32> mStickSpinAngle;  /* was m3578: setStickData 023F32E4: the summed stick rotation while it keeps turning one way (reset when it turns back or mStickSpinTimer runs out); more than 0xF800 either way is a spin input: checkNextActionFromButton 023EFF20, procBackJumpLand 0241C140, procJumpCut 02442C1C / setJumpCutAtParam 023ED380; changeCutProc 023E73FC / procCutTurn_init 023E6380 / procCutRoll_init 02441A44 clear it (tww the same) */
    /* 0x69CC */ be<s32> mOldGroundCode;  /* was m357C: execute 0240CDD0 / makeBgWait 024131D0 copy mGroundCode here before updating it; changeAutoJumpProc 023F81A4 tests 3 (tww the same) */
    /* 0x69D0 */ be<s32> mGroundCode;  /* was m3580: execute 0240CDD0 / makeBgWait 024131D0: GetGroundCode of the ground (-1 in the air); checkFallCode 023FEA6C, setNeckAngle 02400C4C, setBlendMoveAnime 023E14A0, posMoveFromFootPos 023FCB9C ... test codes 4 / 5 / 8 (tww the same) */
    /* 0x69D4 */ be<s32> mCurrAttributeCode;
    /* 0x69D8 */ be<s32> mHangAttributeCode;  /* was m3588: changeHangEndProc 02428230: the attribute code of the ground found when hanging (dBgS_Attr_UNK1B when none); setWaterRipple 0240AB88 starts the waterfall splash while hanging on code 0x17 (tww the same) */
    /* 0x69DC */ be<s32> mStaffIdx;
    /* 0x69E0 */ be<s32> mEventIdx;
    /* 0x69E4 */ be<s32> mRestartPoint;
    /* 0x69E8 */ be<u8> mStrongJumpCut;  /* was m69E8: the name of the d_a_player_main_02.h view jumpCutState_c: procJumpCut_init 023ED380 / procJumpCut 02442C1C set it for a sword / Boko jump cut with |mStickSpinAngle| > 0xF800 (a spun-up jump cut, not cursed); procJumpCutLand then goes on to the spin cut (procCutTurn_init) while it is set and clears it; the damage code (d_cc_uty 025192A8) applies a spin attack's damage twice and the Stalfos / ReDead hit timers (d_a_st damage_check, daRd_c::checkTgHit 0245C770) use 2 / 3 while it is set; 023ED5A4 clears it; HD-only (+0x3458 from 0x69F0) */
    /* 0x69E9 */ u8 _69E9[5];
    /* 0x69EE */ be<s16> mHeavyStateCounter;  /* was m69EE: the name of the d_a_player_main_04_update.h view LinkUpdateHeavyState: each stuck morth (action_omoi in daKS_Execute) adds 1 per frame; execute 0240CDD0 turns daPyFlg0_HEAVY_STATE on at 5 or more, off otherwise, and clears it; HD-only */
    /* 0x69F0 */ be<f32> m3598;  /* GameCube 0x3598 (+0x3458) */
    /* 0x69F4 */ be<f32> mFootMoveDist;  /* was m359C: posMoveFromFootPos 023FCB9C: the XZ move of the base foot, smoothed (0.3 new + 0.7 old) while the stick is steady (tww the same) */
    /* 0x69F8 */ be<f32> m35A0;
    /* 0x69FC */ be<f32> m35A4;
    /* 0x6A00 */ be<f32> m35A8;
    /* 0x6A04 */ be<f32> m35AC;
    /* 0x6A08 */ be<f32> mStickDistance;
    /* 0x6A0C */ be<f32> mOldStickDistance;  /* was m35B4: execute 0240CDD0 copies mStickDistance here; posMoveFromFootPos 023FCB9C smooths mFootMoveDist only while the stick changed by less than 0.2 (tww the same) */
    /* 0x6A10 */ be<f32> mFootBgOffsetY;  /* was m35B8: footBgCheck 0240005C: eased (cLib_addCalc) to the lower foot's ground height below current.pos.y; added to the model base matrix y and taken off mInvWorldMtx (tww the same) */
    /* 0x6A14 */ be<f32> mNormalSpeed;
    /* 0x6A18 */ u8 m35C0[4];
    /* 0x6A1C */ be<f32> m35C4;
    /* 0x6A20 */ be<f32> m35C8;
    /* 0x6A24 */ be<f32> mGrabItemBounceY;  /* was m35CC: setGrabItemPos 0240A41C: -0.2 x speed.y per frame (at most 40), chased back to 0 and added to the held actor's y; freeGrabItem 023DCF8C clears it (tww the same) */
    /* 0x6A28 */ be<f32> mWaterY;
    /* 0x6A2C */ be<f32> m35D4;
    /* 0x6A30 */ be<f32> m35D8;
    /* 0x6A34 */ be<f32> mHangGroundH;
    /* 0x6A38 */ be<f32> mRootTransY;  /* was m35E0: jointBeforeCB 023D6B30 sets the root translation y to it in mRootMoveMode 5; posMove 023FDA70 takes it from the animation when mode 4 becomes 5; changeFrontWallTypeProc 02418E00 / procClimbDownStart_init set 43.67353 (tww the same) */
    /* 0x6A3C */ be<f32> m35E4;
    /* 0x6A40 */ be<f32> m35E8;
    /* 0x6A44 */ be<f32> m35EC;
    /* 0x6A48 */ be<f32> mFallStartY;  /* was m35F0: the height a fall is measured from: posMoveFromFootPos 023FCB9C keeps it at current.pos.y on the ground, commonProcInit 023DFDD8 / procHookshotFly / procFanGlide / procAutoJump_init set mProcStartPos.y; changeLandProc 0241C898, setFallVoice 0241D6CC, procJumpCutLand_init 02442990 and procBtVerticalJumpLand_init 02430858 use it - current.pos.y (tww the same) */
    /* 0x6A4C */ be<f32> m35F4;
    /* 0x6A50 */ be<f32> mLadderTopY;  /* was m35F8: setFrontWallType 023E3850: the top y of the ladder triangle in front (wall codes 4 / 5; m3724 gets the bottom middle); changeFrontWallTypeProc 02418E00 keeps current.pos.y at most 150 below it (tww the same) */
    /* 0x6A54 */ be<f32> mSwordAtScale;  /* was m35FC: the reach of the sword attack capsules: setNormalCutAtParam 023E5B0C / setFinishCutAtParam 023E5678 / setExtraCutAtParam 023E6D00 / setExtraFinishCutAtParam 023E6A58 / setJumpCutAtParam 023ED380 set it from the HIO cut tables; setSwordAtCollision 02406E74 scales the root-to-top vectors by it (tww the same) */
    /* 0x6A58 */ be<f32> m3600;
    /* 0x6A5C */ be<f32> m3604;
    /* 0x6A60 */ be<f32> mSwimTiredOffsetY;  /* was m3608: setSwimTimerStartStop 023F8924 eases it toward a sink depth as the swim timer runs low (0 otherwise); setWorldMatrix 023FF6A0 adds it to the model y; commonProcInit 023DFDD8 clears it (tww the same) */
    /* 0x6A64 */ be<f32> mSeAnmRate;  /* [?] see m693C */
    /* 0x6A68 */ be<f32> mWhirlPullSpeed;  /* was m3610: posMove 023FDA70: chased toward 40 while a whirlpool (mWhirlId) pulls Link, who moves by it around the whirlpool; 0 otherwise (tww the same) */
    /* 0x6A6C */ be<u32> mShadowId;  /* GameCube 0x3614 (+0x3458); probably unused in HD (no shadow ids) */
    /* 0x6A70 */ be<u32> mModeFlg;
    /* 0x6A74 */ be<u32> mMtrlSndId;
    /* 0x6A78 */ be<u32> mFrollCrashMtrlSndId;  /* was m3620: TWW onFrollCrashFlg(): the material sound id of what Link rolled into; procFrontRoll 0241BAB8 sets it from mAcchCir[0], onFrollCrashFlg (in 02444F0C) from the caller; procFrontRollCrash_init 0241B904 plays JA_SE_LK_BODYATTACK with it */
    /* 0x6A7C */ be<u32> m3624;
    /* 0x6A80 */ be<u32> mGameOverId;
    /* 0x6A84 */ be<u32> mTactZevPartnerId;
    /* 0x6A88 */ be<u32> m3630;
    /* 0x6A8C */ be<u32> mWhirlId;
    /* 0x6A90 */ be<u32> mMsgId;
    /* 0x6A94 */ be<u32> mpSeAnmFrameCtrl;
    /* 0x6A98 */ be<s16> mOutPowerAngle;  /* was m3640: the direction of the outside push: setOutPower 024438D4 sets it or combines a new push with the old one (atan2 of the summed vectors); posMove 023FDA70 moves Link by mOutPower along it (tww the same) */
    /* 0x6A9A */ u8 _6A9A[2];
    /* 0x6A9C */ be<f32> mOutPower;  /* was m3644: the strength of the outside push (TWW setOutPower()): setOutPower 024438D4 sets or sums it; posMove 023FDA70 moves current.pos by it along mOutPowerAngle and clears it; setNeckAngle 02400C4C / setBlendMoveAnime 023E14A0 test it (tww the same) */
    /* 0x6AA0 */ u8 mRootJointQuat[0x10];  /* was m3648: jointBeforeCB 023D6B30: during the guard / grab / ready-throw upper animations it keeps the root joint's quaternion (the root gets the identity) and concatenates it on the waist chain; playerInit 02410BE8 sets w 1 (tww the same root save); Quaternion */
    /* 0x6AB0 */ u8 mJointState[0x7E0];  /* was m3658: the per-joint saved state PlayerJointState (d_a_player_main_01.h: 42 quaternions, 42 J3DTransformInfo): jointBeforeCB 023D6B30 saves a joint's quaternion before turning it, lk_saveTransInfo its transform before moving it; jointAfterCB 023D7564 restores them as mJointFlag says (tww mJointState / m3668 for one joint); HD: the per-joint saved state PlayerJointState (d_a_player_main_01.h): 42 quaternions then 42 J3DTransformInfo; GameCube mJointState (Quaternion) / m3668 (J3DTransformInfo) for one joint, HD-only from 0x6AE0 */
    /* 0x7290 */ cXyz mProcStartPos;  /* was m3688: commonProcInit 023DFDD8: current.pos when a proc starts (procHookshotFly 02437F00 / procFanGlide 02438E78 / procAutoJump_init 023F7958 / procRopeReady_init 023EB864 renew it); procSideStep 0241A604 / procBackJump 0241C094 / procFall 0241DD5C / changeLandProc 0241C898 measure how far Link dropped below its y (tww the same); GameCube 0x3688 (+0x3C08) */
    /* 0x729C */ cXyz mOldSpeed;
    /* 0x72A8 */ cXyz mSlipSpeed;  /* was m36A0: posMoveFromFootPos 023FCB9C: the sliding inertia, built from the speed lost each frame (0.75 x (old - new speed)) and eased back to 0 (cleared off slippery ground); posMove 023FDA70 adds it to current.pos; checkIceSlipFall 024197A0 / procIceSlipFall_init 02419514 and the blend-move functions read it; procFanSwing 024386C4 pushes it back (tww the same) */
    /* 0x72B4 */ cXyz mOldSlipSpeed;  /* was m36AC: posMoveFromFootPos 023FCB9C copies mSlipSpeed here before updating it; procWait 024177A8 and procIceSlipAlmostFall_init 024173F4 use it (tww the same) */
    /* 0x72C0 */ cXyz mBeltConveyerSpeed;  /* was m36B8: setBeltConveyerPower 023FC568: eased toward the conveyor's push (0 off a conveyor); posMove 023FDA70 adds it to current.pos; changeAutoJumpProc 023F81A4 / setBlendMoveAnime / setNeckAngle read it (tww the same) */
    /* 0x72CC */ cXyz mSwordRootPos;  /* was m36C4: execute 0240CDD0: the root of the held weapon (sword, Skull Hammer, Boko stick, Deku Leaf, else the CL_PODA joint), next to mSwordTopPos; setSwordAtCollision 02406E74 builds the attack capsules from it; setCollision 024076CC / setHammerWaterSplash 0240C0A8 (tww the same) */
    /* 0x72D8 */ cXyz mOldSwordTopPos;  /* was m36D0: execute 0240CDD0 copies mSwordTopPos here before updating it; setSwordAtCollision 02406E74 sweeps from it (tww the same) */
    /* 0x72E4 */ cXyz mOldSwordRootPos;  /* was m36DC: execute 0240CDD0 copies mSwordRootPos here before updating it; setSwordAtCollision 02406E74 (tww the same) */
    /* 0x72F0 */ cXyz mHookshotRootPos;
    /* 0x72FC */ cXyz mBoomerangCatchPos;
    /* 0x7308 */ cXyz mPrevRootTrans;  /* was m3700: posMove 023FDA70: the root translation of the last frame, whose change moves Link in mRootMoveMode 1 / 5 / 8 / 9; the cut, crawl, auto-jump and door procs reset it when they start (tww the same) */
    /* 0x7314 */ cXyz m370C;
    /* 0x7320 */ cXyz mHatPrevPos;  /* was m3718: setHatAngle 024022D0: the HATA joint position of the last frame (set to the new one at the end); the hat leans away from its movement since then (tww the same) */
    /* 0x732C */ cXyz m3724;
    /* 0x7338 */ cXyz mWindSpeed;  /* was m3730: setWindAtPower 023FC7F0 eases it toward the wind push (0 in the heavy state, events and demos); posMove 023FDA70 adds its x / z to current.pos; procFanGlide 02438E78 drives the glide with it (procFanGlide_init 0241A3C4 clears it) (tww the same) */
    /* 0x7344 */ cXyz mWindAnmSpeed;  /* was m373C: setWindAtPower 023FC7F0 eases it toward the wind push like mWindSpeed but also in the heavy state (0 in events and demos); it does not move Link: setBlendMoveAnime 023E14A0 and setNeckAngle 02400C4C react to it (tww the same) */
    /* 0x7350 */ cXyz mExecStartPos;  /* was m3748: execute 0240CDD0 stores current.pos before the proc runs; procGrabReady_init 023ED0F0 / procGrabUp_init 0242D8DC measure the held actor from it (tww the same) */
    /* 0x735C */ u8 m3754[0x60];
    /* 0x73BC */ Mtx34 mInvWorldMtx;  /* was m37B4: setWorldMatrix 023FF6A0: the inverse of Link's world matrix; footBgCheck 0240005C, setLegAngle 023FFBE0 and posMoveFromFootPos 023FCB9C bring joint matrices into Link's local space with it (tww the same) */
    /* 0x73EC */ be<u32> mpSwBlur;  /* HD: daPy_swBlur_c is allocated (GameCube member at 0x37E4) */
    /* 0x73F0 */ u8 mFootData[0x230];  /* daPy_footData_c[2] (GameCube 0x3DB8, +0x3638) */
    /* 0x7620 */ dCcD_Stts mStts;
    /* 0x765C */ dCcD_Cyl mCyl;
    /* 0x778C */ dCcD_Cyl mWindCyl;
    /* 0x78BC */ dCcD_Cyl mAtCyl;
    /* 0x79EC */ dCcD_Cyl mLightCyl;
    /* 0x7B1C */ dCcD_Cps mAtCps[3];
    /* 0x7EC4 */ dCcD_Cps mFanWindCps;
    /* 0x7FFC */ dCcD_Sph mFanWindSph;
    /* 0x8128 */ dCcD_Cps mFanLightCps;
    /* 0x8260 */ u8 m8260[0x24];  /* HD-only tail */
};
WWHD_OFFSET(daPy_lk_c, mPhase, 0x43C);
WWHD_OFFSET(daPy_lk_c, mpCLModelData, 0x444);
WWHD_OFFSET(daPy_lk_c, mpCLModel, 0x448);
WWHD_OFFSET(daPy_lk_c, mpKatsuraModel, 0x44C);
WWHD_OFFSET(daPy_lk_c, mpYamuModel, 0x450);
WWHD_OFFSET(daPy_lk_c, mpCurrLinktex, 0x454);
WWHD_OFFSET(daPy_lk_c, mLinktexList, 0x458);
WWHD_OFFSET(daPy_lk_c, mpAnmTexPatternData, 0x5F4);
WWHD_OFFSET(daPy_lk_c, m_texNoAnms, 0x5F8);
WWHD_OFFSET(daPy_lk_c, mpTexScrollResData, 0x5FC);
WWHD_OFFSET(daPy_lk_c, m_texMtxAnm, 0x600);
WWHD_OFFSET(daPy_lk_c, m_tex_eye_scroll, 0x604);
WWHD_OFFSET(daPy_lk_c, mpZOffBlendShape, 0x60C);
WWHD_OFFSET(daPy_lk_c, mpZOffNoneShape, 0x61C);
WWHD_OFFSET(daPy_lk_c, mpZOnShape, 0x62C);
WWHD_OFFSET(daPy_lk_c, mpLhandShape, 0x63C);
WWHD_OFFSET(daPy_lk_c, mpRhandShape, 0x640);
WWHD_OFFSET(daPy_lk_c, mAcchCir, 0x74C);
WWHD_OFFSET(daPy_lk_c, mAcch, 0x80C);
WWHD_OFFSET(daPy_lk_c, mLinkLinChk, 0x9D0);
WWHD_OFFSET(daPy_lk_c, mRopeLinChk, 0xA3C);
WWHD_OFFSET(daPy_lk_c, mBoomerangLinChk, 0xAA8);
WWHD_OFFSET(daPy_lk_c, mGndChk, 0xB14);
WWHD_OFFSET(daPy_lk_c, mRoofChk, 0xB68);
WWHD_OFFSET(daPy_lk_c, mArrowLinChk, 0xBB4);
WWHD_OFFSET(daPy_lk_c, mMirLightLinChk, 0xC20);
WWHD_OFFSET(daPy_lk_c, mLavaGndChk, 0xC8C);
WWHD_OFFSET(daPy_lk_c, mPolyInfo, 0xCE0);
WWHD_OFFSET(daPy_lk_c, m_HIO, 0xCF0);
WWHD_OFFSET(daPy_lk_c, mpHandsModel, 0xCF4);
WWHD_OFFSET(daPy_lk_c, mpEquippedSwordModel, 0xCF8);
WWHD_OFFSET(daPy_lk_c, mpSwgripaModel, 0xCFC);
WWHD_OFFSET(daPy_lk_c, mpSwgripmsModel, 0xD00);
WWHD_OFFSET(daPy_lk_c, mSwgripmsabBckAnim, 0xD04);
WWHD_OFFSET(daPy_lk_c, m_pbCalc, 0x57F0);
WWHD_OFFSET(daPy_lk_c, mAnmRatioUnder, 0x57F8);
WWHD_OFFSET(daPy_lk_c, mAnmRatioUpper, 0x5818);
WWHD_OFFSET(daPy_lk_c, m_anm_heap_under, 0x5848);
WWHD_OFFSET(daPy_lk_c, m_anm_heap_upper, 0x5868);
WWHD_OFFSET(daPy_lk_c, mFrameCtrlUnder, 0x5898);
WWHD_OFFSET(daPy_lk_c, mFrameCtrlUpper, 0x58B8);
WWHD_OFFSET(daPy_lk_c, mJAIZelAnime, 0x64F0);
WWHD_OFFSET(daPy_lk_c, m_sanm_buffer, 0x6588);
WWHD_OFFSET(daPy_lk_c, mActorKeepEquip, 0x6590);
WWHD_OFFSET(daPy_lk_c, mActorKeepThrow, 0x6598);
WWHD_OFFSET(daPy_lk_c, mActorKeepGrab, 0x65A0);
WWHD_OFFSET(daPy_lk_c, mActorKeepRope, 0x65A8);
WWHD_OFFSET(daPy_lk_c, mpAttnActorLockOn, 0x65B0);
WWHD_OFFSET(daPy_lk_c, mpAttnActorAction, 0x65B4);
WWHD_OFFSET(daPy_lk_c, mpAttnActorA, 0x65B8);
WWHD_OFFSET(daPy_lk_c, mpAttnActorX, 0x65BC);
WWHD_OFFSET(daPy_lk_c, mpAttnActorY, 0x65C0);
WWHD_OFFSET(daPy_lk_c, mpAttnActorZ, 0x65C4);
WWHD_OFFSET(daPy_lk_c, mpAttnActorHD, 0x65C8);
WWHD_OFFSET(daPy_lk_c, m_old_fdata, 0x65CC);
WWHD_OFFSET(daPy_lk_c, m_tex_anm_heap, 0x65D0);
WWHD_OFFSET(daPy_lk_c, m_tex_scroll_heap, 0x65E0);
WWHD_OFFSET(daPy_lk_c, mCurProc, 0x65F0);
WWHD_OFFSET(daPy_lk_c, mCurProcFunc, 0x65F4);
WWHD_OFFSET(daPy_lk_c, mFootEffect, 0x65FC);
WWHD_OFFSET(daPy_lk_c, mRippleEcallBack, 0x6694);
WWHD_OFFSET(daPy_lk_c, mSwimTailEcallBack, 0x66A8);
WWHD_OFFSET(daPy_lk_c, mProcEffectEcallBack0, 0x66F8);
WWHD_OFFSET(daPy_lk_c, mProcEffectEcallBack1, 0x6704);
WWHD_OFFSET(daPy_lk_c, mSmokeEcallBack, 0x6710);
WWHD_OFFSET(daPy_lk_c, mCutTurnEcallBack0, 0x6730);
WWHD_OFFSET(daPy_lk_c, mCutTurnEcallBack1, 0x6740);
WWHD_OFFSET(daPy_lk_c, mCutTurnEcallBack2, 0x6750);
WWHD_OFFSET(daPy_lk_c, mNeckWaterDropEcallBack, 0x6760);
WWHD_OFFSET(daPy_lk_c, mBodyWaterDropEcallBack, 0x6780);
WWHD_OFFSET(daPy_lk_c, mTalismanFlashEcallBack, 0x67A0);
WWHD_OFFSET(daPy_lk_c, mProcEffectEcallBack2, 0x67BC);
WWHD_OFFSET(daPy_lk_c, mDmEcallBack, 0x67CC);
WWHD_OFFSET(daPy_lk_c, mFanWindEcallBack, 0x67FC);
WWHD_OFFSET(daPy_lk_c, mFanSwingCb, 0x6808);
WWHD_OFFSET(daPy_lk_c, mWaterfallSplashEcallBack, 0x6814);
WWHD_OFFSET(daPy_lk_c, mWadeSplashEcallBack, 0x6824);
WWHD_OFFSET(daPy_lk_c, mMirrorShieldEcallBack, 0x6840);
WWHD_OFFSET(daPy_lk_c, mShieldLightHitEcallBack, 0x684C);
WWHD_OFFSET(daPy_lk_c, mLightSaverEcallBack, 0x6868);
WWHD_OFFSET(daPy_lk_c, mRainDropEcallBack, 0x6874);
WWHD_OFFSET(daPy_lk_c, mpAttention, 0x6894);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryA, 0x6898);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryX, 0x689C);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryY, 0x68A0);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryZ, 0x68A4);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryHD, 0x68A8);
WWHD_OFFSET(daPy_lk_c, mTactZevEventName, 0x68AC);
WWHD_OFFSET(daPy_lk_c, mLightInfluence, 0x68B0);
WWHD_OFFSET(daPy_lk_c, m68D0, 0x68D0);
WWHD_OFFSET(daPy_lk_c, mDirection, 0x68D4);
WWHD_OFFSET(daPy_lk_c, mFrontWallType, 0x68D5);
WWHD_OFFSET(daPy_lk_c, m34BA, 0x68D6);
WWHD_OFFSET(daPy_lk_c, mCurrItemHeapIdx, 0x68D7);
WWHD_OFFSET(daPy_lk_c, mBaseFootIdx, 0x68D8);
WWHD_OFFSET(daPy_lk_c, mReadyItemBtn, 0x68D9);
WWHD_OFFSET(daPy_lk_c, mFootEffectPosType, 0x68DA);
WWHD_OFFSET(daPy_lk_c, mReverb, 0x68DB);
WWHD_OFFSET(daPy_lk_c, mLeftHandIdx, 0x68DC);
WWHD_OFFSET(daPy_lk_c, mRightHandIdx, 0x68DD);
WWHD_OFFSET(daPy_lk_c, mRootMoveMode, 0x68DE);
WWHD_OFFSET(daPy_lk_c, m34C3, 0x68DF);
WWHD_OFFSET(daPy_lk_c, mCutComboCount, 0x68E0);
WWHD_OFFSET(daPy_lk_c, m34C5, 0x68E1);
WWHD_OFFSET(daPy_lk_c, mActivePlayerBombs, 0x690C);
WWHD_OFFSET(daPy_lk_c, mItemTrigger, 0x690D);
WWHD_OFFSET(daPy_lk_c, mItemButton, 0x690E);
WWHD_OFFSET(daPy_lk_c, mOldItemButton, 0x690F);
WWHD_OFFSET(daPy_lk_c, mDekuSpRestartPoint, 0x6910);
WWHD_OFFSET(daPy_lk_c, mTactZevMelodyNum, 0x6911);
WWHD_OFFSET(daPy_lk_c, mWaterfallRippleTimer, 0x6912);
WWHD_OFFSET(daPy_lk_c, mGetDemoFlg, 0x6913);
WWHD_OFFSET(daPy_lk_c, mProcVar0, 0x6916);
WWHD_OFFSET(daPy_lk_c, mProcVar1, 0x6918);
WWHD_OFFSET(daPy_lk_c, mProcVar2, 0x691A);
WWHD_OFFSET(daPy_lk_c, mProcVar3, 0x691C);
WWHD_OFFSET(daPy_lk_c, mProcVar4, 0x691E);
WWHD_OFFSET(daPy_lk_c, mProcVar5, 0x6920);
WWHD_OFFSET(daPy_lk_c, mStickAngle, 0x6924);
WWHD_OFFSET(daPy_lk_c, m34DE, 0x6926);
WWHD_OFFSET(daPy_lk_c, mWaistAngleZ, 0x6928);
WWHD_OFFSET(daPy_lk_c, mGroundAngle, 0x692A);
WWHD_OFFSET(daPy_lk_c, mWaistAngleY, 0x692C);
WWHD_OFFSET(daPy_lk_c, mAtnShapeAngleY, 0x692E);
WWHD_OFFSET(daPy_lk_c, mMoveTargetAngleY, 0x6930);
WWHD_OFFSET(daPy_lk_c, mOldStickAngle, 0x6932);
WWHD_OFFSET(daPy_lk_c, m34EC, 0x6934);
WWHD_OFFSET(daPy_lk_c, mpSeAnm, 0x6938);
WWHD_OFFSET(daPy_lk_c, m693C, 0x693C);
WWHD_OFFSET(daPy_lk_c, mSeAnmDemoAnmNo, 0x6940);
WWHD_OFFSET(daPy_lk_c, m34F2, 0x6942);
WWHD_OFFSET(daPy_lk_c, m34F4, 0x6944);
WWHD_OFFSET(daPy_lk_c, mHatAAngleZ, 0x6946);
WWHD_OFFSET(daPy_lk_c, mHatAAngleY, 0x6948);
WWHD_OFFSET(daPy_lk_c, mHatBAngleZ, 0x694A);
WWHD_OFFSET(daPy_lk_c, mHatBAngleY, 0x694C);
WWHD_OFFSET(daPy_lk_c, mHatCAngleZ, 0x694E);
WWHD_OFFSET(daPy_lk_c, mHatCAngleY, 0x6950);
WWHD_OFFSET(daPy_lk_c, mHatASpeedZ, 0x6952);
WWHD_OFFSET(daPy_lk_c, mHatASpeedY, 0x6954);
WWHD_OFFSET(daPy_lk_c, mHatBSpeedZ, 0x6956);
WWHD_OFFSET(daPy_lk_c, mHatBSpeedY, 0x6958);
WWHD_OFFSET(daPy_lk_c, mHatCSpeedZ, 0x695A);
WWHD_OFFSET(daPy_lk_c, mHatCSpeedY, 0x695C);
WWHD_OFFSET(daPy_lk_c, mHatASwingZ, 0x695E);
WWHD_OFFSET(daPy_lk_c, mHatBSwingZ, 0x6960);
WWHD_OFFSET(daPy_lk_c, mHatCSwingZ, 0x6962);
WWHD_OFFSET(daPy_lk_c, mHatSwingPhase, 0x6964);
WWHD_OFFSET(daPy_lk_c, mLMomiAngleY, 0x6966);
WWHD_OFFSET(daPy_lk_c, mRMomiAngleY, 0x6968);
WWHD_OFFSET(daPy_lk_c, mMomiAngleZ, 0x696A);
WWHD_OFFSET(daPy_lk_c, mMoveSlantAngle, 0x696C);
WWHD_OFFSET(daPy_lk_c, m351E, 0x696E);
WWHD_OFFSET(daPy_lk_c, mCutComboTimer, 0x6972);
WWHD_OFFSET(daPy_lk_c, mStickSpinTimer, 0x6974);
WWHD_OFFSET(daPy_lk_c, mSlideStartTimer, 0x6976);
WWHD_OFFSET(daPy_lk_c, mHeadFrontAngleX, 0x6978);
WWHD_OFFSET(daPy_lk_c, mHeadFrontAngleY, 0x697A);
WWHD_OFFSET(daPy_lk_c, m352C, 0x697C);
WWHD_OFFSET(daPy_lk_c, mBgWaitTimer, 0x697E);
WWHD_OFFSET(daPy_lk_c, mTexAnmFrame, 0x6980);
WWHD_OFFSET(daPy_lk_c, mTexScrollFrame, 0x6982);
WWHD_OFFSET(daPy_lk_c, mFanWindTimer, 0x6984);
WWHD_OFFSET(daPy_lk_c, mFanWindAngleY, 0x6986);
WWHD_OFFSET(daPy_lk_c, mFanWindAngleX, 0x6988);
WWHD_OFFSET(daPy_lk_c, mFanWindSphTimer, 0x698A);
WWHD_OFFSET(daPy_lk_c, mShipRideAngleX, 0x698C);
WWHD_OFFSET(daPy_lk_c, mShipRideAngleZ, 0x698E);
WWHD_OFFSET(daPy_lk_c, mShipRideAngleXSpeed, 0x6990);
WWHD_OFFSET(daPy_lk_c, mShipRideAngleZSpeed, 0x6992);
WWHD_OFFSET(daPy_lk_c, mFrontWallTimer, 0x6994);
WWHD_OFFSET(daPy_lk_c, mShieldFrontRangeYAngle, 0x6996);
WWHD_OFFSET(daPy_lk_c, m3548, 0x6998);
WWHD_OFFSET(daPy_lk_c, mTinkleHoverTimer, 0x699C);
WWHD_OFFSET(daPy_lk_c, mTinkleShieldTimer, 0x699E);
WWHD_OFFSET(daPy_lk_c, m3550, 0x69A0);
WWHD_OFFSET(daPy_lk_c, mKeepItem, 0x69A2);
WWHD_OFFSET(daPy_lk_c, mForestWaterUseTimer, 0x69A4);
WWHD_OFFSET(daPy_lk_c, mFanSmaAngleZ, 0x69A8);
WWHD_OFFSET(daPy_lk_c, mFanSmbAngleZ, 0x69AA);
WWHD_OFFSET(daPy_lk_c, m355C, 0x69AC);
WWHD_OFFSET(daPy_lk_c, m355E, 0x69AE);
WWHD_OFFSET(daPy_lk_c, mEquipItem, 0x69B0);
WWHD_OFFSET(daPy_lk_c, mAnimeEquipItem, 0x69B2);
WWHD_OFFSET(daPy_lk_c, mNeckAngle, 0x69B4);
WWHD_OFFSET(daPy_lk_c, mCameraInfoIdx, 0x69BC);
WWHD_OFFSET(daPy_lk_c, mProcVar6, 0x69C0);
WWHD_OFFSET(daPy_lk_c, mProcVar7, 0x69C4);
WWHD_OFFSET(daPy_lk_c, mStickSpinAngle, 0x69C8);
WWHD_OFFSET(daPy_lk_c, mOldGroundCode, 0x69CC);
WWHD_OFFSET(daPy_lk_c, mGroundCode, 0x69D0);
WWHD_OFFSET(daPy_lk_c, mCurrAttributeCode, 0x69D4);
WWHD_OFFSET(daPy_lk_c, mHangAttributeCode, 0x69D8);
WWHD_OFFSET(daPy_lk_c, mStaffIdx, 0x69DC);
WWHD_OFFSET(daPy_lk_c, mEventIdx, 0x69E0);
WWHD_OFFSET(daPy_lk_c, mRestartPoint, 0x69E4);
WWHD_OFFSET(daPy_lk_c, mStrongJumpCut, 0x69E8);
WWHD_OFFSET(daPy_lk_c, mHeavyStateCounter, 0x69EE);
WWHD_OFFSET(daPy_lk_c, m3598, 0x69F0);
WWHD_OFFSET(daPy_lk_c, mFootMoveDist, 0x69F4);
WWHD_OFFSET(daPy_lk_c, m35A0, 0x69F8);
WWHD_OFFSET(daPy_lk_c, m35A4, 0x69FC);
WWHD_OFFSET(daPy_lk_c, m35A8, 0x6A00);
WWHD_OFFSET(daPy_lk_c, m35AC, 0x6A04);
WWHD_OFFSET(daPy_lk_c, mStickDistance, 0x6A08);
WWHD_OFFSET(daPy_lk_c, mOldStickDistance, 0x6A0C);
WWHD_OFFSET(daPy_lk_c, mFootBgOffsetY, 0x6A10);
WWHD_OFFSET(daPy_lk_c, mNormalSpeed, 0x6A14);
WWHD_OFFSET(daPy_lk_c, m35C4, 0x6A1C);
WWHD_OFFSET(daPy_lk_c, m35C8, 0x6A20);
WWHD_OFFSET(daPy_lk_c, mGrabItemBounceY, 0x6A24);
WWHD_OFFSET(daPy_lk_c, mWaterY, 0x6A28);
WWHD_OFFSET(daPy_lk_c, m35D4, 0x6A2C);
WWHD_OFFSET(daPy_lk_c, m35D8, 0x6A30);
WWHD_OFFSET(daPy_lk_c, mHangGroundH, 0x6A34);
WWHD_OFFSET(daPy_lk_c, mRootTransY, 0x6A38);
WWHD_OFFSET(daPy_lk_c, m35E4, 0x6A3C);
WWHD_OFFSET(daPy_lk_c, m35E8, 0x6A40);
WWHD_OFFSET(daPy_lk_c, m35EC, 0x6A44);
WWHD_OFFSET(daPy_lk_c, mFallStartY, 0x6A48);
WWHD_OFFSET(daPy_lk_c, m35F4, 0x6A4C);
WWHD_OFFSET(daPy_lk_c, mLadderTopY, 0x6A50);
WWHD_OFFSET(daPy_lk_c, mSwordAtScale, 0x6A54);
WWHD_OFFSET(daPy_lk_c, m3600, 0x6A58);
WWHD_OFFSET(daPy_lk_c, m3604, 0x6A5C);
WWHD_OFFSET(daPy_lk_c, mSwimTiredOffsetY, 0x6A60);
WWHD_OFFSET(daPy_lk_c, mSeAnmRate, 0x6A64);
WWHD_OFFSET(daPy_lk_c, mWhirlPullSpeed, 0x6A68);
WWHD_OFFSET(daPy_lk_c, mShadowId, 0x6A6C);
WWHD_OFFSET(daPy_lk_c, mModeFlg, 0x6A70);
WWHD_OFFSET(daPy_lk_c, mMtrlSndId, 0x6A74);
WWHD_OFFSET(daPy_lk_c, mFrollCrashMtrlSndId, 0x6A78);
WWHD_OFFSET(daPy_lk_c, m3624, 0x6A7C);
WWHD_OFFSET(daPy_lk_c, mGameOverId, 0x6A80);
WWHD_OFFSET(daPy_lk_c, mTactZevPartnerId, 0x6A84);
WWHD_OFFSET(daPy_lk_c, m3630, 0x6A88);
WWHD_OFFSET(daPy_lk_c, mWhirlId, 0x6A8C);
WWHD_OFFSET(daPy_lk_c, mMsgId, 0x6A90);
WWHD_OFFSET(daPy_lk_c, mpSeAnmFrameCtrl, 0x6A94);
WWHD_OFFSET(daPy_lk_c, mOutPowerAngle, 0x6A98);
WWHD_OFFSET(daPy_lk_c, mOutPower, 0x6A9C);
WWHD_OFFSET(daPy_lk_c, mRootJointQuat, 0x6AA0);
WWHD_OFFSET(daPy_lk_c, mJointState, 0x6AB0);
WWHD_OFFSET(daPy_lk_c, mProcStartPos, 0x7290);
WWHD_OFFSET(daPy_lk_c, mOldSpeed, 0x729C);
WWHD_OFFSET(daPy_lk_c, mSlipSpeed, 0x72A8);
WWHD_OFFSET(daPy_lk_c, mOldSlipSpeed, 0x72B4);
WWHD_OFFSET(daPy_lk_c, mBeltConveyerSpeed, 0x72C0);
WWHD_OFFSET(daPy_lk_c, mSwordRootPos, 0x72CC);
WWHD_OFFSET(daPy_lk_c, mOldSwordTopPos, 0x72D8);
WWHD_OFFSET(daPy_lk_c, mOldSwordRootPos, 0x72E4);
WWHD_OFFSET(daPy_lk_c, mHookshotRootPos, 0x72F0);
WWHD_OFFSET(daPy_lk_c, mBoomerangCatchPos, 0x72FC);
WWHD_OFFSET(daPy_lk_c, mPrevRootTrans, 0x7308);
WWHD_OFFSET(daPy_lk_c, m370C, 0x7314);
WWHD_OFFSET(daPy_lk_c, mHatPrevPos, 0x7320);
WWHD_OFFSET(daPy_lk_c, m3724, 0x732C);
WWHD_OFFSET(daPy_lk_c, mWindSpeed, 0x7338);
WWHD_OFFSET(daPy_lk_c, mWindAnmSpeed, 0x7344);
WWHD_OFFSET(daPy_lk_c, mExecStartPos, 0x7350);
WWHD_OFFSET(daPy_lk_c, mInvWorldMtx, 0x73BC);
WWHD_OFFSET(daPy_lk_c, mpSwBlur, 0x73EC);
WWHD_OFFSET(daPy_lk_c, mFootData, 0x73F0);
WWHD_OFFSET(daPy_lk_c, mStts, 0x7620);
WWHD_OFFSET(daPy_lk_c, mCyl, 0x765C);
WWHD_OFFSET(daPy_lk_c, mWindCyl, 0x778C);
WWHD_OFFSET(daPy_lk_c, mAtCyl, 0x78BC);
WWHD_OFFSET(daPy_lk_c, mLightCyl, 0x79EC);
WWHD_OFFSET(daPy_lk_c, mAtCps, 0x7B1C);
WWHD_OFFSET(daPy_lk_c, mFanWindCps, 0x7EC4);
WWHD_OFFSET(daPy_lk_c, mFanWindSph, 0x7FFC);
WWHD_OFFSET(daPy_lk_c, mFanLightCps, 0x8128);
WWHD_SIZE(daPy_lk_c, 0x8284);
