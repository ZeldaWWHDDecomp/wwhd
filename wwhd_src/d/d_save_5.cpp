/* d_save — Active save information, new-game state and packed memory-card serialization.
 * GameCube names with verified WWHD layouts and behavior. Unit: d_save. */
#include "bindings.h"
#include "d/d_save_readable.h"

namespace d_save_cpp {

#include "d/d_save_local.h"

/* 025E1C74 probably mDoAud_setOutputMode */
static inline u8 saveGetEventReg(u32 ev, u32 reg) { return view<dSv_event_c>(ev)->getEventReg(reg); }
static inline void saveSetEventReg(u32 ev, u32 reg, u32 no) { view<dSv_event_c>(ev)->setEventReg(reg, no); }
static inline void saveOnEventBit(u32 ev, u32 no) { view<dSv_event_c>(ev)->onEventBit(no); }
static inline void initializeZoneRecord(u32 z, s32 roomNo) { view<dSv_zone_c>(z)->init(roomNo); }

/* dComIfGp_roomControl_getZoneNo(roomNo) (inline): HD calls dComIfGp_get, then reads the static
 * room status table */
static inline s32 roomZoneNo(u32 roomNo) {
    play();
    return (s8)indexed(view<RoomZone>(0x1047E8EB), roomNo).mZoneNo;
}

/* 025B99DC */
void d_save_cpp::dSv_info_c::initZone() {
    for (u32 i = 0; i < ZONE_MAX; i++) view<dSv_zone_c>(gabi::ea(&indexed(mZone, i)))->init((u32)-1);
}

/* 025B9A18 */
void d_save_cpp::dSv_info_c::init() {
    view<dSv_save_c>(gabi::ea(&mSavedata))->init();
    view<dSv_memory_c>(gabi::ea(&mMemory))->init();
    view<dSv_danBit_c>(gabi::ea(&mDan))->init(-1);
    initZone();
    view<dSv_event_c>(gabi::ea(&mTmp))->init();
    mMemCardCheckID[1] = 0;
    mNewFile = 0;
    mMemCardCheckID[0] = 0;
    mNoFile = 0;
    mDataNum = 0;
    l_shipRaceRupee = 0;
    l_shipRaceResult = 0;
}

// HD: preserves sound, vibration, picture count and the two validated HD progress bytes across a new game.
/* 025B9A90 */
void d_save_cpp::dSv_info_c::reinit() {

    u8 hold[17];
    u32 b = svbase(); /* kept in a register across the getEventReg calls */
    for (u32 i = 0; i < 17; i++) hold[i] = saveGetEventReg(gabi::ea(&view<SaveGlobal>(b)->mInfo.mSavedata.mEvent), (u16)l_holdEventReg[i]);
    b = svbase();
    u8 clearCount = (u8)savePlayer(b).mInfo.mClearCount;
    u8 hd0 = (u8)view<HdProgress>(saveHdProgress(gabi::ea(view<SaveGlobal>(b)->mHdSave)))->mFirst;
    b = svbase();
    u8 hd3 = (u8)view<HdProgress>(saveHdProgress(gabi::ea(view<SaveGlobal>(b)->mHdSave)))->mSecond;
    b = svbase();
    u8 vib = (u8)savePlayer(b).mConfig.mVibration;
    u8 sound = (u8)savePlayer(b).mConfig.mSoundMode;
    u32 pic = savePictureInfo(gabi::ea(view<SaveGlobal>(b)->mHdSave));
    u8 pictureNum = (u8)view<HdPictureInfo>(pic)->mCount;
    u8 reg89 = saveGetEventReg(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), 0x89FF);

    init();

