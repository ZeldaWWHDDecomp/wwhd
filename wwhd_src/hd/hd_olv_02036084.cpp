/* HD JPEG encode/decode worker singleton. Private reconstruction. */
#include "hd/hd_jpeg_mgr.h"
using namespace gabi;
static HdJpegSingleton l_singleton;
static HdJpegDisposerHolder l_disposerHolder;
static u32 l_disposerVTable;
static u32 l_stringVTable;
static u32 l_delegateVTable;
static u32 l_threadHandler;
static u32 l_threadPriority;
static HdJpegConfiguration l_defaultConfig;
static HdJpegStatics l_statics;
static HdJpegRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdJpegRegistration l_registration1;
static HdJpegRegistration l_registration2;
static HdJpegRegistration l_registration3;
extern void* hdJpegAllocate(u32);
extern void* hdJpegHeapAllocate(u32,HdJpegHeap*,u32);
extern void hdJpegDelete(void*);
extern void hdJpegDeleteArray(void*);
extern void hdJpegClear(void*,u32);
extern void hdJpegCriticalConstruct(HdJpegCriticalSection*);
extern void hdJpegCriticalDestroy(HdJpegCriticalSection*,u32);
extern void hdJpegCriticalEnter(HdJpegCriticalSection*);
extern void hdJpegCriticalLeave(HdJpegCriticalSection*);
extern void hdJpegDisposerConstruct(HdJpegDisposer*,HdJpegHeap*,u32);
extern void hdJpegDisposerDestroy(HdJpegDisposer*,u32);
extern HdJpegHeap* hdJpegGetParentHeap();
extern HdJpegHeap* hdJpegGetWorkHeap();
extern HdJpegHeap* hdJpegCreateHeap(u32,HdJpegSafeString*,HdJpegHeap*,u32,u32);
extern void* hdJpegHeapNew(u32,HdJpegHeap*,u32);
extern HdJpegThread* hdJpegThreadConstruct(HdJpegThread*,HdJpegSafeString*,HdJpegDelegate*,HdJpegHeap*,u32,u32,u32,u32,u32);
extern void hdJpegSaveConstruct(HdJpegSaveObject*,HdJpegHeap*,u32);
extern void hdJpegSavePicture(HdJpegSaveObject*,void*,HdJpegSaveParameters*,u32,u32);
extern u32 hdJpegGetDescriptor1();
extern u32 hdJpegGetDescriptor2();
extern void hdJpegSaveDescriptor1(HdJpegSaveObject*,u32,u32);
extern void hdJpegSaveDescriptor2(HdJpegSaveObject*,u32,u32);
extern void hdJpegSaveDestroy(HdJpegSaveObject*,u32);
extern u32 hdJpegReadHeader(void*,u32,u32,be<u32>*,be<u32>*);
extern u32 hdJpegDecode(void*,u32,void*,u32,u32,void*,u32);
extern void hdJpegSurfaceSize(HdJpegSurface*);
extern void hdJpegTextureRegisters(HdJpegTexture*);
extern void hdJpegCopySurface(HdJpegSurface*,u32,u32,HdJpegSurface*,u32,u32);
extern void hdJpegBlockMove(void*,void*,u32,u32);
extern void hdJpegTextureFormat(be<u32>*,u32,u32);
extern void hdJpegRegister(void*);
extern void hdJpegInitSigned(be<u8>*);
extern void hdJpegInitUnsigned(be<u8>*);

/* 02036084 */
HdJpegMgr* hdJpegMgrConstruct(HdJpegMgr* self) {
    if (!self) {
        self = static_cast<HdJpegMgr*>(hdJpegAllocate(sizeof(HdJpegMgr)));
        if (!self) return NULL;
    }
    self->mThread = NULL;
    self->mState = HD_JPEG_NOT_STARTED;
    self->mOk = 0;
    self->mDone = 0;
    hdJpegClear(&self->mSave,sizeof(HdJpegSaveJob));
    hdJpegClear(&self->mDecode,sizeof(HdJpegDecodeJob));
    hdJpegCriticalConstruct(&self->mCriticalSection);
    return self;
}

