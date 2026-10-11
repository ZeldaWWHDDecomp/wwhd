/* HD-only RootTask: graphics heaps, archive/manager initialization and child task creation. */
#include "hd/hd_sys_0203E9EC.h"

static u32 l_taskVtable;
static u32 l_rootVtable;
static u32 l_stringVtable;
static u32 l_fixedStringVtable;
static u32 l_fixedStringBufferVtable;
static u32 l_fixedStringFinalVtable;
static u32 l_resourceBaseVtable;
static u32 l_resourceBaseDisposerVtable;
static u32 l_resourceVtable;
static u32 l_resourceDisposerVtable;
static HdRootPointer l_resourceManager;
static HdRootFileManagerPointer l_fileManager;
static HdRootPointer l_defaultHeapManager;
static HdRootPointer l_input;
static HdRootPointer l_resourceLoader;
static HdRootSaveManagerPointer l_save;
static HdRootPointer l_uiResourceManager;
static HdRootPointer l_fontManager;
static HdRootPointer l_miiverseManager;
static HdRootPointer l_uiGraphics;
static HdRootPointer l_taskManager;
static HdRootProfilePointer l_profile;
static HdRootConfig l_config;
static u32 l_configGuard;
static u32 l_configVtable;
static u32 l_resourceTypeGuard;
static HdRootCallback l_resourceType;
static u32 l_resourceTypeVtable;
static u32 l_resourceTaskGuard;
static HdRootCallback l_resourceTaskType;
static u32 l_resourceTaskTypeVtable;
static u32 l_gpuTaskGuard;
static HdRootCallback l_gpuTaskType;
static u32 l_gpuTaskTypeVtable;
static u32 l_systemTaskGuard;
static HdRootCallback l_systemTaskType;
static u32 l_resourceTaskFactory;
static u32 l_gpuTaskFactory;
static u32 l_systemTaskFactory;
static u32 l_profileTaskFactory;
static u32 l_resourceTaskCallback;
static u32 l_gpuTaskCallback;
static u32 l_systemTaskCallback;
static u32 l_profileTaskCallback;
static HdRootStaticGlobals l_globals;
static HdRootStaticRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdRootStaticRegistration l_registration;
static HdRootStaticRegistration l_signedRegistration;
static HdRootStaticRegistration l_unsignedRegistration;
static u32 l_inputConfig;

enum HdRootSlots : u32 { HdRoot_AdjustHeap = 11, HdRoot_AllocateHeap = 13,
    HdRoot_ResourceCast = 3, HdRoot_TaskCast = 3, HdRoot_ConfigureTask = 11 };
enum HdRootConstants : u32 { HdRoot_Delete = 1, HdRoot_CreateTaskKind = 2 };

static void setString(HdRootSafeString* string, const char* text) {
    string->mVtable = &l_stringVtable;
    string->mpString = gabi::at<void>(gabi::guest_addr(text));
}
static void adjustHeap(HdRootHeap* heap) {
    gabi::call_ptr<u32>(heap->mVtable[HdRoot_AdjustHeap].get(), heap);
}
// HD: relative resource offsets are based on the address of the offset word itself.
static void* resolveOffset(be<u32>* word) {
    u32 offset = *word;
    return offset ? gabi::at<void>(gabi::ea(word) + offset) : NULL;
}
static void* rangeEnd(void* begin, u32 size) {
    return gabi::at<void>(gabi::ea(begin) + size);
}

/* 0203E9EC */
static HdRootTask* HdRootTask__ct(HdRootTask* self, void* arg) {
    if (self == NULL) {
        self = static_cast<HdRootTask*>(hdRootAllocate(sizeof(HdRootTask)));
        if (self == NULL) return NULL;
    }
    hdRootTaskConstruct(self, arg, "RootTask");
    self->mRootVtable = reinterpret_cast<gfn*>(&l_rootVtable);
    self->mTaskVtable = &l_taskVtable;
    self->mpMultiArchive = NULL;
    self->mpArchiveD = NULL;
    self->mpArchiveA = NULL;
    self->mpArchiveC = NULL;
    self->mpArchiveB = NULL;
    l_profile.mpProfile = hdRootCreateProfile(0);
    return self;
}

