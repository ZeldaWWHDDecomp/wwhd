#pragma once
#include "f_op/f_op_actor.h"
#include "gc_api/gc_api.h"
#include "JSystem/J3DGraph.h"
#include "d/d_cc_d.h"
#include "d/d_kankyo.h"
#include "d/d_particle.h"
#include "gc_api/JSystem/JPAEmitter.h"
#include "gc_api/f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"
// Treasure chest. HD embeds enlarged animation objects and 0x24-byte light influences.
struct daTbox_c : fopAc_ac_c {
    struct modelInfo {
        be<s16> modelId, openBckId, btkId, brkId, closedColId, openColId;
    };
    using ActionFunc = BOOL (daTbox_c::*)();
    enum FuncType {
        FUNC_TYPE_NORMAL,
        FUNC_TYPE_SWITCH,
        FUNC_TYPE_ENEMIES,
        FUNC_TYPE_SWITCH_VISIBLE,
        FUNC_TYPE_SWITCH_TRANSPARENT,
        FUNC_TYPE_GRAVITY,
        FUNC_TYPE_TACT,
        FUNC_TYPE_EXTRA_SAVE_INFO,
        FUNC_TYPE_EXTRA_SAVE_INFO_SPAWN
    };
    enum {
        daTboxFlg_UNK_01 = 1,
        daTboxFlg_UNK_02 = 2,
        daTboxFlg_UNK_04 = 4,
        daTboxFlg_UNK_08 = 8,
        daTboxFlg_OPENING_e = 0x10,
        daTboxFlg_APPEARING_e = 0x20
    };

    // HD: resolved GHS entry words used by the eight-byte action member pointer.
    enum ActionEntry : u32 {
        ACTION_OPEN_WAIT = 0x024B7394,
        ACTION_WAIT = 0x024B79AC,
        ACTION_SWITCH_WAIT2 = 0x024B6334,
        ACTION_GENOCIDE = 0x024B75EC,
        ACTION_SWITCH_WAIT = 0x024B7514,
        ACTION_DEMO = 0x024B7708,
        ACTION_DEMO2 = 0x024B7804,
    };

    s32 getShapeType();
    u32 getModelInfo();
    s32 getFuncType();
    BOOL checkEnv();
    BOOL checkOpen();
    BOOL checkNormal();
    void lightReady();
    void setCollision();
    void clrDzb();
    void setDzb();
    void searchRoomNo();
    BOOL actionSwOnWait2();
    void lightUpProc();
    void lightDownProc();
    void darkProc();
    void volmProc();
    BOOL actionWait();
    BOOL CreateHeap();
    BOOL checkRoomDisp(s32 room);
    void OpenInit_com();
    void demoProcOpen();
    BOOL boxCheck();
    BOOL actionDemo();
    BOOL actionDemo2();
    void surfaceProc();
    void OpenInit();
    void demoInitAppear_Tact();
    void demoInitAppear();
    void demoProcAppear_Tact();
    void demoProcAppear();
    BOOL actionOpenWait();
    BOOL actionSwOnWait();
    BOOL actionGenocide();
    s32 commonShapeSet();
    s32 effectShapeSet();
    s32 bgCheckSet();
    BOOL execute();
    s32 demoProc();
    void CreateInit();
    void setAction(u32 target) {
        mActionFunc.d = 0;
        mActionFunc.i = -1;
        mActionFunc.f = target;
    }

    be<s32> mRoomNo;
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpChestMdl;
    mDoExt_bckAnm mOpenAnm;
    gptr<mDoExt_btkAnm> mpAppearTexAnm;
    gptr<mDoExt_brkAnm> mpAppearRegAnm;
    gptr<dBgW> mpBgWClosed, mpBgWOpen, mpBgWVines, mpBgWCurrent;
    gptr<J3DModel> mpFlashMdl;
    mDoExt_bckAnm mFlashAnm;
    mDoExt_btkAnm mFlashTexAnm;
    mDoExt_brkAnm mFlashRegAnm;
    be<u32> m0308; // HD: the preceding animation objects are larger.
    mDoExt_brkAnm mBrkAnm3;
    gptr<J3DModel> mpTactPlatformMdl;
    mDoExt_brkAnm mTactPlatformBrk;
    gabi::ptmf<ActionFunc> mActionFunc;
    be<f32> mInvisibleScrollVal;
    be<s32> mStaffId;
    Mtx34 mMtx;
    dKankyo::LIGHT_INFLUENCE mPLight, mEfLight;
    dPa_smokeEcallBack mSmokeCB;
    gptr<JPABaseEmitter> mSmokeEmitter;
    be<f32> mAllColRatio, mAppearingYOffset;
    be<u16> mFlags, mOpenTimer;
    be<u8> mHasOpenAnmFinished, mIsFlashPlaying;
    be<u16> mAppearTimer;
    be<u8> mGenocideDelayTimer;
    u8 pad791[3];
    dBgS_ObjAcch mObjAcch;
    dBgS_AcchCir mAcchCir;
    dCcD_Stts mColStatus;
    dCcD_Cyl mColCyl;
    be<u8> mTboxNo;
    u8 padB05[3];
};
WWHD_OFFSET(daTbox_c, mpChestMdl, 0x3B8);
WWHD_OFFSET(daTbox_c, mActionFunc, 0x6D4);
WWHD_OFFSET(daTbox_c, mFlags, 0x788);
WWHD_OFFSET(daTbox_c, mColCyl, 0x9D4);
WWHD_SIZE(daTbox_c, 0xB08);

