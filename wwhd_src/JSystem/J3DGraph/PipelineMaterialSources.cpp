// Material source binding and bounded name comparison, derived from own 027F70D8.
#include "JSystem/J3DGraph/ModelDataPipeline.h"
#include "JSystem/J3DGraph/PipelineFormattedText.h"
#include "agl/ShaderArchive.h"
using namespace j3d::pipeline;
static sead::SafeTextVtable sMaterialSourceNameMethods;
static u8 sMaterialSourceFailureFormat;
struct MaterialSourceWords_l { be<u32> mWords[9]; };
static MaterialSourceWords_l sMaterialSourceInitialWords;
static u8 sMaterialSourceMaterialLabel;
static u8 sMaterialSourceShaderPathKey;
static u8 sMaterialSourceSkinCountKey;
sead::SafeText* findMaterialSourceText(void*,sead::SafeText*);
s32 selectMaterialSourceVariant(void*,u32,void*,void*,u32);
void constructMaterialSourceVariant(PipelineResourceVariant_l*,void*,void*,void*,void*);
s32 findMaterialSource(void*,sead::SafeText*);
void initializeMaterialSource(void*,void*,u32);
void applyPipelineMaterialSource(ModelDataPipeline_l*,void*,void*,sead::RegistryHeap*);
PipelineFormattedText_l* formatMaterialSourceFailure(PipelineFormattedText_l*,const char*,const char*);
namespace j3d::pipeline {
namespace {
struct SourceVariant_l { u8 mPrefix[2]; be<u16> mIdentifier; u8 mTail[0x80]; };
struct SourceVariantTable_l { gptr<SourceVariant_l> mDefault; be<u32> mCount; gptr<SourceVariant_l> mVariants; };
struct SourceVariantMetadata_l { u8 mPrefix[8]; SourceVariantTable_l mTable; };
struct SourceRecordResource_l { u8 mPrefix[2]; be<u16> mIdentifier; u8 mState[0x78]; gptr<SourceVariantMetadata_l> mMetadata; };
struct SourceRecord_l;
struct SourceRecordTable_l { be<u32> mCount; gptr<SourceRecord_l> mRecords; };
struct SourceRecord_l { gptr<SourceRecordResource_l> mResource; u8 mState[0x18]; gptr<SourceRecordTable_l> mNested; be<u8> mReady; u8 mTail[3]; };
struct SourceNames_l { be<u32> mOwnerName,mMaterialName; u8 mState[6]; be<u16> mParameterCount; u8 mBeforeParameters[8]; be<u32> mParameters; };
struct SourceParameterEntry_l { be<u32> mNameOffset,mValueOffset; u8 mTail[8]; };
struct SourceParameterTable_l { u8 mPrefix[0x20]; SourceParameterEntry_l mEntries[1]; };
struct SourceShape_l { u8 mPrefix[0xE]; be<u16> mMaterialIndex; u8 mState[6]; be<u8> mVtxSkinCount; };
// mFirstEntry begins at block+0x18; each node's mDataOffset is at +0xC,
// matching the target's observed dictionary+0x24 field address.
struct SourceLookupEntries_l { u8 mPrefix[0x18]; RawLookupNode_l mEntries[1]; };
struct SourcePointerArray_l { gptr<void> mPointers[256]; };
struct SourceParameterPair_l { be<u32> mFirst,mSecond; };
struct SourceKeyPair_l { gptr<be<u8>> mFirst,mSecond; };
static_assert(sizeof(SourceKeyPair_l)==8);
struct SourceMaterial_l { u8 mPrefix[4]; be<u32> mName; u8 mMiddle[0x1C]; be<u32> mSourceNames; }; /* FMAT: +4 self-relative name (027F6C40/027F6CCC), +0x24 source names */
static_assert(sizeof(SourceRecord_l)==0x24);
static_assert(sizeof(SourceVariant_l)==0x84);
static_assert(offsetof(SourceRecord_l,mNested)==0x1C);
static_assert(offsetof(SourceRecordResource_l,mMetadata)==0x7C);
static_assert(offsetof(SourceNames_l,mParameterCount)==0xE);
static_assert(offsetof(SourceNames_l,mParameters)==0x18);
static_assert(sizeof(SourceParameterEntry_l)==0x10);
static_assert(offsetof(SourceParameterTable_l,mEntries)==0x20);
static_assert(offsetof(SourceShape_l,mVtxSkinCount)==0x16);
static_assert(offsetof(SourceLookupEntries_l,mEntries)==offsetof(RawLookupBlock_l,mFirstEntry));
static_assert(sizeof(SourceLookupEntries_l)==sizeof(RawLookupBlock_l));
static_assert(offsetof(agl::ShaderArchive_l,mBinaryImageAddress)==0x14);
static_assert(offsetof(agl::ShaderArchive_l,mResource)==0x18);
static_assert(offsetof(agl::ShaderArchive_l,mProgramCount)==0x1C);
static_assert(offsetof(agl::ShaderArchive_l,mPrograms)==0x20);
template<class T> T* materialSourceRelative(const be<u32>* field) {
    u32 relative=*field;
    if(relative==0) return nullptr;
    return gabi::at<T>(gabi::ea(field)+relative);
}
be<u32>* materialSourceReference(RawModelResource_l* resource,u32 index) {
    auto* dictionary=materialSourceRelative<SourceLookupEntries_l>(&resource->mMaterialLookupOffset);
    return &dictionary->mEntries[index].mDataOffset;
}
SourceShape_l* sourceShape(RawModelResource_l* resource,u32 index) {
    auto* dictionary=materialSourceRelative<SourceLookupEntries_l>(&resource->mShapeLookupOffset);
    return materialSourceRelative<SourceShape_l>(&dictionary->mEntries[index].mDataOffset);
}
SourceMaterial_l* sourceMaterial(RawModelResource_l* resource,u32 index) {
    return materialSourceRelative<SourceMaterial_l>(materialSourceReference(resource,index));
}
SourceVariant_l* selectedSourceVariant(SourceVariantTable_l* table,s32 result) {
    if(result<=0) return table->mDefault;
    SourceVariant_l* entries=table->mVariants;
    if(static_cast<u32>(result-1)<static_cast<u32>(table->mCount)) entries=&entries[result-1];
    return entries;
}
SourceRecord_l* selectedSourceRecord(SourceRecord_l* record,u16 identifier) {
    SourceRecordTable_l* nested=record->mNested;
    if(nested==nullptr) return record;
    if(identifier>=static_cast<u32>(nested->mCount)) return nullptr;
    SourceRecord_l* records=nested->mRecords;
    return &records[identifier];
}
bool matchingSourceText(sead::SafeText* left,sead::SafeText* right) {
    PipelineValueNameText_l::assureTermination(reinterpret_cast<PipelineValueNameText_l*>(left));
    left->assureTermination();
    const be<u8>* cached=left->mText;
    right->assureTermination();
    const be<u8>* required=right->mText;
    if(cached==required) return true;
    const be<u8>* reloaded=left->mText;
    for(u32 byte=0;byte!=0x40001;++byte) {
        u8 a=reloaded[byte];u8 b=required[byte];
        if(a!=b) return false;
        if(a==0) return true;
    }
    return false;
}

}
/* 027F70D8 */
u32 ModelDataPipeline_l::bindMaterialSources(FrameRecords* input,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F70D8u};
    gabi::FrameLocal<sead::SafeText> ownerName(8),requiredOwner(0x10),materialName(0x18);
    gabi::FrameLocal<PipelineFormattedText_l> failure(0x20);
    FrameRecords* sources=input;
    RawModelResource_l* resource=mResource;
    u32 status=0;
    for(s32 index=0;index<static_cast<s32>(resource->mMaterialCount);++index) {
        be<u32>* reference=materialSourceReference(resource,index);
        SourceMaterial_l* material=materialSourceRelative<SourceMaterial_l>(reference);
        SourceNames_l* names=materialSourceRelative<SourceNames_l>(&material->mSourceNames);
        if(names==nullptr) {
            status|=0xFFFFu;
            continue;
        }
        // 027F7180, 027F72A0, 027F72F8: the original loads input->mSource again for each use.
        auto* nameArchive=reinterpret_cast<agl::ShaderArchive_l*>(sources->mSource.get());
        u32 primaryAddress=static_cast<u32>(nameArchive->mBinaryImageAddress);
        if(primaryAddress!=0) {
            ownerName->mText.v=primaryAddress+0x18;
        }
        else {
            u32 resourceAddress=gabi::ea(nameArchive->mResource.get());
            ownerName->mText.v=resourceAddress+0x14;
        }
        ownerName->mVtable=&sMaterialSourceNameMethods;
        // Re-resolve after the stores, matching the original observable alias order.
        names=materialSourceRelative<SourceNames_l>(&material->mSourceNames);
        requiredOwner->mText=materialSourceRelative<be<u8>>(&names->mOwnerName);
        requiredOwner->mVtable=&sMaterialSourceNameMethods;
        PipelineValueNameText_l::assureTermination(reinterpret_cast<PipelineValueNameText_l*>(ownerName.get()));
        ownerName->assureTermination();
        const be<u8>* cachedOwner=ownerName->mText;
        requiredOwner->assureTermination();
        const be<u8>* required=requiredOwner->mText;
        bool same=cachedOwner==required;
        if(!same) {
            const be<u8>* left=ownerName->mText;
            const be<u8>* right=required;
            for(u32 character=0;character!=0x40001;++character) {
                u8 a=left[character];
                u8 b=right[character];
                if(a!=b) break;
                if(a==0) { same=true;break; }
            }
        }
        SourceRecord_l* selected=nullptr;
        if(same) {
            names=materialSourceRelative<SourceNames_l>(&material->mSourceNames);
            materialName->mText=materialSourceRelative<be<u8>>(&names->mMaterialName);
            materialName->mVtable=&sMaterialSourceNameMethods;
            s32 found=findMaterialSource(reinterpret_cast<agl::ShaderArchive_l*>(sources->mSource.get()),materialName.get());
            if(found>=0) {
                u32 count=static_cast<u32>(sources->mCount);
                SourceRecord_l* records=reinterpret_cast<SourceRecord_l*>(sources->mRecords.get());
                selected=static_cast<u32>(found)<count ? &records[found] : records;
                if(selected->mReady==0) {
                    void* source=nullptr;
                    auto* programArchive=reinterpret_cast<agl::ShaderArchive_l*>(sources->mSource.get());
                    if(static_cast<u32>(found)<static_cast<u32>(programArchive->mProgramCount)) {
                        auto* programs=reinterpret_cast<FrameSourceRecord*>(programArchive->mPrograms.get());
                        source=&programs[found];
                    }
                    initializeMaterialSource(selected,source,0);
                    count=static_cast<u32>(sources->mCount);
                    records=reinterpret_cast<SourceRecord_l*>(sources->mRecords.get());
                }
                selected=static_cast<u32>(found)<count ? &records[found] : records;
            }
        }
        if(selected!=nullptr) {
            resource=mResource;
            // Re-resolve the dictionary after source initialization; it may have changed.
            auto* dictionary=materialSourceRelative<SourceLookupEntries_l>(&resource->mMaterialLookupOffset);
            be<u32>* sourceField=&dictionary->mEntries[static_cast<u32>(index)].mDataOffset;
            void* materialSource=materialSourceRelative<void>(sourceField);
            applyPipelineMaterialSource(this,selected,materialSource,heap);
            resource=mResource;
            status|=0xFFFF0000u;
        } else {
            resource=mResource;
            status|=0xFFFFu;
        }
    }
    if((status&0xFFFFu)==0) mFlag=1;
    else {
        auto* dictionary=materialSourceRelative<SourceLookupEntries_l>(&resource->mMaterialLookupOffset);
        be<u32>* first=&dictionary->mEntries[0].mDataOffset;
        SourceMaterial_l* material=materialSourceRelative<SourceMaterial_l>(first);
        SourceNames_l* names=materialSourceRelative<SourceNames_l>(&material->mSourceNames);
        const be<u8>* owner=materialSourceRelative<be<u8>>(&names->mOwnerName);
        gabi::cpu->cr[6]=0;
        PipelineFormattedText_l* text=formatMaterialSourceFailure(failure.get(),reinterpret_cast<const char*>(&sMaterialSourceFailureFormat),reinterpret_cast<const char*>(owner));
        text->assureTermination();
        mFlag=0;
    }
    return status;
}
/* 027F691C */
void ModelDataPipeline_l::applyMaterialSource(void* sourceObject,void* materialObject) {
    gabi::AutoFrame frame{0x027F691Cu};
    gabi::FrameLocal<sead::SafeText> shapeName(8),materialName(0x10),parameterName(0x20);
    gabi::FrameLocal<SourceKeyPair_l> pairKeys(0x28);
    gabi::FrameLocal<SourceParameterPair_l> pair(0x18);
    gabi::FrameLocal<MaterialSourceWords_l> words(0x30);
    gabi::FrameLocal<SourcePointerArray_l> parameterNames(0x54),parameterValues(0x454);
    for(u32 word=0;word!=9;++word) words->mWords[word]=static_cast<u32>(sMaterialSourceInitialWords.mWords[word]);
    SourceRecord_l* record=static_cast<SourceRecord_l*>(sourceObject);
    SourceMaterial_l* material=static_cast<SourceMaterial_l*>(materialObject);
    SourceNames_l* names=materialSourceRelative<SourceNames_l>(&material->mSourceNames);
    RawModelResource_l* resource;
    if(names==nullptr || record==nullptr) {
        resource=mResource;
        for(s32 index=0;index<static_cast<s32>(resource->mShapeCount);++index) {
            SourceShape_l* shape=sourceShape(resource,index);
            SourceMaterial_l* linked=sourceMaterial(resource,shape->mMaterialIndex);
            if(linked==material) {
                for(u32 pass=0;pass!=4;++pass) {
                    u32 variantIndex=static_cast<u32>(index)*4+pass;
                    PipelineResourceVariant_l* variant=nullptr;
                    if(variantIndex<static_cast<u32>(mVariantCount)) {
                        PipelineResourceVariant_l* variants=mVariants;variant=&variants[variantIndex];
                    }
                    shape=sourceShape(resource,index);
                    constructMaterialSourceVariant(variant,material,shape,nullptr,&sMaterialSourceMaterialLabel);
                    resource=mResource;
                }
            }
        }
        return;
    }
    u32 parameterCount=names->mParameterCount;
    for(u32 index=0;index!=parameterCount;++index) {
        auto* parameters=materialSourceRelative<SourceParameterTable_l>(&names->mParameters);
        auto* nameField=&parameters->mEntries[index].mNameOffset;
        be<u8>* name=materialSourceRelative<be<u8>>(nameField);
        parameters=materialSourceRelative<SourceParameterTable_l>(&names->mParameters);
        auto* valueField=&parameters->mEntries[index].mValueOffset;
        void* value=materialSourceRelative<void>(valueField);
        SourceRecordResource_l* recordResource=record->mResource;
        parameterName->mText=name;parameterName->mVtable=&sMaterialSourceNameMethods;
        SourceVariantMetadata_l* metadata=recordResource->mMetadata;
        sead::SafeText* resolved=findMaterialSourceText(&metadata->mTable,parameterName.get());
        resolved->assureTermination();
        be<u8>* text=resolved->mText;
        parameterValues->mPointers[index]=value;
        parameterNames->mPointers[index]=text;
    }
    SourceRecordResource_l* recordResource=record->mResource;
    SourceVariantMetadata_l* metadata=recordResource->mMetadata;
    u32 identifier=recordResource->mIdentifier;
    s32 result=selectMaterialSourceVariant(&metadata->mTable,parameterCount,&parameterNames->mPointers[0],&parameterValues->mPointers[0],identifier);
    metadata=recordResource->mMetadata;
    SourceVariant_l* selection=selectedSourceVariant(&metadata->mTable,result);
    u16 selectedIdentifier=selection->mIdentifier;
    record=selectedSourceRecord(record,selectedIdentifier);
    resource=mResource;
    for(s32 index=0;index<static_cast<s32>(resource->mShapeCount);++index) {
        SourceShape_l* shape=sourceShape(resource,index);
        SourceMaterial_l* linked=sourceMaterial(resource,shape->mMaterialIndex);
        be<u8>* left=materialSourceRelative<be<u8>>(&linked->mName); /* 027F6CCC..027F6CE0: the linked material's name */
        shapeName->mVtable=&sMaterialSourceNameMethods;shapeName->mText=left;
        materialName->mVtable=&sMaterialSourceNameMethods;
        materialName->mText=materialSourceRelative<be<u8>>(&material->mName); /* 027F6C40, 027F6CF0..027F6D00 */
        bool matched=matchingSourceText(shapeName.get(),materialName.get());
        if(record!=nullptr && matched) {
            SourceRecord_l* current=record;
            pairKeys->mSecond=reinterpret_cast<be<u8>*>(&sMaterialSourceShaderPathKey);
            pairKeys->mFirst=reinterpret_cast<be<u8>*>(&sMaterialSourceSkinCountKey);
            for(u32 pass=0;pass!=4;++pass) {
                resource=mResource;shape=sourceShape(resource,index);
                u32 passWord=words->mWords[pass];
                u32 selectedWord=words->mWords[shape->mVtxSkinCount];
                pair->mSecond=passWord;pair->mFirst=selectedWord;
                recordResource=current->mResource;metadata=recordResource->mMetadata;
                identifier=recordResource->mIdentifier;
                result=selectMaterialSourceVariant(&metadata->mTable,2,pairKeys.get(),pair.get(),identifier);
                metadata=recordResource->mMetadata;
                selection=selectedSourceVariant(&metadata->mTable,result);
                selectedIdentifier=selection->mIdentifier;
                current=selectedSourceRecord(current,selectedIdentifier);
                u32 variantIndex=static_cast<u32>(index)*4+pass;
                PipelineResourceVariant_l* variant=nullptr;
                if(variantIndex<static_cast<u32>(mVariantCount)) {
                    PipelineResourceVariant_l* variants=mVariants;variant=&variants[variantIndex];
                }
                resource=mResource;shape=sourceShape(resource,index);
                recordResource=current->mResource;
                constructMaterialSourceVariant(variant,material,shape,recordResource,&sMaterialSourceMaterialLabel);
                variant->mState=gabi::ea(current);
            }
        }
        resource=mResource;
    }
}

}
/* WWHD glue
data sMaterialSourceNameMethods 1016E5A4
data sMaterialSourceFailureFormat 1016E888
data sMaterialSourceInitialWords 1016E834
data sMaterialSourceMaterialLabel 1016E830
data sMaterialSourceShaderPathKey 1016E858
data sMaterialSourceSkinCountKey 1016E864
ext 027BBF6C sead::SafeText* findMaterialSourceText(void*,sead::SafeText*)
ext 027BBD04 s32 selectMaterialSourceVariant(void*,u32,void*,void*,u32)
ext 0277462C void constructMaterialSourceVariant(j3d::pipeline::PipelineResourceVariant_l*,void*,void*,void*,void*)
ext 027B90AC s32 findMaterialSource(void*,sead::SafeText*)
ext 02800B0C void initializeMaterialSource(void*,void*,u32)
ext 027F691C void applyPipelineMaterialSource(j3d::pipeline::ModelDataPipeline_l*,void*,void*,sead::RegistryHeap*)
ext 027F76FC j3d::pipeline::PipelineFormattedText_l* formatMaterialSourceFailure(j3d::pipeline::PipelineFormattedText_l*,const char*,const char*)
*/
