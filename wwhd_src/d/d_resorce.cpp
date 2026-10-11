/** Resource archive, heap and loader management.
 * HD: SafeString names and relative BFRES resources use pooled archive/path maps.
 * Verified against the HD implementation; all 81 entry points and call boundaries retained.
 */
#include "d/d_resorce.h"
using StringBuffer128 = resorce_hd::ResourceStringBuffer128HD;
using ResourceSafeStringRaw = resorce_hd::ResourceSafeStringHD;
static resorce_hd::HeaderStatics l_resourceHeaderStatics;
static resorce_hd::DestructorRecords l_resourceDestructors;
static resorce_hd::ResourceBounds l_resourceBounds;
static resorce_hd::ResourceTransferMethodsHD l_resourceTransferMethods = {
    {resorce_hd::DIRECT_METHOD, resorce_hd::ARCHIVE_TRANSFER_METHOD},
    {resorce_hd::DIRECT_METHOD, resorce_hd::TEXTURE_TRANSFER_METHOD}};
static resorce_hd::ResourceControlGlobalsHD l_resourceControlGlobals;
static resorce_hd::ResourceObjectSlotHD l_resourceCacheManager;
static resorce_hd::ResourceObjectSlotHD l_resourceArchiveLoader;
static resorce_hd::ResourceObjectSlotHD l_resourceHeapManager;
static s32 l_resourceTableCount = 629;
static u32 l_resourceIdTableCount = 626;
static f32 l_resourceInitialFrame = 0.0f;
/* 02603720 */
void resorce_hd::ResourceControlHD::destroy(u32 flags) {
    u32 self = gabi::ea(this);
    if (!self) {
        return;
    }
    __vtbl = resorce_hd::CONTROL_VTABLE;
    if (flags & 1) {
        resourceFree(self);
    }
}
/* 026066A0 */
u32 resorce_hd::ResourceArchiveInfoHD::getRes(u32 index) {
    if (s32(index) >= mResCount) {
        return 0;
    }
    gptr<void> *resources = mRes;
    return resources[index].v;
}
/* 026067D0 */
u32 resorce_hd::ResourceArchiveInfoHD::getIDRes(u32 id) {
    if (s32(id) >= mIdResCount) {
        return 0;
    }
    gptr<void> *resources = mIdRes;
    return resources[id].v;
}

/* 026082D0 */
void resorce_hd::ResourceArchiveInfoHD::setRes(s32 index, u32 value) {
    s32 count = mResCount;
    if (index < count) {
        gptr<void> *resources = mRes;
        resources[u32(index)].v = value;
    }
}
/* 026082EC */
void resorce_hd::ResourceArchiveInfoHD::setIDRes(s32 index, u32 value) {
    s32 count = mIdResCount;
    if (index < count) {
        gptr<void> *resources = mIdRes;
        resources[u32(index)].v = value;
    }
}
/* 02603680 */
static u32 Initialize(u32 heap) {
    u32 self = l_resourceControlGlobals.mpControl.v.get();
    if (!self) {
        u32 allocated = resourceAllocateArray(0x2130u, heap, 4u);
        u32 disposer = allocated + 4;
        if (disposer) {
            resourceDisposerConstructOnHeap(disposer, heap, 3u);
            gabi::at<resorce_hd::ResourceDisposerHD>(disposer)->__vtbl =
                resorce_hd::CONTROL_DISPOSER_VTABLE;
        }
        l_resourceControlGlobals.mpDisposer.v = disposer;
        self = allocated ? resourceCallControlCtor(allocated) : allocated;
        l_resourceControlGlobals.mpControl.v = self;
    }
    return self;
}
/* 02608198 */
static void DisposeSingleton(u32 self, u32 flags) {
    if (!self) {
        return;
    }
    gabi::at<resorce_hd::ResourceDisposerHD>(self)->__vtbl = resorce_hd::CONTROL_DISPOSER_VTABLE;
    if (self == l_resourceControlGlobals.mpDisposer.v.get()) {
        u32 control = l_resourceControlGlobals.mpControl.v.get();
        l_resourceControlGlobals.mpDisposer.v = 0;
        resourceCallControlDestructor(control, 2u);
        l_resourceControlGlobals.mpControl.v = 0;
    }
    resourceDisposer_destroy(gabi::at<void>(self), 0u);
    if (flags & 1) {
        resourceFree(self);
    }
}
/* 0260827C */
static void DisposeResourceInfo(u32 self, u32 flags) {
    if (!self) {
        return;
    }
    resourceDisposer_destroy(gabi::at<void>(self), 0u);
    if (flags & 1) {
        resourceFree(self);
    }
}

/* 02608234 */
static void TransferArchiveResources(u32 first, u32 unused, u32 second) {
    auto *infoRef = gabi::at<gptr<resorce_hd::ResourceArchiveInfoHD>>(second);
    auto *info = infoRef->get();
    auto *archive = gabi::at<resorce_hd::ResourceArchiveHD>(info->mpArchive.v);
    u32 from = archive->mpHeader.v;
    auto *archiveRef = gabi::at<gptr<resorce_hd::ResourceArchiveHD>>(first);
    auto *root = archiveRef->get();
    if (!from) {
        return;
    }
    u32 to = root->mpHeader.v;
    resourceTransferArchiveData(to, from, 1u);
}
/* 02608258 */
static void TransferTextureResources(u32 first, u32 unused, u32 second) {
    auto *archiveRef = gabi::at<gptr<resorce_hd::ResourceArchiveHD>>(first);
    auto *root = archiveRef->get();
    u32 from = root->mpHeader.v;
    auto *infoRef = gabi::at<gptr<resorce_hd::ResourceArchiveInfoHD>>(second);
    auto *info = infoRef->get();
    u32 archive = info->mpArchive.v;
    if (!from) {
        return;
    }
    u32 to = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    resourceTransferArchiveData(to, from, 1u);
}

/* 0260344C */
static u32 ControlCtor(u32 self) {
    if (!self) {
        self = resourceAllocate(0x2130u);
    }
    if (!self) {
        return self;
    }
    auto *control = gabi::at<resorce_hd::ResourceControlHD>(self);
    control->__vtbl = resorce_hd::CONTROL_VTABLE;
    for (u32 offset : {u32(offsetof(resorce_hd::ResourceControlHD, mArchiveInfo)),
                       u32(offsetof(resorce_hd::ResourceControlHD, mPathInfo))}) {
        u32 pool = self + offset;
        if (!pool) {
            pool = resourceAllocate(0x1014u);
        }
        if (pool) {
            u32 data = pool + 20;
            gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v = 0;
            gabi::at<resorce_hd::ResourcePool>(pool)->mCount = 0;
            gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v = data;
            gabi::at<resorce_hd::ResourcePool>(pool)->mCapacity = 128;
            for (u32 i = 0; i < 127; ++i) {
                gabi::at<resorce_hd::ResourceNode>(data + i * 32)->mLeft.v = data + (i + 1) * 32;
            }
            gabi::at<resorce_hd::ResourceNode>(data + 127 * 32)->mLeft.v = 0;
            gabi::at<resorce_hd::ResourcePool>(pool)->mStorage.v = data;
        }
    }
    control->mpObjectHeap.v = 0;
    control->mpResourceHeap.v = 0;
    control->mpObjectParentHeap.v = 0;
    control->mpStageHeap.v = 0;
    control->mpObjectSpecialHeap.v = 0;
    control->mpStageSpecialHeap.v = 0;
    control->mpStageParentHeap.v = 0;
    u32 first = self + 0x2058, second = self + 0x2068, third = self + 0x2078,
        fourth = self + 0x20D0;
    if (!first) {
        resourceAllocate(16u);
    }
    if (!second) {
        resourceAllocate(16u);
    }
    if (!third) {
        resourceAllocate(88u);
    }
    if (!fourth) {
        resourceAllocate(88u);
    }
    control->mPendingCount = 0;
    control->mLoading = 0;
    for (u32 i = 0; i < 4; ++i) {
        gabi::at<gptr<void>>(first)[i].v = 0;
    }
    for (u32 i = 0; i < 4; ++i) {
        gabi::at<gptr<void>>(second)[i].v = 0;
    }
    for (u32 i = 0; i < 22; ++i) {
        gabi::at<gptr<void>>(third)[i].v = 0;
    }
    for (u32 i = 0; i < 22; ++i) {
        gabi::at<gptr<void>>(fourth)[i].v = 0;
    }
    return self;
}
/* 026055E4 */
void resorce_hd::ResourcePathInfoHD::allocatePool(s32 count, u32 heap) {
    if (count <= 0) {
        return;
    }
    u32 data = resourceAllocateArray(u32(count) * 32, heap, 4u);
    if (!data) {
        return;
    }
    mResources.mCapacity = count;
    mResources.mFree.v = data;
    for (s32 i = 0; i < count - 1; ++i) {
        gabi::at<resorce_hd::ResourceNode>(data + u32(i) * 32)->mLeft.v = data + u32(i + 1) * 32;
    }
    gabi::at<resorce_hd::ResourceNode>(data + u32(count) * 32 - 32)->mLeft.v = 0;
    mResources.mStorage.v = data;
}
static void StringAssure(u32 str) {
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get();
    gabi::call_ptr<void>(
        gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get(), str);
}
static u32 StringHash(u32 str) {
    StringAssure(str);
    u32 saved = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
    StringAssure(str);
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
    u32 length = 0;
    while (gabi::at<be<u8>>(data)[length].get()) {
        ++length;
        if (length > 0x40000) {
            length = 0;
            break;
        }
    }
    return resourceHashString(saved, length);
}
/* 026066C4 */
u32 resorce_hd::ResourceControlHD::getRes(u32 name, u32 index) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20u),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return resourceCallGetIndex(info, index);
}
/* 026067F4 */
u32 resorce_hd::ResourceControlHD::getIDRes(u32 name, u32 index) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20u),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return resourceCallGetId(info, index);
}
/* 02606AF8 */
u32 resorce_hd::ResourcePathInfoHD::getRes(u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mResources.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 172u),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return info;
}
/* 0260524C */
u32 resorce_hd::ResourceControlHD::getPathArchive(u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mPathInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 4136u),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mpArchive.v;
}
/* 026065A8 */
u32 resorce_hd::ResourceControlHD::getArchive(u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mpArchive.v;
}
/* 0260702C */
u32 resorce_hd::ResourceControlHD::getResByName(u32 index, u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return resourceCallResourceByFilename(info, index);
}

static u32 Length(u32 data) {
    u32 n = 0;
    while (gabi::at<be<u8>>(data)[n].get()) {
        if (++n > 0x40000) {
            return 0;
        }
    }
    return n;
}
/* 02606D74 */
u32 resorce_hd::ResourceArchiveInfoHD::getResByFilename(u32 name) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get()), data = str + 12;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl =
        resorce_hd::RAW_RESOURCE_STRING_VTABLE;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
    gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = data;
    StringAssure(name);
    u32 n = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get());
    u32 capacity = gabi::at<resorce_hd::BufferedString>(str)->mCapacity.get();
    u32 sourceVt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target =
        gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(sourceVt)->mAssureTermination.get();
    if (s32(n) >= s32(capacity)) {
        n = capacity - 1;
    }
    gabi::call_ptr<void>(target, name);
    u32 source = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    resourceCopyBytes(data, source, n, 0u);
    gabi::at<be<u8>>(data)[n] = 0;
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    if (!gabi::at<be<u8>>(data)->get()) {
        return 0;
    }
    gabi::Local<ResourceSafeStringRaw> suffix;
    suffix->mStringTop = gabi::guest_addr(".bas");
    suffix->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    resourceTerminateString(str);
    u32 oldLen = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    u32 offset = oldLen;
    u32 suffixVt = suffix->__vtbl.get();
    u32 suffixTarget =
        gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(suffixVt)->mAssureTermination.get();
    if (s32(offset) < 0) {
        offset = 0;
    }
    gabi::call_ptr<void>(suffixTarget, suffix.get());
    u32 appendLen = Length(suffix->mStringTop.get());
    u32 left = gabi::at<resorce_hd::BufferedString>(str)->mCapacity.get() - offset;
    if (s32(appendLen) >= s32(left)) {
        appendLen = left - 1;
    }
    if (s32(appendLen) > 0) {
        suffixVt = suffix->__vtbl.get();
        suffixTarget =
            gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(suffixVt)->mAssureTermination.get();
        u32 dest = data + offset;
        gabi::call_ptr<void>(suffixTarget, suffix.get());
        source = suffix->mStringTop.get();
        resourceCopyBytes(dest, source, appendLen, 0u);
        u32 total = offset + appendLen;
        if (s32(total) > s32(oldLen)) {
            gabi::at<be<u8>>(data)[total] = 0;
        }
    }
    u32 hash = StringHash(str);
    u32 root = mExtraResources.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(info + 0x68),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    return gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
}

/* 02604470 */
void resorce_hd::ResourceArchiveInfoHD::allocateRes(s32 count, u32 heap) {
    u32 data = resourceAllocateArray(u32(count) * 4, heap, 4u);
    mRes.v = data;
    mResCount = count;
    for (s32 i = 0; i < count; ++i) {
        gptr<void> *resources = mRes;
        resources[u32(i)].v = 0;
    }
}
/* 026044E0 */
void resorce_hd::ResourceArchiveInfoHD::allocateIDRes(s32 count, u32 heap) {
    u32 data = resourceAllocateArray(u32(count) * 4, heap, 4u);
    mIdRes.v = data;
    mIdResCount = count;
    for (s32 i = 0; i < count; ++i) {
        gptr<void> *resources = mIdRes;
        resources[u32(i)].v = 0;
    }
}

