// Gohdan base unit (d_a_bst.cpp): tables, HIO and data views; the actor is bst_class (d_a_bst.h).
#pragma once
#include "d/actor/d_a_bst_move.h"
struct BstBaseLight {
    be<f32> mPositionDirection[6];
    be<u8> mColor[4];
    be<s16> mShortParams[4];
    be<f32> mFloatParams[8];
};
struct BstBaseLighting {
    BstBaseLight mLight;
    u8 _44[0x40];
    be<u32> mLightPosWorld[3];
    be<u16> mColorC0[4];
    be<u32> mColorK[2];
    be<u16> mFogColor[4];
    be<f32> mFogParams[3];
    be<u8> mFlags[9];
    u8 _BD[3];
    BstBaseLight mHdLight1;
    u8 _104[0x40];
    BstBaseLight mHdLight2;
    u8 _188[0x40];
};
WWHD_SIZE(BstBaseLight, 0x44);
WWHD_OFFSET(BstBaseLight, mColor, 0x18);
WWHD_OFFSET(BstBaseLight, mShortParams, 0x1C);
WWHD_OFFSET(BstBaseLight, mFloatParams, 0x24);
WWHD_SIZE(BstBaseLighting, 0x1C8);
WWHD_OFFSET(BstBaseLighting, mLightPosWorld, 0x84);
WWHD_OFFSET(BstBaseLighting, mColorC0, 0x90);
WWHD_OFFSET(BstBaseLighting, mColorK, 0x98);
WWHD_OFFSET(BstBaseLighting, mFogColor, 0xA0);
WWHD_OFFSET(BstBaseLighting, mFogParams, 0xA8);
WWHD_OFFSET(BstBaseLighting, mFlags, 0xB4);
WWHD_OFFSET(BstBaseLighting, mHdLight1, 0xC0);
WWHD_OFFSET(BstBaseLighting, mHdLight2, 0x144);

