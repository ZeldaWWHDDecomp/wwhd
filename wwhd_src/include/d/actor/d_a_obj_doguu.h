/* daObjDoguu_c (Triangle Island goddess statue).
 * GameCube -> WWHD: NPC base +0x118; brk storage grows by 0x60, each bck by 0x7C.
 * mStts through mPos therefore move +0x2EC. LIGHT_INFLUENCE grows by four bytes;
 * field_0x8FC moves +0x2F0, and the actor grows from 0x900 to 0xBF0.
 * field_0xNNN member names are the GameCube offsets. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/d_npc.h"
enum dRes_INDEX_DOGUU {
    /* BCK  */
    dRes_INDEX_DOGUU_BCK_VGSBA_e = 0x6,
    dRes_INDEX_DOGUU_BCK_VGSHD_e = 0x7,
    dRes_INDEX_DOGUU_BCK_VGSHF_e = 0x8,
    dRes_INDEX_DOGUU_BCK_VGSHN_e = 0x9,
    dRes_INDEX_DOGUU_BCK_VGSPA_e = 0xA,
    /* BDLM */
    dRes_INDEX_DOGUU_BDL_VGSBA_e = 0xD,
    dRes_INDEX_DOGUU_BDL_VGSHD_e = 0xE,
    dRes_INDEX_DOGUU_BDL_VGSHF_e = 0xF,
    dRes_INDEX_DOGUU_BDL_VGSHN_e = 0x10,
    dRes_INDEX_DOGUU_BDL_VGSMA_e = 0x11,
    dRes_INDEX_DOGUU_BDL_VGSPD_e = 0x12,
    dRes_INDEX_DOGUU_BDL_VGSPF_e = 0x13,
    dRes_INDEX_DOGUU_BDL_VGSPN_e = 0x14,
    /* BMT  */
    dRes_INDEX_DOGUU_BMT_VGSBD_e = 0x17,
    dRes_INDEX_DOGUU_BMT_VGSBF_e = 0x18,
    dRes_INDEX_DOGUU_BMT_VGSBN_e = 0x19,
    dRes_INDEX_DOGUU_BMT_VGSMD_e = 0x1A,
    dRes_INDEX_DOGUU_BMT_VGSMF_e = 0x1B,
    dRes_INDEX_DOGUU_BMT_VGSMN_e = 0x1C,
    /* BRK  */
    dRes_INDEX_DOGUU_BRK_VGSMD_e = 0x1F,
    dRes_INDEX_DOGUU_BRK_VGSMF_e = 0x20,
    dRes_INDEX_DOGUU_BRK_VGSMN_e = 0x21,
    dRes_INDEX_DOGUU_BTK_VGSPA_e = 0x22,
};

