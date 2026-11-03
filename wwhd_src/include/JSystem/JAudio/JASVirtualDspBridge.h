#pragma once
#include "bindings.h"
#include "JSystem/JAudio/JASThread.h"
#include "JSystem/JAudio/JASBridgeDecoder.h"
#include "JSystem/JAudio/JASBridgeResampler.h"
#include "JSystem/JAudio/JASBridgeFilters.h"
#include "JSystem/JAudio/JASBridgeEffects.h"
namespace JASBridge {
struct VirtualDspMessage { be<u32> words[4]; };
struct VirtualDspProfileTable { u8 storage[0x18]; };
struct VirtualDspProfileLabel { be<u8> text[0x18]; };
/* The worker's profile name: a string view {text, vtable} with this TU's own vtable copy. */
struct VirtualDspProfileName {
    void destroy(u32 flags);
    void assureTermination();
    gptr<be<u8>> text;
    gptr<VirtualDspProfileTable> vtable;
};
struct VirtualDspProfileState { be<u32> state; };
struct VirtualDspWorkerLocals {
    VirtualDspProfileName name;
    VirtualDspProfileState profile;
    VirtualDspMessage message;
};
struct VirtualDspStatics { be<u32> words[4]; };
struct VirtualDspDestructorRecord { be<u32> words[3]; };
class VirtualDsp {
public:
    void setBootCallback(u32 callback);
    void setChannels(u32 count, DSPBuffer_l* channels, EffectLine_l* effects);
    void setMixerGain(f64 ratio);
    be<s16>* outputPlane(u32 index);
    void finishOutput();
    void processFrame();
    void* run();
    void sync();
    JASThread thread;
    gptr<be<s16>> outputPlanes[4];
    be<u32> bootCallback;
    be<u32> channelCount;
    gptr<DSPBuffer_l> channels;
    gptr<EffectLine_l> effects;
    be<u8> processing;
    u8 processingPadding[3];
    gptr<SampleDecoder> decoder;
    gptr<Resampler> resampler;
    gptr<ChannelFilters> filters;
    gptr<VoiceMixer> mixer;
    gptr<EffectsMixer> effectsMixer;
    gptr<u8> decodeStorage;
    gptr<be<s16>> decodeScratch;
    gptr<u8> sampleStorage;
    gptr<be<s16>> sampleScratch;
    gptr<be<s16>> currentSamples;
};
VirtualDsp* constructVirtualDsp(VirtualDsp* object, s32 priority, u32 affinity, u32 messages, u32 stackSize);
void createVirtualDsp(s32 priority, u32 affinity);
void initializeVirtualDspStatics();
WWHD_SIZE(VirtualDspMessage, 16);
WWHD_OFFSET(VirtualDspWorkerLocals, message, 0xC);
WWHD_SIZE(VirtualDspWorkerLocals, 0x1C);
WWHD_OFFSET(VirtualDsp, outputPlanes, 0x58);
WWHD_OFFSET(VirtualDsp, decoder, 0x7C);
WWHD_OFFSET(VirtualDsp, decodeStorage, 0x90);
WWHD_OFFSET(VirtualDsp, currentSamples, 0xA0);
WWHD_SIZE(VirtualDsp, 0xA4);
}
static void freeVirtualDspProfileName(void* object);
static void* allocateVirtualDsp(u32 size);
static void* allocateVirtualDspObject(u32 size, void* heap, s32 alignment);
static void* allocateVirtualDspStorage(u32 size, void* heap, s32 alignment);
static JASThread* constructVirtualDspThread(JASThread*, void*, u32, u32, s32, u32);
static void constructVirtualDspProfile(JASBridge::VirtualDspProfileState*, JASBridge::VirtualDspProfileName*);
static void destroyVirtualDspProfile(JASBridge::VirtualDspProfileState*, u32);
static void registerVirtualDspStatics(JASBridge::VirtualDspDestructorRecord*);
static void clearVirtualDspStorage(void* destination, u32 value, u32 size);
static void resumeVirtualDspThread(void* thread);
static void receiveVirtualDspMessage(JASMessageQueue* queue, JASBridge::VirtualDspMessage* message, u32 flags);
static void sendVirtualDspMessage(JASMessageQueue* queue, JASBridge::VirtualDspMessage* message, u32 flags);
static void exitVirtualDspThread(u32 value);
