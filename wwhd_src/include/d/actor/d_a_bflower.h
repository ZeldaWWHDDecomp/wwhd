#pragma once
#include "gc_api/gc_api.h"
// HD: actor base and inline animation objects enlarge the GameCube layout.
struct daBFlower_c : fopAc_ac_c {
    enum State_e { STATE_LIVE_E, STATE_DEAD_e };
    using ActionFunc = BOOL (daBFlower_c::*)();
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    dCcD_Sph mSph;
    mDoExt_bckAnm mBck1;
    mDoExt_brkAnm mBrk1;
    be<s16> m55C;
    u8 pad756[2];
    gptr<J3DModel> mpModel2;
    mDoExt_bckAnm mBck2;
    mDoExt_brkAnm mBrk2;
    be<u8> m58C, m58D, m58E, mState;
    be<s32> mSwitchNo, m594, mGrabbable;
    gptr<fopAc_ac_c> mpBombActor;
    be<s32> mAnimTimer;
    be<f32> mPrevPlayerDist;
    be<u32> mPrevGrabActorID;
    cXyz mBombScale;
    be<u8> m5B8;
    BOOL CreateHeap();
    void CreateInit();
    s32 init_bck_anm(s16);
    s32 _create();
    void set_mtx();
    bool _execute();
    BOOL actLive();
    BOOL actDead();
    void animPlay();
    void setCollision();
    bool _draw();
};
WWHD_OFFSET(daBFlower_c, mpModel, 0x3B4);
WWHD_OFFSET(daBFlower_c, m55C, 0x754);
WWHD_OFFSET(daBFlower_c, mState, 0x863);
WWHD_OFFSET(daBFlower_c, mBombScale, 0x880);

WWHD_OFFSET(daBFlower_c, mStts, 0x3B8);
WWHD_OFFSET(daBFlower_c, mCyl, 0x3F4);
WWHD_OFFSET(daBFlower_c, mSph, 0x524);
WWHD_OFFSET(daBFlower_c, mBck1, 0x650);
WWHD_OFFSET(daBFlower_c, mBrk1, 0x6DC);
WWHD_OFFSET(daBFlower_c, mBck2, 0x75C);
WWHD_OFFSET(daBFlower_c, mBrk2, 0x7E8);

// HD: Link's carry-controller subobject is beyond the shared actor prefix.
struct BFlowerCarryController {
    be<u32> unused;
};
struct BFlowerLinkView {
    u8 prefix[0x65A0];
    BFlowerCarryController mActorKeep;
};
struct BFlowerPlayerVtable {
    u8 prefix[0xBC];
    be<u32> getGrabActorID;
};
struct BFlowerPlayerView {
    u8 prefix[0xB4];
    gptr<BFlowerPlayerVtable> __vtbl;
};
struct BFlowerJointTree {
    u8 prefix[0x14];
    be<u32> mpMtxCalc;
};
struct BFlowerModelData {
    u8 prefix[8];
    gptr<BFlowerJointTree> mJointTree;
};
// HD: the inline BCK controller embeds its animation/calculator storage.
struct BFlowerBckStorage {
    J3DFrameCtrl mFrameCtrl;
    be<u32> __vtbl;
    u8 mTransform[0x34];
    be<u32> __vtblMtxCalc;
    u8 padding_4C[0xC];
    be<u32> mpAnmTransform;
    u8 padding_5C[0x20];
    be<u32> mpTransformInfo;
    be<u32> mpQuat;
    be<u32> mpCallback1;
    be<u32> mpCallback2;
};
WWHD_SIZE(BFlowerBckStorage, 0x8C);
WWHD_OFFSET(BFlowerBckStorage, mpTransformInfo, 0x7C);
struct BFlowerHeaderStatics {
    be<u32> randomSeed;
    be<u32> initialized;
    be<u32> counter;
    be<u32> unused;
};
struct BFlowerSaveView {
    u8 prefix[0x20];
    dSv_info_c mSwitchInfo;
};
using BFlowerSavePointer = gptr<BFlowerSaveView>;
using BFlowerResourcePointer = gptr<void>;
struct BFlowerActionTable {
    gabi::ptmf<daBFlower_c::ActionFunc> actions[3];
};