struct daObjDoguu_c : fopNpc_npc_c {
    enum State {
        STATE_WAIT_PEARL_e = 0,
        STATE_ORDER_DEMO1_e = 1,
        STATE_DEMO1_e = 2,
        STATE_SELECT_DEMO2_e = 3,
        STATE_ORDER_DEMO2_e = 4,
        STATE_DEMO2_e = 5,
        STATE_START_DEMO3_e = 6,
        STATE_ORDER_DEMO3_e = 7,
        STATE_DEMO3_e = 8,
        STATE_FINISH_DEMO3_e = 9,
        STATE_WAIT_e = 10,
        STATE_ORDER_MEGAMI_e = 11,
        STATE_MEGAMI_e = 12,
        STATE_MEGAMI_DONE_e = 13,
        STATE_DONE_e = 14,
    };
    enum CutAction {
        ACT_SETGOAL_e = 0,
        ACT_SETBALL_e = 1,
        ACT_SETANGLE_e = 2,
        ACT_EYEFLASH_e = 3,
        ACT_MESSAGE_KOKOE_e = 4,
        ACT_MESSAGE_OITA_e = 5,
        ACT_QUAKE_e = 6,
        ACT_JDEMO_e = 7,
        ACT_SETLIGHT_e = 8,
    };
    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ gptr<J3DModel> field_0x6CC;
    /* 0x7E8 */ gptr<J3DModel> field_0x6D0;
    /* 0x7EC */ gptr<J3DModel> field_0x6D4;
    /* 0x7F0 */ gptr<J3DModel> field_0x6D8;
    /* 0x7F4 */ mDoExt_brkAnm mBrk;
    /* 0x86C */ mDoExt_bckAnm mBckHead;
    /* 0x8F8 */ mDoExt_bckAnm mBckBody;
    /* 0x984 */ mDoExt_bckAnm mBckCrystal;
    /* 0xA10 */ dCcD_Stts mStts;
    /* 0xA4C */ dCcD_Cyl mCyl;
    /* 0xB7C */ be<u8> mItemNo;
    /* 0xB7D */ u8 _B7D[3];
    /* 0xB80 */ be<s32> field_0x894;
    /* 0xB84 */ be<u32> mHead1JntNo;
    /* 0xB88 */ be<u32> field_0x89C;
    /* 0xB8C */ be<u8> field_0x8A0;
    /* 0xB8D */ be<u8> field_0x8A1;
    /* 0xB8E */ be<u8> field_0x8A2;
    /* 0xB8F */ be<u8> field_0x8A3;
    /* 0xB90 */ be<s16> mDoguuDemo1EventIdx;
    /* 0xB92 */ be<s16> mDoguuDemo2EventIdx;
    /* 0xB94 */ be<s16> mDoguuDemo3EventIdx;
    /* 0xB96 */ be<s16> mMegamiDemoEventIdx;
    /* 0xB98 */ be<s32> field_0x8AC;
    /* 0xB9C */ be<s8> mActIdx;
    /* 0xB9D */ u8 _B9D[3];
    /* 0xBA0 */ cXyz field_0x8B4;
    /* 0xBAC */ be<s32> mTimer;
    /* 0xBB0 */ be<s32> field_0x8C4;
    /* 0xBB4 */ be<f32> mAllColRatio;
    /* 0xBB8 */ be<u32> mShape;
    /* 0xBBC */ cXyz mPos;
    /* 0xBC8 */ LIGHT_INFLUENCE mLightInfluence;
    /* 0xBEC */ be<f32> field_0x8FC;
    BOOL _delete();
    void CreateInit();
    s32 _create();
    BOOL _execute();
    BOOL _draw();
    void privateCut();
    s32 getFinishEventCount();
    void setFinishMyEvent();
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    BOOL CreateHeap();
    void setPointLight();
    void set_mtx();
    void setGoal(s32);
    void setPlayerAngle(s32);
    void setQuake(s32);
    void setJDemo(s32);
};
WWHD_OFFSET(daObjDoguu_c, mPhs, 0x7DC);
WWHD_OFFSET(daObjDoguu_c, field_0x6CC, 0x7E4);
WWHD_OFFSET(daObjDoguu_c, field_0x6D0, 0x7E8);
WWHD_OFFSET(daObjDoguu_c, field_0x6D4, 0x7EC);
WWHD_OFFSET(daObjDoguu_c, field_0x6D8, 0x7F0);
WWHD_OFFSET(daObjDoguu_c, mBrk, 0x7F4);
WWHD_OFFSET(daObjDoguu_c, mBckHead, 0x86C);
WWHD_OFFSET(daObjDoguu_c, mBckBody, 0x8F8);
WWHD_OFFSET(daObjDoguu_c, mBckCrystal, 0x984);
WWHD_OFFSET(daObjDoguu_c, mStts, 0xA10);
WWHD_OFFSET(daObjDoguu_c, mCyl, 0xA4C);
WWHD_OFFSET(daObjDoguu_c, mItemNo, 0xB7C);
WWHD_OFFSET(daObjDoguu_c, field_0x894, 0xB80);
WWHD_OFFSET(daObjDoguu_c, mHead1JntNo, 0xB84);
WWHD_OFFSET(daObjDoguu_c, field_0x89C, 0xB88);
WWHD_OFFSET(daObjDoguu_c, field_0x8A0, 0xB8C);
WWHD_OFFSET(daObjDoguu_c, field_0x8A1, 0xB8D);
WWHD_OFFSET(daObjDoguu_c, field_0x8A2, 0xB8E);
WWHD_OFFSET(daObjDoguu_c, field_0x8A3, 0xB8F);
WWHD_OFFSET(daObjDoguu_c, mDoguuDemo1EventIdx, 0xB90);
WWHD_OFFSET(daObjDoguu_c, mDoguuDemo2EventIdx, 0xB92);
WWHD_OFFSET(daObjDoguu_c, mDoguuDemo3EventIdx, 0xB94);
WWHD_OFFSET(daObjDoguu_c, mMegamiDemoEventIdx, 0xB96);
WWHD_OFFSET(daObjDoguu_c, field_0x8AC, 0xB98);
WWHD_OFFSET(daObjDoguu_c, mActIdx, 0xB9C);
WWHD_OFFSET(daObjDoguu_c, field_0x8B4, 0xBA0);
WWHD_OFFSET(daObjDoguu_c, mTimer, 0xBAC);
WWHD_OFFSET(daObjDoguu_c, field_0x8C4, 0xBB0);
WWHD_OFFSET(daObjDoguu_c, mAllColRatio, 0xBB4);
WWHD_OFFSET(daObjDoguu_c, mShape, 0xBB8);
WWHD_OFFSET(daObjDoguu_c, mPos, 0xBBC);
WWHD_OFFSET(daObjDoguu_c, mLightInfluence, 0xBC8);
WWHD_OFFSET(daObjDoguu_c, field_0x8FC, 0xBEC);
WWHD_SIZE(daObjDoguu_c, 0xBF0);

