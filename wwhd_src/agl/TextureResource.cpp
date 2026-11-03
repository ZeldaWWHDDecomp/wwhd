// agl resource records and texture references: construction (names copied into fixed texts), release of
// a referenced dynamic-cache texture, and teardown; plus this translation unit's text copies.
// Written from WWHD's instructions.
#include "agl/TextureResource.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void destroyRegistered(sead::RegisteredObject* object, u32 flags);
static void copyBytes(void* destination, const void* source, u32 size, u32 flags);
static void constructDisposer(sead::Disposer* disposer);
static sead::DisposerVtable sRecordInterfaceBaseMethods;
static sead::DisposerVtable sRecordMethods;
static sead::DisposerVtable sRecordInterfaceMethods;
static sead::DisposerVtable sReferenceMethods;
static sead::SafeTextVtable sTextMethods;
static sead::SafeTextVtable sTextBufferMethods;
static sead::SafeTextVtable sFixedTextBase32Methods;
static sead::SafeTextVtable sFixedText32Methods;
static sead::SafeTextVtable sFixedTextBase128Methods;
static sead::SafeTextVtable sFixedText128Methods;

/* inlined: the BufferedSafeString part of a fixed text constructor */
template <u32 N> static void initializeFixedText(sead::FixedSafeText<N>* text, sead::SafeTextVtable* methods) {
    text->mVtable = &sTextBufferMethods;
    text->mText = text->mStorage;
    text->mCapacity = N;
    text->mStorage[N - 1] = 0;
    text->mVtable = methods;
}

/* inlined: SafeString length scan, 0 for an unterminated text */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        ++length;
        if (length > sead::kMaxTextLength) return 0;
    }
    return length;
}

/* inlined: FixedSafeString copy of a text, cut to the capacity */
static void copyText(sead::SafeTextBuffer* destination, sead::SafeText* source) {
    be<u8>* output = destination->mText;
    source->assureTermination();
    s32 length = textLength(source->mText);
    s32 capacity = destination->mCapacity;
    u32 terminate = source->mVtable->mAssureTermination.mFunction;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr<void>(terminate, source);
    copyBytes(output, source->mText, length, 0);
    output[length] = 0;
}

/* 027A03A0 */
ResourceRecord* ResourceRecord::create(ResourceRecord* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    ResourceRecord* record = storage;
    if (record == nullptr) record = static_cast<ResourceRecord*>(allocateObject(sizeof(ResourceRecord)));
    if (record == nullptr) return record;
    constructDisposer(record);
    record->mInterfaceMethods = &sRecordInterfaceBaseMethods;
    OwnedListNode<ResourceRecord>* node = &record->mNode;
    if (node == nullptr) node = static_cast<OwnedListNode<ResourceRecord>*>(allocateObject(sizeof(*node)));
    if (node != nullptr) {
        node->mLink.mNext = nullptr;
        node->mOwner = record;
        node->mLink.mPrevious = nullptr;
        node->mList = nullptr;
    }
    record->mVtable = &sRecordMethods;
    record->mInterfaceMethods = &sRecordInterfaceMethods;
    gabi::FrameLocal<sead::SafeText> empty(8);
    empty->mVtable = &sTextMethods;
    empty->mText.v = gabi::guest_str("");
    sead::FixedSafeText<128>* text = &record->mName;
    if (text == nullptr) text = static_cast<sead::FixedSafeText<128>*>(allocateObject(sizeof(*text)));
    if (text != nullptr) {
        initializeFixedText(text, &sFixedTextBase128Methods);
        copyText(text, empty);
        text->mVtable = &sFixedText128Methods;
    }
    text = &record->mPath;
    if (text == nullptr) text = static_cast<sead::FixedSafeText<128>*>(allocateObject(sizeof(*text)));
    if (text != nullptr) {
        initializeFixedText(text, &sFixedTextBase128Methods);
        text->mText[0] = 0;
        text->mVtable = &sFixedText128Methods;
    }
    text = &record->mLabel;
    if (text == nullptr) text = static_cast<sead::FixedSafeText<128>*>(allocateObject(sizeof(*text)));
    if (text != nullptr) {
        initializeFixedText(text, &sFixedTextBase128Methods);
        text->mText[0] = 0;
        text->mVtable = &sFixedText128Methods;
    }
    record->mLoaded = 0;
    record->mFlags = 0;
    be<u32>* range = record->mRange;
    if (range == nullptr) range = static_cast<be<u32>*>(allocateObject(sizeof(record->mRange)));
    if (range != nullptr) { range[1] = 0; range[0] = 0; }
    record->mState = 0;
    record->mPending = 0;
    record->mEnabled = 1;
    record->mBusy = 0;
    record->mUseCount = 0;
    record->mLocked = 0;
    return record;
}

