/* Great Sea rafts and ships; verified HD layout.
 * HD: the actor base grows from 0x290 to 0x3AC, animation and rope-material
 * objects grow, and a sea model/texture animation and motion tail are added.
 */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
#include "d/d_bg_w.h"
#include "d/d_drawlist.h"
#include "d/actor/d_a_player.h"

// HD: process number used by this actor.
enum { fpcNm_MAJUU_FLAG_e = 0xAE };

// HD: emitter fields beyond the shared minimal JPABaseEmitter interface.

enum IkadaResource_e {
    dRes_INDEX_IKADAH_BCK_SVSHIP_KAITEN_e = 5,
    dRes_INDEX_IKADAH_BDL_VIKAE_e = 8,
    dRes_INDEX_IKADAH_BDL_VIKAH_e = 9,
    dRes_INDEX_IKADAH_BDL_VSVSP_e = 10,
    dRes_INDEX_IKADAH_BDL_VTSP_e = 11,
    dRes_INDEX_IKADAH_BDL_VTSP2_e = 12,
    dRes_INDEX_IKADAH_DZB_VIKAE_e = 15,
    dRes_INDEX_IKADAH_DZB_VIKAH_e = 16,
    dRes_INDEX_IKADAH_DZB_VSVSP_e = 17,
    dRes_INDEX_IKADAH_DZB_VTSP_e = 18,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
    dRes_INDEX_LINK_BDL_ROPEEND_e = 0x2E,
};

