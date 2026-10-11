/** Ballad of Gales tornado.
 * HD: enlarged animation wrappers and actor-owned eleven-joint matrix block.
 */
#pragma once
#include "gc_api/gc_api.h"

enum { TORNADO_VTBL = 0x10040964, BCK_TU_VTBL = 0x1004093C };

enum {
    dRes_INDEX_TRND_BCK_YTRND00_e = 6,
    dRes_INDEX_TRND_BCK_YWUWT00_e = 7,
    dRes_INDEX_TRND_BDL_YTRND00_e = 0xA,
    dRes_INDEX_TRND_BDL_YWUWT00_e = 0xB,
    dRes_INDEX_TRND_BRK_YTRND00_e = 0xE,
    dRes_INDEX_TRND_BRK_YWUWT00_e = 0xF,
    dRes_INDEX_TRND_BTK_YTRND00_e = 0x12,
    dRes_INDEX_TRND_BTK_YWUWT00_e = 0x13,
};
enum { TEV_TYPE_BG1 = 2 };
enum { JA_SE_OBJ_TORNADE_SUS = 0x106B };
enum { dPa_name_ID_AK_SN_TORNADOWIND00 = 0x8213, dPa_name_ID_AK_SN_WINDUPWATER00 = 0x81BB };

enum { daPyStts0_SHIP_RIDE_e = 0x10000 };
// HD: the embedded transform calculator and its TU-specific vtable.
struct TornadoBckView {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mTransformCalculator[0x48 - 0x14];
    be<u32> mCalculatorVtable;
    u8 mCalculatorTail[0x58 - 0x4C];
    be<u32> mModelState;
    u8 mAnimationState[0x7C - 0x5C];
    be<u32> mState[4];
};
WWHD_OFFSET(TornadoBckView, mVtable, 0x10);
WWHD_OFFSET(TornadoBckView, mCalculatorVtable, 0x48);
WWHD_OFFSET(TornadoBckView, mModelState, 0x58);
WWHD_OFFSET(TornadoBckView, mState, 0x7C);
WWHD_SIZE(TornadoBckView, 0x8C);
inline void tornadoConstructBck(mDoExt_bckAnm* b) {
    auto* p = reinterpret_cast<TornadoBckView*>(b);
    p->mFrameCtrl.init(0);
    p->mVtable = 0x1016E54C;
    gabi::call(0x027DA984, p->mTransformCalculator);
    p->mState[2] = 0;
    p->mState[1] = 0;
    p->mModelState = 0;
    p->mVtable = BCK_TU_VTBL;
    p->mCalculatorVtable = 0x1016D820;
    p->mState[3] = 0;
    p->mState[0] = 0;
}
inline void tornadoDestroyBck(mDoExt_bckAnm* b) {
    auto* p = reinterpret_cast<TornadoBckView*>(b);
    gabi::call(0x027F3628, &p->mVtable, 0);
}
struct TornadoAnmVtable {
    u8 mPrefix[0x14];
    be<u32> mGetFrameMax;
};
struct TornadoAnmView {
    u8 mPrefix[4];
    gptr<TornadoAnmVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->mGetFrameMax, this); }
};
struct TornadoBtkView {
    u8 mPrefix[0x68];
    gptr<TornadoAnmView> mpAnm;
};
WWHD_OFFSET(TornadoBtkView, mpAnm, 0x68);
/* 027F58E0: HD J3DModel helper called after creating the model: stores the argument at +0xA0 of
 * each of the model's 0xAC-byte entries (count +0x128, array +0x12C) */
static inline void tornadoSetModelEntryFlags(J3DModel* m, u32 v) { gabi::call(0x027F58E0, m, v); }
/* 0201B31C cXyz::normalize(this, out): normalizes in place and copies the result to out */
static inline void cXyz_normalize(cXyz* a, cXyz* res) { gabi::call(0x0201B31C, a, res); }
/* 0257E77C dKyw_tornado_Notice(cXyz*) */
static inline void dKyw_tornado_Notice(cXyz* pos) { gabi::call(0x0257E77C, pos); }
/* 025602F0 dKy_get_seacolor(GXColor* amb, GXColor* dif) */
static inline void dKy_get_seacolor(GXColor* amb, GXColor* dif) {
    gabi::call(0x025602F0, amb, dif);
}
/* 028E9108 PSMTXConcat, 028E945C PSMTXScale */
static inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) {
    gabi::call(0x028E9108, a, b, ab);
}
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
struct TornadoShipView : fopAc_ac_c {
    u8 mShipState[0x700 - 0x3AC];
    be<s32> mTornadoID;
    gptr<fopAc_ac_c> mTornadoActor;
    void offTornadoFlg() {
        mTornadoID = -1;
        mTornadoActor = nullptr;
    }
};
WWHD_OFFSET(TornadoShipView, mTornadoID, 0x700);
WWHD_OFFSET(TornadoShipView, mTornadoActor, 0x704);
WWHD_SIZE(TornadoShipView, 0x708);
inline TornadoShipView* tornadoShip() {
    return static_cast<TornadoShipView*>(dComIfGp_getShipActor());
}

