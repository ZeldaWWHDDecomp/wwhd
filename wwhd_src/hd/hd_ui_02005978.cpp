/* HD layout child, graphics singleton and application entry. Private reconstruction. */
#include "hd/hd_app_entry.h"
using namespace gabi;
extern void *hdAppAllocate(u32);
extern void hdAppDelete(void *);
extern void *hdAppHeapAllocate(u32, HdAppHeap *, u32);
extern void *hdAppHeapNew(u32, HdAppHeap *, u32);
extern void hdAppDisposerConstruct(HdAppDisposer *, HdAppHeap *, u32);
extern void hdAppDisposerDestroy(HdAppDisposer *, u32);
extern void *hdAppAllocatorConstruct(void *, HdAppHeap *);
extern void hdAppSetAllocator(void *);
extern void hdAppThreadEnter(void *);
extern void hdAppThreadLeave(void *);
extern void *hdAppResourceConstruct(void *);
extern void hdAppResourceSetup(void *, u32, void *);
extern void *hdAppFontConstruct(void *);
extern void hdAppFontSetup(void *, void *, void *);
extern void *hdAppDrawInfoConstruct(void *);
extern void hdAppRuntimeInit();
extern void hdAppSystemInit();
extern void hdAppSystemConfigure();
extern void hdAppArenaQuery(HdAppArena *);
extern HdAppBaseHeap *hdAppBaseHeap(u32);
extern u32 hdAppExpHeapFree(HdAppBaseHeap *);
extern u32 hdAppExpHeapAvailable(HdAppBaseHeap *, u32);
extern void hdAppArenaConfigure(HdAppArena *);
extern void hdAppRootInit();
extern void hdAppRootConfigure();
extern u32 hdAppWidth();
extern u32 hdAppHeight();
extern u32 hdAppDisplayWidth();
extern u32 hdAppDisplayHeight();
extern HdAppHeap *hdAppCreateHeap(u32, HdAppSafeString *, HdAppHeap *, u32, u32);
extern HdAppFramework *hdAppFrameworkConstruct(HdAppFramework *, HdAppFrameworkParams *);
extern void hdAppGraphicsSystemCreate(HdAppGraphicsSystem *, HdAppHeap *, u32, u32, u32);
extern void hdAppFrameworkSetup(HdAppFramework *, HdAppHeap *, s32, void *, HdAppDimensions *,
                                HdAppDimensions *, void *);
extern void hdAppTaskArgsConstruct(HdAppTaskArgs *, HdAppTaskFactory *);
extern void *hdAppRunArgConstruct(HdAppRunArg *);
extern void hdAppRegister(void *);
extern void hdAppInitSigned(be<u8> *);
extern void hdAppInitUnsigned(be<u8> *);
static u32 l_childStringVTable;
static u32 l_shaderStringVTable;
static u32 l_appStringVTable;
static u32 l_graphicsVTable;
static u32 l_disposerVTable;
static u32 l_emptyName;
static HdAppGraphicsPointer l_graphics;
static HdAppDisposerPointer l_disposer;
static HdAppThreadPointer l_thread;
static HdAppSafeString l_fontShader;
static HdAppSafeString l_layoutShader;
static u32 l_rootReady;
static HdAppRootPointer l_rootArray;
static HdAppGraphicsSystem l_graphicsSystem;
static HdAppHeapSizes l_heapSizes;
static u32 l_rootTaskEntry;
static u32 l_memoryBase;
static HdAppStatics l_statics1;
static HdAppRange l_range1;
static f32 l_lower1;
static f32 l_upper1;
static HdAppRegistration l_registration1_1;
static HdAppRegistration l_registration1_2;
static HdAppRegistration l_registration1_3;
static HdAppStatics l_statics2;
static HdAppRange l_range2;
static f32 l_lower2;
static f32 l_upper2;
static HdAppRegistration l_registration2_1;
static HdAppRegistration l_registration2_2;
static HdAppRegistration l_registration2_3;
static HdAppStatics l_statics3;
static HdAppRange l_range3;
static f32 l_lower3;
static f32 l_upper3;
static HdAppRegistration l_registration3_1;
static HdAppRegistration l_registration3_2;
static HdAppRegistration l_registration3_3;

