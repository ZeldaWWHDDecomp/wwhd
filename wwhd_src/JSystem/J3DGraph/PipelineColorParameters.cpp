// Selected color parameters and linked dirty bits, from own9FB8 instructions.
#include "JSystem/J3DGraph/PipelineColorParameters.h"
#include "sead/FileDevice.h"
using namespace j3d::pipeline;
void assurePipelineParameterName(sead::SafeText*);
s32 findPipelineParameter(void*,const char*);
struct PipelineParameterNames_l { gptr<be<u8>> mNames[22]; };
static PipelineParameterNames_l sPipelineParameterNames;
static sead::SafeTextVtable sPipelineParameterNameMethods;
namespace {
template<class T> T* relativeParameter(const be<u32>* field) {
    u32 offset=*field;
    return offset==0?nullptr:gabi::at<T>(gabi::ea(field)+offset);
}
void markPipelineParameter(PipelineParameterSet_l* parameters,u32 index) {
    parameters->mFlags=static_cast<u16>(parameters->mFlags)|4u;
    be<u32>* words=parameters->mDirtyWords;
    auto* word=&words[index>>5];
    *word=static_cast<u32>(*word)|(1u<<(index&31u));
}
}
namespace j3d::pipeline {
/* 027F9FB8 */
void PipelineColorTable_l::applyParameters(PipelineParameterSet_l* parameters) {
    gabi::AutoFrame frame{0x027F9FB8u};
    gabi::FrameLocal<sead::SafeText> name(8);
    u32 bit=1;
    for(u32 index=0;index!=22;++index,bit<<=1) {
        if((static_cast<u32>(mFlags)&bit)==0) continue;
        name->mText=sPipelineParameterNames.mNames[index];
        name->mVtable=&sPipelineParameterNameMethods;
        assurePipelineParameterName(name.get());
        PipelineParameterResource_l* resource=parameters->mResource;
        void* lookup=relativeParameter<void>(&resource->mLookupOffset);
        s32 selected=findPipelineParameter(lookup,reinterpret_cast<const char*>(name->mText.get()));
        if(selected<0) continue;
        resource=parameters->mResource;
        auto* records=relativeParameter<PipelineParameterRecord_l>(&resource->mRecordsOffset);
        auto* record=&records[static_cast<u32>(selected)];
        if(record->mSignedWord4>=0) {
            markPipelineParameter(parameters,static_cast<u32>(selected));
            resource=parameters->mResource;
        }
        records=relativeParameter<PipelineParameterRecord_l>(&resource->mRecordsOffset);
        u16 linkedIndex=record->mLinkedIndex;
        auto* linked=&records[linkedIndex];
        if(linked->mSignedWord4>=0) markPipelineParameter(parameters,linkedIndex);
        be<u8>* values=parameters->mValues;
        u16 offset=record->mDataOffset;
        auto* destination=reinterpret_cast<sead::ColorChannels*>(values+offset);
        if(index<11) {
            const sead::ColorChannels* source=color(this,index);
            destination->r.bits=source->r.bits;
            destination->g.bits=source->g.bits;
            destination->b.bits=source->b.bits;
            destination->a.bits=source->a.bits;
        }
    }
}
}
/* WWHD glue
data sPipelineParameterNames 101F9984
data sPipelineParameterNameMethods 1016EC54
ext 027FB3B4 void assurePipelineParameterName(sead::SafeText*)
ext 027DF9B0 s32 findPipelineParameter(void*,const char*)
*/
