// Deletion flags, termination writes and tail queries from the remaining local method tables.
#include "agl/EnvironmentRemainingAdapters.h"
#include "agl/EnvironmentObject.h"
using namespace agl::env;
void releaseEnvironmentAdapter(void*);

/* 0276B204 */
void ShaderHelperText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276CC2C */
void ManagerTextBufferBase_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276CC40 */
void ManagerTextBuffer_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276CC54 */
void ManagerFixedText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EB8C */
void SetHelperText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EBA0 */
void SetHelperTextBuffer_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EBD0 */
void SetHelperTextBufferBase_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EBE4 */
void SetFixedText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EBF8 */
void SetFormattingBufferBase_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276EC0C */
void SetFormattingBuffer_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276ED40 */
void SetFormattingFixedText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 027706B4 */
void ObjectHelperText_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 027706C8 */
void ObjectHelperTextBuffer_l::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseEnvironmentAdapter),this);
}
/* 0276B218 */
void ShaderHelperText_l::terminateEmpty() { }
/* 0276CC10 */
void ManagerHelperText_l::terminateEmpty() { }
/* 0276D828 */
void ProjectorClassText_l::terminateEmpty() { }
/* 0276D82C */
void ProjectionParameterCallback_l::onValueChanged() { }
/* 0276EBB4 */
void SetHelperText_l::terminateEmpty() { }
/* 0276F228 */
void FogClassText_l::terminateEmpty() { }
/* 0276F22C */
void FogParameterCallback_l::onValueChanged() { }
/* 0276CC14 */
void ManagerTextBufferBase_l::terminateBuffer() {
    be<u8>* text = mText;
    u32 capacity = u32(mCapacity);
    lowMemoryView(text)[capacity - 1] = 0;
}
/* 0276EBB8 */
void SetHelperTextBufferBase_l::terminateBuffer() {
    be<u8>* text = mText;
    u32 capacity = u32(mCapacity);
    lowMemoryView(text)[capacity - 1] = 0;
}
/* 0276EE1C */
void EnvironmentSetInitialView_l::destroy(u32) { }
/* 0277067C */
const sead::SafeText* EnvironmentIndexedQuery_l::objectName(s32 objectIndex) const {
    agl::HierarchyNode* owner = mOwner;
    return gabi::tail_ptr<const sead::SafeText*>(owner->mTable->mNameAt,owner,objectIndex,u32(mClassId));
}
/* 02770698 */
u32 EnvironmentIndexedQuery_l::classObjectCount() const {
    agl::HierarchyNode* owner = mOwner;
    return gabi::tail_ptr<u32>(owner->mTable->mChildCount,owner,u32(mClassId));
}
/* WWHD glue
 ext 0273AF40 void releaseEnvironmentAdapter(void*) callback
*/
