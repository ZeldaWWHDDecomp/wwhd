#ifndef WWHD_JASDSPBUF_H
#define WWHD_JASDSPBUF_H
#include "JSystem/JAudio/JASRate.h"
#include "JSystem/JAudio/JASThread.h"
struct AudioThreadState {
    gptr<JASThread> mThread;
    be<u32> mRemainingSubframes, mFrameCount;
    be<u8> mReady, mSuspended;
    u8 padding[2];
};
extern AudioThreadState& audioThreadState;
namespace JASystem {
class TDSP_DACBuffer {
public:
    static void mixDSP(be<s16>* output, u32 samples);
    static void updateDSP();
    static void finishDSPFrame();
};
namespace Kernel {
void updateDac();
void startAudioBridge();
void stopAudioBridge();
}
}
WWHD_SIZE(AudioThreadState, 0x10);
WWHD_OFFSET(AudioThreadState, mRemainingSubframes, 4);
WWHD_OFFSET(AudioThreadState, mReady, 0xC);
#endif
