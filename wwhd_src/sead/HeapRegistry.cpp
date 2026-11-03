// WWHD heap registration helpers, reconstructed from the game instructions.
// SDK boundary facts: wut GX2 declarations (argument order and enum values only).
// Mutex extent uses wut coreinit declarations/offsets only; no SDK implementation.
#include "sead/HeapRegistry.h"
#include "sead/ElementAt.h"

static sead::RegistryMatrix44 sDefaultModel;
static u32 sTexturedPrimitiveColor[4] = {};
static u32 sPlainPrimitiveColor[4] = {};
static u32 sFreeHeapObjectTable;
static u32 sFrontierHeapObjectTable;
static u32 sFreeHeapInterfaceTable;
static u32 sFrontierHeapInterfaceTable;
static u32 sFreeHeapTypeGuard;
static u32 sFrontierHeapTypeGuard;
static sead::RuntimeType sFreeHeapType;
static sead::RuntimeType sFrontierHeapType;
static u32 sFreeHeapTypeTable;
static u32 sFrontierHeapTypeTable;
static u32 sFreeHeapParentTypeTable;
static u32 sFrontierHeapParentTypeTable;
static u32 sRegistryHeapTypeTable;
static u32 sRegistryManagerTable;
static u32 sRootNameTable;
static sead::RegistryManagerState sRegistryState;
static sead::RegistryArena_l sStaticRegistryArena;
static sead::RegistryArena_l* sRegistryArena;
static sead::RegistryMutex_l sRegistryMutex;
static sead::RegistryHeapBuffer sRootHeapBuffer;
static sead::RegistryHeapBuffer sIndependentHeapBuffer;
static u32 sArenaLifetime;
static u32 sMutexLifetime;
static u32 sHeapTextTable;
static u32 sHeapInterfaceTable;
static u32 sRegistryHeapTable;
static u32 sRegistrationTable;
static sead::RegistryHeap* sRootRegistryHeap;
static heap_adapters::HeapManager* sHeapModuleManager;
static u32 sResourcePathTable;

/* 02754908 */
void sead::RegistryText::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 0275491C */
void sead::RegistryText::terminateEmpty() {
    
}

/* 02754920 */
void sead::RegistryText::terminateBuffer() {
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 02754938 */
void sead::RegistryText::destroyBuffer(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 027549D4 */
void sead::RegistryText::destroyBase(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 027549E8 */
void sead::RegistryText::destroyBufferBase(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755600 */
void sead::RegionText::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755614 */
void sead::RegionText::terminateEmpty() {
    
}

/* 02755618 */
void sead::RegionText::terminateBuffer() {
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 02755630 */
void sead::RegionText::destroyBuffer(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 027556D4 */
void sead::RegionText::destroyBase(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 027556E8 */
void sead::RegionText::destroyBufferBase(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755DA8 */
void sead::HeapText::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755DBC */
void sead::HeapText::terminateEmpty() {
    
}

/* 02755DC0 */
void sead::HeapText::terminateBuffer() {
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity - 1] = 0;
}

/* 02755DD8 */
void sead::HeapText::destroyBuffer(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02754564 */
u32 sead::FreeRegionHeap::regionStart() const { return mRegionStart; }

/* 02754618 */
u32 sead::FreeRegionHeap::regionSize() const { return mRegionSize; }

/* 02754A8C */
u32 sead::DirectionalHeap::usableStart() const {
    s32 direction = mDirection;
    u32 start = mRegionStart;
    if (direction == 1) start += 0x9C;
    return start;
}

/* 02754AA4 */
u32 sead::DirectionalHeap::usableEnd() const {
    s32 direction = mDirection;
    u32 start = mRegionStart;
    u32 size = mRegionSize;
    if (direction != 1) size -= 0x9C;
    return start + size;
}

/* 02755280 */
u32 sead::FrontierHeap::regionStart() const { return mRegionStart; }

/* 02755288 */
u32 sead::FrontierHeap::regionEnd() const {
    u32 start = mRegionStart;
    u32 size = mRegionSize;
    return start + size;
}

/* 02755298 */
u32 sead::FrontierHeap::regionSize() const { return mRegionSize; }

/* 027532BC */
static s32 compareHeapAddresses(u32 left, u32 right) {
    // The original tests the sign of the wrapped subtraction.
    return (s32)(left - right) >= 0 ? 1 : -1;
}

/* 027552A0 */
u32 sead::FrontierHeap::freeSize() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 back = mBack;
    u32 front = mFront;
    u32 size = back - front;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 02755BA4 */
void sead::RegistryHeap::removeChild(void* child) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mChildren.mOffset;
    Link* link = linkAt(child,offset);
    link->detach();
    mChildren.mCount = (u32)mChildren.mCount - 1;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02755CA4 */
void sead::RegistryHeap::appendDisposable(void* object) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mDisposables.mOffset;
    Link* link = linkAt(object,offset);
    mDisposables.insertBefore(link);
    mDisposables.mCount = (u32)mDisposables.mCount + 1;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02755D28 */
void sead::RegistryHeap::removeDisposable(void* object) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mDisposables.mOffset;
    Link* link = linkAt(object,offset);
    link->detach();
    mDisposables.mCount = (u32)mDisposables.mCount - 1;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02754AC4 */
void sead::FrontierHeap::resetCursors() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    mFront = usableStart();
    mBack = usableEnd();
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 027551FC */
void sead::FrontierHeap::releaseAll() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    disposeRange(0,0);
    mFront = usableStart();
    mBack = usableEnd();
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02755818 */
void sead::RegistryHeap::appendChild(void* child) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mChildren.mOffset;
    Link* link = linkAt(child,offset);
    mChildren.insertBefore(link);
    mChildren.mCount = (u32)mChildren.mCount + 1;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02752A84 */
static sead::RegisteredObject* constructRegisteredObject(sead::RegisteredObject* object) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (object == NULL) object = static_cast<sead::RegisteredObject*>(allocateRegistryStorage(16));
    if (object == NULL) return NULL;
    object->mHeap = NULL;
    object->mLink.mNext = NULL;
    object->mVtable = gabi::at<sead::DisposerVtable>(gabi::ea(&sRegistrationTable));
    object->mLink.mPrevious = NULL;
    void* manager = sRegistryManager;
    if (manager != NULL) {
        sead::RegistryHeap* heap = findRegistryHeap(manager,object);
        object->mHeap = heap;
        if (heap != NULL) heap->appendDisposable(object);
    }
    return object;
}

/* 02752B0C */
static sead::RegisteredObject* constructRegisteredObjectInHeap(sead::RegisteredObject* object,
        sead::RegistryHeap* heap,u32 selection) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (object == NULL) object = static_cast<sead::RegisteredObject*>(allocateRegistryStorage(16));
    if (object == NULL) return NULL;
    object->mHeap = heap;
    object->mLink.mPrevious = NULL;
    object->mLink.mNext = NULL;
    object->mVtable = gabi::at<sead::DisposerVtable>(gabi::ea(&sRegistrationTable));
    if (heap == NULL) {
        if (selection <= 1) {
            void* manager = sRegistryManager;
            if (manager == NULL) return object;
            heap = findRegistryHeap(manager,object);
        } else if (selection == 3) {
            void* manager = sRegistryManager;
            if (manager == NULL) return object;
            heap = registryCurrentHeap(manager);
        } else {
            return object;
        }
        object->mHeap = heap;
        if (heap == NULL) return object;
    }
    heap->appendDisposable(object);
    return object;
}

/* 02752BEC */
void sead::RegisteredObject::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this == NULL) return;
    RegistryHeap* heap = static_cast<RegistryHeap*>(mHeap.get());
    mVtable = gabi::at<DisposerVtable>(gabi::ea(&sRegistrationTable));
    RegistryHeap* root = sRootRegistryHeap;
    if (heap != root) {
        if (heap != NULL) {
            heap->removeDisposable(this);
            root = sRootRegistryHeap;
        }
        mHeap = root;
    }
    if (flags & 1) releaseRegistryStorage(this);
}

/* 02752800 */
void sead::PrimitiveRenderer::setModel(const RegistryMatrix34* matrix) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    copyRegistryModel(matrix,&mModel);
}

/* 02752810 */
void sead::PrimitiveRenderer::setProjection(void* projection) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const RegistryMatrix44* matrix = updateRegistryProjection(projection);
    copyRegistryProjection(matrix,&mProjection);
}

/* 02752848 */
void sead::PrimitiveRenderer::prepare() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<RegistryMatrix44> transform(8);
    combineRegistryMatrices(transform,&mProjection,&mModel);
    void* vertexShader = mVertexShader;
    void* pixelShader = mPixelShader;
    selectRegistryFetchShader(mFetchShader);
    selectRegistryVertexShader(vertexShader);
    selectRegistryPixelShader(pixelShader);
    setRegistryVertexUniform(mProjectionRegister,16,transform);
    setRegistryVertexUniform(mModelRegister,16,&sDefaultModel);
    setRegistryLineWidth(2.0f);
}

/* 027528E0 */
void sead::PrimitiveRenderer::draw(const RegistryMatrix34* model,const void* topColor,
        const void* bottomColor,const void* vertices,u32 vertexCount,
        const void* indices,u32 indexCount,const void* texture) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    enum { VertexStride = 0x24, TriangleMode = 4, BigEndianIndex16 = 4 };
    setRegistryVertexUniform(mModelRegister,12,model);
    setRegistryVertexUniform(mTopColorRegister,4,topColor);
    setRegistryVertexUniform(mBottomColorRegister,4,bottomColor);
    u32 textureRegister = mTextureRegister;
    if (texture != NULL) {
        setRegistryPixelUniform(textureRegister,4,sTexturedPrimitiveColor);
        setRegistryPixelTexture(texture,mTextureSlot);
        setRegistryPixelSampler(mSampler,mTextureSlot);
    } else {
        setRegistryPixelUniform(textureRegister,4,sPlainPrimitiveColor);
    }
    setRegistryAttribute(0,vertexCount*VertexStride,VertexStride,vertices);
    drawRegistryIndices(TriangleMode,indexCount,BigEndianIndex16,indices,0,1);
}

