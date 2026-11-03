#ifndef WWHD_JASAUDIOTHREAD_H
#define WWHD_JASAUDIOTHREAD_H
#include "JSystem/JAudio/JASDSPBuf.h"
class JASAudioThread : public JASThread {
public:
    void* run();
    static void syncDSP();
    static void DMACallback();
    static void create(s32 priority, u32 affinity);
};
JASAudioThread* constructAudioDriverThread(JASAudioThread*, s32 priority, u32 affinity, u32 messages, u32 stackSize);
WWHD_SIZE(JASAudioThread, 0x58);
#endif
