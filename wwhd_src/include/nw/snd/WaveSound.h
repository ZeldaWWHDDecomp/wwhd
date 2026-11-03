/* Wave sound instance and its driver-side player, recovered from WWHD accesses. Own names. */
#pragma once
#include "nw/snd/BasicSound.h"
#include "nw/snd/ArchivePlayback.h"
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/AudioWorkQueue.h"
#include "nw/snd/SequenceFileCache.h"
#include "nw/snd/SoundMemoryArena.h"
#include "nw/snd/SoundRangeListeners.h"
#include "nw/snd/WaveChannel.h"
#include "nw/snd/WaveFileReader.h"
namespace nw { namespace snd {
/* The parameter half (first 16 bytes) of a Modulator: depth, frequency, delay and the
   range/shape/phase bytes. The player keeps one and copies it word by word into the channel. */
struct ModulatorSettings { be<u32> words[4]; };
WWHD_SIZE(ModulatorSettings,0x10);
/* What a deferred load needs; the player keeps a copy and hands it to its load task. */
struct WaveSoundLoadConfiguration {
    gptr<SoundArchive> archive;
    gptr<ArchiveAddressCache> addressCache;  // the archive player's file-address cache (its only producer, 02894D84)
    gptr<void> player;                   // sound player whose heap the task borrows
    be<u32> itemId;                      // the sound whose wave sound file to load (ArchiveFileLoader item id), or NoFile
    be<u32> image;                       // the loaded file, once known
    be<u32> waveIndex;
};
WWHD_SIZE(WaveSoundLoadConfiguration,0x18);
enum : u32 { NoFile = 0xFFFFFFFF };
/* Where a prepared sound starts: wave index in the file, offset unit and offset. */
struct WaveSoundStart { be<u32> waveIndex,offsetUnit,offset; };
enum WaveSoundOffsetUnit { OffsetInSamples = 0, OffsetInMilliseconds = 1 };
/* A loaded wave sound: file image, wave data and the channel to play it on. */
struct WaveSoundSource { be<u32> image,waveData; be<u8> channel; u8 padding[3]; };
WWHD_SIZE(WaveSoundSource,12);
enum WaveSoundPrepareState { PrepareIdle, PrepareDeferred, PrepareLoading, PrepareReady };
struct WaveSoundPlayerVtable { u8 prefix[0x28]; be<u32> updateAdjustment,update; };
WWHD_OFFSET(WaveSoundPlayerVtable,update,0x2C);
struct WaveSoundLoadTask;
struct WaveSoundPlayer {
    DriverSoundPlayer mBase; u8 mBaseAlignment[3];
    be<f32> mVolume,mPitch,mPan,mBiquadValue;
    be<s8> mBiquadType; be<u8> mRemoteFilter,mRemoteSend,mReserved3F;
    be<u32> mOutputLine,mOutputMode,mOutputOption,mVoiceOption;
    OutputParam mTv,mDrc[2]; RemoteOutputParam mRemoteOutputs[4]; be<u32> mReserved170;
    gptr<WaveSoundPlayerVtable> mVtable;
    SoundRangeListener mRangeListener;   // stops the sound when its memory range is invalidated
    FrameListener mFrameListener;        // per-frame update from the audio frame driver
    be<u8> mVoiceStarted,mRemote,mPriority,mChannel;
    be<f32> mPitchScale; be<u32> mImage,mWaveData;
    WaveSoundStart mStart;
    ModulatorSettings mModulation;
    WaveSoundParameters mFile;           // per-sound parameters read from the wave sound file
    u8 mFileAlignment[2];
    gptr<WaveChannel> mWaveChannel;
    be<u8> mPrepareState,mInitialized,mObserved,mReservedD7;
    gptr<ManagedPlaybackPool> mLoadPool; gptr<WaveSoundLoadTask> mLoadTask;
    WaveSoundLoadConfiguration mLoadConfiguration;
    static WaveSoundPlayer* construct(WaveSoundPlayer*);
    void updateParameters(); u32 prepareVoice();
    void initialize(); void detachLoad(); u32 acquireLoad(); void update(); void updateAlias(); void dispatchUpdate();
    static void updateThunk(nw::ut::HeapLink*); static void dispatchThunk(nw::ut::HeapLink*);
    void prepareDirect(const WaveSoundStart*,const WaveSoundSource*);
    void prepareDeferred(const WaveSoundStart*,const WaveSoundLoadConfiguration*);
    void stop(); void stopAlias(); void releaseVoice(); void finalize(); static void destroy(WaveSoundPlayer*,u32);
    void stopRange(u32,u32); static void stopRangeThunk(SoundRangeLink*,u32,u32);
    void startSource(const WaveSoundSource*);
    static void voiceCallback(u32,u32,WaveSoundPlayer*);
    void start(); void pause(u32); void setPriority(u32); void setRemote(u32);
};
WWHD_OFFSET(WaveSoundPlayer,mOutputLine,0x40); WWHD_OFFSET(WaveSoundPlayer,mTv,0x50);
WWHD_OFFSET(WaveSoundPlayer,mRemoteOutputs,0x140); WWHD_OFFSET(WaveSoundPlayer,mVtable,0x174);
WWHD_OFFSET(WaveSoundPlayer,mRangeListener,0x178); WWHD_OFFSET(WaveSoundPlayer,mFrameListener,0x184);
WWHD_OFFSET(WaveSoundPlayer,mVoiceStarted,0x190); WWHD_OFFSET(WaveSoundPlayer,mStart,0x1A0);
WWHD_OFFSET(WaveSoundPlayer,mModulation,0x1AC); WWHD_OFFSET(WaveSoundPlayer,mFile,0x1BC);
WWHD_OFFSET(WaveSoundPlayer,mWaveChannel,0x1D0); WWHD_OFFSET(WaveSoundPlayer,mPrepareState,0x1D4);
WWHD_OFFSET(WaveSoundPlayer,mLoadPool,0x1D8); WWHD_OFFSET(WaveSoundPlayer,mLoadConfiguration,0x1E0);
WWHD_SIZE(WaveSoundPlayer,0x1F8);
/* Asynchronous load of a wave sound file, run by the audio work queue. */
struct WaveSoundLoadTask : AudioWork {
    WaveSoundLoadConfiguration mConfiguration;
    be<u32> mImage,mWaveData; gptr<SoundMemoryArena> mWork; be<u8> mSuccess; u8 mAlignment[3];
    SoundListNode mLink;
    u32 isPending(); void release(); void releaseAlias(); void initialize(); void configure(const WaveSoundLoadConfiguration*);
    u32 acquireWork(); u32 prepare(); void execute();
};
enum WaveSoundLoadState { LoadIdle, LoadQueued, LoadRunning, LoadDone, LoadCancelled };
WWHD_OFFSET(WaveSoundLoadTask,mConfiguration,0x38); WWHD_OFFSET(WaveSoundLoadTask,mWork,0x58);
WWHD_OFFSET(WaveSoundLoadTask,mLink,0x60); WWHD_SIZE(WaveSoundLoadTask,0x68);
/* Game-side description of a deferred load (sendConfiguration turns it into a WaveSoundLoadConfiguration). */
struct WaveSoundFileReference { be<u32> itemId,image; };   // the sound's item id and its file, once loaded
struct WaveSoundLoadRequest {
    gptr<SoundArchive> archive; gptr<ArchiveAddressCache> addressCache;
    gptr<const WaveSoundFileReference> file; gptr<void> player;
};
enum WaveSoundCommandId { WaveSoundCommand_Setup=0x2D, WaveSoundCommand_Configuration=0x2E, WaveSoundCommand_Priority=0x30 };
/* The driver commands WaveSound sends (handled by DriverCommandDispatch). */
struct WaveSetupCommand : PlayerCommand { WaveSoundStart start; WaveSoundSource source; };
struct WaveConfigurationCommand : PlayerCommand { WaveSoundStart start; WaveSoundLoadConfiguration configuration; };
WWHD_SIZE(WaveSetupCommand,0x2C); WWHD_SIZE(WaveConfigurationCommand,0x38);
inline WaveSoundPlayer* commandWavePlayer(DriverSoundPlayer* player) { return gabi::at<WaveSoundPlayer>(gabi::ea(player)); }
struct WaveSound {
    BasicSound mBasic; gptr<void> mHandle; gptr<PlaybackPool> mPool;
    be<u32> mWaveData; be<u8> mChannel,mActive; u8 mAlignment[2]; WaveSoundPlayer mPlayer;
    static WaveSound* construct(WaveSound*,PlaybackPool*);
    void sendSetup(u32,u32,const WaveSoundStart*,u32);   // image, wave data, start, channel
    void sendConfiguration(const WaveSoundLoadRequest*,const WaveSoundStart*);
    void initialize(); void finalize(); void sendPriority(u32,u32);
    u32 hasHandle() const; void updatePriority(); u32 isPrepared() const;
    void updateParameters(); void detachHandle(); WaveSoundPlayer* driverPlayer(); static void destroy(WaveSound*,u32);
};
WWHD_OFFSET(WaveSound,mPool,0x29C); WWHD_OFFSET(WaveSound,mPlayer,0x2A8); WWHD_SIZE(WaveSound,0x4A0);
}}
