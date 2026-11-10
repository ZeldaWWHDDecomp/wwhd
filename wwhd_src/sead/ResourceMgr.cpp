/* WWHD sead resource manager: construction with its default factory for direct resources, the singleton
 * and its disposer, registering and finding factories and decompressors by file extension, loading a
 * resource (through a decompressor chosen by the extension, or directly), the post-creation list, the
 * default factory's own functions, and this translation unit's copies of its text classes.
 * Reconstructed from this game's instructions. */
#include "sead/ResourceMgr.h"

using namespace sead;

static void* allocateObject(u32 size);
static void* allocateFromHeap(u32 size, RegistryHeap* heap, s32 alignment);
static void* allocateArrayFromHeap(u32 size, RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static void constructDisposer(Disposer* disposer);
static void constructHeapDisposer(Disposer* disposer, RegistryHeap* heap, u32 mode);
static void destroyDisposer(Disposer* disposer, u32 flags);
static void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kDestroyAll = 3;
static constexpr u32 kDisposerHeapMode = 3;

/* vtables of this translation unit */
static ResourceFactoryVtable sResourceFactoryVtable;
static DisposerVtable sResourceFactoryDisposerVtable;
static ResourceFactoryVtable sDirectFactoryVtable;
static DisposerVtable sDirectFactoryDisposerVtable;
static DisposerVtable sDirectFactoryBaseDisposerVtable;
static DisposerVtable sManagerDisposerVtable;
static SafeTextVtable sTextVtable;
static SafeTextVtable sExtensionTextVtable;

/* the singleton, its disposer and the heap manager */
static ResourceMgr* sResourceMgr;
static ResourceMgrDisposer* sResourceMgrDisposer;

/* the storage of a member object: its own address, or a new block when that address is NULL */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* inlined: length of a text, 0 when it runs past kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    if (text[0] != 0) {
        do {
            ++length;
            if (length > kMaxTextLength) return 0;
        } while (text[length] != 0);
    }
    return length;
}

/* inlined: text equality as the manager compares extensions */
static bool textEquals(SafeText* text, SafeText* other) {
    text->assureTermination();
    text->assureTermination();
    be<u8>* left = text->mText;
    other->assureTermination();
    if (left == other->mText) return true;
    const be<u8>* a = text->mText;
    const be<u8>* b = other->mText;
    for (s32 i = 0; i <= kMaxTextLength; ++i) {
        u8 ca = a[i];
        u8 cb = b[i];
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}

/* inlined: copy source into a fixed text, cut at its capacity */
static void copyText(SafeTextBuffer* destination, SafeText* source) {
    be<u8>* out = destination->mText;
    source->assureTermination();
    s32 length = textLength(source->mText);
    if (length >= destination->mCapacity) length = destination->mCapacity - 1;
    source->assureTermination();
    OSBlockMove(out, source->mText, length, 0);
    out[length] = 0;
}

/* inlined: an empty list */
static void initList(LinkList* list) {
    list->mNext = list;
    list->mCount = 0;
    list->mPrevious = list;
}

/* inlined: take a node out of the list it is in */
static void unlinkNode(TypedListNode* node, LinkList* list) {
    if (node->mList != NULL) {
        node->mList = NULL;
        node->detach();
        list->mCount = list->mCount - 1;
    }
}

/* 0275D674 */
void sead::ResourceMgr::unregisterDecompressor(Decompressor* decompressor) {
    unlinkNode(&decompressor->mNode, &mDecompressorList);
}

/* 0275D6C4 */
void sead::DirectResourceFactory::destroyFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<DirectResourceFactory*>(reinterpret_cast<u8*>(disposer) - offsetof(ResourceFactory, mDisposer))
        ->destroy(flags);
}

/* 0275D6CC */
void sead::ResourceMgr::registerFactory(ResourceFactory* factory, SafeText* extension) {
    copyText(&factory->mExtension, extension);
    LinkList* list = factory->mNode.mList;
    if (list != NULL) {
        factory->mNode.mList = NULL;
        factory->mNode.detach();
        list->mCount = list->mCount - 1;
    }
    factory->mNode.mList = &mFactories;
    mFactories.insertBefore(&factory->mNode);
    mFactories.mCount = mFactories.mCount + 1;
}

