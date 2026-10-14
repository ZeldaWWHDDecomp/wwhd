/** HD save-data holder; local decomp */
#include "d/hd_save_holders.h"
#include "d/d_save.h"
static u32 sSaveHoldersHeaderStatics;
static u32 sSaveHoldersHeaderDescriptors;
static u32 sSaveHoldersHeaderZero;
static u32 sSaveHoldersPiConstants;

struct hdSaveSelection_l { u8 mBase[0x20]; dSv_info_c mSaveInfo; };
WWHD_OFFSET(hdSaveSelection_l,mSaveInfo,0x20);
static hdSaveSelection_l* sSaveSelection_hd_save_holders;
static void initSaveName(hdSaveName_l*);
static void clearSaveName(hdSaveName_l*);
static void* createSaveName(void*);
static void initSavePictureInfo(hdSavePictureInfo_l*);
static void clearSavePictureInfo(hdSavePictureInfo_l*);
static void* createSavePictureInfo(void*);
static void initSavePicture(hdSavePicture_l*);
static void clearSavePicture(hdSavePicture_l*);
static void* createSavePicture(void*);
static void constructSaveArray(void*,u32,u32,u32);
static void* getSaveHeap();
static void* allocateSaveMemory(u32,void*,s32);
static void setSaveMemory(void*,u32,u32);
static void copySaveMemory(void*,void*,u32,u32);
static u32 calculateSaveCrc(void*,u32);
static void freeSaveMemory(void*);


/* 0271FD94 */
void hdSaveHolders_c::init() {
    for (u32 slot = 0; slot < SLOT_COUNT; ++slot) {
        mSlotFlags[slot].init(); mPairs[slot].init(); mInventories[slot].init();
        initSaveName(&mNames[slot]); mPlayLogs[slot].init();
        initSavePictureInfo(&mPictureInfo[slot]); initSavePicture(&mPictures[slot]);
    }
    mChecksum = 0;
}

/* 0271FE48 */
hdSaveHolders_c* createSaveHolders(hdSaveHolders_c* holder) {
    if (holder == NULL) { holder = (hdSaveHolders_c*)operator_new(sizeof(hdSaveHolders_c)); if (holder == NULL) return NULL; }
    constructSaveArray(holder->mSlotFlags, 4, sizeof(hdSaveSlotFlags_c), gabi::guest_fn((const void*)createSaveSlotFlags));
    constructSaveArray(holder->mPairs, 4, sizeof(hdSavePair_c), gabi::guest_fn((const void*)createSavePair));
    constructSaveArray(holder->mInventories, 4, sizeof(hdSaveInventory_c), gabi::guest_fn((const void*)createSaveInventory));
    constructSaveArray(holder->mNames, 3, sizeof(hdSaveName_l), gabi::guest_fn((const void*)createSaveName));
    constructSaveArray(holder->mPlayLogs, 4, sizeof(hdSavePlayLog_c), gabi::guest_fn((const void*)createSavePlayLog));
    holder->mSelectedSlot = 4;
    constructSaveArray(holder->mPictureInfo, 4, sizeof(hdSavePictureInfo_l), gabi::guest_fn((const void*)createSavePictureInfo));
    constructSaveArray(holder->mPictures, 4, sizeof(hdSavePicture_l), gabi::guest_fn((const void*)createSavePicture));
    holder->init(); return holder;
}

/* 0271FF4C */
void hdSaveHolders_c::clearSlot(u32 slot) {
    if (slot >= SLOT_COUNT) return;
    mSlotFlags[slot].clear(); mPairs[slot].clear(); mInventories[slot].clear();
    clearSaveName(&mNames[slot]); mPlayLogs[slot].clear(); clearSavePicture(&mPictures[slot]);
    clearSavePictureInfo(&mPictureInfo[slot]); mSelectedSlot = 4;
}

/* 02720004 */
void hdSaveHolders_c::clear() {
    for (u32 slot = 0; slot < SLOT_COUNT; ++slot) clearSlot(slot);
}

/* 0272005C */
void hdSaveHolders_c::clearScratch() {
    mSlotFlags[SLOT_COUNT].setLoaded(0); mInventories[SLOT_COUNT].clear(); mPlayLogs[SLOT_COUNT].clear();
}

/* 027200A0 */
hdSaveSlotFlags_c* hdSaveHolders_c::getSlotFlags(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mSlotFlags[slot];
}

/* 027200B8 */
hdSaveSlotFlags_c* hdSaveHolders_c::getSlotFlagsConst(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mSlotFlags[slot];
}

/* 027200D8 */
hdSavePair_c* hdSaveHolders_c::getPair(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mPairs[slot];
}

/* 027200FC */
hdSaveInventory_c* hdSaveHolders_c::getInventory(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mInventories[slot];
}

/* 02720120 */
hdSavePicture_l* hdSaveHolders_c::getPicture(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mPictures[slot];
}

/* 02720154 */
hdSaveName_l* hdSaveHolders_c::getName(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mNames[slot];
}

/* 02720180 */
hdSavePlayLog_c* hdSaveHolders_c::getPlayLog(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mPlayLogs[slot];
}

