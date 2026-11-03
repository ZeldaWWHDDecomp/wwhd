#pragma once
#include "wwhd.h"
#include "nw/ut/Platform.h"
#include "nw/snd/AudioMixRuntime.h"
namespace nw { namespace snd {
struct SoundMemoryStream {
    be<u32> baseFlags;
    be<u32> pendingRead;
    be<u32> pendingWrite;
    be<u32> vtable;
    gptr<const u8> buffer;
    be<u32> length;
    be<u32> position;
    static SoundMemoryStream* create(SoundMemoryStream*,const void*,u32);
    void reset();
    u32 read(void*,u32);
    u32 seek(u32,u32);
    u32 isBusy();
    void close();
    void cancel();
    u32 canRead();
    u32 canWrite();
    u32 alignment();
    u32 canSeek();
    u32 canCancel();
    u32 tell();
    u32 size();
};
WWHD_OFFSET(SoundMemoryStream,vtable,0xC);
WWHD_OFFSET(SoundMemoryStream,buffer,0x10);
WWHD_OFFSET(SoundMemoryStream,length,0x14);
WWHD_OFFSET(SoundMemoryStream,position,0x18);
WWHD_SIZE(SoundMemoryStream,0x1C);
struct SoundWorkRecord {
    be<u32> readOffset,writeOffset,readCount,writeCount;
    be<u32> current,end;
    be<u16> flags,generation;
    static SoundWorkRecord* create(SoundWorkRecord*);
};
WWHD_SIZE(SoundWorkRecord,0x1C);
struct SoundPoolLink { gptr<SoundPoolLink> next; };
struct SoundBufferPool {
    gptr<SoundPoolLink> freeHead;
    gptr<void> buffer;
    be<u32> length;
    void removeBuffer();
    u32 addBuffer(void*,u32,u32);
    u32 countFree();
    SoundPoolLink* allocate();
    void release(SoundPoolLink*);
};
WWHD_OFFSET(SoundBufferPool,buffer,4);
WWHD_OFFSET(SoundBufferPool,length,8);
WWHD_SIZE(SoundBufferPool,0xC);
} }
void* runtimeAllocate(u32);
void runtimeCopy(void*,const void*,u32);
namespace nw { namespace snd {
struct TrackOutputControl {
    be<f32> volume;
    be<u32> mixMode;
    MixVolume mixVolume[2];
    be<f32> pan,surroundPan,mainSend;
    be<f32> effectSend[3];
};
WWHD_OFFSET(TrackOutputControl,pan,0x38);
WWHD_OFFSET(TrackOutputControl,effectSend,0x44);
WWHD_SIZE(TrackOutputControl,0x50);
struct SequenceRuntimeSupport;
struct ChannelRuntime_l;
struct ByteRamp_l {
    be<u8> origin,target;
    be<s16> frames,counter;
};
WWHD_SIZE(ByteRamp_l,6);
struct TrackRuntimeVtable_l { be<u32> entry; };
struct TrackRuntimeControl {
    be<u8> state,activeFlag;
    be<u16> stateCount;
    be<f32> volume,pitch;
    be<f32> panRange;
    be<f32> tvVolume;
    be<u32> outputMask;
    MixVolume outputMix[2];
    be<f32> tvPan,tvSurroundPan,tvMainSend;
    be<f32> tvEffectSend[3];
    TrackOutputControl output[2];
    gptr<const u8> sequenceData;
    be<u32> readerCursor;
    be<u8> ready,active,tied,oneShot;
    u8 parseState[0x50];
    be<u8> noteState,routeMode,muted,silent;
    be<u32> delayTicks;
    be<u8> portamento,allocationMode,voiceMode,transpose;
    be<u32> waitCounter;
    be<f32> noteLength;
    ByteRamp_l volumeRamp,expressionRamp,panOffsetRamp,surroundOffsetRamp,pitchBendRamp;
    u8 rampPadding[2];
    u8 modulationState[4][16];
    be<u8> modulationType[4];
    be<u8> velocity,pitchBendRange,voiceFlags,voicePadding;
    be<u8> transposeControl,panControl,keyControl,phaseControl;
    be<u8> attack,decay,sustain,release;
    be<u16> envelopeRange;
    be<s8> biquadType;
    be<u8> priority;
    be<u8> envelopeFlags[3];
    u8 envelopePadding;
    be<f32> lpfOffset,biquadValue;
    be<s32> outputLine;
    be<s16> variables[16];
    gptr<SequenceRuntimeSupport> owner;
    gptr<ChannelRuntime_l> channels;
    gptr<TrackRuntimeVtable_l> vtable;
    static TrackRuntimeControl* create(TrackRuntimeControl*);
    void reset();
    void updateChannels();
    void setVolume(f32);
    void setPitch(f32);
    void setLpfOffset(f32);
    void setTvVolume(f32);
    void setTvPan(f32);
    void setTvSurroundPan(f32);
    void setTvMainSend(f32);
    void setTvEffectSend(u32,f32);
    void setOutputVolume(u32,f32);
    void setOutputPan(u32,f32);
    void setOutputSurroundPan(u32,f32);
    void setOutputMainSend(u32,f32);
    void setOutputEffectSend(u32,u32,f32);
};
WWHD_OFFSET(TrackRuntimeControl,volume,4);
WWHD_OFFSET(TrackRuntimeControl,tvVolume,0x10);
WWHD_OFFSET(TrackRuntimeControl,tvPan,0x48);
WWHD_OFFSET(TrackRuntimeControl,tvEffectSend,0x54);
WWHD_OFFSET(TrackRuntimeControl,output,0x60);
WWHD_OFFSET(TrackRuntimeControl,lpfOffset,0x1E8);
struct SoundResourceChoice {
    be<s32> type;
    be<u32> value;
    static SoundResourceChoice* create(SoundResourceChoice*);
};
WWHD_SIZE(SoundResourceChoice,8);
} }