/* 027529F8 */
void sead::PrimitiveRenderer::drawFilled(const RegistryMatrix34* model,const void* topColor,const void* bottomColor) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const void* vertices = mFilledVertices;
    const void* indices = mFilledIndices;
    draw(model,topColor,bottomColor,vertices,0x22,indices,0xC0,NULL);
}

/* 02752A30 */
void sead::PrimitiveRenderer::drawWire(const RegistryMatrix34* model,const void* topColor,const void* bottomColor) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const void* vertices = mWireVertices;
    const void* indices = mWireIndices;
    draw(model,topColor,bottomColor,vertices,0x22,indices,0xC0,NULL);
}

/* inlined into constructRegistryHeap */
static void initializeRegistryObjectList(sead::RegistryObjectList* list) {
    if (gabi::ea(list) == 0) list = static_cast<sead::RegistryObjectList*>(allocateRegistryStorage(16));
    if (list == NULL) return;
    list->mCount = 0;
    list->mPrevious = list;
    list->mOffset = 0xFFFFFFFF;
    list->mNext = list;
}

/* 0275589C */
static sead::RegistryHeap* constructRegistryHeap(sead::RegistryHeap* object,
        const sead::SafeText* name,sead::RegistryHeap* parent,u32 regionStart,
        u32 regionSize,s32 direction,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (object == NULL) object = static_cast<sead::RegistryHeap*>(allocateRegistryStorage(0x94));
    if (object == NULL) return NULL;
    constructRegisteredObjectInHeap(gabi::at<sead::RegisteredObject>(gabi::ea(object)),parent,1);
    object->mTextTable = gabi::ea(&sHeapTextTable);
    be<u8>* text = name->mText;
    object->mParent = parent;
    object->mRegionStart = regionStart;
    object->mHeapInterface = gabi::ea(&sHeapInterfaceTable);
    object->mName = text;
    object->mObject.mTable = gabi::ea(&sRegistryHeapTable);
    object->mRegionSize = regionSize;
    initializeRegistryObjectList(&object->mChildren);
    object->mParentLink.mPrevious = NULL;
    object->mParentLink.mNext = NULL;
    initializeRegistryObjectList(&object->mDisposables);
    object->mDirection = direction;
    void* lockStorage = &object->mLock;
    constructRegistryLock(lockStorage,parent);
    be<u32>* flags = &object->mFlags;
    if (gabi::ea(flags) == 0) flags = static_cast<be<u32>*>(allocateRegistryStorage(4));
    if (flags != NULL) *flags = 4;
    u32 state = object->mFlags;
    state = locking != 0 ? state | 1 : state & ~1u;
    object->mFlags = state;
    void* lock = NULL;
    if (state & 1) {
        lock = lockStorage;
        lockRegistryHeap(lock);
    }
    object->mChildren.mOffset = 0x38;
    object->mDisposables.mOffset = 4;
    if (parent != NULL) parent->appendChild(object);
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return object;
}

/* 02754754 */
u32 sead::FreeRegionHeap::contains(u32 address) const {
    s32 direction = mDirection;
    u32 start = mRegionStart;
    u32 size = mRegionSize;
    u32 begin;
    u32 end;
    if (direction == 1) {
        begin = start + 0xB8;
        end = start + size;
    } else {
        begin = start;
        end = start + (size - 0xB8);
    }
    return address >= begin && address < end ? 1 : 0;
}

/* 02755E7C */
static sead::RegistryManagerState* constructRegistryManager(sead::RegistryManagerState* manager) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (manager == NULL) manager = static_cast<sead::RegistryManagerState*>(allocateRegistryStorage(8));
    if (manager == NULL) return NULL;
    manager->mFailureReceiver = NULL;
    manager->mTable = gabi::ea(&sRegistryManagerTable);
    return manager;
}

/* 02755EC4 */
static void createInitialRegistryHeap() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sead::RegistryArena_l* arena = sRegistryArena;
    u32 size = arena->mSize;
    u32 start = arena->mStart;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sRootNameTable));
    name->mText = gabi::at<be<u8>>(gabi::guest_str("RootHeap"));
    sead::RegistryHeap* root = sead::FreeRegionHeap::createAt(start,size,name,0);
    s32 count = sRootHeapBuffer.mBuffer.mCount;
    s32 capacity = sRootHeapBuffer.mBuffer.mCapacity;
    if (count < capacity) {
        gptr<sead::RegistryHeap>* storage = static_cast<gptr<sead::RegistryHeap>*>(static_cast<void*>(sRootHeapBuffer.mBuffer.mStorage));
        sead::elementAt(storage, (u32)count) = root;
        sRootHeapBuffer.mBuffer.mCount = (u32)sRootHeapBuffer.mBuffer.mCount + 1;
    }
}

/* 02755F40 */
static void installRegistryManager() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sRegistryState.mFailureReceiver = NULL;
    createInitialRegistryHeap();
    sRegistryManager = &sRegistryState;
}

/* 02755F80 */
static void initializeRegistryArena(u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    sRegistryArena = &sStaticRegistryArena;
    configureRegistryArena(&sStaticRegistryArena,size);
    installRegistryManager();
    unlockRegistryHeap(&sRegistryMutex);
}

/* 02755FE0 */
static void installExternalRegistryArena(sead::RegistryArena_l* arena) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    sRegistryArena = arena;
    installRegistryManager();
}

/* 02752D8C */
void sead::FreeRegionHeap::coalesceFreeBlocks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 traversalOffset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - traversalOffset;
    RegistryBlock* block = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),traversalOffset);
    while (gabi::ea(block) != sentinel) {
        u32 payload = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
        Link* link = linkAt(block,traversalOffset);
        RegistryBlock* next = objectOf<RegistryBlock>(link->mNext.get(),traversalOffset);
        u32 size = block->mSize;
        if (gabi::ea(next) == sentinel) break;
        if (payload + size == gabi::ea(next)) {
            block->mSize = size + (u32)next->mPadding + (u32)next->mSize + sizeof(RegistryBlock);
            linkAt(next,(u32)mFreeBlocks.mOffset)->detach();
            sentinel = gabi::ea(&mFreeBlocks) - (u32)mFreeBlocks.mOffset;
            mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
        } else {
            block = next;
        }
    }
}

/* 02752E60 */
void sead::FreeRegionHeap::insertFreeBlock(RegistryBlock* block) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 offset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - offset;
    RegistryBlock* current = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),offset);
    Link* position = &mFreeBlocks;
    while (gabi::ea(current) != sentinel) {
        if (gabi::ea(current) > gabi::ea(block)) {
            position = linkAt(current,offset);
            break;
        }
        Link* link = linkAt(current,offset);
        current = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    position->insertBefore(linkAt(block,offset));
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount + 1;
    coalesceFreeBlocks();
}

