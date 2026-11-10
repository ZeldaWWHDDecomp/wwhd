/* daNpc_So_c (the Fishman), WWHD layout.
 *
 * GameCube -> WWHD, measured from _create 022DED84 (inlined constructor) and the profile
 * 0x101C63B0 (size 0xD5C, GameCube 0xBE4):
 * - fopNpc_npc_c base: HD 0x7DC (GameCube 0x6C4): members m6C4..mpModel/mBtpAnm are +0x118;
 * - mDoExt_btpAnm grew from 0x14 to 0x74: members from mBlinkTimer on are +0x178.
 * - vtable 0x100223F0 (stored at +0xB4 by _create).
 * Field names are the GameCube ones (mNNN = GameCube offset).
 *
 */
#pragma once
#include "bindings.h"

enum : u32 { SO_VTBL = 0x100223F0 };

struct daNpc_So_c : fopNpc_npc_c {
    enum Proc_e {
        PROC_INIT_e,
        PROC_RUN_e,
    };
    enum Mode_e {
        /*  0 */ MODE_WAIT_e,
        /*  1 */ MODE_HIDE_e,
        /*  2 */ MODE_JUMP_e,
        /*  3 */ MODE_SWIM_e,
        /*  4 */ MODE_NEAR_SWIM_e,
        /*  5 */ MODE_EVENT_FIRST_WAIT_e,
        /*  6 */ MODE_EVENT_FIRST_e,
        /*  7 */ MODE_EVENT_FIRST_END_e,
        /*  8 */ MODE_EVENT_ESA_e,
        /*  9 */ MODE_EVENT_MAPOPEN_e,
        /* 10 */ MODE_EVENT_BOW_e,
        /* 11 */ MODE_TALK_e,
        /* 12 */ MODE_DISAPPEAR_e,
        /* 13 */ MODE_DEBUG_e,
        /* 14 */ MODE_GET_RUPEE_e,
        /* 15 */ MODE_EVENT_TRIFORCE_e,
    };
    s32 getMiniGameRestArrow() { return 10 - mMiniGameShotCount; }
    bool isAnm(s8 idx) { return mAnmPrmIdx == idx; }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }
    void *_searchEsa(fopAc_ac_c *);
    void _nodeControl(J3DNode *, J3DModel *);
    void *_searchTagSo(fopAc_ac_c *);
    void *_searchMinigameTagSo(fopAc_ac_c *);
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL _createHeap();
    bool jntHitCreateHeap();
    bool checkTgHit();
    void offsetZero();
    void offsetDive();
    void offsetSwim();
    void offsetAppear();
    u32 getMsg();
    u16 next_msgStatus(be<u32> *);
    void lookBack();
    void setAttention();
    void setAnm(s8, u32); /* HD: the bool is passed on as the whole register */
    void setAnmSwimSpeed();
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeHideInit();
    void modeHide();
    void modeJumpInit();
    void modeJump();
    void modeSwimInit();
    void modeSwim();
    void modeNearSwimInit();
    void modeNearSwim();
    void modeEventFirstWaitInit();
    void modeEventFirstWait();
    void modeEventFirstInit();
    void modeEventFirst();
    void modeEventFirstEndInit();
    void modeEventFirstEnd();
    void modeEventEsaInit();
    void modeEventEsa();
    void modeEventMapopenInit();
    void modeEventMapopen();
    void modeEventBowInit();
    void modeEventBow();
    void modeTalkInit();
    void modeTalk();
    void modeDisappearInit();
    void modeDisappear();
    void modeDebugInit();
    void modeDebug();
    void modeGetRupeeInit();
    void modeGetRupee();
    void modeEventTriForceInit();
    void modeEventTriForce();
    void modeProc(Proc_e, int);
    void eventOrder();
    void checkOrder();
    void setScale();
    bool _execute();
    void debugDraw();
    void hudeDraw();
    bool _draw();
    void createInit();
    void getArg();
    cPhs_State _create();
    bool _delete();
    void cutAppearProc();
    void cutAppearStart();
    void cutDisappearProc();
    void cutDisappearStart();
    void cutDiveProc();
    void cutDiveStart();
    void cutEatesaFirstProc();
    void cutEatesaFirstStart();
    void cutEatesaProc();
    void cutEatesaStart();
    void cutEffectProc();
    void cutEffectStart();
    void cutEquipProc();
    void cutEquipStart();
    void cutJumpMapopenProc();
    void cutJumpMapopenStart();
    void cutJumpProc();
    void cutJumpStart();
    void cutMiniGameEndProc();
    void cutMiniGameEndStart();
    void cutMiniGamePlTurnProc();
    void cutMiniGamePlTurnStart();
    void cutMiniGamePlUpProc();
    void cutMiniGamePlUpStart();
    void cutMiniGameProc();
    void cutMiniGameReturnProc();
    void cutMiniGameReturnStart();
    void cutMiniGameStart();
    void cutMiniGameWaitProc();
    void cutMiniGameWaitStart();
    void cutMiniGameWarpProc();
    void cutMiniGameWarpStart();
    void cutPartnerShipProc();
    void cutPartnerShipStart();
    void cutProc();
    void cutSetAnmProc();
    void cutSetAnmStart();
    void cutSwimProc();
    void cutSwimStart();
    void cutTurnProc();
    void cutTurnStart();
    void cutUnequipProc();
    void cutUnequipStart();
    void initCam();
    void moveCam();

    /* 0x7DC */ u8 m6C4[0x8]; /* GameCube 0x6C4 */
    /* 0x7E4 */ be<s32> mMode;  /* was m6CC: modeProc 022DE7D8 stores the mode index and runs mode_data[it]; setScale 022DF060 / _execute 022DF6EC / _draw 022DFEDC test it (MODE_EVENT_TRIFORCE_e, MODE_HIDE_e) */
    /* 0x7E8 */ be<s16> mParamMsgNo;  /* was m6D0: getArg 022DE414: home.angle.x (1 when 0); getMsg 022E1CA8 returns it while mMapopenDone is set; next_msgStatus 022E179C: 0x32D4 goes on to it, and it goes on to 0x32CE / 0x32D2 / 0x32D6 (tww the same) */
    /* 0x7EA */ be<s8> mBckIdx;
    /* 0x7EB */ be<s8> mAnmPrmIdx;
    /* 0x7EC */ be<s8> mOldAnmPrmIdx;
    /* 0x7ED */ u8 _7ED[3];
    /* 0x7F0 */ be<s32> mTgHitTimer;  /* was m6D8: checkTgHit 022E160C counts it down and, on a hit, sets HIO m7C; cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) sets the hit sphere only while it is 0 */
    /* 0x7F4 */ dCcD_Stts mStts2;
    /* 0x830 */ dCcD_Sph mSph;
    /* 0x95C */ request_of_phase_process_class mPhase; /* GameCube 0x844 */
    /* 0x964 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x968 */ gptr<J3DModel> mpModel;
    /* 0x96C */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74, GameCube 0x14) */ /* GameCube 0x854 */
    /* 0x9E0 */ be<s32> mBlinkTimer;  /* was m868: setAnmSwimSpeed 022DF60C: counted down; at 0 mBtpFrame steps, and after the btp animation's end frame it restarts at 100 + rnd(100) with mBtpFrame 0 (tww _execute the same); GameCube 0x868 */
    /* 0x9E4 */ be<s16> mBtpFrame;  /* was m86C: setAnmSwimSpeed 022DF60C advances it; _draw 022DFEDC enters the btp animation at this frame */
    /* 0x9E6 */ u8 _9E6[2];
    /* 0x9E8 */ dBgS_ObjAcch mAcch; /* GameCube 0x870 */
    /* 0xBAC */ dBgS_AcchCir mAcchCir;
    /* 0xBEC */ gptr<mDoExt_McaMorf>
        mA74;                /* HD: a second morf of the So model (GameCube: shadow id) */
    /* 0xBF0 */ be<u8> mBF0; /* HD-only: set = the second morf is not drawn */
    /* 0xBF1 */ be<u8> mEquip;  /* was mA78: cutEquipProc 022E26A4 sets 1, cutUnequipProc 022E26E4 sets 0; _draw 022DFEDC draws the equipment while set; GameCube 0xA78 (brush drawn) */
    /* 0xBF2 */ be<u8> mTagRndNum;  /* was mA79: createInit 022DE884: cM_rndF(4.9f); _searchTagSo 022DDF20 picks the tag whose mRndNum equals it */
    u8 _BF3[1]; /* GameCube 0xA79 */
    /* 0xBF4 */ be<f32> mJumpRange;  /* was mA7C: _searchTagSo 022DDF20 copies the tag's getJumpRange(); modeHideInit 022E0084 clears it; _execute 022DF6EC / modeJump 022E01D8 use it */
    /* 0xBF8 */ cXyz mTagPos;  /* was mA80: _searchTagSo 022DDF20 copies the found tag's position; modeHide 022E00B8 puts current.pos there */
    /* 0xC04 */ u8 mA8C[0x4];
    /* 0xC08 */ be<s32> mSwimTimer;  /* was mA90: modeSwimInit 022E0324 sets 30 + rnd(90); modeSwim 022E03A4 counts it down; modeEventFirstWaitInit 022E09B0 sets 150 */
    /* 0xC0C */ u8 mA94[0x8];
    /* 0xC14 */ be<s32> mCutTimer;  /* was mA9C: cutJumpStart 022E1F90 sets 0xF, cutMiniGameProc 022E3484 30, cutTurnStart 022E3CF0; the cut procs count it down */
    /* 0xC18 */ u8 mAA0[0x8];
    /* 0xC20 */ be<u32> mJntHit; /* JntHit_c* */
    /* 0xC24 */ cXyz mInitialPos;  /* was mAAC: createInit 022DE884: current.pos after the 500 drop (tww mInitialPos = current.pos); modeEventFirstWait 022E09CC waits while the ship is within HIO m54 (XZ) of it (tww the same) */
    /* 0xC30 */ u8 mAB8[0x30];
    /* 0xC60 */ u8 mRippleCb[0x14];  /* was mAE8: _create 022DED84 constructs it as dPa_rippleEcallBack; _delete 022DEFEC ends it; dPa_rippleEcallBack (constructor 025A9084) */ /* GameCube 0xAE8 */
    /* 0xC74 */ be<f32> mTargetSpeedF;  /* was mAFC: _execute 022DF6EC eases speedF toward it (cLib_addCalc2 0.3, 4); the cut procs (cutSwimProc 022E1E74, ...) set it */
    /* 0xC78 */ be<f32> mJumpSpeedY;  /* was mB00: modeJumpInit 022E00D4 / cutJumpMapopenProc 022E2C5C: the jump's start speed.y; _execute 022DF6EC picks the pitch target (HIO m64-m6A) from speed.y against a quarter / half of it (tww the same) */
    /* 0xC7C */ be<f32> mPathBlend;  /* was mB04: modeSwim 022E03A4 / modeNearSwim 022E06A4: eased 0 -> 1 (cLib_addCalc2 0.01, 0.05) near the circle path, current.pos += (path pos - current.pos) * it; 0 when far; cutEatesaFirstProc 022E2830 clears it (tww the same) */
    /* 0xC80 */ be<f32> mTargetScale;  /* was mB08: setScale 022DF060: from the player's XZ distance (HIO m44 / m48 / m4C), 1 in MODE_EVENT_TRIFORCE; scale eases toward it (cLib_addCalcPos2); modeJumpInit 022E00D4 / cutMiniGameProc 022E3484 scale the jump speeds by it (tww the same) */
    /* 0xC84 */ be<u8> mBowEventDone;  /* was mB0C: modeEventBow 022E100C sets it when SO_BOW ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8 gives the minigame result messages (0x32DD..0x32E2) while set (tww the same) */
    /* 0xC85 */ u8 _C85[3];
    /* 0xC88 */ u8 mCirclePath[0x24];  /* was mB10: the cut and mode code read it as SoCutCirclePath / SoModeCirclePath; dLib_circle_path_c */
    /* 0xCAC */ be<f32> mOffsetY;  /* was mB34: setMtx 022DE434 translates the model by (0, it, 0); _execute 022DF6EC eases it with the mOffsetYCalc parameters */
    /* 0xCB0 */ cXyz mOffsetYCalc;  /* was mB38: _execute 022DF6EC: cLib_addCalc2(&mOffsetY, x, z, y) (target, scale, max step); offsetZero 022DE6AC sets (-1, 20, ...); the appear / dive cuts set it */
    /* 0xCBC */ cXyz mLookPos;  /* was mB44: lookBack 022DF1CC copies the event cut's attention position; setAttention 022DF148 passes it to lookAtTarget as the target */
    /* 0xCC8 */ be<s16> mLookMaxVel;  /* was mB50: lookBack 022DF1CC eases it (cLib_addCalcAngleS2 4, 0x800) or clears it; setAttention 022DF148 passes it to lookAtTarget as maxVel */
    /* 0xCCA */ u8 _CCA[2];
    /* 0xCCC */ cXyz mHeadEyePos;  /* was mB54: _nodeControl 022DDCB0: the head joint matrix (head turned) times (24, -16, 0); setAttention 022DF148 copies it into eyePos and looks from it + (0, 200, 0); cutEatesaFirstProc 022E2830 turns from it (tww the same) */
    /* 0xCD8 */ cXyz mHeadAttnPos;  /* was mB60: _nodeControl 022DDCB0: the head joint's origin; setAttention 022DF148: attention_info.position = it + HIO mAttnYOffset, at least the water height (tww the same) */
    /* 0xCE4 */ be<s32> mStaffId;  /* was mB6C: the cut procs (cutSwimProc 022E1E74, cutJumpStart 022E1F90, ...) read the event data with it (getMyP(it, "Speed_y")) and end the cut with dComIfGp_evmng_cutEnd(it) */
    /* 0xCE8 */ be<u8> m_order_event_num;  /* was mB70: eventOrder 022DF46C orders by it: 1 / 2 the talk (1 orders the speak event), 3+ a_demo_name_tbl[it - 3] (assert 0x73E names it "m_order_event_num"); checkOrder 022DF3F0 clears it once the event starts; the modes set it (tww the same) */
    /* 0xCE9 */ u8 _CE9[3];
    /* 0xCEC */ be<s32> mMiniGameStep;  /* was mB74: cutMiniGameProc 022E3484 switches on it (0 start, 2 / 4 pick the attention point, 6 jump, 7 the jump in the air) and steps it by 1; the landing (case 7, inlined so_miniGameJump) sets 0 again; cutMiniGameStart 022E2F8C clears it */
    /* 0xCF0 */ be<s32> mMiniGameShotCount;  /* was mB78: cutMiniGameStart 022E2F8C clears it; cutMiniGameProc 022E3484 adds 1, ends at 10; TWW getMiniGameRestArrow() = 10 - it */
    /* 0xCF4 */ be<s32> mMiniGameHitCount;  /* was mB7C: cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) adds 1 (at most 10) on a target hit; next_msgStatus 022E179C pays it * 10 rupees; getMsg 022E1CA8 picks the result message by it (tww getMsg the same); cutMiniGameStart 022E2F8C clears it */
    /* 0xCF8 */ be<s32> mMiniGameJumpCount;  /* was mB80: cutMiniGameProc 022E3484 (case 7, inlined so_miniGameJump) adds 1 per landing and ends the minigame at 10; cutMiniGameStart 022E2F8C / modeDisappear 022E12B4 clear it */
    /* 0xCFC */ be<u8> mB84;
    /* 0xCFD */ u8 _CFD[3];
    /* 0xD00 */ be<f32> mMiniGameAttnOfsX;  /* was mB88: cutMiniGameProc 022E3484: case 2 sets -500 + rnd(1000), case 4 moves it by -150 + rnd(300); the attention point is (it, 0, mMiniGameAttnOfsZ) turned by the ship's angle plus the ship's position */
    /* 0xD04 */ be<f32> mMiniGameAttnOfsZ;  /* was mB8C: cutMiniGameProc 022E3484: case 2 sets 1000 + rnd(400); the z of the attention point (mMiniGameAttnOfsX, 0, it), turned by the ship's angle plus the ship's position */
    /* 0xD08 */ cXyz mMiniGameTagPos;  /* was mB90: _searchMinigameTagSo 022DDF9C copies the minigame tag's position; cutMiniGameWarpStart 022E3E08 puts the ship and the player there */
    /* 0xD14 */ be<s16> mMiniGameTagAngle;  /* was mB9C: _searchMinigameTagSo 022DDF9C copies the tag's shape_angle.y; cutMiniGameWarpStart 022E3E08 turns the ship and the player by it */
    /* 0xD16 */ u8 _D16[2];
    /* 0xD18 */ cXyz mShipReturnPos;  /* was mBA0: cutMiniGameWarpStart 022E3E08 keeps the ship's position; cutMiniGameReturnStart 022E40B8 puts the ship and the player back there */
    /* 0xD24 */ be<s16> mShipReturnAngle;  /* was mBAC: cutMiniGameWarpStart 022E3E08 keeps the ship's shape_angle.y; cutMiniGameReturnStart 022E40B8 restores it */
    /* 0xD26 */ be<u8> mMiniGameTagFound;  /* was mBAE: _searchMinigameTagSo 022DDF9C sets 1 when the minigame tag is found; next_msgStatus 022E179C: 0x32D6 offers the minigame (0x32D8 / 0x32DC) only while it (or HIO m30) is set (tww the same) */
    /* 0xD27 */ u8 _D27[1];
    /* 0xD28 */ cXyz mMapopenGoalPos;  /* was mBB0: cutJumpMapopenProc 022E2C5C: the ship's position moved sideways by -HIO m88 (step 1) / -m8C (step 2); current.pos chases it (cLib_chasePosXZ, HIO m90) */
    /* 0xD34 */ be<s32> mMapopenStep;  /* was mBBC: cutJumpMapopenStart 022E2C18 sets 0; cutJumpMapopenProc 022E2C5C runs steps 0 / 1 / 2 by it and steps it by 1 */
    /* 0xD38 */ cXyz mCamEye;  /* was mBC0: so_camAroundLink (in cutMiniGameStart 022E2F8C) sets it from Link's position; cutMiniGameStart passes it to the camera reset as the eye */
    /* 0xD44 */ cXyz mCamCenter;  /* was mBCC: so_camAroundLink (in cutMiniGameStart 022E2F8C) sets it 100 in front of Link, +50 up; cutMiniGameStart passes it to the camera reset as the center */
    /* 0xD50 */ be<u8> mMapopenDone;  /* was mBD8: modeEventMapopen 022E0F10 sets it when SO_MAPOPEN ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8 / next_msgStatus 022E179C give mParamMsgNo and its follow-ups while set (tww the same) */
    /* 0xD51 */ be<u8> mGetRupeeDone;  /* was mBD9: modeGetRupee 022E140C sets it when SO_GET_RUPEE ends; modeDisappear 022E12B4 clears it; getMsg 022E1CA8: 0x32E2 instead of 0x32DD while set (tww the same) */
    /* 0xD52 */ be<u8> mLookHeadOnly;  /* was mBDA: setAttention 022DF148 passes it to lookAtTarget as the head-only flag (d_npc.h lookAtTarget headOnly; tww the same); createInit 022DE884 0; cutEatesaFirstStart 022E27C4 / cutJumpMapopenStart 022E2C18 set 1, their procs clear it */
    /* 0xD53 */ be<u8> mNoPosMove;  /* was mBDB: _execute 022DF6EC runs fopAcM_posMoveF / CrrPos only while it is 0 (tww the same); modeHideInit 022E0084, modeDisappear 022E12B4, cutMiniGameWarpStart 022E3E08, cutMiniGameReturnStart 022E40B8 set 1; modeSwim 022E03A4, modeEventBow 022E100C, cutMiniGameStart 022E2F8C clear it */
    /* 0xD54 */ be<s16> mEsaXyEventIdx;  /* was mBDC: XyEventCB 022DE088 sets dComIfGp_evmng_getEventIdx("SO_ESA_XY") and returns it; modeEventEsa 022E0E60 waits for its end */
    /* 0xD56 */ be<u8> mCamInitDone;  /* was mBDE: initCam 022E3158: 0 stops the camera and places it around Link once, then 1; cutMiniGameStart 022E2F8C, cutMiniGameEndStart 022E4540, cutMiniGamePlTurnStart 022E45B4 clear it */
    /* 0xD57 */ u8 _D57[1];
    /* 0xD58 */ be<s32> mPosMoveDelay;  /* was mBE0: createInit 022DE884 sets 30; _execute 022DF6EC counts it down and moves the actor only once it is 0 (tww the same); GameCube 0xBE0 */
};
WWHD_OFFSET(daNpc_So_c, m6C4, 0x7DC);
WWHD_OFFSET(daNpc_So_c, mTgHitTimer, 0x7F0);
WWHD_OFFSET(daNpc_So_c, mStts2, 0x7F4);
WWHD_OFFSET(daNpc_So_c, mSph, 0x830);
WWHD_OFFSET(daNpc_So_c, mPhase, 0x95C);
WWHD_OFFSET(daNpc_So_c, mBtpAnm, 0x96C);
WWHD_OFFSET(daNpc_So_c, mAcch, 0x9E8);
WWHD_OFFSET(daNpc_So_c, mAcchCir, 0xBAC);
WWHD_OFFSET(daNpc_So_c, mRippleCb, 0xC60);
WWHD_OFFSET(daNpc_So_c, mCirclePath, 0xC88);
WWHD_OFFSET(daNpc_So_c, mMapopenStep, 0xD34);
WWHD_OFFSET(daNpc_So_c, mPosMoveDelay, 0xD58);
WWHD_SIZE(daNpc_So_c, 0xD5C);

