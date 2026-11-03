// J3D HD renderer: shader resource name tables (attribute/uniform-block/sampler locations per program part).
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderResourceBindings.h"

using RenderResourceSpanConstruct = void (*)(j3d::RenderResourceSpan*);
void constructRenderResourceSpans(void*,u32,u32,RenderResourceSpanConstruct);
void initializeRenderResourceSpan(j3d::RenderResourceSpan*);
void* allocateRenderResourceContainer(u32,void*,s32);
void* allocateRenderResourceMember(u32);
void resolveRenderAttributeName(void*,void*);
void resolveRenderUniformName(void*,void*);
void resolveRenderSamplerName(void*,void*);

struct RenderResourceParts_l { u8 mPrefix[8]; gptr<void> mRoot; be<u32> mCount; gptr<u8> mRecords; };
struct RenderResourceGraph_l { u8 mPrefix[0x7C]; gptr<RenderResourceParts_l> mParts; };
struct RenderResourceName_l { be<u32> mIndex; sead::SafeText mName; be<s16> mLocation; u8 mPadding[2]; };
struct RenderResourceSlots_l { be<u32> mIndex; sead::SafeText mName; be<s16> mSlots[4]; };
struct RenderResourceNames_l { gptr<be<u8>> mNames[14]; };
struct RenderResourceUniformNames_l { gptr<be<u8>> mNames[5]; };
struct RenderResourceSamplerNames_l { gptr<be<u8>> mNames[15]; };
static RenderResourceNames_l sAttributeNames;
static RenderResourceUniformNames_l sUniformBlockNames;
static RenderResourceSamplerNames_l sSamplerNames;
static sead::SafeTextVtable sRenderResourceTextVtable;
using RenderTextByte = be<u8>;
static RenderTextByte sRenderAttributeDefaultText;
static RenderTextByte sRenderUniformDefaultText;
static RenderTextByte sRenderSamplerDefaultText;

// Returns the span's storage to the heap that owns it (no NULL test: callers test first).
static inline void freeResourceSpan(j3d::RenderResourceSpan& span) {
    auto* heap = findRegistryHeap(sRegistryManager.get(), span.mStorage);
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, span.mStorage.get());
    span.mCount = 0;
    span.mStorage = NULL;
}
static inline void releaseResourceSpan(j3d::RenderResourceSpan& span) {
    if (span.mStorage != NULL) freeResourceSpan(span);
}
// Destroys the attribute states of a span, then frees it. The storage is tested once, before the
// destructors run, as in the original.
static inline void releaseResourceAttributes(j3d::RenderResourceSpan& span) {
    if (span.mStorage != NULL) {
        s32 count = span.mCount;
        for (s32 index = 0; index < count; ++index) {
            auto* object = &static_cast<j3d::RenderAttributes_l*>(span.mStorage.get())[index];
            gabi::call_ptr<void>(object->mVtable->mDestroy, object, 2u);
            count = span.mCount;
        }
        freeResourceSpan(span);
    }
}
static inline void* allocateResourceBytes(u32 size, sead::RegistryHeap* heap) {
    if (heap == NULL) heap = registryCurrentHeap(sRegistryManager.get());
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, size, 4);
}

// Texture-matrix callback stored into matching material entries (only its address is used here).
void renderTextureMatrixCallbackAddress();
static sead::SafeTextVtable sRenderMaterialNameVtable;
static RenderTextByte sTextureMatrixNamePrefix;

