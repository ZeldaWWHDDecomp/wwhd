/* Tapestry actor and cloth packet, WWHD layouts.
 * HD: actor base is 0x3AC; the 0x2520 packet includes GX2 resources, two cloth buffers,
 * 48 velocities and sixteen particle callbacks. tww supplies names but no matching layouts. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_cc_d.h"
// HD: the verified HIO retains the scalar tuning layout and a trailing vtable.
struct daObjTapestry_HIO_c {
  /* 0x00 */ be<s8> mChild;
  /* 0x01 */ be<u8> mDebugEnabled, mDebugSelected;
  /* 0x03 */ u8 mPad03;
  /* 0x04 */ be<u32> mDisplayFlags;
  /* 0x08 */ be<f32> mGravity;
  /* 0x0C */ be<f32> mStiffness;
  /* 0x10 */ be<f32> mFriction;
  /* 0x14 */ be<f32> mGroundFriction;
  /* 0x18 */ be<f32> mWindSpeed;
  /* 0x1C */ be<f32> mFlutterSpeed;
  /* 0x20 */ be<f32> mRandomWind;
  /* 0x24 */ be<f32> mTopWindScale;
  /* 0x28 */ be<f32> mMiddleWindScale;
  /* 0x2C */ be<f32> mBottomWindScale;
  /* 0x30 */ be<f32> mGravityScale;
  /* 0x34 */ be<f32> mAxialSpringScale;
  /* 0x38 */ be<f32> mBendingSpringScale;
  /* 0x3C */ be<f32> mDiagonalSpringScale;
  /* 0x40 */ be<f32> mDepthResponse;
  /* 0x44 */ be<f32> mHitResponse;
  /* 0x48 */ be<u8> mBurnStart;
  /* 0x49 */ be<u8> mBurnEnd;
  /* 0x4A */ u8 mPad4A[2];
  /* 0x4C */ be<f32> mFadeSpeed;
  /* 0x50 */ be<u8> mBurnDelay;
  /* 0x51 */ u8 mPad51[3];
  /* 0x54 */ be<f32> mFireVelocityScale;
  /* 0x58 */ be<f32> mFireVelocityLimit;
  /* 0x5C */ be<f32> mFireScaleMin;
  /* 0x60 */ be<f32> mFireScaleMax;
  /* 0x64 */ be<u8> mUseDiffuse;
  /* 0x65 */ u8 mPad65[3];
  /* 0x68 */ be<u32> mVtable;
};
WWHD_SIZE(daObjTapestry_HIO_c, 0x6C);
WWHD_OFFSET(daObjTapestry_HIO_c, mGravity, 0x8);
WWHD_OFFSET(daObjTapestry_HIO_c, mStiffness, 0xC);
WWHD_OFFSET(daObjTapestry_HIO_c, mFriction, 0x10);
WWHD_OFFSET(daObjTapestry_HIO_c, mGroundFriction, 0x14);
WWHD_OFFSET(daObjTapestry_HIO_c, mWindSpeed, 0x18);
WWHD_OFFSET(daObjTapestry_HIO_c, mFlutterSpeed, 0x1C);
WWHD_OFFSET(daObjTapestry_HIO_c, mRandomWind, 0x20);
WWHD_OFFSET(daObjTapestry_HIO_c, mTopWindScale, 0x24);
WWHD_OFFSET(daObjTapestry_HIO_c, mMiddleWindScale, 0x28);
WWHD_OFFSET(daObjTapestry_HIO_c, mBottomWindScale, 0x2C);
WWHD_OFFSET(daObjTapestry_HIO_c, mGravityScale, 0x30);
WWHD_OFFSET(daObjTapestry_HIO_c, mAxialSpringScale, 0x34);
WWHD_OFFSET(daObjTapestry_HIO_c, mBendingSpringScale, 0x38);
WWHD_OFFSET(daObjTapestry_HIO_c, mDiagonalSpringScale, 0x3C);
WWHD_OFFSET(daObjTapestry_HIO_c, mDepthResponse, 0x40);
WWHD_OFFSET(daObjTapestry_HIO_c, mHitResponse, 0x44);
WWHD_OFFSET(daObjTapestry_HIO_c, mBurnStart, 0x48);
WWHD_OFFSET(daObjTapestry_HIO_c, mBurnEnd, 0x49);
WWHD_OFFSET(daObjTapestry_HIO_c, mFadeSpeed, 0x4C);
WWHD_OFFSET(daObjTapestry_HIO_c, mBurnDelay, 0x50);
WWHD_OFFSET(daObjTapestry_HIO_c, mFireVelocityScale, 0x54);
WWHD_OFFSET(daObjTapestry_HIO_c, mFireVelocityLimit, 0x58);
WWHD_OFFSET(daObjTapestry_HIO_c, mFireScaleMin, 0x5C);
WWHD_OFFSET(daObjTapestry_HIO_c, mFireScaleMax, 0x60);
WWHD_OFFSET(daObjTapestry_HIO_c, mUseDiffuse, 0x64);

