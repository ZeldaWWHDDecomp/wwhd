/* agl::detail::RootNode: the graphics library's root debug node singleton, its node-meta helpers and the text
 * instances of its translation unit. Reconstructed from this game's instructions. */
#include "agl/RootNode.h"
using namespace sead;
using namespace agl::detail;

static void* allocateObject(u32 size);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static RegisteredObject* constructHeapDisposer(RegisteredObject* object, RegistryHeap* heap, u32 option);
static void* moveBytes(void* destination, const void* source, u32 size, u32 flags);
static CriticalSection* constructCriticalSection(CriticalSection* section);
static void destroyCriticalSection(CriticalSection* section, u32 flags);
static void destroyRegisteredObject(RegisteredObject* object, u32 flags);

static RootNode* sInstance;
static RootNodeDisposer* sDisposer;
static PrivateResource* sPrivateResource;
static SafeText sEmptyText;
static DisposerVtable sDisposerVtable;
static RootNodeVtable sRootNodeVtable;
static SafeTextVtable sTextVtable;
static SafeTextVtable sNameVtable;
static SafeTextVtable sNameBaseVtable;
static SafeTextVtable sFormatTextVtable;

/* the inline sead text length: 0 for an empty text or one longer than kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    u32 address = gabi::ea(text);
    if (*gabi::at<const be<u8>>(address) == 0) return 0;
    s32 length = 0;
    do {
        ++length;
        ++address;
        if (length > kMaxTextLength) return 0;
    } while (*gabi::at<const be<u8>>(address) != 0);
    return length;
}
/* the inline sead text comparison: equal through the first NUL, at most kMaxTextLength + 1 characters */
static bool textsEqual(const be<u8>* first, const be<u8>* second) {
    u32 left = gabi::ea(first);
    u32 right = gabi::ea(second);
    for (s32 index = 0; index <= kMaxTextLength; ++index) {
        u8 character = *gabi::at<const be<u8>>(left + index);
        if (character != *gabi::at<const be<u8>>(right + index)) return false;
        if (character == 0) return true;
    }
    return false;
}
static void assure(const SafeText* text, const SafeTextVtable* methods) {
    gabi::call_ptr<void>(methods->mAssureTermination.mFunction, text);
}

/* 02785A64 */
RootNode* RootNode::construct(RootNode* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (node == nullptr) {
        node = static_cast<RootNode*>(allocateObject(sizeof(RootNode)));
        if (node == nullptr) return nullptr;
    }
    node->mVtable = &sRootNodeVtable;
    FixedSafeText<256>* name = &node->mName;
    name->mText = &name->mStorage[0];
    name->mCapacity = 256;
    name->mStorage[255] = 0;
    name->mVtable = &sNameBaseVtable;
    // the name starts as a copy of sead's empty text
    const SafeText* empty = &sEmptyText;
    const SafeTextVtable* methods = empty->mVtable;
    be<u8>* destination = name->mText;
    assure(empty, methods);
    be<u8>* source = empty->mText;
    s32 length = textLength(source);
    s32 capacity = name->mCapacity;
    if (length >= capacity) length = capacity - 1;
    assure(empty, methods);
    moveBytes(destination, source, (u32)length, 0);
    destination[length] = 0;
    name->mVtable = &sNameVtable;
    constructCriticalSection(&node->mCriticalSection);
    return node;
}

/* 02785BC0 */
void RootNode::createInstance(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sInstance != nullptr) return;
    RootNode* storage = static_cast<RootNode*>(allocateArray(sizeof(RootNode), heap, 4));
    RegisteredObject* disposer = &storage->mDisposer;
    if (gabi::ea(disposer) != 0) {
        constructHeapDisposer(disposer, heap, 3);
        disposer->mVtable = &sDisposerVtable;
    }
    sDisposer = static_cast<RootNodeDisposer*>(disposer);
    RootNode* node = storage;
    if (node != nullptr) node = construct(node);
    sInstance = node;
}

