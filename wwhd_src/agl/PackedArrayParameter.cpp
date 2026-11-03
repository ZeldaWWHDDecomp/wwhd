// Packed-array copy and storage-view reconstruction. External queries remain opaque.
#include "agl/PackedArrayParameter.h"
using namespace agl::packed_array;

/* 0278C9CC */
void Parameter::copyFrom(const Parameter* source) {
    gabi::AutoFrame frame{0x0278C9CCu};
    u32 destinationKindQuery = mVtable->mKind.get();
    QueryTable* sourceTable = source->mVtable;
    u32 destinationKind = gabi::call_ptr<u32>(destinationKindQuery, this);
    u32 sourceKindQuery = sourceTable->mKind.get();
    u32 sourceKind = gabi::call_ptr<u32>(sourceKindQuery, source);
    if (destinationKind != sourceKind) return;
    u32 writeQuery = mVtable->mWriteData.get();
    void* destinationData = gabi::call_ptr<void*>(writeQuery, this);
    u32 readQuery = source->mVtable->mReadData.get();
    const void* sourceData = gabi::call_ptr<const void*>(readQuery, source);
    u32 sizeQuery = mVtable->mByteSize.get();
    u32 bytes = gabi::call_ptr<u32>(sizeQuery, this);
    movePackedParameterBytes(destinationData, sourceData, bytes, 0);
    for (u32 i = 0; i != 4; ++i) {
        mDescriptors[i].mCount = source->mDescriptors[i].mCount;
        mDescriptors[i].mKind = source->mDescriptors[i].mKind;
    }
}

/* 0278CACC */
void Parameter::restoreViews(u32, u32 serializedSize) {
    gabi::AutoFrame frame{0x0278CACCu};
    u32 query = mVtable->mByteSize.get();
    u32 expectedSize = gabi::call_ptr<u32>(query, this);
    if (serializedSize != expectedSize) {
        for (u32 i = 0; i != 4; ++i) {
            mDescriptors[i].mCapacity = 30;
            mDescriptors[i].mSamples = mArrays[i].mSamples;
        }
        return;
    }
    for (u32 i = 0; i != 4; ++i) {
        u32 count = mArrays[i].mCount;
        mDescriptors[i].mSamples = mArrays[i].mSamples;
        mDescriptors[i].mCount = static_cast<u8>(count);
        mDescriptors[i].mCapacity = 30;
        u32 kind = mArrays[i].mKind;
        mDescriptors[i].mKind = static_cast<u8>(kind);
    }
}

/* WWHD glue
ext C0009988 void movePackedParameterBytes(void*,const void*,u32,u32)
*/
