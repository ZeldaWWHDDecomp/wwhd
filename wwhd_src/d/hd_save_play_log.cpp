/** HD save helpers; local decomp */
#include "d/hd_save_info_holder.h"
static u32 sSavePlayLogHeaderStatics;
static u32 sSavePlayLogHeaderDescriptors;
static u32 sSavePlayLogHeaderZero;
static u32 sSavePlayLogPiConstants;

static u32 sSaveInfoVtable_hd_save_play_log;
static void destroySaveArray(void*,u32,u32,u32,s32,s32);
dSv_memory_c* createSaveMemory(dSv_memory_c*);
static void initSaveMemory(void*);
static void destroySavePictureManager(void*,s32);
static void destroySavePictureInfo(void*,s32);
static void destroySaveName(void*,s32);
static void destroySaveInventory(void*,s32);
static void destroySaveHost(void*,s32);
static void deleteSaveMemory(void*);

static void clearSaveMemory(void*, u32, u32);

/* 027207B0 */
void hdSavePlayLog_c::init() {
    mStart.mXBits = 0; mStart.mYBits = 0; mEndTime = 0.0f;
    mEnd.mXBits = 0; mEnd.mYBits = 0; mStartTime = 0.0f;
    clearSaveMemory(mFlags, 0, sizeof(mFlags));
}

/* 027207E0 */
hdSavePlayLog_c* createSavePlayLog(hdSavePlayLog_c* record) {
    if (record == NULL) { record = (hdSavePlayLog_c*)operator_new(sizeof(hdSavePlayLog_c)); if (record == NULL) return NULL; }
    // GHS retains the null-preserving subobject constructor allocation test.
    if (gabi::ea(&record->mEnd) == 0) operator_new(sizeof(hdSavePoint_c));
    record->init(); return record;
}

/* 02720848 */
void hdSavePlayLog_c::clear() {
    init();
}

/* 0272084C */
void hdSavePlayLog_c::setStart(hdSavePoint_c* point) {
    mStart.mXBits = point->mXBits; mStart.mYBits = point->mYBits;
}

/* 02720864 */
void hdSavePlayLog_c::setStartTime(f32 value) {
    mStartTime = value;
}

/* 0272086C */
f32 hdSavePlayLog_c::getStartTime() {
    return mStartTime;
}

/* 02720874 */
void hdSavePlayLog_c::setEnd(hdSavePoint_c* point) {
    mEnd.mXBits = point->mXBits; mEnd.mYBits = point->mYBits;
}

/* 02720888 */
hdSavePoint_c* hdSavePlayLog_c::getEnd() {
    return &mEnd;
}

/* 02720890 */
void hdSavePlayLog_c::setEndTime(f32 value) {
    mEndTime = value;
}

/* 02720898 */
f32 hdSavePlayLog_c::getEndTime() {
    return mEndTime;
}

/* 027208A0 */
void hdSavePlayLog_c::setFlag(u32 index, u32 bit) {
    u32 mask = (bit & 63) < 32 ? 1u << (bit & 31) : 0;
    mFlags[index] = u32(mFlags[index]) | mask;
}

/* 027208C0 */
void hdSavePlayLog_c::setVisited(u32 index) {
    setFlag(index, 0);
}

/* 027208C8 */
BOOL hdSavePlayLog_c::hasFlag(u32 index, u32 bit) {
    u32 mask = (bit & 63) < 32 ? 1u << (bit & 31) : 0;
    return (u32(mFlags[index]) & mask) != 0;
}

/* 027208EC */
BOOL hdSavePlayLog_c::isVisited(u32 index) {
    return hasFlag(index, 0);
}

/* 027208F4 */
void hdSavePlayLog_c::copyFrom(hdSavePlayLog_c* source) {
    mStart.mXBits = source->mStart.mXBits; mStart.mYBits = source->mStart.mYBits;
    // Bit copies preserve the signalling NaNs of GHS lfs/stfs copies.
    *(be<u32>*)&mStartTime = *(be<u32>*)&source->mStartTime;
    mEnd.mXBits = source->mEnd.mXBits; mEnd.mYBits = source->mEnd.mYBits;
    *(be<u32>*)&mEndTime = *(be<u32>*)&source->mEndTime;
    for (u32 i = 0; i < 49; ++i) mFlags[i] = source->mFlags[i];
}

/* 0272094C */
static void __sinit_hd_save_play_log_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSavePlayLogHeaderStatics), gabi::ea(&sSavePlayLogHeaderDescriptors), gabi::ea(&sSavePlayLogHeaderZero), gabi::ea(&sSavePlayLogPiConstants));
}

/* 02720684 */
dSv_memory_c* createSaveMemory(dSv_memory_c* memory) {
    if (memory == NULL) { memory = (dSv_memory_c*)operator_new(sizeof(dSv_memory_c)); if (memory == NULL) return NULL; }
    initSaveMemory(memory); return memory;
}

/* 027206E0 */
static void destroySaveInfoHolder(hdSaveInfoHolder_c* holder, u32 flags) {
    if (holder == NULL) return;
    holder->mVtable = gabi::ea(&sSaveInfoVtable_hd_save_play_log);
    destroySavePictureManager(holder->mPictureManager, 2);
    destroySaveArray(holder->mHolders.mPictureInfo, 4, sizeof(hdSavePictureInfo_l), gabi::guest_fn((const void*)destroySavePictureInfo), 0, 0);
    destroySaveArray(holder->mHolders.mNames, 3, sizeof(hdSaveName_l), gabi::guest_fn((const void*)destroySaveName), 0, 0);
    destroySaveArray(holder->mHolders.mInventories, 4, sizeof(hdSaveInventory_c), gabi::guest_fn((const void*)destroySaveInventory), 0, 0);
    destroySaveHost(holder, 0);
    if (flags & 1) deleteSaveMemory(holder);
}

/* WWHD glue
data sSavePlayLogHeaderStatics 1049F784 nocheck
data sSavePlayLogHeaderDescriptors 101F84E4 nocheck
data sSavePlayLogHeaderZero 1049F790 nocheck
data sSavePlayLogPiConstants 1014078C nocheck

 data sSaveInfoVtable_hd_save_play_log 10140758
 ext 028F0164 void destroySaveArray(void*,u32,u32,u32,s32,s32)
 ext 025B9170 void initSaveMemory(void*)
 ext 02725560 void destroySavePictureManager(void*,s32)
 ext 02726AC0 void destroySavePictureInfo(void*,s32) callback
 ext 027206CC void destroySaveName(void*,s32) callback
 ext 0271F858 void destroySaveInventory(void*,s32) callback
 ext 02752BEC void destroySaveHost(void*,s32)
 ext 0273AF40 void deleteSaveMemory(void*)
 ext C0009990 void clearSaveMemory(void*, u32, u32)
*/
