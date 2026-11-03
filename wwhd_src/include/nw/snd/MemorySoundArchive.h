#pragma once
#include "nw/snd/SoundRuntimeSupport.h"
#include "nw/snd/SoundArchive.h"
namespace nw { namespace snd {
struct ArchiveResourceHeader { be<u32> count; };
struct ArchiveResourceReader { gptr<ArchiveResourceHeader> header; u8 state[8]; };
struct ArchiveResourceEntry { be<u32> id; gptr<u8> data; };
/* A sound archive whose whole file is already in memory (the archive facade at +0, its own reader after it). */
struct MemorySoundArchive : SoundArchive {
    gptr<const u8> buffer;
    SoundArchiveReader reader;
    static MemorySoundArchive* create(MemorySoundArchive*);
    void destroy(u32);
    u32 open(const u8*);
    void close();
    u8* itemData(u32);
    SoundMemoryStream* makeStream(SoundMemoryStream*,u32,u32,u32);
};
WWHD_OFFSET(MemorySoundArchive,buffer,0x290);
WWHD_OFFSET(MemorySoundArchive,reader,0x294);
WWHD_SIZE(MemorySoundArchive,0x2D8);
WWHD_SIZE(ArchiveResourceReader,12);
WWHD_SIZE(ArchiveResourceEntry,8);
} }
void memoryArchiveResourceOpen(nw::snd::ArchiveResourceReader*,const u8*);
u32 memoryArchiveResourceEntry(nw::snd::ArchiveResourceReader*,nw::snd::ArchiveResourceEntry*,u32);
void runtimeDelete(void*);
