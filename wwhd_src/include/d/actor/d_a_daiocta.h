/* daDaiocta_c (Big Octo, body), WWHD layout. 
 *
 * GameCube -> WWHD, measured from _create (02119CE8, fopAcM_ct with the inline member
 * constructors), the deleting destructor (0211D204) and the other functions of the unit; the
 * profile size is 0x5910 (GameCube 0x31C8):
 * - fopAc_ac_c +0x11C up to m057C (0x698);
 * - HD-only s32 at 0x69C (a demo fallback timer: getArg zeroes it, modeDemo arms it with 60 and
 *   ends the suction event when it runs down), so mPhs and everything after it is +0x120;
 * - mDoExt_brkAnm 0x78 (GameCube 0x18): from mSph on +0x180;
 * - mDoExt_btkAnm 0x74, mDoExt_bckAnm 0x8C: the bubble ("awa") arrays grow, mAwaTimers.. +0x2748.
 * The separate eye actor is daDaiocta_Eye_c (d_a_daiocta_eye.h). */
#pragma once
#include "SSystem/SComponent/c_cc_d.h"
#include "bindings.h"
#include "d/actor/d_a_daiocta_eye.h"
#include "d/actor/d_a_obj_auzu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_lib.h"
#include "gc_api/f_op/f_op_actor.h"