/* 02785C60 */
void RootNode::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    destroyCriticalSection(&mCriticalSection, 2);
    if (flags & 1) releaseObject(this);
}

/* 02785CB4 */
void RootNode::setNodeMeta(void* node, const SafeText* meta) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sInstance == nullptr) return;
    if (sPrivateResource->mDebugHeap == nullptr) return;
    assure(meta, meta->mVtable);
    RootNode* root = sInstance;
    const SafeTextVtable* methods = root->mName.mVtable;
    be<u8>* metaText = meta->mText;
    assure(&root->mName, methods);
    gabi::FrameLocal<RootFormatText> text(8);
    gabi::cpu->cr[6] = 0;
    // the meta text and the root name are the only arguments; the other saved registers are not format inputs
    gabi::call_ptr<RootFormatText*>(gabi::guest_fn(reinterpret_cast<const void*>(&RootFormatText::createFormatted)),
        text.get(), reinterpret_cast<const char*>(gabi::guest_str("%s%s")), metaText, (be<u8>*)root->mName.mText);
}

/* 02785D54 */
void RootNode::initialize(RegistryHeap* heap, const SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    assure(name, name->mVtable);
    assure(name, name->mVtable);
    const SafeText* empty = &sEmptyText;
    const SafeTextVtable* emptyMethods = empty->mVtable;
    be<u8>* nameText = name->mText;
    assure(empty, emptyMethods);
    be<u8>* emptyText = empty->mText;
    // the pointer test uses the text read before the call, the character compare reads it again
    if (nameText == emptyText || textsEqual(name->mText, emptyText)) {
        // no name: the node name is sead's empty text
        be<u8>* destination = mName.mText;
        assure(empty, emptyMethods);
        s32 length = textLength(emptyText);
        s32 capacity = mName.mCapacity;
        if (length >= capacity) length = capacity - 1;
        assure(empty, emptyMethods);
        moveBytes(destination, emptyText, (u32)length, 0);
        destination[length] = 0;
    } else {
        // the node name is "," followed by the name
        gabi::FrameLocal<SafeText> separator(8);
        separator->mVtable = &sTextVtable;
        separator->mText.v = gabi::guest_str(",");
        be<u8>* destination = mName.mText;
        static_cast<RootText*>(separator.get())->assureTermination();
        s32 length = textLength(separator->mText);
        s32 capacity = mName.mCapacity;
        if (length >= capacity) length = capacity - 1;
        assure(separator.get(), separator->mVtable);
        moveBytes(destination, separator->mText, (u32)length, 0);
        destination[length] = 0;

        be<u8>* text = mName.mText;
        assure(&mName, mName.mVtable);
        s32 oldLength = textLength(mName.mText);
        s32 position = oldLength >= 0 ? oldLength : 0;
        assure(name, name->mVtable);
        s32 count = textLength(name->mText);
        s32 available = mName.mCapacity - position;
        if (count >= available) count = available - 1;
        if (count > 0) {
            assure(name, name->mVtable);
            moveBytes(text + position, name->mText, (u32)count, 0);
            s32 end = position + count;
            if (end > oldLength) text[end] = 0;
        }
    }
    gabi::FrameLocal<SafeText> meta(0x10);
    meta->mVtable = &sTextVtable;
    meta->mText.v = gabi::guest_str("Icon = DRAW, Order = -1");
    setNodeMeta(this, meta.get());
}

/* 02786064 */
void RootNode::setNoteMeta(u32 unusedFirst, u32 unusedSecond, void* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SafeText> meta(8);
    meta->mVtable = &sTextVtable;
    meta->mText.v = gabi::guest_str("Icon=NOTE");
    setNodeMeta(node, meta.get());
}

/* 027860A4 */
void RootNodeDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = &sDisposerVtable;
    if (this == sDisposer) {
        RootNode* node = sInstance;
        sDisposer = nullptr;
        gabi::call_ptr<void>(node->mVtable->mDestroy.mFunction, node, 2);
        sInstance = nullptr;
    }
    destroyRegisteredObject(this, 0);
    if (flags & 1) releaseObject(this);
}

