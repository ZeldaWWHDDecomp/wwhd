// Hidden-result color conversions; failed lookup preserves the original fallback bytes.
#include "JSystem/J3DGraph/PipelineColorParameters.h"
#include "sead/FileDevice.h"
using namespace j3d::pipeline;
void formatPipelineIndexedName(sead::SafeTextBuffer*,const char*,s32);
s32 findPipelineParameter(void*,const char*);
void* allocatePipelineStorage(u32);
f64 limitPipelineColorComponent(f64,f64,f64);
static u8 sPipelineTevColorFormat;
static u8 sPipelineKonstColorFormat;
static sead::SafeTextVtable sPipelineColorReadNameMethods;
static Scalar sPipelineColorScale;
static Scalar sPipelineColorMinimum;
namespace {
template<class T> T* colorReadRelative(const be<u32>* field) {
    u32 offset=*field;
    return offset==0?nullptr:gabi::at<T>(gabi::ea(field)+offset);
}
void prepareColorReadName(sead::FixedSafeText<64>* name,const char* format,s32 index) {
    name->mText=&name->mStorage[0];
    name->mCapacity=64;
    name->mStorage[0]=0;
    name->mStorage[63]=0;
    name->mVtable=&sPipelineColorReadNameMethods;
    gabi::cpu->cr[6]=0;
    formatPipelineIndexedName(name,format,index);
    name->assureTermination();
}
s32 lookupColorRead(PipelineParameterSet_l* parameters,sead::FixedSafeText<64>* name) {
    PipelineParameterResource_l* resource=parameters->mResource;
    void* lookup=colorReadRelative<void>(&resource->mLookupOffset);
    return findPipelineParameter(lookup,reinterpret_cast<const char*>(name->mText.get()));
}
const sead::ViewScalar* colorReadChannel(const sead::ColorChannels* source,u32 index) {
    switch(index) {
    case 0: return &source->r;
    case 1: return &source->g;
    case 2: return &source->b;
    case 3: return &source->a;
    default: return gabi::at<sead::ViewScalar>(gabi::ea(source)+index*sizeof(sead::ViewScalar));
    }
}
f64 scaledColorChannel(const sead::ColorChannels* source,u32 index,f64 scale) {
    const auto* channel=colorReadChannel(source,index);
    f64 value=ppc_lfs_value(channel->bits.get());
    return static_cast<f64>(gabi::fmuls_fpr(value,scale));
}
void setSignedColorChannel(SignedColor* color,u32 index,s16 value) {
    switch(index) {
    case 0: color->r=value;break;
    case 1: color->g=value;break;
    case 2: color->b=value;break;
    case 3: color->a=value;break;
    default: *gabi::at<be<s16>>(gabi::ea(color)+index*sizeof(be<s16>))=value;break;
    }
}
}
namespace j3d::pipeline {
/* 027FA160 */
void PipelineParameterSet_l::readTevColor(SignedColor* result,PipelineParameterSet_l* parameters,s32 index) {
    gabi::AutoFrame frame{0x027FA160u};
    gabi::FrameLocal<SignedColor> staged(8);
    gabi::FrameLocal<be<u32>> conversion(0x10);
    gabi::FrameLocal<sead::FixedSafeText<64>> name(0x18);
    prepareColorReadName(name.get(),reinterpret_cast<const char*>(&sPipelineTevColorFormat),index);
    s32 selected=lookupColorRead(parameters,name.get());
    if(selected>=0) {
        auto* source=static_cast<sead::ColorChannels*>(detail::editableParameterPayload(parameters,selected,0));
        f64 scale=ppc_lfs_value(sPipelineColorScale.bits.get());
        f64 minimum=ppc_lfs_value(sPipelineColorMinimum.bits.get());
        for(u32 channel=0;channel!=4;++channel) {
            f64 limited=limitPipelineColorComponent(scaledColorChannel(source,channel,scale),minimum,scale);
            u32 word=static_cast<u32>(gabi::ftoi(limited));
            conversion->set(word);
            setSignedColorChannel(staged.get(),channel,static_cast<s16>(static_cast<u16>(conversion->get())));
        }
    }
    if(result==nullptr) result=static_cast<SignedColor*>(allocatePipelineStorage(sizeof(SignedColor)));
    if(result!=nullptr) {
        result->r=staged->r;result->g=staged->g;result->b=staged->b;result->a=staged->a;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
/* 027FA3F4 */
void PipelineParameterSet_l::readKonstColor(ByteColor* result,PipelineParameterSet_l* parameters,s32 index) {
    gabi::AutoFrame frame{0x027FA3F4u};
    gabi::FrameLocal<be<u32>> conversion(8);
    gabi::FrameLocal<be<u32>> packedOutput(0x10);
    gabi::FrameLocal<sead::FixedSafeText<64>> name(0x14);
    u32 packed=gabi::ea(parameters);
    prepareColorReadName(name.get(),reinterpret_cast<const char*>(&sPipelineKonstColorFormat),index);
    s32 selected=lookupColorRead(parameters,name.get());
    if(selected>=0) {
        auto* source=static_cast<sead::ColorChannels*>(detail::editableParameterPayload(parameters,selected,0));
        f64 minimum=ppc_lfs_value(sPipelineColorMinimum.bits.get());
        f64 scale=ppc_lfs_value(sPipelineColorScale.bits.get());
        f64 scaled=scaledColorChannel(source,0,scale);
        for(u32 channel=0;channel!=4;++channel) {
            f64 limited=limitPipelineColorComponent(scaled,minimum,scale);
            u32 word=static_cast<u32>(gabi::ftoi(limited));
            // The next payload word is loaded before the current conversion scratch is stored.
            f64 next=0;
            if(channel+1<4) {
                const auto* nextChannel=colorReadChannel(source,channel+1);
                next=ppc_lfs_value(nextChannel->bits.get());
            }
            conversion->set(word);
            if(channel+1<4) scaled=static_cast<f64>(gabi::fmuls_fpr(next,scale));
            u32 shift=(3u-channel)*8u;
            packed=(packed&~(255u<<shift))|((conversion->get()&255u)<<shift);
        }
    }
    if(result==nullptr) result=static_cast<ByteColor*>(allocatePipelineStorage(sizeof(ByteColor)));
    if(result!=nullptr) {
        packedOutput->set(packed);
        u32 output=packedOutput->get();
        result->r=static_cast<u8>(output>>24);
        result->g=static_cast<u8>(output>>16);
        result->b=static_cast<u8>(output>>8);
        result->a=static_cast<u8>(output);
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
data sPipelineTevColorFormat 1016ED90
data sPipelineKonstColorFormat 1016ED9C
data sPipelineColorReadNameMethods 1016EC9C
data sPipelineColorScale 1016ED8C
data sPipelineColorMinimum 1016EBE8
ext 027382E8 f64 limitPipelineColorComponent(f64,f64,f64)
*/