/* 027FB168 */
void j3d::RenderMaterialNames_l::bindTextureMatrixCallbacks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 count = mCount;
    for (u32 index = 0; index < count; ++index) {
        s32 entriesOffset = mEntriesOffset;
        auto* entries = entriesOffset == 0 ? NULL : reinterpret_cast<RenderMaterialNameEntry_l*>(reinterpret_cast<u8*>(&mEntriesOffset) + entriesOffset);
        auto* entry = &entries[index];
        s32 nameOffset = entry->mNameOffset;
        be<u8>* text = nameOffset == 0 ? NULL : reinterpret_cast<be<u8>*>(reinterpret_cast<u8*>(&entry->mNameOffset) + nameOffset);
        gabi::FrameLocal<sead::SafeText> name(8);
        gabi::FrameLocal<sead::SafeText> prefix(0x10);
        name->mText = text;
        prefix->mVtable = &sRenderMaterialNameVtable;
        prefix->mText = &sTextureMatrixNamePrefix;
        name->mVtable = &sRenderMaterialNameVtable;
        RenderTextEntries::assureText();
        name->assureTermination();
        be<u8>* firstText = name->mText;
        prefix->assureTermination();
        bool equal = firstText == prefix->mText.get();
        if (!equal) {
            be<u8>* first = name->mText;
            be<u8>* second = prefix->mText;
            equal = true;
            for (u32 character = 0; character < 6; ++character) {
                u8 left = first[character];
                u8 right = second[character];
                if (left == 0 && right == 0) break;
                if (left != right) { equal = false; break; }
            }
        }
        if (equal) entry->mCallback = gabi::guest_fn(reinterpret_cast<const void*>(renderTextureMatrixCallbackAddress));
        count = mCount;
    }
}

/* 028003E8 */
void j3d::RenderGeometryBindings::prepare(RenderAttributeLocations_l* locations,
    RenderGeometryPool* geometry, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mLocations = NULL;
    releaseResourceAttributes(mSets[0]);
    releaseResourceAttributes(mSets[1]);
    mLocations = locations;
    s32 count = geometry->mGeometryCount;
    for (u32 set = 0; set < 2; ++set) {
        if (count > 0) {
            auto* allocated = static_cast<RenderAttributes_l*>(allocateResourceBytes((u32)count * sizeof(RenderAttributes_l), heap));
            for (s32 index = 0; index < count; ++index) {
                RenderAttributes_l* object = guestElement(allocated, index);
                if (object != NULL) constructRenderAttributes(object);
            }
            if (allocated != NULL) {
                mSets[set].mStorage = allocated;
                mSets[set].mCount = count;
            }
            count = geometry->mGeometryCount;
        }
    }
    for (u32 set = 0; set < 2; ++set) {
        for (u32 index = 0; index < (u32)count; ++index) {
            auto* attributes = static_cast<RenderAttributes_l*>(mSets[set].mStorage.get());
            if (index < (u32)mSets[set].mCount) attributes += index;
            auto* entry = static_cast<RenderGeometryEntry_l*>(geometry->mSets[set].mStorage.get());
            if (index < (u32)geometry->mSets[set].mCount) entry += index;
            RenderGeometryEntries::configureAttributes(locations, attributes, &entry->mVertexDescriptor, heap);
            count = geometry->mGeometryCount;
        }
    }
}

/* 0280068C */
void j3d::RenderResourceBindingGroups::prepare(RenderResourceBinding* resource, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* graph = static_cast<RenderResourceGraph_l*>(resource->mResource.get());
    s32 count = (u32)graph->mParts->mCount + 1u;
    if (mGroups.mStorage != NULL) {
        s32 oldCount = mGroups.mCount;
        for (s32 index = 0; index < oldCount; ++index) {
            auto* object = guestElement(static_cast<RenderGeometryBindings*>(mGroups.mStorage.get()), index);
            if (object != NULL) {
                object->mLocations = NULL;
                releaseResourceAttributes(object->mSets[0]);
                releaseResourceAttributes(object->mSets[1]);
                oldCount = mGroups.mCount;
            }
        }
        freeResourceSpan(mGroups);
    }
    if (count > 0) {
        auto* allocated = static_cast<RenderGeometryBindings*>(allocateResourceBytes((u32)count * sizeof(RenderGeometryBindings), heap));
        for (s32 index = 0; index < count; ++index) {
            auto* object = guestElement(allocated, index);
            if (object != NULL) {
                object->mLocations = NULL;
                constructRenderResourceSpans(object->mSets, 2, sizeof(RenderResourceSpan), initializeRenderResourceSpan);
            }
        }
        if (allocated != NULL) {
            mGroups.mStorage = allocated;
            mGroups.mCount = count;
        }
    }
    u32 remaining = (u32)count;
    u32 index = 0;
    while (remaining != 0) {
        auto* object = static_cast<RenderGeometryBindings*>(mGroups.mStorage.get());
        if (index < (u32)mGroups.mCount) object += index;
        // This first word is a resource-record reference until subsequent setup.
        RenderResourceBinding* selected = resource;
        RenderResourceChildren_l* children = resource->mChildren;
        if (children != NULL) {
            selected = NULL;
            if (index < (u32)children->mCount) selected = &children->mRecords.get()[index];
        }
        object->mLocations = reinterpret_cast<RenderAttributeLocations_l*>(selected);
        ++index;
        --remaining;
    }
    mCount = count;
}

