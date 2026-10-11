/* Carryable stones: retail GC layout plus the HD actor-base delta of 0x11C. Verified WWHD behavior. */
#pragma once
#include "gc_api/gc_api.h"
namespace daStone {
// HD actor profile allocates 0x7AC bytes. The five resource records retain
// their GameCube 0xB4 stride; derived actor fields move by 0x11C.
struct Act_c : fopAc_ac_c {
    s32 chk_appear();
    u32 chk_sink_water();
    s32 chk_sink_lava();
    void init_rot_clean();
    void mode_carry_init();
    void cam_lockoff();
    void eff_land_smoke();
    void init_mtx();
    void water_tention();
    s32 create_heap();
    void cull_set_draw();
    void cull_set_move();
    void set_mtx();
    void mode_wait();
    void mode_wait_init();
    void eff_hit_water_splash();
    void eff_hit_lava_splash();
    void mode_drop();
    void set_drop_rot();
    void set_sink_rot();
    void mode_sink();
    void init_rot_throw();
    void bg_crr_lava();
    void mode_drop_init();
    void mode_carry();
    void mode_sink_init();
    void se_fall_lava();
    void bg_crr_water();
    void se_fall_water();
    s32 damage_cc_proc();
    s32 damage_bg_proc();
    s32 damage_bg_proc_directly();
    s32 mode_proc_call();
    void eff_break();
    void damaged(s32 kind);
    s32 _draw();
    s32 _execute();
    s32 _create();
    s32 _delete();
    void set_senv(u32 firstSoundParameter, u32 secondSoundParameter);

    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    dBgS_ObjGndChk mGndChkYogan;
    cXyz mSurfaceHeights; // lava, current water, previous water
    be<u8> mWaterHit, mSeaHit;
    u8 pad61E[2];
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    be<s32> mType, mMode;
    be<u8> mGrounded;
    be<s8> mLandingDelay;
    be<u8> mMovingGround, mMassDelay;
    be<s16> mRoll, mPitch, mPitchSpeed, mTilt, mTiltSpeed, mYaw;
    be<f32> mPreviousHeight;
    be<u8> mAppeared, mGrabStarted;
    u8 pad7AA[2];
};
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mpModel, 0x3B4);
WWHD_OFFSET(Act_c, mAcch, 0x3B8);
WWHD_OFFSET(Act_c, mAcchCir, 0x57C);
WWHD_OFFSET(Act_c, mGndChkYogan, 0x5BC);
WWHD_OFFSET(Act_c, mSurfaceHeights, 0x610);
WWHD_OFFSET(Act_c, mWaterHit, 0x61C);
WWHD_OFFSET(Act_c, mSeaHit, 0x61D);
WWHD_OFFSET(Act_c, mStts, 0x620);
WWHD_OFFSET(Act_c, mCyl, 0x65C);
WWHD_OFFSET(Act_c, mType, 0x78C);
WWHD_OFFSET(Act_c, mMode, 0x790);
WWHD_OFFSET(Act_c, mPreviousHeight, 0x7A4);
WWHD_SIZE(Act_c, 0x7AC);
} // namespace daStone

struct StoneShapeAttribute {
    u8 prefix[0x18];
    be<u32> boundsVtable, shapeVtable;
};
WWHD_SIZE(StoneShapeAttribute, 0x20);
inline StoneShapeAttribute& stone_shape(dCcD_Cyl& cylinder) {
    return gabi::object_at<StoneShapeAttribute>(gabi::ea(&cylinder.mShapeAttr[0]));
}

