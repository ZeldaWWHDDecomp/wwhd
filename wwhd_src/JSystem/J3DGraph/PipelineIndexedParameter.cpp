// Format the original texmtx%d key and return its parameter payload, with an optional index result.
#include "JSystem/J3DGraph/PipelineColorParameters.h"
#include "sead/ElementAt.h"
#include "sead/FileDevice.h"
using namespace j3d::pipeline;
void formatPipelineIndexedName(sead::SafeTextBuffer*,const char*,s32);
s32 findPipelineParameter(void*,const char*);
static u8 sPipelineTextureMatrixFormat;
static u8 sPipelineTexturePivotEditFormat;
static u8 sPipelineTextureMatrixEditFormat;
static sead::SafeTextVtable sPipelineTextureMatrixNameMethods;
namespace {
template<class T> T* indexedParameterRelative(const be<u32>* field) {
    u32 offset=*field;
    return offset==0?nullptr:gabi::at<T>(gabi::ea(field)+offset);
}
void prepareIndexedParameterName(sead::FixedSafeText<64>* name,const char* format,s32 index) {
    name->mText=&name->mStorage[0];
    name->mCapacity=64;
    name->mStorage[0]=0;
    name->mStorage[63]=0;
    name->mVtable=&sPipelineTextureMatrixNameMethods;
    gabi::cpu->cr[6]=0;
    formatPipelineIndexedName(name,format,index);
    name->assureTermination();
}
void markIndexedParameter(PipelineParameterSet_l* parameters,u32 index) {
    parameters->mFlags=static_cast<u16>(parameters->mFlags)|4u;
    be<u32>* words=parameters->mDirtyWords;
    auto* word=&words[index>>5];
    *word=static_cast<u32>(*word)|(1u<<(index&31u));
}
void* editableParameterPayload(PipelineParameterSet_l* parameters,s32 selected,u32 extraOffset) {
    PipelineParameterResource_l* resource=parameters->mResource;
    auto* records=indexedParameterRelative<PipelineParameterRecord_l>(&resource->mRecordsOffset);
    auto* record=&sead::elementAt<PipelineParameterRecord_l>(gabi::ea(records),static_cast<u32>(selected)); /* guest EA arithmetic: wraps at 32 bits as on the console */
    if(record->mSignedWord4>=0) {
        markIndexedParameter(parameters,static_cast<u32>(selected));
        resource=parameters->mResource;
    }
    records=indexedParameterRelative<PipelineParameterRecord_l>(&resource->mRecordsOffset);
    u16 linkedIndex=record->mLinkedIndex;
    auto* linked=&records[linkedIndex];
    if(linked->mSignedWord4>=0) markIndexedParameter(parameters,linkedIndex);
    be<u8>* values=parameters->mValues;
    u16 offset=record->mDataOffset;
    return static_cast<void*>(values+offset+extraOffset);
}
}
namespace j3d::pipeline {
/* 027FA974 */
void* PipelineParameterSet_l::findTextureMatrix(s32 index,be<s32>* selectedIndex) {
    gabi::AutoFrame frame{0x027FA974u};
    gabi::FrameLocal<sead::FixedSafeText<64>> name(8);
    name->mText=&name->mStorage[0];
    name->mCapacity=64;
    name->mStorage[0]=0;
    name->mStorage[63]=0;
    name->mVtable=&sPipelineTextureMatrixNameMethods;
    // This call supplies one integer variadic argument and no floating-point arguments.
    gabi::cpu->cr[6]=0;
    formatPipelineIndexedName(name.get(),reinterpret_cast<const char*>(&sPipelineTextureMatrixFormat),index);
    name->assureTermination();
    PipelineParameterResource_l* resource=mResource;
    void* lookup=indexedParameterRelative<void>(&resource->mLookupOffset);
    s32 selected=findPipelineParameter(lookup,reinterpret_cast<const char*>(name->mText.get()));
    void* result=nullptr;
    if(selected>=0) {
        resource=mResource;
        auto* records=indexedParameterRelative<PipelineParameterRecord_l>(&resource->mRecordsOffset);
        be<u8>* values=mValues;
        auto* record=&sead::elementAt<PipelineParameterRecord_l>(gabi::ea(records),static_cast<u32>(selected)); /* guest EA arithmetic: wraps at 32 bits as on the console */
        u16 offset=record->mDataOffset;
        result=static_cast<void*>(values+offset);
    }
    if(selectedIndex!=nullptr) *selectedIndex=selected;
    return result;
}
/* 027FA678 */
void* PipelineParameterSet_l::editTexturePivot(s32 index) {
    gabi::AutoFrame frame{0x027FA678u};
    gabi::FrameLocal<sead::FixedSafeText<64>> name(8);
    prepareIndexedParameterName(name.get(),reinterpret_cast<const char*>(&sPipelineTexturePivotEditFormat),index);
    PipelineParameterResource_l* resource=mResource;
    void* lookup=indexedParameterRelative<void>(&resource->mLookupOffset);
    s32 selected=findPipelineParameter(lookup,reinterpret_cast<const char*>(name->mText.get()));
    return selected<0?nullptr:editableParameterPayload(this,selected,8);
}
/* 027FA7F8 */
void* PipelineParameterSet_l::editTextureMatrix(s32 index) {
    gabi::AutoFrame frame{0x027FA7F8u};
    gabi::FrameLocal<sead::FixedSafeText<64>> name(8);
    prepareIndexedParameterName(name.get(),reinterpret_cast<const char*>(&sPipelineTextureMatrixEditFormat),index);
    PipelineParameterResource_l* resource=mResource;
    void* lookup=indexedParameterRelative<void>(&resource->mLookupOffset);
    s32 selected=findPipelineParameter(lookup,reinterpret_cast<const char*>(name->mText.get()));
    return selected<0?nullptr:editableParameterPayload(this,selected,0);
}
namespace detail {
void* editableParameterPayload(PipelineParameterSet_l* parameters,s32 selected,u32 extraOffset) {
    return ::editableParameterPayload(parameters,selected,extraOffset);
}
}
}
/* WWHD glue
data sPipelineTextureMatrixFormat 1016EDBC
data sPipelineTexturePivotEditFormat 1016EDA4
data sPipelineTextureMatrixEditFormat 1016EDB0
data sPipelineTextureMatrixNameMethods 1016EC9C
ext 02759C28 void formatPipelineIndexedName(sead::SafeTextBuffer*,const char*,s32)
*/
