/* Merchant display pedestal; GameCube member names, HD layout. Local use only. */
#pragma once
#include "gc_api/d/d_npc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_item.h"
#include "gc_api/d/d_kankyo.h"
#include "d/actor/d_a_dai_item.h"
struct daDai_c : fopNpc_npc_c {
    // HD: fopNpc_npc_c grows from 0x6C4 to 0x7DC; the pedestal fields follow it.
    request_of_phase_process_class mPhsFdai, mPhsCloth; gptr<J3DModel> mpModel;
    dCcD_Stts mStts; dCcD_Cyl mCyl;
    /* mCanTalk (0x95C) was m844: proc 02116648: 1 while the player is within 100 (XZ, |dy| < 10) and faces the stand; eventOrder 02116750 offers the talk / item talk only then (tww the same) */
    be<s16> mCanTalk, mEvtDaiItemIdx, mEvtDefaultTalkIdx;
    /* mSelectItem (0x962) was m84A: XyCheckCB 02115E7C stores the X / Y item when it can go on the stand; checkOrder 02116470 refuses (talk) unless it is a daiza item; next_msgStatus 02116848 clears it when the item is taken back */
    /* mPlacedItemNo (0x963) was m84B: checkOrder 02116470: the mItemNo of the daStandItem_c the player puts down (mStandItemId), saved into the stand's event register when the DAI_ITEM event ends */
    /* mTalkPending (0x964) was m84C: checkOrder 02116470 sets 1 for a plain talk (or an item it cannot take); proc 02116648 runs talk(1) while it is set and clears it (and resets the event) when the box closes */
    /* mStandItemId (0x968) was m850: CreateInit 02116054: process ID of the fpcNm_STANDITEM_e created for the saved item (or -1); checkOrder 02116470 takes the item the player holds when DAI_ITEM starts and moves it onto the stand at the end; next_msgStatus 02116848 deletes it when taken back */
    be<u8> mSelectItem, mPlacedItemNo, mTalkPending, mSaveID; u8 m84E[2]; be<u32> mStandItemId;
    daDai_c() {
        __ct(static_cast<fopNpc_npc_c*>(this));
        __vtbl = 0x1000CC70;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1000CBCC);
    }
    BOOL CreateHeap(); void CreateInit(); cPhs_State _create(); BOOL _delete(); BOOL _draw();
    s16 XyCheckCB(s32 button); s16 XyEventCB(s32 button); void set_mtx();
    void checkOrder(); void proc(); void eventOrder(); BOOL _execute();
    u32 getMsg(); u16 next_msgStatus(be<u32>* message);
};
WWHD_OFFSET(daDai_c, mPhsFdai, 0x7DC);
WWHD_OFFSET(daDai_c, mPhsCloth, 0x7E4);
WWHD_OFFSET(daDai_c, mpModel, 0x7EC);
WWHD_OFFSET(daDai_c, mStts, 0x7F0);
WWHD_OFFSET(daDai_c, mCyl, 0x82C);
WWHD_OFFSET(daDai_c, mCanTalk, 0x95C);
WWHD_OFFSET(daDai_c, mSaveID, 0x965);
WWHD_OFFSET(daDai_c, mStandItemId, 0x968);
WWHD_SIZE(daDai_c, 0x96C);
struct dai_resourceName_c { gptr<be<u8>> mpText; be<u32> mVtable; };
struct dai_linkage_c { be<u32> mWords[2]; };
struct dai_positionWords_c { be<u32> mWords[3]; };
struct dai_resourceControlPointer_c { gptr<void> mpValue; };
struct dai_playerVtable_c { u8 _00[0xBC]; be<u32> mGetGrabActorID; };
WWHD_SIZE(dai_resourceName_c, 8);
WWHD_SIZE(dai_linkage_c, 8);
WWHD_SIZE(dai_positionWords_c, 12);
enum { fpcNm_STANDITEM_e = 0x1CE, dItemNo_DELIVERY_BAG_e = 0x30 };
J3DModelData* dai_getObjectResource(void* control, dai_resourceName_c* name, s32 index);
void dai_assert(const char* file, u32 line, const char* condition);
BOOL dai_isDaizaItem(u32 item);
u32 dai_readEventRegister(dSv_event_c* event, u16 label);
fopAc_ac_c* dai_fastCreate(s32 name, u32 parameters, cXyz* position, s32 room, csXyz* angle, void* scale, s32 subtype, void* callback, u32 user);
void dai_subtractVector(const cXyz* first, cXyz* out, const cXyz* second);
f64 dai_sqrt(f64 value);
BOOL dai_checkReserveEmpty(dSv_player_bag_item_c* bag);
void dai_setReserve(dSv_player_bag_item_c* bag, u32 item);
s32 dai_getRotenItemCount();
