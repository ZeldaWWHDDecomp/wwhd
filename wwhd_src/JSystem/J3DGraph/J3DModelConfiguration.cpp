// HD model material storage, shader assets and per-material draw state.
#include "JSystem/J3DGraph/J3DModelConfiguration.h"
#include "JSystem/J3DGraph/J3DAdapterUtilities.h"
using namespace j3d;
u32 modelConfigurationSize(const agl::gx2util::ModelBuilder*);
void* allocateConfigurationStorage(u32,u32);
void initializeModelConfiguration(ModelMaterialState_l*,const agl::gx2util::ModelBuilder*,void*,u32);
u32 modelConfigurationAuxiliarySize(ModelMaterialState_l*);
void bindModelConfigurationAuxiliary(ModelMaterialState_l*,void*,u32);
void* modelMaterialBuffer(MaterialDualBuffer_l*,u32);
void copyModelMaterialBuffer(void*,const void*,u32,u32);
void setModelPipelineMode(ModelMaterialState_l*,u32);
void setModelPipelineAsset(ModelMaterialState_l*,void*);
FrameRecords* modelFrameRegistry();
s32 findModelFrame(FrameSource*,const sead::SafeText*);
void activateModelFrame(FrameRecord*,FrameSourceRecord*,void*);
void* allocateConfigurationObject(u32);
void constructModelRenderBase(ModelRenderState_l*);
void zeroModelConfiguration(void*,u32);
void constructModelConfigurationArray(void*,u32,u32,u32);
void constructModelConfigurationVector(void*);
void initializeModelRenderState(ModelRenderState_l*,u32);
void initializeModelAnimationMode(void*,u32,u32);
sead::RegistryHeap* configurationCurrentHeap(void*);
static sead::SafeTextVtable sConfigurationTextMethods;
static sead::VtableSlot sModelRenderMethods;
static ModelScalar sConfigurationZero;
static ModelScalar sConfigurationOne;
// Borrowed read-only default: this entry only reads the four stored words.
static ModelColorWords_l sConfigurationColor;
/* inlined into ModelAdapter::configure */
static bool configurationTextEqual(sead::SafeText* expected,sead::SafeText* actual) {
    expected->assureTermination();
    expected->assureTermination();
    const be<u8>* saved=expected->mText;
    actual->assureTermination();
    if(gabi::ea(saved)==gabi::ea(static_cast<be<u8>*>(actual->mText))) return true;
    const be<u8>* left=expected->mText;
    const be<u8>* right=actual->mText;
    for(u32 index=0;index<=u32(sead::kMaxTextLength);++index) {
        u32 a=left[index],b=right[index];
        if(a!=b) return false;
        if(a==0) return true;
    }
    return false;
}
/* inlined into ModelAdapter::configure */
static ShaderAttribute_l* configurationMaterialAttribute(ShaderAttributes_l* attributes,sead::SafeText* expected,sead::SafeText* actual) {
    expected->mText=gabi::at<be<u8>>(gabi::guest_str("Mat"));
    expected->mVtable=&sConfigurationTextMethods;
    u32 count=attributes->mCount;
    ShaderAttribute_l* attribute=gabi::at<ShaderAttribute_l>(gabi::ea(attributes)+8);
    for(u32 index=0;index<count;++index) {
        actual->mVtable=&sConfigurationTextMethods;
        actual->mText=&attribute->mName[0];
        if(configurationTextEqual(expected,actual)) return attribute;
        attribute=gabi::at<ShaderAttribute_l>(gabi::ea(attribute)+u32(attribute->mNextOffset));
    }
    return NULL;
}
/* inlined into ModelAdapter::configure */
static FrameRecord* configurationFrame(FrameRecords* registry,s32 index) {
    u32 count=registry->mCount;
    FrameRecord* records=registry->mRecords;
    if(u32(index)<count) records+=index;
    return records;
}
/* inlined into ModelAdapter::configure */
static void copyConfigurationColor(ModelVector_l* destination) {
    // Wordwise load/store preserves the original float payloads and alias order.
    for(u32 word=0;word<4;++word)
        gabi::store<u32>(gabi::ea(destination)+word*4,u32(sConfigurationColor.mWords[word]));
}
/* 027F4404 */
u32 ModelAdapter::configure(J3DModel* model,J3DModelData* data,u32,u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    ModelState_l* state=reinterpret_cast<ModelState_l*>(model);
    state->mModelData=data;
    state->mRenderMode=mode;
    void* header=reinterpret_cast<ResourceHandle*>(data)->get();
    // The AGL model builder (layout block at +0x20 is filled by the size query).
    gabi::FrameLocal<agl::gx2util::ModelBuilder> descriptor(0x20);
    descriptor->mInstanceCount=mode;
    descriptor->mCached=0;
    descriptor->mPoseViews=2;
    descriptor->mCustomBytes=0;
    descriptor->mParameterViews=2;
    descriptor->mLayout.mTotal=0;
    descriptor->mUniformViews=2;
    descriptor->mResource=static_cast<agl::gx2util::ModelDescriptor*>(header);
    descriptor->mLayout.mBlocks=NULL;
    descriptor->mExternalPose=NULL;
    u32 size=modelConfigurationSize(descriptor.get());
    void* storage=allocateConfigurationStorage(size,64);
    state->mMaterialState.mStorage=storage;
    initializeModelConfiguration(&state->mMaterialState,descriptor.get(),storage,size);
    u32 auxiliarySize=modelConfigurationAuxiliarySize(&state->mMaterialState);
    state->mMaterialState.mAuxiliaryBytes=auxiliarySize;
    void* auxiliary=allocateConfigurationStorage(auxiliarySize,256);
    state->mMaterialState.mAuxiliary=auxiliary;
    bindModelConfigurationAuxiliary(&state->mMaterialState,auxiliary,state->mMaterialState.mAuxiliaryBytes);
    ModelProgramData_l* originalData=reinterpret_cast<ModelProgramData_l*>(data);
    bool direct=originalData->mDirect!=0;
    state->mDirectPrograms=originalData->mDirect;
    gabi::FrameLocal<sead::SafeText> expected(8),actual(0x10),pipeline(0x18);
    if(direct) {
        state->mDirectRoot=gabi::at<void>(gabi::ea(data)+0x14);
        u32 bindingIndex=0;
        u32 bindingOffset=0;
        while(bindingIndex<u32(state->mMaterialState.mBindingCount)) {
            u32 shapeCount=state->mMaterialState.mShapeCount;
            PacketMaterialBinding_l* bindings=state->mMaterialState.mBindings;
            PacketMaterialBinding_l* binding=gabi::at<PacketMaterialBinding_l>(gabi::ea(bindings)+bindingOffset);
            J3DEntryMaterialLookup* lookup=state->mMaterialState.mLookup;
            u32 shapeIndex=0;
            for(;shapeIndex<shapeCount;++shapeIndex) {
                ShapeResource_l* resource=gabi::at<ShapeResource_l>(gabi::ea(lookup)+shapeIndex*sizeof(J3DEntryMaterialLookup));
                u32 ordinal=resource->mHeader->mBindingIndex;
                if(gabi::ea(binding)==gabi::ea(bindings)+ordinal*sizeof(PacketMaterialBinding_l)) break;
            }
            if(shapeIndex<shapeCount) {
                ModelProgramData_l* currentData=reinterpret_cast<ModelProgramData_l*>(static_cast<J3DModelData*>(state->mModelData));
                // Out of range, the original reads the shader word through a NULL program unchecked; reading it by
                // address keeps that load as written instead of a host NULL dereference.
                u32 programAddress=0;
                if(shapeIndex*4+2<u32(currentData->mProgramCount)) {
                    ModelProgram_l* programs=currentData->mPrograms;
                    programAddress=gabi::ea(programs+shapeIndex*4+2);
                }
                ShaderProgram_l* shader=gabi::at<ShaderProgram_l>(gabi::load<u32>(programAddress+offsetof(ModelProgram_l,mShader)));
                ShaderState_l* shaderState=shader->mState;
                ShaderAttributes_l* attributes=shaderState->mAttributes;
                if(attributes!=NULL) {
                    ShaderAttribute_l* attribute=configurationMaterialAttribute(attributes,expected.get(),actual.get());
                    if(attribute!=NULL) {
                        for(u32 buffer=0;buffer<2;++buffer) {
                            void* destination=modelMaterialBuffer(&binding->mBuffer,buffer);
                            u32 payload=attribute->mPayloadOffset;
                            u32 extra=attribute->mPayloadExtra;
                            const void* source=gabi::at<void>(gabi::ea(attribute)+payload+0x18+extra);
                            u32 bytes=binding->mBuffer.mBytes;
                            copyModelMaterialBuffer(destination,source,bytes,0);
                        }
                    }
                }
            }
            ++bindingIndex;
            bindingOffset+=sizeof(PacketMaterialBinding_l);
        }
        state->mSetupState=NULL;
        allocateConfigurationObject(0x14);
    } else {
        // Existing shared interfaces remain opaque; their implementations are not this unit's scope.
        setModelPipelineMode(&state->mMaterialState,0);
        pipeline->mVtable=&sConfigurationTextMethods;
        pipeline->mText=gabi::at<be<u8>>(gabi::guest_str("wii_pipeline"));
        FrameRecords* registry=modelFrameRegistry();
        s32 index=findModelFrame(registry->mSource,pipeline.get());
        void* asset;
        if(index>=0) {
            FrameRecord* record=configurationFrame(registry,index);
            if(record->mActive==0) {
                FrameSource* source=registry->mSource;
                FrameSourceRecord* sourceRecord=NULL;
                if(u32(index)<u32(source->mCount)) {
                    FrameSourceRecord* records=source->mRecords;
                    sourceRecord=records+index;
                }
                activateModelFrame(record,sourceRecord,NULL);
            }
            record=configurationFrame(registry,index);
            asset=static_cast<ShaderProgram_l*>(record->mShader);
        } else {
            asset=gabi::at<void>(gabi::load<u32>(0));
        }
        setModelPipelineAsset(&state->mMaterialState,asset);
        state->mSetupState=NULL;
    }
    ModelRenderState_l* render=static_cast<ModelRenderState_l*>(allocateConfigurationObject(sizeof(ModelRenderState_l)));
    if(render!=NULL) {
        constructModelRenderBase(render);
        render->mMethods=&sModelRenderMethods;
        zeroModelConfiguration(&render->mColors[0],0x2F0);
        // These independent stores have no intervening calls and initialize eleven complete vectors.
        for(u32 vector=0;vector<11;++vector) {
            for(u32 component=0;component<3;++component) render->mColors[vector].mComponents[component]=sConfigurationZero.mValue;
            render->mColors[vector].mComponents[3]=sConfigurationOne.mValue;
        }
        for(u32 pair=0;pair<3;++pair)
            constructModelConfigurationArray(&render->mVectorPairs[pair][0],2,0x10,gabi::guest_fn((const void*)&constructModelConfigurationVector));
        for(u32 matrix=0;matrix<8;++matrix)
            if(gabi::ea(&render->mMatrices[matrix])==0) allocateConfigurationObject(0x30);
        for(u32 vector=0;vector<6;++vector)
            if(gabi::ea(&render->mVectors[vector])==0) allocateConfigurationObject(0x10);
    }
    state->mRenderState=render;
    initializeModelRenderState(render,0);
    copyConfigurationColor(&state->mRenderState->mColors[2]);
    copyConfigurationColor(&state->mRenderState->mColors[3]);
    copyConfigurationColor(&state->mRenderState->mColors[0]);
    copyConfigurationColor(&state->mRenderState->mColors[1]);
    copyConfigurationColor(&state->mRenderState->mColors[4]);
    u32 count=state->mMaterialState.mBindingCount;
    if(count!=0) {
        sead::RegistryHeap* heap=configurationCurrentHeap(sRegistryManager);
        ModelMaterialDrawState_l* draws=gabi::call_ptr<ModelMaterialDrawState_l*>(heap->dispatch()->mAllocate.mFunction,heap,count*u32(sizeof(ModelMaterialDrawState_l)),4);
        u32 offset=0;
        u32 constructions=count>0?count:1;
        for(u32 left=constructions;left!=0;--left) {
            ModelMaterialDrawState_l* draw=gabi::at<ModelMaterialDrawState_l>(gabi::ea(draws)+offset);
            if(draw!=NULL) {
                zeroModelConfiguration(draw,0x30);
                draw->mPassMask=7;
                be<u8>* dirty=&draw->mDirty;
                if(gabi::ea(dirty)==0) dirty=static_cast<be<u8>*>(allocateConfigurationObject(1));
                if(dirty!=NULL) *dirty=0;
            }
            offset+=sizeof(ModelMaterialDrawState_l);
        }
        if(draws!=NULL) { state->mMaterials=draws; state->mMaterialCount=count; }
    }
    initializeModelAnimationMode(&state->mAnimation[0],2,0);
    return initializePackets(model);
}
/* WWHD glue
 ext 027DDFA4 u32 modelConfigurationSize(const agl::gx2util::ModelBuilder*)
 ext 027EC0AC void* allocateConfigurationStorage(u32,u32)
 ext 027DE0AC void initializeModelConfiguration(j3d::ModelMaterialState_l*,const agl::gx2util::ModelBuilder*,void*,u32)
 ext 027DE4D8 u32 modelConfigurationAuxiliarySize(j3d::ModelMaterialState_l*)
 ext 027DE5BC void bindModelConfigurationAuxiliary(j3d::ModelMaterialState_l*,void*,u32)
 ext 027DBBB0 void* modelMaterialBuffer(j3d::MaterialDualBuffer_l*,u32)
 ext C0009988 void copyModelMaterialBuffer(void*,const void*,u32,u32)
 ext 02772FD0 void setModelPipelineMode(j3d::ModelMaterialState_l*,u32)
 ext 02773134 void setModelPipelineAsset(j3d::ModelMaterialState_l*,void*)
 ext 027FFCBC j3d::FrameRecords* modelFrameRegistry()
 ext 027B90AC s32 findModelFrame(j3d::FrameSource*,const sead::SafeText*)
 ext 02800B0C void activateModelFrame(j3d::FrameRecord*,j3d::FrameSourceRecord*,void*)
 ext 0273AD10 void* allocateConfigurationObject(u32)
 ext 027FB40C void constructModelRenderBase(j3d::ModelRenderState_l*)
 ext 028F521C void zeroModelConfiguration(void*,u32)
 ext 028EFFD0 void constructModelConfigurationArray(void*,u32,u32,u32)
 ext 027F7654 void constructModelConfigurationVector(void*) callback
 ext 027FB5D4 void initializeModelRenderState(j3d::ModelRenderState_l*,u32)
 ext 027FD838 void initializeModelAnimationMode(void*,u32,u32)
 ext 02756140 sead::RegistryHeap* configurationCurrentHeap(void*)
 str 1016E808 "Mat"
 str 1016E80C "wii_pipeline"
 data sConfigurationTextMethods 1016E5A4
 data sModelRenderMethods 1016EFB4
 data sConfigurationZero 10145180
 data sConfigurationOne 1014517C
 data sConfigurationColor 104A01EC
*/
