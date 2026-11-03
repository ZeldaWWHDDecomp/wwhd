// Native audio file adapters; views cover only canonical opaque fields and suffixes.
#pragma once
#include "JSystem/JAudio/JASDvdThread_extra.h"
#include "JSystem/JAudio/JASWorkPool.h"
#include "sead/FileDevice.h"
struct NativeAudioWorkPool : JASystem::Kernel::TWorkPool {
    JASystem::Kernel::WorkPoolBlock* allocate();
};
struct NativeAudioFileHandle {
    be<u32> mHandle;
};
struct NativeAudioFile : JASDvdFile {
    u32 open(const be<u8>* path);
    void close();
    u32 read(void* buffer,u32 size);
    NativeAudioFileHandle* handle() { return reinterpret_cast<NativeAudioFileHandle*>(prefix); }
};
WWHD_SIZE(NativeAudioWorkPool,0x34);
WWHD_SIZE(NativeAudioFileHandle,4);
WWHD_SIZE(NativeAudioFile,0xB4);
void initializeNativeFileRecord(JASystem::Kernel::WorkPoolBlock*);
s32 closeNativeAudioFile(void*,AudioFileRecord*,u32,u32);
s32 readNativeAudioFile(void*,AudioFileRecord*,void*,u32,u32,u32,u32,u32);
void assureNativeAudioText(sead::SafeText*);
void* moveNativeAudioPath(void*,const void*,u32,u32);
s32 openNativeAudioFile(void*,AudioFileRecord*,const be<u8>*,const char*,be<u32>*,u32);
s32 statNativeAudioFile(void*,AudioFileRecord*,u32,void*,u32);
