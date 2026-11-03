/* Bank instrument selection, note scaling and channel setup. From WWHD's instructions only. */
#include "nw/snd/WaveChannel.h"
#include "nw/snd/BankReader.h"
using namespace nw::snd;

/* 028A2FE0 */
WaveChannel* WaveChannel::playBankInstrument(void* unused,BankReader* bank,WaveArchiveReader* archive,const BankNote* note) {
    gabi::AutoFrame frame{0x028A2FE0};
    gabi::FrameLocal<BankInstrumentInfo> instrument(8);
    gabi::FrameLocal<WaveDataReader> reader(0x28);
    gabi::FrameLocal<WaveDescription> wave(0x38);
    instrument->envelope = 0;
    instrument->release = 0;
    s32 instrumentIndex = note->instrument;
    s32 key = note->key;
    s32 velocity = note->velocity;
    if (!bank->readInstrument(instrument.get(),instrumentIndex,key,velocity)) return nullptr;
    const void* waveData = archive->loadedAddress(instrument->wave.waveIndex);
    WaveDataReader::create(reader.get(),static_cast<const WaveFileDirectory*>(waveData),0);
    if (!reader->readDescription(wave.get(),nullptr)) return nullptr;
    s32 channels = wave->mChannelCount;
    u32 mode = note->mode;
    if (channels > 2) channels = 2;
    u32 priority = note->priority;
    u32 callback = note->callback;
    u32 argument = note->argument;
    WaveChannel* channel = allocate(channels,priority,callback,argument,mode);
    if (!channel) return nullptr;
    channel->key = note->key;
    channel->baseKey = instrument->baseKey;
    u32 velocityBits = note->velocity;
    u32 scaled = velocityBits * velocityBits * (u32)instrument->volume;
    channel->volume = gabi::fdivs_ppc((f32)(s32)scaled,2048383.0f);
    channel->pitch = instrument->pitch;
    channel->envelope.setAttack((u32)instrument->envelope >> 24);
    channel->envelope.setHold((u32)instrument->envelope & 0xFF);
    channel->envelope.setDecay(((u32)instrument->envelope >> 16) & 0xFF);
    channel->envelope.setSustain(((u32)instrument->envelope >> 8) & 0xFF);
    channel->envelope.setRelease(instrument->release);
    u32 adjustment = (u32)note->panOffset + (u32)instrument->pan - 64;
    channel->pan = gabi::fdivs_ppc((f32)(s32)adjustment,63.0f);
    channel->interpolation = instrument->interpolation;
    channel->ignoreRelease = instrument->ignoreRelease;
    channel->outputMode = instrument->outputMode;
    channel->start(wave.get(),note->startFrame,0);
    return channel;
}

/* WWHD glue
 ext 0289FD24 const void* nw::snd::WaveArchiveReader::loadedAddress(u32)
 ext 028B0C5C nw::snd::WaveDataReader* nw::snd::WaveDataReader::create(nw::snd::WaveDataReader*,const nw::snd::WaveFileDirectory*,u32)
 ext 028B0D68 u32 nw::snd::WaveDataReader::readDescription(nw::snd::WaveDescription*,const void*) const
*/
