/* Bomb actor and fuse callbacks, using tww names with verified WWHD offsets.
 * HD: the actor base, embedded animation controllers and extra bomb model enlarge the GC actor
 * to 0xB50. Numbered fields retain tww's GC names; asserts below specify their HD offsets.
 * Private views preserve the full-register callback ABI. */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
// HD: particle fields used by both fuse callbacks and the explosion effects.
struct BombEmitter {
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
struct BombParticle { u8 prefix[0x10]; cXyz mPosition; };
struct BombParticleNode { be<u32> mObject; u8 padding[8]; be<u32> mNext; };
WWHD_OFFSET(BombEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BombEmitter, mStatus, 0x254);
WWHD_OFFSET(BombEmitter, mVolumeType, 0x262);
WWHD_SIZE(BombEmitter, 0x264);
WWHD_SIZE(BombParticle, 0x1C);
WWHD_SIZE(BombParticleNode, 0x10);

// HD: Link's bomb animation is an embedded controller; its curve callback keeps full registers.
struct BombCurve {
  be<f32> mValue, mScale, mOffset;
  u8 padding[4];
  gabi::gfn mCalc;
  be<u32> mObject;
};
struct BombLinkBrk {
  u8 prefix[4]; be<f32> mFrame;
  be<s16> mStart, mEnd;
  u8 paddingC[4];
  be<u32> mpAnm;
  u8 padding14[0x20-0x14];
  be<u32> mpCurve;
};
struct BombLinkView {
  u8 prefix[0x44D4];
  BombLinkBrk mBombBrk;
  u8 padding44F8[0x690C-0x44F8];
  be<u8> mBombCnt;
};
struct BombPlayerView : fopAc_ac_c {
  u8 padding3AC[0x3C0-0x3AC];
  be<u32> mResetFlg0;
};
struct BombPlayerVtable { u8 prefix[0xBC]; gabi::gfn getGrabActorID; };
struct BombShapeVtable { u8 prefix[0x9C]; gabi::gfn GetNVec; };
struct BombShape { u8 prefix[0x1C]; gptr<BombShapeVtable> __vtbl; };
struct BombHitVtable { u8 prefix[0x2C]; gabi::gfn GetShapeAttr; };
struct BombHitView { u8 prefix[0x3C]; gptr<BombHitVtable> __vtbl; };
struct BombAnmVtable { u8 prefix[0x14]; gabi::gfn getFrameMax; };
struct BombAnmTransform { be<f32> mFrame; gptr<BombAnmVtable> __vtbl; };
struct BombCameraView { u8 prefix[0xDC]; cXyz mEye; u8 paddingE8[0x234-0xE8]; csXyz mAngle; };
WWHD_SIZE(BombCurve, 0x18);
WWHD_SIZE(BombLinkBrk, 0x24);
WWHD_OFFSET(BombLinkView, mBombBrk, 0x44D4);
WWHD_OFFSET(BombLinkView, mBombCnt, 0x690C);
WWHD_OFFSET(BombPlayerView, mResetFlg0, 0x3C0);
WWHD_OFFSET(BombCameraView, mAngle, 0x234);

struct daBomb_fuseSmokeEcallBack {
  be<u32> __vtbl;
  be<s16> mPrevLife;  /* was field_0x04: daBomb_fuseSmokeEcallBack::executeAfter 020C6860 stores this frame's particle life in it; the next frame steps the in-between particles' life from it to the new one; setFuseEffect 020C584C sets 20 (tww set() the same) */
  u8 _6[2];
  gptr<cXyz> mpPos;
  gptr<cXyz> mpOldPos;  /* was field_0x0C: setFuseEffect 020C584C points it at daBomb_c::mFusePos2 (last frame's fuse point); executeAfter 020C6860 fills the smoke trail from mpPos back to it (distance, spline end) (tww set() the same) */
  gptr<cXyz> mpOldOldPos;  /* was field_0x10: setFuseEffect 020C584C points it at daBomb_c::mFusePos3 (the fuse point two frames back); executeAfter 020C6860 takes the spline tangent at mpOldPos from it (tww set() the same) */
  gptr<void> mpEmitter;
  void executeAfter(void *);
  void execute(void *);
  void draw(void *);
  void drawAfter(void *);
  void cleanup(void *);
  void setup(void *, cXyz *);
};
struct daBomb_fuseSparksEcallBack {
  be<u32> __vtbl;
  gptr<cXyz> mpPos;
  gptr<void> mpEmitter;
  void execute(void *);
  void draw(void *);
  void setup(void *, cXyz *);
};
struct BombWindInfluence {
  cXyz mPos, mDir;
  be<f32> mRadius;
  be<f32> mStrength;
  be<f32> field_0x20;
  be<s32> mRegistIdx;
  be<u8> mbConstant;
  u8 padding[3];
};
WWHD_SIZE(BombWindInfluence, 0x2C);
struct daBomb_c : fopAc_ac_c {
  enum State_e { STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6, STATE_7, STATE_8 };
  enum AttrSt_e { ATTR_STATE_1=1, ATTR_STATE_2=2, ATTR_STATE_4=4, ATTR_STATE_8=8,
    ATTR_STATE_10=0x10, ATTR_STATE_20=0x20, ATTR_STATE_80=0x80,
    ATTR_STATE_100=0x100, ATTR_STATE_200=0x200 };
  enum Prm_e { PRM_STATE_W=8, PRM_STATE_S=0, PRM_CHEAP_EFF_W=1, PRM_CHEAP_EFF_S=0x10,
    PRM_ANGXZERO_W=1, PRM_ANGXZERO_S=0x11, PRM_VERSION_W=1, PRM_VERSION_S=0x1F };
  gabi::ptmf<u32 (daBomb_c::*)()> mFunc; // HD: the active procs return the full r3 value.
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel;
  mDoExt_bckAnm mBck0, mBck1;
  mDoExt_brkAnm mBrk0, mBrk1;
  be<s32> mType;
  u8 hdExtra[0xB0];
  dBgS_Acch mAcch;
  dBgS_AcchCir mCir;
  dBgS_GndChk mGndChk; // HD: lava filter uses group 4.
  be<f32> mLavaY;  /* was field_0x554: bgCrrPos_lava 020C5980 stores the lava ground height (GroundCross with the lava filter); chk_lava_hit 020C7AF4 is true below it, chk_dead_zone 020C7EB0 tests it with mWaterY / ground; create_init 020C6408 sets -1e9 (tww the same) */
  be<f32> mWaterY;  /* was field_0x558: bgCrrPos_water 020C59EC stores the water surface height (the water BG or the sea wave, -1e9 without); chk_water_sink 020C7A98 (30 below it), water_tention 020C6E44 and eff_water_splash 020C7B24 use it; create_init 020C6408 sets -1e9 (tww the same) */
  be<f32> mWaterYOld;  /* was field_0x55C: bgCrrPos_water 020C59EC copies mWaterY into it before the update; water_tention 020C6E44 reads it as the old surface height; create_init 020C6408 sets -1e9 (tww the same) */
  be<u8> mWaterLand;  /* was field_0x560: bgCrrPos_water 020C59EC sets it on the frame the bomb enters water (in water and mbWaterIn still 0); chk_water_land 020C7AEC returns it; create_init 020C6408 clears it (tww the same) */
  be<u8> mbWaterIn;
  be<u8> mWaterIsSea;  /* was field_0x562: bgCrrPos_water 020C59EC sets 1 when mWaterY is the sea wave (daSea_calcWave), 0 for the water BG; eff_water_splash 020C7B24 then passes no water polygon to the splash; create_init 020C6408 clears it (tww the same) */
  u8 _8e3;
  dCcD_Stts mStts;
  dCcD_Sph mSph;
  daBomb_fuseSmokeEcallBack mSmoke;
  daBomb_fuseSparksEcallBack mSparks;
  be<u8> mInPlayerBombCnt;  /* was field_0x6F0: create_init 020C6408 sets 1 for a bomb in STATE_3 (Link's); procExplode_init 020C5EF8 and bombDelete 020C50BC take 1 off Link's mBombCnt while it is set (explode then clears it) (tww the same) */
  be<u8> mBombCheckFlag;  /* was field_0x6F1: getBombCheck_Flag 020CB678 returns it, setBombCheck_Flag 020CB6A8 sets 1 (tww accessor names; d_a_am / d_a_nz skip a bomb whose flag is set) */
  be<u8> mBombFire;
  be<u8> mNoHit;  /* was field_0x6F3: setBombNoHit 020CB710 sets 1 (tww accessor name); posMoveF 020C6EB4 skips water_tention and the ground-slope handling while it is 1 */
  be<u8> mFuseLit;  /* was field_0x6F4: waitState_bomtyu 020C713C sets 1 when the bomb flower bomb is lit (mBombFire); execute 020C4D84 plays the fuse sound 0x6100 every frame while it is 1 (or ATTR_STATE_10); create_init 020C6408 clears it (tww the same) */
  be<u8> field_0x6F5;
  be<u8> field_0x6F6;
  be<u8> field_0x6F7;
  be<s32> mInitialState;
  be<s16> mRestTime;
  be<s16> mExplodeCcTimer;  /* was field_0x6FE: procExplode_init 020C5EF8 sets 2; procExplode 020C74A0 registers the explosion sphere (mSph at current.pos) while it counts down, then waits for mExplodeLightStep (tww the same) */
  be<s16> mNoGravityTime;
  be<s16> field_0x702;
  cXyz mFusePos, mFusePos2, mFusePos3;
  LIGHT_INFLUENCE mPntLight;
  BombWindInfluence mPntWind;
  be<u8> mExplodeLightStep;  /* was field_0x774: procExplode_init 020C5EF8 clears it; procExplode 020C74A0: 0 brightens mExplodeLight to 1, 1 fades it to 0, then the bomb deletes itself (mDeleteReq) once it is above 1 (tww the same) */
  u8 _af9[3];
  be<f32> mExplodeLight;  /* was field_0x778: procExplode 020C74A0 eases it per mExplodeLightStep and scales the explosion's actor / BG ambient and diffuse add colours (dKy_*addcol*_set) by it; procExplode_init 020C5EF8 clears it (tww the same) */
  be<u8> mFireEffectSet;  /* was field_0x77C: execute 020C4D84 makes the fire effect (makeFireEffect) once in STATE_4 and sets it; create_init 020C6408 clears it (tww the same) */
  be<u8> mWaterExplodeEffDone;  /* was field_0x77D: makeWaterEffect 020C7CC0 runs once (water pillar, sound, the water explosion sphere) and sets it; draw 020C4660 hides the bomb in STATE_4 once it is set; create_init 020C6408 clears it (tww the same) */
  be<u8> mFuseEffectSet;  /* was field_0x77E: setFuseEffect 020C584C starts the fuse sparks / smoke once and sets it; create_init 020C6408 clears it (tww the same) */
  be<u8> mCannonFlySeOn;  /* was field_0x77F: se_cannon_fly_set 020C70DC sets it; se_cannon_fly_stop 020C5078 stops the flying sound only while it is set and clears it; create_init 020C6408 clears it (tww the same) */
  be<u8> mThrown;  /* was field_0x780: procCarry 020C76E0 sets it when the bomb leaves the hand with speedF > 0; bound 020C71FC clears it on a ground bounce slower than 19.5; while it is set a bomb nut explodes on a CO hit (checkExplodeCc_nut 020C789C) or a wall / ground hit (checkExplodeBg_nut 020C7DC4) (tww the same) */
  be<u8> mDeleteReq;  /* was field_0x781: checkExplodeBg_norm 020C7C34 / _nut 020C7DC4 / _cannon 020C7EEC (sinking, the cannon bomb also in the dead zone) and procExplode 020C74A0 (end) set it; execute 020C4D84 deletes the actor while it is set; procWait 020C7358 returns before bound then (tww the same) */
  be<u8> mExplodeCcSet;  /* was field_0x782: set when the explosion sphere is first registered (procExplode_init 020C5EF8, procExplode 020C74A0, makeWaterEffect 020C7CC0); execute 020C4D84 counts mExplodeFrame up while it is set (tww the same) */
  u8 _b07;
  be<s32> mExplodeFrame;  /* was field_0x784: execute 020C4D84 adds 1 per frame while mExplodeCcSet; get_explode_instant 020CB98C is true at 1; create_init 020C6408 clears it (tww the same) */
  cXyz mWindVec;
  u8 field_0x794[0x30];
  be<s32> mMassCounter;
  be<s32> mDrawDelay;  /* was field_0x7C8: create_init 020C6408 sets 2 for a bomb in STATE_3, else 0; draw 020C4660 draws nothing while it is above 0; set_real_shadow_flag 020C506C counts it down and keeps the real shadow at 1 (tww the same) */
  s32 prm_get_state() const { return gabi::call<s32>(0x020CB8F0, this); }
  bool chk_state(s32 state) const {
    return gabi::call<bool>(0x020CB92C, this, state);
  }
  BOOL procExplode_init();
  u32 checkExplodeCc();
  u32 checkExplodeBg();
  void init_mtx();
  void se_cannon_fly_set();
  void draw_norm();
  BOOL draw();
  void draw_nut();
  void anm_play_nut();
  void water_tention();
  void waitState_bomtyu();
  bool procWait_init();
  void change_state(s32 state) { gabi::call(0x020CB978, this, state); }
  void setFuseEffect();
  bool checkExplodeCc_norm();
  bool checkExplodeCc_nut();
  bool checkExplodeCc_cannon();
  bool procCarry_init();
  void setRoomInfo();
  void bgCrrPos();
  void bgCrrPos_lava();
  void bgCrrPos_water();
  void bound(f32);
  bool checkExplodeBg_norm();
  bool checkExplodeBg_nut();
  bool checkExplodeBg_cannon();
  void eff_water_splash();
  void makeWaterEffect();
  cPhs_State create();
  void create_init();
  BOOL createHeap();
  void eff_explode_cheap(const csXyz *);
  void eff_explode_normal(const csXyz *);
  void eff_explode();
  BOOL execute();
  bool bombDelete();
  void set_wind_vec();
  void makeFireEffect(cXyz *pos, csXyz *angle);
  void set_mtx();
  u32 procWait();
  u32 procCarry();
  bool procExplode();
  void posMoveF();
  void set_real_shadow_flag();
  bool checkExplodeTimer();
  bool checkExplode();
  u8 chk_water_in();
  bool chk_water_sink();
  u8 chk_water_land();
  bool chk_lava_hit();
  bool chk_dead_zone();
  bool waitState_cannon();
  void se_cannon_fly_stop();
};
WWHD_OFFSET(daBomb_c, mpModel, 0x3BC);
WWHD_OFFSET(daBomb_c, mType, 0x5C8);
WWHD_OFFSET(daBomb_c, mAcch, 0x67C);
WWHD_OFFSET(daBomb_c, mStts, 0x8E4);
WWHD_OFFSET(daBomb_c, mRestTime, 0xA7C);
WWHD_OFFSET(daBomb_c, mPntLight, 0xAA8);
WWHD_SIZE(daBomb_c, 0xB50);

struct BombSafeString { be<u32> text, vt; };
WWHD_SIZE(BombSafeString, 8);

// HD: private exact-ABI bridges preserve the verified argument and result widths.
void bomb_mDoAud_seStopActor(cXyz *);
void bomb_J3DAnmCurve_update(u32);
void bomb_Link_drawBombExtra(u32, u32, J3DModel *);
s32 bomb_J3DUClipper_clip(s32, s32, cXyz *, f32);
u32 bomb_dKy_getEnvlight();
void bomb_dScnKy_env_light_c_settingTevStruct(u32, s32, cXyz *, dKy_tevstr_c *);
void bomb_dScnKy_env_light_c_setLightTevColorType(u32, J3DModel *, dKy_tevstr_c *);
void bomb_dCcD_GObjInf_ClrCoHit(dCcD_Sph *);
void bomb_mDoAud_seStart(s32, cXyz *);
s32 bomb_dBgS_GetRoomId(u32, u32);
u32 bomb_dBgS_GetPolyColor(u32, u32);
f32 bomb_cBgS_GroundCross(u32, u32);
void bomb_dBgS_Acch_CrrPos(dBgS_Acch *, u32);
s32 bomb_daSea_ChkArea(f32, f32);
f32 bomb_daSea_calcWave(f32, f32);
void bomb_daObj_make_land_effect(daBomb_c *, u32, f32);
f32 bomb_cLib_addCalc(be<f32> *, f32, f32, f32, f32);
bool bomb_daBomb_c_prm_get_angXZero(daBomb_c *);
void bomb_mDoMtx_ZXYrotM(s32, s16, s16, s16);
u32 bomb_daBomb_c_procCarry_init(daBomb_c *);
u32 bomb_cBgS_GetTriPla(u32, u16, u16);
f32 bomb_PSVECSquareMag(cXyz *);
void bomb_daObj_posMoveF_grade(daBomb_c *, dCcD_Stts *, cXyz *, u32, s32, f32, f32, f32, f32);
void bomb_fopAcM_posMoveF(daBomb_c *, dCcD_Stts *);
void bomb_fopAcM_delete(daBomb_c *);
void bomb_dCcD_GStts_Move(u32);
void bomb_cCcS_Set(u32, dCcD_Sph *);
void bomb_dCcMassS_Mng_Set(u32, dCcD_Sph *, s32);
s32 bomb_dComIfGp_getReverb(s8);
void bomb_dKy_actor_addcol_set(s32, s32, s32, f32);
void bomb_dKy_efplight_cut(LIGHT_INFLUENCE *);
void bomb_dKyw_pntwind_cut(BombWindInfluence *);
void bomb_dPa_control_c_setBombSmoke(u32, s32, cXyz *, csXyz *, cXyz *, s32);
void bomb_dVibration_c_StartShock(u32, s32, s32, cXyz *);
void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, daBomb_fuseSparksEcallBack *, s32, s32, s32, s32);
void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, daBomb_fuseSmokeEcallBack *, s8, u32, u32, s32);
void bomb_fopKyM_createWpillar(cXyz *, s32, f32, f32);
s32 bomb_dBgS_GetMtrlSndId(u32, u32);
void bomb_PSVECScale(cXyz *, cXyz *, f32);
f32 bomb_sqrtf(f32);
u32 bomb_dCcD_GAtTgCoCommonBase_GetAc(u32);
s32 bomb_cM_atan2s(f32, f32);
void bomb_cXyz___ml(cXyz *, cXyz *, f32);
void bomb_cXyz___pl(cXyz *, cXyz *, cXyz *);
void bomb_randomSecondObject_ct(s32);
void bomb_randomFirstObject_ct(s32);
void bomb_dl(void *);
void bomb_dCcD_Sph_dtor(dCcD_Sph *, s32);
void bomb_dCcD_Stts_dtor(dCcD_Stts *, s32);
void bomb_cBgS_Chk_cBgS_Chk(u32, s32);
void bomb_cM3dGCir_dtor(u32, s32);
void bomb_dBgS_Acch_dtor(dBgS_Acch *, s32);
void bomb_HDExtraModel_dtor(u32, s32);
void bomb_J3DFrameCtrl_dtor(u32, s32);
void bomb_fopAc_ac_c_fopAc_ac_c(daBomb_c *, s32);
void bomb_dl(daBomb_c *);
void bomb_J3DFrameCtrl_init(u32, s32);
void bomb_J3DAnmCurve_ct(u32);
daBomb_c * bomb_operator_new(s32);
void bomb_fopAc_ac_c_fopAc_ac_c(daBomb_c *);
void bomb_mDoExt_brkAnm_init(mDoExt_brkAnm *);
void bomb_HDExtraModel_ct(u32);
void bomb_dBgS_Acch_dBgS_Acch(dBgS_Acch *);
void bomb_dBgS_AcchCir_dBgS_AcchCir(dBgS_AcchCir *);
void bomb_cBgS_GndChk_cBgS_GndChk(u32);
void bomb_cCcD_Stts_ct(dCcD_Stts *);
void bomb_dCcD_GStts_dCcD_GStts(u32);
void bomb_dCcD_Sph_ct(dCcD_Sph *);
void bomb_dCcD_Stts_Init(dCcD_Stts *, s32, s32, daBomb_c *);
void bomb_dCcD_Sph_Set(dCcD_Sph *, s32);
void bomb_fopAcM_setCullSizeBoxMin(daBomb_c *, f32, f32, f32);
void bomb_fopAcM_setCullSizeBoxMax(daBomb_c *, f32, f32, f32);
s32 bomb_fopAcM_entrySolidHeap(daBomb_c *, s32, u32);
void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, const csXyz *, cXyz *, s32, s32, s32, s32, s32, s32);
void bomb_dPa_control_c_setBombSmoke(u32, s32, cXyz *, s32, cXyz *, s32);
void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, s32, s32, s32, s32, s32);
u32 bomb_dPa_control_c_set_result(u32, s32, s32, cXyz *, const csXyz *, cXyz *, s32, s32, s32, s32, s32, s32);
u32 bomb_dPa_control_c_setBombSmoke_result(u32, s32, cXyz *, s32, cXyz *, s32);
u32 bomb_dPa_control_c_set_result(u32, s32, s32, cXyz *, s32, cXyz *, s32, s32, s32, s32, s32, s32);
bool bomb_daBomb_c_prm_get_cheapEff(daBomb_c *);
f32 bomb_PSVECSquareDistance(cXyz *, u32);
void bomb_dKy_actor_addcol_amb_set(s32, s32, s32, f32);
void bomb_dKy_bg_addcol_amb_set(s32, s32, s32, f32);
void bomb_dKy_bg_addcol_dif_set(s32, s32, s32, f32);
void bomb_dCamera_c_ForceLockOff(u32, u32);
u32 bomb_fopAcM_getWaterY(cXyz *, gabi::be<float> *);
void bomb_dKy_efplight_set(LIGHT_INFLUENCE *);
void bomb_dKyw_pntwind_set(BombWindInfluence *);
void bomb_mDoAud_seStartLevel(s32, u32, s32, s32);
void bomb_fopAcM_cancelCarryNow(daBomb_c *);
void bomb_dKy_Sound_set(cXyz *, s32, u32, s32);
f32 bomb_PSVECSquareDistance(cXyz *, cXyz *);
u32 bomb_JPABaseEmitter_createParticle(u32);
u32 bomb_dRes_control_c_getRes(u32, BombSafeString *, u32);
void bomb_JUTAssertion_showAssert(s32, u32, u32);
u32 bomb_mDoExt_J3DModel__create(u32, s32, s32);
u32 bomb_mDoExt_bckAnm_init(mDoExt_bckAnm *, u32, u32, s32, s32, s32, s32, s32, f32);
u32 bomb_mDoExt_brkAnm_init(mDoExt_brkAnm *, u32, u32, s32, s32, s32, s32, s32, s32, f32);
void bomb_HDExtraModel_reset(u32, s32);

