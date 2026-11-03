/* Text virtual adapters identified by this unit's image tables.
 * Destructor-only views do not imply a fixed capacity or payload width. */
#include "agl/UtilityText.h"
namespace agl { namespace capture_text {
/* 027C5898 */
static void destroyCaptureText(sead::SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C58C8 */
static void destroyCaptureTextBuffer(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C5950 */
static void destroyCaptureFixedTextA(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C5964 */
static void destroyCaptureFixedTextB(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C5A98 */
static void destroyCaptureFormattedText(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C58AC */
static void assureCaptureTermination(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027C58B0 */
static void terminateCaptureBuffer(sead::SafeTextBuffer* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    storage[capacity - 1] = 0;
}
} }
namespace agl { namespace shadow_text {
static sead::SafeTextVtable sShadowDefinitionTextTable;
static sead::SafeTextVtable sShadowDefinitionBufferTable;
static sead::SafeTextVtable sAllocatedBufferTable;
sead::SafeTextVtable* definitionTextTable() { return &sShadowDefinitionTextTable; }
sead::SafeTextVtable* definitionBufferTable() { return &sShadowDefinitionBufferTable; }
sead::SafeTextVtable* ownedIncludeBufferTable() { return &sAllocatedBufferTable; }
/* 027C6B84 */
static void destroyShadowText(sead::SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C6BB4 */
static void destroyShadowTextBuffer(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C6C2C */
static void destroyShadowFixedTextA(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C6C40 */
static void destroyShadowFixedTextB(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027C6B98 */
void assureShadowTermination(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027C6B9C */
static void terminateShadowBuffer(sead::SafeTextBuffer* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    storage[capacity - 1] = 0;
}
/* 027C6BC8 */
static void destroyShadowAllocatedBuffer(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!text) return;
    be<u8>* storage = text->mText;
    text->mVtable = &sAllocatedBufferTable;
    if (storage) releaseShadowTextArray(storage);
    if (flags & 1) aglUtilityTextStorageDelete(text);
}
} }
namespace agl { namespace cache_text {
/* 027CAAFC */
static void destroyCacheText(void* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027CAB14 */
static void destroyCacheAlternateText(void* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027CAB44 */
static void destroyCacheTextBuffer(sead::SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (text && (flags & 1)) aglUtilityTextStorageDelete(text);
}
/* 027CAB10 */
static void assureCacheTermination(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CAB28 */
static void assureCacheAlternateTermination(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 027CAB2C */
static void terminateCacheBuffer(sead::SafeTextBuffer* text) {
    gabi::AutoFrame frame{gabi::current_function()};
    be<u8>* storage = text->mText;
    u32 capacity = text->mCapacity;
    storage[capacity - 1] = 0;
}
} }
/* WWHD glue
data sShadowDefinitionTextTable 1016A490
data sShadowDefinitionBufferTable 1016A4A8
data sAllocatedBufferTable 1016A4C0
ext 0273AFC8 void releaseShadowTextArray(void*)
*/
