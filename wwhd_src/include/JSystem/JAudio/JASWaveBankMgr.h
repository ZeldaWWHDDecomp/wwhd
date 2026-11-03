#ifndef WWHD_JASWAVEBANKMGR_H
#define WWHD_JASWAVEBANKMGR_H
#include "wwhd.h"
using namespace gabi;
namespace JASystem {
class TWaveArc;
namespace Kernel { class THeap; }
struct WaveBankVTable { u8 prefix[0x1C]; gfn getWaveArc; };
class TWaveBank { public: gptr<WaveBankVTable> mVtable; static void* getCurrentHeap(); /* 028452A4 */ };
namespace WaveBankMgr {
void init(s32 size);
TWaveBank* getWaveBank(s32 banknum);
u32 registWaveBank(s32 banknum, TWaveBank* bank);
u32 registWaveBankWS(s32 banknum, void* stream);
TWaveArc* getWaveArc(s32 banknum, s32 arcnum);
u32 loadWave(s32 banknum, s32 arcnum, Kernel::THeap* heap);
u32 eraseWave(s32 banknum, s32 arcnum);
}
}
WWHD_OFFSET(JASystem::TWaveBank, mVtable, 0);
WWHD_SIZE(JASystem::TWaveBank, 4);
WWHD_OFFSET(JASystem::WaveBankVTable, getWaveArc, 0x1C);
WWHD_SIZE(JASystem::WaveBankVTable, 0x20);
#endif