namespace DoguuHD {
enum : s32 {
    fpcNm_PLAYER_e = 0xA8,
    FINISH_DIN_EVENT = 0x1480,
    FINISH_FARORE_EVENT = 0x1440,
    FINISH_NAYRU_EVENT = 0x1410,
    TOWER_OPEN_EVENT = 0x1E40,
    ITEM_DIN_PEARL = 0x6A,
    ITEM_FARORE_PEARL = 0x6B,
    ITEM_NAYRU_PEARL = 0x69,
};
enum : u32 {
    StatusVtable = 0x1004AE88,
    GlobalStatusVtable = 0x1004AEC0,
    ShapeVtable = 0x100015A8,
    CylinderGeometryVtable = 0x1004B150,
    CylinderHitVtable = 0x1004B108,
    CylinderShapeVtable = 0x1004B160,
    ActorVtable = 0x100275A0,
    FrameCtrlVtable = 0x1016E54C,
    TransformCacheVtable = 0x1016D820,
    BckVtable = 0x10027314,
    AabVtable = 0x10027304,
    AcchVtable = 0x1002733C,
    AcchSecondaryVtable = 0x1002734C,
};
// HD: animation embeds a transform cache and bookkeeping after the frame controller.
struct BckConstruction {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mTransformCache[0x34];
    be<u32> mCacheVtable;
    u8 cache[0xC];
    be<u32> mCacheData;
    u8 state[0x20];
    be<u32> mCacheCount, mData, mResource, mFlags;
};
WWHD_OFFSET(BckConstruction, mTransformCache, 0x14);
WWHD_OFFSET(BckConstruction, mCacheVtable, 0x48);
WWHD_OFFSET(BckConstruction, mCacheData, 0x58);
WWHD_OFFSET(BckConstruction, mCacheCount, 0x7C);
WWHD_SIZE(BckConstruction, 0x8C);
struct CylinderConstruction : dCcD_GObjInf {
    u8 mAab[0x18];
    be<u32> mAabVtable, mShapeVtable;
    cM3dGCyl mCyl;
};
WWHD_OFFSET(CylinderConstruction, mAabVtable, 0x110);
WWHD_OFFSET(CylinderConstruction, mShapeVtable, 0x114);
WWHD_OFFSET(CylinderConstruction, mCyl, 0x118);
WWHD_SIZE(CylinderConstruction, 0x130);
struct AcchConstruction {
    u8 prefix[0x14];
    be<u32> mSecondaryVtable;
    u8 gap[8];
    be<u32> mVtable;
};
WWHD_OFFSET(AcchConstruction, mVtable, 0x20);
WWHD_SIZE(AcchConstruction, 0x24);
struct HeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mInitA, mInitB, padding[2];
    be<u32> mZeros[4];
};
WWHD_OFFSET(HeaderStatics, mZeros, 0xC);
WWHD_SIZE(HeaderStatics, 0x1C);
struct DestructorRecords {
    u8 records[3][12];
};
WWHD_SIZE(DestructorRecords, 0x24);
} // namespace DoguuHD

