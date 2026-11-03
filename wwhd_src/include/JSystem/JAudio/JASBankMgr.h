#ifndef WWHD_JASBANKMGR_H
#define WWHD_JASBANKMGR_H
#include "JSystem/JAudio/JASBank.h"
#include "JSystem/JAudio/JASChannel.h"
namespace JASystem {
namespace BankMgr {
TChannel* noteOn(s32 banknum, s32 program, s32 key, u32 velocity, u32 channelId, u32 callback, u32 userArg);
void gateOn(TChannel* channel, s32 key, u32 velocity);
void init(s32 size);
u32 registBank(s32 banknum, TBank* bank);
void setVir2PhyTable(u32 virtualId, u32 banknum);
u32 registBankBNK(s32 banknum, void* stream);
TBank* getBank(s32 banknum);
u32 getPhysicalNumber(u32 virtualId);
u32 assignWaveBank(s32 banknum, s32 waveBanknum);
}
}
#endif
