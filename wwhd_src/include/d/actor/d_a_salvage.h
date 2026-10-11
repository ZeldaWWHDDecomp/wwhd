/** Salvage crane rewards: tww actor members at HD +0x11C after its base.
 * HD keeps an eight-byte procedure descriptor, runtime HIO tuning and additional color updates. */
#pragma once
#include "d/d_salvage.h"
#include "gc_api/gc_api.h"
namespace salvage_actor {
// HD: immediate procedure addresses encode the compact GHS member pointer.
enum : u32 {
    HioVtable = 0x10039AA0,
    fpcSch_JudgeForPName = 0x025E121C,
    ActorVtable = 0x10039974,
    proc_wait = 0x02467364,
    proc_salvage = 0x02467CD4,
    proc_wait_init = 0x02465E9C,
    proc_salvage_init = 0x024679A0
};
enum {
    dRes_INDEX_YSLVG00_BDL_YSLVG00_e = 5,
    dRes_INDEX_YSLVG00_BRK_YSLVG00_e = 8,
    dRes_INDEX_YSLVG00_BTK_YSLVG00_e = 11,
    fpcNm_OQ_e = 0xE3,
    fpcNm_SBOX_e = 0x125,
    fpcNm_SALVAGE_TBOX_e = 0x192,
    fpcNm_DAIOCTA_e = 0xE1,
    MODE_CRANE_e = 10,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    JA_SE_OBJ_SALVAGE_TAG = 0x1069,
    JA_SE_OBJ_SALVAGE_NEAR = 0x10A9
};
struct Hio {
    be<u32> __vtbl;
    be<s8> mNo;
    u8 pad5[3];
    be<u32> mCallback;
    be<f32> mDistance, mEnemyDistance;
    be<u8> mUnused14, mUnused15, mDebugDraw, mDebugPrint, mForceEnabled, mTypeOverride,
        mKeepSalvage;
    u8 pad1b;
    be<f32> mMaxDistance, mVolumeNear, mOuterseaNear, mInsideNear, mCmapNear;
    be<u8> mDisappearFrame, mDebugRegister;
    u8 pad32[2];
};
WWHD_OFFSET(Hio, mForceEnabled, 0x18);
WWHD_OFFSET(Hio, mOuterseaNear, 0x24);
WWHD_SIZE(Hio, 0x34);
struct SaveLabels {
    be<u16> values[256];
};
struct Depths {
    be<f32> values[3];
};
struct VectorWords {
    be<u32> x, y, z;
};
struct Color {
    be<f32> rgba[4];
};
struct ColorBytes {
    be<u8> rgba[4];
};
struct Triangle {
    u8 bytes[0x38];
};
WWHD_SIZE(SaveLabels, 0x200);
WWHD_SIZE(Depths, 12);
WWHD_SIZE(VectorWords, 12);
WWHD_SIZE(Color, 16);
WWHD_SIZE(ColorBytes, 4);
WWHD_SIZE(Triangle, 0x38);
struct Ship : fopAc_ac_c {
    u8 pad3ac[0x635 - 0x3ac];
    be<u8> mCurMode, mNextMode;
    u8 pad637[0x644 - 0x637];
    be<u32> mStateFlag;
    u8 pad648[0x680 - 0x648];
    be<s16> m0398;
    u8 pad682[0x704 - 0x682];
    be<u32> mTornadoActor;
    u8 pad708[4];
    be<u32> mWhirlActor;
    u8 pad710[12];
    gptr<cXyz> mCraneTop;
};
WWHD_OFFSET(Ship, mStateFlag, 0x644);
WWHD_OFFSET(Ship, mCraneTop, 0x71c);
WWHD_SIZE(Ship, 0x720);
struct OtherShip : fopAc_ac_c {
    u8 pad3ac[0x3cd - 0x3ac];
    be<u8> mSwitchA;
};
WWHD_OFFSET(OtherShip, mSwitchA, 0x3cd);
WWHD_SIZE(OtherShip, 0x3ce);
struct Octopus : fopAc_ac_c {
    u8 pad3ac[0x690 - 0x3ac];
    be<u8> mSwitchNo;
};
WWHD_OFFSET(Octopus, mSwitchNo, 0x690);
WWHD_SIZE(Octopus, 0x691);
struct Emitter {
    u8 pad0[0x5c];
    be<s32> mMaxFrame;
    u8 pad60[0x1f0 - 0x60];
    Mtx34 mGlobalRotation;
    u8 pad220[12];
    cXyz mGlobalTranslation;
    u8 pad238[0x254 - 0x238];
    be<u32> mStatus;
    u8 pad258[10];
    be<u8> mPlane;
};
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22c);
WWHD_OFFSET(Emitter, mStatus, 0x254);
WWHD_SIZE(Emitter, 0x263);
// HD: nullable actor receivers still form guest base+position, including EA zero.
inline cXyz* actorPosition(fopAc_ac_c* actor) {
    return gabi::at<cXyz>(gabi::ea(actor) + offsetof(fopAc_ac_c, current) +
                          offsetof(actor_place, pos));
}
struct MaterialColor {
    be<u32> mFlags;
};
struct TevVtable {
    u8 pad0[0x3c];
    be<u32> setKColor;
    u8 pad40[12];
    be<u32> getKColor;
};
struct TevBlock {
    be<u32> unused;
    gptr<TevVtable> __vtbl;
};
struct Material {
    u8 pad0[0x18];
    gptr<TevBlock> mTevBlock;
    u8 pad1c[0xa0 - 0x1c];
    MaterialColor mColor;
    ColorBytes* getTevKColor(s32 index) {
        TevBlock* block = mTevBlock;
        TevVtable* table = block->__vtbl;
        return gabi::call_ptr<ColorBytes*>(table->getKColor, block, index);
    }
    void setTevKColor(s32 index, ColorBytes* color) {
        TevBlock* block = mTevBlock;
        TevVtable* table = block->__vtbl;
        gabi::call_ptr(table->setKColor, block, index, color);
    }
};
WWHD_OFFSET(TevVtable, getKColor, 0x4c);
WWHD_SIZE(TevVtable, 0x50);
WWHD_SIZE(TevBlock, 8);
WWHD_OFFSET(Material, mColor, 0xa0);
WWHD_SIZE(Material, 0xa4);
WWHD_SIZE(MaterialColor, 4);
inline Material* firstMaterial(J3DModelData* data) {
    return gabi::at<Material>(data->mMaterials);
}
} // namespace salvage_actor
struct daSalvage_c : fopAc_ac_c {
    BOOL proc_wait_init();
    BOOL proc_salvage_init();
    BOOL isEffectKind(s32 id);
    BOOL getDistance(s32 id, be<f32>* dist);
    void createEnemy();
    BOOL end_salvage();
    void eventOrder();
    BOOL _delete();
    s32 checkXZDistance();
    BOOL onSalvageForOship(void* other);
    s32 checkDistance();
    BOOL CreateInit();
    cPhs_State _create();
    void send_agb();
    void calcAlpha();
    BOOL checkArea(cXyz* crane, s32 id);
    BOOL _execute();
    void set_mtx(J3DModel* model, s32 id);
    BOOL CreateHeap();
    BOOL _draw();
    void checkOrder();
    BOOL proc_wait();
    BOOL proc_salvage();
    void setProc(BOOL (daSalvage_c::*next)()) {
        u32 target = next == &daSalvage_c::proc_wait        ? salvage_actor::proc_wait
                     : next == &daSalvage_c::proc_salvage   ? salvage_actor::proc_salvage
                     : next == &daSalvage_c::proc_wait_init ? salvage_actor::proc_wait_init
                                                            : salvage_actor::proc_salvage_init;
        mProcAdjustment = 0;
        mProcIndex = -1;
        mProc = target;
    }
    // HD compact PTMF: signed adjustment and table offset wrap in the 32-bit guest address space.
    void runProc() {
        s16 index = mProcIndex;
        s16 adjustment = mProcAdjustment;
        u32 adjusted = gabi::ea(this) + adjustment;
        if (index < 0)
            gabi::call_ptr((u32)mProc, adjusted);
        else {
            s16 off = (s16)((u32)mProc & 0xffff);
            u32 table = gabi::load<u32>(adjusted + off);
            gabi::call_ptr(gabi::load<u32>(table + index * 8 + 4), adjusted);
        }
    }
    /* 0x3ac */ request_of_phase_process_class mPhase;
    /* 0x3b4 */ gptr<J3DModelData> mModelData;
    /* 0x3b8 */ gptr<J3DModel> mModels[16];
    /* 0x3f8 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3fc */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x400 */ gptr<salvage_actor::Emitter> mEmitter;
    /* 0x404 */ be<s8> m2E8; // HD: mPreviousRoom
    /* 0x405 */ be<u8> m2E9, m2EA;
    u8 pad407;
    /* 0x408 */ be<u32> m2EC;
    /* 0x40c */ be<s16> mSalvageGetItemEventIdx, mSalvageHazureEventIdx, mSlvgGetItemLeftEventIdx,
        mEventIdx;
    /* 0x414 */ be<s8> mStayNo;
    u8 pad415[3];
    /* 0x418 */ be<s32> m2FC;
    /* 0x41c */ be<u8> mRndDepthIdx, m301, m302;
    u8 pad41f;
    /* 0x420 */ be<u32> m304;
    /* 0x424 */ be<s16> mProcAdjustment, mProcIndex;
    /* 0x428 */ be<u32> mProc;
};
WWHD_OFFSET(daSalvage_c, mEmitter, 0x400);
WWHD_OFFSET(daSalvage_c, mProc, 0x428);
WWHD_SIZE(daSalvage_c, 0x42c);
using SalvageCreateCallback = u32;
using SalvageHeapCallback = BOOL (*)(daSalvage_c*);
u32 salvageCreateActor(s32, u32, const cXyz*, s32, const csXyz*, const cXyz*, s32,
                       SalvageCreateCallback);
