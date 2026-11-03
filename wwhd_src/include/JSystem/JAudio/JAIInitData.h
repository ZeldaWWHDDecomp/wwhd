/* JAudio 1 AAF data dispatch. */
#pragma once
#include "JSystem/JAudio/JAIConst.h"
#include "JSystem/JAudio/JAIBankWave.h"
namespace JAInter { namespace InitData {
void checkInitDataOnMemory();
u32 checkInitDataFile();
}}
void initAudioSoundTable(be<u8>*, u32);
void setAudioSceneMax(u32);
u32 getAudioSceneMax();
be<u8>* getAudioInitFilename();
be<u8>* getAudioResourcePath();
u32 audioInitFileExists(be<u8>*);
void setAudioInitFilename(void*);
void registerInitDataStatics(AudioDestructorRecord*);