/* 020360FC */
HdJpegMgr* hdJpegMgrCreateInstance(HdJpegHeap* heap) {
    HdJpegMgr* current = l_singleton.mInstance;
    if (current) return current;
    HdJpegMgr* instance = static_cast<HdJpegMgr*>(hdJpegHeapAllocate(sizeof(HdJpegMgr),heap,4));
    if (instance) {
        hdJpegDisposerConstruct(&instance->mDisposer,heap,3);
        instance->mDisposer.mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    }
    l_disposerHolder.mDisposer = reinterpret_cast<HdJpegDisposer*>(instance);
    HdJpegMgr* result = instance;
    if (instance) result = hdJpegMgrConstruct(instance);
    l_singleton.mInstance = result;
    return result;
}

/* 02036190 */
void hdJpegMgrDestroy(HdJpegMgr* self,u32 flags) {
    if (!self) return;
    hdJpegCriticalDestroy(&self->mCriticalSection,2);
    if (flags&1) hdJpegDelete(self);
}

/* 020361E4 */
void hdJpegMgrCreateThread(HdJpegMgr* self) {
    // HD: preserve the original frame and reused SafeString parameter storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::Local<HdJpegSafeString> storage;
    HdJpegSafeString* name = storage.get();
    name->mVTable = &l_stringVTable;
    name->mText = gabi::at<const char>(gabi::guest_addr("JpegWorkerHeap"));
    HdJpegHeap* parent = hdJpegGetParentHeap();
    HdJpegHeap* heap = hdJpegCreateHeap(0,name,parent,1,0);
    HdJpegThread* thread = static_cast<HdJpegThread*>(hdJpegHeapNew(0x98,heap,4));
    if (thread) {
        name->mVTable = &l_stringVTable;
        name->mText = gabi::at<const char>(gabi::guest_addr("JpegEncodeMgrThread"));
        HdJpegDelegate* delegate = static_cast<HdJpegDelegate*>(hdJpegHeapNew(sizeof(HdJpegDelegate),heap,4));
        if (delegate) {
            delegate->mObject = self;
            delegate->mThisAdjustment = 0;
            delegate->mFunction = &l_threadHandler;
            delegate->mVirtualOffset = 0xFFFF;
            delegate->mVTable = reinterpret_cast<gfn*>(&l_delegateVTable);
        }
        thread = hdJpegThreadConstruct(thread,name,delegate,heap,l_threadPriority+2,0,0x7FFFFFFF,0x5000,0x20);
    }
    self->mThread = thread;
    gabi::call_ptr<u32>(heap->mVTable[11].get(),heap);
    thread = self->mThread;
    gabi::call_ptr<u32>(thread->mVTable[11].get(),thread);
    self->mState = HD_JPEG_IDLE;
}

/* 020363E4 */
u32 hdJpegMgrRequestSave(HdJpegMgr* self,void* buffer,u32 width,u32 height,u32 parameter,u32 slot) {
    if (self->mState != HD_JPEG_IDLE) return 0;
    self->mSave.mBuffer = buffer;
    self->mSave.mSlot = slot;
    self->mSave.mWidth = width;
    self->mState = HD_JPEG_SAVE;
    self->mSave.mHeight = height;
    HdJpegThread* thread = self->mThread;
    self->mDone = 0;
    self->mSave.mParameter = parameter;
    gabi::call_ptr<u32>(thread->mVTable[7].get(),thread,1u,0u);
    return 1;
}

/* 02036474 */
u32 hdJpegMgrRequestDecode(HdJpegMgr* self,void* jpeg,HdJpegTextureMap* texture,void* image,u32 size) {
    if (self->mState != HD_JPEG_IDLE) return 0;
    self->mDecode.mSize = size;
    HdJpegThread* thread = self->mThread;
    self->mDecode.mImage = image;
    self->mDone = 0;
    self->mState = HD_JPEG_DECODE;
    self->mDecode.mJpeg = jpeg;
    self->mDecode.mTexture = texture;
    gabi::call_ptr<u32>(thread->mVTable[7].get(),thread,2u,0u);
    return 1;
}

