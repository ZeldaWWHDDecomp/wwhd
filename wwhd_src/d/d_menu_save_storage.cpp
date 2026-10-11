// HD SaveMgr platform storage adapters.
#include "d/d_menu_save.h"
using namespace hdSave;
/* 02724580 */
void SaveMgr::threadEvent(u32 arg, u32 event) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (event == 1)
        saveStorageApi_saveMgrWorkerLoadCall(p, arg, event);
    else if (event == 2)
        saveStorageApi_saveMgrWorkerWriteCall(p, 0u, event);
    else if (event == 3)
        saveStorageApi_saveMgrWorkerWriteCall(p, 1u, event);
    else if (event == 4)
        saveStorageApi_saveMgrWorkerMiiverseDataCall(p, arg, event);
}
/* 027245BC */
u32 SaveMgr::checkSlot(u32 data, u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 block = data + slot * sizeof(SlotData);
    u32 sum = saveStorageApi_saveMgrChecksumCall(p, block, 0xA8Cu), complement = cpu->r[4];
    return sum == (u32)gabi::at<SlotData>(block)->mChecksum &&
           complement == (u32)gabi::at<SlotData>(block)->mChecksumComplement;
}
/* 02724614 */
void SaveMgr::terminateSlotName(u32 data, u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 name = data + slot * sizeof(SlotData) + 0x30;
    u32 temp = saveStorageApi_allocateSaveAligned(8u, (u32)gabi::at<SaveMgr>(p)->mpHeap, 4u);
    saveStorageApi_copySaveBytes(temp, name, 8u, 0u);
    if (gabi::load<u8>(temp + 7)) {
        gabi::store<u8>(temp + 7, 0);
        saveStorageApi_copySaveBytes(name, temp, 8u, 0u);
    }
    saveStorageApi_freeSaveBuffer(temp);
}
/* 027246A0 */
u32 SaveMgr::readSlots(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 result = 0;
    if ((u32)gabi::at<FileHandle>(file)->mHandle) {
        result = saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpSaveData, SaveBufferSize);
        if (result) {
            for (u32 i = 0; i < 3; i++) {
                u32 data = (u32)gabi::at<SaveMgr>(p)->mpSaveData;
                if (!saveStorageApi_saveMgrCheckSlotCall(p, data, i)) {
                    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
                    return 0;
                }
                saveStorageApi_saveMgrTerminateSlotNameCall(p, data, i);
            }
        }
    } else
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-2);
    return result;
}
/* 02724764 */
u32 SaveMgr::readPlayer(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 dest =
            saveStorageApi_getCardPlayer(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u, 0u);
    }
    return 1;
}
/* 0272480C */
u32 SaveMgr::readStatus(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 dest =
            saveStorageApi_getCardStatus(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u, 0u);
    }
    return 1;
}
/* 027248B4 */
u32 SaveMgr::readEvent(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 20u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 dest =
            saveStorageApi_getCardEvents(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 20u, 0u);
    }
    return 1;
}
/* 02724A28 */
u32 SaveMgr::readMap(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 220u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 dest = saveStorageApi_getCardMap(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 220u, 0u);
    }
    return 1;
}
/* 0272495C */
u32 SaveMgr::readName(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 18u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 temp = saveStorageApi_allocateSaveAligned(18u, (u32)gabi::at<SaveMgr>(p)->mpHeap, 4u);
        saveStorageApi_copySaveBytes(temp, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 18u, 0u);
        u32 dest =
            saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_setCardName(dest, temp);
        saveStorageApi_freeSaveBuffer(temp);
    }
    return 1;
}
/* 02724AD0 */
u32 SaveMgr::checkSerial(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u)) {
        u32 expected = readWord(dComIfGs_ea(), 0x1790);
        Local<be<u32>> got;
        *got.get() = 0;
        saveStorageApi_copySaveBytes(gabi::ea(got.get()), (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u, 0u);
        if (expected == (u32)*got.get())
            return 1;
    }
    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
    return 0;
}
/* 02724B74 */
u32 SaveMgr::checkVersion(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u)) {
        u32 expected =
            saveStorageApi_getCardDataVersion(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
        Local<be<u32>> got;
        *got.get() = 0;
        saveStorageApi_copySaveBytes(gabi::ea(got.get()), (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u, 0u);
        if (expected == (u32)*got.get())
            return 1;
    }
    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
    return 0;
}
/* 02724C20 */
u32 SaveMgr::writeSlots(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (!(u32)gabi::at<FileHandle>(file)->mHandle) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-1);
        return 0;
    }
    u32 result = saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpSaveData, SaveBufferSize);
    return readWord(count) == SaveBufferSize ? result : 0;
}
/* 02724C9C */
u32 SaveMgr::writePlayer(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 state = dComIfGs_ea(), slot = (u8)gabi::at<SaveGameState>(state)->mSave.mDataNum;
        u32 dest = saveStorageApi_getCardPlayer(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), i);
        saveStorageApi_setCardPlayerSlot(dest, slot);
        u32 stateNow = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
        u32 src = saveStorageApi_getCardPlayer(gabi::ea(&gabi::at<SaveGameState>(stateNow)->mCardInfo), i);
        saveStorageApi_copySaveBytes(buffer, src, 16u, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u) ||
            readWord(count) != 16u) {
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            return 0;
        }
    }
    return 1;
}
/* 02724D74 */
u32 SaveMgr::writeStatus(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 stateNow = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
        u32 src = saveStorageApi_getCardStatus(gabi::ea(&gabi::at<SaveGameState>(stateNow)->mCardInfo), i);
        saveStorageApi_copySaveBytes(buffer, src, 4u, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u) ||
            readWord(count) != 4u) {
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            return 0;
        }
    }
    return 1;
}
/* 02724E30 */
u32 SaveMgr::writeEvent(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 stateNow = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
        u32 src = saveStorageApi_getCardEvents(gabi::ea(&gabi::at<SaveGameState>(stateNow)->mCardInfo), i);
        saveStorageApi_copySaveBytes(buffer, src, 20u, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 20u) ||
            readWord(count) != 20u) {
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            return 0;
        }
    }
    return 1;
}
/* 02725018 */
u32 SaveMgr::writeMap(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 stateNow = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
        u32 src = saveStorageApi_getCardMap(gabi::ea(&gabi::at<SaveGameState>(stateNow)->mCardInfo), i);
        saveStorageApi_copySaveBytes(buffer, src, 220u, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 220u)) {
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            return 0;
        }
    }
    return 1;
}
/* 027250C8 */
u32 SaveMgr::writeSerial(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveStorageApi_copySaveBytes((u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, dComIfGs_ea() + 0x1790, 4u, 0u);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    return 1;
}
/* 0272516C */
u32 SaveMgr::writeVersion(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveStorageApi_updateCardDataVersion(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    saveStorageApi_copySaveBytes((u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, dComIfGs_ea() + 0x1794, 4u, 0u);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 4u)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    return 1;
}
/* 02725228 */
void SaveDisposer::destroy(u32 flags) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (!p)
        return;
    gabi::store<u32>(p + 12, SaveDisposerVtableAddress);
    if (p == readWord(0x101F8530u)) {
        u32 manager = readWord(0x101F852Cu);
        gabi::store<u32>(0x101F8530u, 0);
        if (manager) {
            gabi::at<SaveMgr>(manager)->mVtable = (u32)(SaveMgrVtableAddress);
            saveStorageApi_destroySaveCriticalSection(manager + 0x28, 2u);
        }
        gabi::store<u32>(0x101F852Cu, 0);
    }
    saveStorageApi_destroySaveDisposer(p, 0u);
    if (flags & 1)
        saveStorageApi_deleteSaveObject(p);
}

static void saveStorageNameVcall(u32 object, u32 arg5, u32 arg6, u32 arg7, u32 arg9, u32 arg10) {
    u32 target = gabi::at<SafeStringVtable>(gabi::at<SaveName>(object)->owner)->assureTermination.get();
    gabi::call_ptr<void>(target, object, cpu->r[4], arg5, arg6, arg7, cpu->r[8], arg9, arg10, cpu->f[1].ps0,
                         cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0, cpu->f[5].ps0, cpu->f[6].ps0,
                         cpu->f[7].ps0, cpu->f[8].ps0);
}
/* 02724EEC */
u32 SaveMgr::writeName(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 firstState = dComIfGs_ea();
        u32 object = saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(firstState)->mCardInfo), i);
        saveStorageNameVcall(object, cpu->r[5], cpu->r[6], cpu->r[7], cpu->r[9], firstState);
        u32 text = (u32)gabi::at<SaveName>(object)->name, length = 0, last = cpu->r[6];
        if (gabi::load<u16>(text)) {
            do {
                length++;
                text += 2;
                if (length > 0x40000) {
                    length = 0;
                    break;
                }
            } while ((last = gabi::load<u16>(text)));
        }
        u32 state = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, bytes = (length + 1) * 2;
        object = saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), i);
        saveStorageNameVcall(object, text, last, 0x40000u, state, (u32)gabi::at<SaveName>(object)->owner);
        saveStorageApi_copySaveBytes(buffer, (u32)gabi::at<SaveName>(object)->name, bytes, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 18u)) {
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            return 0;
        }
    }
    return 1;
}
/* 027252DC */
void saveMgrStaticInit() {
    gabi::store<u32>(0x1049F7C8u, 0);
    gabi::store<u32>(0x1049F7C4u, 0);
    gabi::store<u32>(0x1049F7C0u, 0);
    gabi::store<u32>(0x1049F7BCu, 0);
    saveStorageApi_registerSaveHeaderObject(0x101F8508u);
    u32 negative = readWord(0x10140A98u), positive = readWord(0x10140A9Cu);
    gabi::store<u32>(0x1049F7A0u, negative);
    gabi::store<u32>(0x1049F7A4u, positive);
    saveStorageApi_initializeSaveHeaderByte(0x1049F7B8u);
    saveStorageApi_registerSaveHeaderObject(0x101F8514u);
    saveStorageApi_initializeSaveHeaderFlag(0x1049F7B9u);
    saveStorageApi_registerSaveHeaderObject(0x101F8520u);
    u32 far = readWord(0x10140AA0u), near = readWord(0x10140AA4u);
    gabi::store<u32>(0x1049F7A8u, far);
    gabi::store<u32>(0x1049F7B0u, near);
    gabi::store<u32>(0x1049F7ACu, far);
    gabi::store<u32>(0x1049F7B4u, near);
}
/* 027222E4 */
void SaveMgr::selectSlot(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum = (u8)(slot);
}
/* 027225AC */
void SaveMgr::resetSlot(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveStorageApi_saveMgrSelectSlotCall(p, slot);
    saveStorageApi_saveMgrInitialCardCall(cpu->r[3], cpu->r[4]);
    saveStorageApi_saveMgrLoadCardCall(p, slot, 0u);
    saveStorageApi_saveMgrSaveCardCall(p, slot, 0u);
    saveStorageApi_saveMgrEndCall(p);
}
/* 02722610 */
void SaveMgr::clearPictureStore(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 stage =
        saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), slot);
    saveStorageApi_clearPictureStore(stage);
}
/* 02722640 */
void SaveMgr::clearMiiverseData() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveStorageApi_clearMiiverseDataSaveData(
        gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mMiiverseData));
}
/* 02722654 */
void SaveMgr::clearPictures() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        u32 image =
            saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), i);
        saveStorageApi_clearSavePlayLog(image);
    }
}
/* 027226B4 */
void SaveMgr::clearAll() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 3; i++) {
        saveStorageApi_saveMgrInitialCardCall(p, i);
        saveStorageApi_saveMgrChecksumSlotCall(p, (u32)gabi::at<SaveMgr>(p)->mpSaveData, i);
        saveStorageApi_saveMgrClearPictureStoreCall(p, i);
    }
    saveStorageApi_saveMgrClearMiiverseDataCall(p);
    saveStorageApi_saveMgrClearPicturesCall(p);
}
/* 02722738 */
void SaveMgr::queueCreate() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if ((u32)gabi::at<SaveMgr>(p)->mCommand)
        return;
    u32 thread = (u32)gabi::at<SaveMgr>(p)->mpThread;
    gabi::at<SaveMgr>(p)->mCommand = (u32)(2);
    gabi::call_ptr<void>((u32)gabi::at<WorkerVtable>(gabi::at<Worker>(thread)->mVtable)->postMessage, thread,
                         3u, 1u, cpu->r[6], cpu->r[7], cpu->r[8], cpu->r[9], cpu->r[10], cpu->f[1].ps0,
                         cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0, cpu->f[5].ps0, cpu->f[6].ps0,
                         cpu->f[7].ps0, cpu->f[8].ps0);
}
/* 0272294C */
void SaveMgr::queueDelete() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if ((u32)gabi::at<SaveMgr>(p)->mCommand)
        return;
    u32 thread = (u32)gabi::at<SaveMgr>(p)->mpThread;
    gabi::at<SaveMgr>(p)->mCommand = (u32)(3);
    gabi::call_ptr<void>((u32)gabi::at<WorkerVtable>(gabi::at<Worker>(thread)->mVtable)->postMessage, thread,
                         4u, 1u, cpu->r[6], cpu->r[7], cpu->r[8], cpu->r[9], cpu->r[10], cpu->f[1].ps0,
                         cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0, cpu->f[5].ps0, cpu->f[6].ps0,
                         cpu->f[7].ps0, cpu->f[8].ps0);
}
/* 0272276C */
void SaveMgr::createCard() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    saveStorageApi_saveMgrClearAllCall(p);
    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(0);
    saveStorageApi_saveMgrQueueCreateCall(p);
}
/* 027227A8 */
void SaveMgr::copySlot(u32 to, u32 from) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 src = saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), from);
    u32 dest = saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), to);
    saveStorageApi_copyCardName(dest, src);
    saveStorageApi_saveMgrLoadCardCall(p, from, 0u);
    u32 stage =
        saveStorageApi_getCurrentCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    saveStorageApi_enablePictureSlots(stage);
    src = saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), to);
    dest = saveStorageApi_getCurrentCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    saveStorageApi_copySavePlayLog(dest, src);
    src = saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), from);
    dest = saveStorageApi_getCurrentCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    saveStorageApi_mergeSavePlayLog(dest, src);
    saveStorageApi_saveMgrSelectSlotCall(p, to);
    saveStorageApi_saveMgrSaveCardCall(cpu->r[3], cpu->r[4], 0u);
    saveStorageApi_saveMgrEndCall(p);
}
/* 02722894 */
u32 SaveMgr::openForWrite() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if ((s32)(u32)gabi::at<SaveMgr>(p)->mWriteStatus == -3)
        return 0;
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act");
    name->owner = SafeStringVtableAddress;
    u32 record = saveStorageApi_getSaveFileEntry(readWord(0x101F8B08u), gabi::ea(name.get()));
    u32 file = saveStorageApi_getSaveFileHandle(record);
    if (!file) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-3);
        u32 audio = readWord(0x1018F2BCu);
        if (audio)
            saveStorageApi_playSaveErrorSound(audio, 0x17A714u, 1u);
    }
    return file;
}

