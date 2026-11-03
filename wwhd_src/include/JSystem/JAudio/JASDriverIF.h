/* JAudio driver settings; HD omits the debug range checks. */
#pragma once
#include "bindings.h"
namespace JASystem { namespace Driver {
void setMixerLevel(f32 channel_level, f32 dsp_level);
u16 getChannelLevel();
u16 getAutoLevel();
void setOutputMode(u32 mode);
u32 getOutputMode();
}}
void setDriverDSPMixerLevel(f32 level);