/* 0275D7D0 */
ResourceMgr* sead::ResourceMgr::construct(ResourceMgr* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (manager == NULL) {
        manager = static_cast<ResourceMgr*>(allocateObject(sizeof(ResourceMgr)));
        if (manager == NULL) return NULL;
    }
    LinkList* list = memberStorage(&manager->mFactories);
    if (list != NULL) initList(list);
    list = memberStorage(&manager->mPostCreateList);
    if (list != NULL) initList(list);
    list = memberStorage(&manager->mDecompressorList);
    if (list != NULL) initList(list);
    manager->mFallbackFactory = NULL;
    if (sRegistryManager == NULL) return manager;

    /* the default factory lives in the manager's heap */
    RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), manager);
    DirectResourceFactory* factory =
        static_cast<DirectResourceFactory*>(allocateFromHeap(sizeof(ResourceFactory), heap, 4));
    if (factory != NULL) {
        TypedListNode* node = memberStorage(&factory->mNode);
        if (node != NULL) {
            node->mNext = NULL;
            node->mData = factory;
            node->mPrevious = NULL;
            node->mList = NULL;
        }
        constructDisposer(&factory->mDisposer);
        factory->mVtable = &sResourceFactoryVtable;
        factory->mDisposer.mVtable = &sResourceFactoryDisposerVtable;
        FixedSafeText<32>* extension = memberStorage(&factory->mExtension);
        if (extension != NULL) {
            extension->mStorage[31] = 0;
            extension->mStorage[0] = 0;
            extension->mText = &extension->mStorage[0];
            extension->mVtable = &sExtensionTextVtable;
            extension->mCapacity = 32;
        }
        factory->mVtable = &sDirectFactoryVtable;
        factory->mDisposer.mVtable = &sDirectFactoryDisposerVtable;
        manager->mFallbackFactory = factory;
    } else {
        manager->mFallbackFactory = NULL;
    }
    gabi::FrameLocal<SafeText> anyExtension(8);
    anyExtension->mVtable = &sTextVtable;
    anyExtension->mText.v = gabi::guest_str("");
    manager->registerFactory(manager->mFallbackFactory, anyExtension.get());
    return manager;
}

/* 0275D9A0 */
ResourceMgr* sead::ResourceMgr::createInstance(RegistryHeap* heap) {
    if (sResourceMgr != NULL) return sResourceMgr;
    ResourceMgrDisposer* disposer =
        static_cast<ResourceMgrDisposer*>(allocateArrayFromHeap(sizeof(ResourceMgr), heap, 4));
    if (disposer != NULL) {
        constructHeapDisposer(disposer, heap, kDisposerHeapMode);
        disposer->mVtable = &sManagerDisposerVtable;
    }
    sResourceMgrDisposer = disposer;
    ResourceMgr* manager = NULL;
    if (disposer != NULL) manager = construct(reinterpret_cast<ResourceMgr*>(disposer));
    sResourceMgr = manager;
    return manager;
}

/* 0275DA34 */
void sead::ResourceMgr::destroy(u32 flags) {
    if (this == NULL) return;
    ResourceFactory* factory = mFallbackFactory;
    if (factory != NULL) {
        factory->dispatchDestroy(kDestroyAll);
        mFallbackFactory = NULL;
    }
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275DAA4 */
ResourceFactory* sead::ResourceMgr::findFactory(SafeText* extension) {
    for (Link* link = mFactories.mNext; link != &mFactories; link = link->mNext) {
        TypedListNode* node = static_cast<TypedListNode*>(link);
        ResourceFactory* factory = static_cast<ResourceFactory*>(node->mData.get());
        if (textEquals(&factory->mExtension, extension)) return static_cast<ResourceFactory*>(node->mData.get());
    }
    return mFallbackFactory;
}

/* 0275DB94 */
Resource* sead::ResourceMgr::tryLoadWithoutDecomp(ResourceLoadArg* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    ResourceFactory* factory = arg->mFactory;
    if (factory == NULL) {
        gabi::FrameLocal<FixedSafeText<32>> extension(8);
        extension->mStorage[31] = 0;
        extension->mText = &extension->mStorage[0];
        extension->mStorage[0] = 0;
        extension->mCapacity = 32;
        extension->mVtable = &sExtensionTextVtable;
        if (!path::getExtension(extension.get(), &arg->mPath)) {
            factory = mFallbackFactory;
            return factory->dispatchTryCreate(arg);
        }
        factory = findFactory(extension.get());
    }
    return factory->dispatchTryCreate(arg);
}

/* 0275DC4C */
Decompressor* sead::ResourceMgr::findDecompressor(SafeText* extension) {
    for (Link* link = mDecompressorList.mNext; link != &mDecompressorList; link = link->mNext) {
        TypedListNode* node = static_cast<TypedListNode*>(link);
        Decompressor* decompressor = static_cast<Decompressor*>(node->mData.get());
        if (textEquals(&decompressor->mExtension, extension)) return static_cast<Decompressor*>(node->mData.get());
    }
    return NULL;
}

/* 0275DD38 */
Resource* sead::ResourceMgr::tryLoad(ResourceLoadArg* arg, SafeText* convertExtension, Decompressor* decompressor) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SafeText> factoryExtension(8);
    gabi::FrameLocal<FixedSafeText<32>> extension(0x10);
    factoryExtension->mVtable = &sTextVtable;
    extension->mStorage[31] = 0;
    extension->mText = &extension->mStorage[0];
    extension->mStorage[0] = 0;
    extension->mCapacity = 32;
    extension->mVtable = &sExtensionTextVtable;
    if (decompressor == NULL) {
        factoryExtension->mText.v = gabi::guest_str("");
        if (!path::getExtension(extension.get(), &arg->mPath)) return NULL;
        decompressor = findDecompressor(extension.get());
        /* without a decompressor the factory goes by the file's own extension */
        if (decompressor == NULL) factoryExtension->mText = extension->mText;
        else factoryExtension->mText = convertExtension->mText;
    } else {
        factoryExtension->mText = convertExtension->mText;
    }
    ResourceFactory* factory = arg->mFactory;
    if (factory == NULL) factory = findFactory(factoryExtension.get());
    if (decompressor != NULL) return factory->dispatchTryCreateWithDecomp(arg, decompressor);
    return factory->dispatchTryCreate(arg);
}

