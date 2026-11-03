// Sound-driver message loop, derived from WWHD instructions.
#include "nw/snd/AudioFrameDriver.h"
#include "nw/snd/BasicSound.h"
using namespace nw::snd;
using AudioFrameCallback=void(*)();
u32 registerAudioFrameCallback(AudioFrameCallback);
void deregisterAudioFrameCallback(AudioFrameCallback);
void initializeAudioProfiles(void*,u32);
/* 02898F88 */
void AudioFrameDriver::run() {
    gabi::AutoFrame frame{gabi::current_function()};
    if(registerAudioFrameCallback(&AudioFrameDriver::frameCallback)) return;
    initializeAudioProfiles(profileBuffers,32);
    previousProfile=nullptr; profileBufferIndex=0;
    gabi::FrameLocal<be<u32>> message(8);
    // HD: frame-exact for cross-build save states: the sound thread waits in one of the three
    // receives, each with its own return address and r31 = the driver.
    for(;;) {
        gabi::call_site<u32>(0x02898FD8,{{31,gabi::ea(this)}},{},0x028F6C94,&messages,message.get(),1u);
        u32 word=*message;
        for(;;) {
            u32 kind=word&0xF0000000;
            while(kind==(1u<<28)) {
                receivedSerial=word&0x0FFFFFFF;
                processFrame();
                gabi::call_site<u32>(0x0289900C,{{31,gabi::ea(this)}},{},0x028F6C94,&messages,message.get(),1u);
                word=*message; kind=word&0xF0000000;
            }
            if(kind!=0x30000000) break;
            while(DriverCommandManager::instance()->executePending()) {}
            while(DriverCommandManager::taskThreadInstance()->executePending()) {}
            gabi::call_site<u32>(0x02899060,{{31,gabi::ea(this)}},{},0x028F6C94,&messages,message.get(),1u);
            word=*message;
        }
        if((word&0xF0000000)==0x20000000) break;
    }
    deregisterAudioFrameCallback(&AudioFrameDriver::frameCallback);
}
/* WWHD glue
ext C0006E98 u32 registerAudioFrameCallback(AudioFrameCallback)
ext C0006CA0 void deregisterAudioFrameCallback(AudioFrameCallback)
ext C0006E48 void initializeAudioProfiles(void*,u32)
ext 02885F68 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::taskThreadInstance()
ext 02887464 u32 nw::snd::DriverCommandManager::executePending()
*/