/* 028009A0 */
void j3d::RenderResourceBinding::reset() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mResource = NULL;
    mActive = 0;
    releaseResourceSpan(mFields[0]);
    releaseResourceSpan(mFields[1]);
    releaseResourceSpan(mFields[2]);
    RenderResourceChildren_l* children = mChildren;
    if (children != NULL && mBorrowedChildren == 0 && children->mRecords != NULL) {
        s32 count = children->mCount;
        for (s32 index = 0; index < count; ++index) {
            auto* child = guestElement(children->mRecords.get(), index);
            if (child != NULL && child->mActive != 0) {
                child->reset();
                count = children->mCount;
            }
        }
        auto* heap = findRegistryHeap(sRegistryManager.get(), children->mRecords);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, children->mRecords.get());
        children->mCount = 0;
        children->mRecords = NULL;
    }
}

// Constructs the name text at +4 of a name-table entry. GHS allocates the text when its address
// wraps to NULL.
static inline void constructResourceName(void* item, RenderTextByte* defaultText) {
    auto* name = j3d::guestMember<sead::SafeText>(item, 4);
    if (name == NULL) name = static_cast<sead::SafeText*>(allocateRenderResourceMember(sizeof(sead::SafeText)));
    if (name != NULL) {
        name->mVtable = &sRenderResourceTextVtable;
        name->mText = defaultText;
    }
}

// Allocates a uniform-block or sampler name table: every location starts unresolved (-1).
static inline RenderResourceSlots_l* allocateResourceSlots(u32 count, RenderTextByte* defaultText, sead::RegistryHeap* heap) {
    auto* slots = static_cast<RenderResourceSlots_l*>(allocateResourceBytes(count * sizeof(RenderResourceSlots_l), heap));
    for (u32 index = 0; index < count; ++index) {
        RenderResourceSlots_l* item = j3d::guestElement(slots, index);
        if (item == NULL) continue;
        constructResourceName(item, defaultText);
        item->mSlots[1] = -1;
        item->mSlots[2] = -1;
        item->mSlots[0] = -1;
        item->mSlots[3] = -1;
    }
    return slots;
}

template <class T> static inline T* resourceItem(j3d::RenderResourceSpan& span, u32 index) {
    T* item = j3d::guestNull<T>();
    if (index < (u32)span.mCount) item = &static_cast<T*>(span.mStorage.get())[index];
    return item;
}

