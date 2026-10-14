/* HD resident layout archives, cached resources and asynchronous preload groups. */
#include "d/hd_layout_mgr.h"
static u32 sResMgrHeaderStatics;
static u32 sResMgrHeaderDescriptors;
static u32 sResMgrHeaderZero;
static u32 sResMgrPiConstants;
static hdLayoutResMgr_c* sLayoutResMgr;
static hdLayoutDisposer_c* sLayoutResDisposer;
static hdScreenStringTable sResourceStringTable;
static hdScreenStringTable sNameBaseTable;
static hdScreenStringTable sPathTable;
static hdScreenStringTable sNameIntermediateTable;
static hdScreenStringTable sNameFinalTable;
static u32 sManagerTable;
static u32 sDisposerTable;
static u32 sTaskTable;
static u32 sNodeTable;
static u8 sEmptyName[1] = {0};
static void* sArchiveMgr;
static hdLayoutUiManager_c* sUiManager;
static u32 sThreadPriority;
static void preloadLayoutResources(hdLayoutResMgr_c*, u32, u32);
void* resAllocate(u32, hdLayoutHeap_c*, u32);
void* resAllocateSingleton(u32, hdLayoutHeap_c*, u32);
void resConstructDisposer(hdLayoutDisposer_c*, hdLayoutHeap_c*, u32);
void resDestroyDisposer(hdLayoutDisposer_c*, u32);
hdLayoutHeap_c* resCreateHeap(u32, hdScreenString*, hdLayoutHeap_c*, s32, u32);
hdLayoutThread_c* resConstructThread(hdLayoutThread_c*, hdScreenString*, hdLayoutTask_c*, hdLayoutHeap_c*,
                                     u32, u32, u32, u32, u32);