// HD: dKy_tevstr_c as the constructor fills it (light block and the two HD copies of it).
inline BstBaseLighting& bstBaseLighting(dKy_tevstr_c& tevstr) { return *reinterpret_cast<BstBaseLighting*>(&tevstr); }
struct BstBaseGlobals {
    be<u32> hand[2];
    be<s8> m4;
    be<u8> m5, m6;
    u8 _0B;
    be<u32> mHioVtable;
    be<u32> boss, msg;
    be<f32> mHeaderValues[2];
    be<u8> msg_end, mAngle, mVector;
    u8 _23;
    be<u32> mHeaderZeros[4];
    cXyz center_pos;
    be<u32> mCenterGuard;
};
WWHD_OFFSET(BstBaseGlobals, boss, 0x10);
WWHD_OFFSET(BstBaseGlobals, msg, 0x14);
WWHD_OFFSET(BstBaseGlobals, mHeaderValues, 0x18);
WWHD_OFFSET(BstBaseGlobals, msg_end, 0x20);
WWHD_OFFSET(BstBaseGlobals, mHeaderZeros, 0x24);
WWHD_OFFSET(BstBaseGlobals, center_pos, 0x34);
WWHD_OFFSET(BstBaseGlobals, mCenterGuard, 0x40);
WWHD_SIZE(BstBaseGlobals, 0x44);
struct BstBaseHio {
    be<s8> m4;
    be<u8> m5, m6;
    u8 _03;
    be<u32> mVtable;
};
WWHD_SIZE(BstBaseHio, 8);
struct BstBaseHitInfo {
    be<u32> mWord[7];
};
WWHD_SIZE(BstBaseHitInfo, 0x1C);
struct BstBaseShapeAttribute {
    u8 _00[0x18];
    be<u32> mVtable, mShapeVtable;
};
WWHD_SIZE(BstBaseShapeAttribute, 0x20);
struct BstBaseJoint {
    u8 _00[4];
    be<u16> mJntNo;
    be<u16> _06;
    be<u16> mCount;
};
WWHD_SIZE(BstBaseJoint, 0xA);
struct BstBaseModelData {
    u8 _00[4];
    be<u32> mJointCount, mpJoints;
    u8 _0C[0x38];
    be<u32> mpTextureMatrixAnimator, mpTevAnimator;
};
WWHD_OFFSET(BstBaseModelData, mpTextureMatrixAnimator, 0x44);
WWHD_SIZE(BstBaseModelData, 0x4C);
struct BstBaseJointNode {
    u8 _00[8];
    be<u32> mCallback;
    u8 _0C[0x10];
};
WWHD_SIZE(BstBaseJointNode, 0x1C);
struct BstBaseSmokeVtable {
    u8 _00[0x44];
    be<u32> mRemove;
};
WWHD_SIZE(BstBaseSmokeVtable, 0x48);
struct BstBaseMessage {
    u8 _00[0x6AB];
    be<u8> mSend;
};
WWHD_SIZE(BstBaseMessage, 0x6AC);
struct BstBaseBlur {
    be<u8> mEnabled, mRate;
};
WWHD_SIZE(BstBaseBlur, 2);
struct BstBaseRoom {
    u8 _00[0x1E44];
    be<u8> mBossClear;
};
WWHD_SIZE(BstBaseRoom, 0x1E45);
struct BstBaseDefaults {
    f32 initial[6], tail[8];
    u8 flags[4];
    s16 angles[4];
};
WWHD_SIZE(BstBaseDefaults, 0x44);
using BstBaseID = be<u32>;
template <class T> inline T *bstBaseAddress(u32 a) { return gabi::at<T>(a); }
inline bst_class *bstBaseActor(u32 a) { return gabi::at<bst_class>(a); }
inline BstBaseGlobals &bstBaseGlobals() { return *gabi::at<BstBaseGlobals>(0x10462978); }
inline be<u8> &bstBaseHioSet() { return *gabi::at<be<u8>>(0x10192910); }
inline be<u32> &bstBaseMessagePointer() { return *gabi::at<be<u32>>(0x101F4B5C); }
inline be<u32> &bstBaseRoomPointer() { return *gabi::at<be<u32>>(0x101FFC78); }
inline BstBaseBlur &bstBaseBlur() { return *gabi::at<BstBaseBlur>(0x101F4825); }
inline BstBaseLight &bstBaseLightDefaults() { return *gabi::at<BstBaseLight>(0x1016E414); }
inline BstBaseLighting *bstBaseLighting(u32 a) { return gabi::at<BstBaseLighting>(a); }
inline BstBaseLight *bstBaseLight(u32 a) { return gabi::at<BstBaseLight>(a); }
inline be<f32> &bstBaseVectorAxis(u32 a, u32 i) { return *gabi::at<be<f32>>(a + i * 4); }
inline be<f32> &bstBaseMinusOne() { return *gabi::at<be<f32>>(0x1000B858); }
inline be<f32> &bstBaseSeventy() { return *gabi::at<be<f32>>(0x1000B894); }
inline be<f32> &bstBaseChaseRatio() { return *gabi::at<be<f32>>(0x1000B8E4); }
inline be<f32> *bstBaseInitialHeaderValues() { return gabi::at<be<f32>>(0x1000B918); }
inline constexpr dBgS_GndChk_vt BstBaseGroundVtable = {0x1000B75C, 0x1000B76C, 0x1000B78C,
                                                       0x1000B77C};
inline constexpr dBgS_LinChk_vt BstBaseLineVtable = {0x1000B7CC, 0x1000B7DC, 0x1000B7FC,
                                                     0x1000B7EC};

inline u32 bstBaseCurrentModel() { return gabi::ea(j3dSys.getModel()); }
inline BstBaseShapeAttribute *bstBaseShape(u8 *a) {
    return reinterpret_cast<BstBaseShapeAttribute *>(a);
}
inline be<u32> *bstBaseCoFlags(u32 a) { return gabi::at<be<u32>>(a); }
inline be<f32> *bstBaseAxis(cXyz *v, u32 i) { return i == 0 ? &v->x : i == 1 ? &v->y : &v->z; }
template <class T> inline be<u32> &bstBasePointerWord(gptr<T> &p) {
    return *reinterpret_cast<be<u32> *>(&p);
}

inline be<u32> &bstBasePart(u32 i) {
    return i < 2 ? bstBaseGlobals().hand[i] : bstBaseGlobals().boss;
}
inline be<f32> &bstBaseSpread() { return *gabi::at<be<f32>>(0x1000B900); }

inline be<u32> &bstBasePointerWord(be<u32> &p) { return p; }

