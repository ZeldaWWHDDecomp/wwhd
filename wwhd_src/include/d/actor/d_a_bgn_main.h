// Puppet Ganon's phase-one implementation views. HD reflection helpers have no GC counterpart.
// Kept private because d_a_bgn.h is shared by the remaining phase-one sources.
#pragma once
#include "d/actor/d_a_bgn.h"
#include "gc_api/d/d_com_inf_game.h"

struct BgnReflectionColor {
    be<f32> mColor[4];
    void set(const void* color, f32 intensity) { gabi::call(0x0274D458, this, color, intensity); }
};
WWHD_SIZE(BgnReflectionColor, 0x10);
struct BgnReflectionMaterialView {
    u8 _000[0x1C4];
    BgnReflectionColor mColor0;
    BgnReflectionColor mColor1;
};
WWHD_OFFSET(BgnReflectionMaterialView, mColor0, 0x1C4);
WWHD_OFFSET(BgnReflectionMaterialView, mColor1, 0x1D4);
WWHD_SIZE(BgnReflectionMaterialView, 0x1E4);

struct BgnReflectionColorSource {
    u8 _000[0x28];
    be<f32> mIntensity0;
    u8 _02C[0x90 - 0x2C];
    be<s16> mColor0[4];
    u8 _098[0x160 - 0x98];
    be<s16> mColor1[4];
    u8 _168[4];
    be<f32> mIntensity1;
};
WWHD_OFFSET(BgnReflectionColorSource, mColor0, 0x90);
WWHD_OFFSET(BgnReflectionColorSource, mColor1, 0x160);
WWHD_SIZE(BgnReflectionColorSource, 0x170);

struct BgnReflectionHelper {
    u8 _000[4];
    gptr<BgnReflectionMaterialView> mpMaterial;
    u8 _008[4];
    BgnReflectionHelper* construct() { return gabi::call<BgnReflectionHelper*>(0x027FD6F4, this); }
    void setup(int count, u32 heap) { gabi::call(0x027FD838, this, count, heap); }
    void destroy(int flags) { gabi::call(0x027FD764, this, flags); }
    void setModel(J3DModel* model, int count, Mtx34* matrix) {
        gabi::call(0x027F53CC, model, this, count, matrix);
    }
    void setTexture(int index, Mtx34* matrix, const void* textureMatrix, const void* texture) {
        gabi::call(0x027FDA54, this, index, matrix, textureMatrix, texture);
    }
    void update(int index) { gabi::call(0x027FDFF4, this, index); }
    BgnReflectionColor* color0() { return &mpMaterial->mColor0; }
    BgnReflectionColor* color1() { return &mpMaterial->mColor1; }
};
WWHD_OFFSET(BgnReflectionHelper, mpMaterial, 4);
WWHD_SIZE(BgnReflectionHelper, 0xC);

struct BgnReflectionState {
    u8 _000;
    be<u8> mBlendEnable;
    u8 _002[2];
    be<u32> mBlendMode;
    be<u32> mDepthMode;
    be<u32> mCullMode;
    u8 _010[0xE0 - 0x10];
    be<u8> mPolygonOffsetEnable;
    u8 _0E1[3];
    be<u32> mPolygonOffsetMode;
    be<f32> mPolygonOffset;
    be<u32> mFlags;
    u8 _0F0[0x11C - 0xF0];
    void construct() { gabi::call(0x02750250, this); }
    void apply() { gabi::call(0x02750370, this); }
};
WWHD_OFFSET(BgnReflectionState, mCullMode, 0xC);
WWHD_OFFSET(BgnReflectionState, mPolygonOffsetEnable, 0xE0);
WWHD_OFFSET(BgnReflectionState, mFlags, 0xEC);
WWHD_SIZE(BgnReflectionState, 0x11C);