// HD: each particle callback embeds its followed position and velocity.
struct daObjTapestryFireEff_c : dPa_followEcallBack {
  /* 0x14 */ cXyz mPosition;
  /* 0x20 */ cXyz mVelocity;
  void execute(u8* emitter);
};
WWHD_OFFSET(daObjTapestryFireEff_c, mPosition, 0x14);
WWHD_OFFSET(daObjTapestryFireEff_c, mVelocity, 0x20);
WWHD_SIZE(daObjTapestryFireEff_c, 0x2C);
struct TapestrySmokeCallback : dPa_followEcallBack {
  /* 0x14 */ cXyz mPosition;
  /* 0x20 */ csXyz mAngle;
  /* 0x26 */ u8 mPad26[2];
  /* 0x28 */ be<s32> mTimer;
};
WWHD_OFFSET(TapestrySmokeCallback, mTimer, 0x28);
WWHD_SIZE(TapestrySmokeCallback, 0x2C);
struct TapestryClothVertices {
  /* 0x000 */ cXyz mPositions[48];
  /* 0x240 */ cXyz mNormals[48];
  /* 0x480 */ cXyz mBackNormals[48];
};
WWHD_SIZE(TapestryClothVertices, 0x6C0);

// HD: the point-light core is 0x20 bytes; the packet carries its extra power separately.
struct TapestryLightInfluence {
  /* 0x00 */ cXyz mPosition;
  /* 0x0C */ be<u16> mColor[4];
  /* 0x14 */ be<f32> mPower;
  /* 0x18 */ be<f32> mFluctuation;
  /* 0x1C */ be<s32> mIndex;
};
WWHD_SIZE(TapestryLightInfluence, 0x20);

struct daObjTapestryPLight_c {
  /* 0x000 */ be<u8> mActive;
  /* 0x001 */ u8 _1[3];
  /* 0x004 */ TapestryLightInfluence mInfluence;
  /* 0x024 */ be<f32> mPower;
  /* 0x028 */ gptr<u8> mpEmitter;
  /* 0x02C */ be<f32> mIntensity;
  void plight_move(cXyz* position, u8* rotation);
  void setPointLight(cXyz* position, u8* rotation);
  void plight_make();
  void plight_delete();
};
WWHD_SIZE(daObjTapestryPLight_c, 0x30);

struct daObjTapestryDrawData_c {
  /* 0x000 */ be<u16> mIndices[7][6][2];
  /* 0x0A8 */ be<f32> mTexCoords[8][6][2];
  void ct_tex();
  void ct_dl();
};
WWHD_OFFSET(daObjTapestryDrawData_c, mTexCoords, 0xA8);
WWHD_SIZE(daObjTapestryDrawData_c, 0x228);

struct TapestryHioDefaults { be<u32> mWords[23]; };
WWHD_SIZE(TapestryHioDefaults, 0x5C);

struct TapestryWindDefaults { cXyz mDirections[3]; };
WWHD_SIZE(TapestryWindDefaults, 0x24);
struct TapestryWindGuards { be<u32> mFlags[3]; };
WWHD_SIZE(TapestryWindGuards, 0x0C);