/* 02005978 */
HdAppChild *hdAppChildConstruct(HdAppChild *self, HdAppRect *rect, HdAppOwner *owner, u32 value) {
    if (!self) {
        self = static_cast<HdAppChild *>(hdAppAllocate(sizeof(HdAppChild)));
        if (!self)
            return NULL;
    }
    self->mName.mVTable = &l_childStringVTable;
    self->mName.mText = &l_emptyName;
    // HD: these embedded addresses can wrap to zero for the steered high-address receivers.
    HdAppSafeString *name =
        gabi::at<HdAppSafeString>(gabi::ea(self) + offsetof(HdAppChild, mOwnerName));
    void *ownerName = owner->mName;
    if (!name)
        name = static_cast<HdAppSafeString *>(hdAppAllocate(sizeof(HdAppSafeString)));
    if (name) {
        name->mText = ownerName;
        name->mVTable = &l_childStringVTable;
    }
    self->mEnabled = 1;
    HdAppSafeString *text =
        gabi::at<HdAppSafeString>(gabi::ea(self) + offsetof(HdAppChild, mOwnerText));
    void *ownerText = owner->mText;
    if (!text)
        text = static_cast<HdAppSafeString *>(hdAppAllocate(sizeof(HdAppSafeString)));
    if (text) {
        text->mVTable = &l_childStringVTable;
        text->mText = gabi::at<void>(gabi::ea(ownerText) + 0x80);
    }
    self->mValue = value;
    self->mOwner = owner;
    self->mRect.mWords[0] = rect->mWords[0];
    self->mRect.mWords[1] = rect->mWords[1];
    self->mRect.mWords[2] = rect->mWords[2];
    self->mReserved34 = 0;
    self->mRect.mWords[3] = rect->mWords[3];
    self->mReserved38 = 0;
    return self;
}

/* 02005A7C */
void hdAppChildDestroy(HdAppChild *self, s32 flags) {
    if (!self)
        return;
    if (!(flags & 1))
        return;
    hdAppDelete(self);
}

static void initStatics(HdAppStatics *statics, HdAppRange *range, const be<f32> *lower,
                        const be<f32> *upper, HdAppRegistration *first, HdAppRegistration *second,
                        HdAppRegistration *third) {
    statics->mWords[2] = 0;
    statics->mWords[0] = 0;
    statics->mWords[3] = 0;
    statics->mWords[1] = 0;
    hdAppRegister(first);
    const f32 a = *lower;
    const f32 b = *upper;
    range->mLower = a;
    range->mUpper = b;
    hdAppInitSigned(&range->mSigned);
    hdAppRegister(second);
    hdAppInitUnsigned(&range->mUnsigned);
    hdAppRegister(third);
}

/* 02005A90 */
void hdAppChildStaticInit() {
    initStatics(&l_statics1, &l_range1, &l_lower1, &l_upper1, &l_registration1_1,
                &l_registration1_2, &l_registration1_3);
}

/* 02005B24 */
void hdAppChildDeleteCompanion(void *self, s32 flags) {
    if (!self)
        return;
    if (!(flags & 1))
        return;
    hdAppDelete(self);
}

/* 02005B38 */
void hdAppChildEmptyCompanion() {}

/* 02005B3C */
HdAppGraphics *hdAppGraphicsConstruct(HdAppGraphics *self) {
    if (!self) {
        self = static_cast<HdAppGraphics *>(hdAppAllocate(sizeof(HdAppGraphics)));
        if (!self)
            return NULL;
    }
    self->mHeap = NULL;
    self->mVTable = reinterpret_cast<gfn *>(&l_graphicsVTable);
    self->mDrawInfo = NULL;
    self->mFont = NULL;
    self->mResource = NULL;
    return self;
}

/* 02005B90 */
void hdAppGraphicsCreateInstance(HdAppHeap *heap) {
    if (l_graphics.mInstance)
        return;
    HdAppGraphics *instance =
        static_cast<HdAppGraphics *>(hdAppHeapAllocate(sizeof(HdAppGraphics), heap, 4));
    if (instance) {
        hdAppDisposerConstruct(&instance->mDisposer, heap, 3);
        instance->mDisposer.mVTable = reinterpret_cast<gfn *>(&l_disposerVTable);
    }
    l_disposer.mDisposer = reinterpret_cast<HdAppDisposer *>(instance);
    HdAppGraphics *result = instance;
    if (instance)
        result = hdAppGraphicsConstruct(instance);
    l_graphics.mInstance = result;
}

/* 02005C24 */
void hdAppGraphicsDestroy(HdAppGraphics *self, s32 flags) {
    if (!self)
        return;
    if (!(flags & 1))
        return;
    hdAppDelete(self);
}

