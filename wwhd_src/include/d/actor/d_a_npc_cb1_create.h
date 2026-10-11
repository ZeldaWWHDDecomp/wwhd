// Makar creation helpers. Shared actor layout and other parts remain unchanged.
// HD: HIO retains GameCube offsets; its embedded NPC HIO has a trailing vtable.
#pragma once
#include "d/actor/d_a_npc_cb1.h"
#include "gc_api/gc_api.h"
namespace cb1_create {
struct daNpc_Cb1_HIO_c {
    be<u32> vtable;
    /* 0x04 */ be<s8> mNo;
    u8 mPadding[3];
    /* 0x08 */ dNpc_HIO_l mNpc;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> mPlayerChaseDistance;
    /* 0x38 */ be<f32> mChaseDistScale;
    /* 0x3C */ be<f32> mMaxWalkSpeed;
    /* 0x40 */ be<f32> mMinWalkSpeed;
    /* 0x44 */ be<f32> mForwardAccel;
    /* 0x48 */ be<f32> mDecelScale;
    /* 0x4C */ be<f32> mMaxDecel;
    /* 0x50 */ be<f32> mDecel;
    /* 0x54 */ be<f32> mWalkAnmSpeedScale;
    /* 0x58 */ be<f32> mMaxWalkAnmSpeed;
    /* 0x5C */ be<f32> mNpcFlyLaunchSpeedF;
    /* 0x60 */ be<f32> mNpcFlyLaunchSpeedY;
    /* 0x64 */ be<f32> field_0x64;
    /* 0x68 */ be<f32> mHitSpeedScaleF;
    /* 0x6C */ be<f32> mHitSpeedScaleY;
    /* 0x70 */ be<f32> field_0x70;
    /* 0x74 */ be<f32> field_0x74;
    /* 0x78 */ be<f32> field_0x78;
    /* 0x7C */ be<f32> field_0x7C;
    /* 0x80 */ be<f32> field_0x80;
    /* 0x84 */ be<f32> field_0x84;
    /* 0x88 */ be<f32> mStickWalkSpeedScale;
    /* 0x8C */ be<f32> field_0x8C;
    /* 0x90 */ be<f32> field_0x90;
    /* 0x94 */ be<u8> field_0x94[4];
    /* 0x98 */ be<f32> field_0x98;
    /* 0x9C */ be<f32> field_0x9C;
    /* 0xA0 */ be<f32> field_0xA0;
    /* 0xA4 */ be<f32> field_0xA4;
    /* 0xA8 */ be<f32> field_0xA8;
    /* 0xAC */ be<f32> mStickFlySpeedScale;
    /* 0xB0 */ be<f32> field_0xB0;
    /* 0xB4 */ be<f32> field_0xB4;
    /* 0xB8 */ be<f32> field_0xB8;
    /* 0xBC */ be<f32> mFlyLaunchSpeedY;
    /* 0xC0 */ be<f32> field_0xC0;
    /* 0xC4 */ be<f32> field_0xC4;
    /* 0xC8 */ be<s16> field_0xC8;
    /* 0xCA */ be<s16> mPlayerFlyTimer;
    /* 0xCC */ be<s16> field_0xCC;
    /* 0xCE */ be<s16> field_0xCE;
    /* 0xD0 */ be<s16> field_0xD0;
    /* 0xD2 */ be<s16> field_0xD2;
    /* 0xD4 */ be<s16> field_0xD4;
    /* 0xD6 */ be<s16> field_0xD6;
    /* 0xD8 */ be<s16> field_0xD8;
    /* 0xDA */ be<s16> field_0xDA;
    /* 0xDC */ be<s16> mNpcFlyTimer;
    /* 0xDE */ be<s16> field_0xDE;
    /* 0xE0 */ be<s16> field_0xE0;
    /* 0xE2 */ be<s16> field_0xE2;
    /* 0xE4 */ be<s16> field_0xE4;
    /* 0xE6 */ be<s16> field_0xE6;
    /* 0xE8 */ be<s16> field_0xE8;
    /* 0xEA */ be<s16> field_0xEA;
    /* 0xEC */ be<s16> field_0xEC;
    /* 0xEE */ be<u8> mDamageTimer;
    /* 0xEF */ be<u8> field_0xEF;
};
WWHD_SIZE(daNpc_Cb1_HIO_c, 0xF0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpc, 8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDamageTimer, 0xEE);
} // namespace cb1_create

// HD: preserve every original call ABI, including compiler array helpers.
s32 CbCreate_drawPriority(fopAc_ac_c *actor);
void CbCreate_enqueueDraw(u32 tag, s32 priority);
void CbCreate_offsetPos(cXyz *out, const cXyz *base, s16 angle, const cXyz *offset);
void CbCreate_initFrameCtrl(u32 control, s32 end);
void CbCreate_constructMtxCalc(u32 control);
void CbCreate_concatMtx(const Mtx34 *a, const Mtx34 *b, Mtx34 *out);
void CbCreate_constructNpcHio(u32 hio);
void CbCreate_destroyCircle(u32 circle, s32 flags);
void CbCreate_musicStop(daNpc_Cb1_c *actor);
void CbCreate_destroyCylinder(dCcD_Cyl *cylinder, s32 flags);
void CbCreate_destroyStatus(dCcD_Stts *status, s32 flags);
void CbCreate_destroyArray(void *array, s32 count, s32 size, u32 destructor, s32 flags);
void CbCreate_destroyMtxCalc(u32 calculator, s32 flags);
void CbCreate_destroyNpc(daNpc_Cb1_c *actor, s32 flags);
BOOL CbCreate_createHeap(fopAc_ac_c *actor);
void CbCreate_drawDamageFog(daNpc_Cb1_c *actor);
BOOL CbCreate_isFlyAction(daNpc_Cb1_c *actor);
cPhs_State CbCreate_create(fopAc_ac_c *actor);
void CbCreate_rotateXYZ(Mtx34 *matrix, s16 x, s16 y, s16 z);
void CbCreate_constructNpc(daNpc_Cb1_c *actor);
void CbCreate_constructArray(void *array, s32 count, s32 size, u32 constructor);
void CbCreate_constructJointCtrl(dNpc_JntCtrl_c *joint);
void CbCreate_setRestartOption(u32 restart, s32 mode, u32 position, s16 angle, s8 room);
void CbCreate_setPriest(u32 priest, s32 mode, u32 position, s16 angle, s8 room);
void CbCreate_checkRestart(daNpc_Cb1_c *actor, s32 mode);
void CbCreate_upMtxBase(const cXyz *base, const cXyz *direction, Mtx34 *result);

