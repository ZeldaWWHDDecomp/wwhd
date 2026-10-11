/** Forest Firefly. HD: actor base grows by0x11C, BRK state by0x6C, action descriptors
 * shrink to8 bytes, and an HD point light extends the actor to0x844. Private derived source. */
#pragma once
#include "JSystem/J3DGraph.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"

struct J3DModel;

// HD: action member pointers use8 bytes rather than the GameCube12-byte form.
struct NhAction {
    be<s16> thisAdjustment;
    be<s16> virtualIndex;
    be<u32> target;
};
WWHD_SIZE(NhAction, 8);

struct NhCatch {
    u8 opaque;
};
struct NhAttentionView {
    u8 padding[0x130];
    NhCatch mCatch;
};
inline NhCatch *nhCatchController(dAttention_c *p) {
    return &reinterpret_cast<NhAttentionView *>(p)->mCatch;
}
struct NhBrkAnm {
    u8 padding[4];
    be<f32> mFrame;
    u8 padding8[0x7C];
};
struct NhModelData {
    u8 padding[0x48];
    be<u32> mTevRegAnimatorHD;
};
struct NhShapeAttr {
    u8 padding[0x18];
    be<u32> aabVtable, shapeVtable;
};
struct NhSourceCylinder {
    u8 data[0x54];
};
struct NhHioDefaults {
    be<u32> words[15];
};
struct NhHeaderStatics {
    be<u32> words[4];
};
struct NhDtorRecord {
    be<u32> words[3];
};
struct NhPiPair {
    be<f32> lo, hi;
};
using NhGuestByte = be<u8>;
struct daNh_c : fopAc_ac_c {
    enum Type { TYPE_BOTTLE = 1 };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    u8 _3AC[8];
    gptr<J3DModel> mpModel;   // 3B4
    dBgS_ObjAcch mAcch;       // 3B8
    dBgS_AcchCir mAcchCir;    // 57C
    dCcD_Stts mStts;          // 5BC
    dCcD_Cyl mCyl;            // 5F8
    NhBrkAnm mBrkAnm;         // 728
    cBgS_PolyInfo mPolyInfo;  // 7AC
    NhAction mCurrActionFunc; // 7BC
    be<f32> mGlowMtx[3][4];   // 7C4
    be<f32> mPlayerDist;      // 7F4
    be<f32> mGroundY;         // 7F8
    be<s32> mBottleTimer;     // 7FC
    be<s32> mAlpha;           // 800
    be<u8> unk68C;
    be<s8> mActionStatus;
    be<u8> mGlowAlpha;
    be<u8> mWobbleDir;
    be<u8> mWobbleTimer;
    be<u8> mType;
    u8 _80A[2];
    be<s16> mEscapeTimer;
    be<s16> unk696;
    be<s16> unk698;
    be<s16> unk69A;
    u8 _814[4];
    be<f32> unk6A0;
    u8 _81C[4];
    cXyz mLightPos; // 820
    be<u16> mLightRed;
    be<u16> mLightGreen;
    be<u16> mLightBlue;
    u8 _832[2];
    be<f32> mLightBlend;     // 834
    be<f32> mLightParameter; // 838
    be<s32> mLightId;        // 83C native registered-light slot ID
    be<f32> mLightIntensity; // 840

    BOOL init();
    BOOL execute();
    void BGCheck();
    void setBaseMtx();
    BOOL searchPlayer();
    BOOL checkBinCatch();
    BOOL waitAction(void *);
    BOOL checkEscapeEnd();
    BOOL escapeAction(void *);
    BOOL returnAction(void *);
    BOOL setAction(const NhAction *, void *);
    void action(void *);
    BOOL initBrkAnm(u32);
    BOOL createHeap();
    BOOL draw();
    BOOL checkTimer();
    void airMove();
    BOOL moveProc(f32, f32, u32);
    f32 getHomeDistance();
};
WWHD_SIZE(daNh_c, 0x844);
WWHD_OFFSET(daNh_c, mpModel, 0x3B4);
WWHD_OFFSET(daNh_c, mAcch, 0x3B8);
WWHD_OFFSET(daNh_c, mStts, 0x5BC);
WWHD_OFFSET(daNh_c, mCyl, 0x5F8);
WWHD_OFFSET(daNh_c, mBrkAnm, 0x728);
WWHD_OFFSET(daNh_c, mCurrActionFunc, 0x7BC);
WWHD_OFFSET(daNh_c, mLightPos, 0x820);