static u32 Relative(u32 address) {
    u32 offset = gabi::at<be<u32>>(address)->get();
    return offset ? address + offset : 0;
}
/* 0260990C */
void resorce_hd::ResourceArchiveInfoHD::loadRawResource(u32 index, u32 id, u32 name) {
    u32 info = gabi::ea(this);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    s32 found = resourceFindName(entries, data);
    if (found < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    found = resourceFindName(entries, data);
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + u32(found) * 16)
                        ->mResource.getAddress();
    u32 resource = Relative(reference);
    resourceCallSetResourceIndex(info, index, resource);
    resource = Relative(reference);
    resourceCallSetResourceId(info, id, resource);
}
/* 02609E54 */
void resorce_hd::ResourceArchiveInfoHD::loadDemoScript(u32 index, u32 id, u32 name) {
    u32 info = gabi::ea(this);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    s32 found = resourceFindName(entries, data);
    if (found < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    found = resourceFindName(entries, data);
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + u32(found) * 16)
                        ->mResource.getAddress();
    u32 resource = Relative(reference);
    resourceCallSetResourceIndex(info, index, resource);
    resource = Relative(reference);
    resourceCallSetResourceId(info, id, resource);
}
/* 02609F84 */
void resorce_hd::ResourceArchiveInfoHD::loadData(u32 index, u32 id, u32 name) {
    u32 info = gabi::ea(this);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    s32 found = resourceFindName(entries, data);
    if (found < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    found = resourceFindName(entries, data);
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + u32(found) * 16)
                        ->mResource.getAddress();
    u32 resource = Relative(reference);
    resourceCallSetResourceIndex(info, index, resource);
    resource = Relative(reference);
    resourceCallSetResourceId(info, id, resource);
}
/* 0260A5F8 */
void resorce_hd::ResourceArchiveInfoHD::loadClothVertices(u32 index, u32 id, u32 name) {
    u32 info = gabi::ea(this);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    s32 found = resourceFindName(entries, data);
    if (found < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    found = resourceFindName(entries, data);
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + u32(found) * 16)
                        ->mResource.getAddress();
    u32 resource = Relative(reference);
    resourceCallSetResourceIndex(info, index, resource);
    resource = Relative(reference);
    resourceCallSetResourceId(info, id, resource);
}

/* 02603ACC */
void resorce_hd::ResourceControlHD::createStageHeaps() {
    if (mpStageHeap.v) {
        return;
    }
    gabi::Local<ResourceSafeStringRaw> name;
    name->mStringTop = 269359856u;
    name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    u32 parent = mpStageParentHeap.v;
    u32 root = resourceCreateExpHeap(1572864u, name.get(), parent, 0xFFFFFFFFu, 0u);
    name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    name->mStringTop = 269359872u;
    mpStageHeap.v = root;
    u32 sub = resourceCreateExpHeap(51200u, name.get(), root, 1u, 0u);
    mpStageSpecialHeap.v = sub;
    gabi::Local<ResourceSafeStringRaw> group;
    for (u32 i = 0; i < 4; ++i) {
        group->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        u32 heap = mpStageHeap.v;
        group->mStringTop = 269359884u;
        u32 result = resourceCreateExpHeap(51200u, group.get(), heap, 1u, 0u);
        mStageGroupHeaps[i].v = result;
    }
    gabi::Local<ResourceSafeStringRaw> archive;
    for (u32 i = 0; i < 22; ++i) {
        archive->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        u32 heap = mpStageHeap.v;
        archive->mStringTop = 269359896u;
        u32 result = resourceCreateExpHeap(30720u, archive.get(), heap, 1u, 0u);
        mStageArchiveHeaps[i].v = result;
    }
}
/* 026079B4 */
void resorce_hd::ResourceControlHD::createObjectHeaps() {
    if (mpObjectHeap.v) {
        return;
    }
    gabi::Local<ResourceSafeStringRaw> name;
    name->mStringTop = 269360084u;
    name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    u32 parent = mpObjectParentHeap.v;
    u32 root = resourceCreateExpHeap(15728640u, name.get(), parent, 0xFFFFFFFFu, 0u);
    name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    name->mStringTop = 269360072u;
    mpObjectHeap.v = root;
    u32 sub = resourceCreateExpHeap(1048576u, name.get(), root, 1u, 0u);
    mpObjectSpecialHeap.v = sub;
    gabi::Local<ResourceSafeStringRaw> group;
    for (u32 i = 0; i < 4; ++i) {
        group->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        u32 heap = mpObjectHeap.v;
        group->mStringTop = 269360064u;
        u32 result = resourceCreateExpHeap(786432u, group.get(), heap, 1u, 0u);
        mObjectGroupHeaps[i].v = result;
    }
    gabi::Local<ResourceSafeStringRaw> archive;
    for (u32 i = 0; i < 22; ++i) {
        archive->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        u32 heap = mpObjectHeap.v;
        archive->mStringTop = 269360100u;
        u32 result = resourceCreateExpHeap(440320u, archive.get(), heap, 1u, 0u);
        mObjectArchiveHeaps[i].v = result;
    }
}
/* 026097C0 */
void resorce_hd::ResourceArchiveInfoHD::loadCollision(u32 index, u32 id, u32 name) {
    u32 info = gabi::ea(this);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    s32 found = resourceFindName(entries, data);
    if (found < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, name);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    data = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    found = resourceFindName(entries, data);
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
    u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + u32(found) * 16)
                        ->mResource.getAddress();
    u32 resource = Relative(reference);
    resourceConvertCollision(resource);
    resource = Relative(reference);
    resourceCallSetResourceIndex(info, index, resource);
    resource = Relative(reference);
    resourceCallSetResourceId(info, id, resource);
}
/* 0260566C */
void resorce_hd::ResourcePathInfoHD::setRes(u32 name, u32 resource) {
    u32 info = gabi::ea(this);
    u32 hash = StringHash(name);
    s32 capacity = s32(mResources.mCapacity);
    s32 count = s32(mResources.mCount);
    u32 pool = info + 0xAC;
    if (count < capacity) {
        u32 node = gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v;
        if (node) {
            u32 next = gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v;
            gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v = next;
        }
        if (node) {
            gabi::at<resorce_hd::ResourceNode>(node)->mKey = hash;
            gabi::at<resorce_hd::ResourceNode>(node)->mpPool.v = pool;
            gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v = 0;
            gabi::at<resorce_hd::ResourceNode>(node)->mRed = 1;
            gabi::at<resorce_hd::ResourceNode>(node)->mHash = hash;
            gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = resource;
            gabi::at<resorce_hd::ResourceNode>(node)->__vtbl.v = resorce_hd::VALUE_NODE_VTABLE;
            gabi::at<resorce_hd::ResourceNode>(node)->mRight.v = 0;
        }
        u32 oldCount = gabi::at<resorce_hd::ResourcePool>(pool)->mCount;
        u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
        gabi::at<resorce_hd::ResourcePool>(pool)->mCount = oldCount + 1;
        root = gabi::ea(resourceInsertNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                           gabi::at<resorce_hd::ResourceNode>(root),
                                           gabi::at<resorce_hd::ResourceNode>(node)));
        gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v = root;
        gabi::at<resorce_hd::ResourceNode>(root)->mRed = 0;
    } else {
        u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
        gabi::Local<be<u32>> key;
        *key = hash;
        u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                             gabi::at<resorce_hd::ResourceNode>(root), key.get()));
        if (node && node + 24) {
            gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = resource;
        }
    }
}
static bool EqualBounded(u32 a, u32 b, bool leftInR4 = false) {
    for (u32 i = 0; i < 0x40001; ++i) {
        if (leftInR4) {
            gabi::cpu->r[4] = a + i;
        }
        u8 x = gabi::at<be<u8>>(a)[i].get(), y = gabi::at<be<u8>>(b)[i].get();
        if (x != y) {
            return false;
        }
        if (!x) {
            return true;
        }
    }
    return false;
}
/* 02606900 */
u32 resorce_hd::ResourceControlHD::getObjectResName2Index(u32 archiveName, u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(archiveName);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !(node + 24)) {
        return 0;
    }
    s32 count = l_resourceTableCount;
    s32 found = -1;
    if (count <= 0) {
        return 0;
    }
    u32 table = 0x1009BB10;
    do {
        table += 12;
        if (hash == gabi::at<resorce_hd::ResourceTableRowHD>(table)->mHash.get()) {
            s32 names = s32(gabi::at<resorce_hd::ResourceTableRowHD>(table)->mCount);
            if (names > 0) {
                u32 array = gabi::at<resorce_hd::ResourceTableRowHD>(table)->mValues.v.get();
                for (s32 index = 0; index < names; ++index) {
                    gabi::Local<ResourceSafeStringRaw> candidate;
                    candidate->mStringTop =
                        gabi::at<resorce_hd::ResourceTableEntryHD>(array + u32(index) * 8)
                            ->mpName.v.get();
                    candidate->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
                    StringAssure(name);
                    StringAssure(name);
                    u32 saved = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
                    StringAssure(gabi::ea(candidate.get()));
                    u32 other = candidate->mStringTop.get();
                    if (saved == other) {
                        found = index;
                        break;
                    }
                    u32 actual = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
                    other = candidate->mStringTop.get();
                    if (EqualBounded(actual, other)) {
                        found = index;
                        break;
                    }
                }
            }
        }
    } while (--count);
    if (found <= 0) {
        return 0;
    }
    u32 info = gabi::at<resorce_hd::ResourceNode>(node)->mValue.v;
    return resourceCallGetIndex(info, found);
}
/* 02607138 */
void resorce_hd::ResourceControlHD::deleteRes(u32 name) {
    u32 self = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 root = mArchiveInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 first = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                          gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (first) {
        first += 24;
    }
    *key = hash;
    gabi::Local<be<u32>> keyAgain;
    *keyAgain = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                         gabi::at<resorce_hd::ResourceNode>(root), keyAgain.get()));
    if (node && node + 24) {
        u32 changed =
            gabi::ea(resourceDeleteNode(gabi::at<resorce_hd::ResourcePool>(self + 20),
                                        gabi::at<resorce_hd::ResourceNode>(root), key.get()));
        mArchiveInfo.mPool.mRoot.v = changed;
        if (changed) {
            gabi::at<resorce_hd::ResourceNode>(changed)->mRed = 0;
        }
    }
    u32 loader = l_resourceCacheManager.mpObject.v.get();
    resourceReleaseCachedArchive(loader, hash);
    u32 info = gabi::at<gptr<void>>(first)->v.get();
    u32 heap = gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mpParentHeap.v.get();
    if (heap) {
        heap = gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mpParentHeap.v.get();
        auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
        gabi::call_ptr<void>(vt->mDestroy.get(), heap);
        info = gabi::at<gptr<void>>(first)->v.get();
    }
    heap = gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mDataHeap.v.get();
    auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
    gabi::call_ptr<void>(vt->mDestroy.get(), heap);
}
static void DestroyHeap(u32 heap) {
    auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
    gabi::call_ptr<void>(vt->mDestroy.get(), heap);
}
/* 026072AC */
void resorce_hd::ResourceControlHD::destroyObjectHeaps() {
    if (!mpObjectHeap.v) {
        return;
    }
    DestroyHeap(mpObjectSpecialHeap.v);
    mpObjectSpecialHeap.v = 0;
    for (u32 i = 0; i < 4; ++i) {
        DestroyHeap(mObjectGroupHeaps[i].v);
        mObjectGroupHeaps[i].v = 0;
    }
    for (u32 i = 0; i < 22; ++i) {
        DestroyHeap(mObjectArchiveHeaps[i].v);
        mObjectArchiveHeaps[i].v = 0;
    }
    DestroyHeap(mpObjectHeap.v);
    mpObjectHeap.v = 0;
}
/* 02607414 */
void resorce_hd::ResourceControlHD::destroyStageHeaps() {
    if (!mpStageHeap.v) {
        return;
    }
    DestroyHeap(mpStageSpecialHeap.v);
    mpStageSpecialHeap.v = 0;
    for (u32 i = 0; i < 4; ++i) {
        DestroyHeap(mStageGroupHeaps[i].v);
        mStageGroupHeaps[i].v = 0;
    }
    for (u32 i = 0; i < 22; ++i) {
        DestroyHeap(mStageArchiveHeaps[i].v);
        mStageArchiveHeaps[i].v = 0;
    }
    DestroyHeap(mpStageHeap.v);
    mpStageHeap.v = 0;
}

