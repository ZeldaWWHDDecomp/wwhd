/* daNpc_So_c (the Fishman), WWHD layout. 
 *
 * GameCube -> WWHD, measured from _create 022DED84 (inlined constructor) and the profile
 * 0x101C63B0 (size 0xD5C, GameCube 0xBE4):
 * - fopNpc_npc_c base: HD 0x7DC (GameCube 0x6C4): members m6C4..mpModel/mBtpAnm are +0x118;
 * - mDoExt_btpAnm grew from 0x14 to 0x74: members from m868 on are +0x178.
 * - vtable 0x100223F0 (stored at +0xB4 by _create).
 * Field names are the GameCube ones (mNNN = GameCube offset).
 *
 * Coordination: this header holds the layout and the class declaration only. Each part
 * file keeps its own statics/bindings locally; add a member declaration here only with a small
 * python replace of one line. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l */

#define SO_VTBL 0x100223F0 /* daNpc_So_c vtable (_create 022DED84) */

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
    s32 getMiniGameRestArrow() { return 10 - mB78; }
    bool isAnm(s8 idx) { return mAnmPrmIdx == idx; }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }
    void* _searchEsa(fopAc_ac_c*);
    void _nodeControl(J3DNode*, J3DModel*);
    void* _searchTagSo(fopAc_ac_c*);
    void* _searchMinigameTagSo(fopAc_ac_c*);
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
    u16 next_msgStatus(be<u32>*);
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

    /* 0x7DC */ u8 m6C4[0x8];  /* GameCube 0x6C4 */
    /* 0x7E4 */ be<s32> m6CC;
    /* 0x7E8 */ be<s16> m6D0;
    /* 0x7EA */ be<s8> mBckIdx;
    /* 0x7EB */ be<s8> mAnmPrmIdx;
    /* 0x7EC */ be<s8> mOldAnmPrmIdx;
    /* 0x7ED */ u8 _7ED[3];
    /* 0x7F0 */ be<s32> m6D8;
    /* 0x7F4 */ dCcD_Stts mStts2;
    /* 0x830 */ dCcD_Sph mSph;
    /* 0x95C */ request_of_phase_process_class mPhase;  /* GameCube 0x844 */
    /* 0x964 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x968 */ gptr<J3DModel> mpModel;
    /* 0x96C */ u8 mBtpAnm[0x74];                            /* mDoExt_btpAnm (HD 0x74, GameCube 0x14) */  /* GameCube 0x854 */
    /* 0x9E0 */ be<s32> m868;  /* GameCube 0x868 */
    /* 0x9E4 */ be<s16> m86C;
    /* 0x9E6 */ u8 _9E6[2];
    /* 0x9E8 */ dBgS_ObjAcch mAcch;  /* GameCube 0x870 */
    /* 0xBAC */ dBgS_AcchCir mAcchCir;
    /* 0xBEC */ gptr<mDoExt_McaMorf> mA74;  /* HD: a second morf of the So model (GameCube: shadow id) */
    /* 0xBF0 */ be<u8> mBF0;  /* HD-only: set = the second morf is not drawn */
    /* 0xBF1 */ be<u8> mA78;  /* GameCube 0xA78 (brush drawn) */
    /* 0xBF2 */ be<u8> mA79; u8 _BF3[1];  /* GameCube 0xA79 */
    /* 0xBF4 */ be<f32> mA7C;
    /* 0xBF8 */ cXyz mA80;
    /* 0xC04 */ u8 mA8C[0x4];
    /* 0xC08 */ be<s32> mA90;
    /* 0xC0C */ u8 mA94[0x8];
    /* 0xC14 */ be<s32> mA9C;
    /* 0xC18 */ u8 mAA0[0x8];
    /* 0xC20 */ be<u32> mJntHit; /* JntHit_c* */
    /* 0xC24 */ cXyz mAAC;
    /* 0xC30 */ u8 mAB8[0x30];
    /* 0xC60 */ u8 mAE8[0x14];                               /* dPa_rippleEcallBack (constructor 025A9084) */  /* GameCube 0xAE8 */
    /* 0xC74 */ be<f32> mAFC;
    /* 0xC78 */ be<f32> mB00;
    /* 0xC7C */ be<f32> mB04;
    /* 0xC80 */ be<f32> mB08;
    /* 0xC84 */ be<u8> mB0C;
    /* 0xC85 */ u8 _C85[3];
    /* 0xC88 */ u8 mB10[0x24];                               /* dLib_circle_path_c */
    /* 0xCAC */ be<f32> mB34;
    /* 0xCB0 */ cXyz mB38;
    /* 0xCBC */ cXyz mB44;
    /* 0xCC8 */ be<s16> mB50;
    /* 0xCCA */ u8 _CCA[2];
    /* 0xCCC */ cXyz mB54;
    /* 0xCD8 */ cXyz mB60;
    /* 0xCE4 */ be<s32> mB6C;
    /* 0xCE8 */ be<u8> mB70;
    /* 0xCE9 */ u8 _CE9[3];
    /* 0xCEC */ be<s32> mB74;
    /* 0xCF0 */ be<s32> mB78;
    /* 0xCF4 */ be<s32> mB7C;
    /* 0xCF8 */ be<s32> mB80;
    /* 0xCFC */ be<u8> mB84;
    /* 0xCFD */ u8 _CFD[3];
    /* 0xD00 */ be<f32> mB88;
    /* 0xD04 */ be<f32> mB8C;
    /* 0xD08 */ cXyz mB90;
    /* 0xD14 */ be<s16> mB9C;
    /* 0xD16 */ u8 _D16[2];
    /* 0xD18 */ cXyz mBA0;
    /* 0xD24 */ be<s16> mBAC;
    /* 0xD26 */ be<u8> mBAE;
    /* 0xD27 */ u8 _D27[1];
    /* 0xD28 */ cXyz mBB0;
    /* 0xD34 */ be<s32> mBBC;
    /* 0xD38 */ cXyz mBC0;
    /* 0xD44 */ cXyz mBCC;
    /* 0xD50 */ be<u8> mBD8;
    /* 0xD51 */ be<u8> mBD9;
    /* 0xD52 */ be<u8> mBDA;
    /* 0xD53 */ be<u8> mBDB;
    /* 0xD54 */ be<s16> mBDC;
    /* 0xD56 */ be<u8> mBDE;
    /* 0xD57 */ u8 _D57[1];
    /* 0xD58 */ be<s32> mBE0;  /* GameCube 0xBE0 */
};
WWHD_OFFSET(daNpc_So_c, m6C4, 0x7DC);
WWHD_OFFSET(daNpc_So_c, m6D8, 0x7F0);
WWHD_OFFSET(daNpc_So_c, mStts2, 0x7F4);
WWHD_OFFSET(daNpc_So_c, mSph, 0x830);
WWHD_OFFSET(daNpc_So_c, mPhase, 0x95C);
WWHD_OFFSET(daNpc_So_c, mBtpAnm, 0x96C);
WWHD_OFFSET(daNpc_So_c, mAcch, 0x9E8);
WWHD_OFFSET(daNpc_So_c, mAcchCir, 0xBAC);
WWHD_OFFSET(daNpc_So_c, mAE8, 0xC60);
WWHD_OFFSET(daNpc_So_c, mB10, 0xC88);
WWHD_OFFSET(daNpc_So_c, mBBC, 0xD34);
WWHD_OFFSET(daNpc_So_c, mBE0, 0xD58);
WWHD_SIZE(daNpc_So_c, 0xD5C);