// HD: parameters precede the Disposer subobject rather than a JOR vtable.
struct daNh_HIO_c {
    be<s8> mNo;
    u8 _01[3];
    struct hio_prm_c {
        be<f32> field_0x08, field_0x0c, field_0x10;
        be<f32> mGlowOffsetY, mGlowScale, mMinFrightenSpeed;
        be<f32> mHeightAboveGround, mAscentSpeed, mDescentSpeed, mGravity;
        be<f32> mMaxHomeDist, field_0x34, mModelScale;
        be<s16> field_0x3c, mDefaultGlowAlpha, mBottleLifetime;
        u8 padding[2];
    } prm;
    gptr<daNh_c> mpActor;
    be<u32> disposerVtable;
};
WWHD_SIZE(daNh_HIO_c, 0x48);

WWHD_OFFSET(daNh_c, mLightParameter, 0x838);
WWHD_OFFSET(daNh_c, mLightId, 0x83C);

s32 nhCalcTimerInt(be<s32> *);
BOOL nhChaseAngleRegister(be<s16> *, u32, u32);
f32 nhSqrtCurrentFloat();
u32 nhPlayBrk(NhBrkAnm *);
void nhCutPointLight(cXyz *);
void *nhAllocate(u32);
void nhFree(void *);
void nhInitHeaderByte(NhGuestByte *);
void nhInitSecondHeaderByte(NhGuestByte *);
void nhRegisterGlobal(NhDtorRecord *);
BOOL nhInitBrk(NhBrkAnm *, J3DModelData *, void *, u32, u32, f32, u32, s32, u32, u32);
void nhEntryBrk(NhBrkAnm *, J3DModelData *, f32);
void nhConstructAcch(dBgS_ObjAcch *);
void nhConstructAcchCir(dBgS_AcchCir *);
void nhConstructStts(dCcD_Stts *);
void nhConstructGStts(be<u32> *);
void nhConstructGObjInf(dCcD_Cyl *);
void nhConstructCyl(cM3dGCyl *);
void nhConstructBrk(NhBrkAnm *);
void nhDestroyCylinder(dCcD_Cyl *, u32);
void nhDestroyStts(dCcD_Stts *, u32);
void nhDestroyAcchCircle(void *, u32);
void nhDestroyAcch(dBgS_ObjAcch *, u32);
void nhDestroyActorBase(daNh_c *, u32);
f32 nhAddCalc(be<f32> *, f32, f32, f32, f32);
u32 nhCalcTimerShort(be<s16> *);
u32 nhCalcTimerByte(be<u8> *);
void nhCatchRequest(NhCatch *, daNh_c *, u32, f32, f32, f32, s16, u32);
u32 nhGetRoomId(dBgS *, cBgS_PolyInfo *);
u32 nhGetPolyColor(dBgS *, cBgS_PolyInfo *);
void nhDestroyGroundCheck(dBgS_GndChk *, u32);
void nhTranslateMatrix(Mtx34 *, f32, f32, f32);
void nhRotateMatrixY(Mtx34 *, s16);
void nhScaleMatrix(f32, f32, f32);
void nhSetPointLight(cXyz *);
BOOL nhEntrySolidHeap(daNh_c *, u32, u32);
s32 nhCreateHioChild(const char *, daNh_HIO_c *);