    for (u32 i = 0; i < 17; i++) saveSetEventReg(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), (u16)l_holdEventReg[i], hold[i]);
    for (u32 i = 0; i < 5; i++) saveOnEventBit(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), (u16)l_onEventBit[i]);
    saveSetEventReg(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), 0xC407, 7);
    savePlayer().mInfo.mClearCount = clearCount;
    view<dSv_memBit_c>(gabi::ea(&saveBase()->mInfo.mSavedata.mMemory[dSv_save_c::STAGE_MISC].mMembit))->onTbox(0);
    view<dSv_memBit_c>(gabi::ea(&saveBase()->mInfo.mSavedata.mMemory[dSv_save_c::STAGE_SEA].mMembit))->onSwitch(0x47);
    view<dSv_memBit_c>(gabi::ea(&saveBase()->mInfo.mSavedata.mMemory[dSv_save_c::STAGE_SEA].mMembit))->onSwitch(0x5E);
    u32 p = saveHdProgress(gabi::ea(saveBase()->mHdSave));
    if (hd0 < 2) view<HdProgress>(p)->mFirst = hd0;
    p = saveHdProgress(gabi::ea(saveBase()->mHdSave));
    if (hd3 < 2) view<HdProgress>(p)->mSecond = hd3;
    savePlayer().mConfig.mSoundMode = sound;
    savePlayer().mConfig.mVibration = vib;
    savePlayer().mInfo.mRandomSalvagePoint = 3;
    savePlayer().mPlayerItem.mItems[dInvSlot_CAMERA_e] = item_data::dItemNo_DELUXE_PICTO_BOX_e;
    u32 pl = play();
    playInfo(pl)->play.mItemNo = item_data::dItemNo_DELUXE_PICTO_BOX_e;
    playInfo(pl)->play.mItemSlot = dInvSlot_CAMERA_e;
    view<dSv_player_get_item_c>(gabi::ea(&savePlayer().mGetItem))->onItem(dInvSlot_CAMERA_e, 0);
    view<dSv_player_get_item_c>(gabi::ea(&savePlayer().mGetItem))->onItem(dInvSlot_CAMERA_e, 1);
    pic = savePictureInfo(gabi::ea(saveBase()->mHdSave));
    savePictureCount(pic, pictureNum);
    saveSetEventReg(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), 0x89FF, reg89);
}

/* 025B9C9C */
void d_save_cpp::dSv_info_c::getSave(u32 stageNo) {
    if (stageNo >= dSv_save_c::STAGE_MAX) saveAssert("d_save.cpp", 0xD77, "0 <= i_stageNo && i_stageNo < dSv_save_c::STAGE_MAX");
    MemoryWords& src = memoryWords(&indexed(mSavedata.mMemory, stageNo));
    MemoryWords& dst = memoryWords(&mMemory);
    for (u32 i = 0; i < 9; i++) dst.mWords[i] = src.mWords[i];
}

/* 025B9D24 */
void d_save_cpp::dSv_info_c::putSave(u32 stageNo) {
    if (stageNo >= dSv_save_c::STAGE_MAX) saveAssert("d_save.cpp", 0xD87, "0 <= i_stageNo && i_stageNo < dSv_save_c::STAGE_MAX");
    u32 tmp[9];
    for (u32 i = 0; i < 9; i++) tmp[i] = memoryWords(&mMemory).mWords[i];
    MemoryWords& dst = memoryWords(&indexed(mSavedata.mMemory, stageNo));
    for (u32 i = 0; i < 9; i++) dst.mWords[i] = tmp[i];
}

/* 025B9DD8 */
s32 d_save_cpp::dSv_info_c::createZone(u32 roomNo) {
    ::dSv_zone_c* zone_p = mZone;
    for (s32 i = 0; i < 32; i++) {
        if ((s8)zone_p->mRoomNo < 0) {
            view<dSv_zone_c>(gabi::ea(zone_p))->init(roomNo);
            return i;
        }
        zone_p++;
    }
    return -1;
}

/* switch numbers: memory 0..0x7F, dungeon 0x80..0xBF, zone 0xC0..0xEF */
/* 025B9E38 */
void d_save_cpp::dSv_info_c::onSwitch(s32 no, s32 roomNo) {
    if (no == -1 || no == 0xFF) return;
    if ((u32)no >= MEMORY_SWITCH + DAN_SWITCH + ZONE_SWITCH) saveAssert("d_save.cpp", 0xDBF, "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255");
    if (no < MEMORY_SWITCH) {
        view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->onSwitch(no);
    } else if (no < MEMORY_SWITCH + DAN_SWITCH) {
        view<dSv_danBit_c>(gabi::ea(&mDan))->onSwitch(no - MEMORY_SWITCH);
    } else {
        if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xDD0, "0 <= i_roomNo && i_roomNo < 64");
        s32 zoneNo = roomZoneNo((u32)roomNo);
        if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xDD2, "0 <= zoneId && zoneId < ZONE_MAX");
        view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->onSwitch(no - (MEMORY_SWITCH + DAN_SWITCH));
    }
}

