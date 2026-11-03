#ifndef WWHD_JASWAVEARCLOADER_H
#define WWHD_JASWAVEARCLOADER_H
#include "JSystem/JAudio/JASSystemHeap.h"
namespace JASystem {
struct WaveArcVTable { u8 prefix[0x1C]; gfn onLoadDone; u8 gap[4]; gfn onEraseDone; };
class TWaveArc : public Kernel::TDisposer {
public:
    Kernel::THeap mHeap;
    be<u32> mLoaded, mState, mFileSize, mLoadCount;
    gptr<be<u8>> mFileName;
    void setFileName(const be<u8>*);
    u32 loadSetup(u32 loadCount);
    u32 sendLoadCmd();
    u32 load(Kernel::THeap*);
    u32 erase();
    u32 eraseSetup();
    void onDispose();
    struct LoadMessage {
        gptr<TWaveArc> mArc;
        gptr<be<u8>> mFileName;
        be<u32> mBase, mLoadCount;
    };
    static void loadToAramCallback(LoadMessage*);
};
TWaveArc* constructWaveArc(TWaveArc*);
namespace WaveArcLoader {
void init(Kernel::THeap*);
void setCurrentDir(const be<u8>*);
}
}
WWHD_OFFSET(JASystem::TWaveArc, mHeap, 4);
WWHD_OFFSET(JASystem::TWaveArc, mLoaded, 0x5C);
WWHD_OFFSET(JASystem::TWaveArc, mState, 0x60);
WWHD_OFFSET(JASystem::TWaveArc, mFileSize, 0x64);
WWHD_OFFSET(JASystem::TWaveArc, mLoadCount, 0x68);
WWHD_OFFSET(JASystem::TWaveArc, mFileName, 0x6C);
WWHD_SIZE(JASystem::TWaveArc, 0x70);
WWHD_SIZE(JASystem::TWaveArc::LoadMessage, 0x10);
void destroyWaveArc(JASystem::TWaveArc* arc, u32 flags);  // 028126F0 (defined in JASWaveArcLoader.cpp)
#endif
