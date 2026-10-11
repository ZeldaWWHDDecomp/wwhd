/* DSP channel allocation and playback scheduling. Verified against the retained
 * HD unit. */
#include "JSystem/JAudio/JASDSPChannel.h"

using namespace JASystem;
using namespace gabi;
using ChannelTableHD = gptr<TDSPChannel>;
using AllocationPaddingHD = be<u32>;
using AudioHeapHD = gptr<void>;
using StaticWordHD = be<u32>;
static ChannelTableHD DSPCH;
static AllocationPaddingHD sArrayPadding;
static AudioHeapHD sAudioHeap;
static StaticWordHD sStaticWord0;
static StaticWordHD sStaticWord1;
static StaticWordHD sStaticWord2;
static StaticWordHD sStaticWord3;
static GlobalRegistrationHD sGlobalRegistration;

void DSPChannel_allocInit(DSPInterface::DSPBuffer *buffer);
void DSPChannel_playStart(DSPInterface::DSPBuffer *buffer);
void DSPChannel_stop(DSPInterface::DSPBuffer *buffer);
void DSPChannel_forceStop(DSPInterface::DSPBuffer *buffer);
void DSPChannel_acknowledge(DSPInterface::DSPBuffer *buffer);
void DSPChannel_markWaveReady(DSPInterface::DSPBuffer *buffer);
u32 DSPChannel_isFinished(DSPInterface::DSPBuffer *buffer);
void DSPChannel_flushChannel(DSPInterface::DSPBuffer *buffer);
DSPInterface::DSPBuffer *DSPChannel_getDSPHandle(u32 index);
void DSPChannel_releaseHalt(u32 flag);

void *JASHeap_alloc(u32 size, void *heap, u32 alignment);
TDSPChannel *constructArrayHD(void *allocation, u32 count, u32 stride,
                              ChannelConstructorHD constructor, u32 destructor);
void registerGlobalObjectHD(GlobalRegistrationHD *registration);
TDSPChannel *TDSPChannel_getLower(s32 maximum);
TDSPChannel *TDSPChannel_getLowerActive();

// HD: construction initializes the expanded fields and allocates when passed
// NULL.
/* 028157D8 */
TDSPChannel *TDSPChannel_construct(TDSPChannel *channel) {
  if (!channel)
    channel = static_cast<TDSPChannel *>(operator_new(sizeof(TDSPChannel)));
  if (channel) {
    channel->mStatus = TDSPChannel::STATUS_FREE_e;
    channel->mPriority = TDSPChannel::FREE_PRIORITY;
    channel->mpDSPBuffer = nullptr;
    channel->mFlags = 0;
    channel->mCallbackArg = 0;
    channel->mPriorityTime = 0;
    channel->mCallback = 0;
  }
  return channel;
}

// HD: a caller-supplied signed priority limit replaces the GameCube global
// break limit.
/* 02815834 */
TDSPChannel *TDSPChannel_getLower(s32 maximum) {
  TDSPChannel *channels = DSPCH;
  u32 time = 0;
  s32 chosen = -1, priority = 255;
  for (u32 i = 0; i < TDSPChannel::CHANNEL_COUNT; ++i) {
    TDSPChannel *channel = channels + i;
    s32 pr = channel->mPriority;
    if (pr < 0)
      return channel;
    if (pr <= maximum && pr <= priority) {
      u32 age = channel->mPriorityTime;
      if (pr < priority || age > time) {
        time = age;
        chosen = i;
        priority = pr;
      }
    }
  }
  return chosen < 0 ? nullptr : channels + chosen;
}

// HD: force-stop notifies the data callback and queues DSP stop for an active
// channel.
/* 028158C8 */
void TDSPChannel::forceStop() {
  u32 callback = mCallback;
  if (callback)
    call_ptr<s32>(callback, CALLBACK_FORCE_STOP_e, 0u, mCallbackArg.get());
  u32 flags = mFlags & ~FLAG_PLAY_e;
  u32 status = mStatus;
  mPriority = FREE_PRIORITY;
  mCallback = 0;
  mCallbackArg = 0;
  mFlags = flags;
  if (status == STATUS_ACTIVE_e) {
    mFlags = flags | FLAG_STOP_e;
    mStatus = STATUS_STOPPING_e;
  }
}