struct daDaiocta_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e {
        MODE_HIDE = 0,
        MODE_APPEAR = 1,
        MODE_WAIT = 2,
        MODE_DAMAGE = 3,
        MODE_DAMAGE_BOMB = 4,
        MODE_DELETE = 5,
        MODE_DEMO = 6,
        MODE_NULL = 7
    };

    /* 0x03AC */ cXyz mSphCenters[37]; /* joint positions (_nodeControl) */
    /* 0x0568 */ u8 m0568[0x5FC - 0x568];
    /* 0x05FC */ be<u32> mAnmMtxIndices[12]; /* eye joints */
    /* 0x062C */ u8 m062C[0x688 - 0x62C];
    /* 0x0688 */ be<s32> mMode;
    /* 0x068C */ be<s8> mAnmIdx;
    /* 0x068D */ be<s8> mPrmIdx;
    /* 0x068E */ be<s8> mOldPrmIdx;
    /* 0x068F */ be<u8> mOctoType;
    /* 0x0690 */ be<u8> mSwitchNo;
    /* 0x0691 */ be<u8> m575; /* exit (scls) number after the suction demo */
    /* 0x0692 */ u8 m0692[2];
    /* 0x0694 */ be<f32> mAppearRadius;
    /* 0x0698 */ be<u8> m057C; /* from home.angle.z: 1 = the fairy-island variant */
    /* 0x0699 */ u8 m0699[3];
    /* 0x069C */ be<s32> mDemoEndTimer;               /* HD-only */
    /* 0x06A0 */ request_of_phase_process_class mPhs; /* request_of_phase_process_class */
    /* 0x06A8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x06AC */ mDoExt_brkAnm mBrkAnm1; /* mDoExt_brkAnm (frame at +4) */
    /* 0x0724 */ dCcD_Sph mSph[6];       /* dCcD_Sph */
    /* 0x0E2C */ dCcD_Cps mCps[17];      /* dCcD_Cps */
    /* 0x22E4 */ dCcD_Stts mStts;        /* dCcD_Stts */
    /* 0x2320 */ cXyz m21A0;
    /* 0x232C */ cXyz m21AC;
    /* 0x2338 */ cXyz m21B8;
    /* 0x2344 */ cXyz m21C4;
    /* 0x2350 */ cXyz m21D0;
    /* 0x235C */ cXyz m21DC;
    /* 0x2368 */ cXyz m21E8;
    /* 0x2374 */ cXyz m21F4;  /* ship position during the suction demo */
    /* 0x2380 */ csXyz m2200; /* ship angle during the suction demo */
    /* 0x2386 */ u8 m2386[2];
    /* 0x2388 */ be<f32> m2208;
    /* 0x238C */ cXyz m220C;         /* position at creation */
    /* 0x2398 */ dBgS_ObjAcch mAcch; /* dBgS_ObjAcch (flags +0x28, sea height +0xD0, m_gnd +0xE8) */
    /* 0x255C */ dBgS_AcchCir mAcchCir; /* dBgS_AcchCir */
    /* 0x259C */ dPa_followEcallBack mParticleCallback;
    /* 0x25B0 */ be<f32> mWaterY;
    /* 0x25B4 */ cXyz m2434; /* mouth position */
    /* 0x25C0 */ be<u32> mDaioctaEyePcId[12];
    /* 0x25F0 */ be<u8> mEyeAlloc[12];
    /* 0x25FC */ u8 m25FC[0x283C - 0x25FC];
    /* 0x283C */ be<s32> mRotEyeTimer;
    /* 0x2840 */ be<u8> m26C0;
    /* 0x2841 */ be<u8> m26C1;
    /* 0x2842 */ u8 m2842[2];
    /* 0x2844 */ be<u32> mAuzuId;
    /* 0x2848 */ be<u32> mpJntHit;
    /* 0x284C */ gptr<J3DModel> mpSuikomiModel;
    /* 0x2850 */ mDoExt_brkAnm mBrkAnm2; /* mDoExt_brkAnm */
    /* 0x28C8 */ mDoExt_btkAnm mBtkAnm;  /* mDoExt_btkAnm */
    /* 0x293C */ gptr<J3DModel> mpAwaModels[30];
    /* 0x29B4 */ mDoExt_bckAnm mAwaBckAnms[30]; /* mDoExt_bckAnm */
    /* 0x3A1C */ mDoExt_btkAnm mAwaBtkAnms[30]; /* mDoExt_btkAnm */
    /* 0x47B4 */ mDoExt_brkAnm mAwaBrkAnms[30]; /* mDoExt_brkAnm */
    /* 0x55C4 */ be<s32> mAwaTimers[30];
    /* 0x563C */ cXyz mAwaTranslation[30];
    /* 0x57A4 */ cXyz mAwaScale[30];
    /* 0x590C */ be<u8> m31C4;
    /* 0x590D */ u8 m590D[3];

    void _coHit(fopAc_ac_c*);
    void _nodeControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    BOOL createAwaHeap();
    BOOL createSuikomiHeap();
    BOOL createBodyHeap();
    BOOL createArrowHitHeap();
    void setMtx();
    void setSuikomiMtx();
    void setAwaMtx();
    void initMtx();
    void setEffect(u32);
    void setAwaRandom(int);
    void initAwa();
    void execAwa();
    bool isLivingEye();
    bool isDead();
    bool isDamageEye();
    bool isDamageBombEye();
    void setRotEye();
    void setCollision();
    void modeHideInit();
    void modeHide();
    void modeAppearInit();
    void modeAppear();
    void modeWaitInit();
    void modeWait();
    void modeDamageInit();
    void modeDamage();
    void modeDamageBombInit();
    void modeDamageBomb();
    void modeDemoInit();
    void modeDemo();
    void modeDeleteInit();
    void modeDelete();
    void modeProc(int, int);
    void setAnm();
    void setWater();
    bool _execute();
    void drawAwa();
    void drawSuikomi();
    void drawDebug();
    bool _draw();
    void getArg();
    void createInit();
    cPhs_State _create();
    bool _delete();
};
WWHD_OFFSET(daDaiocta_c, mSphCenters, 0x3AC);
WWHD_OFFSET(daDaiocta_c, mAnmMtxIndices, 0x5FC);
WWHD_OFFSET(daDaiocta_c, mMode, 0x688);
WWHD_OFFSET(daDaiocta_c, mDemoEndTimer, 0x69C);
WWHD_OFFSET(daDaiocta_c, mpMorf, 0x6A8);
WWHD_OFFSET(daDaiocta_c, mSph, 0x724);
WWHD_OFFSET(daDaiocta_c, mCps, 0xE2C);
WWHD_OFFSET(daDaiocta_c, mStts, 0x22E4);
WWHD_OFFSET(daDaiocta_c, m21A0, 0x2320);
WWHD_OFFSET(daDaiocta_c, m2208, 0x2388);
WWHD_OFFSET(daDaiocta_c, mAcch, 0x2398);
WWHD_OFFSET(daDaiocta_c, mAcchCir, 0x255C);
WWHD_OFFSET(daDaiocta_c, mParticleCallback, 0x259C);
WWHD_OFFSET(daDaiocta_c, mWaterY, 0x25B0);
WWHD_OFFSET(daDaiocta_c, mDaioctaEyePcId, 0x25C0);
WWHD_OFFSET(daDaiocta_c, mEyeAlloc, 0x25F0);
WWHD_OFFSET(daDaiocta_c, mRotEyeTimer, 0x283C);
WWHD_OFFSET(daDaiocta_c, mAuzuId, 0x2844);
WWHD_OFFSET(daDaiocta_c, mBrkAnm2, 0x2850);
WWHD_OFFSET(daDaiocta_c, mpAwaModels, 0x293C);
WWHD_OFFSET(daDaiocta_c, mAwaBckAnms, 0x29B4);
WWHD_OFFSET(daDaiocta_c, mAwaBtkAnms, 0x3A1C);
WWHD_OFFSET(daDaiocta_c, mAwaBrkAnms, 0x47B4);
WWHD_OFFSET(daDaiocta_c, mAwaTimers, 0x55C4);
WWHD_OFFSET(daDaiocta_c, mAwaTranslation, 0x563C);
WWHD_OFFSET(daDaiocta_c, mAwaScale, 0x57A4);
WWHD_OFFSET(daDaiocta_c, m31C4, 0x590C);
WWHD_SIZE(daDaiocta_c, 0x5910);

