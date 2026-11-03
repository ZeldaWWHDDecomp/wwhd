/* Bounded file views used by the sound stream loader. Binary-derived, no external source. */
#include "nw/snd/StreamSource.h"
using namespace nw::snd;
static u32 sSourceVtable;
static u32 sSliceVtable;
static u32 sBufferedVtable;

/* 02887E60 */
StreamSource* StreamSource::create(StreamSource* source) {
    gabi::AutoFrame frame{0x02887E60};
    if (!source) {
        source = (StreamSource*)sndAllocate(sizeof(StreamSource));
        if (!source) return source;
    }
    constructStreamPlayer(source);
    source->vtable = (StreamSourceVtable*)&sSourceVtable;
    constructStreamHeader(&source->header);
    AudioFileStream::construct(&source->reader);
    source->fileContext.mCommands.length = 0;
    source->active = 1;
    source->fileContext.mCommands.buffer = nullptr;
    source->fileContext.mCommands.freeHead = nullptr;
    source->file = nullptr;
    source->pendingReads = 0;
    source->loading = 0;
    source->open = 0;
    OSInitMutex(&source->fileContext.mMutex);
    source->readBlockLimit = 16;
    source->channelLimit = 8;
    return source;
}

/* 02887F04 */
void StreamSource::finalize() {
    gabi::AutoFrame frame{0x02887F04};
    gabi::call_ptr<void>(vtable->beginRead.function, this);
    if (open) {
        reader.close();
        file = nullptr;
        OSLockMutex(&fileContext.mMutex);
        fileContext.mCommands.removeBuffer();
        OSUnlockMutex(&fileContext.mMutex);
        open = 0;
    }
    cleanupStreamPlayer(this);
}

/* 02887F9C */
void StreamSource::destroy(u32 flags) {
    gabi::AutoFrame frame{0x02887F9C};
    if (!this) return;
    vtable = (StreamSourceVtable*)&sSourceVtable;
    finalize();
    active = 0;
    destroyStreamPlayer(this, 0);
    if (flags & 1) sndDelete(this);
}

/* 02888008 */
u32 StreamSource::readerWorkSize() {
    gabi::AutoFrame frame{0x02888008};
    return 0x1500;
}

struct HeaderScratch {
    u8 bytes[0x238];
};

/* 02888010 */
u32 StreamSource::readHeader() {
    gabi::AutoFrame frame{0x02888010};
    gabi::FrameLocal<HeaderScratch> scratch(8);
    void* buffer = gabi::at<void>((gabi::native_sp() + 0x107) & ~255u);
    reader.mCommandPriority = (u8)readBlockLimit;
    if (reader.read(buffer, 256) != 256) return 0;
    loadStreamHeader(&header, buffer);
    bindStreamHeader(this, &header);
    return 1;
}

struct PathBuffer {
    be<u8> bytes[0x280];
};

