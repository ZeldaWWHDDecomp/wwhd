/* The light collections' record accessors, kinds and empty record-walk hooks. An index past the record array
 * selects the first record, as everywhere in the collections.
 * References: WWHD's own instructions only. */
#include "agl/LightCollections.h"
using namespace agl;

/* ---- directed lights ---- */

/* 027D1D60 */
void agl::DirectedLightCollection::setCount(u32 count) { mCount = count; }
/* 027D1D68 */
u32 agl::DirectedLightCollection::isEnabled(u32 index) const {
    const ExtendedRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    return record->mFlags & Record_Enabled;
}
/* 027D1D8C */
void agl::DirectedLightCollection::setRecordEnabled(u32 index, u32 enabled) {
    ExtendedRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    if (enabled) {
        u32 mask = record->mMask;
        record->mMask = mask | ~0u;
    } else {
        record->mMask = 0;
    }
}
/* 027D1DD0 */
u32 agl::DirectedLightCollection::canApply(u32 index, u32 valueIndex) const {
    const ExtendedRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    u32 mask = record->mMask;
    u32 shift = valueIndex & 63;
    u32 bit = shift < 32 ? 1u << shift : 0;
    return (mask & bit) != 0;
}
/* 027D1ED8 */
s32 agl::DirectedLightCollection::kind() const { return 1; }
/* 027D1E04 */
void agl::DirectedLightCollection::beforeUpdate() {}
/* 027D1E08 */
void agl::DirectedLightCollection::beforeRefresh() {}
/* 027D1E0C */
void agl::DirectedLightCollection::refreshRecord() {}
/* 027D1E10 */
void agl::DirectedLightCollection::prepareApply() {}
/* 027D1E14 */
void agl::DirectedLightCollection::beginApply() {}
/* 027D1E18 */
void agl::DirectedLightCollection::prepareContext() {}

/* ---- spatial lights ---- */

/* 027D2B18 */
void agl::SpatialLightCollection::setCount(u32 count) { mCount = count; }
/* 027D2B20 */
u32 agl::SpatialLightCollection::isEnabled(u32 index) const {
    const SpatialRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    return record->mFlags & Record_Enabled;
}
/* 027D2B44 */
void agl::SpatialLightCollection::setRecordEnabled(u32 index, u32 enabled) {
    SpatialRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    if (enabled) {
        u32 mask = record->mMask;
        record->mMask = mask | ~0u;
    } else {
        record->mMask = 0;
    }
}
/* 027D2B88 */
u32 agl::SpatialLightCollection::canApply(u32 index, u32 valueIndex) const {
    const SpatialRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    u32 mask = record->mMask;
    u32 shift = valueIndex & 63;
    u32 bit = shift < 32 ? 1u << shift : 0;
    return (mask & bit) != 0;
}
/* 027D2C90 */
s32 agl::SpatialLightCollection::kind() const { return 2; }
/* 027D2BBC */
void agl::SpatialLightCollection::beforeUpdate() {}
/* 027D2BC0 */
void agl::SpatialLightCollection::beforeRefresh() {}
/* 027D2BC4 */
void agl::SpatialLightCollection::refreshRecord() {}
/* 027D2BC8 */
void agl::SpatialLightCollection::prepareApply() {}
/* 027D2BCC */
void agl::SpatialLightCollection::beginApply() {}
/* 027D2BD0 */
void agl::SpatialLightCollection::prepareContext() {}

/* ---- paired point lights ---- */

/* 027D37C8 */
void agl::PairedLightCollection::setCount(u32 count) { mCount = count; }
/* 027D37D0 */
u32 agl::PairedLightCollection::count() const { return mCount; }
/* 027D37D8 */
u32 agl::PairedLightCollection::capacity() const { return mCapacity; }
/* 027D37E0 */
u32 agl::PairedLightCollection::isEnabled(u32 index) const {
    const CurveRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    return record->mFlags & Record_Enabled;
}
/* 027D3804 */
void agl::PairedLightCollection::setRecordEnabled(u32 index, u32 enabled) {
    CurveRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    if (enabled) {
        u32 mask = record->mMask;
        record->mMask = mask | ~0u;
    } else {
        record->mMask = 0;
    }
}
/* 027D3848 */
u32 agl::PairedLightCollection::canApply(u32 index, u32 valueIndex) const {
    const CurveRecord* record = clampedElement(mRecords.get(), mCapacity, index);
    u32 mask = record->mMask;
    u32 shift = valueIndex & 63;
    u32 bit = shift < 32 ? 1u << shift : 0;
    return (mask & bit) != 0;
}
/* 027D3954 */
s32 agl::PairedLightCollection::kind() const { return 0; }
/* 027D387C */
void agl::PairedLightCollection::releaseRecord() {}
/* 027D3880 */
void agl::PairedLightCollection::beforeUpdate() {}
/* 027D3884 */
void agl::PairedLightCollection::beforeRefresh() {}
/* 027D3888 */
void agl::PairedLightCollection::refreshRecord() {}
/* 027D388C */
void agl::PairedLightCollection::prepareApply() {}
/* 027D3890 */
void agl::PairedLightCollection::beginApply() {}
/* 027D3894 */
void agl::PairedLightCollection::prepareContext() {}
/* WWHD glue
*/