void *BFlower_getResource(void *, void *, s32);
void BFlower_panic(const char *, s32, const char *);
void BFlower_subtractPosition(cXyz *, cXyz *, cXyz *);
f32 BFlower_squareMagnitude(cXyz *);
f32 BFlower_sqrt(f32);
s32 BFlower_getReverb(s8);
fopAc_ac_c *BFlower_createBomb(s32, u32, cXyz *, s8, csXyz *, void *, s32, s32, s32);
f32 BFlower_addCalc(be<f32> *, f32, f32, f32, f32);
s32 BFlower_isExecuting(u32);
void BFlower_setCarryNow(fopAc_ac_c *, s32);
void BFlower_setActorKeep(BFlowerCarryController *, fopAc_ac_c *);
void BFlower_actorCtor(fopAc_ac_c *);
void BFlower_sphereCtor(dCcD_Sph *);
void BFlower_transformCtor(void *);
void BFlower_headerDtor(void *);
void BFlower_headerInit1(void *);
void BFlower_headerInit2(void *);
void BFlower_delete(void *);
void BFlower_transformDtor(void *, s32);
void BFlower_sphereDtor(dCcD_Sph *, s32);
void BFlower_actorDtor(fopAc_ac_c *, s32);

void BFlower_setCollision(cCcS *, dCcD_GObjInf *);

using BFlowerFloat = be<f32>;
using BFlowerByte = be<u8>;

// VbakH archive indices (GameCube names, also verified in HD).
enum {
    dRes_INDEX_VBAKH_BCK_VBAHX_e = 5,
    dRes_INDEX_VBAKH_BCK_VBAKH_e = 6,
    dRes_INDEX_VBAKH_BCK_VBAMX_e = 8,
    dRes_INDEX_VBAKH_BDL_VBAKH_e = 11,
    dRes_INDEX_VBAKH_BDL_VBAKM_e = 12,
    dRes_INDEX_VBAKH_BRK_VBAHX_e = 15,
    dRes_INDEX_VBAKH_BRK_VBAMX_e = 17
};

// HD: GHS inline constructors use translation-unit vtables.
enum : u32 {
    BFLOWER_SAFE_STRING_VTABLE = 0x10008764,
    BFLOWER_ACTOR_VTABLE = 0x100087B4,
    BFLOWER_CYL_AABB_VTABLE = 0x1000877C,
    BFLOWER_TRANSFORM_BASE_VTABLE = 0x1016E54C,
    BFLOWER_BCK_TRANSFORM_VTABLE = 0x1000878C,
    BFLOWER_MTX_CALC_VTABLE = 0x1016D820,
};

WWHD_OFFSET(BFlowerLinkView, mActorKeep, 0x65A0);
WWHD_OFFSET(BFlowerPlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(BFlowerPlayerView, __vtbl, 0xB4);
WWHD_OFFSET(BFlowerJointTree, mpMtxCalc, 0x14);
WWHD_OFFSET(BFlowerModelData, mJointTree, 8);
WWHD_OFFSET(BFlowerSaveView, mSwitchInfo, 0x20);
WWHD_OFFSET(BFlowerBckStorage, mTransform, 0x14);
WWHD_OFFSET(BFlowerBckStorage, __vtblMtxCalc, 0x48);
WWHD_OFFSET(BFlowerBckStorage, mpAnmTransform, 0x58);
WWHD_OFFSET(BFlowerBckStorage, mpQuat, 0x80);
WWHD_OFFSET(BFlowerBckStorage, mpCallback1, 0x84);
WWHD_OFFSET(BFlowerBckStorage, mpCallback2, 0x88);
WWHD_SIZE(BFlowerHeaderStatics, 0x10);
WWHD_SIZE(BFlowerActionTable, 24);
