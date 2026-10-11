/* Crystal switch, GameCube member names on the WWHD layout. Base actor grows by 0x11C;
 * bck/btk animation wrappers grow to 0x8C/0x74, moving collision and state members by 0x1F8. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"

struct daSwhit0_c : fopAc_ac_c {
    s32 getSwNo();
    u8 getEvNo();
    u8 getType();
    s32 getTimer();
    s32 getSwNo2();
    BOOL CreateHeap();
    void decisionRtType();
    BOOL CreateInit();
    cPhs_State create();
    s32 checkHit();
    s32 DemoProc();
    s32 actionOffWait();
    s32 actionToOnReady();
    s32 actionToOnOrder();
    s32 actionToOnDemo();
    s32 actionOnWait();
    s32 actionOnTimer();
    void setDrawMtx();
    void onFlag(u16 f) { mFlags |= f; }
    void offFlag(u16 f) { mFlags &= (u16)~f; }
    s32 checkFlag(u16 f) { return mFlags & f; }

    /* 0x3AC */ gptr<J3DModel> mpModel;
    /* 0x3B0 */ mDoExt_bckAnm mAnm;
    /* 0x43C */ mDoExt_btkAnm mTexAnm;
    /* 0x4B0 */ dCcD_Stts mColStatus;
    /* 0x4EC */ dCcD_Cyl mColCyl;
    /* 0x61C */ dCcD_Sph mColSph;
    /* 0x748 */ be<u8> mHitTimer;
    /* 0x749 */ be<u8> mState;
    /* 0x74A */ be<u8> mTimer;
    /* 0x74B */ be<u8> mRetType;
    /* 0x74C */ be<s16> mOnTimer;
    /* 0x74E */ be<u16> mFlags;
    /* 0x750 */ be<s16> mEventIdx;
    /* 0x752 */ u8 _752[2];
    /* 0x754 */ be<s32> mStaffId;
};
WWHD_OFFSET(daSwhit0_c, mpModel, 0x3AC);
WWHD_OFFSET(daSwhit0_c, mAnm, 0x3B0);
WWHD_OFFSET(daSwhit0_c, mTexAnm, 0x43C);
WWHD_OFFSET(daSwhit0_c, mColStatus, 0x4B0);
WWHD_OFFSET(daSwhit0_c, mColCyl, 0x4EC);
WWHD_OFFSET(daSwhit0_c, mHitTimer, 0x748);
WWHD_OFFSET(daSwhit0_c, mState, 0x749);
WWHD_OFFSET(daSwhit0_c, mTimer, 0x74A);
WWHD_OFFSET(daSwhit0_c, mRetType, 0x74B);
WWHD_OFFSET(daSwhit0_c, mOnTimer, 0x74C);
WWHD_OFFSET(daSwhit0_c, mFlags, 0x74E);
WWHD_OFFSET(daSwhit0_c, mEventIdx, 0x750);
WWHD_OFFSET(daSwhit0_c, mColSph, 0x61C);
WWHD_OFFSET(daSwhit0_c, mStaffId, 0x754);
WWHD_SIZE(daSwhit0_c, 0x758);

enum {
    dRes_INDEX_ALWAYS_BCK_OBM_SYOUGEKISW_e = 0x0D,
    dRes_INDEX_ALWAYS_BDL_OBM_SYOUGEKISW_e = 0x35,
    dRes_INDEX_ALWAYS_BTK_OBM_SYOUGEKISW_e = 0x58,
};
enum { JA_SE_SHOCK_SW_ON = 0x083E, JA_SE_OBJ_COL_SWC_NSTONE = 0x6820 };
enum SwitchState { OFF_WAIT, TO_ON_READY, TO_ON_ORDER, TO_ON_DEMO, ON_WAIT, ON_TIMER };
enum SwitchFlag { SWITCH_ON = 1, COLLISION_CYLINDER = 2 };
enum SwitchType { TYPE_DIRECT, TYPE_EVENT_DELAY, TYPE_EXTERNAL_RESET, TYPE_EVENT_NOSOUND };

