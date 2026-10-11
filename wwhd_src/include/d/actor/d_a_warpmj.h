/** Forsaken Fortress warp portal layout.
 * HD: tww leaves this actor opaque. The actor has one extra tev structure;
 * renderer and constructor views are private to this unit and retain shared animation layouts.
 */
#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
// HD actor allocation is 0x5AC. GC provides no recovered layout.
enum GmjwpResource {
    GMJWP_BCK_PORTAL = 6,
    GMJWP_BDL_PORTAL = 9,
    GMJWP_BRK_PORTAL = 12,
    GMJWP_BTK_PORTAL = 15
};
enum WarpmjOrder { ORDER_IDLE = 0, ORDER_REQUESTED = 1 };
enum WarpmjFlags { WARP_PLAYER_READY = 0x10000, WARP_UNIFORM_DIRTY = 0x100 };
struct daWarpmj_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x3BC */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3C0 */ gptr<mDoExt_bckAnm> mpBck;
    /* 0x3C4 */ gptr<void> mpParticles[3];
    /* 0x3D0 */ be<u32> mDestination;
    /* 0x3D4 */ be<s32> mOrder;
    /* 0x3D8 */ be<s16> mEventIndex;
    u8 pad3DA[2];
    /* 0x3DC */ be<s32> mDemoState;
    /* 0x3E0 */ be<s32> mStaff;
    /* 0x3E4 */ dKy_tevstr_c mWarpTev;
    BOOL CreateHeap();
    f32 getSeaY(cXyz* position);
    void setEndAnm();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    u32 animPlay();
    BOOL check_warp();
    void normal_execute();
    void checkOrder();
    void demo_proc();
    void eventOrder();
    void demo_execute();
    BOOL _execute();
    BOOL actWait(s32 staff);
    void initWarp(s32 staff);
    BOOL actWarp(s32 staff);
    void initWarpArrive(s32 staff);
    BOOL actWarpArrive(s32 staff);
    void initWait(s32 staff);
};