/* 028880A0 */
u32 StreamSource::openSource(void* source, const be<u8>* path, u32 handleData, u32 handleSize, u32 blocks,
                             u32 channels) {
    gabi::AutoFrame frame{0x028880A0};
    OSLockMutex(&fileContext.mMutex);
    fileContext.mCommands.addBuffer(gabi::at<void>(handleData), handleSize, 0xA80);
    OSUnlockMutex(&fileContext.mMutex);
    file = source;
    readBlockLimit = (u8)blocks;
    channelLimit = (u8)channels;
    reader.mCommandPriority = (u8)blocks;
    if (!reader.open(source, reinterpret_cast<const char*>(path), "r", &fileContext)) return 0;
    u32 count = pendingReads;
    open = 1;
    pendingReads = count + 1;
    if (!readHeader()) return 0;
    gabi::call_ptr<void>(vtable->endRead.function, this);
    u8 first = path[0];
    bool absolute = first == '/' || first == '\\';
    gabi::FrameLocal<PathBuffer> directory(0x288);
    gabi::FrameLocal<PathBuffer> cwd(8);
    directory->bytes[0] = 0;
    u32 length = 0;
    while (path[length]) ++length;
    if ((s32)length >= 0x27F) return 0;
    length = 0;
    while (path[length]) ++length;
    s32 slash = (s32)length - 1;
    while (slash >= 0) {
        u8 byte = path[(u32)slash];
        if (byte == '/' || byte == '\\') break;
        --slash;
    }
    if (slash >= 0) {
        u32 copied = 0;
        u32 limit = (u32)slash > 0x27E ? 0x27E : (u32)slash;
        while (copied < limit) {
            u8 byte = path[copied];
            if (!byte) break;
            directory->bytes[copied] = byte;
            ++copied;
        }
        directory->bytes[copied] = 0;
        directory->bytes[(u32)slash] = 0;
    }
    OSLockMutex(&fileContext.mMutex);
    void* command = fileContext.mCommands.allocate();
    if (command) prepareStreamCommand(command, 0xA80);
    FSInitCmdBlock(command);
    OSUnlockMutex(&fileContext.mMutex);
    if (absolute) {
        setStreamWorkingDirectory(this, directory->bytes);
        void* fileObject = file;
        if (FSGetCwd(fileObject, command, (be<u8>*)pathState, 0x27F, 0)) {
            OSLockMutex(&fileContext.mMutex);
            if (command) fileContext.mCommands.release(static_cast<SoundPoolLink*>(command));
            OSUnlockMutex(&fileContext.mMutex);
            return 0;
        }
    } else {
        void* fileObject = file;
        if (FSGetCwd(fileObject, command, cwd->bytes, 0x27F, 0)) {
            OSLockMutex(&fileContext.mMutex);
            if (command) fileContext.mCommands.release(static_cast<SoundPoolLink*>(command));
            OSUnlockMutex(&fileContext.mMutex);
            return 0;
        }
        u32 directoryLength = 0;
        while (directory->bytes[directoryLength]) ++directoryLength;
        appendStreamPath(cwd->bytes, 0x27F, directory->bytes, directoryLength);
        setStreamWorkingDirectory(this, cwd->bytes);
        fileObject = file;
        if (FSGetCwd(fileObject, command, (be<u8>*)pathState, 0x27F, 0)) {
            OSLockMutex(&fileContext.mMutex);
            if (command) fileContext.mCommands.release(static_cast<SoundPoolLink*>(command));
            OSUnlockMutex(&fileContext.mMutex);
            return 0;
        }
    }
    length = 0;
    while (path[length]) ++length;
    appendStreamPath((be<u8>*)pathState, 0x27F, path, length);
    OSLockMutex(&fileContext.mMutex);
    if (command) fileContext.mCommands.release(static_cast<SoundPoolLink*>(command));
    OSUnlockMutex(&fileContext.mMutex);
    return 1;
}

/* 0288843C */
u32 StreamSource::readDataInfo(void* destination, u32 capacity) {
    gabi::AutoFrame frame{0x0288843C};
    u32 offset = streamDataInfoOffset(&header);
    u32 bytes = streamDataInfoSize(&header);
    if (capacity < bytes) return 0;
    gabi::call_ptr<void>(vtable->beginRead.function, this);
    reader.seek((s32)offset, 0);
    if ((u32)reader.read(destination, bytes) != bytes) return 0;
    gabi::call_ptr<void>(vtable->endRead.function, this);
    bindStreamDataInfo(&header, destination);
    return 1;
}

/* 02888528 */
u32 StreamSource::readSeekInfo(void* destination, u32 capacity) {
    gabi::AutoFrame frame{0x02888528};
    u32 offset = streamSeekInfoOffset(&header);
    u32 bytes = streamSeekInfoSize(&header);
    if (offset == 0xFFFFFFFF || capacity < bytes) return 0;
    gabi::call_ptr<void>(vtable->beginRead.function, this);
    reader.seek((s32)offset, 0);
    if ((u32)reader.read(destination, bytes) != bytes) return 0;
    gabi::call_ptr<void>(vtable->endRead.function, this);
    bindStreamSeekInfo(&header, destination);
    return 1;
}

/* 028885F8 */
void StreamSource::beginRead() {
    gabi::AutoFrame frame{0x028885F8};
    if (loading != 1) return;
    u32 count = pendingReads;
    if (!count) {
        reader.mCommandPriority = (u8)readBlockLimit;
        void* source = file;
        reader.open(source, reinterpret_cast<const char*>(pathState), "r", &fileContext);
        count = pendingReads;
    }
    pendingReads = count + 1;
}

/* 02888668 */
void StreamSource::endRead() {
    gabi::AutoFrame frame{0x02888668};
    if (loading != 1) return;
    u32 count = pendingReads;
    if (count == 1) {
        reader.close();
        count = pendingReads;
    }
    if (count) pendingReads = count - 1;
}