struct daTornado_c : fopAc_ac_c {
    BOOL jointCallBack(int);
    BOOL draw();
    BOOL execute();
    BOOL tornado_delete();
    BOOL createHeap();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_bckAnm mBck;
    /* 0x444 */ mDoExt_btkAnm mBtk; /* anm at +0x68 */
    /* 0x4B8 */ mDoExt_brkAnm mBrk;
    /* 0x530 */ gptr<J3DModel> mpModelUnder;
    /* 0x534 */ mDoExt_bckAnm mBckUnder;
    /* 0x5C0 */ mDoExt_btkAnm mBtkUnder;
    /* 0x634 */ mDoExt_brkAnm mBrkUnder;
    /* 0x6AC */ be<s16> mAngle1;
    /* 0x6AE */ be<s16> mAngle2;
    /* 0x6B0 */ be<s16> m31c;
    /* 0x6B2 */ be<s16> mPtclTimer;
    /* 0x6B4 */ be<f32> mBtkFrame;
    /* 0x6B8 */ be<f32> mBtkUnderFrame;
    /* 0x6BC */ be<f32> mBrkFrame;
    /* 0x6C0 */ be<f32> m32c;
    /* 0x6C4 */ be<f32> mJointX[11];
    /* 0x6F0 */ be<f32> mJointZ[11];
    /* 0x71C */ be<f32> mJointScale[11];
    /* 0x748 */ cXyz mCenter;
    /* 0x754 */ dPa_followEcallBack mPtclCb;
    /* 0x768 */ Mtx34 mJointMtx[11]; // HD: actor-owned joint matrices.

    TornadoAnmView* btkAnm() { return reinterpret_cast<TornadoBtkView*>(&mBtk)->mpAnm; }
    TornadoAnmView* btkUnderAnm() { return reinterpret_cast<TornadoBtkView*>(&mBtkUnder)->mpAnm; }
    TornadoAnmView* brkAnm() { return reinterpret_cast<TornadoAnmView*>(mBrk.mpAnm.get()); }
};
WWHD_OFFSET(daTornado_c, mBtk, 0x444);
WWHD_OFFSET(daTornado_c, mpModelUnder, 0x530);
WWHD_OFFSET(daTornado_c, mAngle1, 0x6AC);
WWHD_OFFSET(daTornado_c, mJointScale, 0x71C);
WWHD_OFFSET(daTornado_c, mCenter, 0x748);
WWHD_OFFSET(daTornado_c, mJointMtx, 0x768);
WWHD_SIZE(daTornado_c, 0x978);

struct TornadoStaticScales {
    cXyz wind;
    cXyz small;
    cXyz underSmall;
    cXyz under;
    be<u32> windGuard;
    be<u32> smallGuard;
    be<u32> underSmallGuard;
    be<u32> underGuard;
};
WWHD_OFFSET(TornadoStaticScales, windGuard, 0x30);
WWHD_SIZE(TornadoStaticScales, 0x40);
inline TornadoStaticScales& tornadoScales() { return *gabi::at<TornadoStaticScales>(0x1046E9EC); }
struct TornadoPositionWords {
    be<u32> x, y, z;
};
inline TornadoPositionWords& tornadoWords(cXyz& p) {
    return *reinterpret_cast<TornadoPositionWords*>(&p);
}
inline void tornadoSetAttention(fopAc_ac_c* actor) {
    auto& src = tornadoWords(actor->current.pos);
    u32 x = src.x, y = src.y, z = src.z;
    auto& attention = tornadoWords(actor->attention_info.position);
    attention.x = x;
    attention.y = y;
    attention.z = z;
    auto& eye = tornadoWords(actor->eyePos);
    eye.x = x;
    eye.y = y;
    eye.z = z;
}
struct TornadoJointView {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(TornadoJointView, mJntNo, 4);
struct TornadoColorView {
    be<u8> r, g, b, a;
};
struct TornadoEmitterView {
    u8 mPrefix[0x244];
    TornadoColorView mGlobalPrmColor;
};
WWHD_OFFSET(TornadoEmitterView, mGlobalPrmColor, 0x244);
inline void tornadoSetScale(J3DModel* model, const cXyz& scale) {
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
}
inline void tornadoInitScale(be<u32>& guard, cXyz& scale, f32 x, f32 y, f32 z) {
    if (guard == 0) {
        scale.x = x;
        scale.y = y;
        scale.z = z;
        guard = 1;
    }
}

WWHD_SIZE(TornadoPositionWords, 0xC);
WWHD_SIZE(TornadoColorView, 4);
WWHD_OFFSET(daTornado_c, mBrk, 0x4B8);
WWHD_OFFSET(daTornado_c, mPtclCb, 0x754);