/* 0275DE9C */
void sead::ResourceMgr::unregisterFactory(ResourceFactory* factory) {
    unlinkNode(&factory->mNode, &mFactories);
}

/* 0275DEEC */
void sead::ResourceMgr::postCreate() {
    Link* link;
    while ((link = mPostCreateList.mNext) != &mPostCreateList) {
        static_cast<Resource*>(static_cast<TypedListNode*>(link)->mData.get())->finishCreate();
    }
}

/* 0275DF34 */
void sead::ResourceMgr::removeFromPostCreateList(Resource* resource) {
    unlinkNode(&resource->mNode, &mPostCreateList);
}

/* 0275DF84 */
void sead::ResourceMgrDisposer::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sManagerDisposerVtable;
    if (this == sResourceMgrDisposer) {
        sResourceMgrDisposer = NULL;
        sResourceMgr->destroy(2);
        sResourceMgr = NULL;
    }
    destroyDisposer(this, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E020 */
void sead::resource_mgr_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E034 */
void sead::DirectResourceFactory::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sDirectFactoryBaseDisposerVtable;
    ResourceFactory::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E094 */
void sead::resource_mgr_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E0A8 */
void sead::resource_mgr_text::Text::assureTerminationImpl() {}

/* 0275E0AC */
void sead::resource_mgr_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E0C0 */
void sead::resource_mgr_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 0275E0D8 */
void sead::resource_mgr_text::ExtensionText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275E0EC */
DirectResource* sead::DirectResourceFactory::newResource(RegistryHeap* heap, s32 alignment) {
    DirectResource* resource = static_cast<DirectResource*>(allocateFromHeap(sizeof(DirectResource), heap, alignment));
    if (resource == NULL) return NULL;
    return DirectResource::construct(resource);
}

/* WWHD glue
 data sResourceFactoryVtable 1015DFE0 nocheck
 data sResourceFactoryDisposerVtable 1015E018 nocheck
 data sDirectFactoryVtable 1015E0A8 nocheck
 data sDirectFactoryDisposerVtable 1015E0E8 nocheck
 data sDirectFactoryBaseDisposerVtable 1015E028 nocheck
 data sManagerDisposerVtable 1015E0FC nocheck
 data sTextVtable 1015E048 nocheck
 data sExtensionTextVtable 1015E090 nocheck
 data sResourceMgr 101F8B68
 data sResourceMgrDisposer 101F8B6C
 str 1015E0F8 ""
 str 10000160 "" in 0275DD38
 ext 0273AD10 void* allocateObject(u32)
 ext 0273B050 void* allocateFromHeap(u32, sead::RegistryHeap*, s32)
 ext 0273B0D4 void* allocateArrayFromHeap(u32, sead::RegistryHeap*, s32)
 ext 0273AF40 void releaseObject(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02752B0C void constructHeapDisposer(sead::Disposer*, sead::RegistryHeap*, u32)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*, void*)
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 027426B0 bool sead::path::getExtension(sead::SafeTextBuffer*, sead::SafeText*)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
*/