/* 02005C38 */
void hdAppGraphicsSetup(HdAppGraphics *self, HdAppHeap *heap, void *resourceData, void *fontData) {
    self->mHeap = heap;
    void *allocator = hdAppAllocatorConstruct(NULL, heap);
    hdAppSetAllocator(allocator);
    hdAppThreadEnter(l_thread.mThread);
    void *resource = hdAppHeapNew(0x310, self->mHeap, 4);
    if (resource)
        resource = hdAppResourceConstruct(resource);
    self->mResource = resource;
    hdAppResourceSetup(resource, 0x200, resourceData);
    void *font = hdAppHeapNew(0x168, self->mHeap, 4);
    if (font)
        font = hdAppFontConstruct(font);
    self->mFont = font;
    hdAppFontSetup(font, self->mResource, fontData);
    hdAppThreadLeave(l_thread.mThread);
    void *drawInfo = hdAppHeapNew(0x68, self->mHeap, 4);
    if (drawInfo)
        drawInfo = hdAppDrawInfoConstruct(drawInfo);
    self->mDrawInfo = drawInfo;
}

/* 02005D1C */
void hdAppGraphicsDisposerDestroy(HdAppDisposer *self, s32 flags) {
    if (!self)
        return;
    self->mVTable = reinterpret_cast<gfn *>(&l_disposerVTable);
    if (self == l_disposer.mDisposer) {
        HdAppGraphics *instance = l_graphics.mInstance;
        l_disposer.mDisposer = NULL;
        gabi::call_ptr<void>(instance->mVTable[3].get(), instance, 2);
        l_graphics.mInstance = NULL;
    }
    hdAppDisposerDestroy(self, 0);
    if (flags & 1)
        hdAppDelete(self);
}

/* 02005DC4 */
void hdAppGraphicsStaticInit() {
    initStatics(&l_statics2, &l_range2, &l_lower2, &l_upper2, &l_registration2_1,
                &l_registration2_2, &l_registration2_3);
    l_layoutShader.mVTable = &l_shaderStringVTable;
    l_layoutShader.mText = gabi::at<void>(gabi::guest_addr("lyt_BuildinShader.gsh"));
    l_fontShader.mVTable = &l_shaderStringVTable;
    l_fontShader.mText = gabi::at<void>(gabi::guest_addr("font_BuildinShader.gsh"));
}

/* 02005E90 */
void hdAppGraphicsDeleteCompanion(void *self, s32 flags) {
    if (!self)
        return;
    if (!(flags & 1))
        return;
    hdAppDelete(self);
}

/* 02005EA4 */
void hdAppGraphicsEmptyCompanion() {}

static HdAppHeap *rootHeap() { return l_rootReady ? l_rootArray.mArray->mRoot.get() : NULL; }

