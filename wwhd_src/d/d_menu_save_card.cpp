// HD SaveMgr card range.
#include "d/d_menu_save.h"
using namespace hdSave;
/* 027219AC */
void SaveMgr::initialCard(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = dComIfGs_ea();
    saveCardApi_initializeSaveSlot(gabi::ea(&gabi::at<SaveGameState>(state)->mSave),
                                   (u32)gabi::at<SaveMgr>(p)->mpSaveData, slot);
    saveCardApi_initializeCardSlot(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), slot);
}
/* 02721A04 */
void SaveMgr::initialCardWrapper(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveCardApi_saveMgrInitialCardCall(p, slot);
}
/* 02721A08 */
void SaveMgr::mode(u32 mode) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum = (u8)(mode);
}
/* 02721A18 */
void SaveMgr::begin(u32 mode) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveCardApi_saveMgrModeCall(p, mode);
    if (!(u32)gabi::at<SaveMgr>(p)->mCommand) {
        u32 thread = (u32)gabi::at<SaveMgr>(p)->mpThread;
        gabi::at<SaveMgr>(p)->mCommand = (u32)(1);
        gabi::call_ptr<void>((u32)gabi::at<WorkerVtable>(gabi::at<Worker>(thread)->mVtable)->postMessage,
                             thread, 1u, 1u);
    }
}
/* 02721A68 */
void saveMgrSessionCounter() {
    u32 state = dComIfGs_ea();
    u16 n = (u16)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mInfo.field_0x10;
    if (n < MaxSaveCount)
        gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mInfo.field_0x10 = (u16)(n + 1);
}
/* 02721A8C */
void saveMgrStampTime() {
    u32 state = dComIfGs_ea();
    saveCardApi_getSaveSystemTime();
    u32 high = cpu->r[3], low = cpu->r[4];
    gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mPlayerStatusB.mDateIPL[1] = (u32)(low);
    gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mPlayerStatusB.mDateIPL[0] = (u32)(high);
}
/* 02721AFC */
void SaveMgr::checksumSlot(u32 data, u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 block = data + slot * sizeof(SlotData);
    u32 sum = saveCardApi_saveMgrChecksumCall(p, block, 0xA8Cu);
    u32 complement = cpu->r[4];
    gabi::at<SlotData>(block)->mChecksum = (u32)(sum);
    gabi::at<SlotData>(block)->mChecksumComplement = (u32)(complement);
}
/* 02721B34 */
void SaveMgr::syncSlot(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo);
    u32 a = saveCardApi_getCurrentCardPlayer(state);
    u32 b = saveCardApi_getCardPlayer(state, slot);
    saveCardApi_copyCardPlayer(b, a);
    a = saveCardApi_getCurrentCardStatus(state);
    b = saveCardApi_getCardStatus(state, slot);
    saveCardApi_copyCardStatus(b, a);
    a = saveCardApi_getCurrentCardEvents(state);
    b = saveCardApi_getCardEvents(state, slot);
    saveCardApi_copyCardEvents(b, a);
    a = saveCardApi_getCurrentCardMap(state);
    b = saveCardApi_getCardMap(state, slot);
    saveCardApi_copyCardMap(b, a);
    a = saveCardApi_getCurrentCardPictures(state);
    b = saveCardApi_getCardPictures(state, slot);
    saveCardApi_copyPictureStore(b, a);
    a = saveCardApi_getCurrentCardPictures(state);
    saveCardApi_disablePictureSlots(a);
    a = saveCardApi_getCurrentCardPlayLog(state);
    b = saveCardApi_getCardPlayLog(state, slot);
    saveCardApi_copySavePlayLog(b, a);
}
/* 02721C44 */
void SaveMgr::saveCard(u32 slot, u32 update) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (update) {
        u32 g = saveCardApi_getGameInfoAddress() + 0x5150;
        u32 q = gabi::call_ptr<u32>(readWord(readWord(g), 0x15C), g);
        u32 index = (gabi::load<u8>(q + 9) >> 1) & 0x7F;
        if (index < 16)
            saveCardApi_markVisitedStage(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mSave), index);
        saveCardApi_saveMgrSessionCounterCall(p);
        saveCardApi_saveMgrStampTimeCall();
        saveCardApi_updateSaveGameState();
    }
    s32 result = saveCardApi_storeSaveInfo(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mSave),
                                           (u32)gabi::at<SaveMgr>(p)->mpSaveData, slot);
    if (result != -1) {
        for (u32 i = 0; i < 3; i++)
            saveCardApi_saveMgrChecksumSlotCall(p, (u32)gabi::at<SaveMgr>(p)->mpSaveData, i);
        saveCardApi_saveMgrSyncSlotCall(p, slot);
    }
}
/* 02721D18 */
void SaveMgr::end() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (!(u32)gabi::at<SaveMgr>(p)->mCommand) {
        u32 thread = (u32)gabi::at<SaveMgr>(p)->mpThread;
        gabi::at<SaveMgr>(p)->mCommand = (u32)(2);
        gabi::call_ptr<void>((u32)gabi::at<WorkerVtable>(gabi::at<Worker>(thread)->mVtable)->postMessage,
                             thread, 2u, 1u);
    }
}
/* 02721AC4 */
Pair32 SaveMgr::checksum(u32 data, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 sum = 0, complement = 0;
    for (u32 i = 0; i < count; i++) {
        u32 b = gabi::load<u8>(data + i);
        sum += b;
        complement += ~b;
    }
    return {sum, complement};
}
/* 02721730 */
void saveMgrRefreshEquipment() {
    if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 0u, 3u))
        saveCardApi_setSelectedEquipment(0u, 0x3Eu);
    else if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 0u, 2u))
        saveCardApi_setSelectedEquipment(0u, 0x3Au);
    else if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 0u, 1u))
        saveCardApi_setSelectedEquipment(0u, 0x39u);
    else if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 0u, 0u))
        saveCardApi_setSelectedEquipment(0u, 0x38u);
    if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 1u, 1u))
        saveCardApi_setSelectedEquipment(1u, 0x3Cu);
    else if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 1u, 0u))
        saveCardApi_setSelectedEquipment(1u, 0x3Bu);
    if (saveCardApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 2u, 0u))
        saveCardApi_setSelectedEquipment(2u, 0x28u);
}
/* 02721880 */
void SaveMgr::refreshGame() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveCardApi_saveMgrStartupCall(p);
    saveCardApi_saveMgrRefreshEquipmentCall(p);
    for (u32 n : {1u, 3u, 5u, 6u, 7u})
        saveCardApi_markOwnedChart(dComIfGs_ea() + 0xE4, n);
}
/* 02721908 */
void SaveMgr::loadCard(u32 slot, u32 resetItems) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (slot >= 3)
        return;
    saveCardApi_initializeSaveInfo(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mSave));
    if (resetItems)
        saveCardApi_initializePlayState(saveCardApi_getGameInfoAddress() + 0x12A0);
    gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum = (u8)(slot);
    saveCardApi_loadSaveInfo(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mSave),
                             (u32)gabi::at<SaveMgr>(p)->mpSaveData, slot);
    saveCardApi_saveMgrSyncCall(p, slot);
    saveCardApi_saveMgrRefreshGameCall(p);
}
/* WWHD glue

ext 025200D4 u32 saveCardApi_getGameInfoAddress()
ext 025217F8 void saveCardApi_updateSaveGameState()
ext 02522398 void saveCardApi_setSelectedEquipment(u32, u32)
ext 025248BC void saveCardApi_initializePlayState(u32)
ext 025B7A2C u32 saveCardApi_checkCollectFlag(u32, u32, u32)
ext 025B82A0 void saveCardApi_markOwnedChart(u32, u32)
ext 025B9A18 void saveCardApi_initializeSaveInfo(u32)
ext 025B9D24 void saveCardApi_markVisitedStage(u32, u32)
ext 025BA7B0 void saveCardApi_loadSaveInfo(u32, u32, u32)
ext 025BA9FC s32 saveCardApi_storeSaveInfo(u32, u32, u32)
ext 025BAC50 void saveCardApi_initializeSaveSlot(u32, u32, u32)
ext 0271F914 void saveCardApi_copyCardEvents(u32, u32)
ext 0271FAC0 void saveCardApi_copyCardStatus(u32, u32)
ext 0271FCB4 void saveCardApi_copyCardPlayer(u32, u32)
ext 0271FF4C void saveCardApi_initializeCardSlot(u32, u32)
ext 027200A0 u32 saveCardApi_getCardPlayer(u32, u32)
ext 027200D0 u32 saveCardApi_getCurrentCardPlayer(u32)
ext 027200D8 u32 saveCardApi_getCardStatus(u32, u32)
ext 027200F4 u32 saveCardApi_getCurrentCardStatus(u32)
ext 027200FC u32 saveCardApi_getCardEvents(u32, u32)
ext 02720118 u32 saveCardApi_getCurrentCardEvents(u32)
ext 02720120 u32 saveCardApi_getCardPictures(u32, u32)
ext 02720144 u32 saveCardApi_getCurrentCardPictures(u32)
ext 02720180 u32 saveCardApi_getCardMap(u32, u32)
ext 0272019C u32 saveCardApi_getCurrentCardMap(u32)
ext 027201A4 u32 saveCardApi_getCardPlayLog(u32, u32)
ext 027201DC u32 saveCardApi_getCurrentCardPlayLog(u32)
ext 027208F4 void saveCardApi_copyCardMap(u32, u32)
ext 02720D34 void saveCardApi_saveMgrSyncCall(u32, u32)
ext 02720E34 void saveCardApi_saveMgrStartupCall(u32)
ext 02721730 void saveCardApi_saveMgrRefreshEquipmentCall(u32)
ext 02721880 void saveCardApi_saveMgrRefreshGameCall(u32)
ext 027219AC void saveCardApi_saveMgrInitialCardCall(u32, u32)
ext 02721A08 void saveCardApi_saveMgrModeCall(u32, u32)
ext 02721A68 void saveCardApi_saveMgrSessionCounterCall(u32)
ext 02721A8C void saveCardApi_saveMgrStampTimeCall()
ext 02721AC4 u32 saveCardApi_saveMgrChecksumCall(u32, u32, u32)
ext 02721AFC void saveCardApi_saveMgrChecksumSlotCall(u32, u32, u32)
ext 02721B34 void saveCardApi_saveMgrSyncSlotCall(u32, u32)
ext 02726044 void saveCardApi_disablePictureSlots(u32)
ext 027262BC void saveCardApi_copyPictureStore(u32, u32)
ext 02726BC8 void saveCardApi_copySavePlayLog(u32, u32)
ext C0009CD8 void saveCardApi_getSaveSystemTime()
*/