/* 027A063C */
void TextureReference::notify() {}

/* 027A0640 */
void TextureReference::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureCacheEntry* texture = mTexture;
    if (texture == nullptr) return;
    if (mOwnsTexture != 0) sDynamicTextureCache->releaseShared(texture);
    mTexture = nullptr;
    mOwnsTexture = 0;
}

/* 027A0698 */
TextureReference* TextureReference::create(TextureReference* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureReference* reference = storage;
    if (reference == nullptr) reference = static_cast<TextureReference*>(allocateObject(sizeof(TextureReference)));
    if (reference == nullptr) return reference;
    OwnedListNode<TextureReference>* node = &reference->mNode;
    if (node == nullptr) node = static_cast<OwnedListNode<TextureReference>*>(allocateObject(sizeof(*node)));
    if (node != nullptr) {
        node->mLink.mNext = nullptr;
        node->mOwner = reference;
        node->mLink.mPrevious = nullptr;
        node->mList = nullptr;
    }
    reference->mMethods = &sReferenceMethods;
    gabi::FrameLocal<sead::SafeText> empty(8);
    empty->mVtable = &sTextMethods;
    empty->mText.v = gabi::guest_str("");
    sead::FixedSafeText<32>* text = &reference->mName;
    if (text == nullptr) text = static_cast<sead::FixedSafeText<32>*>(allocateObject(sizeof(*text)));
    if (text != nullptr) {
        initializeFixedText(text, &sFixedTextBase32Methods);
        copyText(text, empty);
        text->mVtable = &sFixedText32Methods;
    }
    reference->mOption = 0;
    reference->mActive = 0;
    reference->mTexture = nullptr;
    reference->mOffset = 0.0f;
    reference->mScale = 0.0f;
    reference->mOwnsTexture = 0;
    reference->mReady = 0;
    return reference;
}

/* 027A0844 */
void TextureReference::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sReferenceMethods;
    release();
    if (flags & 1) releaseObject(this);
}

/* HD: compiler-generated and template code of this translation unit */

/* 027A08A0 */
void resource_text::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A08B4 */
void resource_text::FixedTextBase32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A08C8 */
void resource_text::Text::assureTerminationImpl() {}

/* 027A08CC */
void resource_text::TextBuffer::assureTerminationImpl() {
    mText[static_cast<s32>(mCapacity) - 1] = 0;
}

/* 027A08E4 */
void resource_text::TextBuffer::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A08F8 */
void resource_text::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A090C */
void resource_text::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0920 */
void resource_text::FixedTextBase128::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0934 */
void resource_text::FixedText128::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0948 */
void resource_text::FixedText32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A095C */
void resource_text::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0970 */
void resource_text::FixedTextD::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0984 */
void resource_text::FixedTextE::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 027A0998 */
void ResourceRecord::detach() {}

/* 027A099C */
void ResourceRecord::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sRecordMethods;
    mInterfaceMethods = &sRecordInterfaceMethods;
    destroyRegistered(this, 0);
    if (flags & 1) releaseObject(this);
}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext C0009988 void copyBytes(void* destination, const void* source, u32 size, u32 flags)
ext 02752A84 void constructDisposer(sead::Disposer* disposer)
ext 02752BEC void destroyRegistered(sead::RegisteredObject* object, u32 flags)
data sRecordInterfaceBaseMethods 10167EF8
data sRecordMethods 10167F98
data sRecordInterfaceMethods 10167FA8
data sReferenceMethods 10168030
data sTextMethods 10167EB8
data sTextBufferMethods 10167ED0
data sFixedTextBase32Methods 10167F38
data sFixedText32Methods 10167F50
data sFixedTextBase128Methods 10167F68
data sFixedText128Methods 10167F80
str 10168000 "" in 027A03A0
str 1016802C "" in 027A0698
*/