void salvageCompleteMap(dSv_player_map_c*, s32);
void salvageOnEventBit(dSv_event_c*, u32);
void salvageOnOceanBit(dSv_ocean_c*, u32, u32);
BOOL salvageIsEventBit(dSv_event_c*, u32);
BOOL salvageIsOceanBit(dSv_ocean_c*, u32, u32);
BOOL salvageIsSwitch(dSv_info_c*, s32, u32);
BOOL salvageIsOpenMap(dSv_player_map_c*, s32);
BOOL salvageIsCompleteMap(dSv_player_map_c*, s32);
BOOL salvageIsTriforceMap(dSv_player_map_c*, s32);
void salvageOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32);
void salvageActorDelete(fopAc_ac_c*, s32);
s16 salvageEventIdx(dEvent_manager_c*, const char*, u32);
salvage_actor::Emitter* salvageSetParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*,
                                           const cXyz*, u32, void*, s32, void*, void*, void*);
cPhs_State salvageResLoad(request_of_phase_process_class*, const char*);
BOOL salvageSolidHeap(fopAc_ac_c*, SalvageHeapCallback, u32);
void salvageSetGbaName(fopAc_ac_c*, u32, u32, u32);
void salvageDrawPoint(u32, f32, f32, f32, s32, s32, u32, u32, u32);
void salvageSubtract(const cXyz*, cXyz*, const cXyz*);
f32 salvageSquareMag(const cXyz*);
f32 salvageSqrt(f32);
f32 salvageWaterHeight(const cXyz*);
void salvageTriangle(salvage_actor::Triangle*, const cXyz*, const cXyz*, const cXyz*);
void salvageTriangleAngles(salvage_actor::Triangle*, be<s16>*, be<s16>*);
void salvageEmitterRotation(s32, s32, s32, Mtx34*);
void salvageRotateMatrix(Mtx34*, s32, s32, s32);
void* salvageObjectResource(dRes_control_c*, SafeString*, s32);
mDoExt_brkAnm* salvageCreateBrk(mDoExt_brkAnm*);
mDoExt_btkAnm* salvageCreateBtk(mDoExt_btkAnm*);
BOOL salvageInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, u32, s32, f32, s32, s32, u32,
                    u32);
BOOL salvageInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, u32, s32, f32, s32, s32,
                    u32, u32);
void salvageEntryBrk(mDoExt_brkAnm*, J3DModelData*, f32);
void salvageEntryBtk(mDoExt_btkAnm*, J3DModelData*, f32);
void salvagePlayAnimation(mDoExt_baseAnm*);
void salvageScaleColor(salvage_actor::Color*, salvage_actor::Color*, f32);
salvage_actor::Color* salvageMaterialColor(salvage_actor::MaterialColor*, u32);
BOOL salvageSearchActor(u32, be<u32>*);
void salvageDeleteActor(u32);
salvage_actor::Octopus* salvageFindActor(u32, be<u16>*);
BOOL salvageIsTriforce(u32);
s32 salvageDayNight();
s32 salvageMoonType();
BOOL salvageMoonArrival();
s32 salvageReverb(s32);
BOOL salvageEventStart(dEvent_manager_c*, s32);
BOOL salvageEventEnd(dEvent_manager_c*, s32);
void salvageGetPos(dSalvage_control_c*, cXyz*, s32);
void salvageGetScale(dSalvage_control_c*, cXyz*, s32);

BOOL cLib_chaseUC(be<u8>*, u32, u32);
