// d_a_dai_item.h (WWHD) - stand item actor layout.
#pragma once
#include "gc_api/gc_api.h"
struct StandItemEmitterHD;
struct StandItemClothHD;
struct StandItemBckCtorHD;
struct daStandItem_c : fopAc_ac_c {
    enum Mode { MODE_CARRY, MODE_WAIT, MODE_DROP };
    // HD: retain the verified full register return widths on these entries.
    BOOL _delete();
    BOOL CreateHeap();
    void CreateInit();
    s32 _create();
    void set_mtx();
    BOOL _execute();
    void itemProc();
    BOOL actionFobj00();
    BOOL actionFobj01();
    BOOL actionFobj02();
    BOOL actionFobj03();
    BOOL actionFobj04();
    BOOL actionFobj05();
    BOOL actionFobj06();
    BOOL actionFobj07();
    BOOL actionFobj08();
    BOOL actionFobj09();
    BOOL actionFobj10();
    BOOL actionFobj11();
    void animTest();
    void animTestForOneTime();
    void execAction();
    void mode_carry_init();
    void mode_carry();
    void mode_wait_init();
    void mode_wait();
    void mode_drop_init();
    void mode_drop();
    BOOL _draw();

    request_of_phase_process_class mPhsDai, mPhsCloth;
    gptr<J3DModel> mpModel;
    gptr<StandItemBckCtorHD> mpBckAnm;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mAcchCir;
    be<u8> mItemNo, mItemType;
    u8 m61A[2];
    be<s32> mTimer, mMode;
    be<u8> mCarry;
    u8 m625[3];
    be<s16> mBckPlayTimer, mBckStopTimer;
    be<f32> mBckSpeed;
    Mtx34 m630, m660;
    gptr<StandItemEmitterHD> m690, m694, m698;
    gptr<StandItemClothHD> mpCloth;
    be<u8> mClothType;
    u8 m6A1;
    be<s16> m6A2;
    cXyz m6A4;
    be<u8> mbBckDidPlay, m6B1;
    be<s16> m6B2, m6B4;
    u8 m6B6[6];
    be<f32> m6BC;
    u8 m6C0[4];
    be<f32> m6C4;
};
WWHD_OFFSET(daStandItem_c, mPhsDai, 0x3AC);
WWHD_OFFSET(daStandItem_c, mpModel, 0x3BC);
WWHD_OFFSET(daStandItem_c, mStts, 0x3C4);
WWHD_OFFSET(daStandItem_c, mCyl, 0x400);
WWHD_OFFSET(daStandItem_c, mAcch, 0x530);
WWHD_OFFSET(daStandItem_c, mAcchCir, 0x6F4);
WWHD_OFFSET(daStandItem_c, mClothType, 0x7BC);
WWHD_OFFSET(daStandItem_c, m6A4, 0x7C0);
WWHD_OFFSET(daStandItem_c, mItemNo, 0x734);
WWHD_OFFSET(daStandItem_c, mMode, 0x73C);
WWHD_OFFSET(daStandItem_c, mBckPlayTimer, 0x744);
WWHD_OFFSET(daStandItem_c, mbBckDidPlay, 0x7CC);
WWHD_OFFSET(daStandItem_c, m6C4, 0x7E0);
WWHD_SIZE(daStandItem_c, 0x7E4);

