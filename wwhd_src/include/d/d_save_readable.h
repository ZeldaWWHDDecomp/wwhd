/* Save-system implementation views. The shared WWHD layouts retain their public API;
 * these private methods keep the full register widths observed in this unit. */
#pragma once
#include "d/d_save.h"
#include "d/d_com_inf_game.h"
#include "gc_api/tu.h"
#include "d/d_item_data.h"

void saveAssert(const char* file, s32 line, const char* message);
f32 saveRandom(f32 max);
void saveAudioOutput(u32 mode);
u32 saveAllocate(u32 size);
void saveZero(u32 address, u32 size);
void saveOffsetPosition(u32 out, u32 pos, s32 angle, u32 offset);
void saveCopy(u32 dst, u32 src, u32 size);
void saveFill(u32 dst, u32 value, u32 size);
u32 saveHdProgress(u32 save);
u32 savePictureInfo(u32 save);
void savePictureCount(u32 picture, u32 count);

namespace d_save_cpp {

enum dInventorySlot_e {
    dInvSlot_CAMERA_e = 8,
    dInvSlot_BOTTLE0_e = 14,
    dInvSlot_BOTTLE_COUNT_e = 4,
    dInvSlot_ItemLast_e = 21,
    dInvSlot_BeastFirst_e = 24,
    dInvSlot_BeastLast_e = 32,
    dInvSlot_BaitFirst_e = 36,
    dInvSlot_BaitLast_e = 44,
    dInvSlot_ReserveFirst_e = 48,
    dInvSlot_ReserveLast_e = 56,
    dInvSlot_NONE_e = 0xFF,
};
enum SaveLimits {
    ITEM_BUTTON_COUNT = 3,
    HD_ITEM_BUTTON_COUNT = 5,
    ROOM_MAX = 64,
    MEMORY_SWITCH = 128,
    DAN_SWITCH = 64,
    ZONE_SWITCH = 48,
    MEMORY_ITEM = 64,
    ZONE_ITEM = 16,
    CARD_SLOT_SIZE = 0xA94,
    CARD_PAYLOAD_SIZE = 0xA8C,
};

extern be<u8> (&l_defaultName)[8];
extern be<u8>& l_kg1HighScore;
extern cXyz& l_offsetPos;
extern be<u32>& l_offsetPosGuard;
extern be<u16> (&l_holdEventReg)[17];
extern be<u16> (&l_onEventBit)[5];
extern be<u32>& l_shipRaceRupee;
extern be<u32>& l_shipRaceResult;

struct MemoryWords { be<u32> mWords[9]; };
WWHD_SIZE(MemoryWords, 0x24);
inline MemoryWords& memoryWords(::dSv_memory_c* memory) { return *(MemoryWords*)memory; }

struct PositionWords { be<u32> x, y, z; };
WWHD_SIZE(PositionWords, 0xC);
inline PositionWords& wordPosition(cXyz* pos) { return *(PositionWords*)pos; }
inline const PositionWords& wordPosition(const cXyz* pos) { return *(const PositionWords*)pos; }

template<class T> inline T* view(u32 address) { return gabi::at<T>(address); }

// HD: guest address arithmetic wraps at 32 bits, including invalid inventory indices.
template<class T> inline T& indexed(T* values, u32 index) {
    return *gabi::at<T>(gabi::ea(values) + index * (u32)sizeof(T));
}

struct dSv_player_status_a_c : ::dSv_player_status_a_c {
    void init();
};
WWHD_SIZE(dSv_player_status_a_c, sizeof(::dSv_player_status_a_c));

struct dSv_player_status_b_c : ::dSv_player_status_b_c {
    void init();
};
WWHD_SIZE(dSv_player_status_b_c, sizeof(::dSv_player_status_b_c));

struct dSv_player_return_place_c : ::dSv_player_return_place_c {
    void init();
    // Keep the full guest register: invalid name addresses must not be treated as host literals.
    void set(u32 name, u32 roomNo, u32 status);
};
WWHD_SIZE(dSv_player_return_place_c, sizeof(::dSv_player_return_place_c));

struct dSv_player_item_c : ::dSv_player_item_c {
    void init();
    void setBottleItemIn(u32 prevItemNo, u32 newItemNo);
    void setEmptyBottleItemIn(u32 itemNo);
    void setEmptyBottle();
    void setEquipBottleItemIn(u32 btn, u32 itemNo);
    void setEquipBottleItemEmpty(u32 btn);
    void setEquipBottleItemIn(u32 itemNo);
    void setEquipBottleItemEmpty();
    u8 checkEmptyBottle();
    u8 checkBottle(u32 itemNo);
};
WWHD_SIZE(dSv_player_item_c, sizeof(::dSv_player_item_c));

struct dSv_player_get_item_c : ::dSv_player_get_item_c {
    void init();
    void onItem(u32 field, u32 item);
    BOOL isItem(u32 field, u32 item);
    void onBottleItem(u32 item);
    BOOL isBottleItem(u32 item);
};
WWHD_SIZE(dSv_player_get_item_c, sizeof(::dSv_player_get_item_c));

struct dSv_player_item_record2_c : ::dSv_player_item_record2_c {
    void init();
};
WWHD_SIZE(dSv_player_item_record2_c, sizeof(::dSv_player_item_record2_c));

struct dSv_player_item_record_c : ::dSv_player_item_record_c {
    void init();
    void resetTimer(u32 timer);
    void decTimer();
    u16 getTimer();
};
WWHD_SIZE(dSv_player_item_record_c, sizeof(::dSv_player_item_record_c));

struct dSv_player_item_max2_c : ::dSv_player_item_max2_c {
    void init();
};
WWHD_SIZE(dSv_player_item_max2_c, sizeof(::dSv_player_item_max2_c));

struct dSv_player_item_max_c : ::dSv_player_item_max_c {
    void init();
};
WWHD_SIZE(dSv_player_item_max_c, sizeof(::dSv_player_item_max_c));

struct dSv_player_bag_item_c : ::dSv_player_bag_item_c {
    void init();
    bool checkBeastItem(u32 itemNo);
    void setBeastItem(u32 itemNo);
    void setBeastItemEmpty(u32 itemNo);
    void setBaitItemChange(u32 itemNo);
    void setBaitItemChange(u32 btn, u32 itemNo);
    void setBaitItemEmpty();
    void setBaitItemEmpty(u32 btn);
    u8 checkBaitItem(u32 itemNo);
    u8 checkBaitItemEmpty();
    void setBaitItem(u32 itemNo);
    void setReserveItemChange(u32 itemNo);
    void setReserveItemEmpty();
    void setReserveItemChange(u32 btn, u32 itemNo);
    void setReserveItemEmpty(u32 btn);
    u8 checkReserveItem(u32 itemNo);
    u8 checkReserveItemEmpty();
    void setReserveItem(u32 itemNo);
};
WWHD_SIZE(dSv_player_bag_item_c, sizeof(::dSv_player_bag_item_c));

struct dSv_player_get_bag_item_c : ::dSv_player_get_bag_item_c {
    void init();
    void onBeast(u32 no);
    BOOL isBeast(u32 no);
    void onBait(u32 no);
    BOOL isBait(u32 no);
    void onReserve(u32 no);
    BOOL isReserve(u32 no);
};
WWHD_SIZE(dSv_player_get_bag_item_c, sizeof(::dSv_player_get_bag_item_c));

struct dSv_player_bag_item_record_c : ::dSv_player_bag_item_record_c {
    void init();
};
WWHD_SIZE(dSv_player_bag_item_record_c, sizeof(::dSv_player_bag_item_record_c));

struct dSv_player_collect_c : ::dSv_player_collect_c {
    void init();
    void onCollect(u32 field, u32 no);
    void offCollect(u32 field, u32 no);
    BOOL isCollect(u32 field, u32 no);
    void onTact(u32 no);
    BOOL isTact(u32 no);
    void onTriforce(u32 no);
    void offTriforce(u32 no);
    BOOL isTriforce(u32 no);
    void onSymbol(u32 no);
    void offSymbol(u32 no);
    BOOL isSymbol(u32 no);
    s32 getTriforceNum();
};
WWHD_SIZE(dSv_player_collect_c, sizeof(::dSv_player_collect_c));

struct dSv_player_map_c : ::dSv_player_map_c {
    void init();
    void onGetMap(u32 no);
    BOOL isGetMap(u32 no);
    void onOpenMap(u32 no);
    void offOpenMap(u32 no);
    BOOL isOpenMap(u32 no);
    void onCompleteMap(u32 no);
    void offCompleteMap(u32 no);
    BOOL isCompleteMap(u32 no);
    void onTriforce(u32 no);
    BOOL isTriforce(u32 no);
    s32 getCollectMapNum();
    void onFmapBit(u32 idx, u32 no);
    void onSaveArriveGrid(u32 no);
    BOOL isFmapBit(u32 idx, u32 no);
    BOOL isSaveArriveGrid(u32 no);
    void onSaveArriveGridForAgb(u32 no);
};
WWHD_SIZE(dSv_player_map_c, sizeof(::dSv_player_map_c));


struct dSv_player_info_c : ::dSv_player_info_c {
    void init();
};
WWHD_SIZE(dSv_player_info_c, sizeof(::dSv_player_info_c));


struct dSv_player_config_c : ::dSv_player_config_c {
    void init();
    u8 checkVibration();
};
WWHD_SIZE(dSv_player_config_c, sizeof(::dSv_player_config_c));


struct dSv_player_priest_c : ::dSv_player_priest_c {
    void init();
    void set(u32 a, const cXyz* pos, u32 angle, u32 roomNo);
};
WWHD_SIZE(dSv_player_priest_c, sizeof(::dSv_player_priest_c));


struct dSv_player_status_c_c : ::dSv_player_status_c_c {
    void init();
};
WWHD_SIZE(dSv_player_status_c_c, sizeof(::dSv_player_status_c_c));


struct dSv_player_c : ::dSv_player_c {
    void init();
};
WWHD_SIZE(dSv_player_c, sizeof(::dSv_player_c));


struct dSv_ocean_c : ::dSv_ocean_c {
    void init();
    void onOceanSvBit(u32 grid, u32 bit);
    BOOL isOceanSvBit(u32 grid, u32 bit);
};
WWHD_SIZE(dSv_ocean_c, sizeof(::dSv_ocean_c));


struct dSv_event_c : ::dSv_event_c {
    void setEventReg(u32 reg, u32 no);
    void init();
    void onEventBit(u32 no);
    void offEventBit(u32 no);
    BOOL isEventBit(u32 no);
    u8 getEventReg(u32 reg);
};
WWHD_SIZE(dSv_event_c, sizeof(::dSv_event_c));


struct dSv_memBit_c : ::dSv_memBit_c {
    void init();
    void onTbox(u32 no);
    BOOL isTbox(u32 no);
    void onSwitch(u32 no);
    void offSwitch(u32 no);
    BOOL isSwitch(u32 no);
    BOOL revSwitch(u32 no);
    void onItem(u32 no);
    BOOL isItem(u32 no);
    void onVisitedRoom(u32 no);
    BOOL isVisitedRoom(u32 no);
    void onDungeonItem(u32 no);
    BOOL isDungeonItem(u32 no);
};
WWHD_SIZE(dSv_memBit_c, sizeof(::dSv_memBit_c));


struct dSv_memory_c : ::dSv_memory_c {
    void init();
};
WWHD_SIZE(dSv_memory_c, sizeof(::dSv_memory_c));


struct dSv_danBit_c : ::dSv_danBit_c {
    s32 init(u32 stageNo);
    void onSwitch(u32 no);
    void offSwitch(u32 no);
    BOOL isSwitch(u32 no);
    BOOL revSwitch(u32 no);
};
WWHD_SIZE(dSv_danBit_c, sizeof(::dSv_danBit_c));


struct dSv_zoneBit_c : ::dSv_zoneBit_c {
    void init();
    void clearRoomSwitch();
    void onSwitch(u32 no);
    void offSwitch(u32 no);
    BOOL isSwitch(u32 no);
    BOOL revSwitch(u32 no);
    void onItem(u32 no);
    BOOL isItem(u32 no);
};
WWHD_SIZE(dSv_zoneBit_c, sizeof(::dSv_zoneBit_c));


struct dSv_zoneActor_c : ::dSv_zoneActor_c {
    void init();
    void on(u32 id);
    BOOL is(u32 id);
};
WWHD_SIZE(dSv_zoneActor_c, sizeof(::dSv_zoneActor_c));


struct dSv_zone_c : ::dSv_zone_c {
    u32 __ct();
    void init(u32 roomNo);
};
WWHD_SIZE(dSv_zone_c, sizeof(::dSv_zone_c));


struct dSv_restart_c : ::dSv_restart_c {
    void setRoom(const cXyz* pos, u32 angle, u32 roomNo);
    void setRestartOption(u32 option, const cXyz* pos, u32 angle, u32 roomNo);
    void setRestartOption(u32 option);
};
WWHD_SIZE(dSv_restart_c, sizeof(::dSv_restart_c));


struct dSv_reserve_c : ::dSv_reserve_c {
    void init();
};
WWHD_SIZE(dSv_reserve_c, sizeof(::dSv_reserve_c));


struct dSv_save_c : ::dSv_save_c {
    enum { STAGE_SEA = 0, STAGE_MISC = 11 };
    void init();
};
WWHD_SIZE(dSv_save_c, sizeof(::dSv_save_c));


struct dSv_turnRestart_c : ::dSv_turnRestart_c {
    void set(const cXyz* pos, u32 angle, u32 roomNo, u32 param, const cXyz* shipPos, u32 shipAngle,
                                  u32 hasShip);
};
WWHD_SIZE(dSv_turnRestart_c, sizeof(::dSv_turnRestart_c));


struct dSv_info_c : ::dSv_info_c {
    void initZone();
    void init();
    void reinit();
    void getSave(u32 stageNo);
    void putSave(u32 stageNo);
    s32 createZone(u32 roomNo);
    void onSwitch(s32 no, s32 roomNo);
    void offSwitch(s32 no, s32 roomNo);
    BOOL isSwitch(s32 no, s32 roomNo);
    BOOL revSwitch(s32 no, s32 roomNo);
    void onItem(s32 no, s32 roomNo);
    BOOL isItem(s32 no, s32 roomNo);
    void onActor(s32 id, s32 roomNo);
    BOOL isActor(s32 id, s32 roomNo);
    s32 card_to_memory(u8* card, s32 dataNum);
    s32 memory_to_card(u8* card, s32 dataNum);
    s32 initdata_to_card(u8* card, s32 dataNum);
};
WWHD_SIZE(dSv_info_c, sizeof(::dSv_info_c));

struct SaveGlobal {
    /* 0x0000 */ u8 mPrefix[0x20];
    /* 0x0020 */ ::dSv_info_c mInfo;
    /* 0x12C0 */ u8 mHdSave[1];
};
WWHD_SIZE(SaveGlobal, 0x12C1);
WWHD_OFFSET(SaveGlobal, mInfo, 0x20);
WWHD_OFFSET(SaveGlobal, mHdSave, 0x12C0);
inline SaveGlobal* saveBase() { return *(gptr<SaveGlobal>*)gabi::at<void>(0x101F84DC); }
inline ::dSv_player_c& savePlayer(u32 base) { return view<SaveGlobal>(base)->mInfo.mSavedata.mPlayer; }
inline ::dSv_player_c& savePlayer() { return saveBase()->mInfo.mSavedata.mPlayer; }
inline ::dComIfG_inf_c* playInfo(u32 base) { return view<::dComIfG_inf_c>(base); }
inline ::dComIfG_inf_c* playInfo() { return (::dComIfG_inf_c*)dComIfGp_get(); }
struct HdVibrationView {
    /* 0x0000 */ u8 mPrefix[0x5BED];
    /* 0x5BED */ be<u8> mSetting;
};
WWHD_OFFSET(HdVibrationView, mSetting, 0x5BED);

// HD: these stack slots are serialized with their original, untouched padding bytes.
struct SaveCardFrame {
    /* 0x000 */ be<u32> mBackChain;
    /* 0x004 */ u8 mReserved04[4];
    /* 0x008 */ ::dSv_player_config_c mConfig;
    /* 0x00D */ u8 mReserved0D[3];
    /* 0x010 */ ::dSv_player_item_record_c mItemRecord;
    /* 0x018 */ ::dSv_player_item_max_c mItemMax;
    /* 0x020 */ ::dSv_player_return_place_c mReturnPlace;
    /* 0x02C */ ::dSv_player_get_bag_item_c mGetBagItem;
    /* 0x038 */ ::dSv_player_collect_c mCollect;
    /* 0x045 */ u8 mReserved45[3];
    /* 0x048 */ ::dSv_player_priest_c mPriest;
    /* 0x058 */ ::dSv_player_item_c mItem;
    /* 0x06D */ u8 mReserved6D[3];
    /* 0x070 */ ::dSv_player_get_item_c mGetItem;
    /* 0x085 */ u8 mReserved85[3];
    /* 0x088 */ ::dSv_player_status_a_c mStatusA;
    /* 0x0A0 */ ::dSv_player_bag_item_c mBagItem;
    /* 0x0B8 */ ::dSv_player_bag_item_record_c mBagItemRecord;
    /* 0x0D0 */ ::dSv_player_status_b_c mStatusB;
    /* 0x0E8 */ ::dSv_memory_c mMemory;
    /* 0x10C */ ::dSv_reserve_c mReserve;
    /* 0x15C */ ::dSv_ocean_c mOcean;
    /* 0x1C0 */ ::dSv_player_status_c_c mStatusC;
    /* 0x230 */ ::dSv_player_map_c mMap;
    /* 0x2B4 */ ::dSv_player_info_c mInfo;
    /* 0x310 */ ::dSv_event_c mEvent;
    /* 0x410 */ be<u32> mSavedR28;
    /* 0x414 */ be<u32> mSavedR29;
    /* 0x418 */ be<u32> mSavedR30;
    /* 0x41C */ be<u32> mSavedR31;
};
WWHD_SIZE(SaveCardFrame, 0x420);
WWHD_OFFSET(SaveCardFrame, mConfig, 0x8);
WWHD_OFFSET(SaveCardFrame, mItemRecord, 0x10);
WWHD_OFFSET(SaveCardFrame, mItemMax, 0x18);
WWHD_OFFSET(SaveCardFrame, mReturnPlace, 0x20);
WWHD_OFFSET(SaveCardFrame, mGetBagItem, 0x2C);
WWHD_OFFSET(SaveCardFrame, mCollect, 0x38);
WWHD_OFFSET(SaveCardFrame, mPriest, 0x48);
WWHD_OFFSET(SaveCardFrame, mItem, 0x58);
WWHD_OFFSET(SaveCardFrame, mGetItem, 0x70);
WWHD_OFFSET(SaveCardFrame, mStatusA, 0x88);
WWHD_OFFSET(SaveCardFrame, mBagItem, 0xA0);
WWHD_OFFSET(SaveCardFrame, mBagItemRecord, 0xB8);
WWHD_OFFSET(SaveCardFrame, mStatusB, 0xD0);
WWHD_OFFSET(SaveCardFrame, mMemory, 0xE8);
WWHD_OFFSET(SaveCardFrame, mReserve, 0x10C);
WWHD_OFFSET(SaveCardFrame, mOcean, 0x15C);
WWHD_OFFSET(SaveCardFrame, mStatusC, 0x1C0);
WWHD_OFFSET(SaveCardFrame, mMap, 0x230);
WWHD_OFFSET(SaveCardFrame, mInfo, 0x2B4);
WWHD_OFFSET(SaveCardFrame, mEvent, 0x310);
WWHD_OFFSET(SaveCardFrame, mSavedR28, 0x410);

// HD-only save extensions are accessed through their verified accessor calls.
struct HdProgress {
    /* 0x0 */ be<u8> mFirst;
    /* 0x1 */ u8 mReserved[2];
    /* 0x3 */ be<u8> mSecond;
};
WWHD_SIZE(HdProgress, 4);
struct HdPictureInfo {
    /* 0x000000 */ u8 mReserved[0x3C030C];
    /* 0x3C030C */ be<u8> mCount;
};
WWHD_SIZE(HdPictureInfo, 0x3C030D);
struct RoomZone {
    /* 0x000 */ be<s8> mZoneNo;
    /* 0x001 */ u8 mReserved[0x22B];
};
WWHD_SIZE(RoomZone, 0x22C);

}  // namespace d_save_cpp