hdScreenResource* resFindArchive(void*, hdScreenString*);
u32 resLoadArchive(void*, hdScreenString*, hdLayoutPath_c*, hdLayoutHeap_c*, u32, u32, u32);
u32 resUnloadArchive(void*, hdScreenString*);
hdLayoutCacheNode_c* resFindNode(hdLayoutCachePool_c*, hdLayoutCacheNode_c*, hdScreenString*);
hdLayoutCacheNode_c* resInsertNode(hdLayoutCachePool_c*, hdLayoutCacheNode_c*, hdLayoutCacheNode_c*);
hdLayoutCacheNode_c* resEraseNode(hdLayoutCachePool_c*, hdLayoutCacheNode_c*, hdScreenString*);
void resFormat(hdLayoutPath_c*, const char*, const char*);
void resCopy(be<u8>*, const char*, u32, u32);
/* 026FB26C */
static hdLayoutResMgr_c* constructLayoutResMgr(hdLayoutResMgr_c* object) {
    if (!object) {
        object = (hdLayoutResMgr_c*)operator_new(sizeof(hdLayoutResMgr_c));
        if (!object)
            return NULL;
    }
    object->mpVtable = &sManagerTable;
    object->mBusy = 0;
    object->mpResidentHeap = NULL;
    object->mpThread = NULL;
    object->mpStaffRollHeap = NULL;
    object->mpHeap = NULL;
    hdLayoutCachePool_c* pool =
        gabi::at<hdLayoutCachePool_c>(gabi::ea(object) + offsetof(hdLayoutResMgr_c, mCache));
    if (!pool) {
        pool = (hdLayoutCachePool_c*)operator_new(0x19014);
        if (!pool)
            return object;
    }
    pool->mpRoot = NULL;
    pool->mCapacity = 1024;
    pool->mCount = 0;
    hdLayoutCacheNode_c* nodes = gabi::at<hdLayoutCacheNode_c>(gabi::ea(pool) + sizeof(hdLayoutCachePool_c));
    pool->mpFree = nodes;
    for (u32 i = 0; i < 1023; ++i)
        nodes[i].mpNext = &nodes[i + 1];
    nodes[1023].mpNext = NULL;
    pool->mpStorage = nodes;
    return object;
}
/* 026FB344 */
static hdLayoutResMgr_c* createLayoutResMgr(hdLayoutHeap_c* heap) {
    if (sLayoutResMgr)
        return sLayoutResMgr;
    hdLayoutResMgr_c* object = (hdLayoutResMgr_c*)resAllocateSingleton(sizeof(hdLayoutResMgr_c), heap, 4);
    if (object) {
        resConstructDisposer((hdLayoutDisposer_c*)object, heap, 3);
        ((hdLayoutDisposer_c*)object)->mpVtable = &sDisposerTable;
    }
    sLayoutResDisposer = (hdLayoutDisposer_c*)object;
    sLayoutResMgr = object ? constructLayoutResMgr(object) : NULL;
    return sLayoutResMgr;
}
/* 026FB3F0 */
void hdLayoutResMgr_c::init(hdLayoutHeap_c* heap) {
    mpHeap = heap;
    hdScreenString name;
    name.mText.v = gabi::guest_addr("LayoutResidentResHeap");
    name.mVtable = &sResourceStringTable;
    mpResidentHeap = resCreateHeap(0x4C00000, &name, heap, 1, 0);
    hdLayoutThread_c* thread = (hdLayoutThread_c*)resAllocate(0x98, mpHeap, 4);
    if (thread) {
        name.mText.v = gabi::guest_addr("LayoutResMgr");
        name.mVtable = &sResourceStringTable;
        hdLayoutTask_c* task = (hdLayoutTask_c*)resAllocate(sizeof(hdLayoutTask_c), mpHeap, 4);
        if (task) {
            task->mAdjust = 0;
            task->mpOwner = this;
            task->mpVtable = &sTaskTable;
            task->mMethod.v = gabi::guest_fn((const void*)&preloadLayoutResources);
            task->mVirtualIndex = -1;
        }
        thread =
            resConstructThread(thread, &name, task, mpHeap, sThreadPriority + 1, 0, 0x7FFFFFFF, 0x5000, 32);
    }
    mpThread = thread;
    gabi::call_ptr(thread->mpVtable->mStart.get(), thread);
}
/* 026FB58C */
hdScreenResource* hdLayoutResMgr_c::loadArchive(hdScreenString* name, hdLayoutHeap_c* heap) {
    if (!heap)
        heap = mpHeap;
    hdScreenResource* archive = resFindArchive(sArchiveMgr, name);
    if (archive)
        return archive;
    hdLayoutPath_c path;
    path.mText = path.mBuffer;
    path.mCapacity = 128;
    path.mVtable = &sPathTable;
    path.mBuffer[127] = 0;
    path.mBuffer[0] = 0;
    name->assureTermination();
    resFormat(&path, "Layout/%s.szs", name->mText);
    if (!resLoadArchive(sArchiveMgr, name, &path, heap, 1, 0x40000, 0x2000))
        return NULL;
    return resFindArchive(sArchiveMgr, name);
}
/* 026FB694 */
u32 hdLayoutResMgr_c::cache(hdScreenString* name, hdLayoutHeap_c* heap) {
    if (!heap)
        heap = mpHeap;
    hdLayoutCachePool_c* pool = &mCache;
    hdLayoutCacheNode_c* found = resFindNode(pool, pool->mpRoot, name);
    if (found && u32(gabi::ea(found) + offsetof(hdLayoutCacheNode_c, mpResource)) != 0)
        return 1;
    heap->begin();
    heap->end();
    hdScreenResource* archive = loadArchive(name, heap);
    hdLayoutCachedResource_c* resource =
        (hdLayoutCachedResource_c*)resAllocate(sizeof(hdLayoutCachedResource_c), heap, 4);
    if (resource) {
        hdScreenFixedString* string =
            gabi::at<hdScreenFixedString>(gabi::ea(resource) + offsetof(hdLayoutCachedResource_c, mName));
        if (!string)
            string = (hdScreenFixedString*)operator_new(sizeof(hdScreenFixedString));
        if (string) {
            string->mText = string->mBuffer;
            string->mVtable = &sNameBaseTable;
            string->mCapacity = 64;
            string->mBuffer[63] = 0;
            string->mVtable = &sNameIntermediateTable;
            string->mText->set(0);
            string->mVtable = &sNameFinalTable;
        }
    }
    resource->mpArchive = archive;
    be<u8>* destination = resource->mName.mText;
    name->assureTermination();
    const char* source = name->mText;
    u32 length = 0;
    while (*gabi::at<be<u8>>(gabi::ea(source) + length) != 0) {
        if (++length > 0x40000) {
            length = 0;
            break;
        }
    }
    s32 capacity = resource->mName.mCapacity;
    if ((s32)length >= capacity)
        length = u32(capacity) - 1;
    name->assureTermination();
    resCopy(destination, name->mText, length, 0);
    *gabi::at<be<u8>>(gabi::ea(destination) + length) = 0;
    if ((s32)pool->mCount < (s32)pool->mCapacity) {
        hdLayoutCacheNode_c* node = pool->mpFree;
        if (node)
            pool->mpFree = node->mpNext;
        if (node) {
            node->mpPool = pool;
            node->mpResource = resource;
            node->mpNext = NULL;
            node->mpStringTable = &sResourceStringTable;
            node->mRed = 1;
            node->mNameEnd = 0;
            node->mpName = (const char*)sEmptyName;
            node->mpNodeTable = &sNodeTable;
            node->mUnused = 0;
            name->assureTermination();
            source = name->mText;
            length = 0;
            while (*gabi::at<be<u8>>(gabi::ea(source) + length) != 0) {
                if (++length > 0x40000) {
                    length = 0;
                    break;
                }
            }
            if (length >= 65)
                length = 64;
            name->assureTermination();
            resCopy(node->mNameBuffer, name->mText, length, 0);
            *gabi::at<be<u8>>(gabi::ea(node->mNameBuffer) + length) = 0;
            node->mpName = (const char*)node->mNameBuffer;
        }
        pool->mCount = u32(pool->mCount) + 1;
        pool->mpRoot = resInsertNode(pool, pool->mpRoot, node);
        pool->mpRoot->mRed = 0;
    } else {
        found = resFindNode(pool, pool->mpRoot, name);
        if (found && u32(gabi::ea(found) + offsetof(hdLayoutCacheNode_c, mpResource)) != 0)
            found->mpResource = resource;
    }
    heap->begin();
    heap->end();
    return 1;
}
/* 026FBA38 */
u32 hdLayoutResMgr_c::unload(hdScreenString* name) {
    hdLayoutCacheNode_c* node = resFindNode(&mCache, mCache.mpRoot, name);
    if (!node || u32(gabi::ea(node) + offsetof(hdLayoutCacheNode_c, mpResource)) == 0)
        return 1;
    gptr<hdLayoutCachedResource_c>* value = &node->mpResource;
    if (!resUnloadArchive(sArchiveMgr, name))
        return 0;
    hdLayoutCacheNode_c* again = resFindNode(&mCache, mCache.mpRoot, name);
    if (again && u32(gabi::ea(again) + offsetof(hdLayoutCacheNode_c, mpResource)) != 0) {
        mCache.mpRoot = resEraseNode(&mCache, mCache.mpRoot, name);
        if (mCache.mpRoot)
            mCache.mpRoot->mRed = 0;
    }
    hdLayoutCachedResource_c* resource = *value;
    if (resource)
        operator_delete(resource);
    return 1;
}
/* 026FBB24 */
void* hdLayoutResMgr_c::find(hdScreenString* name) {
    hdLayoutCacheNode_c* node = resFindNode(&mCache, mCache.mpRoot, name);
    if (!node || u32(gabi::ea(node) + offsetof(hdLayoutCacheNode_c, mpResource)) == 0)
        return NULL;
    return ((hdLayoutArchiveData_c*)node->mpResource->mpArchive.get())->mpData;
}
/* 026FBB7C */
u8 hdLayoutResMgr_c::isIdle() {
    return u8(u8(mBusy) ^ 1);
}
/* 026FBB8C */
u32 hdLayoutResMgr_c::resume() {
    if (!isIdle())
        return 0;
    hdLayoutThread_c* thread = mpThread;
    mBusy = 1;
    return gabi::call_ptr<u32>(thread->mpVtable->mResume.get(), thread);
}
/* 026FBBE8 */
void hdLayoutResMgr_c::createStaffRollHeap() {
    hdScreenString name;
    name.mVtable = &sResourceStringTable;
    name.mText.v = gabi::guest_addr("StaffRollRes");
    mpStaffRollHeap = resCreateHeap(0x2800000, &name, sUiManager->mpHeap, -1, 0);
}
/* 026FBC4C */
void hdLayoutResMgr_c::destroyStaffRollHeap() {
    hdLayoutHeap_c* heap = mpStaffRollHeap;
    if (!heap)
        return;
    gabi::call_ptr(heap->mpVtable->mDestroy.get(), heap);
    mpStaffRollHeap = NULL;
}
/* 026FBC98 */
static void preloadLayoutResources(hdLayoutResMgr_c* self, u32 unused, u32 group) {
    hdScreenString name;
    name.mVtable = &sResourceStringTable;
    switch (group) {
    case 1:
        name.mText.v = gabi::guest_addr("Title_00");
        self->cache(&name, NULL);
        break;
    case 2:
        name.mText.v = gabi::guest_addr("Opening_00");
        self->cache(&name, NULL);
        break;
    case 3:
        name.mText.v = gabi::guest_addr("SelectController_00");
        self->cache(&name, NULL);
        name.mText.v = gabi::guest_addr("BtnController_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, NULL);
        name.mText.v = gabi::guest_addr("StartSequeneBG_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, NULL);
        name.mText.v = gabi::guest_addr("SaveData_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, NULL);
        name.mText.v = gabi::guest_addr("BtnSaveData_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, NULL);
        name.mText.v = gabi::guest_addr("BtnDifficulty_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, NULL);
        break;
    case 5:
        name.mText.v = gabi::guest_addr("StaffRoll_00");
        self->cache(&name, self->mpStaffRollHeap);
        name.mText.v = gabi::guest_addr("StaffRollBG_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, self->mpStaffRollHeap);
        name.mText.v = gabi::guest_addr("StaffRollBubble_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, self->mpStaffRollHeap);
        name.mText.v = gabi::guest_addr("StaffRollBubbleS_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, self->mpStaffRollHeap);
        name.mText.v = gabi::guest_addr("Bubble_00");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, self->mpStaffRollHeap);
        name.mText.v = gabi::guest_addr("FadeWipe_01");
        name.mVtable = &sResourceStringTable;
        self->cache(&name, self->mpStaffRollHeap);
        break;
    case 6:
        name.mText.v = gabi::guest_addr("EndingEnd_00");
        self->cache(&name, NULL);
        break;
    }
    self->mBusy = 0;
}
/* 026FBEF8 */
static void destroyLayoutResDisposer(hdLayoutDisposer_c* object, u32 flags) {
    if (!object)
        return;
    object->mpVtable = &sDisposerTable;
    if (object == sLayoutResDisposer) {
        hdLayoutResMgr_c* manager = sLayoutResMgr;
        sLayoutResDisposer = NULL;
        hdScreenDeleteTable* table = (hdScreenDeleteTable*)manager->mpVtable.get();
        gabi::call_ptr(table->mDestroy.get(), manager, 2u);
        sLayoutResMgr = NULL;
    }
    resDestroyDisposer(object, 0);
    if (flags & 1)
        operator_delete(object);
}
/* 026FBFA4 */
static void __sinit_hd_layout_res_mgr_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sResMgrHeaderStatics), gabi::ea(&sResMgrHeaderDescriptors), gabi::ea(&sResMgrHeaderZero),
                                gabi::ea(&sResMgrPiConstants));
}
/* WWHD glue
data sResMgrHeaderStatics 1049DDF8 nocheck
data sResMgrHeaderDescriptors 101F7250 nocheck
data sResMgrHeaderZero 1049DE04 nocheck
data sResMgrPiConstants 1010A9E8 nocheck
data sLayoutResMgr 101F7274
data sLayoutResDisposer 101F7278
data sManagerTable 1010AA00
data sDisposerTable 1010A9F0
data sResourceStringTable 1010A7EC
data sNameBaseTable 1010A804
data sPathTable 1010A864
data sNameIntermediateTable 1010A81C
data sNameFinalTable 1010A834
data sTaskTable 1010A87C
data sNodeTable 1010A88C
data sEmptyName 10000160
data sArchiveMgr 101F4F7C
data sUiManager 101F4F28
data sThreadPriority 101F8B9C
str 1010A8AC "LayoutResidentResHeap"
str 1010A8C4 "LayoutResMgr"
str 1010A8D4 "Layout/%s.szs"
str 1010A8E4 "StaffRollRes"
str 1010A954 "Title_00"
str 1010A960 "Opening_00"
str 1010A8F4 "SelectController_00"
str 1010A96C "BtnController_00"
str 1010A934 "StartSequeneBG_00"
str 1010A908 "SaveData_00"
str 1010A980 "BtnSaveData_00"
str 1010A990 "BtnDifficulty_00"
str 1010A9A4 "StaffRoll_00"
str 1010A9B4 "StaffRollBG_00"
str 1010A9C4 "StaffRollBubble_00"
str 1010A914 "StaffRollBubbleS_00"
str 1010A948 "Bubble_00"
str 1010A928 "FadeWipe_01"
str 1010A9D8 "EndingEnd_00"
ext 0273B050 void* resAllocate(u32,hdLayoutHeap_c*,u32)
ext 0273B0D4 void* resAllocateSingleton(u32,hdLayoutHeap_c*,u32)
ext 02752B0C void resConstructDisposer(hdLayoutDisposer_c*,hdLayoutHeap_c*,u32)
ext 02752BEC void resDestroyDisposer(hdLayoutDisposer_c*,u32)
ext 02753004 hdLayoutHeap_c* resCreateHeap(u32,hdScreenString*,hdLayoutHeap_c*,s32,u32)
ext 0275FBD8 hdLayoutThread_c* resConstructThread(hdLayoutThread_c*,hdScreenString*,hdLayoutTask_c*,hdLayoutHeap_c*,u32,u32,u32,u32,u32)
ext 026123FC hdScreenResource* resFindArchive(void*,hdScreenString*)
ext 02612E64 u32 resLoadArchive(void*,hdScreenString*,hdLayoutPath_c*,hdLayoutHeap_c*,u32,u32,u32)
ext 0261261C u32 resUnloadArchive(void*,hdScreenString*)
ext 02613F20 hdLayoutCacheNode_c* resFindNode(hdLayoutCachePool_c*,hdLayoutCacheNode_c*,hdScreenString*)
ext 0261401C hdLayoutCacheNode_c* resInsertNode(hdLayoutCachePool_c*,hdLayoutCacheNode_c*,hdLayoutCacheNode_c*)
ext 026142C4 hdLayoutCacheNode_c* resEraseNode(hdLayoutCachePool_c*,hdLayoutCacheNode_c*,hdScreenString*)
ext 02759C28 void resFormat(hdLayoutPath_c*,const char*,const char*)
ext C0009988 void resCopy(be<u8>*,const char*,u32,u32)
*/
