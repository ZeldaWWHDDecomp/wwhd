/* HD name lookup for effect IDs, shared by the effect callers. */
#pragma once
#include "bindings.h"
#include "JSystem/JAudio/JASCompanionStatics.h"
enum { AUDIO_EFFECT_NAME_COUNT = 0x474 };
struct AudioEffectNameTable { gptr<const char> mNames[AUDIO_EFFECT_NAME_COUNT]; };
WWHD_SIZE(AudioEffectNameTable,AUDIO_EFFECT_NAME_COUNT * 4);
void audioNameTableNoOp();
const char* lookupAudioEffectName(u32 id);
void registerAudioNameTable(AudioCompanionRecord* record);