/* 0203EA88 */
static void hdRoot_setupGraphicsHeaps() {
    // HD: descriptor/config/holder views retain the original overlapping stack storage.
    // HD: replay the original prologue stores required by the frame-exact specification.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x198> native;
    HdRootHeap* root = hdRootGetMainHeap();
    HdRootGfxResource* resource = static_cast<HdRootGfxResource*>(hdRootAllocate(sizeof(HdRootGfxResource)));
    HdRootSafeString* name = gabi::at<HdRootSafeString>(native + 8);
    HdRootGfxConfig* config = gabi::at<HdRootGfxConfig>(native + 0x40);
    HdRootResourceDescriptor* descriptor = gabi::at<HdRootResourceDescriptor>(native + 0x14);
    HdRootHeapHolder* holder = gabi::at<HdRootHeapHolder>(native + 0x10);
    if (resource != NULL) {
        resource->mpNext = NULL;
        resource->mpOwner = resource;
        resource->mpPrevious = NULL;
        resource->mReserved = 0;
        hdRootDisposerConstruct(&resource->padding);
        resource->mVtable = &l_resourceBaseVtable;
        resource->mDisposerVtable = &l_resourceBaseDisposerVtable;
        HdRootFixedString* string = &resource->mName;
        if (!gabi::ea(string)) string = static_cast<HdRootFixedString*>(hdRootAllocate(sizeof(HdRootFixedString)));
        if (string != NULL) {
            HdRootFixedString* buffer = string;
            if (buffer == NULL) buffer = static_cast<HdRootFixedString*>(hdRootAllocate(12));
            if (buffer != NULL) {
                buffer->mpString = string->mBuffer;
                buffer->mVtable = &l_fixedStringVtable;
                buffer->mCapacity = 32;
                string->mBuffer[31] = 0;
            }
            be<u8>* bytes = static_cast<be<u8>*>(string->mpString.get());
            string->mVtable = &l_fixedStringBufferVtable;
            *bytes = 0;
            string->mVtable = &l_fixedStringFinalVtable;
        }
        resource->mVtable = &l_resourceVtable;
        resource->mDisposerVtable = &l_resourceDisposerVtable;
    }
    setString(name, "graphics resource");
    hdRootRegisterGfxResource(l_resourceManager.mpValue, resource, name);
    hdRootGfxConfigConstruct(config);
    config->mCommandFlags = 0;
    config->mGpuAlignment = 0;
    config->mpHeap = root;
    config->mGpuCount = 0;
    config->mCommandMemorySize = 0x01FFFF80;
    config->mCommandSize = 0x00800000;
    config->mGpuMemorySize = 0x02000000;
    config->mGpuMode = 0;
    config->mCommandMode = 0;
    setString(name, "graphics memory heap");
    config->mpHeap = hdRootCreateExpHeap(0, name, root, 1, 0);
    u32 baseHeap = MEMGetBaseHeapHandle(0);
    config->mpCommandMemory = MEMAllocFromFrmHeapEx(baseHeap, config->mCommandMemorySize, 4);
    HdRootHeap* heap = config->mpHeap;
    config->mpGpuMemory = gabi::call_ptr<void*>(heap->mVtable[HdRoot_AllocateHeap].get(), heap,
                                               (u32)config->mGpuMemorySize, 0x2000u);
    config->mGpuFlags = 0;
    adjustHeap(config->mpHeap);
    HdRootHeap* parent = config->mpHeap->mpParent;
    void* manager = l_resourceManager.mpValue;
    descriptor->mFirst = 0;
    descriptor->mSize = 0x2000;
    descriptor->mSecond = 0;
    descriptor->mFourth = 0;
    descriptor->mName.mVtable = &l_stringVtable;
    config->mpHeap = parent;
    descriptor->mFlags = 0;
    descriptor->mAlignment = 0x20;
    descriptor->mName.mpString = gabi::at<void>(gabi::guest_addr("graphics resource data"));
    descriptor->mThird = 0;
    descriptor->mFifth = 0;
    descriptor->mMode = 0;
    HdRootResource* graphicsResource = hdRootFindResource(manager, descriptor);
    if (!l_resourceTypeGuard) {
        l_resourceTypeGuard = 1;
        l_resourceType.mVtable = &l_resourceTypeVtable;
    }
    if (graphicsResource != NULL && !gabi::call_ptr<u32>(graphicsResource->mVtable[HdRoot_ResourceCast].get(), graphicsResource, &l_resourceType)) graphicsResource = NULL;
    setString(name, "graphics initialization heap");
    config->mpHeap = hdRootCreateExpHeap(0, name, config->mpHeap, 1, 0);
    hdRootGfxInitialize(config);
    adjustHeap(config->mpHeap);
    config->mpHeap = config->mpHeap->mpParent;
    hdRootGfxSetMode(0);
    hdRootGfxSetResource(graphicsResource);
    setString(name, "graphics command heap");
    HdRootHeap* commandHeap = hdRootCreateExpHeap(0, name, root, 1, 0);
    hdRootHeapHolderConstruct(holder);
    holder->mpHeap = commandHeap;
    hdRootHeapHolderApply(holder);
    adjustHeap(commandHeap);
}

