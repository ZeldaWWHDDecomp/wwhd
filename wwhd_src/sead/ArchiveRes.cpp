/* WWHD sead archive resources: creating an archive resource prepares its image (the archive format's own
 * check), and this translation unit's copies of its text classes.
 * Reconstructed from this game's instructions. */
#include "sead/ArchiveRes.h"

using namespace sead;

static void releaseObject(void* object);

static constexpr u32 kDestroyRelease = 1;

/* 0275CF28 */
void sead::ArchiveRes::doCreate(u8* data, u32 size, RegistryHeap* heap) {
    mEnable = gabi::call_ptr<u8>(archiveVtable()->mPrepareArchive.mFunction, this, data);
}

/* 0275CF64 */
void sead::archive_res_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CF78 */
void sead::archive_res_text::Text::assureTerminationImpl() {}

/* 0275CF7C */
void sead::archive_res_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CF90 */
void sead::archive_res_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0275CFA8 */
void sead::archive_res_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275CFBC */
void sead::archive_res_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 ext 0273AF40 void releaseObject(void*)
*/