/* daDaiocta_HIO_c: GameCube layout (0x108, the mDoHIO_entry_c vtable at +0) */
#define DAIOCTA_HIO 0x10463944u
#define DAIOCTA_ARC 0x1000D418u             /* "daiocta" (m_arc_name) */
#define DAIOCTA_SAFESTRING_VTBL 0x1000CFB4u /* this TU's sead::SafeString vtable */
#define DAIOCTA_VTBL 0x1000D034u            /* daDaiocta_c vtable (HD virtual destructor) */

struct daDaiocta_HIO_c {
    be<u32> __vtbl;
    /* 0x004 */ be<u8> m004;
    /* 0x005 */ be<u8> m005;
    /* 0x006 */ be<u8> m006;
    /* 0x007 */ be<u8> m007;
    /* 0x008 */ be<u8> m008;
    /* 0x009 */ u8 m009[0x00A - 0x009];
    /* 0x00A */ be<s16> m00A;
    /* 0x00C */ be<s16> mRotEyeTimerMin;
    /* 0x00E */ be<s16> mRotEyeTimerAddMax;
    /* 0x010 */ be<s16> m010;
    /* 0x012 */ be<s16> m012;
    /* 0x014 */ be<s16> m014;
    /* 0x016 */ be<s16> m016;
    /* 0x018 */ be<s16> m018;
    /* 0x01A */ u8 m01A[0x01C - 0x01A];
    /* 0x01C */ be<f32> mSphRadii[6];
    /* 0x034 */ be<f32> mCpsRadii[10];
    /* 0x05C */ be<f32> m05C;
    /* 0x060 */ be<f32> m060;
    /* 0x064 */ be<f32> m064;
    /* 0x068 */ be<f32> m068;
    /* 0x06C */ be<f32> m06C;
    /* 0x070 */ be<f32> m070;
    /* 0x074 */ be<f32> m074;
    /* 0x078 */ be<f32> mDefaultAppearRadius;
    /* 0x07C */ be<f32> mMinAppearRadius;
    /* 0x080 */ be<f32> m080;
    /* 0x084 */ be<f32> m084;
    /* 0x088 */ be<f32> m088;
    /* 0x08C */ be<f32> m08C;
    /* 0x090 */ be<f32> m090;
    /* 0x094 */ be<f32> m094;
    /* 0x098 */ be<f32> m098[9];
    /* 0x0BC */ be<f32> m0BC[9];
    /* 0x0E0 */ be<f32> m0E0;
    /* 0x0E4 */ be<f32> m0E4;
    /* 0x0E8 */ be<f32> m0E8;
    /* 0x0EC */ be<f32> m0EC;
    /* 0x0F0 */ be<f32> m0F0;
    /* 0x0F4 */ be<f32> m0F4;
    /* 0x0F8 */ be<f32> m0F8;
    /* 0x0FC */ cXyz m0FC;
};
WWHD_SIZE(daDaiocta_HIO_c, 0x108);

// HD: preserve integer-register vector copies, including signalling NaNs.
template <class T, class U> inline be<T>& DaioctaBits(be<U>& value) {
    static_assert(sizeof(T) == sizeof(U));
    return reinterpret_cast<be<T>&>(value);
}

// HD: indices form unchecked modulo-32 guest addresses, including negative bubble indices.
template <class T, unsigned N> inline T* DaioctaArray(T (&values)[N], s32 index) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + (gabi::ea(values) + u32(index) * u32(sizeof(T))));
}