/* ---- sead text instances of this translation unit ---- */

/* 0278614C */
void RootText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 02786160 */
void RootText::assureTermination() {}
/* 02786164 */
void RootTextBuffer::assureTermination() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 0278617C */
void RootTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 02786190 */
void RootNameBase::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027861A4 */
void RootName::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027861B8 */
void RootFormatTextBase::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027861CC */
void RootSpareText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}

/* 027861E0 */
RootFormatText* RootFormatText::createFormatted(RootFormatText* text, const char* format, u32 firstArgument,
        u32 secondArgument, u32 thirdArgument, u32 fourthArgument, u32 fifthArgument, u32 sixthArgument) {
    u32 entryStack = gabi::cpu->r[1];
    bool floatingArguments = gabi::cpu->cr[6] != 0;
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeFormatSave> save(8);
    save->mIntegerArguments[0] = gabi::ea(text);
    save->mIntegerArguments[1] = gabi::ea(format);
    save->mIntegerArguments[2] = firstArgument;
    save->mIntegerArguments[3] = secondArgument;
    save->mIntegerArguments[4] = thirdArgument;
    save->mIntegerArguments[5] = fourthArgument;
    save->mIntegerArguments[6] = fifthArgument;
    save->mIntegerArguments[7] = sixthArgument;
    if (floatingArguments) {
        for (u32 index = 0; index < 8; ++index) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[index + 1].ps0, sizeof(bits));
            save->mFloatingArgumentWords[index * 2] = bits >> 32;
            save->mFloatingArgumentWords[index * 2 + 1] = (u32)bits;
        }
    }
    if (text == nullptr) {
        text = static_cast<RootFormatText*>(allocateObject(sizeof(RootFormatText)));
        if (text == nullptr) return nullptr;
    }
    text->mText = &text->mStorage[0];
    text->mCapacity = 0x400;
    text->mStorage[0x3FF] = 0;
    be<u8>* first = text->mText;
    *first = 0;
    text->mVtable = &sFormatTextVtable;
    // the other two bytes of the packed register-state word are untouched GHS padding
    auto* indexes = gabi::at<be<u8>>(gabi::ea(&save->mArguments.mRegisterState));
    indexes[0] = 2;
    save->mArguments.mOverflowArguments = gabi::at<void>(entryStack + 8);
    save->mArguments.mRegisterArguments = &save->mIntegerArguments[0];
    indexes[1] = 0;
    text->formatArguments(format, &save->mArguments);
    return text;
}

/* 02786300 */
void RootFormatText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}

/* WWHD glue
data sInstance 101F8C1C
data sDisposer 101F8C20
data sPrivateResource 101F8C14
data sEmptyText 104A0CD8 nocheck
data sDisposerVtable 1016375C nocheck
data sRootNodeVtable 1016376C nocheck
data sTextVtable 10163664 nocheck
data sNameVtable 101636CC nocheck
data sNameBaseVtable 101636B4 nocheck
data sFormatTextVtable 10163714 nocheck
str 1016372C
str 10163734
str 10163738
str 10163750
ext 0273AD10 void* allocateObject(u32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273AF40 void releaseObject(void*)
ext 02752B0C sead::RegisteredObject* constructHeapDisposer(sead::RegisteredObject*,sead::RegistryHeap*,u32)
ext 02752BEC void destroyRegisteredObject(sead::RegisteredObject*,u32)
ext C0009988 void* moveBytes(void*,const void*,u32,u32)
ext 02759C10 s32 sead::format::ByteTextBuffer::formatArguments(const char*,sead::format::VariadicState*)
ext 02760084 sead::CriticalSection* constructCriticalSection(sead::CriticalSection*)
ext 02760168 void destroyCriticalSection(sead::CriticalSection*,u32)
*/