// HD: views below describe only the fields used by this unit; no shared padding
// is changed.
enum : u32 { EMITTER_INVALID = 1, EMITTER_IMMORTAL = 0x40, STAND_ITEM_NODE_CALLBACK = 0x02526580 };
struct StandItemPointWordsHD {
    be<u32> x, y, z;
};
WWHD_SIZE(StandItemPointWordsHD, 12);
inline be<u32> &standItemWord(be<f32> *field) { return *reinterpret_cast<be<u32> *>(field); }
struct StandItemCtorHD {
    u8 mPadding0[0x510];
    be<u32> mCylShapePad, mCylShapeVtable;
    u8 mPadding518[0x540 - 0x518];
    be<u32> mAcchVtable, mAcchPolyVtable;
    be<u8> mAcchObjThrough;
    u8 mPadding549[7];
    be<u32> mAcchGroupVtable;
};
WWHD_OFFSET(StandItemCtorHD, mAcchGroupVtable, 0x550);
inline StandItemCtorHD *standItemCtor(daStandItem_c *actor) {
    return reinterpret_cast<StandItemCtorHD *>(actor);
}
struct StandItemMethodHD {
    be<s16> mThisAdjustment, mVtableIndex;
    be<u32> mTarget;
};
WWHD_SIZE(StandItemMethodHD, 8);
struct StandItemClothVtableHD {
    u8 mPadding0[0x3C];
    be<u32> mMove;
    u8 mPadding40[4];
    be<u32> mDraw;
};
struct StandItemClothHD {
    u8 mPadding0[12];
    gptr<StandItemClothVtableHD> __vtbl;
};
WWHD_OFFSET(StandItemClothHD, __vtbl, 12);
struct StandItemEmitterHD {
    u8 mPadding0[0x5C];
    be<s32> mMaxFrame;
    u8 mPadding60[0x1F0 - 0x60];
    u8 mGlobalRotation[0x22C - 0x1F0];
    cXyz mGlobalTranslation;
    u8 mPadding238[0x254 - 0x238];
    be<u32> mFlags;
};
WWHD_OFFSET(StandItemEmitterHD, mFlags, 0x254);
struct StandItemBckCtorHD {
    be<f32> mRate, mFrame;
    u8 mPadding8[8];
    be<u32> __vtbl;
    u8 mFrameCtrl[0x48 - 0x14];
    be<u32> mFrameCtrlVtable;
    u8 mPadding4C[12];
    be<u32> mAnimation58;
    u8 mPadding5C[0x7C - 0x5C];
    be<u32> mAnimation7C, mAnimation80, mAnimation84, mAnimation88;
};
WWHD_SIZE(StandItemBckCtorHD, 0x8C);
struct StandItemTablesHD {
    be<s16> m_bmdidx[12], m_bckidx[12];
    be<u16> m_heapsize[12];
    be<s16> m_anim_min_time[12], m_anim_max_time[12], m_stop_min_time[12], m_stop_max_time[12];
};
WWHD_SIZE(StandItemTablesHD, 0xA8);
inline const StandItemTablesHD *standItemTables() {
    return gabi::at<StandItemTablesHD>(0x1000CE30);
}
struct StandItemJointHD {
    u8 mPadding0[0x14];
    be<u32> mBckAnm;
};
struct StandItemJointNameHD {
    u8 mPadding0[0x10];
    be<u32> mNameTableOffset;
};
struct StandItemPlayHD {
    u8 mPadding0[0x5AB0];
    gptr<u8> mpParticle;
    u8 mPadding5AB4[0x5B34 - 0x5AB4];
    gptr<fopAc_ac_c> mpPlayer;
};
WWHD_OFFSET(StandItemPlayHD, mpPlayer, 0x5B34);
struct StandItemHeaderStaticsHD {
    be<f32> mMinimum, mMaximum;
    be<u8> mOne, mZero;
    u8 mPaddingA[2];
    be<u32> mCounterC, mCounter10, mCounter14, mCounter18;
};
WWHD_SIZE(StandItemHeaderStaticsHD, 0x1C);
inline StandItemHeaderStaticsHD *standItemHeaderStatics() {
    return gabi::at<StandItemHeaderStaticsHD>(0x10462FAC);
}

void standItemSttsConstruct(void *);
void standItemAddCalc(be<f32> *, f32, f32, f32, f32);
void standItemAddCalcPos(cXyz *, const cXyz *, f32, f32);
void standItemAddCalcAngle(be<s16> *, s32, s32, s32);
void standItemDistanceAngle(s32, s32);
void standItemCircleDestroy(void *, s32);
void standItemCylinderConstruct(void *);
void standItemVectorAdd(const cXyz *, cXyz *, const cXyz *);
void standItemVectorSubtract(const cXyz *, cXyz *, const cXyz *);
void standItemAcchDestroy(void *, s32);
void standItemAcchCircleConstruct(void *);
void standItemAcchCircleSetWall(void *, f32, f32);
void standItemAcchConstruct(void *);
void standItemAcchSet(void *, cXyz *, cXyz *, fopAc_ac_c *, s32, void *, cXyz *, void *, void *);
void standItemAcchCorrectPosition(void *, void *);
void standItemSttsDestroy(void *, s32);
void standItemCollisionDestroy(void *, s32);
void standItemGSttsConstruct(void *);
void standItemCollisionConstruct(void *);
void standItemClothSetMatrix(void *, const void *);
void standItemResourceDelete(void *, const char *);
void standItemSpecularMap(const cXyz *, void *, void *, f32);
void standItemSettingTevStruct(void *, u32, const cXyz *, void *);
void standItemSetLightTevColor(void *, void *, void *);
void standItemAllWind(cXyz *, const cXyz *);
void standItemActorConstruct(fopAc_ac_c *);
void standItemActorDestroy(fopAc_ac_c *, s32);
void standItemCullBox(fopAc_ac_c *, f32, f32, f32, f32, f32, f32);
void standItemPositionMove(fopAc_ac_c *, void *);
void standItemSoundStart(u32, const cXyz *, u32, s32);
void standItemModelUpdate(void *, u32);
BOOL standItemAnimationPlay(void *);
void standItemAnimationEntry(void *, void *, f32);
void standItemMatrixZXYRotate(void *, s32, s32, s32);
void standItemMatrixYRotate(void *, s32);
void standItemMatrixTranslate(f32, f32, f32);
void standItemPanic(const char *, s32, const char *);
void standItemDelete(void *);
void standItemFrameCtrlConstruct(void *);
void standItemAnimationConstruct(void *, u32);
void standItemModelCalc(void *);
void standItemEmitterMatrix(const Mtx34 *, void *, void *);
void standItemMatrixTranslation(void *, f32, f32, f32);
void standItemZeroByteInitialize(void *);
void standItemOneByteInitialize(void *);
void standItemInitializeLock(void *);
BOOL standItemSolidHeap(fopAc_ac_c *, void *, u32);
BOOL standItemAnimationInit(void *, void *, void *, u32, u32, f32, s32, s32, u32);
f32 standItemRandomFX(f32);
f32 standItemWindPower();
f32 standItemSquareMagnitude(const cXyz *);
f32 standItemSquareRoot(f32);
s16 standItemTargetAngle(const cXyz *, const cXyz *);
s16 standItemAtan2s(f32, f32);
s32 standItemResourceLoad(void *, const char *);
s32 standItemReverb(s32);
s32 standItemJointIndex(void *, const char *);
u32 standItemWindVector();
u32 standItemParticleSet(void *, u32, u32, const cXyz *, const csXyz *, void *, u32, void *, s32,
                         const void *, const void *, u32);