void* DaioctaHD_dComIfGp_get();
void DaioctaHD_JUTAssertion_showAssert(u32, s32, u32);
int DaioctaHD_fopAcM_SearchByID(u32, be<u32>*);
int DaioctaHD_dComIfGp_getReverb(s32);
void DaioctaHD_mDoAud_seStartAt(u32, u32, u32, s32);
void DaioctaHD_dVibration_c_StartShock(u32, s32, int, cXyz*);
u32 DaioctaHD_daBomb_c_chk_state(u32, int);
u32 DaioctaHD_dLib_brkInit(u32, u32, u32, int);
u32 DaioctaHD_mDoExt_McaMorf_mDoExt_McaMorf(u32, u32, u32, u32, u32, int, f32, int, int, int, u32,
                                            u32, u32);
u32 DaioctaHD_dLib_btkInit(u32, u32, u32, int);
u32 DaioctaHD_mDoExt_bckAnm_init(u32, u32, u32, int, int, f32, int, int, int);
u32 DaioctaHD_mDoExt_btkAnm_initHD(u32, u32, u32, int, int, f32, int, int, int, int);
u32 DaioctaHD_mDoExt_brkAnm_initHD(u32, u32, u32, int, int, f32, int, int, int, int);
u32 DaioctaHD_JntHit_create(u32, u32, int);
void DaioctaHD_mDoMtx_inverseTranspose(u32, Mtx34*);
void DaioctaHD_mDoMtx_MtxToRot(Mtx34*, u32);
void DaioctaHD_dBgS_AcchCir_SetWall(u32, f32, f32);
void DaioctaHD_dBgS_Acch_Set(u32, u32, u32, u32, int, u32, u32, u32, u32);
u32 DaioctaHD_fopAcM_createChild(int, u32, int, u32, s32, u32, u32, int, u32);
void DaioctaHD_dCcD_Stts_Init(u32, int, int, u32);
void DaioctaHD_dCcD_Sph_Set(u32, u32);
void DaioctaHD_dCcD_Cps_Set(u32, u32);
void DaioctaHD_construct_array(u32, u32, u32, u32);
void DaioctaHD_destroy_arr(u32, u32, u32, u32, u32, u32);
void DaioctaHD_fopAc_ac_c_fopAc_ac_c(u32);
void DaioctaHD_mDoExt_brkAnm_construct(u32);
void DaioctaHD_dBgS_AcchCir_dBgS_AcchCir(u32);
void DaioctaHD_dPa_followEcallBack_dPa_followEcallBack(u32, u32, u32);
void DaioctaHD_mDoExt_btkAnm_construct(u32);
void DaioctaHD_cM3dGSph_SetR(u32, f32);
void DaioctaHD_cM3dGSph_SetC(u32, u32);
void DaioctaHD_cM3dGCps_SetStartEnd(u32, u32, u32);
void DaioctaHD_dKy_get_seacolor(GXColor*, GXColor*);
void DaioctaHD_mDoExt_brkAnm_initModelData(u32, u32, u32, int, int, f32, int, int, int, int);
void DaioctaHD_dLib_setAnm(u32, u32, u32, u32, u32, u32, u32, u32, u32);
void DaioctaHD_mDoExt_McaMorf_play(u32, u32, u32, u32);
void DaioctaHD_mDoExt_McaMorf_calc(u32);
u32 DaioctaHD_dBgS_GetPolyColor(u32, u32);
void DaioctaHD_mDoExt_btkAnm_entry(u32, u32, f32);
void DaioctaHD_mDoExt_brkAnm_entry(u32, u32, f32);
void DaioctaHD_mDoExt_bckAnm_entry(u32, u32, f32);
void DaioctaHD_mDoExt_McaMorf_entryDL(u32);
void DaioctaHD_cXyz_normZP(cXyz*, cXyz*);
void DaioctaHD_mDoAud_bgmAllMute(int);
void DaioctaHD_cLib_addCalcPos2(u32, cXyz*, f32, f32);
void DaioctaHD_mDoAud_subBgmStart(u32);
int DaioctaHD_dEvent_manager_c_getMyStaffId(u32, u32, u32, u32);
u32 DaioctaHD_dEvent_manager_c_getMyNowCutName(u32, s32);
void DaioctaHD_dEvent_manager_c_cutEnd(u32, s32);
int DaioctaHD_dEvent_manager_c_endCheckOld(u32, u32);
void DaioctaHD_fopAcM_orderOtherEvent2(u32, u32, u32, u32);
void DaioctaHD_mDoAud_monsSeStart(int, u32, u32, u32, s32);
void DaioctaHD_dVibration_c_StartQuake(u32, int, int, cXyz*);
void DaioctaHD_cLib_chasePos(u32, cXyz*, f32);
void DaioctaHD_mDoAud_subBgmStop();
void DaioctaHD_dVibration_c_StopQuake(u32, int);
void DaioctaHD_mDoAud_seStart(int);
void DaioctaHD_mDoExt_brkAnm_initDemo(u32, u32, u32, int, int, f32, int, int, int, int);
void DaioctaHD_mDoExt_btkAnm_initDemo(u32, u32, u32, int, int, f32, int, int, int, int);
void DaioctaHD_daShip_c_initStartPos(u32, u32, s32);
void DaioctaHD_dComIfGp_getMapTrans(s32, u32, u32, be<s16>*);
void DaioctaHD_cLib_chasePosXZ(u32, u32, f32);
void DaioctaHD_daPy_lk_c_startRestartRoom(u32, int, int, f32, int);
void DaioctaHD_dComIfGp_setNextStage(u32, u32, s32, int, f32, int, int, int);
void DaioctaHD_dCcD_GObjInf_dCcD_GObjInf(u32);
void DaioctaHD_cM3dGCps_construct(u32);
void DaioctaHD_J3DFrameCtrl_init(u32, int);
void DaioctaHD_J3DAnmTransform_construct(u32);
void DaioctaHD_J3DAnmTransform_delete(u32, int);
void DaioctaHD_cM3dGCir_delete(u32, int);
void DaioctaHD_dBgS_Acch_delete(u32, int);
void DaioctaHD_dCcD_Stts_delete(u32, int);
void DaioctaHD_fopAc_ac_c_dtor(u32, int);