static HdRootTask* createTask(HdRootTask* self, HdRootTaskCreateArg* source,
                              HdRootTaskCreateArg* destination, void* callback) {
    HdRootTaskArgWords* srcWords = reinterpret_cast<HdRootTaskArgWords*>(source);
    HdRootTaskArgWords* dstWords = reinterpret_cast<HdRootTaskArgWords*>(destination);
    for (u32 i = 0; i < 32; i++) dstWords->mWords[i] = srcWords->mWords[i];
    destination->mCallback = callback;
    return hdRootCreateTask(self->mpTaskManager, destination);
}
static void castTask(HdRootTask* task, be<u32>* guard, HdRootCallback* type, void* value) {
    if (!*guard) { type->mVtable = value; *guard = 1; }
    if (task != NULL) gabi::call_ptr<u32>(task->mRootVtable[HdRoot_TaskCast].get(), task, type);
}

/* 0203EE2C */
void HdRootTask::prepare() {
    // HD: replay the original prologue stores required by the frame-exact specification.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x220> native;
    u32 flags;
    if (l_configGuard) flags = l_config.mOptions;
    else {
        l_configGuard = 1;
        l_config.mPriority = 1;
        l_config.mEnabled = 1;
        l_config.mVtable = &l_configVtable;
        l_config.mThreadID = 0x2332;
        l_config.mFlags = 0x4D2;
        flags = 0x140D;
    }
    l_config.mStackSize = 0x40000;
    l_config.mHeapSize = 0x80000;
    l_config.mPoolSize = 0x200000;
    l_config.mOptions = (flags & 0xFFFFFFEEu) | 0x20;
    hdRootProcessInitialize();
    hdRootSystemInitialize();
    void* process = hdRootGetProcess();
    hdRootSetProcessPhase(process, 0);
    HdRootConfigArg* configArg = gabi::at<HdRootConfigArg>(native + 0x70);
    configArg->mpConfig = &l_config;
    configArg->mFirst = 0;
    configArg->mSecond = 0;
    WPADEnableURCC(1);
    WPADEnableWiiRemote(0);
    HdRootCreateInfo* createInfo = mpTaskManager->mpCreateInfo;
    gabi::call_ptr<u32>(createInfo->mVtable[HdRoot_ConfigureTask].get(), createInfo, this, configArg);
    WPADEnableURCC(1);
    WPADEnableWiiRemote(0);
    hdRootInputInitialize(0);
    void* device = l_fileManager.mpManager->mpDefaultDevice;
    HdRootSafeString* nameA = gabi::at<HdRootSafeString>(native + 8);
    HdRootSafeString* nameB = gabi::at<HdRootSafeString>(native + 0x10);
    setString(nameA, "archive C path");
    setString(nameB, "archive C name");
    mpArchiveC = hdRootLoadArchive(0, nameB, device, nameA);
    setString(nameA, "archive D path");
    setString(nameB, "archive D name");
    mpArchiveD = hdRootLoadArchive(0, nameA, device, nameB);
    setString(nameA, "archive B path");
    setString(nameB, "archive B name");
    mpArchiveB = hdRootLoadArchive(0, nameA, device, nameB);
    setString(nameA, "archive A path");
    setString(nameB, "archive A name");
    mpArchiveA = hdRootLoadArchive(0, nameA, device, nameB);
    void* multi = hdRootCreateMultiArchive(0, 4, 0, 4);
    mpMultiArchive = multi;
    hdRootAppendArchive(multi, mpArchiveC);
    hdRootAppendArchive(mpMultiArchive, mpArchiveD);
    hdRootAppendArchive(mpMultiArchive, mpArchiveB);
    hdRootAppendArchive(mpMultiArchive, mpArchiveA);
    l_fileManager.mpManager->mpDefaultDevice = mpMultiArchive;
    HdRootSafeString* mountA = gabi::at<HdRootSafeString>(native + 0x38);
    HdRootSafeString* mountB = gabi::at<HdRootSafeString>(native + 0x40);
    void* firstMount = hdRootCreateMount(0, 0xFF);
    setString(mountA, "first mount");
    hdRootMountDevice(l_fileManager.mpManager, firstMount, mountA);
    nn_act_Initialize();
    u32 slot = nn_act_GetSlotNo();
    void* secondMount = hdRootCreateMount(0, slot);
    setString(mountB, "second mount");
    hdRootMountDevice(l_fileManager.mpManager, secondMount, mountB);
    hdRootHeapInitialize(0);
    nameA->mVtable = &l_stringVtable;
    void* heapManager = l_defaultHeapManager.mpValue;
    // HD: the unchecked heap index uses wrapping guest address arithmetic.
    gptr<HdRootHeap>* selected = gabi::at<gptr<HdRootHeap>>(gabi::ea(mHeaps) + (u32)mHeapIndex * 4);
    HdRootHeap* selectedHeap = *selected;
    nameA->mpString = gabi::at<void>(gabi::guest_addr("default heap"));
    hdRootSelectDefaultHeap(heapManager, selectedHeap, nameA);
    HdRootTaskInfo* info = hdRootGetTaskInfo(this);
    info->mpHeapControl->mFlags = 0;
    hdRoot_setupGraphicsHeaps();
    hdRootInputConfigure(l_input.mpValue, &l_inputConfig, 0);
    HdRootSafeString* heapName = gabi::at<HdRootSafeString>(native + 0x18);
    setString(heapName, "resource heap");
    HdRootHeap* root = hdRootGetMainHeap();
    HdRootHeap* resourceHeap = hdRootCreateExpHeap(0xB00000, heapName, root, 1, 0);
    hdRootResourceInitialize(resourceHeap);
    hdRootResourceSetHeap(l_resourceLoader.mpValue, resourceHeap);
    u32 commandSize = hdRootHeapOverhead(4) + 0xE00000;
    setString(heapName, "command heap");
    root = hdRootGetMainHeap();
    HdRootHeap* commandHeap = hdRootCreateExpHeap(commandSize, heapName, root, 1, 0);
    u32 systemSize = 0x16C00000 - commandSize;
    setString(heapName, "system heap");
    root = hdRootGetMainHeap();
    HdRootHeap* systemHeap = hdRootCreateExpHeap(systemSize, heapName, root, 1, 1);
    u32 dataSize = hdRootHeapOverhead(0x2000) + 0x0D400000;
    setString(heapName, "data heap");
    HdRootHeap* dataHeap = hdRootCreateExpHeap(dataSize, heapName, systemHeap, ~0u, 0);
    hdRootModelInitialize(systemHeap);
    HdRootSaveManager* save = l_save.mpManager;
    hdRootSaveInitialize(save, systemHeap);
    save->mpCommandHeap = commandHeap;
    save->mpSystemHeap = systemHeap;
    save->mpDataHeap = dataHeap;
    HdRootSafeString* archiveName = gabi::at<HdRootSafeString>(native + 0x20);
    HdRootSafeString* archivePath = gabi::at<HdRootSafeString>(native + 0x48);
    setString(archiveName, "common archive");
    root = hdRootGetArchiveHeap();
    HdRootHeap* archiveHeap = hdRootCreateExpHeap(0x100000, archiveName, root, 1, 0);
    archivePath->mVtable = &l_stringVtable;
    setString(archiveName, "common archive");
    archivePath->mpString = gabi::at<void>(gabi::guest_addr("common archive path"));
    hdRootLoadArchiveResource(l_resourceLoader.mpValue, archiveName, archivePath, archiveHeap, 1, 0x40000, 0x2000);
    setString(nameA, "common archive");
    setString(nameB, "shader archive");
    void* archiveResource = hdRootGetArchiveResource(l_resourceLoader.mpValue, nameA, nameB, 0);
    HdRootArchive* archive = hdRootCastArchive(archiveResource);
    HdRootSafeString* graphicsName = gabi::at<HdRootSafeString>(native + 0x50);
    setString(graphicsName, "UI graphics heap");
    root = hdRootGetGraphicsHeap();
    HdRootHeap* graphicsHeap = hdRootCreateExpHeap(0x2000000, graphicsName, root, 1, 0);
    hdRootUiGraphicsInitialize(graphicsHeap);
    HdRootDataRange* firstRange = gabi::at<HdRootDataRange>(native + 0x28);
    HdRootDataRange* secondRange = gabi::at<HdRootDataRange>(native + 0x30);
    firstRange->mpEnd = NULL;
    firstRange->mpBegin = NULL;
    void* files = resolveOffset(&archive->mFilesOffset);
    HdRootFileEntry* file = hdRootFindArchiveFile(files, "first shader binary");
    u32 offset = file->mDataOffset;
    u32 size = file->mSize;
    void* first = offset ? gabi::at<void>(gabi::ea(file) + offset) : NULL;
    secondRange->mpEnd = NULL;
    firstRange->mpEnd = rangeEnd(first, size);
    secondRange->mpBegin = NULL;
    firstRange->mpBegin = first;
    files = resolveOffset(&archive->mFilesOffset);
    file = hdRootFindArchiveFile(files, "second shader binary");
    offset = file->mDataOffset;
    size = file->mSize;
    void* second = offset ? gabi::at<void>(gabi::ea(file) + offset) : NULL;
    secondRange->mpBegin = second;
    secondRange->mpEnd = rangeEnd(second, size);
    hdRootSetShaderRanges(l_uiGraphics.mpValue, graphicsHeap, firstRange, secondRange);
    HdRootSafeString* uiName = gabi::at<HdRootSafeString>(native + 0x58);
    setString(uiName, "UI resource heap");
    root = hdRootGetGraphicsHeap();
    HdRootHeap* uiHeap = hdRootCreateExpHeap(0x5C00000, uiName, root, 1, 0);
    hdRootUiResourceInitialize(uiHeap);
    hdRootUiResourceSetHeap(l_uiResourceManager.mpValue, uiHeap);
    HdRootHeap* fontHeap = hdRootGetFontHeap();
    hdRootFontInitialize(fontHeap);
    hdRootFontSetHeap(l_fontManager.mpValue, fontHeap);
    setString(nameA, "font resource heap");
    HdRootHeap* fontResourceHeap = hdRootCreateFrameHeap(0, nameA, fontHeap, 1, 0);
    void* fontResource = hdRootCreateFontResource(fontResourceHeap);
    adjustHeap(fontResourceHeap);
    hdRootRegisterFontResource(fontResource);
    hdRootMiiverseInitialize(0);
    hdRootMiiverseConfigure(l_miiverseManager.mpValue, 0);
    HdRootTaskFactory* factory = gabi::at<HdRootTaskFactory>(native + 0x60);
    HdRootTaskCreateArg* source = gabi::at<HdRootTaskCreateArg>(native + 0x80);
    HdRootTaskCreateArg* firstTask = gabi::at<HdRootTaskCreateArg>(native + 0x100);
    HdRootTaskCreateArg* secondTask = gabi::at<HdRootTaskCreateArg>(native + 0x180);
    factory->mKind = HdRoot_CreateTaskKind;
    factory->mFactory = &l_resourceTaskFactory;
    hdRootTaskArgsConstruct(source, factory);
    HdRootHeap* childHeap = hdRootGetChildHeap();
    source->mHeaps[source->mHeapIndex].mpHeap = childHeap;
    HdRootTaskHeapSpec* heapSpec = &source->mHeaps[source->mHeapIndex];
    u32 overhead = hdRootHeapOverhead(4);
    HdRootProfile* profile = l_profile.mpProfile;
    heapSpec->mSize = 0x04000000 - overhead;
    source->mpParent = this;
    source->mpResource = profile->mpResource;
    HdRootTask* task = createTask(this, source, firstTask, &l_resourceTaskCallback);
    castTask(task, &l_resourceTaskGuard, &l_resourceTaskType, &l_resourceTaskTypeVtable);
    hdRootBasePrepare(this);
    HdRootTaskFactory* secondFactory = gabi::at<HdRootTaskFactory>(native + 0x68);
    secondFactory->mKind = HdRoot_CreateTaskKind;
    secondFactory->mFactory = &l_gpuTaskFactory;
    hdRootTaskArgsConstructAlternate(source, secondFactory);
    task = createTask(this, source, secondTask, &l_gpuTaskCallback);
    castTask(task, &l_gpuTaskGuard, &l_gpuTaskType, &l_gpuTaskTypeVtable);
    process = hdRootGetProcess();
    hdRootSetProcessPhase(process, 1);
}

