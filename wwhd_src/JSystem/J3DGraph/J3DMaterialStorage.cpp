// Material backing storage with the HD build's four named-model size exceptions.
#include "JSystem/J3DGraph/J3DMaterialBindings.h"
#include "sead/FileDevice.h"
using namespace j3d;
MaterialCountHeader_l* materialCountHeader(void*);
MaterialNameHeader_l* materialNameHeader(void*);
u32 materialBufferStorageSize(const MaterialBufferRequest_l*);
void* allocateMaterialBuffer(u32,u32);
void initializeMaterialBuffer(MaterialProgramState_l*,const MaterialBufferRequest_l*,void*,u32);
static sead::SafeTextVtable sMaterialStorageTextMethods;
/* inlined into MaterialBinding::allocateForResource */
static bool sameMaterialText(sead::SafeText* expected,sead::SafeText* actual) {
    expected->assureTermination();
    expected->assureTermination();
    const be<u8>* saved=expected->mText;
    actual->assureTermination();
    if(gabi::ea(saved)==gabi::ea(static_cast<be<u8>*>(actual->mText))) return true;
    const be<u8>* left=expected->mText;
    const be<u8>* right=actual->mText;
    for(u32 i=0;i<=u32(sead::kMaxTextLength);++i) {
        u32 a=left[i];
        u32 b=right[i];
        if(a!=b) return false;
        if(a==0) return true;
    }
    return false;
}
/* inlined into MaterialBinding::allocateForResource */
static bool matchesMaterialName(void* resource,const char* name,sead::SafeText* expected,sead::SafeText* actual) {
    expected->mVtable=&sMaterialStorageTextMethods;
    expected->mText=gabi::at<be<u8>>(gabi::guest_str(name));
    MaterialNameHeader_l* header=materialNameHeader(resource);
    u32 offset=header->mNameOffset;
    actual->mText=offset==0?NULL:gabi::at<be<u8>>(gabi::ea(&header->mNameOffset)+offset);
    actual->mVtable=&sMaterialStorageTextMethods;
    return sameMaterialText(expected,actual);
}
/* 027F36FC */
void MaterialBinding::allocateForResource(void* resource,ShaderInput_l* input) {
    gabi::AutoFrame frame{gabi::current_function()};
    MaterialCountHeader_l* header=materialCountHeader(resource);
    gabi::FrameLocal<be<u32>> countWord(0x48);
    gabi::FrameLocal<MaterialBufferRequest_l> request(0x4C);
    request->mOffset=0;
    request->mCount=0xFFFFFFFF;
    request->mBase=0;
    request->mBytes=0xFFFFFFFF;
    request->mAllocate=0;
    mCode=header;
    request->mEnabled=1;
    request->mElementCount=0xFFFFFFFF;
    u32 count=header->mCount;
    *countWord.get()=count;
    request->mCount=u32(*countWord.get());
    u32 itemCount=header->mCount;
    request->mElementCount=itemCount;
    gabi::FrameLocal<sead::SafeText> expectedParticle(8),expectedUpper(0x10),expectedPost(0x18),expectedBackground(0x20);
    gabi::FrameLocal<sead::SafeText> actualParticle(0x28),actualUpper(0x30),actualPost(0x38),actualBackground(0x40);
    bool expanded=matchesMaterialName(resource,"cc_ptcl",expectedParticle.get(),actualParticle.get());
    if(!expanded) expanded=matchesMaterialName(resource,"bo_ue1",expectedUpper.get(),actualUpper.get());
    if(!expanded) expanded=matchesMaterialName(resource,"pp",expectedPost.get(),actualPost.get());
    if(!expanded) expanded=matchesMaterialName(resource,"bgn_2",expectedBackground.get(),actualBackground.get());
    if(expanded) {
        request->mOffset=0;
        request->mAllocate=1;
        request->mBase=0;
        request->mBytes=itemCount*10;
    } else {
        if(itemCount<2) itemCount=2;
        request->mBase=0;
        request->mOffset=0;
        request->mBytes=itemCount*4;
        request->mAllocate=1;
    }
    u32 size=materialBufferStorageSize(request.get());
    mStorage=allocateMaterialBuffer(size,64);
    void* storage=mStorage;
    initializeMaterialBuffer(&mState,request.get(),storage,size);
    if(input!=NULL) setInput(input);
}
/* WWHD glue
 ext 027F68FC j3d::MaterialCountHeader_l* materialCountHeader(void*)
 ext 027F68F4 j3d::MaterialNameHeader_l* materialNameHeader(void*)
 ext 027DFCCC u32 materialBufferStorageSize(const j3d::MaterialBufferRequest_l*)
 ext 027EC0AC void* allocateMaterialBuffer(u32,u32)
 ext 027DFB70 void initializeMaterialBuffer(j3d::MaterialProgramState_l*,const j3d::MaterialBufferRequest_l*,void*,u32)
 str 1016E528 "cc_ptcl"
 str 1016E538 "bo_ue1"
 str 1016E540 "pp"
 str 1016E530 "bgn_2"
 data sMaterialStorageTextMethods 1016E510
*/