struct daObjTapestryPacket_c;
struct daObjTapestry_c : fopAc_ac_c {
  /* 0x3AC */ request_of_phase_process_class mPhase;
  /* 0x3B4 */ gptr<daObjTapestryPacket_c> mpPacket;
  /* 0x3B8 */ gptr<J3DModel> mpModel;
  /* 0x3BC */ gptr<dBgW> mpBg;
  /* 0x3C0 */ be<f32> mBgMatrix[12];
  /* 0x3F0 */ dCcD_Tri mTriangles[2];
  /* 0x690 */ dCcD_Stts mCollisionStatus[2];
  /* 0x708 */ be<s16> mBurnTimer;
  /* 0x70A */ be<u8> mBurned;
  /* 0x70B */ u8 _70B;
  /* 0x70C */ be<s16> mEvent;
  /* 0x70E */ u8 _70E[2];
  /* 0x710 */ be<f32> mOpacity;
  using Action = gabi::ptmf<void (daObjTapestry_c::*)()>;
  /* 0x714 */ Action mActionDescriptor;
  /* 0x71C */ be<s32> mAction;
  enum ActionState : s32 { WAIT, DEMO_REQUEST, BURN, FINE };
  void burn_act_init_proc();
  void fine_act_init_proc();
  BOOL checkCollision();
  s32 create_res_load();
  static BOOL solidHeapCB(daObjTapestry_c*);
  BOOL create_heap();
  void set_mtx();
  void init_mtx();
  BOOL chk_appear();
  void setup_action(s32);
  BOOL _delete();
  BOOL _execute();
  void set_cc_pos();
  void init_cc();
  void set_eye_pos();
  s32 _create();
  BOOL _draw();
  void wait_act_proc();
  void demo_request_act_proc();
  void burn_act_proc();
};
WWHD_OFFSET(daObjTapestry_c, mActionDescriptor, 0x714);
WWHD_SIZE(daObjTapestry_c, 0x720);
struct TapestryActionTable { daObjTapestry_c::Action mEntries[4]; };
WWHD_SIZE(TapestryActionTable, 0x20);

struct TapestryMaterialGroup {
  /* 0x000 */ gptr<u8> mpDefinition;
  /* 0x004 */ be<u32> mFrontCount;
  /* 0x008 */ gptr<u8> mpFrontVectors;
  /* 0x00C */ be<u32> mBackCount;
  /* 0x010 */ gptr<u8> mpBackVectors;
};
WWHD_SIZE(TapestryMaterialGroup, 0x14);
// HD: the GX2 vertex stream stores position, normal, color and texture coordinates.
struct TapestryRenderVertex {
  /* 0x00 */ cXyz mPosition;
  /* 0x0C */ cXyz mNormal;
  /* 0x18 */ be<f32> mColor[4];
  /* 0x28 */ be<f32> mTexCoords[2];
};
WWHD_SIZE(TapestryRenderVertex, 0x30);

struct TapestryVertexBuffer {
  /* 0x000 */ gptr<u8> mpData;
  /* 0x004 */ u8 mGX2Buffer[0x154];
  /* 0x158 */ u8 mAttribute[0xF4];
  /* 0x24C */ be<u32> mVertexCount;
  /* 0x250 */ gptr<u8> mpOwnedData;
};
WWHD_SIZE(TapestryVertexBuffer, 0x254);
// HD: packet and shader blocks have their own GX2 resource layout.
struct TapestryUniformHeader {
  /* 0x00 */ u8 mPacket[0x0C];
  /* 0x0C */ be<u32> mVtable;
  /* 0x10 */ u8 mBuffers[2][0x1C];
  /* 0x48 */ u8 mState[0x2C];
};
WWHD_SIZE(TapestryUniformHeader, 0x74);
struct TapestryVertexUniform {
  /* 0x00 */ TapestryUniformHeader mHeader;
  /* 0x74 */ be<f32> mMatrix[12];
  /* 0xA4 */ u8 mPadA4[4];
};
WWHD_SIZE(TapestryVertexUniform, 0xA8);
struct TapestryLightingUniforms {
  /* 0x000 */ be<f32> mColors[11][4];
  /* 0x0B0 */ be<f32> mVectorPairs[3][2][4];
  /* 0x110 */ be<f32> mMatrices[8][12];
  /* 0x290 */ be<f32> mVectors[6][4];
};
WWHD_SIZE(TapestryLightingUniforms, 0x2F0);

