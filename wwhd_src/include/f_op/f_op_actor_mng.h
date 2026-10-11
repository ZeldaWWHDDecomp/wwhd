/* Actor manager (fopAcM), WWHD layouts and native interfaces.
 * Full-register native interfaces
 * preserve verified widths where the GameCube declaration uses narrower integers. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_node.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"

void actMngGuest_JUT_ASSERT(u32 file, s32 line, u32 msg);
void actMngGuest_JUT_CONFIRM(u32 file, s32 line, u32 msg);
u32 actMngGuest_fopScnM_SearchByID(u32 id);
BOOL actMngGuest_fpcBs_Is_JustOfType(s32 a, s32 b);
void actMngGuest_fpcPi_Change(u32 pi, u32 layer, u32 listId, u32 listPrio);
BOOL actMngGuest_fpcM_IsCreating(u32 id);
u32 actMngGuest_fopAcIt_Judge(u32 fn, u32 data);
u32 actMngGuest_cMl_memalignB(s32 align, u32 size);
void actMngGuest_cLib_memSet(u32 p, s32 v, u32 n);
s32 actMngGuest_fpcM_Delete(u32 a);
u32 actMngGuest_fpcLy_CurrentLayer();
u32 actMngGuest_fpcSCtRq_Request(u32 layer, u32 name, u32 fn, u32 data, u32 append);
u32 actMngGuest_fpcM_FastCreate(u32 name, u32 fn, u32 data, u32 append);
u32 actMngGuest_dStage_searchName(u32 name);
u32 actMngGuest_dStage_getName(s16 procName, s8 arg);
u32 actMngGuest_mDoExt_createSolidHeapFromGameToCurrent(u32 size, u32 align);
u32 actMngGuest_mDoExt_createSolidHeapToCurrent(u32 size, u32 parent, u32 align);
void actMngGuest_mDoExt_restoreCurrentHeap();
void actMngGuest_mDoExt_adjustSolidHeap(u32 h);
void actMngGuest_mDoExt_destroySolidHeap(u32 h);
void actMngGuest_JKRHeap_alloc(u32 size, s32 align);
u32 actMngGuest_mDoExt_getHeap(s32 i);
BOOL actMngGuest_fopAcM_entrySolidHeap_(u32 a, u32 cb, u32 size, u32 heap);
u32 actMngGuest_fopAcM_getProcNameString(u32 a);
BOOL actMngGuest_cLib_chaseAngleS(u32 p, u32 target, u32 step);
s16 actMngGuest_cLib_targetAngleY(u32 a, u32 b);
void actMngGuest_cXyz_mi(u32 self, u32 out, u32 other);
f32 actMngGuest_PSVECSquareMag(u32 v);
void actMngGuest_PSMTXConcat(u32 a, u32 b, u32 out);
void actMngGuest_PSMTXMultVec(u32 m, u32 in, u32 out);
BOOL actMngGuest_J3DUClipper_clipBox(u32 self, u32 mtx, u32 max, u32 min);
BOOL actMngGuest_J3DUClipper_clipSphere(u32 self, u32 mtx, u32 c, f32 r);
void actMngGuest_J3DUClipper_calcViewFrustum(u32 self);
void actMngGuest_box_init(u32 box);
void actMngGuest_box_transform(u32 out, u32 in, u32 mtx);
s32 actMngGuest_dEvt_order(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 evId,
                           u32 info);
s32 actMngGuest_dEvt_orderOld(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 name);
u32 actMngGuest_dEvt_convPId(u32 evt, u32 id);
u32 actMngGuest_dEvmng_getEventIdx(u32 mng, u32 name, u32 room);
u32 actMngGuest_dEvmng_getEventPrio(u32 mng, u32 idx);
u32 actMngGuest_fopAcM_create(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn);
u32 actMngGuest_fopAcM_fastCreate(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn,
                                  u32 data);
u32 actMngGuest_check_itemno(u32 itemNo);
u32 actMngGuest_getItemNoByLife(u32 itemNo);
BOOL actMngGuest_isHeart(u32 itemNo);
void actMngGuest_csXyz_ct(u32 p, s16 x, s16 y, s16 z);
void actMngGuest_PSVECAdd(u32 a, u32 b, u32 out);
u32 actMngGuest_save_sub(u32 p);
BOOL actMngGuest_heroMode(u32 p);
u32 actMngGuest_fopAcM_fastCreateItem(u32 pos, u32 itemNo, u32 room, u32 angle, u32 scale, f32 sF, f32 sY,
                                      f32 g, u32 bit, u32 fn);
u32 actMngGuest_fopAcM_createItemFromTable(u32 pos, u32 tbl, u32 bit, u32 room, u32 type, u32 angle,
                                           u32 action, u32 scale);
u32 actMngGuest_getEmonoItemFromLifeBallTable(u32 t);
u32 actMngGuest_getItemFromLifeBallTableWithoutEmono(u32 t);
BOOL actMngGuest_isLimitedItem(u32 i);
BOOL actMngGuest_isNonSavedEmono(u32 i);
BOOL actMngGuest_dSv_memBit_isSwitch(u32 p, u32 bit, s32 room);
BOOL actMngGuest_dSv_info_isItem(u32 p, u32 bit, s32 room);
s32 actMngGuest_cDT_GetInf(u32 tbl, u32 field, u32 idx);
void actMngGuest_daIball_remove_old();
BOOL actMngGuest_fopAc_IsActor(u32 p);
u32 actMngGuest_fpcM_JudgeInLayer(u32 layer, u32 fn, u32 data);
void actMngGuest_cBgS_GndChk_ct(u32 p);
void actMngGuest_cBgS_Chk_dt(u32 p, s32 f);
f32 actMngGuest_cBgS_GroundCross(u32 bgs, u32 chk);
s16 actMngGuest_cM_atan2s(f32 y, f32 x);
void actMngGuest_cLib_addCalcAngleS(u32 p, u32 target, s32 scale, s32 maxStep, s32 minStep);
void actMngGuest_fopAcM_setStageLayer(u32 a);
void actMngGuest_fopAcM_setRoomLayer(u32 a, s32 room);
u32 actMngGuest_dKy_Sound_get();
void actMngGuest_dBgS_WtrChk_ct(u32 p);
void actMngGuest___register_global_object(u32 d);
BOOL actMngGuest_dBgS_SplGrpChk(u32 bgs, u32 chk);
BOOL actMngGuest_dComIfGs_checkGetItem(u32 i);
void actMngGuest_operator_delete(u32 p);

// HD: raw entry signatures retain complete register values; typed views do not narrow them.
inline fopAc_ac_c* actorMngActor(u32 actor) {
    return gabi::at<fopAc_ac_c>(actor);
}

struct fopAcM_prmBase_class {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ be<u16> setID;
};
struct fopAcM_prmScale_class {
    be<u8> x, y, z;
};
struct fopAcM_prm_class {
    /* 0x00 */ fopAcM_prmBase_class base;
    /* 0x18 */ fopAcM_prmScale_class scale;
    /* 0x1B */ be<u8> gbaName;
    /* 0x1C */ be<u32> parent_id;
    /* 0x20 */ be<s8> argument;
    /* 0x21 */ be<s8> room_no;
    /* 0x22 */ u8 mPadding[2];
};
WWHD_SIZE(fopAcM_prmBase_class, 0x18);
WWHD_SIZE(fopAcM_prmScale_class, 3);
WWHD_OFFSET(fopAcM_prm_class, base.position, 4);
WWHD_OFFSET(fopAcM_prm_class, base.angle, 0x10);
WWHD_OFFSET(fopAcM_prm_class, base.setID, 0x16);
WWHD_OFFSET(fopAcM_prm_class, scale, 0x18);
WWHD_OFFSET(fopAcM_prm_class, parent_id, 0x1C);
WWHD_OFFSET(fopAcM_prm_class, argument, 0x20);
WWHD_OFFSET(fopAcM_prm_class, room_no, 0x21);
WWHD_SIZE(fopAcM_prm_class, 0x24);