namespace daStone {
struct Data_c {
    /* 0x00 */ be<f32> gravity;
    /* 0x04 */ be<f32> bottomOffset;
    /* 0x08 */ be<f32> attentionHeight;
    /* 0x0C */ be<f32> modelScale;
    /* 0x10 */ be<u8> weight;
    u8 padding11[3];
    /* 0x14 */ be<f32> throwVerticalSpeed;
    /* 0x18 */ be<f32> throwBaseSpeed;
    /* 0x1C */ be<f32> throwPlayerSpeedScale;
    /* 0x20 */ be<f32> airFriction;
    /* 0x24 */ be<f32> airStreamSpeed;
    /* 0x28 */ be<s16> throwRoll;
    /* 0x2A */ be<s16> throwPitchSpeed;
    /* 0x2C */ be<s16> throwTiltSpeed;
    u8 padding2E[2];
    /* 0x30 */ be<f32> dropLinearDrag;
    /* 0x34 */ be<f32> dropQuadraticDrag;
    /* 0x38 */ be<f32> breakFallHeight;
    /* 0x3C */ be<f32> landSmokeScale;
    /* 0x40 */ be<s16> shockPower;
    u8 padding42[2];
    /* 0x44 */ be<f32> buoyancy;
    /* 0x48 */ be<f32> waterFriction;
    /* 0x4C */ be<f32> waterStreamSpeed;
    /* 0x50 */ be<f32> sinkLinearDrag;
    /* 0x54 */ be<f32> sinkQuadraticDrag;
    /* 0x58 */ be<f32> sinkSpeedLimit;
    /* 0x5C */ be<f32> waterSplashScale;
    /* 0x60 */ be<f32> waterSplashHeight;
    /* 0x64 */ be<f32> lavaSplashScale;
    /* 0x68 */ be<u8> breakSoundParameter;
    /* 0x69 */ be<u8> breakSoundEnvironment;
    /* 0x6A */ be<u8> hitSoundParameter;
    /* 0x6B */ be<u8> hitSoundEnvironment;
    /* 0x6C */ be<u8> fallSoundParameter;
    /* 0x6D */ be<u8> fallSoundEnvironment;
    u8 padding6E[2];
    /* 0x70 */ be<u32> flags;
    /* 0x74 */ be<u32> archive;
    /* 0x78 */ be<u32> heapSize;
    /* 0x7C */ be<u16> modelIndex;
    /* 0x7E */ be<u8> carryDistance;
    u8 padding7F[1];
    /* 0x80 */ be<s16> shadowScaleXZ;
    /* 0x82 */ be<s16> wallRadius;
    /* 0x84 */ be<s16> cylinderHeight;
    /* 0x86 */ be<s16> roofHeight;
    /* 0x88 */ be<u16> textureIndex;
    u8 padding8A[2];
    /* 0x8C */ be<u32> breakSound;
    /* 0x90 */ be<u32> landSound;
    /* 0x94 */ be<u32> waterSound;
    /* 0x98 */ be<u32> lavaSound;
    /* 0x9C */ be<u32> hitSound;
    /* 0xA0 */ be<s16> moveCullX;
    /* 0xA2 */ be<s16> moveCullY;
    /* 0xA4 */ be<s16> moveCullZ;
    /* 0xA6 */ be<s16> moveCullRadius;
    /* 0xA8 */ be<s16> drawCullX;
    /* 0xAA */ be<s16> drawCullY;
    /* 0xAC */ be<s16> drawCullZ;
    /* 0xAE */ be<s16> drawCullRadius;
    /* 0xB0 */ be<f32> cullDistance;
};
WWHD_SIZE(Data_c, 0xB4);
WWHD_OFFSET(Data_c, gravity, 0x0);
WWHD_OFFSET(Data_c, bottomOffset, 0x4);
WWHD_OFFSET(Data_c, attentionHeight, 0x8);
WWHD_OFFSET(Data_c, modelScale, 0xC);
WWHD_OFFSET(Data_c, weight, 0x10);
WWHD_OFFSET(Data_c, throwVerticalSpeed, 0x14);
WWHD_OFFSET(Data_c, throwBaseSpeed, 0x18);
WWHD_OFFSET(Data_c, throwPlayerSpeedScale, 0x1C);
WWHD_OFFSET(Data_c, airFriction, 0x20);
WWHD_OFFSET(Data_c, airStreamSpeed, 0x24);
WWHD_OFFSET(Data_c, throwRoll, 0x28);
WWHD_OFFSET(Data_c, throwPitchSpeed, 0x2A);
WWHD_OFFSET(Data_c, throwTiltSpeed, 0x2C);
WWHD_OFFSET(Data_c, dropLinearDrag, 0x30);
WWHD_OFFSET(Data_c, dropQuadraticDrag, 0x34);
WWHD_OFFSET(Data_c, breakFallHeight, 0x38);
WWHD_OFFSET(Data_c, landSmokeScale, 0x3C);
WWHD_OFFSET(Data_c, shockPower, 0x40);
WWHD_OFFSET(Data_c, buoyancy, 0x44);
WWHD_OFFSET(Data_c, waterFriction, 0x48);
WWHD_OFFSET(Data_c, waterStreamSpeed, 0x4C);
WWHD_OFFSET(Data_c, sinkLinearDrag, 0x50);
WWHD_OFFSET(Data_c, sinkQuadraticDrag, 0x54);
WWHD_OFFSET(Data_c, sinkSpeedLimit, 0x58);
WWHD_OFFSET(Data_c, waterSplashScale, 0x5C);
WWHD_OFFSET(Data_c, waterSplashHeight, 0x60);
WWHD_OFFSET(Data_c, lavaSplashScale, 0x64);
WWHD_OFFSET(Data_c, breakSoundParameter, 0x68);
WWHD_OFFSET(Data_c, breakSoundEnvironment, 0x69);
WWHD_OFFSET(Data_c, hitSoundParameter, 0x6A);
WWHD_OFFSET(Data_c, hitSoundEnvironment, 0x6B);
WWHD_OFFSET(Data_c, fallSoundParameter, 0x6C);
WWHD_OFFSET(Data_c, fallSoundEnvironment, 0x6D);
WWHD_OFFSET(Data_c, flags, 0x70);
WWHD_OFFSET(Data_c, archive, 0x74);
WWHD_OFFSET(Data_c, heapSize, 0x78);
WWHD_OFFSET(Data_c, modelIndex, 0x7C);
WWHD_OFFSET(Data_c, carryDistance, 0x7E);
WWHD_OFFSET(Data_c, shadowScaleXZ, 0x80);
WWHD_OFFSET(Data_c, wallRadius, 0x82);
WWHD_OFFSET(Data_c, cylinderHeight, 0x84);
WWHD_OFFSET(Data_c, roofHeight, 0x86);
WWHD_OFFSET(Data_c, textureIndex, 0x88);
WWHD_OFFSET(Data_c, breakSound, 0x8C);
WWHD_OFFSET(Data_c, landSound, 0x90);
WWHD_OFFSET(Data_c, waterSound, 0x94);
WWHD_OFFSET(Data_c, lavaSound, 0x98);
WWHD_OFFSET(Data_c, hitSound, 0x9C);
WWHD_OFFSET(Data_c, moveCullX, 0xA0);
WWHD_OFFSET(Data_c, moveCullY, 0xA2);
WWHD_OFFSET(Data_c, moveCullZ, 0xA4);
WWHD_OFFSET(Data_c, moveCullRadius, 0xA6);
WWHD_OFFSET(Data_c, drawCullX, 0xA8);
WWHD_OFFSET(Data_c, drawCullY, 0xAA);
WWHD_OFFSET(Data_c, drawCullZ, 0xAC);
WWHD_OFFSET(Data_c, drawCullRadius, 0xAE);
WWHD_OFFSET(Data_c, cullDistance, 0xB0);
} // namespace daStone
inline const daStone::Data_c& stone_dataRecord(u32 address) {
    return gabi::object_at<daStone::Data_c>(address);
}