/* 027201A4 */
hdSavePictureInfo_l* hdSaveHolders_c::getPictureInfo(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mPictureInfo[slot];
}

/* 027201C0 */
hdSavePictureInfo_l* hdSaveHolders_c::getPictureInfoConst(u32 slot) {
    if (slot >= SLOT_COUNT) slot = 0;
    return &mPictureInfo[slot];
}

/* 027200D0 */
hdSaveSlotFlags_c* hdSaveHolders_c::getScratchFlags() {
    return &mSlotFlags[SLOT_COUNT];
}

/* 027200F4 */
hdSavePair_c* hdSaveHolders_c::getScratchPair() {
    return &mPairs[SLOT_COUNT];
}

/* 02720118 */
hdSaveInventory_c* hdSaveHolders_c::getScratchInventory() {
    return &mInventories[SLOT_COUNT];
}

/* 02720144 */
hdSavePicture_l* hdSaveHolders_c::getScratchPicture() {
    return &mPictures[SLOT_COUNT];
}

/* 0272019C */
hdSavePlayLog_c* hdSaveHolders_c::getScratchPlayLog() {
    return &mPlayLogs[SLOT_COUNT];
}

/* 027201DC */
hdSavePictureInfo_l* hdSaveHolders_c::getScratchPictureInfo() {
    return &mPictureInfo[SLOT_COUNT];
}

/* 02720170 */
hdSaveName_l* hdSaveHolders_c::getCurrentName() {
    return getName(sSaveSelection_hd_save_holders->mSaveInfo.mDataNum);
}

/* 027201E4 */
u32 hdSaveHolders_c::calculateChecksum() {
    void* heap = getSaveHeap();
    u8* packed = (u8*)allocateSaveMemory(PACKED_SIZE, heap, 4);
    setSaveMemory(packed, 0, PACKED_SIZE);
    copySaveMemory(packed, mSlotFlags, SLOT_COUNT * sizeof(hdSaveSlotFlags_c), 0);
    u8* output = packed + 0x30;
    copySaveMemory(output, mPairs, SLOT_COUNT * sizeof(hdSavePair_c), 0); output += 0xC;
    copySaveMemory(output, mInventories, SLOT_COUNT * sizeof(hdSaveInventory_c), 0); output += 0x3C;
    for (u32 slot = 0; slot < SLOT_COUNT; ++slot) {
        hdSaveName_l* name = &mNames[slot];
        gabi::call_ptr<void>(name->mVtable->mEnsureTerminated, name);
        be<u16>* text = name->mpText;
        gabi::call_ptr<void>(name->mVtable->mEnsureTerminated, name);
        be<u16>* scan = name->mpText;
        u32 length = 0;
        while (scan[length] != 0) {
            if (++length > 0x40000) { length = 0; break; }
        }
        copySaveMemory(output, text, length + 1, 0); output += 0x12;
    }
    copySaveMemory(output, mPlayLogs, SLOT_COUNT * sizeof(hdSavePlayLog_c), 0);
    u32 checksum = calculateSaveCrc(packed, PACKED_SIZE);
    freeSaveMemory(packed); return checksum;
}

/* 0272033C */
void hdSaveHolders_c::updateChecksum() {
    mChecksum = calculateChecksum();
}

/* 0272036C */
static void __sinit_hd_save_holders_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSaveHoldersHeaderStatics), gabi::ea(&sSaveHoldersHeaderDescriptors), gabi::ea(&sSaveHoldersHeaderZero), gabi::ea(&sSaveHoldersPiConstants));
}

/* WWHD glue
data sSaveHoldersHeaderStatics 1049F74C nocheck
data sSaveHoldersHeaderDescriptors 101F8494 nocheck
data sSaveHoldersHeaderZero 1049F758 nocheck
data sSaveHoldersPiConstants 10140750 nocheck

 data sSaveSelection_hd_save_holders 101F84DC
 ext 02726424 void initSaveName(hdSaveName_l*)
 ext 027265E0 void clearSaveName(hdSaveName_l*)
 ext 02726470 void* createSaveName(void*) callback
 ext 02726AD4 void initSavePictureInfo(hdSavePictureInfo_l*)
 ext 02726B8C void clearSavePictureInfo(hdSavePictureInfo_l*)
 ext 02726A34 void* createSavePictureInfo(void*) callback
 ext 02725C60 void initSavePicture(hdSavePicture_l*)
 ext 02725DA8 void clearSavePicture(hdSavePicture_l*)
 ext 02725CE0 void* createSavePicture(void*) callback
 ext 028EFFD0 void constructSaveArray(void*,u32,u32,u32)
 ext 0203E8B8 void* getSaveHeap()
 ext 0273B0D4 void* allocateSaveMemory(u32,void*,s32)
 ext C0009990 void setSaveMemory(void*,u32,u32)
 ext C0009988 void copySaveMemory(void*,void*,u32,u32)
 ext 0273B264 u32 calculateSaveCrc(void*,u32)
 ext 0273AFC8 void freeSaveMemory(void*)
*/