struct BombVectorWords { be<u32> words[3]; };
WWHD_SIZE(BombVectorWords, 0xC);
struct BombSafeStringVtable { u8 prefix[0x14]; gabi::gfn assureTermination; };
struct BombStaticPointer { be<u32> mValue; };
WWHD_SIZE(BombStaticPointer, 4);

struct BombAttrType { gptr<const char> resName; be<u32> heapSize; };
struct BombAttrTable { BombAttrType mAttr[3]; };
struct BombCounter { be<u32> mValue; };
struct BombStayRoom { be<u8> mValue; };
struct BombNoGradeCos { be<f32> mValue; };
struct BombFuseOffset { cXyz mOffset; be<u32> mGuard; };
struct BombCullSize { be<f32> mRadius, mHeight; };
struct BombCheckTable { gabi::ptmf<u32 (daBomb_c::*)()> mProc[3]; };

namespace BombResource {
enum {
  dRes_INDEX_VBAKH_BDL_VBAKM_e=12, dRes_INDEX_VBAKH_BCK_VBAMX_e=8,
  dRes_INDEX_VBAKH_BCK_VBAKM_e=7, dRes_INDEX_VBAKH_BRK_VBAMX_e=17,
  dRes_INDEX_VBAKH_BRK_VBAKM_e=16, dRes_INDEX_LINK_BDL_BOMB_e=60,
  dRes_INDEX_LINK_BCK_BOMB_e=11
};
}
namespace BombParticleId {
enum {
  ID_IT_JT_BMEX_HAHEN=0x2008, ID_IT_JT_BMEX_SMOKE01=0x2009,
  ID_IT_JT_BMEX_SMOKE02=0x200A, ID_IT_JN_BMEX_SENKO=0xB,
  ID_IT_JN_BM_HIBANA=0x11, ID_IT_JT_BM_SMOKE=0x2012,
  ID_IT_JT_BMEX_SMOKELIGHT=0x232A
};
}

