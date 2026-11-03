// HD native audio file lifecycle; reconstructed from the game's instructions.
#include "bindings.h"
#include "JSystem/JAudio/JASNativeFile.h"

/* 0280F2EC */
JASystem::Kernel::WorkPoolBlock* NativeAudioWorkPool::allocate() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (s32(mPool.mCount) >= s32(mPool.mCapacity)) {
        initializeNativeFileRecord(NULL);
        return NULL;
    }
    JASystem::Kernel::WorkPoolBlock* record = mPool.mFreeBlocks;
    if (record != NULL) mPool.mFreeBlocks = record->mNextFree;
    if (record != NULL) {
        record->mLink.mNext = NULL;
        record->mLink.mPrevious = NULL;
    }
    // Preserve the original link address even when the free-list result is null.
    JASystem::Kernel::WorkPoolLink* link = gabi::at<JASystem::Kernel::WorkPoolLink>(
        gabi::ea(record) + offsetof(JASystem::Kernel::WorkPoolBlock,mLink));
    reinterpret_cast<sead::Link*>(&mPool.mSentinel)->insertBefore(reinterpret_cast<sead::Link*>(link));
    mPool.mCount = u32(mPool.mCount) + 1;
    initializeNativeFileRecord(record);
    return record;
}

/* 0280F9E8 */
void NativeAudioFile::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReady != 0) {
        AudioFileRecord* record = allocateRecord();
        closeNativeAudioFile(mRecord.get(),record,handle()->mHandle,0);
        releaseRecord(record);
        mRequestState = 0;
        mRecord = NULL;
        mReady = 0;
    }
}

/* 0280FA68 */
u32 NativeAudioFile::read(void* buffer,u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!isReady()) return 0;
    AudioFileRecord* record = allocateRecord();
    s32 result = readNativeAudioFile(mRecord.get(),record,buffer,1,size,handle()->mHandle,0,0);
    releaseRecord(record);
    if (result < 0) return 0;
    mRequestState = result;
    return 1;
}


static JASystem::Kernel::TWorkPool* sAudioWorkPool;
static sead::SafeTextVtable sAudioTextTable;
static sead::SafeTextVtable sAudioPathTable;
typedef be<u8> NativeAudioPathByte;
static NativeAudioPathByte sPathSeparator;

/* inlined into NativeAudioFile::open */
static s32 nativeAudioTextLength(const be<u8>* text) {
    s32 length = 0;
    while (*text != 0) {
        ++length;
        ++text;
        if (length > sead::kMaxTextLength) return 0;
    }
    return length;
}

/* inlined into NativeAudioFile::open */
static void appendNativeAudioText(sead::SafeTextBuffer* destination,sead::SafeText* source) {
    be<u8>* storage = destination->mText;
    destination->assureTermination();
    s32 previousLength = nativeAudioTextLength(destination->mText.get());
    s32 offset = previousLength < 0 ? 0 : previousLength;
    source->assureTermination();
    s32 length = nativeAudioTextLength(source->mText.get());
    s32 remaining = s32(u32(destination->mCapacity) - u32(offset));
    if (length >= remaining) length = s32(u32(remaining) - 1);
    if (length > 0) {
        be<u8>* output = gabi::at<be<u8>>(gabi::ea(storage) + u32(offset));
        source->assureTermination();
        moveNativeAudioPath(output,source->mText.get(),length,0);
        s32 end = s32(u32(offset) + u32(length));
        if (end > previousLength)
            *gabi::at<be<u8>>(gabi::ea(storage) + u32(end)) = 0;
    }
}

/* 0280F600 */
u32 NativeAudioFile::open(const be<u8>* path) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReady != 0) return 1;
    JASystem::Kernel::TWorkPool* pool = sAudioWorkPool;
    mRecord = static_cast<AudioFileRecord*>(pool->mContext.get());
    AudioFileRecord* record = allocateRecord();
    gabi::FrameLocal<sead::SafeText> source(8);
    gabi::FrameLocal<sead::FixedSafeText<512>> fullPath(16);
    fullPath->mText = fullPath->mStorage;
    fullPath->mStorage[511] = 0;
    fullPath->mCapacity = 512;
    fullPath->mVtable = &sAudioPathTable;
    fullPath->mStorage[0] = 0;
    pool = sAudioWorkPool;
    source->mVtable = &sAudioTextTable;
    // This adapter treats the canonical mHeap word as the base-path address.
    source->mText = reinterpret_cast<be<u8>*>(pool->mHeap.get());
    assureNativeAudioText(source.get());
    s32 length = nativeAudioTextLength(source->mText.get());
    s32 capacity = fullPath->mCapacity;
    if (length >= capacity) length = s32(u32(capacity) - 1);
    source->assureTermination();
    moveNativeAudioPath(fullPath->mStorage,source->mText.get(),length,0);
    fullPath->mStorage[length] = 0;

    source->mVtable = &sAudioTextTable;
    source->mText = &sPathSeparator;
    appendNativeAudioText(fullPath.get(),source.get());
    source->mVtable = &sAudioTextTable;
    source->mText = const_cast<be<u8>*>(path);
    appendNativeAudioText(fullPath.get(),source.get());
    fullPath->assureTermination();
    s32 result = openNativeAudioFile(mRecord.get(),record,fullPath->mText.get(),"r",&handle()->mHandle,0);
    if (result == 0) {
        result = statNativeAudioFile(mRecord.get(),record,handle()->mHandle,prefix + 4,0);
        if (result != 0) closeNativeAudioFile(mRecord.get(),record,handle()->mHandle,0);
    }
    if (result != 0) {
        releaseRecord(record);
        return 0;
    }
    releaseRecord(record);
    mRequestState = 0;
    mReady = 1;
    return 1;
}


/* WWHD glue
 ext C0008E40 void initializeNativeFileRecord(JASystem::Kernel::WorkPoolBlock*)
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 0280F558 AudioFileRecord* JASDvdFile::allocateRecord()
 ext 0280F5AC void JASDvdFile::releaseRecord(AudioFileRecord*)
 ext 0280FA60 u32 JASDvdFile::isReady() const
 ext C0008CF8 s32 closeNativeAudioFile(void*,AudioFileRecord*,u32,u32)
 ext C0008EC8 s32 readNativeAudioFile(void*,AudioFileRecord*,void*,u32,u32,u32,u32,u32)
 data sAudioWorkPool 101F9D6C
 data sAudioTextTable 1016FDB8
 data sAudioPathTable 1016FE00
 data sPathSeparator 1016FE18
 str 1016FE1A "r"
 ext 0280FBB4 void assureNativeAudioText(sead::SafeText*)
 ext C0009988 void* moveNativeAudioPath(void*,const void*,u32,u32)
 ext C0008E98 s32 openNativeAudioFile(void*,AudioFileRecord*,const be<u8>*,const char*,be<u32>*,u32)
 ext C0008E00 s32 statNativeAudioFile(void*,AudioFileRecord*,u32,void*,u32)
*/