// Verified named ABI bindings; shared headers remain unchanged.
void stone_assertData(const char*, int, const char*);
int stone_dSv_info_c__isSwitch(void*, unsigned int, signed char);
void stone_dKy_Sound_set(cXyz*, unsigned int, unsigned int, unsigned int);
void stone_cSAngle_assign(void*, void*);
void stone_cSAngle_setValue(void*, short);
void* stone_getPlay();
void stone_dCamera_c__ForceLockOff(void*, unsigned int);
void stone_daObj__make_land_effect(daStone::Act_c*, void*, float);
void stone_dl(void*);
void stone_cXyz____ml(void*, cXyz*, float);
void stone_daStone__Act_c__set_mtx(daStone::Act_c*);
void* stone_getEnvironmentLight();
void stone_dScnKy_env_light_c__settingTevStruct(void*, unsigned int, void*, void*);
void stone_setLightTevColorType(void*, void*, void*);
void stone_mDoExt_modelUpdateDL(void*, int);
void stone_register_global_object(void*);
void stone_initializeStaticFlag(void*);
void stone_initializeSecondStaticFlag(void*);
void stone_dCcD_Cyl_destruct(void*, int);
void stone_dCcD_Stts_destruct(void*, int);
void stone_cBgS_GndChk_destruct(void*, int);
void stone_cM3dGAab_destruct(void*, int);
void stone_dBgS_Acch_destruct(void*, int);
void stone_fopAc_ac_c___fopAc_ac_c(daStone::Act_c*, int);
void stone_dl_2(daStone::Act_c*);
void* stone_getObjectResource(void*, SafeString*, unsigned short);
void* stone_mDoExt_J3DModel__create(void*, int, int);
void stone_dBgS_AcchCir__SetWall(void*, float, float);
void stone_dBgS_Acch__Set(void*, void*, void*, daStone::Act_c*, int, void*, void*, void*, void*);
void stone_dComIfG_resDelete(void*, const char*);
void stone_fopAcM_setCullSizeSphere(daStone::Act_c*, float, float, float, float);
void stone_PSMTXTrans(void*, float, float, float);
void stone_mDoMtx_stack_c__transM(float, float, float);
void stone_C_QUATRotAxisRad(void*, void*, float);
void stone_mDoMtx_stack_c__quatM(void*);
void stone_mDoMtx_ZXYrotM(void*, short, short, short);
void stone_fopAcM_posMoveF(daStone::Act_c*, void*);
void stone_daObj__posMoveF_stream(daStone::Act_c*, void*, void*, float, float);
void stone_dCcD_Stts__Init(void*, unsigned char, int, daStone::Act_c*);
void stone_fopKyM_createWpillar(cXyz*, int, float, float);
void stone_fopKyM_createMpillar(cXyz*, float);
void stone_daObj__SetCurrentRoomNo(daStone::Act_c*, void*);
void* stone_daObj__get_wind_spd(daStone::Act_c*, float);
void stone_set_drop_rot(daStone::Act_c*);
float stone_cSAngle__Radian(void*);
int stone_cM_rad2s(float);
void stone_cSAngle_subtract(void*, int);
void stone_cSAngle_add(void*, void*);
void stone_mDoMtx_YrotS(void*, short);
void stone_mDoMtx_XrotM(void*, short);
void stone_mDoMtx_ZrotM(void*, short);
void stone_mDoMtx_YrotM(void*, int);
void stone_PSMTXMultVecSR(void*, void*, void*);
float stone_cM_rnd();
void stone_cSAngle____amu(void*, float);
float stone_cM_rndFX(float);
float stone_cBgS__GroundCross(void*, void*);
float stone_sqrt(float);
void stone_PSVECScale(void*, void*, float);
int stone_dBgS__GetMtrlSndId(void*, void*);
int stone_dComIfGp_getReverb(signed char);
void stone_startActorSound(unsigned int, void*, int, int);
int stone_daStone__Act_c___create(daStone::Act_c*);
int stone_daStone__Act_c___execute(daStone::Act_c*);
int stone_daSea_ChkArea(float, float);
float stone_daSea_calcWave(float, float);
int stone_dCcD_GObjInf__ChkAtHit(void*);
void stone_dCcD_GObjInf__ClrAtHit(void*);
void stone_daStone__Act_c__damaged(daStone::Act_c*, int);
int stone_dCcD_GObjInf__ChkTgHit(void*);
void* stone_dCcD_GObjInf__GetTgHitObj(void*);
void stone_dCcD_GObjInf__ClrTgHit(void*);
void stone_daObj__HitSeStart(void*, signed char, void*, unsigned int);
void stone_daObj__HitEff_hibana(daStone::Act_c*, void*);
void stone_dBgS_Acch__CrrPos(void*, void*);
int stone_dBgS__ChkMoveBG_NoDABg(void*, void*);
int stone_dBgS__ChkMoveBG(void*, void*);
unsigned char stone_dBgS__GetPolyColor(void*, void*);
void* stone_dPa_control_c__set(void*, int, int, cXyz*, void*, void*, int, void*, int, void*, void*, void*);
void* stone_getObjectResource_2(void*, SafeString*, int);
void* stone_dPa_J3DmodelEmitter_c__dPa_J3DmodelEmitter_c(void*, void*, void*, int, void*, void*,
                                                         unsigned short, void*);
