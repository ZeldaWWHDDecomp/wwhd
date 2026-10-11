/* daNpc_So_c (the Fishman), WWHD layout.
 *
 * GameCube -> WWHD, measured from _create 022DED84 (inlined constructor) and the profile
 * 0x101C63B0 (size 0xD5C, GameCube 0xBE4):
 * - fopNpc_npc_c base: HD 0x7DC (GameCube 0x6C4): members m6C4..mpModel/mBtpAnm are +0x118;
 * - mDoExt_btpAnm grew from 0x14 to 0x74: members from m868 on are +0x178.
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
    s32 getMiniGameRestArrow() { return 10 - mB78; }
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
    /* 0x7E4 */ be<s32> m6CC;
    /* 0x7E8 */ be<s16> m6D0;
    /* 0x7EA */ be<s8> mBckIdx;
    /* 0x7EB */ be<s8> mAnmPrmIdx;
    /* 0x7EC */ be<s8> mOldAnmPrmIdx;
    /* 0x7ED */ u8 _7ED[3];
    /* 0x7F0 */ be<s32> m6D8;
    /* 0x7F4 */ dCcD_Stts mStts2;
    /* 0x830 */ dCcD_Sph mSph;
    /* 0x95C */ request_of_phase_process_class mPhase; /* GameCube 0x844 */
    /* 0x964 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x968 */ gptr<J3DModel> mpModel;
    /* 0x96C */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74, GameCube 0x14) */ /* GameCube 0x854 */
    /* 0x9E0 */ be<s32> m868;                                                  /* GameCube 0x868 */
    /* 0x9E4 */ be<s16> m86C;
    /* 0x9E6 */ u8 _9E6[2];
    /* 0x9E8 */ dBgS_ObjAcch mAcch; /* GameCube 0x870 */
    /* 0xBAC */ dBgS_AcchCir mAcchCir;
    /* 0xBEC */ gptr<mDoExt_McaMorf>
        mA74;                /* HD: a second morf of the So model (GameCube: shadow id) */
    /* 0xBF0 */ be<u8> mBF0; /* HD-only: set = the second morf is not drawn */
    /* 0xBF1 */ be<u8> mA78; /* GameCube 0xA78 (brush drawn) */
    /* 0xBF2 */ be<u8> mA79;
    u8 _BF3[1]; /* GameCube 0xA79 */
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
    /* 0xC60 */ u8 mAE8[0x14]; /* dPa_rippleEcallBack (constructor 025A9084) */ /* GameCube 0xAE8 */
    /* 0xC74 */ be<f32> mAFC;
    /* 0xC78 */ be<f32> mB00;
    /* 0xC7C */ be<f32> mB04;
    /* 0xC80 */ be<f32> mB08;
    /* 0xC84 */ be<u8> mB0C;
    /* 0xC85 */ u8 _C85[3];
    /* 0xC88 */ u8 mB10[0x24]; /* dLib_circle_path_c */
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
    /* 0xD58 */ be<s32> mBE0; /* GameCube 0xBE0 */
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
    be<u8> field_0x298;
};
WWHD_OFFSET(SoEsa, field_0x298, 0x3B4);
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
