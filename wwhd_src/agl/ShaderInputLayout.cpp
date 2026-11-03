/* Constructor/destructor forwards and descriptor-weight accumulation in WWHD. */
// References: WWHD's own instructions and strings only.
#include "agl/ShaderInputLayout.h"
static void* allocateObject(u32 bytes);
static void releaseObject(void* object);
static void initializeLayout(agl::ShaderInputLayout_l* layout);
static void finalizeLayout(agl::ShaderInputLayout_l* layout, u32 flags);

/* 027B9470 */
agl::ShaderInputLayout_l* agl::ShaderInputLayout_l::construct(ShaderInputLayout_l* layout) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(layout) == 0) {
        layout = (ShaderInputLayout_l*)allocateObject(sizeof(ShaderInputLayout_l));
        if (!layout)
            return layout;
    }
    initializeLayout(layout);
    return layout;
}
/* 027B94B8 */
void agl::ShaderInputLayout_l::destroy(ShaderInputLayout_l* layout, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(layout) == 0)
        return;
    finalizeLayout(layout, 2);
    if (flags & 1)
        releaseObject(layout);
}
/* 027B950C */
u32 agl::ShaderVariantDomainView_l::weightedSum(const be<u32>* values) {
    u32 count = mCount;
    auto descriptor = mRadices.get();
    auto end = descriptor + count;
    u32 result = 0;
    for (; descriptor != end; ++descriptor, ++values) {
        u32 weight = descriptor->mDivisor;
        u32 value = *values;
        result += value * weight;
    }
    return result;
}
/* WWHD glue
ext 0273AD10 void* allocateObject(u32 bytes)
ext 0273AF40 void releaseObject(void* object)
ext 027D3D3C void initializeLayout(agl::ShaderInputLayout_l* layout)
ext 027D3EC0 void finalizeLayout(agl::ShaderInputLayout_l* layout, u32 flags)
*/
