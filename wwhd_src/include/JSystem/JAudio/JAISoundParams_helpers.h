#pragma once
#include "JSystem/JAudio/JAISoundParams.h"
#include "JSystem/JAudio/JAISound.h"
void soundLinkLock(void*);
void soundLinkUnlock(void*);
void soundLinkDestroyCritical(void*,s32);
void soundLinkDelete(void*);

u32 soundLinkCameraMax();
void* soundLinkAllocate(u32,JKRHeap*,s32);
void soundLinkAssert(const char*,u32,const char*);