// HD: allocation installs a three-argument callback and its separate data word.
/* 0281594C */
TDSPChannel *TDSPChannel_alloc(s32 priority, u32 callback, u32 data) {
  TDSPChannel *channel = TDSPChannel_getLower(priority);
  if (channel) {
    channel->forceStop();
    channel->mCallback = callback;
    channel->mPriorityTime = 0;
    channel->mCallbackArg = data;
    channel->mPriority = priority;
  }
  return channel;
}

/* 028159C0 */
void TDSPChannel::detachCallback() {
  mCallback = 0;
  mCallbackArg = 0;
}

/* 028159D0 */
void TDSPChannel::setPriority(u32 priority) { mPriority = priority; }

/* 028159D8 */
void TDSPChannel::play() { mFlags = mFlags | FLAG_PLAY_e; }

// HD: wave readiness is forwarded to the assigned DSP buffer when present.
/* 028159E8 */
void TDSPChannel::markWaveReady() {
  DSPInterface::DSPBuffer *buffer = mpDSPBuffer;
  if (buffer)
    DSPChannel_markWaveReady(buffer);
}

// HD: channels are constructed as 0x1C-byte elements on the audio heap, aligned
// to 32 bytes.
/* 028159F8 */
void TDSPChannel_initAll() {
  u32 padding = sArrayPadding;
  void *allocation =
      JASHeap_alloc(padding + sizeof(TDSPChannel) * TDSPChannel::CHANNEL_COUNT,
                    sAudioHeap, 32);
  TDSPChannel *channels =
      allocation ? constructArrayHD(
                       static_cast<u8 *>(allocation) + sArrayPadding.get(),
                       TDSPChannel::CHANNEL_COUNT, sizeof(TDSPChannel),
                       TDSPChannel_construct, 0)
                 : nullptr;
  DSPCH = channels;
  for (u32 i = 0; i < TDSPChannel::CHANNEL_COUNT; ++i) {
    TDSPChannel *channel = DSPCH.get() + i;
    DSPInterface::DSPBuffer *buffer = DSPChannel_getDSPHandle(i);
    channel->mpDSPBuffer = buffer;
  }
}

/* 02815AB0 */
TDSPChannel *TDSPChannel_getLowerActive() {
  TDSPChannel *channels = DSPCH;
  u32 time = 0;
  s32 chosen = -1, priority = 255;
  for (u32 i = 0; i < TDSPChannel::CHANNEL_COUNT; ++i) {
    TDSPChannel *channel = channels + i;
    if (channel->mStatus != TDSPChannel::STATUS_ACTIVE_e)
      continue;
    s32 pr = channel->mPriority;
    if (pr < TDSPChannel::UNSTEALABLE_PRIORITY && pr <= priority) {
      u32 age = channel->mPriorityTime;
      if (pr < priority || age > time) {
        time = age;
        chosen = i;
        priority = pr;
      }
    }
  }
  return chosen < 0 ? nullptr : channels + chosen;
}

// HD: the retained entry point has no priority argument and returns a full
// register flag.
/* 02815B40 */
u32 TDSPChannel_breakLowerActive() {
  TDSPChannel *channel = TDSPChannel_getLowerActive();
  if (!channel)
    return 0;
  channel->forceStop();
  return 1;
}

