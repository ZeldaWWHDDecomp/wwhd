/* Water pillar: kankyo_class grows by four bytes in HD; the HD BCK wrapper is
 * 0x8C, BTK is 0x74, and the light state grows to 0x1C8. Private views expose only
 * the fields this unit uses, without changing shared model/player layouts. */
#pragma once
#include "f_op/f_op_kankyo.h"
#include "m_Do/m_Do_ext.h"

struct dWpillar_lightParamHD {
    /* 0x00 */ cXyz mPosition;
    /* 0x0C */ cXyz mDirection;
    /* 0x18 */ GXColor mColor;
    /* 0x1C */ be<u16> mFlags[4];
    /* 0x24 */ be<f32> mParameters[8];
};
WWHD_SIZE(dWpillar_lightParamHD, 0x44);
struct dWpillar_tevstrHD {
    /* 0x000 */ dWpillar_lightParamHD mLight;
    /* 0x044 */ u8 _44[0x40];
    /* 0x084 */ cXyz mLightPosWorld;
    /* 0x090 */ GXColorS10 mColorC0;
    /* 0x098 */ GXColor mColorK0;
    /* 0x09C */ u8 _9C[0x24];
    /* 0x0C0 */ dWpillar_lightParamHD mLightCopy;
    /* 0x104 */ u8 _104[0x40];
    /* 0x144 */ dWpillar_lightParamHD mLightCopy2;
    /* 0x188 */ u8 _188[0x40];
};
WWHD_OFFSET(dWpillar_tevstrHD, mColorC0, 0x90);
WWHD_OFFSET(dWpillar_tevstrHD, mColorK0, 0x98);
WWHD_OFFSET(dWpillar_tevstrHD, mLightCopy, 0xC0);
WWHD_OFFSET(dWpillar_tevstrHD, mLightCopy2, 0x144);
WWHD_SIZE(dWpillar_tevstrHD, 0x1C8);
struct dWpillar_modelHD;
class dWpillar_c : public kankyo_class {
  public:
    s32 jointCallBack(s32 i_jointNo);
    s32 draw();
    s32 execute();
    s32 wp_delete();
    cPhs_State create();
    /* 0x0FC */ gptr<dWpillar_modelHD> mpModel;
    /* 0x100 */ mDoExt_bckAnm mBck;
    /* 0x18C */ mDoExt_btkAnm mBtk;
    /* 0x200 */ gptr<void> mpHeap;
    /* 0x204 */ dWpillar_tevstrHD mTevstr;
    /* 0x3CC */ be<f32> mHeight;
};
WWHD_OFFSET(dWpillar_c, mpModel, 0xFC);
WWHD_OFFSET(dWpillar_c, mBck, 0x100);
WWHD_OFFSET(dWpillar_c, mBtk, 0x18C);
WWHD_OFFSET(dWpillar_c, mpHeap, 0x200);
WWHD_OFFSET(dWpillar_c, mTevstr, 0x204);
WWHD_OFFSET(dWpillar_c, mHeight, 0x3CC);
WWHD_SIZE(dWpillar_c, 0x3D0);

struct dWpillar_bckStorageHD {
    /* 0x000 */ u8 mFrameCtrl[16];
    /* 0x010 */ be<u32> mResourceVtable;
    /* 0x014 */ u8 mShaderPair[52];
    /* 0x048 */ be<u32> mColorVtable;
    /* 0x04C */ u8 _4C[0xC];
    /* 0x058 */ be<u32> mResourceFlags;
    /* 0x05C */ u8 _5C[0x20];
    /* 0x07C */ be<u32> mResourceA;
    /* 0x080 */ be<u32> mResourceB;
    /* 0x084 */ be<u32> mResourceC;
    /* 0x088 */ be<u32> mResourceD;
};
WWHD_OFFSET(dWpillar_bckStorageHD, mFrameCtrl, 0x0);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceVtable, 0x10);
WWHD_OFFSET(dWpillar_bckStorageHD, mShaderPair, 0x14);
WWHD_OFFSET(dWpillar_bckStorageHD, mColorVtable, 0x48);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceFlags, 0x58);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceA, 0x7C);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceB, 0x80);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceC, 0x84);
WWHD_OFFSET(dWpillar_bckStorageHD, mResourceD, 0x88);
WWHD_SIZE(dWpillar_bckStorageHD, 0x8C);

