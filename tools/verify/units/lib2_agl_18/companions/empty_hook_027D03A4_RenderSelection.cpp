// Render collection selection accessors and this translation unit's text interface copies.
#include "agl/RenderSelection.h"

void deleteSelectionStorage(void* storage);
void releaseSelectionArray(void* storage);

namespace agl {
static sead::SafeTextVtable sCollectionOwnedTextTable;

/* 027D093C */
void agl::RenderSelection_l::updateActiveRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginUpdate.mFunction, this);
    RenderSelectionRecord_l* records = mRecords;
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    u32 count = activeCount <= capacity ? activeCount : capacity;
    RenderSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<RenderSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (RenderSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(RenderSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled)
            gabi::call_ptr<void>(mMethods->mUpdateRecord.mFunction, this, record);
    }
}

/* 027D0BF0 */
void agl::RenderSelection_l::finishActiveRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginFinish.mFunction, this);
    RenderSelectionRecord_l* records = mRecords;
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    RenderSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<RenderSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (RenderSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(RenderSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled)
            gabi::call_ptr<void>(mMethods->mFinishRecord.mFunction, this, record);
    }
}

/* 027D1654 */
void agl::ExtendedRenderSelection_l::updateActiveRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginUpdate.mFunction, this);
    ExtendedSelectionRecord_l* records = mRecords;
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    u32 count = activeCount <= capacity ? activeCount : capacity;
    ExtendedSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<ExtendedSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (ExtendedSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(ExtendedSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled)
            gabi::call_ptr<void>(mMethods->mUpdateRecord.mFunction, this, record);
    }
}

/* 027D1908 */
void agl::ExtendedRenderSelection_l::finishActiveRecords() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (u32(mActiveCount) == 0)
        return;
    gabi::call_ptr<void>(mMethods->mBeginFinish.mFunction, this);
    ExtendedSelectionRecord_l* records = mRecords;
    u32 activeCount = mActiveCount;
    u32 capacity = mRecordCount;
    u32 count = activeCount <= capacity ? activeCount : u32(mRecordCount);
    ExtendedSelectionRecord_l* end = records + count;
    gabi::FrameLocal<gptr<ExtendedSelectionRecord_l>> rangeEnd(8);
    *rangeEnd = end;
    for (ExtendedSelectionRecord_l* record = records; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(records)) / sizeof(ExtendedSelectionRecord_l);
        u32 enabled = gabi::call_ptr<u32>(mMethods->mIsRecordEnabled.mFunction, this, index);
        if (enabled)
            gabi::call_ptr<void>(mMethods->mFinishRecord.mFunction, this, record);
    }
}

/* 027CB7F0 */
void agl::render_selection_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027CB804 */
void agl::render_selection_text::Text::assureTerminationImpl() {}

/* 027CB808 */
void agl::render_selection_text::TextBufferA::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 027CB820 */
void agl::render_selection_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027CB834 */
void agl::render_selection_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027CB848 */
void agl::render_selection_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D0390 */
void agl::collection_selection_text::Text::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D03A4 */
void agl::collection_selection_text::Text::assureTerminationImpl() { gabi::store<u32>(gabi::ea(this), 0xC0DEC0DEu); }

/* 027D03A8 */
void agl::collection_selection_text::TextBufferA::assureTerminationImpl() {
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 027D03C0 */
void agl::collection_selection_text::TextBufferA::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D0438 */
void agl::collection_selection_text::TextBufferB::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D044C */
void agl::collection_selection_text::TextBufferC::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D0460 */
void agl::collection_selection_text::TextBufferD::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1))
        deleteSelectionStorage(this);
}

/* 027D1048 */
void agl::RenderSelection_l::setActiveCount(u32 count) { mActiveCount = count; }

/* 027D1050 */
u32 agl::RenderSelection_l::activeCount() const { return mActiveCount; }

/* 027D1058 */
u32 agl::RenderSelection_l::isRecordEnabled(u32 index) const {
    u32 count = mRecordCount;
    RenderSelectionRecord_l* record = mRecords;
    if (index < count)
        record += index;
    return u16(record->mFlags) & 1;
}

/* 027D107C */
void agl::RenderSelection_l::setRecordMask(u32 index, u32 enabled) {
    u32 count = mRecordCount;
    RenderSelectionRecord_l* record = mRecords;
    if (index < count)
        record += index;
    if (enabled) {
        u32 mask = record->mSelectionMask;
        record->mSelectionMask = mask | 0xFFFFFFFFu;
    } else {
        record->mSelectionMask = 0;
    }
}

/* 027D10C0 */
u32 agl::RenderSelection_l::testRecordMask(u32 index, u32 bit) const {
    u32 count = mRecordCount;
    RenderSelectionRecord_l* record = mRecords;
    if (index < count)
        record += index;
    u32 mask = record->mSelectionMask;
    u32 shift = bit & 63;
    u32 selected = shift < 32 ? (1u << shift) : 0;
    return (mask & selected) != 0;
}

/* 027D10F4 */
void agl::RenderSelection_l::emptyHookA() {}

/* 027D10F8 */
void agl::RenderSelection_l::emptyHookB() {}

/* 027D10FC */
void agl::RenderSelection_l::emptyHookC() {}

/* 027D1100 */
void agl::RenderSelection_l::emptyHookD() {}

/* 027D1104 */
void agl::RenderSelection_l::emptyHookE() {}

/* 027D1108 */
void agl::RenderSelection_l::emptyHookF() {}

/* 027D110C */
void agl::RenderSelection_l::emptyHookG() {}

/* 027D11CC */
u32 agl::RenderSelection_l::unavailableHook() const { return 0; }

/* 027D03D4 */
void agl::collection_selection_text::OwnedText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0) {
        be<u8>* text = mText;
        mVtable = &sCollectionOwnedTextTable;
        if (gabi::ea(text) != 0)
            releaseSelectionArray(text);
        if (flags & 1)
            deleteSelectionStorage(this);
    }
}
}

/* WWHD glue
data sCollectionOwnedTextTable 1016A830
ext 0273AF40 void deleteSelectionStorage(void* storage)
ext 0273AFC8 void releaseSelectionArray(void* storage)
*/