static void InitializeString64(u32 object, u32 vt) {
    u32 where = object;
    if (!where) {
        where = resourceAllocate(12u);
    }
    if (where) {
        gabi::at<resorce_hd::ResourceSafeStringHD>(where)->__vtbl =
            resorce_hd::BUFFERED_STRING_VTABLE;
        gabi::at<resorce_hd::ResourceSafeStringHD>(where)->mStringTop = object + 12;
        gabi::at<resorce_hd::BufferedString>(where)->mCapacity = 64;
        gabi::at<resorce_hd::FixedString64>(object)->mBuffer[63] = 0;
    }
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(object)->mStringTop.get();
    gabi::at<resorce_hd::ResourceSafeStringHD>(object)->__vtbl = resorce_hd::FIXED_STRING_VTABLE;
    *gabi::at<be<u8>>(data) = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(object)->__vtbl = vt;
}
static void CopyString(u32 destination, u32 source, u32 capacityAddress, s32 assureR4 = -1,
                       u32 capacityRegister = 0) {
    StringAssure(source);
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(source)->mStringTop.get());
    u32 assureFourth = length ? 0u : gabi::cpu->r[4];
    u32 capacity = gabi::at<be<u32>>(capacityAddress)->get();
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(source)->__vtbl.get();
    u32 target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    if (s32(length) >= s32(capacity)) {
        length = capacity - 1;
    }
    // Inline capacity loads remain live at the second assure callback.
    if (capacityRegister) {
        gabi::cpu->r[capacityRegister] = capacity;
    }
    if (assureR4 >= 0) {
        gabi::call_ptr<void>(target, source, assureR4 ? capacity : assureFourth);
    } else {
        gabi::call_ptr<void>(target, source);
    }
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(source)->mStringTop.get();
    resourceCopyBytes(destination, data, length, 0u);
    gabi::at<be<u8>>(destination)[length] = 0;
}
/* 02605344 */
static u32 InfoCtor(u32 self, u32 archiveName, u32 path, u32 unused, u32 parent, u32 heap) {
    if (!self) {
        self = resourceAllocate(200u);
    }
    if (!self) {
        return self;
    }
    resourceDisposerConstruct(self);
    u32 first = self + 16;
    gabi::at<resorce_hd::ResourceDisposerHD>(self)->__vtbl = resorce_hd::PATH_DISPOSER_VTABLE;
    if (!first) {
        first = resourceAllocate(76u);
    }
    if (first) {
        InitializeString64(first, resorce_hd::RESOURCE_NAME_VTABLE);
    }
    u32 rawSecond = self + 0x5C, second = rawSecond;
    if (!second) {
        second = resourceAllocate(76u);
    }
    if (second) {
        InitializeString64(second, resorce_hd::RESOURCE_NAME_VTABLE);
    }
    gabi::at<resorce_hd::ResourcePathInfoHD>(self)->mpArchive.v = unused;
    u32 pool = self + 0xAC;
    if (!pool) {
        pool = resourceAllocate(20u);
    }
    if (pool) {
        gabi::at<resorce_hd::ResourcePool>(pool)->mCapacity = 0;
        gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v = 0;
        gabi::at<resorce_hd::ResourcePool>(pool)->mStorage.v = 0;
        gabi::at<resorce_hd::ResourcePool>(pool)->mCount = 0;
        gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v = 0;
    }
    gabi::at<resorce_hd::ResourcePathInfoHD>(self)->mDataHeap.v = heap;
    gabi::at<resorce_hd::ResourcePathInfoHD>(self)->mpParentHeap.v = parent;
    u32 destination = gabi::at<resorce_hd::ResourcePathInfoHD>(self)->mArchiveName.mStringTop;
    CopyString(destination, archiveName, self + 24, -1, 5);
    destination = gabi::at<resorce_hd::ResourceSafeStringHD>(rawSecond)->mStringTop.get();
    CopyString(destination, path, rawSecond + 8, -1, 6);
    u32 archive = gabi::at<resorce_hd::ResourcePathInfoHD>(self)->mpArchive.v;
    if (archive) {
        gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpArchiveName.v = rawSecond;
    }
    return self;
}
static void StripExtension(u32 buffer, u32 name, const char *literal, s32 assureR4 = -1) {
    u32 data = buffer + 12;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(buffer)->mBuffer[127] = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop = data;
    gabi::at<resorce_hd::BufferedString>(buffer)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->__vtbl =
        resorce_hd::RAW_RESOURCE_STRING_VTABLE;
    CopyString(data, name, buffer + 8, assureR4);
    gabi::Local<ResourceSafeStringRaw> suffix;
    u32 suffixAddress = gabi::ea(suffix.get());
    suffix->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    suffix->mStringTop = gabi::guest_addr(literal);
    gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    resourceTerminateString(buffer);
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get());
    StringAssure(suffixAddress);
    u32 suffixLength =
        Length(gabi::at<resorce_hd::ResourceSafeStringHD>(suffixAddress)->mStringTop.get());
    s32 position = s32(length - suffixLength);
    if (position >= 0) {
        u32 remaining = position + 1;
        if (position - 1 < 0) {
            remaining = 1;
        }
        for (;;) {
            gabi::Local<ResourceSafeStringRaw> part;
            u32 partAddress = gabi::ea(part.get());
            part->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
            part->mStringTop =
                gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get() +
                u32(position);
            resourceAssureSafeString(partAddress);
            StringAssure(partAddress);
            u32 left = gabi::at<resorce_hd::ResourceSafeStringHD>(partAddress)->mStringTop.get();
            StringAssure(suffixAddress);
            u32 right = gabi::at<resorce_hd::ResourceSafeStringHD>(suffixAddress)->mStringTop.get();
            bool equal = left == right || s32(suffixLength) <= 0;
            if (!equal) {
                equal = true;
                for (u32 i = 0; i < suffixLength; ++i) {
                    u8 a = gabi::at<be<u8>>(left)[i].get();
                    if (!a) {
                        equal = !gabi::at<be<u8>>(right)[i].get();
                        break;
                    }
                    u8 b = gabi::at<be<u8>>(right)[i].get();
                    if (!b || a != b) {
                        equal = false;
                        break;
                    }
                }
            }
            if (equal) {
                break;
            }
            --remaining;
            --position;
            if (!remaining) {
                break;
            }
        }
    }
    if (position < 0) {
        StringAssure(buffer);
        position =
            s32(Length(gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get()) + 1);
    }
    if (position >= s32(gabi::at<resorce_hd::BufferedString>(buffer)->mCapacity)) {
        StringAssure(buffer);
        Length(gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get());
    } else {
        u32 destination = gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get();
        if (position < 0) {
            position = 0;
        }
        gabi::at<be<u8>>(destination)[u32(position)] = 0;
    }
}
/* 02608F80 */
void resorce_hd::ResourceArchiveInfoHD::loadTextureTransformAnimation(u32 index, u32 id, u32 name,
                                                                      u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".");
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureTransforms.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureTransforms.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 animation = resourceAllocateOnHeap(16u, heap, 4u);
    if (animation) {
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFlags = 0;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->__vtbl =
            resorce_hd::TEXTURE_TRANSFORM_VTABLE;
        f32 initial = l_resourceInitialFrame;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = 0;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFrame = initial;
    }
    gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = resource;
    resourceCallSetResourceIndex(info, index, animation);
    resourceCallSetResourceId(info, id, animation);
}
/* 026093A0 */
void resorce_hd::ResourceArchiveInfoHD::loadTevRegAnimation(u32 index, u32 id, u32 name, u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".");
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTevColors.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTevColors.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 animation = resourceAllocateOnHeap(16u, heap, 4u);
    if (animation) {
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFlags = 0;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->__vtbl =
            resorce_hd::TEV_REG_ANIMATION_VTABLE;
        f32 initial = l_resourceInitialFrame;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = 0;
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFrame = initial;
    }
    gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = resource;
    resourceCallSetResourceIndex(info, index, animation);
    resourceCallSetResourceId(info, id, animation);
}
/* 02609A3C */
void resorce_hd::ResourceArchiveInfoHD::loadTexture(u32 index, u32 id, u32 name, u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".");
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextures.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextures.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 texture = resourceAllocateOnHeap(36u, heap, 4u);
    if (texture) {
        gabi::at<resorce_hd::ResourceTextureHD>(texture)->mpResource.v = resource;
        u32 width = gabi::at<resorce_hd::ResourceTextureHeaderHD>(resource)->mWidth;
        gabi::at<resorce_hd::ResourceTextureHD>(texture)->mWidth = width;
        u32 height = gabi::at<resorce_hd::ResourceTextureHeaderHD>(resource)->mHeight;
        gabi::at<resorce_hd::ResourceTextureHD>(texture)->mFlags = 0;
        gabi::at<resorce_hd::ResourceTextureHD>(texture)->mHeight = height;
    }
    resourceCallSetResourceIndex(info, index, texture);
    resourceCallSetResourceId(info, id, texture);
}
/* 02608308 */
void resorce_hd::ResourceArchiveInfoHD::loadModel(u32 index, u32 id, u32 name, u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".");
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mModels.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mModels.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 resourceId = resourceArchiveGetId(archive, found);
    archive = mpArchive.v;
    u32 archiveName = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpArchiveName.v;
    StringAssure(archiveName);
    gabi::Local<ResourceSafeStringRaw> copy;
    copy->mStringTop = gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get();
    copy->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    u32 result = resourceCreateModel(resource, resourceId, copy.get(), heap);
    resourceCallSetResourceIndex(info, index, result);
    resourceCallSetResourceId(info, id, result);
}
/* 0260873C */
void resorce_hd::ResourceArchiveInfoHD::loadSkeletalAnimation(u32 index, u32 id, u32 name,
                                                              u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".", 1);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mSkeletalAnimations.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get();
    target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mSkeletalAnimations.getAddress();
    found = resourceFindName(entries,
                             gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mSkeletalAnimations.getAddress();
    archive = mpArchive.v;
    u32 archiveName = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpArchiveName.v;
    u32 reference = entries + found * 16 + 36;
    u32 nameVt = gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->__vtbl.get();
    u32 resource = Relative(reference);
    u32 nameTarget =
        gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(nameVt)->mAssureTermination.get();
    gabi::call_ptr<void>(nameTarget, archiveName);
    gabi::Local<ResourceSafeStringRaw> copy;
    copy->mStringTop = gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get();
    copy->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    u32 result = resourceCreateSkeletalAnimation(resource, copy.get(), heap);
    resourceCallSetResourceIndex(info, index, result);
    resourceCallSetResourceId(info, id, result);
}
/* 02608B90 */
void resorce_hd::ResourceArchiveInfoHD::loadTexturePatternAnimation(u32 index, u32 id, u32 name,
                                                                    u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".", 0);
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTexturePatterns.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTexturePatterns.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 result = resourceCreateTexturePatternAnimation(resource, heap);
    resourceCallSetResourceIndex(info, index, result);
    resourceCallSetResourceId(info, id, result);
}
/* 026042AC */
static u32 ArchiveInfoCtor(u32 self, u32 name, u32 archive, u32 parent, u32 heap) {
    if (!self) {
        self = resourceAllocate(140u);
    }
    if (!self) {
        return self;
    }
    resourceDisposerConstruct(self);
    u32 rawString = self + 16;
    gabi::at<resorce_hd::ResourceDisposerHD>(self)->__vtbl = resorce_hd::ARCHIVE_DISPOSER_VTABLE;
    u32 str = rawString;
    if (!str) {
        str = resourceAllocate(76u);
    }
    if (str) {
        InitializeString64(str, resorce_hd::RESOURCE_NAME_VTABLE);
    }
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mRes.v = 0;
    u32 tree = self + 104;
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mIdRes.v = 0;
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mpArchive.v = archive;
    if (!tree) {
        tree = resourceAllocate(20u);
    }
    if (tree) {
        gabi::at<resorce_hd::ResourcePool>(tree)->mCapacity = 0;
        gabi::at<resorce_hd::ResourcePool>(tree)->mRoot.v = 0;
        gabi::at<resorce_hd::ResourcePool>(tree)->mStorage.v = 0;
        gabi::at<resorce_hd::ResourcePool>(tree)->mCount = 0;
        gabi::at<resorce_hd::ResourcePool>(tree)->mFree.v = 0;
    }
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mpParentHeap.v = parent;
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mIdResCount = 0;
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mResCount = 0;
    gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mDataHeap.v = heap;
    u32 nameVt = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->__vtbl.get(),
        target =
            gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(nameVt)->mAssureTermination.get();
    u32 destination = gabi::at<resorce_hd::ResourceSafeStringHD>(rawString)->mStringTop.get();
    gabi::call_ptr<void>(target, name);
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get());
    u32 capacity = gabi::at<resorce_hd::BufferedString>(rawString)->mCapacity.get();
    if (s32(length) >= s32(capacity)) {
        length = capacity - 1;
    }
    StringAssure(name);
    resourceCopyBytes(destination,
                      gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get(), length,
                      0u);
    gabi::at<be<u8>>(destination)[length] = 0;
    archive = gabi::at<resorce_hd::ResourceArchiveInfoHD>(self)->mpArchive.v;
    if (archive) {
        gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpArchiveName.v = rawString;
    }
    return self;
}
/* 02604094 */
u32 resorce_hd::ResourceControlHD::selectStageHeap(u32 name, u32 path, u32 heap) {
    u32 control = gabi::ea(this);
    if (!resourceCallNeedsSeparateHeap(control, name, path, heap)) {
        return mpStageParentHeap.v;
    }
    if (!mpStageHeap.v) {
        resourceCallCreateStageHeaps(control);
    }
    gabi::Local<ResourceSafeStringRaw> needle;
    u32 needleAddress = gabi::ea(needle.get());
    needle->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    needle->mStringTop = gabi::guest_addr("LOD");
    StringAssure(name);
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get());
    StringAssure(needleAddress);
    u32 needleLength =
        Length(gabi::at<resorce_hd::ResourceSafeStringHD>(needleAddress)->mStringTop.get());
    s32 end = s32(length - needleLength), found = -1;
    if (end >= 0) {
        for (s32 i = 0; i <= end; ++i) {
            gabi::Local<ResourceSafeStringRaw> part;
            u32 partAddress = gabi::ea(part.get());
            part->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
            part->mStringTop =
                gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get() + u32(i);
            resourceAssureSafeString(partAddress);
            StringAssure(partAddress);
            u32 left = gabi::at<resorce_hd::ResourceSafeStringHD>(partAddress)->mStringTop.get();
            StringAssure(needleAddress);
            u32 right = gabi::at<resorce_hd::ResourceSafeStringHD>(needleAddress)->mStringTop.get();
            bool equal = left == right || s32(needleLength) <= 0;
            if (!equal) {
                equal = true;
                for (u32 j = 0; j < needleLength; ++j) {
                    u8 a = gabi::at<be<u8>>(left)[j].get();
                    if (!a) {
                        equal = !gabi::at<be<u8>>(right)[j].get();
                        break;
                    }
                    u8 b = gabi::at<be<u8>>(right)[j].get();
                    if (!b || a != b) {
                        equal = false;
                        break;
                    }
                }
            }
            if (equal) {
                found = i;
                break;
            }
        }
    }
    if (found < 0) {
        return mpStageHeap.v;
    }
    return resourceCallChooseSpecialHeap(control, name);
}
static s32 FindSubstring(u32 name, const char *literal, bool assure = true) {
    gabi::Local<ResourceSafeStringRaw> needle;
    u32 needleAddress = gabi::ea(needle.get());
    needle->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    needle->mStringTop = gabi::guest_addr(literal);
    if (assure) {
        StringAssure(name);
    }
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get());
    StringAssure(needleAddress);
    u32 needleLength =
        Length(gabi::at<resorce_hd::ResourceSafeStringHD>(needleAddress)->mStringTop.get());
    s32 end = s32(length - needleLength), found = -1;
    if (end >= 0) {
        for (s32 i = 0; i <= end; ++i) {
            gabi::Local<ResourceSafeStringRaw> part;
            u32 partAddress = gabi::ea(part.get());
            part->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
            part->mStringTop =
                gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get() + u32(i);
            resourceAssureSafeString(partAddress);
            StringAssure(partAddress);
            u32 left = gabi::at<resorce_hd::ResourceSafeStringHD>(partAddress)->mStringTop.get();
            StringAssure(needleAddress);
            u32 right = gabi::at<resorce_hd::ResourceSafeStringHD>(needleAddress)->mStringTop.get();
            bool equal = left == right || s32(needleLength) <= 0;
            if (!equal) {
                equal = true;
                for (u32 j = 0; j < needleLength; ++j) {
                    u8 a = gabi::at<be<u8>>(left)[j].get();
                    if (!a) {
                        equal = !gabi::at<be<u8>>(right)[j].get();
                        break;
                    }
                    u8 b = gabi::at<be<u8>>(right)[j].get();
                    if (!b || a != b) {
                        equal = false;
                        break;
                    }
                }
            }
            if (equal) {
                found = i;
                break;
            }
        }
    }
    return found;
}
static bool LiteralEquals(u32 name, const char *literal, bool leftInR4 = false) {
    gabi::Local<ResourceSafeStringRaw> comparison;
    u32 object = gabi::ea(comparison.get());
    comparison->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    comparison->mStringTop = gabi::guest_addr(literal);
    StringAssure(name);
    StringAssure(name);
    u32 saved = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    StringAssure(object);
    u32 other = gabi::at<resorce_hd::ResourceSafeStringHD>(object)->mStringTop.get();
    if (saved == other) {
        return true;
    }
    return EqualBounded(gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get(), other,
                        leftInR4);
}
/* 02603740 */
u32 resorce_hd::ResourceControlHD::needsSeparateHeap(u32 name, u32 path) {
    if (!LiteralEquals(path, "sea", true)) {
        return LiteralEquals(path, "gy");
    }
    if (FindSubstring(name, "LOD") >= 0) {
        return 1;
    }
    return LiteralEquals(name, "Room0");
}
/* 02603C84 */
u32 resorce_hd::ResourceControlHD::selectStageArchiveHeap(u32 name) {
    if (LiteralEquals(name, "LOD26")) {
        return mpStageSpecialHeap.v;
    }
    if (LiteralEquals(name, "LOD01")) {
        return mStageGroupHeaps[0].v;
    }
    if (LiteralEquals(name, "LOD11")) {
        return mStageGroupHeaps[1].v;
    }
    if (LiteralEquals(name, "LOD13")) {
        return mStageGroupHeaps[2].v;
    }
    if (LiteralEquals(name, "LOD44")) {
        return mStageGroupHeaps[3].v;
    }
    for (u32 i = 0; i < 22; ++i) {
        u32 heap = mStageArchiveHeaps[i].v;
        auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
        u32 target = vt->mGetFreeSize.get();
        if (gabi::call_ptr<u32>(target, heap) > 0x5000) {
            return mStageArchiveHeaps[i].v;
        }
    }
    return mpStageHeap.v;
}
static void StorePool(u32 pool, u32 hash, u32 resource, u32 nodeVtable) {
    s32 capacity = s32(gabi::at<resorce_hd::ResourcePool>(pool)->mCapacity);
    s32 count = s32(gabi::at<resorce_hd::ResourcePool>(pool)->mCount);
    if (count < capacity) {
        u32 node = gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v;
        if (node) {
            u32 next = gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v;
            gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v = next;
        }
        if (node) {
            gabi::at<resorce_hd::ResourceNode>(node)->mKey = hash;
            gabi::at<resorce_hd::ResourceNode>(node)->mpPool.v = pool;
            gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v = 0;
            gabi::at<resorce_hd::ResourceNode>(node)->mRed = 1;
            gabi::at<resorce_hd::ResourceNode>(node)->mHash = hash;
            gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = resource;
            gabi::at<resorce_hd::ResourceNode>(node)->__vtbl.v = nodeVtable;
            gabi::at<resorce_hd::ResourceNode>(node)->mRight.v = 0;
        }
        u32 oldCount = gabi::at<resorce_hd::ResourcePool>(pool)->mCount;
        u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
        gabi::at<resorce_hd::ResourcePool>(pool)->mCount = oldCount + 1;
        root = gabi::ea(resourceInsertNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                           gabi::at<resorce_hd::ResourceNode>(root),
                                           gabi::at<resorce_hd::ResourceNode>(node)));
        gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v = root;
        gabi::at<resorce_hd::ResourceNode>(root)->mRed = 0;
    } else {
        u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
        gabi::Local<be<u32>> key;
        *key = hash;
        u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                             gabi::at<resorce_hd::ResourceNode>(root), key.get()));
        if (node && node + 24) {
            gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = resource;
        }
    }
}
/* 0260623C */
u32 resorce_hd::ResourceControlHD::setRes(u32 archiveName, u32 path, u32 parent) {
    u32 control = gabi::ea(this);
    u32 parentHeap = resourceCallSelectHeap(control, archiveName, path, parent);
    StringAssure(archiveName);
    gabi::Local<ResourceSafeStringRaw> copy;
    copy->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    copy->mStringTop = gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get();
    u32 heap = resourceCreateSolidHeap(0u, copy.get(), parentHeap, 1u, 0u);
    u32 archive = resourceAllocateOnHeap(36u, heap, 4u);
    if (archive) {
        archive = resourceConstructArchive(archive);
    }
    if (!archive) {
        DestroyHeap(heap);
        return 0;
    }
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[0] = 0;
    gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = str + 12;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    StringAssure(path);
    u32 pathChars = gabi::at<resorce_hd::ResourceSafeStringHD>(path)->mStringTop.get();
    StringAssure(archiveName);
    resourceFormatPath(str, "%s_%s", pathChars,
                       gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get());
    u32 loaded = resourceLoadArchiveFile(l_resourceArchiveLoader.mpObject.v.get(), str);
    if (!loaded) {
        DestroyHeap(heap);
        return 0;
    }
    u32 oldHeap = 1;
    if (heap) {
        oldHeap = resourceSwitchCurrentHeap(l_resourceHeapManager.mpObject.v.get(), heap);
    }
    resourceMountPathArchive(archive, loaded, archiveName, heap);
    u32 info = resourceAllocateOnHeap(200u, heap, 4u);
    if (info) {
        info = resourceCallInfoCtor(info, path, archiveName, archive, parent, heap);
    }
    u32 hash = StringHash(str);
    resourceCallLoadAllResources(info);
    StorePool(control + 0x1028, hash, info, resorce_hd::PATH_NODE_VTABLE);
    if (parent) {
        gabi::at<resorce_hd::ResourceHeapHD>(parent)->mpName.v =
            gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mArchivePath.mStringTop.get();
    }
    u32 value = gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mArchivePath.mStringTop.get();
    auto *heapVt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
    gabi::at<resorce_hd::ResourceHeapHD>(heap)->mpName.v = value;
    u32 target = heapVt->mAdjust.get();
    gabi::call_ptr<void>(target, heap);
    if (oldHeap != 1) {
        resourceSwitchCurrentHeap(l_resourceHeapManager.mpObject.v.get(), oldHeap);
    }
    return 1;
}
/* 02604EB4 */
u32 resorce_hd::ResourceControlHD::setArchive(u32 name, u32 parent, u32 arg6) {
    u32 control = gabi::ea(this);
    u32 parentHeap = resourceCallSelectHeap(control, name, name, arg6);
    StringAssure(name);
    gabi::Local<ResourceSafeStringRaw> copy;
    copy->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    copy->mStringTop = gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get();
    u32 heap = resourceCreateSolidHeap(0u, copy.get(), parentHeap, 1u, 0u);
    u32 archive = resourceAllocateOnHeap(36u, heap, 4u);
    if (archive) {
        archive = resourceConstructArchive(archive);
    }
    if (!archive) {
        DestroyHeap(heap);
        return 0;
    }
    u32 loaded = resourceLoadArchiveFile(l_resourceArchiveLoader.mpObject.v.get(), name);
    if (!loaded) {
        DestroyHeap(heap);
        return 0;
    }
    u32 oldHeap = 1;
    if (heap) {
        oldHeap = resourceSwitchCurrentHeap(l_resourceHeapManager.mpObject.v.get(), heap);
    }
    resourceMountArchive(archive, loaded, name, heap);
    u32 info = resourceAllocateOnHeap(140u, heap, 4u);
    if (info) {
        info = resourceCallArchiveInfoCtor(info, name, archive, parent, heap);
    }
    u32 hash = StringHash(name);
    resourceCallLoadTableResources(info, name, heap);
    resourceCallLoadExtraResources(info, heap);
    u32 pool = control + 20;
    StorePool(pool, hash, info, resorce_hd::ARCHIVE_NODE_VTABLE);
    u32 parentValue = 0;
    if (parent) {
        parentValue =
            gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mArchiveName.mStringTop.get();
        gabi::at<resorce_hd::ResourceHeapHD>(parent)->mpName.v = parentValue;
    }
    u32 value = gabi::at<resorce_hd::ResourceArchiveInfoHD>(info)->mArchiveName.mStringTop.get();
    auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
    gabi::at<resorce_hd::ResourceHeapHD>(heap)->mpName.v = value;
    u32 resizeTarget = vt->mAdjust.get();
    // The inlined parent assignment leaves its value live in r8 at the virtual resize.
    if (parent) {
        gabi::cpu->r[8] = parentValue;
    }
    gabi::call_ptr<void>(resizeTarget, heap);
    gabi::Local<resorce_hd::ResourceTransferReceiverHD> receiverA;
    u32 receiverAAddress = gabi::ea(receiverA.get());
    receiverA->mpArchive.v = archive;
    receiverA->mpSelf.v = receiverAAddress;
    gabi::Local<resorce_hd::ResourceMethodWordsHD> memberA;
    memberA->mWords[0] = l_resourceTransferMethods.mArchive[0].get();
    memberA->mWords[1] = l_resourceTransferMethods.mArchive[1].get();
    gabi::Local<resorce_hd::ResourceCallback> callbackA;
    resourceConstructArchiveCallback(reinterpret_cast<be<u32> *>(callbackA.get()),
                                     receiverAAddress + 12,
                                     reinterpret_cast<be<u32> *>(memberA.get()));
    u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
    if (root) {
        resourceVisitArchive(gabi::at<resorce_hd::ResourceNode>(root),
                             reinterpret_cast<resorce_hd::ResourceCallback *>(callbackA.get()));
    }
    gabi::Local<resorce_hd::ResourceTransferReceiverHD> receiverB;
    u32 receiverBAddress = gabi::ea(receiverB.get());
    receiverB->mpArchive.v = archive;
    receiverB->mpSelf.v = receiverBAddress;
    gabi::Local<resorce_hd::ResourceMethodWordsHD> memberB;
    memberB->mWords[0] = l_resourceTransferMethods.mTexture[0].get();
    memberB->mWords[1] = l_resourceTransferMethods.mTexture[1].get();
    gabi::Local<resorce_hd::ResourceCallback> callbackB;
    resourceConstructTextureCallback(reinterpret_cast<be<u32> *>(callbackB.get()),
                                     receiverBAddress + 12,
                                     reinterpret_cast<be<u32> *>(memberB.get()));
    root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
    if (root) {
        resourceVisitTexture(gabi::at<resorce_hd::ResourceNode>(root),
                             reinterpret_cast<resorce_hd::ResourceCallback *>(callbackB.get()));
    }
    if (oldHeap != 1) {
        resourceSwitchCurrentHeap(l_resourceHeapManager.mpObject.v.get(), oldHeap);
    }
    return 1;
}
/* 02606BEC */
u32 resorce_hd::ResourceControlHD::getPathRes(u32 archiveName, u32 path, u32 resourceName) {
    u32 control = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = str + 12;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[0] = 0;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
    gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    StringAssure(path);
    u32 pathChars = gabi::at<resorce_hd::ResourceSafeStringHD>(path)->mStringTop.get();
    StringAssure(archiveName);
    resourceFormatPath(str, "%s_%s", pathChars,
                       gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get());
    u32 hash = StringHash(str);
    u32 root = mPathInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(control + 0x1028),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    if (!node || !u32(node + 24)) {
        return 0;
    }
    return resourceCallGetPathResource(gabi::at<resorce_hd::ResourceNode>(node)->mValue.v,
                                       resourceName);
}
/* 02607F80 */
u32 resorce_hd::ResourceControlHD::selectObjectHeap(u32 name, u32 path, u32 heap) {
    u32 control = gabi::ea(this);
    if (!resourceCallNeedsSeparateHeap(control, name, path, heap)) {
        return mpObjectParentHeap.v;
    }
    if (!mpObjectHeap.v) {
        resourceCallCreateObjectHeaps(control);
    }
    if (FindSubstring(name, "LOD") < 0) {
        return mpObjectHeap.v;
    }
    return resourceCallChooseObjectHeap(control, name);
}
/* 02607B68 */
u32 resorce_hd::ResourceControlHD::selectObjectArchiveHeap(u32 name) {
    if (LiteralEquals(name, "LOD26")) {
        return mpObjectSpecialHeap.v;
    }
    if (LiteralEquals(name, "LOD01")) {
        return mObjectGroupHeaps[0].v;
    }
    if (LiteralEquals(name, "LOD11")) {
        return mObjectGroupHeaps[1].v;
    }
    if (LiteralEquals(name, "LOD13")) {
        return mObjectGroupHeaps[2].v;
    }
    if (LiteralEquals(name, "LOD44")) {
        return mObjectGroupHeaps[3].v;
    }
    for (u32 i = 0; i < 22; ++i) {
        u32 heap = mObjectArchiveHeaps[i].v;
        auto *vt = gabi::at<resorce_hd::ResourceHeapHD>(heap)->__vtbl.get();
        u32 target = vt->mGetFreeSize.get();
        if (gabi::call_ptr<u32>(target, heap) > 0x69000) {
            return mObjectArchiveHeaps[i].v;
        }
    }
    return mpObjectHeap.v;
}
/* 0260757C */
void resorce_hd::ResourceControlHD::deletePathRes(u32 name, u32 path) {
    u32 control = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[0] = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = str + 12;
    StringAssure(path);
    u32 pathChars = gabi::at<resorce_hd::ResourceSafeStringHD>(path)->mStringTop.get();
    StringAssure(name);
    resourceFormatPath(str, "%s_%s", pathChars,
                       gabi::at<resorce_hd::ResourceSafeStringHD>(name)->mStringTop.get());
    u32 hash = StringHash(str);
    u32 root = mPathInfo.mPool.mRoot.v;
    gabi::Local<be<u32>> key;
    *key = hash;
    u32 node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(control + 0x1028),
                                         gabi::at<resorce_hd::ResourceNode>(root), key.get()));
    u32 infoAddress = node ? node + 24 : 0;
    if (!infoAddress) {
        if (FindSubstring(name, "LOD") >= 0) {
            return;
        }
        root = mPathInfo.mPool.mRoot.v;
    }
    *key = hash;
    gabi::Local<be<u32>> again;
    *again = hash;
    node = gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(control + 0x1028),
                                     gabi::at<resorce_hd::ResourceNode>(root), again.get()));
    if (node && node + 24) {
        root = gabi::ea(resourceDeleteNode(gabi::at<resorce_hd::ResourcePool>(control + 0x1028),
                                           gabi::at<resorce_hd::ResourceNode>(root), key.get()));
        mPathInfo.mPool.mRoot.v = root;
        if (root) {
            gabi::at<resorce_hd::ResourceNode>(root)->mRed = 0;
        }
    }
    resourceReleaseCachedArchive(l_resourceCacheManager.mpObject.v.get(), hash);
    u32 info = gabi::at<gptr<void>>(infoAddress)->v.get();
    u32 heap = gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mpParentHeap.v.get();
    if (heap) {
        heap = gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mpParentHeap.v.get();
        DestroyHeap(heap);
        info = gabi::at<gptr<void>>(infoAddress)->v.get();
    }
    DestroyHeap(gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mDataHeap.v.get());
    if (LiteralEquals(name, "Stage")) {
        resourceCallDestroyObjectHeaps(control);
        resourceCallDestroyStageHeaps(control);
    }
}
static void AppendString(u32 buffer, u32 source) {
    u32 data = gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop.get();
    u32 oldLength = Length(data), offset = oldLength;
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(source)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    if (s32(offset) < 0) {
        offset = 0;
    }
    gabi::call_ptr<void>(target, source);
    u32 length = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(source)->mStringTop.get());
    u32 left = gabi::at<resorce_hd::BufferedString>(buffer)->mCapacity.get() - offset;
    if (s32(length) >= s32(left)) {
        length = left - 1;
    }
    if (s32(length) > 0) {
        vt = gabi::at<resorce_hd::ResourceSafeStringHD>(source)->__vtbl.get();
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
        u32 destination = data + offset;
        gabi::call_ptr<void>(target, source);
        resourceCopyBytes(destination,
                          gabi::at<resorce_hd::ResourceSafeStringHD>(source)->mStringTop.get(),
                          length, 0u);
        u32 total = offset + length;
        if (s32(total) > s32(oldLength)) {
            gabi::at<be<u8>>(data)[total] = 0;
        }
    }
}
/* 02604550 */
void resorce_hd::ResourceArchiveInfoHD::loadResourceTable(u32 name, u32 heap) {
    u32 info = gabi::ea(this);
    u32 hash = StringHash(name);
    u32 idTableCount = l_resourceIdTableCount;
    u32 resourceCount = 0, resourceRow = 0, idCount = 0, idRow = 0;
    s32 tableCount = l_resourceTableCount;
    u32 resourceTable = 0x1009BB1C, idTable = 0x100DE8F4;
    for (s32 i = 0; i < tableCount; ++i) {
        auto *row = gabi::at<resorce_hd::ResourceTableRowHD>(resourceTable + u32(i) * 12);
        if (row->mHash == hash) {
            resourceCount = row->mCount;
            resourceRow = i;
            break;
        }
    }
    for (s32 i = 0; i < s32(idTableCount); ++i) {
        auto *row = gabi::at<resorce_hd::ResourceTableRowHD>(idTable + u32(i) * 12);
        if (row->mHash == hash) {
            idCount = row->mCount;
            idRow = i;
            break;
        }
    }
    resourceCallAllocateResourceIndices(info, resourceCount, heap);
    resourceCallAllocateResourceIds(info, idCount, heap);
    if (s32(resourceCount) <= 0) {
        return;
    }
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    for (u32 i = 0; i < resourceCount; ++i) {
        gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
        gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl =
            resorce_hd::RESOURCE_STRING_VTABLE;
        gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
        gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = str + 12;
        gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[0] = 0;
        u32 values =
            gabi::at<resorce_hd::ResourceTableRowHD>(resourceTable + resourceRow * 12)->mValues.v;
        gabi::Local<ResourceSafeStringRaw> resource;
        resource->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        resource->mStringTop = gabi::at<resorce_hd::ResourceTableEntryHD>(values + i * 8)->mpName.v;
        resourceTerminateString(str);
        AppendString(str, gabi::ea(resource.get()));
        u32 chars = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
        values =
            gabi::at<resorce_hd::ResourceTableRowHD>(resourceTable + resourceRow * 12)->mValues.v;
        u8 first = gabi::at<be<u8>>(chars)->get();
        u32 type = gabi::at<resorce_hd::ResourceTableEntryHD>(values + i * 8)->mType;
        if (!first) {
            continue;
        }
        auto *member = gabi::at<resorce_hd::ResourceMethod>(0x100E1C40 + type * 8);
        if (!member->mSelector || s32(idCount) <= 0) {
            continue;
        }
        for (u32 id = 0; id < idCount; ++id) {
            u32 names = gabi::at<resorce_hd::ResourceTableRowHD>(idTable + idRow * 12)->mValues.v;
            gabi::Local<ResourceSafeStringRaw> candidate;
            candidate->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
            candidate->mStringTop =
                gabi::at<resorce_hd::ResourceTableEntryHD>(names + id * 8)->mpName.v;
            StringAssure(str);
            StringAssure(str);
            u32 saved = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
            StringAssure(gabi::ea(candidate.get()));
            u32 other = candidate->mStringTop.get();
            if (saved != other &&
                !EqualBounded(gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get(),
                              other)) {
                continue;
            }
            s16 selector = member->mSelector;
            s16 adjustment = member->mThisAdjustment;
            u32 object = info + u32(s32(adjustment));
            u32 target;
            if (selector < 0) {
                target = member->mTarget;
            } else {
                s16 offset = member->mVirtual.mVtableOffset;
                u32 vt = gabi::at<gptr<void>>(object + u32(s32(offset)))->v.get();
                target =
                    gabi::at<resorce_hd::ResourceMethodVirtualEntry>(vt + u32(s32(selector)) * 8)
                        ->mTarget;
            }
            gabi::call_ptr<void>(target, object, i, id, str, heap);
            break;
        }
    }
}
/* 0260A0B4 */
void resorce_hd::ResourceArchiveInfoHD::loadMaterialModel(u32 index, u32 id, u32 name, u32 heap) {
    u32 info = gabi::ea(this);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    StripExtension(str, name, ".");
    gabi::Local<ResourceSafeStringRaw> suffix;
    suffix->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    suffix->mStringTop = gabi::guest_addr("_bmt");
    StringAssure(str);
    AppendString(str, gabi::ea(suffix.get()));
    u32 vt = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl.get(),
        target = gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get();
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    gabi::call_ptr<void>(target, str);
    u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mModels.getAddress();
    u32 found = resourceFindName(entries,
                                 gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get());
    if (s32(found) < 0) {
        return;
    }
    archive = mpArchive.v;
    header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mModels.getAddress();
    u32 resource = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + found * 16)
                       ->mResource.getAddress();
    u32 resourceId = resourceArchiveGetId(archive, found);
    archive = mpArchive.v;
    u32 archiveName = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpArchiveName.v;
    StringAssure(archiveName);
    gabi::Local<ResourceSafeStringRaw> copy;
    copy->mStringTop = gabi::at<resorce_hd::ResourceSafeStringHD>(archiveName)->mStringTop.get();
    copy->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    u32 result = resourceCreateModel(resource, resourceId, copy.get(), heap);
    resourceCallSetResourceIndex(info, index, result);
    resourceCallSetResourceId(info, id, result);
}
/* 02604A08 */
void resorce_hd::ResourceArchiveInfoHD::loadExtraResources(u32 heap) {
    u32 info = gabi::ea(this);
    u32 archive = mpArchive.v;
    u32 header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
    u32 count = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResourceCount;
    if (!count) {
        return;
    }
    u32 data = resourceAllocateArray(count * 32, heap, 4u);
    if (data) {
        mExtraResources.mFree.v = data;
        mExtraResources.mCapacity = count;
        for (u32 i = 0; i + 1 < count; ++i) {
            gabi::at<resorce_hd::ResourceNode>(data + i * 32)->mLeft.v = data + (i + 1) * 32;
        }
        gabi::at<resorce_hd::ResourceNode>(data + (count - 1) * 32)->mLeft.v = 0;
        mExtraResources.mStorage.v = data;
    }
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop = str + 12;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[127] = 0;
    gabi::at<resorce_hd::BufferedString>(str)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[0] = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl = resorce_hd::RESOURCE_STRING_VTABLE;
    for (u32 i = 0; i < count; ++i) {
        u32 destination = gabi::at<resorce_hd::ResourceSafeStringHD>(str)->mStringTop.get();
        archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        u32 entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
        gabi::Local<ResourceSafeStringRaw> name;
        name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        name->mStringTop = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + i * 16)
                               ->mName.getAddress();
        u32 nameAddress = gabi::ea(name.get());
        resourceAssureSafeString(nameAddress);
        u32 n = Length(gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->mStringTop.get());
        u32 capacity = gabi::at<resorce_hd::BufferedString>(str)->mCapacity.get();
        if (s32(n) >= s32(capacity)) {
            n = capacity - 1;
        }
        StringAssure(nameAddress);
        resourceCopyBytes(destination,
                          gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->mStringTop.get(),
                          n, 0u);
        gabi::at<be<u8>>(destination)[n] = 0;
        if (FindSubstring(str, ".bas") < 0) {
            continue;
        }
        u32 hash = StringHash(str);
        archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        entries = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress();
        u32 reference = gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entries + i * 16)
                            ->mResource.getAddress();
        u32 offset = gabi::at<be<u32>>(reference)->get();
        u32 value = offset ? reference + offset : 0;
        s32 capacityCount = s32(mExtraResources.mCapacity), used = s32(mExtraResources.mCount);
        u32 pool = info + 104;
        if (used < capacityCount) {
            u32 node = gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v;
            if (node) {
                gabi::at<resorce_hd::ResourcePool>(pool)->mFree.v =
                    gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v;
            }
            if (node) {
                gabi::at<resorce_hd::ResourceNode>(node)->mLeft.v = 0;
                gabi::at<resorce_hd::ResourceNode>(node)->mKey = hash;
                gabi::at<resorce_hd::ResourceNode>(node)->__vtbl.v = resorce_hd::VALUE_NODE_VTABLE;
                gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = value;
                gabi::at<resorce_hd::ResourceNode>(node)->mpPool.v = pool;
                gabi::at<resorce_hd::ResourceNode>(node)->mRight.v = 0;
                gabi::at<resorce_hd::ResourceNode>(node)->mRed = 1;
                gabi::at<resorce_hd::ResourceNode>(node)->mHash = hash;
            }
            u32 old = gabi::at<resorce_hd::ResourcePool>(pool)->mCount,
                root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
            gabi::at<resorce_hd::ResourcePool>(pool)->mCount = old + 1;
            root = gabi::ea(resourceInsertNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                               gabi::at<resorce_hd::ResourceNode>(root),
                                               gabi::at<resorce_hd::ResourceNode>(node)));
            gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v = root;
            gabi::at<resorce_hd::ResourceNode>(root)->mRed = 0;
        } else {
            u32 root = gabi::at<resorce_hd::ResourcePool>(pool)->mRoot.v;
            gabi::Local<be<u32>> key;
            *key = hash;
            u32 node =
                gabi::ea(resourceFindNode(gabi::at<resorce_hd::ResourcePool>(pool),
                                          gabi::at<resorce_hd::ResourceNode>(root), key.get()));
            if (node && node + 24) {
                gabi::at<resorce_hd::ResourceNode>(node)->mValue.v = value;
            }
        }
    }
}