struct IkadaEmitter {
    /* 0x000 */ u8 mPrefix[0x28];
    /* 0x028 */ be<f32> mEmitterDir[3];
    /* 0x034 */ u8 mDynamics[0x220 - 0x34];
    /* 0x220 */ be<f32> mGlobalDynamicsScale[3];
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ be<f32> mGlobalParticleScale[3];
    /* 0x244 */ u8 mColors[0x254 - 0x244];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_SIZE(IkadaEmitter, 0x258);
WWHD_OFFSET(IkadaEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(IkadaEmitter, mFlags, 0x254);

struct IkadaWaveCallback {
    be<u32> __vtbl;
    u8 _04[4];
    be<f32> mVelFade1;
    u8 _0C[4];
    be<f32> mVelSpeed, mVelFade2, mMaxParticleVelocity;
    cXyz mCollapsePos[2];
    u8 _34[0x2C];
    be<u32> mpEmitter;
};
struct IkadaSplashCallback {
    be<u32> __vtbl;
    u8 _04[4];
    be<f32> mScaleTimer, mMaxScaleTimer;
    u8 _10[8];
    be<u32> mpEmitter;
};
struct IkadaTrackCallback {
    be<u32> __vtbl;
    be<u16> mStop;
    u8 _06[2];
    be<f32> mWaterY, mWaterFlatY;
    u8 _10[0x2C];
    be<f32> mIndTransY, mIndScaleY, mSpeed, mLimitSpeed;
    be<u32> mpEmitter;
};
struct IkadaFollowCallback {
    be<u32> __vtbl;
    be<u32> mpEmitter;
    u8 _08[12];
};
struct IkadaRippleCallback {
    be<u32> __vtbl;
    be<u32> mpEmitter;
    u8 _08[8];
    be<f32> mRate;
};
struct IkadaWave {
    be<s16> mAngX, mAngZ;
    be<s16> mAnimX, mAnimZ, mRotX, mRotZ;
};

struct daObj_Ikada_HIO_c {
    /* 0x00 */ be<u32> __vtbl;

    /* 0x04 */ be<u8> mbDebugDraw;
    /* 0x05 */ be<u8> m05;
    /* 0x06 */ be<u8> m06;
    /* 0x07 */ be<u8> mbNoRotAnim;
    /* 0x08 */ be<u8> m08;
    /* 0x09 */ be<u8> m09[0x0C - 0x09];
    /* 0x0C */ cXyz mFlagOffset;
    /* 0x18 */ be<f32> mFlagScale;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<s16> m1E;
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<s16> m24;
    /* 0x26 */ be<u8> m26[0x28 - 0x26];
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> mShipOffsY_Attention;
    /* 0x30 */ be<f32> mShipOffsY_Eye;
    /* 0x34 */ be<f32> mTerryWaveOffsZ;
    /* 0x38 */ be<f32> mTerryWaveOffsY;
    /* 0x3C */ be<f32> mTerryTrackOffsZ;
    /* 0x40 */ be<f32> mSvWaveOffsX;
    /* 0x44 */ be<f32> mSvTrackOffsX;
    /* 0x48 */ be<f32> mSvOffsX[4];
    /* 0x58 */ be<f32> mTrackIndTransY;
    /* 0x5C */ be<f32> mTrackIndScaleY;
    /* 0x60 */ be<f32> mSplashScaleMax;
    /* 0x64 */ be<f32> mSplashMaxScaleTimer;
    /* 0x68 */ be<f32> mWaveVelFade;
    /* 0x6C */ be<f32> mTrackVel;
    /* 0x70 */ be<f32> mWaveVelSpeed;
    /* 0x74 */ be<f32> mWaveVelOffs;
    /* 0x78 */ be<f32> mWaveMaxVelocity;
    /* 0x7C */ cXyz mWaveCollapsePos0;
    /* 0x88 */ cXyz mWaveCollapsePos1;
    /* 0x94 */ be<s16> m94;
    /* 0x96 */ be<s16> mPlayerStopDistance;
    /* 0x98 */ be<s16> m98;
    /* 0x9A */ be<s16> m9A;
    /* 0x9C */ be<f32> m9C;
    /* 0xA0 */ be<f32> mVelocityTargetTerry1;
    /* 0xA4 */ be<f32> mVelocityTargetTerry3;
    /* 0xA8 */ be<f32> mA8;
};
WWHD_SIZE(daObj_Ikada_HIO_c, 0xAC);

struct IkadaSafeString { be<u32> mpText, mpVtable; };
struct IkadaColorVector { be<f32> mValues[4]; };
WWHD_SIZE(IkadaSafeString, 8);
WWHD_SIZE(IkadaColorVector, 16);

struct daObj_Ikada_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e { MODE_WAIT_e, MODE_CRANE_UP_e, MODE_CRANE_UP_WAIT_e,
        MODE_CRANE_DOWN_e, MODE_CRANE_DOWN_WAIT_e, MODE_CRANE_TURN_e,
        MODE_CRANE_RESET_e, MODE_CRANE_WAIT_e, MODE_PATH_MOVE_e, MODE_STOP_e,
        MODE_PATH_MOVE_TERRY_e, MODE_STOP_TERRY_e, MODE_STOP_BOMB_TERRY_e };
    using modeFunc = void (daObj_Ikada_c::*)();
    struct ModeFuncs { gabi::ptmf<modeFunc> init; gabi::ptmf<modeFunc> exec; gptr<const char> name; };
    void modeWaitInit();
    void modeCraneUpInit();
    void modeStopInit();
    void modeStopBombTerryInit();
    void _nodeControl(void*, J3DModel*);
    void debugDraw();
    /* 0x3AC */ be<s32> mType;
    /* 0x3B0 */ be<u32> m0294, m0298, m029C, m02A0;
    /* 0x3C0 */ be<u8> mPathId;
    /* 0x3C1 */ u8 _3C1[3];
    /* 0x3C4 */ cXyz mCurPathP0;
    /* 0x3D0 */ be<s8> mCurPathPoint;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<path_layout::dPath> mpPath;
    /* 0x3D8 */ u8 _3D8[4];
    /* 0x3DC */ cXyz mCurPathP1, m02CC;
    /* 0x3F4 */ be<f32> mVelocityFwdTarget;
    /* 0x3F8 */ cXyz mPathPosTarget;
    /* 0x404 */ u8 _404[4];
    /* 0x408 */ be<u8> mbCraneMode;
    /* 0x409 */ u8 _409[3];
    /* 0x40C */ be<s32> mCurMode, m02F4;
    /* 0x414 */ IkadaWaveCallback mWaveLCallback, mWaveRCallback;
    /* 0x4DC */ IkadaSplashCallback mSplashCallBack;
    /* 0x4F8 */ IkadaTrackCallback mTrackCallBack;
    /* 0x548 */ IkadaRippleCallback mRippleCallBack;
    /* 0x55C */ be<u8> m0440;
    /* 0x55D */ u8 _55D[3];
    /* 0x560 */ cXyz m0444, mWavePos;
    /* 0x578 */ csXyz mWaveRot;
    /* 0x57E */ u8 _57E[2];
    /* 0x580 */ be<f32> mSplashScaleTimer;
    /* 0x584 */ cXyz mTrackPos, m0474;
    /* 0x59C */ IkadaFollowCallback mBombSmokeEasterEgg;
    /* 0x5B0 */ csXyz mBombSmokeRot;
    /* 0x5B6 */ u8 _5B6[2];
    /* 0x5B8 */ cXyz mBombSmokePos;
    /* 0x5C4 */ be<s32> m04A8;
    /* 0x5C8 */ be<s16> mBombSmokeAngle;
    /* 0x5CA */ u8 _5CA[2];
    /* 0x5CC */ IkadaFollowCallback mFireParticle;
    /* 0x5E0 */ cXyz mFirePos;
    /* 0x5EC */ Mtx34 mLightMtx;
    /* 0x61C */ be<f32> mLightPower, mLightPowerTarget, m0508, m050C;
    /* 0x62C */ be<s32> mEpTimer0, mEpTimer1;
    /* 0x634 */ u8 _634[4];
    /* 0x638 */ be<s32> mTimer;
    /* 0x63C */ u8 _63C[8];
    /* 0x644 */ be<s16> mLightRotX, mLightRotY;
    /* 0x648 */ request_of_phase_process_class mPhase;
    /* 0x650 */ gptr<J3DModel> mpModel;
    /* 0x654 */ mDoExt_bckAnm mBckAnm;
    /* 0x6E0 */ be<u8> mbIsLinkRiding;
    /* 0x6E1 */ u8 _6E1[3];
    /* 0x6E4 */ gptr<dBgW> mpBgW;
    /* 0x6E8 */ Mtx34 mMtx;
    /* 0x718 */ dBgS_ObjAcch mObjAcch;
    /* 0x8DC */ dBgS_AcchCir mAcchCir;
    /* 0x91C */ be<u32> mFlagPcId;
    /* 0x920 */ cXyz mFlagOffset;
    /* 0x92C */ be<f32> mFlagScale;
    /* 0x930 */ gptr<J3DModel> mpRopeEnd;
    /* 0x934 */ mDoExt_3DlineMat1_c mRopeLine;
    /* 0xABC */ be<s32> m07D8;
    /* 0xAC0 */ cXyz m07DC[200];
    /* 0x1420 */ csXyz mJointRot[4];
    /* 0x1438 */ be<s32> m1154;
    /* 0x143C */ gptr<J3DModel> mpSeaModel;
    /* 0x1440 */ mDoExt_btkAnm mSeaBtk;
    be<s16> m1158, m115A, m115C, m115E, mLinkRideRockTimer,
    /* 0x14BE */ mLinkRideRockAmpl, mWaveAnimTimer;
    /* 0x14C2 */ be<u8> m1166;
    /* 0x14C3 */ u8 _14C3;
    /* 0x14C4 */ be<s32> m1168;
    /* 0x14C8 */ be<u32> mSvId[4];
    /* 0x14D8 */ dCcD_Sph mSph;
    /* 0x1604 */ dCcD_Stts mStts;
    /* 0x1640 */ be<s32> m12E4;
    /* 0x1644 */ cXyz mSePos;
    /* 0x1650 */ be<s32> mStopTimer;
    /* 0x1654 */ IkadaWave mWave;
    /* 0x1660 */ u8 _1660[4];
    /* 0x1664 */ LIGHT_INFLUENCE mPLight;
    /* 0x1688 */ cXyz mInitPos;
    /* 0x1694 */ be<u8> mHDMotionMode;
    /* 0x1695 */ u8 _1695;
    /* 0x1696 */ be<s16> mHDPitch, mHDRoll;
    /* 0x169A */ u8 _169A[2];
    /* 0x169C */ be<f32> mHDWaterOffset;
    void modeProc(Proc_e proc, s32 mode);
    void modeWait();
    void pathMove();
    s32 _pathMove(cXyz *, cXyz *, cXyz *);
    void modePathMove();
    s32 checkTgHit();
    s32 _draw();
    s32 _delete();
    void createWave();
    void setWave();
    void epProc();
    s32 _execute();
    void setRopePos();
    void setMtx();
    void drawSea();
    s32 _create();
    s32 _createHeap();
    void createInit();
    void modePathMoveTerry();
    void modeStopTerry();
    void modeStopBombTerry();
    void incRopeCnt(s32, s32);
    void modeStop();
    void setCollision();
    void HandleRight();
    void HandleLeft();
    void modeCraneUp();
    void modeCraneUpWaitInit();
    void modeCraneUpWait();
    void modeCraneDownInit();
    void modeCraneDown();
    void modeCraneDownWaitInit();
    void modeCraneDownWait();
    void modeCraneTurnInit();
    void modeCraneTurn();
    void modeCraneResetInit();
    void modeCraneReset();
    void modeCraneWaitInit();
    void modeCraneWait();
    void modePathMoveInit();
    void modePathMoveTerryInit();
    void modeStopTerryInit();
    void getArg();
    void _ride(fopAc_ac_c *);
};
WWHD_OFFSET(daObj_Ikada_c, mType, 0x3AC);
WWHD_OFFSET(daObj_Ikada_c, mCurMode, 0x40C);
WWHD_OFFSET(daObj_Ikada_c, mLightMtx, 0x5EC);
WWHD_OFFSET(daObj_Ikada_c, mBckAnm, 0x654);
WWHD_OFFSET(daObj_Ikada_c, mbIsLinkRiding, 0x6E0);
WWHD_OFFSET(daObj_Ikada_c, mObjAcch, 0x718);
WWHD_OFFSET(daObj_Ikada_c, mRopeLine, 0x934);
WWHD_OFFSET(daObj_Ikada_c, m07D8, 0xABC);
WWHD_OFFSET(daObj_Ikada_c, mJointRot, 0x1420);
WWHD_OFFSET(daObj_Ikada_c, m1158, 0x14B4);
WWHD_OFFSET(daObj_Ikada_c, mSph, 0x14D8);
WWHD_OFFSET(daObj_Ikada_c, mPLight, 0x1664);
WWHD_OFFSET(daObj_Ikada_c, mInitPos, 0x1688);
WWHD_SIZE(daObj_Ikada_c, 0x16A0);

// HD: the timer specialization returns the complete signed register.
s32 ikadaApi_calcTimer(be<s32>*);

WWHD_SIZE(IkadaWaveCallback, 0x64);
WWHD_SIZE(IkadaSplashCallback, 0x1C);
WWHD_SIZE(IkadaTrackCallback, 0x50);
WWHD_SIZE(IkadaFollowCallback, 0x14);
WWHD_SIZE(IkadaRippleCallback, 0x14);
WWHD_SIZE(IkadaWave, 0xC);
WWHD_SIZE(daObj_Ikada_c::ModeFuncs, 0x14);

// HD: fixed ABI bindings for this unit's compiler, graphics, and SDK calls.
// Explicit result and argument widths preserve the original call sites.
s32 ikadaApi_daBomb_c_getBombRestTime(fopAc_ac_c *);
void ikadaApi_cM3dGSph_SetR(u32, f32);
void ikadaApi_cM3dGSph_SetC(u32, cXyz *);
void ikadaApi_cCcS_Set(u32, dCcD_Sph *);
void ikadaApi_dLib_pathMove(cXyz *, be<s8> *, u32, s32, daObj_Ikada_c *, f32);
void ikadaApi_cLib_addCalcPosXZ2(cXyz *, cXyz *, f32, f32);
s32 ikadaApi_cLib_targetAngleY(cXyz *, cXyz *);
void ikadaApi_dCcD_GStts_Move(u32);
s32 ikadaApi_cXyz_normalizeRS(cXyz *);
s32 ikadaApi_cM_atan2s(f32, f32);
u32 ikadaApi_cLib_addCalcAngleS(be<s16> *, s32, s32, s32, s32);
void ikadaApi_cLib_chasePosXZ(cXyz *, cXyz *, f32);
u32 ikadaApi_getEnvironment();
void ikadaApi_mDoExt_3DlineMat1_c_update(u32, u16, s32, s32, u32, f32);
void ikadaApi_mDoExt_3DlineMatSortPacket_setMat(u32, u32);
void ikadaApi_dSnap_RegistFig(s32, daObj_Ikada_c *, f32, f32, f32);
void ikadaApi_destroyObjectStatus(u32, s32);
void ikadaApi_destroyCollisionSphere(u32, s32);
void ikadaApi_destroyRopeMaterial(u32, s32);
void ikadaApi_destroyAcchCircle(u32, s32);
void ikadaApi_destroyAcch(u32, s32);
void ikadaApi_destroyShaderUniform(u32, s32);
void ikadaApi_fopAc_ac_c_fopAc_ac_c(daObj_Ikada_c *, s32);
void ikadaApi_dComIfG_resDelete(request_of_phase_process_class *, const char*);
void ikadaApi_dPa_waveEcallBack_remove(IkadaWaveCallback *);
void ikadaApi_dPa_splashEcallBack_remove(IkadaSplashCallback *);
void ikadaApi_dPa_trackEcallBack_remove(IkadaTrackCallback *);
void ikadaApi_dPa_rippleEcallBack_end(IkadaRippleCallback *);
void ikadaApi_JAIZelBasic_seDeleteObject(cXyz *);
void ikadaApi_dKy_plight_cut(LIGHT_INFLUENCE *);
void ikadaApi_cBgS_Release(u32, u32);
void ikadaApi_registerGlobalObject(s32);
void ikadaApi_initializeMatrixStatics(s32);
void ikadaApi_initializeMathStatics(s32);
f32 ikadaApi_fopAcM_searchActorDistanceXZ(daObj_Ikada_c *, u32);
void ikadaApi_fopAc_ac_c_fopAc_ac_c(daObj_Ikada_c *);
void ikadaApi_constructArray(u32, s32, s32, s32);
void ikadaApi_constructRippleCallback(u32);
void ikadaApi_dPa_followEcallBack_dPa_followEcallBack(u32, s32, s32);
void ikadaApi_J3DFrameCtrl_init(u32, s32);
void ikadaApi_constructShaderUniform(u32);
void ikadaApi_dBgS_Acch_dBgS_Acch(u32);
void ikadaApi_dBgS_AcchCir_dBgS_AcchCir(u32);
void ikadaApi_constructRopeMaterial(u32);
void ikadaApi_mDoExt_btkAnm_init(u32);
void ikadaApi_constructCollisionSphere(u32);
void ikadaApi_constructCollisionStatus(u32);
void ikadaApi_dCcD_GStts_dCcD_GStts(u32);
void ikadaApi_assertionShow(const char*, s32, const char*);
s32 ikadaApi_dComIfG_resLoad(request_of_phase_process_class *, const char*);
s32 ikadaApi_fopAcM_entrySolidHeap(daObj_Ikada_c *, s32, u32);
s32 ikadaApi_dComIfGp_getReverb(s32);
u32 ikadaApi_dRes_control_c_getRes(u32, IkadaSafeString*, u32);
u32 ikadaApi_getNamedResource(u32, IkadaSafeString*, IkadaSafeString*);
J3DModel * ikadaApi_mDoExt_J3DModel_create(u32, s32, s32);
s32 ikadaApi_mDoExt_bckAnm_init(mDoExt_bckAnm *, u32, u32, s32, s32, s32, s32, s32, f32);
u32 ikadaApi_getJointTree(u32);
s32 ikadaApi_initSeaTextureAnimation(mDoExt_btkAnm *, u32, u32, s32, s32, s32, s32, s32, s32, f32);
dBgW * ikadaApi_dBgW_dBgW(s32);
s32 ikadaApi_cBgW_Set(u32, u32, s32, Mtx34 *);
s32 ikadaApi_mDoExt_3DlineMat1_c_init(u32, s32, s32, u32, s32);
path_layout::dPath * ikadaApi_dPath_GetRoomPath(u8, s8);
u32 ikadaApi_fopAcM_createChild(u32, u32, s32, cXyz *, s8, s32, s32, s32);
void ikadaApi_dBgW_Move(u32);
void ikadaApi_dBgS_Regist(u32, u32, daObj_Ikada_c *);
void ikadaApi_dBgS_AcchCir_SetWall(dBgS_AcchCir *, f32, f32);
void ikadaApi_dBgS_Acch_Set(dBgS_ObjAcch *, cXyz *, cXyz *, daObj_Ikada_c *, s32, dBgS_AcchCir *, cXyz *, s32, s32);
void ikadaApi_dKy_plight_set(LIGHT_INFLUENCE *);
void ikadaApi_fopAcM_posMoveF(daObj_Ikada_c *, s32);
void ikadaApi_fopAcM_setCullSizeBox(daObj_Ikada_c *, f32, f32, f32, f32, f32, f32);
void ikadaApi_mDoMtx_stack_c_transM(f32, f32, f32);
void ikadaApi_dCcD_Stts_Init(dCcD_Stts *, s32, s32, daObj_Ikada_c *);
void ikadaApi_dCcD_Sph_Set(dCcD_Sph *, s32);
s32 ikadaApi_dKy_daynight_check();
void ikadaApi_dPa_followEcallBack_end(IkadaFollowCallback *);
void ikadaApi_dPa_control_c_setSimple(u32, s32, cXyz *, s32, s32, s32, s32);
s32 ikadaApi_fopAcM_checkCullingBox(u32, f32, f32, f32, f32, f32, f32);
u32 ikadaApi_fopAcIt_Judge(s32, gabi::be<unsigned int> *);
void ikadaApi_cXyz_normalizeZP(cXyz *, cXyz *);
void ikadaApi_cXyz_dv(cXyz *, cXyz *, f32);
void ikadaApi_fopKyM_createWpillar(cXyz *, s32, f32, f32);
void ikadaApi_dLib_waveRot(cXyz *, IkadaWave *, f32);
void ikadaApi_mDoMtx_YrotS(Mtx34*, s16);
void ikadaApi_MtxPosition(cXyz *, cXyz *);
void ikadaApi_mDoMtx_XYZrotM(Mtx34*, s16, s32, s16);
s32 ikadaApi_fopAcM_SearchByID(u32, gabi::be<unsigned int> *);
void ikadaApi_mDoMtx_stack_c_scaleM(f32, f32, f32);
void ikadaApi_mDoExt_baseAnm_play(mDoExt_btkAnm *);
void ikadaApi_cXyz_outprod(cXyz *, cXyz *, cXyz *);
void ikadaApi_dKy_get_seacolor(GXColor*, GXColor*);
void ikadaApi_scaleColorBrightness(IkadaColorVector*, IkadaColorVector*, f32);
u32 ikadaApi_getMaterialColor(u32, s32);

void ikadaApi_subtractVector(cXyz*, cXyz*, cXyz*);

void ikadaApi_addVector(cXyz*, cXyz*, cXyz*);

void ikadaApi_multiplyVector(cXyz*, cXyz*, f32);

f32 ikadaApi_getWaterY(cXyz*, dBgS_ObjAcch*);

void ikadaApi_copyMatrix(Mtx34*, Mtx34*);

void ikadaApi_rotateZXY(Mtx34*, s32, s32, s32);

void ikadaApi_rotateX(Mtx34*, s32);

void ikadaApi_rotateY(Mtx34*, s32);

void ikadaApi_transformVector(Mtx34*, cXyz*, cXyz*);


u32 ikadaApi_getTargetHitObject(dCcD_Sph*);

void ikadaApi_setParticle(dPa_control_c*, s32, s32, cXyz*, csXyz*, cXyz*, s32, void*, s32, void*, void*, void*);

void ikadaApi_startSound(u32, cXyz*, s32, s32);


void ikadaApi_setTranslationMatrix(Mtx34*, f32, f32, f32);

u32 ikadaApi_createActor(s32, u32, cXyz*, s8, csXyz*, s32, s32, s32);

s32 pathMove_CB(cXyz*, cXyz*, cXyz*, daObj_Ikada_c*);

WWHD_OFFSET(daObj_Ikada_c, m0294, 0x3B0);
WWHD_OFFSET(daObj_Ikada_c, m0298, 0x3B4);
WWHD_OFFSET(daObj_Ikada_c, m029C, 0x3B8);
WWHD_OFFSET(daObj_Ikada_c, m02A0, 0x3BC);
WWHD_OFFSET(daObj_Ikada_c, mPathId, 0x3C0);
WWHD_OFFSET(daObj_Ikada_c, mCurPathP0, 0x3C4);
WWHD_OFFSET(daObj_Ikada_c, mCurPathPoint, 0x3D0);
WWHD_OFFSET(daObj_Ikada_c, mpPath, 0x3D4);
WWHD_OFFSET(daObj_Ikada_c, mCurPathP1, 0x3DC);
WWHD_OFFSET(daObj_Ikada_c, m02CC, 0x3E8);
WWHD_OFFSET(daObj_Ikada_c, mVelocityFwdTarget, 0x3F4);
WWHD_OFFSET(daObj_Ikada_c, mPathPosTarget, 0x3F8);
WWHD_OFFSET(daObj_Ikada_c, mbCraneMode, 0x408);
WWHD_OFFSET(daObj_Ikada_c, m02F4, 0x410);
WWHD_OFFSET(daObj_Ikada_c, mWaveLCallback, 0x414);
WWHD_OFFSET(daObj_Ikada_c, mWaveRCallback, 0x478);
WWHD_OFFSET(daObj_Ikada_c, mSplashCallBack, 0x4DC);
WWHD_OFFSET(daObj_Ikada_c, mTrackCallBack, 0x4F8);
WWHD_OFFSET(daObj_Ikada_c, mRippleCallBack, 0x548);
WWHD_OFFSET(daObj_Ikada_c, m0440, 0x55C);
WWHD_OFFSET(daObj_Ikada_c, m0444, 0x560);
WWHD_OFFSET(daObj_Ikada_c, mWavePos, 0x56C);
WWHD_OFFSET(daObj_Ikada_c, mWaveRot, 0x578);
WWHD_OFFSET(daObj_Ikada_c, mSplashScaleTimer, 0x580);
WWHD_OFFSET(daObj_Ikada_c, mTrackPos, 0x584);
WWHD_OFFSET(daObj_Ikada_c, m0474, 0x590);
WWHD_OFFSET(daObj_Ikada_c, mBombSmokeEasterEgg, 0x59C);
WWHD_OFFSET(daObj_Ikada_c, mBombSmokeRot, 0x5B0);
WWHD_OFFSET(daObj_Ikada_c, mBombSmokePos, 0x5B8);
WWHD_OFFSET(daObj_Ikada_c, m04A8, 0x5C4);
WWHD_OFFSET(daObj_Ikada_c, mBombSmokeAngle, 0x5C8);
WWHD_OFFSET(daObj_Ikada_c, mFireParticle, 0x5CC);
WWHD_OFFSET(daObj_Ikada_c, mFirePos, 0x5E0);
WWHD_OFFSET(daObj_Ikada_c, mLightPower, 0x61C);
WWHD_OFFSET(daObj_Ikada_c, mLightPowerTarget, 0x620);
WWHD_OFFSET(daObj_Ikada_c, m0508, 0x624);
WWHD_OFFSET(daObj_Ikada_c, m050C, 0x628);
WWHD_OFFSET(daObj_Ikada_c, mEpTimer0, 0x62C);
WWHD_OFFSET(daObj_Ikada_c, mEpTimer1, 0x630);
WWHD_OFFSET(daObj_Ikada_c, mTimer, 0x638);
WWHD_OFFSET(daObj_Ikada_c, mLightRotX, 0x644);
WWHD_OFFSET(daObj_Ikada_c, mLightRotY, 0x646);
WWHD_OFFSET(daObj_Ikada_c, mPhase, 0x648);
WWHD_OFFSET(daObj_Ikada_c, mpModel, 0x650);
WWHD_OFFSET(daObj_Ikada_c, mpBgW, 0x6E4);
WWHD_OFFSET(daObj_Ikada_c, mMtx, 0x6E8);
WWHD_OFFSET(daObj_Ikada_c, mAcchCir, 0x8DC);
WWHD_OFFSET(daObj_Ikada_c, mFlagPcId, 0x91C);
WWHD_OFFSET(daObj_Ikada_c, mFlagOffset, 0x920);
WWHD_OFFSET(daObj_Ikada_c, mFlagScale, 0x92C);
WWHD_OFFSET(daObj_Ikada_c, mpRopeEnd, 0x930);
WWHD_OFFSET(daObj_Ikada_c, m07DC, 0xAC0);
WWHD_OFFSET(daObj_Ikada_c, m1154, 0x1438);
WWHD_OFFSET(daObj_Ikada_c, mpSeaModel, 0x143C);
WWHD_OFFSET(daObj_Ikada_c, mSeaBtk, 0x1440);
WWHD_OFFSET(daObj_Ikada_c, m115A, 0x14B6);
WWHD_OFFSET(daObj_Ikada_c, m115C, 0x14B8);
WWHD_OFFSET(daObj_Ikada_c, m115E, 0x14BA);
WWHD_OFFSET(daObj_Ikada_c, mLinkRideRockTimer, 0x14BC);
WWHD_OFFSET(daObj_Ikada_c, mLinkRideRockAmpl, 0x14BE);
WWHD_OFFSET(daObj_Ikada_c, mWaveAnimTimer, 0x14C0);
WWHD_OFFSET(daObj_Ikada_c, m1166, 0x14C2);
WWHD_OFFSET(daObj_Ikada_c, m1168, 0x14C4);
WWHD_OFFSET(daObj_Ikada_c, mSvId, 0x14C8);
WWHD_OFFSET(daObj_Ikada_c, mStts, 0x1604);
WWHD_OFFSET(daObj_Ikada_c, m12E4, 0x1640);
WWHD_OFFSET(daObj_Ikada_c, mSePos, 0x1644);
WWHD_OFFSET(daObj_Ikada_c, mStopTimer, 0x1650);
WWHD_OFFSET(daObj_Ikada_c, mWave, 0x1654);
WWHD_OFFSET(daObj_Ikada_c, mHDMotionMode, 0x1694);
WWHD_OFFSET(daObj_Ikada_c, mHDPitch, 0x1696);
WWHD_OFFSET(daObj_Ikada_c, mHDRoll, 0x1698);
WWHD_OFFSET(daObj_Ikada_c, mHDWaterOffset, 0x169C);

struct IkadaNameTable { gptr<const char> mValues[5]; };
WWHD_SIZE(IkadaNameTable, 0x14);

struct IkadaGuard { be<u32> mInitialized; };
WWHD_SIZE(IkadaGuard, 0x4);

struct IkadaQuad { cXyz mPoints[4]; };
WWHD_SIZE(IkadaQuad, 0x30);

struct IkadaQuadTable { gptr<IkadaQuad> mValues[5]; };
WWHD_SIZE(IkadaQuadTable, 0x14);

struct IkadaVectorTable { cXyz mValues[5]; };
WWHD_SIZE(IkadaVectorTable, 0x3C);

struct IkadaIndexTable { be<u32> mValues[5]; };
WWHD_SIZE(IkadaIndexTable, 0x14);

struct IkadaScaleTable { be<f32> mValues[5]; };
WWHD_SIZE(IkadaScaleTable, 0x14);

void ride_CB(void *bg, daObj_Ikada_c *actor, fopAc_ac_c *rider);

s32 nodeControl_CB(void *node, s32 timing);

s32 createHeap_CB(daObj_Ikada_c *actor);

s32 daObj_IkadaCreate(daObj_Ikada_c *actor);

s32 daObj_IkadaDelete(daObj_Ikada_c *actor);

s32 daObj_IkadaExecute(daObj_Ikada_c *actor);

s32 daObj_IkadaDraw(daObj_Ikada_c *actor);

void hioDestructor(void *self, s32 flags);

cXyz *vectorConstructor(cXyz *self);

s32 daObj_IkadaIsDelete(daObj_Ikada_c *actor);

void emptyVirtual(daObj_Ikada_c *actor);

void actorDestructor(daObj_Ikada_c *actor, s32 flags);

void *hioConstructor(void *self);

void __sinit_d_a_obj_ikada_cpp();

f32 searchPlayerDistance(daObj_Ikada_c *actor);

daObj_Ikada_c *actorConstructor(daObj_Ikada_c *actor);
