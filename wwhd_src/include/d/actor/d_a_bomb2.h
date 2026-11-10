/* Bomb flower bomb: tww member names and verified WWHD layouts.
 * HD: enlarged actor base and animation controllers shift the GC members. The point light
 * adds four bytes; numbered field names retain their GameCube spelling. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
namespace daBomb2 {
// HD: particle fields used by both fuse callbacks and the explosion effects.
struct Bomb2Emitter {
  u8 prefix[0x5C];
  be<s32> mMaxFrame;
  be<s16> mLifeTime;
  u8 padding62[0x6C-0x62];
  be<f32> mInitialVelAxis, mInitialVelDir;
  u8 padding74[0x1AC-0x74];
  be<u32> mAlivePtclHead;
  u8 padding1B0[0x1E4-0x1B0];
  be<u32> mpEmitterCallBack;
  u8 padding1E8[0x22C-0x1E8];
  cXyz mGlobalTranslation, mGlobalParticleScale;
  u8 padding244[0x254-0x244];
  be<u32> mStatus;
  u8 padding258[0x262-0x258];
  be<u8> mVolumeType;
  u8 padding263;
};
struct Bomb2Particle { u8 prefix[0x10]; cXyz mPosition; };
struct Bomb2ParticleNode { be<u32> mObject; u8 padding[8]; be<u32> mNext; };
WWHD_OFFSET(Bomb2Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Bomb2Emitter, mStatus, 0x254);
WWHD_OFFSET(Bomb2Emitter, mVolumeType, 0x262);
WWHD_SIZE(Bomb2Emitter, 0x264);
WWHD_SIZE(Bomb2Particle, 0x1C);
WWHD_SIZE(Bomb2ParticleNode, 0x10);

struct Bomb2PlayerView : fopAc_ac_c {
  u8 padding3AC[0x3C0-0x3AC];
  be<u32> mResetFlg0;
};
struct Bomb2PlayerVtable { u8 prefix[0xBC]; gabi::gfn getGrabActorID; };
struct Bomb2ShapeVtable { u8 prefix[0x9C]; gabi::gfn GetNVec; };
struct Bomb2Shape { u8 prefix[0x1C]; gptr<Bomb2ShapeVtable> __vtbl; };
struct Bomb2HitVtable { u8 prefix[0x2C]; gabi::gfn GetShapeAttr; };
struct Bomb2HitView { u8 prefix[0x3C]; gptr<Bomb2HitVtable> __vtbl; };
struct Bomb2CameraView { u8 prefix[0xDC]; cXyz mEye; u8 paddingE8[0x234-0xE8]; csXyz mAngle; };
struct Bomb2WindInfluence {
  cXyz mPos, mDir;
  be<f32> mRadius, mStrength, field_0x20;
  be<s32> mRegistIdx;
  be<u8> mbConstant;
  u8 padding[3];
};
WWHD_SIZE(Bomb2WindInfluence, 0x2C);
struct Bomb2ModelView {
  u8 prefix[0xAC]; be<u32> mpModelData; u8 padB0[0xBC-0xB0];
  cXyz mBaseScale; be<f32> mBaseTRMtx[12];
};
struct Bomb2ModelDataView {
  u8 prefix[8]; be<u32> mJointTree; u8 padC[0x48-0xC]; be<u32> mpTevRegAnimator;
};
struct Bomb2JointTreeView { u8 prefix[0x14]; be<u32> mpTransformAnimator; };
struct Bomb2Plane { cXyz mNormal; };
struct Act_c;
using Bomb2PtMf = gabi::ptmf<void (Act_c::*)()>;
struct Bomb2StartTable { Bomb2PtMf mProc[3]; };
struct Bomb2ModeTable { Bomb2PtMf mProc[4]; };
struct Bomb2VectorWords { be<u32> mWords[3]; };
struct Bomb2RandomObjects { be<u8> mSecond, mFirst; };
struct Bomb2Registration { be<u32> mWords[3]; };
struct Bomb2Word { be<u32> mValue; };
struct Bomb2Byte { be<u8> mValue; };
struct Bomb2Float { be<f32> mValue; };
struct Bomb2Angles { be<f32> mNegativePi,mPositivePi; };
struct Bomb2Words { be<u32> mWords[4]; };
struct Bomb2FuseOffset { cXyz mOffset; be<u32> mGuard; };
struct Bomb2CullSize { be<f32> mRadius,mHeight; };
struct Bomb2SphereSource { u8 prefix[0x3C]; be<f32> mRadius; };
struct Bomb2SinCosTable { be<f32> mSinCos[8192][2]; };
WWHD_OFFSET(Bomb2ModelView, mBaseScale, 0xBC);
WWHD_OFFSET(Bomb2ModelView, mBaseTRMtx, 0xC8);
WWHD_SIZE(Bomb2PtMf, 8);
WWHD_SIZE(Bomb2ModelView, 0xF8);
WWHD_SIZE(Bomb2ModelDataView, 0x4C);
WWHD_SIZE(Bomb2JointTreeView, 0x18);
WWHD_SIZE(Bomb2Plane, 0xC);
WWHD_SIZE(Bomb2SinCosTable, 0x10000);

struct Brk_c : mDoExt_baseAnm {
  u8 bytes[0x68];
  void entry(J3DModelData*, f32);
};
struct Env_c {
  LIGHT_INFLUENCE mPntLight;
  Bomb2WindInfluence mPntWind;
  be<u8> mStep; /* was field_0x4C: Env_c::proc 020C8308: 0 raises mIntensity to 1, 1 lowers it to 0, each step advances when done; Env_c::is_end 020C82F4 returns it > 1; Env_c::set 020C81A4 clears it */
  u8 pad[3];
  be<f32> mIntensity; /* was field_0x50: Env_c::proc 020C8308: point wind strength, point light power (1500 * it) and the ambient/bg colour add factor; eased 0 -> 1 -> 0 by mStep; Env_c::set 020C81A4 clears it */
  void set(const cXyz *);
  void clean();
  bool is_end();
  void proc(const cXyz *);
};
struct FuseSmokeCB_c {
  be<u32> vt;
  be<s16> mPrevLife; /* was field_0x04: FuseSmokeCB_c::executeAfter 020C8564: last frame's particle life time (stored at the end), interpolated towards the new one over the in-between particles; setOldPosP 020C8518 sets 20 */
  u8 pad[2];
  gptr<const cXyz> mpPos, mpOldPos, mpOldOldPos; /* mpOldPos was field_0x0C: FuseSmokeCB_c::setOldPosP 020C8518 first argument (mFuseOldPos); executeAfter 020C8564 interpolates the smoke from it to mpPos; mpOldOldPos was field_0x10: FuseSmokeCB_c::setOldPosP 020C8518 second argument (mFuseOldOldPos); executeAfter 020C8564 uses mpOldPos - it as the start tangent of the curve */
  gptr<void> mpEmitter;
  void setOldPosP(const cXyz *, const cXyz *);
  void setup(void*, const cXyz*, const csXyz*, s8);
  void deleteCallBack();
  void executeAfter(void *);
};
struct FuseSparksCB_c {
  be<u32> vt;
  gptr<const cXyz> mpPos;
  gptr<void> mpEmitter;
  void setup(void*, const cXyz*, const csXyz*, s8);
  void deleteCallBack();
  void execute(void *);
};
enum { fpcNm_PLAYER_e = 0xA8 };
enum Start_e { Start_UNK0_e, Start_UNK1_e, Start_UNK2_e };
enum State_e { Mode_WAIT_e, Mode_CARRY_e, Mode_EXPLODE_e, Mode_SINK_e };
enum Prm_e { PRM_1_W = 1, PRM_1_S = 8, PRM_2_W = 2, PRM_2_S = 0 };
enum VbakHResource_e {
  dRes_INDEX_VBAKH_BCK_VBAKM_e = 7,
  dRes_INDEX_VBAKH_BDL_VBAKM_e = 12,
  dRes_INDEX_VBAKH_BRK_VBAKM_e = 16,
};
struct Act_c : fopAc_ac_c {
  /* 0x3AC */ request_of_phase_process_class mPhase;
  /* 0x3B4 */ gptr<J3DModel> mpModel;
  /* 0x3B8 */ mDoExt_bckAnm mBck0;
  /* 0x444 */ Brk_c mBrk0;
  /* 0x4BC */ dBgS_Acch mAcch;
  /* 0x680 */ dBgS_AcchCir mCir;
  /* 0x6C0 */ dBgS_GndChk mGndChk; // HD: lava-only filter.
  /* 0x714 */ be<f32> mLavaY;  /* was field_0x51C: crr_init 020C8B58 sets -inf; bgCrrPos_lava 020C8C78: cBgS_GroundCross of the lava (Yogan) ground check mGndChk; chk_lava_in 020CA790 tests it */
  /* 0x718 */ be<f32> mWaterY;  /* was field_0x520: crr_init 020C8B58 sets -inf; bgCrrPos_water 020C8CE4 stores mAcch's water height or the daSea wave height (-inf when out of water); eff_water_splash 020CA9F8 puts the splash there */
  /* 0x71C */ be<u8> field_0x524;
  /* 0x71D */ be<u8> mbWaterIn;
  /* 0x71E */ be<u8> mWaterIsSea;  /* was field_0x526: bgCrrPos_water 020C8CE4 sets 1 when mWaterY comes from the sea wave (daSea_calcWave), 0 when from mAcch's water polygon or no water; se_fall_water 020CA91C then passes no water polygon */
  /* 0x71F */ u8 pad71F;
  /* 0x720 */ be<f32> mSinkWaterY;  /* was field_0x528: crr_init 020C8B58 sets -inf; mode_sink_init 020CAFB4: fopAcM_getWaterY at the bomb; mode_sink 020CB218 moves the bomb by the water height change and keeps the new height */
  /* 0x724 */ dCcD_Stts mStts;
  /* 0x760 */ dCcD_Sph mSph;
  /* 0x88C */ be<s32> mState;
  /* 0x890 */ be<s32> mSinkTimer;  /* was field_0x698: mode_sink_init 020CAFB4 sets 4; mode_sink 020CB218 counts it down and sets mDeleteReq when it runs out */
  /* 0x894 */ FuseSmokeCB_c mSmoke;
  /* 0x8AC */ FuseSparksCB_c mSparks;
  /* 0x8B8 */ cXyz mFusePos;  /* was field_0x6C0: eff_fuse_start 020CA4F0: current.pos + 60 in y, the position of the sparks and smoke emitters (dPa_control_c::set); eff_fuse_move 020C8FB4: l_currentMtx times the fuse offset (0, 60, 5) */
  /* 0x8C4 */ cXyz mFuseOldPos;  /* was field_0x6CC: eff_fuse_move 020C8FB4 copies last frame's mFusePos here; eff_fuse_start 020CA4F0 passes it as the first FuseSmokeCB_c::setOldPosP argument */
  /* 0x8D0 */ cXyz mFuseOldOldPos;  /* was field_0x6D8: eff_fuse_move 020C8FB4 copies last frame's mFuseOldPos here; second FuseSmokeCB_c::setOldPosP argument in eff_fuse_start 020CA4F0 */
  /* 0x8DC */ Env_c mEnv;
  /* 0x934 */ be<s32> mBombTimer;
  /* 0x938 */ be<s32> mExplodeCcTimer;  /* was field_0x73C: mode_explode_init 020CA3A4 sets 4; cc_set 020C9D04 counts it down while exploding and sets the attack sphere only while it stays above 0; mode_explode 020CAE14 waits for 0 (and mEnv.is_end) before mDeleteReq */
  /* 0x93C */ be<u8> mDeleteReq;  /* was field_0x740: create_init 020C9218 clears it; set by mode_explode 020CAE14 and mode_sink 020CB218; _execute 020C9E78 deletes the actor (fopAcM_delete) when set; mode_wait 020CB044 bounces only while clear */
  /* 0x93D */ be<u8> mThrown;  /* was field_0x741: mode_carry 020CAE6C sets 1 when released with speedF > 0; bound 020CA7C0 clears it once the bounce is below 19.5; chk_exp_cc_nut 020CAA6C / chk_exp_bg_nut 020CAB88 light the fuse on any contact while it is set */
  /* 0x93E */ be<u8> mExplodeEffDone;  /* was field_0x742: eff_explode 020CA2A4 returns if set, else sets it and starts the explosion effects/sound once; is_draw 020C9F18 hides the bomb when set */
  /* 0x93F */ be<u8> mWaterExplodeEffDone;  /* was field_0x743: eff_explode_water 020CA0F8 returns if set, else creates the water pillar and sets it; is_draw 020C9F18 hides the bomb when set */
  /* 0x940 */ be<u8> mFuseLit;  /* was field_0x744: eff_fuse_start 020CA4F0 sets it (returns early if set), eff_fuse_init 020C8F2C / eff_fuse_end 020C95F4 clear it; eff_fuse_move 020C8FB4 plays the ignition sound while set; carry_fuse_start 020CAD9C returns if set */
  /* 0x941 */ be<u8> mEaten;  /* was field_0x745: our decomp's accessors daBomb2::Act_c::chk_eat 020CBA70 (returns it) and set_eat 020CBA78 (sets 1) in d_a_bomb_static.cpp; mode_wait 020CB044 skips posMoveF while set */
  /* 0x942 */ u8 pad942[2];
  /* 0x944 */ cXyz mWindVec;
  /* 0x950 */ be<f32> mTensorMtx[12];  /* was field_0x754: tensor_init 020C8EF4 sets the identity; set_vib_tensor 020C9BB8 builds it (PSMTXQuat); set_mtx 020C9074 concatenates it into the model matrix */
  /* 0x980 */ be<f32> mVibTargetZ;  /* was field_0x784: vib_proc 020C9B18: the spring pulls mVibZ towards it; tensor_wait_ground 020CB2BC sets the ground normal's z * 1.5, the other tensor_* modes 0 */
  /* 0x984 */ be<f32> mVibTargetX;  /* was field_0x788: vib_proc 020C9B18: the spring pulls mVibX towards it; tensor_wait_ground 020CB2BC sets the ground normal's x * 1.5, the other tensor_* modes 0 */
  /* 0x988 */ be<f32> mVibZ;  /* was field_0x78C: vib_proc 020C9B18 adds mVibVelZ; set_vib_tensor 020C9BB8 uses it as the z of the tilt direction (quat_rotBaseY2) */
  /* 0x98C */ be<f32> mVibX;  /* was field_0x790: vib_proc 020C9B18 adds mVibVelX; set_vib_tensor 020C9BB8 uses it as the x of the tilt direction (quat_rotBaseY2) */
  /* 0x990 */ be<f32> mVibVelZ;  /* was field_0x794: vib_proc 020C9B18: velocity of mVibZ (old + 2 * (impulse - 0.03 * (mVibZ - target)), times 1 - mVibDamping) */
  /* 0x994 */ be<f32> mVibVelX;  /* was field_0x798: vib_proc 020C9B18: velocity of mVibX, same update as mVibVelZ */
  /* 0x998 */ be<f32> mVibImpulseZ;  /* was field_0x79C: tensor_wait_drop 020CB34C sets speed.z * -0.005 while mVibImpulseTimer > 0; vib_proc 020C9B18 adds it to the velocity once and clears it */
  /* 0x99C */ be<f32> mVibImpulseX;  /* was field_0x7A0: tensor_wait_drop 020CB34C sets speed.x * -0.005 while mVibImpulseTimer > 0; vib_proc 020C9B18 adds it once and clears it */
  /* 0x9A0 */ be<f32> mVibDamping;  /* was field_0x7A4: vib_proc 020C9B18 scales the velocity by 1 - it; tensor_wait_drop 020CB34C sets 0.1, tensor_wait_ground 020CB2BC / tensor_carry 020CB3AC 0.4 */
  /* 0x9A4 */ be<s32> mVibImpulseTimer;  /* was field_0x7A8: set_wind_vec 020C9674 (wind push) and mode_carry 020CAE6C (throw) set 2; vib_proc 020C9B18 counts it down; tensor_wait_drop 020CB34C sets the impulse only while it is above 0 */
  bool create_heap_nut();
  u32 create_heap();
  void crr_init();
  void cc_init();
  void start_explode_instant();
  void start_explode_interval();
  void start_carry();
  void start_proc_call();
  void create_init();
  cPhs_State _create();
  bool _delete();
  void set_mtx();
  void init_mtx();
  void cc_set();
  void camera_lockoff();
  void posMoveF();
  void bgCrrPos();
  void bgCrrPos_lava();
  void bgCrrPos_water();
  u8 chk_water_in();
  bool chk_lava_in();
  void setRoomInfo();
  void bound(f32 value);
  void set_nut_exp_interval();
  void anm_play();
  void set_wind_vec();
  void eff_explode();
  void eff_explode_normal(const csXyz *angle);
  void eff_explode_water();
  void eff_fuse_init();
  void eff_fuse_start();
  void eff_fuse_move();
  void eff_fuse_end();
  void eff_water_splash();
  void se_fall_water();
  void se_explode();
  void se_explode_water();
  void se_ignition();
  void set_sound_env(s32 a, s32 b);
  bool chk_exp_cc_nut();
  u32 chk_exp_cc();
  bool chk_exp_bg_nut();
  bool chk_exp_bg();
  bool chk_exp_timer();
  bool chk_sink_bg_nut();
  bool chk_sink_bg();
  u8 chk_exp_pre();
  u8 chk_exp_post();
  u8 chk_sink_post();
  void set_real_shadow_flag();
  void carry_fuse_start();
  void on_carry();
  void off_carry();
  void mode_wait_init();
  void mode_wait();
  void mode_carry_init();
  void mode_carry();
  void mode_explode_init();
  void mode_explode();
  void mode_sink_init();
  void mode_sink();
  void mode_proc_call();
  void tensor_init();
  void vib_init();
  void vib_proc();
  void set_vib_tensor();
  void tensor_wait();
  void tensor_wait_drop();
  void tensor_wait_ground();
  void tensor_carry();
  void tensor_explode();
  void tensor_sink();
  void tensor_proc_call();
  bool _execute();
  bool is_draw();
  void draw_nut();
  bool _draw();
};
WWHD_OFFSET(Env_c, mPntWind, 0x24);
WWHD_OFFSET(Env_c, mStep, 0x50);
WWHD_OFFSET(Env_c, mIntensity, 0x54);
WWHD_SIZE(Env_c, 0x58);
WWHD_OFFSET(Act_c, mAcch, 0x4BC);
WWHD_OFFSET(Act_c, mSph, 0x760);
WWHD_OFFSET(Act_c, mEnv, 0x8DC);
WWHD_OFFSET(Act_c, mBombTimer, 0x934);
WWHD_SIZE(Act_c, 0x9A8);
struct Bomb2SafeString { be<u32> a, b; };
struct Bomb2Quaternion { be<f32> x, y, z, w; };
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mpModel, 0x3B4);
WWHD_OFFSET(Act_c, mBck0, 0x3B8);
WWHD_OFFSET(Act_c, mBrk0, 0x444);
WWHD_OFFSET(Act_c, mCir, 0x680);
WWHD_OFFSET(Act_c, mGndChk, 0x6C0);
WWHD_OFFSET(Act_c, mLavaY, 0x714);
WWHD_OFFSET(Act_c, mWaterY, 0x718);
WWHD_OFFSET(Act_c, field_0x524, 0x71C);
WWHD_OFFSET(Act_c, mbWaterIn, 0x71D);
WWHD_OFFSET(Act_c, mWaterIsSea, 0x71E);
WWHD_OFFSET(Act_c, pad71F, 0x71F);
WWHD_OFFSET(Act_c, mSinkWaterY, 0x720);
WWHD_OFFSET(Act_c, mStts, 0x724);
WWHD_OFFSET(Act_c, mState, 0x88C);
WWHD_OFFSET(Act_c, mSinkTimer, 0x890);
WWHD_OFFSET(Act_c, mSmoke, 0x894);
WWHD_OFFSET(Act_c, mSparks, 0x8AC);
WWHD_OFFSET(Act_c, mFusePos, 0x8B8);
WWHD_OFFSET(Act_c, mFuseOldPos, 0x8C4);
WWHD_OFFSET(Act_c, mFuseOldOldPos, 0x8D0);
WWHD_OFFSET(Act_c, mExplodeCcTimer, 0x938);
WWHD_OFFSET(Act_c, mDeleteReq, 0x93C);
WWHD_OFFSET(Act_c, mThrown, 0x93D);
WWHD_OFFSET(Act_c, mExplodeEffDone, 0x93E);
WWHD_OFFSET(Act_c, mWaterExplodeEffDone, 0x93F);
WWHD_OFFSET(Act_c, mFuseLit, 0x940);
WWHD_OFFSET(Act_c, mEaten, 0x941);
WWHD_OFFSET(Act_c, pad942, 0x942);
WWHD_OFFSET(Act_c, mWindVec, 0x944);
WWHD_OFFSET(Act_c, mTensorMtx, 0x950);
WWHD_OFFSET(Act_c, mVibTargetZ, 0x980);
WWHD_OFFSET(Act_c, mVibTargetX, 0x984);
WWHD_OFFSET(Act_c, mVibZ, 0x988);
WWHD_OFFSET(Act_c, mVibX, 0x98C);
WWHD_OFFSET(Act_c, mVibVelZ, 0x990);
WWHD_OFFSET(Act_c, mVibVelX, 0x994);
WWHD_OFFSET(Act_c, mVibImpulseZ, 0x998);
WWHD_OFFSET(Act_c, mVibImpulseX, 0x99C);
WWHD_OFFSET(Act_c, mVibDamping, 0x9A0);
WWHD_OFFSET(Act_c, mVibImpulseTimer, 0x9A4);
WWHD_SIZE(Brk_c, 0x78);
WWHD_SIZE(FuseSmokeCB_c, 0x18);
WWHD_SIZE(FuseSparksCB_c, 0xC);
WWHD_OFFSET(FuseSmokeCB_c, mpPos, 8);
WWHD_OFFSET(FuseSmokeCB_c, mpEmitter, 0x14);
WWHD_OFFSET(Bomb2PlayerView, mResetFlg0, 0x3C0);
WWHD_OFFSET(Bomb2CameraView, mAngle, 0x234);
WWHD_SIZE(Bomb2StartTable, 0x18);
WWHD_SIZE(Bomb2ModeTable, 0x20);
WWHD_SIZE(Bomb2VectorWords, 0xC);
WWHD_SIZE(Bomb2RandomObjects, 2);
WWHD_SIZE(Bomb2Registration, 0xC);
WWHD_SIZE(Bomb2Word, 4);
WWHD_SIZE(Bomb2Byte, 1);
WWHD_SIZE(Bomb2Float, 4);
WWHD_SIZE(Bomb2Angles, 8);
WWHD_SIZE(Bomb2Words, 0x10);
WWHD_SIZE(Bomb2FuseOffset, 0x10);
WWHD_SIZE(Bomb2CullSize, 8);
WWHD_SIZE(Bomb2SphereSource, 0x40);

} // namespace daBomb2
// HD: private exact-ABI bridges retain full GHS registers and stack signatures.
void bomb2_PSMTXIdentity(u32);
u32 bomb2_daBomb2__Act_c__create_heap_nut_u32(daBomb2::Act_c *);
u8 bomb2_chk_exp_bg_u8(daBomb2::Act_c *);
u8 bomb2_chk_sink_bg_u8(daBomb2::Act_c *);
void bomb2_dCcD_Stts__Init(dCcD_Stts *, int, int, daBomb2::Act_c *);
void bomb2_dCcD_Sph__Set(dCcD_Sph *, int);
void bomb2_dKy_efplight_set(daBomb2::Env_c *);
void bomb2_dKyw_pntwind_set(u32);
void bomb2_dKy_actor_addcol_set(int, int, int, float);
void bomb2_dKy_efplight_cut(daBomb2::Env_c *);
void bomb2_dKyw_pntwind_cut(u32);
f32 bomb2_PSVECSquareDistance_f32(const cXyz *, u32);
f32 bomb2_sqrtf_f32(f32);
void bomb2_dKy_actor_addcol_amb_set(int, int, int, f32);
void bomb2_dKy_bg_addcol_amb_set(int, int, int, f32);
void bomb2_dKy_bg_addcol_dif_set(int, int, int, f32);
f32 bomb2_cLib_addCalc_f32(be<f32> *, float, float, float, float);
u32 bomb2_operator_new_u32(int);
u32 bomb2_Act_c_create_u32(daBomb2::Act_c *);
u32 bomb2_Act_c_delete_u32(daBomb2::Act_c *);
u32 bomb2_Act_c_execute_u32(daBomb2::Act_c *);
u32 bomb2_Act_c_draw_u32(daBomb2::Act_c *);
void bomb2___dl(void *);
f32 bomb2_PSVECSquareDistance_f32(const cXyz *, const cXyz *);
u32 bomb2_JPABaseEmitter__createParticle_u32(u32);
f32 bomb2_cBgS__GroundCross_f32(u32, u32);
s32 bomb2_daSea_ChkArea_s32(f32, f32);
f32 bomb2_daSea_calcWave_f32(f32, f32);
s32 bomb2_dBgS__GetRoomId_s32(u32, u32);
u32 bomb2_dBgS__GetPolyColor_u32(u32, u32);
void bomb2_dBgS_Acch__CrrPos(dBgS_Acch *, u32);
u32 bomb2_dKy_getEnvlight_u32();
void bomb2_dScnKy_env_light_c__settingTevStruct(u32, int, cXyz *, dKy_tevstr_c *);
void bomb2_dScnKy_env_light_c_setLightTevColorType(u32, J3DModel *, dKy_tevstr_c *);
s32 bomb2_dComIfGp_getReverb_s32(s8);
void bomb2_mDoAud_seStartLevel(int, cXyz *, int, s32);
void bomb2_dKy_Sound_set(cXyz *, s32, unsigned int, s32);
void bomb2_fopKyM_createWpillar(cXyz *, int, float, float);
void bomb2_dPa_control_c__set(u32, int, int, cXyz *, const csXyz *, cXyz *, int, int, int, int, int, int);
void bomb2_dPa_control_c__setBombSmoke(u32, int, cXyz *, int, cXyz *, int);
void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, int, int, int, int, int);
void bomb2_dCamera_c__ForceLockOff(u32, unsigned int);
void bomb2_fopAcM_cancelCarryNow(daBomb2::Act_c *);
void bomb2_dVibration_c__StartShock(u32, int, int, cXyz *);
void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, daBomb2::FuseSparksCB_c *, int, int, int, int);
void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, daBomb2::FuseSmokeCB_c *, s32, u32, u32, int);
u32 bomb2_dRes_control_c__getRes_u32(unsigned int, daBomb2::Bomb2SafeString *, int);
void bomb2_JUTAssertion__getSDevice(int, int, int);
u32 bomb2_mDoExt_J3DModel__create_u32(u32, int, int);
u32 bomb2_mDoExt_bckAnm__init_u32(mDoExt_bckAnm *, u32, u32, int, int, int, int, int, float);
u32 bomb2_mDoExt_brkAnm_init_u32(daBomb2::Brk_c *, u32, u32, int, int, int, int, int, int, float);
void bomb2_fopAcM_setCullSizeBox(daBomb2::Act_c *, f32, float, f32, f32, f32, f32);
u32 bomb2_PrmAbstract_u32(daBomb2::Act_c *, int, int);
void bomb2_PSMTXMultVec(int, int, cXyz *);
void bomb2_PSMTXTrans(int, f32, f32, f32);
void bomb2_PSMTXConcat(int, u32, int);
void bomb2_mDoMtx_ZXYrotM(int, s16, s16, s16);
void bomb2_mDoExt_baseAnm__play(mDoExt_bckAnm *);
void bomb2_mDoExt_baseAnm__play(daBomb2::Brk_c *);
u32 bomb2_cBgS__GetTriPla_u32(u32, unsigned short, unsigned short);
u32 bomb2_fopAcM_getWaterY_u32(cXyz *, gabi::be<float> *);
void bomb2_daObj__quat_rotBaseY2(daBomb2::Bomb2Quaternion *, cXyz *);
void bomb2_PSMTXQuat(u32, daBomb2::Bomb2Quaternion *);
void bomb2___register_global_object(int);
void bomb2_randomSecondObject_ct(int);
void bomb2_randomFirstObject_ct(int);
void bomb2_dCcD_Sph_dtor(u32, int);
void bomb2_dCcD_Stts_dtor(u32, int);
void bomb2_cBgS_Chk___cBgS_Chk(u32, int);
void bomb2_cM3dGCir_dtor(u32, int);
void bomb2_dBgS_Acch_dtor(u32, int);
void bomb2_J3DFrameCtrl_dtor(u32, int);
void bomb2_fopAc_ac_c___fopAc_ac_c(daBomb2::Act_c *, int);
void bomb2___dl(daBomb2::Act_c *);
void bomb2_fopAc_ac_c__fopAc_ac_c(daBomb2::Act_c *);
void bomb2_J3DFrameCtrl__init(u32, int);
void bomb2_J3DAnmCurve_ct(u32);
void bomb2_mDoExt_brkAnm__init(u32);
void bomb2_dBgS_Acch__dBgS_Acch(u32);
void bomb2_dBgS_AcchCir__dBgS_AcchCir(u32);
void bomb2_cBgS_GndChk__cBgS_GndChk(u32);
void bomb2_cCcD_Stts_ct(u32);
void bomb2_dCcD_GStts__dCcD_GStts(u32);
void bomb2_dCcD_Sph_ct(u32);
void bomb2_Env_c_ctor(u32);
u32 bomb2_dComIfG_resLoad_u32(u32, int);
u32 bomb2_fopAcM_entrySolidHeap_u32(daBomb2::Act_c *, int, int);
void bomb2_fopAcM_delete(daBomb2::Act_c *);
f32 bomb2_PSVECSquareMag_f32(cXyz *);
void bomb2_daObj__posMoveF_grade(daBomb2::Act_c *, dCcD_Stts *, cXyz *, u32, int, float, float, float, float);
void bomb2_fopAcM_posMoveF(daBomb2::Act_c *, dCcD_Stts *);
u32 bomb2_dBgS__GetMtrlSndId_u32(u32, u32);
u32 bomb2_dComIfGp_getReverb_u32(s32);
void bomb2_mDoAud_seStartLevel(int, u32, u32, u32);
u32 bomb2_dCcD_GObjInf__ChkTgHit_u32(dCcD_Sph *);
u32 bomb2_dCcD_GObjInf__GetTgHitObj_u32(dCcD_Sph *);
void bomb2_dCcD_GObjInf__ClrTgHit(dCcD_Sph *);
u32 bomb2_dCcD_GObjInf__ChkCoHit_u32(dCcD_Sph *);
void bomb2_dCcD_GObjInf__ClrCoHit(dCcD_Sph *);
u32 bomb2_daBomb2__Act_c__chk_exp_cc_nut_u32(daBomb2::Act_c *);
void bomb2_fopAcM_getWaterY(cXyz *, be<f32> *);
void bomb2_daObj__make_land_effect(daBomb2::Act_c *, u32, float);
void bomb2_PSMTXCopy(u32, int);
void bomb2_mDoMtx_stack_c__scaleM(f32, f32, f32);
void bomb2_dCcD_GStts__Move(u32);
void bomb2_cM3dGSph__SetR(u32, f32);
void bomb2_cM3dGSph__SetC(u32, cXyz *);
void bomb2_cCcS__Set(u32, dCcD_Sph *);
void bomb2_dCcMassS_Mng__Set(u32, dCcD_Sph *, int);
void bomb2_PSVECScale(cXyz *, cXyz *, float);
void bomb2_PSVECScale(cXyz *, cXyz *, f32);
u32 bomb2_dCcD_GAtTgCoCommonBase__GetAc_u32(u32);
s16 bomb2_cM_atan2s_s16(f32, f32);
void bomb2_cXyz____ml(cXyz *, cXyz *, f32);
void bomb2_cXyz____pl(cXyz *, cXyz *, cXyz *);
u32 bomb2_create_heap_u32(daBomb2::Act_c *);
