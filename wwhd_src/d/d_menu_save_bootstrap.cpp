// HD SaveMgr bootstrap range.
#include "d/d_menu_save.h"
using namespace hdSave;
/* 027209E0 */
u32 saveMgrCtor(u32 p) {
    if (!p) {
        p = saveBootstrapApi_allocateSaveObject(0x6Cu);
        if (!p)
            return 0;
    }
    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(0);
    gabi::at<SaveMgr>(p)->mVtable = (u32)(SaveMgrVtableAddress);
    gabi::at<SaveMgr>(p)->mpHeap = (u32)(0);
    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0);
    gabi::at<SaveMgr>(p)->mpThread = (u32)(0);
    gabi::at<SaveMgr>(p)->mCommand = (u32)(0);
    saveBootstrapApi_initializeSaveCriticalSection(p + 0x28);
    gabi::at<SaveMgr>(p)->mpWorkBuffer = (u32)(0);
    gabi::at<SaveMgr>(p)->mpSaveData = (u32)(0);
    return p;
}
/* 02720A5C */
u32 saveMgrGet(u32 heap) {
    u32 p = readWord(0x101F852Cu);
    if (p)
        return p;
    u32 base = saveBootstrapApi_allocateSaveAligned(0x6Cu, heap, 4u), thread = base + 4;
    if (thread) {
        saveBootstrapApi_constructSaveDisposer(thread, heap, 3u);
        gabi::store<u32>(thread + 0xC, SaveDisposerVtableAddress);
    }
    gabi::store<u32>(0x101F8530u, thread);
    p = base;
    if (p)
        p = saveBootstrapApi_saveMgrCtorCall(p);
    gabi::store<u32>(0x101F852Cu, p);
    return p;
}
/* 02720AFC */
u32 saveMgrAllocate(u32 size, u32 heap, u32 alignment) {
    return saveBootstrapApi_allocateSaveHeap(size, heap, alignment);
}

/* 02720B34 */
void SaveMgr::init(u32 heap) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<SaveName> name;
    name->name = gabi::guest_addr("SaveMgr");
    name->owner = SafeStringVtableAddress;
    u32 h = saveBootstrapApi_createSaveHeap(SaveHeapSize, gabi::ea(name.get()), heap, 1u, 0u);
    u32 buffer = saveBootstrapApi_saveMgrAllocateCall(SaveBufferSize, h, FileAlignment);
    gabi::at<SaveMgr>(p)->mpSaveData = (u32)(buffer);
    saveBootstrapApi_fillSaveBytes(buffer, 0u, SaveBufferSize);
    buffer = saveBootstrapApi_saveMgrAllocateCall(WorkBufferSize, h, FileAlignment);
    gabi::at<SaveMgr>(p)->mpWorkBuffer = (u32)(buffer);
    saveBootstrapApi_fillSaveBytes(buffer, 0u, WorkBufferSize);
    if (!readWord(0x101F8B08u))
        saveBootstrapApi_initializeSaveFileDevice((u32)gabi::at<SaveMgr>(p)->mpHeap);
    u32 thread = saveBootstrapApi_allocateSaveHeap(0x98u, h, 4u);
    if (thread) {
        name->name = gabi::guest_addr("SaveMgrThread");
        u32 record = saveBootstrapApi_allocateSaveHeap(0x10u, h, 4u);
        if (record) {
            gabi::at<WorkerCallback>(record)->mpObject = p;
            gabi::at<WorkerCallback>(record)->mCallback.mThisAdjustment = 0;
            gabi::at<WorkerCallback>(record)->mCallback.mVirtualIndex = -1;
            gabi::at<WorkerCallback>(record)->mCallback.mTarget = WorkerEventAddress;
            gabi::at<WorkerCallback>(record)->mVtable = WorkerCallbackVtableAddress;
        }
        thread = saveBootstrapApi_constructSaveWorker(thread, gabi::ea(name.get()), record, h,
                                                      readWord(0x101F8B9Cu), 0u, 0x7FFFFFFFu, 0x5000u, 0x20u);
    }
    gabi::at<SaveMgr>(p)->mpThread = (u32)(thread);
    u32 vt = (u32)gabi::at<Worker>(thread)->mVtable;
    gabi::call_ptr<void>((u32)gabi::at<WorkerVtable>(vt)->start, thread, cpu->r[4], cpu->r[5], cpu->r[6],
                         cpu->r[7], cpu->r[8], cpu->r[9], vt, cpu->f[1].ps0, cpu->f[2].ps0, cpu->f[3].ps0,
                         cpu->f[4].ps0, cpu->f[5].ps0, cpu->f[6].ps0, cpu->f[7].ps0, cpu->f[8].ps0);
    gabi::at<SaveMgr>(p)->mpHeap = (u32)(h);
}
/* 02720D34 */
void SaveMgr::sync(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), b, a;
    b = saveBootstrapApi_getCardPlayer(state, slot);
    a = saveBootstrapApi_getCurrentCardPlayer(state);
    saveBootstrapApi_copyCardPlayer(a, b);
    b = saveBootstrapApi_getCardStatus(state, slot);
    a = saveBootstrapApi_getCurrentCardStatus(state);
    saveBootstrapApi_copyCardStatus(a, b);
    b = saveBootstrapApi_getCardEvents(state, slot);
    a = saveBootstrapApi_getCurrentCardEvents(state);
    saveBootstrapApi_copyCardEvents(a, b);
    b = saveBootstrapApi_getCardMap(state, slot);
    a = saveBootstrapApi_getCurrentCardMap(state);
    saveBootstrapApi_copyCardMap(a, b);
    b = saveBootstrapApi_getCardPictures(state, slot);
    a = saveBootstrapApi_getCurrentCardPictures(state);
    saveBootstrapApi_copyPictureStore(a, b);
    b = saveBootstrapApi_getCardPlayLog(state, slot);
    a = saveBootstrapApi_getCurrentCardPlayLog(state);
    saveBootstrapApi_copySavePlayLog(a, b);
}