/* 02800B0C */
void j3d::RenderResourceBinding::activate(void* resource, sead::RegistryHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mResource = resource;
    mActive = 1;

    // Vertex attribute names.
    auto* attributes = static_cast<RenderResourceName_l*>(allocateResourceBytes(14 * sizeof(RenderResourceName_l), heap));
    for (u32 index = 0; index < 14; ++index) {
        RenderResourceName_l* item = guestElement(attributes, index);
        if (item == NULL) continue;
        constructResourceName(item, &sRenderAttributeDefaultText);
        item->mLocation = 0;
    }
    if (attributes != NULL) {
        mFields[0].mStorage = attributes;
        mFields[0].mCount = 14;
    }
    for (u32 index = 0; index < 14; ++index) {
        auto* item = resourceItem<RenderResourceName_l>(mFields[0], index);
        item->mName.mText = sAttributeNames.mNames[index];
        resolveRenderAttributeName(&item->mName, resource);
        item->mIndex = index;
    }

    // Uniform-block names.
    auto* uniforms = allocateResourceSlots(5, &sRenderUniformDefaultText, heap);
    if (uniforms != NULL) {
        mFields[1].mStorage = uniforms;
        mFields[1].mCount = 5;
    }
    for (u32 index = 0; index < 5; ++index) {
        auto* item = resourceItem<RenderResourceSlots_l>(mFields[1], index);
        item->mName.mText = sUniformBlockNames.mNames[index];
        resolveRenderUniformName(&item->mName, resource);
        item->mIndex = index;
    }

    // Sampler names.
    auto* samplers = allocateResourceSlots(15, &sRenderSamplerDefaultText, heap);
    if (samplers != NULL) {
        mFields[2].mStorage = samplers;
        mFields[2].mCount = 15;
    }
    for (u32 index = 0; index < 15; ++index) {
        auto* item = resourceItem<RenderResourceSlots_l>(mFields[2], index);
        item->mName.mText = sSamplerNames.mNames[index];
        resolveRenderSamplerName(&item->mName, resource);
        item->mIndex = index;
    }

    // One child binding per part (the root first), sharing one container.
    auto* graph = static_cast<RenderResourceGraph_l*>(resource);
    s32 childCount = (u32)graph->mParts->mCount + 1u;
    if (mChildren != NULL || childCount <= 1) return;
    auto* children = static_cast<RenderResourceChildren_l*>(allocateRenderResourceContainer(sizeof(RenderResourceChildren_l), heap, 4));
    if (children != NULL) {
        children->mCount = 0;
        children->mRecords = NULL;
    } else {
        // A failed container allocation is used without a test (the stores below reach address 0).
        children = guestNull<RenderResourceChildren_l>();
    }
    mChildren = children;
    auto* records = static_cast<RenderResourceBinding*>(allocateResourceBytes((u32)childCount * sizeof(RenderResourceBinding), heap));
    for (s32 index = 0; index < childCount; ++index) {
        RenderResourceBinding* child = guestElement(records, index);
        if (child == NULL) continue;
        for (u32 field = 0; field < 3; ++field) {
            auto* span = guestMember<RenderResourceSpan>(child, offsetof(RenderResourceBinding, mFields) + field * sizeof(RenderResourceSpan));
            if (span == NULL) span = static_cast<RenderResourceSpan*>(allocateRenderResourceMember(sizeof(RenderResourceSpan)));
            if (span != NULL) {
                span->mStorage = NULL;
                span->mCount = 0;
            }
        }
        child->mActive = 0;
        child->mChildren = NULL;
        child->mBorrowedChildren = 0;
    }
    if (records != NULL) {
        children->mRecords = records;
        children->mCount = childCount;
    }
    for (s32 index = 0; index < childCount; ++index) {
        RenderResourceChildren_l* shared = guestObject(mChildren);
        RenderResourceBinding* child = guestNull<RenderResourceBinding>();
        if ((u32)index < (u32)shared->mCount) child = &shared->mRecords.get()[index];
        child->mChildren = shared;
        child->mBorrowedChildren = 1;
        auto* parts = graph->mParts.get();
        void* childResource;
        if (index == 0) {
            childResource = parts->mRoot;
        } else {
            u8* selected = parts->mRecords;
            if ((u32)(index - 1) < (u32)parts->mCount) selected += (u32)(index - 1) * 0x84;
            childResource = selected;
        }
        child->activate(childResource, heap);
    }
}

/* WWHD glue
 data sRenderMaterialNameVtable 1016EC54
 data sTextureMatrixNamePrefix 1016EDE0
 ext 027FAA70 void renderTextureMatrixCallbackAddress() callback
 data sAttributeNames 101F9A54
 data sUniformBlockNames 101F9A8C
 data sSamplerNames 101F9AA0
 data sRenderResourceTextVtable 1016F098
 data sRenderAttributeDefaultText 1016F0E4
 data sRenderUniformDefaultText 1016F0F0
 data sRenderSamplerDefaultText 1016F0FC
 ext 0273B050 void* allocateRenderResourceContainer(u32,void*,s32)
 ext 0273AD10 void* allocateRenderResourceMember(u32)
 ext 028EFFD0 void constructRenderResourceSpans(void*,u32,u32,RenderResourceSpanConstruct)
 ext 02800960 void initializeRenderResourceSpan(j3d::RenderResourceSpan*) callback
 ext 027BDD2C void resolveRenderAttributeName(void*,void*)
 ext 027BDA2C void resolveRenderUniformName(void*,void*)
 ext 027BD72C void resolveRenderSamplerName(void*,void*)
*/
