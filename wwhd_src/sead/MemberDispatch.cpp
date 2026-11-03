// WWHD's per-translation-unit state callbacks, reconstructed from local instructions.
#include "sead/MemberDispatch.h"

// The direct form stores a code address; the virtual form uses its low half as a vtable offset.
static u32 invokeMember(const sead::MemberSlot& slot, be<u8>* object) {
    s32 index = slot.mVirtualIndex;
    be<u8>* adjusted = object + (s32)slot.mObjectAdjustment;
    u32 target;
    if (index < 0) target = slot.mTarget;
    else {
        s16 tableOffset = (u32)slot.mTarget;
        sead::VirtualEntry* table = *reinterpret_cast<gptr<sead::VirtualEntry>*>(adjusted + tableOffset);
        target = table[index].mTarget;
    }
    return gabi::tail_ptr<u32>(target, adjusted);
}

/* 0270960C */
static u32 dispatchSceneEnter(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mEnter, object);
}

/* 0270964C */
static u32 dispatchSceneExecute(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mExecute, object);
}

/* 0270968C */
static u32 dispatchSceneLeave(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mLeave, object);
}

/* 0270C6C0 */
static u32 dispatchMenuEnter(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mEnter, object);
}

/* 0270C700 */
static u32 dispatchMenuExecute(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mExecute, object);
}

/* 0270C740 */
static u32 dispatchMenuLeave(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mLeave, object);
}

/* 02719860 */
static u32 dispatchItemEnter(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mEnter, object);
}

/* 027198A0 */
static u32 dispatchItemExecute(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mExecute, object);
}

/* 027198E0 */
static u32 dispatchItemLeave(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mLeave, object);
}

/* 0271B94C */
static u32 dispatchMapEnter(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mEnter, object);
}

/* 0271B98C */
static u32 dispatchMapExecute(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mExecute, object);
}

/* 0271B9CC */
static u32 dispatchMapLeave(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mLeave, object);
}

/* 0271ED08 */
static u32 dispatchMessageEnter(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mEnter, object);
}

/* 0271ED48 */
static u32 dispatchMessageExecute(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mExecute, object);
}

/* 0271ED88 */
static u32 dispatchMessageLeave(sead::CallbackSet* callbacks, be<u8>* object) {
    return invokeMember(callbacks->mLeave, object);
}

/* WWHD glue
*/
