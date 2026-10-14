/** Misc render resources: environment blocks and shader name collections for HD layers. */
#include "hd/hd_render_misc.h"
static u8 sMiscFixedNameVtable;
static u8 sMiscBufferedNameVtable;
static u8 sMiscGroupNameVtable;
static u8 sMiscProgramNameVtable;
static u8 sMiscResourceVtable;
static u8 sMiscSafeStringVtable;
static hdRenderResources_c* sMiscResources;
static hdRenderResources_c* sMiscRegisteredResources;
static void* sMiscResourceManager;
static u32 sMiscParticleNamesReady;
static u32 sMiscPostNamesReady;
struct hdMiscParticleNames_l { SafeString mNames[4]; };
struct hdMiscPostNames_l { SafeString mNames[7]; };
static hdMiscParticleNames_l sMiscParticleNames;
static hdMiscPostNames_l sMiscPostNames;
static u8 sMiscParticleNamesDescriptor;
static u8 sMiscPostNamesDescriptor;
struct hdMiscSafeStringVtable_l { u8 mReserved[0x14]; be<u32> mAssureTermination; };
struct hdMiscRelativePointer_l {
    be<s32> mDisplacement;
    void* get() {
        s32 displacement = mDisplacement;
        return displacement != 0 ? gabi::at<void>(gabi::ea(this) + displacement) : NULL;
    }
};
struct hdMiscResourceFile_l { u8 mHeader[0x4C]; hdMiscRelativePointer_l mDictionary; };
static void* loadMiscFile(void*, SafeString*, SafeString*, u32);
static hdMiscResourceFile_l* getMiscResourceFile(void*);
static hdMiscRelativePointer_l* findMiscEnvironment(void*, const char*);
static void* allocateMiscResources(u32, void*, u32);
static void constructMiscDisposer(hdRenderResources_c*, void*, u32);
static void destroyMiscName(void*, s32);
static void destroyMiscNameArray(void*, u32, u32, u32, s32, s32);

/* 027275E0 */
static hdRenderShaderNames_c* hdRenderShaderNames_construct(hdRenderShaderNames_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdRenderShaderNames_c*)operator_new(sizeof(hdRenderShaderNames_c));
        if (i_this == NULL)
            return NULL;
    }
    hdFixedShaderName_l* group = gabi::at<hdFixedShaderName_l>(
        gabi::ea(i_this) + offsetof(hdRenderShaderNames_c, mGroupName));
    if (group == NULL)
        group = (hdFixedShaderName_l*)operator_new(sizeof(hdFixedShaderName_l));
    if (group != NULL) {
        group->mpText = group->mBuffer;
        group->mVtable = gabi::ea(&sMiscFixedNameVtable);
        group->mCapacity = 8;
        group->mBuffer[7] = 0;
        be<u8>* text = group->mpText;
        group->mVtable = gabi::ea(&sMiscBufferedNameVtable);
        *text = 0;
        group->mVtable = gabi::ea(&sMiscGroupNameVtable);
    }
    hdFixedWideShaderName_l* program = gabi::at<hdFixedWideShaderName_l>(
        gabi::ea(i_this) + offsetof(hdRenderShaderNames_c, mProgramName));
    if (program == NULL)
        program = (hdFixedWideShaderName_l*)operator_new(sizeof(hdFixedWideShaderName_l));
    if (program != NULL) {
        program->mBuffer[8] = 0;
        program->mpText = program->mBuffer;
        program->mBuffer[0] = 0;
        program->mVtable = gabi::ea(&sMiscProgramNameVtable);
        program->mCapacity = 9;
    }
    return i_this;
}

/* 027276E0 */
static void destroyMiscParticleNames() {
    destroyMiscNameArray(&sMiscParticleNames, 4, sizeof(SafeString),
                         gabi::guest_fn((const void*)destroyMiscName), 0, 0);
}

/* 02727704 */
static void destroyMiscPostNames() {
    destroyMiscNameArray(&sMiscPostNames, 7, sizeof(SafeString),
                         gabi::guest_fn((const void*)destroyMiscName), 0, 0);
}

/* 02727728 */
static hdRenderResources_c* hdRenderResources_construct(hdRenderResources_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdRenderResources_c*)operator_new(sizeof(hdRenderResources_c));
        if (i_this == NULL)
            return NULL;
    }
    i_this->mpPostEnvironment = NULL;
    i_this->mpShadowEnvironment = NULL;
    i_this->mpSecondaryEnvironment = NULL;
    i_this->mpMainEnvironment = NULL;
    const u32 particleAddress =
        gabi::ea(i_this) + offsetof(hdRenderResources_c, mpParticleEnvironments);
    const u32 postAddress = gabi::ea(i_this) + offsetof(hdRenderResources_c, mpPostEnvironments);
    if (particleAddress == 0)
        operator_new(sizeof(i_this->mpParticleEnvironments));
    if (postAddress == 0)
        operator_new(sizeof(i_this->mpPostEnvironments));
    // The inline array constructors retain their original wrapped destination even after allocation.
    // A guest address of zero remains writable memory here, so native null pointers cannot model it.
    for (u32 i = 0; i < 4; i++)
        gmem_st32(particleAddress + i * sizeof(gptr<void>), 0);
    for (u32 i = 0; i < 7; i++)
        gmem_st32(postAddress + i * sizeof(gptr<void>), 0);
    return i_this;
}

