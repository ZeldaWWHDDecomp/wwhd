// JAudio DSP/DAC buffering; HD bridges four channels through the native mixer.
#include "JSystem/JAudio/JASDSPBuf.h"
#include "sead/ElementAt.h"
using namespace JASystem;
using namespace JASystem::Kernel;
using namespace gabi;
using DspBuffers = gptr<gptr<be<s16>>>;
using BridgePointer = gptr<void>;
using SamplePointer = gptr<be<s16>>;
struct DacBuffers { gptr<be<s16>> values[3]; };
struct TickHistory { be<u32> values[10]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static AudioThreadState audioThreadState;
static DspBuffers dsp_buf;
static u8 numDSPBuf;
static u32 writeBuffer;
static u32 readBuffer;
static u32 dspStatus;
static gfn callback;
static BridgePointer sMixer;
static BridgePointer sBridge;
static u32 sLastTick;
static TickHistory sTickHistory;
static SamplePointer lastRspMadep;
static gfn dacCallbackFunc;
static DacBuffers dac;
static u32 sDacIndex;
static Initialization sInitialization;
static Registration sRegistration;
extern be<s16>* getMixerChannel(void*, u32);
extern void finishMixerBuffer(void*);
extern void executeDspCommands();
extern void invalidateDspChannels();
extern u32 getDspTick();
extern void reportDspStarvation(const char*);
extern void killDspChannel();
extern void updateDspChannels();
extern void flushDspChannels();
extern void submitBridgeBuffer(void*, be<s16>*, u32, u32);
extern void startBridgeMixer(void*);
extern void stopBridgeMixer(void*);
extern void registerDspBufferGlobals(void*);
/* 0281361C */
void TDSP_DACBuffer::mixDSP(be<s16>* output, u32 samples) {
    u32 read = readBuffer;
    u32 count = numDSPBuf;
    u32 next = read + 1;
    if (next == count)
        next = 0;
    if (next == writeBuffer && count >= 3) {
        be<s16>* current = dsp_buf.get()[read];
        s16 left = current[(s32)(samples >> 1) - 1];
        s16 right = current[(s32)samples - 1];
        for (u32 i = 0; i < samples; ++i) {
            u32 index = readBuffer;
            dsp_buf.get()[index].get()[i] = left;
        }
        if (samples < samples * 2) {
            for (u32 i = 0; i < samples; ++i) {
                u32 index = readBuffer;
                dsp_buf.get()[index].get()[samples + i] = right;
            }
        }
    } else {
        readBuffer = next;
    }
    void* mixer = sMixer;
    be<s16>* first = getMixerChannel(mixer, 0);
    be<s16>* second = getMixerChannel(mixer, 1);
    be<s16>* third = getMixerChannel(mixer, 2);
    be<s16>* fourth = getMixerChannel(mixer, 3);
    for (u32 i = 0; i < samples; ++i) {
        output[i * 4] = second[i];
        output[i * 4 + 1] = first[i];
        output[i * 4 + 2] = fourth[i];
        output[i * 4 + 3] = third[i];
    }
    finishMixerBuffer(mixer);
}
/* 028137E0 */
void TDSP_DACBuffer::updateDSP() {
    executeDspCommands();
    invalidateDspChannels();
    u32 tick = getDspTick();
    u32 previous = sLastTick;
    u32 subframes = gSubFrames;
    u32 remaining = audioThreadState.mRemainingSubframes;
    u32 elapsed = tick - previous;
    sLastTick = tick;
    sTickHistory.values[subframes - remaining] = elapsed;
    if (subframes != remaining) {
        f32 ratio = (f32)((f32)(u32)sTickHistory.values[0] / (f32)elapsed);
        if (!(ratio >= 1.1f)) {
            reportDspStarvation("kill DSP channel");
            killDspChannel();
        }
    }
    updateDspChannels();
    flushDspChannels();
}
/* 028138A8 */
void TDSP_DACBuffer::finishDSPFrame() {
    u32 next = (u32)writeBuffer + 1;
    if (next == numDSPBuf)
        next = 0;
    if (next == readBuffer) {
        dspStatus = 0;
        return;
    }
    writeBuffer = next;
    audioThreadState.mRemainingSubframes = gSubFrames;
    u32 samples = getFrameSamples();
    dspStatus = 1;
    updateDSP();
    u32 target = callback.get();
    if (target) {
        u32 index = writeBuffer;
        be<s16>* buffer = sead::elementAt(dsp_buf.get(), index);
        gabi::call_ptr<void>(target, buffer, samples);
    }
}
namespace JASystem { namespace Kernel {
/* 02813980 */
void updateDac() {
    be<s16>* ready = lastRspMadep;
    lastRspMadep = nullptr;
    if (ready) {
        u32 size = getDacSize();
        submitBridgeBuffer(sBridge, ready, size * 2, 0);
        size = getDacSize();
        submitBridgeBuffer(sBridge, ready, size * 2, 1);
    }
    u32 samples = getFrameSamples();
    u32 index = sDacIndex;
    be<s16>* buffer = dac.values[index];
    TDSP_DACBuffer::mixDSP(buffer, samples);
    if (!dspStatus)
        TDSP_DACBuffer::finishDSPFrame();
    index = sDacIndex;
    buffer = dac.values[index];
    u32 next = index + 1;
    if (next >= 3)
        next = 0;
    lastRspMadep = buffer;
    u32 target = dacCallbackFunc.get();
    sDacIndex = next;
    if (target) {
        samples = getFrameSamples();
        gabi::call_ptr<void>(target, buffer, samples);
    }
}
/* 02813A88 */
void startAudioBridge() { startBridgeMixer(sBridge); }
/* 02813A94 */
void stopAudioBridge() { stopBridgeMixer(sBridge); }
}}
/* 02813AA0 */
void __sinit_JASDSPBuf_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerDspBufferGlobals(&sRegistration);
}
/* WWHD glue
data audioThreadState 101F9F04
data dsp_buf 101F9E9C
data callback 101F9EA0
data numDSPBuf 101F9EB8
data writeBuffer 104B539C
data readBuffer 104B53A0
data dspStatus 104B53A4
data sMixer 101FA00C
data sBridge 101F9FF4
data sLastTick 101F9EBC
data sTickHistory 101F9EC0
data lastRspMadep 101F9EA8
data dacCallbackFunc 101F9EAC
data dac 104B53B8
data sDacIndex 101F9EE8
data sInitialization 104B53A8
data sRegistration 101F9EEC
str 10170034
ext 0281ADA4 be<s16>* getMixerChannel(void*, u32)
ext 0281ADB4 void finishMixerBuffer(void*)
ext 0280EB24 void executeDspCommands()
ext 0281561C void invalidateDspChannels()
ext C0009CD0 u32 getDspTick()
ext C0009EE8 void reportDspStarvation(const char*)
ext 02815B40 void killDspChannel()
ext 02815D44 void updateDspChannels()
ext 02815610 void flushDspChannels()
ext 0281A9EC void submitBridgeBuffer(void*, be<s16>*, u32, u32)
ext 0281A97C void startBridgeMixer(void*)
ext 0281A9B4 void stopBridgeMixer(void*)
ext 028F026C void registerDspBufferGlobals(void*)
*/
