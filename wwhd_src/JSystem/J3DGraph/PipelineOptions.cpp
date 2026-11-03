// Named option and scalar binding factories from this unit's own instructions.
#include "JSystem/J3DGraph/PipelineOptions.h"
using namespace j3d::pipeline;
void* allocatePipelineParameter(u32,sead::RegistryHeap*,s32);
void* allocatePipelineStorage(u32);
void releasePipelineStorage(void*);
void clearPipelineParameter(void*,u32);
void* copyPipelineOptionName(void*,const void*,u32,u32);
struct PipelineOptionDefault_l { be<f32> mScalar; };
static PipelineOptionDefault_l sPipelineOptionDefault;
static u8 sPipelineOptionBaseMethods;
static u8 sPipelineNamedOptionMethods;
static u8 sPipelineScalarBindingMethods;
static sead::SafeTextVtable sPipelineOptionBaseTextMethods;
static sead::SafeTextVtable sPipelineOptionBaseBufferMethods;
static sead::SafeTextVtable sPipelineOptionTextMethods;
namespace j3d::pipeline {
/* 027F78A8 */
PipelineNamedOption_l* PipelineNamedOption_l::create(OptionDescriptor_l* descriptor,sead::SafeText* name,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F78A8u};
    auto* option=static_cast<PipelineNamedOption_l*>(allocatePipelineParameter(sizeof(PipelineNamedOption_l),heap,4));
    if(option!=nullptr) {
        clearPipelineParameter(option,sizeof(PipelineNamedOption_l));
        option->mDescriptor=nullptr;
        option->mMethods=gabi::ea(&sPipelineOptionBaseMethods);
        option->mScalar=sPipelineOptionDefault.mScalar;
        auto* text=&option->mName;
        if(text==nullptr) text=static_cast<PipelineOptionText_l*>(allocatePipelineStorage(sizeof(PipelineOptionText_l)));
        if(text!=nullptr) {
            text->mText=&text->mStorage[0];
            text->mVtable=&sPipelineOptionBaseTextMethods;
            text->mCapacity=32;
            text->mStorage[31]=0;
            text->mVtable=&sPipelineOptionBaseBufferMethods;
            *text->mText=0;
            text->mVtable=&sPipelineOptionTextMethods;
        }
        option->mMethods=gabi::ea(&sPipelineNamedOptionMethods);
    }
    // Allocation failure still reaches the original's unconditional descriptor store.
    option->mDescriptor=descriptor;
    be<u8>* destination=option->mName.mText;
    name->assureTermination();
    be<u8>* scan=name->mText;
    u8 byte=*scan;
    s32 length=0;
    while(byte!=0) {
        ++length;
        ++scan;
        if(length>sead::kMaxTextLength) { length=0;break; }
        byte=*scan;
    }
    s32 capacity=option->mName.mCapacity;
    if(length>=capacity) length=static_cast<s32>(static_cast<u32>(capacity)-1u);
    name->assureTermination();
    copyPipelineOptionName(destination,name->mText.get(),static_cast<u32>(length),0);
    destination[static_cast<u32>(length)]=0;
    return option;
}
/* 027F7A2C */
PipelineScalarBinding_l* PipelineScalarBinding_l::create(void* source,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F7A2Cu};
    auto* binding=static_cast<PipelineScalarBinding_l*>(allocatePipelineParameter(sizeof(PipelineScalarBinding_l),heap,4));
    if(binding!=nullptr) {
        binding->mMethods=gabi::ea(&sPipelineScalarBindingMethods);
        binding->mScalar=sPipelineOptionDefault.mScalar;
        binding->mWord10=0;
        binding->mWord8=0;
        binding->mSource=nullptr;
    }
    binding->mSource=source;
    return binding;
}
/* 027F7ABC */
void PipelineOptionNameText_l::destroy(PipelineOptionNameText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7ABCu};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F7AD0 */
void PipelineOptionBaseBuffer_l::destroy(PipelineOptionBaseBuffer_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7AD0u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F7B24 */
void PipelineOptionNameText_l::assureTermination(PipelineOptionNameText_l*) {}
/* 027F7B28 */
void PipelineOptionBaseText_l::destroy(PipelineOptionBaseText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7B28u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F7B3C */
void PipelineOptionText_l::destroy(PipelineOptionText_l* object,u32 flags) {
    gabi::AutoFrame frame{0x027F7B3Cu};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
}
/* WWHD glue
data sPipelineOptionDefault 1016E8E0
data sPipelineOptionBaseMethods 1016E944
data sPipelineNamedOptionMethods 1016E95C
data sPipelineScalarBindingMethods 1016E4EC
data sPipelineOptionBaseTextMethods 1016E8FC
data sPipelineOptionBaseBufferMethods 1016E914
data sPipelineOptionTextMethods 1016E92C
ext 0273B050 void* allocatePipelineParameter(u32,sead::RegistryHeap*,s32)
ext 028F521C void clearPipelineParameter(void*,u32)
ext C0009988 void* copyPipelineOptionName(void*,const void*,u32,u32)
*/