void nhMoveActor(daNh_c *, dCcD_Stts *);
void nhSetCollision(cCcS *, dCcD_Cyl *);
dScnKy_env_light_c *nhGetEnvironmentLight();
f32 nhGroundCross(dBgS *, dBgS_GndChk *);
inline J3DModelData *nhModelData(J3DModel *model) {
    return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(model) + 0xAC));
}
inline be<u32> *nhCreationCondition(daNh_c *a) {
    return gabi::at<be<u32>>(gabi::ea(a) + offsetof(fopAc_ac_c, actor_condition));
}
inline J3DModel *nhActorModel(daNh_c *a) {
    return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(a) + offsetof(daNh_c, mpModel)));
}
inline be<u32> *nhCullMatrix(daNh_c *a) {
    return gabi::at<be<u32>>(gabi::ea(a) + offsetof(fopAc_ac_c, cullMtx));
}
WWHD_SIZE(NhBrkAnm, 0x84);
WWHD_SIZE(daNh_HIO_c::hio_prm_c, 0x3C);
WWHD_OFFSET(daNh_HIO_c, prm, 4);
WWHD_OFFSET(daNh_HIO_c, mpActor, 0x40);

// HD: compact8-byte action descriptors retain signed adjustment and virtual-slot dispatch.
inline void nhInvoke(daNh_c *actor, const NhAction &action, void *context) {
    const s16 adjustment = action.thisAdjustment;
    const s16 index = action.virtualIndex;
    void *receiver = gabi::at<void>(gabi::ea(actor) + s32(adjustment));
    u32 function;
    if (index < 0) {
        function = action.target;
    } else {
        const s16 tableOffset = s16(u32(action.target));
        const u32 table = gabi::load<u32>(gabi::ea(receiver) + s32(tableOffset));
        function = gabi::load<u32>(table + u32(s32(index)) * 8 + 4);
    }
    gabi::call_ptr(function, receiver, context);
}

inline cXyz *nhModelScale(J3DModel *m) {
    return gabi::at<cXyz>(gabi::ea(m) + offsetof(J3DModel, mBaseScale));
}
inline Mtx34 *nhModelMatrix(J3DModel *m) {
    return gabi::at<Mtx34>(gabi::ea(m) + offsetof(J3DModel, mBaseTransformMtx));
}
inline be<u32> *nhTevRegAnimator(J3DModelData *data) {
    return gabi::at<be<u32>>(gabi::ea(data) + offsetof(NhModelData, mTevRegAnimatorHD));
}
WWHD_OFFSET(NhBrkAnm, mFrame, 4);
WWHD_OFFSET(NhModelData, mTevRegAnimatorHD, 0x48);
WWHD_SIZE(NhShapeAttr, 0x20);
WWHD_OFFSET(NhShapeAttr, aabVtable, 0x18);
WWHD_OFFSET(NhShapeAttr, shapeVtable, 0x1C);
WWHD_OFFSET(NhAttentionView, mCatch, 0x130);

WWHD_OFFSET(daNh_c, mAcchCir, 0x57C);
WWHD_OFFSET(daNh_c, mPolyInfo, 0x7AC);
WWHD_OFFSET(daNh_c, mGlowMtx, 0x7C4);
WWHD_OFFSET(daNh_c, mPlayerDist, 0x7F4);
WWHD_OFFSET(daNh_c, mGroundY, 0x7F8);
WWHD_OFFSET(daNh_c, mBottleTimer, 0x7FC);
WWHD_OFFSET(daNh_c, mAlpha, 0x800);
WWHD_OFFSET(daNh_c, mActionStatus, 0x805);
WWHD_OFFSET(daNh_c, mType, 0x809);
WWHD_OFFSET(daNh_c, mEscapeTimer, 0x80C);
WWHD_OFFSET(daNh_c, mLightIntensity, 0x840);
WWHD_OFFSET(daNh_HIO_c, disposerVtable, 0x44);