void stone_cLs_Addition(void*, void*);
void stone_csXyz__csXyz(csXyz*, int, short, int);
void stone_fopAcM_createItemFromTable(void*, unsigned int, unsigned int, signed char, void*, csXyz*, int,
                                      void*);
void stone_fopAcM_cancelCarryNow(daStone::Act_c*);
void stone_dVibration_c__StartShock(void*, short, int, cXyz*);
void stone_dSv_info_c__onSwitch(void*, unsigned int, signed char);
int stone_fopAcM_cullingCheck(daStone::Act_c*);
void stone_dCcD_Cyl__MoveCAtTg(void*, void*);
void stone_cCcS__Set(void*, void*);
void stone_dCcMassS_Mng__Set(void*, void*, int);
void stone_fopAcM_delete(daStone::Act_c*);
void stone_fopAc_ac_c__fopAc_ac_c(daStone::Act_c*);
void stone_dBgS_Acch__dBgS_Acch(void*);
void stone_dBgS_AcchCir__dBgS_AcchCir(void*);
void stone_cBgS_GndChk__cBgS_GndChk(void*);
void stone_cCcD_Stts_construct(void*);
void stone_dCcD_GStts__dCcD_GStts(void*);
void stone_dCcD_GObjInf__dCcD_GObjInf(void*);
void stone_cM3dGCyl_construct(void*);
void stone_cSAngle_construct(void*);
int stone_dComIfG_resLoad(void*, const char*);
int stone_fopAcM_entrySolidHeap(daStone::Act_c*, void*, unsigned int);
void stone_dCcD_Cyl__Set(void*, void*);
void stone_cM3dGCyl__SetR(void*, float);
void stone_cM3dGCyl__SetH(void*, float);