// HD: private bindings retain the verified argument and return widths.
u32 BstBase_messageSet(void *, u32, void *);
void *BstBase_alloc(s32);
void BstBase_registerGlobalObject(void *);
void BstBase_angleCtor(void *);
void BstBase_vectorCtor(void *);
s8 BstBase_reverb(s8);
void BstBase_mDoAud_seStart(u32, void *, u32, s8);
void BstBase_free(void *);
void BstBase_destroyArray(void *, s32, s32, u32, s32, s32);
void BstBase_cylinderDtor(void *, s32);
void BstBase_statusDtor(void *, s32);
void BstBase_acchDtor(void *, s32);
void BstBase_circleDtor(void *, s32);
void BstBase_actorDtor(void *, s32);
void *BstBase_getResource(void *, SafeString *, s32);
void BstBase_setAnimation(void *, void *, u8, f32, f32, f32, f32, void *);
void *BstBase_getJoint(void *);
void BstBase_PSMTXCopy(void *, void *);
void BstBase_mDoMtx_YrotM(void *, s16);
void BstBase_cXyz_subtract(void *, void *, void *);
s16 BstBase_cM_atan2s(f32, f32);
f32 BstBase_cM_sqrt(f32);
void BstBase_cLib_addCalcAngleS2(void *, s16, s16, s16);
void BstBase_cLib_addCalc(void *, f32, f32, f32);
void BstBase_mDoMtx_YrotS(void *, s16);
void BstBase_mDoMtx_XrotM(void *, s16);
void BstBase_MtxPosition(void *, void *);
void BstBase_PSVECAdd(void *, void *, void *);
void *BstBase_dComIfGp_get();
void BstBase_dComIfG_resDelete(void *, const char *);
void BstBase_hioDelete(s8);
void *BstBase_searchActor(u32, void *);
void BstBase_deleteActor(void *);
void BstBase_stopSound(void *);
void BstBase_setAngles(void *, s16, s16, s16);
u32 BstBase_dPa_control_set(void *, s32, u32, void *, void *, void *, s32, void *, s32, s32, void *,
                            void *);
void BstBase_actorCtor(void *);
void BstBase_constructArray(void *, s32, s32, u32);
void BstBase_acchCircleCtor(void *);
void BstBase_acchCtor(void *);
void BstBase_statusCtor(void *);
void BstBase_globalStatusCtor(void *);
void BstBase_cylinderCtor(void *);
void BstBase_cylinderShapeCtor(void *);
void BstBase_smokeCtor(void *, s32);
void BstBase_emitterMatrix(void *, void *, void *);
void BstBase_matrixTranslate(void *, f32, f32, f32);
void BstBase_mDoMtx_stack_scaleM(f32, f32, f32);
f32 BstBase_dBgS_GroundCross(void *, void *);
void BstBase_lineSet(void *, void *, void *, void *);
BOOL BstBase_dBgS_LineCross(void *, void *);
void BstBase_lineDtor(void *, s32);
void BstBase_shock(void *, s32, s32, void *);
void BstBase_sphereStart(void *, void *);
void BstBase_sphereMove(void *, void *);
void BstBase_collisionSet(void *, void *);
void BstBase_groundDtor(void *, s32);
void *BstBase_dKy_getEnvlight();
void BstBase_tevSetup(void *, s32, void *, void *);
void BstBase_tevApply(void *, void *, void *);
void BstBase_btkEntry(void *, void *, f32);
void BstBase_brkEntry(void *, void *, f32);
void BstBase_modelEntry(void *);
void BstBase_updateModel(void *, s32);
void BstBase_blurOn();
void BstBase_dSnap_RegistFig(s32, void *, f32, f32, f32);
u32 BstBase_morphCtor(void *, void *, void *, void *, void *, s32, s32, s32, s32, void *, s32, u32,
                      f32);