/* 02722BD8 */
u32 SaveMgr::eventBit(u32 bit) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (bit >= 0x86)
        return 0;
    u32 index = bit >> 3;
    if (index >= 0x86)
        return 1;
    u32 reg =
        saveStorageApi_getSaveEventRegister(dComIfGs_ea() + 0x644, gabi::load<u16>(0x101408A4u + index * 2));
    return (reg >> (bit & 7)) & 1;
}
/* 02722C6C */
u32 SaveMgr::eventCount() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 n = 0;
    for (u32 i = 0; i < 0x86; i++)
        if (saveStorageApi_saveMgrEventBitCall(p, i))
            n++;
    return n;
}
/* 02722CE4 */
void SaveMgr::describeSlot(u32 playLog) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = dComIfGs_ea(), slot = (u8)gabi::at<SaveGameState>(state)->mSave.mDataNum,
        card = gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo);
    u32 status = saveStorageApi_getCardStatus(card, slot);
    saveStorageApi_setPlayLogPropertyI(playLog, gabi::load<u8>(status) != 2);
    saveStorageApi_setPlayLogPropertyB(playLog, saveStorageApi_saveMgrProgressCall(p, 0u));
    saveStorageApi_setPlayLogPropertyC(playLog, saveStorageApi_saveMgrProgressCall(p, 1u));
    u32 upgraded = saveStorageApi_checkItemFlag(dComIfGs_ea() + 0x71, 1u, 1u) != 0;
    saveStorageApi_setPlayLogPropertyD(playLog, upgraded);
    u32 menu = readWord(0x101F8344u);
    if (menu) {
        u32 screen = readWord(menu, 0x218);
        if (screen)
            saveStorageApi_setPlayLogPropertyE(playLog, saveStorageApi_getMenuPlayLogState(screen));
    }
    u32 player = saveStorageApi_getCardPlayer(card, slot);
    saveStorageApi_setPlayLogPropertyF(playLog, gabi::load<u8>(player));
    player = saveStorageApi_getCardPlayer(card, slot);
    saveStorageApi_setPlayLogPropertyG(playLog, gabi::load<u8>(player + 1));
    player = saveStorageApi_getCardPlayer(card, slot);
    saveStorageApi_setPlayLogPropertyH(playLog, gabi::load<u8>(player + 2));
    player = saveStorageApi_getCardPlayer(card, slot);
    saveStorageApi_setPlayLogPropertyJ(playLog, gabi::load<u8>(player + 3) != 1);
    player = saveStorageApi_getCardPlayer(card, slot);
    u32 a = saveStorageApi_getCardPlayerCompletion(player);
    saveStorageApi_setPlayLogPropertyK(playLog, a ^ 1);
    player = saveStorageApi_getCardPlayer(card, slot);
    a = saveStorageApi_getCardPlayerMode(player);
    saveStorageApi_setPlayLogPropertyL(playLog, a);
    a = saveStorageApi_saveMgrEventCountCall(p);
    saveStorageApi_setPlayLogProgress(playLog, a);
}

