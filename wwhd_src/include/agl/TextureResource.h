// agl resource records and texture references: a registered record with three 128-character names
// (name, path, label) in an intrusive list, and a named reference to a dynamic-texture-cache entry that it
// may own (released to the shared pool); plus this translation unit's text copies.
#pragma once
#include "agl/DynamicTextureCache.h"
namespace agl {

// node of the intrusive lists these objects live in: link, owning object, list
template <class T> struct OwnedListNode {
    sead::Link mLink;
    gptr<T> mOwner;
    gptr<void> mList;
};

struct ResourceRecord : sead::RegisteredObject {
    gptr<const void> mInterfaceMethods;
    OwnedListNode<ResourceRecord> mNode;
    sead::FixedSafeText<128> mName;
    sead::FixedSafeText<128> mPath;
    sead::FixedSafeText<128> mLabel;
    be<u8> mLoaded;
    u8 mPaddingA[3];
    be<u32> mFlags;
    be<u32> mRange[2];
    be<u8> mEnabled;
    be<u8> mLocked;
    be<u8> mPending;
    be<u8> mBusy;
    be<u32> mState;
    be<u32> mUseCount;
    static ResourceRecord* create(ResourceRecord* storage);
    void destroy(u32 flags);
    void detach();
};

struct TextureReference {
    OwnedListNode<TextureReference> mNode;
    sead::FixedSafeText<32> mName;
    gptr<TextureCacheEntry> mTexture;
    be<u8> mActive;
    be<u8> mReady;
    u8 mPaddingA;
    be<u8> mOption;
    be<f32> mScale;
    be<f32> mOffset;
    be<u8> mOwnsTexture;
    u8 mPaddingB[3];
    gptr<sead::DisposerVtable> mMethods;
    static TextureReference* create(TextureReference* storage);
    void release();
    void notify();
    void destroy(u32 flags);
};

// text copies emitted by this translation unit (only their out-of-line virtuals)
namespace resource_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextBase32 : sead::FixedSafeText<32> { void destroy(u32 flags); };
struct FixedText32 : sead::FixedSafeText<32> { void destroy(u32 flags); };
struct FixedTextBase128 : sead::FixedSafeText<128> { void destroy(u32 flags); };
struct FixedText128 : sead::FixedSafeText<128> { void destroy(u32 flags); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextE : sead::SafeTextBuffer { void destroy(u32 flags); };
}
}
WWHD_SIZE(agl::OwnedListNode<agl::ResourceRecord>, 0x10);
WWHD_OFFSET(agl::ResourceRecord, mInterfaceMethods, 0x10);
WWHD_OFFSET(agl::ResourceRecord, mNode, 0x14);
WWHD_OFFSET(agl::ResourceRecord, mName, 0x24);
WWHD_OFFSET(agl::ResourceRecord, mPath, 0xB0);
WWHD_OFFSET(agl::ResourceRecord, mLabel, 0x13C);
WWHD_OFFSET(agl::ResourceRecord, mLoaded, 0x1C8);
WWHD_OFFSET(agl::ResourceRecord, mRange, 0x1D0);
WWHD_OFFSET(agl::ResourceRecord, mEnabled, 0x1D8);
WWHD_OFFSET(agl::ResourceRecord, mState, 0x1DC);
WWHD_SIZE(agl::ResourceRecord, 0x1E4);
WWHD_OFFSET(agl::TextureReference, mName, 0x10);
WWHD_OFFSET(agl::TextureReference, mTexture, 0x3C);
WWHD_OFFSET(agl::TextureReference, mScale, 0x44);
WWHD_OFFSET(agl::TextureReference, mOwnsTexture, 0x4C);
WWHD_OFFSET(agl::TextureReference, mMethods, 0x50);
WWHD_SIZE(agl::TextureReference, 0x54);