/* 02005EA8 */
s32 hdAppMain(s32 argc, void *argv) {
    // HD: the frame-exact run call requires the original prologue, scratch bytes and saved
    // registers.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x118> frame;
    hdAppRuntimeInit();
    hdAppSystemInit();
    hdAppSystemConfigure();
    HdAppArena *arena = gabi::at<HdAppArena>(frame + 0x54);
    hdAppArenaQuery(arena);
    HdAppBaseHeap *baseHeap = hdAppBaseHeap(1);
    if (baseHeap->mSignature == HD_APP_EXP_HEAP) {
        hdAppExpHeapFree(baseHeap);
        hdAppExpHeapAvailable(baseHeap, 4);
    }
    arena->mSize = 0x3B7780C4;
    hdAppArenaConfigure(arena);
    hdAppRootInit();
    hdAppRootConfigure();
    HdAppFrameworkParams *params = gabi::at<HdAppFrameworkParams>(frame + 0x20);
    params->mWidth = HD_APP_INITIAL_WIDTH;
    params->mHeight = HD_APP_INITIAL_HEIGHT;
    u32 heap0 = l_heapSizes.mSizes[0];
    u32 heap1 = l_heapSizes.mSizes[1];
    params->mHeapSize0 = heap0;
    params->mFlag = 1;
    params->mHeapMode = HD_APP_FRAMEWORK_SINGLE_HEAP;
    params->mCommandBufferSize = 0x400000;
    u32 heap3 = l_heapSizes.mSizes[3];
    u32 heap2 = l_heapSizes.mSizes[2];
    params->mHeapSize1 = heap1;
    params->mHeapSize2 = heap2;
    params->mHeapSize3 = heap3;
    params->mWidth = hdAppWidth();
    params->mHeight = hdAppHeight();
    params->mFlag = 0;
    u32 rootReady = l_rootReady;
    params->mName.mText = gabi::at<void>(gabi::guest_addr("FrameworkCafe"));
    params->mHeapMode = HD_APP_FRAMEWORK_MULTI_HEAP;
    params->mName.mVTable = &l_appStringVTable;
    HdAppHeap *parent = rootReady ? l_rootArray.mArray->mRoot.get() : NULL;
    HdAppHeap *heap = hdAppCreateHeap(0, &params->mName, parent, 1, 0);
    HdAppFramework *framework = static_cast<HdAppFramework *>(hdAppHeapNew(0x390, heap, 4));
    if (framework)
        framework = hdAppFrameworkConstruct(framework, params);
    gabi::call_ptr<void>(heap->mVTable[11].get(), heap);
    hdAppGraphicsSystemCreate(&l_graphicsSystem, rootHeap(), 0x64, 0x2800000, 0);
    HdAppSafeString *graphicsName = gabi::at<HdAppSafeString>(frame + 0x5C);
    rootReady = l_rootReady;
    graphicsName->mVTable = &l_appStringVTable;
    graphicsName->mText = gabi::at<void>(gabi::guest_addr("GraphicsSystem"));
    parent = rootReady ? l_rootArray.mArray->mRoot.get() : NULL;
    HdAppHeap *graphicsHeap = hdAppCreateHeap(0, graphicsName, parent, 1, 0);
    HdAppDimensions *size = gabi::at<HdAppDimensions>(frame + 0x10);
    HdAppDimensions *displaySize = gabi::at<HdAppDimensions>(frame + 0x18);
    u32 width = params->mWidth;
    u32 height = params->mHeight;
    size->mWidth = f32(f64(width));
    size->mHeight = f32(f64(height));
    f32 displayWidth = f32(f64(hdAppDisplayWidth()));
    u32 displayHeightWord = hdAppDisplayHeight();
    f32 displayHeight = f32(f64(displayHeightWord));
    HdAppConversion *conversion = gabi::at<HdAppConversion>(frame + 8);
    if (gabi::native_sp()) {
        conversion->mMagic = 0x43300000;
        conversion->mValue = displayHeightWord;
    }
    displaySize->mWidth = displayWidth;
    displaySize->mHeight = displayHeight;
    hdAppFrameworkSetup(framework, graphicsHeap, argc, argv, size, displaySize,
                        l_graphicsSystem.mSystem);
    gabi::call_ptr<void>(graphicsHeap->mVTable[11].get(), graphicsHeap);
    HdAppHeap *root = rootHeap();
    gabi::call_ptr<void>(root->mVTable[31].get(), root, 4);
    HdAppTaskFactory *factory = gabi::at<HdAppTaskFactory>(frame + 0x4C);
    HdAppTaskArgs *taskArgs = gabi::at<HdAppTaskArgs>(frame + 0x6C);
    factory->mKind = 2;
    factory->mEntry = &l_rootTaskEntry;
    hdAppTaskArgsConstruct(taskArgs, factory);
    u32 run = framework->mVTable[9].get();
    HdAppRunArg *runArg = gabi::at<HdAppRunArg>(frame + 0x64);
    void *argument = hdAppRunArgConstruct(runArg);
    // HD: the save-state boundary is the original bctrl return site; registers mirror that site.
    gabi::call_ptr_site(0x020061C4,
                        {{25, gabi::ea(graphicsHeap)},
                         {26, gabi::ea(&l_rootReady)},
                         {27, gabi::ea(&l_memoryBase)},
                         {28, 0x43300000u},
                         {29, run},
                         {30, gabi::ea(root)},
                         {31, gabi::ea(framework)}},
                        {{31, (f64)displayWidth, (f64)displayWidth}}, run, framework, root,
                        taskArgs, argument);
    return 0;
}

/* 020061E8 */
void hdAppEntryStaticInit() {
    initStatics(&l_statics3, &l_range3, &l_lower3, &l_upper3, &l_registration3_1,
                &l_registration3_2, &l_registration3_3);
}

/* 0200627C */
void hdAppEntryDeleteCompanion(void *self, s32 flags) {
    if (!self)
        return;
    if (!(flags & 1))
        return;
    hdAppDelete(self);
}

