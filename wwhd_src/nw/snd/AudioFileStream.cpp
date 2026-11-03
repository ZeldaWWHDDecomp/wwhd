/* Audio file stream construction and positioning. Binary-derived; own names. */
#include "nw/snd/AudioFileStream.h"
using namespace nw::snd;
static AudioFileStreamVtable sAudioFileStreamVtable;
static AudioFileStreamVtable sBufferedAudioFileStreamVtable;
static void* audioFileStreamAllocate(u32);
static void audioFileStreamRelease(void*);
static void audioFileStreamMove(void*,const void*,u32);

/* 028A2394 */
BufferedAudioFileStream* BufferedAudioFileStream::construct(BufferedAudioFileStream* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<BufferedAudioFileStream*>(audioFileStreamAllocate(sizeof(BufferedAudioFileStream)));
    if (object) {
        AudioFileStream::construct(object);
        object->mBuffer=nullptr; object->mVtable=&sBufferedAudioFileStreamVtable;
        object->mBufferedPosition=-1; object->mBufferBegin=-1; object->mBufferSize=0;
    }
    return object;
}
/* 028A2400 */
void BufferedAudioFileStream::setBuffer(void* buffer,u32 bytes) {
    mBuffer=buffer; mBufferSize=bytes; mBufferBegin=-1; mBufferedPosition=-1;
}
/* 028A2418 */
u32 BufferedAudioFileStream::hasBuffer() const {
    if (!mBuffer || !mBufferSize) return 0;
    return 1;
}
/* 028A2440 */
u32 BufferedAudioFileStream::canReadBuffered(u32 bytes) const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (hasBuffer()) {
        s32 begin=mBufferBegin;
        if (begin>=0) {
            s32 position=mBufferedPosition;
            if (position>=begin) {
                u32 size=mBufferSize;
                if (static_cast<u32>(begin)+size>=static_cast<u32>(position)+bytes) return 1;
            }
        }
    }
    return 0;
}
/* 028A2784 */
u32 BufferedAudioFileStream::bufferedPosition() const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (hasBuffer()) return static_cast<u32>(mBufferedPosition);
    return mPosition;
}
/* 028A27C8 */
u8 BufferedAudioFileStream::isOpen() const { return mOpen; }
/* 028A27D0 */
u32 BufferedAudioFileStream::supportsPosition() const { return 1; }
/* 028A27D8 */
AudioFileStream* AudioFileStream::construct(AudioFileStream* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<AudioFileStream*>(audioFileStreamAllocate(sizeof(AudioFileStream)));
    if (object) {
        object->mOpen=0; object->mReserved=0; object->mCommandPriority=16;
        object->mVtable=&sAudioFileStreamVtable; object->mClient=nullptr;
        object->mPosition=0; object->mLength=0; object->mContext=nullptr;
        object->mOwnsHandle=0; object->mStatus=0;
    }
    return object;
}
/* 028A2844 */
AudioFileStream* AudioFileStream::copy(AudioFileStream* object,const AudioFileStream* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<AudioFileStream*>(audioFileStreamAllocate(sizeof(AudioFileStream)));
    if (object) {
        object->mReserved=0; object->mVtable=&sAudioFileStreamVtable;
        object->mOwnsHandle=1; object->mOpen=1; object->mStatus=0;
        object->mClient=source->mClient; object->mFileHandle=source->mFileHandle;
        audioFileStreamMove(&object->mStat,&source->mStat,sizeof(object->mStat));
        object->mContext=source->mContext;
        u32 priority=source->mCommandPriority;
        object->mPosition=0;
        object->mCommandPriority=priority;
        object->mLength=source->mLength;
    }
    return object;
}
/* 028A2DB0 */
u8 AudioFileStream::isOpen() const { return mOpen; }
/* 028A2DB8 */
u32 AudioFileStream::length() const { return mLength; }
/* 028A2F88 */
u8 AudioFileStream::canRead() const { return mOpen; }
/* 028A2F90 */
u32 AudioFileStream::supportsPosition() const { return 1; }
/* 028A2F98 */
u32 AudioFileStream::position() const { return mPosition; }
/* 028A2FA0 */
BankNotePlayer* BankNotePlayer::construct(BankNotePlayer* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<BankNotePlayer*>(audioFileStreamAllocate(sizeof(BankNotePlayer)));
    return object;
}
/* 028A2FCC */
void BankNotePlayer::destroy(BankNotePlayer* object,u32 flags) {
    if (object && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)audioFileStreamRelease),object);
}

