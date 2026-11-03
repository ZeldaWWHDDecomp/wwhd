#ifndef WWHD_JASRESARCLOADER_H
#define WWHD_JASRESARCLOADER_H
#include "JSystem/JAudio/JASThread.h"
namespace JASystem { namespace ResArcLoader {
struct ArchiveEntry { gptr<void> mData; be<u32> mSize; };
struct Archive { be<u32> mCount; u8 prefix[0x1C]; ArchiveEntry mEntries[1]; };
struct Message { be<u32> mWords[4]; };
struct ResourceRequest {
    gptr<Archive> mArchive;
    be<u16> mId; u8 padding[2];
    gptr<void> mBuffer;
    be<u32> mSize;
    gfn mCallback;
    be<u32> mCallbackArgument;
    gptr<JASMessageQueue> mQueue;
};
struct ResourceLoadLocals {
    ResourceRequest mRequest;
    Message mResult, mMessages;
    JASMessageQueue mQueue;
};
u32 getResSize(Archive*, u32 id);
void loadResourceCallback(ResourceRequest*);
u32 loadResource(Archive*, u32 id, void* buffer, u32 size);
void loadResourceAsync(Archive*, u32 id, void* buffer, u32 size, u32 callback, u32 callbackArgument);
}}
WWHD_OFFSET(JASystem::ResArcLoader::Archive, mEntries, 0x20);
WWHD_SIZE(JASystem::ResArcLoader::ArchiveEntry, 8);
WWHD_SIZE(JASystem::ResArcLoader::ResourceRequest, 0x1C);
WWHD_OFFSET(JASystem::ResArcLoader::ResourceRequest, mQueue, 0x18);
WWHD_SIZE(JASystem::ResArcLoader::Message, 0x10);
WWHD_OFFSET(JASystem::ResArcLoader::ResourceLoadLocals, mResult, 0x1C);
WWHD_OFFSET(JASystem::ResArcLoader::ResourceLoadLocals, mMessages, 0x2C);
WWHD_OFFSET(JASystem::ResArcLoader::ResourceLoadLocals, mQueue, 0x3C);
WWHD_SIZE(JASystem::ResArcLoader::ResourceLoadLocals, 0x78);
#endif