/* 025B9F7C */
void d_save_cpp::dSv_info_c::offSwitch(s32 no, s32 roomNo) {
    if (no == -1 || no == 0xFF) return;
    if ((u32)no >= MEMORY_SWITCH + DAN_SWITCH + ZONE_SWITCH) saveAssert("d_save.cpp", 0xDE4, "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255");
    if (no < MEMORY_SWITCH) {
        view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->offSwitch(no);
    } else if (no < MEMORY_SWITCH + DAN_SWITCH) {
        view<dSv_danBit_c>(gabi::ea(&mDan))->offSwitch(no - MEMORY_SWITCH);
    } else {
        if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xDF5, "0 <= i_roomNo && i_roomNo < 64");
        s32 zoneNo = roomZoneNo((u32)roomNo);
        if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xDF7, "0 <= zoneNo && zoneNo < ZONE_MAX");
        view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->offSwitch(no - (MEMORY_SWITCH + DAN_SWITCH));
    }
}

/* 025BA0C0 */
BOOL d_save_cpp::dSv_info_c::isSwitch(s32 no, s32 roomNo) {
    if (no == -1 || no == 0xFF) return FALSE;
    if (no < MEMORY_SWITCH) return view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->isSwitch(no);
    if (no < MEMORY_SWITCH + DAN_SWITCH) return view<dSv_danBit_c>(gabi::ea(&mDan))->isSwitch(no - MEMORY_SWITCH);
    if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xE25, "0 <= i_roomNo && i_roomNo < 64");
    s32 zoneNo = roomZoneNo((u32)roomNo);
    if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xE27, "0 <= zoneNo && zoneNo < ZONE_MAX");
    return view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->isSwitch(no - (MEMORY_SWITCH + DAN_SWITCH));
}

/* 025BA20C */
BOOL d_save_cpp::dSv_info_c::revSwitch(s32 no, s32 roomNo) {
    if (no == -1 || no == 0xFF) return FALSE;
    if ((u32)no >= MEMORY_SWITCH + DAN_SWITCH + ZONE_SWITCH) saveAssert("d_save.cpp", 0xE38, "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255");
    if (no < MEMORY_SWITCH) return view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->revSwitch(no);
    if (no < MEMORY_SWITCH + DAN_SWITCH) return view<dSv_danBit_c>(gabi::ea(&mDan))->revSwitch(no - MEMORY_SWITCH);
    if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xE48, "0 <= i_roomNo && i_roomNo < 64");
    s32 zoneNo = roomZoneNo((u32)roomNo);
    if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xE4A, "0 <= zoneNo && zoneNo < ZONE_MAX");
    return view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->revSwitch(no - (MEMORY_SWITCH + DAN_SWITCH));
}

/* item numbers: memory 0..0x3F, zone 0x40..0x4F */
/* 025BA384 */
void d_save_cpp::dSv_info_c::onItem(s32 no, s32 roomNo) {
    if (no == -1 || no == 0x7F) return;
    if ((u32)no >= MEMORY_ITEM + ZONE_ITEM) saveAssert("d_save.cpp", 0xE5B, "(0 <= i_no && i_no < (MEMORY_ITEM+ZONE_ITEM)) || i_no == -1 || i_no == 127");
    if (no < MEMORY_ITEM) {
        view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->onItem(no);
    } else {
        if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xE67, "0 <= i_roomNo && i_roomNo < 64");
        s32 zoneNo = roomZoneNo((u32)roomNo);
        if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xE69, "0 <= zoneNo && zoneNo < ZONE_MAX");
        view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->onItem(no - MEMORY_ITEM);
    }
}

/* 025BA494 */
BOOL d_save_cpp::dSv_info_c::isItem(s32 no, s32 roomNo) {
    if (no == -1 || no == 0x7F) return FALSE;
    if ((u32)no >= MEMORY_ITEM + ZONE_ITEM) saveAssert("d_save.cpp", 0xE9B, "(0 <= i_no && i_no < (MEMORY_ITEM+ZONE_ITEM)) || i_no == -1 || i_no == 127");
    if (no < MEMORY_ITEM) return view<dSv_memBit_c>(gabi::ea(&mMemory.mMembit))->isItem(no);
    if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xEA6, "0 <= i_roomNo && i_roomNo < 64");
    s32 zoneNo = roomZoneNo((u32)roomNo);
    if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xEA8, "0 <= zoneNo && zoneNo < ZONE_MAX");
    return view<dSv_zoneBit_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneBit))->isItem(no - MEMORY_ITEM);
}