/* 02006290 */
void hdAppEntryEmptyCompanion() {}
/* WWHD glue (tools/verify/autoglue.py)
data l_childStringVTable 10000980
data l_shaderStringVTable 100009A0
data l_appStringVTable 10000A34
data l_graphicsVTable 10000A20
data l_disposerVTable 10000A10
data l_emptyName 10000160
data l_graphics 1018C404
data l_disposer 1018C408
data l_thread 101F8B18
data l_fontShader 101FF2C8
data l_layoutShader 101FF2D0
data l_rootReady 104A030C
data l_rootArray 104A0314
data l_graphicsSystem 104A0380
data l_heapSizes 104A01DC
data l_rootTaskEntry 02006294
data l_memoryBase 104A0000
data l_statics1 101FF2B8
data l_range1 101FF2AC
data l_lower1 10000998
data l_upper1 1000099C
data l_registration1_1 1018C3BC
data l_registration1_2 1018C3C8
data l_registration1_3 1018C3D4
data l_statics2 101FF2E4
data l_range2 101FF2D8
data l_lower2 100009B8
data l_upper2 100009BC
data l_registration2_1 1018C3E0
data l_registration2_2 1018C3EC
data l_registration2_3 1018C3F8
data l_statics3 101FF300
data l_range3 101FF2F4
data l_lower3 10000A7C
data l_upper3 10000A80
data l_registration3_1 1018C40C
data l_registration3_2 1018C418
data l_registration3_3 1018C424
str 100009E8 "lyt_BuildinShader.gsh" renamed
str 100009C0 "font_BuildinShader.gsh" renamed
str 10000A58 "FrameworkCafe" renamed
str 10000A68 "GraphicsSystem" renamed
ext 0273AD10 void* hdAppAllocate(u32)
ext 0273AF40 void hdAppDelete(void*)
ext 0273B0D4 void* hdAppHeapAllocate(u32,HdAppHeap*,u32)
ext 0273B050 void* hdAppHeapNew(u32,HdAppHeap*,u32)
ext 02752B0C void hdAppDisposerConstruct(HdAppDisposer*,HdAppHeap*,u32)
ext 02752BEC void hdAppDisposerDestroy(HdAppDisposer*,u32)
ext 020039C0 void* hdAppAllocatorConstruct(void*,HdAppHeap*)
ext 02874890 void hdAppSetAllocator(void*)
ext 0274FBF8 void hdAppThreadEnter(void*)
ext 0274FCCC void hdAppThreadLeave(void*)
ext 0287EB18 void* hdAppResourceConstruct(void*)
ext 0287ECB4 void hdAppResourceSetup(void*,u32,void*)
ext 0287D924 void* hdAppFontConstruct(void*)
ext 0287E4C4 void hdAppFontSetup(void*,void*,void*)
ext 0286DED4 void* hdAppDrawInfoConstruct(void*)
ext 028EA09C void hdAppRuntimeInit()
ext 02034380 void hdAppSystemInit()
ext 02034884 void hdAppSystemConfigure()
ext 027478C4 void hdAppArenaQuery(HdAppArena*)
ext C0009790 HdAppBaseHeap* hdAppBaseHeap(u32)
ext C00097D8 u32 hdAppExpHeapFree(HdAppBaseHeap*)
ext C0009778 u32 hdAppExpHeapAvailable(HdAppBaseHeap*,u32)
ext 0274B1F0 void hdAppArenaConfigure(HdAppArena*)
ext 0203E584 void hdAppRootInit()
ext 0203E6A4 void hdAppRootConfigure()
ext 02739398 u32 hdAppWidth()
ext 027393E8 u32 hdAppHeight()
ext 027394D8 u32 hdAppDisplayWidth()
ext 02739528 u32 hdAppDisplayHeight()
ext 02753004 HdAppHeap* hdAppCreateHeap(u32,HdAppSafeString*,HdAppHeap*,u32,u32)
ext 02034C10 HdAppFramework* hdAppFrameworkConstruct(HdAppFramework*,HdAppFrameworkParams*)
ext 02756B94 void hdAppGraphicsSystemCreate(HdAppGraphicsSystem*,HdAppHeap*,u32,u32,u32)
ext 0274B40C void hdAppFrameworkSetup(HdAppFramework*,HdAppHeap*,s32,void*,HdAppDimensions*,HdAppDimensions*,void*)
ext 02748B98 void hdAppTaskArgsConstruct(HdAppTaskArgs*,HdAppTaskFactory*)
ext 02747908 void* hdAppRunArgConstruct(HdAppRunArg*)
ext 028F026C void hdAppRegister(void*)
ext 028ED6F8 void hdAppInitSigned(be<u8>*)
ext 028EAB2C void hdAppInitUnsigned(be<u8>*)
*/
