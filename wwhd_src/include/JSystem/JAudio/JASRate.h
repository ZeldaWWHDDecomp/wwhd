#ifndef WWHD_JASRATE_H
#define WWHD_JASRATE_H
#include "wwhd.h"
namespace JASystem { namespace Kernel {
extern be<u32>& gSubFrames;
extern be<f32>& gDacRate;
u32 getDacSize();
u32 getFrameSamples();
u32 getSubFrames();
f32 getDacRate();
}}
#endif