/* 027277CC */
hdRenderResources_c* hdRenderResources_c::create(void* heap) {
    hdRenderResources_c* resources = sMiscResources;
    if (resources == NULL) {
        resources = (hdRenderResources_c*)allocateMiscResources(sizeof(hdRenderResources_c), heap, 4);
        if (resources != NULL) {
            constructMiscDisposer(resources, heap, 3);
            resources->mVtable = gabi::ea(&sMiscResourceVtable);
        }
        sMiscRegisteredResources = resources;
        if (resources != NULL) resources = hdRenderResources_construct(resources);
        sMiscResources = resources;
    }
    return resources;
}

/* inlined into hdRenderResources_c::load */
static void setMiscResourceName(SafeString* name, const char* text) {
    name->__vtbl = gabi::ea(&sMiscSafeStringVtable);
    name->mStringTop = gabi::guest_addr(text);
}

/* inlined into hdRenderResources_c::load */
static void* lookupMiscNamedEnvironment(hdMiscResourceFile_l* file, SafeString* name) {
    hdMiscSafeStringVtable_l* vtable = gabi::at<hdMiscSafeStringVtable_l>(name->__vtbl);
    gabi::call_ptr(vtable->mAssureTermination.get(), name);
    void* dictionary = file->mDictionary.get();
    const char* text = gabi::at<const char>(name->mStringTop);
    return findMiscEnvironment(dictionary, text)->get();
}

/* 02727874 */
void hdRenderResources_c::load() {
    SafeString archive;
    SafeString resource;
    setMiscResourceName(&archive, "Misc");
    setMiscResourceName(&resource, "Misc.bfres");
    void* loaded = loadMiscFile(sMiscResourceManager, &archive, &resource, 0);
    if (loaded == NULL) return;
    hdMiscResourceFile_l* file = getMiscResourceFile(loaded);
    mpShadowEnvironment = findMiscEnvironment(file->mDictionary.get(), "shadow_pre_pass.baglshpp")->get();
    mpMainEnvironment = findMiscEnvironment(file->mDictionary.get(), "wwHD_basic.baglblm")->get();
    mpSecondaryEnvironment = findMiscEnvironment(file->mDictionary.get(), "wwHD_basic.baglssao")->get();
    mpPostEnvironment = findMiscEnvironment(file->mDictionary.get(), "wwHD_basic.baglfila")->get();
    if (sMiscParticleNamesReady == 0) {
        sMiscParticleNamesReady = 1;
        setMiscResourceName(&sMiscParticleNames.mNames[0], "UICommon.baglmf");
        setMiscResourceName(&sMiscParticleNames.mNames[1], "Photo.baglmf");
        setMiscResourceName(&sMiscParticleNames.mNames[2], "PhotoMonochro.baglmf");
        setMiscResourceName(&sMiscParticleNames.mNames[3], "UIPauseDRC.baglmf");
        __register_global_object(gabi::ea(&sMiscParticleNamesDescriptor));
    }
    for (u32 i = 0; i < 4; i++) {
        mpParticleEnvironments[i] = lookupMiscNamedEnvironment(file, &sMiscParticleNames.mNames[i]);
    }
    if (sMiscPostNamesReady == 0) {
        sMiscPostNamesReady = 1;
        setMiscResourceName(&sMiscPostNames.mNames[0], "wwHD_basic.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[1], "wwHD_night.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[2], "scope.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[3], "Monotone.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[4], "MonotoneOut.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[5], "MonotoneOff.bagldof");
        setMiscResourceName(&sMiscPostNames.mNames[6], "wwHD_figure.bagldof");
        __register_global_object(gabi::ea(&sMiscPostNamesDescriptor));
    }
    for (u32 i = 0; i < 7; i++) {
        mpPostEnvironments[i] = lookupMiscNamedEnvironment(file, &sMiscPostNames.mNames[i]);
    }
}

/* WWHD glue
 data sMiscFixedNameVtable 10140DC8 nocheck
 data sMiscBufferedNameVtable 10140DF8 nocheck
 data sMiscGroupNameVtable 10140E10 nocheck
 data sMiscProgramNameVtable 10140E28 nocheck
 data sMiscResourceVtable 10140FC8 nocheck
 data sMiscSafeStringVtable 10140E5C nocheck
 data sMiscResources 101F8664
 data sMiscRegisteredResources 101F8668
 data sMiscResourceManager 101F4F7C
 data sMiscParticleNamesReady 1049F8F8
 data sMiscPostNamesReady 1049F8FC
 data sMiscParticleNames 1049F890
 data sMiscPostNames 1049F8B0
 data sMiscParticleNamesDescriptor 101F8628 nocheck
 data sMiscPostNamesDescriptor 101F8634 nocheck
 str 10140E7C
 str 10140F1C
 str 10140F28
 str 10140F44
 str 10140E94
 str 10140EA8
 str 10140E84
 str 10140F58
 str 10140F68
 str 10140EF8
 str 10140F80
 str 10140F94
 str 10140F0C
 str 10140FA8
 str 10140EBC
 str 10140ED0
 str 10140EE4
 ext 026124B0 void* loadMiscFile(void*, SafeString*, SafeString*, u32)
 ext 027E2DC0 hdMiscResourceFile_l* getMiscResourceFile(void*)
 ext 027DFA24 hdMiscRelativePointer_l* findMiscEnvironment(void*, const char*)
 ext 0273B0D4 void* allocateMiscResources(u32, void*, u32)
 ext 02752B0C void constructMiscDisposer(hdRenderResources_c*, void*, u32)
 ext 02727F64 void destroyMiscName(void*, s32) callback
 ext 028F0164 void destroyMiscNameArray(void*, u32, u32, u32, s32, s32)
*/