/* 02722980 */
u32 SaveMgr::progress(u32 alternate) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = dComIfGs_ea();
    u32 mode = (u8)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mInfo.mClearCount;
    if (alternate) {
        if (!mode)
            return 0;
        if (mode > 1)
            return 15;
    } else if (mode)
        return 15;
    if (saveStorageApi_checkSaveEventBit(state + 0x644, 0x3F10u))
        return 14;
    if (saveStorageApi_getTriforceCount(dComIfGs_ea() + 0xD4) == 8)
        return 13;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x2C01u))
        return 13;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x4004u) &&
        saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x3A02u))
        return 12;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x4004u))
        return 11;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x3A02u))
        return 10;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x2D10u))
        return 9;
    if (saveStorageApi_checkCollectSymbol(dComIfGs_ea() + 0xD4, 0u))
        return 8;
    if (saveStorageApi_checkAcquiredItem(0x31u))
        return 7;
    if (saveStorageApi_checkCollectSymbol(dComIfGs_ea() + 0xD4, 2u))
        return 6;
    if (saveStorageApi_checkCollectSymbol(dComIfGs_ea() + 0xD4, 1u))
        return 5;
    if (saveStorageApi_checkItemFlag(dComIfGs_ea() + 0x71, 1u, 0u))
        return 4;
    if (saveStorageApi_checkSaveEventBit(dComIfGs_ea() + 0x644, 0x2E01u))
        return 3;
    if (saveStorageApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 1u, 0u))
        return 2;
    if (saveStorageApi_checkCollectFlag(dComIfGs_ea() + 0xD4, 0u, 0u))
        return 1;
    return 0;
}
/* 027222F4 */
void SaveMgr::adjustPlayLog(u32 dest, u32 old, u32 fresh) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 a, b, c;
    a = saveStorageApi_getPlayLogCounterA(dest);
    b = saveStorageApi_getPlayLogCounterA(old);
    c = saveStorageApi_getPlayLogCounterA(fresh);
    saveStorageApi_setPlayLogCounterA(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterB(dest);
    b = saveStorageApi_getPlayLogCounterB(old);
    c = saveStorageApi_getPlayLogCounterB(fresh);
    saveStorageApi_setPlayLogCounterB(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterC(dest);
    b = saveStorageApi_getPlayLogCounterC(old);
    c = saveStorageApi_getPlayLogCounterC(fresh);
    saveStorageApi_setPlayLogCounterC(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterD(dest);
    b = saveStorageApi_getPlayLogCounterD(old);
    c = saveStorageApi_getPlayLogCounterD(fresh);
    saveStorageApi_setPlayLogCounterD(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterE(dest);
    b = saveStorageApi_getPlayLogCounterE(old);
    c = saveStorageApi_getPlayLogCounterE(fresh);
    saveStorageApi_setPlayLogCounterE(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterF(dest);
    b = saveStorageApi_getPlayLogCounterF(old);
    c = saveStorageApi_getPlayLogCounterF(fresh);
    saveStorageApi_setPlayLogCounterF(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterG(dest);
    b = saveStorageApi_getPlayLogCounterG(old);
    c = saveStorageApi_getPlayLogCounterG(fresh);
    saveStorageApi_setPlayLogCounterG(dest, a - b + c);
    a = saveStorageApi_getPlayLogCounterH(dest);
    b = saveStorageApi_getPlayLogCounterH(old);
    c = saveStorageApi_getPlayLogCounterH(fresh);
    saveStorageApi_setPlayLogCounterH(dest, a - b + c);
}

/* 0272249C */
void SaveMgr::advanceSlot(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = dComIfGs_ea(),
        mode = ((u8)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mInfo.mClearCount + 1) & 0xFF,
        previous = (u8)gabi::at<SaveGameState>(state)->mSave.mDataNum;
    saveStorageApi_reinitializeSaveInfo(gabi::ea(&gabi::at<SaveGameState>(state)->mSave));
    saveStorageApi_saveMgrSelectSlotCall(p, slot);
    saveStorageApi_initializeCurrentCardData(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    if (previous != slot) {
        u32 from = saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                              previous);
        u32 to =
            saveStorageApi_getCardName(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), slot);
        saveStorageApi_copyCardName(to, from);
        u32 dest = saveStorageApi_getCurrentCardPlayLog(
            gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
        u32 old = saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                                previous);
        u32 fresh =
            saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), slot);
        saveStorageApi_saveMgrAdjustPlayLogCall(p, dest, old, fresh);
    }
    u32 stage =
        saveStorageApi_getCurrentCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
    saveStorageApi_enablePictureSlots(stage);
    gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mInfo.mClearCount = (u8)(mode);
    saveStorageApi_saveMgrSaveCardCall(p, slot, 0u);
    saveStorageApi_saveMgrEndCall(p);
}
/* 02723224 */
u32 SaveMgr::readPictureData(u32 file, u32 count, u32 slot, u32 room) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, PhotoDataSize)) {
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
        return 0;
    }
    u32 stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                               slot & 0xFF);
    u32 dest = saveStorageApi_getPictureData(stage, room);
    saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, PhotoDataSize, 0u);
    return 1;
}
/* 02723F9C */
u32 SaveMgr::writePlayLog(u32 file, u32 count) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 state = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
    u32 playLog = saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), 0u);
    saveStorageApi_copySaveBytes(buffer, playLog, 0x174u, 0u);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 0x174u)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0);
    return 1;
}