static void Initialize128(u32 buffer, u32 vt = resorce_hd::RESOURCE_STRING_VTABLE,
                          bool clearFirst = true) {
    gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->__vtbl = vt;
    if (clearFirst) {
        gabi::at<resorce_hd::ResourceStringBuffer128HD>(buffer)->mBuffer[0] = 0;
    }
    gabi::at<resorce_hd::BufferedString>(buffer)->mCapacity = 128;
    gabi::at<resorce_hd::ResourceStringBuffer128HD>(buffer)->mBuffer[127] = 0;
    gabi::at<resorce_hd::ResourceSafeStringHD>(buffer)->mStringTop = buffer + 12;
}
static u32 ArchiveHeader(u32 info) {
    return gabi::at<resorce_hd::ResourceArchiveHD>(
               gabi::at<resorce_hd::ResourcePathInfoHD>(info)->mpArchive.v)
        ->mpHeader.v;
}
static void FormatResourceName(u32 str, u32 entry, const char *format) {
    gabi::Local<ResourceSafeStringRaw> name;
    u32 object = gabi::ea(name.get());
    name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
    name->mStringTop =
        gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mName.getAddress();
    resourceAssureSafeString(object);
    resourceFormatName(str, format,
                       gabi::at<resorce_hd::ResourceSafeStringHD>(object)->mStringTop.get());
}
/* 026057F4 */
void resorce_hd::ResourcePathInfoHD::loadResource() {
    u32 info = gabi::ea(this);
    u32 header = ArchiveHeader(info);
    u32 skeletalAnimations =
            gabi::at<resorce_hd::ResourceHeaderHD>(header)->mSkeletalAnimationCount,
        raw = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResourceCount;
    u32 models = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mModelCount,
        textures = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureCount;
    u32 color = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTevColorCount,
        texturePatterns = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTexturePatternCount;
    u32 transforms = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureTransformCount;
    u32 heap = mDataHeap.v;
    resourceCallAllocatePool(
        info, models + skeletalAnimations + textures + raw + texturePatterns + transforms + color,
        heap);
    gabi::Local<StringBuffer128> buffer;
    u32 str = gabi::ea(buffer.get());
    for (u32 i = 0; i < models; ++i) {
        u32 entry =
            gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))->mModels.getAddress() +
            i * 16;
        u32 resource =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        StringAssure(info + 16);
        u32 archiveChars = mArchiveName.mStringTop.get();
        StringAssure(info + 92);
        u32 pathChars = mArchivePath.mStringTop.get();
        u32 archive = mpArchive.v;
        u32 resourceId = resourceArchiveGetId(archive, i);
        gabi::Local<u8[76]> label;
        u32 labelResult = resourceFormatLabel(reinterpret_cast<u8 *>(label.get()), "%s/%s",
                                              archiveChars, pathChars);
        heap = mDataHeap.v;
        u32 result = resourceCreateNamedModel(resource, resourceId, labelResult, heap);
        Initialize128(str);
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))->mModels.getAddress() +
                i * 16;
        resource =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        gabi::Local<ResourceSafeStringRaw> name;
        u32 nameAddress = gabi::ea(name.get());
        name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        name->mStringTop =
            gabi::at<resorce_hd::ResourceModelHeaderHD>(resource)->mName.getAddress();
        resourceAssureSafeString(nameAddress);
        resourceFormatName(
            str, "%s.bmd",
            gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->mStringTop.get());
        resourceCallSetResource(info, str, result);
    }
    for (u32 i = 0; i < skeletalAnimations; ++i) {
        u32 entry = gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))
                        ->mSkeletalAnimations.getAddress() +
                    i * 16;
        u32 resource =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        heap = mDataHeap.v;
        u32 result = resourceCreateSkeletalAnimation(
            resource, gabi::at<resorce_hd::ResourceSafeStringHD>(info + 16), heap);
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))
                    ->mSkeletalAnimations.getAddress() +
                i * 16;
        gabi::Local<ResourceSafeStringRaw> name;
        name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        name->mStringTop =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mName.getAddress();
        resourceCallSetResource(info, gabi::ea(name.get()), result);
    }
    for (u32 i = 0; i < textures; ++i) {
        heap = mDataHeap.v;
        u32 texture = resourceAllocateOnHeap(36u, heap, 4u);
        if (texture) {
            u32 entry = gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))
                            ->mTextures.getAddress() +
                        i * 16;
            u32 resource =
                gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
            gabi::at<resorce_hd::ResourceTextureHD>(texture)->mpResource.v = resource;
            gabi::at<resorce_hd::ResourceTextureHD>(texture)->mWidth =
                gabi::at<resorce_hd::ResourceTextureHeaderHD>(resource)->mWidth;
            u32 height = gabi::at<resorce_hd::ResourceTextureHeaderHD>(resource)->mHeight;
            gabi::at<resorce_hd::ResourceTextureHD>(texture)->mFlags = 0;
            gabi::at<resorce_hd::ResourceTextureHD>(texture)->mHeight = height;
        }
        Initialize128(str);
        u32 entry =
            gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))->mTextures.getAddress() +
            i * 16;
        FormatResourceName(str, entry, "%s.bti");
        resourceCallSetResource(info, str, texture);
    }
    for (u32 i = 0; i < texturePatterns; ++i) {
        u32 archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        u32 entry = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTexturePatterns.getAddress() +
                    i * 16,
            resource =
                gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        Initialize128(str);
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        entry =
            gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTexturePatterns.getAddress() + i * 16;
        FormatResourceName(str, entry, "%s.btp");
        heap = mDataHeap.v;
        u32 result = resourceCreateTexturePatternAnimation(resource, heap);
        resourceCallSetResource(info, str, result);
    }
    f32 initial = l_resourceInitialFrame;
    for (u32 i = 0; i < transforms; ++i) {
        u32 archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        u32 entry =
                gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureTransforms.getAddress() +
                i * 16,
            resource =
                gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        Initialize128(str);
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTextureTransforms.getAddress() +
                i * 16;
        FormatResourceName(str, entry, "%s.btk");
        heap = mDataHeap.v;
        u32 animation = resourceAllocateOnHeap(16u, heap, 4u);
        if (animation) {
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->__vtbl =
                resorce_hd::TEXTURE_TRANSFORM_VTABLE;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFlags = 0;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFrame = initial;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = 0;
        }
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = resource;
        resourceCallSetResource(info, str, animation);
    }
    for (u32 i = 0; i < color; ++i) {
        u32 archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        u32 entry =
                gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTevColors.getAddress() + i * 16,
            resource =
                gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress();
        Initialize128(str);
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mTevColors.getAddress() + i * 16;
        FormatResourceName(str, entry, "%s.brk");
        heap = mDataHeap.v;
        u32 animation = resourceAllocateOnHeap(16u, heap, 4u);
        if (animation) {
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->__vtbl =
                resorce_hd::TEV_REG_ANIMATION_VTABLE;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFlags = 0;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mFrame = initial;
            gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = 0;
        }
        gabi::at<resorce_hd::ResourceFrameAnimationHD>(animation)->mpResource.v = resource;
        resourceCallSetResource(info, str, animation);
    }
    for (u32 i = 0; i < raw; ++i) {
        u32 archive = mpArchive.v;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        u32 entry =
            gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress() + i * 16;
        u32 reference =
                gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mResource.getAddress(),
            offset = gabi::at<be<u32>>(reference)->get();
        u32 resource = offset ? reference + offset : 0;
        header = gabi::at<resorce_hd::ResourceArchiveHD>(archive)->mpHeader.v;
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(header)->mRawResources.getAddress() + i * 16;
        Initialize128(str, resorce_hd::RAW_RESOURCE_STRING_VTABLE, false);
        gabi::Local<ResourceSafeStringRaw> name;
        u32 nameAddress = gabi::ea(name.get());
        name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        name->mStringTop =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mName.getAddress();
        resourceAssureSafeString(nameAddress);
        u32 length =
            Length(gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->mStringTop.get());
        u32 capacity = gabi::at<resorce_hd::BufferedString>(str)->mCapacity.get(),
            vt = gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->__vtbl.get();
        if (s32(length) >= s32(capacity)) {
            length = capacity - 1;
        }
        gabi::call_ptr<void>(
            gabi::at<resorce_hd::ResourceSafeStringVirtualsHD>(vt)->mAssureTermination.get(),
            nameAddress);
        resourceCopyBytes(str + 12,
                          gabi::at<resorce_hd::ResourceSafeStringHD>(nameAddress)->mStringTop.get(),
                          length, 0u);
        gabi::at<resorce_hd::ResourceSafeStringHD>(str)->__vtbl =
            resorce_hd::RESOURCE_STRING_VTABLE;
        gabi::at<resorce_hd::ResourceStringBuffer128HD>(str)->mBuffer[length] = 0;
        resourceTerminateString(str);
        s32 position = FindSubstring(str, ".dzb", false);
        if (position > 0) {
            resourceConvertCollision(resource);
        }
        entry = gabi::at<resorce_hd::ResourceHeaderHD>(ArchiveHeader(info))
                    ->mRawResources.getAddress() +
                i * 16;
        name->__vtbl = resorce_hd::SAFE_STRING_VTABLE;
        name->mStringTop =
            gabi::at<resorce_hd::ResourceDictionaryEntryViewHD>(entry)->mName.getAddress();
        resourceCallSetResource(info, gabi::ea(name.get()), resource);
    }
}

