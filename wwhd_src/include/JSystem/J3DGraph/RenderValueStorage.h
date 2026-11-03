// J3D HD renderer: shared small layouts and guest-address helpers of the J3D render sources.
// Names are our own; written from the WWHD binary.
#pragma once
#include "wwhd.h"
#include "sead/Foundation.h"
#include "sead/MethodTreeNode.h"

void registerRenderStatics(void* token);

namespace j3d {
// Guest address 0 as an object pointer. Where an index is out of range the original leaves an element
// pointer NULL and then reads through it (or passes it on) without a test; written as a C++ null
// pointer, the compiler would assume that path cannot happen.
template <class T> inline T* guestNull() { return reinterpret_cast<T*>(PPC_MEM_BASE); }
// The original's NULL tests on embedded sub-objects and array elements use 32-bit address arithmetic:
// only storage whose member address wraps to 0 takes them. Written with C++ member/element addresses,
// the compiler would fold such tests away.
// A stored pointer the original uses without a NULL test (address 0 stays an addressable object).
template <class T> inline T* guestObject(const gptr<T>& pointer) { return reinterpret_cast<T*>(PPC_MEM_BASE + pointer.v.get()); }
inline bool memberWrapsToNull(const void* object, u32 offset) { return gabi::ea(object) + offset == 0; }
// A member at a byte offset, with the original's wrapping 32-bit address arithmetic (NULL when it wraps to 0).
template <class T> inline T* guestMember(const void* object, u32 offset) { return gabi::at<T>(gabi::ea(object) + offset); }
template <class T> inline T* guestElement(T* base, u32 index) { return gabi::at<T>(gabi::ea(base) + index * (u32)sizeof(T)); }

struct RenderStaticRegistry {
    be<u32> mWords[4];
    static RenderStaticRegistry* get();
};
// These compiler constructors establish storage, without implying ownership or
// a vector/quaternion interpretation in the surrounding material resources.
struct RenderStorageSpan {
    be<s32> mCount;
    gptr<void> mStorage;
    static RenderStorageSpan* construct(RenderStorageSpan* storage);
};
struct RenderResourceSpan {
    be<s32> mCount;
    gptr<void> mStorage;
    static RenderResourceSpan* construct(RenderResourceSpan* storage);
};
struct RenderFloat3 {
    be<f32> mX, mY, mZ;
    static RenderFloat3* construct(RenderFloat3* storage);
};
struct RenderFloat4 {
    be<f32> mX, mY, mZ, mW;
    static RenderFloat4* construct(RenderFloat4* storage);
};
struct RenderUniformRow {
    be<f32> mValues[4];
};
// Only the prefix reached by this unit's callback-dispatch entry.
struct RenderCallbackNode_l {
    sead::SafeText mText;
    be<u8> mEmptyCharacter;
    u8 mPadding[3];
    gptr<void> mDispatchMethods;
    sead::MethodDelegate mDelegate;
};
struct RenderDelegateEntries {
    static void dispatch(RenderCallbackNode_l* node);
    static void invoke(sead::MethodDelegate* delegate);
};
struct RenderTextEntries {
    static void destroyBase(sead::SafeText* text, u32 flags);
    static void destroyBufferedA(sead::BufferedText* text, u32 flags);
    static void destroyBufferedB(sead::BufferedText* text, u32 flags);
    static void destroyBufferedC(sead::BufferedText* text, u32 flags);
    static void destroyCallback(sead::SafeText* text, u32 flags);
    static void ensureTerminated(sead::BufferedText* text);
    static void assureText();
    static void assureCallbackText();
};
// Prefix reached by the completion callback; unrelated bytes stay opaque.
struct RenderCompletionState_l {
    be<u8> mEnabled, mReady;
    u8 mPadding02[10];
    be<u32> mFlags;
    u8 mPadding10[0xD0];
    be<u8> mChanged;
    u8 mPaddingE1[11];
    be<u32> mEpochFlags;
    u8 mPaddingF0[0x24];
    be<u8> mPending, mComplete, mFailed;
    static void complete(u32 result, RenderCompletionState_l* state);
};
struct RenderPointerEntries {
    static void* get(const gptr<void>* entries, u32 index);
};
}
WWHD_OFFSET(j3d::RenderFloat3, mX, 0);
WWHD_OFFSET(j3d::RenderFloat3, mY, 4);
WWHD_OFFSET(j3d::RenderFloat3, mZ, 8);
WWHD_SIZE(j3d::RenderFloat3, 12);
WWHD_OFFSET(j3d::RenderFloat4, mW, 12);
WWHD_SIZE(j3d::RenderFloat4, 16);

WWHD_SIZE(j3d::RenderStaticRegistry, 16);

WWHD_OFFSET(j3d::RenderStorageSpan, mStorage, 4);
WWHD_SIZE(j3d::RenderStorageSpan, 8);
WWHD_OFFSET(j3d::RenderResourceSpan, mStorage, 4);
WWHD_SIZE(j3d::RenderResourceSpan, 8);

WWHD_OFFSET(j3d::RenderCallbackNode_l, mDelegate, 16);

WWHD_OFFSET(j3d::RenderCallbackNode_l, mEmptyCharacter, 8);
WWHD_OFFSET(j3d::RenderCallbackNode_l, mDispatchMethods, 12);
WWHD_SIZE(j3d::RenderCallbackNode_l, 32);

WWHD_SIZE(j3d::RenderUniformRow, 16);
WWHD_OFFSET(j3d::RenderCompletionState_l, mFlags, 0xC);
WWHD_OFFSET(j3d::RenderCompletionState_l, mChanged, 0xE0);
WWHD_OFFSET(j3d::RenderCompletionState_l, mEpochFlags, 0xEC);
WWHD_OFFSET(j3d::RenderCompletionState_l, mPending, 0x114);
WWHD_SIZE(j3d::RenderCompletionState_l, 0x117);
