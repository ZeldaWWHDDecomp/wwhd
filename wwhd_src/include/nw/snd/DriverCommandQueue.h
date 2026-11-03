/* Driver command storage and queues, reconstructed from WWHD. Names are our own. */
#pragma once
#include "nw/snd/BasicSound.h"
namespace nw { namespace snd {
/* CommandArena and DriverCommandManager are declared in nw/snd/BasicSound.h. */
}}
void commandIdle();
void dispatchAudioCommandList(nw::snd::DriverCommand*);
void sndDelete(void*);
void OSMemoryBarrier();
void processSoundThreadCommands(void*);
void clearCommandQueue(nw::ut::MessageQueue*);
void* soundThreadInstance();