u32 standItemModelCreate(void *, u32, u32);
u32 standItemNew(u32);
u32 standItemJointNameTable(void *);

// HD: guest vtables and eight-byte dispatch targets retain this TU's addresses;
// host function/member pointers cannot represent these GHS values.
enum : u32 {
    STAND_ITEM_HEAP_CALLBACK = 0x02116E28,
    STAND_ITEM_VTABLE = 0x1000CD1C,
    STAND_ITEM_STTS_VTABLE = 0x1004AE88,
    STAND_ITEM_GSTTS_VTABLE = 0x1004AEC0,
    STAND_ITEM_CYL_SHAPE_BASE_VTABLE = 0x100015A8,
    STAND_ITEM_CYL_SHAPE_VTABLE = 0x1000CCB4,
    STAND_ITEM_CYL_HIT_VTABLE = 0x1004B108,
    STAND_ITEM_CYL_SHAPE_FINAL_VTABLE = 0x1004B160,
    STAND_ITEM_CYL_VTABLE = 0x1004B150,
    STAND_ITEM_ACCH_VTABLE = 0x1000CCEC,
    STAND_ITEM_ACCH_GROUP_VTABLE = 0x1000CCFC,
    STAND_ITEM_ACCH_POLY_VTABLE = 0x1000CD0C,
    STAND_ITEM_ANIMATION_BASE_VTABLE = 0x1016E54C,
    STAND_ITEM_FRAME_CTRL_VTABLE = 0x1016D820,
    STAND_ITEM_ANIMATION_VTABLE = 0x1000CCC4,
    STAND_ITEM_CLOTH03_FACTORY = 0x0251BFA4,
    STAND_ITEM_CLOTH04_FACTORY = 0x0251C134,
    STAND_ITEM_CLOTH05_FACTORY = 0x0251C2D0,
    STAND_ITEM_CLOTH07_FACTORY = 0x0251C478,
};

WWHD_OFFSET(StandItemCtorHD, mCylShapePad, 0x510);
WWHD_OFFSET(StandItemCtorHD, mCylShapeVtable, 0x514);
WWHD_OFFSET(StandItemCtorHD, mAcchVtable, 0x540);
WWHD_OFFSET(StandItemCtorHD, mAcchPolyVtable, 0x544);
WWHD_OFFSET(StandItemCtorHD, mAcchObjThrough, 0x548);
WWHD_OFFSET(StandItemMethodHD, mVtableIndex, 0x2);
WWHD_OFFSET(StandItemMethodHD, mTarget, 0x4);
WWHD_OFFSET(StandItemClothVtableHD, mMove, 0x3C);
WWHD_OFFSET(StandItemClothVtableHD, mDraw, 0x44);
WWHD_OFFSET(StandItemEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(StandItemEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(StandItemEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(StandItemBckCtorHD, mRate, 0x0);
WWHD_OFFSET(StandItemBckCtorHD, mFrame, 0x4);
WWHD_OFFSET(StandItemBckCtorHD, __vtbl, 0x10);
WWHD_OFFSET(StandItemBckCtorHD, mFrameCtrl, 0x14);
WWHD_OFFSET(StandItemBckCtorHD, mFrameCtrlVtable, 0x48);
WWHD_OFFSET(StandItemBckCtorHD, mAnimation58, 0x58);
WWHD_OFFSET(StandItemBckCtorHD, mAnimation7C, 0x7C);
WWHD_OFFSET(StandItemJointHD, mBckAnm, 0x14);
WWHD_OFFSET(StandItemJointNameHD, mNameTableOffset, 0x10);
WWHD_OFFSET(StandItemPlayHD, mpParticle, 0x5AB0);