/* ---------------- HD: compiler-generated and template code of this translation unit
 * ---------------- */

/* inlined into resource tree operations */
static resorce_hd::ResourceNode* RotateLeft(resorce_hd::ResourceNode* node,
                                            resorce_hd::ResourceNode* right) {
    resorce_hd::ResourceNode* child = right->mLeft;
    node->mRight = child;
    right->mLeft = node;
    u8 color = node->mRed;
    right->mRed = color;
    node->mRed = 1;
    return right;
}

/* inlined into resource tree operations */
static resorce_hd::ResourceNode* RotateRight(resorce_hd::ResourceNode* node,
                                             resorce_hd::ResourceNode* left) {
    resorce_hd::ResourceNode* child = left->mRight;
    node->mLeft = child;
    left->mRight = node;
    u8 color = node->mRed;
    left->mRed = color;
    node->mRed = 1;
    return left;
}

/* inlined into resource tree operations */
static void Flip(resorce_hd::ResourceNode* node) {
    u8 color = node->mRed;
    resorce_hd::ResourceNode* left = node->mLeft;
    node->mRed = color ^ 1;
    color = left->mRed;
    left->mRed = color ^ 1;
    resorce_hd::ResourceNode* right = node->mRight;
    color = right->mRed;
    right->mRed = color ^ 1;
}