static void saveInitFile(u32 file) {
    saveStorageApi_constructFileDisposer(file);
    gabi::at<FileHandle>(file)->mHandle = (u32)(0);
    gabi::at<FileHandle>(file)->mpHeap = (u32)(0);
    gabi::at<FileHandle>(file)->mState = (u32)(0);
    gabi::at<FileHandle>(file)->mVtable = (u32)(FileDisposerVtableAddress);
}
static void saveDestroyFile(u32 file) {
    u32 heap = (u32)gabi::at<FileHandle>(file)->mpHeap;
    gabi::at<FileHandle>(file)->mVtable = (u32)(FileDisposerVtableAddress);
    if (heap)
        saveStorageApi_unlinkFileDisposer(heap, file);
    saveStorageApi_destroySaveDisposer(file, 0u);
}
/* 0272369C */
void SaveMgr::readMiiverseData() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act://cking_miiverse.sav");
    name->owner = SafeStringVtableAddress;
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 0u, 0u);
    Local<be<u32>> count;
    *count.get() = 0;
    bool valid = false;
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle &&
        saveStorageApi_readSaveFile(gabi::ea(file.get()), gabi::ea(count.get()),
                                    (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, MiiverseDataSize) &&
        ((u32)*count.get() == MiiverseDataSize)) {
        saveStorageApi_copySaveBytes(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mMiiverseData),
                                     (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, MiiverseDataSize, 0u);
        valid = true;
    }
    if (!valid)
        saveStorageApi_saveMgrClearMiiverseDataCall(p);
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle)
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    saveDestroyFile(file.a);
}
/* 027237D4 */
void SaveMgr::readPlayLog() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act://cking_playlog.sav");
    name->owner = SafeStringVtableAddress;
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 0u, 0u);
    Local<be<u32>> count;
    *count.get() = 0;
    bool valid = false;
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle &&
        saveStorageApi_readSaveFile(gabi::ea(file.get()), gabi::ea(count.get()),
                                    (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 0x174u) &&
        ((u32)*count.get() == 0x174)) {
        u32 dest =
            saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), 0u);
        saveStorageApi_copySaveBytes(dest, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 0x174u, 0u);
        saveStorageApi_getCurrentCardPlayLogDiscard(
            gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo));
        valid = true;
    }
    if (!valid)
        saveStorageApi_saveMgrClearPicturesCall(p);
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle)
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    saveDestroyFile(file.a);
}
/* 027232D8 */
u32 SaveMgr::readPictureHeaders(u32 file, u32 count, u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 12; i++) {
        if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u)) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
        u32 stage = saveStorageApi_getCardPictures(
            gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), slot & 0xFF);
        u32 part = saveStorageApi_getPictureRecord(stage, i);
        saveStorageApi_copySaveBytes(part, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u, 0u);
        stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                               slot & 0xFF);
        part = saveStorageApi_getPictureRecord(stage, i);
        u32 expected = saveStorageApi_getPictureHeaderChecksum(part);
        stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                               slot & 0xFF);
        part = saveStorageApi_getPictureRecord(stage, i);
        u32 actual = saveStorageApi_calculatePictureHeaderChecksum(part);
        if (expected != actual) {
            gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
            return 0;
        }
    }
    if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 12u)) {
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
        return 0;
    }
    u32 stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                               slot & 0xFF);
    saveStorageApi_copySaveBytes(gabi::ea(gabi::at<dPictureStoreHD_c>(stage)->mPictureSlotSortMap),
                                 (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 12u, 0u);
    if (!saveStorageApi_readSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 1u)) {
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
        return 0;
    }
    Local<be<u8>> flag;
    *flag.get() = 0;
    saveStorageApi_copySaveBytes(gabi::ea(flag.get()), (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 1u, 0u);
    stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                           slot & 0xFF);
    saveStorageApi_setPictureSelection(stage, (u32)*flag.get());
    return 1;
}
/* 02723B54 */
u32 SaveMgr::writePictureData(u32 file, u32 count, u32 slot, u32 room) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 card = gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo);
    u32 stage = saveStorageApi_getCardPictures(card, slot & 0xFF);
    u32 part = saveStorageApi_getPictureRecord(stage, room);
    saveStorageApi_updatePictureHeaderChecksum(part);
    stage = saveStorageApi_getCardPictures(card, slot & 0xFF);
    saveStorageApi_disablePictureSlot(stage, room);
    u32 buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
    stage = saveStorageApi_getCardPictures(card, slot & 0xFF);
    u32 src = saveStorageApi_getPictureData(stage, room);
    saveStorageApi_copySaveBytes(buffer, src, PhotoDataSize, 0u);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, PhotoDataSize)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0);
    return 1;
}
/* 02723C38 */
u32 SaveMgr::writePictureHeaders(u32 file, u32 count, u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    for (u32 i = 0; i < 12; i++) {
        u32 state = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer;
        u32 stage =
            saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), slot & 0xFF);
        u32 part = saveStorageApi_getPictureRecord(stage, i);
        saveStorageApi_copySaveBytes(buffer, part, 16u, 0u);
        if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 16u))
            return 0;
    }
    u32 state = dComIfGs_ea(), buffer = (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer,
        stage =
            saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), slot & 0xFF);
    saveStorageApi_copySaveBytes(buffer, gabi::ea(gabi::at<dPictureStoreHD_c>(stage)->mPictureSlotSortMap),
                                 12u, 0u);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 12u)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                           slot & 0xFF);
    gabi::store<u8>((u32)gabi::at<SaveMgr>(p)->mpWorkBuffer,
                    (u8)gabi::at<dPictureStoreHD_c>(stage)->mSelectedPhotoSlot);
    if (!saveStorageApi_writeSaveFile(file, count, (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, 1u)) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        return 0;
    }
    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0);
    return 1;
}

