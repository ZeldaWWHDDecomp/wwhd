/* Based on CC0 tww JASDriverIF.cpp; HD mixer and output settings. */
#include "JSystem/JAudio/JASDriverIF.h"
using namespace JASystem::Driver;
static u16 MAX_MIXERLEVEL = 0x2EE0;
// HD: automatic and channel mixer limits use the same initial level.
static u16 MAX_AUTOMIXERLEVEL = 0x2EE0;
// HD: the initial output mode is 2.
static u32 JAS_SYSTEM_OUTPUT_MODE = 2;
namespace JASystem { namespace Driver {
/* 028155B4 */
void setMixerLevel(f32 channel_level, f32 dsp_level) {
    // HD: the debug range assertions are omitted.
    MAX_MIXERLEVEL = (u16)gabi::ftoi(gabi::fmuls_ppc(channel_level, 16383.5f));
    setDriverDSPMixerLevel(dsp_level);
}
/* 028157B0 */
u16 getChannelLevel() { return MAX_MIXERLEVEL; }
/* 028157BC */
u16 getAutoLevel() { return MAX_AUTOMIXERLEVEL; }
/* 028157C8 */
void setOutputMode(u32 mode) {
    // HD: output-mode requests have no effect.
}
/* 028157CC */
u32 getOutputMode() { return JAS_SYSTEM_OUTPUT_MODE; }
}}
/* WWHD glue
data MAX_MIXERLEVEL 101F9F24
data MAX_AUTOMIXERLEVEL 101F9F26
data JAS_SYSTEM_OUTPUT_MODE 101F9F20
ext 0281621C void setDriverDSPMixerLevel(f32)
*/