struct StonePlayView {
    u8 prefix[0x12A0];
    u8 background[0x26A4 - 0x12A0];
    u8 collision[0x4EF8 - 0x26A4];
    u8 mass[0x5134 - 0x4EF8];
    be<u8> stageName[8];
    u8 padding513C[0x5150 - 0x513C];
    be<u32> stageVtable;
    u8 padding5154[0x599C - 0x5154];
    u8 vibration[0x5AB0 - 0x599C];
    be<u32> particles;
    u8 padding5AB4[0x5AF8 - 0x5AB4];
    be<u32> camera;
    u8 padding5AFC[0x5B2C - 0x5AFC];
    be<u32> player;
};
WWHD_OFFSET(StonePlayView, player, 0x5B2C);
struct StoneCamera {
    u8 prefix[0x248];
    u8 camera;
};
struct StonePlayerVtable {
    u8 prefix[0xBC];
    be<u32> getGrabActorID;
};
struct StonePlayer {
    u8 prefix[0xB4];
    be<u32> vtable;
    u8 paddingB8[0x370 - 0xB8];
    be<f32> speedF;
    u8 padding374[0x3C0 - 0x374];
    be<u32> grabStatus;
};
struct StoneParticleManager {
    u8 prefix[0x130];
    be<u32> modelEmitters;
};
struct StoneAttack {
    u8 prefix[0x10];
    be<u32> type;
};
struct StoneSafeStringVtable {
    u8 prefix[0x14];
    be<u32> assureTermination;
};
struct StoneStageVtable {
    u8 prefix[0x15C];
    be<u32> getEnvironment;
};
struct StoneStageEnvironment {
    u8 prefix[0x12];
    be<u16> cullPoint;
};
struct StoneVectorWords {
    be<u32> xyz[3];
};
inline StoneVectorWords& stone_words(cXyz& v) { return gabi::object_at<StoneVectorWords>(gabi::ea(&v)); }
inline Mtx34& stone_matrix() { return gabi::object_at<Mtx34>(0x1048D0CC); }
inline be<f32>* stone_matrixValues() { return reinterpret_cast<be<f32>*>(&stone_matrix()); }
inline const cXyz& stone_zeroVector() { return gabi::object_at<cXyz>(0x101FFBA8); }
inline const cXyz& stone_baseZ() { return gabi::object_at<cXyz>(0x101FFBCC); }
inline cXyz& stone_axis() { return gabi::object_at<cXyz>(0x1046E044); }
struct StoneStaticState {
    be<f32> first, second;
    be<u8> firstFlag, secondFlag;
    u8 padding[2];
    be<u32> words[4];
};
inline StoneStaticState& stone_staticState() { return gabi::object_at<StoneStaticState>(0x1046E028); }

inline const be<u32>& stone_saveAddress() { return gabi::object_at<be<u32>>(0x101F84DC); }

inline const be<f32>& stone_lavaOffsetLimit() { return gabi::object_at<be<f32>>(0x1003DB9C); }

inline const be<f32>& stone_invalidHeight() { return gabi::object_at<be<f32>>(0x1003D92C); }

inline const be<f32>& stone_fallingWaterRate() { return gabi::object_at<be<f32>>(0x1003DF70); }

inline const be<f32>& stone_risingWaterRate() { return gabi::object_at<be<f32>>(0x1003DF6C); }

inline const be<f32>& stone_staticFirstInitial() { return gabi::object_at<be<f32>>(0x1003DFDC); }

inline const be<f32>& stone_staticSecondInitial() { return gabi::object_at<be<f32>>(0x1003DFE0); }

