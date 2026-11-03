// HD DSP/DAC buffer initialization; reconstructed from the game's instructions.
#include "bindings.h"
#include "JSystem/JAudio/JASNativeBufferInit.h"

static sead::AllocationHeap* sAudioHeap;
static NativeDacBufferSet sDacBuffers;
static NativeDspBufferTable sDspBuffers;
static u8 sDspBufferCount;
static u32 sWriteBuffer;
static u32 sReadBuffer;
static u32 sDspStatus;
static JASBridge::DeviceMixer* sDeviceMixer;
static u32 sUpsampling;
static NativeOutputModeMap sOutputModes;

/* 02813428 */
static void initializeNativeDspBuffers(u32 callback) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 bytes = nativeDacSampleCount() * 2;
    for (u32 i = 0; i < 3; ++i) {
        be<s16>* buffer = allocateNativeAudioBuffer(bytes,sAudioHeap,64);
        sDacBuffers.mBuffers[i] = buffer;
        clearNativeAudioBuffer(buffer,0,bytes);
    }
    be<s16>* tableStorage = allocateNativeAudioBuffer(u32(sDspBufferCount) * 4,sAudioHeap,64);
    u32 count = sDspBufferCount;
    sDspBuffers = reinterpret_cast<gptr<be<s16>>*>(tableStorage);
    for (u32 i = 0; i < count; ++i) {
        gptr<be<s16>>* table = sDspBuffers;
        be<s16>* buffer = allocateNativeAudioBuffer(bytes,sAudioHeap,64);
        table[i] = buffer;
        clearNativeAudioBuffer(sDspBuffers.get()[i].get(),0,bytes);
        count = sDspBufferCount;
    }
    sWriteBuffer = count - 1;
    sReadBuffer = 0;
    sDspStatus = 0;
    sDeviceMixer->initialize();
    sDeviceMixer->submit(sDacBuffers.mBuffers[2].get(),bytes,0);
    sDeviceMixer->submit(sDacBuffers.mBuffers[2].get(),bytes,1);
    sDeviceMixer->configureUpsampling(sUpsampling != 0);
    gabi::FrameLocal<be<u32>> tvMode(8);
    queryNativeAudioDeviceMode(0,tvMode.get());
    u32 mode = *tvMode;
    if (mode <= 3) sDeviceMixer->setOutputMode(0,sOutputModes.mModes[mode]);
    else if (mode == 5) sDeviceMixer->setOutputMode(0,1);
    gabi::FrameLocal<be<u32>> controllerMode(12);
    queryNativeAudioDeviceMode(1,controllerMode.get());
    mode = *controllerMode;
    if (mode <= 3 || mode == 5) sDeviceMixer->setOutputMode(1,1);
    sDeviceMixer->exchangeCompletion(callback,1);
}

/* WWHD glue
 data sAudioHeap 101F9DA8
 data sDacBuffers 104B53B8
 data sDspBuffers 101F9E9C
 data sDspBufferCount 101F9EB8
 data sWriteBuffer 104B539C
 data sReadBuffer 104B53A0
 data sDspStatus 104B53A4
 data sDeviceMixer 101F9FF4
 data sUpsampling 101F9EB0
 data sOutputModes 10170024
 ext 02813418 u32 nativeDacSampleCount()
 ext 0273B0D4 be<s16>* allocateNativeAudioBuffer(u32,sead::AllocationHeap*,s32)
 ext C000A858 void* clearNativeAudioBuffer(void*,u32,u32)
 ext C0006D48 void queryNativeAudioDeviceMode(u32,be<u32>*)
 ext 0281A8AC void JASBridge::DeviceMixer::initialize()
 ext 0281A9EC void JASBridge::DeviceMixer::submit(be<s16>*,u32,u32)
 ext 0281A91C void JASBridge::DeviceMixer::configureUpsampling(u32)
 ext 0281AA60 void JASBridge::DeviceMixer::setOutputMode(u32,u32)
 ext 0281A968 u32 JASBridge::DeviceMixer::exchangeCompletion(u32,u32)
*/