struct __jnt_hit_data_c;
struct dLib_anm_prm_c;
enum { daPyStts0_SHIP_RIDE_e = 0x10000 };

// HD: private views preserve the GHS integer copy operations and caller ABI.
struct SoVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(SoVectorWords, 12);
inline SoVectorWords *soWords(cXyz *p) { return reinterpret_cast<SoVectorWords *>(p); }
struct SoStringVtable {
    u8 _00[0x14];
    gfn assureTermination;
};
struct SoString {
    gptr<const be<u8>> text;
    gptr<SoStringVtable> vt;
};
WWHD_SIZE(SoString, 8);
WWHD_OFFSET(SoString, vt, 4);
inline void soTerminate(SoString *s) { gabi::call_ptr(s->vt->assureTermination.get(), s); }
struct SoModelFlags {
    u8 _00[0x74];
    be<u32> mFlags;
};
WWHD_OFFSET(SoModelFlags, mFlags, 0x74);
struct SoRipple {
    be<u32> __vtbl;
    gptr<void> mpBaseEmitter;
    u8 _08[8];
    be<f32> mRate;
};
WWHD_SIZE(SoRipple, 0x14);
WWHD_OFFSET(SoRipple, mRate, 0x10);
struct SoBtpPrefix {
    u8 _00[0xA];
    be<s16> mEndFrame;
};
WWHD_OFFSET(SoBtpPrefix, mEndFrame, 0xA);
struct SoJntHitHIO {
    u8 _00[0x2C];
};
struct daNpc_So_HIO_c {
    be<u32> __vtbl;
    dNpc_HIO_c mNpc;
    be<u8> m2C, m2D, m2E, m2F, m30, m31;
    u8 _32[2];
    be<f32> m34, m38, m3C, m40, m44, m48, m4C, m50, m54, m58, m5C, m60;
    be<s16> m64, m66, m68, m6A;
    be<f32> m6C, m70, m74, m78;
    be<s16> m7C;
    u8 _7E[2];
    be<f32> m80, m84, m88, m8C, m90;
    be<u8> m94;
    u8 _95[3];
    SoJntHitHIO jntHit;
};
WWHD_OFFSET(daNpc_So_HIO_c, mNpc, 4);
WWHD_OFFSET(daNpc_So_HIO_c, m34, 0x34);
WWHD_OFFSET(daNpc_So_HIO_c, jntHit, 0x98);
WWHD_SIZE(daNpc_So_HIO_c, 0xC4);
struct SoModeEntry {
    gabi::ptmf<void (daNpc_So_c::*)()> init, run;
    gptr<const char> name;
};
WWHD_SIZE(SoModeEntry, 0x14);
struct SoModelSystem {
    u8 _00[0x3C];
    gptr<J3DModel> mModel;
};
WWHD_OFFSET(SoModelSystem, mModel, 0x3C);
struct SoEsa : fopAc_ac_c {
    u8 _3AC[8];
    be<u8> mTaken;  /* was field_0x298: esa_class mTaken (d_a_esa.h) */
};
WWHD_OFFSET(SoEsa, mTaken, 0x3B4);
struct SoTag : fopAc_ac_c {
    be<u8> mRndNum;
    u8 _3AD[3];
    be<f32> mJumpRange;
    be<u8> mType;
};
WWHD_OFFSET(SoTag, mType, 0x3B4);
struct SoHeaderStatics {
    be<f32> mPiMin, mPiMax, mCenterY, mCenterZ, mEyeZ, mEyeY;
    be<u8> mFirst, mSecond;
    u8 _1A[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(SoHeaderStatics, mZero, 0x1C);

f64 soWaterY(cXyz *, dBgS_ObjAcch *);
f64 soRandom(f32);
u32 soCreateJntHit(J3DModel *, const __jnt_hit_data_c *, s32);
void soEndRipple(void *);
BOOL soExecute(void *);
void soSetAnm(const char *, mDoExt_McaMorf *, be<s8> *, be<s8> *, be<s8> *, const be<s32> *,
              const dLib_anm_prm_c *, u32, mDoExt_McaMorf *);
void soSetPacketFlags(J3DModel *, u32);
BOOL soBtpInit(void *, J3DModelData *, void *, s32, s32, f32, s32, s32, u32, u32);
void soNpcCtor(daNpc_So_c *);
void soSphCtor(dCcD_Sph *);
void soBtpCtor(void *);
void soRippleCtor(void *);
f64 soActorDistance(fopAc_ac_c *, fopAc_ac_c *);
void soAddCalcPos(cXyz *, cXyz *, f32, f32);
void soPlayerEye(cXyz *, f32);
void soLookAtTarget(dNpc_JntCtrl_c *, be<s16> *, cXyz *, cXyz *, s32, s32, u32);
void soAddCalcAngle(be<s16> *, s32, s32, s32);
void soOrderChangeEvent(fopAc_ac_c *, const char *, u32, u32);
void soOrderOtherEvent(fopAc_ac_c *, const char *, u32, u32);
void soSubtract(cXyz *, cXyz *, cXyz *);
f64 soSquareMagnitude(cXyz *);
f64 soSqrt(f64);
s32 soCalcTimer(be<s32> *);
void soNpcHIOCtor(dNpc_HIO_c *);
void soJntHitHIOCtor(SoJntHitHIO *);
void soFirstHeaderCtor(be<u8> *);
void soSecondHeaderCtor(be<u8> *);
void soCircleDtor(void *, s32);
void soAcchDtor(dBgS_Acch *, s32);
void soActorDtor(fopAc_ac_c *, s32);
void soSphDtor(dCcD_Sph *, s32);
struct SoModeData {
    be<u32> words[4];
    gptr<const char> name;
};
WWHD_SIZE(SoModeData, 0x14);
struct SoDtorRecords {
    u8 records[36];
};
WWHD_SIZE(SoDtorRecords, 36);

struct SoJointIndex {
    u8 _00[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(SoJointIndex, mJntNo, 4);

enum {
    dRes_INDEX_SO_BCK_SO_JUMP_e = 5,
    dRes_INDEX_SO_BCK_SO_SWIM01_e = 6,
    dRes_INDEX_SO_BCK_SO_TALK01_e = 7,
    dRes_INDEX_SO_BCK_SO_TALK02_e = 8,
    dRes_INDEX_SO_BCK_SO_WAIT01_e = 9,
    dRes_INDEX_SO_BDL_SO_e = 12,
    dRes_INDEX_SO_BDL_SO_FUDE_e = 13,
    dRes_INDEX_SO_BTP_SO_e = 16,
    SO_JNT_BACKBONE_e = 1,
    SO_JNT_HEAD_e = 11,
    fpcNm_TAG_SO_e = 0x23,
    dIsleRoom_DragonRoostIsland_e = 13,
    dIsleRoom_GaleIsle_e = 4,
    dIsleRoom_ToweroftheGods_e = 26,
};
