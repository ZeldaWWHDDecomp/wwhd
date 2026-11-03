/* Game-thread wave sound command and list operations, from WWHD. */
#include "nw/snd/WaveSound.h"
using namespace nw::snd;
static BasicSoundVtable sWaveSoundVtable;
static BasicSoundVtable sWaveBaseSoundVtable;
static void* waveSoundAllocate(u32);
static void waveSoundRelease(void*);
/* Payloads of the two prepare commands; the driver hands &start and the rest to the player. */
static SoundListNode* poolNode(BasicSound* sound) { return gabi::at<SoundListNode>(gabi::ea(&sound->poolLink)); }
/* 0289FE28 */
WaveSound* WaveSound::construct(WaveSound* object,PlaybackPool* pool) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) object=static_cast<WaveSound*>(waveSoundAllocate(sizeof(WaveSound)));
    if (object) {
        BasicSound::create(&object->mBasic); object->mPool=pool; object->mActive=0;
        object->mBasic.vtable=&sWaveSoundVtable; WaveSoundPlayer::construct(&object->mPlayer);
    }
    return object;
}
/* 0289FE9C */
void WaveSound::sendSetup(u32 image,u32 waveData,const WaveSoundStart* start,u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=DriverCommandManager::instance();
    WaveSetupCommand* command=static_cast<WaveSetupCommand*>(manager->allocCommand(0x2C));
    command->id=WaveSoundCommand_Setup; command->player=reinterpret_cast<DriverSoundPlayer*>(&mPlayer);
    command->start.waveIndex=start->waveIndex; command->start.offsetUnit=start->offsetUnit; command->start.offset=start->offset;
    command->source.image=image; command->source.waveData=waveData; command->source.channel=channel;
    manager->pushCommand(command); mChannel=channel; mWaveData=waveData;
}
/* 0289FF2C */
void WaveSound::sendConfiguration(const WaveSoundLoadRequest* request,const WaveSoundStart* start) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=DriverCommandManager::instance();
    WaveConfigurationCommand* command=static_cast<WaveConfigurationCommand*>(manager->allocCommand(0x38));
    command->id=WaveSoundCommand_Configuration; command->player=reinterpret_cast<DriverSoundPlayer*>(&mPlayer);
    command->start.waveIndex=start->waveIndex; command->start.offsetUnit=start->offsetUnit; command->start.offset=start->offset;
    WaveSoundLoadConfiguration* configuration=&command->configuration;
    configuration->addressCache=request->addressCache; configuration->archive=request->archive; configuration->player=request->player;
    const WaveSoundFileReference* file=request->file;
    configuration->itemId=file->itemId; configuration->image=file->image;
    configuration->waveIndex=start->waveIndex; manager->pushCommand(command);
}
/* 0289FFEC */
void WaveSound::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mBasic.initialize(); mWaveData=0; mHandle=nullptr; mChannel=0xFF; mActive=1;
}
/* 028A0034 */
void WaveSound::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mActive) return;
    mActive=0; mBasic.finalize(); PlaybackPool* pool=mPool;
    removeSoundList(&pool->activeCount,poolNode(&mBasic));
    u32 function=mBasic.vtable->finalize.function;
    gabi::call_ptr<void>(function,this);
    gabi::FrameLocal<SoundListIterator> position(8); position->node=&pool->free;
    insertSoundList(&pool->freeCount,position.get(),poolNode(&mBasic));
}
/* 028A00BC */
void WaveSound::sendPriority(u32 first,u32 second) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager=DriverCommandManager::instance();
    PlayerFlagCommand* command=static_cast<PlayerFlagCommand*>(manager->allocCommand(0x18));
    command->secondFlag=second; command->flag=first; command->id=WaveSoundCommand_Priority;
    command->player=reinterpret_cast<DriverSoundPlayer*>(&mPlayer); manager->pushCommand(command);
}
/* 028A0138 */
u32 WaveSound::hasHandle() const { return mHandle!=nullptr; }
static s32 waveSoundPriority(const BasicSound* sound) {
    s32 priority=(s32)((u32)(s32)sound->ambient.priority+(u32)(u8)(s8)sound->playerPriority);
    if (priority>127) return 127;
    return priority<0?0:priority;
}
/* 028A0148 */
void WaveSound::updatePriority() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 priority=waveSoundPriority(&mBasic); PlaybackPool* pool=mPool;
    removeSoundList(&pool->activeCount,poolNode(&mBasic));
    // Keep the active list sorted by priority: insert before the first louder sound.
    SoundListNode* node=pool->active.next;
    SoundListNode* end=&pool->active;
    while (node!=end) {
        BasicSound* sound=gabi::at<BasicSound>(gabi::ea(node)-offsetof(BasicSound,poolLink));
        if (priority<waveSoundPriority(sound)) break;
        node=node->next;
    }
    gabi::FrameLocal<SoundListIterator> position(8); position->node=node;
    insertSoundList(&pool->activeCount,position.get(),poolNode(&mBasic));
}
/* 028A0218 */
u32 WaveSound::isPrepared() const { if (!mBasic.driverInitialized) return 0; return mPlayer.mPrepareState>=PrepareReady; }
/* 028A0240 */
void WaveSound::updateParameters() {}
/* 028A0244 */
void WaveSound::detachHandle() {}
/* 028A0248 */
WaveSoundPlayer* WaveSound::driverPlayer() { return &mPlayer; }
/* 028A0250 */
void WaveSound::destroy(WaveSound* object,u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object) { WaveSoundPlayer::destroy(&object->mPlayer,2); object->mBasic.state=3; object->mBasic.vtable=&sWaveBaseSoundVtable; if (flags&1) waveSoundRelease(object); }
}
/* WWHD glue
data sWaveSoundVtable 101854E8
data sWaveBaseSoundVtable 10181B60
ext 0273AD10 void* waveSoundAllocate(u32)
ext 0273AF40 void waveSoundRelease(void*)
ext 02883C64 nw::snd::BasicSound* nw::snd::BasicSound::create(nw::snd::BasicSound*)
ext 0288540C void nw::snd::BasicSound::initialize()
ext 02885724 void nw::snd::BasicSound::finalize()
ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::allocCommand(u32)
ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
ext 0286DA9C nw::snd::SoundListNode* removeSoundList(void*,nw::snd::SoundListNode*)
*/