// HD: the enlarged inline bck animation embeds additional cache/subobjects.
struct SwhitBckAnm {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ be<u32> mVtable;
    /* 0x14 */ u8 mTransformCache[0x34];
    /* 0x48 */ be<u32> mCacheVtable;
    /* 0x4C */ u8 _4C[0xC];
    /* 0x58 */ be<u32> mCacheData;
    /* 0x5C */ u8 _5C[0x20];
    /* 0x7C */ be<u32> mCacheCount;
    /* 0x80 */ be<u32> mData;
    /* 0x84 */ be<u32> mResource;
    /* 0x88 */ be<u32> mFlags;
};
WWHD_OFFSET(SwhitBckAnm, mTransformCache, 0x14);
WWHD_OFFSET(SwhitBckAnm, mCacheVtable, 0x48);
WWHD_OFFSET(SwhitBckAnm, mCacheData, 0x58);
WWHD_OFFSET(SwhitBckAnm, mCacheCount, 0x7C);
WWHD_OFFSET(SwhitBckAnm, mData, 0x80);
WWHD_OFFSET(SwhitBckAnm, mResource, 0x84);
WWHD_OFFSET(SwhitBckAnm, mFlags, 0x88);
WWHD_SIZE(SwhitBckAnm, 0x8C);
struct SwhitColor {
    be<s16> rgba[4];
};
WWHD_SIZE(SwhitColor, 8);
struct SwhitFloatColor {
    be<f32> rgba[4];
};
WWHD_SIZE(SwhitFloatColor, 16);
struct SwhitTevVtable {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ gfn setTevColor;
    /* 0x28 */ u8 _28[0xC];
    /* 0x34 */ gfn getTevColor;
};
WWHD_OFFSET(SwhitTevVtable, setTevColor, 0x24);
WWHD_OFFSET(SwhitTevVtable, getTevColor, 0x34);
WWHD_SIZE(SwhitTevVtable, 0x38);
struct SwhitTevBlock {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ gptr<SwhitTevVtable> mVtable;
};
WWHD_OFFSET(SwhitTevBlock, mVtable, 4);
WWHD_SIZE(SwhitTevBlock, 8);
struct SwhitMaterial {
    /* 0x00 */ u8 _00[0x18];
    /* 0x18 */ gptr<SwhitTevBlock> mTevBlock;
    /* 0x1C */ u8 _1C[0x84];
    /* 0xA0 */ be<u32> mFlags;
};
WWHD_OFFSET(SwhitMaterial, mTevBlock, 0x18);
WWHD_OFFSET(SwhitMaterial, mFlags, 0xA0);
WWHD_SIZE(SwhitMaterial, 0xA4);
struct SwhitMaterialTable {
    /* 0x00 */ u8 _00[0x24];
    /* 0x24 */ be<u16> mMaterialNum;
};
WWHD_OFFSET(SwhitMaterialTable, mMaterialNum, 0x24);
WWHD_SIZE(SwhitMaterialTable, 0x26);

u8 swhitCalcTimer(be<u8> *);
void swhitInitTransformCache(void *);
void swhitConstructSphere(dCcD_Sph *);
void swhitDestroySphere(dCcD_Sph *, s32);
void swhitDestroyBckCache(void *, s32);
void swhitDestroyActor(fopAc_ac_c *, s32);
void swhitConvertColor(SwhitFloatColor *, SwhitFloatColor *, f32);
SwhitFloatColor *swhitGetColorSlot(be<u32> *, s32);
SwhitMaterialTable *swhitGetMaterialTable(J3DModelData *);
void swhitXYZrotM(Mtx34 *, s16, s16, s16);

// HD: this TU's inline constructor vtables.
enum : u32 {
    SWHIT_ACTOR_VTABLE = 0x1003EB44,
    SWHIT_ANM_BASE_VTABLE = 0x1016E54C,
    SWHIT_BCK_VTABLE = 0x1003EB1C,
    SWHIT_TRANSFORM_CACHE_VTABLE = 0x1016D820,
    SWHIT_AAB_VTABLE = 0x1003EB0C,
};
