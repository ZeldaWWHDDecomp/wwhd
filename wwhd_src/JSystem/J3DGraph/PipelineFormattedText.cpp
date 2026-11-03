// This guest entry captures the documented GHS variadic bank; it does not infer trailing types.
#include "JSystem/J3DGraph/PipelineFormattedText.h"
#include "sead/Reports.h"
#include <bit>
using namespace j3d::pipeline;
void* allocatePipelineStorage(u32);
void formatPipelineTextArguments(sead::SafeTextBuffer*,const char*,ReportArguments*);
static sead::SafeTextVtable sPipelineBaseTextMethods;
static sead::SafeTextVtable sPipelineBaseBufferMethods;
static sead::SafeTextVtable sPipelineFormattedTextMethods;
namespace j3d::pipeline {
/* 027F76FC */
PipelineFormattedText_l* PipelineFormattedText_l::create(PipelineFormattedText_l* object,const char* format) {
    u32 entryStack=gabi::cpu->r[1];
    bool hasFloatingArguments=gabi::cpu->cr[6]!=0;
    gabi::AutoFrame frame{0x027F76FCu};
    gabi::FrameLocal<ReportSave> saved(8);
    for(u32 index=0;index!=8;++index) saved->mIntegers[index]=gabi::cpu->r[index+3];
    if(hasFloatingArguments) {
        for(u32 index=0;index!=8;++index) {
            u64 bits=std::bit_cast<u64>(gabi::cpu->f[index+1].ps0);
            saved->mFloatWords[index*2]=static_cast<u32>(bits>>32);
            saved->mFloatWords[index*2+1]=static_cast<u32>(bits);
        }
    }
    if(object==nullptr) object=static_cast<PipelineFormattedText_l*>(allocatePipelineStorage(sizeof(PipelineFormattedText_l)));
    if(object!=nullptr) {
        object->mText=&object->mStorage[0];
        object->mVtable=&sPipelineBaseTextMethods;
        object->mCapacity=128;
        object->mStorage[127]=0;
        object->mVtable=&sPipelineBaseBufferMethods;
        *object->mText=0;
        object->mVtable=&sPipelineFormattedTextMethods;
        saved->mArguments.mIntegerIndex=2;
        saved->mArguments.mFloatIndex=0;
        saved->mArguments.mOverflow=entryStack+8;
        saved->mArguments.mSavedRegisters=gabi::ea(&saved->mIntegers[0]);
        formatPipelineTextArguments(object,format,&saved->mArguments);
    }
    return object;
}
}
/* WWHD glue
data sPipelineBaseTextMethods 1016E5BC
data sPipelineBaseBufferMethods 1016E5D4
data sPipelineFormattedTextMethods 1016E67C
ext 02759C10 void formatPipelineTextArguments(sead::SafeTextBuffer*,const char*,ReportArguments*)
*/