struct fopAcM_search_prm {
    be<u32> procname, prm_mask, parameter;
};
WWHD_OFFSET(fopAcM_search_prm, prm_mask, 4);
WWHD_OFFSET(fopAcM_search_prm, parameter, 8);
WWHD_SIZE(fopAcM_search_prm, 12);
struct ActorManagerNameInfo {
    /* 0x00 */ u8 mName[8];
    /* 0x08 */ be<s16> mProcName;
    /* 0x0A */ be<s8> mArgument;
    /* 0x0B */ u8 mPadding;
};
WWHD_OFFSET(ActorManagerNameInfo, mProcName, 8);
WWHD_OFFSET(ActorManagerNameInfo, mArgument, 0xA);
WWHD_SIZE(ActorManagerNameInfo, 12);
struct ActorManagerHeap {
    /* 0x00 */ u8 mHeader[0xC];
    /* 0x0C */ be<u32> mVtable;
    /* 0x10 */ be<u32> mName;
    /* 0x14 */ be<u32> mNameVtable;
};
WWHD_OFFSET(ActorManagerHeap, mVtable, 0xC);
WWHD_OFFSET(ActorManagerHeap, mName, 0x10);
WWHD_OFFSET(ActorManagerHeap, mNameVtable, 0x14);
WWHD_SIZE(ActorManagerHeap, 0x18);
struct ActorManagerHeapVtable {
    u8 mMethods[0x6C];
    /* 0x6C */ be<u32> getHeapSize;
    u8 mBetween[4];
    /* 0x74 */ be<u32> getFreeSize;
};
WWHD_OFFSET(ActorManagerHeapVtable, getHeapSize, 0x6C);
WWHD_OFFSET(ActorManagerHeapVtable, getFreeSize, 0x74);
WWHD_SIZE(ActorManagerHeapVtable, 0x78);
struct ActorManagerSinCos {
    be<f32> sin, cos;
};
WWHD_SIZE(ActorManagerSinCos, 8);
struct ActorManagerGuestID {
    be<u32> value;
};
struct ActorManagerGuestName {
    be<s16> value;
};
WWHD_SIZE(ActorManagerGuestID, 4);
WWHD_SIZE(ActorManagerGuestName, 2);

