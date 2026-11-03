#pragma once
#include "JSystem/JAudio/JASWaveBank_extra.h"
namespace JASystem {
class TSimpleWaveBank : public TWaveBank, public TWaveArc {
public:
    class TWaveHandle : public JASystem::TWaveHandle {
    public:
        TWaveInfo mWaveInfo; gptr<Kernel::THeap> mHeap;
        const TWaveInfo* getWaveInfo() const;
        u32 getWavePtr() const; // HD: intptr_t occupies one guest word.
    };
    gptr<TWaveHandle> mWaveTable; be<u32> mWaveCount;
    void setWaveTableSize(u32 count);
    void setWaveInfo(u32 index,const TWaveInfo& info);
    TWaveHandle* getWaveHandle(u32 index) const;
    TWaveArc* getWaveArc(s32 index);
    void onLoadDone();
    void onEraseDone();
};
TSimpleWaveBank* constructSimpleWaveBank(TSimpleWaveBank* bank);
TSimpleWaveBank::TWaveHandle* constructSimpleWaveHandle(TSimpleWaveBank::TWaveHandle* handle);
void destroySimpleWaveHandle(TSimpleWaveBank::TWaveHandle* handle,u32 flags);
}
WWHD_SIZE(JASystem::TSimpleWaveBank,0x7C);
WWHD_OFFSET(JASystem::TSimpleWaveBank,mWaveTable,0x74);
WWHD_OFFSET(JASystem::TSimpleWaveBank,mWaveCount,0x78);
WWHD_SIZE(JASystem::TSimpleWaveBank::TWaveHandle,0x30);
WWHD_OFFSET(JASystem::TSimpleWaveBank::TWaveHandle,mWaveInfo,4);
WWHD_OFFSET(JASystem::TSimpleWaveBank::TWaveHandle,mHeap,0x2C);
void __sinit_JASSimpleWaveBank_cpp();