/* 027532D0 */
sead::RegistryBlock* sead::FreeRegionHeap::findTrailingFreeBlock() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mUsedBlocks.sort(mUsedBlocks.mOffset,gabi::guest_fn((const void*)&compareHeapAddresses));
    u32 block = 0;
    if ((s32)mFreeBlocks.mCount > 0 && gabi::ea(mFreeBlocks.mPrevious.get()) != 0)
        block = gabi::ea(mFreeBlocks.mPrevious.get()) - (u32)mFreeBlocks.mOffset;
    u32 used = 0;
    if ((s32)mUsedBlocks.mCount > 0 && gabi::ea(mUsedBlocks.mPrevious.get()) != 0)
        used = gabi::ea(mUsedBlocks.mPrevious.get()) - (u32)mUsedBlocks.mOffset;
    return gabi::at<RegistryBlock>(block > used ? block : 0);
}

/* 0275341C */
sead::RegistryBlock* sead::FreeRegionHeap::findLeadingFreeBlock() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    mUsedBlocks.sort(mUsedBlocks.mOffset,gabi::guest_fn((const void*)&compareHeapAddresses));
    u32 block = 0;
    if ((s32)mFreeBlocks.mCount > 0 && gabi::ea(mFreeBlocks.mNext.get()) != 0)
        block = gabi::ea(mFreeBlocks.mNext.get()) - (u32)mFreeBlocks.mOffset;
    u32 used = 0;
    if ((s32)mUsedBlocks.mCount > 0 && gabi::ea(mUsedBlocks.mNext.get()) != 0)
        used = gabi::ea(mUsedBlocks.mNext.get()) - (u32)mUsedBlocks.mOffset;
    return gabi::at<RegistryBlock>(block < used ? block : 0);
}

/* 027551E8 */
void sead::FrontierHeap::release(void*) {}

/* 027551EC */
u32 sead::FrontierHeap::resizeFront(void*,u32) { return 0; }

/* 027551F4 */
u32 sead::FrontierHeap::resizeBack(void*,u32) { return 0; }

/* 02753378 */
u32 sead::FreeRegionHeap::trimTrailingFreeBlock() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryBlock* block = findTrailingFreeBlock();
    if (gabi::ea(block) == 0) return mRegionSize;
    u32 size = gabi::ea(block) - (u32)mRegionStart;
    linkAt(block,(u32)mFreeBlocks.mOffset)->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    RegistryHeap* parent = mParent;
    RegistryHeapDispatch* table = parent->dispatch();
    if (gabi::call_ptr<u32>(table->mResizeBack.mFunction,parent,gabi::at<void>(mRegionStart),size) == 0)
        return mRegionSize;
    mRegionSize = size;
    return size;
}

/* 027534C4 */
u32 sead::FreeRegionHeap::trimLeadingFreeBlock() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryBlock* block = findLeadingFreeBlock();
    if (gabi::ea(block) == 0) return mRegionSize;
    u32 size = (u32)mRegionSize - ((u32)block->mPadding + (u32)block->mSize + sizeof(RegistryBlock));
    linkAt(block,(u32)mFreeBlocks.mOffset)->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    RegistryHeap* parent = mParent;
    RegistryHeapDispatch* table = parent->dispatch();
    void* start = gabi::call_ptr<void*>(table->mResizeFront.mFunction,parent,gabi::at<void>(mRegionStart),size);
    if (start == NULL) return mRegionSize;
    mRegionSize = size;
    mRegionStart = gabi::ea(start);
    return size;
}

/* 0275357C */
u32 sead::FreeRegionHeap::trimToParent() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mParent.get() == NULL) return mRegionSize;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 size = (u32)mDirection == 1 ? trimTrailingFreeBlock() : trimLeadingFreeBlock();
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 027549C4 */
u32 sead::FreeRegionHeap::supportsRelease() const { return 1; }

/* 027556BC */
u32 sead::FrontierHeap::supportsRelease() const { return 0; }

/* 0275456C */
u32 sead::FreeRegionHeap::freeSize() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),offset);
    u32 size = 0;
    while (gabi::ea(block) != sentinel) {
        size += (u32)block->mSize;
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 02754620 */
u32 sead::FreeRegionHeap::maxAllocation(s32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 sign = alignment < 0 ? ~0u : 0;
    u32 magnitude = ((u32)alignment ^ sign) - sign;
    if (((magnitude - 1) & magnitude) != 0) return 0;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    s32 directedAlignment = (s32)((u32)alignment * (u32)mDirection);
    RegistryBlock* block;
    if (directedAlignment < 0) {
        s32 reverseAlignment = (s32)(0u - (u32)directedAlignment);
        block = reverseAlignment <= 4 ? findFreeBlockBack(4,2)
            : findAlignedFreeBlockBack(4,(u32)reverseAlignment,2);
    } else {
        block = directedAlignment <= 4 ? findFreeBlock(4,2)
            : findAlignedFreeBlock(4,(u32)directedAlignment,2);
    }
    u32 size = gabi::ea(block) == 0 ? 0 : (u32)block->mSize;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 02754D40 */
u32 sead::FrontierHeap::trimFrontierEnd() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    u32 front = mFront;
    u32 start = gabi::call_ptr<u32>(table->mRegionStart.mFunction,this);
    u32 size = front - start;
    RegistryHeap* parent = mParent;
    table = parent->dispatch();
    if (gabi::call_ptr<u32>(table->mResizeBack.mFunction,parent,gabi::at<void>(mRegionStart),size) == 0)
        return mRegionSize;
    mRegionSize = size;
    mBack = mFront;
    return size;
}

/* 02754DD8 */
u32 sead::FrontierHeap::trimFrontierStart() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    u32 end = gabi::call_ptr<u32>(table->mRegionEnd.mFunction,this);
    u32 size = end - (u32)mBack;
    RegistryHeap* parent = mParent;
    table = parent->dispatch();
    void* start = gabi::call_ptr<void*>(table->mResizeFront.mFunction,parent,gabi::at<void>(mRegionStart),size);
    if (start == NULL) return mRegionSize;
    u32 back = mBack;
    mRegionSize = size;
    mRegionStart = back;
    mFront = back;
    return size;
}

/* 02754E74 */
u32 sead::FrontierHeap::trimToParent() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mParent.get() == NULL) return mRegionSize;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 size;
    if ((u32)mDirection == 1) {
        u32 end = usableEnd();
        size = (u32)mBack == end ? trimFrontierEnd() : (u32)mRegionSize;
    } else {
        u32 start = usableStart();
        size = (u32)mFront == start ? trimFrontierStart() : (u32)mRegionSize;
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 02754814 */
u32 sead::FreeRegionHeap::allocationSize(void* allocation) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    if (gabi::call_ptr<u32>(table->mContains.mFunction,this,allocation) == 0) return 0;
    u32 address = gabi::ea(allocation);
    u32 marker = *gabi::at<be<u32>>(address - 4);
    if (marker & 1) return *gabi::at<be<u32>>(marker + 7);
    return *gabi::at<be<u32>>(address - 8);
}

/* 027556FC */
sead::RegistryHeap* sead::RegistryHeap::findContaining(void* address) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    if (gabi::call_ptr<u32>(table->mContains.mFunction,this,address) == 0) return NULL;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 traversalOffset = mChildren.mOffset;
    u32 sentinel = gabi::ea(&mChildren) - traversalOffset;
    RegistryHeap* child = gabi::at<RegistryHeap>(gabi::ea(mChildren.mNext.get()) - traversalOffset);
    while (gabi::ea(child) != sentinel) {
        table = child->dispatch();
        if (gabi::call_ptr<u32>(table->mContains.mFunction,child,address) != 0) {
            RegistryHeap* found = child->findContaining(address);
            if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
            return found;
        }
        Link* link = linkAt(child,traversalOffset);
        child = gabi::at<RegistryHeap>(gabi::ea(link->mNext.get()) - traversalOffset);
        sentinel = gabi::ea(&mChildren) - (u32)mChildren.mOffset;
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return this;
}

