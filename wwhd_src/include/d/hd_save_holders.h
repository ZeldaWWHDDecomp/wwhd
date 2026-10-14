/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "d/hd_save_inventory.h"
#include "d/hd_save_pair.h"
#include "d/hd_save_slot_flags.h"
#include "d/hd_save_play_log.h"

// Soft-dependency layouts are local until INF-picture_save is merged.
struct hdSaveNameVtable_l {
    u8 mReserved[0x14]; be<u32> mEnsureTerminated;
};
WWHD_OFFSET(hdSaveNameVtable_l,mEnsureTerminated,0x14);
struct hdSaveName_l {
    gptr<be<u16>> mpText;
    gptr<hdSaveNameVtable_l> mVtable;
    u8 mStorage[0x38];
};
WWHD_OFFSET(hdSaveName_l,mVtable,4);
WWHD_SIZE(hdSaveName_l,0x40);
struct hdSavePicture_l { u8 mStorage[0x3C0340]; };
struct hdSavePictureInfo_l { u8 mStorage[0x7C]; };
WWHD_SIZE(hdSavePicture_l,0x3C0340);
WWHD_SIZE(hdSavePictureInfo_l,0x7C);
struct hdSaveHolders_c {
    enum { SLOT_COUNT = 3, ARRAY_COUNT = 4, PACKED_SIZE = 0x342 };
    /* 0x00 */ hdSaveSlotFlags_c mSlotFlags[ARRAY_COUNT];
    /* 0x40 */ hdSavePair_c mPairs[ARRAY_COUNT];
    /* 0x50 */ hdSaveInventory_c mInventories[ARRAY_COUNT];
    /* 0xA0 */ hdSaveName_l mNames[SLOT_COUNT];
    /* 0x160 */ hdSavePlayLog_c mPlayLogs[ARRAY_COUNT];
    /* 0x4D0 */ be<u32> mSelectedSlot;
    /* 0x4D4 */ be<u32> mChecksum;
    /* 0x4D8 */ hdSavePictureInfo_l mPictureInfo[ARRAY_COUNT];
    /* 0x6C8 */ u8 mAlignment[0x38];
    /* 0x700 */ hdSavePicture_l mPictures[ARRAY_COUNT];
    void init(); void clearSlot(u32); void clear(); void clearScratch();
    hdSaveSlotFlags_c* getSlotFlags(u32); hdSaveSlotFlags_c* getSlotFlagsConst(u32); hdSaveSlotFlags_c* getScratchFlags();
    hdSavePair_c* getPair(u32); hdSavePair_c* getScratchPair();
    hdSaveInventory_c* getInventory(u32); hdSaveInventory_c* getScratchInventory();
    hdSavePicture_l* getPicture(u32); hdSavePicture_l* getScratchPicture();
    hdSaveName_l* getName(u32); hdSaveName_l* getCurrentName();
    hdSavePlayLog_c* getPlayLog(u32); hdSavePlayLog_c* getScratchPlayLog();
    hdSavePictureInfo_l* getPictureInfo(u32); hdSavePictureInfo_l* getPictureInfoConst(u32); hdSavePictureInfo_l* getScratchPictureInfo();
    u32 calculateChecksum(); void updateChecksum();
};
WWHD_OFFSET(hdSaveHolders_c,mPairs,0x40);
WWHD_OFFSET(hdSaveHolders_c,mInventories,0x50);
WWHD_OFFSET(hdSaveHolders_c,mNames,0xA0);
WWHD_OFFSET(hdSaveHolders_c,mPlayLogs,0x160);
WWHD_OFFSET(hdSaveHolders_c,mSelectedSlot,0x4D0);
WWHD_OFFSET(hdSaveHolders_c,mChecksum,0x4D4);
WWHD_OFFSET(hdSaveHolders_c,mPictureInfo,0x4D8);
WWHD_OFFSET(hdSaveHolders_c,mPictures,0x700);
WWHD_SIZE(hdSaveHolders_c,0xF01400);
hdSaveInventory_c* createSaveInventory(hdSaveInventory_c*);
hdSavePair_c* createSavePair(hdSavePair_c*);
hdSaveSlotFlags_c* createSaveSlotFlags(hdSaveSlotFlags_c*);
hdSavePlayLog_c* createSavePlayLog(hdSavePlayLog_c*);
hdSaveHolders_c* createSaveHolders(hdSaveHolders_c*);
