/* Wave player lifecycle and asynchronous load state, from WWHD instructions. */
#include "nw/snd/WaveSound.h"
#include "nw/snd/WaveFileReader.h"
#include "gc_api/PowerPC_EABI_Support/Runtime/Inc/CPlusLibPPC.h"
using namespace nw::snd;
static WaveSoundPlayerVtable sWaveSoundPlayerVtable;
static SoundRangeListenerTable sWaveSoundRangeListenerTable;
static FrameListenerVtable sWaveSoundFrameListenerVtable;
// ChannelParameters (ChannelParameters.h) is the same prefix as the player's DriverSoundPlayer + mix block.
static void constructChannelParameters(WaveSoundPlayer*);
static void resetChannelParameters(WaveSoundPlayer*);
static void finishChannelParameters(WaveSoundPlayer*);
static void initializeModulation(ModulatorSettings*);
static Pair32 divideSigned64(u32,u32,u32,u32);
static void setChannelPan(WaveChannel*,u32,f32);
static void pauseVoice(PlaybackVoice*,u32);
static u32 resolveWave(u32,u32,SoundArchive*,ArchiveLoader*);
static SoundListNode* removeSoundListAt(void*,SoundListIterator*);
static void updateAliasTail(WaveSoundPlayer*);
static void dispatchAliasTail(WaveSoundPlayer*);
static void updateTail(WaveSoundPlayer*);
static void stopTail(WaveSoundPlayer*);
static void stopRangeTail(WaveSoundPlayer*,u32,u32);
static void releaseTaskTail(WaveSoundLoadTask*);
static void* allocatePlayer(u32);
static void releasePlayer(void*);
static u32 eventSignalled(nw::ut::Event*);
static SoundMemoryArena* takePlayerHeap(void*);
// The task's heap is the loader's allocator: the arena's vtable implements the allocator interface.
static ArchiveAllocator_l* heapAllocator(SoundMemoryArena* arena) { return gabi::at<ArchiveAllocator_l>(gabi::ea(arena)); }
static void returnPlayerHeap(void*,SoundMemoryArena*);
/* 028A05A0 */
void WaveSoundPlayer::stop() {
    gabi::AutoFrame frame{gabi::current_function()};
    mBase.playFinished=1;
    if (mObserved) { AudioFrameDriver::instance()->removeListener(&mFrameListener.link); mObserved=0; }
    if (mBase.started) mBase.started=0;
}
/* 028A0BA8 */
void WaveSoundPlayer::releaseVoice() {
    gabi::AutoFrame frame{gabi::current_function()};
    WaveChannel* channel=mWaveChannel;
    if (channel) {
        if (channel->started) { updateParameters(); mWaveChannel->beginRelease(); channel=mWaveChannel; }
        if (channel) { channel->clearCallback(); mWaveChannel=nullptr; }
    }
}
/* 028A0C10 */
u32 WaveSoundLoadTask::isPending() {
    gabi::AutoFrame frame{gabi::current_function()};
    return eventSignalled(&finished)^1u;
}
/* 028A0C38 */
void WaveSoundLoadTask::release() {
    SoundMemoryArena* work=mWork;
    if (work) gabi::tail_ptr<void>(gabi::guest_fn((const void*)returnPlayerHeap),(void*)mConfiguration.player,work);
}
/* 028A0C4C */
void WaveSoundLoadTask::releaseAlias() { gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseTaskTail),this); }
/* 028A0CF4 */
void WaveSoundPlayer::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    stop();
    if (mBase.active) { SoundRangeListeners::instance()->remove(&mRangeListener); releaseVoice(); mBase.active=0; }
    if (mInitialized) { finishChannelParameters(this); mInitialized=0; }
    detachLoad();
}
/* 028A0D70 */
void WaveSoundPlayer::destroy(WaveSoundPlayer* object,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object) {
        object->mVtable=&sWaveSoundPlayerVtable; object->mFrameListener.vtable=&sWaveSoundFrameListenerVtable; object->mRangeListener.table=&sWaveSoundRangeListenerTable;
        object->finalize(); object->mBase.event.destroy(2); if (flags&1) releasePlayer(object);
    }
}
/* 028A0DF0 */
void WaveSoundPlayer::stopRange(u32 first,u32 last) {
    if (mBase.active && first<=(u32)mImage && (u32)mImage<=last)
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)stopTail),this);
}
/* 028A0E14 */
void WaveSoundPlayer::stopRangeThunk(SoundRangeLink* link,u32 first,u32 last) {
    WaveSoundPlayer* player=gabi::at<WaveSoundPlayer>(gabi::ea(link)-offsetof(WaveSoundPlayer,mRangeListener));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)stopRangeTail),player,first,last);
}
/* 028A0EB0 */
void WaveSoundLoadTask::initialize() { state=LoadIdle; mSuccess=0; mWaveData=0; mImage=0; }
/* 028A0EC8 */
void WaveSoundLoadTask::configure(const WaveSoundLoadConfiguration* config) {
    gabi::AutoFrame frame{gabi::current_function()};
    initialize();
    mConfiguration.archive=config->archive; mConfiguration.addressCache=config->addressCache;
    mConfiguration.player=config->player; mConfiguration.itemId=config->itemId;
    mConfiguration.image=config->image; u32 waveIndex=config->waveIndex; mWork=nullptr; mConfiguration.waveIndex=waveIndex;
}
/* 028A0F20 */
u32 WaveSoundLoadTask::acquireWork() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mWork) { SoundMemoryArena* work=takePlayerHeap(mConfiguration.player); mWork=work; if (!work) return 0; work->reset(); }
    return 1;
}
/* 028A0F88 */
u32 WaveSoundLoadTask::prepare() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!acquireWork()) return 0;
    u32 current=state;
    if (current==LoadIdle) { AudioWorkQueue::instance()->submit(this,1); return 0; }
    return current>=LoadDone && current<=LoadCancelled;
}
/* 028A1024 */
void WaveSoundPlayer::startSource(const WaveSoundSource* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mBase.active) stop();
    mImage=source->image; mWaveData=source->waveData;
    u32 channel=source->channel; mPrepareState=PrepareReady; mChannel=channel; mBase.active=1;
    SoundRangeListeners::instance()->append(&mRangeListener);
}
/* 028A10A0 */
void WaveSoundPlayer::voiceCallback(u32,u32,WaveSoundPlayer* player) { player->mWaveChannel=nullptr; }
/* 028A17CC */
void WaveSoundPlayer::start() { mBase.started=1; }
/* 028A17D8 */
void WaveSoundPlayer::stopAlias() { gabi::tail_ptr<void>(gabi::guest_fn((const void*)stopTail),this); }
/* 028A17DC */
void WaveSoundPlayer::pause(u32 paused) {
    WaveChannel* channel=mWaveChannel;
    mBase.paused=paused;
    // The channel's first flag byte (WaveChannel::active) holds its pause state.
    if (channel && channel->started && (u32)((u32)channel->active!=0)!=paused) {
        PlaybackVoice* voice=channel->voice; channel->active=paused;
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)pauseVoice),voice,paused);
    }
}
/* 028A1820 */
void WaveSoundPlayer::setPriority(u32 priority) { mPriority=priority; }
/* 028A1828 */
void WaveSoundPlayer::setRemote(u32 remote) { mRemote=remote; }
/* 028A1830 */
MixVolume* waveMixConstruct(MixVolume* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<MixVolume*>(allocatePlayer(sizeof(MixVolume)));
    if (object) { object->frontRight=1.0f; object->lfe=0.0f; object->rearLeft=0.0f; object->center=0.0f; object->frontLeft=1.0f; object->rearRight=0.0f; }
    return object;
}
/* 028A188C */
OutputParam* waveOutputConstruct(OutputParam* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<OutputParam*>(allocatePlayer(sizeof(OutputParam)));
    if (object) __construct_array(object->mixVolume,2,0x18,reinterpret_cast<void*(*)(void*)>(&waveMixConstruct));
    return object;
}
/* 028A0C50 */
void WaveSoundPlayer::detachLoad() {
    gabi::AutoFrame frame{gabi::current_function()};
    WaveSoundLoadTask* task=mLoadTask;
    if (task) {
        ManagedPlaybackPool* pool=mLoadPool;
        gabi::FrameLocal<SoundListIterator> position(8);
        // A task whose load is still running waits in the pool's loading list (the pool's per-frame collection
        // releases it later); a finished one gives its heap back and is available again.
        if (task->isPending()) { position->node=&pool->loading; insertSoundList(&pool->loadingCount,position.get(),&task->mLink); mLoadTask=nullptr; }
        else { task->releaseAlias(); position->node=&pool->available; insertSoundList(&pool->availableCount,position.get(),&task->mLink); mLoadTask=nullptr; }
    }
}
/* 028A0E1C */
u32 WaveSoundPlayer::acquireLoad() {
    gabi::AutoFrame frame{gabi::current_function()};
    ManagedPlaybackPool* pool=mLoadPool;
    WaveSoundLoadTask* task=nullptr;
    if (pool && pool->availableCount) {
        SoundListNode* node=pool->available.next;
        task=gabi::at<WaveSoundLoadTask>(gabi::ea(node)-offsetof(WaveSoundLoadTask,mLink));
        gabi::FrameLocal<SoundListIterator> position(8); position->node=node;
        removeSoundListAt(&pool->availableCount,position.get());
    }
    if (!task) return 0;
    mLoadTask=task; mPrepareState=PrepareLoading; return 1;
}
/* 028A1270 */
void WaveSoundPlayer::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 state=mPrepareState;
    if (state==PrepareDeferred) { if (!acquireLoad()) return; mLoadTask->configure(&mLoadConfiguration); state=PrepareLoading; }
    if (state==PrepareLoading) {
        if (!mLoadTask->prepare()) return;
        WaveSoundLoadTask* task=mLoadTask;
        if (!task->mSuccess) { stop(); return; }
        gabi::FrameLocal<WaveSoundSource> source(8);
        source->waveData=0; source->channel=0; source->image=0;
        source->image=task->mImage; source->waveData=task->mWaveData;
        startSource(source.get());
    }
    if (!mBase.active || !mBase.started) return;
    if (!mBase.paused) {
        if (mVoiceStarted) { if (!mWaveChannel) { stop(); return; } }
        else if (!prepareVoice()) { stop(); return; }
    }
    updateParameters();
}
/* 028A1398 */
void WaveSoundPlayer::updateThunk(nw::ut::HeapLink* link) {
    WaveSoundPlayer* player=gabi::at<WaveSoundPlayer>(gabi::ea(link)-offsetof(WaveSoundPlayer,mFrameListener));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)updateAliasTail),player);
}
/* 028A13A0 */
void WaveSoundPlayer::dispatchThunk(nw::ut::HeapLink* link) {
    WaveSoundPlayer* player=gabi::at<WaveSoundPlayer>(gabi::ea(link)-offsetof(WaveSoundPlayer,mFrameListener));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)dispatchAliasTail),player);
}
/* 028A1500 */
WaveSoundPlayer* WaveSoundPlayer::construct(WaveSoundPlayer* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<WaveSoundPlayer*>(allocatePlayer(sizeof(WaveSoundPlayer)));
    if (object) {
        constructChannelParameters(object);
        object->mFrameListener.link.previous=nullptr; object->mRangeListener.next=nullptr; object->mRangeListener.previous=nullptr; object->mFrameListener.link.next=nullptr;
        object->mRangeListener.table=&sWaveSoundRangeListenerTable; object->mVtable=&sWaveSoundPlayerVtable; object->mFrameListener.vtable=&sWaveSoundFrameListenerVtable;
        initializeModulation(&object->mModulation);
        object->mLoadConfiguration.addressCache=nullptr; object->mLoadConfiguration.itemId=NoFile;
        object->mLoadTask=nullptr; object->mFile.mEnvelope.values[1]=0; object->mLoadConfiguration.image=0;
        object->mFile.mEnvelope.values[3]=0; object->mObserved=0; object->mLoadConfiguration.waveIndex=0;
        object->mFile.mEnvelope.values[2]=0; object->mFile.mEnvelope.values[0]=0; object->mFile.mEnvelope.values[4]=0;
        object->mLoadConfiguration.archive=nullptr; object->mLoadPool=nullptr; object->mLoadConfiguration.player=nullptr; object->mInitialized=0;
    }
    return object;
}
/* 028A15D0 */
void WaveSoundPlayer::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    resetChannelParameters(this);
    mRemote=0; mFile.mPan=0x40; mStart.waveIndex=0xFFFFFFFF; mFile.mSurround=0; mPitchScale=1.0f;
    mPriority=0x40; mWaveData=0; mFile.mPitch=1.0f; mImage=0;
    for (u32 i=0;i<3;i++) mFile.mSend[i]=0;
    mFile.mFilter=0x7F; initializeModulation(&mModulation);
    mObserved=0; mInitialized=1; mVoiceStarted=0; mWaveChannel=nullptr; mPrepareState=PrepareIdle; mChannel=0xFF;
}
/* 028A168C */
void WaveSoundPlayer::prepareDirect(const WaveSoundStart* start,const WaveSoundSource* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mBase.active) stop();
    mStart.waveIndex=start->waveIndex; mStart.offsetUnit=start->offsetUnit; mStart.offset=start->offset;
    mImage=source->image; mWaveData=source->waveData;
    u32 channel=source->channel; mPrepareState=PrepareReady; mChannel=channel; mBase.active=1;
    SoundRangeListeners::instance()->append(&mRangeListener);
    AudioFrameDriver::instance()->addListener(&mFrameListener.link); mObserved=1;
}
/* 028A173C */
void WaveSoundPlayer::prepareDeferred(const WaveSoundStart* start,const WaveSoundLoadConfiguration* config) {
    gabi::AutoFrame frame{gabi::current_function()};
    mStart.waveIndex=start->waveIndex; mStart.offsetUnit=start->offsetUnit; mStart.offset=start->offset;
    mLoadConfiguration.archive=config->archive; mLoadConfiguration.addressCache=config->addressCache;
    mLoadConfiguration.player=config->player; mLoadConfiguration.itemId=config->itemId;
    mLoadConfiguration.image=config->image; u32 waveIndex=config->waveIndex; mPrepareState=PrepareDeferred; mLoadConfiguration.waveIndex=waveIndex;
    AudioFrameDriver::instance()->addListener(&mFrameListener.link); mObserved=1;
}
/* 028A18E4 */
void WaveSoundPlayer::updateAlias() { gabi::tail_ptr<void>(gabi::guest_fn((const void*)updateTail),this); }
/* 028A18E8 */
void WaveSoundPlayer::dispatchUpdate() {
    u32 function=mVtable->update;
    gabi::tail_ptr<void>(function,this);
}
/* 028A10AC */
u32 WaveSoundPlayer::prepareVoice() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<WaveDataReader> reader(0x10);
    u32 priority=(u32)mPriority+0x40u;
    WaveDataReader::create(reader.get(),gabi::at<WaveFileDirectory>(mWaveData),(u32)(s32)(s8)mChannel);
    gabi::FrameLocal<WaveDescription> info(0x20);
    if (!reader->readDescription(info.get(),nullptr)) return 0;
    u32 offset=0;
    u32 unit=mStart.offsetUnit;
    if (unit==OffsetInSamples) { offset=mStart.offset; if (offset>(u32)info->mLoopEnd) return 0; }
    else if (unit==OffsetInMilliseconds) {
        u64 product=(u64)((s64)(s32)mStart.offset*(s64)(s32)info->mSampleRate);
        auto divided=divideSigned64((u32)(product>>32),(u32)product,0,1000);
        offset=divided.r4;
        if (offset>(u32)info->mLoopEnd) return 0;
    }
    s32 channels=info->mChannelCount; if (channels>2) channels=2;
    WaveChannel* channel=WaveChannel::allocate(channels,priority,gabi::guest_fn((const void*)&WaveSoundPlayer::voiceCallback),gabi::ea(this),mVoiceOption);
    if (!channel) return 0;
    gabi::FrameLocal<WaveSoundDataReader> data(8);
    WaveSoundDataReader::construct(data.get(),gabi::at<WaveSoundArchiveDirectory>(mImage));
    if (!data->readParameters(&mFile,mStart.waveIndex)) return 0;
    Envelope* envelope=&channel->envelope;
    envelope->setAttack(mFile.mEnvelope.values[0]); envelope->setHold(mFile.mEnvelope.values[3]);
    envelope->setDecay(mFile.mEnvelope.values[1]); envelope->setSustain(mFile.mEnvelope.values[2]); envelope->setRelease(mFile.mEnvelope.values[4]);
    channel->hold=mRemote; channel->voice->setGamepadSurround(mRemoteFilter);
    channel->start(info.get(),0xFFFFFFFFu,offset); mWaveChannel=channel; mVoiceStarted=1;
    return 1;
}
/* 028A13A8 */
void WaveSoundLoadTask::execute() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SequenceFileCache> cache(8); SequenceFileCache::construct(cache.get()); cache->initialize(mConfiguration.archive);
    u32 image=mConfiguration.image;
    ArchiveFileLoader* loader=static_cast<ArchiveFileLoader*>(&cache->mLoader);
    if (!image) {
        u32 itemId=mConfiguration.itemId;
        if (itemId!=NoFile) {
            if (!loader->loadNamedGroup(itemId,heapAllocator(mWork),2,0)) {
                mWork->mState=2; mSuccess=0; cache->destroy(2); return;
            }
            image=gabi::ea(loader->fileAddressForItem(itemId)); mConfiguration.image=image;
        }
    }
    ArchiveAddressCache* addressCache=mConfiguration.addressCache;
    ArchiveLoader* cached=addressCache ? &addressCache->loader : nullptr;
    u32 wave=resolveWave(image,mConfiguration.waveIndex,mConfiguration.archive,cached);
    if (!wave) {
        if (!loader->loadSoundWave(gabi::at<void>(mConfiguration.image),mConfiguration.waveIndex,heapAllocator(mWork))) {
            mWork->mState=2; mSuccess=0; cache->destroy(2); return;
        }
        wave=resolveWave(mConfiguration.image,mConfiguration.waveIndex,mConfiguration.archive,loader);
    }
    mWaveData=wave; u32 loadedImage=mConfiguration.image; SoundMemoryArena* work=mWork;
    mImage=loadedImage; work->mState=2; cache->finalize(); mSuccess=1; cache->destroy(2);
}
static void waveCopyOutput(void* destination,const OutputParam* source) {
    be<u32>* out=reinterpret_cast<be<u32>*>(destination);
    const be<u32>* in=reinterpret_cast<const be<u32>*>(source);
    for (u32 i=0;i<20;i++) out[i]=in[i];
}
static void waveAddFileOutput(OutputParam* output,f32 pan,f32 surround,f32 main,const be<f32>* sends) {
    output->pan=gabi::fadds_ppc(output->pan,pan);
    output->surroundPan=gabi::fadds_ppc(output->surroundPan,surround);
    output->mainSend=gabi::fadds_ppc(output->mainSend,main);
    for (u32 i=0;i<3;i++) output->fxSend[i]=gabi::fadds_ppc(output->fxSend[i],sends[i]);
}
/* 028A0608 */
void WaveSoundPlayer::updateParameters() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mWaveChannel) return;
    f32 pitch=gabi::fmuls_ppc(mPitch,mFile.mPitch);
    f32 pan=gabi::fmadds((f32)((s32)(u32)mFile.mVolume-64),0.015625f,mPan);
    f32 panAmount=gabi::fdivs_ppc((f32)(u32)mFile.mMode,127.0f);
    u32 panEnabled=mFile.mRelease;
    if ((s8)mBiquadType!=-1) { panEnabled=(u32)(s32)(s8)mBiquadType; panAmount=mBiquadValue; }
    f32 volume=mVolume;
    u32 filePan=mFile.mPan;
    f32 filePanValue=gabi::fmuls_ppc(gabi::fdivs_ppc((f32)((s32)filePan-(filePan<=1?63:64)),63.0f),mPitchScale);
    u32 fileSurround=mFile.mSurround;
    f32 surround=fileSurround>63?gabi::fmuls_ppc((f32)(fileSurround+1u),0.015625f):gabi::fdivs_ppc((f32)fileSurround,63.0f);
    f32 mainSend=gabi::fsubs_ppc(gabi::fdivs_ppc((f32)(u32)mFile.mFilter,127.0f),1.0f);
    gabi::FrameLocal<be<f32>[3]> sends(0x60);
    for (u32 i=0;i<3;i++) (*sends.get())[i]=gabi::fdivs_ppc((f32)(u32)mFile.mSend[i],127.0f);
    gabi::FrameLocal<OutputParam> tv(0x10);
    waveCopyOutput(tv.get(),&mTv);
    waveAddFileOutput(tv.get(),filePanValue,surround,mainSend,*sends.get());
    gabi::FrameLocal<OutputParam[2]> drc(0x6C);
    __construct_array(*drc.get(),2,0x50,reinterpret_cast<void*(*)(void*)>(&waveOutputConstruct));
    for (u32 i=0;i<2;i++) {
        waveCopyOutput(&(*drc.get())[i],&mDrc[i]);
        waveAddFileOutput(&(*drc.get())[i],filePanValue,surround,mainSend,*sends.get());
    }
    // WaveChannel.h names these three fields after their sequence-track use (track, flags, ending).
    mWaveChannel->track=gabi::at<void>(mOutputLine);
    mWaveChannel->flags=mOutputMode;
    mWaveChannel->ending=mRemoteSend;
    mWaveChannel->mix.gain=volume;
    mWaveChannel->mix.pitch=pitch;
    WaveChannel* channel=mWaveChannel;
    ModulatorSettings* modulation=gabi::at<ModulatorSettings>(gabi::ea(&channel->modulators[0]));
    for (u32 i=0;i<4;i++) modulation->words[i]=mModulation.words[i];
    mWaveChannel->mix.pan=pan;
    setChannelPan(mWaveChannel,panEnabled,panAmount);   // WaveChannel::setPan; the f32 stays bit-exact in f1
    mWaveChannel->mix.mode=mOutputOption;
    channel=mWaveChannel; waveCopyOutput(&channel->mix.groups[0],tv.get());
    for (u32 i=0;i<2;i++) { channel=mWaveChannel; waveCopyOutput(&channel->mix.groups[1+i],&(*drc.get())[i]); }
    for (u32 i=0;i<4;i++) {
        channel=mWaveChannel;
        channel->mix.outputs[i].gain=mRemoteOutputs[i].volume;
        channel->mix.outputs[i].pan=mRemoteOutputs[i].filter;
        channel->mix.outputs[i].surroundPan=mRemoteOutputs[i].bypass;
    }
}
/* WWHD glue
data sWaveSoundPlayerVtable 1018558C
data sWaveSoundRangeListenerTable 101855DC
data sWaveSoundFrameListenerVtable 101855F4
ext 028A3610 void constructChannelParameters(nw::snd::WaveSoundPlayer*)
ext 028A36C8 void resetChannelParameters(nw::snd::WaveSoundPlayer*)
ext 028A370C void finishChannelParameters(nw::snd::WaveSoundPlayer*)
ext 028A54F4 void initializeModulation(nw::snd::ModulatorSettings*)
ext 028F5B8C Pair32 divideSigned64(u32,u32,u32,u32)
ext 0289ECC4 void nw::snd::PlaybackVoice::setGamepadSurround(u32)
ext 0289D8BC void pauseVoice(nw::snd::PlaybackVoice*,u32) callback
ext 028B0C5C nw::snd::WaveDataReader* nw::snd::WaveDataReader::create(nw::snd::WaveDataReader*,const nw::snd::WaveFileDirectory*,u32)
ext 028B0D68 u32 nw::snd::WaveDataReader::readDescription(nw::snd::WaveDescription*,const void*) const
ext 028A3DA0 nw::snd::WaveChannel* nw::snd::WaveChannel::allocate(u32,u32,u32,u32,u32)
ext 028A4AA4 void setChannelPan(nw::snd::WaveChannel*,u32,f32)
ext 028A4984 void nw::snd::WaveChannel::start(const nw::snd::WaveDescription*,u32,u32)
ext 028A4A30 void nw::snd::WaveChannel::beginRelease()
ext 028A3EE0 void nw::snd::WaveChannel::clearCallback()
ext 028A5098 void nw::snd::Envelope::setAttack(u32)
ext 028A50B0 void nw::snd::Envelope::setHold(s32)
ext 028A5190 void nw::snd::Envelope::setDecay(s32)
ext 028A51BC void nw::snd::Envelope::setSustain(u32)
ext 028A51C4 void nw::snd::Envelope::setRelease(s32)
ext 028ADAF4 nw::snd::SequenceFileCache* nw::snd::SequenceFileCache::construct(nw::snd::SequenceFileCache*)
ext 028ADB80 void nw::snd::SequenceFileCache::initialize(nw::snd::SoundArchive*)
ext 028AD988 void nw::snd::SequenceFileCache::finalize()
ext 028AD9B0 void nw::snd::SequenceFileCache::destroy(u32)
ext 02892430 u32 nw::snd::ArchiveFileLoader::loadNamedGroup(u32,nw::snd::ArchiveAllocator_l*,u32,s32)
ext 028926BC void* nw::snd::ArchiveFileLoader::fileAddressForItem(u32)
ext 028928B8 u32 nw::snd::ArchiveFileLoader::loadSoundWave(void*,u32,nw::snd::ArchiveAllocator_l*)
ext 0289D018 u32 resolveWave(u32,u32,nw::snd::SoundArchive*,nw::snd::ArchiveLoader*)
ext 0286DA70 nw::snd::SoundListNode* insertSoundList(void*,nw::snd::SoundListIterator*,nw::snd::SoundListNode*)
ext 0286DB18 nw::snd::SoundListNode* removeSoundListAt(void*,nw::snd::SoundListIterator*)
ext 028A18E4 void updateAliasTail(nw::snd::WaveSoundPlayer*) callback
ext 028A18E8 void dispatchAliasTail(nw::snd::WaveSoundPlayer*) callback
ext 028A1270 void updateTail(nw::snd::WaveSoundPlayer*) callback
ext 028A05A0 void stopTail(nw::snd::WaveSoundPlayer*) callback
ext 028A0DF0 void stopRangeTail(nw::snd::WaveSoundPlayer*,u32,u32) callback
ext 028A0C38 void releaseTaskTail(nw::snd::WaveSoundLoadTask*) callback
ext 0273AD10 void* allocatePlayer(u32)
ext 0273AF40 void releasePlayer(void*)
ext 028986B0 nw::snd::AudioFrameDriver* nw::snd::AudioFrameDriver::instance()
ext 02898E74 nw::ut::HeapLink* nw::snd::AudioFrameDriver::addListener(nw::ut::HeapLink*)
ext 02898EA8 nw::ut::HeapLink* nw::snd::AudioFrameDriver::removeListener(nw::ut::HeapLink*)
ext 028A5B00 nw::snd::SoundRangeListeners* nw::snd::SoundRangeListeners::instance()
ext 028A5BD0 nw::snd::SoundRangeLink* nw::snd::SoundRangeListeners::append(nw::snd::SoundRangeLink*)
ext 028A5C00 nw::snd::SoundRangeLink* nw::snd::SoundRangeListeners::remove(nw::snd::SoundRangeLink*)
ext 028F6A10 void nw::ut::Event::destroy(u32)
ext 028F6A30 u32 eventSignalled(nw::ut::Event*)
ext 02897B84 nw::snd::SoundMemoryArena* takePlayerHeap(void*)
ext 02897BE8 void returnPlayerHeap(void*,nw::snd::SoundMemoryArena*) callback
ext 028AD91C void nw::snd::SoundMemoryArena::reset()
ext 0289C1A4 nw::snd::AudioWorkQueue* nw::snd::AudioWorkQueue::instance()
ext 0289C464 void nw::snd::AudioWorkQueue::submit(nw::snd::AudioWork*,u32)
*/