/* 02755FEC */
sead::RegistryHeap* findRegistryHeap(void*,void* address) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    heap_adapters::HeapManager* manager = sHeapModuleManager;
    if (manager != NULL) {
        heap_adapters::ThreadHeapContext* context = getThreadSpecificHeapContext(manager->mThreadSpecificKey);
        if (context != NULL && (u32)context->mCurrentHeap != 0) {
            sead::RegistryHeap* heap = gabi::at<sead::RegistryHeap>(context->mCurrentHeap);
            sead::RegistryHeap* found = heap->findContaining(address);
            if (found != NULL) {
                unlockRegistryHeap(&sRegistryMutex);
                return found;
            }
        }
    }
    u32 begin = gabi::ea(sRootHeapBuffer.mBuffer.mStorage.get());
    u32 end = begin + ((u32)sRootHeapBuffer.mBuffer.mCount << 2);
    for (u32 slot = begin; slot != end; slot += sizeof(gptr<sead::RegistryHeap>)) {
        sead::RegistryHeap* heap = gabi::at<gptr<sead::RegistryHeap>>(slot)->get();
        sead::RegistryHeap* found = heap->findContaining(address);
        if (found != NULL) {
            unlockRegistryHeap(&sRegistryMutex);
            return found;
        }
    }
    begin = gabi::ea(sIndependentHeapBuffer.mBuffer.mStorage.get());
    end = begin + ((u32)sIndependentHeapBuffer.mBuffer.mCount << 2);
    for (u32 slot = begin; slot != end; slot += sizeof(gptr<sead::RegistryHeap>)) {
        sead::RegistryHeap* heap = gabi::at<gptr<sead::RegistryHeap>>(slot)->get();
        sead::RegistryHeap* found = heap->findContaining(address);
        if (found != NULL) {
            unlockRegistryHeap(&sRegistryMutex);
            return found;
        }
    }
    unlockRegistryHeap(&sRegistryMutex);
    return NULL;
}

/* 02753F84 */
void sead::FreeRegionHeap::walkUsedBlocks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mUsedBlocks.mOffset;
    u32 sentinel = gabi::ea(&mUsedBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mUsedBlocks.mNext.get(),offset);
    while (gabi::ea(block) != sentinel) {
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02754018 */
void sead::FreeRegionHeap::walkFreeBlocks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),offset);
    while (gabi::ea(block) != sentinel) {
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 027547A8 */
void sead::FreeRegionHeap::walkBlocks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    walkUsedBlocks();
    walkFreeBlocks();
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02752A68 */
void sead::ResourcePathText::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755E2C */
void sead::HeapFixedTextA::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755E54 */
void sead::HeapFixedTextB::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755E40 */
void sead::HeapFixedTextC::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02755E68 */
void sead::HeapFixedTextD::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this != NULL && (flags & 1)) releaseRegistryStorage(this);
}

/* 02752A7C */
void sead::ResourcePathText::terminateEmpty() {}

/* 02752A80 */
void sead::PrimitiveRenderer::finish() {}

/* 02755E28 */
void sead::RegistryHeap::inspectState() {}

// Purposes unresolved: slot traits retain only the established concrete values.
/* 027549CC */
u32 sead::FreeRegionHeap::queryRegionHeapTrait() const { return 1; }

/* 027556C4 */
u32 sead::FrontierHeap::queryRegionHeapTrait() const { return 0; }

/* 027556CC */
u32 sead::FrontierHeap::queryFrontierHeapTrait() const { return 1; }

/* 0275320C */
static u32 freeRegionSizeWithOverhead(u32 size) {
    return size + sizeof(sead::FreeRegionHeap) + sizeof(sead::RegistryBlock);
}

/* 02754C90 */
static u32 frontierSizeWithOverhead(u32 size) {
    return size + sizeof(sead::FrontierHeap);
}

/* 027540AC */
void sead::FreeRegionHeap::release(void* allocation) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (allocation == NULL) return;
    RegistryHeapDispatch* table = dispatch();
    if (gabi::call_ptr<u32>(table->mContains.mFunction,this,allocation) == 0) return;
    u32 flags = mFlags;
    if (flags & 2) return;
    void* lock = NULL;
    if (flags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 address = gabi::ea(allocation);
    u32 marker = *gabi::at<be<u32>>(address - 4);
    u32 blockAddress = marker & 1 ? marker - 1 : address - sizeof(RegistryBlock);
    if (blockAddress != 0) {
        RegistryBlock* block = gabi::at<RegistryBlock>(blockAddress);
        gabi::at<Link>(blockAddress + (u32)mUsedBlocks.mOffset)->detach();
        mUsedBlocks.mCount = (u32)mUsedBlocks.mCount - 1;
        u32 size = (u32)block->mSize + (u32)block->mPadding;
        block->mPadding = 0;
        block->mSize = size;
        insertFreeBlock(block);
    } else {
        walkUsedBlocks();
        walkFreeBlocks();
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 027544E0 */
void sead::FreeRegionHeap::releaseAll() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    disposeRange(0,0);
    mUsedBlocks.clear();
    mFreeBlocks.clear();
    seedFreeBlocks();
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02753628 */
sead::RegistryBlock* sead::FreeRegionHeap::findFreeBlock(u32 size,u32 mode) {
    u32 offset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),offset);
    RegistryBlock* best = NULL;
    while (gabi::ea(block) != sentinel) {
        u32 blockSize = block->mSize;
        if (blockSize >= size) {
            if (mode == 0) return block;
            if (best == NULL) best = block;
            else if (mode == 1 && (u32)best->mSize > blockSize) best = block;
            else if (mode == 2 && (u32)best->mSize < blockSize) best = block;
        }
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    return best;
}

/* 027536B0 */
void sead::FreeRegionHeap::appendUsedBlock(RegistryBlock* block) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 offset = mUsedBlocks.mOffset;
    Link* link = linkAt(block,offset);
    mUsedBlocks.insertBefore(link);
    mUsedBlocks.mCount = (u32)mUsedBlocks.mCount + 1;
}

/* 027537B8 */
sead::RegistryBlock* sead::FreeRegionHeap::findAlignedFreeBlock(u32 size,u32 alignment,u32 mode) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 offset = mFreeBlocks.mOffset;
    u32 sentinel = gabi::ea(&mFreeBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mFreeBlocks.mNext.get(),offset);
    RegistryBlock* best = NULL;
    while (gabi::ea(block) != sentinel) {
        u32 blockSize = block->mSize;
        if (blockSize >= size) {
            u32 padding = block->mPadding;
            u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
            u32 aligned = (payload + alignment - 1) & ~(alignment - 1);
            u32 prefix = aligned - payload;
            if (blockSize >= size + prefix) {
                if (mode == 0) return block;
                if (best == NULL) best = block;
                else if (mode == 1 && (u32)best->mSize > blockSize) best = block;
                else if (mode == 2 && (u32)best->mSize < blockSize) best = block;
            }
        }
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    return best;
}

/* 027561B4 */
static void initializeHeapRegistryStatics() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    constructRegistryManager(&sRegistryState);
    constructStaticRegistryArena(&sStaticRegistryArena);
    registerRegistryLifetime(&sArenaLifetime);
    sRootHeapBuffer.mBuffer.mCapacity = 0;
    sRootHeapBuffer.mBuffer.mStorage = NULL;
    sRootHeapBuffer.mBuffer.mCount = 0;
    sRootHeapBuffer.mBuffer.setStorage(4,sRootHeapBuffer.mInlineStorage);
    sIndependentHeapBuffer.mBuffer.mCount = 0;
    sIndependentHeapBuffer.mBuffer.mCapacity = 0;
    sIndependentHeapBuffer.mBuffer.mStorage = NULL;
    sIndependentHeapBuffer.mBuffer.setStorage(4,sIndependentHeapBuffer.mInlineStorage);
    constructStaticRegistryMutex(&sRegistryMutex);
    registerRegistryLifetime(&sMutexLifetime);
}

/* 0275494C */
u32 sead::FreeRegionHeap::matchesType(const RuntimeType* type) const {
    if (sFreeHeapTypeGuard == 0) {
        sFreeHeapTypeGuard = 1;
        sFreeHeapType.mTable = &sFreeHeapTypeTable;
    }
    if (type == &sFreeHeapType) return 1;
    if (sRegistryHeapTypeGuard == 0) {
        sRegistryHeapTypeGuard = 1;
        sRegistryHeapType.mTable = &sFreeHeapParentTypeTable;
    }
    return type == &sRegistryHeapType ? 1 : 0;
}

/* 02755644 */
u32 sead::FrontierHeap::matchesType(const RuntimeType* type) const {
    if (sFrontierHeapTypeGuard == 0) {
        sFrontierHeapTypeGuard = 1;
        sFrontierHeapType.mTable = &sFrontierHeapTypeTable;
    }
    if (type == &sFrontierHeapType) return 1;
    if (sRegistryHeapTypeGuard == 0) {
        sRegistryHeapTypeGuard = 1;
        sRegistryHeapType.mTable = &sFrontierHeapParentTypeTable;
    }
    return type == &sRegistryHeapType ? 1 : 0;
}