void *BstBase_getJoints(void *);
u32 BstBase_createModel(void *, s32, u32);
BOOL BstBase_initBtk(void *, void *, void *, s32, s32, s32, s32, s32, f32, s32);
BOOL BstBase_initBrk(void *, void *, void *, s32, s32, s32, s32, s32, f32, s32);
void *BstBase_btkCtor(void *);
void *BstBase_brkCtor(void *);
s16 BstBase_playerAngle(void *, void *);
void BstBase_hitFlash(void *, s32);
void BstBase_statusMove(void *);
BOOL BstBase_checkTargetHit(void *);
u32 BstBase_getTargetHit(void *);
void BstBase_hitSound(void *, void *, s32);
u32 BstBase_attackCheck(void *, void *);
s32 BstBase_dComIfG_resLoad(void *, const char *);
BOOL BstBase_entryHeap(void *, u32, u32);
u8 BstBase_hioCreate(const char *, void *);
void BstBase_acchSet(void *, void *, void *, void *, s32, void *, void *, void *, void *);
void BstBase_acchCircleSet(void *, f32, f32);
void BstBase_statusInit(void *, s32, s32, void *);
void BstBase_cylinderSet(void *, void *);
void BstBase_sphereSet(void *, void *);
u32 BstBase_createActor(s32, s32, void *, s8, void *, void *, s32, void *);
BOOL BstBase_dungeonItem(void *, s32);
f32 BstBase_cM_rndF(f32);

// HD: raw storage lifetime; construction/destruction is explicit in beam_move.
struct BstBaseGround : dBgS_GndChk {};
struct BstBaseLine : dBgS_LinChk {};
WWHD_SIZE(BstBaseGround, 0x54);
WWHD_SIZE(BstBaseLine, 0x6C);

inline Mtx34 &bstBaseModelMatrix() { return *gabi::at<Mtx34>(0x1048D0CC); }
inline Mtx34 &bstBaseJ3DMatrix() { return *gabi::at<Mtx34>(0x104B4868); }
struct BstBaseZeroHeader {
    be<u32> mWords[3];
};
inline BstBaseZeroHeader *bstBaseZeroHeaders() { return gabi::at<BstBaseZeroHeader>(0x10192AAC); }
struct BstBaseSave {
    u8 _00[0x798];
    dSv_memBit_c mStage;
};
WWHD_OFFSET(BstBaseSave, mStage, 0x798);
inline dSv_memBit_c *bstBaseStageBits(u32 a) { return &gabi::at<BstBaseSave>(a)->mStage; }
inline dCcD_SrcSph &bstBaseHandSphere() { return *gabi::at<dCcD_SrcSph>(0x10192964); }
inline dCcD_SrcSph &bstBaseEyeSphere() { return *gabi::at<dCcD_SrcSph>(0x101929A4); }
inline dCcD_SrcSph &bstBaseBeamSphere() { return *gabi::at<dCcD_SrcSph>(0x101929E4); }
inline dCcD_SrcCyl &bstBaseHeadCylinder() { return *gabi::at<dCcD_SrcCyl>(0x10192A24); }
inline dCcD_SrcCyl &bstBaseHandCylinder() { return *gabi::at<dCcD_SrcCyl>(0x10192A68); }

namespace BstBaseVtable {
inline constexpr u32 Hio = 0x1000B80C;
inline constexpr u32 AcchGroup = 0x1000B7AC;
inline constexpr u32 AcchPoly = 0x1000B7BC;
inline constexpr u32 Actor = 0x1000B81C;
inline constexpr u32 Acch = 0x1000B79C;
inline constexpr u32 Status = 0x1004AE88;
inline constexpr u32 GlobalStatus = 0x1004AEC0;
inline constexpr u32 CylinderShape = 0x100015A8;
inline constexpr u32 ShapeAttribute = 0x1000B72C;
inline constexpr u32 Cylinder = 0x1004B108;
inline constexpr u32 CylinderAttribute = 0x1004B160;
inline constexpr u32 CylinderGeometry = 0x1004B150;
inline constexpr u32 LinePoly = 0x1000B7FC;
inline constexpr u32 Group = 0x1000B74C;
inline constexpr u32 LineInfo = 0x1000B73C;
inline constexpr u32 GroundPoly = 0x1000B78C;
inline constexpr u32 GroundInfo = 0x1000B76C;
} // namespace BstBaseVtable

namespace BstBaseFunction {
inline constexpr u32 SphereDestructor = 0x02515AE8;
inline constexpr u32 SphereConstructor = 0x025166F0;
inline constexpr u32 SearchByID = 0x025E1234;
} // namespace BstBaseFunction

namespace bst_base_names {
enum Process { fpcNm_ATT_e = 0x1C0 };
enum Sound {
    JA_SE_CM_BST_EYE_HIT = 0x6985,
    JA_SE_CM_BST_BEAM_BOMB = 0x6986,
    JA_SE_CM_BST_JUMP_L = 0x698E
};
} // namespace bst_base_names