struct dWpillar_jointHD;
struct dWpillar_materialHD;

struct dWpillar_mtxBufferHD {
    /* 0x000 */ u8 _0[0x4];
    /* 0x004 */ be<u16> mFlags;
    /* 0x006 */ u8 _6[0xA];
    /* 0x010 */ gptr<Mtx34> mpAnmMtx;
};
WWHD_OFFSET(dWpillar_mtxBufferHD, mFlags, 0x4);
WWHD_OFFSET(dWpillar_mtxBufferHD, mpAnmMtx, 0x10);
WWHD_SIZE(dWpillar_mtxBufferHD, 0x14);

struct dWpillar_modelDataHD {
    /* 0x000 */ u8 _0[0x4];
    /* 0x004 */ be<u32> mJointCount;
    /* 0x008 */ gptr<dWpillar_jointHD> mpJoint;
    /* 0x00C */ be<u32> mMaterialCount;
    /* 0x010 */ gptr<dWpillar_materialHD> mpMaterial;
};
WWHD_OFFSET(dWpillar_modelDataHD, mJointCount, 0x4);
WWHD_OFFSET(dWpillar_modelDataHD, mpJoint, 0x8);
WWHD_OFFSET(dWpillar_modelDataHD, mMaterialCount, 0xC);
WWHD_OFFSET(dWpillar_modelDataHD, mpMaterial, 0x10);
WWHD_SIZE(dWpillar_modelDataHD, 0x14);

struct dWpillar_modelHD {
    /* 0x000 */ u8 _0[0x2A];
    /* 0x02A */ be<u16> mMaterialNum;
    /* 0x02C */ gptr<dWpillar_mtxBufferHD> mpMtxBuffer;
    /* 0x030 */ u8 _30[0x4];
    /* 0x034 */ gptr<void> mpMatColor;
    /* 0x038 */ u8 _38[0x74];
    /* 0x0AC */ gptr<dWpillar_modelDataHD> mModelData;
    /* 0x0B0 */ u8 _B0[0x8];
    /* 0x0B8 */ gptr<dWpillar_c> mUserArea;
    /* 0x0BC */ cXyz mBaseScale;
    /* 0x0C8 */ Mtx34 mBaseTransformMtx;
};
WWHD_OFFSET(dWpillar_modelHD, mMaterialNum, 0x2A);
WWHD_OFFSET(dWpillar_modelHD, mpMtxBuffer, 0x2C);
WWHD_OFFSET(dWpillar_modelHD, mpMatColor, 0x34);
WWHD_OFFSET(dWpillar_modelHD, mModelData, 0xAC);
WWHD_OFFSET(dWpillar_modelHD, mUserArea, 0xB8);
WWHD_OFFSET(dWpillar_modelHD, mBaseScale, 0xBC);
WWHD_OFFSET(dWpillar_modelHD, mBaseTransformMtx, 0xC8);
WWHD_SIZE(dWpillar_modelHD, 0xF8);

struct dWpillar_jointHD {
    /* 0x000 */ u8 _0[0x4];
    /* 0x004 */ be<u16> mJntNo;
    /* 0x006 */ u8 _6[0x2];
    /* 0x008 */ be<u32> mCallBack;
    /* 0x00C */ u8 _C[0x10];
};
WWHD_OFFSET(dWpillar_jointHD, mJntNo, 0x4);
WWHD_OFFSET(dWpillar_jointHD, mCallBack, 0x8);
WWHD_SIZE(dWpillar_jointHD, 0x1C);

struct dWpillar_materialHD {
    /* 0x000 */ u8 _0[0x18];
    /* 0x018 */ gptr<void> mTevBlock;
    /* 0x01C */ u8 _1C[0x84];
    /* 0x0A0 */ be<u32> mHDColorFlags;
    /* 0x0A4 */ u8 _A4[0x2F8];
};
WWHD_OFFSET(dWpillar_materialHD, mTevBlock, 0x18);
WWHD_OFFSET(dWpillar_materialHD, mHDColorFlags, 0xA0);
WWHD_SIZE(dWpillar_materialHD, 0x39C);

