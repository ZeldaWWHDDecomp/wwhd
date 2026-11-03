#pragma once
#include "JSystem/JAudio/JASWSParser.h"
namespace JASystem {
struct WaveHandleVtable {
    u8 prefix[0x14]; gfn getWaveInfo; u8 gap[4]; gfn getWavePtr;
};
class TWaveHandle { public: gptr<WaveHandleVtable> mVtable; };
}
WWHD_SIZE(JASystem::TWaveHandle,4);
WWHD_OFFSET(JASystem::WaveHandleVtable,getWaveInfo,0x14);
WWHD_OFFSET(JASystem::WaveHandleVtable,getWavePtr,0x1C);
void __sinit_JASWaveBank_cpp();