/* inlined into resource tree operations */
static bool Red(resorce_hd::ResourceNode* node) { return node && node->mRed; }

/* inlined into resource tree operations */
static resorce_hd::ResourceNode* MoveRedLeft(resorce_hd::ResourceNode* node) {
    Flip(node);
    resorce_hd::ResourceNode* right = node->mRight;
    resorce_hd::ResourceNode* left = right->mLeft;
    if (Red(left)) {
        resorce_hd::ResourceNode* next = RotateRight(right, left);
        node->mRight = next;
        node = RotateLeft(node, next);
        Flip(node);
    }
    return node;
}

/* inlined into resource tree operations */
static resorce_hd::ResourceNode* MoveRedRight(resorce_hd::ResourceNode* node) {
    Flip(node);
    resorce_hd::ResourceNode* left = node->mLeft;
    if (Red(left->mLeft)) {
        node = RotateRight(node, left);
        Flip(node);
    }
    return node;
}

/* inlined into resource tree operations */
static resorce_hd::ResourceNode* Balance(resorce_hd::ResourceNode* node) {
    resorce_hd::ResourceNode* right = node->mRight;
    if (Red(right)) {
        node = RotateLeft(node, right);
    }
    resorce_hd::ResourceNode* left = node->mLeft;
    if (Red(left) && Red(left->mLeft)) {
        node = RotateRight(node, left);
        left = node->mLeft;
    }
    if (Red(left) && Red(node->mRight)) {
        Flip(node);
    }
    return node;
}

