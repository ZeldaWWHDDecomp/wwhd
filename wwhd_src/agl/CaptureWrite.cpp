#include "agl/CaptureProvider.h"
#include "agl/CaptureFileOptions.h"
#include "agl/CaptureTextTools.h"
static sead::SafeTextVtable sCaptureWriteTextTable;
static sead::DisposerVtable sCaptureWriteHandleTable;
/* Borrowed AGL runtime prefix; only its heap pointer is consumed here. */
struct CaptureWriteHeapView_l { u8 mUnobserved[0x2C]; gptr<sead::RegistryHeap> mHeap; };
WWHD_OFFSET(CaptureWriteHeapView_l, mHeap, 0x2C);
WWHD_SIZE(CaptureWriteHeapView_l, 0x30);
static CaptureWriteHeapView_l* sCaptureWriteRuntime;
void aglCaptureWriteLock(sead::CriticalSection*);
void aglCaptureWriteUnlock(sead::CriticalSection*);
void aglCaptureWriteConstructDisposer(sead::Disposer*);
void aglCaptureWriteDestroyDisposer(sead::Disposer*, u32);
sead::FileDevice* aglCaptureWriteOpen(sead::FileDevice*, sead::FileHandle*, sead::SafeText*, u32, u32);
u32 aglCaptureWriteClose(sead::FileDevice*, sead::FileHandle*);
u32 aglCaptureWriteHandle(sead::FileHandle*, const void*, u32);
void* aglCaptureWriteAllocate(u32, sead::RegistryHeap*, s32);
void* aglCaptureWriteMove(void*, const void*, u32, u32);
void aglCaptureWriteDeleteBuffer(void*);
static void finishCaptureWriteHandle(sead::FileHandle* handle) {
    handle->mDisposer.mVtable = &sCaptureWriteHandleTable;
    sead::FileDevice* device = handle->mSourceDevice;
    if (device) aglCaptureWriteClose(device, handle);
    aglCaptureWriteDestroyDisposer(&handle->mDisposer, 0);
}
/* 027C5474 */
u32 agl::CaptureProvider::writeCaptureFile(const void* data, u32 size, CaptureFileOptions* options) {
    gabi::AutoFrame frame{gabi::current_function()};
    aglCaptureWriteLock(&mCriticalSection);
    gabi::FrameLocal<sead::SafeText> path(8);
    path->mVtable = &sCaptureWriteTextTable;
    path->mText.v = gabi::guest_str("");
    options->mPath.assureTermination();
    options->mPath.assureTermination();
    be<u8>* selectedPath = options->mPath.mText;
    sFileDeviceEmptyText.assureTermination();
    bool askForPath = selectedPath == sFileDeviceEmptyText.mText.get();
    if (!askForPath) {
        selectedPath = options->mPath.mText;
        const be<u8>* left = selectedPath;
        const be<u8>* right = sFileDeviceEmptyText.mText;
        for (u32 index = 0; index <= sead::kMaxTextLength; ++index) {
            u8 a = left[index], b = right[index];
            if (a != b) break;
            if (!a) { askForPath = true; break; }
        }
    }
    if (askForPath) {
        gabi::FrameLocal<sead::SafeText> mode(0x18);
        mode->mText.v = gabi::guest_str("Save");
        mode->mVtable = &sCaptureWriteTextTable;
        if (capture_text::describeFileDialogRequest(this, path, mode, &options->mDialogId,
                                                   &options->mExtension, &options->mSuggestedName) != 1) {
            aglCaptureWriteUnlock(&mCriticalSection);
            return 0;
        }
    } else {
        path->mText = selectedPath;
    }
    gabi::FrameLocal<sead::FileHandle> handle(0x20);
    aglCaptureWriteConstructDisposer(&handle->mDisposer);
    handle->mDevice = nullptr;
    handle->mSourceDevice = nullptr;
    handle->mDisposer.mVtable = &sCaptureWriteHandleTable;
    handle->mReadChunkSize = 0;
    sead::FileDevice* opened = aglCaptureWriteOpen(mHostDevice, handle, path, 1, 0);
    if (!opened) {
        finishCaptureWriteHandle(handle);
        aglCaptureWriteUnlock(&mCriticalSection);
        return 0;
    }
    void* storage = aglCaptureWriteAllocate(size, sCaptureWriteRuntime->mHeap, 64);
    aglCaptureWriteMove(storage, data, size, 0);
    aglCaptureWriteHandle(handle, storage, size);
    aglCaptureWriteClose(opened, handle);
    aglCaptureWriteDeleteBuffer(storage);
    sead::SafeTextBuffer* output = options->mOutputPath;
    if (output) {
        be<u8>* destination = output->mText;
        path->assureTermination();
        const be<u8>* text = path->mText;
        s32 length = 0;
        if (text[0]) {
            do {
                ++length;
                if (length > sead::kMaxTextLength) { length = 0; break; }
            } while (text[length]);
        }
        s32 capacity = output->mCapacity;
        u32 copied = length < capacity ? static_cast<u32>(length) : static_cast<u32>(capacity) - 1;
        path->assureTermination();
        aglCaptureWriteMove(destination, path->mText.get(), copied, 0);
        destination[copied] = 0;
    }
    finishCaptureWriteHandle(handle);
    aglCaptureWriteUnlock(&mCriticalSection);
    return 1;
}
/* WWHD glue
data sCaptureWriteTextTable 1016A368
data sCaptureWriteHandleTable 1016A398
data sCaptureWriteRuntime 101F8C14
str 10000160 ""
str 1016A45C "Save"
ext 027601BC void aglCaptureWriteLock(sead::CriticalSection*)
ext 027601F0 void aglCaptureWriteUnlock(sead::CriticalSection*)
ext 02752A84 void aglCaptureWriteConstructDisposer(sead::Disposer*)
ext 02752BEC void aglCaptureWriteDestroyDisposer(sead::Disposer*, u32)
ext 02740B24 sead::FileDevice* aglCaptureWriteOpen(sead::FileDevice*, sead::FileHandle*, sead::SafeText*, u32, u32)
ext 02740BE0 u32 aglCaptureWriteClose(sead::FileDevice*, sead::FileHandle*)
ext 027414F8 u32 aglCaptureWriteHandle(sead::FileHandle*, const void*, u32)
ext 0273B0D4 void* aglCaptureWriteAllocate(u32, sead::RegistryHeap*, s32)
ext C0009988 void* aglCaptureWriteMove(void*, const void*, u32, u32)
ext 0273AFC8 void aglCaptureWriteDeleteBuffer(void*)
*/