/* 025BA5D4 */
void d_save_cpp::dSv_info_c::onActor(s32 id, s32 roomNo) {
    if (id == -1 || (u32)id == 0xFFFF || roomNo == -1) return;
    if ((u32)id >= 0x200 || (u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xEDC, "(0 <= i_id && i_id < dSv_zoneActor_c::ACTOR_MAX) && (0 <= i_roomNo && i_roomNo < 64)");
    s32 zoneNo = roomZoneNo((u32)roomNo);
    if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xEDF, "0 <= zoneNo && zoneNo < ZONE_MAX");
    view<dSv_zoneActor_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneActor))->on(id);
}

/* 025BA6A4 */
BOOL d_save_cpp::dSv_info_c::isActor(s32 id, s32 roomNo) {
    if (id == -1 || (u32)id == 0xFFFF || roomNo == -1) return FALSE;
    if ((u32)id >= 0x200) saveAssert("d_save.cpp", 0xF10, "0 <= i_id && i_id < dSv_zoneActor_c::ACTOR_MAX");
    if ((u32)roomNo >= ROOM_MAX) saveAssert("d_save.cpp", 0xF12, "0 <= i_roomNo && i_roomNo < 64");
    s32 zoneNo = roomZoneNo((u32)roomNo);
    if ((u32)zoneNo >= ZONE_MAX) saveAssert("d_save.cpp", 0xF14, "0 <= zoneNo && zoneNo < ZONE_MAX");
    return view<dSv_zoneActor_c>(gabi::ea(&indexed(mZone, (u32)zoneNo).mZoneActor))->is(id);
}

/* the card layout: (save base offset, size) per block, status c and memory handled apart */
struct CardBlock { u32 mOffset, mSize; };
static constexpr CardBlock kCardHead[] = {
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mPlayerStatusA), sizeof(::dSv_player_status_a_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mPlayerStatusB), sizeof(::dSv_player_status_b_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mReturnPlace), sizeof(::dSv_player_return_place_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mPlayerItem), sizeof(::dSv_player_item_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mGetItem), sizeof(::dSv_player_get_item_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mItemRecord), sizeof(::dSv_player_item_record_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mItemMax), sizeof(::dSv_player_item_max_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mBagItem), sizeof(::dSv_player_bag_item_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mGetBagItem), sizeof(::dSv_player_get_bag_item_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mBagItemRecord), sizeof(::dSv_player_bag_item_record_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mCollect), sizeof(::dSv_player_collect_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mMap), sizeof(::dSv_player_map_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mInfo), sizeof(::dSv_player_info_c)},
    {(u32)offsetof(SaveGlobal, mInfo.mSavedata.mPlayer.mConfig), sizeof(::dSv_player_config_c)},
};

// HD: always selects sound mode 2 after loading the packed player blocks.
/* 025BA7B0 */
s32 d_save_cpp::dSv_info_c::card_to_memory(u8* card, s32 dataNum) {
    u32 start = gabi::ea(card) + (u32)dataNum * CARD_SLOT_SIZE;
    u32 buf = start;
    for (const CardBlock& c : kCardHead) {
        saveCopy(svbase() + c.mOffset, buf, c.mSize);
        buf += c.mSize;
    }
    savePlayer().mConfig.mSoundMode = 2;
    saveAudioOutput(2);
    saveCopy(gabi::ea(&saveBase()->mInfo.mSavedata.mPlayer.mPriest), buf, 0x10); /* priest */
    buf += 0x10;
    for (u32 i = 0; i < 4; i++) {
        saveCopy(gabi::ea(&indexed(savePlayer().mStatusC, i)), buf, 0x70); /* status c */
        buf += 0x70;
    }
    saveCopy(gabi::ea(&saveBase()->mInfo.mSavedata.mMemory), buf, 0x240); /* memory [16] */
    buf += 0x240;
    saveCopy(gabi::ea(&saveBase()->mInfo.mSavedata.mOcean), buf, 0x64); /* ocean */
    buf += 0x64;
    saveCopy(gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), buf, 0x100); /* event */
    buf += 0x100;
    saveCopy(gabi::ea(&saveBase()->mInfo.mSavedata.mReserve), buf, 0x50); /* reserve */
    buf += 0x50;
    return (buf - start) > CARD_PAYLOAD_SIZE ? -1 : 0;
}