static u32 saveItemOffset(u32 index) {
    if (index < SaveItemCount)
        return offsetof(SaveGameState, mSave.mSavedata.mPlayer.mPlayerItem.mItems) + index - 0;
    if (index >= 0x18 && index < 0x20)
        return offsetof(SaveGameState, mSave.mSavedata.mPlayer.mBagItem.mBeast) + index - 24;
    if (index >= 0x24 && index < 0x2C)
        return offsetof(SaveGameState, mSave.mSavedata.mPlayer.mBagItem.mBait) + index - 36;
    if (index >= 0x30 && index < 0x38)
        return offsetof(SaveGameState, mSave.mSavedata.mPlayer.mBagItem.mReserve) + index - 48;
    return 0;
}
static u8 saveSelectedItem(u32 state, u32 index) {
    u32 offset = saveItemOffset(index);
    return offset ? gabi::load<u8>(state + offset) : 0xFF;
}
static void saveRefreshSelection(u32 selector, u32 gameOffset) {
    u32 before = gabi::load<u8>(dComIfGs_ea() + selector);
    u32 game = saveBootstrapApi_getGameInfoAddress() + 0x12A0;
    if (before == 0xFF) {
        gabi::store<u8>(game + gameOffset, 0xFF);
        return;
    }
    u32 state = dComIfGs_ea();
    u32 index = gabi::load<u8>(state + selector);
    gabi::store<u8>(game + gameOffset, saveSelectedItem(state, index));
    state = dComIfGs_ea();
    index = gabi::load<u8>(state + selector);
    if (saveSelectedItem(state, index) == 0xFF)
        gabi::store<u8>(state + selector, 0xFF);
}
/* 02720E34 */
void SaveMgr::startup() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < SaveItemCount; i++) {
        if (!saveBootstrapApi_checkItemFlag(dComIfGs_ea() + 0x71, i, 0u))
            continue;
        if (i == 14 || i == 15 || i == 16 || i == 17)
            continue;
        u32 item = saveBootstrapApi_getItemForSaveIndex(i);
        gabi::store<u8>(dComIfGs_ea() + 0x5C + i, item);
        if (i == 2) {
            gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[3] =
                (u8)(2);
            saveRefreshSelection(0x2C, 0x491E);
        }
    }
    if (saveBootstrapApi_checkItemFlag(dComIfGs_ea() + 0x71, 1u, 1u)) {
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerItem.mItems[1] = (u8)(0x77);
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[4] =
            (u8)(1);
        saveRefreshSelection(0x2D, 0x491F);
    }
    if (saveBootstrapApi_checkItemFlag(dComIfGs_ea() + 0x71, 8u, 1u))
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerItem.mItems[8] = (u8)(0x26);
    if (saveBootstrapApi_checkItemFlag(dComIfGs_ea() + 0x71, 12u, 2u)) {
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] = (u8)(0x36);
        for (u32 i = 0; i < 3; i++) {
            u32 game = saveBootstrapApi_getGameInfoAddress();
            if (*gabi::at<be<u8>>(gabi::ea(gabi::at<dComIfG_inf_c>(game)->play.mSelectItem) + i) == 0x35)
                saveRefreshSelection(0x29 + i, 0x491B + i);
        }
        return;
    }
    if (saveBootstrapApi_checkItemFlag(dComIfGs_ea() + 0x71, 12u, 1u)) {
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerItem.mItems[12] = (u8)(0x35);
        for (u32 i = 0; i < 3; i++) {
            u32 game = saveBootstrapApi_getGameInfoAddress();
            if (*gabi::at<be<u8>>(gabi::ea(gabi::at<dComIfG_inf_c>(game)->play.mSelectItem) + i) == 0x27)
                saveRefreshSelection(0x29 + i, 0x491B + i);
        }
    }
}
/* WWHD glue
str 10140860 "SaveMgr" in 02720B34
str 10140868 "SaveMgrThread" in 02720B34
ext 025200D4 u32 saveBootstrapApi_getGameInfoAddress()
ext 025B5D40 u32 saveBootstrapApi_checkItemFlag(u32, u32, u32)
ext 025DB5AC u32 saveBootstrapApi_getItemForSaveIndex(u32)
ext 0271F914 void saveBootstrapApi_copyCardEvents(u32, u32)
ext 0271FAC0 void saveBootstrapApi_copyCardStatus(u32, u32)
ext 0271FCB4 void saveBootstrapApi_copyCardPlayer(u32, u32)
ext 027200A0 u32 saveBootstrapApi_getCardPlayer(u32, u32)
ext 027200D0 u32 saveBootstrapApi_getCurrentCardPlayer(u32)
ext 027200D8 u32 saveBootstrapApi_getCardStatus(u32, u32)
ext 027200F4 u32 saveBootstrapApi_getCurrentCardStatus(u32)
ext 027200FC u32 saveBootstrapApi_getCardEvents(u32, u32)
ext 02720118 u32 saveBootstrapApi_getCurrentCardEvents(u32)
ext 02720120 u32 saveBootstrapApi_getCardPictures(u32, u32)
ext 02720144 u32 saveBootstrapApi_getCurrentCardPictures(u32)
ext 02720180 u32 saveBootstrapApi_getCardMap(u32, u32)
ext 0272019C u32 saveBootstrapApi_getCurrentCardMap(u32)
ext 027201A4 u32 saveBootstrapApi_getCardPlayLog(u32, u32)
ext 027201DC u32 saveBootstrapApi_getCurrentCardPlayLog(u32)
ext 027208F4 void saveBootstrapApi_copyCardMap(u32, u32)
ext 027209E0 u32 saveBootstrapApi_saveMgrCtorCall(u32)
ext 02720AFC u32 saveBootstrapApi_saveMgrAllocateCall(u32, u32, u32)
ext 027262BC void saveBootstrapApi_copyPictureStore(u32, u32)
ext 02726BC8 void saveBootstrapApi_copySavePlayLog(u32, u32)
ext 0273AD10 u32 saveBootstrapApi_allocateSaveObject(u32)
ext 0273B050 u32 saveBootstrapApi_allocateSaveHeap(u32, u32, u32)
ext 0273B0D4 u32 saveBootstrapApi_allocateSaveAligned(u32, u32, u32)
ext 02741A20 void saveBootstrapApi_initializeSaveFileDevice(u32)
ext 02752B0C void saveBootstrapApi_constructSaveDisposer(u32, u32, u32)
ext 02753004 u32 saveBootstrapApi_createSaveHeap(u32, u32, u32, u32, u32)
ext 0275FBD8 u32 saveBootstrapApi_constructSaveWorker(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 02760084 void saveBootstrapApi_initializeSaveCriticalSection(u32)
ext C0009990 void saveBootstrapApi_fillSaveBytes(u32, u32, u32)
*/