/* 0272391C */
void SaveMgr::workerLoad() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act://cking.sav");
    name->owner = SafeStringVtableAddress;
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 0u, 0u);
    Local<be<u32>> count;
    *count.get() = 0;
    gabi::at<SaveMgr>(p)->mReadStatus = (u32)(0xFF);
    bool failed = false;
    u32 status = 0xFF;
    for (u32 i = 0; i < 8; i++) {
        u32 descriptor = 0x1014094Cu + i * 8;
        s32 index = gabi::load<s16>(descriptor + 2), adjust = gabi::load<s16>(descriptor);
        u32 self = p + (u32)adjust, target, r9 = cpu->r[9], r10 = descriptor;
        if (index < 0)
            target = readWord(descriptor, 4);
        else {
            r9 = (u32)(s32)gabi::load<s16>(descriptor + 6);
            u32 table = readWord(self + r9);
            r10 = (u32)index * 8;
            target = readWord(table + r10 + 4);
        }
        u32 result =
            gabi::call_ptr<u32>(target, self, file.get(), count.get(), cpu->r[6], cpu->r[7], (u32)adjust, r9,
                                r10, cpu->f[1].ps0, cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0,
                                cpu->f[5].ps0, cpu->f[6].ps0, cpu->f[7].ps0, cpu->f[8].ps0);
        if (!result) {
            failed = true;
            status = (u32)gabi::at<SaveMgr>(p)->mReadStatus;
            break;
        }
        status = (u32)gabi::at<SaveMgr>(p)->mReadStatus;
        if (status != 0xFF)
            break;
    }
    if (!failed && status == 0xFF) {
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(0);
        status = 0;
    } else if (status != (u32)-2) {
        gabi::at<SaveMgr>(p)->mReadStatus = (u32)(-3);
        status = -3;
    }
    if (status == (u32)-2)
        saveStorageApi_saveMgrClearAllCall(p);
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle)
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    if (!(u32)gabi::at<SaveMgr>(p)->mReadStatus) {
        for (u32 i = 0; i < 3; i++)
            saveStorageApi_saveMgrReadPictureFilesCall(p, i);
        if (!(u32)gabi::at<SaveMgr>(p)->mReadStatus) {
            saveStorageApi_saveMgrReadMiiverseDataCall(p);
            if (!(u32)gabi::at<SaveMgr>(p)->mReadStatus)
                saveStorageApi_saveMgrReadPlayLogCall(p);
        }
    }
    saveStorageApi_saveMgrLoadCardCall(p, (u8)gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum, 1u);
    gabi::at<SaveMgr>(p)->mCommand = (u32)(0);
    saveDestroyFile(file.a);
}
/* 027243E0 */
void SaveMgr::workerMiiverseData() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act://cking_miiverse.sav");
    name->owner = SafeStringVtableAddress;
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 1u, 0u);
    Local<be<u32>> count;
    *count.get() = 0;
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0xFF);
        saveStorageApi_copySaveBytes((u32)gabi::at<SaveMgr>(p)->mpWorkBuffer,
                                     gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mMiiverseData),
                                     MiiverseDataSize, 0u);
        if (!saveStorageApi_writeSaveFile(gabi::ea(file.get()), gabi::ea(count.get()),
                                          (u32)gabi::at<SaveMgr>(p)->mpWorkBuffer, MiiverseDataSize))
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        else if ((u32)gabi::at<SaveMgr>(p)->mWriteStatus == 0xFF)
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)((u32)*count.get() == MiiverseDataSize ? 0 : -2);
    } else
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-1);
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle)
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    if (!(u32)gabi::at<SaveMgr>(p)->mWriteStatus) {
        Local<SaveName> cleanup;
        cleanup->name = gabi::guest_addr("save_act");
        cleanup->owner = SafeStringVtableAddress;
        u32 record = saveStorageApi_getSaveFileEntry(readWord(0x101F8B08u), gabi::ea(cleanup.get()));
        if (!saveStorageApi_removeSaveFileEntry(record))
            gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
    }
    gabi::at<SaveMgr>(p)->mCommand = (u32)(0);
    saveDestroyFile(file.a);
}