namespace DoguuHD {
struct TextureMatrixEntry {
    u8 prefix[4];
    be<s32> mId;
    u8 gap[4];
    be<u16> mParent;
    u8 tail[6];
};
WWHD_OFFSET(TextureMatrixEntry, mParent, 0xC);
WWHD_SIZE(TextureMatrixEntry, 0x14);
struct MaterialData {
    u8 prefix[0x34];
    be<u32> mMatrixTableOffset;
};
WWHD_SIZE(MaterialData, 0x38);
struct MaterialPacket {
    gptr<MaterialData> mpData;
    be<u16> mFlags;
    u8 padding[6];
    gptr<be<u32>> mpDirtyBits;
    u8 tail[0x3C - 0x10];
};
WWHD_OFFSET(MaterialPacket, mpDirtyBits, 0xC);
WWHD_SIZE(MaterialPacket, 0x3C);
struct Model {
    u8 prefix[0x2A];
    be<u16> mMaterialCount;
    u8 gap[8];
    gptr<MaterialPacket> mpMaterialPackets;
    u8 gapData[0xAC - 0x38];
    gptr<J3DModelData> mpData;
    u8 gapEffect[0xF8 - 0xB0];
    Mtx34 mEffectMtx;
};
WWHD_OFFSET(Model, mpMaterialPackets, 0x34);
WWHD_OFFSET(Model, mpData, 0xAC);
WWHD_OFFSET(Model, mEffectMtx, 0xF8);
WWHD_SIZE(Model, 0x128);
struct JointTree {
    u8 prefix[0x14];
    be<u32> mBck;
};
WWHD_SIZE(JointTree, 0x18);
struct MaterialAnimation {
    u8 prefix[0x48];
    be<u32> mBrk;
};
WWHD_SIZE(MaterialAnimation, 0x4C);
struct Camera {
    u8 prefix[0xDC];
    cXyz mEye;
};
WWHD_SIZE(Camera, 0xE8);
} // namespace DoguuHD

namespace DoguuHD {
struct PlayerVtable {
    u8 prefix[0x114];
    be<u32> mSetAngle;
};
WWHD_SIZE(PlayerVtable, 0x118);
struct JointNames {
    u8 prefix[0x10];
    be<u32> mTableOffset;
};
WWHD_SIZE(JointNames, 0x14);
struct ResourceIndices {
    be<s32> bmt_vgsm[3], bmt_vgsb[3], bck[3], brk[3], bdl_vgsh[3], bdl_vgsp[3];
};
WWHD_SIZE(ResourceIndices, 0x48);
struct LightColors {
    be<s16> colors[9];
};
WWHD_SIZE(LightColors, 0x12);
struct Actions {
    gptr<const char> names[9];
};
WWHD_SIZE(Actions, 0x24);
struct DemoActor {
    enum Enable { ENABLE_POSITION_e = 2, ENABLE_ANGLE_e = 8, ENABLE_SHAPE_e = 16, ENABLE_FRAME_e = 64 };
    u8 prefix[4];
    be<u16> mEnable;
    u8 padding[2];
    cXyz mPos;
    u8 gap[0x20 - 0x14];
    be<u16> mAngleX, mAngleY, mAngleZ;
    u8 gapShape[2];
    be<u32> mShape;
    u8 gapFrame[4];
    be<f32> mFrame;
};
WWHD_OFFSET(DemoActor, mPos, 8);
WWHD_OFFSET(DemoActor, mAngleX, 0x20);
WWHD_OFFSET(DemoActor, mShape, 0x28);
WWHD_OFFSET(DemoActor, mFrame, 0x30);
WWHD_SIZE(DemoActor, 0x34);
} // namespace DoguuHD

