#ifndef WWHD_JASBANK_H
#define WWHD_JASBANK_H
#include "JSystem/JAudio/JASWaveBankMgr.h"
#include "JSystem/JAudio/JASInst.h"
namespace JASystem {
struct BankVTable { u8 prefix[0x14]; gfn getInst; };
// HD: the wave-bank link precedes the vtable pointer.
class TBank {
public:
    static void* getCurrentHeap();
    gptr<TWaveBank> mWaveBank;
    gptr<BankVTable> mVtable;
};
class TBasicBank : public TBank {
public:
    TBasicBank();
    TInst* getInst(u32) const;
    gptr<gptr<TInst>> mInstTable;
    be<u32> mInstCount;
    void setInstCount(u32);
    void setInst(s32, TInst*);
};
}
WWHD_OFFSET(JASystem::TBank, mWaveBank, 0);
WWHD_OFFSET(JASystem::TBank, mVtable, 4);
WWHD_SIZE(JASystem::TBank, 8);
WWHD_OFFSET(JASystem::TBasicBank, mInstTable, 8);
WWHD_OFFSET(JASystem::TBasicBank, mInstCount, 0xC);
WWHD_SIZE(JASystem::TBasicBank, 0x10);
#endif