/* inlined into resource tree operations */
static void ReleaseNode(resorce_hd::ResourceNode* node) {
    resorce_hd::ResourceNodeVirtuals* vt = node->__vtbl;
    gabi::call_ptr<void>(u32(vt->mRelease), node);
}

/* 0260A810 */
static void resourceSafeString__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A840 */
static void resourceBufferedString__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A854 */
static void resourceFixedString64__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A868 */
static void resourceFormatString64__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A87C */
static void resourceArchiveCallback__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A890 */
static void resourcePathCallback__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260A824 */
static void resourceSafeString_assureTermination(void* self) {}

/* 0260ABF8 */
static void resourceArchiveTree_empty(void* self) {}

/* 0260AC3C */
static void resourceValueTree_empty(void* self) {}

/* 0260ABFC */
static void resourceArchiveNode_release(resorce_hd::ResourceNode* self) {
    resorce_hd::ResourcePool* pool = self->mpPool;
    self->mLeft = pool->mFree;
    u32 count = pool->mCount;
    pool->mFree = self;
    pool->mCount = count - 1;
}

/* 0260AC1C */
static void resourcePathNode_release(resorce_hd::ResourceNode* self) {
    resorce_hd::ResourcePool* pool = self->mpPool;
    self->mLeft = pool->mFree;
    u32 count = pool->mCount;
    pool->mFree = self;
    pool->mCount = count - 1;
}

/* 0260AC40 */
static void resourceValueNode_release(resorce_hd::ResourceNode* self) {
    resorce_hd::ResourcePool* pool = self->mpPool;
    self->mLeft = pool->mFree;
    u32 count = pool->mCount;
    pool->mFree = self;
    pool->mCount = count - 1;
}

/* 0260A828 */
static void resourceBufferedString_terminate(resorce_hd::BufferedString* self) {
    u32 data = self->mStringTop;
    u32 capacity = self->mCapacity;
    // HD: effective-address arithmetic wraps before the byte store.
    *gabi::at<be<u8>>(data + capacity - 1) = 0;
}

/* 0260A728 */
static void resourceInfoDisposer__dt(void* self, u32 flags) {
    if (self == nullptr) {
        return;
    }
    resourceDisposer_destroy(self, 0);
    if (flags & 1) {
        operator_delete(self);
    }
}

/* 0260A9C4 */
static void resourceTreeKey__dt(void* self, u32 flags) {
    if (self != nullptr && (flags & 1)) {
        operator_delete(self);
    }
}

/* 0260AC60 */
static void resourceArchiveTransfer_invoke(resorce_hd::ResourceCallbackReceiver* self,
                                           resorce_hd::ResourceNode* node) {
    u32 key = node->mHash;
    void* root = self->mpObject;
    gabi::Local<be<u32>> copy;
    *copy = key;
    resourceArchiveTransfer_apply(root, copy.get(), &node->mValue);
}

/* 0260AC94 */
static void resourceTextureTransfer_invoke(resorce_hd::ResourceCallbackReceiver* self,
                                           resorce_hd::ResourceNode* node) {
    u32 key = node->mHash;
    void* root = self->mpObject;
    gabi::Local<be<u32>> copy;
    *copy = key;
    resourceTextureTransfer_apply(root, copy.get(), &node->mValue);
}

/* 0260ACC8 */
static resorce_hd::ResourceNode* resourceTree_find(resorce_hd::ResourcePool* tree,
                                                   resorce_hd::ResourceNode* node,
                                                   const be<u32>* key) {
    if (node != nullptr) {
        u32 wanted = *key;
        do {
            u32 value = node->mKey;
            if (value < wanted) {
                node = node->mRight;
            } else if (value > wanted) {
                node = node->mLeft;
            } else {
                break;
            }
        } while (node != nullptr);
    }
    return node;
}

/* 0260B698 */
static resorce_hd::ResourceCallback* resourceArchiveCallback__ct(resorce_hd::ResourceCallback* self,
                                                                 void* receiver,
                                                                 const be<u32>* method) {
    if (self == nullptr) {
        self = static_cast<resorce_hd::ResourceCallback*>(operator_new(sizeof(*self)));
    }
    if (self != nullptr) {
        self->mpReceiver = receiver;
        u32 word = method[0];
        self->mMethodWords[0] = word;
        word = method[1];
        self->__vtbl = resorce_hd::ARCHIVE_CALLBACK_VTABLE;
        self->mMethodWords[1] = word;
    }
    return self;
}

/* 0260B704 */
static resorce_hd::ResourceCallback* resourceTextureCallback__ct(resorce_hd::ResourceCallback* self,
                                                                 void* receiver,
                                                                 const be<u32>* method) {
    if (self == nullptr) {
        self = static_cast<resorce_hd::ResourceCallback*>(operator_new(sizeof(*self)));
    }
    if (self != nullptr) {
        self->mpReceiver = receiver;
        u32 word = method[0];
        self->mMethodWords[0] = word;
        word = method[1];
        self->__vtbl = resorce_hd::TEXTURE_CALLBACK_VTABLE;
        self->mMethodWords[1] = word;
    }
    return self;
}

/* 0260A77C */
static void resourceHeaderStatics_init() {
    l_resourceHeaderStatics.mZero[2] = 0;
    l_resourceHeaderStatics.mZero[0] = 0;
    l_resourceHeaderStatics.mZero[3] = 0;
    l_resourceHeaderStatics.mZero[1] = 0;
    __register_global_object(gabi::ea(&l_resourceDestructors.mEntries[0]));
    f32 minimum = l_resourceBounds.mValues[0];
    f32 maximum = l_resourceBounds.mValues[1];
    l_resourceHeaderStatics.mMinimum = minimum;
    l_resourceHeaderStatics.mMaximum = maximum;
    resourceMath_init(&l_resourceHeaderStatics.mMath);
    __register_global_object(gabi::ea(&l_resourceDestructors.mEntries[1]));
    resourceRandom_init(&l_resourceHeaderStatics.mRandom);
    __register_global_object(gabi::ea(&l_resourceDestructors.mEntries[2]));
}

/* 0260B538 */
static void resourceArchiveTree_traverse(resorce_hd::ResourceNode* node,
                                         resorce_hd::ResourceCallback* callback) {
    do {
        resorce_hd::ResourceNode* child = node->mLeft;
        if (child != nullptr) {
            resourceVisitArchive(child, callback);
        }
        u32 receiver = callback->mpReceiver.v;
        if (receiver != 0) {
            const auto* method =
                reinterpret_cast<const resorce_hd::ResourceMethod*>(callback->mMethodWords);
            s16 selector = method->mSelector;
            if (selector != 0) {
                s16 adjustment = method->mThisAdjustment;
                // HD: the GHS member pointer stores signed halfword adjustments; EA sums wrap.
                u32 object = receiver + u32(s32(adjustment));
                u32 target;
                if (selector < 0) {
                    target = method->mTarget;
                } else {
                    s16 vtableOffset = method->mVirtual.mVtableOffset;
                    auto* tablePointer = gabi::at<gptr<resorce_hd::ResourceMethodVirtualEntry>>(
                        object + u32(s32(vtableOffset)));
                    resorce_hd::ResourceMethodVirtualEntry* table = *tablePointer;
                    target = table[selector].mTarget;
                }
                gabi::call_ptr<void>(target, object, node);
            }
        }
        node = node->mRight;
    } while (node != nullptr);
}

/* 0260B5E8 */
static void resourceTextureTree_traverse(resorce_hd::ResourceNode* node,
                                         resorce_hd::ResourceCallback* callback) {
    do {
        resorce_hd::ResourceNode* child = node->mLeft;
        if (child != nullptr) {
            resourceVisitTexture(child, callback);
        }
        u32 receiver = callback->mpReceiver.v;
        if (receiver != 0) {
            const auto* method =
                reinterpret_cast<const resorce_hd::ResourceMethod*>(callback->mMethodWords);
            s16 selector = method->mSelector;
            if (selector != 0) {
                s16 adjustment = method->mThisAdjustment;
                // HD: the GHS member pointer stores signed halfword adjustments; EA sums wrap.
                u32 object = receiver + u32(s32(adjustment));
                u32 target;
                if (selector < 0) {
                    target = method->mTarget;
                } else {
                    s16 vtableOffset = method->mVirtual.mVtableOffset;
                    auto* tablePointer = gabi::at<gptr<resorce_hd::ResourceMethodVirtualEntry>>(
                        object + u32(s32(vtableOffset)));
                    resorce_hd::ResourceMethodVirtualEntry* table = *tablePointer;
                    target = table[selector].mTarget;
                }
                gabi::call_ptr<void>(target, object, node);
            }
        }
        node = node->mRight;
    } while (node != nullptr);
}

/* 0260AD08 */
static resorce_hd::ResourceNode* resourceTree_insert(resorce_hd::ResourcePool* tree,
                                                     resorce_hd::ResourceNode* node,
                                                     resorce_hd::ResourceNode* inserted) {
    if (!node) {
        inserted->mRed = 1;
        inserted->mRight = 0;
        inserted->mLeft = 0;
        return inserted;
    }
    u32 key = inserted->mKey;
    u32 oldKey = node->mKey;
    resorce_hd::ResourceNode *left, *right;
    if (oldKey > key) {
        resorce_hd::ResourceNode* child = node->mLeft;
        left = resourceInsertNode(tree, child, inserted);
        right = node->mRight;
        node->mLeft = left;
    } else if (oldKey < key) {
        resorce_hd::ResourceNode* child = node->mRight;
        right = resourceInsertNode(tree, child, inserted);
        left = node->mLeft;
        node->mRight = right;
    } else {
        if (node != inserted) {
            right = node->mRight;
            inserted->mRight = right;
            left = node->mLeft;
            inserted->mLeft = left;
            u8 red = node->mRed;
            inserted->mRed = red;
            resorce_hd::ResourceNodeVirtuals* vt = node->__vtbl;
            gabi::call_ptr<void>(u32(vt->mRelease), node);
        }
        node = inserted;
        right = node->mRight;
        left = node->mLeft;
    }
    if (right && right->mRed && (!left || !left->mRed)) {
        resorce_hd::ResourceNode* grandchild = right->mLeft;
        node->mRight = grandchild;
        right->mLeft = node;
        u8 color = node->mRed;
        right->mRed = color;
        node->mRed = 1;
        node = right;
        left = node->mLeft;
    }
    if (left && left->mRed) {
        resorce_hd::ResourceNode* child = left->mLeft;
        if (child && child->mRed) {
            child = left->mRight;
            node->mLeft = child;
            left->mRight = node;
            u8 color = node->mRed;
            left->mRed = color;
            node->mRed = 1;
            node = left;
            left = node->mLeft;
        }
    }
    if (left && left->mRed) {
        right = node->mRight;
        if (right && right->mRed) {
            u8 color = node->mRed;
            left = node->mLeft;
            node->mRed = color ^ 1;
            color = left->mRed;
            left->mRed = color ^ 1;
            right = node->mRight;
            color = right->mRed;
            right->mRed = color ^ 1;
        }
    }
    return node;
}

/* 0260A9D8 */
static resorce_hd::ResourceNode* resourceTree_deleteMinimum(resorce_hd::ResourceNode* node) {
    resorce_hd::ResourceNode* left = node->mLeft;
    if (!left) {
        return 0;
    }
    if ((!left || !left->mRed)) {
        resorce_hd::ResourceNode* grandchild = left->mLeft;
        if (!grandchild || !grandchild->mRed) {
            Flip(node);
            resorce_hd::ResourceNode* right = node->mRight;
            resorce_hd::ResourceNode* child = right->mLeft;
            if (child && child->mRed) {
                child = right->mLeft;
                child = RotateRight(right, child);
                node->mRight = child;
                node = RotateLeft(node, child);
                Flip(node);
            }
            left = node->mLeft;
        }
    }
    resorce_hd::ResourceNode* changed = resourceDeleteMinimum(left);
    resorce_hd::ResourceNode* right = node->mRight;
    node->mLeft = changed;
    if (right && right->mRed) {
        node = RotateLeft(node, right);
    }
    left = node->mLeft;
    if (left && left->mRed) {
        resorce_hd::ResourceNode* child = left->mLeft;
        if (child && child->mRed) {
            node = RotateRight(node, left);
            left = node->mLeft;
        }
    }
    if (left && left->mRed) {
        right = node->mRight;
        if (right && right->mRed) {
            Flip(node);
        }
    }
    return node;
}