inline const be<u32>& stone_resourceAddress() { return gabi::object_at<be<u32>>(0x101F4F28); }

inline const be<f32>& stone_wallHeight() { return gabi::object_at<be<f32>>(0x1003D928); }

inline const be<s16>& stone_zeroAngle() { return gabi::object_at<be<s16>>(0x101FF354); }

inline const be<f32>& stone_zero() { return gabi::object_at<be<f32>>(0x1003D9A8); }

inline const be<f32>& stone_angleToRadians() { return gabi::object_at<be<f32>>(0x1003D9AC); }

inline const be<u32>& stone_quaternionAddress() { return gabi::object_at<be<u32>>(0x1048D3FC); }

inline const be<f32>& stone_windScale() { return gabi::object_at<be<f32>>(0x1003DF34); }

inline const be<f32>& stone_randomAngleRange() { return gabi::object_at<be<f32>>(0x1003DA4C); }

inline const be<f32>& stone_one() { return gabi::object_at<be<f32>>(0x1003D7A8); }

// Nullable guest pointer tokens preserve zero across the native ABI.
template <class T> inline T* stone_pointer(u32 address) { return gabi::at<T>(address); }
struct StoneResources;
struct StoneSaveInfo;
struct StoneParticleList;
struct StoneAcchCircleView {
    u8 prefix[0x14];
    u8 bounds;
};
inline void* stone_callbackPointer(const void* fn) { return gabi::at<void>(gabi::guest_fn(fn)); }
s32 CreateHeapCB(daStone::Act_c*);
u32 parameter(daStone::Act_c*, s32, s32);
inline void* stone_staticRecordA() { return gabi::at<void>(0x101D10C0); }
inline void* stone_staticRecordB() { return gabi::at<void>(0x101D10CC); }
inline void* stone_staticRecordC() { return gabi::at<void>(0x101D10D8); }
inline void* stone_cylinderSource() { return gabi::at<void>(0x1003E000); }
inline const char* stone_archiveName(u32 address) { return gabi::at<const char>(address); }

namespace daStone {
enum Mode_e { MODE_WAIT, MODE_CARRY, MODE_DROP, MODE_SINK };
enum DataFlag_e {
    DATAFLG_SINK_LAVA = 1,
    DATAFLG_ALWAYS_THROW = 2,
    DATAFLG_BACKGROUND_LIGHT = 4,
    DATAFLG_SAVE_SWITCH = 8,
    DATAFLG_KEEP_YAW = 0x10
};
enum Prm_e {
    PRM_TYPE_W = 3,
    PRM_TYPE_S = 24,
    PRM_MOVEBG_W = 2,
    PRM_MOVEBG_S = 6,
    PRM_CULL_W = 3,
    PRM_CULL_S = 28,
    PRM_ITEMNO_W = 6,
    PRM_ITEMNO_S = 0,
    PRM_ITEMSAVE_W = 7,
    PRM_ITEMSAVE_S = 16,
    PRM_SWSAVE_W = 8,
    PRM_SWSAVE_S = 8,
    TYPE_COUNT = 5
};
} // namespace daStone

inline be<u8>& stone_stringByte(u32 address, u32 index) { return gabi::object_at<be<u8>>(address + index); }

WWHD_SIZE(StonePlayView, 0x5B30);
WWHD_SIZE(StoneCamera, 0x249);
WWHD_SIZE(StonePlayerVtable, 0xC0);
WWHD_SIZE(StonePlayer, 0x3C4);
WWHD_SIZE(StoneParticleManager, 0x134);
WWHD_SIZE(StoneAttack, 0x14);
WWHD_SIZE(StoneSafeStringVtable, 0x18);
WWHD_SIZE(StoneStageVtable, 0x160);
WWHD_SIZE(StoneStageEnvironment, 0x14);
WWHD_SIZE(StoneVectorWords, 0xC);
WWHD_SIZE(StoneStaticState, 0x1C);
WWHD_SIZE(StoneAcchCircleView, 0x15);

// A nonleaf guest callee may save LR at caller SP+4. Keep the payload
// above its 32-byte linkage area throughout every call using the object.
struct StoneLinkage {
    u8 bytes[32];
};
template <class T> class StoneLocal {
    gabi::Local<T> payload;
    gabi::Local<StoneLinkage> linkage;

  public:
    T* get() const { return payload.get(); }
    T* operator->() const { return get(); }
};

WWHD_SIZE(StoneLinkage, 32);
