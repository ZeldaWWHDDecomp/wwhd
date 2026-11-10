// Shader variation input layouts: construction (from the archive layout constructor 027B9470), the archive
// loader's reservation (027B7D78), the variant selection (027BB710) and release (027B94B8 / virtual delete).
#include "agl/RenderInputCollections.h"
#include "sead/ElementAt.h"
#include "agl/ShaderStageState.h"
using namespace agl::render;

static sead::SafeTextVtable sLayoutNameMethods_l;
static u8 sLayoutMethods_l;

/* the GHS inline sead::PointerBuffer constructor (with its placement-new null guard) */
static void constructPointerArray(RenderPointerArray* array) {
    if (!array) array = static_cast<RenderPointerArray*>(allocateRenderStorage(sizeof(RenderPointerArray)));
    if (array) {
        array->mCapacity = 0;
        array->mCount = 0;
        array->mStorage = nullptr;
    }
}
/* 027D3D3C */
agl::render::ShaderInputLayout* agl::render::createShaderInputLayout(ShaderInputLayout* layout) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!layout) layout = static_cast<ShaderInputLayout*>(allocateRenderStorage(sizeof(ShaderInputLayout)));
    if (!layout) return nullptr;
    layout->mMethods = &sLayoutMethods_l;
    sead::SafeText* name = &layout->mName;
    if (!name) name = static_cast<sead::SafeText*>(allocateRenderStorage(sizeof(sead::SafeText)));
    if (name) {
        name->mVtable = &sLayoutNameMethods_l;
        name->mText.v = gabi::guest_str("unititled");
    }
    layout->mCompiledSource = nullptr;
    layout->mOutputText = nullptr;
    constructPointerArray(&layout->mNames);
    constructPointerArray(&layout->mValues);
    constructPointerArray(&layout->mSelectedNames);
    constructPointerArray(&layout->mSelectedValues);
    return layout;
}
/* 027D3E60 */
void agl::render::ShaderInputLayout::releaseArrays() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mNames.mStorage) {
        releaseInputArray(&mNames);
        releaseInputArray(&mValues);
    }
    if (mSelectedNames.mStorage) {
        releaseInputArray(&mSelectedNames);
        releaseInputArray(&mSelectedValues);
    }
}
/* 027D3EC0 */
void agl::render::destroyShaderInputLayout(ShaderInputLayout* layout,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (layout) {
        layout->mMethods = &sLayoutMethods_l;
        layout->releaseArrays();
        if (flags & 1u) releaseRenderStorage(layout);
    }
}
/* 027D3F1C */
void agl::render::ShaderInputLayout::reserve(s32 names,s32 selections,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (names > 0) {
        allocateInputArray(&mNames,names,heap,4);
        allocateInputArray(&mValues,names,heap,4);
    }
    if (selections > 0) {
        allocateInputArray(&mSelectedNames,selections,heap,4);
        allocateInputArray(&mSelectedValues,selections,heap,4);
    }
}
/* 027D3FC4 */
void agl::render::ShaderInputLayout::resetSelection() {
    mSelectedNames.mCount = 0;
    mSelectedValues.mCount = 0;
}
/* 027D3FD4 */
void agl::render::ShaderInputLayout::select(const char* name,const char* value) {
    s32 count = mSelectedNames.mCount;
    if (count < (s32)mSelectedNames.mCapacity) {
        sead::elementAt<be<u32>>(gabi::ea(mSelectedNames.entries()),(u32)count) = gabi::ea(name); /* guest EA arithmetic: wraps at 32 bits as on the console */
        mSelectedNames.mCount = (s32)mSelectedNames.mCount + 1;
    }
    count = mSelectedValues.mCount;
    if (count < (s32)mSelectedValues.mCapacity) {
        sead::elementAt<be<u32>>(gabi::ea(mSelectedValues.entries()),(u32)count) = gabi::ea(value);
        mSelectedValues.mCount = (s32)mSelectedValues.mCount + 1;
    }
}
/* WWHD glue
data sLayoutMethods_l 1016BFEC
data sLayoutNameMethods_l 1016B360
str 1016BF60
ext 0273B5DC void releaseInputArray(agl::render::RenderPointerArray*)
ext 0273B584 void allocateInputArray(agl::render::RenderPointerArray*,s32,void*,s32)
*/
