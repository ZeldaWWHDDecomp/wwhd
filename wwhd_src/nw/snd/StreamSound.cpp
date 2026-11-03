// Stream sound commands and lifetime. Own reconstruction from original WWHD instructions.
#include "nw/snd/StreamSound.h"
using namespace nw::snd;
void* allocateStreamSound(u32);
void deleteStreamSound(void*);
using FadeConstructor=StreamTrackFade*(*)(StreamTrackFade*);
void constructStreamFades(void*,u32,u32,FadeConstructor);
SoundListNode* removeStreamSound(void*,SoundListNode*);
SoundListNode* insertStreamSound(void*,gptr<SoundListNode>*,SoundListNode*);
struct StreamSoundTable { be<u32> words[24]; };
static StreamSoundTable sStreamSoundTable;
struct StreamBasicTable { be<u32> words[24]; };
static StreamBasicTable sStreamBasicTable;
struct StreamSetupCommand : DriverCommand { gptr<StreamPlayback> player; be<u32> words[35]; };
struct StreamMetadataSendCommand : DriverCommand {
    gptr<StreamPlayback> player;
    be<u32> words[171];
    gptr<void> buffer;
    be<u32> bytes,firstSource,secondSource;
};
/* 02899268 */
StreamSound* StreamSound::construct(StreamSound* sound,PlaybackPool* pool) {
    if(!sound) sound=static_cast<StreamSound*>(allocateStreamSound(0x7670));
    if(sound) {
        BasicSound::create(&sound->basic);
        sound->pool=pool;
        sound->basic.vtable=reinterpret_cast<BasicSoundVtable*>(&sStreamSoundTable);
        constructStreamFades(sound->fades,8,0x10,&StreamTrackFade::construct);
        sound->firstSource=0; sound->secondSource=0; sound->active=0;
        StreamPlayback::construct(&sound->player);
    }
    return sound;
}
/* 028992FC */
void StreamSound::sendSetup(const StreamSetupBlock* config) {
    trackMask=*reinterpret_cast<const be<u16>*>(reinterpret_cast<const u8*>(config)+8);
    DriverCommandManager* commands=DriverCommandManager::instance();
    StreamSetupCommand* command=static_cast<StreamSetupCommand*>(static_cast<void*>(commands->reserve(0xA0)));
    command->id=0x31;
    command->player=&player;
    for(u32 index=0;index<35;++index) command->words[index]=static_cast<u32>(config->words[index]);
    commands->pushCommand(command);
}
/* 02899388 */
void StreamSound::sendMetadata(const StreamMetadataBlock* metadata) {
    DriverCommandManager* commands=DriverCommandManager::instance();
    StreamMetadataSendCommand* command=static_cast<StreamMetadataSendCommand*>(static_cast<void*>(commands->reserve(0x2D0)));
    command->id=0x32;
    command->player=&player;
    for(u32 index=0;index<171;index+=3) {
        command->words[index]=static_cast<u32>(metadata->words[index]);
        command->words[index+1]=static_cast<u32>(metadata->words[index+1]);
        command->words[index+2]=static_cast<u32>(metadata->words[index+2]);
    }
    command->buffer=metadataStorage;
    command->bytes=0x200;
    command->firstSource=firstSource;
    command->secondSource=secondSource;
    commands->pushCommand(command);
}
/* 0289943C */
void StreamSound::initialize() {
    basic.initialize();
    handle=nullptr;
    for(u32 index=0;index<8;++index) {
        StreamTrackFade& fade=fades[index];
        fade.origin=0.0f; fade.frames=0; fade.counter=0; fade.target=0.0f;
        f32 current=fade.value();
        fade.frames=1; fade.counter=0; fade.target=1.0f; fade.origin=current;
    }
    active=1;
}
/* 02899520 */
void StreamSound::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!active) return;
    active=0;
    basic.finalize();
    PlaybackPool* owner=pool;
    auto* link=reinterpret_cast<SoundListNode*>(&basic.poolLink);
    removeStreamSound(&owner->activeCount,link);
    basic.callFinalize();
    gabi::FrameLocal<gptr<SoundListNode>> position(8);
    *position=&owner->free;
    insertStreamSound(&owner->freeCount,position.get(),link);
}
/* 028995A8 */
u32 StreamSound::isPrepared() const {
    if(!basic.driverInitialized) return 0;
    return player.prepared;
}
/* 028995C4 */
u32 StreamSound::hasHandle() const { return handle!=nullptr; }
/* 028995D4 */
void StreamSound::detachHandle() { handle->detach(); }
/* 028995DC */
void StreamSound::updateFades() {
    basic.updateMoveValue();
    u32 mask=trackMask;
    for(u32 index=0;index<8;++index,mask>>=1) {
        if((mask&1) && fades[index].counter<fades[index].frames)
            fades[index].counter=static_cast<u32>(fades[index].counter)+1;
    }
}
/* 0289964C */
void StreamSound::updatePriority() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 priority=static_cast<u32>(basic.ambient.priority)+static_cast<u8>(basic.playerPriority);
    if(priority>127) priority=127;
    else if(priority<0) priority=0;
    PlaybackPool* owner=pool;
    auto* link=reinterpret_cast<SoundListNode*>(&basic.poolLink);
    removeStreamSound(&owner->activeCount,link);
    SoundListNode* position=owner->active.next;
    while(position!=&owner->active) {
        auto* other=reinterpret_cast<BasicSound*>(reinterpret_cast<u8*>(position)-offsetof(BasicSound,poolLink));
        s32 otherPriority=static_cast<u32>(other->ambient.priority)+static_cast<u8>(other->playerPriority);
        if(otherPriority>127) otherPriority=127;
        else if(otherPriority<0) otherPriority=0;
        if(priority<otherPriority) break;
        position=position->next;
    }
    gabi::FrameLocal<gptr<SoundListNode>> iterator(8);
    *iterator=position;
    insertStreamSound(&owner->activeCount,iterator.get(),link);
}
/* 0289971C */
void StreamSound::sendTrackVolumes() {
    DriverCommandManager* commands=DriverCommandManager::instance();
    u32 mask=trackMask;
    struct TrackVolumeCommand : DriverCommand { gptr<StreamPlayback> player; be<u32> mask; be<f32> value; };
    for(u32 index=0;index<8;++index,mask>>=1) {
        if(!(mask&1)) continue;
        auto* command=static_cast<TrackVolumeCommand*>(static_cast<void*>(commands->reserve(0x24)));
        command->id=0x36; command->player=&player; command->mask=1u<<index;
        command->value=fades[index].value();
        commands->pushCommand(command);
    }
}
/* 02899818 */
StreamTrackFade* StreamTrackFade::construct(StreamTrackFade* fade) {
    if(!fade) fade=static_cast<StreamTrackFade*>(allocateStreamSound(0x10));
    if(fade) { fade->frames=0; fade->target=0.0f; fade->counter=0; fade->origin=0.0f; }
    return fade;
}
/* 02899868 */
StreamPlayback* StreamSound::driverPlayer() { return &player; }
/* 02899870 */
void StreamSound::destroy(u32 flags) {
    if(!this) return;
    player.destroy(2);
    basic.state=3;
    basic.vtable=reinterpret_cast<BasicSoundVtable*>(&sStreamBasicTable);
    if(flags&1) deleteStreamSound(this);
}
/* 028998D8 */
void StreamSoundHandle::detach() {
    StreamSound* attached=sound;
    if(!attached) return;
    if(attached->handle==this) attached->handle=nullptr;
    sound=nullptr;
}
/* WWHD glue
data sStreamSoundTable 101822D8
data sStreamBasicTable 10181B60
ext 0273AD10 void* allocateStreamSound(u32)
ext 0273AF40 void deleteStreamSound(void*)
ext 028EFFD0 void constructStreamFades(void*,u32,u32,FadeConstructor)
ext 02883C64 nw::snd::BasicSound* nw::snd::BasicSound::create(nw::snd::BasicSound*)
ext 0288540C void nw::snd::BasicSound::initialize()
ext 02885724 void nw::snd::BasicSound::finalize()
ext 02885D58 void nw::snd::BasicSound::updateMoveValue()
ext 02885F10 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::instance()
ext 02887248 nw::snd::DriverCommand* nw::snd::DriverCommandManager::reserve(u32)
ext 028870F0 u32 nw::snd::DriverCommandManager::pushCommand(nw::snd::DriverCommand*)
ext 0286DA9C nw::snd::SoundListNode* removeStreamSound(void*,nw::snd::SoundListNode*)
ext 0286DA70 nw::snd::SoundListNode* insertStreamSound(void*,gptr<nw::snd::SoundListNode>*,nw::snd::SoundListNode*)
*/