struct daObjTapestryPacket_c {
  enum VertexFlag : u8 { DETACHED = 1, GROUND_CONTACT = 2 };
  enum : u8 { NO_FIRE = 255 };
  /* 0x000 */ u8 mPacketBase[0x0C];
  /* 0x00C */ be<u32> mVtable;
  /* 0x010 */ u8 mPacketState[8];
  /* 0x018 */ gptr<daObjTapestry_c> mpOwner;
  /* 0x01C */ u8 mPacketTail[0x7C];
  /* 0x098 */ be<u32> mMaterialCount, mMaterialCapacity;
  /* 0x0A0 */ gptr<TapestryMaterialGroup> mpMaterialGroups;
  /* 0x0A4 */ TapestryVertexBuffer mVertexBuffers[2][2];
  /* 0x9F4 */ be<u32> mVertexBuffer, mVertexFormat;
  /* 0x9FC */ be<u32> mRenderVertexCount;
  /* 0xA00 */ gptr<u8> mpVertexLayout;
  /* 0xA04 */ be<u32> mRenderMode;
  /* 0xA08 */ be<u8> mRenderEnabled;
  /* 0xA09 */ u8 _A09[3];
  /* 0xA0C */ u8 mIndexBuffer[0x18];
  /* 0xA24 */ u8 mLightingMaterial[0x0C];
  /* 0xA30 */ TapestryVertexUniform mVertexUniform;
  /* 0xAD8 */ TapestryUniformHeader mPixelUniform;
  /* 0xB4C */ TapestryLightingUniforms mLightingUniforms;
  /* 0xE3C */ be<u8> mShaderFlags;
  /* 0xE3D */ u8 mShaderPadding[3];
  /* 0xE40 */ u8 mTextureResource[0x90];
  /* 0xED0 */ u8 mTextureSampler[0x190];
  /* 0x1060 */ be<u8> mTextureFlags;
  /* 0x1061 */ u8 mTexturePadding[7];
  /* 0x1068 */ gptr<dKy_tevstr_c> mpTevStr;
  /* 0x106C */ TapestryClothVertices mCloth[2];
  /* 0x1DEC */ cXyz mVelocity[48];
  /* 0x202C */ be<u8> mVertexFlags[48], mFireTimers[48], mFireIndices[48];
  /* 0x20BC */ be<u32> mPositionBuffer;
  /* 0x20C0 */ daObjTapestryFireEff_c mFireCallbacks[16];
  /* 0x2380 */ be<s32> mFireCount;
  /* 0x2384 */ cXyz mAcceleration;
  /* 0x2390 */ be<f32> mWorldMatrix[12], mViewMatrix[12], mInverseMatrix[12];
  /* 0x2420 */ be<u8> mInverseValid;
  /* 0x2421 */ u8 _2421[3];
  /* 0x2424 */ cXyz mWorldWind, mWind;
  /* 0x243C */ be<s16> mWindPhase;
  /* 0x243E */ u8 _243E[2];
  /* 0x2440 */ cXyz mFlutter;
  /* 0x244C */ be<s16> mFlutterAngles[3];
  /* 0x2452 */ u8 _2452[2];
  /* 0x2454 */ cXyz mTopWind, mMiddleWind, mBottomWind;
  /* 0x2478 */ cXyz mHitPosition, mLocalHitPosition, mHitDirection, mLocalHitDirection;
  /* 0x24A8 */ be<f32> mHitForce, mHitRadius;
  /* 0x24B0 */ be<u8> mHitFire;
  /* 0x24B1 */ u8 _24B1[3];
  /* 0x24B4 */ be<f32> mHitAttenuation, mHitRow, mHitColumn;
  /* 0x24C0 */ be<u8> mDetached, mGroundCollision, mSmokeType;
  /* 0x24C3 */ u8 _24C3;
  /* 0x24C4 */ TapestrySmokeCallback mSmokeCallback;
  /* 0x24F0 */ daObjTapestryPLight_c mLight;
  void update();
  void calc_acc_spring_sub(cXyz *first, cXyz *second, f32 rest, f32 factor);
  void calc_acc_spring(s32 row, s32 column);
  void calc_acc_wave(s32 row, s32 column);
  void calc_acc_hit(s32 row, s32 column);
  void calc_spd(s32 row, s32 column);
  void calc_pos_crr(s32 row, s32 column);
  void calc_pos();
  void calc_nrm();
  void calc_wind();
  void calc_hit();
  void calc_fire_leap(s32 row, s32 column);
  void calc_fire();
  void calc(daObjTapestry_c *actor);
  void set_hit(cXyz *position, cXyz *direction, s32 smoke, f32 force, f32 radius);
  u8 eff_start(s32 row, s32 column);
  BOOL eff_start_chk(s32 row, s32 column);
  void eff_pos();
  void smoke_set();
  void smoke_move(daObjTapestry_c *actor);
  void init(u8 *owner);
  void setup_vtx();
  void initializeRenderResources();
  void drawSurface(u8 *surface);
  cXyz *get_now_pos(s32, s32);
  void calc_acc_gravity();
  void eff_end();
  void eff_delete();
  void smokeCallback_init();
};
WWHD_OFFSET(daObjTapestryPacket_c, mVertexBuffers, 0xA4);
WWHD_OFFSET(daObjTapestryPacket_c, mVertexBuffer, 0x9F4);
WWHD_OFFSET(daObjTapestryPacket_c, mCloth, 0x106C);
WWHD_OFFSET(daObjTapestryPacket_c, mWorldWind, 0x2424);
WWHD_OFFSET(daObjTapestryPacket_c, mPositionBuffer, 0x20BC);
WWHD_OFFSET(daObjTapestryPacket_c, mAcceleration, 0x2384);
WWHD_SIZE(daObjTapestryPacket_c, 0x2520);