/* 028886C4 */
StreamSlice* StreamSlice::create(StreamSlice* stream, AudioFileStream* source, u32 offset, u32 size) {
    gabi::AutoFrame frame{0x028886C4};
    if (!stream) {
        stream = (StreamSlice*)sndAllocate(sizeof(StreamSlice));
        if (!stream) return stream;
    }
    AudioFileStream::copy(stream, source);
    stream->sliceLength = size;
    stream->sliceBegin = offset;
    stream->mVtable = reinterpret_cast<AudioFileStreamVtable*>(&sSliceVtable);
    if (!size) stream->sliceLength = stream->length();
    s32 position = (u32)stream->sliceBegin;
    stream->AudioFileStream::seek(position, 0);
    return stream;
}

/* 0288876C */
StreamSlice* StreamSource::createSlice(StreamSlice* storage, u32 size, u32 offset, u32 length) {
    gabi::AutoFrame frame{0x0288876C};
    if (!open || size < sizeof(StreamSlice)) return nullptr;
    if (storage) return StreamSlice::create(storage, &reader, offset, length);
    return storage;
}

/* 028887D0 */
/* This TU's buffered stream on the source's file (the BufferedAudioFileStream with the source's vtable). */
static BufferedAudioFileStream* createSourceBufferedStream(BufferedAudioFileStream* stream, void* file, u32 path, AudioFileWorkContext* context) {
    gabi::AutoFrame frame{0x028887D0};
    if (!stream) {
        stream = (BufferedAudioFileStream*)sndAllocate(sizeof(BufferedAudioFileStream));
        if (!stream) return stream;
    }
    BufferedAudioFileStream::construct(stream);
    stream->mVtable = reinterpret_cast<AudioFileStreamVtable*>(&sBufferedVtable);
    stream->open(file, gabi::at<const char>(path), "r", context);
    return stream;
}

struct StreamVtable {
    u8 prefix[0x6C];
    be<u32> isOpen;
};

/* 02888864 */
BufferedAudioFileStream* StreamSource::createBuffered(BufferedAudioFileStream* storage, u32 size, u32 path, void* buffer, u32 capacity) {
    gabi::AutoFrame frame{0x02888864};
    if (!open || size < sizeof(BufferedAudioFileStream)) return nullptr;
    BufferedAudioFileStream* stream = nullptr;
    if (storage) {
        void* source = file;
        stream = createSourceBufferedStream(storage, source, path, &fileContext);
        if (stream) {
            StreamVtable* table = (StreamVtable*)stream->mVtable.get();
            if (!gabi::call_ptr<u32>(table->isOpen, stream)) return nullptr;
        }
    }
    if (stream && buffer && capacity) stream->setBuffer(buffer, capacity);
    return stream;
}

/* 02888944 */
s32 StreamSlice::read(void* destination, u32 bytes) {
    gabi::AutoFrame frame{0x02888944};
    u32 current = mPosition;
    u32 start = sliceBegin;
    u32 end = start + (u32)sliceLength;
    if (current + bytes > end) bytes = (end - current + 31) & ~31u;
    return AudioFileStream::read(destination, bytes);
}

/* 02888970 */
s32 StreamSlice::seek(s32 displacement, u32 origin) {
    gabi::AutoFrame frame{0x02888970};
    u32 start;
    s32 target;
    switch (origin) {
    case 0:
        start = sliceBegin;
        target = (s32)((u32)displacement + start);
        break;
    case 1: {
        u32 current = mPosition;
        start = sliceBegin;
        target = (s32)((u32)displacement + current);
        break;
    }
    case 2:
        start = sliceBegin;
        target = (s32)(start + (u32)sliceLength - (u32)displacement);
        break;
    default: return 0;
    }
    if (target < (s32)start) return (s32)AudioFileStream::seek((s32)start, 0);
    s32 end = (s32)(start + (u32)sliceLength);
    if (target > end) target = end;
    return (s32)AudioFileStream::seek(target, 0);
}

/* 028889F8 */
u32 StreamSource::hasExternalHandle() {
    gabi::AutoFrame frame{0x028889F8};
    return 0;
}

/* 02888A00 */
AudioFileWorkContext* StreamSource::fileHandle() {
    gabi::AutoFrame frame{0x02888A00};
    return &fileContext;
}

/* 02888A08 */
void* StreamSource::fileObject() {
    gabi::AutoFrame frame{0x02888A08};
    return file;
}

