/* Stream input wrappers and loader state. Own names; offsets recovered from WWHD. */
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/ut/FrameHeap.h"
#include "nw/snd/AudioFileStream.h"
namespace nw { namespace snd {
struct StreamHeader {
    u8 storage[0x44];
};
struct StreamSourceVtable {
    VtableEntry header, destroy, hasExternal, baseHook, beginRead, endRead;
};
/* A bounded view of the source's file: an AudioFileStream copied from the source's reader (028A2844), limited
   to [sliceBegin, sliceBegin + sliceLength). */
struct StreamSlice : AudioFileStream {
    be<u32> sliceBegin;
    be<u32> sliceLength;
    static StreamSlice* create(StreamSlice*, AudioFileStream*, u32, u32);
    s32 read(void*, u32);
    s32 seek(s32, u32);
    u32 relativePosition();
    u32 size();
    u8 canSeek();
    u32 hasBoundedRange();
};
WWHD_OFFSET(StreamSlice, sliceBegin, 0x90);
WWHD_OFFSET(StreamSlice, sliceLength, 0x94);
WWHD_SIZE(StreamSlice, 0x98);
struct StreamSource {
    u8 playerState[0x28C];
    gptr<StreamSourceVtable> vtable;
    StreamHeader header;
    AudioFileStream reader;
    be<u8> open;
    be<u8> loading;
    u8 flagsPadding[2];
    be<u32> pendingReads;
    u8 pathState[0x280];
    gptr<void> file;
    AudioFileWorkContext fileContext;   // the file command blocks and their mutex
    be<u8> active;
    u8 activePadding[3];
    be<u8> readBlockLimit;
    be<u8> channelLimit;
    u8 limitPadding[2];
    static StreamSource* create(StreamSource*);
    void finalize();
    void destroy(u32);
    u32 readHeader();
    u32 openSource(void*, const be<u8>*, u32, u32, u32, u32);
    u32 readDataInfo(void*, u32);
    u32 readSeekInfo(void*, u32);
    void beginRead();
    void endRead();
    static u32 readerWorkSize();
    StreamSlice* createSlice(StreamSlice*, u32, u32, u32);
    BufferedAudioFileStream* createBuffered(BufferedAudioFileStream*, u32, u32, void*, u32);
    u32 hasExternalHandle();
    AudioFileWorkContext* fileHandle();
    void* fileObject();
    u8 maxChannels();
};
WWHD_OFFSET(StreamSource, vtable, 0x28C);
WWHD_OFFSET(StreamSource, header, 0x290);
WWHD_OFFSET(StreamSource, reader, 0x2D4);
WWHD_OFFSET(StreamSource, open, 0x364);
WWHD_OFFSET(StreamSource, pendingReads, 0x368);
WWHD_OFFSET(StreamSource, file, 0x5EC);
WWHD_OFFSET(StreamSource, fileContext, 0x5F0);
WWHD_OFFSET(StreamSource, active, 0x628);
WWHD_OFFSET(StreamSource, readBlockLimit, 0x62C);
WWHD_OFFSET(StreamSource, channelLimit, 0x62D);
WWHD_SIZE(StreamSource, 0x630);
}}

void constructStreamPlayer(nw::snd::StreamSource*);
void cleanupStreamPlayer(nw::snd::StreamSource*);
void destroyStreamPlayer(nw::snd::StreamSource*, u32);
nw::snd::StreamHeader* constructStreamHeader(nw::snd::StreamHeader*);
void OSInitMutex(void*);
void OSLockMutex(void*);
void OSUnlockMutex(void*);
u32 loadStreamHeader(nw::snd::StreamHeader*, void*);
void bindStreamHeader(nw::snd::StreamSource*, nw::snd::StreamHeader*);
u32 streamDataInfoOffset(nw::snd::StreamHeader*);
u32 streamDataInfoSize(nw::snd::StreamHeader*);
u32 streamSeekInfoOffset(nw::snd::StreamHeader*);
u32 streamSeekInfoSize(nw::snd::StreamHeader*);
void bindStreamDataInfo(nw::snd::StreamHeader*, void*);
void bindStreamSeekInfo(nw::snd::StreamHeader*, void*);

void sndDelete(void*);

void prepareStreamCommand(void*, u32);
void FSInitCmdBlock(void*);
s32 FSGetCwd(void*, void*, be<u8>*, u32, u32);
void setStreamWorkingDirectory(nw::snd::StreamSource*, const be<u8>*);

u32 appendStreamPath(be<u8>*, u32, const be<u8>*, u32);