template <class T> inline T* tboxView(u32 address) {
    // HD: preserve zero-plus-member and wrapped guest addresses used by the inline code.
    return reinterpret_cast<T*>(PPC_MEM_BASE + address);
}
struct TboxWordVector {
    be<u32> x, y, z;
};
WWHD_SIZE(TboxWordVector, 12);
struct TboxCylView : dCcD_GObjInf {
    u8 mBeforeBounds[0x18];
    be<u32> mBoundsVtable, mShapeVtable;
    cM3dGCyl mCyl;
};
WWHD_OFFSET(TboxCylView, mCyl, 0x118);
WWHD_SIZE(TboxCylView, 0x130);

// HD: HIO places the JOR vtable after the editable values.
struct daTbox_HIO_c {
    be<s8> mNo;
    u8 mPadding;
    be<s16> m06, m08, m0A, m0C;
    u8 mPaddingA[2];
    be<u32> __vtbl;
};
WWHD_OFFSET(daTbox_HIO_c, m08, 4);
WWHD_OFFSET(daTbox_HIO_c, __vtbl, 12);
WWHD_SIZE(daTbox_HIO_c, 16);
WWHD_SIZE(daTbox_c::modelInfo, 12);

struct TboxModelInfoTable {
    daTbox_c::modelInfo entries[4];
};
WWHD_SIZE(TboxModelInfoTable, 48);