/* 0203F670 */
void HdRootTask::enter() {
    // HD: replay the original prologue stores required by the frame-exact specification.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x1A8> native;
    HdRootTaskFactory* factory = gabi::at<HdRootTaskFactory>(native + 8);
    HdRootTaskCreateArg* source = gabi::at<HdRootTaskCreateArg>(native + 0x118);
    HdRootTaskCreateArg* first = gabi::at<HdRootTaskCreateArg>(native + 0x18);
    factory->mKind = HdRoot_CreateTaskKind;
    factory->mFactory = &l_systemTaskFactory;
    hdRootTaskArgsConstruct(source, factory);
    HdRootTask* task = createTask(this, source, first, &l_systemTaskCallback);
    castTask(task, &l_systemTaskGuard, &l_systemTaskType, &l_gpuTaskTypeVtable);
    HdRootTaskFactory* secondFactory = gabi::at<HdRootTaskFactory>(native + 0x10);
    HdRootTaskCreateArg* second = gabi::at<HdRootTaskCreateArg>(native + 0x98);
    secondFactory->mKind = HdRoot_CreateTaskKind;
    secondFactory->mFactory = &l_profileTaskFactory;
    hdRootTaskArgsConstruct(source, secondFactory);
    task = createTask(this, source, second, &l_profileTaskCallback);
    castTask(task, &l_systemTaskGuard, &l_systemTaskType, &l_gpuTaskTypeVtable);
    hdRootTaskManagerFinalize(l_taskManager.mpValue, 0);
}

