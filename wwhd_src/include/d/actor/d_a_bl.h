/* Bubble enemy, verified WWHD layout (0xE18).
 * HD: the GameCube actor layout is not reconstructed; fields name the audited HD state. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_player.h"
#include "d/d_path.h"
// Embedded HD collision objects retain their audited extents. Their internal
// interfaces are accessed through guest calls; gameplay state is named below.
struct BubbleBodyCollider {
    be<u32> attackFlags;
    be<u32> attackType;
    u8 pad008[0x8];
    be<u32> attackAttributes;
    u8 pad014[0x4];
    be<u32> targetFlags;
    u8 pad01C[0x10];
    be<u32> correctionFlags;
    u8 pad030[0x14];
    be<u32> status;
    u8 pad048[0x8];
    u8 contactObject[4];
    be<u32> contactFlags;
    u8 pad058[0x17];
    be<u8> targetHitType;
    u8 pad070[0x24];
    be<u32> targetLinkFlags;
    u8 pad098[0x18];
    be<u8> targetState;
    u8 pad0B1[0x1];
    be<u8> targetMaterial;
    u8 pad0B3[0x19];
    cXyz targetHitPosition;
    u8 pad0D8[0x40];
    u8 sphere[0x18];
    u8 pad130[0x20];
};

WWHD_OFFSET(BubbleBodyCollider, targetFlags, 0x18);
WWHD_OFFSET(BubbleBodyCollider, correctionFlags, 0x2C);
WWHD_OFFSET(BubbleBodyCollider, contactFlags, 0x54);
WWHD_OFFSET(BubbleBodyCollider, targetState, 0xB0);
WWHD_OFFSET(BubbleBodyCollider, sphere, 0x118);
WWHD_SIZE(BubbleBodyCollider, 0x150);
// HD: embedded damage helpers retain their original guest storage and call-site widths.
struct BubbleIceState {
    be<u32> owner;
    be<s16> freezeTimer;
    be<u8> state;
    u8 pad007;
    be<f32> speed;
    u8 pad00C[2];
    be<s16> thawTimer;
    u8 pad010[0x20];
};
WWHD_OFFSET(BubbleIceState, thawTimer, 0xE);
WWHD_SIZE(BubbleIceState, 0x30);

struct BubbleGroundCollision {
    u8 pad000[0x10];
    be<u32> baseVtable;
    be<u32> objectVtable;
    be<u8> constructed;
    u8 pad019[0x7];
    be<u32> actorVtable;
    u8 pad024[0x4];
    be<u32> flags;
    u8 pad02C[0x68];
    be<f32> groundHeight;
    u8 pad098[0x50];
    u8 groundPolygon[0x14];
    u8 pad0FC[0xC8];
};
WWHD_OFFSET(BubbleGroundCollision, baseVtable, 0x10);
WWHD_OFFSET(BubbleGroundCollision, objectVtable, 0x14);
WWHD_OFFSET(BubbleGroundCollision, constructed, 0x18);
WWHD_OFFSET(BubbleGroundCollision, actorVtable, 0x20);
WWHD_OFFSET(BubbleGroundCollision, flags, 0x28);
WWHD_OFFSET(BubbleGroundCollision, groundHeight, 0x94);
WWHD_OFFSET(BubbleGroundCollision, groundPolygon, 0xE8);
WWHD_SIZE(BubbleGroundCollision, 0x1C4);
struct BubbleCollisionStatus {
    u8 pad000[0x14];
    be<u8> weight;
    u8 pad015[0x3];
    be<u32> baseVtable;
    be<u32> globalVtable;
    u8 pad020[0x1C];
};
WWHD_OFFSET(BubbleCollisionStatus, weight, 0x14);
WWHD_OFFSET(BubbleCollisionStatus, baseVtable, 0x18);
WWHD_OFFSET(BubbleCollisionStatus, globalVtable, 0x1C);
WWHD_SIZE(BubbleCollisionStatus, 0x3C);
struct BubbleGroundCircle {
    u8 pad000[0x14];
    u8 circle[0x2c];
};
WWHD_OFFSET(BubbleGroundCircle, circle, 0x14);
WWHD_SIZE(BubbleGroundCircle, 0x40);
struct BubbleFireCylinder {
    u8 pad000[0x3C];
    be<u32> correctionFlags;
    u8 pad040[0xD0];
    be<u32> shapeVtable;
    be<u32> cylinderVtable;
    u8 cylinder[0x14];
    be<u32> collisionVtable;
    be<f32> radius;
    be<f32> height;
    u8 pad138[0x8];
    be<f32> scale;
    u8 pad144[0x4];
};
WWHD_OFFSET(BubbleFireCylinder, correctionFlags, 0x3C);
WWHD_OFFSET(BubbleFireCylinder, shapeVtable, 0x110);
WWHD_OFFSET(BubbleFireCylinder, cylinderVtable, 0x114);
WWHD_OFFSET(BubbleFireCylinder, cylinder, 0x118);
WWHD_OFFSET(BubbleFireCylinder, collisionVtable, 0x12C);
WWHD_OFFSET(BubbleFireCylinder, radius, 0x130);
WWHD_OFFSET(BubbleFireCylinder, height, 0x134);
WWHD_OFFSET(BubbleFireCylinder, scale, 0x140);
WWHD_SIZE(BubbleFireCylinder, 0x148);
struct BubbleSmokeCallback {
    u8 pad000[0x4];
    be<u32> emitter;
    u8 pad008[0xE];
    be<u32> color;
    u8 pad01A[0x6];
};
WWHD_OFFSET(BubbleSmokeCallback, emitter, 0x4);
WWHD_OFFSET(BubbleSmokeCallback, color, 0x16);
WWHD_SIZE(BubbleSmokeCallback, 0x20);
struct AttackInfo {
    be<u32> hitObject;
    u8 pad004[0x10];
    be<u32> attackPower;
    u8 pad018[0x8];
};
WWHD_OFFSET(AttackInfo, hitObject, 0x0);
WWHD_OFFSET(AttackInfo, attackPower, 0x14);
WWHD_SIZE(AttackInfo, 0x20);
struct LineCheck {
    be<u32> groupPass;
    be<u32> objectPass;
    u8 pad008[0x8];
    be<u32> baseVtable;
    u8 pad014[0xC];
    be<u32> lineVtable;
    u8 pad024[0x34];
    be<u32> groupVtable;
    be<u8> groupFlags[7];
    u8 pad063[0x1];
    be<u32> objectVtable;
    be<u32> objectFlags;
};
WWHD_OFFSET(LineCheck, groupPass, 0x0);
WWHD_OFFSET(LineCheck, objectPass, 0x4);
WWHD_OFFSET(LineCheck, baseVtable, 0x10);
WWHD_OFFSET(LineCheck, lineVtable, 0x20);
WWHD_OFFSET(LineCheck, groupVtable, 0x58);
WWHD_OFFSET(LineCheck, groupFlags, 0x5C);
WWHD_OFFSET(LineCheck, objectVtable, 0x64);
WWHD_OFFSET(LineCheck, objectFlags, 0x68);
WWHD_SIZE(LineCheck, 0x6C);

struct BubbleFireSphere {
    u8 collision[0x14C];
    be<f32> scale;
};
WWHD_OFFSET(BubbleFireSphere, scale, 0x14C);
WWHD_SIZE(BubbleFireSphere, 0x150);
struct BubbleFireHelper {
    be<u32> owner;
    be<s16> bounceCount;
    u8 pad006[6];
    be<u32> morph;
    be<u8> byteParameters[10];
    u8 pad01A[2];
    be<f32> floatParameters[10];
    u8 pad044[0x48];
    cXyz position;
    u8 pad098[8];
    BubbleCollisionStatus status;
    BubbleFireSphere sphere;
};
WWHD_OFFSET(BubbleFireHelper, morph, 0xC);
WWHD_OFFSET(BubbleFireHelper, byteParameters, 0x10);
WWHD_OFFSET(BubbleFireHelper, floatParameters, 0x1C);
WWHD_OFFSET(BubbleFireHelper, position, 0x8C);
WWHD_OFFSET(BubbleFireHelper, status, 0xA0);
WWHD_OFFSET(BubbleFireHelper, sphere, 0xDC);
WWHD_SIZE(BubbleFireHelper, 0x22C);

struct bl_class : fopAc_ac_c {
    u8 pad3AC[0x1C];
    request_of_phase_process_class phase;
    gptr<u8> morph;
    gptr<u8> modelAuxiliary;
    gptr<u8> colorAnimation;
    gptr<u8> textureAnimation;
    cXyz roamingOrigin;
    be<u8> variant;
    be<u8> flying;
    be<u8> action;
    be<u8> burning;
    be<u8> damageKind;
    be<u8> switchNo;
    u8 pad3F2[2];
    be<u32> path;
    u8 pad3F8[0xC];
    be<s8> pathPoint;
    be<u8> pathNo;
    be<s8> stolenItemCount;
    u8 pad407;
    be<s16> timers[6];
    be<s16> spinX;
    be<s16> spinZ;
    be<s16> bounceOrSmokeFade;
    be<s16> skullSmokeFade;
    be<s16> targetYaw;
    be<s16> attackYawOffset;
    be<s16> flightPhase;
    be<s16> actionStep;
    be<s32> animationId;
    u8 pad428[4];
    be<f32> bounceSpeed;
    be<f32> modelYOffset;
    be<f32> targetHeight;
    be<f32> targetScale;
    be<f32> homeGroundHeight;
    be<f32> pathSpeed;
    be<f32> collisionRadius;
    BubbleGroundCircle groundCircle;
    BubbleGroundCollision groundCollision;
    BubbleCollisionStatus bodyCollisionStatus;
    BubbleBodyCollider bodyCollider;
    cXyz smokePosition;
    BubbleSmokeCallback smokeCallback;
    dPa_followEcallBack appearanceCallback;
    dPa_followEcallBack fireCallback;
    BubbleIceState iceState;
    BubbleCollisionStatus fireCollisionStatus;
    BubbleFireCylinder fireCylinder;
    BubbleGroundCircle fireGroundCircle;
    BubbleGroundCollision fireGroundCollision;
    BubbleFireHelper fireHelper;
    u8 lightInfo[8];
};
WWHD_OFFSET(bl_class, phase, 0x3C8);
WWHD_OFFSET(bl_class, morph, 0x3D0);
WWHD_OFFSET(bl_class, roamingOrigin, 0x3E0);
WWHD_OFFSET(bl_class, variant, 0x3EC);
WWHD_OFFSET(bl_class, path, 0x3F4);
WWHD_OFFSET(bl_class, timers, 0x408);
WWHD_OFFSET(bl_class, actionStep, 0x422);
WWHD_OFFSET(bl_class, targetScale, 0x438);
WWHD_OFFSET(bl_class, groundCircle, 0x448);
WWHD_OFFSET(bl_class, groundCollision, 0x488);
WWHD_OFFSET(bl_class, bodyCollisionStatus, 0x64C);
WWHD_OFFSET(bl_class, bodyCollider, 0x688);
WWHD_OFFSET(bl_class, smokePosition, 0x7D8);
WWHD_OFFSET(bl_class, appearanceCallback, 0x804);
WWHD_OFFSET(bl_class, fireCallback, 0x818);
WWHD_OFFSET(bl_class, iceState, 0x82C);
WWHD_OFFSET(bl_class, fireCollisionStatus, 0x85C);
WWHD_OFFSET(bl_class, fireCylinder, 0x898);
WWHD_OFFSET(bl_class, fireGroundCircle, 0x9E0);
WWHD_OFFSET(bl_class, fireGroundCollision, 0xA20);
WWHD_OFFSET(bl_class, fireHelper, 0xBE4);
WWHD_OFFSET(bl_class, lightInfo, 0xE10);
WWHD_SIZE(bl_class, 0xE18);

using BubbleHeapCallback = BOOL (*)(bl_class*);

// HD: exact call-site signatures preserve the guest register widths.
void bl_MtxTrans(int, float, float, float);
void bl_mDoMtx_YrotM(unsigned char*, short);
void bl_mDoMtx_XrotM(unsigned char*, short);
void bl_mDoMtx_ZrotM(unsigned char*, short);
void bl_mDoExt_McaMorf_calc(unsigned char*);
void bl_enemy_fire(unsigned char*);
dScnKy_env_light_c* bl_getEnvLight();
void bl_dScnKy_env_light_c_settingTevStruct(unsigned char*, int, cXyz*, unsigned char*);
void bl_setLightTevColorType(unsigned char*, unsigned char*, unsigned char*);
int bl_dSv_info_c_isSwitch(unsigned char*, unsigned char, signed char);
unsigned int bl_getGameInfo();
void bl_dSnap_RegistFig(int, bl_class*, float, float, float);
void bl_dMat_ice_c_entryDL(unsigned char*, int, unsigned char*);
void bl_mDoExt_btkAnm_entry(unsigned char*, unsigned char*, float);
void bl_touchGameInfo();
void bl_mDoExt_McaMorf_entryDL(unsigned char*);
void bl_dPa_control_c_set(unsigned char*, int, int, cXyz*, csXyz*, int, int, unsigned char*, signed char, int,
                          int, int);
void bl_dPa_control_c_set_2(unsigned char*, int, unsigned int, cXyz*, int, int, int, unsigned char*,
                            signed char, int, int, int);
void bl_JPAGetXYZRotateMtx(int, short, int, unsigned char*);
void bl_dPa_followEcallBack_end(unsigned char*);
void bl_JPASetRMtxTVecfromMtx(unsigned char*, unsigned char*, unsigned char*);
int bl_dComIfGp_getReverb(signed char);
void bl_seStartFull_1(int, cXyz*, int, int);
void bl_PSMTXCopy(unsigned char*, unsigned char*);
float bl_sqrtf(float);
unsigned char* bl_dRes_control_c_getRes(unsigned char*, SafeString*, int);
void bl_mDoExt_McaMorf_setAnm(unsigned char*, unsigned char*, unsigned char, unsigned char*, float, float,
                              float, float);
void bl_dCcD_GStts_Move(unsigned char*);
int bl_dCcD_GObjInf_ChkTgHit(unsigned char*);
unsigned int bl_dCcD_GObjInf_GetTgHitObj(unsigned char*);
int bl_fopAcM_searchActorAngleY(bl_class*, unsigned char*);
void bl_seStartFull_2(unsigned int, cXyz*, unsigned int, int);
void bl_seStartLevelFull_1(int, cXyz*, unsigned int, int, int);
void bl_dPa_control_c_set_3(unsigned char*, int, unsigned int, cXyz*, void*, cXyz*, int, int, int, int, int,
                            int);
void bl_cc_at_check(bl_class*, AttackInfo*);
void bl_enemy_fire_remove(unsigned char*);
void bl_seStartFull_3(int, cXyz*, unsigned int, int);
void bl_cLib_addCalc2(gabi::be<float>*, float, float, float);
void bl_dBgS_AcchCir_SetWall(unsigned char*, float, float);
void bl_dBgS_Acch_CrrPos(unsigned char*, unsigned char*);
void bl_cLib_addCalcAngleS2(gabi::be<short>*, int, int, int);
void bl_cBgS_LinChk_ct(LineCheck*);
float bl_cM_rndFX(float);
void bl_mDoMtx_YrotS(unsigned char*, short);
void bl_MtxPosition(cXyz*, cXyz*);
void bl_PSVECAdd(cXyz*, cXyz*, cXyz*);
void bl_dBgS_LinChk_Set(LineCheck*, cXyz*, cXyz*, bl_class*);
int bl_cBgS_LineCross(unsigned char*, LineCheck*);
void bl_lineCheckDestructor(LineCheck*, int);
void bl_seStartLevelFull_2(unsigned int, unsigned char*, unsigned int, int, int);
float bl_fopAcM_searchActorDistance(bl_class*, unsigned char*);
void bl_dPa_control_c_set_4(unsigned char*, int, int, cXyz*, int, int, int, unsigned char*, int, int, int,
                            int);
void bl_cLib_addCalc2_2(unsigned char*, float, float, float);
int bl_J3DFrameCtrl_checkPass(unsigned char*, float);
short bl_cM_atan2s(float, float);
float bl_cM_rndF(float);
int bl_cM_atan2s_2(float, float);
void bl_cLib_addCalcAngleS2_2(gabi::be<short>*, short, int, int);
void bl_cLib_addCalc0(gabi::be<float>*, float, float);
int bl_dCcD_GObjInf_ChkAtHit(unsigned char*);
unsigned int bl_dCcD_GAtTgCoCommonBase_GetAc(unsigned char*);
void bl_fopAcM_createDisappear(bl_class*, cXyz*, int, int, int);
void bl_dSv_info_c_onActor(unsigned char*, unsigned short, signed char);
void bl_fopAcM_delete(bl_class*);
float bl_cM_rnd();
unsigned int bl_dPa_control_c_set_5(unsigned char*, int, int, cXyz*, csXyz*, cXyz*, int, int, signed char,
                                    unsigned char*, unsigned char*, int);
void bl_dCcD_GObjInf_ClrTgHit(unsigned char*);
void bl_dPa_smokeEcallBack_end(unsigned char*);
void bl_fopAcM_createItem(cXyz*, int, int, int, int, int, int, int);
void bl_mDoExt_baseAnm_play(unsigned char*);
int bl_enemy_ice(unsigned char*);
unsigned int bl_dBgS_GetMtrlSndId(unsigned char*, unsigned char*);
void bl_mDoExt_McaMorf_play(unsigned char*, unsigned char*, unsigned int, int);
void bl_cM3dGSph_SetC(unsigned char*, cXyz*);
void bl_cM3dGSph_SetR(unsigned char*, float);
void bl_cCcS_Set(unsigned char*, unsigned char*);
void bl_fopAcM_posMove(bl_class*, unsigned char*);
int bl_cBgS_ChkPolySafe(unsigned char*, unsigned char*);
int bl_dBgS_GetGroundCode(unsigned char*, unsigned char*);
unsigned char* bl_mDoExt_McaMorf_mDoExt_McaMorf(int, unsigned char*, int, int, unsigned char*, int, int, int,
                                                int, int, int, int, float);
unsigned char* bl_allocateBytes(int);
unsigned char* bl_mDoExt_btkAnm_init(unsigned char*);
int bl_textureAnimationInit(unsigned char*, unsigned char*, unsigned char*, int, int, int, int, int, int,
                            float);
int bl_invisibleModelCreate(unsigned char*, unsigned char*);
void bl_dComIfG_resDelete(request_of_phase_process_class*, const char*);
void bl_allocateVectorSubobject(int);
void bl_collisionStatusConstructor(unsigned char*);
void bl_dCcD_GStts_dCcD_GStts(unsigned char*);
void bl_collisionSphereConstructor(unsigned char*);
bl_class* bl_allocateActor(int);
void bl_fopAc_ac_c_fopAc_ac_c(bl_class*);
void bl_dBgS_AcchCir_dBgS_AcchCir(unsigned char*);
void bl_dBgS_Acch_dBgS_Acch(unsigned char*);
void bl_dPa_smokeEcallBack_dPa_smokeEcallBack(unsigned char*, int);
void bl_dPa_followEcallBack_dPa_followEcallBack(unsigned char*, int, int);
void bl_dCcD_GObjInf_dCcD_GObjInf(unsigned char*);
void bl_cylinderConstructor(unsigned char*);
void bl_invisibleModelConstructor(unsigned char*);
int bl_dComIfG_resLoad(request_of_phase_process_class*, const char*);
int bl_fopAcM_entrySolidHeap(bl_class*, BubbleHeapCallback, int);
unsigned char* bl_dPath_GetRoomPath(unsigned char, signed char);
unsigned char* bl_dPath_GetRoomPath_2(int, signed char);
void bl_dBgS_Acch_Set(unsigned char*, cXyz*, cXyz*, bl_class*, int, unsigned char*, cXyz*, int, int);
void bl_dCcD_Stts_Init(unsigned char*, int, int, bl_class*);
void bl_dCcD_Sph_Set(unsigned char*, unsigned char*);
int bl_cDT_NamePTbl_GetIndex(unsigned char*, const char*, int);
void bl_register_global_object(unsigned char*);
void bl_fixedVectorConstructor(unsigned char*);
void bl_vectorConstructor(unsigned char*);
void bl_dl(unsigned char*);
void bl_invisibleModelDestructor(unsigned char*, int);
void bl_collisionSphereDestructor(unsigned char*, int);
void bl_collisionStatusDestructor(unsigned char*, int);
void bl_groundCollisionDestructor(unsigned char*, int);
void bl_groundCircleDestructor(unsigned char*, int);
void bl_dBgS_Acch_dBgS_Acch_2(unsigned char*, int);
void bl_fopAc_ac_c_fopAc_ac_c_2(bl_class*, int);
void bl_dl_2(bl_class*);

struct BubbleModel {
    u8 pad000[0x2C];
    be<u32> jointController;
    u8 pad030[0x7C];
    be<u32> modelData;
    u8 pad0B0[0x8];
    be<u32> userArea;
    be<f32> scaleX;
    be<f32> scaleY;
    be<f32> scaleZ;
    Mtx34 baseMatrix;
};
WWHD_OFFSET(BubbleModel, jointController, 0x2C);
WWHD_OFFSET(BubbleModel, modelData, 0xAC);
WWHD_OFFSET(BubbleModel, userArea, 0xB8);
WWHD_OFFSET(BubbleModel, scaleX, 0xBC);
WWHD_OFFSET(BubbleModel, scaleY, 0xC0);
WWHD_OFFSET(BubbleModel, scaleZ, 0xC4);
WWHD_OFFSET(BubbleModel, baseMatrix, 0xC8);
WWHD_SIZE(BubbleModel, 0xF8);

struct BubbleJointController {
    u8 pad000[0x4];
    be<u16> flags;
    u8 pad006[0xA];
    gptr<Mtx34> matrices;
};
WWHD_OFFSET(BubbleJointController, flags, 0x4);
WWHD_OFFSET(BubbleJointController, matrices, 0x10);
WWHD_SIZE(BubbleJointController, 0x14);

struct BubbleEmitter {
    u8 pad000[0x34];
    be<f32> emissionRate;
    u8 pad038[0x24];
    be<u32> maximumParticles;
    be<u16> lifetime;
    u8 pad062[0x6];
    be<f32> initialVelocity;
    be<f32> velocityRandomness;
    be<f32> spread;
    u8 pad074[0x17C];
    Mtx34 rotation;
    u8 pad220[0xC];
    cXyz translation;
    be<f32> scaleX;
    be<f32> scaleY;
    be<f32> scaleZ;
    u8 pad244[0x3];
    be<u8> alpha;
    be<u8> red;
    be<u8> green;
    be<u8> blue;
    u8 pad24B[0x9];
    be<u32> flags;
};
WWHD_OFFSET(BubbleEmitter, emissionRate, 0x34);
WWHD_OFFSET(BubbleEmitter, maximumParticles, 0x5C);
WWHD_OFFSET(BubbleEmitter, lifetime, 0x60);
WWHD_OFFSET(BubbleEmitter, initialVelocity, 0x68);
WWHD_OFFSET(BubbleEmitter, velocityRandomness, 0x6C);
WWHD_OFFSET(BubbleEmitter, spread, 0x70);
WWHD_OFFSET(BubbleEmitter, scaleX, 0x238);
WWHD_OFFSET(BubbleEmitter, scaleY, 0x23C);
WWHD_OFFSET(BubbleEmitter, scaleZ, 0x240);
WWHD_OFFSET(BubbleEmitter, alpha, 0x247);
WWHD_OFFSET(BubbleEmitter, red, 0x248);
WWHD_OFFSET(BubbleEmitter, green, 0x249);
WWHD_OFFSET(BubbleEmitter, blue, 0x24A);
WWHD_OFFSET(BubbleEmitter, flags, 0x254);
WWHD_SIZE(BubbleEmitter, 0x258);

struct BubbleAnimation {
    u8 pad000[0x4];
    be<f32> frame;
};
WWHD_OFFSET(BubbleAnimation, frame, 0x4);
WWHD_SIZE(BubbleAnimation, 0x8);

struct BubbleModelData {
    u8 pad000[0x44];
    be<u32> materialAnimation;
};
WWHD_OFFSET(BubbleModelData, materialAnimation, 0x44);
WWHD_SIZE(BubbleModelData, 0x48);

struct BubbleCamera {
    u8 pad000[0x236];
    be<s16> yaw;
};
WWHD_OFFSET(BubbleCamera, yaw, 0x236);
WWHD_SIZE(BubbleCamera, 0x238);

struct BubbleAttackHit {
    u8 pad000[0x10];
    be<u32> attackType;
};
WWHD_OFFSET(BubbleAttackHit, attackType, 0x10);
WWHD_SIZE(BubbleAttackHit, 0x14);

// GHS copies coordinate storage as integer words, preserving signaling NaNs and store order.
struct BubbleVectorWords {
    be<u32> x, y, z;
};
WWHD_OFFSET(BubbleVectorWords, z, 8);
WWHD_SIZE(BubbleVectorWords, 12);
inline BubbleVectorWords& bl_words(cXyz* value) {
    return gabi::object_at<BubbleVectorWords>(gabi::ea(value));
}
struct BubbleHalfBytes {
    be<u8> high, low;
};
WWHD_SIZE(BubbleHalfBytes, 2);
inline BubbleHalfBytes& bl_bytes(be<s16>* value) { return gabi::object_at<BubbleHalfBytes>(gabi::ea(value)); }

inline u8 bl_playerCutType(daPy_py_c* player) { return gabi::object_at<be<u8>>(gabi::ea(&player->mCutType)); }

struct BubbleMorph {
    u8 pad000[0x90];
    be<u32> model;
    u8 pad094[4];
    be<f32> frameRate;
    u8 pad09C[0xB];
    be<u8> flags;
};
WWHD_OFFSET(BubbleMorph, model, 0x90);
WWHD_OFFSET(BubbleMorph, frameRate, 0x98);
WWHD_OFFSET(BubbleMorph, flags, 0xA7);
WWHD_SIZE(BubbleMorph, 0xA8);
struct BubbleStringVtable {
    u8 pad000[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(BubbleStringVtable, assureTermination, 0x14);
WWHD_SIZE(BubbleStringVtable, 0x18);
struct BubbleCallbackVtable {
    u8 pad000[0x44];
    be<u32> end;
};
WWHD_OFFSET(BubbleCallbackVtable, end, 0x44);
WWHD_SIZE(BubbleCallbackVtable, 0x48);
struct BubbleCallbackRoot {
    be<u32> vtable;
};
WWHD_SIZE(BubbleCallbackRoot, 4);

inline BubbleModel* bl_getMorphModel(u8* morph) {
    return gabi::at<BubbleModel>((u32)gabi::object_at<BubbleMorph>(gabi::ea(morph)).model);
}
inline BubbleModel* bl_getModel(bl_class* actor) { return bl_getMorphModel(actor->morph); }

enum BubbleAction {
    BL_ACTION_DOUSA = 0,
    BL_ACTION_KOUGEKI = 1,
    BL_ACTION_SAGARIMASU = 2,
    BL_ACTION_KAZE_MOVE = 3,
    BL_ACTION_ITAIYO = 4,
    BL_ACTION_HOOK = 5,
    BL_ACTION_COME_WAIT = 6,
    BL_ACTION_SKULL = 10,
};

u8* constructFire(u8* fire);
bl_class* constructBubble(bl_class* actor);

WWHD_OFFSET(BubbleEmitter, rotation, 0x1F0);
WWHD_OFFSET(BubbleEmitter, translation, 0x22C);

// HD: the save pointer names a root with a prefix before the dSv_info_c subobject.
struct BubbleSaveRoot {
    u8 prefix[0x20];
    dSv_info_c save;
};
WWHD_OFFSET(BubbleSaveRoot, save, 0x20);
WWHD_SIZE(BubbleSaveRoot, 0x12C0);
inline dSv_info_c* bl_saveInfo(u32 root) {
    return gabi::at<dSv_info_c>(gabi::ea(&gabi::object_at<BubbleSaveRoot>(root).save));
}
// HD: the item-name table occupies an opaque portion of the shared play layout.
struct BubblePlayTables {
    u8 prefix[0x3E0C];
    u8 itemNames;
};
WWHD_OFFSET(BubblePlayTables, itemNames, 0x3E0C);
WWHD_SIZE(BubblePlayTables, 0x3E0D);
inline u8* bl_itemNameTable(dComIfG_play_c* play) {
    return gabi::at<u8>(gabi::ea(&gabi::object_at<BubblePlayTables>(gabi::ea(play)).itemNames));
}