struct CbCreateHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mHeaderA, mHeaderB;
    u8 mPadding[2];
    be<u32> mWords[4];
};
WWHD_SIZE(CbCreateHeaderStatics, 0x1C);
struct CbCreateGlobalRecords {
    be<u32> mWords[9];
};
WWHD_SIZE(CbCreateGlobalRecords, 0x24);
struct CbCreateEventNames {
    be<u32> entries[5];
};
WWHD_SIZE(CbCreateEventNames, 0x14);
struct CbCreateLinkView {
    u8 mPrefix[0x65F0];
    be<u32> mCurProc;
};
WWHD_OFFSET(CbCreateLinkView, mCurProc, 0x65F0);
WWHD_SIZE(CbCreateLinkView, 0x65F4);
// HD: only the ship fields used by this TU are exposed; the shared ship header has raw helpers.
struct CbCreateShipView : fopAc_ac_c {
    u8 mPrefix[0x3B8 - 0x3AC];
    gptr<mDoExt_McaMorf> mpHeadAnm;
    u8 mMiddle[0x644 - 0x3BC];
    be<u32> mStateFlag;
};
WWHD_OFFSET(CbCreateShipView, mpHeadAnm, 0x3B8);
WWHD_OFFSET(CbCreateShipView, mStateFlag, 0x644);
WWHD_SIZE(CbCreateShipView, 0x648);

// HD: constructor subobjects and vtables differ from the shared animation facade.
struct CbCreateBckView {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mMtxCalc[0x34];
    be<u32> mMtxCalcVtable;
    u8 mPadding[0xC];
    be<u32> mpAnm;
    u8 mPadding2[0x20];
    be<u32> mConstructorState[4];
};
WWHD_SIZE(CbCreateBckView, 0x8C);
WWHD_OFFSET(CbCreateBckView, mMtxCalcVtable, 0x48);
WWHD_OFFSET(CbCreateBckView, mpAnm, 0x58);
WWHD_OFFSET(CbCreateBckView, mConstructorState, 0x7C);
struct CbCreateJointData {
    u8 mPrefix[4];
    be<u16> mJointNo;
};
WWHD_SIZE(CbCreateJointData, 6);
WWHD_OFFSET(CbCreateJointData, mJointNo, 4);
// HD: word views retain signaling-NaN bits during non-arithmetic matrix/vector copies.
struct CbCreateWordMatrix {
    be<u32> m[3][4];
};
struct CbCreateWordVector {
    be<u32> x, y, z;
};
WWHD_SIZE(CbCreateWordMatrix, 0x30);
WWHD_SIZE(CbCreateWordVector, 0xC);
struct CbCreateSafeStringVtable {
    u8 mPrefix[0x14];
    be<u32> mAssureTermination;
};
WWHD_SIZE(CbCreateSafeStringVtable, 0x18);
WWHD_OFFSET(CbCreateSafeStringVtable, mAssureTermination, 0x14);
struct CbCreateRoomStatus {
    be<u8> mStatus;
    u8 mPadding[0x22B];
};
WWHD_SIZE(CbCreateRoomStatus, 0x22C);

struct CbCreateRoomStatuses {
    CbCreateRoomStatus mRooms[64];
};
WWHD_SIZE(CbCreateRoomStatuses, 64 * 0x22C);

namespace cb1_create {
// HD: per-TU vtables and GHS PTMF targets retain their original guest identities.
enum : u32 {
    BaseAnmVtable = 0x1016E54C,
    BckAnmVtable = 0x10018D24,
    MtxCalcVtable = 0x1016D820,
    HioVtable = 0x10018DDC,
    SafeStringVtable = 0x10018CBC,
    CylinderVtable = 0x10018CF4,
    PolyInfoVtable = 0x10018D04,
    WaitNpcAction = 0x02223048,
    SearchNpcAction = 0x022244A0,
    CarryNpcAction = 0x02223694,
    RescueNpcAction = 0x02224BC0,
    AcchCirConstructor = 0x024EFE94,
};
enum { daPyProc_GRAB_UP_e = 0x6F, daPyProc_GRAB_THROW_e = 0x71, daPyProc_GRAB_PUT_e = 0x72 };
enum {
    dRes_INDEX_CB_BDL_CB_e = 0x1D,
    dRes_INDEX_CB_BDL_CB_CELLO_e = 0x1E,
    dRes_INDEX_CB_BDL_CB_FACE_e = 0x1F,
    dRes_INDEX_CB_BDL_CB_NUT_e = 0x20,
    dRes_INDEX_CB_BDL_CB_STICK_e = 0x21,
    dRes_INDEX_CB_BDL_PP_e = 0x22,
    dRes_INDEX_CB_BCK_M_OPEN_e = 0x0E,
    dRes_INDEX_CB_BCK_NUT_SOW_e = 0x0F,
};
} // namespace cb1_create