struct BombHeaderAngles { be<f32> mNegativePi, mPositivePi; };
struct BombRandomState { be<u32> mWords[4]; };
struct BombSphereSource { u8 prefix[0x3C]; be<f32> mRadius; };
struct BombSinCosTable { be<f32> mSinCos[8192][2]; };
WWHD_SIZE(BombHeaderAngles, 8);
WWHD_SIZE(BombRandomState, 0x10);
WWHD_SIZE(BombSphereSource, 0x40);
WWHD_SIZE(BombSinCosTable, 0x10000);
WWHD_OFFSET(daBomb_c, mFunc, 0x3AC);
WWHD_OFFSET(daBomb_c, mPhase, 0x3B4);
WWHD_OFFSET(daBomb_c, mBck0, 0x3C0);
WWHD_OFFSET(daBomb_c, mBck1, 0x44C);
WWHD_OFFSET(daBomb_c, mBrk0, 0x4D8);
WWHD_OFFSET(daBomb_c, mBrk1, 0x550);
WWHD_OFFSET(daBomb_c, hdExtra, 0x5CC);
WWHD_OFFSET(daBomb_c, mCir, 0x840);
WWHD_OFFSET(daBomb_c, mGndChk, 0x880);
WWHD_OFFSET(daBomb_c, mLavaY, 0x8D4);
WWHD_OFFSET(daBomb_c, mWaterY, 0x8D8);
WWHD_OFFSET(daBomb_c, mWaterYOld, 0x8DC);
WWHD_OFFSET(daBomb_c, mWaterLand, 0x8E0);
WWHD_OFFSET(daBomb_c, mbWaterIn, 0x8E1);
WWHD_OFFSET(daBomb_c, mWaterIsSea, 0x8E2);
WWHD_OFFSET(daBomb_c, mSph, 0x920);
WWHD_OFFSET(daBomb_c, mSmoke, 0xA4C);
WWHD_OFFSET(daBomb_c, mSparks, 0xA64);
WWHD_OFFSET(daBomb_c, mInPlayerBombCnt, 0xA70);
WWHD_OFFSET(daBomb_c, mBombFire, 0xA72);
WWHD_OFFSET(daBomb_c, mNoHit, 0xA73);
WWHD_OFFSET(daBomb_c, mFuseLit, 0xA74);
WWHD_OFFSET(daBomb_c, mInitialState, 0xA78);
WWHD_OFFSET(daBomb_c, mExplodeCcTimer, 0xA7E);
WWHD_OFFSET(daBomb_c, mNoGravityTime, 0xA80);
WWHD_OFFSET(daBomb_c, mFusePos, 0xA84);
WWHD_OFFSET(daBomb_c, mFusePos2, 0xA90);
WWHD_OFFSET(daBomb_c, mFusePos3, 0xA9C);
WWHD_OFFSET(daBomb_c, mPntWind, 0xACC);
WWHD_OFFSET(daBomb_c, mExplodeLightStep, 0xAF8);
WWHD_OFFSET(daBomb_c, mExplodeLight, 0xAFC);
WWHD_OFFSET(daBomb_c, mFireEffectSet, 0xB00);
WWHD_OFFSET(daBomb_c, mWaterExplodeEffDone, 0xB01);
WWHD_OFFSET(daBomb_c, mFuseEffectSet, 0xB02);
WWHD_OFFSET(daBomb_c, mCannonFlySeOn, 0xB03);
WWHD_OFFSET(daBomb_c, mThrown, 0xB04);
WWHD_OFFSET(daBomb_c, mDeleteReq, 0xB05);
WWHD_OFFSET(daBomb_c, mExplodeCcSet, 0xB06);
WWHD_OFFSET(daBomb_c, mExplodeFrame, 0xB08);
WWHD_OFFSET(daBomb_c, mWindVec, 0xB0C);
WWHD_OFFSET(daBomb_c, field_0x794, 0xB18);
WWHD_OFFSET(daBomb_c, mMassCounter, 0xB48);
WWHD_OFFSET(daBomb_c, mDrawDelay, 0xB4C);
WWHD_SIZE(daBomb_fuseSmokeEcallBack, 0x18);
WWHD_SIZE(daBomb_fuseSparksEcallBack, 0xC);