/* 02755DEC */
u32 sead::RegistryHeap::matchesType(const RuntimeType* type) const {
    if (sRegistryHeapTypeGuard == 0) {
        sRegistryHeapTypeGuard = 1;
        sRegistryHeapType.mTable = &sRegistryHeapTypeTable;
    }
    return type == &sRegistryHeapType ? 1 : 0;
}

/* 02752C74 */
static sead::FreeRegionHeap* constructFreeRegionHeap(sead::FreeRegionHeap* heap,
        const sead::SafeText* name,sead::RegistryHeap* parent,u32 regionStart,
        u32 regionSize,s32 direction,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (heap == NULL) heap = static_cast<sead::FreeRegionHeap*>(allocateRegistryStorage(sizeof(sead::FreeRegionHeap)));
    if (heap == NULL) return NULL;
    constructRegistryHeap(heap,name,parent,regionStart,regionSize,direction,locking);
    heap->mSearchMode = 0;
    heap->mObject.mTable = gabi::ea(&sFreeHeapObjectTable);
    initializeRegistryObjectList(&heap->mFreeBlocks);
    initializeRegistryObjectList(&heap->mUsedBlocks);
    void* lock = NULL;
    if (heap->mFlags & 1) {
        lock = &heap->mLock;
        lockRegistryHeap(lock);
    }
    heap->mFreeBlocks.mOffset = 0;
    heap->mUsedBlocks.mOffset = 0;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return heap;
}

/* 027549FC */
static sead::FrontierHeap* constructFrontierHeap(sead::FrontierHeap* heap,
        const sead::SafeText* name,sead::RegistryHeap* parent,u32 regionStart,
        u32 regionSize,s32 direction,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (heap == NULL) heap = static_cast<sead::FrontierHeap*>(allocateRegistryStorage(sizeof(sead::FrontierHeap)));
    if (heap == NULL) return NULL;
    constructRegistryHeap(heap,name,parent,regionStart,regionSize,direction,locking);
    heap->mObject.mTable = gabi::ea(&sFrontierHeapObjectTable);
    clearRegistryMemory(&heap->mFront,8);
    return heap;
}

/* 02755A48 */
void sead::RegistryHeap::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this == NULL) return;
    mHeapInterface = gabi::ea(&sHeapInterfaceTable);
    destroyRegistryLock(&mLock,2);
    gabi::at<RegisteredObject>(gabi::ea(this))->destroy(0);
    if (flags & 1) releaseRegistryStorage(this);
}

/* 02754894 */
void sead::FreeRegionHeap::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this == NULL) return;
    mObject.mTable = gabi::ea(&sFreeHeapObjectTable);
    mHeapInterface = gabi::ea(&sFreeHeapInterfaceTable);
    detachAndDispose();
    RegistryHeap::destroy(0);
    if (flags & 1) releaseRegistryStorage(this);
}

/* 0275558C */
void sead::FrontierHeap::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (this == NULL) return;
    mObject.mTable = gabi::ea(&sFrontierHeapObjectTable);
    mHeapInterface = gabi::ea(&sFrontierHeapInterfaceTable);
    detachAndDispose();
    RegistryHeap::destroy(0);
    if (flags & 1) releaseRegistryStorage(this);
}

/* 027553E0 */
u32 sead::FrontierHeap::contains(u32 address) const {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 start = usableStart();
    u32 end = usableEnd();
    return address >= start && address < end ? 1 : 0;
}

/* 0275542C */
void sead::FrontierHeap::synchronize() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mFlags & 1) {
        void* lock = &mLock;
        lockRegistryHeap(lock);
        if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    }
}

/* 02755C24 */
void sead::RegistryHeap::detachAndDispose() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    disposeRange(0,0);
    RegistryHeap* parent = mParent;
    if (parent != NULL) parent->removeChild(this);
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02753B48 */
sead::RegistryBlock* sead::FreeRegionHeap::findAlignedFreeBlockBack(u32 size,u32 alignment,u32 mode) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryObjectList* list = &mFreeBlocks;
    if ((s32)list->mCount <= 0) return NULL;
    Link* previous = list->mPrevious;
    if (previous == NULL) return NULL;
    u32 offset = list->mOffset;
    RegistryBlock* block = objectOf<RegistryBlock>(previous,offset);
    RegistryBlock* best = NULL;
    u32 mask = ~(alignment - 1);
    while (block != NULL) {
        u32 blockSize = block->mSize;
        if (blockSize >= size) {
            u32 padding = block->mPadding;
            u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
            u32 unaligned = payload + (blockSize - size);
            u32 aligned = unaligned & mask;
            u32 suffix = unaligned - aligned;
            if (blockSize >= size + suffix) {
                if (mode == 0) return block;
                if (best == NULL) best = block;
                else if (mode == 1 && (u32)best->mSize > blockSize) best = block;
                else if (mode == 2 && (u32)best->mSize < blockSize) best = block;
            }
        }
        Link* link = linkAt(block,offset);
        Link* before = link->mPrevious;
        block = gabi::ea(before) == gabi::ea(list) ? NULL : objectOf<RegistryBlock>(before,offset);
    }
    return best;
}

/* 027536F4 */
sead::RegistryBlock* sead::FreeRegionHeap::allocateBlockFront(u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 mode = mSearchMode;
    RegistryBlock* block = findFreeBlock(size,mode);
    if (block == NULL) return NULL;
    u32 padding = block->mPadding;
    u32 oldSize = block->mSize;
    u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
    block->mSize = size;
    u32 offset = mFreeBlocks.mOffset;
    Link* link = linkAt(block,offset);
    u32 remaining = oldSize - size;
    link->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    appendUsedBlock(block);
    if (remaining > sizeof(RegistryBlock)) {
        RegistryBlock* tail = gabi::at<RegistryBlock>(payload + size);
        if (tail != NULL) {
            tail->mPadding = 0;
            tail->mLink.mPrevious = NULL;
            tail->mSize = 0;
            tail->mLink.mNext = NULL;
        }
        tail->mSize = remaining - sizeof(RegistryBlock);
        insertFreeBlock(tail);
    } else if (remaining != 0) {
        block->mSize = size + remaining;
    }
    return block;
}

/* 02753880 */
sead::RegistryBlock* sead::FreeRegionHeap::allocateAlignedBlockFront(u32 size,u32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 mode = mSearchMode;
    RegistryBlock* block = findAlignedFreeBlock(size,alignment,mode);
    if (block == NULL) return NULL;
    u32 padding = block->mPadding;
    u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
    u32 aligned = (payload + alignment - 1) & ~(alignment - 1);
    u32 prefix = aligned - payload;
    u32 oldSize = block->mSize;
    u32 occupied = size + prefix;
    block->mPadding = prefix;
    u32 remaining = oldSize - occupied;
    if (prefix != 0) {
        RegistryBlock* marker = gabi::at<RegistryBlock>(gabi::ea(block) + prefix);
        marker->mPadding = gabi::ea(block) + 1;
    }
    block->mSize = size;
    u32 offset = mFreeBlocks.mOffset;
    linkAt(block,offset)->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    appendUsedBlock(block);
    if (remaining > sizeof(RegistryBlock)) {
        u32 currentPadding = block->mPadding;
        u32 currentPayload = gabi::ea(block) + currentPadding + sizeof(RegistryBlock);
        RegistryBlock* tail = gabi::at<RegistryBlock>(currentPayload + size);
        if (tail != NULL) {
            tail->mPadding = 0;
            tail->mLink.mPrevious = NULL;
            tail->mSize = 0;
            tail->mLink.mNext = NULL;
        }
        tail->mSize = remaining - sizeof(RegistryBlock);
        insertFreeBlock(tail);
    } else if (remaining != 0) {
        block->mSize = size + remaining;
    }
    return block;
}

