// Command-state construction and resource cleanup, reconstructed from this unit's instructions.
#include "agl/ShaderCommandState.h"
using namespace agl::environment;
using gabi::ea;
using InlineConstructor=void* (*)(void*);
using InlineDestructor=void (*)(void*,u32);
void* allocateCommandState(u32 size);
void releaseCommandState(void* storage);
void* allocateProgramCommandStorage(u32 bytes,void* heap,s32 alignment);
void initializeCommandList(CommandList* list);
void prepareCommandList(CommandList* list);
void setCommandListStorage(CommandList* list,void* storage,u32 bytes);
void constructInlineElements(void* array,u32 stride,u32 count,InlineConstructor constructor);
void destroyInlineElements(void* array,s32 count,u32 stride,InlineDestructor destructor,u32 destroyStorage,u32 flags);
void initializeWordList(InlineWordList* list,u32 capacity,be<u32>* storage);
void* constructUniformBindingElement(void* storage);
static sead::SafeTextVtable sShaderNameTable;
static u8 sUnnamedAttribute;
static u8 sUndefinedUniform;
static u8 sUndefinedProgram;
static u8 sResourceCodeMap[4] = {0,1,2,3};
// Guest EA -> typed view (address zero stays null).
template <typename Target>
static Target* shaderGuestPointer(u32 address);
template <typename Field>
static Field* shaderEmbeddedObject(Field* field) {
    const u32 address=ea(field);
    return shaderGuestPointer<Field>(address);
}
template <typename Target>
static Target* shaderRelativePointer(const be<u32>* relativeField) {
    const u32 relative=*relativeField;
    return shaderGuestPointer<Target>(relative ? ea(relativeField)+relative : 0);
}
template <typename Target>
static Target* shaderGuestPointer(u32 address) {
    return gabi::at<Target>(address);
}
template <typename Target>
static Target* shaderRelativePointerAt(u32 relativeFieldAddress) {
    const u32 relative=*shaderGuestPointer<be<u32>>(relativeFieldAddress);
    return shaderGuestPointer<Target>(relative ? relativeFieldAddress+relative : 0);
}
/* 0277376C */
static u32 mapShaderResourceCode(u32 code) {
    if(code<=3) return sResourceCodeMap[code];
    if(code==5) return 5;
    return 4;
}
struct ShaderStateDescriptor { be<u32> mWords[39]; };
struct ShaderStateResource { u8 mPrefix[4]; be<u32> mWords[39]; u8 mGap[0x10]; be<u32> mRelativeA,mRelativeB; };
struct ShaderStateTarget { u8 mPrefix[0x7C]; be<u32> mCodes[4]; };
static_assert(offsetof(ShaderStateResource,mRelativeA)==0xB0);
static_assert(offsetof(ShaderStateResource,mRelativeB)==0xB4);
static_assert(sizeof(ShaderStateDescriptor)==0x9C);
void configureShaderState(ShaderStateTarget*,ShaderStateDescriptor*);
/* 02773798 */
void loadShaderStateResource(ShaderStateTarget* target,const ShaderStateResource* resource) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ShaderStateDescriptor> descriptor(8);
    for(u32 i=0;i<39;++i) descriptor->mWords[i]=(u32)resource->mWords[i];
    descriptor->mWords[9]=gabi::ea(shaderRelativePointer<void>(&resource->mRelativeA));
    descriptor->mWords[11]=gabi::ea(shaderRelativePointer<void>(&resource->mRelativeB));
    configureShaderState(target,descriptor.get());
    const u32 codes=descriptor->mWords[33];
    const u32 first=mapShaderResourceCode(codes>>24);
    const u32 second=mapShaderResourceCode((codes>>16)&255);
    const u32 third=mapShaderResourceCode((codes>>8)&255);
    const u32 fourth=mapShaderResourceCode(codes&255);
    target->mCodes[2]=third;target->mCodes[1]=second;
    target->mCodes[3]=fourth;target->mCodes[0]=first;
}
struct ShaderStateArchive { u8 mPrefix[0x24]; be<u32> mRelativeResources; };
static_assert(offsetof(ShaderStateArchive,mRelativeResources)==0x24);
ShaderStateArchive* resolveShaderStateArchive(void*);
const ShaderStateResource* selectShaderStateResource(void*,const be<u8>*);
/* 02773870 */
void loadShaderStateNamed(ShaderStateTarget* target,void* archive,const be<u8>* name) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    ShaderStateArchive* resolved=resolveShaderStateArchive(archive);
    void* resources=shaderRelativePointer<void>(&resolved->mRelativeResources);
    const ShaderStateResource* resource=selectShaderStateResource(resources,name);
    loadShaderStateResource(target,resource);
}
/* 027738DC */
void DisplayCommandState::reset() {
    mActive=0;
    for(u32 slot=0;slot<16;++slot) mSlots[slot]=nullptr;
}
/* 0277390C */
DisplayCommandState* DisplayCommandState::construct(DisplayCommandState* state) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(state==nullptr) state=static_cast<DisplayCommandState*>(allocateCommandState(sizeof(DisplayCommandState)));
    if(state!=nullptr) {
        if(gabi::ea(&state->mSlots[0])==0) allocateCommandState(64);
        initializeCommandList(&state->mList);
        state->reset();
        state->mList.mCount=16;
        prepareCommandList(&state->mList);
    }
    return state;
}
/* 0277397C */
void DisplayCommandState::setStorage(void* storage,u32 bytes) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    setCommandListStorage(&mList,storage,bytes);
}
/* 02773E90 */
PixelCommandState* PixelCommandState::construct(PixelCommandState* state) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(state==nullptr) state=static_cast<PixelCommandState*>(allocateCommandState(sizeof(PixelCommandState)));
    if(state!=nullptr) {
        constructInlineElements(state,16,16,constructUniformBindingElement);
        InlineWordList* list=shaderEmbeddedObject(&state->mActiveBindings);
        if(list==nullptr) list=static_cast<InlineWordList*>(allocateCommandState(sizeof(InlineWordList)));
        if(list!=nullptr) {
            list->mEntries=nullptr;list->mCount=0;list->mCapacity=0;
            initializeWordList(list,16,list->mStorage);
        }
    }
    return state;
}
/* 02774954 */
static void destroyUniformBinding(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseCommandState(storage);
}
/* 02773F30 */
static void destroyPixelCommandState(PixelCommandState* state,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(state!=nullptr) {
        destroyInlineElements(state,16,16,destroyUniformBinding,0,0);
        if(flags&1) releaseCommandState(state);
    }
}
/* 02773F98 */
void PixelCommandState::reset() { mActiveBindings.mCount=0;mActive=0; }
/* 027742F0 */
void ProgramInstance::reset() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    mDisplay.reset();mPixel.reset();
}
/* 02774320 */
ProgramInstance* ProgramInstance::construct(ProgramInstance* instance) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(instance==nullptr) instance=static_cast<ProgramInstance*>(allocateCommandState(sizeof(ProgramInstance)));
    if(instance!=nullptr) {
        instance->mProgram=nullptr;instance->mResource=nullptr;
        sead::SafeText* name=shaderEmbeddedObject(&instance->mName);
        if(name==nullptr) name=static_cast<sead::SafeText*>(allocateCommandState(sizeof(sead::SafeText)));
        if(name!=nullptr) { name->mVtable=&sShaderNameTable;name->mText=&sUndefinedProgram; }
        instance->mVertexIndex=-1;
        instance->mGeometryIndex=-1;
        instance->mPixelIndex=-1;
        instance->mPassIndex=0xFFFF;
        DisplayCommandState::construct(shaderEmbeddedObject(&instance->mDisplay));
        PixelCommandState::construct(shaderEmbeddedObject(&instance->mPixel));
        instance->mStorage=nullptr;
        instance->reset();
    }
    return instance;
}
/* 027743DC */
static void destroyProgramInstance(ProgramInstance* instance,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(instance!=nullptr) {
        void* storage=instance->mStorage;
        if(storage!=nullptr) { releaseCommandState(storage);instance->mStorage=nullptr; }
        destroyPixelCommandState(&instance->mPixel,2);
        if(flags&1) releaseCommandState(instance);
    }
}
/* 02774448 */
u32 ProgramInstance::storageBytes() { return 0x110; }
/* 02774450 */
void ProgramInstance::setStorage(void* storage) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    mDisplay.setStorage(storage,0x110);
}
/* 0277445C */
void ProgramInstance::initializeStorage(void* heap) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    void* storage=allocateProgramCommandStorage(storageBytes(),heap,256);
    mStorage=storage;
    setStorage(storage);
}
/* 0277482C */
static void destroyVertexAttribute(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseCommandState(storage);
}
/* 02774840 */
static void emptyShaderNameCallback() {}
/* 02774844 */
VertexAttribute* VertexAttribute::construct(VertexAttribute* attribute) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(attribute==nullptr) attribute=static_cast<VertexAttribute*>(allocateCommandState(sizeof(VertexAttribute)));
    if(attribute!=nullptr) {
        attribute->mLocation=0;
        sead::SafeText* name=shaderEmbeddedObject(&attribute->mName);
        if(name==nullptr) name=static_cast<sead::SafeText*>(allocateCommandState(sizeof(sead::SafeText)));
        if(name!=nullptr) { name->mVtable=&sShaderNameTable;name->mText=&sUnnamedAttribute; }
        attribute->mStream=0;
    }
    return attribute;
}
/* 027748C8 */
UniformBinding* UniformBinding::construct(UniformBinding* binding) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(binding==nullptr) binding=static_cast<UniformBinding*>(allocateCommandState(sizeof(UniformBinding)));
    if(binding!=nullptr) {
        binding->mName.mVtable=&sShaderNameTable;
        binding->mName.mText=&sUndefinedUniform;
        binding->mIndices[0]=-1;
        binding->mIndices[3]=-1;
        binding->mIndices[2]=-1;
        binding->mIndices[1]=-1;
    }
    return binding;
}
/* 02774968 */
static void destroyShaderNameStorage(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseCommandState(storage);
}
/* 02773668 */
static void destroyShaderLookupStorage(void* storage,u32 flags) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(storage!=nullptr && (flags&1)) releaseCommandState(storage);
}
/* 0277367C */
static void emptyShaderLookupCallback() {}
void finishCommandList(CommandList*);
void bindVertexStream(void*,u32,u32);
/* 02773E1C */
void DisplayCommandState::bindStreams() {
    auto frame=gabi::AutoFrame(gabi::current_function());
    finishCommandList(&mList);
    u32 stream=0;
    while(stream<(u8)mActive) {
        bindVertexStream(mSlots[stream].get(),stream,0);
        ++stream;
    }
}
/* 02773FA8 */
void PixelCommandState::append(u32 key,const UniformBinding* binding) {
    const s32 first=binding->mIndices[0];
    if(first==-1&&(s16)binding->mIndices[1]==-1&&(s16)binding->mIndices[2]==-1) return;
    const u32 count=mActiveBindings.mCount;
    u32 entry=gabi::ea(mActiveBindings.mEntries.get());
    const u32 end=entry+count*4;u32 bindingOffset=0;
    while(entry!=end) {
        auto* keyEntry=shaderGuestPointer<be<u32>>(entry);
        if((u32)*keyEntry==key) {
            auto* current=shaderGuestPointer<UniformBinding>(gabi::ea(this)+bindingOffset);
            if((s16)current->mIndices[0]==first&&
               (s16)current->mIndices[1]==(s16)binding->mIndices[1]&&
               (s16)current->mIndices[2]==(s16)binding->mIndices[2]) return;
        }
        entry+=4;bindingOffset+=0x10;
    }
    auto* destination=shaderGuestPointer<UniformBinding>(gabi::ea(this)+count*sizeof(UniformBinding));
    destination->mName.mText=binding->mName.mText.get();
    destination->mIndices[0]=(s16)binding->mIndices[0];
    destination->mIndices[1]=(s16)binding->mIndices[1];
    destination->mIndices[2]=(s16)binding->mIndices[2];
    destination->mIndices[3]=(s16)binding->mIndices[3];
    const s32 capacity=(u32)mActiveBindings.mCapacity;
    const u32 reloadCount=mActiveBindings.mCount;
    if((s32)reloadCount>=capacity) return;
    *shaderGuestPointer<be<u32>>(gabi::ea(mActiveBindings.mEntries.get())+reloadCount*sizeof(be<u32>))=key;
    mActiveBindings.mCount=(u32)mActiveBindings.mCount+1;
}
struct SamplerKey { u8 mBeforeSlot[0x14]; be<u8> mSlot; };
struct SamplerCommandObject { gptr<void> mVtable,mCommands; };
struct SamplerOwner { u8 mPrefix[0x2C]; gptr<gptr<void>> mSamplers; };
static_assert(offsetof(SamplerCommandObject,mCommands)==4);
// GX2SetVertexTexture / GX2SetGeometryTexture / GX2SetPixelTexture (texture, unit) through their 027E150C.. wrappers.
void setVertexTexture(void*,s32);void setGeometryTexture(void*,s32);void setPixelTexture(void*,s32);
// GX2SetVertexSampler / GX2SetGeometrySampler / GX2SetPixelSampler (sampler, location) through their 027E1468.. wrappers.
void bindVertexSampler(SamplerKey*,s32);void bindGeometrySampler(SamplerKey*,s32);void bindPixelSampler(SamplerKey*,s32);
/* 02774210 */
void PixelCommandState::bindSamplerStages(void* ownerAddress) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* owner=static_cast<SamplerOwner*>(ownerAddress);
    u32 entry=gabi::ea(mActiveBindings.mEntries.get());
    const u32 end=entry+(u32)mActiveBindings.mCount*4;u32 bindingOffset=0;
    while(entry!=end) {
        auto* keyAddress=shaderGuestPointer<be<u32>>(entry);
        auto* key=shaderGuestPointer<SamplerKey>(*keyAddress);
        const u32 slot=key->mSlot;
        auto* samplerSlot=shaderGuestPointer<gptr<void>>(gabi::ea(owner->mSamplers.get())+slot*sizeof(gptr<void>));
        void* sampler=samplerSlot->get();
        if(sampler!=nullptr) {
            auto* binding=shaderGuestPointer<UniformBinding>(gabi::ea(this)+bindingOffset);
            auto* samplerObject=shaderGuestPointer<SamplerCommandObject>(gabi::ea(sampler));
            void* texture=&samplerObject->mCommands;
            const s32 vertexUnit=(s16)binding->mIndices[0];
            if(vertexUnit!=-1) {
                setVertexTexture(texture,vertexUnit);bindVertexSampler(key,(s16)binding->mIndices[0]);
            }
            const s32 geometryUnit=(s16)binding->mIndices[2];
            if(geometryUnit!=-1) {
                setGeometryTexture(texture,geometryUnit);bindGeometrySampler(key,(s16)binding->mIndices[2]);
            }
            const s32 pixelUnit=(s16)binding->mIndices[1];
            if(pixelUnit!=-1) {
                setPixelTexture(texture,pixelUnit);bindPixelSampler(key,(s16)binding->mIndices[1]);
            }
            bindingOffset+=16;
        }
        entry+=4;
    }
}
struct SamplerSelectionDescriptor { u8 mPrefix[0x24]; be<u32> mRelativeOverrides; u8 mBetween[8]; be<u32> mRelativeKeys; };
static_assert(offsetof(SamplerSelectionDescriptor,mRelativeOverrides)==0x24);
static_assert(offsetof(SamplerSelectionDescriptor,mRelativeKeys)==0x30);
struct SamplerMaterial { u8 mPrefix[2]; be<u16> mPass; u8 mBeforeArchive[0x78]; gptr<void> mArchive; };
struct SamplerSelectionTable { be<u32> mWord,mCount; be<u8> mEntries[1]; };
static_assert(offsetof(SamplerSelectionTable,mEntries)==8);
struct SamplerArchive { u8 mBeforeSamplerSelections[0x3C]; gptr<SamplerSelectionTable> mSamplerSelections,mAttributeSelections; };
static_assert(offsetof(SamplerArchive,mSamplerSelections)==0x3C);
static_assert(offsetof(SamplerArchive,mAttributeSelections)==0x40);
struct SamplerSelectionEntry { be<u32> mSize,mWord,mRelativeName,mNameOffset,mEnabledOffset; u8 mBeforeNameData[4]; be<u8> mNameData[1]; };
static_assert(offsetof(SamplerSelectionEntry,mSize)==0);
static_assert(offsetof(SamplerSelectionEntry,mNameData)==0x18);
struct SamplerOverrideBlock { u8 mBeforeAttributeDictionary[0x10]; be<u32> mRelativeAttributeDictionary,mRelativeSamplerDictionary; };
static_assert(offsetof(SamplerOverrideBlock,mRelativeAttributeDictionary)==0x10);
static_assert(offsetof(SamplerOverrideBlock,mRelativeSamplerDictionary)==0x14);
static const be<u8>* samplerSelectionName(const SamplerSelectionEntry* entry) {
    return shaderGuestPointer<const be<u8>>(ea(&entry->mNameData[0])+(u32)entry->mRelativeName);
}
static sead::SafeTextVtable sSelectedSamplerNameTable;
void* findSamplerSelection(void*,const be<u8>*);
void resolveSamplerIndices(UniformBinding*,SamplerMaterial*);
/* 0277408C */
void PixelCommandState::selectSamplerBindings(void* descriptorAddress,void* materialAddress,u32 useOverrides,u32 includeDefaults) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* descriptor=static_cast<SamplerSelectionDescriptor*>(descriptorAddress);
    auto* material=static_cast<SamplerMaterial*>(materialAddress);
    u32 overrides=0;
    if(useOverrides!=0) {
        overrides=gabi::ea(shaderRelativePointer<void>(&descriptor->mRelativeOverrides));
    }
    const u32 archive=gabi::ea(material->mArchive.get());
    auto* samplerTableField=shaderGuestPointer<gptr<SamplerSelectionTable>>(archive+offsetof(SamplerArchive,mSamplerSelections));
    auto* table=samplerTableField->get();
    const u32 count=table->mCount;u32 entryAddress=ea(&table->mEntries[0]);
    for(u32 index=0;index<count;++index) {
        auto* entry=shaderGuestPointer<SamplerSelectionEntry>(entryAddress);
        if(entryAddress!=0) {
            const be<u8>* entryName=samplerSelectionName(entry);
            const u32 name=ea(entryName);
            const u32 enabled=name+(u32)entry->mNameOffset+(u32)entry->mEnabledOffset+(u16)material->mPass;
            if((u8)*shaderGuestPointer<be<u8>>(enabled)!=0) {
                const be<u8>* selectedName=nullptr;
                if(overrides!=0) {
                    auto* overrideBlock=shaderGuestPointer<SamplerOverrideBlock>(overrides);
                    void* dictionary=shaderRelativePointer<void>(&overrideBlock->mRelativeSamplerDictionary);
                    selectedName=static_cast<const be<u8>*>(findSamplerSelection(dictionary,entryName));
                }
                if(selectedName==nullptr&&includeDefaults!=0) {
                    selectedName=samplerSelectionName(entry);
                }
                if(selectedName!=nullptr) {
                    void* dictionary=shaderRelativePointer<void>(&descriptor->mRelativeKeys);
                    void* key=findSamplerSelection(dictionary,selectedName);
                    if(key!=nullptr) {
                        gabi::FrameLocal<UniformBinding> binding(8);
                        binding->mIndices[0]=-1;binding->mIndices[1]=-1;binding->mIndices[2]=-1;
                        binding->mName.mVtable=&sSelectedSamplerNameTable;
                        binding->mName.mText=shaderEmbeddedObject(&entry->mNameData[0]);binding->mIndices[3]=-1;
                        resolveSamplerIndices(binding.get(),material);
                        append(gabi::ea(key),binding.get());
                    }
                }
            }
        }
        entryAddress+=(u32)entry->mSize;
    }
}
struct ProgramNameDescriptor { u8 mPrefix[0x18]; be<s8> mSelected; u8 mBeforeNames[0x37]; be<u32> mRelativeNames; };
struct ProgramNameResource { u8 mPrefix[0x10]; be<u32> mRelativeStageIndices; };
struct ProgramNameEntry { be<u32> mRelativeText; u8 mTail[0xC]; };
struct ProgramNameTable { u8 mBeforeEntries[0x20]; ProgramNameEntry mEntries[1]; };
struct ProgramStageIndices { be<s8> mVertex,mGeometry,mPixel; };
static_assert(offsetof(ProgramNameResource,mRelativeStageIndices)==0x10);
static_assert(offsetof(ProgramNameTable,mEntries)==0x20);
static_assert(sizeof(ProgramNameEntry)==0x10);
static_assert(offsetof(ProgramStageIndices,mPixel)==2);
void resolveProgramName(sead::SafeText*,void*);
void prepareProgramName(sead::SafeText*);
s32 findProgramNameIndex(void*,const be<u8>*);
/* 027744A0 */
void ProgramInstance::selectProgramName(void*,void* descriptorAddress,const be<u8>* name) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    if(mProgram.get()!=nullptr) {
        void* program=mProgram.get();mName.mText=const_cast<be<u8>*>(name);resolveProgramName(&mName,program);return;
    }
    void* resource=mResource.get();
    if(resource==nullptr) return;
    auto* descriptor=static_cast<ProgramNameDescriptor*>(descriptorAddress);
    const s32 selected=descriptor->mSelected;
    if(selected==-1) return;
    const u32 names=gabi::ea(shaderRelativePointer<void>(&descriptor->mRelativeNames));
    const u32 nameSlotAddress=names+(u32)selected*sizeof(ProgramNameEntry)+offsetof(ProgramNameTable,mEntries);
    auto* nameSlot=shaderGuestPointer<ProgramNameEntry>(nameSlotAddress);
    const u32 nameRelative=nameSlot->mRelativeText;
    const be<u8>* selectedText=shaderGuestPointer<const be<u8>>(nameRelative?gabi::ea(nameSlot)+nameRelative:0);
    gabi::FrameLocal<sead::SafeText> selectedName(8);
    selectedName->mText=const_cast<be<u8>*>(selectedText);
    selectedName->mVtable=&sSelectedSamplerNameTable;
    prepareProgramName(selectedName.get());
    void* dictionary=shaderRelativePointer<void>(&descriptor->mRelativeNames);
    const s32 index=findProgramNameIndex(dictionary,selectedName->mText.get());
    s32 vertex=-1,geometry=-1,pixel=-1;
    if(index!=-1) {
        auto* resourceView=static_cast<ProgramNameResource*>(resource);
        auto* indices=shaderRelativePointer<ProgramStageIndices>(&resourceView->mRelativeStageIndices);
        auto* entry=shaderGuestPointer<ProgramStageIndices>(ea(indices)+(u32)index*sizeof(ProgramStageIndices));
        vertex=(s8)entry->mVertex;geometry=(s8)entry->mGeometry;pixel=(s8)entry->mPixel;
    }
    mPixelIndex=pixel;mName.mText=selectedName->mText.get();mPassIndex=0xFFFF;
    mVertexIndex=vertex;mGeometryIndex=geometry;
}
static_assert(offsetof(ProgramNameDescriptor,mRelativeNames)==0x50);
void buildDisplayBindings(DisplayCommandState*,void*,void*,void*,u32,u32);
/* 0277462C */
void ProgramInstance::selectOverride(void* descriptorAddress,void* selection,void* material,const be<u8>* name) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* descriptor=static_cast<SamplerSelectionDescriptor*>(descriptorAddress);
    const u32 overrides=gabi::ea(shaderRelativePointer<void>(&descriptor->mRelativeOverrides));
    reset();
    if(overrides!=0&&material!=nullptr) {
        mProgram=material;mResource=nullptr;
        mPixel.selectSamplerBindings(descriptor,material,1,0);
        buildDisplayBindings(&mDisplay,descriptor,selection,material,1,0);
        selectProgramName(descriptor,nullptr,name);
    }
}
/* 027746E4 */
void ProgramInstance::selectDefault(void* descriptor,void* selection,void* material,const be<u8>* name) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    reset();
    if(material!=nullptr) {
        mProgram=material;mResource=nullptr;
        mPixel.selectSamplerBindings(descriptor,material,0,1);
        buildDisplayBindings(&mDisplay,descriptor,selection,material,0,1);
        selectProgramName(descriptor,nullptr,name);
    }
}
struct UniformBlockDescriptor { u8 mBeforeParameters[0x14]; gptr<void> mParameters; be<u32> mParameterCount; };
static_assert(offsetof(UniformBlockDescriptor,mParameters)==0x14);
static_assert(offsetof(UniformBlockDescriptor,mParameterCount)==0x18);
void bindVertexUniformBlock(void*,s32,u32);void bindGeometryUniformBlock(void*,s32,u32);void bindPixelUniformBlock(void*,s32,u32);
/* 02774778 */
void ProgramInstance::bindUniformBlocks(void* block,u32 bindingParameter) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    const s32 vertex=mVertexIndex;
    if(vertex==-1&&(s16)mPixelIndex==-1&&(s16)mGeometryIndex==-1) return;
    auto* descriptor=static_cast<UniformBlockDescriptor*>(block);
    if((u32)descriptor->mParameterCount==0) return;
    void* parameters=&descriptor->mParameters;
    if(vertex!=-1) bindVertexUniformBlock(parameters,vertex,bindingParameter);
    const s32 geometry=mGeometryIndex;
    if(geometry!=-1) bindGeometryUniformBlock(parameters,geometry,bindingParameter);
    const s32 pixel=mPixelIndex;
    if(pixel!=-1) bindPixelUniformBlock(parameters,pixel,bindingParameter);
}
struct VertexResourceHeader { u8 mPrefix[0x10]; be<u32> mRelativeAttributes,mRelativeNames,mRelativeBuffers; };
static_assert(offsetof(VertexResourceHeader,mRelativeAttributes)==0x10);
static_assert(offsetof(VertexResourceHeader,mRelativeNames)==0x14);
static_assert(offsetof(VertexResourceHeader,mRelativeBuffers)==0x18);
struct VertexResourceAttribute { be<u32> mWord; be<u8> mStream,mReserved; be<u16> mFormat; be<u32> mOffset; };
static_assert(sizeof(VertexResourceAttribute)==12);
struct VertexResourceBuffer { u8 mBytes[24]; };
static_assert(sizeof(VertexResourceBuffer)==24);
struct LocalVertexAttributes { VertexAttribute mEntries[16]; };
struct LocalStreamSlots { be<u32> mSlots[16]; };
void* constructVertexAttributeCallback(void*);
void destroyVertexAttributeNameCallback(void*,u32);
void beginAttributeCommands(CommandList*);void finishAttributeCommands(CommandList*);void flushAttributeCommands(CommandList*);
void selectAttributeCommandStorage(CommandList*,void*);
void setAttributeLocation(CommandList*,void*,u32,s32);
void setAttributeBuffer(CommandList*,u32,VertexResourceBuffer*);
void setAttributeStream(CommandList*,void*,u32,u32);
void setAttributeOffset(CommandList*,void*,u32,u32);
void setAttributeFormat(CommandList*,void*,u32,u32);
static void* attributeCommandStorage(CommandList* list) {
    return list->mStorage.get();
}
/* 02773984 */
void DisplayCommandState::buildAttributes(void* resourceAddress,const VertexAttribute* attributes,s32 count) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* resource=static_cast<VertexResourceHeader*>(resourceAddress);
    gabi::FrameLocal<LocalVertexAttributes> compact(0x48);
    gabi::FrameLocal<LocalStreamSlots> streamSlots(8);
    mActive=0;constructInlineElements(compact.get(),16,16,constructVertexAttributeCallback);
    u32 compactCount=0;
    for(s32 input=0;input<count;++input) {
        const u32 slot=(u32)input<16?(u32)input:0;
        streamSlots->mSlots[slot]=0xFFFFFFFF;
        auto* source=shaderGuestPointer<VertexAttribute>(gabi::ea(attributes)+(u32)input*16);
        if((s16)source->mStream!=-1&&(u32)source->mLocation!=0xFFFFFFFF) {
            auto* destination=shaderGuestPointer<VertexAttribute>(gabi::ea(compact.get())+compactCount*16);
            destination->mLocation=(u32)source->mLocation;
            destination->mName.mText=source->mName.mText.get();
            destination->mStream=(u16)source->mStream;++compactCount;
        }
    }
    beginAttributeCommands(&mList);mList.mCount=compactCount;prepareCommandList(&mList);
    selectAttributeCommandStorage(&mList,attributeCommandStorage(&mList));
    for(u32 output=0;output<compactCount;++output) {
        const u32 attributeBase=gabi::ea(shaderRelativePointer<void>(&resource->mRelativeAttributes));
        auto* source=shaderGuestPointer<VertexAttribute>(gabi::ea(compact.get())+output*16);
        auto* attribute=shaderGuestPointer<VertexResourceAttribute>(attributeBase+(u32)source->mLocation*12);
        const u32 bufferBase=gabi::ea(shaderRelativePointer<void>(&resource->mRelativeBuffers));
        u32 stream=attribute->mStream;
        auto* buffer=shaderGuestPointer<VertexResourceBuffer>(bufferBase+stream*24);
        const u32 slot=stream<16?stream:0;
        if((u32)streamSlots->mSlots[slot]==0xFFFFFFFF) {
            const u32 active=mActive;streamSlots->mSlots[slot]=active;
            stream=attribute->mStream;
            const u32 reloadedBuffer=gabi::ea(shaderRelativePointer<void>(&resource->mRelativeBuffers));
            mSlots[active].v=reloadedBuffer+stream*24;
            mActive=(u8)((u8)mActive+1);
        }
        setAttributeLocation(&mList,attributeCommandStorage(&mList),output,(s16)source->mStream);
        setAttributeBuffer(&mList,output,buffer);
        stream=attribute->mStream;
        setAttributeStream(&mList,attributeCommandStorage(&mList),output,streamSlots->mSlots[stream<16?stream:0]);
        setAttributeOffset(&mList,attributeCommandStorage(&mList),output,attribute->mOffset);
        setAttributeFormat(&mList,attributeCommandStorage(&mList),output,attribute->mFormat);
    }
    finishAttributeCommands(&mList);flushAttributeCommands(&mList);
    destroyInlineElements(compact.get(),16,16,destroyVertexAttributeNameCallback,0,0);
}
struct AttributeSelection { sead::SafeText mName; be<s16> mStream; be<u16> mReserved; };
struct VertexResourceSelection { u8 mPrefix[0x20]; be<u32> mRelativeResource; };
static_assert(offsetof(VertexResourceSelection,mRelativeResource)==0x20);
void resolveAttributeSelection(AttributeSelection*,SamplerMaterial*);
/* 02773C0C */
void buildDisplayBindings(DisplayCommandState* state,void* descriptorAddress,void* selectionAddress,
                          void* materialAddress,u32 useOverrides,u32 includeDefaults) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    auto* descriptor=static_cast<SamplerSelectionDescriptor*>(descriptorAddress);
    auto* selection=static_cast<VertexResourceSelection*>(selectionAddress);
    auto* material=static_cast<SamplerMaterial*>(materialAddress);
    u32 overrides=0;
    if(useOverrides!=0) {
        overrides=gabi::ea(shaderRelativePointer<void>(&descriptor->mRelativeOverrides));
    }
    gabi::FrameLocal<LocalVertexAttributes> attributes(0x14);
    constructInlineElements(attributes.get(),16,16,constructVertexAttributeCallback);
    const u32 archive=gabi::ea(material->mArchive.get());
    auto* attributeTableField=shaderGuestPointer<gptr<SamplerSelectionTable>>(archive+offsetof(SamplerArchive,mAttributeSelections));
    auto* table=attributeTableField->get();
    const u32 count=table->mCount;u32 entryAddress=ea(&table->mEntries[0]),selectedCount=0;
    for(u32 index=0;index<count;++index) {
        auto* entry=shaderGuestPointer<SamplerSelectionEntry>(entryAddress);
        if(entryAddress!=0) {
            const be<u8>* entryName=samplerSelectionName(entry);
            const u32 name=ea(entryName);
            const u32 enabled=name+(u32)entry->mNameOffset+(u32)entry->mEnabledOffset+(u16)material->mPass;
            if((u8)*shaderGuestPointer<be<u8>>(enabled)!=0) {
                const be<u8>* selectedName=nullptr;
                if(overrides!=0) {
                    auto* overrideBlock=shaderGuestPointer<SamplerOverrideBlock>(overrides);
                    void* dictionary=shaderRelativePointer<void>(&overrideBlock->mRelativeAttributeDictionary);
                    selectedName=static_cast<const be<u8>*>(findSamplerSelection(dictionary,entryName));
                }
                if(selectedName==nullptr&&includeDefaults!=0) selectedName=samplerSelectionName(entry);
                if(selectedName!=nullptr) {
                    gabi::FrameLocal<AttributeSelection> selected(8);
                    selected->mName.mVtable=&sSelectedSamplerNameTable;
                    selected->mName.mText=shaderEmbeddedObject(&entry->mNameData[0]);
                    resolveAttributeSelection(selected.get(),material);
                    auto* output=shaderGuestPointer<VertexAttribute>(gabi::ea(attributes.get())+selectedCount*16);
                    output->mName.mText=selected->mName.mText.get();output->mStream=(s16)selected->mStream;
                    const u32 resource=gabi::ea(shaderRelativePointer<void>(&selection->mRelativeResource));
                    void* names=shaderRelativePointerAt<void>(resource+offsetof(VertexResourceHeader,mRelativeNames));
                    output->mLocation=findProgramNameIndex(names,selectedName);
                    ++selectedCount;
                }
            }
        }
        entryAddress+=(u32)entry->mSize;
    }
    void* resource=shaderRelativePointer<void>(&selection->mRelativeResource);
    state->buildAttributes(resource,shaderGuestPointer<VertexAttribute>(gabi::ea(attributes.get())),selectedCount);
    destroyInlineElements(attributes.get(),16,16,destroyVertexAttributeNameCallback,0,0);
}
/* WWHD glue
ext 027BDD2C void resolveAttributeSelection(AttributeSelection*,SamplerMaterial*)
ext 02774844 void* constructVertexAttributeCallback(void*) callback
ext 02774968 void destroyVertexAttributeNameCallback(void*,u32) callback
ext 027E39F0 void beginAttributeCommands(agl::environment::CommandList*)
ext 027E39EC void finishAttributeCommands(agl::environment::CommandList*)
ext 027E15A4 void flushAttributeCommands(agl::environment::CommandList*)
ext 027E1648 void selectAttributeCommandStorage(agl::environment::CommandList*,void*)
ext 027E1784 void setAttributeLocation(agl::environment::CommandList*,void*,u32,s32)
ext 027E1618 void setAttributeBuffer(agl::environment::CommandList*,u32,VertexResourceBuffer*)
ext 027E17B4 void setAttributeStream(agl::environment::CommandList*,void*,u32,u32)
ext 027E17E0 void setAttributeOffset(agl::environment::CommandList*,void*,u32,u32)
ext 027E1878 void setAttributeFormat(agl::environment::CommandList*,void*,u32,u32)
ext 027E13B4 void bindVertexUniformBlock(void*,s32,u32)
ext 027E13F0 void bindGeometryUniformBlock(void*,s32,u32)
ext 027E142C void bindPixelUniformBlock(void*,s32,u32)
ext 027BDA2C void resolveProgramName(sead::SafeText*,void*)
ext 02774840 void prepareProgramName(sead::SafeText*)
ext 027DF9B0 s32 findProgramNameIndex(void*,const be<u8>*)
ext 027DFA24 void* findSamplerSelection(void*,const be<u8>*)
ext 027BD72C void resolveSamplerIndices(agl::environment::UniformBinding*,SamplerMaterial*)
data sSelectedSamplerNameTable 101617C0
ext 027E150C void setVertexTexture(void*,s32)
ext 027E1510 void setGeometryTexture(void*,s32)
ext 027E1514 void setPixelTexture(void*,s32)
ext 027E1468 void bindVertexSampler(SamplerKey*,s32)
ext 027E146C void bindGeometrySampler(SamplerKey*,s32)
ext 027E1470 void bindPixelSampler(SamplerKey*,s32)
ext 027E15F8 void finishCommandList(agl::environment::CommandList*)
ext 027E1374 void bindVertexStream(void*,u32,u32)
ext 027E2DC0 ShaderStateArchive* resolveShaderStateArchive(void*)
ext 027DFA24 const ShaderStateResource* selectShaderStateResource(void*,const be<u8>*)
ext 02786360 void configureShaderState(ShaderStateTarget*,ShaderStateDescriptor*)
ext 0273AD10 void* allocateCommandState(u32)
ext 0273AF40 void releaseCommandState(void*)
ext 0273B0D4 void* allocateProgramCommandStorage(u32,void*,s32)
ext 027E1550 void initializeCommandList(agl::environment::CommandList*)
ext 027E15D0 void prepareCommandList(agl::environment::CommandList*)
ext 027E15FC void setCommandListStorage(agl::environment::CommandList*,void*,u32)
ext 028EFFD0 void constructInlineElements(void*,u32,u32,InlineConstructor)
ext 028F0164 void destroyInlineElements(void*,s32,u32,InlineDestructor,u32,u32)
ext 0273B560 void initializeWordList(agl::environment::InlineWordList*,u32,be<u32>*)
ext 027748C8 void* constructUniformBindingElement(void*) callback
data sShaderNameTable 101617C0
data sUnnamedAttribute 101617D8
data sUndefinedUniform 101617E4
data sUndefinedProgram 101617F0
data sResourceCodeMap 101617BC
*/
