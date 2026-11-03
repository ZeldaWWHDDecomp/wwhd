// Resource wrapper factory and four per-shape variants, derived from own7B50.
#include "JSystem/J3DGraph/ModelDataPipeline.h"
#include "sead/HeapRegistry.h"
using namespace j3d::pipeline;
void* allocatePipelineResourceObject(u32,sead::RegistryHeap*,s32);
void* allocatePipelineStorage(u32);
sead::RegistryHeap* pipelineCurrentHeap(sead::RegistryManagerState*);
void initializePipelineBindingBlock(j3d::FrameRecords*,void*,sead::RegistryHeap*);
void constructPipelineResourceVariant(PipelineResourceVariant_l*);
void initializePipelineResourceVariant(PipelineResourceVariant_l*,sead::RegistryHeap*);
void finalizePipelineResourceBindings(ModelDataPipeline_l*,j3d::FrameRecords*,sead::RegistryHeap*);
namespace {
struct ResourcePairWords_l { be<u32> mWords[2]; };
static_assert(sizeof(ResourcePairWords_l)==8);
template<class Count,class T> void initializeResourcePair(Count& count,gptr<T>& pointer) {
    static_assert(sizeof(Count)==4);
    (void)pointer;
    auto* pair=gabi::at<ResourcePairWords_l>(gabi::ea(&count));
    if(pair==nullptr) pair=static_cast<ResourcePairWords_l*>(allocatePipelineStorage(sizeof(ResourcePairWords_l)));
    if(pair!=nullptr) { pair->mWords[1]=0;pair->mWords[0]=0; }
}
}
static_assert(offsetof(ModelDataPipeline_l,mEntries)==offsetof(ModelDataPipeline_l,mEntryCount)+4);
static_assert(offsetof(ModelDataPipeline_l,mMaterials)==offsetof(ModelDataPipeline_l,mMaterialCount)+4);
static_assert(offsetof(j3d::FrameRecords,mRecords)==offsetof(j3d::FrameRecords,mCount)+4);
static_assert(offsetof(ModelDataPipeline_l,mVariants)==offsetof(ModelDataPipeline_l,mVariantCount)+4);
namespace j3d::pipeline {
/* 027F7B50 */
ModelDataPipeline_l* ModelDataPipeline_l::create(RawModelResource_l* resource,void* bindingSource,u32,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F7B50u};
    auto* data=static_cast<ModelDataPipeline_l*>(allocatePipelineResourceObject(sizeof(ModelDataPipeline_l),heap,4));
    if(data!=nullptr) {
        data->mResource=nullptr;
        initializeResourcePair(data->mEntryCount,data->mEntries);
        initializeResourcePair(data->mMaterialCount,data->mMaterials);
        data->mBindings.mPublishedCount=0;
        data->mBindings.mSource=nullptr;
        initializeResourcePair(data->mBindings.mCount,data->mBindings.mRecords);
        initializeResourcePair(data->mVariantCount,data->mVariants);
        data->mValueTable=nullptr;
        data->mFlag=0;
        data->mWord38=0;
        data->mNameBindings=nullptr;
        data->mWord40=0;
        data->mWord3C=0;
        data->mWord48=0;
        data->mWord44=0;
    }
    data->initializeResource(resource,heap);
    initializePipelineBindingBlock(&data->mBindings,bindingSource,heap);
    RawModelResource_l* current=data->mResource;
    u32 variantCount=static_cast<u32>(current->mShapeCount)*4;
    if(variantCount!=0) {
        sead::RegistryHeap* allocationHeap=heap;
        if(allocationHeap==nullptr) allocationHeap=pipelineCurrentHeap(sRegistryManager);
        auto* variants=gabi::call_ptr<PipelineResourceVariant_l*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,variantCount*sizeof(PipelineResourceVariant_l),4);
        for(u32 index=0;index<variantCount;++index) {
            auto* variant=gabi::at<PipelineResourceVariant_l>(gabi::ea(variants)+index*sizeof(PipelineResourceVariant_l));
            if(variant!=nullptr) {
                constructPipelineResourceVariant(variant);
                variant->mState=0;
            }
        }
        if(variants!=nullptr) { data->mVariants=variants;data->mVariantCount=variantCount; }
    }
    s32 index=0;
    u32 offset=0;
    while(index<static_cast<s32>(data->mVariantCount)) {
        PipelineResourceVariant_l* variant=data->mVariants;
        if(static_cast<u32>(index)<data->mVariantCount) variant=variant+index;
        initializePipelineResourceVariant(variant,heap);
        ++index;
        offset+=sizeof(PipelineResourceVariant_l);
    }
    finalizePipelineResourceBindings(data,&data->mBindings,heap);
    return data;
}
}
/* WWHD glue
ext 028010F8 void initializePipelineBindingBlock(j3d::FrameRecords*,void*,sead::RegistryHeap*)
ext 02774320 void constructPipelineResourceVariant(j3d::pipeline::PipelineResourceVariant_l*)
ext 0277445C void initializePipelineResourceVariant(j3d::pipeline::PipelineResourceVariant_l*,sead::RegistryHeap*)
ext 027F70D8 void finalizePipelineResourceBindings(j3d::pipeline::ModelDataPipeline_l*,j3d::FrameRecords*,sead::RegistryHeap*)
*/