/* 02753998 */
sead::RegistryBlock* sead::FreeRegionHeap::findFreeBlockBack(u32 size,u32 mode) {
    RegistryObjectList* list = &mFreeBlocks;
    if ((s32)list->mCount <= 0) return NULL;
    Link* previous = list->mPrevious;
    if (previous == NULL) return NULL;
    u32 offset = list->mOffset;
    RegistryBlock* block = objectOf<RegistryBlock>(previous,offset);
    RegistryBlock* best = NULL;
    while (block != NULL) {
        u32 blockSize = block->mSize;
        if (blockSize >= size) {
            if (mode == 0) return block;
            if (best == NULL) best = block;
            else if (mode == 1 && (u32)best->mSize > blockSize) best = block;
            else if (mode == 2 && (u32)best->mSize < blockSize) best = block;
        }
        Link* link = linkAt(block,offset);
        Link* before = link->mPrevious;
        block = gabi::ea(before) == gabi::ea(list) ? NULL : objectOf<RegistryBlock>(before,offset);
    }
    return best;
}

/* 02753A34 */
sead::RegistryBlock* sead::FreeRegionHeap::allocateBlockBack(u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 mode = mSearchMode;
    RegistryBlock* block = findFreeBlockBack(size,mode);
    if (block == NULL) return NULL;
    u32 remaining = (u32)block->mSize - size;
    if (remaining > sizeof(RegistryBlock)) {
        u32 padding = block->mPadding;
        u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
        u32 freeSize = remaining - sizeof(RegistryBlock);
        RegistryBlock* result = gabi::at<RegistryBlock>(payload + freeSize);
        block->mSize = freeSize;
        if (result != NULL) {
            result->mPadding = 0;
            result->mLink.mPrevious = NULL;
            result->mSize = 0;
            result->mLink.mNext = NULL;
        }
        result->mSize = size;
        appendUsedBlock(result);
        return result;
    }
    u32 offset = mFreeBlocks.mOffset;
    linkAt(block,offset)->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    appendUsedBlock(block);
    return block;
}

/* 02753C28 */
sead::RegistryBlock* sead::FreeRegionHeap::allocateAlignedBlockBack(u32 size,u32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 mode = mSearchMode;
    RegistryBlock* block = findAlignedFreeBlockBack(size,alignment,mode);
    if (block == NULL) return NULL;
    u32 padding = block->mPadding;
    u32 oldSize = block->mSize;
    u32 payload = gabi::ea(block) + padding + sizeof(RegistryBlock);
    u32 unaligned = payload + (oldSize - size);
    u32 aligned = unaligned & ~(alignment - 1);
    u32 suffix = unaligned - aligned;
    u32 occupied = size + suffix;
    u32 remaining = oldSize - occupied;
    if (remaining > sizeof(RegistryBlock)) {
        u32 currentPadding = block->mPadding;
        u32 currentPayload = gabi::ea(block) + currentPadding + sizeof(RegistryBlock);
        u32 freeSize = remaining - sizeof(RegistryBlock);
        RegistryBlock* result = gabi::at<RegistryBlock>(currentPayload + freeSize);
        block->mSize = freeSize;
        if (result != NULL) {
            result->mPadding = 0;
            result->mLink.mPrevious = NULL;
            result->mSize = 0;
            result->mLink.mNext = NULL;
        }
        result->mSize = occupied;
        appendUsedBlock(result);
        return result;
    }
    block->mPadding = remaining;
    if (remaining != 0) {
        RegistryBlock* marker = gabi::at<RegistryBlock>(gabi::ea(block) + remaining);
        marker->mPadding = gabi::ea(block) + 1;
    }
    block->mSize = occupied;
    u32 offset = mFreeBlocks.mOffset;
    linkAt(block,offset)->detach();
    mFreeBlocks.mCount = (u32)mFreeBlocks.mCount - 1;
    appendUsedBlock(block);
    return block;
}

/* 02752F10 */
void sead::FreeRegionHeap::seedFreeBlocks() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (mFlags & 1) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 address;
    if (mDirection == 1) address = gabi::ea(this) + sizeof(FreeRegionHeap);
    else address = mRegionStart;
    RegistryBlock* block = gabi::at<RegistryBlock>(address);
    if (block != NULL) {
        block->mLink.mPrevious = NULL;
        block->mSize = 0;
        block->mPadding = 0;
        block->mLink.mNext = NULL;
    }
    u32 regionSize = mRegionSize;
    block->mSize = regionSize - (sizeof(FreeRegionHeap) + sizeof(RegistryBlock));
    insertFreeBlock(block);
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02753000 */
static void initializeFreeHeap(sead::FreeRegionHeap* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    heap->seedFreeBlocks();
}

/* 0275315C */
sead::RegistryHeap* sead::FreeRegionHeap::createAt(u32 start,u32 size,const SafeText* name,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    u32 roundedSize = (size + 3) & ~3u;
    if (roundedSize <= sizeof(sead::FreeRegionHeap) + sizeof(sead::RegistryBlock)) {
        unlockRegistryHeap(&sRegistryMutex);
        return NULL;
    }
    sead::FreeRegionHeap* heap = gabi::at<sead::FreeRegionHeap>(start);
    if (heap != NULL) heap = constructFreeRegionHeap(heap,name,NULL,start,roundedSize,1,locking);
    initializeFreeHeap(heap);
    unlockRegistryHeap(&sRegistryMutex);
    return heap;
}

/* 02753004 */
sead::RegistryHeap* sead::FreeRegionHeap::create(u32 size,const SafeText* name,
        RegistryHeap* parent,s32 direction,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    if (parent == NULL) {
        parent = registryCurrentHeap(sRegistryManager);
        if (parent == NULL) {
            unlockRegistryHeap(&sRegistryMutex);
            return NULL;
        }
    }
    if (size == 0) {
        sead::RegistryHeapDispatch* table = parent->dispatch();
        u32 query = table->mMaxAllocation.mFunction;
        size = gabi::call_ptr<u32>(query,parent,4);
    }
    u32 roundedSize = (size + 3) & ~3u;
    if (roundedSize <= sizeof(sead::FreeRegionHeap) + sizeof(sead::RegistryBlock)) {
        unlockRegistryHeap(&sRegistryMutex);
        return NULL;
    }
    sead::RegistryHeapDispatch* table = parent->dispatch();
    u32 allocate = table->mAllocate.mFunction;
    void* allocation = gabi::call_ptr<void*>(allocate,parent,roundedSize,direction * 4);
    if (allocation == NULL) {
        unlockRegistryHeap(&sRegistryMutex);
        return NULL;
    }
    u32 start = gabi::ea(allocation);
    u32 objectAddress = direction == 1 ? start : start + roundedSize - sizeof(sead::FreeRegionHeap);
    sead::FreeRegionHeap* heap = gabi::at<sead::FreeRegionHeap>(objectAddress);
    if (heap != NULL) heap = constructFreeRegionHeap(heap,name,parent,start,roundedSize,direction,locking);
    initializeFreeHeap(heap);
    unlockRegistryHeap(&sRegistryMutex);
    return heap;
}

/* 02753214 */
void sead::FreeRegionHeap::destroyHeap() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    RegistryLifetimeDispatch* lifetime = gabi::at<RegistryLifetimeDispatch>(mObject.mTable);
    u32 destroy = lifetime->mDestroy.mFunction;
    u32 start = mRegionStart;
    RegistryHeap* parent = mParent;
    gabi::call_ptr<void>(destroy,this,2);
    if (parent != NULL) {
        RegistryHeapDispatch* table = parent->dispatch();
        u32 query = table->mSupportsRelease.mFunction;
        if (gabi::call_ptr<u32>(query,parent) != 0) {
            table = parent->dispatch();
            u32 release = table->mRelease.mFunction;
            gabi::call_ptr<void>(release,parent,start);
        }
    }
    unlockRegistryHeap(&sRegistryMutex);
}

/* 02754C98 */
void sead::FrontierHeap::destroyHeap() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    RegistryLifetimeDispatch* lifetime = gabi::at<RegistryLifetimeDispatch>(mObject.mTable);
    u32 destroy = lifetime->mDestroy.mFunction;
    u32 start = mRegionStart;
    RegistryHeap* parent = mParent;
    gabi::call_ptr<void>(destroy,this,2);
    if (parent != NULL) {
        RegistryHeapDispatch* table = parent->dispatch();
        u32 query = table->mSupportsRelease.mFunction;
        if (gabi::call_ptr<u32>(query,parent) != 0) {
            table = parent->dispatch();
            u32 release = table->mRelease.mFunction;
            gabi::call_ptr<void>(release,parent,start);
        }
    }
    unlockRegistryHeap(&sRegistryMutex);
}

/* 0275530C */
u32 sead::FrontierHeap::maxAllocation(s32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 sign = alignment < 0 ? ~0u : 0;
    u32 magnitude = ((u32)alignment ^ sign) - sign;
    if (((magnitude - 1) & magnitude) != 0) return 0;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 aligned = ((u32)mFront + magnitude - 1) & ~(magnitude - 1);
    u32 back = mBack;
    u32 size = aligned > back ? 0 : back - aligned;
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return size;
}

