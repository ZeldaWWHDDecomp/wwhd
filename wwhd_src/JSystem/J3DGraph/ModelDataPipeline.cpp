// HD model resource preparation, reconstructed from this function's own instructions.
// Every layout and effect below is derived from this unit's own instructions.
// A historical tww declaration cross-check supplies no layout or acceptance evidence.
#include "JSystem/J3DGraph/ModelDataPipeline.h"
#include "JSystem/J3DGraph/ModelLighting.h"
#include "sead/HeapRegistry.h"
#include "bindings.h"
using namespace j3d::pipeline;
RawModelInfo_l* pipelineModelInfo(ModelDataPipeline_l*);
RawModelResource_l* pipelineModelResource(ModelDataPipeline_l*);
sead::RegistryHeap* pipelineCurrentHeap(sead::RegistryManagerState*);
void initializePipelineEntry(MaterialResourceEntry_l*,RawTransformEntry_l*,ModelMaterial_l*);
void* allocatePipelineStorage(u32);
void* allocatePipelineResourceObject(u32,sead::RegistryHeap*,s32);
void* allocatePipelineResourceArray(u32,sead::RegistryHeap*,s32);
void constructPipelineArray(void*,u32,u32,u32);
void* constructPipelineValueArray(void*,u32,u32,u32,u32);
void initializePipelineLights(PipelineLightProvider_l*);
void initializePipelineColorState(MaterialStateProvider_l*);
void initializePipelineIndexedState(MaterialIndexedProvider_l*);
void initializePipelineVectorState(MaterialVectorProvider_l*);
u32 queryPipelineRenderState(void*);
u32 readPipelineRenderBits(void*);
void writePipelineRenderBits(void*,u32);
void setPipelineRenderOption(void*,u32);
f64 readPipelineScalar(RawScalarResource_l*);
void initializePipelineNames(MaterialNameBindings_l*,u16,sead::RegistryHeap*);
void setPipelineName(MaterialNameBindings_l*,u16,const sead::SafeText*);
void finishPipelineRawMaterial(RawMaterialResource_l*);
static u8 sPipelineEntryMethods;
static PipelineLightMethods_l sPipelineResourceLightMethods;
static u8 sPipelineResourceColorMethods;
static MaterialIndexedMethods_l sPipelineResourceIndexedMethods;
static u8 sPipelineResourceVectorMethods;
static PipelineFogMethods_l sPipelineResourceFogMethods;
static MaterialFogSettings_l sPipelineDefaultFog;
static ByteColor sPipelineDefaultFogColor;
static MaterialTextureInfo_l sPipelineDefaultTextureInfo;
static u8 sPipelineNameBindingMethods;
static sead::SafeTextVtable sPipelineValueNameMethods;
struct PipelineArrayCookie_l { be<u32> mSize; };
struct RawLookupEntries_l { u8 mPrefix[0x18]; RawLookupNode_l mEntries[1]; };
static_assert(offsetof(RawLookupEntries_l,mEntries)==offsetof(RawLookupBlock_l,mFirstEntry));
static_assert(sizeof(RawLookupEntries_l)==sizeof(RawLookupBlock_l));
static PipelineArrayCookie_l sPipelineArrayCookie;
namespace {
template <class T> T* relativeResource(const be<u32>* offset) {
    u32 distance=*offset;
    return distance==0?nullptr:gabi::at<T>(gabi::ea(offset)+distance);
}
template <class T> T* resourceElement(T* base,u32 index) {
    return base+index;
}
template <class T> T* resourceOrFirst(T* base,u32 count,u32 index) {
    return index<count?resourceElement(base,index):base;
}
RawLookupNode_l* lookupNode(RawLookupBlock_l* block,u32 index) {
    auto* entries=reinterpret_cast<RawLookupEntries_l*>(block);
    return &entries->mEntries[index];
}
RawMaterialResource_l* materialResource(RawModelResource_l* resource,u32 index) {
    RawLookupBlock_l* block=relativeResource<RawLookupBlock_l>(&resource->mMaterialLookupOffset);
    return relativeResource<RawMaterialResource_l>(&lookupNode(block,index)->mDataOffset);
}
void* allocateFromPipelineHeap(sead::RegistryHeap* heap,u32 bytes) {
    if(heap==nullptr) heap=pipelineCurrentHeap(sRegistryManager);
    sead::RegistryHeapDispatch* methods=heap->dispatch();
    return gabi::call_ptr<void*>(methods->mAllocate.mFunction,heap,bytes,4);
}
void preserveEmptyPlacement(const void* address,u32 bytes) {
    if(gabi::ea(address)==0) allocatePipelineStorage(bytes);
}
void initializeMaterialStorage(ModelMaterial_l* material) {
    material->mNext=nullptr;
    material->mShapeBinding=nullptr;
    material->mEntry=nullptr;
    material->mLightProvider=nullptr;
    material->mIndexedProvider=nullptr;
    material->mStateProvider=nullptr;
    material->mVectorProvider=nullptr;
    material->mFogProvider=nullptr;
    material->mApplyObject=nullptr;
    material->mResource=nullptr;
    material->mValueStart=0;
    auto* state=&material->mRenderState;
    sead::CompactRenderState::create(state);
    material->mFlags=0;
    for(u32 index=0;index!=11;++index) {
        material->mColors[index].r.value=0.0f;
        material->mColors[index].g.value=0.0f;
        material->mColors[index].b.value=0.0f;
        material->mColors[index].a.value=1.0f;
    }
    for(u32 group=0;group!=3;++group)
        constructPipelineArray(&material->mStorage[group],2,sizeof(PipelineStorageRecord),gabi::guest_fn(reinterpret_cast<const void*>(&PipelineStorageRecord::create)));
    for(u32 index=0;index!=8;++index) preserveEmptyPlacement(&material->mMatrices[index],sizeof(MatrixRows));
    for(u32 index=0;index!=6;++index) preserveEmptyPlacement(&material->mExtraColors[index],sizeof(sead::ColorChannels));
    auto* values=&material->mScalarValues;
    if(values==nullptr) values=static_cast<MaterialScalarValues_l*>(allocatePipelineStorage(sizeof(MaterialScalarValues_l)));
    if(values!=nullptr) { values->mValues=nullptr;values->mCount=0; }
}
void initializeFogSettings(PipelineFog_l* fog) {
    fog->mKind=sPipelineDefaultFog.mKind;
    fog->mAdjustmentEnabled=sPipelineDefaultFog.mAdjustmentEnabled;
    fog->mCenter=sPipelineDefaultFog.mCenter;
    fog->mNear.value=sPipelineDefaultFog.mStart;
    fog->mFar.value=sPipelineDefaultFog.mEnd;
    fog->mNearPlane.value=sPipelineDefaultFog.mNear;
    fog->mFarPlane.value=sPipelineDefaultFog.mFar;
    fog->mColor.r=sPipelineDefaultFog.mColor.r;
    fog->mColor.g=sPipelineDefaultFog.mColor.g;
    fog->mColor.b=sPipelineDefaultFog.mColor.b;
    fog->mColor.a=sPipelineDefaultFog.mColor.a;
}
void initializeTextureMatrix(MaterialTextureMatrix_l* matrix) {
    preserveEmptyPlacement(&matrix->mMatrix,sizeof(MatrixRows));
    for(u32 row=0;row!=4;++row)
        for(u32 column=0;column!=4;++column)
            matrix->mInfo.mEffect.mRows[row][column]=sPipelineDefaultTextureInfo.mEffect.mRows[row][column];
    for(u32 component=0;component!=3;++component) matrix->mInfo.mCenter[component]=sPipelineDefaultTextureInfo.mCenter[component];
    matrix->mInfo.mProjection=sPipelineDefaultTextureInfo.mProjection;
    matrix->mInfo.mInfo=sPipelineDefaultTextureInfo.mInfo;
    for(u32 component=0;component!=2;++component) matrix->mInfo.mScale[component]=sPipelineDefaultTextureInfo.mScale[component];
    matrix->mInfo.mRotation=sPipelineDefaultTextureInfo.mRotation;
    for(u32 component=0;component!=2;++component) matrix->mInfo.mTranslation[component]=sPipelineDefaultTextureInfo.mTranslation[component];
}
void copyValueRecord(MaterialValue_l* destination,const MaterialValue_l* source) {
    static_assert(sizeof(MaterialValueWords_l)==sizeof(MaterialValue_l));
    auto* output=reinterpret_cast<MaterialValueWords_l*>(destination);
    const auto* input=reinterpret_cast<const MaterialValueWords_l*>(source);
    for(u32 word=0;word!=9;++word) { u32 bits=input->mWords[word];output->mWords[word]=bits; }
}
}
namespace j3d::pipeline {
/* 027F5A5C */
void ModelDataPipeline_l::initializeResource(RawModelResource_l* input,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F5A5Cu};
    gabi::FrameLocal<MaterialValue_l> stagedValue(0x24);
    gabi::FrameLocal<sead::SafeText> valueName(0x1C);
    mResource=input;
    RawModelInfo_l* initialInfo=pipelineModelInfo(this);
    u32 entryCount=initialInfo->mEntryCount;
    if(entryCount!=0) {
        auto* entries=static_cast<MaterialResourceEntry_l*>(allocateFromPipelineHeap(heap,entryCount*sizeof(MaterialResourceEntry_l)));
        for(u32 index=0;index<entryCount;++index) {
            MaterialResourceEntry_l* entry=resourceElement(entries,index);
            if(entry!=nullptr) {
                entry->mMethods=gabi::ea(&sPipelineEntryMethods);
                // The word at+4 has no further role established here.
                entry->mWord4=0;
                entry->mCallback.v=0;
                entry->mFirstMaterial=nullptr;
                entry->mState=0;
                entry->mResource=nullptr;
                entry->mFlags=1;
            }
        }
        if(entries!=nullptr) { mEntryCount=entryCount;mEntries=entries; }
    }
    RawModelResource_l* current=pipelineModelResource(this);
    u32 materialCount=current->mMaterialCount;
    if(materialCount!=0) {
        auto* materials=static_cast<ModelMaterial_l*>(allocateFromPipelineHeap(heap,materialCount*sizeof(ModelMaterial_l)));
        for(u32 index=0;index<materialCount;++index) {
            ModelMaterial_l* material=resourceElement(materials,index);
            if(material!=nullptr) initializeMaterialStorage(material);
        }
        if(materials!=nullptr) { mMaterialCount=materialCount;mMaterials=materials; }
    }
    RawModelInfo_l* info=pipelineModelInfo(this);
    current=mResource;
    s32 shapes=current->mShapeCount;
    for(u32 shapeIndex=0;shapes!=0;++shapeIndex,--shapes) {
        RawLookupBlock_l* shapesBlock=relativeResource<RawLookupBlock_l>(&current->mShapeLookupOffset);
        RawShapeResource_l* shape=relativeResource<RawShapeResource_l>(&lookupNode(shapesBlock,shapeIndex)->mDataOffset);
        u16 entryIndex=shape->mEntryIndex;
        if(entryIndex<entryCount) {
            MaterialResourceEntry_l* entry=resourceOrFirst(mEntries.get(),mEntryCount,entryIndex);
            RawTransformEntry_l* rawEntries=relativeResource<RawTransformEntry_l>(&info->mEntriesOffset);
            RawTransformEntry_l* rawEntry=resourceElement(rawEntries,entryIndex);
            u16 materialIndex=shape->mMaterialIndex;
            ModelMaterial_l* material=resourceOrFirst(mMaterials.get(),mMaterialCount,materialIndex);
            initializePipelineEntry(entry,rawEntry,material);
            material=resourceOrFirst(mMaterials.get(),mMaterialCount,materialIndex);
            current=mResource;
            RawMaterialResource_l* rawMaterial=materialResource(current,materialIndex);
            auto* shapeBinding=static_cast<MaterialShapeBinding_l*>(allocatePipelineResourceObject(sizeof(MaterialShapeBinding_l),heap,4));
            if(shapeBinding!=nullptr) { shapeBinding->mEnabled=1;shapeBinding->mResource=shape; }
            entry=resourceOrFirst(mEntries.get(),mEntryCount,entryIndex);
            material->mResource=rawMaterial;
            material->mShapeBinding=shapeBinding;
            material->mEntry=entry;
            auto* lights=static_cast<PipelineLightProvider_l*>(allocatePipelineResourceObject(sizeof(PipelineLightProvider_l),nullptr,4));
            if(lights!=nullptr) { lights->mMethods=&sPipelineResourceLightMethods;initializePipelineLights(lights); }
            material->mLightProvider=lights;
            auto* colors=static_cast<MaterialStateProvider_l*>(allocatePipelineResourceObject(sizeof(MaterialStateProvider_l),nullptr,4));
            if(colors!=nullptr) { colors->mMethods=gabi::ea(&sPipelineResourceColorMethods);initializePipelineColorState(colors); }
            material->mStateProvider=colors;
            auto* indexed=static_cast<MaterialIndexedProvider_l*>(allocatePipelineResourceObject(sizeof(MaterialIndexedProvider_l),nullptr,4));
            if(indexed!=nullptr) { indexed->mMethods=&sPipelineResourceIndexedMethods;initializePipelineIndexedState(indexed); }
            material->mIndexedProvider=indexed;
            auto* vectors=static_cast<MaterialVectorProvider_l*>(allocatePipelineResourceObject(sizeof(MaterialVectorProvider_l),nullptr,4));
            if(vectors!=nullptr) {
                vectors->mMethods=gabi::ea(&sPipelineResourceVectorMethods);
                constructPipelineArray(&vectors->mValues,3,sizeof(PipelineVectorPair),gabi::guest_fn(reinterpret_cast<const void*>(&PipelineVectorPair::create)));
                initializePipelineVectorState(vectors);
            }
            material->mVectorProvider=vectors;
            auto* fogProvider=static_cast<PipelineFogProvider_l*>(allocatePipelineResourceObject(sizeof(PipelineFogProvider_l),nullptr,4));
            if(fogProvider!=nullptr) {
                fogProvider->mFog=nullptr;
                fogProvider->mMethods=&sPipelineResourceFogMethods;
                fogProvider->mFlags[0]=sPipelineDefaultFogColor.r;
                fogProvider->mFlags[1]=sPipelineDefaultFogColor.g;
                fogProvider->mFlags[2]=sPipelineDefaultFogColor.b;
                fogProvider->mFlags[3]=sPipelineDefaultFogColor.a;
                fogProvider->mOtherFlag=1;
            }
            material->mFogProvider=fogProvider;
            PipelineFogMethods_l* fogMethods=fogProvider->mMethods;
            u32 setFog=fogMethods->mSet.get();
            auto* fog=static_cast<PipelineFog_l*>(allocatePipelineResourceObject(sizeof(PipelineFog_l),nullptr,4));
            if(fog!=nullptr) initializeFogSettings(fog);
            gabi::call_ptr<void>(setFog,material->mFogProvider.get(),fog);
            be<u32>* renderOffset=&rawMaterial->mRenderStateOffset;
            RawRenderState_l* renderState=relativeResource<RawRenderState_l>(renderOffset);
            for(u32 matrixIndex=0;matrixIndex!=8;++matrixIndex) {
                MaterialIndexedProvider_l* provider=material->mIndexedProvider;
                MaterialIndexedMethods_l* methods=provider->mMethods;
                u32 setter=methods->mSet.get();
                auto* matrix=static_cast<MaterialTextureMatrix_l*>(allocatePipelineResourceObject(sizeof(MaterialTextureMatrix_l),nullptr,4));
                if(matrix!=nullptr) initializeTextureMatrix(matrix);
                gabi::call_ptr<void>(setter,material->mIndexedProvider.get(),matrixIndex,matrix);
            }
            u32 active=1;
            if(renderState!=nullptr) {
                RawRenderState_l* changed=relativeResource<RawRenderState_l>(renderOffset);
                void* settings=&changed->mState[4];
                if(queryPipelineRenderState(settings)==0) active=0;
            }
            fogProvider=material->mFogProvider;
            fogMethods=fogProvider->mMethods;
            be<u8>* other=gabi::call_ptr<be<u8>*>(fogMethods->mOther.get(),fogProvider);
            *other=static_cast<u8>(active);
            u32 mode=0;
            if(renderState!=nullptr) {
                RawRenderState_l* changed=relativeResource<RawRenderState_l>(renderOffset);
                if(((static_cast<u32>(changed->mFlags)>>4)&3u)==1) mode=1;
            }
            fogProvider=material->mFogProvider;
            fogMethods=fogProvider->mMethods;
            be<u8>* flags=gabi::call_ptr<be<u8>*>(fogMethods->mFlags.get(),fogProvider);
            *flags=static_cast<u8>(mode);
            void* renderBits=&renderState->mState[0x10];
            u32 bits=readPipelineRenderBits(renderBits);
            if((bits&1u)!=0) bits|=2u;
            writePipelineRenderBits(renderBits,bits);
            setPipelineRenderOption(&renderState->mState[0x14],0);
            u32 scalarCount=rawMaterial->mScalarCount;
            if(scalarCount!=0) {
                auto* values=static_cast<be<s32>*>(allocateFromPipelineHeap(nullptr,scalarCount*sizeof(be<s32>)));
                if(values!=nullptr) { material->mScalarValues.mCount=scalarCount;material->mScalarValues.mValues=values; }
                s32 count=material->mScalarValues.mCount;
                s32 index=0;
                while(index<count) {
                    be<s32>* output=resourceOrFirst(material->mScalarValues.mValues.get(),count,index);
                    RawScalarResource_l* rawValues=relativeResource<RawScalarResource_l>(&rawMaterial->mScalarsOffset);
                    f64 value=readPipelineScalar(resourceElement(rawValues,index));
                    *output=gabi::ftoi(value);
                    count=material->mScalarValues.mCount;
                    ++index;
                }
            }
        }
        if(shapes!=1) current=mResource;
    }
    for(u32 index=0;index<entryCount;++index) {
        MaterialResourceEntry_l* entry=resourceOrFirst(mEntries.get(),mEntryCount,index);
        if(entry->mFirstMaterial.get()==nullptr) {
            RawTransformEntry_l* rawEntries=relativeResource<RawTransformEntry_l>(&info->mEntriesOffset);
            initializePipelineEntry(entry,resourceElement(rawEntries,index),nullptr);
        }
    }
    if(materialCount!=0) {
        u32 totalValues=0;
        for(u32 index=0;index<materialCount;++index) totalValues+=materialResource(input,index)->mValueCount;
        if(totalValues!=0) {
            auto* table=static_cast<MaterialValueTable_l*>(allocatePipelineResourceObject(sizeof(MaterialValueTable_l),heap,4));
            if(table!=nullptr) {
                table->mCount=0;
                table->mValues=nullptr;
                for(u32 word=0;word!=5;++word) table->mReserved[word]=0;
                table->mEnabled=1;
                table->mCapacity=0;
            }
            mValueTable=table;
            u16 count=static_cast<u16>(totalValues);
            table->mCount=count;
            u32 cookie=sPipelineArrayCookie.mSize;
            void* storage=allocatePipelineResourceArray(count*sizeof(MaterialValue_l)+cookie,heap,4);
            MaterialValue_l* values=nullptr;
            if(storage!=nullptr) {
                cookie=sPipelineArrayCookie.mSize;
                void* first=static_cast<u8*>(storage)+cookie;
                values=static_cast<MaterialValue_l*>(constructPipelineValueArray(first,count,sizeof(MaterialValue_l),gabi::guest_fn(reinterpret_cast<const void*>(&PipelineFlagState::create)),0));
            }
            table->mValues=values;
            for(u32 index=0;index<table->mCount;++index) {
                MaterialValue_l* records=table->mValues;
                resourceElement(records,index)->mFirstRelative=0;
                records=table->mValues;
                resourceElement(records,index)->mLastRelative=0;
            }
            auto* names=static_cast<MaterialNameBindings_l*>(allocatePipelineResourceObject(sizeof(MaterialNameBindings_l),heap,4));
            if(names!=nullptr) {
                names->mMethods=gabi::ea(&sPipelineNameBindingMethods);
                names->mWords[1]=0;
                names->mWords[0]=0;
                names->mCount=0;
            }
            mNameBindings=names;
            initializePipelineNames(names,static_cast<u16>(totalValues),heap);
            u32 valueIndex=0;
            for(u32 materialIndex=0;materialIndex<materialCount;++materialIndex) {
                RawMaterialResource_l* rawMaterial=materialResource(input,materialIndex);
                u32 valueCount=rawMaterial->mValueCount;
                ModelMaterial_l* material=resourceOrFirst(mMaterials.get(),mMaterialCount,materialIndex);
                material->mValueStart=valueIndex;
                for(u32 index=0;index<valueCount;++index,++valueIndex) {
                    RawValueReference_l* refs=relativeResource<RawValueReference_l>(&rawMaterial->mValuesOffset);
                    RawValueReference_l* ref=resourceElement(refs,index);
                    RawValueAttribute_l* attribute=relativeResource<RawValueAttribute_l>(&ref->mAttributeOffset);
                    stagedValue->mWord=gabi::ea(attribute);
                    stagedValue->mKind=static_cast<u16>(attribute->mKind);
                    stagedValue->mCount=static_cast<u16>(attribute->mCount);
                    stagedValue->mFlag=0;
                    MaterialValueTable_l* currentTable=mValueTable;
                    MaterialValue_l* destination=resourceElement(currentTable->mValues.get(),static_cast<u16>(valueIndex));
                    copyValueRecord(destination,stagedValue.get());
                    destination=resourceElement(currentTable->mValues.get(),static_cast<u16>(valueIndex));
                    destination->mLastRelative=static_cast<u32>(destination->mLastRelative)+gabi::ea(stagedValue.get())-gabi::ea(destination);
                    destination=resourceElement(currentTable->mValues.get(),static_cast<u16>(valueIndex));
                    destination->mFirstRelative=static_cast<u32>(destination->mFirstRelative)+gabi::ea(stagedValue.get())-gabi::ea(destination);
                    destination=resourceElement(currentTable->mValues.get(),static_cast<u16>(valueIndex));
                    destination->mWord=stagedValue->mWord;
                    refs=relativeResource<RawValueReference_l>(&rawMaterial->mValuesOffset);
                    const char* name=relativeResource<char>(&resourceElement(refs,index)->mNameOffset);
                    valueName->mVtable=&sPipelineValueNameMethods;
                    valueName->mText=reinterpret_cast<be<u8>*>(const_cast<char*>(name));
                    setPipelineName(mNameBindings.get(),static_cast<u16>(valueIndex),valueName.get());
                }
            }
        }
    }
    u32 materialIndex=0;
    while(materialIndex<input->mMaterialCount) {
        finishPipelineRawMaterial(materialResource(input,materialIndex));
        ++materialIndex;
    }
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
}
/* WWHD glue
data sPipelineEntryMethods 1016E65C
data sPipelineResourceLightMethods 1016E6AC
data sPipelineResourceColorMethods 1016E72C
data sPipelineResourceIndexedMethods 1016E614
data sPipelineResourceVectorMethods 1016E63C
data sPipelineResourceFogMethods 1016E79C
data sPipelineDefaultFog 1016E37C
data sPipelineDefaultFogColor 1016E378
data sPipelineDefaultTextureInfo 1016E3B0
data sPipelineNameBindingMethods 1016E604
data sPipelineValueNameMethods 1016E5A4
data sPipelineArrayCookie 101FCD08
ext 027F3F94 j3d::pipeline::RawModelInfo_l* pipelineModelInfo(j3d::pipeline::ModelDataPipeline_l*)
ext 027F3F8C j3d::pipeline::RawModelResource_l* pipelineModelResource(j3d::pipeline::ModelDataPipeline_l*)
ext 02756140 sead::RegistryHeap* pipelineCurrentHeap(sead::RegistryManagerState*)
ext 027F3C38 void initializePipelineEntry(j3d::pipeline::MaterialResourceEntry_l*,j3d::pipeline::RawTransformEntry_l*,j3d::pipeline::ModelMaterial_l*)
ext 0273B050 void* allocatePipelineResourceObject(u32,sead::RegistryHeap*,s32)
ext 0273B0D4 void* allocatePipelineResourceArray(u32,sead::RegistryHeap*,s32)
ext 028EFF8C void* constructPipelineValueArray(void*,u32,u32,u32,u32)
ext 02843B1C void initializePipelineLights(j3d::pipeline::PipelineLightProvider_l*)
ext 02843BC4 void initializePipelineColorState(j3d::pipeline::MaterialStateProvider_l*)
ext 02843C5C void initializePipelineIndexedState(j3d::pipeline::MaterialIndexedProvider_l*)
ext 02843CA8 void initializePipelineVectorState(j3d::pipeline::MaterialVectorProvider_l*)
ext 027E1918 u32 queryPipelineRenderState(void*)
ext 027E1974 u32 readPipelineRenderBits(void*)
ext 027E194C void writePipelineRenderBits(void*,u32)
ext 027E19D4 void setPipelineRenderOption(void*,u32)
ext 027E1474 f64 readPipelineScalar(j3d::pipeline::RawScalarResource_l*)
ext 027ED0B4 void initializePipelineNames(j3d::pipeline::MaterialNameBindings_l*,u16,sead::RegistryHeap*)
ext 027ED268 void setPipelineName(j3d::pipeline::MaterialNameBindings_l*,u16,const sead::SafeText*)
ext 027FB168 void finishPipelineRawMaterial(j3d::pipeline::RawMaterialResource_l*)
*/