// HD: pending play/stop bits and callback results drive this expanded state
// machine.
/* 02815B84 */
void TDSPChannel::update() {
  u32 finished = DSPChannel_isFinished(mpDSPBuffer);
  u32 flags = mFlags;
  if (finished) {
    u32 status = mStatus;
    mFlags = flags & ~FLAG_STOP_e;
    if (status == STATUS_ACTIVE_e) {
      u32 callback = mCallback;
      if (!callback || call_ptr<s32>(callback, CALLBACK_FINISH_e, 0u,
                                     mCallbackArg.get()) < 0)
        mPriority = FREE_PRIORITY;
    }
    DSPInterface::DSPBuffer *buffer = mpDSPBuffer;
    mStatus = STATUS_FREE_e;
    DSPChannel_acknowledge(buffer);
    DSPChannel_flushChannel(mpDSPBuffer);
    return;
  }
  if (flags & FLAG_STOP_e) {
    DSPInterface::DSPBuffer *buffer = mpDSPBuffer;
    mFlags = flags & ~FLAG_STOP_e;
    DSPChannel_forceStop(buffer);
    DSPChannel_flushChannel(mpDSPBuffer);
    return;
  }
  u32 status = mStatus;
  if (status == STATUS_STOPPING_e)
    return;
  if (status == STATUS_FREE_e) {
    if (!(flags & FLAG_PLAY_e))
      return;
    DSPInterface::DSPBuffer *buffer = mpDSPBuffer;
    mStatus = STATUS_ACTIVE_e;
    mFlags = flags & ~FLAG_PLAY_e;
    DSPChannel_allocInit(buffer);
    u32 callback = mCallback;
    if (callback)
      call_ptr<s32>(callback, CALLBACK_PLAY_e, mpDSPBuffer.get(),
                    mCallbackArg.get());
    DSPChannel_playStart(mpDSPBuffer);
    DSPChannel_flushChannel(mpDSPBuffer);
    return;
  }
  u32 callback = mCallback;
  bool haveCallback = callback != 0;
  if (callback && call_ptr<s32>(callback, CALLBACK_UPDATE_e, mpDSPBuffer.get(),
                                mCallbackArg.get()) < 0) {
    callback = mCallback;
    mStatus = STATUS_FREE_e;
    if (!callback ||
        call_ptr<s32>(callback, CALLBACK_FINISH_e, 0u, mCallbackArg.get()) < 0)
      mPriority = FREE_PRIORITY;
    DSPChannel_stop(mpDSPBuffer);
    DSPChannel_flushChannel(mpDSPBuffer);
    return;
  }
  mPriorityTime = mPriorityTime + 1;
  if (haveCallback)
    DSPChannel_flushChannel(mpDSPBuffer);
}

/* 02815D44 */
void TDSPChannel_updateAll() {
  for (u32 i = 0; i < TDSPChannel::CHANNEL_COUNT; ++i)
    (DSPCH.get() + i)->update();
  DSPChannel_releaseHalt(3);
}

// HD: compiler-generated and template code of this translation unit.
/* 02815DA4 */
void __sinit_JASDSPChannel_cpp() {
  sStaticWord2 = 0;
  sStaticWord0 = 0;
  sStaticWord3 = 0;
  sStaticWord1 = 0;
  registerGlobalObjectHD(&sGlobalRegistration);
}

/* WWHD glue
data DSPCH 101F9F64
data sArrayPadding 101FCD08
data sAudioHeap 101F9DA8
data sStaticWord0 104B563C
data sStaticWord1 104B5640
data sStaticWord2 104B5644
data sStaticWord3 104B5648
data sGlobalRegistration 101F9F58
ext 0273B0D4 void* JASHeap_alloc(u32, void*, u32)
ext 028EFF8C TDSPChannel* constructArrayHD(void*, u32, u32, ChannelConstructorHD, u32)
ext 028F026C void registerGlobalObjectHD(GlobalRegistrationHD*)
ext 02816240 JASystem::DSPInterface::DSPBuffer* DSPChannel_getDSPHandle(u32)
ext 02816210 void DSPChannel_releaseHalt(u32)
ext 02815E18 void DSPChannel_allocInit(DSPInterface::DSPBuffer*)
ext 02815E3C void DSPChannel_playStart(DSPInterface::DSPBuffer*)
ext 02815EB4 void DSPChannel_stop(DSPInterface::DSPBuffer*)
ext 02815EC0 void DSPChannel_forceStop(DSPInterface::DSPBuffer*)
ext 02815ECC void DSPChannel_acknowledge(DSPInterface::DSPBuffer*)
ext 028161B8 void DSPChannel_markWaveReady(DSPInterface::DSPBuffer*)
ext 02815EDC u32 DSPChannel_isFinished(DSPInterface::DSPBuffer*)
ext 02816444 void DSPChannel_flushChannel(DSPInterface::DSPBuffer*)
*/