/* 0203F810 */
static void __sinit_hd_root_task_cpp() {
    l_globals.mThird = 0;
    l_globals.mFirst = 0;
    l_globals.mFourth = 0;
    l_globals.mSecond = 0;
    hdRootRegisterStatic(&l_registration);
    f32 lower = l_lower;
    f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdRootInitSigned(&l_range.mSignedInitialized);
    hdRootRegisterStatic(&l_signedRegistration);
    hdRootInitUnsigned(&l_range.mUnsignedInitialized);
    hdRootRegisterStatic(&l_unsignedRegistration);
}

/* 0203F8A4 */
static void hdRoot_deleteFirstCompanion(void* self, u32 flags) {
    if (self == NULL) return;
    if (flags & HdRoot_Delete) hdRootDelete(self);
}
/* 0203F8B8 */
static void hdRoot_deleteSecondCompanion(void* self, u32 flags) {
    if (self == NULL) return;
    if (flags & HdRoot_Delete) hdRootDelete(self);
}

/* WWHD glue (tools/verify/autoglue.py): native RootTask services, data and imports.
data l_taskVtable 100067A0
data l_rootVtable 100066E0
data l_stringVtable 100063F8
data l_fixedStringVtable 10006410
data l_fixedStringBufferVtable 10006468
data l_fixedStringFinalVtable 10006480
data l_resourceBaseVtable 1015DFE0
data l_resourceBaseDisposerVtable 1015E018
data l_resourceVtable 100064D0
data l_resourceDisposerVtable 10006510
data l_resourceManager 101F8B68
data l_fileManager 101F8B08
data l_defaultHeapManager 101F8B28
data l_input 101F5088
data l_resourceLoader 101F4F7C
data l_save 101F4F28
data l_uiResourceManager 101F7274
data l_fontManager 101F852C
data l_miiverseManager 1018F3F4
data l_uiGraphics 1018C404
data l_taskManager 101F8BA8
data l_profile 104612E0
data l_config 104612BC
data l_configGuard 104612B8
data l_configVtable 100064B8
data l_resourceTypeGuard 101FD6F4
data l_resourceType 101FD9D0
data l_resourceTypeVtable 100064A8
data l_resourceTaskGuard 101FD59C
data l_resourceTaskType 101FD9D4
data l_resourceTaskTypeVtable 10006458
data l_gpuTaskGuard 101FD71C
data l_gpuTaskType 101FD9D8
data l_gpuTaskTypeVtable 10006448
data l_systemTaskGuard 101FD598
data l_systemTaskType 101FD97C
data l_resourceTaskFactory 0203FAF4
data l_gpuTaskFactory 0203FB48
data l_systemTaskFactory 0203FB9C
data l_profileTaskFactory 0203FBF0
data l_resourceTaskCallback 02760FE4
data l_gpuTaskCallback 02729488
data l_systemTaskCallback 0203FC78
data l_profileTaskCallback 0203E360
data l_globals 104612A8
data l_range 1046129C
data l_lower 100066D4
data l_upper 100066D8
data l_registration 1018F560
data l_signedRegistration 1018F56C
data l_unsignedRegistration 1018F578
data l_inputConfig 104A0CD8
str 10006520 "RootTask" renamed
str 1000652C "graphics resource" renamed
str 10006534 "graphics memory heap" renamed
str 10006564 "graphics resource data" renamed
str 10006580 "graphics initialization heap" renamed
str 10006558 "graphics command heap" renamed
str 10006624 "archive C path" renamed
str 1000659C "archive C name" renamed
str 10006630 "archive D path" renamed
str 1000663C "archive D name" renamed
str 100065A8 "archive B path" renamed
str 10006590 "archive B name" renamed
str 10006598 "archive A path" renamed
str 100065A4 "archive A name" renamed
str 100065C0 "first mount" renamed
str 1000664C "second mount" renamed
str 10006658 "default heap" renamed
str 100065B0 "resource heap" renamed
str 100065CC "command heap" renamed
str 10006688 "system heap" renamed
str 100065DC "data heap" renamed
str 100065B8 "common archive" renamed
str 100065F0 "common archive path" renamed
str 10006694 "shader archive" renamed
str 10006600 "UI graphics heap" renamed
str 100066A0 "first shader binary" renamed
str 1000660C "second shader binary" renamed
str 100066B8 "UI resource heap" renamed
str 100066C8 "font resource heap" renamed
ext 0273AD10 void* hdRootAllocate(u32)
ext 0273AF40 void hdRootDelete(void*)
ext 02748100 void hdRootTaskConstruct(HdRootTask*, void*, const char*)
ext 0202BE64 HdRootProfile* hdRootCreateProfile(u32)
ext 0203E8F4 HdRootHeap* hdRootGetMainHeap()
ext 02752A84 void hdRootDisposerConstruct(void*)
ext 0275D6CC void hdRootRegisterGfxResource(void*, HdRootGfxResource*, HdRootSafeString*)
ext 027B5534 void hdRootGfxConfigConstruct(HdRootGfxConfig*)
ext 02753004 HdRootHeap* hdRootCreateExpHeap(u32, HdRootSafeString*, HdRootHeap*, u32, u32)
ext C0009790 u32 MEMGetBaseHeapHandle(u32)
ext C00096A0 void* MEMAllocFromFrmHeapEx(u32, u32, u32)
ext 0275DB94 HdRootResource* hdRootFindResource(void*, HdRootResourceDescriptor*)
ext 027B56D0 void hdRootGfxInitialize(HdRootGfxConfig*)
ext 027B59E4 void hdRootGfxSetMode(u32)
ext 027B59F4 void hdRootGfxSetResource(HdRootResource*)
ext 0278D258 void hdRootHeapHolderConstruct(HdRootHeapHolder*)
ext 0278D294 void hdRootHeapHolderApply(HdRootHeapHolder*)
ext 02749ECC HdRootTask* hdRootCreateTask(HdRootTaskManager*, HdRootTaskCreateArg*)
ext 02035E2C void hdRootProcessInitialize()
ext 02032B48 void hdRootSystemInitialize()
ext 02035D78 void* hdRootGetProcess()
ext 02035ED4 void hdRootSetProcessPhase(void*, u32)
ext C00083D8 void WPADEnableURCC(u32)
ext C00083E8 void WPADEnableWiiRemote(u32)
ext 026177B8 void hdRootInputInitialize(u32)
ext 0273EFF8 void* hdRootLoadArchive(u32, HdRootSafeString*, void*, HdRootSafeString*)
ext 0273F9E0 void* hdRootCreateMultiArchive(u32, u32, u32, u32)
ext 0273FBB8 void hdRootAppendArchive(void*, void*)
ext 0274468C void* hdRootCreateMount(u32, u32)
ext 02741770 void hdRootMountDevice(HdRootFileManager*, void*, HdRootSafeString*)
ext C00046E8 void nn_act_Initialize()
ext C0004660 u32 nn_act_GetSlotNo()
ext 027507E8 void hdRootHeapInitialize(u32)
ext 02750940 void hdRootSelectDefaultHeap(void*, HdRootHeap*, HdRootSafeString*)
ext 02748974 HdRootTaskInfo* hdRootGetTaskInfo(HdRootTask*)
ext 02617858 void hdRootInputConfigure(void*, void*, u32)
ext 0260FBF4 void hdRootResourceInitialize(HdRootHeap*)
ext 0260FC98 void hdRootResourceSetHeap(void*, HdRootHeap*)
ext 0275320C u32 hdRootHeapOverhead(u32)
ext 02603680 void hdRootModelInitialize(HdRootHeap*)
ext 0260ABF8 void hdRootSaveInitialize(HdRootSaveManager*, HdRootHeap*)
ext 0203E8DC HdRootHeap* hdRootGetArchiveHeap()
ext 02612E64 void hdRootLoadArchiveResource(void*, HdRootSafeString*, HdRootSafeString*, HdRootHeap*, u32, u32, u32)
ext 026124B0 void* hdRootGetArchiveResource(void*, HdRootSafeString*, HdRootSafeString*, u32)
ext 027E2DC0 HdRootArchive* hdRootCastArchive(void*)
ext 0203E8E8 HdRootHeap* hdRootGetGraphicsHeap()
ext 02005B90 void hdRootUiGraphicsInitialize(HdRootHeap*)
ext 027DFA24 HdRootFileEntry* hdRootFindArchiveFile(void*, const char*)
ext 02005C38 void hdRootSetShaderRanges(void*, HdRootHeap*, HdRootDataRange*, HdRootDataRange*)
ext 026FB344 void hdRootUiResourceInitialize(HdRootHeap*)
ext 026FB3F0 void hdRootUiResourceSetHeap(void*, HdRootHeap*)
ext 0203E8B8 HdRootHeap* hdRootGetFontHeap()
ext 02720A5C void hdRootFontInitialize(HdRootHeap*)
ext 02720B34 void hdRootFontSetHeap(void*, HdRootHeap*)
ext 02754B38 HdRootHeap* hdRootCreateFrameHeap(u32, HdRootSafeString*, HdRootHeap*, u32, u32)
ext 02720400 void* hdRootCreateFontResource(HdRootHeap*)
ext 02720510 void hdRootRegisterFontResource(void*)
ext 020360FC void hdRootMiiverseInitialize(u32)
ext 020361E4 void hdRootMiiverseConfigure(void*, u32)
ext 02748CFC void hdRootTaskArgsConstruct(HdRootTaskCreateArg*, HdRootTaskFactory*)
ext 0203E8AC HdRootHeap* hdRootGetChildHeap()
ext 027488E8 void hdRootBasePrepare(HdRootTask*)
ext 02748B98 void hdRootTaskArgsConstructAlternate(HdRootTaskCreateArg*, HdRootTaskFactory*)
ext 0276198C void hdRootTaskManagerFinalize(void*, u32)
ext 028F026C void hdRootRegisterStatic(void*)
ext 028ED6F8 void hdRootInitSigned(be<u8>*)
ext 028EAB2C void hdRootInitUnsigned(be<u8>*)
*/