namespace nw { namespace snd {
struct SequenceLoadTask {
    u8 taskPrefix[8];
    nw::ut::Event event;
    be<u32> status;
    u8 requestState[8];
    be<u32> parameters[13];
    be<u32> sequence;
    be<u32> banks[4];
    be<u32> waveArchiveImages[4];
    be<u8> individualWaveLoads[4];
    gptr<void> heap;
    be<u8> loadSucceeded;
    u8 loadSucceededPadding[3];
    u8 listNode[8];
    void loadResources();
    u32 isPending();
    void releaseHeap();
    void releaseHeapCallback();
    void reset();
    void initialize(const be<u32>*);
    u32 ensureHeap();
    u32 request();
};
WWHD_OFFSET(SequenceLoadTask,status,0x2C);
WWHD_OFFSET(SequenceLoadTask,parameters,0x38);
WWHD_OFFSET(SequenceLoadTask,sequence,0x6C);
WWHD_OFFSET(SequenceLoadTask,heap,0x94);
WWHD_OFFSET(SequenceLoadTask,loadSucceeded,0x98);
struct SequenceResourceParameters {
    be<u32> sequence,offset,archive;
    gptr<u8> banks[4];
    be<u32> options;
    static SequenceResourceParameters* create(SequenceResourceParameters*,u32,u32,u32,u8*,u32);
};
WWHD_OFFSET(SequenceResourceParameters,banks,0xC);
WWHD_SIZE(SequenceResourceParameters,0x20);
} }
void* runtimeAcquirePlayerHeap(void*);
void runtimeInitializePlayerHeap(void*);
void* runtimeLoadScheduler();
void runtimeScheduleLoad(void*,nw::snd::SequenceLoadTask*,u32);
namespace nw { namespace snd {
u32 addAuxEffect(AudioMixRuntime*,u32,void*,u32);
void fadeAuxEffect(AudioMixRuntime*,u32,u32,u32);
f32 outputMasterVolume(AudioMixRuntime*);
void setOutputMasterVolume(AudioMixRuntime*,f32,u32);
void addOutputFrameCallback(AudioMixRuntime*,void*);
void* removeOutputFrameCallback(AudioMixRuntime*,void*);
} }
void* runtimeListInsert(nw::snd::MixEffectChain*,const gptr<nw::snd::SoundListNode>*,void*);
void* runtimeListRemove(nw::snd::MixEffectChain*,void*);

u32 runtimeEventStatus(nw::ut::Event*);
void runtimeReleasePlayerHeap(void*,void*);

WWHD_OFFSET(nw::snd::TrackRuntimeControl,sequenceData,0x100);
WWHD_OFFSET(nw::snd::TrackRuntimeControl,volumeRamp,0x170);
WWHD_OFFSET(nw::snd::TrackRuntimeControl,modulationState,0x190);
WWHD_OFFSET(nw::snd::TrackRuntimeControl,velocity,0x1D4);
WWHD_OFFSET(nw::snd::TrackRuntimeControl,owner,0x214);
WWHD_OFFSET(nw::snd::TrackRuntimeControl,channels,0x218);
WWHD_SIZE(nw::snd::TrackRuntimeControl,0x220);
void* runtimeTrackMixConstruct(void*);
void* runtimeTrackOutputConstruct(void*);
void* runtimeTrackModulationConstruct(void*);
void runtimeTrackModulationReset(void*);
