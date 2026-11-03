/* Audio file stream layouts reconstructed from WWHD instructions. Own names. */
#pragma once
#include "nw/snd/BasicSound.h"
#include "sead/Thread.h"
#include "nw/snd/SoundRuntimeSupport.h"
namespace nw { namespace snd {
struct AudioFileStreamVtable {
    u8 mBeforeClose[0x1C]; be<u32> mClose;
    u8 mBeforeLength[0x6C]; be<u32> mLength;
    u8 mBeforePosition[0x2C]; be<u32> mPosition;
};
/* Shared by the streams of one file client: a pool of Cafe file command blocks and the mutex guarding it. */
struct AudioFileWorkContext { SoundBufferPool mCommands; sead::PlatformMutexStorage mMutex; };
WWHD_OFFSET(AudioFileWorkContext,mMutex,12);
/* One Cafe file-system command block (FSCmdBlock, 0xA80 bytes). */
struct AudioFileCommandBlock { u8 mStorage[0xA80]; };
WWHD_SIZE(AudioFileCommandBlock,0xA80);
enum AudioFileSeekOrigin { SeekBegin = 0, SeekCurrent = 1, SeekEnd = 2 };
struct AudioFileStat { u8 mPrefix[16]; be<u32> mByteLength; u8 mSuffix[0x50]; };
WWHD_SIZE(AudioFileStat,0x64);
WWHD_OFFSET(AudioFileStreamVtable,mClose,0x1C);
WWHD_OFFSET(AudioFileStreamVtable,mLength,0x8C);
WWHD_OFFSET(AudioFileStreamVtable,mPosition,0xBC);
struct AudioFileStream {
    be<u32> mHandleState,mStatus,mReserved;
    gptr<AudioFileStreamVtable> mVtable;
    be<u8> mOpen,mOwnsHandle; u8 mAlignment[2];
    gptr<void> mClient; be<s32> mFileHandle;
    AudioFileStat mStat;
    gptr<AudioFileWorkContext> mContext;
    be<u32> mCommandPriority,mPosition,mLength;
    static AudioFileStream* construct(AudioFileStream*);
    static AudioFileStream* copy(AudioFileStream*,const AudioFileStream*);
    u32 open(void*,const char*,const char*,AudioFileWorkContext*);
    void close(); s32 read(void*,u32); u32 seek(s32,u32);
    u8 isOpen() const; u32 length() const; u8 canRead() const;
    u32 supportsPosition() const; u32 position() const;
};
WWHD_OFFSET(AudioFileStream,mVtable,0xC);
WWHD_OFFSET(AudioFileStream,mOpen,0x10);
WWHD_OFFSET(AudioFileStream,mClient,0x14);
WWHD_OFFSET(AudioFileStream,mStat,0x1C);
WWHD_OFFSET(AudioFileStream,mContext,0x80);
WWHD_OFFSET(AudioFileStream,mCommandPriority,0x84);
WWHD_OFFSET(AudioFileStream,mPosition,0x88);
WWHD_OFFSET(AudioFileStream,mLength,0x8C);
WWHD_SIZE(AudioFileStream,0x90);
struct BufferedAudioFileStream : AudioFileStream {
    gptr<void> mBuffer; be<u32> mBufferSize;
    be<s32> mBufferBegin,mBufferedPosition;
    static BufferedAudioFileStream* construct(BufferedAudioFileStream*);
    void setBuffer(void*,u32); u32 hasBuffer() const;
    u32 canReadBuffered(u32) const; u32 bufferedPosition() const;
    s32 readBuffered(void*,u32); u32 seekBuffered(s32,u32);
    u8 isOpen() const; u32 supportsPosition() const;
};
WWHD_OFFSET(BufferedAudioFileStream,mBuffer,0x90);
WWHD_OFFSET(BufferedAudioFileStream,mBufferBegin,0x98);
WWHD_SIZE(BufferedAudioFileStream,0xA0);
/* The empty helper object the archive player's note-on callback (02895E24, its only user) builds on the stack
   and passes to WaveChannel::playBankInstrument (028A2FE0). */
struct BankNotePlayer {
    u8 mTag;
    static BankNotePlayer* construct(BankNotePlayer*);
    static void destroy(BankNotePlayer*,u32);
};
WWHD_SIZE(BankNotePlayer,1);
}}
