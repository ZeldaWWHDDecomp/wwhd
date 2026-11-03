/* WWHD sead resource manager (singleton): the factory and decompressor lists keyed by file extension, the
 * list of resources awaiting post-creation, the default factory for direct resources, and loading with
 * and without a decompressor.
 * Layout from this game's constructor (0x38 bytes, the singleton disposer in the first 0x10) and
 * instructions. */
#pragma once
#include "sead/Resource.h"

namespace sead {

struct ResourceMgr {
    Disposer mInstanceDisposer;   // constructed by createInstance before the manager
    LinkList mFactories;
    LinkList mPostCreateList;
    LinkList mDecompressorList;
    gptr<ResourceFactory> mFallbackFactory;

    static ResourceMgr* construct(ResourceMgr* manager);
    static ResourceMgr* createInstance(RegistryHeap* heap);
    void destroy(u32 flags);
    void registerFactory(ResourceFactory* factory, SafeText* extension);
    void unregisterFactory(ResourceFactory* factory);
    void unregisterDecompressor(Decompressor* decompressor);
    void removeFromPostCreateList(Resource* resource);
    void postCreate();
    ResourceFactory* findFactory(SafeText* extension);
    Decompressor* findDecompressor(SafeText* extension);
    Resource* tryLoadWithoutDecomp(ResourceLoadArg* arg);
    Resource* tryLoad(ResourceLoadArg* arg, SafeText* convertExtension, Decompressor* decompressor);
};

/* the singleton's disposer: destroys the manager with its heap */
struct ResourceMgrDisposer : Disposer {
    void destroy(u32 flags);
};

/* the factory for plain direct resources (the manager's default factory) */
struct DirectResourceFactory : DirectResourceFactoryBase {
    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);
    DirectResource* newResource(RegistryHeap* heap, s32 alignment);
};

/* per-TU copies of the manager translation unit's text classes */
namespace resource_mgr_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct ExtensionText : SafeTextBuffer { void destroy(u32 flags); };   // FixedSafeText<32>
}

}

WWHD_OFFSET(sead::ResourceMgr, mFactories, 0x10);
WWHD_OFFSET(sead::ResourceMgr, mPostCreateList, 0x1C);
WWHD_OFFSET(sead::ResourceMgr, mDecompressorList, 0x28);
WWHD_OFFSET(sead::ResourceMgr, mFallbackFactory, 0x34);
WWHD_SIZE(sead::ResourceMgr, 0x38);