using WarpmjAction = gabi::ptmf<s32 (daWarpmj_c::*)(s32)>;
struct WarpmjActionTable {
    WarpmjAction mActions[3];
};
struct WarpmjActionNames {
    gptr<const char> mNames[3];
};
struct WarpmjLightBlock {
    be<f32> mVectors[6];
    be<u8> mColor[4];
    be<s16> mCoefficients[4];
    be<f32> mAttenuation[8];
};
struct WarpmjBckConstruction {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mAnimationVtable;
    u8 mTransform[0x34];
    be<u32> mTransformVtable;
    u8 gap[0xC];
    gptr<void> mpTransform;
    u8 gap2[0x20];
    be<u32> mAnimationWords[4];
};
struct WarpmjSave {
    u8 prefix[0x644];
    u8 mEventBits;
};
struct WarpmjDemoActor {
    u8 prefix[0x28];
    be<s32> mState;
};
struct WarpmjGameInfo {
    u8 prefix[0x5292];
    be<u8> mEventRunning;
    u8 gap[0x52C4 - 0x5293];
    u8 mEventManager;
    u8 gap2[0x5AB0 - 0x52C5];
    gptr<void> mpParticles;
    u8 gap3[0x5B3C - 0x5AB4];
    gptr<fopAc_ac_c> mpPlayer;
    u8 gap4[0x5CD8 - 0x5B40];
    be<u32> mPlayerStatus;
};
struct WarpmjColorVtable {
    u8 prefix[0x3C];
    gfn mSetColor;
    u8 gap[0xC];
    gfn mGetColor;
};
struct WarpmjColorBlock {
    u8 prefix[4];
    gptr<WarpmjColorVtable> mpVtable;
    be<u8>* getColor(s32 index) {
        return gabi::call_ptr<be<u8>*>(mpVtable->mGetColor.get(), this, index);
    }
    void setColor(s32 index, be<u8>* color) {
        gabi::call_ptr(mpVtable->mSetColor.get(), this, index, color);
    }
};
struct WarpmjMaterial {
    u8 prefix[0x18];
    gptr<WarpmjColorBlock> mpColorBlock;
    u8 gap[0xA0 - 0x1C];
    be<u32> mUniformFlags;
    u8 tail[0x39C - 0xA4];
};
struct WarpmjMaterialBlock {
    u8 prefix[0x24];
    be<u16> mCount;
};
struct WarpmjHeader {
    be<f32> mMinAngle, mMaxAngle;
    u8 mByte, mFlag;
    u8 pad[2];
};
struct WarpmjWords {
    be<u32> mWords[4];
};
struct WarpmjDestructorRecords {
    u8 mRecords[3][12];
};
void* warpmjGetResource(void*, SafeString*, s32);
void warpmjAssert(const char*, s32, const char*);
J3DModel* warpmjCreateModel(void*, u32, u32);
WarpmjBckConstruction* warpmjAllocateBck(u32);
void warpmjInitFrame(void*, s32);
void warpmjConstructTransform(void*);
s32 warpmjInitBck(void*, void*, void*, s32, s32, f32, s32, s32, s32);
mDoExt_btkAnm* warpmjCreateBtk(void*);
s32 warpmjInitBtk(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
mDoExt_brkAnm* warpmjCreateBrk(void*);
s32 warpmjInitBrk(void*, void*, void*, s32, s32, f32, s32, s32, s32, s32);
void warpmjConstructActor(daWarpmj_c*);
s32 warpmjCheckSea(f32, f32);
f32 warpmjGetSeaHeight(f32, f32);
f32 warpmjGetWaterHeight(cXyz*);
void warpmjSetCullBox(daWarpmj_c*, f32, f32, f32, f32, f32, f32);
void warpmjTranslate(Mtx34*, f32, f32, f32);
void* warpmjSpawnParticle(void*, u32, u32, cXyz*, u32, u32, u32, u32, s32, u32, u32, u32);
s32 warpmjGetEventIndex(void*, const char*, s32);
void warpmjInitTev(dKy_tevstr_c*, s32, s32);
s32 warpmjCheckEventBit(void*, u32);
s32 warpmjLoadResource(request_of_phase_process_class*, const char*);
s32 warpmjEntryHeap(daWarpmj_c*, u32, u32);
void warpmjDeleteResource(request_of_phase_process_class*, const char*);
void warpmjSetLight(void*, s32, cXyz*, dKy_tevstr_c*);
void warpmjApplyLight(void*, J3DModel*, dKy_tevstr_c*);
WarpmjMaterialBlock* warpmjGetMaterialBlock(J3DModelData*);
void warpmjCorrectColor(void*, void*, f32);
be<f32>* warpmjGetUniform(be<u32>*, s32);
void warpmjEntryBtk(mDoExt_baseAnm*, J3DModelData*, f32);
void warpmjEntryBrk(mDoExt_baseAnm*, J3DModelData*, f32);
void warpmjEntryBck(mDoExt_baseAnm*, J3DModelData*, f32);
void warpmjUpdateModel(J3DModel*, u32);
u32 warpmjPlayAnimation(mDoExt_btkAnm*);
void warpmjSubtract(cXyz*, cXyz*, cXyz*);
f32 warpmjSquaredMagnitude(cXyz*);
f32 warpmjSqrt(f32);
s32 warpmjCheckStart(void*, s32);
s32 warpmjCheckEnd(void*, s32);
void warpmjChangeScene(u32, s32);
s32 warpmjGetStaff(void*, const char*, u32, u32);
s32 warpmjGetAction(void*, s32, void*, s32, s32, s32);
void warpmjEndCut(void*, s32);
s32 warpmjIsAdvance(void*, s32);
void warpmjOrderEvent(daWarpmj_c*, s32, u32, u32, u32, u32);
WarpmjDemoActor* warpmjGetDemoActor(void*, u32);
void warpmjRemoveDrawTag(void*);
void* warpmjGetDrawQueue(daWarpmj_c*);
void warpmjQueueDrawTag(void*, void*);
s32 warpmjGetReverb(s32);
void warpmjStartLevelSound(u32, cXyz*, u32, s32);
void warpmjSetGoal(void*, cXyz*);
void warpmjStartSound(u32);
void warpmjRegisterGlobal(void*);
void warpmjInitHeaderByte(void*);
void warpmjInitHeaderFlag(void*);
void warpmjDeleteObject(void*);
void warpmjDestroyActor(daWarpmj_c*, s32);
WarpmjGameInfo* warpmjGetGameInfo();
void* warpmjGetEnvironment();
daWarpmj_c* warpmjAllocateActor(u32);
WWHD_OFFSET(daWarpmj_c, mPhase, 0x3AC);
WWHD_OFFSET(daWarpmj_c, mpModel, 0x3B4);
WWHD_OFFSET(daWarpmj_c, mpBtk, 0x3B8);
WWHD_OFFSET(daWarpmj_c, mpBrk, 0x3BC);
WWHD_OFFSET(daWarpmj_c, mpBck, 0x3C0);
WWHD_OFFSET(daWarpmj_c, mpParticles, 0x3C4);
WWHD_OFFSET(daWarpmj_c, mDestination, 0x3D0);
WWHD_OFFSET(daWarpmj_c, mOrder, 0x3D4);
WWHD_OFFSET(daWarpmj_c, mEventIndex, 0x3D8);
WWHD_OFFSET(daWarpmj_c, mDemoState, 0x3DC);
WWHD_OFFSET(daWarpmj_c, mStaff, 0x3E0);
WWHD_OFFSET(daWarpmj_c, mWarpTev, 0x3E4);
WWHD_SIZE(daWarpmj_c, 0x5AC);
WWHD_OFFSET(WarpmjAction, d, 0x0);
WWHD_OFFSET(WarpmjAction, i, 0x2);
WWHD_OFFSET(WarpmjAction, f, 0x4);
WWHD_SIZE(WarpmjAction, 0x8);
WWHD_OFFSET(WarpmjActionTable, mActions, 0x0);
WWHD_SIZE(WarpmjActionTable, 0x18);
WWHD_OFFSET(WarpmjActionNames, mNames, 0x0);
WWHD_SIZE(WarpmjActionNames, 0xC);
WWHD_OFFSET(WarpmjLightBlock, mVectors, 0x0);
WWHD_OFFSET(WarpmjLightBlock, mColor, 0x18);
WWHD_OFFSET(WarpmjLightBlock, mCoefficients, 0x1C);
WWHD_OFFSET(WarpmjLightBlock, mAttenuation, 0x24);
WWHD_SIZE(WarpmjLightBlock, 0x44);
WWHD_OFFSET(WarpmjBckConstruction, mFrameCtrl, 0x0);
WWHD_OFFSET(WarpmjBckConstruction, mAnimationVtable, 0x10);
WWHD_OFFSET(WarpmjBckConstruction, mTransform, 0x14);
WWHD_OFFSET(WarpmjBckConstruction, mTransformVtable, 0x48);
WWHD_OFFSET(WarpmjBckConstruction, mpTransform, 0x58);
WWHD_OFFSET(WarpmjBckConstruction, mAnimationWords, 0x7C);
WWHD_SIZE(WarpmjBckConstruction, 0x8C);
WWHD_OFFSET(WarpmjSave, mEventBits, 0x644);
WWHD_SIZE(WarpmjSave, 0x645);
WWHD_OFFSET(WarpmjDemoActor, mState, 0x28);
WWHD_SIZE(WarpmjDemoActor, 0x2C);
WWHD_OFFSET(WarpmjGameInfo, mEventRunning, 0x5292);
WWHD_OFFSET(WarpmjGameInfo, mEventManager, 0x52C4);
WWHD_OFFSET(WarpmjGameInfo, mpParticles, 0x5AB0);
WWHD_OFFSET(WarpmjGameInfo, mpPlayer, 0x5B3C);
WWHD_OFFSET(WarpmjGameInfo, mPlayerStatus, 0x5CD8);
WWHD_SIZE(WarpmjGameInfo, 0x5CDC);
WWHD_OFFSET(WarpmjColorVtable, mSetColor, 0x3C);
WWHD_OFFSET(WarpmjColorVtable, mGetColor, 0x4C);
WWHD_SIZE(WarpmjColorVtable, 0x50);
WWHD_OFFSET(WarpmjColorBlock, mpVtable, 0x4);
WWHD_SIZE(WarpmjColorBlock, 0x8);
WWHD_OFFSET(WarpmjMaterial, mpColorBlock, 0x18);
WWHD_OFFSET(WarpmjMaterial, mUniformFlags, 0xA0);
WWHD_SIZE(WarpmjMaterial, 0x39C);
WWHD_OFFSET(WarpmjMaterialBlock, mCount, 0x24);
WWHD_SIZE(WarpmjMaterialBlock, 0x26);
WWHD_OFFSET(WarpmjHeader, mMinAngle, 0x0);
WWHD_OFFSET(WarpmjHeader, mMaxAngle, 0x4);
WWHD_OFFSET(WarpmjHeader, mByte, 0x8);
WWHD_OFFSET(WarpmjHeader, mFlag, 0x9);
WWHD_SIZE(WarpmjHeader, 0xC);
WWHD_OFFSET(WarpmjWords, mWords, 0x0);
WWHD_SIZE(WarpmjWords, 0x10);
WWHD_OFFSET(WarpmjDestructorRecords, mRecords, 0x0);
WWHD_SIZE(WarpmjDestructorRecords, 0x24);