struct DaioctaModeEntry {
    gabi::ptmf<void (daDaiocta_c::*)()> mInitFunc, mUpdFunc;
    gptr<const char> mModeName;
};
struct DaioctaModes {
    DaioctaModeEntry mEntries[7];
};
WWHD_SIZE(DaioctaModeEntry, 20);
WWHD_SIZE(DaioctaModes, 140);

struct DaioctaBrkIndices {
    be<s32> mValues[7];
};
struct DaioctaBrkParameters {
    be<s32> mValues[9];
};
struct DaioctaAnimationIndices {
    dLib_anm_idx_c mValues[7];
};
struct DaioctaAnimationParameters {
    be<u32> mWords[36];
};
struct DaioctaSphereJoints {
    be<u8> mValues[6];
};
struct DaioctaJointHitTable {
    octoEyeHD::JointHitData mValues[17];
};

// HD: typed views of raw guest memory preserve guest address zero when adding field offsets.
template <class T> inline T* DaioctaGuest(u32 address) {
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}
struct DaioctaEmitterColors {
    u8 padding[0x244];
    GXColor mGlobalPrmColor;
};
WWHD_OFFSET(DaioctaEmitterColors, mGlobalPrmColor, 0x244);
WWHD_SIZE(DaioctaEmitterColors, 0x248);

// HD: dCcD_Cps keeps the shape and capsule as byte arrays in the shared layout.
struct DaioctaCapsule : dCcD_GObjInf {
    CcDataHD::cCcD_ShapeAttr mShape;
    CcDataHD::Capsule mCps;
};
WWHD_OFFSET(DaioctaCapsule, mShape, 0xF8);
WWHD_OFFSET(DaioctaCapsule, mCps, 0x118);
WWHD_SIZE(DaioctaCapsule, 0x138);
WWHD_SIZE(DaioctaBrkIndices, 28);
WWHD_SIZE(DaioctaBrkParameters, 36);
WWHD_SIZE(DaioctaAnimationParameters, 144);
WWHD_SIZE(DaioctaSphereJoints, 6);
WWHD_SIZE(DaioctaJointHitTable, 204);

// HD: resource indices from the capitalized Daiocta archive.
enum DaioctaResourceIndex {
    dRes_INDEX_DAIOCTA_BCK_GAWA00_e = 0x0A,
    dRes_INDEX_DAIOCTA_BDL_DO_MAIN1_e = 0x11,
    dRes_INDEX_DAIOCTA_BDL_GAWA00_e = 0x12,
    dRes_INDEX_DAIOCTA_BDL_GDO_SUI00_e = 0x13,
    dRes_INDEX_DAIOCTA_BRK_GAWA00_e = 0x1C,
    dRes_INDEX_DAIOCTA_BRK_GDO_SUI00_e = 0x1D,
    dRes_INDEX_DAIOCTA_BRK_WAIT1_e = 0x20,
    dRes_INDEX_DAIOCTA_BTK_GAWA00_e = 0x25,
    dRes_INDEX_DAIOCTA_BTK_GDO_SUI00_e = 0x26,
};