/* 020364F4 */
void hdJpegMgrSave(HdJpegMgr* self) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self->mSave.mBuffer) return;
    // HD: the constructor intentionally writes beyond the short object view into its caller frame.
    gabi::Local<HdJpegSaveFrame> storage;
    HdJpegSaveFrame* frame = storage.get();
    HdJpegHeap* heap = hdJpegGetWorkHeap();
    hdJpegSaveConstruct(&frame->mObject,heap,0);
    void* buffer = self->mSave.mBuffer;
    frame->mParameters.mHeight = self->mSave.mHeight;
    frame->mParameters.mWidth = self->mSave.mWidth;
    u32 parameter = self->mSave.mParameter;
    u32 slot = self->mSave.mSlot;
    frame->mParameters.mFormat = 3;
    hdJpegSavePicture(&frame->mObject,buffer,&frame->mParameters,parameter,slot);
    u32 ok = frame->mObject.mOk;
    self->mOk = ok;
    if (ok) {
        u32 descriptor = hdJpegGetDescriptor1();
        hdJpegSaveDescriptor1(&frame->mObject,self->mSave.mSlot,descriptor);
        descriptor = hdJpegGetDescriptor2();
        hdJpegSaveDescriptor2(&frame->mObject,self->mSave.mSlot,descriptor);
        hdJpegDeleteArray(self->mSave.mBuffer);
        self->mSave.mBuffer = NULL;
        self->mOk = 1;
    }
    hdJpegSaveDestroy(&frame->mObject,2);
}

/* 020365C8 */
void hdJpegMgrDecode(HdJpegMgr* self) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self->mDecode.mJpeg) {
        self->mOk = 0;
        return;
    }
    // HD: named views retain all original surface/texture/config offsets and untouched bytes.
    gabi::Local<HdJpegDecodeFrame> storage;
    HdJpegDecodeFrame* frame = storage.get();
    HdJpegSurface* source = &frame->mSource;
    HdJpegTexture* destination = &frame->mDestination;
    HdJpegHeap* heap = hdJpegGetWorkHeap();
    frame->mWorkSize = 0;
    if (hdJpegReadHeader(self->mDecode.mJpeg,0x50000,self->mDecode.mSize,&frame->mWidth,&frame->mWorkSize)) {
        self->mOk = 0;
        return;
    }
    u32 freeSize = gabi::call_ptr<u32>(heap->mVTable[29].get(),heap);
    u32 workSize = frame->mWorkSize;
    if (freeSize <= workSize) {
        self->mOk = 0;
        return;
    }
    void* work = hdJpegHeapAllocate(workSize,heap,4);
    source->mDepth = 0;
    source->mWidth = frame->mWidth;
    source->mFormat = HD_JPEG_FORMAT_RGBA8;
    source->mDimension = HD_JPEG_SURFACE_2D;
    source->mAa = 0;
    source->mMipLevels = 0;
    source->mUse = 1;
    source->mMip = NULL;
    source->mImage = NULL;
    source->mHeight = frame->mHeight;
    source->mSwizzle = 0;
    source->mTileMode = HD_JPEG_TILE_LINEAR;
    hdJpegSurfaceSize(source);
    void* buffer = hdJpegHeapAllocate((source->mImageSize+3)&~3u,heap,source->mAlignment);
    u32 result = hdJpegDecode(work,frame->mWorkSize,self->mDecode.mJpeg,0x50000,self->mDecode.mSize,buffer,source->mImageSize);
    hdJpegDeleteArray(work);
    source->mImage = buffer;
    for (u32 i=0;i<0x36;++i) frame->mConfig[i] = l_defaultConfig.mWords[i];
    destination->mSurface.mHeight = frame->mHeight;
    destination->mSurface.mMipLevels = 0;
    destination->mMipCount = 0;
    destination->mComponentSelect = frame->mConfig[26];
    destination->mSliceCount = 0;
    destination->mSurface.mDepth = 0;
    destination->mFirstMip = 0;
    destination->mSurface.mDimension = HD_JPEG_SURFACE_2D;
    destination->mSurface.mAa = 0;
    destination->mSurface.mTileMode = 0;
    destination->mFirstSlice = 0;
    destination->mSurface.mWidth = frame->mWidth;
    destination->mSurface.mFormat = HD_JPEG_FORMAT_RGBA8;
    destination->mSurface.mSwizzle = 0;
    destination->mSurface.mUse = 1;
    hdJpegSurfaceSize(&destination->mSurface);
    hdJpegTextureRegisters(destination);
    destination->mSurface.mImage = self->mDecode.mImage;
    hdJpegCopySurface(source,0,0,&destination->mSurface,0,0);
    HdJpegTextureMap* texture = self->mDecode.mTexture;
    hdJpegBlockMove(&texture->mTexture,destination,sizeof(HdJpegSurface),0);
    texture->mTexture.mFirstMip = 0;
    u32 mipLevels = texture->mTexture.mSurface.mMipLevels;
    texture->mTexture.mComponentSelect = 0x10203;
    texture->mTexture.mMipCount = mipLevels;
    texture->mTexture.mSurface.mFormat = 0x41A;
    u32 depth = texture->mTexture.mSurface.mDepth;
    texture->mTexture.mFirstSlice = 0;
    texture->mTexture.mSliceCount = depth;
    hdJpegTextureRegisters(&texture->mTexture);
    hdJpegTextureFormat(&frame->mFormat,0x41A,1);
    u32 dimensions = ((u32)(u16)destination->mSurface.mWidth<<16)|(u16)destination->mSurface.mHeight;
    u32 format = frame->mFormat;
    texture->mType = 1;
    frame->mPackedSize = dimensions;
    texture->mWidth = frame->mSizeHalves[0];
    texture->mFormat = format;
    texture->mHeight = frame->mSizeHalves[1];
    hdJpegDeleteArray(buffer);
    self->mOk = result==0?1:0;
}

