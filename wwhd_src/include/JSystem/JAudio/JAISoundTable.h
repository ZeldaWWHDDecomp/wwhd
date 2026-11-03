/* Sound table records, following CC0 tww; share the existing Cafe info layout. */
#pragma once
#include "JSystem/JAudio/JAIBasic.h"
using SoundInfo=JAISoundInfoView;
namespace JAInter { namespace SoundTable {
void init(be<u8>*,u32);
be<u8>* getAddress();
SoundInfo* getInfoPointer(u32 soundID);
u8 getCategotyMax();
u8 getInfoFormat(u32 soundID);
}}
void soundTableAssert(const char*,u32,const char*);
void soundTableRegister(void*);

u32 soundTableCategoryMax();

void* soundTableAllocate(u32,JKRHeap*,s32);
