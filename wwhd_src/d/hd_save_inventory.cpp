/** HD save helpers; local decomp */
#include "d/hd_save_inventory.h"
static u32 sSaveInventoryHeaderStatics;
static u32 sSaveInventoryHeaderDescriptors;
static u32 sSaveInventoryHeaderZero;
static u32 sSaveInventoryPiConstants;

/* 0271F7D8 */
hdSaveInventory_c* createSaveInventory(hdSaveInventory_c* record) {
    if (record == NULL) { record = (hdSaveInventory_c*)operator_new(sizeof(hdSaveInventory_c)); if (record == NULL) return NULL; }
    record->mCount = 0; record->mFlags = 0; record->mSelection = 0;
    for (auto& value : record->mEntries) value = 0;
    for (auto& value : record->mReserved) value = 0;
    return record;
}

/* 0271F86C */
void hdSaveInventory_c::init() {
    mFlags = 0;
    for (auto& value : mEntries) value = 0;
    mCount = 0; mSelection = 0;
    for (auto& value : mReserved) value = 0;
}

/* 0271F8C0 */
void hdSaveInventory_c::clear() {
    mFlags = 0;
    for (auto& value : mEntries) value = 0;
    mCount = 0; mSelection = 0;
    for (auto& value : mReserved) value = 0;
}

/* 0271F914 */
void hdSaveInventory_c::copyFrom(hdSaveInventory_c* source) {
    mFlags = source->mFlags;
    mEntries[0] = source->mEntries[0];
    mEntries[1] = source->mEntries[1];
    mEntries[2] = source->mEntries[2];
    mEntries[3] = source->mEntries[3];
    mEntries[4] = source->mEntries[4];
    mEntries[5] = source->mEntries[5];
    mEntries[6] = source->mEntries[6];
    mEntries[7] = source->mEntries[7];
    mEntries[8] = source->mEntries[8];
    mEntries[9] = source->mEntries[9];
    mEntries[10] = source->mEntries[10];
    mEntries[11] = source->mEntries[11];
    mCount = source->mCount; mSelection = source->mSelection;
}

/* 0271F954 */
BOOL hdSaveInventory_c::hasFlags(u32 flags) {
    return (u32(mFlags) & flags) != 0;
}

/* 0271F968 */
void hdSaveInventory_c::setFlags(u32 flags) {
    mFlags = u32(mFlags) | flags;
}

/* 0271F978 */
u32 hdSaveInventory_c::getEntry(u32 index) {
    return index < CAPACITY ? u32(mEntries[index]) : 0;
}

/* 0271F994 */
void hdSaveInventory_c::setEntry(u32 index, u32 value) {
    if (index < CAPACITY) mEntries[index] = value;
}

/* 0271F9A8 */
void hdSaveInventory_c::addCount(u32 count) {
    u32 total = u32(mCount) + count;
    if (total <= CAPACITY) mCount = total;
}

/* 0271F9C4 */
static void __sinit_hd_save_inventory_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSaveInventoryHeaderStatics), gabi::ea(&sSaveInventoryHeaderDescriptors), gabi::ea(&sSaveInventoryHeaderZero), gabi::ea(&sSaveInventoryPiConstants));
}

/* WWHD glue
data sSaveInventoryHeaderStatics 1049F6F8 nocheck
data sSaveInventoryHeaderDescriptors 101F8428 nocheck
data sSaveInventoryHeaderZero 1049F704 nocheck
data sSaveInventoryPiConstants 10140738 nocheck
*/