// HD: private ABI adapters preserve the verified argument and result widths.
u8* tapestryApi_dRes_control_c_getRes(void*, void*, s32);
s32 tapestryApi_dComIfG_resLoad(void*, void*);
void tapestryApi_assertionShow(void*, s32, void*);
J3DModel* tapestryApi_mDoExt_J3DModel__create(void*, s32, s32);
dBgW* tapestryApi_dBgW_NewSet(void*, s32, void*);
void tapestryApi_PSMTXTrans(void*, f32, f32, f32);
void tapestryApi_mDoMtx_ZXYrotM(void*, s16, s16, s16);
void tapestryApi_mDoMtx_stack_c_scaleM(f32, f32, f32);
void tapestryApi_PSMTXCopy(void*, void*);
void tapestryApi_dKy_plight_set(void*);
void tapestryApi_dKy_plight_cut(void*);
void tapestryApi_dPa_followEcallBack_end(void*);
s32 tapestryApi_dComIfGp_getReverb(s8);
void tapestryApi_startSound(s32, void*, s32, s32);
s32 tapestryApi_dSv_info_c_isSwitch(void*, u32, s8);
void tapestryApi_mDoMtx_stack_c_transM(f32, f32, f32);
cXyz* tapestryApi_daObjTapestryPacket_c_get_now_posVariantA(void*, u8, u8);
void tapestryApi_PSMTXMultVec(void*, void*, void*);
void tapestryApi_setTriangleVertices(void*, void*, void*, void*);
void tapestryApi_dCcD_Stts_Init(void*, s32, s32, void*);
void tapestryApi_dCcD_Tri_Set(void*, void*);
cXyz* tapestryApi_daObjTapestryPacket_c_get_now_posVariantB(void*, s32, s32);
void tapestryApi_cXyz___pl(void*, void*, void*);
void tapestryApi_cXyz___ml(void*, void*, f32);
void tapestryApi_daObjTapestryPacket_c_eff_delete(void*);
void tapestryApi_dComIfG_resDelete(void*, void*);
u32 tapestryApi_getGameInfo();
void tapestryApi_cBgS_Release(void*, void*);
void tapestryApi_mDoHIO_subRoot_c_deleteChild(s8);
void tapestryApi_daObjTapestry_c_set_eye_pos(void*);
void tapestryApi_dBgW_Move(void*);
void tapestryApi_tapestryPacketCalc(void*, void*);
u32 tapestryApi_getEnvironment();
void tapestryApi_dScnKy_env_light_c_settingTevStruct(void*, s32, void*, void*);
void tapestryApi_setLightTevColorType(void*, void*, void*);
void tapestryApi_mDoExt_modelUpdateDL(void*, s32);
void tapestryApi_tapestryPacketUpdate(void*);
s32 tapestryApi_tapestryCheckCollision(void*);
u32 tapestryApi_dEvent_manager_c_getEventData(void*, s16);
void tapestryApi_cCcS_Set(void*, void*);
void tapestryApi_fopAcM_orderOtherEventId(void*, s16, u8, s32, s32, s32);
s32 tapestryApi_cLib_chaseF(void*, f32, f32);
void tapestryApi_dSv_info_c_onSwitch(void*, u32, s8);
void tapestryApi_daObjTapestryPacket_c_eff_end(void*);
void tapestryApi_fopAc_ac_c_fopAc_ac_cVariantA(void*);
void tapestryApi_construct_arrayVariantA(void*, s32, s32, void*);
s32 tapestryApi_fopAcM_entrySolidHeap(void*, void*, s32);
s32 tapestryApi_dBgS_Regist(void*, void*, void*);
void tapestryApi_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32);
daObjTapestryPacket_c* tapestryApi_tapestryPacketConstruct(void*);
void tapestryApi_tapestryPacketInit(void*, void*);
s16 tapestryApi_dEvent_manager_c_getEventIdx(void*, s32, u8);
s8 tapestryApi_mDoHIO_subRoot_c_createChild(void*, void*);
void tapestryApi_dCcD_GObjInf_dCcD_GObjInf(void*);
void tapestryApi_constructTrianglePlane(void*);
void tapestryApi_constructCollisionStatus(void*);
void tapestryApi_dCcD_GStts_dCcD_GStts(void*);
void tapestryApi_constructVertexBuffer(void*);
void tapestryApi_constructVertexAttribute(void*);
void tapestryApi_dPa_followEcallBack_dPa_followEcallBack(void*, s32, s32);
void tapestryApi_cBgS_Chk_cBgS_Chk(void*, s32);
void tapestryApi_destroyVertexAttribute(void*, s32);
void tapestryApi_destroyVertexBuffer(void*, s32);
void tapestryApi_destroy_arr(void*, s32, s32, s32, s32, s32);
void tapestryApi_fopAc_ac_c_fopAc_ac_cVariantB(void*, s32);
void tapestryApi_PSMTXConcat(void*, void*, void*);
void tapestryApi_J3DDrawBuffer_entryImm(void*, void*, s32);
void tapestryApi_tapestryUploadVertices(void*);
u8* tapestryApi_tapestryTextureCoordinates(void*);
void tapestryApi_tapestryDrawIndices(void*);
void tapestryApi_register_global_object(void*);
void tapestryApi_initializeMatrixStatics(void*);
void tapestryApi_initializeMathStatics(void*);
f32 tapestryApi_std_sqrtf(f32);
void tapestryApi_tapestryHIOConstruct(void*);
void tapestryApi_tapestryDrawDataConstruct(void*);
void tapestryApi_tapestrySetPointLight(void*, void*, void*);
u32 tapestryApi_PSMTXInverse(void*, void*);
void tapestryApi_tapestryWind(void*);
void tapestryApi_tapestryHitCoordinates(void*);
void tapestryApi_tapestryFireSimulation(void*);
void tapestryApi_tapestryPositions(void*);
void tapestryApi_tapestryNormals(void*);
void tapestryApi_tapestryParticlePositions(void*);
void tapestryApi_tapestrySmokeMove(void*, void*);
void tapestryApi_tapestryLightMove(void*, void*, void*);
f32 tapestryApi_cM_rnd();
void tapestryApi_dPa_followEcallBack_execute(void*, void*);
f32 tapestryApi_cM_rndF(f32);
u32 tapestryApi_tapestryStartFire(void*, s32, s32);
f32 tapestryApi_PSVECSquareMag(void*);
void tapestryApi_cXyz___dv(void*, void*, f32);
void tapestryApi_PSVECAdd(void*, void*, void*);
void tapestryApi_tapestrySmokeSet(void*);
s32 tapestryApi_cM_atan2s(f32, f32);
void tapestryApi_cLib_addCalc2(void*, f32, f32, f32);
u8* tapestryApi_dPa_control_c_setVariantA(void*, s32, s32, void*, void*, void*, s32, s32, s32, s32, s32, s32);
void tapestryApi_dPa_control_c_setVariantB(void*, s32, s32, void*, void*, void*, s32, void*, s32, s32, s32, s32);
void tapestryApi_daObjTapestryPLight_c_plight_make(void*);
s32 tapestryApi_tapestryCanStartFire(void*, s32, s32);
void tapestryApi_dPa_control_c_setVariantC(void*, s32, s32, void*, s32, void*, s32, void*, s32, s32, s32, s32);
void tapestryApi_tapestrySpringNeighbors(void*, s32, s32);
u8* tapestryApi_daObjTapestryPacket_c_calc_acc_gravity(void*);
void tapestryApi_tapestryWaveForce(void*, s32, s32);
void tapestryApi_tapestryHitForce(void*, s32, s32);
void tapestryApi_tapestryVelocity(void*, s32, s32);
void tapestryApi_tapestryPositionCorrection(void*, s32, s32);
void tapestryApi_tapestrySpringForce(void*, void*, void*, f32, f32);
void tapestryApi_PSMTXMultVecSR(void*, void*, void*);
f32 tapestryApi_cM3d_lineVsPosSuisenCross(void*, void*, void*, void*);
void tapestryApi_cBgS_GndChk_cBgS_GndChk(void*);
f32 tapestryApi_cBgS_GroundCross(void*, void*);
void tapestryApi_tapestryFireLeap(void*, s32, s32);
void tapestryApi_constructArrayCopy(void*, s32, s32, s32, void*);
void tapestryApi_cXyz___mi(void*, void*, void*);
u8* tapestryApi_daObjTapestryPacket_c_smokeCallback_init(void*);
void tapestryApi_tapestryRenderInit(void*);
void tapestryApi_PSVECScale(void*, void*, f32);
void tapestryApi_bgnPacket_construct(void*);
void tapestryApi_construct_arrayVariantB(void*, s32, s32, s32);
void tapestryApi_constructIndexBuffer(void*);
void tapestryApi_construct(void*);
void tapestryApi_constructMatPacket(void*);
void tapestryApi_clearMemory(void*, s32);
void tapestryApi_constructTextureResource(void*);
void tapestryApi_constructTextureSampler(void*);
void tapestryApi_tapestryLightConstruct(void*);
void tapestryApi_PSMTXIdentity(void*);
void tapestryApi_flushBuffer(void*, s32, u32);
void tapestryApi_prepareTevColors(void*);
void tapestryApi_tapestryMatrixCopy(void*, void*);
void tapestryApi_setupShader(void*, s32, void*, void*, void*);
void tapestryApi_tapestrySignedColor(void*, void*);
void tapestryApi_scaleColor(void*, void*, f32);
void tapestryApi_selectUniform(void*, s32);
void tapestryApi_tapestryByteColor(void*, void*);
void tapestryApi_scaleColorRGB(void*, f32);
void tapestryApi_prepareUniform(void*);
void tapestryApi_resetVertexAttribute(void*);
u32 tapestryApi_findFromRoot(void*, void*);
void tapestryApi_resetUniformBuffer(void*);
void tapestryApi_destroyTextureSampler(void*, s32);
void tapestryApi_destroyMatPacket(void*, s32);
void tapestryApi_destroyModelPacket(void*, s32);
void tapestryApi_destroyIndexBuffer(void*, s32);
void tapestryApi_destroyPacket(void*, s32);
void tapestryApi_cXyz_outprod(void*, void*, void*);
s32 tapestryApi_cXyz_normalizeRS(void*);
s32 tapestryApi_dCcD_GObjInf_ChkTgHit(void*);
u32 tapestryApi_dCcD_GObjInf_GetTgHitObj(void*);
void tapestryApi_dCcD_GObjInf_ClrTgHit(void*);
void tapestryApi_tapestrySetHit(void*, void*, void*, s32, f32, f32);
void tapestryApi_setPixelUniform(s16, void*, void*);
void tapestryApi_setVertexUniform(s16, void*, void*);
void tapestryApi_setGeometryUniform(s16, void*, void*);
void tapestryApi_bindTexture(void*, void*, s32, s32);
u32 tapestryApi_shaderCache(void*);
void tapestryApi_updateMaterial(void*, s32);
void tapestryApi_setShader(void*);
void tapestryApi_setMaterialData(void*, u32);
void tapestryApi_applyMaterial(void*);
void tapestryApi_applySurfaceShader(void*, void*);
void tapestryApi_constructRenderState(void*);
void tapestryApi_configureSurfaceRenderState(u32, void*);
void tapestryApi_restoreRenderState(void*);
void tapestryApi_prepareGeometry(void*);
void tapestryApi_drawIndexed(void*, s32, u32, void*, s32, s32);
u32 tapestryApi_getCurrentAllocator(void*);
u32 tapestryApi_getPacketModel(void*, void*);
s32 tapestryApi_findMaterialName(void*, void*);
void tapestryApi_initializeMaterial(void*, void*, s32);
void tapestryApi_initializeMaterialGroups(void*, void*, s32);
void tapestryApi_initializeVertexBuffer(void*, void*, s32, void*);
void tapestryApi_initializeVertexAttribute(void*, void*, void*, void*, s32);
void tapestryApi_initializeLightingMaterial(void*, s32, s32);
void tapestryApi_initializeIndexBuffer(void*, void*, s32, s32);
void tapestryApi_beginTextureAccess(void*);
u8* tapestryApi_loadArchiveResource(void*, void*, void*, s32);
void tapestryApi_initializeTextureResource(void*, void*, void*);
void tapestryApi_endTextureAccess(void*);
void tapestryApi_copyTextureSampler(void*, void*);