using DoguuHeapCallback = BOOL (*)(daObjDoguu_c*);

// HD: observed full-width callee signatures used by this translation unit.

void doguuStatusBaseCtor(void*);
void doguuCollisionSet(void*, void*);
void doguuCircleDtor(void*, s32);
void doguuCylinderSetC(void*, cXyz*);
void doguuCylinderSetR(void*, f32);
void doguuCylinderCtor(void*);
void doguuAcchDtor(void*, s32);
u32 doguuGetCamera();
void doguuStatusDtor(void*, s32);
void doguuCollisionDtor(void*, s32);
void doguuGlobalStatusCtor(void*);
void doguuStatusInit(void*, s32, s32, fopAc_ac_c*);
void doguuCollisionCtor(void*);
u32 doguuCoHitObject(void*);
s32 doguuCheckCoHit(void*);
void doguuCollisionAssign(void*, void*);
void doguuSetNextStage(const char*, s32, s32, s32, s32, s32, s32, f32);
s32 doguuResourceLoad(request_of_phase_process_class*, const char*);
void doguuResourceDelete(request_of_phase_process_class*, const char*);
s32 doguuReverb(s32);
u32 doguuGetDemoActor(void*, u32);
s32 doguuGetStaff(void*, const char*, s32, s32);
s32 doguuGetAct(void*, s32, void*, s32, s32, s32);
void doguuCutEnd(void*, s32);
void doguuSetGoal(void*, cXyz*);
s32 doguuGetEventIndex(void*, const char*, s32);
s32 doguuCheckEventEnd(void*, s32);
s32 doguuIsAdvance(void*, s32);
u32 doguuGetSubstance(void*, s32, const char*, s32);
s32 doguuCheckItem(s32, s32);
u32 doguuEnvironment();
void doguuSetColorRatio(f32);
void doguuSettingTev(void*, s32, cXyz*, void*);
void doguuLightTev(void*, void*, void*);
void doguuLightDirection(void*, cXyz*, cXyz*);
s32 doguuTalk(void*, s32);
void doguuNpcCtor(void*);
void doguuParticle(void*, s32, u32, cXyz*, csXyz*, s32, s32, s32, s32, s32, s32, s32);
void doguuOnEvent(void*, s32);
s32 doguuIsEvent(void*, s32);
void doguuStartQuake(void*, void*, s32, s32, cXyz*);
void doguuStopQuake(void*, s32);
void doguuActorDtor(void*, s32);
s32 doguuEntryHeap(void*, DoguuHeapCallback, s32);
void doguuOrderChange(void*, s32, s32, s32);
void doguuOrderOther(void*, s32, s32, s32, s32, s32);
void doguuSubBgm(u32);
void doguuModelUpdate(void*, s32);
J3DModel* doguuModelCreate(void*, u32, u32);
void doguuBrkCtor(void*);
s32 doguuBrkInit(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32);
void doguuBrkEntry(void*, void*, f32);
s32 doguuBckInit(void*, void*, void*, s32, s32, s32, s32, s32, f32);
void doguuBckEntry(void*, void*, f32);
u32 doguuObjectResource(void*, SafeString*, s32);
daObjDoguu_c* doguuAllocate(u32);
void doguuAnimationCacheCtor(void*);
s32 doguuJointIndex(void*, const char*);
void doguuFrameCtrlInit(void*, s32);
void doguuAnimationDtor(void*, s32);
u32 doguuJointNames(void*);
u32 doguuTexEffectMatrix(void*, s32);
u32 doguuTexMatrixInfo(void*, s32, be<s32>*);
void doguuMatrixCopy(void*, void*);
void doguuMatrixConcat(Mtx34*, Mtx34*, Mtx34*);
void doguuMatrixScale(Mtx34*, f32, f32, f32);
void doguuMatrixLookAt(Mtx34*, cXyz*, cXyz*, cXyz*);
void doguuHalfAngle(cXyz*, cXyz*, cXyz*);
void doguuHeaderInitB(void*);
void doguuHeaderInitA(void*);
void doguuRegisterDtor(void*);

daObjDoguu_c* daObjDoguu_c_construct(daObjDoguu_c*);