// HD: leaves the copied IPL timestamp unchanged.
/* 025BA9FC */
s32 d_save_cpp::dSv_info_c::memory_to_card(u8* card, s32 dataNum) {
    u32 start = gabi::ea(card) + (u32)dataNum * CARD_SLOT_SIZE;
    u32 buf = start;
    bool first = true;
    for (const CardBlock& c : kCardHead) {
        saveCopy(buf, svbase() + c.mOffset, c.mSize);
        if (first) {
            dSv_player_status_a_c* status_a = view<dSv_player_status_a_c>(buf);
            if ((u16)status_a->mLife < 12) status_a->mLife = 12;
            first = false;
        }
        buf += c.mSize;
    }
    saveCopy(buf, gabi::ea(&saveBase()->mInfo.mSavedata.mPlayer.mPriest), 0x10);
    buf += 0x10;
    for (u32 i = 0; i < 4; i++) {
        saveCopy(buf, gabi::ea(&indexed(savePlayer().mStatusC, i)), 0x70);
        buf += 0x70;
    }
    saveCopy(buf, gabi::ea(&saveBase()->mInfo.mSavedata.mMemory), 0x240);
    buf += 0x240;
    saveCopy(buf, gabi::ea(&saveBase()->mInfo.mSavedata.mOcean), 0x64);
    buf += 0x64;
    saveCopy(buf, gabi::ea(&saveBase()->mInfo.mSavedata.mEvent), 0x100);
    buf += 0x100;
    saveCopy(buf, gabi::ea(&saveBase()->mInfo.mSavedata.mReserve), 0x50);
    buf += 0x50;
    return (buf - start) > CARD_PAYLOAD_SIZE ? -1 : 0;
}

// HD: serializes this exact stack frame, including stale padding; the player-info block is written as zeros.
/* 025BAC50 */
s32 d_save_cpp::dSv_info_c::initdata_to_card(u8* card, s32 dataNum) {
    gabi::NativeFrame<sizeof(SaveCardFrame)> f;
    SaveCardFrame& frame = *view<SaveCardFrame>(f.sp());
    *view<be<u32>>(f.entry() + 4) = gabi::cpu->lr;
    frame.mBackChain = f.entry();
    frame.mSavedR30 = gabi::cpu->r[30];
    frame.mSavedR28 = gabi::cpu->r[28];
    frame.mSavedR29 = gabi::cpu->r[29];
    frame.mSavedR31 = gabi::cpu->r[31];
    u32 start = gabi::ea(card) + (u32)dataNum * CARD_SLOT_SIZE;
    u32 buf = start;
    auto copy = [&](const auto& object) {
        saveCopy(buf, gabi::ea(&object), (u32)sizeof(object));
        buf += (u32)sizeof(object);
    };
    view<dSv_player_status_a_c>(gabi::ea(&frame.mStatusA))->init();
    copy(frame.mStatusA);
    view<dSv_player_status_b_c>(gabi::ea(&frame.mStatusB))->init();
    copy(frame.mStatusB);
    view<dSv_player_return_place_c>(gabi::ea(&frame.mReturnPlace))->init();
    copy(frame.mReturnPlace);
    view<dSv_player_item_c>(gabi::ea(&frame.mItem))->init();
    copy(frame.mItem);
    view<dSv_player_get_item_c>(gabi::ea(&frame.mGetItem))->init();
    copy(frame.mGetItem);
    view<dSv_player_item_record_c>(gabi::ea(&frame.mItemRecord))->init();
    copy(frame.mItemRecord);
    view<dSv_player_item_max_c>(gabi::ea(&frame.mItemMax))->init();
    copy(frame.mItemMax);
    view<dSv_player_bag_item_c>(gabi::ea(&frame.mBagItem))->init();
    copy(frame.mBagItem);
    view<dSv_player_get_bag_item_c>(gabi::ea(&frame.mGetBagItem))->init();
    copy(frame.mGetBagItem);
    view<dSv_player_bag_item_record_c>(gabi::ea(&frame.mBagItemRecord))->init();
    copy(frame.mBagItemRecord);
    view<dSv_player_collect_c>(gabi::ea(&frame.mCollect))->init();
    copy(frame.mCollect);
    view<dSv_player_map_c>(gabi::ea(&frame.mMap))->init();
    copy(frame.mMap);
    view<dSv_player_info_c>(gabi::ea(&frame.mInfo))->init();
    saveFill(buf, 0, sizeof(frame.mInfo));
    buf += sizeof(frame.mInfo);
    view<dSv_player_config_c>(gabi::ea(&frame.mConfig))->init();
    copy(frame.mConfig);
    view<dSv_player_priest_c>(gabi::ea(&frame.mPriest))->init();
    copy(frame.mPriest);
    view<dSv_player_status_c_c>(gabi::ea(&frame.mStatusC))->init();
    for (u32 i = 0; i < dSv_player_c::PLAYER_STATUS_C_COUNT; i++) copy(frame.mStatusC);
    // HD: one initialized memory record is copied to all sixteen stages.
    view<dSv_memory_c>(gabi::ea(&frame.mMemory))->init();
    view<dSv_memory_c>(gabi::ea(&frame.mMemory))->init();
    for (u32 i = 0; i < dSv_save_c::STAGE_MAX; i++) copy(frame.mMemory);
    view<dSv_ocean_c>(gabi::ea(&frame.mOcean))->init();
    copy(frame.mOcean);
    view<dSv_event_c>(gabi::ea(&frame.mEvent))->init();
    copy(frame.mEvent);
    view<dSv_reserve_c>(gabi::ea(&frame.mReserve))->init();
    copy(frame.mReserve);
    return (buf - start) > CARD_PAYLOAD_SIZE ? -1 : 0;
}