/* 02036860 */
void hdJpegMgrThreadProc(HdJpegMgr* self,HdJpegThread* thread,u32 message) {
    if (message != HD_JPEG_SAVE && message != HD_JPEG_DECODE) return;
    HdJpegCriticalSection* critical = &self->mCriticalSection;
    hdJpegCriticalEnter(critical);
    if (message == HD_JPEG_SAVE) hdJpegMgrSave(self);
    else hdJpegMgrDecode(self);
    self->mState = HD_JPEG_IDLE;
    self->mDone = 1;
    hdJpegCriticalLeave(critical);
}

/* 0203691C */
void hdJpegMgrDisposerDestroy(HdJpegDisposer* disposer,u32 flags) {
    if (!disposer) return;
    disposer->mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    if (disposer == l_disposerHolder.mDisposer) {
        HdJpegMgr* instance = l_singleton.mInstance;
        l_disposerHolder.mDisposer = NULL;
        hdJpegMgrDestroy(instance,2);
        l_singleton.mInstance = NULL;
    }
    hdJpegDisposerDestroy(disposer,0);
    if (flags&1) hdJpegDelete(disposer);
}

/* 020369B8 */
void hdJpegMgrStaticInit() {
    l_statics.mWords[2] = 0;
    l_statics.mWords[0] = 0;
    l_statics.mWords[3] = 0;
    l_statics.mWords[1] = 0;
    hdJpegRegister(&l_registration1);
    const f32 lower = l_lower;
    const f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdJpegInitSigned(&l_range.mSigned);
    hdJpegRegister(&l_registration2);
    hdJpegInitUnsigned(&l_range.mUnsigned);
    hdJpegRegister(&l_registration3);
}

/* 02036A4C */
void hdJpegDeleteCompanion(void* object,u32 flags) {
    if (!object) return;
    if (flags&1) hdJpegDelete(object);
}

