// HD JKernel file adapters; reconstructed from the game's instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRNativeFile.h"

static sead::FileDeviceManager* sFileManager;
static sead::SafeTextVtable sFileTextTable;

/* 0284B690 */
static u32 nativeFileSize(sead::FileHandle* handle) {
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)queryNativeFileSize),handle);
}

/* 0284B694 */
static u32 openNativeFile(const be<u8>* path,sead::FileHandle* handle) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> text(8);
    text->mVtable = &sFileTextTable;
    text->mText = const_cast<be<u8>*>(path);
    return sFileManager->mDefaultDevice->tryOpen(handle,text.get(),0,0) != NULL;
}

/* 0284B6E8 */
static u32 openNativeFileForward(const be<u8>* path,sead::FileHandle* handle) {
    return gabi::tail_ptr<u32>(gabi::guest_fn((const void*)openNativeFileAlias),path,handle);
}

/* 0284B6EC */
static u32 readNativeFile(sead::FileHandle* handle,void* buffer,u32 size,s32 offset) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (handle->seek(offset,0)) {
        gabi::FrameLocal<be<u32>> readSize(8);
        if (handle->tryRead(readSize.get(),buffer,size)) return *readSize;
    }
    return 0xFFFFFFFF;
}

/* 0284B780 */
static u32 closeNativeFile(sead::FileHandle* handle) {
    return handle->tryClose() ? 1 : 0xFFFFFFFF;
}

/* 0284B7BC */
static void copyExistingNativePath(void* output,const be<u8>* path) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u8>> exists(8);
    gabi::FrameLocal<sead::SafeText> text(12);
    gabi::FrameLocal<NativeFilePath> buffer(20);
    buffer->mBytes[0] = 0;
    text->mVtable = &sFileTextTable;
    text->mText = const_cast<be<u8>*>(path);
    *exists = 0;
    sFileManager->mDefaultDevice->tryIsExistFile(exists.get(),text.get());
    if (*exists != 0) {
        u32 i = 0;
        for (; i < sizeof(NativeFilePath); ++i) {
            u8 value = path[i];
            buffer->mBytes[i] = value;
            if (value == 0) break;
        }
        for (; i < sizeof(NativeFilePath); ++i) buffer->mBytes[i] = 0;
    }
    // When the path is absent, the original copies the uninitialized stack tail too.
    copyNativeFilePath(output,buffer.get(),sizeof(NativeFilePath));
}

/* WWHD glue
 data sFileManager 101F8B08
 data sFileTextTable 1017B6E4
 ext 027415B0 u32 queryNativeFileSize(sead::FileHandle*) callback
 ext 0284B694 u32 openNativeFileAlias(const be<u8>*,sead::FileHandle*) callback
 ext 02740B24 sead::FileDevice* sead::FileDevice::tryOpen(sead::FileHandle*,sead::SafeText*,u32,u32)
 ext 02741584 u32 sead::FileHandle::seek(s32,u32)
 ext 027414CC u32 sead::FileHandle::tryRead(be<u32>*,void*,u32)
 ext 02741450 u32 sead::FileHandle::tryClose()
 ext 02740FB0 u32 sead::FileDevice::tryIsExistFile(be<u8>*,sead::SafeText*)
 ext C000A848 void* copyNativeFilePath(void*,const void*,u32)
*/