// HD: the emitter draw state is larger; the editable dynamics prefix is unchanged.
struct TboxEmitter : JPABaseEmitter {
    /* 0x070 */ be<f32> mInitialVelDir;
    u8 mBeforeColor[0x244 - 0x74];
    /* 0x244 */ be<u8> mGlobalPrmColor[4];
    u8 mBeforeFlags[0x254 - 0x248];
    /* 0x254 */ be<u32> mFlags;
};
WWHD_OFFSET(TboxEmitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(TboxEmitter, mFlags, 0x254);
WWHD_SIZE(TboxEmitter, 0x258);
// HD: shader parameters replace GameCube material texture-matrix animation.
struct TboxShaderParameter {
    be<u16> mType, mOffset;
    be<s32> mIndex;
    u8 mBeforeParent[4];
    be<u16> mParent;
    u8 mTail[6];
};
WWHD_OFFSET(TboxShaderParameter, mParent, 12);
WWHD_SIZE(TboxShaderParameter, 20);
struct TboxMaterial {
    be<u32> mShader;
    be<u16> mFlags;
    u8 mBeforeDirty[6];
    be<u32> mDirtyMask;
    u8 mBeforeParameters[0x28 - 0x10];
    be<u32> mParameters;
    u8 mTail[0x3C - 0x2C];
};
WWHD_OFFSET(TboxMaterial, mParameters, 0x28);
WWHD_SIZE(TboxMaterial, 0x3C);
struct TboxShaderValues {
    be<f32> mScale, mUnused, mScroll, mScaleY;
};
WWHD_SIZE(TboxShaderValues, 16);
struct TboxModelRenderView {
    u8 mBeforeMaterials[0x34];
    be<u32> mMaterials;
};
WWHD_SIZE(TboxModelRenderView, 0x38);
struct TboxMaterialHeader {
    u8 mBeforeCount[0x24];
    be<u16> mMaterialCount;
};
WWHD_SIZE(TboxMaterialHeader, 0x26);

void tboxSetAllColorRatio(f32 ratio);
void tboxStartStream(u32 id);
void tboxStartSound(u32 id, const cXyz* pos, u32 flags, s32 reverb);
u32 tboxAllocate(u32 size);
void tboxAssert(u32 file, u32 line, u32 expression);
void tboxSetPriorityLight(u32 light);
void tboxDeleteHioChild(s32 child);
u8 tboxCreateHioChild(u32 name, u32 hio);
void tboxOnStageBox(u32 stage, u32 box);
BOOL tboxIsStageBox(u32 stage, u32 box);
void tboxOnBox(u32 save, u32 box);
BOOL tboxIsBox(u32 save, u32 box);
s32 tboxEnvShapeSet(daTbox_c* actor);
s32 tboxGetStaffId(u32 manager, u32 name, u32 actor, u32 id);
void tboxCutEnd(u32 manager, s32 staff);
BOOL tboxEventEndCheck(u32 manager, s32 event);
BOOL tboxEventEndCheckOld(u32 manager, u32 name);
BOOL tboxEventIsAdvance(u32 manager, s32 staff);
u32 tboxGetActionIndex(u32 manager, s32 staff, u32 actions, u32 count, u32 force, u32 parameter);
u32 tboxGetEventPid(u32 control, u32 index);
void tboxSetEventName(u32 info, u32 name);
void tboxOrderOtherEvent(daTbox_c* actor, u32 name, u32 priority, u32 flags);
u32 tboxSearchRoomEnemy(s8 room);
s32 tboxSeenActorAngle(u32 actor, u32 target);
s32 tboxFindShaderParameter(u32 names, u32 name);
void tboxSetModelInvisible(u32 model, u32 invisible);
u32 tboxGetMaterialHeader(u32 modelData);
void tboxDrawMapPoint(u32 type, s8 room, s32 angle, u8 number, u8 gbaName, u32 flags, f32 x, f32 y,
                      f32 z);
u32 tboxGetEnvLight();
void tboxSettingTevStruct(u32 env, s32 type, cXyz* pos, dKy_tevstr_c* tev);
void tboxSetLightTevColor(u32 env, void* model, dKy_tevstr_c* tev);

void tboxRegisterGlobal(u32 record);
void tboxInitMathHeader(u32 object);
void tboxInitMatrixHeader(u32 object);
void tboxDestroyCircle(u32 object, u32 flags);
void tboxDestroyAcch(u32 object, u32 flags);
void tboxDestroyMtxCalc(u32 object, u32 flags);
void tboxDestroyActor(daTbox_c* actor, u32 flags);
u32 tboxConstructBtk(u32 object);
u32 tboxConstructBrk(u32 object);
u32 tboxConstructBgW(u32 object);
void tboxConstructActor(daTbox_c* actor);
void tboxInitFrameCtrl(u32 object, s32 end);
void tboxConstructAnmCalc(u32 object);
void tboxConstructSmokeCallback(u32 object, u32 windOff);
void tboxConstructAcch(u32 object);
u32 tboxCreateDemoItem(cXyz* pos, u32 item, s32 room, s32 flag, u32 angle, u32 scale);
BOOL tboxEntrySolidHeap(daTbox_c* actor, u32 callback, u32 size);

struct TboxSafeString {
    be<u32> text, vtable;
};
WWHD_SIZE(TboxSafeString, 8);

void tboxRegisterCylinder(u32 collision, u32 cylinder);
void tboxDeleteResource(request_of_phase_process_class* phase, u32 name);
s32 tboxLoadResource(request_of_phase_process_class* phase, u32 name);
void tboxSubtractVector(u32 left, cXyz* result, const cXyz* right);
u32 tboxGetResource(u32 control, TboxSafeString* name, s32 index);
u32 tboxSetParticle(u32 manager, u32 group, u32 id, cXyz* pos, u32 angle, u32 scale, u32 alpha,
                    u32 callback, s32 setup, u32 prmColor, u32 envColor, u32 scale2D);

struct TboxRoomStatus {
    u8 mBeforeFlags[0x21C];
    be<u8> mFlags;
    u8 mTail[0x22C - 0x21D];
};
WWHD_SIZE(TboxRoomStatus, 0x22C);
struct TboxRoomControl {
    be<s8> mStayNo;
    u8 mPadding[3];
    TboxRoomStatus mStatus[64];
};
WWHD_OFFSET(TboxRoomControl, mStatus, 4);
struct TboxResourceControl {
    be<u32> mControl;
};
WWHD_SIZE(TboxResourceControl, 4);
struct TboxDrawBuffers {
    be<u32> mOpa, mXlu;
};
WWHD_SIZE(TboxDrawBuffers, 8);

// HD: bck animation embeds its matrix calculator and blend state (GameCube stores a pointer).
struct TboxBckView : mDoExt_baseAnm {
    be<u32> mMtxCalcVtable;
    u8 mMtxCalcBase[0x34];
    be<u32> mBlendVtable;
    u8 mBlendState[0xC];
    be<u32> mBlendFlags;
    u8 mTransformState[0x20];
    be<u32> mAnmFlags, mAnmIndex, mAnmData, mAnmTarget;
};
WWHD_OFFSET(TboxBckView, mMtxCalcVtable, 0x10);
WWHD_OFFSET(TboxBckView, mBlendVtable, 0x48);
WWHD_OFFSET(TboxBckView, mBlendFlags, 0x58);
WWHD_OFFSET(TboxBckView, mAnmFlags, 0x7C);
WWHD_SIZE(TboxBckView, 0x8C);
struct TboxHeaderStatics {
    be<f32> mAngleMin, mAngleMax;
    be<u8> mMathHeader, mMatrixHeader;
};
WWHD_SIZE(TboxHeaderStatics, 10);
struct TboxHeaderZeros {
    be<u32> mWords[4];
};
WWHD_SIZE(TboxHeaderZeros, 16);
struct TboxPiPair {
    be<f32> mMin, mMax;
};
WWHD_SIZE(TboxPiPair, 8);

WWHD_OFFSET(daTbox_c, mRoomNo, 0x3AC);
WWHD_OFFSET(daTbox_c, mPhase, 0x3B0);
WWHD_OFFSET(daTbox_c, mOpenAnm, 0x3BC);
WWHD_OFFSET(daTbox_c, mpAppearTexAnm, 0x448);
WWHD_OFFSET(daTbox_c, mpAppearRegAnm, 0x44C);
WWHD_OFFSET(daTbox_c, mpBgWClosed, 0x450);
WWHD_OFFSET(daTbox_c, mpBgWOpen, 0x454);
WWHD_OFFSET(daTbox_c, mpBgWVines, 0x458);
WWHD_OFFSET(daTbox_c, mpBgWCurrent, 0x45C);
WWHD_OFFSET(daTbox_c, mpFlashMdl, 0x460);
WWHD_OFFSET(daTbox_c, mFlashAnm, 0x464);
WWHD_OFFSET(daTbox_c, mFlashTexAnm, 0x4F0);
WWHD_OFFSET(daTbox_c, mFlashRegAnm, 0x564);
WWHD_OFFSET(daTbox_c, m0308, 0x5DC);
WWHD_OFFSET(daTbox_c, mBrkAnm3, 0x5E0);
WWHD_OFFSET(daTbox_c, mpTactPlatformMdl, 0x658);
WWHD_OFFSET(daTbox_c, mTactPlatformBrk, 0x65C);
WWHD_OFFSET(daTbox_c, mInvisibleScrollVal, 0x6DC);
WWHD_OFFSET(daTbox_c, mStaffId, 0x6E0);
WWHD_OFFSET(daTbox_c, mMtx, 0x6E4);
WWHD_OFFSET(daTbox_c, mPLight, 0x714);
WWHD_OFFSET(daTbox_c, mEfLight, 0x738);
WWHD_OFFSET(daTbox_c, mSmokeCB, 0x75C);
WWHD_OFFSET(daTbox_c, mSmokeEmitter, 0x77C);
WWHD_OFFSET(daTbox_c, mAllColRatio, 0x780);
WWHD_OFFSET(daTbox_c, mAppearingYOffset, 0x784);
WWHD_OFFSET(daTbox_c, mOpenTimer, 0x78A);
WWHD_OFFSET(daTbox_c, mHasOpenAnmFinished, 0x78C);
WWHD_OFFSET(daTbox_c, mIsFlashPlaying, 0x78D);
WWHD_OFFSET(daTbox_c, mAppearTimer, 0x78E);
WWHD_OFFSET(daTbox_c, mGenocideDelayTimer, 0x790);
WWHD_OFFSET(daTbox_c, mObjAcch, 0x794);
WWHD_OFFSET(daTbox_c, mAcchCir, 0x958);
WWHD_OFFSET(daTbox_c, mColStatus, 0x998);
WWHD_OFFSET(daTbox_c, mTboxNo, 0xB04);