/* 02036A60 */
void hdJpegEmptyCompanion() {}

/* WWHD glue (tools/verify/autoglue.py)
data l_singleton 1018F3F4
data l_disposerHolder 1018F3F8
data l_disposerVTable 10005250
data l_stringVTable 10005120
data l_delegateVTable 10005138
data l_threadHandler 02036860
data l_threadPriority 101F8B9C
data l_defaultConfig 10005148
data l_statics 102009B4
data l_range 102009A8
data l_lower 10005244
data l_upper 10005248
data l_registration1 1018F3D0
data l_registration2 1018F3DC
data l_registration3 1018F3E8
str 10005234 "JpegWorkerHeap" renamed
str 10005220 "JpegEncodeMgrThread" renamed
ext 0273AD10 void* hdJpegAllocate(u32)
ext 0273B0D4 void* hdJpegHeapAllocate(u32,HdJpegHeap*,u32)
ext 0273AF40 void hdJpegDelete(void*)
ext 0273AFC8 void hdJpegDeleteArray(void*)
ext 028F521C void hdJpegClear(void*,u32)
ext 02760084 void hdJpegCriticalConstruct(HdJpegCriticalSection*)
ext 02760168 void hdJpegCriticalDestroy(HdJpegCriticalSection*,u32)
ext 027601BC void hdJpegCriticalEnter(HdJpegCriticalSection*)
ext 027601F0 void hdJpegCriticalLeave(HdJpegCriticalSection*)
ext 02752B0C void hdJpegDisposerConstruct(HdJpegDisposer*,HdJpegHeap*,u32)
ext 02752BEC void hdJpegDisposerDestroy(HdJpegDisposer*,u32)
ext 0203E900 HdJpegHeap* hdJpegGetParentHeap()
ext 0203E90C HdJpegHeap* hdJpegGetWorkHeap()
ext 02753004 HdJpegHeap* hdJpegCreateHeap(u32,HdJpegSafeString*,HdJpegHeap*,u32,u32)
ext 0273B050 void* hdJpegHeapNew(u32,HdJpegHeap*,u32)
ext 0275FBD8 HdJpegThread* hdJpegThreadConstruct(HdJpegThread*,HdJpegSafeString*,HdJpegDelegate*,HdJpegHeap*,u32,u32,u32,u32,u32)
ext 0272720C void hdJpegSaveConstruct(HdJpegSaveObject*,HdJpegHeap*,u32)
ext 027272FC void hdJpegSavePicture(HdJpegSaveObject*,void*,HdJpegSaveParameters*,u32,u32)
ext 025BE8E8 u32 hdJpegGetDescriptor1()
ext 025BE8F8 u32 hdJpegGetDescriptor2()
ext 02727420 void hdJpegSaveDescriptor1(HdJpegSaveObject*,u32,u32)
ext 02727470 void hdJpegSaveDescriptor2(HdJpegSaveObject*,u32,u32)
ext 027272A0 void hdJpegSaveDestroy(HdJpegSaveObject*,u32)
ext 0284DDB0 u32 hdJpegReadHeader(void*,u32,u32,be<u32>*,be<u32>*)
ext 0284BE8C u32 hdJpegDecode(void*,u32,void*,u32,u32,void*,u32)
ext C00060D0 void hdJpegSurfaceSize(HdJpegSurface*)
ext C0006498 void hdJpegTextureRegisters(HdJpegTexture*)
ext C0006120 void hdJpegCopySurface(HdJpegSurface*,u32,u32,HdJpegSurface*,u32,u32)
ext C0009988 void hdJpegBlockMove(void*,void*,u32,u32)
ext 0287F1EC void hdJpegTextureFormat(be<u32>*,u32,u32)
ext 028F026C void hdJpegRegister(void*)
ext 028ED6F8 void hdJpegInitSigned(be<u8>*)
ext 028EAB2C void hdJpegInitUnsigned(be<u8>*)
*/