/* 02755478 */
void sead::FrontierHeap::restoreCursors(const FrontierCheckpoint* checkpoint) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 front = checkpoint->mFront;
    if (front != 0 && front != (u32)mFront) {
        RegistryHeapDispatch* table = dispatch();
        if (gabi::call_ptr<u32>(table->mContains.mFunction,this,front) != 0) {
            front = checkpoint->mFront;
            u32 current = mFront;
            if (front <= current) {
                disposeRange(front,current);
                mFront = checkpoint->mFront;
            }
        }
    }
    u32 back = checkpoint->mBack;
    if (back != 0 && back != (u32)mBack) {
        RegistryHeapDispatch* table = dispatch();
        if (gabi::call_ptr<u32>(table->mContains.mFunction,this,back) != 0) {
            back = checkpoint->mBack;
            u32 current = mBack;
            if (back >= current) {
                disposeRange(current,back);
                mBack = checkpoint->mBack;
            }
        }
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
}

/* 02754B38 */
sead::RegistryHeap* sead::FrontierHeap::create(u32 size,const SafeText* name,
        RegistryHeap* parent,s32 direction,u32 locking) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    lockRegistryHeap(&sRegistryMutex);
    if (parent == NULL) {
        parent = registryCurrentHeap(sRegistryManager);
        if (parent == NULL) {
            unlockRegistryHeap(&sRegistryMutex);
            return NULL;
        }
    }
    if (size == 0) {
        sead::RegistryHeapDispatch* table = parent->dispatch();
        u32 query = table->mMaxAllocation.mFunction;
        size = gabi::call_ptr<u32>(query,parent,4);
    }
    u32 roundedSize = (size + 3) & ~3u;
    if (roundedSize < sizeof(sead::FrontierHeap)) {
        unlockRegistryHeap(&sRegistryMutex);
        return NULL;
    }
    sead::RegistryHeapDispatch* table = parent->dispatch();
    u32 allocate = table->mAllocate.mFunction;
    void* allocation = gabi::call_ptr<void*>(allocate,parent,roundedSize,direction * 4);
    if (allocation == NULL) {
        unlockRegistryHeap(&sRegistryMutex);
        return NULL;
    }
    u32 start = gabi::ea(allocation);
    u32 objectAddress = direction == 1 ? start : start + roundedSize - sizeof(sead::FrontierHeap);
    sead::FrontierHeap* heap = gabi::at<sead::FrontierHeap>(objectAddress);
    if (heap != NULL) heap = constructFrontierHeap(heap,name,parent,start,roundedSize,direction,locking);
    heap->resetCursors();
    unlockRegistryHeap(&sRegistryMutex);
    return heap;
}

/* 02755AB4 */
void sead::RegistryHeap::disposeRange(u32 begin,u32 end) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 offset = mDisposables.mOffset;
    u32 sentinel = gabi::ea(&mDisposables) - offset;
    Disposer* object = gabi::at<Disposer>(gabi::ea(mDisposables.mNext.get()) - offset);
    mFlags = (u32)mFlags | 2;
    while (gabi::ea(object) != sentinel) {
        u32 address = gabi::ea(object);
        bool inRange = begin == 0 && end == 0;
        if (!inRange && (begin == 0 || begin <= address) && address < end) inRange = true;
        if (object->mHeap.get() != NULL && inRange) {
            gabi::call_ptr<void>(object->mVtable->mDestroy.mFunction,object,2);
            offset = mDisposables.mOffset;
            sentinel = gabi::ea(&mDisposables) - offset;
            object = gabi::at<Disposer>(gabi::ea(mDisposables.mNext.get()) - offset);
        } else {
            Link* link = gabi::at<Link>(address + offset);
            object = gabi::at<Disposer>(gabi::ea(link->mNext.get()) - offset);
        }
    }
    mFlags = (u32)mFlags & ~2u;
}

