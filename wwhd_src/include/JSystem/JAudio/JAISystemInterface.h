#pragma once
#include "JSystem/JAudio/JAISequenceMgr.h"
#include "JSystem/JAudio/JAISound.h"
namespace JAInter { namespace SystemInterface {
void trackInit(SeqUpdateData*);
u8 checkSeqActiveFlag(JASystem::TTrack*);
JASystem::TTrack* trackToSeqp(JAISound*,u8);
void setPortParameter(JASystem::Kernel::TPortArgs*,JASystem::TTrack*,u32,u8);
void setSePortParameter(JASystem::Kernel::TPortArgs*);
void rootInit(SeqUpdateData*);
u32 outerInit(SeqUpdateData*,JASystem::TTrack*,u32,u32,u32);
u32 checkFileExsistence(const char*);
void setSeqPortargsF32(SeqUpdateData*,u32,u8,f32);
void setSeqPortargsU32(SeqUpdateData*,u32,u8,u32);
}}
void systemLookupPath(void*,const char*);
void systemRegister(void*);
void systemConstructPortLink(void*,void*);

u32 systemSoundID(JAISound*);
void systemSetOuterParameter(TOuterParam*,u32,f32);
void systemTrackInterrupt(JASystem::TTrack*,u32);
u32 systemSeqTrackMax();
void systemOuterInit(JAInter::SeqUpdateData*,JASystem::TTrack*,u32,u32,u32);

void systemOuterOn(TOuterParam*,u32);
void systemTrackMute(JASystem::TTrack*,u32);

be<u32>* systemClockInfo();
Pair32 systemDivideTicks(u32,u32,u32,u32);
void systemSleepTicks(u32,u32);
