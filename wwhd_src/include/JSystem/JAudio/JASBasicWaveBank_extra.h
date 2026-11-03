#pragma once
#include "JSystem/JAudio/JASWaveBank_extra.h"
namespace JASystem {
class TBasicWaveBank : public TWaveBank {
public:
    class TWaveHandle : public JASystem::TWaveHandle {
    public:
        JASystem::TWaveInfo mWaveInfo; gptr<Kernel::THeap> mHeap;
        const JASystem::TWaveInfo* getWaveInfo() const;
        u32 getWavePtr() const; // HD: intptr_t occupies one guest word.
    };
    class TWaveInfo {
    public:
        TWaveHandle mWaveHandle; be<u32> mWaveID;
        gptr<TWaveInfo> mPrev, mNext;
    };
    class TWaveGroup : public TWaveArc {
    public:
        gptr<TBasicWaveBank> mBank;
        gptr<TWaveInfo> mCtrlWaveArray; be<u32> mWaveCount;
        u32 getWaveID(s32 index) const;
        void setWaveCount(u32 count);
        void setWaveInfo(u32 index,u32 id,const JASystem::TWaveInfo& info);
        void onLoadDone();
        void onEraseDone();
    };
    Kernel::HeapMutex mMutex;
    gptr<gptr<TWaveInfo>> mWaveTable; be<u32> mWaveCount;
    gptr<gptr<TWaveGroup>> mWaveGroupArray; be<u32> mWaveGroupCount;
    void setGroupCount(u32 count);
    void setWaveTableSize(u32 count);
    TWaveGroup* getWaveGroup(s32 index);
    TWaveHandle* getWaveHandle(u32 index) const;
    TWaveArc* getWaveArc(s32 index);
    void incWaveTable(TWaveGroup* group);
    void decWaveTable(TWaveGroup* group);
};
TBasicWaveBank* constructBasicWaveBank(TBasicWaveBank* bank);
TBasicWaveBank::TWaveGroup* constructBasicWaveGroup(TBasicWaveBank::TWaveGroup* group,TBasicWaveBank* bank);
TBasicWaveBank::TWaveInfo* constructBasicCtrlWave(TBasicWaveBank::TWaveInfo* wave);
void destroyBasicWaveGroup(TBasicWaveBank::TWaveGroup* group,u32 flags);
}
WWHD_SIZE(JASystem::TBasicWaveBank,0x40);
WWHD_OFFSET(JASystem::TBasicWaveBank,mMutex,4);
WWHD_OFFSET(JASystem::TBasicWaveBank,mWaveTable,0x30);
WWHD_OFFSET(JASystem::TBasicWaveBank,mWaveCount,0x34);
WWHD_OFFSET(JASystem::TBasicWaveBank,mWaveGroupArray,0x38);
WWHD_OFFSET(JASystem::TBasicWaveBank,mWaveGroupCount,0x3C);
WWHD_SIZE(JASystem::TBasicWaveBank::TWaveGroup,0x7C);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveGroup,mBank,0x70);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveGroup,mCtrlWaveArray,0x74);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveGroup,mWaveCount,0x78);
WWHD_SIZE(JASystem::TBasicWaveBank::TWaveHandle,0x30);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveHandle,mWaveInfo,4);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveHandle,mHeap,0x2C);
WWHD_SIZE(JASystem::TBasicWaveBank::TWaveInfo,0x3C);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveInfo,mWaveID,0x30);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveInfo,mPrev,0x34);
WWHD_OFFSET(JASystem::TBasicWaveBank::TWaveInfo,mNext,0x38);
void __sinit_JASBasicWaveBank_cpp();
