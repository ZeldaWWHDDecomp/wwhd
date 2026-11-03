#include "agl/BloomParameters.h"
#include "agl/EnvironmentLights.h"
#include "agl/ShaderCommandState.h"
using namespace agl::environment;
using gabi::ea;
template <typename Base>
static Base* bloomBaseFromSecondary(void* secondary) {
    return gabi::at<Base>(ea(secondary)-0x2E4);
}
void resetBloomBase(BloomParameters*,u32);
/* 0277497C */
void BloomColorWeights::normalize() {
    f32 red,green,blue;
    if(mMode==1) { red=mRed;green=mGreen;blue=mBlue; }
    else { red=0.298911988735199f;green=0.5866109728813171f;blue=0.11447799950838089f; }
    const f32 sum=gabi::fadds_ppc(gabi::fadds_ppc(red,green),blue);
    mNormalizedGreen=green;mNormalizedRed=red;mNormalizedBlue=blue;
    if(!(0.0f<sum)) return;
    const f32 inverse=gabi::fdivs_ppc(1.0f,sum);
    mNormalizedRed=gabi::fmuls_ppc(red,inverse);
    mNormalizedGreen=gabi::fmuls_ppc(green,inverse);
    mNormalizedBlue=gabi::fmuls_ppc(blue,inverse);
}
/* 02774A20 */
void BloomParameters::reset() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    resetBloomBase(this,0);mColorWeights.normalize();
}
/* 02774A58 */
void resetBloomThroughSecondary(void* secondary) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    bloomBaseFromSecondary<BloomParameters>(secondary)->reset();
}
struct BloomParameterOwner {
    u8 mBeforeKey[0x18]; be<u32> mKey; u8 mBeforeFirst[0x24];
    agl::env::ParameterBinding mFirst;u8 mBeforeSecond[0xBC];
    agl::env::EnvironmentScalarParameter mSecond;u8 mBeforeThird[0x40];agl::env::ParameterBinding mThird;
};
struct BloomResourceEntry { be<u32> mSize,mWord,mKey; };
struct BloomResourceTable { be<u32> mWord,mCount;u8 mBeforeEntries[8];BloomResourceEntry mEntries[1]; };
static BloomResourceEntry* bloomResourceEntryAt(u32 address) { return gabi::at<BloomResourceEntry>(address); }
struct BloomResourceLink { gptr<BloomResourceTable> mTable; };
static u8 sBloomFirstParameterKey;
static u8 sBloomSecondParameterKey;
static u8 sBloomThirdParameterKey;
static sead::SafeTextVtable sBloomParameterNameTable;
u32 hashBloomParameter(const sead::SafeText*);
void importBloomParameter(agl::env::ParameterBinding*,const gptr<BloomResourceEntry>*);
struct BloomRenderSettings {
    agl::env::EnvironmentScalarParameter mScalars[3];agl::env::EnvironmentColorParameter mColor;agl::env::EnvironmentScalarParameter mFade;
};
struct BloomSampleSettings {
    agl::env::EnvironmentBooleanParameter mEnabled;agl::env::EnvironmentScalarParameter mScalars[4];
};
struct BloomByteParameterStorage { agl::env::ParameterBinding mBinding;be<u8> mValue;u8 mPadding[3]; };
struct BloomIntParameter { agl::env::ParameterBinding mBinding;be<s32> mValue; };
static_assert(sizeof(BloomIntParameter)==sizeof(agl::env::EnvironmentScalarParameter));
struct BloomColorWeightsStorage {
    BloomRenderSettings mRender[2];BloomSampleSettings mSamples[3];agl::env::EnvironmentColorParameter mColors[3];
    BloomIntParameter mScalarParameters[2];agl::env::EnvironmentColorParameter mDefaultColor;
    BloomIntParameter mThresholds[2];BloomByteParameterStorage mFlags[2];
    agl::env::EnvironmentScalarParameter mBiases[2];be<s32> mMode;be<u16> mCount;u8 mCountPadding[2];
    be<f32> mNormalizedValues[3];
};
template <typename Field>
static Field* bloomEmbeddedField(Field* field) {
    return gabi::at<Field>(ea(field));
}
static_assert(sizeof(BloomRenderSettings)==0x5C);
static_assert(sizeof(BloomSampleSettings)==0x50);
static_assert(offsetof(BloomColorWeightsStorage,mRender)==0x0);
static_assert(offsetof(BloomColorWeightsStorage,mSamples)==0xB8);
static_assert(offsetof(BloomColorWeightsStorage,mColors)==0x1A8);
static_assert(offsetof(BloomColorWeightsStorage,mScalarParameters)==0x1FC);
static_assert(offsetof(BloomColorWeightsStorage,mDefaultColor)==0x21C);
static_assert(offsetof(BloomColorWeightsStorage,mThresholds)==0x238);
static_assert(offsetof(BloomColorWeightsStorage,mFlags)==0x258);
static_assert(offsetof(BloomColorWeightsStorage,mBiases)==0x278);
static_assert(offsetof(BloomColorWeightsStorage,mMode)==0x298);
static_assert(offsetof(BloomColorWeightsStorage,mCount)==0x29C);
static_assert(offsetof(BloomColorWeightsStorage,mNormalizedValues)==0x2A0);
static_assert(sizeof(BloomColorWeightsStorage)==0x2AC);
/* 02774A60 */
void importBloomParameterResource(BloomParameterOwner* owner,u32 key,void* parameter,const BloomResourceLink* link) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    BloomResourceTable* table=link->mTable;
    if(table==nullptr||((u32)owner->mKey)!=key) return;
    const u32 parameterAddress=ea(parameter);
    u32 descriptorOffset,referenceOffset;const be<u8>* name=nullptr;
    bool isSecondParameter=false;
    if(parameterAddress==ea(&owner->mFirst)) {descriptorOffset=0x14;referenceOffset=8;name=&sBloomFirstParameterKey;}
    else if(parameterAddress==ea(&owner->mSecond)) {descriptorOffset=0x1C;referenceOffset=0xC;name=&sBloomSecondParameterKey;isSecondParameter=true;}
    else if(parameterAddress==ea(&owner->mThird)) {descriptorOffset=0x24;referenceOffset=0x10;name=&sBloomThirdParameterKey;}
    else return;
    gabi::FrameLocal<sead::SafeText> descriptor(descriptorOffset);
    descriptor->mText=const_cast<be<u8>*>(name);descriptor->mVtable=&sBloomParameterNameTable;
    const u32 hashed=hashBloomParameter(descriptor.get());
    const u32 count=table->mCount;u32 resourceAddress=ea(table)+offsetof(BloomResourceTable,mEntries);
    for(u32 index=0;index<count;++index) {
        BloomResourceEntry* resource=bloomResourceEntryAt(resourceAddress);
        if((u32)resource->mKey==hashed) {
            gabi::FrameLocal<gptr<BloomResourceEntry>> reference(referenceOffset);
            *reference.get()=resource;
            importBloomParameter(static_cast<agl::env::ParameterBinding*>(parameter),reference.get());
            if(isSecondParameter) owner->mSecond.mValue=gabi::fadds_ppc(owner->mSecond.mValue,1.0f);
            return;
        }
        resourceAddress+=(u32)resource->mSize;
    }
}
/* 02774C9C */
void importBloomThroughSecondary(void* secondary,u32 key,void* parameter,const BloomResourceLink* link) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    importBloomParameterResource(bloomBaseFromSecondary<BloomParameterOwner>(secondary),key,parameter,link);
}
static_assert(offsetof(BloomParameterOwner,mFirst)==0x40);
static_assert(offsetof(BloomParameterOwner,mSecond)==0x108);
static_assert(offsetof(BloomParameterOwner,mThird)==0x158);
static_assert(offsetof(BloomResourceTable,mEntries)==0x10);
struct BloomParameterObject { u8 mStorage[0x60]; };
struct BloomEffect {
    BloomParameters mParameters;u8 mBeforeParameterObject[0x18];BloomParameterObject mParameterObject;
    gptr<void> mInterfaceTable;u8 mBeforeSecondary[0x164];gptr<void> mSecondaryTable;
    BloomByteParameterStorage mEnabledParameter;be<u16> mParameterCount;be<u8> mBufferStateMode;u8 mBeforeBufferState;
    u8 mBufferState[0xC];gptr<void> mBufferTable,mTailTable;
};
static u32 sBloomEffectTable;
static u32 sBloomEffectSecondaryTable;
static u32 sBloomEffectBufferTable;
static u32 sBloomEffectTailTable;
void releaseBloomBuffer(void*);
void destroyBloomBufferBase(void*,u32);
void destroyBloomEffectBase(BloomEffect*,u32);
void releaseBloomStorage(void*);
void notifyBloomBase(void*);
/* 02774CA4 */
void destroyBloomEffect(BloomEffect* effect,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(effect==nullptr) return;
    effect->mInterfaceTable=&sBloomEffectTable;effect->mBufferTable=&sBloomEffectBufferTable;
    effect->mSecondaryTable=&sBloomEffectSecondaryTable;effect->mTailTable=&sBloomEffectTailTable;
    releaseBloomBuffer(effect->mBufferState);destroyBloomBufferBase(effect->mBufferState,0);
    destroyBloomEffectBase(effect,0);
    if((flags&1)!=0) releaseBloomStorage(effect);
}
/* 02774D3C */
void notifyBloomThroughSecondary(void* secondary) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    notifyBloomBase(bloomBaseFromSecondary<void>(secondary));
}
using BloomArrayConstructor=void* (*)(void*);
void constructBloomArrayElements(void*,u32,u32,BloomArrayConstructor);
void* constructBloomRenderSlot(void*);
void* constructBloomSampleSlot(void*);
void* constructBloomColorSlot(void*);
void* allocateBloomStorage(u32);
void initializeBloomParameter(agl::env::ParameterBinding*);
static sead::VtableSlot sBloomFloatParameterTable;
static sead::VtableSlot sBloomByteParameterTable;
static sead::VtableSlot sBloomSecondaryFloatParameterTable;
static sead::VtableSlot sBloomColorParameterTable;
static void constructBloomArray(void* array,u32 stride,u32 count,BloomArrayConstructor constructor) {
    if(array==nullptr) array=allocateBloomStorage(stride*count);
    if(array!=nullptr) constructBloomArrayElements(array,count,stride,constructor);
}
template <typename Parameter>
static Parameter* constructBloomParameter(Parameter* parameter,u32 size,sead::VtableSlot* table) {
    if(parameter==nullptr) parameter=static_cast<Parameter*>(allocateBloomStorage(size));
    if(parameter!=nullptr) {
        initializeBloomParameter(&parameter->mBinding);parameter->mBinding.mMethods=table;
    }
    return parameter;
}
/* 02774D44 */
BloomColorWeights* constructBloomColorWeights(BloomColorWeights* weights) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(weights==nullptr) weights=static_cast<BloomColorWeights*>(allocateBloomStorage(sizeof(BloomColorWeights)));
    if(weights!=nullptr) {
        auto* storage=reinterpret_cast<BloomColorWeightsStorage*>(weights);
        constructBloomArray(bloomEmbeddedField(&storage->mRender[0]),sizeof(BloomRenderSettings),2,constructBloomRenderSlot);
        constructBloomArray(bloomEmbeddedField(&storage->mSamples[0]),sizeof(BloomSampleSettings),3,constructBloomSampleSlot);
        constructBloomArray(bloomEmbeddedField(&storage->mColors[0]),sizeof(agl::env::EnvironmentColorParameter),3,constructBloomColorSlot);
        constructBloomParameter(bloomEmbeddedField(&storage->mScalarParameters[0]),sizeof(BloomIntParameter),&sBloomFloatParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mScalarParameters[1]),sizeof(BloomIntParameter),&sBloomFloatParameterTable);
        auto* color=constructBloomParameter(bloomEmbeddedField(&storage->mDefaultColor),sizeof(agl::env::EnvironmentColorParameter),&sBloomColorParameterTable);
        if(color!=nullptr) {color->mValue.r=0.0f;color->mValue.g=0.0f;color->mValue.b=0.0f;color->mValue.a=1.0f;}
        constructBloomParameter(bloomEmbeddedField(&storage->mThresholds[0]),sizeof(BloomIntParameter),&sBloomFloatParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mThresholds[1]),sizeof(BloomIntParameter),&sBloomFloatParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mFlags[0]),sizeof(BloomByteParameterStorage),&sBloomByteParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mFlags[1]),sizeof(BloomByteParameterStorage),&sBloomByteParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mBiases[0]),sizeof(agl::env::EnvironmentScalarParameter),&sBloomSecondaryFloatParameterTable);
        constructBloomParameter(bloomEmbeddedField(&storage->mBiases[1]),sizeof(agl::env::EnvironmentScalarParameter),&sBloomSecondaryFloatParameterTable);
        storage->mMode=0;
        auto* count=bloomEmbeddedField(&storage->mCount);
        if(count==nullptr) count=static_cast<be<u16>*>(allocateBloomStorage(2));
        if(count!=nullptr) *count=0;
        if(bloomEmbeddedField(&storage->mNormalizedValues[0])==nullptr) allocateBloomStorage(12);
    }
    return weights;
}
static_assert(offsetof(BloomEffect,mSecondaryTable)==0x4AC);
static_assert(offsetof(BloomEffect,mParameters.mOwner)==0x18);
static_assert(offsetof(BloomEffect,mParameters.mTable)==0x1C);
static_assert(offsetof(BloomEffect,mParameterObject)==0x2E4);
static_assert(offsetof(BloomEffect,mEnabledParameter)==0x4B0);
static_assert(offsetof(BloomEffect,mParameterCount)==0x4C0);
static_assert(offsetof(BloomEffect,mBufferStateMode)==0x4C2);
static_assert(offsetof(BloomEffect,mBufferState)==0x4C4);
static_assert(sizeof(BloomEffect)==0x4D8);
struct BloomChildSlot { be<u32> mCount;u8 mTail[0x2E0]; };
struct BloomRuntimeEntry {
    u8 mBeforeEnabled[0x91];be<u8> mEnabled;u8 mBeforeBlend[0x32E];
    be<u8> mBlendEnabled;u8 mPadding[3];be<u32> mBlendValue,mBlendMode,mBlendFilter,mBlendFormat;
    u8 mBeforeFlags[0xEC];be<u8> mFlags;u8 mBeforeChildren[0x1DF];
    BloomChildSlot mChildren[2];be<u32> mChildCount;be<f32> mWeight,mNear,mFar,mScale;
    u8 mWorkList[0x1C];
};
void initializeBloomWorkList(void*,u32,void*);
/* 02774FD0 */
void initializeBloomRuntime(void*,BloomRuntimeEntry* runtime,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    const u8 flags=runtime->mFlags;
    runtime->mBlendValue=0;runtime->mBlendFormat=5;runtime->mFlags=flags|1;
    runtime->mBlendFilter=2;runtime->mBlendEnabled=1;runtime->mEnabled=1;runtime->mBlendMode=1;
    runtime->mScale=1.0f;runtime->mNear=0.1f;runtime->mFar=10000.0f;runtime->mWeight=1.0f;
    initializeBloomWorkList(runtime->mWorkList,0x80,heap);
    runtime->mChildCount=0;
    runtime->mChildren[0].mCount=0;runtime->mChildren[1].mCount=0;
}
static_assert(offsetof(BloomRuntimeEntry,mBlendEnabled)==0x3C0);
static_assert(offsetof(BloomRuntimeEntry,mFlags)==0x4C0);
static_assert(offsetof(BloomRuntimeEntry,mChildren)==0x6A0);
static_assert(offsetof(BloomRuntimeEntry,mChildCount)==0xC68);
static_assert(offsetof(BloomRuntimeEntry,mWorkList)==0xC7C);
static_assert(sizeof(BloomRuntimeEntry)==0xC98);
struct BloomThresholdFactors {
    u8 mBeforeStart[0xC];be<f32> mStart;u8 mBeforeWidth[0xC];be<f32> mWidth;
    u8 mBeforeBias[0xC];be<f32> mBias;
};
struct BloomThresholdOutput {be<f32> mOffset,mInverseStart,mBias,mInverseWidth;};
/* 02775930 */
void calculateBloomThreshold(const BloomThresholdFactors* factors,BloomThresholdOutput* output,f64 scale) {
    const f32 width=gabi::fmuls_fpr((f32)factors->mWidth,scale);
    const f32 start=gabi::fmuls_fpr((f32)factors->mStart,scale);
    f32 inverseWidth;
    if(0.0f<width) {
        inverseWidth=gabi::fdivs_ppc(1.0f,width);
        output->mOffset=-gabi::fmuls_ppc(start,inverseWidth);
        output->mInverseStart=0.0f<start?gabi::fdivs_ppc(1.0f,start):9.999999747378752e-06f;
    } else {
        inverseWidth=0.0f;output->mOffset=-gabi::fmuls_ppc(start,inverseWidth);
        output->mInverseStart=0.0f<start?gabi::fdivs_ppc(1.0f,start):9.999999747378752e-06f;
    }
    const f32 bias=factors->mBias;output->mInverseWidth=inverseWidth;output->mBias=bias;
}
struct BloomBufferedEffect {BloomEffect mBase;u8 mTail[0x1D0];};
static u32 sBloomBufferedTable;
static u8 sBloomBufferedKey;
static u8 sBloomBufferKey;
static u8 sBloomEnabledName;
static u8 sBloomEnabledDescription;
static u8 sBloomBlankHint;
static u8 sBloomObjectKey;
static u8 sBloomTextureKey;
void initializeBloomEffectBase(void*);
void initializeBloomParameterObject(void*,const sead::SafeText*,u32);
void constructBloomBufferState(void*);
void initializeBloomBufferInterface(void*,const sead::SafeText*);
/* 027759BC */
BloomBufferedEffect* constructBufferedBloomEffect(BloomBufferedEffect* effect) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(effect==nullptr) effect=static_cast<BloomBufferedEffect*>(allocateBloomStorage(sizeof(BloomBufferedEffect)));
    if(effect!=nullptr) {
        initializeBloomEffectBase(effect);
        gabi::FrameLocal<sead::SafeText> name(8);
        name->mText=&sBloomBufferedKey;name->mVtable=&sBloomParameterNameTable;
        initializeBloomParameterObject(&effect->mBase.mParameterObject,name.get(),1);
        effect->mBase.mParameters.mTable=&sBloomBufferedTable;
        effect->mBase.mSecondaryTable=&sBloomEffectSecondaryTable;effect->mBase.mInterfaceTable=&sBloomEffectTable;
        constructBloomParameter(bloomEmbeddedField(&effect->mBase.mEnabledParameter),sizeof(BloomByteParameterStorage),&sBloomByteParameterTable);
        auto* count=bloomEmbeddedField(&effect->mBase.mParameterCount);
        if(count==nullptr) count=static_cast<be<u16>*>(allocateBloomStorage(2));
        if(count!=nullptr) *count=0;
        effect->mBase.mBufferStateMode=5;
        constructBloomBufferState(effect->mBase.mBufferState);
        gabi::FrameLocal<sead::SafeText> bufferName(0x10);
        bufferName->mText=&sBloomBufferKey;bufferName->mVtable=&sBloomParameterNameTable;
        initializeBloomBufferInterface(&effect->mBase.mSecondaryTable,bufferName.get());
    }
    return effect;
}
void setupBloomEffectBase(void*,void*,u32,void*);
void bindBloomNamedParameter(agl::env::ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,void*);
void linkBloomParameterObject(void*,void*,const sead::SafeText*);
void setupBloomTexture(void*,void*,const sead::SafeText*,void*);
/* 02775AD8 */
void setupBufferedBloomEffect(BloomBufferedEffect* effect,void* context,void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    setupBloomEffectBase(effect,context,0,heap);
    void* owner=effect->mBase.mParameters.mOwner.get();
    gabi::FrameLocal<sead::SafeText> description(8),name(0x10),hint(0x18);
    description->mText=&sBloomEnabledDescription;description->mVtable=&sBloomParameterNameTable;
    name->mText=&sBloomEnabledName;name->mVtable=&sBloomParameterNameTable;
    hint->mText=&sBloomBlankHint;hint->mVtable=&sBloomParameterNameTable;
    bindBloomNamedParameter(bloomEmbeddedField(&effect->mBase.mEnabledParameter.mBinding),name.get(),description.get(),hint.get(),owner);
    owner=effect->mBase.mParameters.mOwner.get();
    description->mVtable=&sBloomParameterNameTable;effect->mBase.mEnabledParameter.mValue=1;
    description->mText=&sBloomObjectKey;
    linkBloomParameterObject(&effect->mBase.mParameterObject,owner,description.get());
    resetBloomBase(&effect->mBase.mParameters,0);
    effect->mBase.mParameters.mColorWeights.normalize();
    description->mVtable=&sBloomParameterNameTable;description->mText=&sBloomTextureKey;
    setupBloomTexture(effect->mBase.mBufferState,context,description.get(),heap);
}
struct BloomTextureOwner {u8 mBeforeSampler[0x3C];gptr<void> mSampler;};
struct BloomDrawContext {u8 mPrefix[0x31C];be<u32> mIndexType,mPrimitive;gptr<void> mIndices;be<u32> mCount;};
struct BloomDrawContextSlot {gptr<BloomDrawContext> mContext;};
static BloomDrawContextSlot sBloomDrawContext;
void bindBloomTexture(void*,void*,s32,u32);
void drawBloomIndexed(u32,u32,u32,const void*,u32,u32);
/* 02775BDC */
void drawBloomTexture(void*,const BloomTextureOwner* owner,void* texture) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    bindBloomTexture(texture,owner->mSampler.get(),-1,0);
    auto* context=sBloomDrawContext.mContext.get();
    const u32 count=context->mCount;
    if(count!=0) drawBloomIndexed(context->mPrimitive,count,context->mIndexType,context->mIndices.get(),0,1);
}
static_assert(sizeof(BloomBufferedEffect)==0x6A8);
static_assert(offsetof(BloomThresholdFactors,mWidth)==0x1C);
static_assert(offsetof(BloomThresholdFactors,mBias)==0x2C);
static_assert(offsetof(BloomDrawContext,mCount)==0x328);
static u8 sBloomText10161BDC;
static u8 sBloomText10161C18;
static u8 sBloomText10161815;
static u8 sBloomText10161B54;
static u8 sBloomText10161C24;
static u8 sBloomText10161816;
static u8 sBloomText10161C30;
static u8 sBloomText10161C44;
static u8 sBloomText10161817;
static u8 sBloomText10161C54;
static u8 sBloomText10161C60;
static u8 sBloomText10161C6C;
static u8 sBloomText10161C80;
static u8 sBloomText10161814;
static u8 sBloomText10161B94;
static u8 sBloomText10161C98;
static u8 sBloomText10161BBC;
static u8 sBloomText10161CAC;
static u8 sBloomText10161B44;
static u8 sBloomText10161CC4;
static u8 sBloomText10161CD0;
static u8 sBloomText10161CE0;
static u8 sBloomText10161B5C;
static u8 sBloomText10161CEC;
static u8 sBloomText10161CFC;
static u8 sBloomText10161BE8;
static u8 sBloomText10161D10;
static u8 sBloomText10161BD0;
static u8 sBloomText10161B64;
static u8 sBloomText10161BFC;
static u8 sBloomText10161BAC;
static u8 sBloomText10161D20;
static u8 sBloomText10161B6C;
static u8 sBloomText10161B4C;
static u8 sBloomText10161D2C;
static u8 sBloomText10161D3C;
static u8 sBloomText10161D48;
static u8 sBloomText10161D58;
static u8 sBloomText10161B74;
static u8 sBloomText10161C0C;
static u8 sBloomText10161B7C;
static u8 sBloomText10161D68;
static u8 sBloomText10161B8C;
static u8 sBloomText10161B84;
static u8 sBloomText10161D74;
static u8 sBloomText10161D80;
struct BloomFormattedName { sead::SafeText mName;be<u32> mCapacity;be<u8> mText[128]; };
struct BloomLabelPair {gptr<const be<u8>> mLabels[2];};
struct BloomLabelTriple {gptr<const be<u8>> mLabels[3];};
static BloomLabelPair sBloomThresholdLabels;
static BloomLabelTriple sBloomSampleLabels;
static agl::env::ParameterColor sBloomDefaultColor;
sead::SafeText* formatBloomLabel(BloomFormattedName*,const be<u8>*,const be<u8>*);
sead::SafeText* formatBloomIndex(BloomFormattedName*,const be<u8>*,u32);
static void setBloomName(sead::SafeText* descriptor,const be<u8>* text) {
    descriptor->mText=const_cast<be<u8>*>(text);descriptor->mVtable=&sBloomParameterNameTable;
}
static void bindBloomFixed(agl::env::ParameterBinding* parameter,void* owner,const be<u8>* name,const be<u8>* description,const be<u8>* hint,
                           u32 nameOffset,u32 descriptionOffset,u32 hintOffset) {
    gabi::FrameLocal<sead::SafeText> localName(nameOffset),localDescription(descriptionOffset),localHint(hintOffset);
    setBloomName(localName.get(),name);setBloomName(localDescription.get(),description);setBloomName(localHint.get(),hint);
    bindBloomNamedParameter(parameter,localName.get(),localDescription.get(),localHint.get(),owner);
}
static void copyBloomDefaultColor(agl::env::EnvironmentColorParameter* parameter) {
    parameter->mValue=sBloomDefaultColor;
}
/* 02775090 */
void bindBloomColorWeights(BloomColorWeights* weights,void* owner) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<BloomColorWeights>> saved(0x2C);*saved.get()=weights;
    auto* storage=reinterpret_cast<BloomColorWeightsStorage*>(weights);
    bindBloomFixed(bloomEmbeddedField(&storage->mScalarParameters[0].mBinding),owner,&sBloomText10161BDC,&sBloomText10161C18,&sBloomText10161815,68,76,180);
    storage->mScalarParameters[0].mValue=0;
    bindBloomFixed(bloomEmbeddedField(&storage->mBiases[1].mBinding),owner,&sBloomText10161B54,&sBloomText10161C24,&sBloomText10161816,76,68,188);
    storage->mBiases[1].mValue=1.0f;
    bindBloomFixed(bloomEmbeddedField(&storage->mDefaultColor.mBinding),owner,&sBloomText10161C30,&sBloomText10161C44,&sBloomText10161817,76,68,196);
    copyBloomDefaultColor(bloomEmbeddedField(&storage->mDefaultColor));
    bindBloomFixed(bloomEmbeddedField(&storage->mScalarParameters[1].mBinding),owner,&sBloomText10161C54,&sBloomText10161C60,&sBloomText10161815,76,68,204);
    storage->mScalarParameters[1].mValue=2;
    bindBloomFixed(bloomEmbeddedField(&storage->mFlags[0].mBinding),owner,&sBloomText10161C6C,&sBloomText10161C80,&sBloomText10161814,76,68,212);
    storage->mFlags[0].mValue=0;
    bindBloomFixed(bloomEmbeddedField(&storage->mFlags[1].mBinding),owner,&sBloomText10161B94,&sBloomText10161C98,&sBloomText10161814,76,68,220);
    storage->mFlags[1].mValue=0;
    bindBloomFixed(bloomEmbeddedField(&storage->mBiases[0].mBinding),owner,&sBloomText10161BBC,&sBloomText10161CAC,&sBloomText10161816,76,68,228);
    storage->mBiases[0].mValue=1.0f;
    bindBloomFixed(bloomEmbeddedField(&storage->mThresholds[0].mBinding),owner,&sBloomText10161B44,&sBloomText10161CC4,&sBloomText10161815,76,68,236);
    storage->mThresholds[0].mValue=0;
    bindBloomFixed(bloomEmbeddedField(&storage->mThresholds[1].mBinding),owner,&sBloomText10161CD0,&sBloomText10161CE0,&sBloomText10161815,76,68,244);
    storage->mThresholds[1].mValue=5;
    gabi::FrameLocal<BloomFormattedName> thresholdName(0xFC),sampleName(0x188),colorName(0x22C);
    for(u32 index=0;index<2;++index) {
        auto* slot=&storage->mRender[index];
        const be<u8>* label=sBloomThresholdLabels.mLabels[index];
        {
            gabi::FrameLocal<sead::SafeText> description(0x34),hint(84);
            setBloomName(description.get(),&sBloomText10161B5C);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(thresholdName.get(),&sBloomText10161CEC,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[0].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[0].mValue=0.75f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x34),hint(92);
            setBloomName(description.get(),&sBloomText10161CFC);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(thresholdName.get(),&sBloomText10161BE8,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[1].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[1].mValue=0.1f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x34),hint(100);
            setBloomName(description.get(),&sBloomText10161D10);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(thresholdName.get(),&sBloomText10161BD0,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[2].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[2].mValue=1.0f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x34),hint(108);
            setBloomName(description.get(),&sBloomText10161B64);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(thresholdName.get(),&sBloomText10161BFC,label);
            setBloomName(hint.get(),&sBloomText10161817);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mColor.mBinding),name,description.get(),hint.get(),owner);
            copyBloomDefaultColor(bloomEmbeddedField(&slot->mColor));
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x34),hint(116);
            setBloomName(description.get(),&sBloomText10161BAC);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(thresholdName.get(),&sBloomText10161D20,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mFade.mBinding),name,description.get(),hint.get(),owner);
            slot->mFade.mValue=1.0f;
        }
    }
    for(u32 index=0;index<3;++index) {
        gabi::FrameLocal<sead::SafeText> description(0x7C),hint(0x84);
        setBloomName(description.get(),&sBloomText10161B6C);gabi::cpu->cr[6]=0;
        sead::SafeText* name=formatBloomIndex(colorName.get(),&sBloomText10161B4C,index+1);
        setBloomName(hint.get(),&sBloomText10161817);
        auto* slot=&storage->mColors[index];
        bindBloomNamedParameter(bloomEmbeddedField(&slot->mBinding),name,description.get(),hint.get(),owner);
        copyBloomDefaultColor(bloomEmbeddedField(slot));
    }
    for(u32 index=0;index<3;++index) {
        auto* slot=&storage->mSamples[index];
        const be<u8>* label=sBloomSampleLabels.mLabels[index];
        struct ThreeNamePointers {gptr<const be<u8>> mPointers[3];};
        gabi::FrameLocal<ThreeNamePointers> left(0x214),right(0x220);
        left->mPointers[0]=&sBloomText10161D2C;
        left->mPointers[1]=&sBloomText10161D3C;
        left->mPointers[2]=&sBloomText10161D2C;
        right->mPointers[0]=&sBloomText10161D48;
        right->mPointers[1]=&sBloomText10161D58;
        right->mPointers[2]=&sBloomText10161D48;
        {
            gabi::FrameLocal<sead::SafeText> description(0x3C),hint(140);
            setBloomName(description.get(),&sBloomText10161B74);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(sampleName.get(),&sBloomText10161C0C,label);
            setBloomName(hint.get(),&sBloomText10161814);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mEnabled.mBinding),name,description.get(),hint.get(),owner);
            slot->mEnabled.mValue=0;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x3C),hint(148);
            setBloomName(description.get(),&sBloomText10161B7C);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(sampleName.get(),&sBloomText10161D68,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[0].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[0].mValue=0.0f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x3C),hint(156);
            setBloomName(description.get(),&sBloomText10161B8C);gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(sampleName.get(),&sBloomText10161B84,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[1].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[1].mValue=1000.0f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x3C),hint(164);
            setBloomName(description.get(),left->mPointers[index].get());gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(sampleName.get(),&sBloomText10161D74,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[2].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[2].mValue=1.0f;
        }
        {
            gabi::FrameLocal<sead::SafeText> description(0x3C),hint(172);
            setBloomName(description.get(),right->mPointers[index].get());gabi::cpu->cr[6]=0;
            sead::SafeText* name=formatBloomLabel(sampleName.get(),&sBloomText10161D80,label);
            setBloomName(hint.get(),&sBloomText10161816);
            bindBloomNamedParameter(bloomEmbeddedField(&slot->mScalars[3].mBinding),name,description.get(),hint.get(),owner);
            slot->mScalars[3].mValue=1.0f;
        }
    }
    weights->normalize();
}
static_assert(sizeof(BloomFormattedName)==0x8C);
/* WWHD glue
ext 02779244 sead::SafeText* formatBloomLabel(BloomFormattedName*,const be<u8>*,const be<u8>*)
ext 02779244 sead::SafeText* formatBloomIndex(BloomFormattedName*,const be<u8>*,u32)
data sBloomThresholdLabels 101F8BF8
data sBloomSampleLabels 101F8C00
data sBloomDefaultColor 104A01EC
data sBloomText10161BDC 10161BDC
data sBloomText10161C18 10161C18
data sBloomText10161815 10161815
data sBloomText10161B54 10161B54
data sBloomText10161C24 10161C24
data sBloomText10161816 10161816
data sBloomText10161C30 10161C30
data sBloomText10161C44 10161C44
data sBloomText10161817 10161817
data sBloomText10161C54 10161C54
data sBloomText10161C60 10161C60
data sBloomText10161C6C 10161C6C
data sBloomText10161C80 10161C80
data sBloomText10161814 10161814
data sBloomText10161B94 10161B94
data sBloomText10161C98 10161C98
data sBloomText10161BBC 10161BBC
data sBloomText10161CAC 10161CAC
data sBloomText10161B44 10161B44
data sBloomText10161CC4 10161CC4
data sBloomText10161CD0 10161CD0
data sBloomText10161CE0 10161CE0
data sBloomText10161B5C 10161B5C
data sBloomText10161CEC 10161CEC
data sBloomText10161CFC 10161CFC
data sBloomText10161BE8 10161BE8
data sBloomText10161D10 10161D10
data sBloomText10161BD0 10161BD0
data sBloomText10161B64 10161B64
data sBloomText10161BFC 10161BFC
data sBloomText10161BAC 10161BAC
data sBloomText10161D20 10161D20
data sBloomText10161B6C 10161B6C
data sBloomText10161B4C 10161B4C
data sBloomText10161D2C 10161D2C
data sBloomText10161D3C 10161D3C
data sBloomText10161D48 10161D48
data sBloomText10161D58 10161D58
data sBloomText10161B74 10161B74
data sBloomText10161C0C 10161C0C
data sBloomText10161B7C 10161B7C
data sBloomText10161D68 10161D68
data sBloomText10161B8C 10161B8C
data sBloomText10161B84 10161B84
data sBloomText10161D74 10161D74
data sBloomText10161D80 10161D80

ext 02779888 void initializeBloomEffectBase(void*)
ext 0279A07C void initializeBloomParameterObject(void*,const sead::SafeText*,u32)
ext 027A03A0 void constructBloomBufferState(void*)
ext 02785CB4 void initializeBloomBufferInterface(void*,const sead::SafeText*)
ext 027798F4 void setupBloomEffectBase(void*,void*,u32,void*)
ext 0279E618 void bindBloomNamedParameter(agl::env::ParameterBinding*,const sead::SafeText*,const sead::SafeText*,const sead::SafeText*,void*)
ext 0279CB9C void linkBloomParameterObject(void*,void*,const sead::SafeText*)
ext 027A063C void setupBloomTexture(void*,void*,const sead::SafeText*,void*)
ext 027BE53C void bindBloomTexture(void*,void*,s32,u32)
ext C0006178 void drawBloomIndexed(u32,u32,u32,const void*,u32,u32)
data sBloomBufferedTable 10161E5C
data sBloomBufferedKey 10161DCC
data sBloomBufferKey 10161DD4
data sBloomEnabledName 10161DF0
data sBloomEnabledDescription 10161DF8
data sBloomBlankHint 10161814
data sBloomObjectKey 10161DE0
data sBloomTextureKey 10161DE8
data sBloomDrawContext 101F8DE4
ext 0279AAFC void initializeBloomWorkList(void*,u32,void*)
ext 027A0998 void releaseBloomBuffer(void*)
ext 02752BEC void destroyBloomBufferBase(void*,u32)
ext 027796EC void destroyBloomEffectBase(BloomEffect*,u32)
ext 0273AF40 void releaseBloomStorage(void*)
ext 027791A8 void notifyBloomBase(void*)
ext 028EFFD0 void constructBloomArrayElements(void*,u32,u32,BloomArrayConstructor)
ext 027794E0 void* constructBloomRenderSlot(void*) callback
ext 02779594 void* constructBloomSampleSlot(void*) callback
ext 02779628 void* constructBloomColorSlot(void*) callback
ext 0273AD10 void* allocateBloomStorage(u32)
ext 0279E674 void initializeBloomParameter(agl::env::ParameterBinding*)
data sBloomEffectTable 10161E94
data sBloomEffectSecondaryTable 10161EFC
data sBloomEffectBufferTable 10161898
data sBloomEffectTailTable 101618A8
data sBloomFloatParameterTable 101619A8
data sBloomByteParameterTable 10161930
data sBloomSecondaryFloatParameterTable 10161A20
data sBloomColorParameterTable 10161A98
ext 0279E57C u32 hashBloomParameter(const sead::SafeText*)
ext 0279EB28 void importBloomParameter(agl::env::ParameterBinding*,const gptr<BloomResourceEntry>*)
data sBloomFirstParameterKey 10161B10
data sBloomSecondParameterKey 10161B18
data sBloomThirdParameterKey 10161B24
data sBloomParameterNameTable 10161818
ext 0279A6B0 void resetBloomBase(agl::environment::BloomParameters*,u32)
*/