struct dWpillar_materialTableHD {
    /* 0x000 */ u8 _0[0x24];
    /* 0x024 */ be<u16> mMaterialNum;
};
WWHD_OFFSET(dWpillar_materialTableHD, mMaterialNum, 0x24);
WWHD_SIZE(dWpillar_materialTableHD, 0x26);

struct dWpillar_jointTableHD {
    /* 0x000 */ u8 _0[0x8];
    /* 0x008 */ be<u16> mJointNum;
};
WWHD_OFFSET(dWpillar_jointTableHD, mJointNum, 0x8);
WWHD_SIZE(dWpillar_jointTableHD, 0xA);

struct dWpillar_animHD {
    /* 0x000 */ be<f32> mFrame;
};
WWHD_OFFSET(dWpillar_animHD, mFrame, 0x0);
WWHD_SIZE(dWpillar_animHD, 0x4);

struct dWpillar_frameBlendHD {
    /* 0x000 */ be<f32> mFrame;
    /* 0x004 */ be<f32> mStart;
    /* 0x008 */ be<f32> mEnd;
    /* 0x00C */ u8 _C[0x4];
    /* 0x010 */ be<u32> mEvaluator;
    /* 0x014 */ gptr<void> mpEvaluatorData;
};
WWHD_OFFSET(dWpillar_frameBlendHD, mFrame, 0x0);
WWHD_OFFSET(dWpillar_frameBlendHD, mStart, 0x4);
WWHD_OFFSET(dWpillar_frameBlendHD, mEnd, 0x8);
WWHD_OFFSET(dWpillar_frameBlendHD, mEvaluator, 0x10);
WWHD_OFFSET(dWpillar_frameBlendHD, mpEvaluatorData, 0x14);
WWHD_SIZE(dWpillar_frameBlendHD, 0x18);

struct dWpillar_playerHD {
    /* 0x000 */ u8 _0[0x4550];
    /* 0x4550 */ be<f32> mBombWaterPillarBrkFrameHD;
    /* 0x4554 */ u8 _4554[0x8];
    /* 0x455C */ gptr<dWpillar_animHD> mpGwp00BrkData;
    /* 0x4560 */ u8 _4560[0xC];
    /* 0x456C */ gptr<dWpillar_frameBlendHD> mpBombWaterPillarBrkBlendHD;
    /* 0x4570 */ u8 _4570[0x58];
    /* 0x45C8 */ be<f32> mBombWaterPillarBtkFrameHD;
    /* 0x45CC */ u8 _45CC[0x8];
    /* 0x45D4 */ gptr<dWpillar_frameBlendHD> mpBombWaterPillarBtkBlendHD;
    /* 0x45D8 */ u8 _45D8[0x54];
    /* 0x462C */ gptr<dWpillar_animHD> mpGwp00BtkData;
};
WWHD_OFFSET(dWpillar_playerHD, mBombWaterPillarBrkFrameHD, 0x4550);
WWHD_OFFSET(dWpillar_playerHD, mpGwp00BrkData, 0x455C);
WWHD_OFFSET(dWpillar_playerHD, mpBombWaterPillarBrkBlendHD, 0x456C);
WWHD_OFFSET(dWpillar_playerHD, mBombWaterPillarBtkFrameHD, 0x45C8);
WWHD_OFFSET(dWpillar_playerHD, mpBombWaterPillarBtkBlendHD, 0x45D4);
WWHD_OFFSET(dWpillar_playerHD, mpGwp00BtkData, 0x462C);
WWHD_SIZE(dWpillar_playerHD, 0x4630);

struct dWpillar_gameInfoHD {
    /* 0x000 */ u8 _0[0x12A0];
    /* 0x12A0 */ u8 mBgS;
    /* 0x12A1 */ u8 _12A1[0x480F];
    /* 0x5AB0 */ gptr<void> mpParticle;
    /* 0x5AB4 */ u8 _5AB4[0x80];
    /* 0x5B34 */ gptr<dWpillar_playerHD> mpPlayer;
};
WWHD_OFFSET(dWpillar_gameInfoHD, mBgS, 0x12A0);
WWHD_OFFSET(dWpillar_gameInfoHD, mpParticle, 0x5AB0);
WWHD_OFFSET(dWpillar_gameInfoHD, mpPlayer, 0x5B34);
WWHD_SIZE(dWpillar_gameInfoHD, 0x5B38);