/* 027541B8 */
void* sead::FreeRegionHeap::resizeFront(void* allocation,u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    if (gabi::call_ptr<u32>(table->mContains.mFunction,this,allocation) == 0) return NULL;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mUsedBlocks.mOffset;
    u32 sentinel = gabi::ea(&mUsedBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mUsedBlocks.mNext.get(),offset);
    while (gabi::ea(block) != sentinel) {
        u32 payload = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
        if (payload == gabi::ea(allocation)) break;
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    if (gabi::ea(block) == sentinel) {
        if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
        return NULL;
    }
    u32 prefix = (u32)block->mPadding + (u32)block->mSize - size;
    u32 result;
    if (prefix < sizeof(RegistryBlock)) {
        block->mPadding = prefix;
        if (prefix != 0) {
            RegistryBlock* marker = gabi::at<RegistryBlock>(gabi::ea(block) + prefix);
            marker->mPadding = gabi::ea(block) + 1;
        }
        result = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
    } else {
        RegistryBlock* replacement = gabi::at<RegistryBlock>(gabi::ea(block) + prefix);
        if (gabi::ea(replacement) != 0) {
            replacement->mLink.mNext = NULL;
            replacement->mSize = 0;
            replacement->mLink.mPrevious = NULL;
            replacement->mPadding = 0;
        }
        replacement->mSize = size;
        appendUsedBlock(replacement);
        linkAt(block,(u32)mUsedBlocks.mOffset)->detach();
        mUsedBlocks.mCount = (u32)mUsedBlocks.mCount - 1;
        block->mSize = prefix - sizeof(RegistryBlock);
        block->mPadding = 0;
        insertFreeBlock(block);
        result = gabi::ea(replacement) + (u32)replacement->mPadding + sizeof(RegistryBlock);
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return gabi::at<void>(result);
}

/* 02754370 */
void* sead::FreeRegionHeap::resizeBack(void* allocation,u32 size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RegistryHeapDispatch* table = dispatch();
    if (gabi::call_ptr<u32>(table->mContains.mFunction,this,allocation) == 0) return NULL;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    u32 offset = mUsedBlocks.mOffset;
    u32 sentinel = gabi::ea(&mUsedBlocks) - offset;
    RegistryBlock* block = objectOf<RegistryBlock>(mUsedBlocks.mNext.get(),offset);
    while (gabi::ea(block) != sentinel) {
        u32 payload = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
        if (payload == gabi::ea(allocation)) break;
        Link* link = linkAt(block,offset);
        block = objectOf<RegistryBlock>(link->mNext.get(),offset);
    }
    if (gabi::ea(block) == sentinel) {
        if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
        return NULL;
    }
    u32 remaining = (u32)block->mSize - size;
    if (remaining >= sizeof(RegistryBlock)) {
        u32 payload = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
        RegistryBlock* tail = gabi::at<RegistryBlock>(payload + size);
        block->mSize = size;
        if (gabi::ea(tail) != 0) {
            tail->mPadding = 0;
            tail->mLink.mPrevious = NULL;
            tail->mSize = 0;
            tail->mLink.mNext = NULL;
        }
        tail->mSize = remaining - sizeof(RegistryBlock);
        insertFreeBlock(tail);
    }
    u32 result = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return gabi::at<void>(result);
}

/* 02752760 */
u32 sead::PrimitiveRenderer::loadFromFile(void* heap,const SafeText* path) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<LoadArg> request(8);
    request->mPath.mText = path->mText;
    request->mPath.mVtable = gabi::at<SafeTextVtable>(gabi::ea(&sResourcePathTable));
    request->mBuffer = NULL;
    request->mBufferSize = 0;
    request->mHeap = heap;
    request->mAlignment = 32;
    request->mReadChunkSize = 0;
    request->mReadSize = 0;
    request->mRoundedSize = 0;
    request->mNeedsRelease = 0;
    u8* buffer = sFileDeviceManager->tryLoad(request);
    PrimitiveReceiverDispatch* table = gabi::at<PrimitiveReceiverDispatch>(mTable);
    // Forward the consumer's return word; its semantic use is unresolved.
    return gabi::call_ptr<u32>(table->mConsume.mFunction,this,heap,buffer,(u32)request->mReadSize);
}

/* 02753D6C */
void* sead::FreeRegionHeap::allocate(u32 size,s32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* managerStorage = sRegistryManager;
    RegistryManagerState* manager = static_cast<RegistryManagerState*>(managerStorage);
    u32 workingSize = size < 4 ? 4 : size;
    s32 workingAlignment = alignment;
    auto reportFailure = [&](u32 stackOffset) {
        if (manager == NULL) return;
        AllocationFailureReceiver* receiver = manager->mFailureReceiver;
        if (receiver == NULL) return;
        gabi::FrameLocal<AllocationFailureEvent> event(stackOffset);
        event->mHeap = this;
        event->mRequestedSize = size;
        event->mRequestedAlignment = alignment;
        event->mWorkingSize = workingSize;
        event->mWorkingAlignment = workingAlignment;
        gabi::call_ptr<void>(receiver->mTable->mReport.mFunction,receiver,event.get());
    };
    if (workingSize >= (u32)mRegionSize) {
        reportFailure(8);
        return NULL;
    }
    u32 sign = alignment < 0 ? ~0u : 0;
    u32 magnitude = ((u32)alignment ^ sign) - sign;
    if (((magnitude - 1) & magnitude) != 0) {
        reportFailure(0x1C);
        return NULL;
    }
    workingSize = (workingSize + 3) & ~3u;
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    workingAlignment = (s32)((u32)workingAlignment * (u32)mDirection);
    RegistryBlock* block;
    if (workingAlignment < 0) {
        workingAlignment = (s32)(0u - (u32)workingAlignment);
        block = workingAlignment <= 4 ? allocateBlockBack(workingSize)
            : allocateAlignedBlockBack(workingSize,(u32)workingAlignment);
    } else {
        block = workingAlignment <= 4 ? allocateBlockFront(workingSize)
            : allocateAlignedBlockFront(workingSize,(u32)workingAlignment);
    }
    u32 result = 0;
    if (gabi::ea(block) != 0)
        result = gabi::ea(block) + (u32)block->mPadding + sizeof(RegistryBlock);
    else reportFailure(0x30);
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return gabi::at<void>(result);
}

/* 02754F50 */
void* sead::FrontierHeap::allocate(u32 size,s32 alignment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    void* managerStorage = sRegistryManager;
    RegistryManagerState* manager = static_cast<RegistryManagerState*>(managerStorage);
    u32 workingSize = ((size < 4 ? 4 : size) + 3) & ~3u;
    s32 workingAlignment = alignment;
    auto reportFailure = [&](u32 stackOffset) {
        if (manager == NULL) return;
        AllocationFailureReceiver* receiver = manager->mFailureReceiver;
        if (receiver == NULL) return;
        gabi::FrameLocal<AllocationFailureEvent> event(stackOffset);
        event->mHeap = this;
        event->mRequestedSize = size;
        event->mRequestedAlignment = alignment;
        event->mWorkingSize = workingSize;
        event->mWorkingAlignment = workingAlignment;
        gabi::call_ptr<void>(receiver->mTable->mReport.mFunction,receiver,event.get());
    };
    u32 sign = alignment < 0 ? ~0u : 0;
    u32 magnitude = ((u32)alignment ^ sign) - sign;
    if (((magnitude - 1) & magnitude) != 0) {
        reportFailure(8);
        return NULL;
    }
    void* lock = NULL;
    if (((u32)mFlags & 1) != 0) {
        lock = &mLock;
        lockRegistryHeap(lock);
    }
    workingAlignment = (s32)((u32)workingAlignment * (u32)mDirection);
    u32 result;
    if (workingAlignment >= 0) {
        result = ((u32)mFront + (u32)workingAlignment - 1) & ~((u32)workingAlignment - 1);
        u32 end = result + workingSize;
        if (end < result) {
            reportFailure(0x1C);
            if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
            return NULL;
        }
        if (end > (u32)mBack) {
            reportFailure(0x30);
            if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
            return NULL;
        }
        mFront = end;
    } else {
        u32 back = mBack;
        u32 begin = back - workingSize;
        workingAlignment = (s32)(0u - (u32)workingAlignment);
        if (begin > back) {
            reportFailure(0x44);
            if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
            return NULL;
        }
        result = begin & ~((u32)workingAlignment - 1);
        if (result < (u32)mFront) {
            reportFailure(0x58);
            if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
            return NULL;
        }
        mBack = result;
    }
    if (gabi::ea(lock) != 0) unlockRegistryHeap(lock);
    return gabi::at<void>(result);
}

/* WWHD glue
 data sResourcePathTable 101457B4
 ext 02741DAC u8* sead::FileDeviceManager::tryLoad(sead::LoadArg*)
 ext 0273B524 void sead::LinkList::clear()
 ext C0009CC8 heap_adapters::ThreadHeapContext* getThreadSpecificHeapContext(u32)
 data sHeapModuleManager 101F8B94
 ext 0273B378 void sead::LinkList::sort(s32,u32)
 data sDefaultModel 104A048C nocheck
 data sTexturedPrimitiveColor 104A0974 nocheck
 data sPlainPrimitiveColor 104A0964 nocheck
 data sFreeHeapObjectTable 101459E0
 data sFrontierHeapObjectTable 10145B38
 data sFreeHeapInterfaceTable 10145990
 data sFrontierHeapInterfaceTable 10145AE8
 data sFreeHeapTypeGuard 101FD764 nocheck
 data sFrontierHeapTypeGuard 101FD76C nocheck
 data sFreeHeapType 101FDF98 nocheck
 data sFrontierHeapType 101FDFA0 nocheck
 data sFreeHeapTypeTable 101459A0
 data sFrontierHeapTypeTable 10145AF8
 data sFreeHeapParentTypeTable 10145950
 data sFrontierHeapParentTypeTable 10145AA8
 data sRegistryHeapTypeTable 10145BF0
 data sRegistryManagerTable 10145E00
 data sRootNameTable 10145D74
 data sRegistryState 104A02F8 nocheck
 data sStaticRegistryArena 104A0300 nocheck
 data sRegistryArena 101F8B50
 data sRegistryMutex 104A0328 nocheck
 data sRootHeapBuffer 104A030C nocheck
 data sIndependentHeapBuffer 104A0364 nocheck
 data sArenaLifetime 101F8B34
 data sMutexLifetime 101F8B40
 data sHeapTextTable 10145C00
 data sHeapInterfaceTable 10145C30
 data sRegistryHeapTable 10145CBC
 data sRegistrationTable 10145940
 data sRootRegistryHeap 101F8B30
 ext 028E90D4 void copyRegistryModel(const sead::RegistryMatrix34*,sead::RegistryMatrix34*)
 ext 0274D83C const sead::RegistryMatrix44* updateRegistryProjection(void*)
 ext 028E8970 void copyRegistryProjection(const sead::RegistryMatrix44*,sead::RegistryMatrix44*)
 ext 0272D680 void combineRegistryMatrices(sead::RegistryMatrix44*,const sead::RegistryMatrix44*,const sead::RegistryMatrix34*)
 ext C0006860 void selectRegistryFetchShader(const void*)
 ext C0006A28 void selectRegistryVertexShader(const void*)
 ext C00068F0 void selectRegistryPixelShader(const void*)
 ext C0006A40 void setRegistryVertexUniform(u32,u32,const void*)
 ext C0006908 void setRegistryPixelUniform(u32,u32,const void*)
 ext C00068B8 void setRegistryLineWidth(f32)
 ext C00068F8 void setRegistryPixelTexture(const void*,u32)
 ext C00068E0 void setRegistryPixelSampler(const void*,u32)
 ext C0006798 void setRegistryAttribute(u32,u32,u32,const void*)
 ext C0006178 void drawRegistryIndices(u32,u32,u32,const void*,u32,u32)
 ext 0273AF40 void releaseRegistryStorage(void*)
 str 10145DF4 "RootHeap"
 ext 02760168 void destroyRegistryLock(void*,u32)
 ext 028F521C void clearRegistryMemory(void*,u32)
 ext 02756CC0 void constructStaticRegistryArena(sead::RegistryArena_l*)
 ext 02760084 void constructStaticRegistryMutex(void*)
 ext 028F026C void registerRegistryLifetime(void*)
 ext 0273B560 void sead::PointerBuffer::setStorage(s32,void*)
 ext 02756D18 void configureRegistryArena(sead::RegistryArena_l*,u32)
 ext 0273AD10 void* allocateRegistryStorage(u32)
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
 ext 027600EC void constructRegistryLock(void*,sead::RegistryHeap*)
 ext 027601BC void lockRegistryHeap(void*)
 ext 027601F0 void unlockRegistryHeap(void*)
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 0273B310 sead::Link* sead::Link::detach()
*/
