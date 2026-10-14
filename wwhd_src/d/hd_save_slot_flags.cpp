/** HD save helpers; local decomp */
#include "d/hd_save_slot_flags.h"
static u32 sSaveSlotFlagsHeaderStatics;
static u32 sSaveSlotFlagsHeaderDescriptors;
static u32 sSaveSlotFlagsHeaderZero;
static u32 sSaveSlotFlagsPiConstants;

/* 0271FB68 */
void hdSaveSlotFlags_c::init() {
    for (auto& value : mProgress) value = 0;
    mCreated = 0; mLoaded = 0; mChanged = 0; mStatus = 0;
    for (auto& value : mReserved) value = 0;
}

/* 0271FBB0 */
hdSaveSlotFlags_c* createSaveSlotFlags(hdSaveSlotFlags_c* record) {
    if (record == NULL) { record = (hdSaveSlotFlags_c*)operator_new(sizeof(hdSaveSlotFlags_c)); if (record == NULL) return NULL; }
    record->init(); return record;
}

/* 0271FBE8 */
void hdSaveSlotFlags_c::clear() {
    mChanged = 0; mProgress[3] = 0; mCreated = 1; mStatus = 0; mLoaded = 0;
    mProgress[1] = 0; mProgress[0] = 0; mProgress[2] = 0;
    for (auto& value : mReserved) value = 0;
}

/* 0271FC30 */
BOOL hdSaveSlotFlags_c::isCreated() {
    return mCreated == 1;
}

/* 0271FC44 */
u32 hdSaveSlotFlags_c::getCreated() {
    return mCreated;
}

/* 0271FC4C */
void hdSaveSlotFlags_c::setCreated(u32 value) {
    mCreated = value != 0;
}

/* 0271FC5C */
BOOL hdSaveSlotFlags_c::isLoaded() {
    return mLoaded == 1;
}

/* 0271FC70 */
void hdSaveSlotFlags_c::setLoaded(u32 value) {
    mLoaded = value;
}

/* 0271FC78 */
BOOL hdSaveSlotFlags_c::isChanged() {
    return mChanged == 1;
}

/* 0271FC8C */
u32 hdSaveSlotFlags_c::getChanged() {
    return mChanged;
}

/* 0271FC94 */
void hdSaveSlotFlags_c::setChanged(u32 value) {
    mChanged = value != 0;
}

/* 0271FCA4 */
u32 hdSaveSlotFlags_c::getStatus() {
    return mStatus;
}

/* 0271FCAC */
void hdSaveSlotFlags_c::setStatus(u32 value) {
    mStatus = value;
}

/* 0271FCB4 */
void hdSaveSlotFlags_c::copyFrom(hdSaveSlotFlags_c* source) {
    mProgress[0] = source->mProgress[0];
    mProgress[1] = source->mProgress[1];
    mProgress[2] = source->mProgress[2];
    mProgress[3] = source->mProgress[3];
    mProgress[4] = source->mProgress[4];
    mCreated = source->mCreated; mLoaded = source->mLoaded; mChanged = source->mChanged; mStatus = source->mStatus;
}

/* 0271FD00 */
static void __sinit_hd_save_slot_flags_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sSaveSlotFlagsHeaderStatics), gabi::ea(&sSaveSlotFlagsHeaderDescriptors), gabi::ea(&sSaveSlotFlagsHeaderZero), gabi::ea(&sSaveSlotFlagsPiConstants));
}

/* WWHD glue
data sSaveSlotFlagsHeaderStatics 1049F730 nocheck
data sSaveSlotFlagsHeaderDescriptors 101F8470 nocheck
data sSaveSlotFlagsHeaderZero 1049F73C nocheck
data sSaveSlotFlagsPiConstants 10140748 nocheck
*/
