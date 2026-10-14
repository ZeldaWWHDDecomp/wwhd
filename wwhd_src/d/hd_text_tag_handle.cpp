/* HD text tag owner/handle helpers. Roles inferred from their stored handle and virtual operations. */
#include "d/hd_text_tag_handle.h"
/* 025F9F50 */
void* hdTextTagHandle_c::getVtable() { return mpVtable; }
/* 025FA050 */
be<u32>* hdTextTagHandle_c::setContext(u32 context) { mContext = context; return &mHandle; }
/* 025FA05C */
void hdTextTagHandle_c::clearOutput(be<u32>* output) { *output = 0; }
/* 025FA068 */
hdTextTagOwner_c* hdTextTagHandle_c::getOwner() { return mpOwner; }
/* 025FA070 */
u32 hdTextTagHandle_c::begin() {
    hdTextTagOwner_c* owner = mpOwner;
    return gabi::call_ptr<u32>(owner->mpVtable->mBegin.get(), owner, (u32)mHandle);
}
/* 025FA08C */
u32 hdTextTagHandle_c::end() {
    hdTextTagOwner_c* owner = mpOwner;
    return gabi::call_ptr<u32>(owner->mpVtable->mEnd.get(), owner, (u32)mHandle);
}
/* 025FA0A8 */
void hdTextTagHandle_c::release() {
    hdTextTagOwner_c* owner = mpOwner;
    gabi::call_ptr(owner->mpVtable->mRelease.get(), owner, (u32)mHandle);
    mpOwner = NULL;
}
/* WWHD glue: no TU-wide definitions duplicated from the merged source. */