static void audioFileStreamCopy(void*,const void*,u32);
static void audioFileLock(void*);
static void audioFileUnlock(void*);
static AudioFileCommandBlock* audioFileTakeCommand(SoundBufferPool*);
static void audioFileReturnCommand(SoundBufferPool*,AudioFileCommandBlock*);
static void audioFileClearCommand(AudioFileCommandBlock*,u32);
static void audioFileInitializeCommand(AudioFileCommandBlock*);
static void audioFileCommandPriority(AudioFileCommandBlock*,u32);
static s32 audioFileOpen(void*,AudioFileCommandBlock*,const char*,const char*,be<s32>*,u32);
static s32 audioFileClose(void*,AudioFileCommandBlock*,s32,u32);
static s32 audioFileStat(void*,AudioFileCommandBlock*,s32,AudioFileStat*,u32);
static s32 audioFileRead(void*,AudioFileCommandBlock*,void*,u32,u32,s32,u32,u32);
static s32 audioFileSeek(void*,AudioFileCommandBlock*,s32,u32,u32);

static AudioFileCommandBlock* acquireAudioFileCommand(AudioFileStream* stream) {
    AudioFileWorkContext* context=stream->mContext;
    void* mutex=&context->mMutex;
    audioFileLock(mutex);
    AudioFileCommandBlock* command=audioFileTakeCommand(&context->mCommands);
    if (command) audioFileClearCommand(command,sizeof(AudioFileCommandBlock));
    audioFileInitializeCommand(command);
    audioFileUnlock(mutex);
    audioFileCommandPriority(command,stream->mCommandPriority);
    return command;
}
static void releaseAudioFileCommand(AudioFileStream* stream,AudioFileCommandBlock* command) {
    AudioFileWorkContext* context=stream->mContext;
    void* mutex=&context->mMutex;
    audioFileLock(mutex);
    if (command) audioFileReturnCommand(&context->mCommands,command);
    audioFileUnlock(mutex);
}
/* 028A24B0 */
s32 BufferedAudioFileStream::readBuffered(void* destination,u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!hasBuffer()) return AudioFileStream::read(destination,bytes);
    u32 cached=canReadBuffered(bytes);
    s32 position=mBufferedPosition;
    s32 begin=mBufferBegin;
    if (cached) {
        void* source=gabi::at<void>(gabi::ea(mBuffer)+(u32)position-(u32)begin);
        audioFileStreamCopy(destination,source,bytes);
        mBufferedPosition=(u32)mBufferedPosition+bytes;
        return (s32)bytes;
    }
    if ((u32)position!=(u32)begin) AudioFileStream::seek(position,SeekBegin);
    u32 current=gabi::call_ptr<u32>(mVtable->mPosition,this);
    u32 size=mBufferSize;
    mBufferBegin=current;
    if (bytes<=size) {
        s32 result=AudioFileStream::read(mBuffer,size);
        if (result<0) { mBufferBegin=-1; return result; }
        audioFileStreamCopy(destination,mBuffer,bytes);
    } else {
        s32 result=AudioFileStream::read(destination,bytes);
        if (result<0) { mBufferBegin=-1; return result; }
        u32 size=mBufferSize;
        audioFileStreamCopy(mBuffer,destination,size);
    }
    mBufferedPosition=(u32)mBufferedPosition+bytes;
    return (s32)bytes;
}
/* 028A2688 */
u32 BufferedAudioFileStream::seekBuffered(s32 offset,u32 origin) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!hasBuffer()) return AudioFileStream::seek(offset,origin);
    switch (origin) {
    case SeekBegin: mBufferedPosition=offset; break;
    case SeekCurrent: mBufferedPosition=(u32)mBufferedPosition+(u32)offset; break;
    case SeekEnd: {
        u32 length=gabi::call_ptr<u32>(mVtable->mLength,this);
        mBufferedPosition=length-(u32)offset;
        break;
    }
    default: return 0;
    }
    return 1;
}
/* 028A28F8 */
u32 AudioFileStream::open(void* client,const char* path,const char* mode,AudioFileWorkContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mOpen) gabi::call_ptr<void>(mVtable->mClose,this);
    mContext=context;
    AudioFileCommandBlock* command=acquireAudioFileCommand(this);
    if (audioFileOpen(client,command,path,mode,&mFileHandle,0)!=0) {
        releaseAudioFileCommand(this,command);
        return 0;
    }
    if (audioFileStat(client,command,mFileHandle,&mStat,0)!=0) {
        audioFileClose(client,command,mFileHandle,0);
        releaseAudioFileCommand(this,command);
        return 0;
    }
    releaseAudioFileCommand(this,command);
    mClient=client;
    u32 length=mStat.mByteLength;
    mPosition=0;
    mLength=length;
    mOpen=1;
    return 1;
}
/* 028A2B14 */
void AudioFileStream::close() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mOwnsHandle) {
        AudioFileCommandBlock* command=acquireAudioFileCommand(this);
        audioFileClose(mClient,command,mFileHandle,0);
        releaseAudioFileCommand(this,command);
        if (command) { mClient=nullptr; mPosition=0; mContext=nullptr; mLength=0; }
        else { mPosition=0; mClient=nullptr; mLength=0; mContext=nullptr; }
    }
    mOpen=0;
}
/* 028A2C54 */
s32 AudioFileStream::read(void* destination,u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    AudioFileCommandBlock* command=acquireAudioFileCommand(this);
    s32 result=audioFileRead(mClient,command,destination,1,bytes,mFileHandle,0,0);
    releaseAudioFileCommand(this,command);
    if (result>=0) mPosition=(u32)mPosition+(u32)result;
    return result;
}
/* 028A2DC0 */
u32 AudioFileStream::seek(s32 offset,u32 origin) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 target;
    switch (origin) {
    case SeekBegin: target=(u32)offset; break;
    case SeekCurrent: target=(u32)mPosition+(u32)offset; break;
    case SeekEnd: target=(u32)mLength-(u32)offset; break;
    default: return 0;
    }
    AudioFileCommandBlock* command=acquireAudioFileCommand(this);
    s32 result=audioFileSeek(mClient,command,mFileHandle,target,0);
    releaseAudioFileCommand(this,command);
    if (result!=0) return 0;
    mPosition=target;
    return 1;
}
/* WWHD glue
ext 0273AD10 void* audioFileStreamAllocate(u32)
ext 0273AF40 void audioFileStreamRelease(void*) callback
ext C000A850 void audioFileStreamMove(void*,const void*,u32)
ext C000A848 void audioFileStreamCopy(void*,const void*,u32)
ext C0009E40 void audioFileLock(void*)
ext C000A1C8 void audioFileUnlock(void*)
ext 0288A248 nw::snd::AudioFileCommandBlock* audioFileTakeCommand(nw::snd::SoundBufferPool*)
ext 0288A26C void audioFileReturnCommand(nw::snd::SoundBufferPool*,nw::snd::AudioFileCommandBlock*)
ext 028F521C void audioFileClearCommand(nw::snd::AudioFileCommandBlock*,u32)
ext C0008E40 void audioFileInitializeCommand(nw::snd::AudioFileCommandBlock*)
ext C0008F38 void audioFileCommandPriority(nw::snd::AudioFileCommandBlock*,u32)
ext C0008E98 s32 audioFileOpen(void*,nw::snd::AudioFileCommandBlock*,const char*,const char*,be<s32>*,u32)
ext C0008CF8 s32 audioFileClose(void*,nw::snd::AudioFileCommandBlock*,s32,u32)
ext C0008E00 s32 audioFileStat(void*,nw::snd::AudioFileCommandBlock*,s32,nw::snd::AudioFileStat*,u32)
ext C0008EC8 s32 audioFileRead(void*,nw::snd::AudioFileCommandBlock*,void*,u32,u32,s32,u32,u32)
ext C0008F48 s32 audioFileSeek(void*,nw::snd::AudioFileCommandBlock*,s32,u32,u32)
data sAudioFileStreamVtable 101856EC
data sBufferedAudioFileStreamVtable 1018562C
*/