/* 025BAEFC */
static void __sinit_d_save_cpp() {
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
}

}  // namespace d_save_cpp
using d_save_cpp::__sinit_d_save_cpp;

/* WWHD glue
str 10054034 "d_save.cpp" in 025B9C9C
str 10054000 "0 <= i_stageNo && i_stageNo < dSv_save_c::STAGE_MAX" in 025B9C9C
str 10054074 "d_save.cpp" in 025B9D24
str 10054040 "0 <= i_stageNo && i_stageNo < dSv_save_c::STAGE_MAX" in 025B9D24
str 10054080 "d_save.cpp" in 025B9E38
str 100540D0 "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255" in 025B9E38
str 1005408C "0 <= i_roomNo && i_roomNo < 64" in 025B9E38
str 100540AC "0 <= zoneId && zoneId < ZONE_MAX" in 025B9E38
str 1005412C "d_save.cpp" in 025B9F7C
str 1005417C "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255" in 025B9F7C
str 10054138 "0 <= i_roomNo && i_roomNo < 64" in 025B9F7C
str 10054158 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025B9F7C
str 100541D8 "d_save.cpp" in 025BA0C0
str 100541E4 "0 <= i_roomNo && i_roomNo < 64" in 025BA0C0
str 10054204 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA0C0
str 10054228 "d_save.cpp" in 025BA20C
str 10054278 "(0 <= i_no && i_no < (MEMORY_SWITCH+ DAN_SWITCH+ ZONE_SWITCH)) || i_no == -1 || i_no == 255" in 025BA20C
str 10054234 "0 <= i_roomNo && i_roomNo < 64" in 025BA20C
str 10054254 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA20C
str 100542D4 "d_save.cpp" in 025BA384
str 10054324 "(0 <= i_no && i_no < (MEMORY_ITEM+ZONE_ITEM)) || i_no == -1 || i_no == 127" in 025BA384
str 100542E0 "0 <= i_roomNo && i_roomNo < 64" in 025BA384
str 10054300 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA384
str 1005437C "d_save.cpp" in 025BA494
str 100543CC "(0 <= i_no && i_no < (MEMORY_ITEM+ZONE_ITEM)) || i_no == -1 || i_no == 127" in 025BA494
str 10054388 "0 <= i_roomNo && i_roomNo < 64" in 025BA494
str 100543A8 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA494
str 10054424 "d_save.cpp" in 025BA5D4
str 10054454 "(0 <= i_id && i_id < dSv_zoneActor_c::ACTOR_MAX) && (0 <= i_roomNo && i_roomNo < 64)" in 025BA5D4
str 10054430 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA5D4
str 100544B4 "d_save.cpp" in 025BA6A4
str 100544C0 "0 <= i_id && i_id < dSv_zoneActor_c::ACTOR_MAX" in 025BA6A4
str 100544F0 "0 <= i_roomNo && i_roomNo < 64" in 025BA6A4
str 10054510 "0 <= zoneNo && zoneNo < ZONE_MAX" in 025BA6A4
*/
