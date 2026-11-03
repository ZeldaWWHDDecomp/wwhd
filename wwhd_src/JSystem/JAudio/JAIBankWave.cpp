/* Based on CC0 tww JAIBankWave.cpp; WWHD behavior controls the implementation. */
#include "JSystem/JAudio/JAIBankWave.h"
namespace JAInter { namespace BankWave {
using BankWord = be<s32>;
static BankWord* wsGroupNumber;
static BankWord* wsLoadStatus;
static initOnCode_s* initOnCodeBnk;
static initOnCode_s* initOnCodeWs;
static s32 wsMax;
static s32 SceneSetFlag;
static u16 flags;
static JAISound* seHandle;
static JKRHeap* msCurrentHeap;
}}
using namespace JAInter::BankWave;
static AudioHeaderStatics bankHeaderStatics;
static AudioDestructorRecord bankDestructorRecord;
namespace JAInter { namespace BankWave {
/* 02802028 */
void setWsGroupNumber(u32 bank, s32 group) { wsGroupNumber[bank] = group; }
/* 0280203C */
void setWsLoadStatus(u32 bank, s32 status) { wsLoadStatus[bank] = status; }
/* 02802050 */
void init() {
    JKRHeap* heap = msCurrentHeap;
    if (!heap) { bankAssert("JAIBasic.h",169,"msCurrentHeap"); heap = msCurrentHeap; }
    wsGroupNumber = (BankWord*)allocateBankArray((u32)wsMax * 4, heap, 32);
    heap = msCurrentHeap;
    if (!heap) { bankAssert("JAIBasic.h",169,"msCurrentHeap"); heap = msCurrentHeap; }
    wsLoadStatus = (BankWord*)allocateBankArray((u32)wsMax * 4, heap, 32);
    void* path = getWavePath();
    setWaveDirectory(path);
    initWaveBankMgr(256);
    initWaveArcLoader(0);
    initOnCode_s* waves = initOnCodeWs;
    if (waves && waves[0].data) {
        waves = initOnCodeWs;
        void* data = waves[0].data;
        for (u32 bank = 0; data; ++bank) {
            registerWaveBank(bank, data);
            wsGroupNumber[bank] = -1;
            wsLoadStatus[bank] = 0;
            waves = initOnCodeWs;
            data = waves[bank + 1].data;
        }
    }
    initBankMgr(256);
    initOnCode_s* banks = initOnCodeBnk;
    if (banks) {
        if (banks[0].data) {
            banks = initOnCodeBnk;
            void* data = banks[0].data;
            for (u32 bank = 0; data; ++bank) {
                registerInstrumentBank(bank, data);
                banks = initOnCodeBnk;
                data = banks[bank + 1].data;
            }
            banks = initOnCodeBnk;
        }
        if (banks[0].data) {
            banks = initOnCodeBnk;
            for (u32 bank = 0; ; ++bank) {
                assignWaveBank(bank, banks[bank].waveBank);
                banks = initOnCodeBnk;
                if (!banks[bank + 1].data) break;
            }
        }
    }
}
/* 02802220 */
void finishSceneSet(u32 token) {
    BankWord* status = wsLoadStatus;
    SceneSetFlag = token;
    status[token >> 16] = 2;
}
/* 02802240 */
void setSceneSetFinishCallback(u32 bank, u32 group) {
    BankWord* status = wsLoadStatus;
    SceneSetFlag = -1;
    status[bank] = 1;
    checkPassDvdThread((bank << 16) + group, nullptr, finishSceneSet);
}
/* 02802278 */
void loadGroupWave(u32 bank, u32 group) {
    loadWave(bank, group, nullptr);
    setSceneSetFinishCallback(bank, group);
    wsGroupNumber[bank] = group;
}
/* 028022D0 */
void setWaveScene() {
    initOnCode_s* waves = initOnCodeWs;
    if (!waves) return;
    if (waves[0].data) {
        waves = initOnCodeWs;
        for (u32 bank = 0; ; ++bank) {
            if (waves[bank].waveBank == 0) { loadGroupWave(bank, 0); waves = initOnCodeWs; }
            if (!waves[bank + 1].data) break;
        }
    }
    flags |= 0x8000;
}
/* 02802374 */
void loadSecondStayWave() {
    if (seHandle->state != 3 || (flags & 0x4000)) return;
    initOnCode_s* waves = initOnCodeWs;
    if (!waves) return;
    if (waves[0].data) {
        waves = initOnCodeWs;
        for (u32 bank = 0; ; ++bank) {
            if (waves[bank].waveBank == 1) { loadGroupWave(bank, 0); waves = initOnCodeWs; }
            if (!waves[bank + 1].data) break;
        }
    }
    flags |= 0x4000;
}
/* 02802440 */
void loadSceneWave(u32 bank, u32 group) {
    if (initOnCodeWs && initOnCodeWs[bank].waveBank == 2) {
        s32 previous = wsGroupNumber[bank];
        if ((u32)previous != group) {
            if (previous != -1) eraseWave(bank, previous);
            loadGroupWave(bank, group);
        }
    }
}
/* 028024CC */
s32 getWaveLoadStatus(u32 bank) { return wsLoadStatus[bank]; }
/* 028024E0 */
bool checkAllWaveLoadStatus() {
    initOnCode_s* waves = initOnCodeWs;
    if (!waves[0].data) return false;
    BankWord* status = wsLoadStatus;
    for (u32 bank = 0; ; ++bank) {
        if (status[bank] == 1) return true;
        if (!waves[bank + 1].data) return false;
    }
}
}}
/* 02802528 */
void __sinit_JAIBankWave_cpp() {
    bankHeaderStatics.mWords[2] = 0;
    bankHeaderStatics.mWords[0] = 0;
    bankHeaderStatics.mWords[3] = 0;
    bankHeaderStatics.mWords[1] = 0;
    registerBankStatics(&bankDestructorRecord);
}
/* WWHD glue
data wsGroupNumber 104B4EE4
data wsLoadStatus 104B4EE8
data initOnCodeBnk 101F9B34
data initOnCodeWs 101F9B38
data SceneSetFlag 101F9B3C
data wsMax 101F9B40
data flags 101F9B44
data seHandle 104B5018
data msCurrentHeap 101F9B54
data bankHeaderStatics 104B4ED4
data bankDestructorRecord 101F9B28
str 1016F39C "JAIBasic.h"
str 1016F38C "msCurrentHeap"
ext 0273AA24 void bankAssert(const char*, u32, const char*)
ext 0273B0D4 void* allocateBankArray(u32, JKRHeap*, s32)
ext 02804784 void* getWavePath()
ext 02812BEC void setWaveDirectory(void*)
ext 02812CC0 void initWaveBankMgr(u32)
ext 02812BBC void initWaveArcLoader(u32)
ext 02812D6C void registerWaveBank(u32, void*)
ext 02810E80 void initBankMgr(u32)
ext 02810F68 void registerInstrumentBank(u32, void*)
ext 02811004 void assignWaveBank(u32, u32)
ext 02812E2C void loadWave(u32, u32, void*)
ext 02812E70 void eraseWave(u32, u32)
ext 0280ECF4 void checkPassDvdThread(u32, void*, WaveSceneCallback)
ext 028F026C void registerBankStatics(AudioDestructorRecord*)
*/