/* 0260A8A4 */
static resorce_hd::FixedString64* resourceFormatString64__ct(resorce_hd::FixedString64* self,
                                                             const char* format, u32 a5, u32 a6,
                                                             u32 a7, u32 a8, u32 a9, u32 a10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::Local<resorce_hd::FormatFrame> frame;
    const u32 registers[8] = {gabi::ea(self), gabi::ea(format), a5, a6, a7, a8, a9, a10};
    for (u32 i = 0; i < 8; ++i) {
        frame->mGprSave[i] = registers[i];
    }
    if (floats) {
        for (u32 i = 0; i < 8; ++i) {
            u64 bits;
            memcpy(&bits, &gabi::cpu->f[i + 1].ps0, 8);
            frame->mFprSave[i].mHigh = u32(bits >> 32);
            frame->mFprSave[i].mLow = u32(bits);
        }
    }
    if (self == nullptr) {
        self = static_cast<resorce_hd::FixedString64*>(operator_new(sizeof(*self)));
    }
    if (self == nullptr) {
        return self;
    }
    resorce_hd::BufferedString* destination = reinterpret_cast<resorce_hd::BufferedString*>(self);
    if (destination == nullptr) {
        destination = static_cast<resorce_hd::BufferedString*>(operator_new(sizeof(*destination)));
    }
    if (destination != nullptr) {
        destination->mStringTop = gabi::ea(self->mBuffer);
        destination->__vtbl = resorce_hd::BUFFERED_STRING_VTABLE;
        destination->mCapacity = sizeof(self->mBuffer);
        self->mBuffer[63] = 0;
    }
    self->__vtbl = resorce_hd::FIXED_STRING_VTABLE;
    be<u8>* data = gabi::at<be<u8>>(self->mStringTop);
    *data = 0;
    self->__vtbl = resorce_hd::FORMAT_STRING_VTABLE;
    frame->mArguments.mGpr = 2;
    frame->mArguments.mOverflow = entry + 8;
    frame->mArguments.mSave = gabi::ea(frame->mGprSave);
    frame->mArguments.mFpr = 0;
    resourceString_format(self, format, &frame->mArguments);
    return self;
}

/* 0260AEFC */
static resorce_hd::ResourceNode* resourceTree_delete(resorce_hd::ResourcePool* tree,
                                                     resorce_hd::ResourceNode* node,
                                                     const be<u32>* key) {
    u32 sought = u32(*key), old = node->mKey;
    resorce_hd::ResourceNode* left = node->mLeft;
    if (sought < old) {
        if (!Red(left) && !Red(left->mLeft)) {
            node = MoveRedLeft(node);
        }
        left = node->mLeft;
        resorce_hd::ResourceNode* replacement = resourceDeleteNode(tree, left, key);
        node->mLeft = replacement;
    } else {
        if (Red(left)) {
            node = RotateRight(node, left);
            sought = u32(*key);
            old = node->mKey;
        }
        resorce_hd::ResourceNode* right = node->mRight;
        if (sought == old && !right) {
            ReleaseNode(node);
            return 0;
        }
        if (!Red(right) && !Red(right->mLeft)) {
            node = MoveRedRight(node);
            old = node->mKey;
            right = node->mRight;
            sought = u32(*key);
        }
        if (old == sought) {
            resorce_hd::ResourceNode* minimum = right;
            resorce_hd::ResourceNode* next = minimum->mLeft;
            while (next) {
                minimum = next;
                next = minimum->mLeft;
            }
            resorce_hd::ResourceNode* successor = resourceFindNode(tree, right, &minimum->mKey);
            left = right->mLeft;
            if (!left) {
                successor->mRight = 0;
            } else {
                if (!Red(left) && !Red(left->mLeft)) {
                    right = MoveRedLeft(right);
                }
                left = right->mLeft;
                resorce_hd::ResourceNode* removed = resourceDeleteMinimum(left);
                right->mLeft = removed;
                right = Balance(right);
                successor->mRight = right;
            }
            successor->mLeft = node->mLeft;
            successor->mRed = node->mRed;
            ReleaseNode(node);
            node = successor;
        } else {
            resorce_hd::ResourceNode* replacement = resourceDeleteNode(tree, right, key);
            node->mRight = replacement;
        }
    }
    return Balance(node);
}

// clang-format off
/* WWHD glue
data l_resourceTransferMethods 100E1980
data l_resourceControlGlobals 101F4F28
data l_resourceCacheManager 101F4F54
data l_resourceArchiveLoader 101F4F7C
data l_resourceHeapManager 101F8B4C
data l_resourceTableCount 10080150
data l_resourceIdTableCount 100C45C0
data l_resourceInitialFrame 100E1B58
str 100E1ADC
str 100E1AE0 in 02603740
str 100E1AE4
str 100E1AEC
str 100E1B24 in 02603C84
str 100E1B2C in 02603C84
str 100E1B34 in 02603C84
str 100E1B3C in 02603C84
str 100E1B44 in 02603C84
str 100E1B4C in 02604094
str 100E1B50 in 02604A08
str 100E1B5C
str 100E1B64
str 100E1B6C
str 100E1B74
str 100E1B7C
str 100E1B84
str 100E1B8C
str 100E1B94 in 0260623C
str 100E1B9C in 02606BEC
str 100E1BA4 in 02606D74
str 100E1BAC in 0260757C
str 100E1BB0 in 0260757C
str 100E1BB8
str 100E1BF0 in 02607B68
str 100E1BF8 in 02607B68
str 100E1C00 in 02607B68
str 100E1C08 in 02607B68
str 100E1C10 in 02607B68
str 100E1C18 in 02607F80
str 100E1C1C in 02608308
str 100E1C1E in 0260873C
str 100E1C20 in 02608B90
str 100E1C22 in 02608F80
str 100E1C24 in 026093A0
str 100E1C26 in 02609A3C
str 100E1C28 in 0260A0B4
str 100E1C2C
ext 02759C28 void resourceFormatPath(u32 string, const char* format, u32 path, u32 archiveName)
ext 02759C28 void resourceFormatName(u32 string, const char* format, u32 name)
ext 0260A8A4 u32 resourceFormatLabel(u8* string, const char* format, u32 archiveName, u32 path)
ext 02753004 u32 resourceCreateExpHeap(u32 size, resorce_hd::ResourceSafeStringHD* name, u32 parent, u32 alignment, u32 flags)
ext 02754B38 u32 resourceCreateSolidHeap(u32 size, resorce_hd::ResourceSafeStringHD* name, u32 parent, u32 alignment, u32 flags)
ext 02756170 u32 resourceSwitchCurrentHeap(u32 manager, u32 heap)
ext 02602AD4 u32 resourceConstructArchive(u32 archive)
ext 026123FC u32 resourceLoadArchiveFile(u32 manager, u32 name)
ext 026031C4 void resourceMountPathArchive(u32 archive, u32 data, u32 name, u32 heap)
ext 02602ECC void resourceMountArchive(u32 archive, u32 data, u32 name, u32 heap)
ext 0260F61C void resourceReleaseCachedArchive(u32 manager, u32 hash)
ext 0260A824 void resourceAssureSafeString(u32 string)
ext 0260A828 void resourceTerminateString(u32 string)
ext 020080C0 void resourceConvertCollision(u32 resource)
ext 0260B698 void resourceConstructArchiveCallback(be<u32>* callback, u32 receiver, be<u32>* method)
ext 0260B704 void resourceConstructTextureCallback(be<u32>* callback, u32 receiver, be<u32>* method)
ext 0273B264 u32 resourceHashString(u32 text, u32 length)
ext 027DF9B0 s32 resourceFindName(u32 entries, u32 name)
ext 026031D0 u32 resourceArchiveGetId(u32 archive, u32 index)
ext 0272D384 u32 resourceCreateModel(u32 resource, u32 id, resorce_hd::ResourceSafeStringHD* name, u32 heap)
ext 027F78A8 u32 resourceCreateSkeletalAnimation(u32 resource, resorce_hd::ResourceSafeStringHD* name, u32 heap)
ext 027F7A2C u32 resourceCreateTexturePatternAnimation(u32 resource, u32 heap)
ext 027F7B50 u32 resourceCreateNamedModel(u32 resource, u32 id, u32 label, u32 heap)
ext 02752A84 void resourceDisposerConstruct(u32 object)
ext 02752B0C void resourceDisposerConstructOnHeap(u32 object, u32 heap, u32 flags)
ext 027E2EA4 void resourceTransferArchiveData(u32 destination, u32 source, u32 mode)
ext C0009988 void resourceCopyBytes(u32 destination, u32 source, u32 size, u32 flags)
ext 02603720 void resourceCallControlDestructor(u32 self, u32 flags)
ext 026066A0 u32 resourceCallGetIndex(u32 info, u32 index)
ext 026067D0 u32 resourceCallGetId(u32 info, u32 id)
ext 026082D0 void resourceCallSetResourceIndex(u32 info, s32 index, u32 value)
ext 026082EC void resourceCallSetResourceId(u32 info, s32 index, u32 value)
ext 0260344C u32 resourceCallControlCtor(u32 self)
ext 026055E4 void resourceCallAllocatePool(u32 self, s32 count, u32 heap)
ext 02606AF8 u32 resourceCallGetPathResource(u32 self, u32 name)
ext 02606D74 u32 resourceCallResourceByFilename(u32 info, u32 name)
ext 02604470 void resourceCallAllocateResourceIndices(u32 info, s32 count, u32 heap)
ext 026044E0 void resourceCallAllocateResourceIds(u32 info, s32 count, u32 heap)
ext 02603ACC void resourceCallCreateStageHeaps(u32 self)
ext 026079B4 void resourceCallCreateObjectHeaps(u32 self)
ext 0260566C void resourceCallSetResource(u32 info, u32 name, u32 resource)
ext 026072AC void resourceCallDestroyObjectHeaps(u32 self)
ext 02607414 void resourceCallDestroyStageHeaps(u32 self)
ext 02605344 u32 resourceCallInfoCtor(u32 self, u32 archiveName, u32 path, u32 unused, u32 parent, u32 heap)
ext 026042AC u32 resourceCallArchiveInfoCtor(u32 self, u32 name, u32 archive, u32 parent, u32 heap)
ext 02604094 u32 resourceCallSelectHeap(u32 control, u32 name, u32 path, u32 heap)
ext 02603740 u32 resourceCallNeedsSeparateHeap(u32 control, u32 name, u32 path, u32 heap)
ext 02603C84 u32 resourceCallChooseSpecialHeap(u32 control, u32 name)
ext 02607B68 u32 resourceCallChooseObjectHeap(u32 control, u32 name)
ext 02604550 void resourceCallLoadTableResources(u32 info, u32 name, u32 heap)
ext 02604A08 void resourceCallLoadExtraResources(u32 info, u32 heap)
ext 026057F4 void resourceCallLoadAllResources(u32 info)
ext 0273AD10 u32 resourceAllocate(u32 size)
ext 0273B0D4 u32 resourceAllocateArray(u32 size, u32 heap, u32 alignment)
ext 0273B050 u32 resourceAllocateOnHeap(u32 size, u32 heap, u32 alignment)
ext 0273AF40 void resourceFree(u32 memory)
data l_resourceHeaderStatics 1048DD14
data l_resourceDestructors 101F4F04
data l_resourceBounds 100E1C34
ext 028ED6F8 void resourceMath_init(be<u8>* state)
ext 028EAB2C void resourceRandom_init(be<u8>* state)
ext 02759C10 void resourceString_format(resorce_hd::FixedString64* self, const char* format, resorce_hd::FormatArguments* arguments)
ext 0260B5E8 void resourceVisitTexture(resorce_hd::ResourceNode* node, resorce_hd::ResourceCallback* callback)
ext 0260B538 void resourceVisitArchive(resorce_hd::ResourceNode* node, resorce_hd::ResourceCallback* callback)
ext 0260AD08 resorce_hd::ResourceNode* resourceInsertNode(resorce_hd::ResourcePool* tree, resorce_hd::ResourceNode* node, resorce_hd::ResourceNode* inserted)
ext 0260A9D8 resorce_hd::ResourceNode* resourceDeleteMinimum(resorce_hd::ResourceNode* node)
ext 0260AEFC resorce_hd::ResourceNode* resourceDeleteNode(resorce_hd::ResourcePool* tree, resorce_hd::ResourceNode* node, const be<u32>* key)
ext 0260ACC8 resorce_hd::ResourceNode* resourceFindNode(resorce_hd::ResourcePool* tree, resorce_hd::ResourceNode* node, const be<u32>* key)
ext 02752BEC void resourceDisposer_destroy(void* self, u32 flags)
ext 02608234 void resourceArchiveTransfer_apply(void* first, const be<u32>* key, gptr<void>* second)
ext 02608258 void resourceTextureTransfer_apply(void* first, const be<u32>* key, gptr<void>* second)
*/
// clang-format on
