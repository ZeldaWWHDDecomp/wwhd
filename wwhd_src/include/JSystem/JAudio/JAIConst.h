/* JAudio 1 initial data helpers and default listener state. */
#pragma once
#include "JSystem/JAudio/JAIBasic.h"
namespace JAInter {
be<u8>* transInitDataFile(be<u8>*, u32);
void loadTmpDVDFile(const char*, gptr<void>*);
u32 routeToTrack(u32);
}
void constHeapAssert(const char*, u32, const char*);
void* allocateAudioInitBytes(u32, JKRHeap*, s32);
u32 checkAudioFile(void*, const char*);
u32 loadAudioFile(void*, const char*, void*);
void constructAudioRandom(be<u32>*, u32);
void registerConstStatics(AudioDestructorRecord*);
// Audio1 owner export: the audio LCG seed (104B4F8C), inlined by Sound::setSeInterRandomPara.
extern be<u32>& audioRandomSeed;
