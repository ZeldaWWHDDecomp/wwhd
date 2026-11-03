/* JAudio 1 wave-bank scene management. */
#pragma once
#include "JSystem/JAudio/JAIGlobalParameter.h"
#include "JSystem/JAudio/JAISound.h"
namespace JAInter { namespace BankWave {
struct initOnCode_s {
    gptr<u8> data;
    be<s32> size;
    be<u32> waveBank;
};
WWHD_OFFSET(initOnCode_s, data, 0);
WWHD_OFFSET(initOnCode_s, waveBank, 8);
WWHD_SIZE(initOnCode_s, 12);
void setWsGroupNumber(u32 bank, s32 group);
void setWsLoadStatus(u32 bank, s32 status);
void init();
void finishSceneSet(u32 token);
void setSceneSetFinishCallback(u32 bank, u32 group);
void loadGroupWave(u32 bank, u32 group);
void setWaveScene();
void loadSecondStayWave();
void loadSceneWave(u32 bank, u32 group);
s32 getWaveLoadStatus(u32 bank);
bool checkAllWaveLoadStatus();
}}
void bankAssert(const char*, u32, const char*);
void* allocateBankArray(u32, JKRHeap*, s32);
void* getWavePath();
void setWaveDirectory(void*);
void initWaveBankMgr(u32);
void initWaveArcLoader(u32);
void registerWaveBank(u32, void*);
void initBankMgr(u32);
void registerInstrumentBank(u32, void*);
void assignWaveBank(u32, u32);
void loadWave(u32, u32, void*);
void eraseWave(u32, u32);
using WaveSceneCallback = void (*)(u32);
void checkPassDvdThread(u32, void*, WaveSceneCallback);
void registerBankStatics(AudioDestructorRecord*);