/* 02888A10 */
u8 StreamSource::maxChannels() {
    gabi::AutoFrame frame{0x02888A10};
    return channelLimit;
}

/* 02888A18 */
u32 StreamSlice::relativePosition() {
    gabi::AutoFrame frame{0x02888A18};
    u32 current = mPosition;
    return current - (u32)sliceBegin;
}

/* 02888A28 */
u32 StreamSlice::size() {
    gabi::AutoFrame frame{0x02888A28};
    return sliceLength;
}

/* 02888A30 */
u8 StreamSlice::canSeek() {
    gabi::AutoFrame frame{0x02888A30};
    return mOpen;   // AudioFileStream's open flag (+0x10)
}

/* 02888A38 */
u32 StreamSlice::hasBoundedRange() {
    gabi::AutoFrame frame{0x02888A38};
    return 1;
}

/* WWHD glue
data sSourceVtable 10181C8C
data sSliceVtable 10181CEC
data sBufferedVtable 10181BC4
str 10181C88 "r" in 028887D0
str 10181C86 "r" in 028885F8
str 10181C84 "r" in 028880A0
ext 0288E878 void constructStreamPlayer(nw::snd::StreamSource*)
ext 0288F33C void cleanupStreamPlayer(nw::snd::StreamSource*)
ext 0288E8D0 void destroyStreamPlayer(nw::snd::StreamSource*,u32)
ext 02890208 nw::snd::StreamHeader* constructStreamHeader(nw::snd::StreamHeader*)
ext 028A27D8 nw::snd::AudioFileStream* nw::snd::AudioFileStream::construct(nw::snd::AudioFileStream*)
ext 028A2B14 void nw::snd::AudioFileStream::close()
ext C0009D68 void OSInitMutex(void*)
ext C0009E40 void OSLockMutex(void*)
ext C000A1C8 void OSUnlockMutex(void*)
ext 0288A1D0 void nw::snd::SoundBufferPool::removeBuffer()
ext 0289028C u32 loadStreamHeader(nw::snd::StreamHeader*,void*)
ext 0288F304 void bindStreamHeader(nw::snd::StreamSource*,nw::snd::StreamHeader*)
ext 0288F458 u32 streamDataInfoOffset(nw::snd::StreamHeader*)
ext 0288F408 u32 streamDataInfoSize(nw::snd::StreamHeader*)
ext 0288F430 u32 streamSeekInfoOffset(nw::snd::StreamHeader*)
ext 0288F3E0 u32 streamSeekInfoSize(nw::snd::StreamHeader*)
ext 02890338 void bindStreamDataInfo(nw::snd::StreamHeader*,void*)
ext 0289032C void bindStreamSeekInfo(nw::snd::StreamHeader*,void*)
ext 0288A184 u32 nw::snd::SoundBufferPool::addBuffer(void*,u32,u32)
ext 0288A248 nw::snd::SoundPoolLink* nw::snd::SoundBufferPool::allocate()
ext 0288A26C void nw::snd::SoundBufferPool::release(nw::snd::SoundPoolLink*)
ext 028F521C void prepareStreamCommand(void*,u32)
ext C0008E40 void FSInitCmdBlock(void*)
ext C0008D58 s32 FSGetCwd(void*,void*,be<u8>*,u32,u32)
ext 0288F190 void setStreamWorkingDirectory(nw::snd::StreamSource*,const be<u8>*)
ext 028F6930 u32 appendStreamPath(be<u8>*,u32,const be<u8>*,u32)
ext 028A2844 nw::snd::AudioFileStream* nw::snd::AudioFileStream::copy(nw::snd::AudioFileStream*,const nw::snd::AudioFileStream*)
ext 028A2DB8 u32 nw::snd::AudioFileStream::length() const
ext 028A2C54 s32 nw::snd::AudioFileStream::read(void*,u32)
ext 028A2DC0 u32 nw::snd::AudioFileStream::seek(s32,u32)
ext 028A2394 nw::snd::BufferedAudioFileStream* nw::snd::BufferedAudioFileStream::construct(nw::snd::BufferedAudioFileStream*)
ext 028A28F8 u32 nw::snd::AudioFileStream::open(void*,const char*,const char*,nw::snd::AudioFileWorkContext*)
ext 028A2400 void nw::snd::BufferedAudioFileStream::setBuffer(void*,u32)
*/