WWHD_OFFSET(daObjTapestryPacket_c, mVtable, 0xC);
WWHD_OFFSET(daObjTapestryPacket_c, mpOwner, 0x18);
WWHD_OFFSET(daObjTapestryPacket_c, mIndexBuffer, 0xA0C);
WWHD_OFFSET(daObjTapestryPacket_c, mLightingMaterial, 0xA24);
WWHD_OFFSET(daObjTapestryPacket_c, mVertexUniform, 0xA30);
WWHD_OFFSET(daObjTapestryPacket_c, mPixelUniform, 0xAD8);
WWHD_OFFSET(daObjTapestryPacket_c, mLightingUniforms, 0xB4C);
WWHD_OFFSET(daObjTapestryPacket_c, mShaderFlags, 0xE3C);
WWHD_OFFSET(daObjTapestryPacket_c, mTextureResource, 0xE40);
WWHD_OFFSET(daObjTapestryPacket_c, mTextureSampler, 0xED0);
WWHD_OFFSET(daObjTapestryPacket_c, mpTevStr, 0x1068);

namespace TapestryCollision {
enum Attack : u32 {
  AT_TYPE_SWORD = 0x2,
  AT_TYPE_UNK8 = 0x8,
  AT_TYPE_BOMB = 0x20,
  AT_TYPE_BOOMERANG = 0x40,
  AT_TYPE_BOKO_STICK = 0x80,
  AT_TYPE_FIRE = 0x200,
  AT_TYPE_MACHETE = 0x400,
  AT_TYPE_UNK800 = 0x800,
  AT_TYPE_SPIKE = 0x1000,
  AT_TYPE_UNK2000 = 0x2000,
  AT_TYPE_NORMAL_ARROW = 0x4000,
  AT_TYPE_HOOKSHOT = 0x8000,
  AT_TYPE_SKULL_HAMMER = 0x10000,
  AT_TYPE_UNK20000 = 0x20000,
  AT_TYPE_FIRE_ARROW = 0x40000,
  AT_TYPE_ICE_ARROW = 0x80000,
  AT_TYPE_LIGHT_ARROW = 0x100000,
  AT_TYPE_WIND = 0x200000,
  AT_TYPE_STALFOS_MACE = 0x1000000,
  AT_TYPE_DARKNUT_SWORD = 0x4000000,
  AT_TYPE_MOBLIN_SPEAR = 0x10000000,
};
}
WWHD_OFFSET(daObjTapestry_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjTapestry_c, mpModel, 0x3B8);
WWHD_OFFSET(daObjTapestry_c, mpBg, 0x3BC);
WWHD_OFFSET(daObjTapestry_c, mTriangles, 0x3F0);
WWHD_OFFSET(daObjTapestry_c, mCollisionStatus, 0x690);
WWHD_OFFSET(daObjTapestry_c, mOpacity, 0x710);
