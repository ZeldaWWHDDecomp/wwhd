/* Barrel race items: GameCube item-base fields shifted by the verified HD actor layout.
 * HD: race-specific fields are +0x114 from GameCube; keep the original integer call widths. */
#pragma once
#include "d/actor/d_a_itembase.h"
#include "d/actor/d_a_player.h"
#include "gc_api/f_op/f_op_actor.h"
struct RaceItemVtable {
    u8 mBeforeDraw[0x14];
    gfn DrawBase;
};
struct RaceItemVtableLink {
    u8 mBeforeVtable[0xB4];
    gptr<RaceItemVtable> mpVtable;
};
struct RaceItemPlayerVtable {
    u8 mBeforeHookshot[0x10C];
    gfn setHookshotCarryOffset;
};
struct RaceItemPlayerVtableLink {
    u8 mBeforeVtable[0xB4];
    gptr<RaceItemPlayerVtable> mpVtable;
};
struct RaceItemFieldResource {
    gptr<char> mArc;
    u8 mOtherResources[0x18];
};
struct RaceItemResource {
    u8 mBeforeHeap[0x20];
    be<u16> mHeapSize;
    u8 mPadding[2];
};
struct RaceItemRotateData {
    be<s16> mRotateYSpeed;
};
struct RaceItemActorVtable {
    be<u32> mFirstEntry;
};
WWHD_OFFSET(RaceItemVtable, DrawBase, 0x14);
WWHD_OFFSET(RaceItemVtableLink, mpVtable, 0xB4);
WWHD_OFFSET(RaceItemPlayerVtable, setHookshotCarryOffset, 0x10C);
WWHD_OFFSET(RaceItemPlayerVtableLink, mpVtable, 0xB4);
WWHD_SIZE(RaceItemFieldResource, 0x1C);
WWHD_OFFSET(RaceItemResource, mHeapSize, 0x20);
WWHD_SIZE(RaceItemResource, 0x24);
WWHD_SIZE(RaceItemRotateData, 2);
WWHD_SIZE(RaceItemActorVtable, 4);
enum RaceItemState {
    RaceItem_Wait = 0,
    RaceItem_GetStart = 1,
    RaceItem_Get = 2,
    RaceItem_DropStart = 3,
    RaceItem_Drop = 4
};
enum RaceItemGetType { RaceItem_Race = 0, RaceItem_Normal = 1 };
enum RaceItemFlags { RaceItem_BoomerangCarry = 1 };
enum RaceItemAttack { AT_TYPE_BOOMERANG = 0x40, AT_TYPE_HOOKSHOT = 0x8000 };
enum RaceItemProcess { fpcNm_BOOMERANG_e = 0x1B0 };
enum RaceItemNumber {
    dItemNo_GREEN_RUPEE_e = 1,
    dItemNo_BLUE_RUPEE_e = 2,
    dItemNo_YELLOW_RUPEE_e = 3,
    dItemNo_RED_RUPEE_e = 4,
    dItemNo_PURPLE_RUPEE_e = 5,
    dItemNo_ORANGE_RUPEE_e = 6,
    dItemNo_SILVER_RUPEE_e = 15,
    dItemNo_TRIPLE_HEART_e = 30
};
enum RaceItemSound {
    JA_SE_LUPY_GET = 0x826,
    JA_SE_BLUE_LUPY_GET = 0x835,
    JA_SE_RED_LUPY_GET = 0x836,
    JA_SE_HEART_PIECE = 0x821,
    JA_SE_OBJ_FALL_WATER_S = 0x6918
};
struct daRaceItem_c : daItemBase_c {
    /*0x750*/ be<s32> field_0x63C;
    /*0x754*/ be<u32> field_0x640;
    /*0x758*/ be<u8> field_0x644;
    /*0x759*/ be<u8> field_0x645;
    u8 mPadding[2];
    void setListStart();
    BOOL execute();
    BOOL draw() {
        auto* vtable = reinterpret_cast<RaceItemVtableLink*>(this)->mpVtable.get();
        return gabi::call_ptr<BOOL>(vtable->DrawBase.get(), this);
    }
    void set_mtx();
    // HD: named separately until autoglue can register overloaded methods.
    void set_mtx_pos(cXyz*);
    BOOL Delete();
    cPhs_State create();
    BOOL CreateInit();
    void checkGet();
    void normalItemGet();
    void raceItemGet();
    void raceItemForceGet();
    BOOL startOffsetPos();
    BOOL endOffsetPos(f32, cXyz*, f32, f32, csXyz*);
    BOOL checkOffsetPos();
};
WWHD_OFFSET(daRaceItem_c, field_0x63C, 0x750);
WWHD_OFFSET(daRaceItem_c, field_0x640, 0x754);
WWHD_OFFSET(daRaceItem_c, field_0x644, 0x758);
WWHD_SIZE(daRaceItem_c, 0x75C);
// HD: these two queries retain the original full-width return register.
u32 RaceItem_getHeight(daItemBase_c*);
u32 RaceItem_getR(daItemBase_c*);
void RaceItem_ClrCoHit(dCcD_Cyl*);
void RaceItemBase_dt(daItemBase_c*, s32);
BOOL CheckFieldItemCreateHeap(fopAc_ac_c*);
void fopAcM_addAngleY(fopAc_ac_c*, s16, s16);
void* fpcSch_JudgeForPName(void*, void*);
void mDoAud_seStart(u32);
void RaceItem_onItem(dSv_info_c*, s32, s32);

struct RaceItemFieldResources {
    RaceItemFieldResource mItems[256];
};
struct RaceItemResources {
    RaceItemResource mItems[256];
};
struct RaceItemSounds {
    be<u16> mSounds[18];
};
WWHD_SIZE(RaceItemFieldResources, 256 * 0x1C);
WWHD_SIZE(RaceItemResources, 256 * 0x24);
WWHD_SIZE(RaceItemSounds, 36);
