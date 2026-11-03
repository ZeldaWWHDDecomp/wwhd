/* Compiler-generated static-registration records for JAudio companion translation units. */
#pragma once
#include "bindings.h"
struct AudioCompanionStatics { be<u32> mWords[4]; };
struct AudioCompanionRecord { be<u32> mWords[3]; };
WWHD_SIZE(AudioCompanionStatics,16);
WWHD_SIZE(AudioCompanionRecord,12);
void registerAudioCompanion(AudioCompanionRecord* record);
void registerOuterParamCompanion(AudioCompanionRecord* record);
