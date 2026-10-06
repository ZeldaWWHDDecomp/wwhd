/* daPy_lk_c (Link), WWHD layout. 
 *
 * Measured for the WWHD build (cking.rpx) from the constructor daPy_lk_c::daPy_lk_c 0240FE40 (it
 * allocates 0x8284 bytes; GameCube 0x4C28), from setAtnList 023DCD30 / resetSeAnime 023E07C0 /
 * small accessors, and from a statistical match of the GameCube members used by each function
 * against the this-relative offsets of the WWHD function of the same name (scratch notes in the
 * phase-1 report). GameCube -> WWHD:
 * - daPy_py_c base: 0x43C (d/actor/d_a_player.h; the GameCube daPy_py_c fields are +0x11C).
 * - 0x320..0x35C (+0x11C); HD replaced the GameCube ResTIMG mOtherLinktex by a 0x34 object and
 *   added ten 0x24 objects at 0x48C; 0x35C..0x3AC (+0x298); three HD 0x58 animation objects at
 *   0x644..0x74C.
 * - 0x3AC..0x974 (+0x3A0): background checks, poly info, hand/sword models, mSwgripmsabBckAnim.
 * - 0x974..0x2FAC: re-laid out in HD (J3DAnm pointers became mDoExt_brk/btkAnm objects, the
 *   mirror packet grew); only constructor anchors are recorded (see mD90).
 * - 0x2FAC..0x3178: animation blending and the sight packet (HD 0xC08, GameCube 0x50).
 * - 0x3178..0x34B8 (+0x3410/+0x3414/+0x3418): HD adds a fourth item-button attention actor
 *   (0x65C8) and list entry (0x68A8) and a float at 0x68D0; mCurProcFunc is an 8-byte GHS pointer
 *   to member.
 * - 0x34B8..0x34C6 (+0x341C); 0x34C7..0x34CF (+0x3445); mProcVar0..5 (+0x3446); 0x34DC.. (+0x3448);
 *   mSeAnmIdx became a pointer (0x6938); 0x34F0..0x3598 (+0x3450); 0x3598..0x3688 (+0x3458);
 *   0x3688..0x37E4 (+0x3C08, after 0x7B0 HD-only bytes at 0x6AE0); daPy_swBlur_c is allocated
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
    /* 0x0458 */ u8 m458[0x34];  /* HD: an object (0x34, constructor 0273B560 with 10 entries of 0xC) in place of the GameCube ResTIMG mOtherLinktex */
    /* 0x048C */ u8 m48C[0x168];  /* HD: 10 objects of 0x24 (constructor 02444280) */
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
    /* 0x0644 */ u8 m644[0x108];  /* HD: three 0x58 animation objects (constructor 027DA984, vtables 0x1016D860..) at 0x644/0x69C/0x6F4 */
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
    /* 0x6694 */ u8 m3280[0x14];  /* dPa_rippleEcallBack */
    /* 0x66A8 */ u8 mSwimTailEcallBack[0x50];  /* daPy_swimTailEcallBack_c[2] */
    /* 0x66F8 */ daPy_mtxFollowEcallBack_c m32E4;
    /* 0x6704 */ daPy_mtxFollowEcallBack_c m32F0;
    /* 0x6710 */ u8 mSmokeEcallBack[0x20];  /* dPa_smokeEcallBack */
    /* 0x6730 */ u8 m331C[0x10];  /* dPa_cutTurnEcallBack_c */
    /* 0x6740 */ u8 m332C[0x10];
    /* 0x6750 */ u8 m333C[0x10];
    /* 0x6760 */ u8 m334C[0x20];  /* daPy_waterDropEcallBack_c */
    /* 0x6780 */ u8 m336C[0x20];
    /* 0x67A0 */ u8 m338C[0x1C];  /* daPy_followEcallBack_c */
    /* 0x67BC */ u8 m33A8[0x10];  /* daPy_mtxPosFollowEcallBack_c */
    /* 0x67CC */ u8 mDmEcallBack[0x30];  /* daPy_dmEcallBack_c[4] */
    /* 0x67FC */ daPy_mtxFollowEcallBack_c m33E8;
    /* 0x6808 */ u8 mFanSwingCb[0xC];  /* daPy_fanSwingEcallBack_c */
    /* 0x6814 */ u8 m3400[0x10];  /* daPy_mtxPosFollowEcallBack_c */
    /* 0x6824 */ u8 m3410[0x1C];  /* daPy_followEcallBack_c */
    /* 0x6840 */ daPy_mtxFollowEcallBack_c m342C;
    /* 0x684C */ u8 m3438[0x1C];  /* daPy_followEcallBack_c */
    /* 0x6868 */ daPy_mtxFollowEcallBack_c m3454;
    /* 0x6874 */ u8 m3460[0x20];  /* daPy_mtxPosFollowEcallBack_c[2] */
    /* 0x6894 */ be<u32> mpAttention;  /* dAttention_c* (play + 0x5804) */
    /* 0x6898 */ be<u32> mpAttnEntryA;  /* dAttList_c* */
    /* 0x689C */ be<u32> mpAttnEntryX;
    /* 0x68A0 */ be<u32> mpAttnEntryY;
    /* 0x68A4 */ be<u32> mpAttnEntryZ;
    /* 0x68A8 */ be<u32> mpAttnEntryHD;  /* HD-only: the fourth item button entry (+4 from here) */
    /* 0x68AC */ be<u32> m3494;  /* GameCube 0x3494 (+0x3418) */
    /* 0x68B0 */ u8 mLightInfluence[0x20];  /* LIGHT_INFLUENCE */
    /* 0x68D0 */ be<f32> m68D0;  /* HD-only float (set by the constructor) */
    /* 0x68D4 */ be<u8> mDirection;  /* GameCube 0x34B8 (+0x341C) */
    /* 0x68D5 */ be<u8> mFrontWallType;
    /* 0x68D6 */ be<u8> m34BA;
    /* 0x68D7 */ be<u8> mCurrItemHeapIdx;
    /* 0x68D8 */ be<u8> m34BC;
    /* 0x68D9 */ be<u8> mReadyItemBtn;
    /* 0x68DA */ be<u8> mFootEffectPosType;
    /* 0x68DB */ be<s8> mReverb;
    /* 0x68DC */ be<u8> mLeftHandIdx;
    /* 0x68DD */ be<u8> mRightHandIdx;
    /* 0x68DE */ be<u8> m34C2;
    /* 0x68DF */ be<u8> m34C3;
    /* 0x68E0 */ be<u8> m34C4;
    /* 0x68E1 */ be<u8> m34C5;
    /* 0x68E2 */ u8 m68E2[0x2A];  /* HD: m34C6 (GameCube 0x34C6) and HD-only bytes, not measured */
    /* 0x690C */ be<u8> mActivePlayerBombs;  /* GameCube 0x34C7 (+0x3445) */
    /* 0x690D */ be<u8> mItemTrigger;
    /* 0x690E */ be<u8> mItemButton;
    /* 0x690F */ be<u8> m34CA;
    /* 0x6910 */ be<u8> mDekuSpRestartPoint;
    /* 0x6911 */ be<u8> m34CC;
    /* 0x6912 */ be<u8> m34CD;
    /* 0x6913 */ be<u8> m34CE;
    /* 0x6914 */ u8 _6914[2];  /* [?] */
    /* 0x6916 */ be<s16> mProcVar0;  /* GameCube 0x34D0 (+0x3446) */
    /* 0x6918 */ be<s16> mProcVar1;
    /* 0x691A */ be<s16> mProcVar2;
    /* 0x691C */ be<s16> mProcVar3;
    /* 0x691E */ be<s16> mProcVar4;
    /* 0x6920 */ be<s16> mProcVar5;
    /* 0x6922 */ u8 _6922[2];  /* HD-only [?] */
    /* 0x6924 */ be<s16> m34DC;  /* GameCube 0x34DC (+0x3448) */
    /* 0x6926 */ be<s16> m34DE;
    /* 0x6928 */ be<s16> m34E0;
    /* 0x692A */ be<s16> m34E2;
    /* 0x692C */ be<s16> m34E4;
    /* 0x692E */ be<s16> m34E6;
    /* 0x6930 */ be<s16> m34E8;
    /* 0x6932 */ be<s16> m34EA;
    /* 0x6934 */ be<s16> m34EC;
    /* 0x6936 */ u8 _6936[2];
    /* 0x6938 */ be<u32> mpSeAnm;  /* HD: a pointer to the SE animation data (GameCube u16 mSeAnmIdx at 0x34EE); resetSeAnime stores 0x10035574 */
    /* 0x693C */ be<f32> m693C;  /* [?] probably mSeAnmRate (GameCube 0x360C) */
    /* 0x6940 */ be<u16> m34F0;  /* GameCube 0x34F0 (+0x3450) */
    /* 0x6942 */ be<s16> m34F2;
    /* 0x6944 */ be<s16> m34F4;
    /* 0x6946 */ be<s16> m34F6;
    /* 0x6948 */ be<s16> m34F8;
    /* 0x694A */ be<s16> m34FA;
    /* 0x694C */ be<s16> m34FC;
    /* 0x694E */ be<s16> m34FE;
    /* 0x6950 */ be<s16> m3500;
    /* 0x6952 */ be<s16> m3502;
    /* 0x6954 */ be<s16> m3504;
    /* 0x6956 */ be<s16> m3506;
    /* 0x6958 */ be<s16> m3508;
    /* 0x695A */ be<s16> m350A;
    /* 0x695C */ be<s16> m350C;
    /* 0x695E */ be<s16> m350E;
    /* 0x6960 */ be<s16> m3510;
    /* 0x6962 */ be<s16> m3512;
    /* 0x6964 */ be<s16> m3514;
    /* 0x6966 */ be<s16> m3516;
    /* 0x6968 */ be<s16> m3518;
    /* 0x696A */ be<s16> m351A;
    /* 0x696C */ be<s16> m351C;
    /* 0x696E */ be<s16> m351E;
    /* 0x6970 */ u8 m3520[2];
    /* 0x6972 */ be<s16> m3522;
    /* 0x6974 */ be<s16> m3524;
    /* 0x6976 */ be<s16> m3526;
    /* 0x6978 */ be<s16> m3528;
    /* 0x697A */ be<s16> m352A;
    /* 0x697C */ be<s16> m352C;
    /* 0x697E */ be<s16> m352E;
    /* 0x6980 */ be<u16> m3530;
    /* 0x6982 */ be<u16> m3532;
    /* 0x6984 */ be<s16> m3534;
    /* 0x6986 */ be<s16> m3536;
    /* 0x6988 */ be<s16> m3538;
    /* 0x698A */ be<s16> m353A;
    /* 0x698C */ be<s16> m353C;
    /* 0x698E */ be<s16> m353E;
    /* 0x6990 */ be<s16> m3540;
    /* 0x6992 */ be<s16> m3542;
    /* 0x6994 */ be<s16> m3544;
    /* 0x6996 */ be<s16> mShieldFrontRangeYAngle;
    /* 0x6998 */ be<s16> m3548;
    /* 0x699A */ u8 m354A[2];
    /* 0x699C */ be<s16> mTinkleHoverTimer;
    /* 0x699E */ be<s16> mTinkleShieldTimer;
    /* 0x69A0 */ be<s16> m3550;
    /* 0x69A2 */ be<u16> mKeepItem;
    /* 0x69A4 */ be<s16> m3554;
    /* 0x69A6 */ u8 m3556[2];
    /* 0x69A8 */ be<s16> m3558;
    /* 0x69AA */ be<s16> m355A;
    /* 0x69AC */ be<s16> m355C;
    /* 0x69AE */ be<s16> m355E;
    /* 0x69B0 */ be<u16> mEquipItem;
    /* 0x69B2 */ be<u16> m3562;
    /* 0x69B4 */ csXyz m3564;
    /* 0x69BA */ u8 _69BA[2];
    /* 0x69BC */ be<s32> mCameraInfoIdx;  /* GameCube 0x356C (+0x3450) */
    /* 0x69C0 */ be<s32> mProcVar6;
    /* 0x69C4 */ be<s32> mProcVar7;
    /* 0x69C8 */ be<s32> m3578;
    /* 0x69CC */ be<s32> m357C;
    /* 0x69D0 */ be<s32> m3580;
    /* 0x69D4 */ be<s32> mCurrAttributeCode;
    /* 0x69D8 */ be<s32> m3588;
    /* 0x69DC */ be<s32> mStaffIdx;
    /* 0x69E0 */ be<s32> mEventIdx;
    /* 0x69E4 */ be<s32> mRestartPoint;
    /* 0x69E8 */ u8 m69E8[8];  /* HD-only [?] (+0x3458 from here) */
    /* 0x69F0 */ be<f32> m3598;  /* GameCube 0x3598 (+0x3458) */
    /* 0x69F4 */ be<f32> m359C;
    /* 0x69F8 */ be<f32> m35A0;
    /* 0x69FC */ be<f32> m35A4;
    /* 0x6A00 */ be<f32> m35A8;
    /* 0x6A04 */ be<f32> m35AC;
    /* 0x6A08 */ be<f32> mStickDistance;
    /* 0x6A0C */ be<f32> m35B4;
    /* 0x6A10 */ be<f32> m35B8;
    /* 0x6A14 */ be<f32> mNormalSpeed;
    /* 0x6A18 */ u8 m35C0[4];
    /* 0x6A1C */ be<f32> m35C4;
    /* 0x6A20 */ be<f32> m35C8;
    /* 0x6A24 */ be<f32> m35CC;
    /* 0x6A28 */ be<f32> mWaterY;
    /* 0x6A2C */ be<f32> m35D4;
    /* 0x6A30 */ be<f32> m35D8;
    /* 0x6A34 */ be<f32> mHangGroundH;
    /* 0x6A38 */ be<f32> m35E0;
    /* 0x6A3C */ be<f32> m35E4;
    /* 0x6A40 */ be<f32> m35E8;
    /* 0x6A44 */ be<f32> m35EC;
    /* 0x6A48 */ be<f32> m35F0;
    /* 0x6A4C */ be<f32> m35F4;
    /* 0x6A50 */ be<f32> m35F8;
    /* 0x6A54 */ be<f32> m35FC;
    /* 0x6A58 */ be<f32> m3600;
    /* 0x6A5C */ be<f32> m3604;
    /* 0x6A60 */ be<f32> m3608;
    /* 0x6A64 */ be<f32> mSeAnmRate;  /* [?] see m693C */
    /* 0x6A68 */ be<f32> m3610;
    /* 0x6A6C */ be<u32> mShadowId;  /* GameCube 0x3614 (+0x3458); probably unused in HD (no shadow ids) */
    /* 0x6A70 */ be<u32> mModeFlg;
    /* 0x6A74 */ be<u32> mMtrlSndId;
    /* 0x6A78 */ be<u32> m3620;
    /* 0x6A7C */ be<u32> m3624;
    /* 0x6A80 */ be<u32> mGameOverId;
    /* 0x6A84 */ be<u32> mTactZevPartnerId;
    /* 0x6A88 */ be<u32> m3630;
    /* 0x6A8C */ be<u32> mWhirlId;
    /* 0x6A90 */ be<u32> mMsgId;
    /* 0x6A94 */ be<u32> mpSeAnmFrameCtrl;
    /* 0x6A98 */ be<s16> m3640;
    /* 0x6A9A */ u8 _6A9A[2];
    /* 0x6A9C */ be<f32> m3644;
    /* 0x6AA0 */ u8 m3648[0x10];  /* Quaternion */
    /* 0x6AB0 */ u8 m3658[0x10];  /* Quaternion */
    /* 0x6AC0 */ u8 m3668[0x20];  /* J3DTransformInfo */
    /* 0x6AE0 */ u8 m6AE0[0x7B0];  /* HD-only (0x7B0 bytes, not initialised by the constructor; probably joint matrices) */
    /* 0x7290 */ cXyz m3688;  /* GameCube 0x3688 (+0x3C08) */
    /* 0x729C */ cXyz mOldSpeed;
    /* 0x72A8 */ cXyz m36A0;
    /* 0x72B4 */ cXyz m36AC;
    /* 0x72C0 */ cXyz m36B8;
    /* 0x72CC */ cXyz m36C4;
    /* 0x72D8 */ cXyz m36D0;
    /* 0x72E4 */ cXyz m36DC;
    /* 0x72F0 */ cXyz mHookshotRootPos;
    /* 0x72FC */ cXyz mBoomerangCatchPos;
    /* 0x7308 */ cXyz m3700;
    /* 0x7314 */ cXyz m370C;
    /* 0x7320 */ cXyz m3718;
    /* 0x732C */ cXyz m3724;
    /* 0x7338 */ cXyz m3730;
    /* 0x7344 */ cXyz m373C;
    /* 0x7350 */ cXyz m3748;
    /* 0x735C */ u8 m3754[0x60];
    /* 0x73BC */ Mtx34 m37B4;
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
WWHD_OFFSET(daPy_lk_c, m458, 0x458);
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
WWHD_OFFSET(daPy_lk_c, m3280, 0x6694);
WWHD_OFFSET(daPy_lk_c, mSwimTailEcallBack, 0x66A8);
WWHD_OFFSET(daPy_lk_c, m32E4, 0x66F8);
WWHD_OFFSET(daPy_lk_c, m32F0, 0x6704);
WWHD_OFFSET(daPy_lk_c, mSmokeEcallBack, 0x6710);
WWHD_OFFSET(daPy_lk_c, m331C, 0x6730);
WWHD_OFFSET(daPy_lk_c, m332C, 0x6740);
WWHD_OFFSET(daPy_lk_c, m333C, 0x6750);
WWHD_OFFSET(daPy_lk_c, m334C, 0x6760);
WWHD_OFFSET(daPy_lk_c, m336C, 0x6780);
WWHD_OFFSET(daPy_lk_c, m338C, 0x67A0);
WWHD_OFFSET(daPy_lk_c, m33A8, 0x67BC);
WWHD_OFFSET(daPy_lk_c, mDmEcallBack, 0x67CC);
WWHD_OFFSET(daPy_lk_c, m33E8, 0x67FC);
WWHD_OFFSET(daPy_lk_c, mFanSwingCb, 0x6808);
WWHD_OFFSET(daPy_lk_c, m3400, 0x6814);
WWHD_OFFSET(daPy_lk_c, m3410, 0x6824);
WWHD_OFFSET(daPy_lk_c, m342C, 0x6840);
WWHD_OFFSET(daPy_lk_c, m3438, 0x684C);
WWHD_OFFSET(daPy_lk_c, m3454, 0x6868);
WWHD_OFFSET(daPy_lk_c, m3460, 0x6874);
WWHD_OFFSET(daPy_lk_c, mpAttention, 0x6894);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryA, 0x6898);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryX, 0x689C);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryY, 0x68A0);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryZ, 0x68A4);
WWHD_OFFSET(daPy_lk_c, mpAttnEntryHD, 0x68A8);
WWHD_OFFSET(daPy_lk_c, m3494, 0x68AC);
WWHD_OFFSET(daPy_lk_c, mLightInfluence, 0x68B0);
WWHD_OFFSET(daPy_lk_c, m68D0, 0x68D0);
WWHD_OFFSET(daPy_lk_c, mDirection, 0x68D4);
WWHD_OFFSET(daPy_lk_c, mFrontWallType, 0x68D5);
WWHD_OFFSET(daPy_lk_c, m34BA, 0x68D6);
WWHD_OFFSET(daPy_lk_c, mCurrItemHeapIdx, 0x68D7);
WWHD_OFFSET(daPy_lk_c, m34BC, 0x68D8);
WWHD_OFFSET(daPy_lk_c, mReadyItemBtn, 0x68D9);
WWHD_OFFSET(daPy_lk_c, mFootEffectPosType, 0x68DA);
WWHD_OFFSET(daPy_lk_c, mReverb, 0x68DB);
WWHD_OFFSET(daPy_lk_c, mLeftHandIdx, 0x68DC);
WWHD_OFFSET(daPy_lk_c, mRightHandIdx, 0x68DD);
WWHD_OFFSET(daPy_lk_c, m34C2, 0x68DE);
WWHD_OFFSET(daPy_lk_c, m34C3, 0x68DF);
WWHD_OFFSET(daPy_lk_c, m34C4, 0x68E0);
WWHD_OFFSET(daPy_lk_c, m34C5, 0x68E1);
WWHD_OFFSET(daPy_lk_c, mActivePlayerBombs, 0x690C);
WWHD_OFFSET(daPy_lk_c, mItemTrigger, 0x690D);
WWHD_OFFSET(daPy_lk_c, mItemButton, 0x690E);
WWHD_OFFSET(daPy_lk_c, m34CA, 0x690F);
WWHD_OFFSET(daPy_lk_c, mDekuSpRestartPoint, 0x6910);
WWHD_OFFSET(daPy_lk_c, m34CC, 0x6911);
WWHD_OFFSET(daPy_lk_c, m34CD, 0x6912);
WWHD_OFFSET(daPy_lk_c, m34CE, 0x6913);
WWHD_OFFSET(daPy_lk_c, mProcVar0, 0x6916);
WWHD_OFFSET(daPy_lk_c, mProcVar1, 0x6918);
WWHD_OFFSET(daPy_lk_c, mProcVar2, 0x691A);
WWHD_OFFSET(daPy_lk_c, mProcVar3, 0x691C);
WWHD_OFFSET(daPy_lk_c, mProcVar4, 0x691E);
WWHD_OFFSET(daPy_lk_c, mProcVar5, 0x6920);
WWHD_OFFSET(daPy_lk_c, m34DC, 0x6924);
WWHD_OFFSET(daPy_lk_c, m34DE, 0x6926);
WWHD_OFFSET(daPy_lk_c, m34E0, 0x6928);
WWHD_OFFSET(daPy_lk_c, m34E2, 0x692A);
WWHD_OFFSET(daPy_lk_c, m34E4, 0x692C);
WWHD_OFFSET(daPy_lk_c, m34E6, 0x692E);
WWHD_OFFSET(daPy_lk_c, m34E8, 0x6930);
WWHD_OFFSET(daPy_lk_c, m34EA, 0x6932);
WWHD_OFFSET(daPy_lk_c, m34EC, 0x6934);
WWHD_OFFSET(daPy_lk_c, mpSeAnm, 0x6938);
WWHD_OFFSET(daPy_lk_c, m693C, 0x693C);
WWHD_OFFSET(daPy_lk_c, m34F0, 0x6940);
WWHD_OFFSET(daPy_lk_c, m34F2, 0x6942);
WWHD_OFFSET(daPy_lk_c, m34F4, 0x6944);
WWHD_OFFSET(daPy_lk_c, m34F6, 0x6946);
WWHD_OFFSET(daPy_lk_c, m34F8, 0x6948);
WWHD_OFFSET(daPy_lk_c, m34FA, 0x694A);
WWHD_OFFSET(daPy_lk_c, m34FC, 0x694C);
WWHD_OFFSET(daPy_lk_c, m34FE, 0x694E);
WWHD_OFFSET(daPy_lk_c, m3500, 0x6950);
WWHD_OFFSET(daPy_lk_c, m3502, 0x6952);
WWHD_OFFSET(daPy_lk_c, m3504, 0x6954);
WWHD_OFFSET(daPy_lk_c, m3506, 0x6956);
WWHD_OFFSET(daPy_lk_c, m3508, 0x6958);
WWHD_OFFSET(daPy_lk_c, m350A, 0x695A);
WWHD_OFFSET(daPy_lk_c, m350C, 0x695C);
WWHD_OFFSET(daPy_lk_c, m350E, 0x695E);
WWHD_OFFSET(daPy_lk_c, m3510, 0x6960);
WWHD_OFFSET(daPy_lk_c, m3512, 0x6962);
WWHD_OFFSET(daPy_lk_c, m3514, 0x6964);
WWHD_OFFSET(daPy_lk_c, m3516, 0x6966);
WWHD_OFFSET(daPy_lk_c, m3518, 0x6968);
WWHD_OFFSET(daPy_lk_c, m351A, 0x696A);
WWHD_OFFSET(daPy_lk_c, m351C, 0x696C);
WWHD_OFFSET(daPy_lk_c, m351E, 0x696E);
WWHD_OFFSET(daPy_lk_c, m3522, 0x6972);
WWHD_OFFSET(daPy_lk_c, m3524, 0x6974);
WWHD_OFFSET(daPy_lk_c, m3526, 0x6976);
WWHD_OFFSET(daPy_lk_c, m3528, 0x6978);
WWHD_OFFSET(daPy_lk_c, m352A, 0x697A);
WWHD_OFFSET(daPy_lk_c, m352C, 0x697C);
WWHD_OFFSET(daPy_lk_c, m352E, 0x697E);
WWHD_OFFSET(daPy_lk_c, m3530, 0x6980);
WWHD_OFFSET(daPy_lk_c, m3532, 0x6982);
WWHD_OFFSET(daPy_lk_c, m3534, 0x6984);
WWHD_OFFSET(daPy_lk_c, m3536, 0x6986);
WWHD_OFFSET(daPy_lk_c, m3538, 0x6988);
WWHD_OFFSET(daPy_lk_c, m353A, 0x698A);
WWHD_OFFSET(daPy_lk_c, m353C, 0x698C);
WWHD_OFFSET(daPy_lk_c, m353E, 0x698E);
WWHD_OFFSET(daPy_lk_c, m3540, 0x6990);
WWHD_OFFSET(daPy_lk_c, m3542, 0x6992);
WWHD_OFFSET(daPy_lk_c, m3544, 0x6994);
WWHD_OFFSET(daPy_lk_c, mShieldFrontRangeYAngle, 0x6996);
WWHD_OFFSET(daPy_lk_c, m3548, 0x6998);
WWHD_OFFSET(daPy_lk_c, mTinkleHoverTimer, 0x699C);
WWHD_OFFSET(daPy_lk_c, mTinkleShieldTimer, 0x699E);
WWHD_OFFSET(daPy_lk_c, m3550, 0x69A0);
WWHD_OFFSET(daPy_lk_c, mKeepItem, 0x69A2);
WWHD_OFFSET(daPy_lk_c, m3554, 0x69A4);
WWHD_OFFSET(daPy_lk_c, m3558, 0x69A8);
WWHD_OFFSET(daPy_lk_c, m355A, 0x69AA);
WWHD_OFFSET(daPy_lk_c, m355C, 0x69AC);
WWHD_OFFSET(daPy_lk_c, m355E, 0x69AE);
WWHD_OFFSET(daPy_lk_c, mEquipItem, 0x69B0);
WWHD_OFFSET(daPy_lk_c, m3562, 0x69B2);
WWHD_OFFSET(daPy_lk_c, m3564, 0x69B4);
WWHD_OFFSET(daPy_lk_c, mCameraInfoIdx, 0x69BC);
WWHD_OFFSET(daPy_lk_c, mProcVar6, 0x69C0);
WWHD_OFFSET(daPy_lk_c, mProcVar7, 0x69C4);
WWHD_OFFSET(daPy_lk_c, m3578, 0x69C8);
WWHD_OFFSET(daPy_lk_c, m357C, 0x69CC);
WWHD_OFFSET(daPy_lk_c, m3580, 0x69D0);
WWHD_OFFSET(daPy_lk_c, mCurrAttributeCode, 0x69D4);
WWHD_OFFSET(daPy_lk_c, m3588, 0x69D8);
WWHD_OFFSET(daPy_lk_c, mStaffIdx, 0x69DC);
WWHD_OFFSET(daPy_lk_c, mEventIdx, 0x69E0);
WWHD_OFFSET(daPy_lk_c, mRestartPoint, 0x69E4);
WWHD_OFFSET(daPy_lk_c, m3598, 0x69F0);
WWHD_OFFSET(daPy_lk_c, m359C, 0x69F4);
WWHD_OFFSET(daPy_lk_c, m35A0, 0x69F8);
WWHD_OFFSET(daPy_lk_c, m35A4, 0x69FC);
WWHD_OFFSET(daPy_lk_c, m35A8, 0x6A00);
WWHD_OFFSET(daPy_lk_c, m35AC, 0x6A04);
WWHD_OFFSET(daPy_lk_c, mStickDistance, 0x6A08);
WWHD_OFFSET(daPy_lk_c, m35B4, 0x6A0C);
WWHD_OFFSET(daPy_lk_c, m35B8, 0x6A10);
WWHD_OFFSET(daPy_lk_c, mNormalSpeed, 0x6A14);
WWHD_OFFSET(daPy_lk_c, m35C4, 0x6A1C);
WWHD_OFFSET(daPy_lk_c, m35C8, 0x6A20);
WWHD_OFFSET(daPy_lk_c, m35CC, 0x6A24);
WWHD_OFFSET(daPy_lk_c, mWaterY, 0x6A28);
WWHD_OFFSET(daPy_lk_c, m35D4, 0x6A2C);
WWHD_OFFSET(daPy_lk_c, m35D8, 0x6A30);
WWHD_OFFSET(daPy_lk_c, mHangGroundH, 0x6A34);
WWHD_OFFSET(daPy_lk_c, m35E0, 0x6A38);
WWHD_OFFSET(daPy_lk_c, m35E4, 0x6A3C);
WWHD_OFFSET(daPy_lk_c, m35E8, 0x6A40);
WWHD_OFFSET(daPy_lk_c, m35EC, 0x6A44);
WWHD_OFFSET(daPy_lk_c, m35F0, 0x6A48);
WWHD_OFFSET(daPy_lk_c, m35F4, 0x6A4C);
WWHD_OFFSET(daPy_lk_c, m35F8, 0x6A50);
WWHD_OFFSET(daPy_lk_c, m35FC, 0x6A54);
WWHD_OFFSET(daPy_lk_c, m3600, 0x6A58);
WWHD_OFFSET(daPy_lk_c, m3604, 0x6A5C);
WWHD_OFFSET(daPy_lk_c, m3608, 0x6A60);
WWHD_OFFSET(daPy_lk_c, mSeAnmRate, 0x6A64);
WWHD_OFFSET(daPy_lk_c, m3610, 0x6A68);
WWHD_OFFSET(daPy_lk_c, mShadowId, 0x6A6C);
WWHD_OFFSET(daPy_lk_c, mModeFlg, 0x6A70);
WWHD_OFFSET(daPy_lk_c, mMtrlSndId, 0x6A74);
WWHD_OFFSET(daPy_lk_c, m3620, 0x6A78);
WWHD_OFFSET(daPy_lk_c, m3624, 0x6A7C);
WWHD_OFFSET(daPy_lk_c, mGameOverId, 0x6A80);
WWHD_OFFSET(daPy_lk_c, mTactZevPartnerId, 0x6A84);
WWHD_OFFSET(daPy_lk_c, m3630, 0x6A88);
WWHD_OFFSET(daPy_lk_c, mWhirlId, 0x6A8C);
WWHD_OFFSET(daPy_lk_c, mMsgId, 0x6A90);
WWHD_OFFSET(daPy_lk_c, mpSeAnmFrameCtrl, 0x6A94);
WWHD_OFFSET(daPy_lk_c, m3640, 0x6A98);
WWHD_OFFSET(daPy_lk_c, m3644, 0x6A9C);
WWHD_OFFSET(daPy_lk_c, m3648, 0x6AA0);
WWHD_OFFSET(daPy_lk_c, m3658, 0x6AB0);
WWHD_OFFSET(daPy_lk_c, m3668, 0x6AC0);
WWHD_OFFSET(daPy_lk_c, m3688, 0x7290);
WWHD_OFFSET(daPy_lk_c, mOldSpeed, 0x729C);
WWHD_OFFSET(daPy_lk_c, m36A0, 0x72A8);
WWHD_OFFSET(daPy_lk_c, m36AC, 0x72B4);
WWHD_OFFSET(daPy_lk_c, m36B8, 0x72C0);
WWHD_OFFSET(daPy_lk_c, m36C4, 0x72CC);
WWHD_OFFSET(daPy_lk_c, m36D0, 0x72D8);
WWHD_OFFSET(daPy_lk_c, m36DC, 0x72E4);
WWHD_OFFSET(daPy_lk_c, mHookshotRootPos, 0x72F0);
WWHD_OFFSET(daPy_lk_c, mBoomerangCatchPos, 0x72FC);
WWHD_OFFSET(daPy_lk_c, m3700, 0x7308);
WWHD_OFFSET(daPy_lk_c, m370C, 0x7314);
WWHD_OFFSET(daPy_lk_c, m3718, 0x7320);
WWHD_OFFSET(daPy_lk_c, m3724, 0x732C);
WWHD_OFFSET(daPy_lk_c, m3730, 0x7338);
WWHD_OFFSET(daPy_lk_c, m373C, 0x7344);
WWHD_OFFSET(daPy_lk_c, m3748, 0x7350);
WWHD_OFFSET(daPy_lk_c, m37B4, 0x73BC);
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
