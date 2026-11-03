// JAudio sample geometry; HD has four interleaved output channels.
#include "JSystem/JAudio/JASRate.h"
using namespace JASystem::Kernel;
namespace JASystem { namespace Kernel {
static u32 gSubFrames;
static f32 gDacRate;
/* 02813418 */
u32 getDacSize() { return (u32)gSubFrames * 0x140; }
/* 0281360C */
u32 getFrameSamples() { return (u32)gSubFrames * 0x50; }
/* 02813AC8 */
u32 getSubFrames() { return gSubFrames; }
/* 02813AD4 */
f32 getDacRate() { return gDacRate; }
}}
/* WWHD glue
data gSubFrames 101F9EA4
data gDacRate 101F9EB4
*/
