// Additional HD save-manager Miiverse storage helpers. Private decomp-derived source.
#include "d/hd_picture_statics.h"
#include "d/hd_miiverse_name_store.h"
static hdPictureStaticState_c sMiiverseNamesHeaderState;
static hdPictureStaticTokens_c sMiiverseNamesHeaderTokens;
static const hdPictureAngles_c sMiiverseNamesHeaderAngles;
/* 027254D4 */
void hdMiiverseNameStore_c::initialize() {
    nameStoreFill(mNames, 0, sizeof(mNames));
    nameStoreFill(mMetadata, 0, sizeof(mMetadata));
}
/* 02725518 */
static hdMiiverseNameStore_c* constructMiiverseNames(hdMiiverseNameStore_c* self) {
    if (!self) {
        self = static_cast<hdMiiverseNameStore_c*>(nameStoreAllocate(sizeof(hdMiiverseNameStore_c)));
        if (!self) return NULL;
    }
    self->initialize(); return self;
}
/* 02725574 */
void hdMiiverseNameStore_c::clear() { initialize(); }
/* 02725578 */
void hdMiiverseNameStore_c::getName(u32 output, u32 index) {
    if (index >= EntryCount) index = 0;
    nameStoreFormat(output, "%s", mNames[index]);
}
/* 027255A4 */
void hdMiiverseNameStore_c::setName(u32 input, u32 index) {
    if (index >= EntryCount) index = 0;
    nameStoreCopy(mNames[index], input, EntrySize, 0);
}
/* 027255C4 */
u32 hdMiiverseNameStore_c::metadata(u32 index) {
    return index < EntryCount ? u32(mMetadata[index]) : 0;
}
/* 027255E4 */
void hdMiiverseNameStore_c::setMetadata(u32 index, u32 value) {
    if (index < EntryCount) mMetadata[index] = value;
}
/* 027255FC */
static void initializeMiiverseNameStatics() {
    initializePictureHeaderStatics(&sMiiverseNamesHeaderState, &sMiiverseNamesHeaderTokens, &sMiiverseNamesHeaderAngles);
}
/* WWHD glue
 data sMiiverseNamesHeaderState 1049F7CC
 data sMiiverseNamesHeaderTokens 101F8534
 data sMiiverseNamesHeaderAngles 10140B24
 str 10140ABC "%s" in 02725578
 ext C0009990 void nameStoreFill(void*, u32, u32)
 ext 02759C28 void nameStoreFormat(u32, const char*, const char*)
 ext C0009988 void nameStoreCopy(void*, u32, u32, u32)
 ext 0273AD10 void* nameStoreAllocate(u32)
*/