struct BgnCameraView {
    u8 _000[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(BgnCameraView, mEye, 0xDC);
WWHD_SIZE(BgnCameraView, 0xE8);
struct BgnPlayView {
    u8 _000[0x5AF8];
    gptr<BgnCameraView> mpCamera;
};
WWHD_OFFSET(BgnPlayView, mpCamera, 0x5AF8);
WWHD_SIZE(BgnPlayView, 0x5AFC);

struct BgnKeeseView : fopAc_ac_c {
    u8 _3BC[0x3D4 - sizeof(fopAc_ac_c)];
    mDoExt_J3DModelPacketS_l m2B8;
};
WWHD_OFFSET(BgnKeeseView, m2B8, 0x3D4);
WWHD_SIZE(BgnKeeseView, 0x484);
struct BgnMorthView : fopAc_ac_c {
    u8 _3BC[0x3D0 - sizeof(fopAc_ac_c)];
    gptr<mDoExt_McaMorf> mpMorf0;
    gptr<mDoExt_McaMorf> mpMorf1;
    u8 _3D8[0x3E4 - 0x3D8];
    mDoExt_J3DModelPacketS_l mReflection0;
    mDoExt_J3DModelPacketS_l mReflection1;
};
WWHD_OFFSET(BgnMorthView, mpMorf0, 0x3D0);
WWHD_OFFSET(BgnMorthView, mReflection0, 0x3E4);
WWHD_OFFSET(BgnMorthView, mReflection1, 0x494);
WWHD_SIZE(BgnMorthView, 0x544);
struct BgnHangingRopeView : fopAc_ac_c {
    u8 _3BC[0x169C - sizeof(fopAc_ac_c)];
    mDoExt_3DlineMat1_bgn mLineMat;
    u8 _1824[8];
    mDoExt_J3DModelPacketS_l mReflection;
};
WWHD_OFFSET(BgnHangingRopeView, mLineMat, 0x169C);
WWHD_OFFSET(BgnHangingRopeView, mReflection, 0x182C);
WWHD_SIZE(BgnHangingRopeView, 0x18DC);

inline void* bgnSmoke_construct(void* smoke, int count) {
    return gabi::call<void*>(0x025A5B18, smoke, count);
}
inline BgnReflectionHelper* bgnReflection_allocate(u32 heap) {
    return gabi::call<BgnReflectionHelper*>(0x0273B050, 0xC, heap, 4);
}
inline void bgnPacket_construct(void* packet) { gabi::call(0x027F1278, packet); }
inline void bgnPacket_destroy(void* packet, int flags) { gabi::call(0x027F13DC, packet, flags); }
inline void bgnPacket_setName(u32 name, const char* text) {
    gabi::call(0x02759C28, name, gabi::guest_addr(text));
}

// Integer views preserve the original word copies, including signaling NaN payloads.
struct BgnVectorWords {
    be<u32> x, y, z;
};
WWHD_SIZE(BgnVectorWords, 12);
inline BgnVectorWords* bgnWords(cXyz* value) { return gabi::at<BgnVectorWords>(gabi::ea(value)); }
struct BgnRopeLine {
    gptr<cXyz> mpPositions;
    gptr<be<u8>> mpSizes;
    u8 _008[8];
};
WWHD_SIZE(BgnRopeLine, 0x10);

struct BgnEmitterColorView {
    u8 _000[0x244];
    be<u8> mPrmR, mPrmG, mPrmB;
    u8 _247;
    be<u8> mEnvR, mEnvG, mEnvB;
};
WWHD_OFFSET(BgnEmitterColorView, mPrmR, 0x244);
WWHD_OFFSET(BgnEmitterColorView, mEnvR, 0x248);
WWHD_SIZE(BgnEmitterColorView, 0x24B);
struct BgnTevColorBytes {
    u8 _000[0x90];
    be<u8> mPrmRHigh, mPrmR;
    be<u8> mPrmGHigh, mPrmG;
    be<u8> mPrmBHigh, mPrmB;
    u8 _096[2];
    be<u8> mEnvR, mEnvG, mEnvB;
};
WWHD_OFFSET(BgnTevColorBytes, mPrmR, 0x91);
WWHD_OFFSET(BgnTevColorBytes, mEnvR, 0x98);
WWHD_SIZE(BgnTevColorBytes, 0x9B);

inline BOOL bgnBomb_getExplodeInstant(fopAc_ac_c* bomb) {
    return gabi::call<BOOL>(0x020CB98C, bomb);
}
inline void bgnActor_destroy(bgn_class* actor, int flags) { gabi::call(0x025D50BC, actor, flags); }

struct BgnPacketVtable {
    u8 _000[0x2C];
    be<u32> mDraw;
};
WWHD_SIZE(BgnPacketVtable, 0x30);
struct BgnMaterialPacket {
    u8 _000[0xC];
    gptr<BgnPacketVtable> mpVtable;
    u8 _010[0xAC - 0x10];
    void draw(u32 info) { gabi::call_ptr(mpVtable->mDraw, this, info); }
};
WWHD_SIZE(BgnMaterialPacket, 0xAC);
struct BgnLinePacket {
    u8 _000[0x130];
    gptr<BgnPacketVtable> mpVtable;
    void draw(u32 info) { gabi::call_ptr(mpVtable->mDraw, this, info); }
};
WWHD_SIZE(BgnLinePacket, 0x134);
struct BgnShapeData {
    u8 _000[0xC];
    be<u16> mIndex;
};
WWHD_SIZE(BgnShapeData, 0xE);
struct BgnShape {
    gptr<BgnShapeData> mpData;
    be<u8> mVisible;
    u8 _005[3];
};
WWHD_SIZE(BgnShape, 8);
struct BgnMeshName {
    u8 _000[4];
    be<s32> mNameOffset;
};
WWHD_SIZE(BgnMeshName, 8);
struct BgnMesh {
    gptr<BgnMeshName> mpHeader;
    gptr<BgnMesh> mpNext;
    gptr<BgnShape> mpShape;
};
WWHD_SIZE(BgnMesh, 0xC);
struct BgnJoint {
    u8 _000[0x10];
    gptr<BgnMesh> mpMesh;
    u8 _014[8];
};
WWHD_SIZE(BgnJoint, 0x1C);
struct BgnModelDataView {
    u8 _000[4];
    be<u32> mJointCount;
    gptr<BgnJoint> mpJoints;
};
WWHD_SIZE(BgnModelDataView, 0xC);
struct BgnModelView {
    u8 _000[0xAC];
    gptr<BgnModelDataView> mpData;
    u8 _0B0[0x128 - 0xB0];
    be<u32> mMaterialPacketCount;
    gptr<BgnMaterialPacket> mpMaterialPackets;
    be<u32> mShapeIndexCount;
    gptr<be<u16>> mpShapeIndices;
};
WWHD_OFFSET(BgnModelView, mpMaterialPackets, 0x12C);
WWHD_OFFSET(BgnModelView, mpShapeIndices, 0x134);
WWHD_SIZE(BgnModelView, 0x138);
struct BgnJointTree {
    u8 _000[8];
    be<u16> mJointCount;
};
WWHD_SIZE(BgnJointTree, 0xA);
struct BgnDrawContext {
    be<u32> mTarget;
    u8 _004[8];
    be<u32> mType;
};
WWHD_SIZE(BgnDrawContext, 0x10);
struct BgnSaveView {
    u8 _000[0x1C0];
    be<u8> mHideReflectionEars;
};
WWHD_SIZE(BgnSaveView, 0x1C1);
struct BgnSafeStringVtable {
    u8 _000[0x14];
    be<u32> mAssureTermination;
};
WWHD_SIZE(BgnSafeStringVtable, 0x18);
inline void bgnSafeString_assureTermination(SafeString* text) {
    gabi::call_ptr(gabi::at<BgnSafeStringVtable>(text->__vtbl)->mAssureTermination, text);
}
inline BgnJointTree* bgnModelData_getJointTree(BgnModelDataView* data) {
    return gabi::call<BgnJointTree*>(0x027F3F94, data);
}

namespace bgnTu {
constexpr u32 packet_vtbl = 0x10008DA8;
constexpr u32 safestring_vtbl = BGN_SAFESTRING_VTBL;
constexpr u32 actor_vtbl = 0x10008AD4;
constexpr u32 hio_vtbl = 0x10008AE4;
constexpr u32 line_vtbl = 0x10008A94;
constexpr u32 group_pass_vtbl = 0x10008AB4;
constexpr u32 poly_pass_vtbl = 0x10008AC4;
constexpr u32 actor_line_vtbl = 0x10008AA4;
constexpr u32 base_group_pass_vtbl = 0x10008A44;
constexpr u32 base_actor_line_vtbl = 0x10008A34;
constexpr u32 header_registration = 0x10190FE8;
constexpr u32 flag_a_registration = 0x10190FF4;
constexpr u32 flag_b_registration = 0x10191000;
} // namespace bgnTu
inline void bgnHeader_constructFlagA(void* flag) { gabi::call(0x028ED6F8, flag); }
inline void bgnHeader_constructFlagB(void* flag) { gabi::call(0x028EAB2C, flag); }
struct BgnMaterialPE {
    void setBlendType(int type) { gabi::call(0x027E212C, this, type); }
    void setBlendSource(int source) { gabi::call(0x027E18F8, gabi::ea(this) + 8, source); }
    void setBlendDestination(int destination) {
        gabi::call(0x027E1908, gabi::ea(this) + 8, destination);
    }
    void setDepthUpdate(int enabled) { gabi::call(0x027E195C, gabi::ea(this) + 0x14, enabled); }
};
struct BgnPhaseTwoView {
    u8 _000[0x316C];
    be<f32> m2E7C;
};
WWHD_OFFSET(BgnPhaseTwoView, m2E7C, 0x316C);
WWHD_SIZE(BgnPhaseTwoView, 0x3170);

enum {
    fpcNm_Obj_Vteng_e = 0x93,
    fpcNm_KS_e = 0xCD,
    fpcNm_KI_e = 0xD7,
    fpcNm_BGN2_e = 0xF4,
    fpcNm_BGN3_e = 0xF5,
    fpcNm_BOMB_e = 0x126,
    fpcNm_HIMO3_e = 0x1BF,
    dRes_INDEX_BGN_BCK_BGN_HEAD1_e = 0x6,
    dRes_INDEX_BGN_BDL_BGN_BODY1_e = 0x13,
    dRes_INDEX_BGN_BDL_BGN_HEAD1_e = 0x15,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENA_e = 0x16,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENB_e = 0x19,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENC_e = 0x1C,
    dRes_INDEX_BGN_BDL_BGN_MAIN1_e = 0x20,
    dRes_INDEX_BGN_BDL_R00_e = 0x21,
    dRes_INDEX_BGN_BDL_R0E_A_e = 0x22,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENB_e = 0x25,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENC_e = 0x28,
    dRes_INDEX_BGN_BTI_HIMO_e = 0x2D,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 0x2E,
};
struct BgnHeaderStatics {
    be<u32> mWords[4];
};
WWHD_SIZE(BgnHeaderStatics, 0x10);
struct BgnAngleLimits {
    be<f32> mMin, mMax;
};
WWHD_SIZE(BgnAngleLimits, 8);
struct BgnPoseStorage {
    csXyz mAngles[12][8];
};
WWHD_SIZE(BgnPoseStorage, 0x240);

namespace dPa_name {
enum : u16 {
    ID_AK_JN_CCTHUNDER00 = 0x3ED,
    ID_AK_JN_CCTHUNDER01 = 0x3EE,
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUFLASH = 0x16,
    ID_AK_SN_KGTT1PUNCHHAHEN00 = 0x8417,
    ID_AK_SN_KGTT1PUNCHHAMON00 = 0x8416,
    ID_AK_SN_KGTT1PUNCHSPLASH00 = 0x8414,
    ID_AK_SN_KGTT1PUNCHSPLASH01 = 0x8415,
    ID_AK_SN_KGTT2CHESTPOTA00 = 0x8419,
    ID_AK_SN_O_KGTCOMMONHAMON00 = 0x8407,
    ID_AK_SN_O_KGTCOMMONHAMON01 = 0x8408,
    ID_AK_SN_O_KGTCOMMONSPLASH01 = 0x840E,
    ID_AK_ST_KGTT1PUNCHSMOKE00 = 0xA418,
};
}
enum : u32 {
    JA_BGM_BGN_TARABA_IN = 0x8000005C,
    JA_SE_CM_BGN_D_FALL = 0x5975,
    JA_SE_CM_BGN_D_FALL_WATER = 0x5974,
    JA_SE_CM_BGN_D_HIT_PUNCH = 0x6A40,
    JA_SE_CM_BGN_D_HIT_PUNCH_W = 0x6A41,
    JA_SE_CM_BGN_D_ROPE_RESET = 0x5972,
    JA_SE_CM_BGN_D_TO_T_1 = 0x5976,
    JA_SE_CM_BGN_D_TO_T_2 = 0x5977,
    JA_SE_CM_BGN_MECHA_ROPE = 0x705B,
    JA_SE_CM_BGN_MECHA_ROTATE = 0x705A,
    JA_SE_CM_BGN_METAM_EXPLODE = 0x5980,
    JA_SE_CM_BGN_STRING_RIPPLE = 0x6A42,
};

using mDoExt_J3DModelPacketS = mDoExt_J3DModelPacketS_l;
using BgnPartTevStr = dKy_tevstr_l;

// HD: typed view of the optional sources and flags in the shared reflection packet.
struct BgnReflectionPacket {
    u8 _000[0xC];
    be<u32> __vtbl;
    u8 _010[0x90 - 0x10];
    be<u32> mpName;
    u8 _094[4];
    gptr<J3DModel> mpModel;
    gptr<BgnMaterialPacket> mpMaterialPacket;
    gptr<BgnLinePacket> mpLineMaterial;
    gptr<BgnReflectionColorSource> mpColorSource;
    gptr<BgnReflectionHelper> mpHelper;
    be<u8> mEnabled;
    be<u8> mDrawFaceMeshes;
    be<u8> mClearMaterialFlags;
    be<u8> mDisableBlend;
};
WWHD_OFFSET(BgnReflectionPacket, mpModel, 0x98);
WWHD_OFFSET(BgnReflectionPacket, mpMaterialPacket, 0x9C);
WWHD_OFFSET(BgnReflectionPacket, mpLineMaterial, 0xA0);
WWHD_OFFSET(BgnReflectionPacket, mpColorSource, 0xA4);
WWHD_OFFSET(BgnReflectionPacket, mpHelper, 0xA8);
WWHD_SIZE(BgnReflectionPacket, 0xB0);