struct dWpillar_envLightHD {
    /* 0x000 */ u8 _0[0x10B8];
    /* 0x10B8 */ be<f32> mTevColorScaleHD;
    /* 0x10BC */ be<f32> mTevKColorScaleHD;
};
WWHD_OFFSET(dWpillar_envLightHD, mTevColorScaleHD, 0x10B8);
WWHD_OFFSET(dWpillar_envLightHD, mTevKColorScaleHD, 0x10BC);
WWHD_SIZE(dWpillar_envLightHD, 0x10C0);

struct dWpillar_heapHD {
    /* 0x000 */ u8 _0[0x10];
    /* 0x010 */ gptr<char> mName;
};
WWHD_OFFSET(dWpillar_heapHD, mName, 0x10);
WWHD_SIZE(dWpillar_heapHD, 0x14);

struct dWpillar_Color8HD {
    union {
        be<s16> value[4];
        struct {
            be<u32> mColorWord;
            be<f32> mRedScratch;
        };
    };
};
WWHD_SIZE(dWpillar_Color8HD, 8);
struct dWpillar_floatColorHD {
    be<f32> value[4];
};
WWHD_SIZE(dWpillar_floatColorHD, 0x10);
struct dWpillar_stringHD {
    gptr<char> text;
    be<u32> vtable;
};
WWHD_SIZE(dWpillar_stringHD, 8);
struct dWpillar_groundHD {
    /* 0x00 */ gptr<void> mpObjectPass;
    /* 0x04 */ gptr<void> mpGroupPass;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 mPolyInfo[12];
    /* 0x20 */ be<u32> mGroundVtable;
    /* 0x24 */ be<u32> mPos[3];
    /* 0x30 */ u8 _30[0x10];
    /* 0x40 */ be<u32> mObjectPassVtable;
    /* 0x44 */ be<u8> mObjectPassEnabled;
    /* 0x45 */ be<u8> mObjectPassFlags[6];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> mGroupPassVtable;
    /* 0x50 */ be<u32> mGroupPassFlags;
};
WWHD_OFFSET(dWpillar_groundHD, mPos, 0x24);
WWHD_OFFSET(dWpillar_groundHD, mObjectPassVtable, 0x40);
WWHD_OFFSET(dWpillar_groundHD, mGroupPassVtable, 0x4C);
WWHD_SIZE(dWpillar_groundHD, 0x54);
struct dWpillar_runtimeStateHD {
    be<f32> lower;
    be<f32> upper;
    be<u8> flagA;
    be<u8> flagB;
    u8 _A[2];
    be<u32> zero[4];
};
WWHD_SIZE(dWpillar_runtimeStateHD, 0x1C);
struct dWpillar_runtimeObjectHD {
    be<u32> words[3];
};
WWHD_SIZE(dWpillar_runtimeObjectHD, 0xC);
struct dWpillar_runtimeConstantsHD {
    be<f32> lower;
    be<f32> upper;
};
WWHD_SIZE(dWpillar_runtimeConstantsHD, 8);
struct dWpillar_tevVtableHD {
    be<u32> slots[16];
};
WWHD_SIZE(dWpillar_tevVtableHD, 0x40);
struct dWpillar_tevBlockHD {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ gptr<dWpillar_tevVtableHD> mVtable;
};
WWHD_SIZE(dWpillar_tevBlockHD, 8);
enum dWpillar_tevMethodHD {
    WPILLAR_SET_TEV_COLOR_HD = 9,
    WPILLAR_GET_TEV_COLOR_HD = 13,
    WPILLAR_SET_TEV_KCOLOR_HD = 15,
};
enum {
    TEV_TYPE_BG1 = 2,
    dRes_INDEX_ALWAYS_BDL_W_PILLAR_A_e = 0x3B,
    dRes_INDEX_LINK_BDL_GWP00_e = 0x42,
    dRes_INDEX_ALWAYS_BCK_W_PILLAR_A_e = 0x16,
    dRes_INDEX_LINK_BCK_GWP00_e = 0xE,
    dRes_INDEX_ALWAYS_BTK_W_PILLAR_A_e = 0x5E,
    WPILLAR_MIZUSHIBUKI_HD = 0x3C,
    WPILLAR_HAMON01_HD = 0x3D,
    WPILLAR_SHIBUKI_HD = 0x40,
    WPILLAR_BOMB_SMOKE_HD = 0x200A,
    WPILLAR_WATER_SMOKE_HD = 0x2041,
};

