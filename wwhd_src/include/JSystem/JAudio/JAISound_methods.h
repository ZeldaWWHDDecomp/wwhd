#pragma once
#include "JSystem/JAudio/JAISound.h"
#include "JSystem/JAudio/JAIStreamMgr.h"
#include "JSystem/JAudio/JAISeMgr.h"
#include "JSystem/JAudio/JAIBasic.h"
#include "JSystem/JAudio/JAIConst.h"
#include "JSystem/JAudio/JAIGlobalParameter.h"
u8 soundCategoryFromID(u32);

void* soundObjectAllocate(u32);
void soundObjectDelete(void*);

#include "JSystem/JAudio/JAISequenceMgr.h"
struct SoundDispatchVtable { be<u32> reserved0[5]; be<u32> stop; be<u32> reserved18[3]; be<u32> start; be<u32> reserved28[3]; be<u32> getID; be<u32> delta38,volume,delta40,pitch; };
WWHD_OFFSET(SoundDispatchVtable,stop,0x14);
WWHD_OFFSET(SoundDispatchVtable,start,0x24);
struct SoundDispatchObject { gptr<SoundDispatchVtable> mVtable; };
WWHD_OFFSET(SoundDispatchVtable,getID,0x34);
void soundControlAssert(const char*,u32,const char*);


void soundStopHandle(JAIBasic*,JAISound*,u32);
u32 sequenceSoundOffset(JAIBasic*,u32);

WWHD_OFFSET(SoundDispatchVtable,volume,0x3C);
u32 soundDistanceMoveTime();
void soundSetSeqVolume(JAISound*,u32,f32,u32);
void soundSetStreamVolume(JAISound*,u32,f32,u32);
void soundSetSeVolume(JAISound*,u32,f32,u32,u32);

void soundRegister(void*);

// HD: callers consume the preserved r3 receiver after this void-method boundary.
JAISound* soundRandomParameter(JAISound*,be<f32>*,u32,f32,f32);

u32 soundReadPort(JASystem::TTrack*,u32,be<u16>*);
u32 soundWritePort(JASystem::TTrack*,u32,u32);
void soundSeTrackPort(JAISound*,u32,u32,u32);

void soundTrackPause(JASystem::TTrack*,u32,u32);

f64 soundSqrt(f64);

