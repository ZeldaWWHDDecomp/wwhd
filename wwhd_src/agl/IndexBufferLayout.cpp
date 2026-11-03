/* Index-buffer storage setup and cache publication in the WWHD graphics library. */
// References: WWHD's own instructions and strings only.
#include "agl/IndexBufferLayout.h"
static void* allocateObject(u32 bytes);
static void releaseObject(void* object);
static void flushRangeNoSync(void* address, u32 bytes);
static sead::DisposerVtable sIndexBufferMethods;

/* 027B5430 */
agl::IndexBufferLayout* agl::IndexBufferLayout::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    IndexBufferLayout* result = this;
    if (gabi::ea(result) == 0) {
        result = (IndexBufferLayout*)allocateObject(sizeof(IndexBufferLayout));
        if (!result)
            return result;
    }
    result->mFormat = 4;
    result->mIndexCount = 0;
    result->mVtable = &sIndexBufferMethods;
    result->mIndexBytes = 0;
    result->mPrimitiveMode = 4;
    result->mStorage = nullptr;
    return result;
}
/* 027B548C */
void agl::IndexBufferLayout::clear() {
    mStorage = nullptr;
    mIndexCount = 0;
    mIndexBytes = 0;
}
/* 027B54A0 */
void agl::IndexBufferLayout::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mVtable = &sIndexBufferMethods;
    clear();
    if (flags & 1)
        releaseObject(this);
}
/* 027B54E0 */
void agl::IndexBufferLayout::setStorage(void* storage, u32 format, u32 count) {
    gabi::AutoFrame frame{gabi::current_function()};
    clear();
    mIndexCount = count;
    mFormat = format;
    mStorage = storage;
    u32 indexBytes = format == 4 ? 2 : 4;
    mIndexBytes = indexBytes;
    flushRangeNoSync(storage, indexBytes * count);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 bytes)
ext 0273AF40 void releaseObject(void* object)
ext C00088A0 void flushRangeNoSync(void* address, u32 bytes)
data sIndexBufferMethods 10169758
*/