static void saveInitFormat(u32 format) {
    gabi::store<u32>(format, format + 12);
    gabi::store<u32>(format + 4, FormatStringVtableAddress);
    gabi::store<u32>(format + 8, 64);
    gabi::store<u8>(format + 12, 0);
    gabi::store<u8>(format + 75, 0);
}
/* 02723460 */
void SaveMgr::readPictureFiles(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    Local<FormatString> name;
    Local<be<u32>> count;
    *count.get() = 0;
    u32 result = 0;
    for (u32 room = 0; room < 12; room++) {
        saveInitFile(file.a);
        saveInitFormat(name.a);
        saveStorageApi_formatSaveFileName(gabi::ea(name.get()),
                                          gabi::guest_addr("save_act://cking_pic%d_%d.sav"), slot, room);
        saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 0u,
                                    0u);
        if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
            result = saveStorageApi_saveMgrReadPictureDataCall(p, gabi::ea(file.get()), gabi::ea(count.get()),
                                                               slot, room);
            if (!result) {
                saveStorageApi_saveMgrClearPictureStoreCall(p, slot & 0xFF);
                saveDestroyFile(file.a);
                return;
            }
            saveStorageApi_closeSaveFile(gabi::ea(file.get()));
        } else
            saveStorageApi_saveMgrClearPictureStoreCall(p, slot & 0xFF);
        saveDestroyFile(file.a);
    }
    if (result) {
        saveInitFile(file.a);
        saveInitFormat(name.a);
        saveStorageApi_formatSaveFileName(gabi::ea(name.get()),
                                          gabi::guest_addr("save_act://cking_pic_order%d.sav"), slot);
        saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 0u,
                                    0u);
        if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
            saveStorageApi_saveMgrReadPictureHeadersCall(p, gabi::ea(file.get()), gabi::ea(count.get()),
                                                         slot);
            saveStorageApi_closeSaveFile(gabi::ea(file.get()));
        }
        saveDestroyFile(file.a);
    }
}
/* 02723D98 */
void SaveMgr::writePictureFiles(u32 slot) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<be<u32>> count;
    *count.get() = 0;
    u32 stage = saveStorageApi_getCardPictures(gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo),
                                               slot & 0xFF),
        result = 0;
    Local<FileHandle> file;
    Local<FormatString> name;
    for (u32 room = 0; room < 12; room++) {
        if (!saveStorageApi_isPictureSlotValid(stage, room))
            continue;
        saveInitFile(file.a);
        saveInitFormat(name.a);
        saveStorageApi_formatSaveFileName(gabi::ea(name.get()),
                                          gabi::guest_addr("save_act://cking_pic%d_%d.sav"), slot, room);
        saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 1u,
                                    0u);
        if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
            result = saveStorageApi_saveMgrWritePictureDataCall(p, gabi::ea(file.get()),
                                                                gabi::ea(count.get()), slot, room);
            saveStorageApi_closeSaveFile(gabi::ea(file.get()));
        }
        saveDestroyFile(file.a);
    }
    if (result) {
        saveInitFile(file.a);
        saveInitFormat(name.a);
        saveStorageApi_formatSaveFileName(gabi::ea(name.get()),
                                          gabi::guest_addr("save_act://cking_pic_order%d.sav"), slot);
        saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 1u,
                                    0u);
        if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
            saveStorageApi_saveMgrWritePictureHeadersCall(p, gabi::ea(file.get()), gabi::ea(count.get()),
                                                          slot);
            saveStorageApi_closeSaveFile(gabi::ea(file.get()));
        }
        saveDestroyFile(file.a);
    }
}
/* 02724064 */
void SaveMgr::writePlayLogFile() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<be<u32>> count;
    *count.get() = 0;
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<FormatString> name;
    saveInitFormat(name.a);
    saveStorageApi_formatSaveFileName(gabi::ea(name.get()), gabi::guest_addr("save_act://cking_playlog.sav"));
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 1u, 0u);
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle) {
        u32 state = dComIfGs_ea(), slot = (u8)gabi::at<SaveGameState>(state)->mSave.mDataNum;
        u32 playLog =
            saveStorageApi_getCardPlayLog(gabi::ea(&gabi::at<SaveGameState>(state)->mCardInfo), slot);
        saveStorageApi_saveMgrDescribeSlotCall(p, playLog);
        saveStorageApi_saveMgrWritePlayLogCall(p, gabi::ea(file.get()), gabi::ea(count.get()));
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    }
    saveDestroyFile(file.a);
}
/* 02722E88 */
void SaveMgr::mergePlayLogs(u32 dest) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    u32 card = gabi::ea(&gabi::at<SaveGameState>(dComIfGs_ea())->mCardInfo), part, value, current;
    saveStorageApi_clearSavePlayLog(dest);
    for (u32 i = 0; i < 3; i++) {
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterA(part);
        saveStorageApi_addPlayLogCounterA(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterB(part);
        saveStorageApi_addPlayLogCounterB(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterC(part);
        saveStorageApi_addPlayLogCounterC(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterD(part);
        saveStorageApi_addPlayLogCounterD(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterE(part);
        saveStorageApi_addPlayLogCounterE(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterF(part);
        saveStorageApi_addPlayLogCounterF(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterG(part);
        saveStorageApi_addPlayLogCounterG(dest, value);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogCounterH(part);
        saveStorageApi_addPlayLogCounterH(dest, value);
    }
    for (u32 i = 0; i < 3; i++) {
        current = saveStorageApi_getPlayLogPropertyA(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogPropertyA(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogPropertyA(part);
            saveStorageApi_setPlayLogPropertyA(dest, value);
        }
        current = saveStorageApi_getPlayLogPropertyB(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogPropertyB(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogPropertyB(part);
            saveStorageApi_setPlayLogPropertyB(dest, value);
        }
        current = saveStorageApi_getPlayLogPropertyC(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogPropertyC(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogPropertyC(part);
            saveStorageApi_setPlayLogPropertyC(dest, value);
        }
        current = saveStorageApi_getPlayLogPropertyD(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogPropertyD(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogPropertyD(part);
            saveStorageApi_setPlayLogPropertyD(dest, value);
        }
        current = saveStorageApi_getPlayLogPropertyE(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogPropertyE(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogPropertyE(part);
            saveStorageApi_setPlayLogPropertyE(dest, value);
        }
        current = saveStorageApi_getPlayLogProgress(dest);
        part = saveStorageApi_getCardPlayLogSummary(card, i);
        value = saveStorageApi_getPlayLogProgress(part);
        if (current < value) {
            part = saveStorageApi_getCardPlayLogSummary(card, i);
            value = saveStorageApi_getPlayLogProgress(part);
            saveStorageApi_setPlayLogProgress(dest, value);
        }
    }
    part = saveStorageApi_getCardPlayerSummary(card, 0u);
    u32 slot = saveStorageApi_getCardPlayerSlot(part);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyI(part);
    saveStorageApi_setPlayLogPropertyI(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyF(part);
    saveStorageApi_setPlayLogPropertyF(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyG(part);
    saveStorageApi_setPlayLogPropertyG(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyH(part);
    saveStorageApi_setPlayLogPropertyH(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyJ(part);
    saveStorageApi_setPlayLogPropertyJ(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyK(part);
    saveStorageApi_setPlayLogPropertyK(dest, value);
    part = saveStorageApi_getCardPlayLogSummary(card, slot & 0xFF);
    value = saveStorageApi_getPlayLogPropertyL(part);
    saveStorageApi_setPlayLogPropertyL(dest, value);
}

/* 02724180 */
void SaveMgr::workerWrite(u32 allSlots) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<FileHandle> file;
    saveInitFile(file.a);
    Local<SaveName> name;
    name->name = gabi::guest_addr("save_act://cking.sav");
    name->owner = SafeStringVtableAddress;
    saveStorageApi_openSaveFile(readWord(0x101F8B08u), gabi::ea(file.get()), gabi::ea(name.get()), 1u, 0u);
    Local<be<u32>> count;
    *count.get() = 0;
    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0xFF);
    bool failed = false;
    u32 status = 0xFF;
    for (u32 i = 0; i < 8; i++) {
        u32 descriptor = 0x10140A08u + i * 8;
        s32 index = gabi::load<s16>(descriptor + 2), adjust = gabi::load<s16>(descriptor);
        u32 self = p + (u32)adjust, target, r7 = (u32)adjust, r8 = cpu->r[8], r10 = cpu->r[10];
        if (index < 0)
            target = readWord(descriptor, 4);
        else {
            r8 = (u32)(s32)gabi::load<s16>(descriptor + 6);
            r10 = readWord(self + r8);
            r7 = r10 + (u32)index * 8;
            target = readWord(r7, 4);
        }
        u32 result = gabi::call_ptr<u32>(target, self, file.get(), count.get(), (u32)index, r7, r8, cpu->r[9],
                                         r10, cpu->f[1].ps0, cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0,
                                         cpu->f[5].ps0, cpu->f[6].ps0, cpu->f[7].ps0, cpu->f[8].ps0);
        if (!result) {
            failed = true;
            status = (u32)gabi::at<SaveMgr>(p)->mWriteStatus;
            break;
        }
        status = (u32)gabi::at<SaveMgr>(p)->mWriteStatus;
        if (status != 0xFF)
            break;
    }
    if (!failed && status == 0xFF) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(0);
        status = 0;
    } else if (status != (u32)-1) {
        gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
        status = -2;
    }
    if ((u32)gabi::at<FileHandle>(file.a)->mHandle)
        saveStorageApi_closeSaveFile(gabi::ea(file.get()));
    if (!(u32)gabi::at<SaveMgr>(p)->mWriteStatus) {
        if (allSlots) {
            for (u32 i = 0; i < 3; i++)
                saveStorageApi_saveMgrWritePictureFilesCall(p, i);
        } else
            saveStorageApi_saveMgrWritePictureFilesCall(
                p, (u8)gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum);
        if (!(u32)gabi::at<SaveMgr>(p)->mWriteStatus) {
            saveStorageApi_saveMgrWritePlayLogFileCall(p);
            if (!(u32)gabi::at<SaveMgr>(p)->mWriteStatus) {
                Local<SaveName> cleanup;
                cleanup->name = gabi::guest_addr("save_act");
                cleanup->owner = SafeStringVtableAddress;
                u32 record = saveStorageApi_getSaveFileEntry(readWord(0x101F8B08u), gabi::ea(cleanup.get()));
                if (!saveStorageApi_removeSaveFileEntry(record))
                    gabi::at<SaveMgr>(p)->mWriteStatus = (u32)(-2);
            }
        }
    }
    gabi::at<SaveMgr>(p)->mCommand = (u32)(0);
    saveDestroyFile(file.a);
}

static void saveNameCheckedCall(u32 object) {
    // SafeString base vtable101407B0+14 is empty027253B4; it reads no argument registers.
    gabi::call_ptr<void>(
        gabi::at<SafeStringVtable>(gabi::at<SaveName>(object)->owner)->assureTermination.get(), object);
}
static bool saveNamesEqual(u32 left, u32 right) {
    saveNameCheckedCall(left);
    saveNameCheckedCall(left);
    u32 saved = readWord(left);
    saveNameCheckedCall(right);
    if (saved == readWord(right))
        return true;
    u32 a = readWord(left), b = readWord(right);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = gabi::load<u8>(a + i), y = gabi::load<u8>(b + i);
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
/* 02721D48 */
void SaveMgr::recollectionSave() {
    [[maybe_unused]] u32 p = gabi::ea(this);
    Local<StageComparisons> comparisons;
    u32 base = comparisons.a;
    u32 old[3] = {0xFF, 0xFF, 0xFF};
    bool first = false;
    const u32 strings[4] = {gabi::guest_addr("Xboss0"), gabi::guest_addr("Xboss1"),
                            gabi::guest_addr("Xboss2"), gabi::guest_addr("Xboss3")};
    for (u32 i = 0; i < 4; i++) {
        u32 left = base + i * 8, right = base + 32 + i * 8;
        gabi::store<u32>(left, strings[i]);
        gabi::store<u32>(left + 4, SafeStringVtableAddress);
        u32 game = saveStorageApi_getGameInfoAddress();
        gabi::store<u32>(right, gabi::ea(gabi::at<dComIfG_inf_c>(game)->play.mCurStage.mName));
        gabi::store<u32>(right + 4, SafeStringVtableAddress);
        if (saveNamesEqual(left, right)) {
            first = true;
            break;
        }
    }
    if (first) {
        u32 state = dComIfGs_ea();
        old[0] = (u8)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[0];
        old[1] = (u8)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[1];
        old[2] = (u8)gabi::at<SaveGameState>(state)->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[2];
        saveStorageApi_prepareRecollectionItems();
    }
    saveStorageApi_saveMgrSaveCardCall(p, (u8)gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mDataNum, 1u);
    bool second = false;
    for (u32 i = 0; i < 4; i++) {
        u32 left = base + (3 - i) * 8, right = base + 64 + i * 8;
        gabi::store<u32>(left, strings[i]);
        gabi::store<u32>(left + 4, SafeStringVtableAddress);
        u32 game = saveStorageApi_getGameInfoAddress();
        gabi::store<u32>(right, gabi::ea(gabi::at<dComIfG_inf_c>(game)->play.mCurStage.mName));
        gabi::store<u32>(right + 4, SafeStringVtableAddress);
        if (saveNamesEqual(left, right)) {
            second = true;
            break;
        }
    }
    if (second) {
        saveStorageApi_restoreRecollectionItems();
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[0] =
            (u8)(old[0]);
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[1] =
            (u8)(old[1]);
        gabi::at<SaveGameState>(dComIfGs_ea())->mSave.mSavedata.mPlayer.mPlayerStatusA.mSelectItem[2] =
            (u8)(old[2]);
    }
    saveStorageApi_saveMgrEndCall(p);
}
/* WWHD glue
str 10140878 "Xboss0" in 02721D48
str 10140880 "Xboss1" in 02721D48
str 10140888 "Xboss2" in 02721D48
str 10140890 "Xboss3" in 02721D48
str 10140898 "save_act" in 02722894
str 1014090C "save_act://cking_miiverse.sav" in 0272369C
str 1014092C "save_act://cking_playlog.sav" in 027237D4
str 101408C8 "save_act://cking_pic%d_%d.sav" in 02723460
str 101408E8 "save_act://cking_pic_order%d.sav" in 02723460
str 1014098C "save_act://cking.sav" in 0272391C
str 10140A6C "save_act://cking_miiverse.sav" in 027243E0
str 10140A8C "save_act" in 027243E0
str 101409A4 "save_act://cking_pic%d_%d.sav" in 02723D98
str 101409C4 "save_act://cking_pic_order%d.sav" in 02723D98
str 101409E8 "save_act://cking_playlog.sav" in 02724064
str 10140A48 "save_act://cking.sav" in 02724180
str 10140A60 "save_act" in 02724180
ext 0203300C void saveStorageApi_playSaveErrorSound(u32, u32, u32)
ext 025200D4 u32 saveStorageApi_getGameInfoAddress()
ext 02520C0C u32 saveStorageApi_checkAcquiredItem(u32)
ext 025224A4 void saveStorageApi_restoreRecollectionItems()
ext 02522C60 void saveStorageApi_prepareRecollectionItems()
ext 025B5D40 u32 saveStorageApi_checkItemFlag(u32, u32, u32)
ext 025B7A2C u32 saveStorageApi_checkCollectFlag(u32, u32, u32)
ext 025B7D90 u32 saveStorageApi_checkCollectSymbol(u32, u32)
ext 025B7E00 u32 saveStorageApi_getTriforceCount(u32)
ext 025B8B94 u32 saveStorageApi_checkSaveEventBit(u32, u32)
ext 025B8BB0 u32 saveStorageApi_getSaveEventRegister(u32, u32)
ext 025B9A90 void saveStorageApi_reinitializeSaveInfo(u32)
ext 0268A4E8 u32 saveStorageApi_getMenuPlayLogState(u32)
ext 0271FC30 u32 saveStorageApi_getCardPlayerCompletion(u32)
ext 0271FC78 u32 saveStorageApi_getCardPlayerMode(u32)
ext 0271FCA4 u32 saveStorageApi_getCardPlayerSlot(u32)
ext 0271FCAC void saveStorageApi_setCardPlayerSlot(u32, u32)
ext 0272005C void saveStorageApi_initializeCurrentCardData(u32)
ext 027200A0 u32 saveStorageApi_getCardPlayer(u32, u32)
ext 027200B8 u32 saveStorageApi_getCardPlayerSummary(u32, u32)
ext 027200D8 u32 saveStorageApi_getCardStatus(u32, u32)
ext 027200FC u32 saveStorageApi_getCardEvents(u32, u32)
ext 02720120 u32 saveStorageApi_getCardPictures(u32, u32)
ext 02720144 u32 saveStorageApi_getCurrentCardPictures(u32)
ext 02720154 u32 saveStorageApi_getCardName(u32, u32)
ext 02720180 u32 saveStorageApi_getCardMap(u32, u32)
ext 027201A4 u32 saveStorageApi_getCardPlayLog(u32, u32)
ext 027201C0 u32 saveStorageApi_getCardPlayLogSummary(u32, u32)
ext 027201DC u32 saveStorageApi_getCurrentCardPlayLog(u32)
ext 027201DC void saveStorageApi_getCurrentCardPlayLogDiscard(u32)
ext 027201E4 u32 saveStorageApi_getCardDataVersion(u32)
ext 0272033C void saveStorageApi_updateCardDataVersion(u32)
ext 02721908 void saveStorageApi_saveMgrLoadCardCall(u32, u32, u32)
ext 027219AC void saveStorageApi_saveMgrInitialCardCall(u32, u32)
ext 02721AC4 u32 saveStorageApi_saveMgrChecksumCall(u32, u32, u32)
ext 02721AFC void saveStorageApi_saveMgrChecksumSlotCall(u32, u32, u32)
ext 02721C44 void saveStorageApi_saveMgrSaveCardCall(u32, u32, u32)
ext 02721D18 void saveStorageApi_saveMgrEndCall(u32)
ext 027222E4 void saveStorageApi_saveMgrSelectSlotCall(u32, u32)
ext 027222F4 void saveStorageApi_saveMgrAdjustPlayLogCall(u32, u32, u32, u32)
ext 02722610 void saveStorageApi_saveMgrClearPictureStoreCall(u32, u32)
ext 02722640 void saveStorageApi_saveMgrClearMiiverseDataCall(u32)
ext 02722654 void saveStorageApi_saveMgrClearPicturesCall(u32)
ext 027226B4 void saveStorageApi_saveMgrClearAllCall(u32)
ext 02722738 void saveStorageApi_saveMgrQueueCreateCall(u32)
ext 02722980 u32 saveStorageApi_saveMgrProgressCall(u32, u32)
ext 02722BD8 u32 saveStorageApi_saveMgrEventBitCall(u32, u32)
ext 02722C6C u32 saveStorageApi_saveMgrEventCountCall(u32)
ext 02722CE4 void saveStorageApi_saveMgrDescribeSlotCall(u32, u32)
ext 02723224 u32 saveStorageApi_saveMgrReadPictureDataCall(u32, u32, u32, u32, u32)
ext 027232D8 void saveStorageApi_saveMgrReadPictureHeadersCall(u32, u32, u32, u32)
ext 02723460 void saveStorageApi_saveMgrReadPictureFilesCall(u32, u32)
ext 0272369C void saveStorageApi_saveMgrReadMiiverseDataCall(u32)
ext 027237D4 void saveStorageApi_saveMgrReadPlayLogCall(u32)
ext 0272391C void saveStorageApi_saveMgrWorkerLoadCall(u32, u32, u32)
ext 02723B54 u32 saveStorageApi_saveMgrWritePictureDataCall(u32, u32, u32, u32, u32)
ext 02723C38 void saveStorageApi_saveMgrWritePictureHeadersCall(u32, u32, u32, u32)
ext 02723D98 void saveStorageApi_saveMgrWritePictureFilesCall(u32, u32)
ext 02723F9C void saveStorageApi_saveMgrWritePlayLogCall(u32, u32, u32)
ext 02724064 void saveStorageApi_saveMgrWritePlayLogFileCall(u32)
ext 02724180 void saveStorageApi_saveMgrWorkerWriteCall(u32, u32, u32)
ext 027243E0 void saveStorageApi_saveMgrWorkerMiiverseDataCall(u32, u32, u32)
ext 027245BC u32 saveStorageApi_saveMgrCheckSlotCall(u32, u32, u32)
ext 02724614 void saveStorageApi_saveMgrTerminateSlotNameCall(u32, u32, u32)
ext 02725574 void saveStorageApi_clearMiiverseDataSaveData(u32)
ext 0272581C u32 saveStorageApi_getPictureHeaderChecksum(u32)
ext 027258C4 void saveStorageApi_updatePictureHeaderChecksum(u32)
ext 02725B44 u32 saveStorageApi_calculatePictureHeaderChecksum(u32)
ext 02725DA8 void saveStorageApi_clearPictureStore(u32)
ext 02725FAC u32 saveStorageApi_isPictureSlotValid(u32, u32)
ext 02725FD0 void saveStorageApi_enablePictureSlots(u32)
ext 02726028 void saveStorageApi_disablePictureSlot(u32, u32)
ext 0272609C u32 saveStorageApi_getPictureRecord(u32, u32)
ext 02726164 u32 saveStorageApi_getPictureData(u32, u32)
ext 027262B0 void saveStorageApi_setPictureSelection(u32, u32)
ext 02726668 void saveStorageApi_setCardName(u32, u32)
ext 027267FC void saveStorageApi_copyCardName(u32, u32)
ext 02726B30 void saveStorageApi_clearSavePlayLog(u32)
ext 02726BC8 void saveStorageApi_copySavePlayLog(u32, u32)
ext 02726C74 void saveStorageApi_mergeSavePlayLog(u32, u32)
ext 02726D2C u32 saveStorageApi_getPlayLogCounterA(u32)
ext 02726D34 void saveStorageApi_setPlayLogCounterA(u32, u32)
ext 02726D3C void saveStorageApi_addPlayLogCounterA(u32, u32)
ext 02726D4C u32 saveStorageApi_getPlayLogCounterB(u32)
ext 02726D54 void saveStorageApi_setPlayLogCounterB(u32, u32)
ext 02726D5C void saveStorageApi_addPlayLogCounterB(u32, u32)
ext 02726D6C u32 saveStorageApi_getPlayLogCounterC(u32)
ext 02726D74 void saveStorageApi_setPlayLogCounterC(u32, u32)
ext 02726D7C void saveStorageApi_addPlayLogCounterC(u32, u32)
ext 02726D8C u32 saveStorageApi_getPlayLogPropertyI(u32)
ext 02726D94 void saveStorageApi_setPlayLogPropertyI(u32, u32)
ext 02726D9C u32 saveStorageApi_getPlayLogCounterD(u32)
ext 02726DA4 void saveStorageApi_setPlayLogCounterD(u32, u32)
ext 02726DAC void saveStorageApi_addPlayLogCounterD(u32, u32)
ext 02726DBC u32 saveStorageApi_getPlayLogCounterE(u32)
ext 02726DC4 void saveStorageApi_setPlayLogCounterE(u32, u32)
ext 02726DCC void saveStorageApi_addPlayLogCounterE(u32, u32)
ext 02726DDC u32 saveStorageApi_getPlayLogCounterF(u32)
ext 02726DE4 void saveStorageApi_setPlayLogCounterF(u32, u32)
ext 02726DEC void saveStorageApi_addPlayLogCounterF(u32, u32)
ext 02726DFC u32 saveStorageApi_getPlayLogPropertyA(u32)
ext 02726E04 void saveStorageApi_setPlayLogPropertyA(u32, u32)
ext 02726E1C u32 saveStorageApi_getPlayLogPropertyB(u32)
ext 02726E24 void saveStorageApi_setPlayLogPropertyB(u32, u32)
ext 02726E2C u32 saveStorageApi_getPlayLogPropertyC(u32)
ext 02726E34 void saveStorageApi_setPlayLogPropertyC(u32, u32)
ext 02726E3C u32 saveStorageApi_getPlayLogPropertyD(u32)
ext 02726E44 void saveStorageApi_setPlayLogPropertyD(u32, u32)
ext 02726E4C u32 saveStorageApi_getPlayLogPropertyE(u32)
ext 02726E54 void saveStorageApi_setPlayLogPropertyE(u32, u32)
ext 02726E5C u32 saveStorageApi_getPlayLogPropertyF(u32)
ext 02726E64 void saveStorageApi_setPlayLogPropertyF(u32, u32)
ext 02726E6C u32 saveStorageApi_getPlayLogPropertyG(u32)
ext 02726E74 void saveStorageApi_setPlayLogPropertyG(u32, u32)
ext 02726E7C u32 saveStorageApi_getPlayLogPropertyH(u32)
ext 02726E84 void saveStorageApi_setPlayLogPropertyH(u32, u32)
ext 02726E8C u32 saveStorageApi_getPlayLogPropertyJ(u32)
ext 02726E94 void saveStorageApi_setPlayLogPropertyJ(u32, u32)
ext 02726E9C u32 saveStorageApi_getPlayLogPropertyK(u32)
ext 02726EA4 void saveStorageApi_setPlayLogPropertyK(u32, u32)
ext 02726EAC u32 saveStorageApi_getPlayLogPropertyL(u32)
ext 02726EB4 void saveStorageApi_setPlayLogPropertyL(u32, u32)
ext 02726EBC u32 saveStorageApi_getPlayLogCounterG(u32)
ext 02726EC4 void saveStorageApi_setPlayLogCounterG(u32, u32)
ext 02726ECC void saveStorageApi_addPlayLogCounterG(u32, u32)
ext 02726EDC u32 saveStorageApi_getPlayLogCounterH(u32)
ext 02726EE4 void saveStorageApi_setPlayLogCounterH(u32, u32)
ext 02726EEC void saveStorageApi_addPlayLogCounterH(u32, u32)
ext 02726EFC u32 saveStorageApi_getPlayLogProgress(u32)
ext 02726F04 void saveStorageApi_setPlayLogProgress(u32, u32)
ext 0273AF40 void saveStorageApi_deleteSaveObject(u32)
ext 0273AFC8 void saveStorageApi_freeSaveBuffer(u32)
ext 0273B0D4 u32 saveStorageApi_allocateSaveAligned(u32, u32, u32)
ext 02740BE0 void saveStorageApi_unlinkFileDisposer(u32, u32)
ext 02741430 void saveStorageApi_closeSaveFile(u32)
ext 027414CC u32 saveStorageApi_readSaveFile(u32, u32, u32, u32)
ext 02741558 u32 saveStorageApi_writeSaveFile(u32, u32, u32, u32)
ext 02741B38 u32 saveStorageApi_getSaveFileEntry(u32, u32)
ext 02741CFC void saveStorageApi_openSaveFile(u32, u32, u32, u32, u32)
ext 0274483C u32 saveStorageApi_getSaveFileHandle(u32)
ext 02744894 u32 saveStorageApi_removeSaveFileEntry(u32)
ext 02752A84 void saveStorageApi_constructFileDisposer(u32)
ext 02752BEC void saveStorageApi_destroySaveDisposer(u32, u32)
ext 02759C28 void saveStorageApi_formatSaveFileName(u32, u32)
ext 02759C28 void saveStorageApi_formatSaveFileName(u32, u32, u32)
ext 02759C28 void saveStorageApi_formatSaveFileName(u32, u32, u32, u32)
ext 02760168 void saveStorageApi_destroySaveCriticalSection(u32, u32)
ext 028EAB2C void saveStorageApi_initializeSaveHeaderFlag(u32)
ext 028ED6F8 void saveStorageApi_initializeSaveHeaderByte(u32)
ext 028F026C void saveStorageApi_registerSaveHeaderObject(u32)
ext C0009988 void saveStorageApi_copySaveBytes(u32, u32, u32, u32)
*/