// HD: typed call adapters for this unit's private layout views.
void dWpillar_jointAssertHD(const char*, u32, const char*);
void MTXCopy(const Mtx34*, Mtx34*);
void MTXTrans(Mtx34*, f32, f32, f32);
dWpillar_jointHD* dWpillar_nodeToJointHD(void*);
dWpillar_envLightHD* dWpillar_getEnvLightHD();
void dWpillar_settingTevStructHD(dWpillar_envLightHD*, u32, cXyz*, dWpillar_tevstrHD*);
void dWpillar_setLightTevColorTypeHD(dWpillar_envLightHD*, dWpillar_modelHD*, dWpillar_tevstrHD*);
void dWpillar_bckEntryHD(mDoExt_bckAnm*, dWpillar_modelDataHD*, f32);
void dWpillar_btkEntryHD(mDoExt_btkAnm*, dWpillar_modelDataHD*, f32);
dWpillar_materialTableHD* dWpillar_getMaterialTableHD(dWpillar_modelDataHD*);
void dWpillar_transformColorHD(dWpillar_floatColorHD*, dWpillar_floatColorHD*, f32);
dWpillar_floatColorHD* dWpillar_getFloatColorHD(be<u32>*, u32);
dWpillar_gameInfoHD* dWpillar_getGameInfoHD();
void dWpillar_updateFrameBindingHD(gptr<dWpillar_frameBlendHD>*);
void dWpillar_modelUpdateDLHD(dWpillar_modelHD*, u32);
void fopKyM_Delete(dWpillar_c*);
s32 fopAcM_getWaterY(cXyz*, be<f32>*);
void mDoExt_destroySolidHeap(void*);
void dWpillar_bckResourceDeleteHD(void*, u32);
void kankyo_class__dt(dWpillar_c*, u32);
dWpillar_c* dWpillar_allocateHD(u32);
void kankyo_class__ct(dWpillar_c*);
void J3DFrameCtrl__ct(J3DFrameCtrl*, u32);
void dWpillar_shaderPairInitHD(void*);
void mDoExt_btkAnm__ct(mDoExt_btkAnm*);
dWpillar_heapHD* mDoExt_createSolidHeapFromGameToCurrent(u32, u32);
void* dWpillar_getObjectResHD(void*, dWpillar_stringHD*, u32);
dWpillar_modelHD* dWpillar_modelCreateHD(dWpillar_modelDataHD*, u32, u32);
s32 dWpillar_bckInitHD(mDoExt_bckAnm*, dWpillar_modelDataHD*, void*, u32, u32, f32, u32, s32, u32);
s32 dWpillar_btkInitHD(mDoExt_btkAnm*, dWpillar_modelDataHD*, void*, u32, u32, f32, u32, s32, u32,
                       u32);
void dWpillar_getMatColorHD(dWpillar_Color8HD*, void*, u32);
void mDoExt_restoreCurrentHeap();
void mDoExt_adjustSolidHeap(dWpillar_heapHD*);
dWpillar_jointTableHD* dWpillar_getJointTableHD(dWpillar_modelDataHD*);
void dWpillar_particleSetHD(void*, u32, u32, cXyz*, void*, cXyz*, u32, void*, s32, u32, u32, u32);
void dWpillar_particleSetBombSmokeHD(void*, u32, cXyz*, void*, void*, u32);
void dWpillar_groundCheckCtorHD(dWpillar_groundHD*);
f32 dWpillar_groundCrossHD(void*, dWpillar_groundHD*);
s32 dWpillar_getRoomIdHD(void*, void*);
u32 dWpillar_getPolyColorHD(void*, void*);
void dWpillar_tevstrInitHD(dWpillar_tevstrHD*, s32, u32);
void dWpillar_groundCheckDtorHD(dWpillar_groundHD*, u32);
void dWpillar_runtimeObjectInitHD(dWpillar_runtimeObjectHD*);
void dWpillar_runtimeFlagAInitHD(be<u8>*);
void dWpillar_runtimeFlagBInitHD(be<u8>*);

dWpillar_c* dWpillar_c__ct(dWpillar_c*);