// GHS word copies must preserve signalling NaN payloads without a floating-point load.
inline be<u32>& actorMngBits(be<f32>& value) {
    return *reinterpret_cast<be<u32>*>(&value);
}
inline be<u32>& actorMngAngleWords(csXyz& value) {
    return *reinterpret_cast<be<u32>*>(&value);
}
template <class T> inline T* actorMngView(u32 address) {
    return gabi::at<T>(address);
}

struct ActorManagerSafeStringVtable {
    u8 mMethods[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(ActorManagerSafeStringVtable, assureTermination, 0x14);
WWHD_SIZE(ActorManagerSafeStringVtable, 0x18);
struct ActorManagerCullBox {
    be<f32> v[6];
};
struct ActorManagerCullSphere {
    be<f32> v[4];
};
struct ActorManagerMatrix {
    be<f32> m[12];
};
struct ActorManagerItemChoices {
    be<s32> v[16];
};
WWHD_SIZE(ActorManagerCullBox, 0x18);
WWHD_SIZE(ActorManagerCullSphere, 0x10);
WWHD_SIZE(ActorManagerMatrix, 0x30);
WWHD_SIZE(ActorManagerItemChoices, 0x40);
struct ActorManagerClipper {
    u8 mHeader[0x54];
    be<f32> mFar;
};
WWHD_OFFSET(ActorManagerClipper, mFar, 0x54);
WWHD_SIZE(ActorManagerClipper, 0x58);
enum ActorManagerEventType {
    actorEvent_TALK = 0,
    actorEvent_DOOR = 1,
    actorEvent_OTHER = 2,
    actorEvent_POTENTIAL = 4,
    actorEvent_ITEM = 5,
    actorEvent_SHOWITEM_X = 6,
    actorEvent_SHOWITEM_Y = 7,
    actorEvent_SHOWITEM_Z = 8,
    actorEvent_SHOWITEM_HD = 9,
    actorEvent_CATCH = 10,
    actorEvent_TREASURE = 11,
    actorEvent_CHANGE = 13
};

// HD: the save singleton has a 0x20-byte header before the GameCube savedata.
struct ActorManagerSave {
    u8 mHeader[0x20];
    dSv_save_c mSavedata;
};
WWHD_OFFSET(ActorManagerSave, mSavedata, 0x20);
WWHD_SIZE(ActorManagerSave, 0x798);
// HD: typed view of the water check's constructor/destructor fields.
struct ActorManagerWaterCheck {
    u8 mPolyInfo[0x10];
    cBgS_Chk mCheck;
    /* 0x24 */ be<u32> mPolyPassVtable;
    u8 mPolyPassFlags[8];
    /* 0x30 */ be<u32> mGrpPassVtable;
    be<u32> mGrp;
    /* 0x38 */ cXyz m_pos;
    /* 0x44 */ be<f32> mTop;
    /* 0x48 */ be<f32> m_height;
    u8 mPadding[4];
};
WWHD_OFFSET(ActorManagerWaterCheck, mCheck.__vtbl, 0x20);
WWHD_OFFSET(ActorManagerWaterCheck, mPolyPassVtable, 0x24);
WWHD_OFFSET(ActorManagerWaterCheck, mGrpPassVtable, 0x30);
WWHD_OFFSET(ActorManagerWaterCheck, m_pos, 0x38);
WWHD_OFFSET(ActorManagerWaterCheck, mTop, 0x44);
WWHD_OFFSET(ActorManagerWaterCheck, m_height, 0x48);
WWHD_SIZE(ActorManagerWaterCheck, 0x50);
void actorMngReportError(u32 format);
void actorMngReportErrorHeap(u32 format, u32 size, u32 name);
void actorMngReportWarning(u32 format);
void actorMngReportWarningHeaps(u32 format, u32 first, u32 second);
u32 actorMngCreateChild(u32 name, u32 parent, u32 parameters, u32 position, s32 room, u32 angle, u32 scale,
                        u32 argument, u32 callback);
void actorMngAssureTermination(u32 string);
void actorMngRandConstructor(u32 object);
void actorMngListConstructor(u32 object);
void actorMngSplGroupDestructor(u32 object, s32 flags);

struct ActorManagerFairyOffsets {
    cXyz positions[3];
};
struct ActorManagerItemTable {
    u8 mHeader[0x10];
    be<u8> items[256][16];
};
struct ActorManagerItem : fopAc_ac_c {
    u8 mItemState[0x783 - 0x3AC];
    /* 0x783 */ be<u8> mFlag;
    u8 mBetween;
    /* 0x785 */ be<u8> mStatus;
};
WWHD_SIZE(ActorManagerFairyOffsets, 0x24);
WWHD_OFFSET(ActorManagerItemTable, items, 0x10);
WWHD_SIZE(ActorManagerItemTable, 0x1010);
WWHD_OFFSET(ActorManagerItem, mFlag, 0x783);
WWHD_OFFSET(ActorManagerItem, mStatus, 0x785);
WWHD_SIZE(ActorManagerItem, 0x786);
struct ActorManagerSound {
    cXyz position;
    be<u32> type;
    be<u32> mReserved;
    be<u32> actor_id;
};
WWHD_OFFSET(ActorManagerSound, type, 0xC);
WWHD_OFFSET(ActorManagerSound, actor_id, 0x14);
WWHD_SIZE(ActorManagerSound, 0x18);

struct ActorManagerRoomStatus {
    be<u32> mProcID;
    u8 mRoomState[0x228];
};
struct ActorManagerRoomStatuses {
    ActorManagerRoomStatus rooms[64];
};
WWHD_SIZE(ActorManagerRoomStatus, 0x22C);
WWHD_SIZE(ActorManagerRoomStatuses, 0x8B00);
struct ActorManagerPlayerVtable {
    u8 mMethods[0xBC];
    be<u32> getGrabActorID;
    u8 mBetween[0xF4 - 0xC0];
    be<u32> onFrollCrashFlg;
};
WWHD_OFFSET(ActorManagerPlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(ActorManagerPlayerVtable, onFrollCrashFlg, 0xF4);
WWHD_SIZE(ActorManagerPlayerVtable, 0xF8);
struct ActorManagerCullObject {};
struct ActorManagerSecondView {
    u8 mHeader[0x610];
    ActorManagerCullObject mCullObject;
    u8 mBetween[0x7DC - 0x611];
    gabi::ptmf<u32 (ActorManagerCullObject::*)(u32, u32)> mCullBox;
    gabi::ptmf<u32 (ActorManagerCullObject::*)(u32, u32)> mCullSphere;
};
WWHD_OFFSET(ActorManagerSecondView, mCullObject, 0x610);
WWHD_OFFSET(ActorManagerSecondView, mCullBox, 0x7DC);
WWHD_OFFSET(ActorManagerSecondView, mCullSphere, 0x7E4);
WWHD_SIZE(ActorManagerSecondView, 0x7EC);
struct ActorManagerViewRoot {
    u8 mHeader[0x1024];
    gptr<gptr<ActorManagerSecondView>> mpSecondView;
};
WWHD_OFFSET(ActorManagerViewRoot, mpSecondView, 0x1024);
WWHD_SIZE(ActorManagerViewRoot, 0x1028);
struct ActorManagerVirtualEntry {
    be<u32> mAdjustment;
    be<u32> mFunction;
};
WWHD_OFFSET(ActorManagerVirtualEntry, mFunction, 4);
WWHD_SIZE(ActorManagerVirtualEntry, 8);
struct ActorManagerCharacterTable {
    u8 mTable[0x2C];
    be<u32> mNItems[16];
    /* 0x6C */ be<u32> mPercent;
};
WWHD_OFFSET(ActorManagerCharacterTable, mNItems, 0x2C);
WWHD_OFFSET(ActorManagerCharacterTable, mPercent, 0x6C);
WWHD_SIZE(ActorManagerCharacterTable, 0x70);
struct ActorManagerHeaderStatics {
    be<f32> mMinPi, mMaxPi;
    u8 mHeaderObjects[4];
    be<u32> mZeros[4];
};
WWHD_OFFSET(ActorManagerHeaderStatics, mZeros, 0xC);
WWHD_SIZE(ActorManagerHeaderStatics, 0x1C);

struct ActorManagerSinCosTable {
    ActorManagerSinCos entries[8192];
};
struct ActorManagerCullBoxes {
    fopAc_cullSizeBox entries[14];
};
struct ActorManagerCullSpheres {
    fopAc_cullSizeSphere entries[8];
};
WWHD_SIZE(ActorManagerSinCosTable, 0x10000);
WWHD_SIZE(ActorManagerCullBoxes, 0x150);
WWHD_SIZE(ActorManagerCullSpheres, 0x80);
struct ActorManagerTuData {
    u32 waterVtable, waterPolyVtable, groupVtable;
    u32 destructorRecords[5];
};
extern const ActorManagerTuData actorManagerTu;

// HD: process IDs differ from the GameCube profile table.
enum ActorManagerProcessName {
    fpcNm_WARPFLOWER_e = 0x68,
    fpcNm_PLAYER_e = 0xA8,
    fpcNm_ITEM_e = 0xFF,
    fpcNm_Demo_Item_e = 0x101,
    fpcNm_RACEITEM_e = 0x102,
    fpcNm_ShopItem_e = 0x103,
    fpcNm_SPC_ITEM01_e = 0x105,
    fpcNm_NPC_FA1_e = 0x168,
    fpcNm_Iball_e = 0x18F,
    fpcNm_DISAPPEAR_e = 0x190
};
enum ActorManagerItemNumber {
    dItemNo_YELLOW_RUPEE_e = 3,
    dItemNo_RECOVER_FAIRY_e = 0x16,
    dItemNo_TRIPLE_HEART_e = 0x1E,
    dItemNo_NONE_e = 0xFF
};

// TU-wide objects, defined in f_op_actor_mng.cpp.
extern ActorManagerSinCosTable& sincosTable;
extern cXyz& zeroPosition;
extern csXyz& zeroAngle;
extern be<u32>& HeapAlign;
extern be<u8>& HeapAdjustVerbose;
extern be<u8>& HeapAdjustQuiet;
extern ActorManagerRoomStatuses& roomStatuses;
extern be<u32>& nodeProcessType;
extern be<u32>& stageProcessID;
extern ActorManagerMatrix& viewMtx;
extern ActorManagerClipper& clipper;
extern be<f32>& systemFar;
extern ActorManagerCullBoxes& l_cullSizeBox;
extern ActorManagerCullSpheres& l_cullSizeSphere;
extern be<u32>& stageHeapFlags;
extern be<u32>& specialActorHeap;
extern gptr<ActorManagerViewRoot>& viewRoot;
extern be<u32>& saveSingleton;
extern ActorManagerFairyOffsets& fairy_offset_tbl;
extern be<u32>& fairy_offset_initialized;
extern ActorManagerWaterCheck& water_check;
extern ActorManagerHeaderStatics& headerStatics;
extern be<u32>& water_check_initialized;
